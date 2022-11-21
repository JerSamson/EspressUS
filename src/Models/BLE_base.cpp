#include "./Models/BLE/BLE_base.h"

// Static variables initialisation
bool BLE_Base::deviceConnected = false;
bool BLE_Base::oldDeviceConnected = false;
BLEServer *BLE_Base::pServer;
BLEService *BLE_Base::pService;
BLEAdvertising *BLE_Base::pAdvertising;


// struct characteristic_definition{
//     std::string uuid_str;
//     uint32_t permissions;
//     CHARAC_TYPE type;
// };

// Unused for now
// std::map<std::string, characteristic_definition> characteristic_definition_map = {
//     {
//         "Test", {
//             TEST_CHAR_UUID,
//             BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY,
//             DATA
//         }
//     },
//     {
//         "Pressure",{
//             PRESSURE_CHAR_UUID,
//             BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY,
//             DATA
//         }
//     },
//     {
//         "StartApp",{
//             USER_ACTION_CHAR_UUID,
//             BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY,
//             USER_ACTION
//         }
//     },
// };

std::map<std::string, user_action> BLE_Base::user_action_map = {
    // Leave empty at initialisation
};

std::map<std::string, BLECharacteristic *> BLE_Base::characteristic_map = {
    // Leave empty at initialisation
};

std::map<std::string, std::string> BLE_Base::characteristic_uuid_map = {
    // {"Test",            TEST_CHAR_UUID},
    // {"TestFloat",       TESTFLOAT_CHAR_UUID},

    {"Pressure",        PRESSURE_CHAR_UUID},
    {"CurrentState",    STATE_CHAR_UUID},
    {"Temp",            TEMP_CHAR_UUID},
    {"Load",            LOAD_CHAR_UUID},
    {"StartApp",        USER_ACTION_CHAR_UUID},

    {"ManualHeat",      MANUAL_HEAT_CHAR_UUID},
    {"ManualFlush",     MANUAL_FLUSH_CHAR_UUID},
    {"ManualVerinUp",   MANUAL_V_UP_CHAR_UUID},
    {"ManualVerinDown", MANUAL_V_DOWN_CHAR_UUID}
};
std::map<std::string, uint32_t> BLE_Base::characteristic_property_map = {
    // {"Test",            BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY},
    // {"TestFloat",       BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY},

    {"Pressure",        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY},
    {"CurrentState",    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY},
    {"Temp",            BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY},
    {"Load",            BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY},
    {"StartApp",        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY},

    {"ManualHeat",      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE},
    {"ManualFlush",     BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE},
    {"ManualVerinUp",   BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE},
    {"ManualVerinDown", BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE}
};
std::map<std::string, CHARAC_TYPE> BLE_Base::characteristic_type_map = {
    // {"Test",            DATA},
    // {"TestFloat",       DATA},

    {"Pressure",        DATA},
    {"CurrentState",    DATA},
    {"Temp",            DATA},
    {"Load",            DATA},
    {"StartApp",        USER_ACTION},

    {"ManualHeat",      DATA},
    {"ManualFlush",     DATA},
    {"ManualVerinUp",   DATA},
    {"ManualVerinDown", DATA}
};
//

void BLE_Base::setup()
{
    Serial.println("INFO - BLE_Base() - Initializing bluetooth module...");

    deviceConnected = false;
    oldDeviceConnected = false;

    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    pService = pServer->createService( BLEUUID::fromString(SERVICE_UUID), 30, 0);

    // Create a characteristic with a unique UUID and stores it in characteristic table with the provided string as a key
    for (std::map<std::string, uint32_t>::iterator it = characteristic_property_map.begin(); it != characteristic_property_map.end(); ++it)
    {
        // if in uuid map
        if (characteristic_uuid_map.count(it->first))
        {
            add_characteristic(it->first, it->second, characteristic_uuid_map.at(it->first));
        }
        else
        {
            Serial.printf("ERROR - Ble_base::Setup() - %s characteristic has no uuid defined.\n", it->first.c_str());
            continue;
        }
    }

    pService->start();
    BLEAdvertising *pAdvertising = pServer->getAdvertising(); // this still is working for backward compatibility
    pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(pService->getUUID());
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    Serial.println("INFO - BLE_Base() - Bluetooth module initialization complete.");
}

