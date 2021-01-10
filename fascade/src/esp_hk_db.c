// Copyright (c) 2015-2019 The HomeKit ADK Contributors
//
// Licensed under the Apache License, Version 2.0 (the “License”);
// you may not use this file except in compliance with the License.
// See [CONTRIBUTORS.md] for the list of HomeKit ADK project authors.

// This file contains the accessory attribute database that defines the accessory information service, HAP Protocol
// Information Service, the Pairing service and finally the service signature exposed by the light bulb.

#include "esp_hk_db.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * IID constants.
 */
#define kIID_AccessoryInformation ((uint64_t)0x0001)
#define kIID_AccessoryInformationIdentify ((uint64_t)0x0002)
#define kIID_AccessoryInformationManufacturer ((uint64_t)0x0003)
#define kIID_AccessoryInformationModel ((uint64_t)0x0004)
#define kIID_AccessoryInformationName ((uint64_t)0x0005)
#define kIID_AccessoryInformationSerialNumber ((uint64_t)0x0006)
#define kIID_AccessoryInformationFirmwareRevision ((uint64_t)0x0007)
#define kIID_AccessoryInformationHardwareRevision ((uint64_t)0x0008)
#define kIID_AccessoryInformationADKVersion ((uint64_t)0x0009)
#define kIID_AccessoryInformationProductData ((uint64_t)0x000A)

#define kIID_HAPProtocolInformation ((uint64_t)0x0010)
#define kIID_HAPProtocolInformationServiceSignature ((uint64_t)0x0011)
#define kIID_HAPProtocolInformationVersion ((uint64_t)0x0012)

#define kIID_Pairing ((uint64_t)0x0020)
#define kIID_PairingPairSetup ((uint64_t)0x0022)
#define kIID_PairingPairVerify ((uint64_t)0x0023)
#define kIID_PairingPairingFeatures ((uint64_t)0x0024)
#define kIID_PairingPairingPairings ((uint64_t)0x0025)

HAP_STATIC_ASSERT(kAttributeCount == 9 + 3 + 5 + 4, AttributeCount_mismatch);

/**
 * Domain used in the key value store for application data.
 *
 * Purged: On factory reset.
 */
#define kAppKeyValueStoreDomain_Configuration ((HAPPlatformKeyValueStoreDomain)0x00)

/**
 * Key used in the key value store to store the configuration state.
 *
 * Purged: On factory reset.
 */
#define kAppKeyValueStoreKey_Configuration_State ((HAPPlatformKeyValueStoreDomain)0x00)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const HAPBoolCharacteristic esp_hk_db_accessoryInformationIdentifyCharacteristic = {
    .format = kHAPCharacteristicFormat_Bool,
    .iid = kIID_AccessoryInformationIdentify,
    .characteristicType = &kHAPCharacteristicType_Identify,
    .debugDescription = kHAPCharacteristicDebugDescription_Identify,
    .manufacturerDescription = NULL,
    .properties = {.readable = false,
                   .writable = true,
                   .supportsEventNotification = false,
                   .hidden = false,
                   .requiresTimedWrite = false,
                   .supportsAuthorizationData = false,
                   .ip = {.controlPoint = false, .supportsWriteResponse = false},
                   .ble = {.supportsBroadcastNotification = false,
                           .supportsDisconnectedNotification = false,
                           .readableWithoutSecurity = false,
                           .writableWithoutSecurity = false}},
    .callbacks = {.handleRead = NULL, .handleWrite = HAPHandleAccessoryInformationIdentifyWrite}};

