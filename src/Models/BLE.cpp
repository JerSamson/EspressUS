#include "./Models/BLE.h"

// Static variables initialisation
bool BLE::deviceConnected = false;
bool BLE::oldDeviceConnected = false;
int BLE::num_client = 0;
BLEServer *BLE::pServer;
BLEService *BLE::pService;
BLEAdvertising *BLE::pAdvertising;

std::map<std::string, user_action> BLE::user_action_map = {
    // Leave empty at initialisation
};

std::map<std::string, BLECharacteristic *> BLE::characteristic_map = {
    // Leave empty at initialisation
};

std::map<std::string, std::string> BLE::characteristic_uuid_map = {
    {"Pressure",        PRESSURE_CHAR_UUID},
    {"CurrentState",    STATE_CHAR_UUID},
    {"Temp",            TEMP_CHAR_UUID},
    {"Load",            LOAD_CHAR_UUID},

    {"OperationMode",   OP_MODE_CHAR_UUID},
    {"Recipe",          RECIPE_CHAR_UUID},

    {"ManualHeat",      MANUAL_HEAT_CHAR_UUID},
    {"ManualFlush",     MANUAL_FLUSH_CHAR_UUID},
    {"ManualVerinUp",   MANUAL_V_UP_CHAR_UUID},
    {"ManualVerinDown", MANUAL_V_DOWN_CHAR_UUID},

    {"StartApp",        USER_ACTION_CHAR_UUID},
    {"PopUp",           POPUP_CONFIMATION_UUID}
};
std::map<std::string, uint32_t> BLE::characteristic_property_map = {
    {"Pressure",        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY},
    {"CurrentState",    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY},
    {"Temp",            BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY},
    {"Load",            BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY},

    {"OperationMode",   BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE},
    {"Recipe",          BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY},

    {"ManualHeat",      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE},
    {"ManualFlush",     BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE},
    {"ManualVerinUp",   BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE},
    {"ManualVerinDown", BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE},

    {"PopUp",           BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY},
    {"StartApp",        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY}
};
std::map<std::string, CHARAC_TYPE> BLE::characteristic_type_map = {
    {"Pressure",        DATA},
    {"CurrentState",    DATA},
    {"Temp",            DATA},
    {"Load",            DATA},

    {"OperationMode",   DATA},
    {"Recipe",          DATA},

    {"ManualHeat",      DATA},
    {"ManualFlush",     DATA},
    {"ManualVerinUp",   DATA},
    {"ManualVerinDown", DATA},

    {"PopUp",           USER_ACTION},    
    {"StartApp",        USER_ACTION}
};
//

void BLE::setup()
{
    Serial.println("INFO\t- BLE() - Initializing bluetooth module...");

    deviceConnected = false;
    oldDeviceConnected = false;

    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    pService = pServer->createService( BLEUUID::fromString(SERVICE_UUID), 60, 0);

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
            Serial.printf("ERROR\t- Ble_base::Setup() - %s characteristic has no uuid defined.\n", it->first.c_str());
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
    Serial.println("INFO\t- BLE() - Bluetooth module initialization complete.");
}

BLE::BLE()
{
    deviceConnected = false;
    oldDeviceConnected = false;
}

// Turn bit to true, returns if has been requested
bool BLE::request_user_action(std::string characteristicName)
{
    if(characteristic_type_map.at(characteristicName) != USER_ACTION){
        Serial.println("ERROR\t- BLE::request_user_action() - characteristic must user action type to provide proper callbacks.");
        return false;
    }

    if (!characteristic_has_persission(characteristicName, BLECharacteristic::PROPERTY_WRITE))
    {
        Serial.println("ERROR\t- BLE::request_user_action() - characteristic must have write permission");
        return false;
    }

    user_action userAction = user_action();
    userAction.chracteristic_name = characteristicName;
    userAction.ble_char = characteristic_map.at(characteristicName);

    update_characteristic(characteristicName, "0");

    user_action_map[characteristicName] = userAction;

    Serial.printf("INFO\t- BLE::request_user_action() - user action '%s' requested\n", characteristicName.c_str());

    return true;
}

// Returns wether the userAction has been received. Store value result in result.
bool BLE::try_get_user_action_result(std::string characteristicName, std::string &result){

    try{
        user_action userAction = user_action_map.at(characteristicName);

        if(userAction.received()){
            result = userAction.value();
            return true;
        }
    }
    catch(std::exception& e){
        Serial.printf("ERROR\t- try_get_user_action_result() - %s", e.what());
    }

    return false;
}

user_action BLE::get_user_action_from_characteristic(std::string uuid){
    auto it = characteristic_uuid_map.find(uuid);
    if (it == characteristic_uuid_map.end()){
        Serial.printf("WARNING - BLE::try_get_user_action_result() - No pending user action under char '%s'.\n", it->first.c_str());
        return user_action();
    }

    std::string charac_name = it->first;

    return user_action_map.at(charac_name);
}

