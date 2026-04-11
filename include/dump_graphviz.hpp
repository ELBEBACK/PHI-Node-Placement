#pragma once

#include "CFGraph.hpp"
#include "DOMtree.hpp"
#include "FDOMtree.hpp"

namespace viz {

void dump_graphviz(const std::string& dst_path, CFGraph::cfgraph&   target);
void dump_graphviz(const std::string& dst_path, DOMtree::dtree&     target);
void dump_graphviz(const std::string& dst_path, FDOMtree::fdtree&   target);

} // viz