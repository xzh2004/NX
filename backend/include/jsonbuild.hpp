#include "loadosm.hpp"
#include "buildgraph.hpp"

#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

std::string construct_json_response(
    double time_ms,
    double distance_km
) {
    std::ostringstream response_stream;
    response_stream << "{";

    // 添加 "path" 数组
    response_stream << "\"path\": [";
    for (int i = path_len; i; --i) {
        const Node& n = node_set[path_id[i]]; // 假设 path_id 是 0-based
        response_stream << "[" << std::fixed << std::setprecision(10) << n.lat << ", " << n.lon << "]";
        if (i > 1) {
            response_stream << ", ";
        }
    }
    response_stream << "],";

    // 添加 "time" 字符串
    response_stream << "\"time\": \"" << std::fixed << std::setprecision(4) << time_ms << "ms\",";

    // 添加 "distance" 字符串
    response_stream << "\"distance\": \"" << std::fixed << std::setprecision(4) << distance_km << "km\",";

    // 添加 "searchPoints" 数组（可选，用于绘制多边形）
    response_stream << "\"searchPoints\": [";
    for (size_t i = 0; i < search_cnt; ++i) {
        response_stream << "[" << std::fixed << std::setprecision(10) << node_set[search_points[i]].lat << ", " << node_set[search_points[i]].lon << "]";
        if (i < search_cnt - 1) {
            response_stream << ", ";
        }
    }
    response_stream << "]";

    response_stream << "}";

    return response_stream.str();
}