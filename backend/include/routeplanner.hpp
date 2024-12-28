
#include <crow.h>
#include "findnode.hpp"
#include "dijkstra.hpp"
#include "AStar.hpp"
#include "reconstruct.hpp"
#include "jsonbuild.hpp"

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
        int mode = msg["mode"].i();
        float heuristic = msg["heuristic"].d();
        int display_search = msg["display_search"].i();

        std :: cout << std::fixed << std::setprecision(10) << start_lat << ' ' << start_lon << ' ' << end_lat << ' ' << end_lon << std :: endl;
        std :: cout << "mode: " << mode << "\n";
        
        auto bp1 = std::chrono::high_resolution_clock::now();

        int start_node = find_nearest_node(start_lat, start_lon); // needs to be updated
        int end_node = find_nearest_node(end_lat, end_lon);

        auto bp2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> d2 = bp2 - bp1;
        std::cout << "程序运行时间: " << d2.count() << " 秒" << std::endl;

        if (start_node == -1 || end_node == -1) {
            conn.send_text("无法找到起点或终点的最近节点.");
            return;
        }

        std :: cout << "起点节点 ID: " << start_node << ' ' << node_set[start_node].lat << ' ' << node_set[start_node].lon << std :: endl;
        std :: cout << ", 终点节点 ID: " << end_node << ' ' << node_set[end_node].lat << ' ' << node_set[end_node].lon << std :: endl;
        auto bp3 = std::chrono::high_resolution_clock::now();
        search_cnt = 0;
        if (mode == 1) dijkstra(start_node, end_node, display_search);
        if (mode == 2){
            std :: cout << "AStar" << std :: endl;
            AStar(start_node, end_node, heuristic, display_search);
        }
        if (mode == 3) AStar_Fib(start_node, end_node);
        int opt;
        if (mode == 4){
            opt = bidirectionalAStar(start_node, end_node, heuristic, display_search);
        }
        auto bp4 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> d4 = bp4 - bp3;
        std::cout << "程序运行时间: " << d4.count() << " 秒" << std::endl;

        if (mode != 4){
            bool is_fail = reconstruct_path(start_node, end_node);
            if (!is_fail) {
                conn.send_text("无法找到从起点到终点的路径.");
                return;
            }
        }
        else if (opt == -1){
            conn.send_text("无法找到从起点到终点的路径.");
                return;
        }
        conn.send_text(construct_json_response(d4.count() * 1000.0, ans_dis / 1000.0));
    });

    app.port(18080).multithreaded().run();
}
