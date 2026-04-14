#include <iostream>

#include "CFGraph.hpp"
#include "DOMtree.hpp"
#include "FDOMtree.hpp"
#include "Looptree.hpp"
#include "dump_graphviz.hpp"
#include "phi_placement.hpp"
#include "parser.hpp"
 

static std::string fmt_block(uint32_t b) {
    if (b == CFGraph::START) return "start";
    if (b == CFGraph::STOP)  return "stop";
    return std::to_string(b);
}


static const char* fmt_type(Looptree::LoopType t) {
    switch (t) {
        case Looptree::LoopType::REDUCIBLE:   return "reducible";
        case Looptree::LoopType::IRREDUCIBLE: return "irreducible";
        default:                              return "root";
    }
}



int main(int argc, const char** argv) {

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <number_of_edges> <number_of_defs>" << std::endl;
        return -1;
    }

    auto in = parser::read(std::cin, atoi(argv[1]), atoi(argv[2]));

    DOMtree::dtree   dom; 
    dom.build(in.cfg);
    
    FDOMtree::fdtree fd;  
    fd.build(in.cfg, dom);
    
    Looptree::ltree  lt;  
    lt.build(in.cfg);

    auto phi = phi_placement::place(in.cfg, fd, in.defs);


    std::cout << "\nLoop blocks:\n";
    for (const auto& [id, L] : lt.loops) {
        std::cout << "[" << id << "] -> {";
        bool first = true;
        for (uint32_t b : L.own_blocks) {
            if (!first) std::cout << ", ";
            first = false;
            std::cout << fmt_block(b);
        }
        std::cout << "}";
        if (L.type != Looptree::LoopType::ROOT)
            std::cout << ", header : " << fmt_block(L.header)
                      << ", " << fmt_type(L.type);
        std::cout << "\n";
    }

    std::cout << "Loop tree:\n";
    for (const auto& [id, L] : lt.loops)
        for (uint32_t child : L.children)
            std::cout << "{" << id << "," << child << "}\n";


    std::cout << "\nPHI nodes: {";
    bool first = true;
    for (uint32_t b : phi) {
        if (!first) std::cout << ", ";
        first = false;
        std::cout << fmt_block(b);
    }
    std::cout << "}\n";


    viz::dump_graphviz("output/dot/cfg.dot", in.cfg);
    viz::dump_graphviz("output/dot/dom.dot", dom);
    viz::dump_graphviz("output/dot/fd.dot",  fd);
    viz::dump_graphviz("output/dot/lt.dot",  lt);

    return 0;
}