bool BLE::user_action_requested(std::string characteristicName){
    if(user_action_map.count(characteristicName) == 0){
        return false;
    }
    return user_action_map.at(characteristicName).requested();
}

bool BLE::user_action_received(std::string characteristicName){
    if(user_action_map.count(characteristicName) == 0){
        return false;
    }
    return user_action_map.at(characteristicName).received();
}

// Returns True if was successful, False if the characteristic does not exist or if operation failed.
bool BLE::update_characteristic(std::string characteristicName, std::string value)
{
    try
    {
        BLECharacteristic *charac = tryGetCharacteristic(characteristicName);

        if (charac == nullptr)
        {
            Serial.printf("ERROR\t- BLE::update_characteristic() - Could not find characteristic '%s'\n", characteristicName.c_str());
            return false;
        }

        charac->setValue(value);

        if(characteristic_has_persission(characteristicName, BLECharacteristic::PROPERTY_NOTIFY)){
            charac->notify();
        }

        Serial.printf("INFO\t- updated characteristic '%s' to %s.\n", characteristicName.c_str(), value.c_str());
        return true;
    }
    catch (const std::exception &e)
    {
        Serial.printf("ERROR\t- Could not update characteristic '%s'.\n\t%s", characteristicName.c_str(), e.what());
        return false;
    }
}

bool BLE::update_characteristic(std::string characteristicName, int value){
    return update_characteristic(characteristicName, patch::to_string(toAscii(value)));
}

bool BLE::update_characteristic(std::string characteristicName, float value){
    char valueStr[10];
    sprintf(valueStr, "%4.4f", value);
    return update_characteristic(characteristicName, valueStr);
}

// Returns true if the characteristic exists and has the specified permissions.
bool BLE::characteristic_has_persission(std::string characteristicName, uint32_t permission)
{
    auto iter = characteristic_property_map.find(characteristicName);
    if (iter != characteristic_property_map.end())
    {
        return (iter->second && permission);
    }
    return false;
}

// Returns a characteristic with the given name if it exist, else NULL.
BLECharacteristic *BLE::tryGetCharacteristic(std::string name)
{
    auto iter = characteristic_map.find(name);
    if (iter != characteristic_map.end())
    {
        return iter->second;
    }
    Serial.printf("ERROR\t- BLE::tryGetCharacteristic - Could not get characteristic '%s'\n", name.c_str());
    return nullptr;
}

// Creates and returns a characteristic with a unique UUID and stores it in thecharacteristic table with the provided string as a key
BLECharacteristic *BLE::add_characteristic(std::string name, uint32_t permissions, std::string uuid, BLECharacteristicCallbacks *callbacks)
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
    Serial.printf("INFO\t- Added characteristic '%s' under UUID '%s'.\n", name.c_str(), uuid.c_str());

    // Add to UUID map if not already in it
    if (!characteristic_uuid_map.count(name))
    {
        characteristic_uuid_map.insert({name, uuid});
    }

    return characteristic;
}

void BLE::ServerCallbacks::onConnect(BLEServer *pServer)
{
    deviceConnected = true;
    num_client++;
    Serial.printf("INFO\t- BLE::onConnect() - Connected to %d client(s).\n", num_client);
};

void BLE::ServerCallbacks::onDisconnect(BLEServer *pServer)
{
    deviceConnected = false;
    num_client--;
    Serial.printf("INFO\t- BLE::onDisconnect() - Now connected to %d client(s).\n", num_client);
    pServer->startAdvertising(); // restart advertising
}

void BLE::UserActionCallback::onWrite(BLECharacteristic *pCharacteristic)
{
    std::string rxValue = pCharacteristic->getValue();
    pCharacteristic->notify();

    Serial.println("DEBUG\t- UserActionCallback::onWrite() invoked");

    if (rxValue.length() > 0)
    {

        Serial.print("INFO\t- Received U.A Value: ");

        for (int i = 0; i < rxValue.length(); i++)
        {
            Serial.print(rxValue[i]);
        }

        Serial.println();
    }
}

void BLE::DefaultCallback::onWrite(BLECharacteristic *pCharacteristic)
{
    std::string rxValue = pCharacteristic->getValue();
    std::string uuid = pCharacteristic->getUUID().toString();
    if (rxValue.length() > 0)
    {
        Serial.printf("INFO\t- onWrite - %s updated to ", uuid.c_str());

        for (int i = 0; i < rxValue.length(); i++)
        {
            Serial.print(rxValue[i]);
        }
        Serial.println();

        if(uuid == RECIPE_CHAR_UUID){
            std::stringstream ss; 
            int num;
            ss << rxValue;
            ss >> num;

            _Configuration::loadConfig((CONFIG)num);
        }
    }
}

void BLE::checkToReconnect() // added
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
