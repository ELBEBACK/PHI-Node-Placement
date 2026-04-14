#include "DOMtree.hpp"


namespace DOMtree {


void dtree::build(const CFGraph::cfgraph& cfg) {

    const uint32_t start_node = CFGraph::START;

    nodes = cfg.nodes;

    std::unordered_map<uint32_t, std::unordered_set<uint32_t>> dom_sets;

    dom_sets[start_node] = {start_node};
    for (uint32_t node : cfg.nodes)
        if (node != start_node)
            dom_sets[node] = cfg.nodes;

    bool sets_changed = true;
    while (sets_changed) {
        sets_changed = false;

        for (uint32_t node : cfg.nodes) {
            if (node == start_node) continue;

            std::unordered_set<uint32_t> new_dom;
            bool first = true;

            auto pred_it = cfg.predecessors.find(node);
            if (pred_it == cfg.predecessors.end()) continue;

            for (uint32_t pred : pred_it->second) {
                
                if (first) {
                    
                    new_dom = dom_sets[pred];
                    first = false;
                } else {
                    
                    for (auto it = new_dom.begin(); it != new_dom.end(); ) {
                        if (dom_sets[pred].count(*it) == 0)
                            it = new_dom.erase(it);
                        else
                            ++it;
                    }
                }
            }

            new_dom.insert(node);

            if (new_dom != dom_sets[node]) {
                dom_sets[node] = std::move(new_dom);
                sets_changed = true;
            }
        }
    }

    
    for (uint32_t node : cfg.nodes) {
        if (node == start_node) continue;

        std::unordered_set<uint32_t> strict_dom = dom_sets[node];
        strict_dom.erase(node);

        for (uint32_t idom_candidate : strict_dom) {
            
            bool is_idom = true;
            for (uint32_t other_dom : strict_dom) {
                if (other_dom == idom_candidate) continue;
                if (dom_sets[other_dom].count(idom_candidate)) {
                    is_idom = false;
                    break;
                }
            }
            if (is_idom) {
                idom[node] = idom_candidate;
                children[idom_candidate].push_back(node);
                break;
            }
        }
    }
}


} // DOMtree