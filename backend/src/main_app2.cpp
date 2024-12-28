#include "buildgraph.hpp"
#include "dijkstra.hpp"
#include "loadosm.hpp"
#include "pugixml.hpp"
#include "findnode.hpp"
#include "routeplanner.hpp"

#include <crow.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <thread>

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

    outFile.close();

    routeplanner();

    return 0;
}