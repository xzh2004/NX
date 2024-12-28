#ifndef ASTAR_HPP
#define ASTAR_HPP

#include "pugixml.hpp"
#include "loadosm.hpp"
#include "buildgraph.hpp"
#include "geodistance.hpp"
#include "FibonacciHeap.hpp"

#include <utility>
#include <queue>
#include <vector>
#include <cmath>
#include <unordered_set>

bool closed_set[NODE_SIZE];
float g_score[NODE_SIZE], f_score[NODE_SIZE];

struct Queue {
    int node;
    float f_score;
    inline bool operator < (const Queue& other) const {
        return f_score > other.f_score;
    }
    Queue (int x, float y):node(x), f_score(y){}
};

struct FibAStarNode {
    int node_num;
    float f_score;

    inline bool operator<(const FibAStarNode& other) const {
        return f_score < other.f_score; // 反向用于最小堆
    }
};

void AStar(int start_num, int goal_num, float heuristic, int display_search) {
    // 定义节点数
    // 定义g_score和f_score数组，初始化为无穷大
    for (int i = 1; i <= N_SIZE; i++){
        f_score[i] = g_score[i] = std::numeric_limits<float>::infinity();
        closed_set[i] = false;
    }
    // 优先队列中存储 (f_score, node_num)
    std :: priority_queue <Queue> open_set;

    const Node goal_node = node_set[goal_num];
    const Node start_node_actual = node_set[start_num];

    // 初始化起点
    g_score[start_num] = 0.0;
    
    float h_start = haversineDistance(start_node_actual, goal_node);
    search_points[search_cnt++] = start_num;
    f_score[start_num] = h_start;
    // open_set.push(Queue(start_num, h_start));
    open_set.emplace(start_num, h_start);
    // 关闭集合
    while (!open_set.empty()) {
        auto current = open_set.top();
        open_set.pop();
        float current_f = current.f_score;
        int u = current.node;

        if (closed_set[u]) continue;
        if (!display_search) search_points[search_cnt++] = u;
        closed_set[u] = true;
        // 遍历所有邻居
        for (int i = graph.head[u]; i; i = graph[i].nxt) {
            int v = graph[i].to;
            if (closed_set[v]) continue;
            
            float now_g = g_score[u] + graph[i].weight;
            
            if (g_score[v] - now_g > 1.0) {
                predecessors[v] = u;
                g_score[v] = now_g;
                float h = haversineDistance(node_set[v], goal_node);
                float now_f = now_g + h * heuristic;
                if (f_score[v] - now_f > 1.0){
                    f_score[v] = now_f;
                    // open_set.push(Queue(v, now_f));
                    open_set.emplace(v, now_f);
                }
            }
        }
    }
    ans_dis = g_score[goal_num];
}

void AStar_Fib(int start_num, int goal_num) {
    // 定义节点数
    // 定义g_score和f_score数组，初始化为无穷大
    for (int i = 1; i <= N_SIZE; i++){
        g_score[i] = INFINITY;
        closed_set[i] = false;
    }
    // 创建斐波那契堆实例
    FibonacciHeap<FibAStarNode, std::less<FibAStarNode>> fib_heap;

    // 数组用于存储每个节点在斐波那契堆中的指针
    FibNode<FibAStarNode, std::less<FibAStarNode>>* fib_nodes[NODE_SIZE] = {nullptr};

    const Node goal_node = node_set[goal_num];
    const Node start_node_actual = node_set[start_num];

    // 初始化起点
    g_score[start_num] = 0.0;
    float h_start = haversineDistance(start_node_actual, goal_node);
    FibAStarNode start_fib_node = {start_num, h_start};
    FibNode<FibAStarNode, std::less<FibAStarNode>>* start_heap_node = new FibNode<FibAStarNode, std::less<FibAStarNode>>(start_fib_node);
    fib_heap.insert(start_heap_node);
    fib_nodes[start_num] = start_heap_node;
    // 关闭集合
    while (!fib_heap.empty()) {
        FibNode<FibAStarNode, std::less<FibAStarNode>>* current_heap_node = fib_heap.extract_min();
        FibAStarNode current = current_heap_node->key;
        float current_f = current.f_score;
        int u = current.node_num;
        delete current_heap_node; 
        if (closed_set[u]) continue;
        closed_set[u] = true;
        // 遍历所有邻居
        for (int i = graph.head[u]; i; i = graph[i].nxt) {
            Edge edge = graph[i];
            int v = edge.to;
            if (closed_set[v]) continue;
            
            float now_g = g_score[u] + edge.weight;
            
            if (now_g < g_score[v]) {
                predecessors[v] = u;
                g_score[v] = now_g;
                float h = haversineDistance(node_set[v], goal_node);
                float f = now_g + h;

                FibAStarNode neighbor_fib_node = {v, f};
                if (fib_nodes[v] == nullptr) {
                    // 如果节点不在堆中，插入新节点
                    FibNode<FibAStarNode, std::less<FibAStarNode>>* new_heap_node = new FibNode<FibAStarNode, std::less<FibAStarNode>>(neighbor_fib_node);
                    fib_heap.insert(new_heap_node);
                    fib_nodes[v] = new_heap_node;
                } else {
                    // 如果节点已经在堆中，减少其键值
                    fib_heap.decrease_key(fib_nodes[v], neighbor_fib_node);
                }
            }
        }
    }
    ans_dis = g_score[goal_num];
}

