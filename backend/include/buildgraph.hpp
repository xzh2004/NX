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
    inline void push_back(int from, int to, float dis){
        e[++cnt].nxt = head[from];
        e[cnt].to = to;
        e[cnt].weight = dis;
        head[from] = cnt;
    }
    int size(){ return cnt;}
} graph, rev_graph;

int search_points[NODE_SIZE];
int search_cnt;

// 搜索节点
void search_nodes() {
    search_cnt = 0;
    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        search_points[search_cnt++] = it->first;
    }
}

// 构建图的邻接表
void build_graph() {
    graph.clear();
    for (int j = 1; j <= ways.size(); ++j) {
        Way way = ways[j];
        for (size_t i = 0; i < way.node_refs.size() - 1; ++i) {
            long long from = way.node_refs[i];
            long long to = way.node_refs[i + 1];
            if (nodes.find(from) != nodes.end() && nodes.find(to) != nodes.end()) {
                int x = nodes[from], y = nodes[to];
                float distance = haversineDistance(node_set[x], node_set[y]);
                graph.push_back(x, y, distance);
                rev_graph.push_back(y, x, distance);
                if (!way.oneway){
                    graph.push_back(y, x, distance); // 如果是双向路径
                    rev_graph.push_back(x, y, distance);
                }
            }
        }
    }

}

#endif // _BUILDGRAPH_HPP