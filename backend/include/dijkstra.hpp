#ifndef DIJKSTRA_HPP
#define DIJKSTRA_HPP

#include "loadosm.hpp"
#include "buildgraph.hpp"

#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <cmath>
#include <limits>
#include <cstring>

float distances[NODE_SIZE];

// 定义优先队列元素
struct QueueElement {
    int node;
    float distance;

    inline bool operator<(const QueueElement& other) const {
        return distance > other.distance; // 最小距离优先
    }
    QueueElement(int x, float y): node(x), distance(y){}
};

// 实现 Dijkstra 算法并记录前驱节点
void dijkstra(int start, int ed, int display_search) {
    std::priority_queue<QueueElement> queue;
    // std::unordered_map<ll, ll> predecessors;
    for (int i = 1; i <= N_SIZE; i++) {
        distances[i] = std::numeric_limits<float>::infinity();
        predecessors[i] = -1;
    }
    distances[start] = 0.0;
    // queue.push(QueueElement(start, 0.0));
    queue.emplace(start, 0.0);
    Node ne = node_set[ed];
    
    while (!queue.empty()) {
        QueueElement current = queue.top();
        queue.pop();
        int u = current.node;
        // out << "u: " << u << " " << id_table[u] << " " << nodes[id_table[u]].name << "\n";
        if (current.distance - distances[u] > 1.0) {
            // out << "End\n";
            continue;
        }
        if (display_search) search_points[search_cnt++] = u;
        // std ::  cout << "u:" << u << std :: endl;
        for (int i = graph.head[u]; i; i = graph[i].nxt) {
            int v = graph[i].to;
            //  out << "v: " << v << " " << nodes[id_table[v]].name << "\n";
            float new_distance = current.distance + graph[i].weight;
            if (distances[v] - new_distance > 1.0) {
                distances[v] = new_distance;
                predecessors[v] = u;
                Node now = node_set[v];
                if (current.distance + haversineDistance(ne, now) - distances[ed] < 1.0){
                    queue.emplace(v, new_distance);
                    // queue.push(QueueElement(v, new_distance));
                }
            }
        }
    }
    ans_dis = distances[ed];
}


#endif