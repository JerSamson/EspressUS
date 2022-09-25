#include "arduino_CAN_test.h"

bool arduino_CAN_test::init_CAN(uint8_t ctx, uint8_t crx)
{
    // start the CAN bus at 500 kbps
    CAN.setPins(crx, ctx);
    if (!CAN.begin(500E3)) {
        Serial.println("Starting CAN failed!");
        return false;
    }
    return true;
}

void arduino_CAN_test::test_CAN()
{
    // try to parse packet
    int packetSize = CAN.parsePacket();

    if (packetSize) {
        // received a packet
        Serial.print("Received ");

        if (CAN.packetExtended()) {
            Serial.print("extended ");
        }

        if (CAN.packetRtr()) {
        // Remote transmission request, packet contains no data
            Serial.print("RTR ");
        }

        Serial.print("packet with id 0x");
        Serial.print(CAN.packetId(), HEX);

        if (CAN.packetRtr()) {
            Serial.print(" and requested length ");
            Serial.println(CAN.packetDlc());
        } else {
            Serial.print(" and length ");
            Serial.println(packetSize);

            // only print packet data for non-RTR packets
            while (CAN.available()) {
                Serial.print((char)CAN.read());
            }
            Serial.println();
        }

        Serial.println();
    }
}