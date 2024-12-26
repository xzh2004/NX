#ifndef GEODISTANCE_HPP
#define GEODISTANCE_HPP

#include "loadosm.hpp"
#include <cmath>

#define PI 3.14159265359
#define EARTH_RADIUS 6378137.0
#define degToRad(x) (x * PI / 180.0)

// const float PI = 3.1415926535897932384626433832795;

inline float haversineDistance(const Node& a, const Node& b) {
    // 转换为弧度
    float lat1 = a.lat;
    float lon1 = a.lon;
    float lat2 = b.lat;
    float lon2 = b.lon;

    float dy = a.lon - b.lon;
    lat1 = degToRad(lat1);
    lon1 = degToRad(lon1);
    lat2 = degToRad(lat2);
    lon2 = degToRad(lon2);

    float dLat = lat2 - lat1;
    float dLon = lon2 - lon1;

    float ax = std::sin(dLat/2) * std::sin(dLat/2) +
               std::cos(lat1)   * std::cos(lat2)   *
               std::sin(dLon/2) * std::sin(dLon/2);

    float c = 2 * std::atan2(std::sqrt(ax), std::sqrt(1-ax));

    float distance = EARTH_RADIUS * c;
    return distance;
}

#endif // GEODISTANCE_HPP