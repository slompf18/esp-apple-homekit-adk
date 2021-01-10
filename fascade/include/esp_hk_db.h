// Copyright (c) 2015-2019 The HomeKit ADK Contributors
//
// Licensed under the Apache License, Version 2.0 (the “License”);
// you may not use this file except in compliance with the License.
// See [CONTRIBUTORS.md] for the list of HomeKit ADK project authors.

// Basic light bulb database example. This header file, and the corresponding DB.c implementation in the ADK, is
// platform-independent.

#ifndef DB_H
#define DB_H

#ifdef __cplusplus
extern "C" {
#endif

#include "HAP.h"

#if __has_feature(nullability)
#pragma clang assume_nonnull begin
#endif

/**
 * Total number of services and characteristics contained in the accessory.
 */
#define kAttributeCount ((size_t) 21)

/**
 * HomeKit Accessory Information service.
 */
extern const HAPService esp_hk_db_accessoryInformationService;

/**
 * Characteristics to expose accessory information and configuration (associated with Accessory Information service).
 */
extern const HAPBoolCharacteristic esp_hk_db_accessoryInformationIdentifyCharacteristic;
extern const HAPStringCharacteristic esp_hk_db_accessoryInformationManufacturerCharacteristic;
extern const HAPStringCharacteristic esp_hk_db_accessoryInformationModelCharacteristic;
extern const HAPStringCharacteristic esp_hk_db_accessoryInformationNameCharacteristic;
extern const HAPStringCharacteristic esp_hk_db_accessoryInformationSerialNumberCharacteristic;
extern const HAPStringCharacteristic esp_hk_db_accessoryInformationFirmwareRevisionCharacteristic;
extern const HAPStringCharacteristic esp_hk_db_accessoryInformationHardwareRevisionCharacteristic;
extern const HAPStringCharacteristic esp_hk_db_accessoryInformationADKVersionCharacteristic;

/**
 * HAP Protocol Information service.
 */
extern const HAPService esp_hk_db_ProtocolInformationService;

/**
 * Pairing service.
 */
extern const HAPService esp_hk_db_pairingService;

#if __has_feature(nullability)
#pragma clang assume_nonnull end
#endif

#ifdef __cplusplus
}
#endif

#endif
