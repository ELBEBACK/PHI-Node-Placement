#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "CFGraph.hpp"


namespace DOMtree {


struct dtree : CFGraph::cfgraph { // preds are unnecessary, no inheritage needed
    
    std::unordered_map<uint32_t, uint32_t> idom;
    
    void build(const CFGraph::cfgraph& cfg);
 
};

    
} // DOMtree