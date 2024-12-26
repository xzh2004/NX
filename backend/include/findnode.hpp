#ifndef FINDNODE_HPP
#define FINDNODE_HPP

#include "loadosm.hpp"
#include "buildgraph.hpp"
#include "geodistance.hpp"


// 查找最近节点
ll find_nearest_node(float lat, float lon) {
    float min_distance = std::numeric_limits<float>::infinity();
    ll nearest_id = -1;
    for (const auto& pair : nodes) {
        float distance = haversineDistance(pair.second, Node(lat, lon));
        if (min_distance - distance > 1.0) {
            min_distance = distance;
            nearest_id = pair.first;
        }
    }
    return nearest_id;
}

#endif