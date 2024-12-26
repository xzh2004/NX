#ifndef LOADOSM_HPP
#define LOADOSM_HPP

#include "pugixml.hpp"
#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <set>


typedef long long ll;

int N_SIZE;
const int NODE_SIZE = 1e6;

long long id_table[NODE_SIZE];

// 定义节点结构
struct Node {
    long long id;
    float lat;
    float lon;
    int num;
    std::string name;
    Node (float x, float y): lat(x), lon(y), num(0) {}
    Node (): id(0) {}
};

// 定义路径结构
struct Way {
    long long id;
    std::vector<long long> node_refs;
    bool oneway;
};

std::unordered_map<long long, Node> nodes;

const int WAY_SIZE = 2e5;

struct WAY{
    int cnt;
    Way val[WAY_SIZE];
    
    void clear(){ cnt = 0;}
    inline void push_back(const Way &x){
        val[++cnt] = x;
    }
    int size(){ return cnt; }
    inline Way& operator[] (int i){return val[i];}
} ways;

// 解析 OSM 文件
bool parse_osm(std::ofstream& osm_file) {
    std:: set <long long> highway_set;
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("/mnt/d/Library/NX/shanghai-latest.osm");

    if (!result) {
        std::cerr << "无法加载 OSM 文件: " << result.description() << "\n";
        osm_file << "无法加载 OSM 文件: " << result.description() << "\n";
        return false;
    }
    osm_file << "成功加载 OSM 文件\n";
    // 解析节点
    // 解析路径
    
    ways.clear();
    for (pugi::xml_node way = doc.child("osm").child("way"); way; way = way.next_sibling("way")) {
        Way w;
        w.id = way.attribute("id").as_llong();
        w.oneway = false;
        // 可根据需要过滤特定类型的路径，例如只处理道路（highway 标签）
        bool is_highway = false;
        for (pugi::xml_node tag = way.child("tag"); tag; tag = tag.next_sibling("tag")) {
            if (std::string(tag.attribute("k").value()) == "highway") {
                is_highway = true;
            }
            if (std::string(tag.attribute("k").value()) == "oneway")
                w.oneway = true;
        }
        if (is_highway) {
            for (pugi::xml_node nd = way.child("nd"); nd; nd = nd.next_sibling("nd")) {
                long long node_id = nd.attribute("ref").as_llong();
                w.node_refs.push_back(node_id);
                highway_set.insert(node_id);
            }
            ways.push_back(w);
        }

    }
    for (pugi::xml_node node = doc.child("osm").child("node"); node; node = node.next_sibling("node")) {
        Node n;
        n.id = node.attribute("id").as_llong();
        if (!highway_set.count(n.id)) continue;
        n.lat = node.attribute("lat").as_float();
        n.lon = node.attribute("lon").as_float();
         for (pugi::xml_node tag = node.child("tag"); tag; tag = tag.next_sibling("tag")) {
            if (std::string(tag.attribute("k").value()) == "name") {
                n.name = tag.attribute("v").value();
                break;
            }
         }
        // n.name = node.attribute("name").as_string();
        n.num = ++N_SIZE;
        id_table[N_SIZE] = n.id;
        
        nodes[n.id] = n;
    }
    
    return true;
}

#endif