BLE_Base::BLE_Base()
{
    deviceConnected = false;
    oldDeviceConnected = false;
}

// Turn bit to true, returns if has been requested
bool BLE_Base::request_user_action(std::string characteristicName)
{
    if(characteristic_type_map.at(characteristicName) != USER_ACTION){
        Serial.println("WARNING - BLE_Base::request_user_action() - characteristic must user action type to provide proper callbacks.");
        return false;
    }

    if (!characteristic_has_persission(characteristicName, BLECharacteristic::PROPERTY_WRITE))
    {
        Serial.println("WARNING - BLE_Base::request_user_action() - characteristic must have write permission");
        return false;
    }

    user_action userAction = user_action();
    userAction.chracteristic_name = characteristicName;
    userAction.ble_char = characteristic_map.at(characteristicName);

    update_characteristic(characteristicName, "0");

    user_action_map[characteristicName] = userAction;

    Serial.printf("INFO - BLE_Base::request_user_action() - Request action requested '%s'\n", characteristicName.c_str());

    return true;
}

// Returns wether the userAction has been requested or has been received. Store result in result.
bool BLE_Base::try_get_user_action_result(std::string characteristicName, std::string &result){

    try{
        user_action userAction = user_action_map.at(characteristicName);

        if(userAction.received()){
            result = userAction.value();
            return true;
        }
    }
    catch(std::exception& e){
        Serial.printf("ERROR - try_get_user_action_result() - %s", e.what());
    }

    return false;
}

user_action BLE_Base::get_user_action_from_characteristic(std::string uuid){
    auto it = characteristic_uuid_map.find(uuid);
    if (it == characteristic_uuid_map.end()){
        Serial.printf("WARNING - BLE_Base::try_get_user_action_result() - No pending user action under char '%s'.\n", it->first.c_str());
        return user_action();
    }

    std::string charac_name = it->first;

    return user_action_map.at(charac_name);
}

bool BLE_Base::user_action_requested(std::string characteristicName){
    if(user_action_map.count(characteristicName) == 0){
        return false;
    }
    return user_action_map.at(characteristicName).requested();
}

bool BLE_Base::user_action_received(std::string characteristicName){
    if(user_action_map.count(characteristicName) == 0){
        return false;
    }
    return user_action_map.at(characteristicName).received();
}

// Returns True if was successful, False if the characteristic does not exist or if operation failed.
bool BLE_Base::update_characteristic(std::string characteristicName, std::string value)
{
    try
    {
        BLECharacteristic *charac = tryGetCharacteristic(characteristicName);

        if (charac == nullptr)
        {
            Serial.printf("ERROR - BLE_Base::update_characteristic() - Could not find characteristic '%s'\n", characteristicName.c_str());
            return false;
        }

        charac->setValue(value);

        if(characteristic_has_persission(characteristicName, BLECharacteristic::PROPERTY_NOTIFY)){
            charac->notify();
        }

        Serial.printf("INFO - updated characteristic '%s' to %s.\n", characteristicName.c_str(), value.c_str());
        return true;
    }
    catch (const std::exception &e)
    {
        Serial.printf("ERROR - Could not update characteristic '%s'.\n\t%s", characteristicName.c_str(), e.what());
        return false;
    }
}

bool BLE_Base::update_characteristic(std::string characteristicName, int value){
    return update_characteristic(characteristicName, patch::to_string(toAscii(value)));
}

bool BLE_Base::update_characteristic(std::string characteristicName, float value){
    char valueStr[10];
    sprintf(valueStr, "%4.4f", value);
    return update_characteristic(characteristicName, valueStr);
}

