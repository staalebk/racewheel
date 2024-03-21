#pragma once

void sendPeerRequest(int channel, uint8_t *own_mac, uint8_t *destination, int magic_bytes);
bool addPeer(const uint8_t *peer_addr, int chan);