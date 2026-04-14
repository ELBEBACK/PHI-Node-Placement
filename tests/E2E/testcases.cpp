#include <gtest/gtest.h>
#include <unordered_set>

#include "CFGraph.hpp"
#include "DOMtree.hpp"
#include "FDOMtree.hpp"
#include "Looptree.hpp"
#include "phi_placement.hpp"


namespace {

struct Pipeline {
    CFGraph::cfgraph cfg;
    DOMtree::dtree   dom;
    FDOMtree::fdtree df;
    Looptree::ltree  lt;

    void run() {
        dom.build(cfg);
        df.build(cfg, dom);
        lt.build(cfg);
    }

    std::unordered_set<uint32_t> phi(const std::unordered_set<uint32_t>& defs) const {
        return phi_placement::place(cfg, df, defs);
    }
};

const Looptree::loop* find_by_header(const Looptree::ltree& lt, uint32_t header) {
    for (const auto& [id, L] : lt.loops)
        if (id != 0 && L.header == header)
            return &L;
    return nullptr;
}

size_t real_loop_count(const Looptree::ltree& lt) {
    size_t n = 0;
    for (const auto& [id, _] : lt.loops) if (id != 0) ++n;
    return n;
}

bool any_loop_of_type(const Looptree::ltree& lt, Looptree::LoopType t) {
    for (const auto& [id, L] : lt.loops)
        if (id != 0 && L.type == t) return true;
    return false;
}

}



TEST(E2E, IrreducibleButterfly) {
    Pipeline p;
    p.cfg.edge_add(CFGraph::START, 1);
    p.cfg.edge_add(1, 2);
    p.cfg.edge_add(1, 3);
    p.cfg.edge_add(2, 3);
    p.cfg.edge_add(3, 2);
    p.cfg.edge_add(2, 4);
    p.cfg.edge_add(3, 4);
    p.cfg.edge_add(4, CFGraph::STOP);
    p.run();

    EXPECT_EQ(real_loop_count(p.lt), 1u);
    EXPECT_TRUE(any_loop_of_type(p.lt, Looptree::LoopType::IRREDUCIBLE));

    const Looptree::loop* L = nullptr;
    for (const auto& [id, loop] : p.lt.loops)
        if (id != 0) L = &loop;
    ASSERT_NE(L, nullptr);

    EXPECT_TRUE(L->header == 2 || L->header == 3);
    EXPECT_EQ(L->own_blocks.count(2), 1u);
    EXPECT_EQ(L->own_blocks.count(3), 1u);
    EXPECT_EQ(L->own_blocks.size(),   2u);

    const auto& root = p.lt.loops[0];
    EXPECT_EQ(root.own_blocks.count(CFGraph::START), 1u);
    EXPECT_EQ(root.own_blocks.count(1),              1u);
    EXPECT_EQ(root.own_blocks.count(4),              1u);
    EXPECT_EQ(root.own_blocks.count(CFGraph::STOP),  1u);

    auto phi = p.phi({ 2 });
    EXPECT_EQ(phi.count(4), 1u);
    EXPECT_EQ(phi.count(3), 1u);
}



TEST(E2E, TripleNestedReducible) {
    Pipeline p;
    p.cfg.edge_add(CFGraph::START, 1);
    p.cfg.edge_add(1, 2);
    p.cfg.edge_add(2, 3);
    p.cfg.edge_add(3, 4);
    p.cfg.edge_add(4, 3);
    p.cfg.edge_add(4, 5);
    p.cfg.edge_add(5, 2);
    p.cfg.edge_add(5, 6);
    p.cfg.edge_add(6, 1);
    p.cfg.edge_add(6, CFGraph::STOP);
    p.run();

    EXPECT_EQ(real_loop_count(p.lt), 3u);

    const auto* outer  = find_by_header(p.lt, 1);
    const auto* middle = find_by_header(p.lt, 2);
    const auto* inner  = find_by_header(p.lt, 3);

    ASSERT_NE(outer,  nullptr);
    ASSERT_NE(middle, nullptr);
    ASSERT_NE(inner,  nullptr);

    EXPECT_EQ(outer->type,  Looptree::LoopType::REDUCIBLE);
    EXPECT_EQ(middle->type, Looptree::LoopType::REDUCIBLE);
    EXPECT_EQ(inner->type,  Looptree::LoopType::REDUCIBLE);

    EXPECT_EQ(inner->own_blocks.count(3), 1u);
    EXPECT_EQ(inner->own_blocks.count(4), 1u);
    EXPECT_EQ(inner->own_blocks.size(),   2u);

    EXPECT_EQ(middle->own_blocks.count(2), 1u);
    EXPECT_EQ(middle->own_blocks.count(5), 1u);
    EXPECT_EQ(middle->own_blocks.size(),   2u);

    EXPECT_EQ(outer->own_blocks.count(1), 1u);
    EXPECT_EQ(outer->own_blocks.count(6), 1u);
    EXPECT_EQ(outer->own_blocks.size(),   2u);

    EXPECT_EQ(inner->parent,  middle->id);
    EXPECT_EQ(middle->parent, outer->id);
    EXPECT_EQ(outer->parent,  0u);

    auto phi = p.phi({ 4 });
    EXPECT_EQ(phi.count(1), 1u);  
    EXPECT_EQ(phi.count(2), 1u);  
    EXPECT_EQ(phi.count(3), 1u);  
}



