
#include "main.h"
#include "esp_log.h"
#include "gpio.h"

#define LOG_LEVEL_LOCAL ESP_LOG_VERBOSE
#define LOG_TAG "MAIN"

static Main my_main;

extern "C" void app_main(void)
{
    // ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(my_main.setup());

    while(true)
    {
        my_main.run();
    }
}

esp_err_t Main::setup(void)
{
    esp_err_t status {ESP_OK};

    ESP_LOGI(LOG_TAG, "Setup Started");

    status |= led.init();

    return status;
}

void Main::run(void)
{
    ESP_LOGI(LOG_TAG, "Application running");

    led.set(true);
    ESP_LOGI(LOG_TAG, "LED ON");

    vTaskDelay(pdSECOND);

    led.set(false);
    ESP_LOGI(LOG_TAG, "LED OFF");

    vTaskDelay(pdSECOND);
}