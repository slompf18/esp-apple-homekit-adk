#include "../include/esp_hk.h"
#include <signal.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

#include <HAP.h>

#define IP 1
#include "HAP.h"
#include "../../port/include/HAPPlatform+Init.h"
#include "../../port/include/HAPPlatformAccessorySetup+Init.h"
#include "../../port/include/HAPPlatformBLEPeripheralManager+Init.h"
#include "../../port/include/HAPPlatformKeyValueStore+Init.h"
#include "../../port/include/HAPPlatformMFiHWAuth+Init.h"
#include "../../port/include/HAPPlatformMFiTokenAuth+Init.h"
#include "../../port/include/HAPPlatformRunLoop+Init.h"
#if IP
#include "../../port/include/HAPPlatformServiceDiscovery+Init.h"
#include "../../port/include/HAPPlatformTCPStreamManager+Init.h"
#endif

static bool requestedFactoryReset = false;
static bool clearPairings = false;
static HAPAccessoryServerRef accessoryServer;
static HAPAccessory *acc;
/**
 * Global platform objects.
 * Only tracks objects that will be released in DeinitializePlatform.
 */
static struct
{
    HAPPlatformKeyValueStore keyValueStore;
    HAPPlatformKeyValueStore factoryKeyValueStore;
    HAPAccessoryServerOptions hapAccessoryServerOptions;
    HAPPlatform hapPlatform;
    HAPAccessoryServerCallbacks hapAccessoryServerCallbacks;

#if HAVE_NFC
    HAPPlatformAccessorySetupNFC setupNFC;
#endif

#if IP
    HAPPlatformTCPStreamManager tcpStreamManager;
#endif

    HAPPlatformMFiHWAuth mfiHWAuth;
    HAPPlatformMFiTokenAuth mfiTokenAuth;
} platform;

static void AppAccessoryServerStart(void)
{
    HAPAccessoryServerStart(&accessoryServer, acc);
}

/**
 * Either simply passes State handling to app, or processes Factory Reset
 */
static void HandleUpdatedState(HAPAccessoryServerRef *_Nonnull server, void *_Nullable context)
{
    if (HAPAccessoryServerGetState(server) == kHAPAccessoryServerState_Idle && requestedFactoryReset)
    {
        HAPPrecondition(server);

        HAPError err;

        HAPLogInfo(&kHAPLog_Default, "A factory reset has been requested.");

        // Purge app state.
        err = HAPPlatformKeyValueStorePurgeDomain(&platform.keyValueStore, ((HAPPlatformKeyValueStoreDomain)0x00));
        if (err)
        {
            HAPAssert(err == kHAPError_Unknown);
            HAPFatalError();
        }

        // Reset HomeKit state.
        err = HAPRestoreFactorySettings(&platform.keyValueStore);
        if (err)
        {
            HAPAssert(err == kHAPError_Unknown);
            HAPFatalError();
        }

        // Restore platform specific factory settings.
        // RestorePlatformFactorySettings(); // todo

        // De-initialize App.
        //AppRelease(); // todo

        requestedFactoryReset = false;

        // Re-initialize App.
        //AppCreate(server, &platform.keyValueStore); // todo

        // Restart accessory server.
        AppAccessoryServerStart();
        return;
    }
    else if (HAPAccessoryServerGetState(server) == kHAPAccessoryServerState_Idle && clearPairings)
    {
        HAPError err;
        err = HAPRemoveAllPairings(&platform.keyValueStore);
        if (err)
        {
            HAPAssert(err == kHAPError_Unknown);
            HAPFatalError();
        }
        AppAccessoryServerStart();
    }
    else
    {
        // AccessoryServerHandleUpdatedState(server, context); // todo
    }
}

/**
 * Initialize global platform objects.
 */
