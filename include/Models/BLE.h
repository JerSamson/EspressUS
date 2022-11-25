#pragma once

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <map>
#include <list>
#include <BLE2902.h>
#include "patch.h"

#define TEST_CHAR_UUID          "cb4e9606-267f-4494-8974-2b10c64ea6da"
#define TESTFLOAT_CHAR_UUID     "f83fe164-46b2-4ed6-9b92-18ab77fa8c54"

#define SERVICE_UUID            "b72c2c7a-6b9b-4eee-82e7-b04983efca6d"

#define USER_ACTION_CHAR_UUID   "4f509bf7-1e77-4f3a-8852-3d86fc48b344"
#define POPUP_CONFIMATION_UUID  "25ebbc6f-6166-4e17-a3f8-cce9e3735a97"

#define PRESSURE_CHAR_UUID      "4efc5750-2abb-425c-801d-d6d111f83cb7"
#define STATE_CHAR_UUID         "e525d474-a055-4f39-9387-6c46e54fcd78"
#define TEMP_CHAR_UUID          "25a62c3f-e74e-4dfd-96c2-bd9da4cac697"
#define LOAD_CHAR_UUID          "d268e4e3-c6f9-491b-a149-fee17cbc3ed9"

#define OP_MODE_CHAR_UUID       "2cc0f92e-12d0-49d2-9d0e-2fa4ea27c525"
#define MANUAL_HEAT_CHAR_UUID   "41d4fe9f-0f2e-447f-82dc-a71e9c684f38"
#define MANUAL_FLUSH_CHAR_UUID  "bd9286f9-f68c-419b-90f3-69d570fea9e6"
#define MANUAL_V_UP_CHAR_UUID   "0592b5b5-65be-4382-bcd2-0f8a54465ef8"
#define MANUAL_V_DOWN_CHAR_UUID "73d79bc7-847a-4aaa-8420-ba8cf50da7ab"

#define DEVICE_NAME             "ESPRESSUS"

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

enum CHARAC_TYPE {
    USER_ACTION,
    DATA
};

// Struct to communicate user action to android app:
// Characteristic value :  0            -> User action requested
// Characteristic value : -1            -> User action 'Idle'
// Characteristic value : anything else -> User action received

struct user_action{
    std::string chracteristic_name;
    BLECharacteristic* ble_char;

    bool requested(){
        std::string value = ble_char->getValue();
        return value == "0";
    };

    bool received(){
        std::string value = ble_char->getValue();
        return value != "0" && value != "-1";
    };

    std::string value(){
      return ble_char->getValue();  
    };
};


class BLE
{
    private:
    static bool deviceConnected;
    static bool oldDeviceConnected;
    static BLEServer *pServer;
    static BLEService *pService;
    static BLEAdvertising *pAdvertising;
    static int num_client;

    // Maps characteristics UUID to their given name
    static std::map<std::string, std::string> characteristic_uuid_map;
    // Maps characteristics permissions to their given name
    static std::map<std::string, uint32_t> characteristic_property_map;
    // Maps characteristics type (used for callbacks) to their given name
    static std::map<std::string, CHARAC_TYPE> characteristic_type_map;
    // Maps characteristics pointers to their given name
    static std::map<std::string, BLECharacteristic*> characteristic_map;
    // Maps active user actions to it's characteristic name
    static std::map<std::string, user_action> user_action_map;
    BLE();

    public:

    static void setup();        

    // Returns True if was successful, False if the characteristic does not exist or if operation failed.
    static bool update_characteristic(std::string characteristicName, std::string value);
    static bool update_characteristic(std::string characteristicName, int value);
    static bool update_characteristic(std::string characteristicName, float value);

    // Returns true if the characteristic exists and has the specified permissions.
    static bool characteristic_has_persission(std::string characteristicName, uint32_t permission);

    static bool request_user_action(std::string characteristicName);
    static bool try_get_user_action_result(std::string characteristicName, std::string &result);
    static bool user_action_requested(std::string characteristicName);
    static bool user_action_received(std::string characteristicName);
    static user_action get_user_action_from_characteristic(std::string uuid);

    // Returns number of characteristics associated to the service.
    static int len()
    {
        return characteristic_map.size();
    }

    static int n_client(){
        return num_client;
    }

    // Returns a characteristic with the given name if it exist, else NULL.
    static BLECharacteristic *tryGetCharacteristic(std::string name);

    // Creates and returns a characteristic with a unique UUID and stores it in thecharacteristic table with the provided string as a key
    static BLECharacteristic *add_characteristic(std::string name, uint32_t permissions, std::string uuid, BLECharacteristicCallbacks *callbacks = nullptr);
    
    static void checkToReconnect(); 

    class ServerCallbacks: public BLEServerCallbacks {
        void onConnect(BLEServer* pServer);
        void onDisconnect(BLEServer* pServer);
    };

    class UserActionCallback: public BLECharacteristicCallbacks {
        void onWrite(BLECharacteristic *pCharacteristic);
    };

    class DefaultCallback: public BLECharacteristicCallbacks {
        void onWrite(BLECharacteristic *pCharacteristic);
    };


}; // BLE class

