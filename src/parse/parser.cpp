#include <iostream>

#include "parser.hpp"


namespace parser {


parsed_input read(std::istream& in, uint32_t edge_count, uint32_t def_count) {

    parsed_input result;

    std::cout << "Enter your edges: <src> <dst>" << std::endl;
    for (uint32_t i = 0; i < edge_count; ++i) {
        uint32_t src, dst;
        in >> src >> dst;
        result.cfg.edge_add(src, dst);
    }

    std::cout << "Enter your defs: <blocks>" << std::endl;
    for (uint32_t i = 0; i < def_count; ++i) {
        uint32_t d;
        in >> d;
        result.defs.insert(d);
    }

    return result;
}


} // parser