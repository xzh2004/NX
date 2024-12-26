
#include <crow.h>
#include "findnode.hpp"
#include "dijkstra.hpp"

void routeplanner() {
    crow::SimpleApp app;

    // WebSocket route for path calculation
    CROW_WEBSOCKET_ROUTE(app, "/ws")
    .onopen([](crow::websocket::connection& conn) {
        std::cout << "WebSocket connection opened" << std::endl;
    }).onclose([&](const crow::websocket::connection& conn, const std::string& reason, uint16_t code) {
        std::cout << "WebSocket connection closed: " << reason << std::endl;
    }).onmessage([&](crow::websocket::connection& conn, const std::string& message, bool is_binary) {
        std::cout << "Received message: " << message << std::endl;

        // Parse message for start and end points
        auto msg = crow::json::load(message);
        double start_lat = msg["start_lat"].d();;
        double start_lon = msg["start_lon"].d();
        double end_lat = msg["end_lat"].d();
        double end_lon = msg["end_lon"].d();

        std :: cout << std::fixed << std::setprecision(10) << start_lat << ' ' << start_lon << ' ' << end_lat << ' ' << end_lon << std :: endl;

        ll start_node = find_nearest_node(start_lat, start_lon);
        ll end_node = find_nearest_node(end_lat, end_lon);

        if (start_node == -1 || end_node == -1) {
            conn.send_text("无法找到起点或终点的最近节点.");
            return;
        }

        std :: cout << "起点节点 ID: " << start_node << ' ' << nodes.at(start_node).name << std :: endl;
        std :: cout << ", 终点节点 ID: " << end_node << ' ' << nodes.at(end_node).name << "\n";
        auto bp3 = std::chrono::high_resolution_clock::now();
        dijkstra(nodes[start_node].num, nodes[end_node].num);
        auto bp4 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> d4 = bp4 - bp3;
        std::cout << "程序运行时间: " << d4.count() << " 秒" << std::endl;
        if (!reconstruct_path(nodes[start_node].num, nodes[end_node].num)) {
            conn.send_text("无法找到从起点到终点的路径.");
            return;
        }
        
        std::string response = "[";
        for (int i = path_len; i; --i) {
            ll node_id = id_table[path_id[i]];
            const Node& n = nodes.at(node_id);
    
    // 使用 std::ostringstream 设置精度
            std::ostringstream lat_lon_stream;
            lat_lon_stream << std::fixed << std::setprecision(10);
            lat_lon_stream << "[" << n.lat << ", " << n.lon << "]";
    
            response += lat_lon_stream.str();
            if (i > 1) response += ", ";
        }
        std :: ostringstream time_stream;
        time_stream << "[\"" << std::fixed << std::setprecision(4) << d4.count() << "s\"]";
        response += ", " + time_stream.str() + "]";
        std::cout << "Response: " << response << std::endl;

        conn.send_text(response);
    });

    app.port(18080).multithreaded().run();
}
