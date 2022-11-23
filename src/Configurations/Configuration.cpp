#include "Configuration.h"

float _Configuration::target_temp = DEFAULT_TARGET_TEMP;
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

    return valid;
}