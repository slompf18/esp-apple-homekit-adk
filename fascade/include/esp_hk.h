#pragma once

#include <esp_err.h>
#include <HAP.h>
#include "esp_hk_db.h"

esp_err_t esp_hk_init(HAPAccessory *accessory);
esp_err_t esp_hk_start();
esp_err_t esp_hk_raise_event(
    const HAPAccessory *accessory,
    const HAPService *service,
    const HAPCharacteristic *characteristic);