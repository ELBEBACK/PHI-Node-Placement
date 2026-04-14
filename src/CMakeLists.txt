add_library(loop_ssa_lib
    general/CFGraph.cpp
    parse/parser.cpp
    analysis/DOMtree.cpp
    analysis/FDOMtree.cpp
    analysis/Looptree.cpp
    analysis/phi_placement.cpp
    dump/dump_graphviz.cpp
)

target_include_directories(loop_ssa_lib
    PUBLIC ${CMAKE_SOURCE_DIR}/include
)

add_executable(loop_ssa main.cpp)
target_link_libraries(loop_ssa PRIVATE loop_ssa_lib)