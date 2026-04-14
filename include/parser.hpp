#pragma once

#include <istream>
#include <unordered_set>
#include <cstdint>

#include "CFGraph.hpp"


namespace parser {


struct parsed_input {
    CFGraph::cfgraph             cfg;
    std::unordered_set<uint32_t> defs;
};


parsed_input read(std::istream& in, uint32_t edge_count, uint32_t def_count);


} // parser