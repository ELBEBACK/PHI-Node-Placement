#include "FDOMtree.hpp"


namespace FDOMtree {


void fdtree::build(const CFGraph::cfgraph& cfg, const DOMtree::dtree& dom) {

    nodes = cfg.nodes;
    
    for (uint32_t block : cfg.nodes) {

        auto predecessors_it = cfg.predecessors.find(block);
        if (predecessors_it == cfg.predecessors.end())       continue;
        if (predecessors_it->second.size() < 2)              continue; 

        for (uint32_t block_pred : predecessors_it->second) {
            uint32_t cursor = block_pred;

            while (dom.idom.count(block) && cursor != dom.idom.at(block)) {
                DF[cursor].insert(block);
                if (dom.idom.count(cursor) == 0) break;
                cursor = dom.idom.at(cursor);
            }
        }
    }
}


} // FDOMtree