const HAPStringCharacteristic esp_hk_db_accessoryInformationManufacturerCharacteristic = {
    .format = kHAPCharacteristicFormat_String,
    .iid = kIID_AccessoryInformationManufacturer,
    .characteristicType = &kHAPCharacteristicType_Manufacturer,
    .debugDescription = kHAPCharacteristicDebugDescription_Manufacturer,
    .manufacturerDescription = NULL,
    .properties = {.readable = true,
                   .writable = false,
                   .supportsEventNotification = false,
                   .hidden = false,
                   .requiresTimedWrite = false,
                   .supportsAuthorizationData = false,
                   .ip = {.controlPoint = false, .supportsWriteResponse = false},
                   .ble = {.supportsBroadcastNotification = false,
                           .supportsDisconnectedNotification = false,
                           .readableWithoutSecurity = false,
                           .writableWithoutSecurity = false}},
    .constraints = {.maxLength = 64},
    .callbacks = {.handleRead = HAPHandleAccessoryInformationManufacturerRead, .handleWrite = NULL}};

const HAPStringCharacteristic esp_hk_db_accessoryInformationModelCharacteristic = {
    .format = kHAPCharacteristicFormat_String,
    .iid = kIID_AccessoryInformationModel,
    .characteristicType = &kHAPCharacteristicType_Model,
    .debugDescription = kHAPCharacteristicDebugDescription_Model,
    .manufacturerDescription = NULL,
    .properties = {.readable = true,
                   .writable = false,
                   .supportsEventNotification = false,
                   .hidden = false,
                   .requiresTimedWrite = false,
                   .supportsAuthorizationData = false,
                   .ip = {.controlPoint = false, .supportsWriteResponse = false},
                   .ble = {.supportsBroadcastNotification = false,
                           .supportsDisconnectedNotification = false,
                           .readableWithoutSecurity = false,
                           .writableWithoutSecurity = false}},
    .constraints = {.maxLength = 64},
    .callbacks = {.handleRead = HAPHandleAccessoryInformationModelRead, .handleWrite = NULL}};

const HAPStringCharacteristic esp_hk_db_accessoryInformationNameCharacteristic = {
    .format = kHAPCharacteristicFormat_String,
    .iid = kIID_AccessoryInformationName,
    .characteristicType = &kHAPCharacteristicType_Name,
    .debugDescription = kHAPCharacteristicDebugDescription_Name,
    .manufacturerDescription = NULL,
    .properties = {.readable = true,
                   .writable = false,
                   .supportsEventNotification = false,
                   .hidden = false,
                   .requiresTimedWrite = false,
                   .supportsAuthorizationData = false,
                   .ip = {.controlPoint = false, .supportsWriteResponse = false},
                   .ble = {.supportsBroadcastNotification = false,
                           .supportsDisconnectedNotification = false,
                           .readableWithoutSecurity = false,
                           .writableWithoutSecurity = false}},
    .constraints = {.maxLength = 64},
    .callbacks = {.handleRead = HAPHandleAccessoryInformationNameRead, .handleWrite = NULL}};

const HAPStringCharacteristic esp_hk_db_accessoryInformationSerialNumberCharacteristic = {
    .format = kHAPCharacteristicFormat_String,
    .iid = kIID_AccessoryInformationSerialNumber,
    .characteristicType = &kHAPCharacteristicType_SerialNumber,
    .debugDescription = kHAPCharacteristicDebugDescription_SerialNumber,
    .manufacturerDescription = NULL,
    .properties = {.readable = true,
                   .writable = false,
                   .supportsEventNotification = false,
                   .hidden = false,
                   .requiresTimedWrite = false,
                   .supportsAuthorizationData = false,
                   .ip = {.controlPoint = false, .supportsWriteResponse = false},
                   .ble = {.supportsBroadcastNotification = false,
                           .supportsDisconnectedNotification = false,
                           .readableWithoutSecurity = false,
                           .writableWithoutSecurity = false}},
    .constraints = {.maxLength = 64},
    .callbacks = {.handleRead = HAPHandleAccessoryInformationSerialNumberRead, .handleWrite = NULL}};

