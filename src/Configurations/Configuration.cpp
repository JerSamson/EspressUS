#include "Configuration.h"

float _Configuration::target_temp = DEFAULT_TARGET_TEMP;
float _Configuration::target_pressure = DEFAULT_TARGET_PRESSURE;
float _Configuration::preinf_pressure = DEFAULT_PREINF_PRESSURE;
float _Configuration::preinf_weight = DEFAULT_PREINF_WEIGHT;
char _Configuration::config_name[] = "Default";

void _Configuration::loadConfig(const char *filename) {
    File file = SD.open(filename);

    StaticJsonDocument<512> doc;

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, file);
    if (error)
        Serial.println(F("Failed to read file, using default configuration"));
        else{
            Serial.printf("Loading configuration from '%s':\n", filename);
            printFile(filename); 
        }

    // Copy values from the JsonDocument to the Config
    strlcpy(config_name,                  // <- destination
            doc["name"] | "Default",      // <- source
            sizeof(config_name));         // <- destination's capacity

    target_temp = doc["target_temp"] | DEFAULT_TARGET_TEMP;
    target_temp = doc["target_pressure"] | DEFAULT_TARGET_TEMP;
    target_temp = doc["preinf_pressure"] | DEFAULT_TARGET_TEMP;
    target_temp = doc["preinf_weight"] | DEFAULT_TARGET_TEMP;

    file.close();
}

// Prints the content of a file to the Serial
void _Configuration::printFile(const char *filename) {
  // Open file for reading
  File file = SD.open(filename);
  if (!file) {
    Serial.println(F("Failed to read file"));
    return;
  }

  // Extract each characters by one by one
  while (file.available()) {
    Serial.print((char)file.read());
  }
  Serial.println();

  // Close the file
  file.close();
}

bool _Configuration::is_valid(){
    bool valid = true;
    valid = valid && target_temp != NAN && target_temp > MIN_TARGET_TEMP && target_temp < MAX_TARGET_TEMP;
    valid = valid && target_pressure != NAN && target_pressure > MIN_TARGET_PRESSURE && target_pressure < MAX_TARGET_PRESSURE;
    valid = valid && preinf_pressure != NAN && preinf_pressure > MIN_PREINF_PRESSURE && preinf_pressure < MAX_PREINF_PRESSURE;
    valid = valid && preinf_weight != NAN && preinf_weight > MIN_PREINF_WEIGHT && preinf_weight < MAX_PREINF_WEIGHT;

    return valid;
}