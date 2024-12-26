#include "buildgraph.hpp"
#include "dijkstra.hpp"
#include "loadosm.hpp"
#include "pugixml.hpp"
#include <crow.h>
#include <iostream>
#pragma GCC optimize(2) 

#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip> 

#include <chrono>
#include <thread>  // 仅用于演示耗时操作

int main() {
    auto start = std::chrono::high_resolution_clock::now();

    std::ofstream outFile("report3.out");
    if (!outFile) {
        std::cerr << "无法打开输出文件" << std::endl;
        return -1;
    }
    outFile << "开始解析 OSM 文件.\n";
    
    if (!parse_osm(outFile)) {
        outFile << "解析失败.\n";
        return -1;
    }

    outFile << "解析完成: " << nodes.size() << " 个节点, " << ways.size() << " 条路径.\n";

    auto bp1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> d1 = bp1 - start;
    outFile << "程序运行时间: " << d1.count() << " 秒" << std::endl;

    build_graph();
    outFile << "图构建完成: " << graph.size() << " 个节点.\n";
    auto bp2 = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> d2 = bp2 - bp1;
    outFile << "程序运行时间: " << d2.count() << " 秒" << std::endl;

    // 用户指定的起点和终点坐标
    float start_lat = 31.300555; // 示例坐标
    float start_lon = 121.498994;
    float end_lat = 31.02841;
    float end_lon = 121.43128;
    // float end_lat = 31.28473925;
    // float end_lon = 121.49694908588742;

    ll start_node = find_nearest_node(start_lat, start_lon);
    ll end_node = find_nearest_node(end_lat, end_lon);

    // outFile << haversineDistance(nodes[start_node], nodes[end_node]) << "\n";

    if (start_node == -1 || end_node == -1) {
        std::cerr << "无法找到起点或终点的最近节点.\n";
        return -1;
    }   

    outFile << "起点节点 ID: " << start_node << ' ' << nodes.at(start_node).name << std :: endl;
    outFile << ", 终点节点 ID: " << end_node << ' ' << nodes.at(end_node).name << "\n";
    
    const Node& ns = nodes.at(start_node);

    start_node = nodes[start_node].num;
    end_node = nodes[end_node].num;

    auto bp3 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> d3 = bp3 - bp2;
    outFile << "程序运行时间: " << d3.count() << " 秒" << std::endl;

    dijkstra(start_node, outFile, end_node);

    auto bp4 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> d4 = bp4 - bp3;
    outFile << "程序运行时间: " << d4.count() << " 秒" << std::endl;

    if (!reconstruct_path(start_node, end_node)) {
        outFile << "无法找到从起点到终点的路径.\n";
        return -1;
    }
    
    outFile << "路径长度: " << path_len << " 个节点.\n";
    for (int i = path_len; i; --i){
        ll node_id = id_table[path_id[i]];
        const Node& n = nodes.at(node_id);
        // outFile << "节点 " << node_id << ":" << n.name <<"(" << n.lat << ", " << n.lon << ")\n";
        outFile << std::fixed << std::setprecision(10) 
            << "[" << n.lat << ", " << n.lon << "], ";
    }

    outFile << "\n";

    
    for (int i = path_len; i; --i){
        ll node_id = id_table[path_id[i]];
        const Node& n = nodes.at(node_id);
        // outFile << "节点 " << node_id << ":" << n.name <<"(" << n.lat << ", " << n.lon << ")\n";
        outFile << n.name << " " << node_id << "m\n";
    }

    outFile.close();
    return 0;
}