const HAPStringCharacteristic esp_hk_db_accessoryInformationFirmwareRevisionCharacteristic = {
    .format = kHAPCharacteristicFormat_String,
    .iid = kIID_AccessoryInformationFirmwareRevision,
    .characteristicType = &kHAPCharacteristicType_FirmwareRevision,
    .debugDescription = kHAPCharacteristicDebugDescription_FirmwareRevision,
    .manufacturerDescription = NULL,
    .properties = {.readable = true,
                   .writable = false,
                   .supportsEventNotification = false,
                   .hidden = false,
                   .requiresTimedWrite = false,
                   .supportsAuthorizationData = false,
                   .ip = {.controlPoint = false, .supportsWriteResponse = false},
                   .ble = {.supportsBroadcastNotification = false,
                           .supportsDisconnectedNotification = false,
                           .readableWithoutSecurity = false,
                           .writableWithoutSecurity = false}},
    .constraints = {.maxLength = 64},
    .callbacks = {.handleRead = HAPHandleAccessoryInformationFirmwareRevisionRead, .handleWrite = NULL}};

const HAPStringCharacteristic esp_hk_db_accessoryInformationHardwareRevisionCharacteristic = {
    .format = kHAPCharacteristicFormat_String,
    .iid = kIID_AccessoryInformationHardwareRevision,
    .characteristicType = &kHAPCharacteristicType_HardwareRevision,
    .debugDescription = kHAPCharacteristicDebugDescription_HardwareRevision,
    .manufacturerDescription = NULL,
    .properties = {.readable = true,
                   .writable = false,
                   .supportsEventNotification = false,
                   .hidden = false,
                   .requiresTimedWrite = false,
                   .supportsAuthorizationData = false,
                   .ip = {.controlPoint = false, .supportsWriteResponse = false},
                   .ble = {.supportsBroadcastNotification = false,
                           .supportsDisconnectedNotification = false,
                           .readableWithoutSecurity = false,
                           .writableWithoutSecurity = false}},
    .constraints = {.maxLength = 64},
    .callbacks = {.handleRead = HAPHandleAccessoryInformationHardwareRevisionRead, .handleWrite = NULL}};

const HAPStringCharacteristic esp_hk_db_accessoryInformationADKVersionCharacteristic = {
    .format = kHAPCharacteristicFormat_String,
    .iid = kIID_AccessoryInformationADKVersion,
    .characteristicType = &kHAPCharacteristicType_ADKVersion,
    .debugDescription = kHAPCharacteristicDebugDescription_ADKVersion,
    .manufacturerDescription = NULL,
    .properties = {.readable = true,
                   .writable = false,
                   .supportsEventNotification = false,
                   .hidden = true,
                   .requiresTimedWrite = false,
                   .supportsAuthorizationData = false,
                   .ip = {.controlPoint = false, .supportsWriteResponse = false},
                   .ble = {.supportsBroadcastNotification = false,
                           .supportsDisconnectedNotification = false,
                           .readableWithoutSecurity = false,
                           .writableWithoutSecurity = false}},
    .constraints = {.maxLength = 64},
    .callbacks = {.handleRead = HAPHandleAccessoryInformationADKVersionRead, .handleWrite = NULL}};

const HAPService esp_hk_db_accessoryInformationService = {
    .iid = kIID_AccessoryInformation,
    .serviceType = &kHAPServiceType_AccessoryInformation,
    .debugDescription = kHAPServiceDebugDescription_AccessoryInformation,
    .name = NULL,
    .properties = {.primaryService = false, .hidden = false, .ble = {.supportsConfiguration = false}},
    .linkedServices = NULL,
    .characteristics = (const HAPCharacteristic *const[]){&esp_hk_db_accessoryInformationIdentifyCharacteristic,
                                                          &esp_hk_db_accessoryInformationManufacturerCharacteristic,
                                                          &esp_hk_db_accessoryInformationModelCharacteristic,
                                                          &esp_hk_db_accessoryInformationNameCharacteristic,
                                                          &esp_hk_db_accessoryInformationSerialNumberCharacteristic,
                                                          &esp_hk_db_accessoryInformationFirmwareRevisionCharacteristic,
                                                          &esp_hk_db_accessoryInformationHardwareRevisionCharacteristic,
                                                          &esp_hk_db_accessoryInformationADKVersionCharacteristic,
                                                          NULL}};

