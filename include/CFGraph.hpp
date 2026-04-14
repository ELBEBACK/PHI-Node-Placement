#pragma once

#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <utility>

#include <cstdint>


namespace CFGraph {

    
constexpr uint32_t START = 0x000;
constexpr uint32_t STOP  = 0xDED;

struct cfgraph { 

    std::unordered_map<uint32_t, std::vector<uint32_t>> successors;
    std::unordered_map<uint32_t, std::vector<uint32_t>> predecessors;
    std::unordered_set<uint32_t>                        nodes;


    void edge_add(uint32_t src, uint32_t dst);
    void edge_add(std::pair<uint32_t, uint32_t>& edge);
    

};


} // CFGraph