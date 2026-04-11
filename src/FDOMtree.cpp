#include "FDOMtree.hpp"


namespace FDOMtree {


void fdtree::build(const CFGraph::cfgraph& cfg, const DOMtree::dtree& dom) {

    nodes = cfg.nodes;
    
    for (uint32_t block : cfg.nodes) {

        auto pred_it = cfg.predecessors.find(block);
        if (pred_it == cfg.predecessors.end())       continue;
        if (pred_it->second.size() < 2)              continue; 

        for (uint32_t block_pred : pred_it->second) {
            uint32_t runner = block_pred;

            while (dom.idom.count(block) && runner != dom.idom.at(block)) {
                DF[runner].insert(block);
                if (dom.idom.count(runner) == 0) break;
                runner = dom.idom.at(runner);
            }
        }
    }
}


} // FDOMtree