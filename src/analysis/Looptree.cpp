#include <algorithm>

#include "Looptree.hpp"


namespace Looptree {


static std::unordered_map<uint32_t, uint32_t> uf_parent;

static uint32_t union_find_lookup(uint32_t elem) {
    if (uf_parent[elem] != elem)
        uf_parent[elem] = union_find_lookup(uf_parent[elem]);
    return uf_parent[elem];
}

static void union_find_merge(uint32_t elem, uint32_t repr) {
    uf_parent[elem] = repr;
}


static uint32_t                               dfs_timer = 1;
static std::unordered_map<uint32_t, uint32_t> block_to_pre;
static std::unordered_map<uint32_t, uint32_t> pre_to_last_desc;
static std::unordered_map<uint32_t, uint32_t> pre_to_block;



static void depth_first_visit(uint32_t block, const CFGraph::cfgraph& cfg) {

    pre_to_block[dfs_timer]        = block;
    block_to_pre[block]            = dfs_timer++;
    uf_parent[block_to_pre[block]] = block_to_pre[block];

    auto succs_it = cfg.successors.find(block);
    if (succs_it != cfg.successors.end())
        for (uint32_t succ : succs_it->second)
            if (block_to_pre.count(succ) == 0)
                depth_first_visit(succ, cfg);

    pre_to_last_desc[block_to_pre[block]] = dfs_timer - 1;
}



static bool is_dfs_ancestor(uint32_t anc, uint32_t desc) {
    return anc <= desc && desc <= pre_to_last_desc[anc];
}



void ltree::build(const CFGraph::cfgraph& cfg) {

    uf_parent.clear();
    block_to_pre.clear();
    pre_to_last_desc.clear();
    pre_to_block.clear();
    dfs_timer = 1;

    loops.clear();
    block_loop.clear();
    

    uint32_t total = (uint32_t)cfg.nodes.size();

    depth_first_visit(CFGraph::START, cfg);

    std::unordered_map<uint32_t, std::vector<uint32_t>> back_preds;
    std::unordered_map<uint32_t, std::vector<uint32_t>> fwd_preds;
    std::unordered_map<uint32_t, uint32_t>              encl_header;
    std::unordered_map<uint32_t, LoopType>              hdr_type;

    for (uint32_t hdr = 1; hdr <= total; ++hdr) {
        back_preds[hdr]  = {};
        fwd_preds[hdr]   = {};
        encl_header[hdr] = 1;
        hdr_type[hdr]    = LoopType::ROOT;
    }

    encl_header[1] = 0;

    for (uint32_t block : cfg.nodes) {
        uint32_t block_pre = block_to_pre[block];

        auto preds_it = cfg.predecessors.find(block);
        if (preds_it == cfg.predecessors.end()) continue;

        for (uint32_t pred : preds_it->second) {
            uint32_t pred_pre = block_to_pre[pred];
            if (is_dfs_ancestor(block_pre, pred_pre))
                back_preds[block_pre].push_back(pred_pre);
            else
                fwd_preds[block_pre].push_back(pred_pre);
        }
    }

    for (uint32_t step = 0; step < total; ++step) {
        uint32_t hdr = total - step;

        std::vector<uint32_t> body;

        for (uint32_t back_pred : back_preds[hdr]) {
            if (back_pred != hdr)
                body.push_back(union_find_lookup(back_pred));
            else
                hdr_type[hdr] = LoopType::REDUCIBLE;
        }

        std::vector<uint32_t> worklist = body;
        if (!body.empty())
            hdr_type[hdr] = LoopType::REDUCIBLE;

        while (!worklist.empty()) {
            uint32_t cur = worklist.back();
            worklist.pop_back();

            for (uint32_t out_pred : fwd_preds[cur]) {
                uint32_t out_pred_repr = union_find_lookup(out_pred);

                if (!is_dfs_ancestor(hdr, out_pred_repr)) {
                    hdr_type[hdr] = LoopType::IRREDUCIBLE;
                    fwd_preds[hdr].push_back(out_pred_repr);
                } else if (std::find(body.begin(), body.end(), out_pred_repr) == body.end()
                           && out_pred_repr != hdr) {
                    body.push_back(out_pred_repr);
                    worklist.push_back(out_pred_repr);
                }
            }
        }

        for (uint32_t mem : body) {
            encl_header[mem] = hdr;
            union_find_merge(mem, hdr);
        }
    }


    std::unordered_map<uint32_t, uint32_t> pre_to_loop_id;
    pre_to_loop_id[1] = 0;

    loops[0] = { 0, CFGraph::START, LoopType::ROOT, {}, 0, {} };

    uint32_t next_id = 1;
    for (uint32_t hdr = 1; hdr <= total; ++hdr) {
        if (hdr_type[hdr] == LoopType::REDUCIBLE
         || hdr_type[hdr] == LoopType::IRREDUCIBLE) {
            loop new_loop;
            new_loop.id     = next_id;
            new_loop.header = pre_to_block[hdr];
            new_loop.type   = hdr_type[hdr];
            new_loop.parent = 0;
            loops[next_id]  = new_loop;
            pre_to_loop_id[hdr] = next_id++;
        }
    }


    for (uint32_t block_pre = 1; block_pre <= total; ++block_pre) {
        
        uint32_t parent_pre = encl_header[block_pre];

        bool     is_hdr        = pre_to_loop_id.count(block_pre)  != 0;
        bool     parent_is_hdr = pre_to_loop_id.count(parent_pre) != 0;

        uint32_t child_id  = is_hdr        ? pre_to_loop_id[block_pre]  : 0;
        uint32_t parent_id = parent_is_hdr ? pre_to_loop_id[parent_pre] : 0;

        if (is_hdr && child_id != 0) {
            loops[child_id].parent = parent_id;
            loops[parent_id].children.push_back(child_id);
        }

        uint32_t block       = pre_to_block[block_pre];
        uint32_t owning_loop = is_hdr ? child_id : parent_id;
        loops[owning_loop].own_blocks.insert(block);
        block_loop[block] = owning_loop;
    }
}


} // Looptree