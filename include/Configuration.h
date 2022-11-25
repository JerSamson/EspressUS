#pragma once

#include "Arduino.h"
#include <ArduinoJson.h>
#include <SD.h>
#include "map"

#define DEFAULT_TARGET_TEMP 95.0
#define DEFAULT_TARGET_PRESSURE 8.5
#define DEFAULT_PREINF_PRESSURE 2.0
#define DEFAULT_PREINF_WEIGHT 5.0
#define MAX_TARGET_TEMP     100
#define MIN_TARGET_TEMP     30
#define MAX_TARGET_PRESSURE     9.5
#define MIN_TARGET_PRESSURE     6.0
#define MAX_PREINF_PRESSURE     3.5
#define MIN_PREINF_PRESSURE     0.8
#define MAX_PREINF_WEIGHT     10.0
#define MIN_PREINF_WEIGHT     2.0

enum CONFIG{
    Default
};

const std::map<CONFIG, const char*> config_map = {
    {CONFIG::Default, "src\\Configurations\\config_default.json"}
};

class _Configuration{
    public:

    static char config_name[];
    static float target_temp;
    static float target_pressure;
    static float preinf_pressure;
    static float preinf_weight;


    static _Configuration& getInstance()
    {
        static _Configuration instance; // Guaranteed to be destroyed.
                                        // Instantiated on first use.
        return instance;
    }

    static void loadConfig(const char *filename);
    static void loadConfig(CONFIG config);
    
    static bool is_valid();
    // Prints the content of a file to the Serial
    static void printFile(const char *filename);
    // _Configuration(_Configuration const&)        = delete;
    // void operator=(_Configuration const&)        = delete;

    private:

    _Configuration() {}
    // _Configuration(_Configuration const&);       // Don't Implement
    // void operator=(_Configuration const&);       // Don't implement
};