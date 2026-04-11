#include "dump_graphviz.hpp"


namespace viz {


#define COLOR_BG        "#1E1B2E"
#define COLOR_TEXT      "#4E2859"
#define COLOR_ARROW     "#c6acff"

#define COLOR_BODY      "#B967FF"   
#define COLOR_FRAME     "#b87ee2" 
#define COLOR_START     "#01CDFE"   
#define COLOR_STOP      "#FF71CE"  


void dump_graphviz(const std::string& dst_path, CFGraph::cfgraph& target) {
        
    std::ofstream output_file(dst_path);
    if (!output_file.is_open()) {
        std::cerr << "Could not open file " << dst_path << "\n";
        return;
    }

    output_file << "digraph CFGraph {\n";
    output_file << "  graph [rankdir=TB splines=true bgcolor=\"" << COLOR_BG << "\"];\n";
    output_file << "  node  [fontname=\"Courier\", style=\"filled\", fontsize=16, margin=0.3, color=\"" << COLOR_FRAME << "\", fillcolor=\"" << COLOR_BODY << "\", fontcolor=\"" << COLOR_TEXT << "\"];\n";
    output_file << "  edge  [arrowhead=normal, penwidth=2, arrowsize=0.5, color=\"" << COLOR_ARROW << "\"];\n";

    output_file << "  " << CFGraph::START << " [label=\"START\", fillcolor=\"" << COLOR_START << "\", shape=oval];\n";
    output_file << "  " << CFGraph::STOP  << " [label=\"STOP\",  fillcolor=\"" << COLOR_STOP  << "\", shape=oval];\n\n";

    for (uint32_t subroot : target.nodes) {
        if (target.successors.count(subroot) == 0)
            continue;

        for (uint32_t first_gen : target.successors.at(subroot)) {
            output_file << "    " << subroot << " -> " << first_gen << ";\n";
        }
    }

    output_file << "}\n";

    return;
}



#define COLOR_BG        "#1E1B2E" // TODO diff pallette
#define COLOR_TEXT      "#4E2859"
#define COLOR_ARROW     "#c6acff"

#define COLOR_BODY      "#B967FF"   
#define COLOR_FRAME     "#b87ee2" 
#define COLOR_START     "#01CDFE"   
#define COLOR_STOP      "#FF71CE"  


void dump_graphviz(const std::string& dst_path, DOMtree::dtree& target) {
        
    std::ofstream output_file(dst_path);
    if (!output_file.is_open()) {
        std::cerr << "Could not open file " << dst_path << "\n";
        return;
    }

    output_file << "digraph CFGraph {\n";
    output_file << "  graph [rankdir=TB splines=true bgcolor=\"" << COLOR_BG << "\"];\n";
    output_file << "  node  [fontname=\"Courier\", style=\"filled\", fontsize=16, margin=0.3, color=\"" << COLOR_FRAME << "\", fillcolor=\"" << COLOR_BODY << "\", fontcolor=\"" << COLOR_TEXT << "\"];\n";
    output_file << "  edge  [arrowhead=normal, penwidth=2, arrowsize=0.5, color=\"" << COLOR_ARROW << "\"];\n";

    output_file << "  " << CFGraph::START << " [label=\"START\", fillcolor=\"" << COLOR_START << "\", shape=oval];\n";
    output_file << "  " << CFGraph::STOP  << " [label=\"STOP\",  fillcolor=\"" << COLOR_STOP  << "\", shape=oval];\n\n";

    for (uint32_t subroot : target.nodes) {
        if (target.successors.count(subroot) == 0)
            continue;

        for (uint32_t first_gen : target.successors.at(subroot)) {
            output_file << "    " << subroot << " -> " << first_gen << ";\n";
        }
    }

    output_file << "}\n";

    return;
}



#define COLOR_BG        "#1E1B2E" // TODO diff pallette
#define COLOR_TEXT      "#4E2859"
#define COLOR_ARROW     "#c6acff"

#define COLOR_BODY      "#B967FF"   
#define COLOR_FRAME     "#b87ee2" 
#define COLOR_START     "#01CDFE"   
#define COLOR_STOP      "#FF71CE"  


void dump_graphviz(const std::string& dst_path, FDOMtree::fdtree& target) {
        
    std::ofstream output_file(dst_path);
    if (!output_file.is_open()) {
        std::cerr << "Could not open file " << dst_path << "\n";
        return;
    }

    output_file << "digraph CFGraph {\n";
    output_file << "  graph [rankdir=TB splines=true bgcolor=\"" << COLOR_BG << "\"];\n";
    output_file << "  node  [fontname=\"Courier\", style=\"filled\", fontsize=16, margin=0.3, color=\"" << COLOR_FRAME << "\", fillcolor=\"" << COLOR_BODY << "\", fontcolor=\"" << COLOR_TEXT << "\"];\n";
    output_file << "  edge  [arrowhead=normal, penwidth=2, arrowsize=0.5, color=\"" << COLOR_ARROW << "\"];\n";

    output_file << "  " << CFGraph::START << " [label=\"START\", fillcolor=\"" << COLOR_START << "\", shape=oval];\n";
    output_file << "  " << CFGraph::STOP  << " [label=\"STOP\",  fillcolor=\"" << COLOR_STOP  << "\", shape=oval];\n\n";

    for (uint32_t n : target.nodes)
        output_file << "  " << n << ";\n";
 
    for (auto& [u, frontier] : target.DF)
        for (uint32_t v : frontier)
            output_file << "  " << u << " -> " << v << ";\n";
 
    output_file << "}\n";;

    return;
}


} // viz