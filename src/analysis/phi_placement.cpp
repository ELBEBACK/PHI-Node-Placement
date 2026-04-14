#include <vector>

#include "phi_placement.hpp"


namespace phi_placement {


std::unordered_set<uint32_t> place(const CFGraph::cfgraph&              cfg,
                                    const FDOMtree::fdtree&             fdom,
                                    const std::unordered_set<uint32_t>& defs) 
{

    std::unordered_set<uint32_t> phi_blocks;
    std::unordered_set<uint32_t> ever_in_worklist = defs;
    std::vector<uint32_t>        worklist(defs.begin(), defs.end());

    while (!worklist.empty()) {
        uint32_t x = worklist.back();
        worklist.pop_back();

        auto fd_it = fdom.DF.find(x);
        if (fd_it == fdom.DF.end()) continue;

        for (uint32_t y : fd_it->second) {
            if (phi_blocks.insert(y).second) 
                if (ever_in_worklist.insert(y).second)
                    worklist.push_back(y);
        }
    }

    return phi_blocks;
}


} // phi_placement