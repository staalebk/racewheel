#include <Arduino.h>
#include <Wire.h>
#include <stdint.h>
#include <stdlib.h>
#include "esp_now.h"
#include "network_protocol.h"

void sendPeerRequest(int channel, uint8_t *own_mac, uint8_t *destination, int magic_bytes) {
    struct_pairing pairingRequest;
    pairingRequest.magic_bytes = magic_bytes;
    pairingRequest.channel = channel;
    pairingRequest.msgType = PAIRING;
    memcpy(pairingRequest.macAddr, own_mac, 6);
    esp_err_t result = esp_now_send(destination, (uint8_t *)&pairingRequest, sizeof(pairingRequest));
    if (result != ESP_OK) {
        Serial.print("Error sending the pairing request: ");
        Serial.println(result);
    }
}

bool addPeer(const uint8_t *peer_addr, int chan) {      // add pairing
    esp_now_peer_info_t peer;
    memset(&peer, 0, sizeof(peer));
    const esp_now_peer_info_t *peer_info = &peer;
    memcpy(peer.peer_addr, peer_addr, 6);
  
    peer.channel = chan; // pick a channel
    peer.encrypt = 0; // no encryption
    // check if the peer exists
    bool exists = esp_now_is_peer_exist(peer.peer_addr);
    if (exists) {
        // peer already paired.
        Serial.println("Already Paired");
        return true;
    }
    else {
        esp_err_t addStatus = esp_now_add_peer(peer_info);
        if (addStatus == ESP_OK) {
            // Pair success
            Serial.println("Pair success");
            return true;
        } else {
            Serial.println("Pair failed");
            return false;
        }
    }
} 