#pragma once

#include "Arduino.h"
#include <ArduinoJson.h>
#include <SD.h>
#include "map"

#define DEFAULT_TARGET_TEMP 95.0
#define MAX_TARGET_TEMP     100
#define MIN_TARGET_TEMP     30

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


    static _Configuration& getInstance()
    {
        static _Configuration instance; // Guaranteed to be destroyed.
                                        // Instantiated on first use.
        return instance;
    }

    static void loadConfig(const char *filename);
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