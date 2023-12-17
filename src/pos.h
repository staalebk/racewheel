#pragma once
#include "ble.h"

#define POS_DIVIDER 6000000.0

void inline getPos(double *lat, double *lon) {
    *lat = getData(DATA_LAT)/POS_DIVIDER;
    *lon = getData(DATA_LONG)/POS_DIVIDER;
}