# loop_ssa: SSA Construction & Loop Analysis

A modular C++17 library for transforming Control Flow Graphs (CFG) into **Static Single Assignment (SSA)** form. This tool provides a full pipeline for dominance analysis, loop nesting detection, and minimal $\phi$-node placement.

## 🚀 Features

* **CFG Management**: Bidirectional graph structure with `START` (`0x000`) and `STOP` (`0xDED`) node markers.
* **Dominance Analysis**: Calculation of Immediate Dominators ($IDom$) and construction of the Dominator Tree.
* **Frontier Computation**: Efficient calculation of Dominance Frontiers ($DF$) for join-point analysis.
* **Loop Analysis**: Detection of natural loops, nesting structures, and categorization of `REDUCIBLE` vs `IRREDUCIBLE` control flow.
* **$\phi$ Placement**: Automated insertion of minimal $\phi$-nodes based on variable definitions and dominance frontiers.
* **Visualization**: Built-in support for exporting CFGs, Dominator Trees, and Loop Trees to Graphviz `.dot` format.

---

## 🏗 Build Instructions

This project requires **CMake 3.18+** and a **C++17** compatible compiler.

1.  **Create a build directory**:
    ```bash
    mkdir build && cd build
    ```
2.  **Configure the project**:
    ```bash
    cmake ..
    ```
3.  **Compile**:
    ```bash
    cmake --build .
    ```

---

## 💻 Usage Example

The library is designed around a clean API. Here is a typical workflow:

```cpp
#include "parser.hpp"
#include "DOMtree.hpp"
#include "FDOMtree.hpp"
#include "phi_placement.hpp"

int main() {
    // 1. Read CFG and variable definitions from stdin
    // parser::read(input_stream, edge_count, def_count)
    auto input = parser::read(std::cin, 10, 2);

    // 2. Build the Dominator Tree
    DOMtree::dtree dom;
    dom.build(input.cfg);

    // 3. Compute Dominance Frontiers
    FDOMtree::fdtree df;
    df.build(input.cfg, dom);

    // 4. Place PHI nodes for the given definitions
    auto phi_nodes = phi_placement::place(input.cfg, df, input.defs);
    
    return 0;
}