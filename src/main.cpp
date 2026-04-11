#include <iostream>
#include <vector>

#include "CFGraph.hpp"
#include "DOMtree.hpp"
#include "FDOMtree.hpp"
#include "dump_graphviz.hpp"


int main(int argc, const char** argv) {

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <number_of_edges>" << std::endl;
        return -1;
    }

    CFGraph::cfgraph cfg;

    std::cout << "Enter your edges: <src> <dst>" << std::endl;
    for (int it = 0; it < atoi(argv[1]); ++it) {
        uint32_t a, b;
        std::cin >> a >> b;
        cfg.edge_add(a, b);
    }

    DOMtree::dtree dom;
    dom.build(cfg);

    FDOMtree::fdtree df;
    df.build(cfg, dom);

    viz::dump_graphviz("output/dot/cfg.dot", cfg);
    viz::dump_graphviz("output/dot/dom.dot", dom);
    viz::dump_graphviz("output/dot/df.dot", df);

    return 0;
}