#include <stdio.h>
#include <string.h>

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "main";

void task1(void *parameter);
void task2(void *parameter);

//xQueueHandle MsgQueue; //!< InterProcess Message Queue
//static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED; //!< Mux Management variable
TaskHandle_t hTask1, hTask2; //!< Task Handle

#define CORE_0 0
#define CORE_1 1

void app_main()
{
    ESP_LOGW(TAG, "Starting Task Core 0");
    xTaskCreatePinnedToCore(task1, "Main Thread", 50000, NULL, 1, &hTask1, CORE_0);

    ESP_LOGW(TAG, "Starting Task Core 1");
    xTaskCreatePinnedToCore(task2, "TFT Thread", 50000, NULL, 1, &hTask2, CORE_1);
}