struct NodeState {
    int nodeId;
    float gScore;
    float fScore;
    NodeState(int id, float g, float f) : nodeId(id), gScore(g), fScore(f) {}
    bool operator>(const NodeState& other) const {
        return fScore > other.fScore;
    }
};

bool forwardVisited[NODE_SIZE];
bool reverseVisited[NODE_SIZE];

// 双向 A* 算法
int bidirectionalAStar(int start_num, int end_num, float heuristic, int display_search) {
    std::priority_queue<NodeState, std::vector<NodeState>, std::greater<NodeState>> forwardQueue;
    std::priority_queue<NodeState, std::vector<NodeState>, std::greater<NodeState>> reverseQueue;
    std::unordered_map<int, float> forwardGScore;
    std::unordered_map<int, float> reverseGScore;
 
    memset(forwardVisited, 0, sizeof(forwardVisited));
    memset(reverseVisited, 0, sizeof(reverseVisited));

    forwardQueue.emplace(start_num, 0.0f, haversineDistance(node_set[start_num], node_set[end_num]));
    reverseQueue.emplace(end_num, 0.0f, haversineDistance(node_set[end_num], node_set[start_num]));
    forwardGScore[start_num] = 0.0f;
    reverseGScore[end_num] = 0.0f;

    while (!forwardQueue.empty() &&!reverseQueue.empty()) {
        // 正向扩展
        NodeState currForward = forwardQueue.top();
        forwardQueue.pop();
        if (display_search) search_points[search_cnt++] = currForward.nodeId;
        if (reverseVisited[currForward.nodeId]) {
            // 找到路径
            path_len = 0;
            int node = currForward.nodeId;
            while (node!= start_num) {
                path_id[++path_len] = node;
                node = predecessors[node];
            }
            std::reverse(path_id + 1, path_id + 1 + path_len);
            int nodeRev = currForward.nodeId;
            while (nodeRev!= end_num) {
                path_id[++path_len] = nodeRev;
                nodeRev = rev_predecessors[nodeRev];
            }
            ans_dis = forwardGScore[currForward.nodeId] + reverseGScore[currForward.nodeId];
            return 1;
        }
        forwardVisited[currForward.nodeId] = 1;
        for (int i = graph.head[currForward.nodeId]; i; i = graph.e[i].nxt) {
            int neighbor = graph.e[i].to;
            float tentativeGScore = currForward.gScore + graph.e[i].weight;
            if (forwardGScore.count(neighbor) == 0 || tentativeGScore < forwardGScore[neighbor]) {
                forwardGScore[neighbor] = tentativeGScore;
                float fScore = tentativeGScore + haversineDistance(node_set[neighbor], node_set[end_num]) * heuristic;
                forwardQueue.emplace(neighbor, tentativeGScore, fScore);
                predecessors[neighbor] = currForward.nodeId;
            }
        }

        // 反向扩展
        NodeState currReverse = reverseQueue.top();
        reverseQueue.pop();
        if (display_search) search_points[search_cnt++] = currReverse.nodeId;
        if (forwardVisited[currReverse.nodeId]) {
            // 找到路径
            path_len = 0;
            int node = currReverse.nodeId;
            while (node!= end_num) {
                path_id[++path_len] = node;
                node = rev_predecessors[node];
            }
            std::reverse(path_id + 1, path_id + 1 + path_len);
            int nodeFor = currReverse.nodeId;
            while (nodeFor!= start_num) {
                path_id[++path_len] = nodeFor;
                nodeFor = predecessors[nodeFor];
            }
            ans_dis = forwardGScore[currReverse.nodeId] + reverseGScore[currReverse.nodeId];
            return 2;
        }
        reverseVisited[currReverse.nodeId] = 1;
        for (int i = rev_graph.head[currReverse.nodeId]; i; i = rev_graph.e[i].nxt) {
            int neighbor = rev_graph.e[i].to;
            float tentativeGScore = currReverse.gScore + rev_graph.e[i].weight;
            if (reverseGScore.count(neighbor) == 0 || tentativeGScore < reverseGScore[neighbor]) {
                reverseGScore[neighbor] = tentativeGScore;
                float fScore = tentativeGScore + haversineDistance(node_set[neighbor], node_set[start_num]) * heuristic;
                reverseQueue.emplace(neighbor, tentativeGScore, fScore);
                rev_predecessors[neighbor] = currReverse.nodeId;
            }
        }
    }
    return -1;
}

#endif