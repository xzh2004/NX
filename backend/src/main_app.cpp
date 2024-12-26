#include "buildgraph.hpp"
#include "dijkstra.hpp"
#include "loadosm.hpp"
#include "pugixml.hpp"
#include <crow.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <thread>  // 仅用于演示耗时操作

// 为了简化全局变量的使用，假设 nodes, ways, graph 等都是全局变量或在相应的头文件中声明

int main() {
    // 初始化图结构
    auto start_init = std::chrono::high_resolution_clock::now();

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
    std::chrono::duration<double> d1 = bp1 - start_init;
    outFile << "解析 OSM 文件时间: " << d1.count() << " 秒" << std::endl;

    build_graph();
    outFile << "图构建完成: " << graph.size() << " 个节点.\n";
    auto bp2 = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> d2 = bp2 - bp1;
    outFile << "图构建时间: " << d2.count() << " 秒" << std::endl;

    // 初始化完成，记录总时间
    auto end_init = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> total_init_time = end_init - start_init;
    outFile << "初始化总时间: " << total_init_time.count() << " 秒" << std::endl;

    outFile.close();

    // 设置 Crow 服务器
    crow::SimpleApp app;

    // 设置静态文件路由（假设前端文件在 ./public 目录下）
    CROW_ROUTE(app, "/ws")
    .websocket()
    .onopen([](crow::websocket::connection& conn) {
        std::cout << "WebSocket 连接已打开" << std::endl;
    })
    .onclose([](crow::websocket::connection& conn, const std::string& reason) {
        std::cout << "WebSocket 连接已关闭: " << reason << std::endl;
    })
    .onmessage([&](crow::websocket::connection& conn, const std::string& data, bool is_binary) {
        // 假设前端发送的是 JSON 格式的字符串，包含起点和终点的经纬度
        // 例如: {"start_lat":31.300555, "start_lon":121.498994, "end_lat":31.02841, "end_lon":121.43128}
        crow::json::rvalue request;
        try {
            request = crow::json::load(data);
            if (!request) {
                throw std::runtime_error("无效的 JSON 格式");
            }
        }
        catch (const std::exception& e) {
            std::cerr << "解析 JSON 失败: " << e.what() << std::endl;
            crow::json::wvalue error_response;
            error_response["error"] = "无效的 JSON 格式";
            conn.send_text(crow::json::dump(error_response));
            return;
        }

        // 提取经纬度
        float start_lat, start_lon, end_lat, end_lon;
        try {
            start_lat = request["start_lat"].d();
            start_lon = request["start_lon"].d();
            end_lat = request["end_lat"].d();
            end_lon = request["end_lon"].d();
        }
        catch (const std::exception& e) {
            std::cerr << "提取经纬度失败: " << e.what() << std::endl;
            crow::json::wvalue error_response;
            error_response["error"] = "缺少必要的经纬度参数";
            conn.send_text(crow::json::dump(error_response));
            return;
        }

        // 查找最近节点
        ll start_node = find_nearest_node(start_lat, start_lon);
        ll end_node = find_nearest_node(end_lat, end_lon);

        if (start_node == -1 || end_node == -1) {
            std::cerr << "无法找到起点或终点的最近节点." << std::endl;
            crow::json::wvalue error_response;
            error_response["error"] = "无法找到起点或终点的最近节点";
            conn.send_text(crow::json::dump(error_response));
            return;
        }

        // 运行 Dijkstra 算法
        // 假设 nodes[start_node].num 和 nodes[end_node].num 是图中的节点编号
        ll graph_start = nodes[start_node].num;
        ll graph_end = nodes[end_node].num;

        // 使用内存中的日志记录（可选），这里为了简化，直接使用临时变量
        std::ofstream temp_out("temp_report.out");
        if (!temp_out) {
            std::cerr << "无法打开临时输出文件" << std::endl;
            crow::json::wvalue error_response;
            error_response["error"] = "服务器内部错误";
            conn.send_text(crow::json::dump(error_response));
            return;
        }

        // 运行 Dijkstra 算法
        dijkstra(graph_start, temp_out, graph_end);

        // 重构路径
        bool path_found = reconstruct_path(graph_start, graph_end);
        temp_out.close();

        if (!path_found) {
            std::cerr << "无法找到从起点到终点的路径." << std::endl;
            crow::json::wvalue error_response;
            error_response["error"] = "无法找到从起点到终点的路径";
            conn.send_text(crow::json::dump(error_response));
            return;
        }
                // 构建 JSON 响应
        crow::json::wvalue response;
        response["path"] = crow::json::wvalue::list();
        // 收集路径节点的经纬度
        for (int i = path_len; i; --i) {
            ll node_id = id_table[path_id[i]];
            const Node& n = nodes.at(node_id);
            crow::json::wvalue point;
            point["lat"] = n.lat;
            point["lon"] = n.lon;
            response["path"].push_back(point);
        }

        // 发送响应给前端
        conn.send_text(crow::json::dump(response));
    });

    // 配置服务器监听的端口
    app.port(18080).multithreaded().run();

    return 0;
}
