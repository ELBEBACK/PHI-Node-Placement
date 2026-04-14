#pragma once

#include <string>

#include "CFGraph.hpp"
#include "DOMtree.hpp"
#include "FDOMtree.hpp"
#include "Looptree.hpp"


namespace viz {


void dump_graphviz(const std::string& dst_path, const CFGraph::cfgraph&   target);
void dump_graphviz(const std::string& dst_path, const DOMtree::dtree&     target);
void dump_graphviz(const std::string& dst_path, const FDOMtree::fdtree&   target);
void dump_graphviz(const std::string& dst_path, const Looptree::ltree&    target);


} // viz