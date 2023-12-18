/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_check.h"
#include "nvs_flash.h"

#include "ui_main.h"

#include "bsp_board_extra.h"
#include "bsp/esp-bsp.h"
#include "settings.h"

#include "app_wifi.h"
#include "app_weather.h"

static char *TAG = "panel_main";

void panel_main(void)
{
    ESP_LOGI(TAG, "system start");
    bsp_spiffs_mount();
    bsp_i2c_init();
    bsp_extra_led_init();
    bsp_extra_codec_init();
    bsp_extra_player_init(BSP_SPIFFS_MOUNT_POINT"/Music");
    bsp_display_start();
    app_weather_start();
    // app_network_start();
    ESP_LOGI(TAG, "Display LVGL demo");
    ui_main();
}
