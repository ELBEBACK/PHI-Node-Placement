#include "DOMtree.hpp"


namespace DOMtree {


void dtree::build(const CFGraph::cfgraph& cfg) {

    const uint32_t start = CFGraph::START;

    nodes.insert(start);

    std::unordered_map<uint32_t, std::unordered_set<uint32_t>> DOM;

    DOM[start] = {start};
    for (uint32_t n : cfg.nodes)
        if (n != start)
            DOM[n] = cfg.nodes;

    bool changed = true;
    while (changed) {
        changed = false;

        for (uint32_t it : cfg.nodes) {
            if (it == start) continue;

            std::unordered_set<uint32_t> new_dom;
            bool first = true;

            auto pred_it = cfg.predecessors.find(it);
            if (pred_it == cfg.predecessors.end()) continue;

            for (uint32_t p : pred_it->second) {
                
                if (first) {
                    
                    new_dom = DOM[p];
                    first = false;
                } else {
                    
                    for (auto it = new_dom.begin(); it != new_dom.end(); ) {
                        if (DOM[p].count(*it) == 0)
                            it = new_dom.erase(it);
                        else
                            ++it;
                    }
                }
            }

            new_dom.insert(it);

            if (new_dom != DOM[it]) {
                DOM[it] = std::move(new_dom);
                changed = true;
            }
        }
    }

    
    for (uint32_t it : cfg.nodes) {
        if (it == start) continue;

        std::unordered_set<uint32_t> strict_dom = DOM[it];
        strict_dom.erase(it);

        for (uint32_t candidate : strict_dom) {
            
            bool is_idom = true;
            for (uint32_t other : strict_dom) {
                if (other == candidate) continue;
                if (DOM[other].count(candidate)) {
                    is_idom = false;
                    break;
                }
            }
            if (is_idom) {
                idom[it] = candidate;
                edge_add(candidate, it);
                break;
            }
        }
    }
}


} // FDOMtree