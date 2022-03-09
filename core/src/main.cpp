
#include "main.h"
#include "esp_log.h"

#define LOG_LEVEL_LOCAL ESP_LOG_VERBOSE
#define LOG_TAG "MAIN"

static Main my_main;

extern "C" void app_main(void)
{
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
    return status;
}

void Main::run(void)
{
    ESP_LOGI(LOG_TAG, "Application running");
    vTaskDelay(pdSECOND);
}