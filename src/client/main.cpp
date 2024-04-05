#include <Arduino.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <esp_system.h>
#include "client.h"
#include "../network_protocol.h"
#include "../espnow.h"
#include "canbus.h"

// Define the MAC address of the receiver
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t master[] = {0x48, 0xE7, 0x29, 0x89, 0x40, 0xB0};
int ESPNOW_channel = 14;
bool locked = false;
int status = 0;
uint8_t own_mac[6]; 


struct_pairing pairingData;
MessageType messageType;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Failure");
}

// callback function that will be executed when data is received on ESPnow
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) { 
    // Copies the sender mac address to a string
    char macStr[18];
    Serial.print("Packet received from: ");
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.println(macStr);
    Serial.print("Packet length: ");
    Serial.println(len);
    //memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
    if(!len) {
        Serial.println("No data! (len == 0)");
        return;
    }
    uint8_t type = incomingData[0];       // first message byte is the type of message
    switch (type) {
        case DATA :
            Serial.println("Data!");
            break;
        case PAIRING :
            if(len != sizeof(pairingData)){
                Serial.print("Wrong length! It should be: ");
                Serial.println(sizeof(pairingData));
                return;
            }
            memcpy(&pairingData, incomingData, sizeof(pairingData));
            if(pairingData.magic_bytes != MAGIC_BYTE_SERVER){
                Serial.print("Wrong magic bytes: ");
                Serial.println(pairingData.magic_bytes);
                return;
            }
            addPeer(pairingData.macAddr, pairingData.channel);
            locked = true;
            ESPNOW_channel = pairingData.channel;
            Serial.print("Locked to master on channel: ");
            Serial.println(ESPNOW_channel);
            break;
        default :
            Serial.print("Unknown data type: ");
            Serial.println(type);
            break;
    }

}

void set_wifi_channel(int channel) {
    Serial.print("Switching to channel ");
    Serial.println(channel);
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
    ESPNOW_channel = channel;
}

void send_pairing_request() {
    if(locked) // Assuming you meant to only proceed if locked is true
        return;
    sendPeerRequest(0,own_mac,broadcastAddress, MAGIC_BYTE_CLIENT);
    delay(50);
    sendPeerRequest(0,own_mac,broadcastAddress, MAGIC_BYTE_CLIENT);
    delay(50);
    sendPeerRequest(0,own_mac,broadcastAddress, MAGIC_BYTE_CLIENT);
}

void scan_for_channel(){
    if (locked)
        return;

    ESPNOW_channel++;
    if (ESPNOW_channel > 14)
        ESPNOW_channel = 1;
    set_wifi_channel(ESPNOW_channel);   
}

void setup() {
    pinMode(YELLOW_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    // Turn the LED on (HIGH is the voltage level)
    digitalWrite(YELLOW_LED, LOW); 
    digitalWrite(RED_LED, LOW);
    Serial.begin(115200);
    // Initialize WiFi in STA mode
    WiFi.mode(WIFI_STA);
    sleep(5);
    Serial.println("Booting");
    esp_read_mac(own_mac, ESP_MAC_WIFI_STA);

    // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        status = 42;
        return;
    }
    addPeer(broadcastAddress, 0);
    // Once ESPNow is successfully Init, we will register for Send CB to
    // get the status of Trasnmitted packet
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);
    setupCanbus();
}

void loop() {
    if(locked)
        digitalWrite(YELLOW_LED, HIGH);
    else
        digitalWrite(YELLOW_LED, LOW); 
    // Send message
    if(!locked) {
        delay(500); // Wait a bit before sending the next message
        scan_for_channel();
        send_pairing_request();
    }
    pollCanbus();
    /*
    if(locked)
        sleep(5);
    Serial.print("I'm on channel ");
    Serial.println(ESPNOW_channel);
    */
    if(!locked)
        Serial.println(status);
}
