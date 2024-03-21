#pragma once
#include <WiFi.h>
#include <esp_now.h>
#include "network_protocol.h"
#include "espnow.h"

const char* ssid = "ez";
const char* password = "ez123456";
int channel = 11;

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t own_mac[6]; 

struct_pairing pairingData;
MessageType messageType;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Failure");
}

// callback function that will be executed when data is received on ESPnow
void OnDataRecvESPNOW(const uint8_t * mac_addr, const uint8_t *incomingData, int len) { 
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
            if(pairingData.magic_bytes != MAGIC_BYTE_CLIENT){
                Serial.print("Wrong magic bytes: ");
                Serial.println(pairingData.magic_bytes);
                return;
            }
            addPeer(pairingData.macAddr, channel);
            sendPeerRequest(channel,own_mac, pairingData.macAddr, MAGIC_BYTE_SERVER);
            break;
        default :
            Serial.print("Unknown data type: ");
            Serial.println(type);
            break;
    }

}

void connected_to_ap(WiFiEvent_t wifi_event, WiFiEventInfo_t wifi_info) {
    channel = WiFi.channel();
    Serial.print("Connected to the WiFi network on channel ");
    Serial.println(channel);
    // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
    } else {
        esp_now_register_recv_cb(OnDataRecvESPNOW);
        esp_now_register_send_cb(OnDataSent);
        //addPeer(broadcastAddress, 0);
    }
}

void disconnected_from_ap(WiFiEvent_t wifi_event, WiFiEventInfo_t wifi_info) {
  Serial.println("Disconnected from the WiFi AP");
  WiFi.begin(ssid, password);
}

void got_ip_from_ap(WiFiEvent_t wifi_event, WiFiEventInfo_t wifi_info) {
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());
}

void setupNetwork() {
    Serial.println();
    Serial.print("ESP Board MAC Address:  ");
    Serial.println(WiFi.macAddress());
    WiFi.mode(WIFI_STA);
    esp_read_mac(own_mac, ESP_MAC_WIFI_STA);
    WiFi.onEvent(connected_to_ap, ARDUINO_EVENT_WIFI_STA_CONNECTED);
    WiFi.onEvent(got_ip_from_ap, ARDUINO_EVENT_WIFI_STA_GOT_IP);
    WiFi.onEvent(disconnected_from_ap, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

    WiFi.begin(ssid, password);
    Serial.print("\nConnecting to Wifi network ");
    Serial.println(ssid);
    //WiFi.setSleep(false);
}