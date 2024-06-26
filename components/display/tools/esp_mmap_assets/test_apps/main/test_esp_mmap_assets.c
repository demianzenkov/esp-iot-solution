/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "unity.h"
#include "unity_test_runner.h"
#include "unity_test_utils_memory.h"

#include "assets_generate.h"

static const char *TAG = "assets_mmap";

TEST_CASE("test assets mmap table", "[mmap_assets][assets]")
{
    mmap_assets_handle_t asset_handle;

    /* partitions.csv
     * --------------------------------------------------------
     * | Name   | Type | SubType | Offset | Size  | Flags     |
     * --------------------------------------------------------
     * | assets | data | spiffs  |        | 6000K |           |
     * --------------------------------------------------------
     */
    const mmap_assets_config_t config = {
        .partition_label = "assets",
        .max_files = TOTAL_MMAP_FILES,
        .checksum = MMAP_CHECKSUM,
    };

    TEST_ESP_OK(mmap_assets_new(&config, &asset_handle));

    /* Index from the enum MMAP_FILES(generated by esp_mmap_assets)
     * Used to reference specific files in the mmap table.
     */

    for (int i = 0; i < TOTAL_MMAP_FILES; i++) {
        const char* name = mmap_assets_get_name(asset_handle, i);
        const uint8_t* mem = mmap_assets_get_mem(asset_handle, i);
        int size = mmap_assets_get_size(asset_handle, i);
        int width = mmap_assets_get_width(asset_handle, i);
        int height = mmap_assets_get_height(asset_handle, i);

        ESP_LOGI(TAG, "Name:[%s], Memory:[%p], Size:[%d bytes], Width:[%d], Height:[%d]", name, mem, size, width, height);
    }

    mmap_assets_del(asset_handle);
}

// Some resources are lazy allocated in the LCD driver, the threadhold is left for that case
#define TEST_MEMORY_LEAK_THRESHOLD  (500)

static size_t before_free_8bit;
static size_t before_free_32bit;

void setUp(void)
{
    before_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    before_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
}

void tearDown(void)
{
    size_t after_free_8bit = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    size_t after_free_32bit = heap_caps_get_free_size(MALLOC_CAP_32BIT);
    unity_utils_check_leak(before_free_8bit, after_free_8bit, "8BIT", TEST_MEMORY_LEAK_THRESHOLD);
    unity_utils_check_leak(before_free_32bit, after_free_32bit, "32BIT", TEST_MEMORY_LEAK_THRESHOLD);
}

void app_main(void)
{
    printf("mmap assets TEST \n");
    unity_run_menu();
}