//----------------------------------------------------------------------------------------------------------------------

static const HAPDataCharacteristic esp_hk_db_ProtocolInformationServiceSignatureCharacteristic = {
    .format = kHAPCharacteristicFormat_Data,
    .iid = kIID_HAPProtocolInformationServiceSignature,
    .characteristicType = &kHAPCharacteristicType_ServiceSignature,
    .debugDescription = kHAPCharacteristicDebugDescription_ServiceSignature,
    .manufacturerDescription = NULL,
    .properties = {.readable = true,
                   .writable = false,
                   .supportsEventNotification = false,
                   .hidden = false,
                   .requiresTimedWrite = false,
                   .supportsAuthorizationData = false,
                   .ip = {.controlPoint = true},
                   .ble = {.supportsBroadcastNotification = false,
                           .supportsDisconnectedNotification = false,
                           .readableWithoutSecurity = false,
                           .writableWithoutSecurity = false}},
    .constraints = {.maxLength = 2097152},
    .callbacks = {.handleRead = HAPHandleServiceSignatureRead, .handleWrite = NULL}};

static const HAPStringCharacteristic esp_hk_db_ProtocolInformationVersionCharacteristic = {
    .format = kHAPCharacteristicFormat_String,
    .iid = kIID_HAPProtocolInformationVersion,
    .characteristicType = &kHAPCharacteristicType_Version,
    .debugDescription = kHAPCharacteristicDebugDescription_Version,
    .manufacturerDescription = NULL,
    .properties = {.readable = true,
                   .writable = false,
                   .supportsEventNotification = false,
                   .hidden = false,
                   .requiresTimedWrite = false,
                   .supportsAuthorizationData = false,
                   .ip = {.controlPoint = false, .supportsWriteResponse = false},
                   .ble = {.supportsBroadcastNotification = false,
                           .supportsDisconnectedNotification = false,
                           .readableWithoutSecurity = false,
                           .writableWithoutSecurity = false}},
    .constraints = {.maxLength = 64},
    .callbacks = {.handleRead = HAPHandleHAPProtocolInformationVersionRead, .handleWrite = NULL}};

const HAPService esp_hk_db_ProtocolInformationService = {
    .iid = kIID_HAPProtocolInformation,
    .serviceType = &kHAPServiceType_HAPProtocolInformation,
    .debugDescription = kHAPServiceDebugDescription_HAPProtocolInformation,
    .name = NULL,
    .properties = {.primaryService = false, .hidden = false, .ble = {.supportsConfiguration = true}},
    .linkedServices = NULL,
    .characteristics = (const HAPCharacteristic *const[]){&esp_hk_db_ProtocolInformationServiceSignatureCharacteristic,
                                                          &esp_hk_db_ProtocolInformationVersionCharacteristic,
                                                          NULL}};

//----------------------------------------------------------------------------------------------------------------------

static const HAPTLV8Characteristic esp_hk_db_PairSetupCharacteristic = {
    .format = kHAPCharacteristicFormat_TLV8,
    .iid = kIID_PairingPairSetup,
    .characteristicType = &kHAPCharacteristicType_PairSetup,
    .debugDescription = kHAPCharacteristicDebugDescription_PairSetup,
    .manufacturerDescription = NULL,
    .properties = {.readable = false,
                   .writable = false,
                   .supportsEventNotification = false,
                   .hidden = false,
                   .requiresTimedWrite = false,
                   .supportsAuthorizationData = false,
                   .ip = {.controlPoint = true},
                   .ble = {.supportsBroadcastNotification = false,
                           .supportsDisconnectedNotification = false,
                           .readableWithoutSecurity = true,
                           .writableWithoutSecurity = true}},
    .callbacks = {.handleRead = HAPHandlePairingPairSetupRead, .handleWrite = HAPHandlePairingPairSetupWrite}};

