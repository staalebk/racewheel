#pragma once
#include "ble.h"

#define POS_DIVIDER 6000000.0

void inline getPos(double *lat, double *lon) {
    int32_t ilat = getData(DATA_LAT);
    int32_t ilon = getData(DATA_LON);
    if(ilat == 0x7FFFFFFF)
        ilat = 0;
    if(ilon == 0x7FFFFFFF)
        ilon = 0;
    *lat = ilat/POS_DIVIDER;
    *lon = ilon/POS_DIVIDER;
}

bool inline hasValidPos() {
    double lat, lon;
    getPos(&lat, &lon);
    if (!lat || !lon)
        return false;
    return true;
}