static void InitializePlatform()
{
    // Key-value store.
    HAPPlatformKeyValueStoreCreate(&platform.keyValueStore, &(const HAPPlatformKeyValueStoreOptions){
                                                                .part_name = "nvs",
                                                                .namespace_prefix = "hap",
                                                                .read_only = false});
    platform.hapPlatform.keyValueStore = &platform.keyValueStore;

    HAPPlatformKeyValueStoreCreate(&platform.factoryKeyValueStore, &(const HAPPlatformKeyValueStoreOptions){
                                                                       .part_name = "fctry",
                                                                       .namespace_prefix = "hap",
                                                                       .read_only = true});

    // Accessory setup manager. Depends on key-value store.
    static HAPPlatformAccessorySetup accessorySetup;
    HAPPlatformAccessorySetupCreate(
        &accessorySetup, &(const HAPPlatformAccessorySetupOptions){.keyValueStore = &platform.factoryKeyValueStore});
    platform.hapPlatform.accessorySetup = &accessorySetup;

#if IP
    // TCP stream manager.
    HAPPlatformTCPStreamManagerCreate(&platform.tcpStreamManager, &(const HAPPlatformTCPStreamManagerOptions){
                                                                      /* Listen on all available network interfaces. */
                                                                      .port = 0 /* Listen on unused port number from the ephemeral port range. */,
                                                                      .maxConcurrentTCPStreams = 9});

    // Service discovery.
    static HAPPlatformServiceDiscovery serviceDiscovery;
    HAPPlatformServiceDiscoveryCreate(&serviceDiscovery, &(const HAPPlatformServiceDiscoveryOptions){
                                                             0, /* Register services on all available network interfaces. */
                                                         });
    platform.hapPlatform.ip.serviceDiscovery = &serviceDiscovery;
#endif

#if (BLE)
    // BLE peripheral manager. Depends on key-value store.
    static HAPPlatformBLEPeripheralManagerOptions blePMOptions = {0};
    blePMOptions.keyValueStore = &platform.keyValueStore;

    static HAPPlatformBLEPeripheralManager blePeripheralManager;
    HAPPlatformBLEPeripheralManagerCreate(&blePeripheralManager, &blePMOptions);
    platform.hapPlatform.ble.blePeripheralManager = &blePeripheralManager;
#endif

#if HAVE_MFI_HW_AUTH
    // Apple Authentication Coprocessor provider.
    HAPPlatformMFiHWAuthCreate(&platform.mfiHWAuth);
#endif

#if HAVE_MFI_HW_AUTH
    platform.hapPlatform.authentication.mfiHWAuth = &platform.mfiHWAuth;
#endif

    // Software Token provider. Depends on key-value store.
    HAPPlatformMFiTokenAuthCreate(
        &platform.mfiTokenAuth,
        &(const HAPPlatformMFiTokenAuthOptions){.keyValueStore = &platform.keyValueStore});

    // Run loop.
    HAPPlatformRunLoopCreate(&(const HAPPlatformRunLoopOptions){.keyValueStore = &platform.keyValueStore});

    platform.hapAccessoryServerOptions.maxPairings = kHAPPairingStorage_MinElements;

    platform.hapPlatform.authentication.mfiTokenAuth =
        HAPPlatformMFiTokenAuthIsProvisioned(&platform.mfiTokenAuth) ? &platform.mfiTokenAuth : NULL;

    platform.hapAccessoryServerCallbacks.handleUpdatedState = HandleUpdatedState;
}

/**
 * Deinitialize global platform objects.
 */
static void DeinitializePlatform()
{
#if HAVE_MFI_HW_AUTH
    // Apple Authentication Coprocessor provider.
    HAPPlatformMFiHWAuthRelease(&platform.mfiHWAuth);
#endif

#if IP
    // TCP stream manager.
    HAPPlatformTCPStreamManagerRelease(&platform.tcpStreamManager);
#endif

    // AppDeinitialize();  // todo

    // Run loop.
    HAPPlatformRunLoopRelease();
}

#if IP
static void InitializeIP()
{
    // Prepare accessory server storage.
    static HAPIPSession ipSessions[kHAPIPSessionStorage_MinimumNumElements];
    static uint8_t ipInboundBuffers[HAPArrayCount(ipSessions)][kHAPIPSession_MinimumInboundBufferSize];
    static uint8_t ipOutboundBuffers[HAPArrayCount(ipSessions)][kHAPIPSession_MinimumOutboundBufferSize];
    static HAPIPEventNotificationRef ipEventNotifications[HAPArrayCount(ipSessions)][kAttributeCount];
    for (size_t i = 0; i < HAPArrayCount(ipSessions); i++)
    {
        ipSessions[i].inboundBuffer.bytes = ipInboundBuffers[i];
        ipSessions[i].inboundBuffer.numBytes = sizeof ipInboundBuffers[i];
        ipSessions[i].outboundBuffer.bytes = ipOutboundBuffers[i];
        ipSessions[i].outboundBuffer.numBytes = sizeof ipOutboundBuffers[i];
        ipSessions[i].eventNotifications = ipEventNotifications[i];
        ipSessions[i].numEventNotifications = HAPArrayCount(ipEventNotifications[i]);
    }
    static HAPIPReadContextRef ipReadContexts[kAttributeCount];
    static HAPIPWriteContextRef ipWriteContexts[kAttributeCount];
    static uint8_t ipScratchBuffer[kHAPIPSession_MinimumScratchBufferSize];
    static HAPIPAccessoryServerStorage ipAccessoryServerStorage = {
        .sessions = ipSessions,
        .numSessions = HAPArrayCount(ipSessions),
        .readContexts = ipReadContexts,
        .numReadContexts = HAPArrayCount(ipReadContexts),
        .writeContexts = ipWriteContexts,
        .numWriteContexts = HAPArrayCount(ipWriteContexts),
        .scratchBuffer = {.bytes = ipScratchBuffer, .numBytes = sizeof ipScratchBuffer}};

    platform.hapAccessoryServerOptions.ip.transport = &kHAPAccessoryServerTransport_IP;
    platform.hapAccessoryServerOptions.ip.accessoryServerStorage = &ipAccessoryServerStorage;

    platform.hapPlatform.ip.tcpStreamManager = &platform.tcpStreamManager;

    // Connect to Wi-Fi
    // app_wifi_connect(); // todo
}
#endif

