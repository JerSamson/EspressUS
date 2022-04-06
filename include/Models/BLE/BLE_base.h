#pragma once

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "./Models/Helpers/UUID_helper.h"
#include <map>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

namespace BLE
{
    class BLE_Base
    {
        private:
            std::string _deviceName;
            // Maps characteristics pointers to their given name
            std::map<std::string, BLECharacteristic*> characteristicTable;
            // Maps characteristics permissions to their given name
            std::map<std::string, uint32_t> characteristicPermssionTable;
            BLEServer *pServer;
            BLEService *pService;
            BLEAdvertising *pAdvertising;

        void setup(std::map<std::string, uint32_t> characteristics_map) {
            characteristicPermssionTable = characteristics_map;

            BLEDevice::init(_deviceName);
            pServer = BLEDevice::createServer();
            pService = pServer->createService(Helper::uuid_helper::AssignUUID());

            // Create a characteristic with a unique UUID and stores it in characteristic table with the provided string as a key
            for (std::map<std::string, uint32_t>::iterator it = characteristics_map.begin(); it != characteristics_map.end(); ++it)
            {
                add_characteristic(it->first, it->second);
            }
            
            pService->start();
            // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
            pAdvertising = BLEDevice::getAdvertising();
            pAdvertising->addServiceUUID(pService->getUUID());
            pAdvertising->setScanResponse(true);
            pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
            pAdvertising->setMinPreferred(0x12);
            BLEDevice::startAdvertising();
            }        
            
            public:
            BLE_Base(std::string deviceName, std::map<std::string, uint32_t> characteristics_map): 
                _deviceName{deviceName}
            {
                setup(characteristics_map);
            }

            // Returns True if was successful, False if the characteristic does not exist or if operation failed.
            bool update_characteristic(std::string characteristicName, std::string value)
            {
                try { 
                    
                    BLECharacteristic *charac = tryGetCharacteristic(characteristicName);
                    charac->setValue(value);
                    Serial.printf("INFO - updated characteristic '%s' from device '%s' to %s.\n", characteristicName.c_str(), _deviceName.c_str(), value.c_str());
                    return true;
                } 
                catch (const std::exception& e) {
                    Serial.printf("ERROR - Could not update characteristic '%s' from device '%s'.\n\t%s", characteristicName.c_str(), _deviceName.c_str(), e.what());
                    return false;
                }
            }

        // Returns true if the characteristic exists and has the specified permissions.
        bool characteristic_has_persission(std::string characteristicName, uint32_t permission)
        {
            auto iter = characteristicPermssionTable.find(characteristicName);
            if (iter != characteristicPermssionTable.end() )
            {
                return (iter->second && permission);
            }
            return false;
        }

        // Returns number of characteristics associated to the service.
        int len()
        {
            return characteristicTable.size();
        }

        // Returns a characteristic with the given name if it exist, else NULL.
        BLECharacteristic *tryGetCharacteristic(std::string name)
        {
            auto iter = characteristicTable.find(name);
            if (iter != characteristicTable.end() )
            {
                return iter->second;
            }
            return NULL;
        }

        // Creates and returns a characteristic with a unique UUID and stores it in thecharacteristic table with the provided string as a key
        BLECharacteristic *add_characteristic(std::string name, uint32_t permissions)
        {
                std::string uuid = Helper::uuid_helper::AssignUUID();
                BLECharacteristic *characteristic = pService->createCharacteristic(
                                                    uuid,
                                                    permissions
                                                );

                // Add characteristic to table
                characteristicTable.insert({name, characteristic});
                Serial.printf("INFO - Added characteristic '%s' to device '%s' under UUID '%s'.\n", name.c_str(), _deviceName.c_str(), uuid.c_str());

                return characteristic;
        }
        
    }; // BLE_base class

} // BLE namespace
