#ifndef BUILDGRAPH_HPP
#define BUILDGRAPH_HPP

#include "loadosm.hpp"
#include "geodistance.hpp"

#include <unordered_map>
#include <vector>
#include <cmath>

// 定义邻接表结构
struct Edge {
    long long to;
    int nxt;
    float weight; // 可以是距离或其他权重
};

const int E_SIZE = 2e6;

struct EDGE{
    Edge e[E_SIZE];
    int cnt;
    int head[NODE_SIZE];
    void clear(){ cnt = 0; }
    inline Edge& operator[] (int i){return e[i];}
    inline void push_back(ll from, ll to, float dis){
        e[++cnt].nxt = head[nodes[from].num];
        e[cnt].to = nodes[to].num;
        e[cnt].weight = dis;
        head[nodes[from].num] = cnt;
    }
    int size(){ return cnt;}
} graph;

// 构建图的邻接表
void build_graph() {
    graph.clear();
    for (int j = 1; j <= ways.size(); ++j) {
        Way way = ways[j];
        for (size_t i = 0; i < way.node_refs.size() - 1; ++i) {
            long long from = way.node_refs[i];
            long long to = way.node_refs[i + 1];
            if (nodes.find(from) != nodes.end() && nodes.find(to) != nodes.end()) {
                float distance = haversineDistance(nodes.at(from), nodes.at(to));
                graph.push_back(from, to, distance);
                if (!way.oneway) graph.push_back(to, from, distance); // 如果是双向路径
            }
        }
    }

}

#endif // _BUILDGRAPH_HPP