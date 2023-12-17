#include "pit.h"
#include "pos.h"


Point nbtrafikk[] = {{11.5598771, 59.8903128},
                    {11.5596182, 59.8902974},
                    {11.5596397, 59.8901285},
                    {11.559928, 59.890144},
                    {11.5598771, 59.8903128}};
int nbtrafikkSize = sizeof(nbtrafikk)/sizeof(nbtrafikk[0]);

Point radarveien[] = {{10.8079163,59.8738961},
              {10.8075287,59.8738295},
              {10.8076105,59.8737083},
              {10.8079968,59.8737783},
              {10.8079163,59.8738961}};
int radarveienSize = sizeof(radarveien)/sizeof(radarveien[0]);

bool isPointInPolygon(Point p, Point* polygon, int n) {
    bool inside = false;
    int i, j;
    for (i = 0, j = n - 1; i < n; j = i++) {
        if (((polygon[i].y > p.y) != (polygon[j].y > p.y)) &&
            (p.x < (polygon[j].x - polygon[i].x) * (p.y - polygon[i].y) / (polygon[j].y - polygon[i].y) + polygon[i].x)) {
            inside = !inside;
        }
    }
    return inside;
}

bool isInPit() {
    Point p;
    getPos(&p.y, &p.x);
    Serial.printf("pos: %.7f, %.5f\n", p.x, p.y);
    if(isPointInPolygon(p, nbtrafikk, nbtrafikkSize))
        return true;
    if(isPointInPolygon(p, radarveien, radarveienSize))
        return true;
    return false;
}