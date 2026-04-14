#pragma once

#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "CFGraph.hpp"


namespace Looptree {


enum class LoopType { ROOT, REDUCIBLE, IRREDUCIBLE };


struct loop {
    uint32_t                        id;
    uint32_t                        header;
    LoopType                        type;  
    std::unordered_set<uint32_t>    own_blocks;
    uint32_t                        parent;
    std::vector<uint32_t>           children;
};


struct ltree {

    std::unordered_map<uint32_t, loop>     loops;
    std::unordered_map<uint32_t, uint32_t> block_loop;

    void build(const CFGraph::cfgraph& cfg);
    
};


} // Looptree