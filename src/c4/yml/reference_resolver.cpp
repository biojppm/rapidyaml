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
#include "c4/yml/detail/print.hpp"


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
    print_tree("before", *t_);
    m_tree = t_;
    m_opts = opts;
    m_size_limit = m_opts.max_size(m_tree->size());
    m_known_keys.reset(m_opts);
}

id_type ReferenceResolver::branch_size_(id_type node)
{
    id_type count = 1;
    for(id_type ch = m_tree->first_child(node); ch != NONE; ch = m_tree->next_sibling(ch))
        count += branch_size_(ch);
    return count;
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
printf("requires_merge[%zu]----\n", node);
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
    for(id_type ch = m_tree->first_child(node); ch != NONE; )
    {
printf("requires_merge[%zu]: child=%zu\n", node, ch);
        NodeType ty = m_tree->type(ch);
        if(!is_merge_elm_(ch))
        {
printf("... %zu is vanilla key\n", ch);
            m_known_keys.add(m_tree->key(ch), ch, detail::KnownKeys::ORIGINAL);
            ch = m_tree->next_sibling(ch);
        }
        else
        {
            if(ty.has_val()) // it's a merge node `<<: *a`
            {
printf("... %zu is_merge val\n", ch);
                add_keys_mergedin_(ch, ++visit_count);
            }
            else // it's a merge node `<<: [*a,*b]`
            {
printf("... %zu is_merge seq\n", ch);
                RYML_ASSERT_VISIT_CB_(m_tree->m_callbacks, ty.is_seq(), m_tree, ch);
                for(id_type gch = m_tree->first_child(ch); gch != NONE; gch = m_tree->next_sibling(gch))
{
printf("... %zu: -> %zu\n", ch, gch);
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
    id_type after = m_tree->last_child(node);
    id_type num_unique = m_known_keys.num_unique_keys();
    for(id_type i = 0; i < num_unique; ++i)
    {
        KNK::Entry const* selected = m_known_keys.select(i, &rng);
        // clear all unselected original keys in the range. keys from aliases
        // must remain there (in the anchored elements).
        bool done = false;
        for(auto const& kv : rng)
        {
            if(kv.val.status & KNK::ORIGINAL)
            {
                if(&kv.val != selected)
                {
                    if(!done)
                    {
                        done = true;
                        if(!(selected->status & KNK::ORIGINAL))
                        {
                            m_tree->remove_children(kv.val.id);
                            m_tree->duplicate_contents(selected->id, kv.val.id);
                        }
                    }
                    else
                    {
                        m_tree->remove(kv.val.id);
                    }
                }
                else
                {
                    RYML_ASSERT_VISIT_CB_(m_tree->m_callbacks, !done, m_tree, node);
                    done = true;
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
        ++visit_count;
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
    if(ty.is_key_ref())
    {
        _c4dbgpf("node[{}]: key ref: '{}'", node, m_tree->key_ref(node));
        AnchorData const* anch = lookup_anchor_(node, visit_count, m_tree->key_ref(node));
        if C4_LIKELY (!!anch)
            handle_key_ref_(node, *anch);
    }
    else if(ty.is_val_ref())
    {
        _c4dbgpf("node[{}]: val ref: '{}'", node, m_tree->key_ref(node));
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
        visit_count = resolve_(ch, visit_count);
    }
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

void ReferenceResolver::handle_key_ref_(id_type ref_node, AnchorData const& anchor)
{
    RYML_CHECK_VISIT_CB_(m_tree->m_callbacks, !m_tree->is_container(anchor.node), m_tree, anchor.node);
    RYML_CHECK_VISIT_CB_(m_tree->m_callbacks, m_tree->has_val(anchor.node), m_tree, anchor.node);
    if(anchor.val_anchor)
    {
        const type_bits existing_style_flags = VAL_STYLE & m_tree->_p(anchor.node)->m_type.m_bits;
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
        const type_bits existing_style_flags = KEY_STYLE & m_tree->_p(anchor.node)->m_type.m_bits;
        m_tree->_add_flags(ref_node, KEY | existing_style_flags);
    }
    m_tree->_rem_flags(ref_node, KEYREF);
}

void ReferenceResolver::handle_val_ref_(id_type ref_node, AnchorData const& anchor)
{
    _c4dbgpf("instance[{}:node{}] is val ref", i, ref_node);
    NodeType tty = m_tree->type(anchor.node);
    if(anchor.val_anchor)
    {
        _c4dbgpf("instance[{}:node{}] duplicate contents", i, ref_node);
        check_size_limit_(anchor.node);
        const type_bits next = (m_tree->type(ref_node).m_bits & ~(VAL_STYLE|CONTAINER_STYLE|VAL|VALREF))
            | (tty.m_bits & (VAL_STYLE|CONTAINER_STYLE|VAL|SEQ|MAP));
        m_tree->duplicate_contents(anchor.node, ref_node);
        m_tree->_p(ref_node)->m_type = next;
    }
    else
    {
        _c4dbgpf("instance[{}:node{}] target.anchor==key.anchor=={}", i, ref_node, m_tree->key_anchor(anchor.node));
        RYML_CHECK_VISIT_CB_(m_tree->m_callbacks, !tty.is_container(), m_tree, anchor.node);
        RYML_CHECK_VISIT_CB_(m_tree->m_callbacks, tty.has_val(), m_tree, anchor.node);
        // keys cannot be containers, so don't inherit container flags
        const type_bits existing_style_flags = (KEY_STYLE) & tty.m_bits;
        static_assert((KEY_STYLE << 1u) == (VAL_STYLE), "bad flags");
        m_tree->_p(ref_node)->m_val.scalar = m_tree->key(anchor.node);
        m_tree->_add_flags(ref_node, VAL | (existing_style_flags << 1u));
    }
    m_tree->_rem_flags(ref_node, VALREF);
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
        _c4dbg_tree("final resolved tree", *m_tree);
    }

    m_tree = nullptr;
    _c4dbgp("resolving references: finished");
}

/** @endcond */

} // namespace ryml
} // namespace c4