static const HAPTLV8Characteristic esp_hk_db_PairVerifyCharacteristic = {
    .format = kHAPCharacteristicFormat_TLV8,
    .iid = kIID_PairingPairVerify,
    .characteristicType = &kHAPCharacteristicType_PairVerify,
    .debugDescription = kHAPCharacteristicDebugDescription_PairVerify,
    .manufacturerDescription = NULL,
    .properties = {.readable = false,
                   .writable = false,
                   .supportsEventNotification = false,
                   .hidden = false,
                   .requiresTimedWrite = false,
                   .supportsAuthorizationData = false,
                   .ip = {.controlPoint = true},
                   .ble = {.supportsBroadcastNotification = false,
                           .supportsDisconnectedNotification = false,
                           .readableWithoutSecurity = true,
                           .writableWithoutSecurity = true}},
    .callbacks = {.handleRead = HAPHandlePairingPairVerifyRead, .handleWrite = HAPHandlePairingPairVerifyWrite}};

static const HAPUInt8Characteristic esp_hk_db_PairingFeaturesCharacteristic = {
    .format = kHAPCharacteristicFormat_UInt8,
    .iid = kIID_PairingPairingFeatures,
    .characteristicType = &kHAPCharacteristicType_PairingFeatures,
    .debugDescription = kHAPCharacteristicDebugDescription_PairingFeatures,
    .manufacturerDescription = NULL,
    .properties = {.readable = false,
                   .writable = false,
                   .supportsEventNotification = false,
                   .hidden = false,
                   .requiresTimedWrite = false,
                   .supportsAuthorizationData = false,
                   .ip = {.controlPoint = false, .supportsWriteResponse = false},
                   .ble = {.supportsDisconnectedNotification = false,
                           .supportsBroadcastNotification = false,
                           .readableWithoutSecurity = true,
                           .writableWithoutSecurity = false}},
    .units = kHAPCharacteristicUnits_None,
    .constraints = {.minimumValue = 0,
                    .maximumValue = UINT8_MAX,
                    .stepValue = 0,
                    .validValues = NULL,
                    .validValuesRanges = NULL},
    .callbacks = {.handleRead = HAPHandlePairingPairingFeaturesRead, .handleWrite = NULL}};

static const HAPTLV8Characteristic esp_hk_db_PairingPairingsCharacteristic = {
    .format = kHAPCharacteristicFormat_TLV8,
    .iid = kIID_PairingPairingPairings,
    .characteristicType = &kHAPCharacteristicType_PairingPairings,
    .debugDescription = kHAPCharacteristicDebugDescription_PairingPairings,
    .manufacturerDescription = NULL,
    .properties = {.readable = true,
                   .writable = true,
                   .supportsEventNotification = false,
                   .hidden = false,
                   .requiresTimedWrite = false,
                   .supportsAuthorizationData = false,
                   .ip = {.controlPoint = true},
                   .ble = {.supportsBroadcastNotification = false,
                           .supportsDisconnectedNotification = false,
                           .readableWithoutSecurity = false,
                           .writableWithoutSecurity = false}},
    .callbacks = {.handleRead = HAPHandlePairingPairingPairingsRead,
                  .handleWrite = HAPHandlePairingPairingPairingsWrite}};

const HAPService esp_hk_db_pairingService = {
    .iid = kIID_Pairing,
    .serviceType = &kHAPServiceType_Pairing,
    .debugDescription = kHAPServiceDebugDescription_Pairing,
    .name = NULL,
    .properties = {.primaryService = false, .hidden = false, .ble = {.supportsConfiguration = false}},
    .linkedServices = NULL,
    .characteristics = (const HAPCharacteristic *const[]){&esp_hk_db_PairSetupCharacteristic,
                                                          &esp_hk_db_PairVerifyCharacteristic,
                                                          &esp_hk_db_PairingFeaturesCharacteristic,
                                                          &esp_hk_db_PairingPairingsCharacteristic,
                                                          NULL}};