TEST(E2E, LoopWithEarlyExit) {
    Pipeline p;
    p.cfg.edge_add(CFGraph::START, 1);
    p.cfg.edge_add(1, 2);
    p.cfg.edge_add(2, 3);
    p.cfg.edge_add(2, 5);
    p.cfg.edge_add(3, 4);
    p.cfg.edge_add(4, 2);
    p.cfg.edge_add(4, 5);
    p.cfg.edge_add(5, CFGraph::STOP);
    p.run();

    EXPECT_EQ(real_loop_count(p.lt), 1u);

    const auto* L = find_by_header(p.lt, 2);
    ASSERT_NE(L, nullptr);
    EXPECT_EQ(L->type, Looptree::LoopType::REDUCIBLE);
    EXPECT_EQ(L->own_blocks.count(2), 1u);
    EXPECT_EQ(L->own_blocks.count(3), 1u);
    EXPECT_EQ(L->own_blocks.count(4), 1u);
    EXPECT_EQ(L->own_blocks.size(),   3u);
    EXPECT_EQ(L->own_blocks.count(5), 0u); 

    EXPECT_EQ(p.lt.loops[0].own_blocks.count(5), 1u);

    auto phi = p.phi({ 3 });
    EXPECT_EQ(phi.count(2), 1u);
    EXPECT_EQ(phi.count(5), 1u);
}



TEST(E2E, PhiCascade) {
    Pipeline p;
    p.cfg.edge_add(CFGraph::START, 1);
    p.cfg.edge_add(1, 2);
    p.cfg.edge_add(2, 1);
    p.cfg.edge_add(2, 3);
    p.cfg.edge_add(3, 4);
    p.cfg.edge_add(4, 3);
    p.cfg.edge_add(4, 5);
    p.cfg.edge_add(5, 6);
    p.cfg.edge_add(6, CFGraph::STOP);
    p.run();

    EXPECT_EQ(real_loop_count(p.lt), 2u);
    EXPECT_TRUE(find_by_header(p.lt, 1) != nullptr);
    EXPECT_TRUE(find_by_header(p.lt, 3) != nullptr);

    auto phi_2 = p.phi({ 2 });
    EXPECT_EQ(phi_2.count(1), 1u);

    auto phi_4 = p.phi({ 4 });
    EXPECT_EQ(phi_4.count(3), 1u);

    auto phi_both = p.phi({ 2, 4 });
    EXPECT_EQ(phi_both.count(1), 1u);
    EXPECT_EQ(phi_both.count(3), 1u);
}



TEST(E2E, SelfLoopOnly) {
    Pipeline p;
    p.cfg.edge_add(CFGraph::START, 1);
    p.cfg.edge_add(1, 1);
    p.cfg.edge_add(1, CFGraph::STOP);
    p.run();

    EXPECT_EQ(real_loop_count(p.lt), 1u);

    const auto* L = find_by_header(p.lt, 1);
    ASSERT_NE(L, nullptr);
    EXPECT_EQ(L->type, Looptree::LoopType::REDUCIBLE);
    EXPECT_EQ(L->own_blocks.count(1), 1u);
    EXPECT_EQ(L->own_blocks.size(),   1u);

    auto phi = p.phi({ 1 });
    EXPECT_EQ(phi.count(1), 1u);
}



