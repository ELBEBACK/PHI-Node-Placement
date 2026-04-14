#include "CFGraph.hpp"


namespace CFGraph {


void cfgraph::edge_add(uint32_t src, uint32_t dst) {
    successors[src].push_back(dst);
    predecessors[dst].push_back(src);
    nodes.insert(src);
    nodes.insert(dst);
}    


void cfgraph::edge_add(std::pair<uint32_t, uint32_t>& edge) {
    edge_add(edge.first, edge.second);
}


} // CFGraph