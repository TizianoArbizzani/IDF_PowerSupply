#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/i2c.h"

#include "esp_log.h"

#include "INA219.h"

static const char *TAG = "Task1";

#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master do not need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master do not need buffer */
#define I2C_MASTER_FREQ_HZ 1000000  /*!< I2C master clock frequency */

#define PRINT_STATS_EVERY 1000000

extern "C"
{
    void task1(void *parameter);
}

void task1(void *parameter)
{
    esp_err_t Rv = 0;

    int64_t ShowData = PRINT_STATS_EVERY;

    int Counts = 0;

    float shuntvoltage = 0;
    float busvoltage = 0;
    float current_mA = 0;
    float loadvoltage = 0;
    float power = 0;

    int i2c_master_port = I2C_NUM_1;
    i2c_config_t conf;

    ESP_LOGW(TAG, "Task1 (MAIN) ........ [Starting]");

    INA219 ina219_left(INA219::I2C_ADDR_40);
    ESP_LOGW(TAG, "INA219 Left addr .... [%X]", INA219::I2C_ADDR_40);

    INA219 ina219_right(INA219::I2C_ADDR_41);
    ESP_LOGW(TAG, "INA219 Right addr ... [%X]", INA219::I2C_ADDR_41);

    conf.mode = i2c_mode_t::I2C_MODE_MASTER;
    conf.sda_io_num = gpio_num_t::GPIO_NUM_21;
    conf.sda_pullup_en = gpio_pullup_t::GPIO_PULLUP_ENABLE;
    conf.scl_io_num = gpio_num_t::GPIO_NUM_22;
    conf.scl_pullup_en = gpio_pullup_t::GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;

    Rv = i2c_param_config(i2c_master_port, &conf);
    ESP_LOGW(TAG, "I2C Configuration ... [%d]", Rv);

    Rv = i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
    ESP_LOGW(TAG, "I2C Driver install .. [%d]", Rv);

    ina219_left.begin();
    ESP_LOGW(TAG, "INA219 Left conf .... [OK]");

    ina219_right.begin();
    ESP_LOGW(TAG, "INA219 Right conf ... [OK]");

    ShowData += esp_timer_get_time();

    while (1)
    {
        shuntvoltage = ina219_left.shuntVoltage() * 1000;
        busvoltage = ina219_left.busVoltage();
        current_mA = ina219_left.shuntCurrent() * 1000;
        loadvoltage = busvoltage + (shuntvoltage / 1000);
        power = ina219_left.busPower() * 1000;

        //ESP_LOGI(TAG, "Left : %f %f %f %f %f", shuntvoltage, busvoltage, current_mA, loadvoltage, power);

        shuntvoltage = ina219_right.shuntVoltage() * 1000;
        busvoltage = ina219_right.busVoltage();
        current_mA = ina219_right.shuntCurrent() * 1000;
        loadvoltage = busvoltage + (shuntvoltage / 1000);
        power = ina219_right.busPower() * 1000;

        //ESP_LOGI(TAG, "Right: %f %f %f %f %f", shuntvoltage, busvoltage, current_mA, loadvoltage, power);

        if (esp_timer_get_time() > ShowData)
        {
            ShowData += PRINT_STATS_EVERY;

            ESP_LOGI(TAG, "Counts: %d", Counts);
            Counts = 0;
        }
        else
        {
            Counts++;
        }

        // WatchDog Trigger
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }

    ESP_LOGI(TAG, "All done!");

    vTaskDelete(NULL);
}