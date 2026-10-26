#include "c4/yml/reference_resolver.hpp"
#include "c4/yml/tree.hpp"
#include "c4/yml/detail/dbgprint.hpp"
#ifdef RYML_DBG
#include "c4/yml/detail/print.hpp"
#else
#define _c4dbg_tree(...)
#define _c4dbg_node(...)
#endif

namespace c4 {
namespace yml {

/** @cond dev */

id_type ReferenceResolver::gather_anchors_and_refs_(id_type n, id_type prev_anchor)
{
    NodeType ty = m_tree->type(n);
    // insert key BEFORE inserting val
    // insert anchors BEFORE inserting refs
    if(ty.has_key())
    {
        // insert key anchors BEFORE inserting val anchors
        if(ty.has_key_anchor())
        {
            _c4dbgpf("node[{}]: instance[{}]: key anchor: '{}'", n, m_refs.size(), m_tree->key_anchor(n));
            RYML_CHECK_VISIT_CB_(m_tree->m_callbacks, ty.has_key(), m_tree, n);
            prev_anchor = add_(KEYANCH, n, prev_anchor);
        }
        else if(ty.is_key_ref())
        {
            _c4dbgpf("node[{}]: instance[{}]: key ref: '{}', key='{}'", n, m_refs.size(), m_tree->key_ref(n), m_tree->has_key(n) ? m_tree->key(n) : csubstr{"-"});
            RYML_ASSERT_VISIT_CB_(m_tree->m_callbacks, m_tree->key(n) != "<<", m_tree, n);
            RYML_CHECK_VISIT_CB_(m_tree->m_callbacks, m_tree->key(n).ends_with(m_tree->key_ref(n)), m_tree, n);
            (void)add_(KEYREF, n, prev_anchor);
        }
        else if(!ty.is_key_quoted() && m_tree->key(n) == "<<")
        {
            _c4dbgpf("node[{}]: key is <<", n);
            if(ty.is_seq())
            {
                // for merging multiple inheritance targets
                //   <<: [ *CENTER, *BIG ]
                _c4dbgpf("node[{}]: is seq!", n);
                RYML_ASSERT_VISIT_CB_(m_tree->m_callbacks, !ty.has_val_anchor(), m_tree, n);
                RYML_ASSERT_VISIT_CB_(m_tree->m_callbacks, !ty.is_val_ref(), m_tree, n);
                for(id_type ch = m_tree->first_child(n), first = ch; ch != NONE; ch = m_tree->next_sibling(ch))
                {
                    _c4dbgpf("node[{}]: instance [{}]: val ref, inheriting multiple: {} '{}'", n, m_refs.size(), ch, m_tree->_p(ch)->m_val.anchor);
                    if(m_tree->is_container(ch) || !m_tree->is_val_ref(ch))
                    {
                        if(ch == first)
                            break;
                        RYML_ERR_VISIT_CB_(m_tree->m_callbacks, m_tree, ch, "malformed ref in inheriting seq");
                    }
                    (void)add_(VALREF, ch, prev_anchor, n);
                }
                goto finish; // don't recurse into the seq  // NOLINT
            }
        }
    }
    if(ty.has_val_anchor())
    {
        _c4dbgpf("node[{}]: instance[{}]: val anchor: '{}'", n, m_refs.size(), m_tree->val_anchor(n));
        RYML_CHECK_VISIT_CB_(m_tree->m_callbacks, ty.has_val() || ty.is_container(), m_tree, n);
        prev_anchor = add_(VALANCH, n, prev_anchor);
    }
    if(ty.is_val_ref())
    {
        _c4dbgpf("node[{}]: instance[{}]: val ref: '{}'", n, m_refs.size(), m_tree->val_ref(n));
        RYML_CHECK_VISIT_CB_(m_tree->m_callbacks, (!ty.has_val()) || m_tree->val(n).ends_with(m_tree->val_ref(n)), m_tree, n);
        (void)add_(VALREF, n, prev_anchor);
    }
    // recurse
    for(id_type ch = m_tree->first_child(n); ch != NONE; ch = m_tree->next_sibling(ch))
        prev_anchor = gather_anchors_and_refs_(ch, prev_anchor);
finish:
    return prev_anchor;
}

void ReferenceResolver::gather_anchors_and_refs_()
{
    _c4dbgp("gathering anchors and refs...");

    // minimize (re-)allocations
    m_refs.clear();
    m_refs.reserve(m_tree->size());

    // now descend through the hierarchy
    gather_anchors_and_refs_(m_tree->root_id(), NONE);

    _c4dbgp("gathering anchors and refs: finished");
}

id_type ReferenceResolver::lookup_(RefData const* C4_RESTRICT ra)
{
    #ifdef RYML_DBG
    id_type instance = static_cast<id_type>(ra-m_refs.m_stack);
    id_type node = ra->node;
    #endif
    RYML_ASSERT_VISIT_CB_(m_tree->m_callbacks, ra->type.is_key_ref() || ra->type.is_val_ref(), m_tree, ra->node);
    RYML_ASSERT_VISIT_CB_(m_tree->m_callbacks, ra->type.is_key_ref() != ra->type.is_val_ref(), m_tree, ra->node);
    csubstr refname;
    _c4dbgpf("instance[{}:node{}]: lookup from node={}...", instance, node, ra->node);
    if(ra->type.is_val_ref())
    {
        refname = m_tree->val_ref(ra->node);
        _c4dbgpf("instance[{}:node{}]: valref: '{}'", instance, node, refname);
    }
    else
    {
        RYML_ASSERT_VISIT_CB_(m_tree->m_callbacks, ra->type.is_key_ref(), m_tree, ra->node);
        refname = m_tree->key_ref(ra->node);
        _c4dbgpf("instance[{}:node{}]: keyref: '{}'", instance, node, refname);
    }
    while(ra->prev_anchor != NONE)
    {
        ra = &m_refs[ra->prev_anchor];
        _c4dbgpf("instance[{}:node{}]: lookup '{}' at [{}:node{}]: keyref='{}' valref='{}'", instance, node, refname, ra-m_refs.m_stack, ra->node,
                 (m_tree->has_key_anchor(ra->node) ? m_tree->key_anchor(ra->node) : csubstr("~")),
                 (m_tree->has_val_anchor(ra->node) ? m_tree->val_anchor(ra->node) : csubstr("~")));
        if(m_tree->has_anchor(ra->node, refname))
        {
            _c4dbgpf("instance[{}:node{}]: got it at [{}:node{}]!", instance, node, ra-m_refs.m_stack, ra->node);
            return ra->node;
        }
    }
    RYML_ERR_VISIT_CB_(m_tree->m_callbacks, m_tree, ra->node, "anchor not found: '{}'", refname);
}

void ReferenceResolver::reset_(Tree *t_, ResolveOptions const& opts)
{
    if(t_->callbacks() != m_refs.m_callbacks)
        m_refs.m_callbacks = t_->callbacks();
    m_refs.clear();
    m_tree = t_;
    m_opts = opts;
    m_size_limit = m_opts.max_size(m_tree->size());
    m_known_key_subject = NONE;
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

void ReferenceResolver::add_known_key_(csubstr key, id_type node, bool original)
{
    RYML_ASSERT_VISIT_CB_(m_tree->m_callbacks, m_tree->key(node) == key, m_tree, node);
    KnownKey &entry = m_known_keys.insert(key);
    entry.node = node;
    entry.original = original;
}

void ReferenceResolver::start_merge_(id_type dst_map)
{
    RYML_ASSERT_VISIT_CB_(m_tree->m_callbacks, m_tree->is_map(dst_map), m_tree, dst_map);
    m_known_keys.clear();
    m_known_key_subject = dst_map;
    for(id_type child = m_tree->first_child(dst_map); child != NONE; child = m_tree->next_sibling(child))
        add_known_key_(m_tree->key(child), child, /*original*/true);
}

id_type ReferenceResolver::merge_to_map_(RefData const& refdata, id_type dst_map, id_type after)
{
    RYML_ASSERT_VISIT_CB_(m_tree->m_callbacks, m_tree->is_map(dst_map), m_tree, refdata.parent_ref);
    if C4_UNLIKELY (!m_tree->is_map(refdata.target))
        RYML_ERR_VISIT_CB_(m_tree->m_callbacks, m_tree, refdata.target, "merge source must be a map");
    if(m_known_key_subject != dst_map)
        start_merge_(dst_map);
    check_size_limit_(refdata.target);
    for(id_type chsrc = m_tree->first_child(refdata.target); chsrc != NONE; chsrc = m_tree->next_sibling(chsrc))
    {
        csubstr key = m_tree->key(chsrc);
        KnownKey *existing = m_known_keys.lookup_modify(key);
        if(!existing)
        {
            after = m_tree->duplicate(chsrc, dst_map, after);
            add_known_key_(key, after, /*original*/false);
        }
        else if(( ! existing->original && m_opts.overwrite_mergedin()) ||
                (existing->original && m_opts.overwrite_original()))
        {
            m_tree->remove_children(existing->node);
            m_tree->duplicate_contents(chsrc, existing->node);
        }
    }
    m_tree->remove(refdata.node);
    return after;
}

void ReferenceResolver::resolve_()
{
    /* from the specs: "an alias node refers to the most recent
     * node in the serialization having the specified anchor". So
     * we need to start looking upward from ref nodes.
     *
     * @see http://yaml.org/spec/1.2/spec.html#id2765878 */
    _c4dbgp("matching anchors/refs...");
    for(RefData &C4_RESTRICT refdata : m_refs)
    {
        if( ! refdata.type.is_ref())
            continue;
        refdata.target = lookup_(&refdata);
        // reject cyclic references to prevent a blowout:
        if C4_UNLIKELY (m_tree->is_ancestor(refdata.node, refdata.target))
            RYML_ERR_VISIT_CB_(m_tree->m_callbacks, m_tree, refdata.node,
                               "cyclic reference: the alias refers to an ancestor anchor");
    }
    _c4dbgp("matching anchors/refs: finished");

    // insert the resolved references
    _c4dbgp("modifying tree...");
    id_type prev_parent_ref = NONE;
    id_type prev_parent_ref_after = NONE;
    for(id_type i = 0, e = m_refs.size(); i < e; ++i)
    {
        RefData const& C4_RESTRICT refdata = m_refs[i];
        _c4dbgpf("instance[{}:node{}]: {}/{}...", i, refdata.node, i+1, e);
        if( ! refdata.type.is_ref())
            continue;
        _c4dbgpf("instance[{}:node{}]: is reference!", i, refdata.node);
        NodeType nty = m_tree->type(refdata.node);
        if(refdata.parent_ref != NONE) // it's a merge seq `<<: [*a,*a]`
        {
            _c4dbgpf("instance[{}:node{}] has parent: {}", i, refdata.node, refdata.parent_ref);
            RYML_ASSERT_VISIT_CB_(m_tree->m_callbacks, "<<" == m_tree->key(refdata.parent_ref), m_tree, refdata.parent_ref);
            RYML_ASSERT_VISIT_CB_(m_tree->m_callbacks, !m_tree->is_key_quoted(refdata.parent_ref), m_tree, refdata.parent_ref);
            RYML_ASSERT_VISIT_CB_(m_tree->m_callbacks, m_tree->is_seq(refdata.parent_ref), m_tree, refdata.parent_ref);
            const id_type p = m_tree->parent(refdata.parent_ref);
            const id_type after = (prev_parent_ref != refdata.parent_ref) ?
                refdata.parent_ref//prev_sibling(rd.parent_ref_sibling)
                :
                prev_parent_ref_after;
            prev_parent_ref_after = merge_to_map_(refdata, p, after);
            prev_parent_ref = refdata.parent_ref;
        }
        else
        {
            _c4dbgpf("instance[{}:node{}] has no parent", i, refdata.node);
            if(nty.has_key() && m_tree->key(refdata.node) == "<<") // it's a merge node `<<: *a`
            {
                _c4dbgpf("instance[{}:node{}] is inheriting", i, refdata.node);
                RYML_ASSERT_VISIT_CB_(m_tree->m_callbacks, !nty.is_key_quoted(), m_tree, refdata.node);
                RYML_ASSERT_VISIT_CB_(m_tree->m_callbacks, nty.is_keyval(), m_tree, refdata.node); // because <<: [] was handled above
                const id_type p = m_tree->parent(refdata.node);
                const id_type after = m_tree->prev_sibling(refdata.node);
                _c4dbgpf("instance[{}:node{}] p={} after={}", i, refdata.node, p, after);
                merge_to_map_(refdata, p, after);
            }
            else if(refdata.type.is_key_ref())
            {
                _c4dbgpf("instance[{}:node{}] is key ref", i, refdata.node);
                NodeType targetty = m_tree->type(refdata.target);
                RYML_ASSERT_VISIT_CB_(m_tree->m_callbacks, nty.is_key_ref(), m_tree, refdata.node);
                RYML_ASSERT_VISIT_CB_(m_tree->m_callbacks, targetty.has_key_anchor() || targetty.has_val_anchor(), m_tree, refdata.node);
                if(targetty.has_val_anchor() && m_tree->val_anchor(refdata.target) == m_tree->key_ref(refdata.node))
                {
                    _c4dbgpf("instance[{}:node{}] target.anchor==val.anchor=={}", i, refdata.node, m_tree->val_anchor(refdata.target));
                    RYML_CHECK_VISIT_CB_(m_tree->m_callbacks, !m_tree->is_container(refdata.target), m_tree, refdata.target);
                    RYML_CHECK_VISIT_CB_(m_tree->m_callbacks, m_tree->has_val(refdata.target), m_tree, refdata.target);
                    const type_bits existing_style_flags = VAL_STYLE & m_tree->_p(refdata.target)->m_type.m_bits;
                    static_assert((VAL_STYLE >> 1u) == (KEY_STYLE), "bad flags");
                    m_tree->_add_flags(refdata.node, KEY | (existing_style_flags >> 1u));
                    m_tree->_p(refdata.node)->m_key.scalar = m_tree->val(refdata.target);
                }
                else
                {
                    _c4dbgpf("instance[{}:node{}] don't inherit container flags", i, refdata.node);
                    RYML_ASSERT_VISIT_CB_(m_tree->m_callbacks, m_tree->key_anchor(refdata.target) == m_tree->key_ref(refdata.node), m_tree, refdata.target);
                    m_tree->_p(refdata.node)->m_key.scalar = m_tree->key(refdata.target);
                    // keys cannot be containers, so don't inherit container flags
                    const type_bits existing_style_flags = KEY_STYLE & m_tree->_p(refdata.target)->m_type.m_bits;
                    m_tree->_add_flags(refdata.node, KEY | existing_style_flags);
                }
            }
            else // val ref
            {
                _c4dbgpf("instance[{}:node{}] is val ref", i, refdata.node);
                NodeType tty = m_tree->type(refdata.target);
                RYML_ASSERT_VISIT_CB_(m_tree->m_callbacks, refdata.type.is_val_ref(), m_tree, refdata.node);
                if(tty.has_key_anchor() && m_tree->key_anchor(refdata.target) == m_tree->val_ref(refdata.node))
                {
                    _c4dbgpf("instance[{}:node{}] target.anchor==key.anchor=={}", i, refdata.node, m_tree->key_anchor(refdata.target));
                    RYML_CHECK_VISIT_CB_(m_tree->m_callbacks, !tty.is_container(), m_tree, refdata.target);
                    RYML_CHECK_VISIT_CB_(m_tree->m_callbacks, tty.has_val(), m_tree, refdata.target);
                    // keys cannot be containers, so don't inherit container flags
                    const type_bits existing_style_flags = (KEY_STYLE) & tty.m_bits;
                    static_assert((KEY_STYLE << 1u) == (VAL_STYLE), "bad flags");
                    m_tree->_p(refdata.node)->m_val.scalar = m_tree->key(refdata.target);
                    m_tree->_add_flags(refdata.node, VAL | (existing_style_flags << 1u));
                }
                else
                {
                    _c4dbgpf("instance[{}:node{}] duplicate contents", i, refdata.node);
                    check_size_limit_(refdata.target);
                    const type_bits next = (m_tree->type(refdata.node).m_bits & ~(VAL_STYLE|CONTAINER_STYLE|VAL|VALREF))
                                         | (tty.m_bits & (VAL_STYLE|CONTAINER_STYLE|VAL|SEQ|MAP));
                    m_tree->duplicate_contents(refdata.target, refdata.node);
                    m_tree->_p(refdata.node)->m_type = next;
                }
            }
        }
        _c4dbg_tree("after insertion", *m_tree);
    }
}

void ReferenceResolver::resolve(Tree *tree, ResolveOptions const& opts)
{
    _c4dbgp("resolving references...");

    reset_(tree, opts);

    _c4dbg_tree("unresolved tree", *m_tree);

    gather_anchors_and_refs_();
    if(m_refs.empty())
        return;
    resolve_();
    _c4dbg_tree("resolved tree", *m_tree);

    // clear anchors and refs
    if(opts.clear_anchors())
    {
        _c4dbgp("clearing anchors/refs");
        auto clear_ = [this]{
            for(auto const& C4_RESTRICT ar : m_refs)
            {
                m_tree->rem_anchor_ref(ar.node);
                if(ar.parent_ref != NONE)
                    if(m_tree->type(ar.parent_ref) != NOTYPE)
                        m_tree->remove(ar.parent_ref);
            }
        };
        clear_();
        // some of the elements injected during the resolution may
        // have nested anchors; these anchors will have been newly
        // injected during the resolution; collect again, and clear
        // again, to ensure those are also cleared:
        gather_anchors_and_refs_();
        clear_();
        _c4dbgp("clearing anchors/refs: finished");
        _c4dbg_tree("final resolved tree", *m_tree);
    }

    m_tree = nullptr;
    _c4dbgp("resolving references: finished");
}

/** @endcond */

} // namespace ryml
} // namespace c4 -````
