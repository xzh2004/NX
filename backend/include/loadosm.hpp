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

const int NODE_SIZE = 1e6; // 开数组用的
int N_SIZE;  // 实际的

long long id_table[NODE_SIZE]; // 图中节点编号到实际节点编号映射

int predecessors[NODE_SIZE];
int rev_predecessors[NODE_SIZE];
int path_id[NODE_SIZE], path_len;

float ans_dis;

std::unordered_map<long long, int> nodes; // 节点编号到节点的映射，节点包含经纬度以及图中的编号

// 定义节点结构
struct Node {
    long long id;
    float lat;
    float lon;
    int num;
    std::string name;
    Node (float x, float y): lat(x), lon(y), num(0) {}
    Node (): id(0) {}
} node_set[NODE_SIZE];

// 定义路径结构
struct Way {
    long long id;
    std::vector<long long> node_refs;
    bool oneway;
};

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
        node_set[n.num] = n;
        nodes[n.id] = n.num;
    }
    // for (int i = 1; i <= N_SIZE; i++)
    //     osm_file << node_set[i].name <<  ' ' << node_set[i].lat << ' ' <<  node_set[i].lon << "\n";
    return true;
}

#endif