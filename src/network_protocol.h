#pragma once
#include <stdint.h>

enum MessageType {PAIRING, DATA,};

#define MAGIC_BYTE_CLIENT 1337
#define MAGIC_BYTE_SERVER 4242

typedef struct struct_pairing {       // new structure for pairing
    uint8_t msgType;
    uint16_t magic_bytes;
    uint8_t macAddr[6];
    uint8_t channel;
} struct_pairing;
