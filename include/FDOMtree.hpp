#pragma once

#include <unordered_map>
#include <unordered_set>

#include "CFGraph.hpp"
#include "DOMtree.hpp"


namespace FDOMtree {


struct fdtree {

    std::unordered_map<uint32_t, std::unordered_set<uint32_t>> DF;
    std::unordered_set<uint32_t> nodes;

    void build(const CFGraph::cfgraph& cfg, const DOMtree::dtree& dom);

};


} // FDOMtree