#ifndef DIJKSTRA_HPP
#define DIJKSTRA_HPP

#include "pugixml.hpp"
#include "loadosm.hpp"
#include "buildgraph.hpp"

#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <cmath>
#include <limits>
#include <cstring>

// 定义优先队列元素
struct QueueElement {
    int node;
    float distance;

    inline bool operator<(const QueueElement& other) const {
        return distance > other.distance; // 最小距离优先
    }
    QueueElement(int x, float y): node(x), distance(y){}
};

float distances[NODE_SIZE];
int predecessors[NODE_SIZE];
int path_id[NODE_SIZE], path_len;

// 实现 Dijkstra 算法并记录前驱节点
void dijkstra(int start, int ed) {
    std::priority_queue<QueueElement> queue;
    // std::unordered_map<ll, ll> predecessors;
    for (int i = 1; i <= N_SIZE; i++) {
        distances[i] = std::numeric_limits<float>::infinity();
    }
    distances[start] = 0.0;
    queue.push(QueueElement(start, 0.0));

    Node ne = nodes.at(id_table[ed]);

    while (!queue.empty()) {
        QueueElement current = queue.top();
        queue.pop();
        int u = current.node;
        // out << "u: " << u << " " << id_table[u] << " " << nodes[id_table[u]].name << "\n";
        if (current.distance - distances[current.node] > 1.0) {
            // out << "End\n";
            continue;
        }
        for (int i = graph.head[u]; i; i = graph[i].nxt) {
            int v = graph[i].to;
            //  out << "v: " << v << " " << nodes[id_table[v]].name << "\n";
            float new_distance = current.distance + graph[i].weight;
            if (distances[v] - new_distance > 1.0) {
                distances[v] = new_distance;
                predecessors[v] = u;
                Node now = nodes.at(id_table[v]);
                if (current.distance + haversineDistance(ne, now) - distances[ed] < 1.0){
                    queue.push(QueueElement(v, new_distance));
                }
            }
        }
    }
}

// 恢复路径
bool reconstruct_path(int start, int end) {
    path_len = 0;
    int current = end;
    while (current != start) {
        path_id[++path_len] = current;
        current = predecessors[current];
    }
    path_id[++path_len] = current;
    return 1;
}


#endif