// Returns true if the characteristic exists and has the specified permissions.
bool BLE_Base::characteristic_has_persission(std::string characteristicName, uint32_t permission)
{
    auto iter = characteristic_property_map.find(characteristicName);
    if (iter != characteristic_property_map.end())
    {
        return (iter->second && permission);
    }
    return false;
}

// Returns a characteristic with the given name if it exist, else NULL.
BLECharacteristic *BLE_Base::tryGetCharacteristic(std::string name)
{
    auto iter = characteristic_map.find(name);
    if (iter != characteristic_map.end())
    {
        return iter->second;
    }
    return nullptr;
}

// Creates and returns a characteristic with a unique UUID and stores it in thecharacteristic table with the provided string as a key
BLECharacteristic *BLE_Base::add_characteristic(std::string name, uint32_t permissions, std::string uuid, BLECharacteristicCallbacks *callbacks)
{

    // Create characteristic
    BLECharacteristic *characteristic = pService->createCharacteristic(
        uuid,
        permissions);

    // Add callbacks
    switch (characteristic_type_map.at(name))
    {
    case DATA:
        characteristic->setCallbacks(new DefaultCallback());
        break;
    case USER_ACTION:
        characteristic->setCallbacks(new UserActionCallback());
        break;
    default:
        break;
    }

    characteristic->addDescriptor(new BLE2902());

    if(characteristic_has_persission(name, BLECharacteristic::PROPERTY_NOTIFY))
        characteristic->setNotifyProperty(true);

    characteristic->setValue("0");

    // Add characteristic to table
    characteristic_map.insert({name, characteristic});
    Serial.printf("INFO - Added characteristic '%s' under UUID '%s'.\n", name.c_str(), uuid.c_str());

    // Add to UUID map if not already in it
    if (!characteristic_uuid_map.count(name))
    {
        characteristic_uuid_map.insert({name, uuid});
    }

    return characteristic;
}

void BLE_Base::ServerCallbacks::onConnect(BLEServer *pServer)
{
    deviceConnected = true;
    Serial.printf("INFO - BLE::onConnect() - Connected to %d client(s).\n", pServer->getConnectedCount());
};

void BLE_Base::ServerCallbacks::onDisconnect(BLEServer *pServer)
{
    Serial.printf("INFO - BLE::onDisconnect() - Now connected to %d client(s).\n", pServer->getConnectedCount());
    deviceConnected = false;
}

void BLE_Base::UserActionCallback::onWrite(BLECharacteristic *pCharacteristic)
{
    std::string rxValue = pCharacteristic->getValue();
    pCharacteristic->notify();

    Serial.println("DEBUG - UserActionCallback::onWrite() invoked");

    if (rxValue.length() > 0)
    {

        Serial.print("Received U.A Value: ");

        for (int i = 0; i < rxValue.length(); i++)
        {
            Serial.print(rxValue[i]);
        }

        Serial.println();
    }
}

void BLE_Base::DefaultCallback::onWrite(BLECharacteristic *pCharacteristic)
{
    std::string rxValue = pCharacteristic->getValue();

    if (rxValue.length() > 0)
    {
        Serial.printf("INFO - onWrite - %s updated to", pCharacteristic->getUUID().toString());

        for (int i = 0; i < rxValue.length(); i++)
        {
            Serial.print(rxValue[i]);
        }
        Serial.println();
    }
}

void BLE_Base::checkToReconnect() // added
{
    // disconnected so advertise
    if (!deviceConnected && oldDeviceConnected)
    {
        delay(500);                  // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("Disconnected: start advertising");
        oldDeviceConnected = deviceConnected;
    }
    
    // connected so reset boolean control
    if (deviceConnected && !oldDeviceConnected)
    {
        // do stuff here on connecting
        Serial.println("Reconnected");
        oldDeviceConnected = deviceConnected;
    }
}
