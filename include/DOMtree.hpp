#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "CFGraph.hpp"


namespace DOMtree {


struct dtree {
    
    std::unordered_map<uint32_t, uint32_t>              idom;
    std::unordered_map<uint32_t, std::vector<uint32_t>> children;
    std::unordered_set<uint32_t>                        nodes;
    
    void build(const CFGraph::cfgraph& cfg);
 
};

    
} // DOMtree