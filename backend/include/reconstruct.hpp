#include "loadosm.hpp"
#include "buildgraph.hpp"

bool reconstruct_path(int start, int end){
    path_len = 0;
    int current = end;
    while (current != start) {
        path_id[++path_len] = current;
        current = predecessors[current];
        if (current == -1) return 0;
    }
    // std :: cout << "finish" << std :: endl;
    path_id[++path_len] = current;
    return 1;
}