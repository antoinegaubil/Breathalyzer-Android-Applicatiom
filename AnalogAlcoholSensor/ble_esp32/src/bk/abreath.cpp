#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Arduino.h>

// BLE server name
#define bleServerName "ABreath"

float hum;

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 300;

// Controls the flow of communication
bool deviceConnected = false;
bool prevDeviceConnected = false;

bool isReadyToSend = false;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
#define SERVICE_UUID "91bad492-b950-4226-aa2b-4ede9fa42f59"
#define CHARACTERISTIC_UUID "ca73b3ba-39f6-4ab3-91ae-186dc9577d99"

// Setup callbacks onConnect and onDisconnect
class MyServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        deviceConnected = true;
    };
    void onDisconnect(BLEServer *pServer)
    {
        deviceConnected = false;
    }
};

class MyCharacteristicCallback : public BLECharacteristicCallbacks
{

    void onWrite(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param)
    {
        isReadyToSend = true;
    }
};

BLECharacteristic *pCharacteristic;
BLEServer *pServer;
void setup()
{
    Serial.begin(115200);
    Serial.println("Starting BLE work!");

    BLEDevice::init(bleServerName);
    pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);
    pServer->setCallbacks(new MyServerCallbacks());
    pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    // pCharacteristic->setValue("First string sent");
    pCharacteristic->setCallbacks(new MyCharacteristicCallback());

    pService->start();

    // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();

    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);

    BLEDevice::startAdvertising();
    Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop()
{
    if (deviceConnected)
    {
        if ((millis() - lastTime) > timerDelay)
        {

            // Generating random numbers
            if (isReadyToSend)
            {

                hum = rand() % 20;
                static char buff[3];
                if (hum < 18)
                {
                    dtostrf(hum, 6, 2, buff);
                    pCharacteristic->setValue(buff);
                    pCharacteristic->notify();
                    Serial.print("Sending to ble: ");
                    Serial.println(hum);
                }
                else
                {
                    hum = -1;
                    static char buff[3];
                    dtostrf(hum, 6, 2, buff);
                    pCharacteristic->setValue(buff);
                    pCharacteristic->notify();
                    isReadyToSend = false;
                    Serial.println("Last packet to ble");
                }
            }

            prevDeviceConnected = true;
            lastTime = millis();
        }
    }

    else if (!deviceConnected && prevDeviceConnected)
    {
        delay(500);
        pServer->startAdvertising(); // Restart scanning
        Serial.println("Scanning");
        prevDeviceConnected = deviceConnected;
    }
    else if (deviceConnected && !prevDeviceConnected)
    {
        prevDeviceConnected = deviceConnected;
        Serial.println("Connecting...");
    }
}