TEST(E2E, ReducibleWithIrreducibleInside) {
    Pipeline p;
    p.cfg.edge_add(CFGraph::START, 1);
    p.cfg.edge_add(1, 2);
    p.cfg.edge_add(2, 3);
    p.cfg.edge_add(2, 4);
    p.cfg.edge_add(3, 4);
    p.cfg.edge_add(4, 3);
    p.cfg.edge_add(3, 5);
    p.cfg.edge_add(4, 5);
    p.cfg.edge_add(5, 2);
    p.cfg.edge_add(5, CFGraph::STOP);
    p.run();

    EXPECT_EQ(real_loop_count(p.lt), 2u);

    const auto* outer = find_by_header(p.lt, 2);
    ASSERT_NE(outer, nullptr);
    EXPECT_EQ(outer->type, Looptree::LoopType::REDUCIBLE);
    EXPECT_EQ(outer->parent, 0u);

    const Looptree::loop* inner = nullptr;
    for (const auto& [id, L] : p.lt.loops) {
        if (id == 0 || L.id == outer->id) continue;
        inner = &L;
    }
    ASSERT_NE(inner, nullptr);
    EXPECT_EQ(inner->type,   Looptree::LoopType::IRREDUCIBLE);
    EXPECT_EQ(inner->parent, outer->id);
    EXPECT_TRUE(inner->header == 3 || inner->header == 4);
    EXPECT_EQ(inner->own_blocks.count(3), 1u);
    EXPECT_EQ(inner->own_blocks.count(4), 1u);
    EXPECT_EQ(inner->own_blocks.size(),   2u);

    auto phi = p.phi({ 3 });
    EXPECT_EQ(phi.count(4), 1u);
    EXPECT_EQ(phi.count(2), 1u);
    EXPECT_EQ(phi.count(5), 1u);
}



TEST(E2E, PhiDenseDiamondChain) {
    Pipeline p;
    p.cfg.edge_add(CFGraph::START, 1);
    p.cfg.edge_add(1, 2);
    p.cfg.edge_add(2, 3);
    p.cfg.edge_add(2, 4);
    p.cfg.edge_add(3, 4);
    p.cfg.edge_add(4, 5);
    p.cfg.edge_add(5, 6);
    p.cfg.edge_add(5, 7);
    p.cfg.edge_add(6, 7);
    p.cfg.edge_add(7, 8);
    p.cfg.edge_add(8, 9);
    p.cfg.edge_add(8, 10);
    p.cfg.edge_add(9, 10);
    p.cfg.edge_add(10, CFGraph::STOP);
    p.run();

    EXPECT_EQ(real_loop_count(p.lt), 0u);

    auto phi = p.phi({ 3, 6, 9 });
    EXPECT_EQ(phi.count(4),  1u);
    EXPECT_EQ(phi.count(7),  1u);
    EXPECT_EQ(phi.count(10), 1u);
    EXPECT_EQ(phi.size(),    3u);
    auto phi_first = p.phi({ 3 });
    EXPECT_EQ(phi_first.count(4),  1u);
    EXPECT_EQ(phi_first.count(7),  0u);
    EXPECT_EQ(phi_first.count(10), 0u);
}



TEST(E2E, HavlakTheoryPDF1) {
    Pipeline p;
    p.cfg.edge_add(CFGraph::START, 1);
    p.cfg.edge_add(1, 2);
    p.cfg.edge_add(1, 4);
    p.cfg.edge_add(2, 3);
    p.cfg.edge_add(3, 4);
    p.cfg.edge_add(3, 6);
    p.cfg.edge_add(4, 3);
    p.cfg.edge_add(4, 5);
    p.cfg.edge_add(5, 4);
    p.cfg.edge_add(6, 2);
    p.cfg.edge_add(6, 6);
    p.cfg.edge_add(6, 7);
    p.cfg.edge_add(7, 1);
    p.cfg.edge_add(7, CFGraph::STOP);
    p.run();


    EXPECT_GE(real_loop_count(p.lt), 4u);

    const auto* alpha_loop = find_by_header(p.lt, 1);
    ASSERT_NE(alpha_loop, nullptr);
    EXPECT_EQ(alpha_loop->type,   Looptree::LoopType::REDUCIBLE);
    EXPECT_EQ(alpha_loop->parent, 0u);

    const auto* delta_loop = find_by_header(p.lt, 4);
    ASSERT_NE(delta_loop, nullptr);
    EXPECT_EQ(delta_loop->type, Looptree::LoopType::REDUCIBLE);

    const auto* kappa_loop = find_by_header(p.lt, 6);
    ASSERT_NE(kappa_loop, nullptr);

    EXPECT_TRUE(any_loop_of_type(p.lt, Looptree::LoopType::IRREDUCIBLE));

    auto phi = p.phi({ 5 });
    EXPECT_EQ(phi.count(4), 1u);
}



TEST(E2E, SingleEdgeStartToStop) {
    Pipeline p;
    p.cfg.edge_add(CFGraph::START, CFGraph::STOP);
    p.run();

    EXPECT_EQ(real_loop_count(p.lt), 0u);
    EXPECT_EQ(p.lt.loops[0].own_blocks.count(CFGraph::START), 1u);
    EXPECT_EQ(p.lt.loops[0].own_blocks.count(CFGraph::STOP),  1u);

    auto phi_empty = p.phi({});
    EXPECT_TRUE(phi_empty.empty());

    auto phi_start = p.phi({ CFGraph::START });
    EXPECT_TRUE(phi_start.empty());
}