#if BLE
static void InitializeBLE()
{
    static HAPBLEGATTTableElementRef gattTableElements[kAttributeCount];
    static HAPBLESessionCacheElementRef sessionCacheElements[kHAPBLESessionCache_MinElements];
    static HAPSessionRef session;
    static uint8_t procedureBytes[2048];
    static HAPBLEProcedureRef procedures[1];

    static HAPBLEAccessoryServerStorage bleAccessoryServerStorage = {
        .gattTableElements = gattTableElements,
        .numGATTTableElements = HAPArrayCount(gattTableElements),
        .sessionCacheElements = sessionCacheElements,
        .numSessionCacheElements = HAPArrayCount(sessionCacheElements),
        .session = &session,
        .procedures = procedures,
        .numProcedures = HAPArrayCount(procedures),
        .procedureBuffer = {.bytes = procedureBytes, .numBytes = sizeof procedureBytes}};

    platform.hapAccessoryServerOptions.ble.transport = &kHAPAccessoryServerTransport_BLE;
    platform.hapAccessoryServerOptions.ble.accessoryServerStorage = &bleAccessoryServerStorage;
    platform.hapAccessoryServerOptions.ble.preferredAdvertisingInterval = PREFERRED_ADVERTISING_INTERVAL;
    platform.hapAccessoryServerOptions.ble.preferredNotificationDuration = kHAPBLENotification_MinDuration;
}
#endif

void esp_hk_task()
{
    // Initialize accessory server.
    HAPAccessoryServerCreate(
        &accessoryServer,
        &platform.hapAccessoryServerOptions,
        &platform.hapPlatform,
        &platform.hapAccessoryServerCallbacks,
        /* context: */ NULL);

    // Start accessory server for App.
    AppAccessoryServerStart();

    // Run main loop until explicitly stopped.
    HAPPlatformRunLoopRun();
    // Run loop stopped explicitly by calling function HAPPlatformRunLoopStop.

    // Cleanup.
    // AppRelease(); //todo

    HAPAccessoryServerRelease(&accessoryServer);

    DeinitializePlatform();
}

esp_err_t esp_hk_init(HAPAccessory *accessory)
{
    acc = accessory;
    // Initialize global platform objects.
    InitializePlatform();

#if IP
    InitializeIP();
#endif

#if BLE
    InitializeBLE();
#endif

    esp_err_t ret = ESP_OK;

    return ret;
}

esp_err_t esp_hk_start()
{
    xTaskCreate(esp_hk_task, "esp_hk_task", 6 * 1024, NULL, 6, NULL);

    return ESP_OK;
}

typedef struct
{
    const HAPCharacteristic *characteristic;
    const HAPService *service;
    const HAPAccessory *accessory;
} esp_hk_raise_event_ctx_t;

static void esp_hk_raise_event_internal(void *ctx_void, size_t ctx_size)
{
    ESP_LOGD("HK", "HAPAccessoryServerRaiseEvent!");
    esp_hk_raise_event_ctx_t *ctx = (esp_hk_raise_event_ctx_t*)ctx_void;
    HAPAccessoryServerRaiseEvent(&accessoryServer, ctx->characteristic, ctx->service, ctx->accessory);
}

esp_err_t esp_hk_raise_event(
    const HAPAccessory *accessory,
    const HAPService *service,
    const HAPCharacteristic *characteristic)
{
    esp_hk_raise_event_ctx_t *ctx = malloc(sizeof(esp_hk_raise_event_ctx_t));
    ctx->accessory = accessory;
    ctx->service = service;
    ctx->characteristic = characteristic;

    HAPError err = HAPPlatformRunLoopScheduleCallback(esp_hk_raise_event_internal, ctx, sizeof(esp_hk_raise_event_ctx_t));
    if (err)
    {
        HAPAssert(err == kHAPError_Unknown);
        HAPFatalError();
    }

    free(ctx);
    return ESP_OK;
}