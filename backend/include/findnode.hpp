#ifndef FINDNODE_HPP
#define FINDNODE_HPP

#include "loadosm.hpp"
#include "buildgraph.hpp"
#include "geodistance.hpp"


// 查找最近节点
int find_nearest_node(float lat, float lon) {
    float min_distance = std::numeric_limits<float>::infinity();
    int nearest_id = -1;
    for (int i = 1; i <= N_SIZE; i++) {
        float distance = haversineDistance(node_set[i], Node(lat, lon));
        if (min_distance - distance > 1.0) {
            min_distance = distance;
            nearest_id = i;
        }
    }
    return nearest_id;
}

#endif