#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"

#include "esp_log.h"

static const char *TAG = "Task2";

#define PRINT_STATS_EVERY 1000000

extern "C"
{
    void task2(void *parameter);
}

void task2(void *parameter)
{
    //esp_err_t Rv = 0;

    int64_t ShowData = PRINT_STATS_EVERY;

    int Counts = 0;
    uint32_t TftLed = 0;

    ESP_LOGW(TAG, "Task2 (TFT) ......... [Starting]");

    gpio_pad_select_gpio(TFT_LED);
    gpio_set_direction(gpio_num_t::TFT_LED, gpio_mode_t::GPIO_MODE_OUTPUT);

    gpio_set_level(TFT_LED, TftLed);

    ShowData += esp_timer_get_time();

    while (1)
    {
        if (esp_timer_get_time() > ShowData)
        {
            ShowData += PRINT_STATS_EVERY;

            // TftLed = !TftLed;
            //gpio_set_level(TFT_LED, TftLed);

            ESP_LOGI(TAG, "Ticks : %d", Counts);
            Counts = 0;
        }
        else
        {
            Counts++;
        }

        // WatchDog Trigger
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    ESP_LOGI(TAG, "All done!");

    vTaskDelete(NULL);
}