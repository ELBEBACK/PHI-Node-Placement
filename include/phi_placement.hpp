#pragma once

#include <unordered_set>
#include <cstdint>

#include "CFGraph.hpp"
#include "FDOMtree.hpp"


namespace phi_placement {


std::unordered_set<uint32_t> place(const CFGraph::cfgraph&              cfg,
                                    const FDOMtree::fdtree&             df,
                                    const std::unordered_set<uint32_t>& defs);


} // phi_placement