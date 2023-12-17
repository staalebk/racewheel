#pragma once

enum PitState {
    Unknown,
    InThePits,
    NotInThePits
};

struct Point {
    double x;
    double y;
};

bool isInPit();
