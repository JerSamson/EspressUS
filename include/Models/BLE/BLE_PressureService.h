#pragma once

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "./Models/BLE/BLE_base.h"
#include <map>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

namespace BLE
{
    class BLE_Pressure : public BLE_Base
    {
        private:
            BLECharacteristic *pPressureCharacteristic;
            const std::map<std::string, uint32_t> characteristics_map =
             {
                {"Pressure", BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY}
             };

        public:
            BLE_Pressure() 
            : BLE_Base{"PressureBLEService", characteristics_map}
            {
            }

            void update_pressure_value(float value)
            {
                char txString[8];
                dtostrf(value, 1, 2, txString);
                update_characteristic("Pressure", txString);
            }
    };
}
