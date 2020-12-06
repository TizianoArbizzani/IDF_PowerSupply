#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"

#include "esp_log.h"

#include "tftspi.h"
#include "tft.h"

static const char *TAG = "T2";

#define PRINT_STATS_EVERY 1000000

#define SPI_BUS TFT_HSPI_HOST

void task2(void *parameter)
{
    //esp_err_t Rv = 0;
    //int64_t ShowData = PRINT_STATS_EVERY;

    int Counts = 0, ret = 0;
    //uint32_t TftLed = 0;

    int tx, ty;

    ESP_LOGW(TAG, "(TFT+Touch) ... [Starting]");

    TFT_PinsInit();

    spi_lobo_device_handle_t spi = NULL;
    spi_lobo_device_handle_t tsspi = NULL;

    spi_lobo_bus_config_t buscfg = {
        .mosi_io_num = PIN_NUM_MOSI, // set SPI MOSI pin
        .miso_io_num = PIN_NUM_MISO, // set SPI MISO pin
        .sclk_io_num = PIN_NUM_CLK,  // set SPI CLK pin
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 6 * 1024,
    };

    spi_lobo_device_interface_config_t devcfg = {
        .clock_speed_hz = 8000000,        // Initial clock out at 8 MHz
        .mode = 0,                        // SPI mode 0
        .spics_io_num = -1,               // we will use external CS pin
        .spics_ext_io_num = PIN_NUM_CS,   // external CS pin
        .flags = LB_SPI_DEVICE_HALFDUPLEX // ALWAYS SET  to HALF DUPLEX MODE!! for display spi
    };

    spi_lobo_device_interface_config_t tsdevcfg = {
        .clock_speed_hz = 2500000,   //Clock out at 2.5 MHz
        .mode = 0,                   //SPI mode 0
        .spics_io_num = PIN_NUM_TCS, //Touch CS pin
        .spics_ext_io_num = -1,      //Not using the external CS
    };

    vTaskDelay(500 / portTICK_RATE_MS);
    ESP_LOGI(TAG, "(TFT+Touch) pin [SO=%d, SI=%d, CLK=%d, CS=%d TCS=%d", PIN_NUM_MISO, PIN_NUM_MOSI, PIN_NUM_CLK, PIN_NUM_CS, PIN_NUM_TCS);

    ret = spi_lobo_bus_add_device(SPI_BUS, &buscfg, &devcfg, &spi);
    assert(ret == ESP_OK);

    ESP_LOGI(TAG, "SPI: display device added to spi bus (%d)", SPI_BUS);
    tft_disp_spi = spi;

    ret = spi_lobo_device_select(spi, 1);
    assert(ret == ESP_OK);
    ESP_LOGI(TAG, "SPI TFT: test select passed");

    ret = spi_lobo_device_deselect(spi);
    assert(ret == ESP_OK);
    ESP_LOGI(TAG, "SPI TFT: test deselect passed");

    ESP_LOGI(TAG, "SPI: attached display device, speed=%u", spi_lobo_get_speed(spi));
    ESP_LOGI(TAG, "SPI: bus uses native pins: %s", spi_lobo_uses_native_pins(spi) ? "true" : "false");

    ret = spi_lobo_bus_add_device(SPI_BUS, &buscfg, &tsdevcfg, &tsspi);
    assert(ret == ESP_OK);
    ESP_LOGI(TAG, "SPI: touch screen device added to spi bus (%d)", SPI_BUS);
    tft_ts_spi = tsspi;

    ret = spi_lobo_device_select(tsspi, 1);
    assert(ret == ESP_OK);
    ESP_LOGI(TAG, "TOUCH TFT: test select passed");

    ret = spi_lobo_device_deselect(tsspi);
    assert(ret == ESP_OK);
    ESP_LOGI(TAG, "TOUCH TFT: test deselect passed");

    ESP_LOGI(TAG, "SPI: attached TS device, speed=%u", spi_lobo_get_speed(tsspi));

    // ==== Initialize the Display ====

    TFT_display_init();
    ESP_LOGI(TAG, "SPI: display init...OK");

    tft_max_rdclock = find_rd_speed();
    ESP_LOGI(TAG, "SPI: Max rd speed = %u", tft_max_rdclock);

    // ==== Set SPI clock used for display operations ====
    spi_lobo_set_speed(spi, DEFAULT_SPI_CLOCK);
    ESP_LOGI(TAG, "SPI: Changed speed to %u", spi_lobo_get_speed(spi));

    tft_font_rotate = 0;
    tft_text_wrap = 0;
    tft_font_transparent = 0;
    tft_font_forceFixed = 0;
    tft_gray_scale = 0;

    TFT_setGammaCurve(DEFAULT_GAMMA_CURVE);
    TFT_setRotation(LANDSCAPE_FLIP);
    TFT_setFont(UBUNTU16_FONT, NULL);
    TFT_resetclipwin();
    TFT_print("Test Write", CENTER, CENTER);

    //ShowData += esp_timer_get_time();

    while (1)
    {
        // if (esp_timer_get_time() > ShowData)
        // {
        //     ShowData += PRINT_STATS_EVERY;

        //     ESP_LOGI(TAG, "Ticks : %d", Counts);
        //     Counts = 0;
        // }
        // else
        // {
        //     Counts++;
        // }

        if (TFT_read_touch(&tx, &ty, 0))
        {
            ESP_LOGW(TAG, "Touch = %d:%d", tx, ty);
        }

        // WatchDog Trigger
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    ESP_LOGI(TAG, "All done!");

    vTaskDelete(NULL);
}