#pragma once

//
// RPI_COM.h
//
// This header holds the communication protocol with the raspberry pi
//

#include <Arduino.h>
#include <Wire.h>
#include <vector> 

using namespace std;

struct Victim {
/*
Victim positionon image
*/
    int8_t angle = 0;
    uint8_t dist = 0;
};

namespace rpi {
/*
namespace that holds raspberry pi comm fucntions
*/

    extern void setup();
    /*
    start communication
    */

    enum Ai {
    /*
    select which ai to start
    */
        NONE    = 0x00,
        VICTIMS = 0x01,
        CORNERS = 0x02,
        EXITS   = 0x03
    };

    extern void start_ai(Ai ai);
    /*
    start the ai

    [param ai] ai to start
    */

    extern void stop_ai();
    /*
    stop any ai
    */

    extern void reset_signal();
    /*
    send a reset signal to the pi
    */

    extern Victim get_victim();
    /*
    get the data from the victim ai

    !raises! AINotStartedException
    [return] nearest victim information
    */

    extern int8_t get_corner_green();
    /*
    get the data from the corner "ai"

    [return] corner angle for both corners or 0xFF each
    */

    extern int8_t get_corner_red();
    /*
    get the data from the corner "ai"

    [return] corner angle for both corners or 0xFF each
    */

    extern uint8_t status();
    /*
    get the pi's status message

    0xFF - everything fine
    */

    class AINotStartedException : exception{
        public:
            Ai ai;
            AINotStartedException(Ai ai);
            const char * what();
    };

}

#include <NimBLEDevice.h>
namespace opta {
    #define SERVICE_UUID        "0987654321"
    #define CHARACTERISTIC_UUID "1"
    #define TARGET_NAME         "Se Arm"

    static NimBLERemoteCharacteristic* status = nullptr;
    static NimBLEClient* client = nullptr;
    static bool connected = false;

    inline void connect() {
        NimBLEDevice::init("");
        NimBLEScan* pScan = NimBLEDevice::getScan();
        NimBLEDevice::setPower(ESP_PWR_LVL_P9); // Max power if needed
        NimBLEScanResults results = pScan->getResults(1000);

        NimBLEUUID serviceUuid(SERVICE_UUID);

        for (int i = 0; i < results.getCount(); i++) {
            const NimBLEAdvertisedDevice* device = results.getDevice(i);
            Serial.println(); Serial.print(i); Serial.print(": "); Serial.print(device->getAddress().toString().c_str());

            if (device->getAddress().toString() == "a8:61:0a:4e:64:0c") {
              if (client != nullptr) NimBLEDevice::deleteClient(client);
                Serial.println("\nINFO: Found device with correct name");
                client = NimBLEDevice::createClient();

                if (!client) { // Make sure the client was created
                    Serial.println("ERROR: Failed to create client!");
                    break;
                }

                if (client->connect(device)) {
                    connected = true;
                    NimBLERemoteService *pService = client->getService(serviceUuid);

                    if (pService != nullptr) {
                        status = pService->getCharacteristic(CHARACTERISTIC_UUID);
                    } else {
                      Serial.println("ERROR: Service unknown!");
                    }
                } else {
                  Serial.println("ERROR: Failed to connect!");
                }
                //NimBLEDevice::deleteClient(pClient);
            }
        }
        if(!connected){
          Serial.println("\n...failed!");
        } else {
          Serial.println("\n...succes!");
        }
    }
}

