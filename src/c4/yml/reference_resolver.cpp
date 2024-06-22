#include "c4/yml/reference_resolver.hpp"
#include "c4/dump.hpp" // this is needed to resolve a function in the next header
#include "c4/yml/common.hpp"
#include "c4/yml/detail/parser_dbg.hpp"
#ifdef RYML_DBG
#include "c4/yml/detail/print.hpp"
#else
#define _c4dbg_tree(...)
#define _c4dbg_node(...)
#endif

namespace c4 {
namespace yml {

id_type ReferenceResolver::count_anchors_and_refs_(id_type n)
{
    id_type c = 0;
    c += m_tree->has_key_anchor(n);
    c += m_tree->has_val_anchor(n);
    c += m_tree->is_key_ref(n);
    c += m_tree->is_val_ref(n);
    c += m_tree->has_key(n) && m_tree->key(n) == "<<";
    for(id_type ch = m_tree->first_child(n); ch != NONE; ch = m_tree->next_sibling(ch))
        c += count_anchors_and_refs_(ch);
    return c;
}

void ReferenceResolver::gather_anchors_and_refs__(id_type n)
{
    // insert key refs BEFORE inserting val refs
    if(m_tree->has_key(n))
    {
        if(m_tree->key(n) == "<<")
        {
            _c4dbgpf("node[{}]: key is <<", n);
            if(m_tree->has_val(n))
            {
                if(m_tree->is_val_ref(n))
                {
                    _c4dbgpf("node[{}]: val ref, inheriting!", n);
                    m_refs.push({VALREF, n, NONE, NONE, NONE, NONE});
                    //m_refs.push({KEYREF, n, NONE, NONE, NONE, NONE});
                }
                else
                {
                    _c4dbgpf("node[{}]: not ref!", n);
                }
            }
            else if(m_tree->is_seq(n))
            {
                // for merging multiple inheritance targets
                //   <<: [ *CENTER, *BIG ]
                _c4dbgpf("node[{}]: is seq!", n);
                for(id_type ich = m_tree->first_child(n); ich != NONE; ich = m_tree->next_sibling(ich))
                {
                    _c4dbgpf("node[{}]: val ref, inheriting multiple: {}", n, ich);
                    if(m_tree->is_container(ich))
                    {
                        detail::_report_err(m_tree->m_callbacks, "ERROR: node {} child {}: refs for << cannot be containers.'", n, ich);
                        C4_UNREACHABLE_AFTER_ERR();
                    }
                    m_refs.push({VALREF, ich, NONE, NONE, n, m_tree->next_sibling(n)});
                }
                return; // don't descend into the seq
            }
            else
            {
                detail::_report_err(m_tree->m_callbacks, "ERROR: node {}: refs for << must be either val or seq", n);
                C4_UNREACHABLE_AFTER_ERR();
            }
        }
        else if(m_tree->is_key_ref(n))
        {
            _c4dbgpf("node[{}]: key ref: '{}'", n, m_tree->key_ref(n));
            _RYML_CB_ASSERT(m_tree->m_callbacks, m_tree->key(n) != "<<");
            _RYML_CB_CHECK(m_tree->m_callbacks, (!m_tree->has_key(n)) || m_tree->key(n).ends_with(m_tree->key_ref(n)));
            m_refs.push({KEYREF, n, NONE, NONE, NONE, NONE});
        }
    }
    // val ref
    if(m_tree->is_val_ref(n) && (!m_tree->has_key(n) || m_tree->key(n) != "<<"))
    {
        _c4dbgpf("node[{}]: val ref: '{}'", n, m_tree->val_ref(n));
        RYML_CHECK((!m_tree->has_val(n)) || m_tree->val(n).ends_with(m_tree->val_ref(n)));
        m_refs.push({VALREF, n, NONE, NONE, NONE, NONE});
    }
    // anchors
    if(m_tree->has_key_anchor(n))
    {
        _c4dbgpf("node[{}]: key anchor: '{}'", n, m_tree->key_anchor(n));
        RYML_CHECK(m_tree->has_key(n));
        m_refs.push({KEYANCH, n, NONE, NONE, NONE, NONE});
    }
    if(m_tree->has_val_anchor(n))
    {
        _c4dbgpf("node[{}]: val anchor: '{}'", n, m_tree->val_anchor(n));
        RYML_CHECK(m_tree->has_val(n) || m_tree->is_container(n));
        m_refs.push({VALANCH, n, NONE, NONE, NONE, NONE});
    }
    // recurse
    for(id_type ch = m_tree->first_child(n); ch != NONE; ch = m_tree->next_sibling(ch))
        gather_anchors_and_refs__(ch);
}

void ReferenceResolver::gather_anchors_and_refs_()
{
    _c4dbgp("gathering anchors and refs...");

    // minimize (re-)allocations by counting first
    id_type num_anchors_and_refs = count_anchors_and_refs_(m_tree->root_id());
    if(!num_anchors_and_refs)
        return;
    m_refs.reserve(num_anchors_and_refs);
    m_refs.clear();

    // now descend through the hierarchy
    gather_anchors_and_refs__(m_tree->root_id());

    _c4dbgpf("found {} anchors/refs", m_refs.size());

    // finally connect the reference list
    id_type prev_anchor = NONE;
    id_type count = 0;
    for(auto &rd : m_refs)
    {
        rd.prev_anchor = prev_anchor;
        if(rd.type.has_anchor())
            prev_anchor = count;
        ++count;
    }
    _c4dbgp("gathering anchors and refs: finished");
}

id_type ReferenceResolver::lookup_(RefData *C4_RESTRICT ra)
{
    RYML_ASSERT(ra->type.is_key_ref() || ra->type.is_val_ref());
    RYML_ASSERT(ra->type.is_key_ref() != ra->type.is_val_ref());
    csubstr refname;
    if(ra->type.is_val_ref())
    {
        refname = m_tree->val_ref(ra->node);
    }
    else
    {
        RYML_ASSERT(ra->type.is_key_ref());
        refname = m_tree->key_ref(ra->node);
    }
    while(ra->prev_anchor != NONE)
    {
        ra = &m_refs[ra->prev_anchor];
        if(m_tree->has_anchor(ra->node, refname))
            return ra->node;
    }
    detail::_report_err(m_tree->m_callbacks, "ERROR: anchor not found: '{}'", refname);
    C4_UNREACHABLE_AFTER_ERR();
}

void ReferenceResolver::reset_(Tree *t_)
{
    if(t_->callbacks() != m_refs.m_callbacks)
    {
        m_refs.m_callbacks = t_->callbacks();
    }
    m_refs.clear();
    m_tree = t_;
}

void ReferenceResolver::resolve(Tree *t_)
{
    _c4dbgp("resolving references...");

    reset_(t_);

    _c4dbg_tree("unresolved tree", *m_tree);

    gather_anchors_and_refs_();
    if(m_refs.empty())
        return;

    /* from the specs: "an alias node refers to the most recent
     * node in the serialization having the specified anchor". So
     * we need to start looking upward from ref nodes.
     *
     * @see http://yaml.org/spec/1.2/spec.html#id2765878 */
    _c4dbgp("matching anchors/refs...");
    for(id_type i = 0, e = m_refs.size(); i < e; ++i)
    {
        RefData &C4_RESTRICT refdata = m_refs.top(i);
        if( ! refdata.type.is_ref())
            continue;
        refdata.target = lookup_(&refdata);
    }
    _c4dbgp("matching anchors/refs: finished");

    // insert the resolved references
    _c4dbgp("modifying tree...");
    id_type prev_parent_ref = NONE;
    id_type prev_parent_ref_after = NONE;
    for(id_type i = 0, e = m_refs.size(); i < e; ++i)
    {
        RefData const& C4_RESTRICT refdata = m_refs[i];
        _c4dbgpf("instance {}/{}...", i, e);
        if( ! refdata.type.is_ref())
            continue;
        _c4dbgpf("instance {} is reference!", i);
        if(refdata.parent_ref != NONE)
        {
            _c4dbgpf("ref {} has parent: {}", i, refdata.parent_ref);
            _RYML_CB_ASSERT(m_tree->m_callbacks, m_tree->is_seq(refdata.parent_ref));
            const id_type p = m_tree->parent(refdata.parent_ref);
            const id_type after = (prev_parent_ref != refdata.parent_ref) ?
                refdata.parent_ref//prev_sibling(rd.parent_ref_sibling)
                :
                prev_parent_ref_after;
            prev_parent_ref = refdata.parent_ref;
            prev_parent_ref_after = m_tree->duplicate_children_no_rep(refdata.target, p, after);
            m_tree->remove(refdata.node);
        }
        else
        {
            _c4dbgpf("ref {} has no parent", i, refdata.parent_ref);
            if(m_tree->has_key(refdata.node) && m_tree->key(refdata.node) == "<<")
            {
                _c4dbgpf("ref {} is inheriting", i);
                _RYML_CB_ASSERT(m_tree->m_callbacks, m_tree->is_keyval(refdata.node));
                const id_type p = m_tree->parent(refdata.node);
                const id_type after = m_tree->prev_sibling(refdata.node);
                m_tree->duplicate_children_no_rep(refdata.target, p, after);
                m_tree->remove(refdata.node);
            }
            else if(refdata.type.is_key_ref())
            {
                _c4dbgpf("ref {} is key ref", i);
                _RYML_CB_ASSERT(m_tree->m_callbacks, m_tree->is_key_ref(refdata.node));
                _RYML_CB_ASSERT(m_tree->m_callbacks, m_tree->has_key_anchor(refdata.target) || m_tree->has_val_anchor(refdata.target));
                if(m_tree->has_val_anchor(refdata.target) && m_tree->val_anchor(refdata.target) == m_tree->key_ref(refdata.node))
                {
                    _RYML_CB_CHECK(m_tree->m_callbacks, !m_tree->is_container(refdata.target));
                    _RYML_CB_CHECK(m_tree->m_callbacks, m_tree->has_val(refdata.target));
                    const type_bits existing_style_flags = VAL_STYLE & m_tree->_p(refdata.target)->m_type.type;
                    static_assert((VAL_STYLE >> 1u) == (KEY_STYLE), "bad flags");
                    m_tree->_p(refdata.node)->m_key.scalar = m_tree->val(refdata.target);
                    m_tree->_add_flags(refdata.node, KEY | (existing_style_flags >> 1u));
                }
                else
                {
                    _RYML_CB_CHECK(m_tree->m_callbacks, m_tree->key_anchor(refdata.target) == m_tree->key_ref(refdata.node));
                    m_tree->_p(refdata.node)->m_key.scalar = m_tree->key(refdata.target);
                    // keys cannot be containers, so don't inherit container flags
                    const type_bits existing_style_flags = KEY_STYLE & m_tree->_p(refdata.target)->m_type.type;
                    m_tree->_add_flags(refdata.node, KEY | existing_style_flags);
                }
            }
            else // val ref
            {
                _c4dbgpf("ref {} is val ref", i);
                _RYML_CB_ASSERT(m_tree->m_callbacks, refdata.type.is_val_ref());
                if(m_tree->has_key_anchor(refdata.target) && m_tree->key_anchor(refdata.target) == m_tree->val_ref(refdata.node))
                {
                    _RYML_CB_CHECK(m_tree->m_callbacks, !m_tree->is_container(refdata.target));
                    _RYML_CB_CHECK(m_tree->m_callbacks, m_tree->has_val(refdata.target));
                    // keys cannot be containers, so don't inherit container flags
                    const type_bits existing_style_flags = (KEY_STYLE) & m_tree->_p(refdata.target)->m_type.type;
                    static_assert((KEY_STYLE << 1u) == (VAL_STYLE), "bad flags");
                    m_tree->_p(refdata.node)->m_val.scalar = m_tree->key(refdata.target);
                    m_tree->_add_flags(refdata.node, VAL | (existing_style_flags << 1u));
                }
                else
                {
                    m_tree->duplicate_contents(refdata.target, refdata.node);
                }
            }
        }
    }
    _c4dbgp("modifying tree: finished");

    // clear anchors and refs
    _c4dbgp("clearing anchors/refs");
    for(auto const& C4_RESTRICT ar : m_refs)
    {
        m_tree->rem_anchor_ref(ar.node);
        if(ar.parent_ref != NONE)
            if(m_tree->type(ar.parent_ref) != NOTYPE)
                m_tree->remove(ar.parent_ref);
    }
    _c4dbgp("clearing anchors/refs: finished");

    _c4dbg_tree("resolved tree", *m_tree);

    m_tree = nullptr;
    _c4dbgp("resolving references: finished");
}


} // namespace ryml
} // namespace c4
