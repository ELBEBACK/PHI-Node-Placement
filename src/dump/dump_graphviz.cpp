#include <iostream>
#include <fstream>
#include <unordered_set>

#include "dump_graphviz.hpp"


namespace viz {


namespace cfg_colors {
constexpr const char* COLOR_BG              = "#1E1B2E";
constexpr const char* COLOR_TEXT            = "#4E2859";
constexpr const char* COLOR_ARROW           = "#c6acff";

constexpr const char* COLOR_BODY            = "#B967FF";
constexpr const char* COLOR_FRAME           = "#b87ee2";
constexpr const char* COLOR_START           = "#01CDFE";
constexpr const char* COLOR_STOP            = "#FF71CE";
} // cfg_colors


namespace dtree_colors {
constexpr const char* COLOR_BG              = "#1E1B2E";
constexpr const char* COLOR_TEXT            = "#4E2859";
constexpr const char* COLOR_ARROW           = "#c6acff";

constexpr const char* COLOR_BODY            = "#B967FF";
constexpr const char* COLOR_FRAME           = "#b87ee2";
constexpr const char* COLOR_START           = "#01CDFE";
constexpr const char* COLOR_STOP            = "#FF71CE";
} // dtree_colors


namespace fdtree_colors {
constexpr const char* COLOR_BG              = "#1E1B2E";
constexpr const char* COLOR_TEXT            = "#4E2859";
constexpr const char* COLOR_ARROW           = "#c6acff";

constexpr const char* COLOR_BODY            = "#B967FF";
constexpr const char* COLOR_FRAME           = "#b87ee2";
constexpr const char* COLOR_START           = "#01CDFE";
constexpr const char* COLOR_STOP            = "#FF71CE";
} // fdtree_colors


namespace ltree_colors {
constexpr const char* COLOR_BG              = "#1E1B2E";
constexpr const char* COLOR_TEXT            = "#4E2859";
constexpr const char* COLOR_ARROW           = "#c6acff";

constexpr const char* COLOR_BODY            = "#B967FF";
constexpr const char* COLOR_FRAME           = "#b87ee2";
constexpr const char* COLOR_ROOT            = "#01CDFE";
constexpr const char* COLOR_IRREDUCIBLE     = "#FF71CE";
} // ltree_colors


static std::ofstream open_dot(const std::string& path, 
                                const char* title,      
                                const char* bg,         
                                const char* frame,      
                                const char* body,           
                                const char* text,       
                                const char* arrow);

static std::string display_id(uint32_t b);
static std::string display_ownership(const std::unordered_set<uint32_t>& own);
static const char* display_loop_type(Looptree::LoopType t);



void dump_graphviz(const std::string& dst_path, const CFGraph::cfgraph& target) {
        
    std::ofstream output_file = open_dot(dst_path,                  
                                            "CFGraph", 
                                            cfg_colors::COLOR_BG,       
                                            cfg_colors::COLOR_FRAME, 
                                            cfg_colors::COLOR_BODY,     
                                            cfg_colors::COLOR_TEXT,     
                                            cfg_colors::COLOR_ARROW);


    output_file << "  " << CFGraph::START << " [label=\"START\", fillcolor=\"" << cfg_colors::COLOR_START << "\", shape=oval];\n";
    output_file << "  " << CFGraph::STOP  << " [label=\"STOP\",  fillcolor=\"" << cfg_colors::COLOR_STOP  << "\", shape=oval];\n\n";

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


void dump_graphviz(const std::string& dst_path, const DOMtree::dtree& target) {
        
    std::ofstream output_file = open_dot(dst_path,                  
                                        "DOMtree", 
                                        dtree_colors::COLOR_BG,       
                                        dtree_colors::COLOR_FRAME, 
                                        dtree_colors::COLOR_BODY,     
                                        dtree_colors::COLOR_TEXT,     
                                        dtree_colors::COLOR_ARROW); // error handling

    output_file << "  " << CFGraph::START << " [label=\"START\", fillcolor=\"" << dtree_colors::COLOR_START << "\", shape=oval];\n";
    output_file << "  " << CFGraph::STOP  << " [label=\"STOP\",  fillcolor=\"" << dtree_colors::COLOR_STOP  << "\", shape=oval];\n\n";

    for (uint32_t subroot : target.nodes) {
        if (target.children.count(subroot) == 0)
            continue;

        for (uint32_t first_gen : target.children.at(subroot)) {
            output_file << "    " << subroot << " -> " << first_gen << ";\n";
        }
    }

    output_file << "}\n";

    return;
}
  

void dump_graphviz(const std::string& dst_path, const FDOMtree::fdtree& target) {
        
    std::ofstream output_file = open_dot(dst_path,                  
                                        "FDOMtree", 
                                        fdtree_colors::COLOR_BG,       
                                        fdtree_colors::COLOR_FRAME, 
                                        fdtree_colors::COLOR_BODY,     
                                        fdtree_colors::COLOR_TEXT,     
                                        fdtree_colors::COLOR_ARROW); // error handling

    output_file << "  " << CFGraph::START << " [label=\"START\", fillcolor=\"" << fdtree_colors::COLOR_START << "\", shape=oval];\n";
    output_file << "  " << CFGraph::STOP  << " [label=\"STOP\",  fillcolor=\"" << fdtree_colors::COLOR_STOP  << "\", shape=oval];\n\n";

    for (uint32_t n : target.nodes)
        output_file << "  " << n << ";\n";
 
    for (const auto& [u, frontier] : target.DF)
        for (uint32_t v : frontier)
            output_file << "  " << u << " -> " << v << ";\n";
 
    output_file << "}\n";

    return;
}



void dump_graphviz(const std::string& dst_path, const Looptree::ltree& target) {

    std::ofstream output_file = open_dot(dst_path,
                                         "LoopTree",
                                         ltree_colors::COLOR_BG,
                                         ltree_colors::COLOR_FRAME,
                                         ltree_colors::COLOR_BODY,
                                         ltree_colors::COLOR_TEXT,
                                         ltree_colors::COLOR_ARROW);

    for (const auto& [id, L] : target.loops) {

        const char* fill = ltree_colors::COLOR_BODY;
        if      (L.type == Looptree::LoopType::ROOT)        fill = ltree_colors::COLOR_ROOT;
        else if (L.type == Looptree::LoopType::IRREDUCIBLE) fill = ltree_colors::COLOR_IRREDUCIBLE;

        output_file << "  L" << id << " [fillcolor=\"" << fill << "\", label=\"";
        output_file << "loop[" << id << "]";
        if (L.type != Looptree::LoopType::ROOT) {
            output_file << "\\nheader: " << display_id(L.header);
            output_file << "\\n" << display_loop_type(L.type);
        }
        output_file << "\\nown: " << display_ownership(L.own_blocks) << "\"];\n";
    }

    output_file << "\n";

    for (const auto& [id, L] : target.loops)
        for (uint32_t child : L.children)
            output_file << "    L" << id << " -> L" << child << ";\n";

    output_file << "}\n";

    return;
}



static std::ofstream open_dot(const std::string& path, const char* title,
                                const char* bg,         const char* frame,
                                const char* body,       const char* text,
                                const char* arrow) 
{
    
    std::ofstream output_file(path);
    if (!output_file.is_open()) {
        std::cerr << "Could not open file " << path << "\n";
        return output_file;
    }
    
    output_file << "digraph " << title << " {\n";
    output_file << "  graph [rankdir=TB splines=true bgcolor=\"" << bg << "\"];\n";
    output_file << "  node  [fontname=\"Courier\", style=\"filled\", fontsize=16, margin=0.3, color=\"" << frame << "\", fillcolor=\"" << body << "\", fontcolor=\"" << text << "\"];\n";
    output_file << "  edge  [arrowhead=normal, penwidth=2, arrowsize=0.5, color=\"" << arrow << "\"];\n";
    
    return output_file;
}


static std::string display_id(uint32_t b) {
    if (b == CFGraph::START) return "START";
    if (b == CFGraph::STOP)  return "STOP";
    return std::to_string(b);
}


static std::string display_ownership(const std::unordered_set<uint32_t>& own) {
    std::string out = "{";
    bool first = true;
    for (uint32_t b : own) {
        if (!first) out += ", ";
        first = false;
        out += display_id(b);
    }
    out += "}";
    return out;
}


static const char* display_loop_type(Looptree::LoopType t) {
    switch (t) {
        case Looptree::LoopType::ROOT:        return "root";
        case Looptree::LoopType::REDUCIBLE:   return "reducible";
        case Looptree::LoopType::IRREDUCIBLE: return "irreducible";
    }
    return "?";
}

} // viz