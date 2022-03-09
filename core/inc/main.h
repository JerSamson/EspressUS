#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define pdSECOND pdMS_TO_TICKS(1000)

class Main final
{
private:
    /* data */
public:
    esp_err_t setup(void);
    void run(void);

    Main(/* args */);
    ~Main();
};

Main::Main(/* args */)
{
}

Main::~Main()
{
}
