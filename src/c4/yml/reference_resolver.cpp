#ifndef C4_YML_REFERENCE_RESOLVER_HPP_
#include "c4/yml/reference_resolver.hpp"
#endif
#ifndef C4_YML_TREE_HPP_
#include "c4/yml/tree.hpp"
#endif
#ifndef C4_YML_DETAIL_LOOKUP_ANCHOR_HPP_
#include "c4/yml/detail/lookup_anchor.hpp"
#endif
#ifndef C4_YML_DETAIL_DBGPRINT_HPP_
#include "c4/yml/detail/dbgprint.hpp"
#endif
#ifdef RYML_DBG
#ifndef C4_YML_DETAIL_PRINT_HPP_
#include "c4/yml/detail/print.hpp"
#endif
#else
#define _c4dbg_tree(...)
#define _c4dbg_node(...)
#endif


#include "c4/yml/detail/print.hpp" // FIXME


namespace c4 {
namespace yml {

/** @cond dev */

ReferenceResolver::ReferenceResolver(Callbacks const& cb)
    : m_anchors(cb)
    , m_known_keys(cb)
    , m_tree(nullptr)
    , m_opts()
    , m_size_limit()
{
}

void ReferenceResolver::reset_(Tree *t_, ResolveOptions const& opts)
{
    m_tree = t_;
    m_opts = opts;
    m_size_limit = m_opts.max_size(m_tree->size());
    m_known_keys.reset(m_opts);
    m_branch_size.resize(m_tree->capacity());
    for(uint32_t & v : m_branch_size)
        v = 0;
}

id_type ReferenceResolver::branch_size_(id_type node)
{
    RYML_ASSERT_BASIC_CB_(m_tree->m_callbacks, node < m_branch_size.size());
    uint32_t sz = m_branch_size[node];
    if(!sz)
    {
        sz = 1;
        for(id_type ch = m_tree->first_child(node); ch != NONE; ch = m_tree->next_sibling(ch))
            sz += static_cast<uint32_t>(branch_size_(ch));
        m_branch_size[node] = sz;
    }
    return static_cast<id_type>(sz);
}

void ReferenceResolver::check_size_limit_(id_type node_to_dup)
{
    if(!m_size_limit)
        return;
    const id_type next_size = m_tree->size() + branch_size_(node_to_dup);
    _c4dbgpf("tree_size={} target={} branch_size={} next_size={} limit={} mult={}",
             m_tree->size(), node_to_dup, branch_size_(node_to_dup), next_size, m_size_limit, m_opts.max_size_multiplier());
    if C4_UNLIKELY(next_size > m_size_limit)
         RYML_ERR_VISIT_CB_(m_tree->m_callbacks, m_tree, node_to_dup, "aliasing would exceed max size limit");
}

bool ReferenceResolver::is_merge_elm_(id_type child) const
{
    NodeType ty = m_tree->type(child);
    if(ty.is_key_quoted() || m_tree->key(child) != "<<")
    {
        return false;
    }
    else if(ty.is_val_ref())
    {
        return true;
    }
    else if(ty.is_seq())
    {
        bool has_merge_elms = false;
        for(id_type gch = m_tree->first_child(child); gch != NONE; gch = m_tree->next_sibling(gch))
        {
            if(m_tree->is_val_ref(gch))
                has_merge_elms = true;
            else if(has_merge_elms)
                RYML_ERR_VISIT_CB_(m_tree->m_callbacks, m_tree, gch, "malformed ref in inheriting seq");
        }
        return has_merge_elms;
    }
    return false;
}

void ReferenceResolver::add_keys_mergedin_(id_type ref_node, id_type visit_count)
{
printf("add_keys_merged_in: vcount=%zu\n", visit_count);
    RYML_ASSERT_VISIT_CB_(m_tree->m_callbacks, m_tree->is_val_ref(ref_node), m_tree, ref_node);
    AnchorData const* anchor = lookup_anchor_(ref_node, visit_count, m_tree->val_ref(ref_node));
    if(!anchor)
        return;
    if C4_LIKELY (m_tree->is_map(anchor->node))
    {
        for(id_type ch = m_tree->first_child(anchor->node); ch != NONE; ch = m_tree->next_sibling(ch))
            m_known_keys.add(m_tree->key(ch), ch, detail::KnownKeys::MERGEDIN);
        return;
    }
    RYML_ERR_VISIT_CB_(m_tree->m_callbacks, m_tree, anchor->node, "merge elements must be maps");
}

bool ReferenceResolver::requires_merge_(id_type node, id_type visit_count)
{
printf("requires_merge[%zu]---- vcount=%zu\n", node, visit_count);
    RYML_ASSERT_VISIT_CB_(m_tree->m_callbacks, m_tree->is_map(node), m_tree, node);
    bool has_merge_elms = false;
    for(id_type ch = m_tree->first_child(node); ch != NONE; ch = m_tree->next_sibling(ch))
    {
printf("requires_merge[%zu]: has_merge(%zu)?", node, ch);
        if(is_merge_elm_(ch))
        {
printf(" yes!\n");
            has_merge_elms = true;
            break;
        }
printf("\n");
    }
    if(!has_merge_elms && !m_opts.merge_repeated_keys())
    {
        return false;
    }
    m_known_keys.start_adding();
printf("requires_merge[%zu]: fill\n", node);
    for(id_type ch = m_tree->first_child(node); ch != NONE; ++visit_count)
    {
printf("requires_merge[%zu]: child=%zu vcount=%zu\n", node, ch, visit_count);
        NodeType ty = m_tree->type(ch);
        if(!is_merge_elm_(ch))
        {
printf("... %zu is vanilla key vcount=%zu\n", ch, visit_count);
            m_known_keys.add(m_tree->key(ch), ch, detail::KnownKeys::ORIGINAL);
            visit_count += branch_size_(ch);
            ch = m_tree->next_sibling(ch);
        }
        else
        {
            if(ty.has_val()) // it's a merge node `<<: *a`
            {
printf("... %zu is_merge val vcount=%zu\n", ch, visit_count);
                add_keys_mergedin_(ch, visit_count);
            }
            else // it's a merge node `<<: [*a,*b]`
            {
printf("... %zu is_merge seq vcount=%zu\n", ch, visit_count);
                RYML_ASSERT_VISIT_CB_(m_tree->m_callbacks, ty.is_seq(), m_tree, ch);
                for(id_type gch = m_tree->first_child(ch); gch != NONE; gch = m_tree->next_sibling(gch))
{
printf("... %zu: -> %zu.   vcount=%zu\n", ch, gch, visit_count);
                    add_keys_mergedin_(gch, ++visit_count);
}
            }
            // finally, remove the merge element.
            // but watchout the loop continuation!
            id_type to_remove = ch;
            ch = m_tree->next_sibling(ch);
            m_tree->remove(to_remove);
        }
    }
    m_known_keys.adding_is_complete();
    return m_known_keys.requires_merge();
}

bool ReferenceResolver::maybe_merge_keys_(id_type node, id_type visit_count)
{
    using KNK = detail::KnownKeys;
    RYML_ASSERT_VISIT_CB_(m_tree->m_callbacks, m_tree->is_map(node), m_tree, node);
    if(!requires_merge_(node, visit_count))
        return false;
    KNK::KnownKeyRange_ rng;
    id_type after = NONE; // keep track where to insert nodes
    id_type num_unique = m_known_keys.num_unique_keys();
print_tree("before merge", *m_tree, node);
printf("merge[%zu]: #unique_keys=%zu\n", node, num_unique);
    for(id_type i = 0; i < num_unique; ++i, ++visit_count)
    {
printf("merge[%zu]: ... unique_key[%zu]\n", node, i);
        KNK::Entry const* selected = m_known_keys.select(i, &rng);
        RYML_ASSERT_VISIT_CB_(m_tree->m_callbacks, rng.hi > rng.lo, m_tree, node);
printf("merge[%zu]: ... unique_key[%zu]: %.*s #selected=%zd\n", node, i, static_cast<int>(rng.lo->key.len), rng.lo->key.str, rng.hi - rng.lo);
        // clear all unselected original keys in the range; but do not
        // clear keys from aliases, which must remain there (in the
        // anchored elements).
        bool done = false;
        for(auto const& kv : rng)
        {
printf("merge[%zu]: ... after=%zu ", node, after);
prkv(kv);
printf("\n");
            if(kv.val.status & KNK::ORIGINAL)
            {
printf("merge[%zu]: ... original\n", node);
                if(&kv.val != selected)
                {
printf("merge[%zu]: ... !selected\n", node);
                    if(!done)
                    {
printf("merge[%zu]: ... !done\n", node);
                        done = true;
                        if(!(selected->status & KNK::ORIGINAL))
                        {
                            m_tree->remove_children(kv.val.id);
                            m_tree->duplicate_contents(selected->id, kv.val.id);
                            after = kv.val.id;
                        }
                    }
                    else
                    {
                        m_tree->remove(kv.val.id);
                    }
                }
                else if(!done)
                {
printf("merge[%zu]: ... selected\n", node);
                    done = true;
                    after = selected->id;
// descend into the original selected key
visit_count = resolve_(selected->id, visit_count);
                }
            }
        }
        RYML_ASSERT_VISIT_CB_(m_tree->m_callbacks, !(selected->status & KNK::ORIGINAL) || done, m_tree, node);
        // enforce the selected key
        if(!done)
        {
            after = m_tree->duplicate(selected->id, node, after);
        }
print_tree("after merge key", *m_tree, node);
    }
    return true;
}

id_type ReferenceResolver::gather_anchors_(id_type node, id_type visit_count, bool *hasrefs)
{
    NodeType ty = m_tree->type(node);
    if(ty.has_key_anchor())
    {
        _c4dbgpf("node[{}]: anchor[{}]: key anchor: '{}'", node, m_anchors.m_arr.size(), m_tree->key_anchor(node));
        add_anchor_(m_tree->key_anchor(node), node, visit_count, /*val_anchor*/false);
    }
    else if(ty.is_key_ref())
    {
        *hasrefs = true;
    }
    if(ty.has_val_anchor())
    {
        _c4dbgpf("node[{}]: anchor[{}]: val anchor: '{}'", node, m_anchors.m_arr.size(), m_tree->val_anchor(node));
        add_anchor_(m_tree->val_anchor(node), node, visit_count, /*val_anchor*/true);
    }
    else if(ty.is_val_ref())
    {
        *hasrefs = true;
    }
    ++visit_count;
    for(id_type ch = m_tree->first_child(node); ch != NONE; ch = m_tree->next_sibling(ch))
    {
        visit_count = gather_anchors_(ch, visit_count, hasrefs);
    }
    return visit_count;
}

id_type ReferenceResolver::resolve_(id_type node, id_type visit_count)
{
    NodeType ty = m_tree->type(node);
    _c4dbgpf("resolve[{}]: enter", node);
printf("resolve[%zu]: enter\n", node);
    if(ty.is_key_ref())
    {
        _c4dbgpf("node[{}]: ... key ref: '{}'", node, m_tree->key_ref(node));
printf("node[%zu]: key ref: '%.*s'\n", node, static_cast<int>(m_tree->key_ref(node).len), m_tree->key_ref(node).str);
        AnchorData const* anch = lookup_anchor_(node, visit_count, m_tree->key_ref(node));
        if C4_LIKELY (!!anch)
            handle_key_ref_(node, *anch);
    }
    if(ty.is_val_ref())
    {
        _c4dbgpf("node[{}]: val ref: '{}'", node, m_tree->val_ref(node));
printf("node[%zu]: val ref: '%.*s'\n", node, static_cast<int>(m_tree->val_ref(node).len), m_tree->val_ref(node).str);
        AnchorData const* anch = lookup_anchor_(node, visit_count, m_tree->val_ref(node));
        if C4_LIKELY (!!anch)
            handle_val_ref_(node, *anch);
    }
    else if(ty.is_map())
    {
        if(maybe_merge_keys_(node, visit_count))
            return visit_count + 1; // don't descend
    }
    ++visit_count;
    for(id_type ch = m_tree->first_child(node); ch != NONE; ch = m_tree->next_sibling(ch))
    {
printf("resolve[%zu]: descending to %zu\n", node, ch);
        visit_count = resolve_(ch, visit_count);
    }
printf("resolve[%zu]: exit\n", node);
    return visit_count;
}

auto ReferenceResolver::lookup_anchor_(id_type ref_node, id_type visit_count, csubstr ref)
    -> ReferenceResolver::AnchorData const*
{
    AnchorData const* anchor = detail::lookup_anchor(m_anchors, visit_count, ref, m_opts);
    if(!anchor)
        return nullptr;
    if C4_LIKELY (!m_tree->is_ancestor(ref_node, anchor->node))
        return anchor;
    // reject cyclic references to prevent a blowout:
    RYML_ERR_VISIT_CB_(m_tree->m_callbacks, m_tree, ref_node,
                       "cyclic reference: reference to ancestor node={}", anchor->node);
}

void ReferenceResolver::handle_val_ref_(id_type ref_node, AnchorData const& anchor)
{
    _c4dbgpf("node[{}] is val ref", ref_node);
    NodeType aty = m_tree->type(anchor.node);
    if(anchor.val_anchor)
    {
        _c4dbgpf("node[{}]: duplicate contents to val", ref_node);
        check_size_limit_(anchor.node);
        const type_bits next = (m_tree->type(ref_node).m_bits & ~(VAL_STYLE|CONTAINER_STYLE|VAL|VALREF))
            | (aty.m_bits & (VAL_STYLE|CONTAINER_STYLE|VAL|SEQ|MAP));
        m_tree->duplicate_contents(anchor.node, ref_node);
        m_tree->_p(ref_node)->m_type = next;
    }
    else
    {
        _c4dbgpf("node[{}] target.anchor==key.anchor=={}", ref_node, m_tree->key_anchor(anchor.node));
        RYML_CHECK_VISIT_CB_(m_tree->m_callbacks, !aty.is_container(), m_tree, anchor.node);
        RYML_CHECK_VISIT_CB_(m_tree->m_callbacks, aty.has_val(), m_tree, anchor.node);
        // keys cannot be containers, so don't inherit container flags
        const type_bits existing_style_flags = (KEY_STYLE) & aty.m_bits;
        static_assert((KEY_STYLE << 1u) == (VAL_STYLE), "bad flags");
        m_tree->_p(ref_node)->m_val.scalar = m_tree->key(anchor.node);
        m_tree->_add_flags(ref_node, VAL | (existing_style_flags << 1u));
    }
    m_tree->_rem_flags(ref_node, VALREF);
}

void ReferenceResolver::handle_key_ref_(id_type ref_node, AnchorData const& anchor)
{
    _c4dbgpf("node[{}] is key ref. anchor={}", ref_node, anchor.node);
    NodeType tty = m_tree->type(anchor.node);
    if C4_LIKELY (anchor.val_anchor ? tty.has_val() : tty.has_key())
    {
        if(anchor.val_anchor)
        {
            RYML_ASSERT_VISIT_CB_(m_tree->m_callbacks, m_tree->val_anchor(anchor.node) == m_tree->key_ref(ref_node), m_tree, anchor.node);
            const type_bits existing_style_flags = VAL_STYLE & tty.m_bits;
            static_assert((VAL_STYLE >> 1u) == (KEY_STYLE), "bad flags");
            m_tree->_add_flags(ref_node, KEY | (existing_style_flags >> 1u));
            m_tree->_p(ref_node)->m_key.scalar = m_tree->val(anchor.node);
        }
        else
        {
            _c4dbgpf("instance[{}:node{}] don't inherit container flags", i, ref_node);
            RYML_ASSERT_VISIT_CB_(m_tree->m_callbacks, m_tree->key_anchor(anchor.node) == m_tree->key_ref(ref_node), m_tree, anchor.node);
            m_tree->_p(ref_node)->m_key.scalar = m_tree->key(anchor.node);
            // keys cannot be containers, so don't inherit container flags
            const type_bits existing_style_flags = KEY_STYLE & tty.m_bits;
            m_tree->_add_flags(ref_node, KEY | existing_style_flags);
        }
        m_tree->_rem_flags(ref_node, KEYREF);
    }
    else
    {
        RYML_ERR_VISIT_CB_(m_tree->m_callbacks, m_tree, ref_node,
                           "key references must be to scalars. anchor node={}", anchor.node);
    }
}

void ReferenceResolver::resolve(Tree *tree, ResolveOptions const& opts)
{
    _c4dbgp("resolving references...");

    reset_(tree, opts);
    _c4dbg_tree("unresolved tree", *m_tree);

    bool has_refs = false;
    gather_anchors_(m_tree->root_id(), 0, &has_refs);
    if(has_refs || m_opts.merge_repeated_keys())
    {
        m_anchors.sort();
        resolve_(m_tree->root_id(), 0);
        _c4dbg_tree("resolved tree", *m_tree);
    }

    if(m_opts.clear_anchors() && !m_anchors.m_arr.empty())
    {
        for(auto const& kv : m_anchors.m_arr)
            m_tree->rem_anchor_ref(kv.val.node);
        m_anchors.clear();
        // some of the elements injected during the resolution may
        // have nested anchors; these anchors will have been newly
        // injected during the resolution; collect again, and clear
        // again, to ensure those are also cleared:
        gather_anchors_(m_tree->root_id(), 0, &has_refs);
        for(auto const& kv : m_anchors.m_arr)
            m_tree->rem_anchor_ref(kv.val.node);
        _c4dbg_tree("final resolved tree", *m_tree);
    }

    m_tree = nullptr;
    _c4dbgp("resolving references: finished");
}

/** @endcond */

} // namespace ryml
} // namespace c4
