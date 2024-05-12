#ifndef _C4_YML_TREE_HPP_
#define _C4_YML_TREE_HPP_

/** @file tree.hpp */

#include "c4/types.hpp"
#ifndef _C4_YML_FWD_HPP_
#include "c4/yml/fwd.hpp"
#endif
#ifndef _C4_YML_COMMON_HPP_
#include "c4/yml/common.hpp"
#endif
#ifndef C4_YML_NODE_TYPE_HPP_
#include "c4/yml/node_type.hpp"
#endif
#ifndef _C4_YML_TAG_HPP_
#include "c4/yml/tag.hpp"
#endif
#ifndef _C4_CHARCONV_HPP_
#include <c4/charconv.hpp>
#endif

#include <cmath>
#include <limits>


C4_SUPPRESS_WARNING_MSVC_PUSH
C4_SUPPRESS_WARNING_MSVC(4251) // needs to have dll-interface to be used by clients of struct
C4_SUPPRESS_WARNING_MSVC(4296) // expression is always 'boolean_value'
C4_SUPPRESS_WARNING_GCC_CLANG_PUSH
C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")
C4_SUPPRESS_WARNING_GCC("-Wuseless-cast")
C4_SUPPRESS_WARNING_GCC("-Wtype-limits")


namespace c4 {
namespace yml {

/** encode a floating point value to a string. */
template<class T>
size_t to_chars_float(substr buf, T val)
{
    C4_SUPPRESS_WARNING_GCC_CLANG_WITH_PUSH("-Wfloat-equal");
    static_assert(std::is_floating_point<T>::value, "must be floating point");
    if(C4_UNLIKELY(std::isnan(val)))
        return to_chars(buf, csubstr(".nan"));
    else if(C4_UNLIKELY(val == std::numeric_limits<T>::infinity()))
        return to_chars(buf, csubstr(".inf"));
    else if(C4_UNLIKELY(val == -std::numeric_limits<T>::infinity()))
        return to_chars(buf, csubstr("-.inf"));
    return to_chars(buf, val);
    C4_SUPPRESS_WARNING_GCC_CLANG_POP
}


/** decode a floating point from string. Accepts special values: .nan,
 * .inf, -.inf */
template<class T>
bool from_chars_float(csubstr buf, T *C4_RESTRICT val)
{
    static_assert(std::is_floating_point<T>::value, "must be floating point");
    if(C4_LIKELY(from_chars(buf, val)))
    {
        return true;
    }
    else if(C4_UNLIKELY(buf == ".nan" || buf == ".NaN" || buf == ".NAN"))
    {
        *val = std::numeric_limits<T>::quiet_NaN();
        return true;
    }
    else if(C4_UNLIKELY(buf == ".inf" || buf == ".Inf" || buf == ".INF"))
    {
        *val = std::numeric_limits<T>::infinity();
        return true;
    }
    else if(C4_UNLIKELY(buf == "-.inf" || buf == "-.Inf" || buf == "-.INF"))
    {
        *val = -std::numeric_limits<T>::infinity();
        return true;
    }
    else
    {
        return false;
    }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


/** @addtogroup doc_tree
 *
 * @{
 */

/** a node scalar is a csubstr, which may be tagged and anchored. */
struct NodeScalar
{
    csubstr tag;
    csubstr scalar;
    csubstr anchor;

public:

    /// initialize as an empty scalar
    inline NodeScalar() noexcept : tag(), scalar(), anchor() {}

    /// initialize as an untagged scalar
    template<size_t N>
    inline NodeScalar(const char (&s)[N]) noexcept : tag(), scalar(s), anchor() {}
    inline NodeScalar(csubstr      s    ) noexcept : tag(), scalar(s), anchor() {}

    /// initialize as a tagged scalar
    template<size_t N, size_t M>
    inline NodeScalar(const char (&t)[N], const char (&s)[N]) noexcept : tag(t), scalar(s), anchor() {}
    inline NodeScalar(csubstr      t    , csubstr      s    ) noexcept : tag(t), scalar(s), anchor() {}

public:

    ~NodeScalar() noexcept = default;
    NodeScalar(NodeScalar &&) noexcept = default;
    NodeScalar(NodeScalar const&) noexcept = default;
    NodeScalar& operator= (NodeScalar &&) noexcept = default;
    NodeScalar& operator= (NodeScalar const&) noexcept = default;

public:

    bool empty() const noexcept { return tag.empty() && scalar.empty() && anchor.empty(); }

    void clear() noexcept { tag.clear(); scalar.clear(); anchor.clear(); }
};
C4_MUST_BE_TRIVIAL_COPY(NodeScalar);


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/** convenience class to initialize nodes */
struct NodeInit
{

    NodeType   type;
    NodeScalar key;
    NodeScalar val;

public:

    /// initialize as an empty node
    NodeInit() : type(NOTYPE), key(), val() {}
    /// initialize as a typed node
    NodeInit(NodeType_e t) : type(t), key(), val() {}
    /// initialize as a sequence member
    NodeInit(NodeScalar const& v) : type(VAL), key(), val(v) { _add_flags(); }
    /// initialize as a sequence member with explicit type
    NodeInit(NodeScalar const& v, NodeType_e t) : type(t|VAL), key(), val(v) { _add_flags(); }
    /// initialize as a mapping member
    NodeInit(              NodeScalar const& k, NodeScalar const& v) : type(KEYVAL), key(k), val(v) { _add_flags(); }
    /// initialize as a mapping member with explicit type
    NodeInit(NodeType_e t, NodeScalar const& k, NodeScalar const& v) : type(t), key(k), val(v) { _add_flags(); }
    /// initialize as a mapping member with explicit type (eg for SEQ or MAP)
    NodeInit(NodeType_e t, NodeScalar const& k                     ) : type(t), key(k), val( ) { _add_flags(KEY); }

public:

    void clear()
    {
        type.clear();
        key.clear();
        val.clear();
    }

    void _add_flags(type_bits more_flags=0)
    {
        type = (type|more_flags);
        if( ! key.tag.empty())
            type = (type|KEYTAG);
        if( ! val.tag.empty())
            type = (type|VALTAG);
        if( ! key.anchor.empty())
            type = (type|KEYANCH);
        if( ! val.anchor.empty())
            type = (type|VALANCH);
    }

    bool _check() const
    {
        // key cannot be empty
        RYML_ASSERT(key.scalar.empty() == ((type & KEY) == 0));
        // key tag cannot be empty
        RYML_ASSERT(key.tag.empty() == ((type & KEYTAG) == 0));
        // val may be empty even though VAL is set. But when VAL is not set, val must be empty
        RYML_ASSERT(((type & VAL) != 0) || val.scalar.empty());
        // val tag cannot be empty
        RYML_ASSERT(val.tag.empty() == ((type & VALTAG) == 0));
        return true;
    }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class RYML_EXPORT Tree
{
public:

    /** @name construction and assignment */
    /** @{ */

    Tree() : Tree(get_callbacks()) {}
    Tree(Callbacks const& cb);
    Tree(id_type node_capacity, size_t arena_capacity=0) : Tree(node_capacity, arena_capacity, get_callbacks()) {}
    Tree(id_type node_capacity, size_t arena_capacity, Callbacks const& cb);

    ~Tree();

    Tree(Tree const& that);
    Tree(Tree     && that) noexcept;

    Tree& operator= (Tree const& that);
    Tree& operator= (Tree     && that) RYML_NOEXCEPT;

    /** @} */

public:

    /** @name memory and sizing */
    /** @{ */

    void reserve(id_type node_capacity);

    /** clear the tree and zero every node
     * @note does NOT clear the arena
     * @see clear_arena() */
    void clear();
    inline void clear_arena() { m_arena_pos = 0; }

    inline bool   empty() const { return m_size == 0; }

    inline id_type size() const { return m_size; }
    inline id_type capacity() const { return m_cap; }
    inline id_type slack() const { RYML_ASSERT(m_cap >= m_size); return m_cap - m_size; }

    Callbacks const& callbacks() const { return m_callbacks; }
    void callbacks(Callbacks const& cb) { m_callbacks = cb; }

    /** @} */

public:

    /** @name node getters */
    /** @{ */

    /** @cond dev */
    #define _ryml_chkid(node) _RYML_CB_ASSERT(m_callbacks, node != NONE && node >= 0 && node < m_cap)
    /** @endcond */

    //! Get the id of the root node
    id_type root_id()       { if(m_cap == 0) { reserve(16); } _RYML_CB_ASSERT(m_callbacks, m_cap > 0 && m_size > 0); return 0; }
    //! Get the id of the root node
    id_type root_id() const {                                 _RYML_CB_ASSERT(m_callbacks, m_cap > 0 && m_size > 0); return 0; }

    //! Get a NodeRef of a node by id
    NodeRef      ref(id_type node);
    //! Get a NodeRef of a node by id
    ConstNodeRef ref(id_type node) const;
    //! Get a NodeRef of a node by id
    ConstNodeRef cref(id_type node) const;

    //! Get the root as a NodeRef
    NodeRef      rootref();
    //! Get the root as a ConstNodeRef
    ConstNodeRef rootref() const;
    //! Get the root as a ConstNodeRef
    ConstNodeRef crootref() const;

    //! get the i-th document of the stream
    //! @note @p i is NOT the node id, but the doc position within the stream
    NodeRef      docref(id_type i);
    //! get the i-th document of the stream
    //! @note @p i is NOT the node id, but the doc position within the stream
    ConstNodeRef docref(id_type i) const;
    //! get the i-th document of the stream
    //! @note @p i is NOT the node id, but the doc position within the stream
    ConstNodeRef cdocref(id_type i) const;

    //! find a root child by name, return it as a NodeRef
    //! @note requires the root to be a map.
    NodeRef      operator[] (csubstr key);
    //! find a root child by name, return it as a NodeRef
    //! @note requires the root to be a map.
    ConstNodeRef operator[] (csubstr key) const;

    //! find a root child by index: return the root node's @p i-th child as a NodeRef
    //! @note @p i is NOT the node id, but the child's position
    NodeRef      operator[] (id_type i);
    //! find a root child by index: return the root node's @p i-th child as a NodeRef
    //! @note @p i is NOT the node id, but the child's position
    ConstNodeRef operator[] (id_type i) const;

    /** @} */

public:

    /** @name node property getters */
    /** @{ */

    NodeType type(id_type node) const { _ryml_chkid(node); return m_type[node]; }
    const char* type_str(id_type node) const { _ryml_chkid(node); return NodeType::type_str(m_type[node]); }

    csubstr    const& key       (id_type node) const { _ryml_chkid(node); _RYML_CB_ASSERT(m_callbacks, has_key(node)); return m_key[node]; }
    csubstr    const& key_tag   (id_type node) const { _ryml_chkid(node); _RYML_CB_ASSERT(m_callbacks, has_key_tag(node)); return m_key_tag[node]; }
    csubstr    const& key_ref   (id_type node) const { _ryml_chkid(node); _RYML_CB_ASSERT(m_callbacks, is_key_ref(node)); return m_key_anchor[node]; }
    csubstr    const& key_anchor(id_type node) const { _ryml_chkid(node); _RYML_CB_ASSERT(m_callbacks, has_key_anchor(node)); return m_key_anchor[node]; }

    csubstr    const& val       (id_type node) const { _ryml_chkid(node); _RYML_CB_ASSERT(m_callbacks, has_val(node)); return m_val[node]; }
    csubstr    const& val_tag   (id_type node) const { _ryml_chkid(node); _RYML_CB_ASSERT(m_callbacks, has_val_tag(node)); return m_val_tag[node]; }
    csubstr    const& val_ref   (id_type node) const { _ryml_chkid(node); _RYML_CB_ASSERT(m_callbacks, is_val_ref(node)); return m_val_anchor[node]; }
    csubstr    const& val_anchor(id_type node) const { _ryml_chkid(node); _RYML_CB_ASSERT(m_callbacks, has_val_anchor(node)); return m_val_anchor[node]; }

    /** @} */

public:

    /** @name node type predicates */
    /** @{ */

    C4_ALWAYS_INLINE bool type_has_any(id_type node, NodeType_e bits) const { _ryml_chkid(node); return m_type[node].has_any(bits); }
    C4_ALWAYS_INLINE bool type_has_all(id_type node, NodeType_e bits) const { _ryml_chkid(node); return m_type[node].has_all(bits); }
    C4_ALWAYS_INLINE bool type_has_none(id_type node, NodeType_e bits) const { _ryml_chkid(node); return m_type[node].has_none(bits); }

    C4_ALWAYS_INLINE bool is_stream(id_type node) const { _ryml_chkid(node); return m_type[node].is_stream(); }
    C4_ALWAYS_INLINE bool is_doc(id_type node) const { _ryml_chkid(node); return m_type[node].is_doc(); }
    C4_ALWAYS_INLINE bool is_container(id_type node) const { _ryml_chkid(node); return m_type[node].is_container(); }
    C4_ALWAYS_INLINE bool is_map(id_type node) const { _ryml_chkid(node); return m_type[node].is_map(); }
    C4_ALWAYS_INLINE bool is_seq(id_type node) const { _ryml_chkid(node); return m_type[node].is_seq(); }
    C4_ALWAYS_INLINE bool has_key(id_type node) const { _ryml_chkid(node); return m_type[node].has_key(); }
    C4_ALWAYS_INLINE bool has_val(id_type node) const { _ryml_chkid(node); return m_type[node].has_val(); }
    C4_ALWAYS_INLINE bool is_val(id_type node) const { _ryml_chkid(node); return m_type[node].is_val(); }
    C4_ALWAYS_INLINE bool is_keyval(id_type node) const { _ryml_chkid(node); return m_type[node].is_keyval(); }
    C4_ALWAYS_INLINE bool has_key_tag(id_type node) const { _ryml_chkid(node); return m_type[node].has_key_tag(); }
    C4_ALWAYS_INLINE bool has_val_tag(id_type node) const { _ryml_chkid(node); return m_type[node].has_val_tag(); }
    C4_ALWAYS_INLINE bool has_key_anchor(id_type node) const { _ryml_chkid(node); return m_type[node].has_key_anchor(); }
    C4_ALWAYS_INLINE bool has_val_anchor(id_type node) const { _ryml_chkid(node); return m_type[node].has_val_anchor(); }
    C4_ALWAYS_INLINE bool has_anchor(id_type node) const { _ryml_chkid(node); return m_type[node].has_anchor(); }
    C4_ALWAYS_INLINE bool is_key_ref(id_type node) const { _ryml_chkid(node); return m_type[node].is_key_ref(); }
    C4_ALWAYS_INLINE bool is_val_ref(id_type node) const { _ryml_chkid(node); return m_type[node].is_val_ref(); }
    C4_ALWAYS_INLINE bool is_ref(id_type node) const { _ryml_chkid(node); return m_type[node].is_ref(); }

    C4_ALWAYS_INLINE bool parent_is_seq(id_type node) const { _ryml_chkid(node); _RYML_CB_ASSERT(m_callbacks, has_parent(node)); return is_seq(m_parent[node]); }
    C4_ALWAYS_INLINE bool parent_is_map(id_type node) const { _ryml_chkid(node); _RYML_CB_ASSERT(m_callbacks, has_parent(node)); return is_map(m_parent[node]); }

    /** true when the node has an anchor named a */
    C4_ALWAYS_INLINE bool has_anchor(id_type node, csubstr a) const { _ryml_chkid(node); return ((m_type[node] & KEYANCH) && m_key_anchor[node] == a) || ((m_type[node] & VALANCH) && m_val_anchor[node] == a); }

    /** true if the node key does not have any KEYQUO flags, and its scalar verifies scalar_is_null().
     * @warning the node must verify .has_key() (asserted) (ie must be a member of a map)
     * @see https://github.com/biojppm/rapidyaml/issues/413 */
    C4_ALWAYS_INLINE bool key_is_null(id_type node) const { _ryml_chkid(node); _RYML_CB_ASSERT(m_callbacks, has_key(node)); return !m_type[node].is_key_quoted() && scalar_is_null(m_key[node]); }
    /** true if the node key does not have any VALQUO flags, and its scalar verifies scalar_is_null().
     * @warning the node must verify .has_val() (asserted) (ie must be a scalar / must not be a container)
     * @see https://github.com/biojppm/rapidyaml/issues/413 */
    C4_ALWAYS_INLINE bool val_is_null(id_type node) const { _ryml_chkid(node); _RYML_CB_ASSERT(m_callbacks, has_val(node)); return !m_type[node].is_val_quoted() && scalar_is_null(m_val[node]); }

    /// true if the key was a scalar requiring filtering and was left
    /// unfiltered during the parsing (see ParserOptions)
    C4_ALWAYS_INLINE bool is_key_unfiltered(id_type node) const { _ryml_chkid(node); return m_type[node].is_key_unfiltered(); }
    /// true if the val was a scalar requiring filtering and was left
    /// unfiltered during the parsing (see ParserOptions)
    C4_ALWAYS_INLINE bool is_val_unfiltered(id_type node) const { _ryml_chkid(node); return m_type[node].is_val_unfiltered(); }

    RYML_DEPRECATED("use has_key_anchor()")    bool is_key_anchor(id_type node) const { _ryml_chkid(node); return m_type[node].has_key_anchor(); }
    RYML_DEPRECATED("use has_val_anchor()")    bool is_val_anchor(id_type node) const { _ryml_chkid(node); return m_type[node].has_val_anchor(); }
    RYML_DEPRECATED("use has_anchor()")        bool is_anchor(id_type node) const { _ryml_chkid(node); return m_type[node].has_anchor(); }
    RYML_DEPRECATED("use has_anchor_or_ref()") bool is_anchor_or_ref(id_type node) const { _ryml_chkid(node); return m_type[node].has_anchor() || m_type[node].is_ref(); }

    /** @} */

public:

    /** @name hierarchy predicates */
    /** @{ */

    bool is_root(id_type node) const { _ryml_chkid(node); _RYML_CB_ASSERT(m_callbacks, m_parent[node] != NONE || node == 0); return m_parent[node] == NONE; }

    bool has_parent(id_type node) const { _ryml_chkid(node); return m_parent[node] != NONE; }

    /** true when key and val are empty, and has no children */
    bool empty(id_type node) const
    {
        _ryml_chkid(node);
        const NodeType t = m_type[node];
        return !(((t & (SEQ|MAP)) && has_children(node)) || ((t & KEY) && !m_key[node].empty()) || ((t & VAL) && !m_val[node].empty()));
    }

    /** true if @p node has a child with id @p ch */
    bool has_child(id_type node, id_type ch) const { _ryml_chkid(node); return m_parent[ch] == node; }
    /** true if @p node has a child with key @p key */
    bool has_child(id_type node, csubstr key) const { _ryml_chkid(node); return find_child(node, key) != NONE; }
    /** true if @p node has any children key */
    bool has_children(id_type node) const { _ryml_chkid(node); return m_first_child[node] != NONE; }

    /** true if @p node has a sibling with id @p sib */
    bool has_sibling(id_type node, id_type sib) const { _ryml_chkid(node); return m_parent[node] == m_parent[sib]; }
    /** true if one of the node's siblings has the given key */
    bool has_sibling(id_type node, csubstr key) const { _ryml_chkid(node); return find_sibling(node, key) != NONE; }
    /** true if node is not a single child */
    bool has_other_siblings(id_type node) const
    {
        _ryml_chkid(node);
        const id_type parent = m_parent[node];
        if(C4_LIKELY(parent != NONE))
            return m_first_child[parent] != m_last_child[parent];
        return false;
    }

    RYML_DEPRECATED("use has_other_siblings()") bool has_siblings(id_type /*node*/) const { return true; }

    /** @} */

public:

    /** @name hierarchy getters */
    /** @{ */

    id_type parent(id_type node) const { _ryml_chkid(node); return m_parent[node]; }

    id_type prev_sibling(id_type node) const { _ryml_chkid(node); return m_prev_sibling[node]; }
    id_type next_sibling(id_type node) const { _ryml_chkid(node); return m_next_sibling[node]; }

    /** O(#num_children) */
    id_type num_children(id_type node) const;
    id_type child_pos(id_type node, id_type ch) const;
    id_type first_child(id_type node) const { _ryml_chkid(node); return m_first_child[node]; }
    id_type last_child(id_type node) const { _ryml_chkid(node); return m_last_child[node]; }
    id_type child(id_type node, id_type pos) const;
    id_type find_child(id_type node, csubstr const& key) const;

    /** O(#num_siblings) */
    /** counts with this */
    id_type num_siblings(id_type node) const { _ryml_chkid(node); return is_root(node) ? 1 : num_children(m_parent[node]); }
    /** does not count with this */
    id_type num_other_siblings(id_type node) const { _ryml_chkid(node); id_type ns = num_siblings(node); _RYML_CB_ASSERT(m_callbacks, ns > 0); return ns-1; }
    id_type sibling_pos(id_type node, id_type sib) const { _ryml_chkid(node); _ryml_chkid(sib); _RYML_CB_ASSERT(m_callbacks,  ! is_root(node) || node == root_id()); return child_pos(m_parent[node], sib); }
    id_type first_sibling(id_type node) const { _ryml_chkid(node); return is_root(node) ? node : m_first_child[m_parent[node]]; }
    id_type last_sibling(id_type node) const { _ryml_chkid(node); return is_root(node) ? node : m_last_child[m_parent[node]]; }
    id_type sibling(id_type node, id_type pos) const { _ryml_chkid(node); return child(m_parent[node], pos); }
    id_type find_sibling(id_type node, csubstr const& key) const { _ryml_chkid(node); return find_child(m_parent[node], key); }

    id_type doc(id_type i) const { id_type rid = root_id(); _RYML_CB_ASSERT(m_callbacks, is_stream(rid)); return child(rid, i); } //!< gets the @p i document node index. requires that the root node is a stream.

    id_type depth_asc(id_type node) const; /**< O(log(num_tree_nodes)) get the ascending depth of the node: number of levels between root and node */
    id_type depth_desc(id_type node) const; /**< O(num_tree_nodes) get the descending depth of the node: number of levels between node and deepest child */

    /** @} */

public:

    /** @name node style predicates and modifiers. see the corresponding predicate in NodeType */
    /** @{ */

    C4_ALWAYS_INLINE bool is_container_styled(id_type node) const { _ryml_chkid(node); return m_type[node].is_container_styled(); }
    C4_ALWAYS_INLINE bool is_block(id_type node) const { _ryml_chkid(node); return m_type[node].is_block(); }
    C4_ALWAYS_INLINE bool is_flow_sl(id_type node) const { _ryml_chkid(node); return m_type[node].is_flow_sl(); }
    C4_ALWAYS_INLINE bool is_flow_ml(id_type node) const { _ryml_chkid(node); return m_type[node].is_flow_ml(); }
    C4_ALWAYS_INLINE bool is_flow(id_type node) const { _ryml_chkid(node); return m_type[node].is_flow(); }

    C4_ALWAYS_INLINE bool is_key_styled(id_type node) const { _ryml_chkid(node); return m_type[node].is_key_styled(); }
    C4_ALWAYS_INLINE bool is_val_styled(id_type node) const { _ryml_chkid(node); return m_type[node].is_val_styled(); }
    C4_ALWAYS_INLINE bool is_key_literal(id_type node) const { _ryml_chkid(node); return m_type[node].is_key_literal(); }
    C4_ALWAYS_INLINE bool is_val_literal(id_type node) const { _ryml_chkid(node); return m_type[node].is_val_literal(); }
    C4_ALWAYS_INLINE bool is_key_folded(id_type node) const { _ryml_chkid(node); return m_type[node].is_key_folded(); }
    C4_ALWAYS_INLINE bool is_val_folded(id_type node) const { _ryml_chkid(node); return m_type[node].is_val_folded(); }
    C4_ALWAYS_INLINE bool is_key_squo(id_type node) const { _ryml_chkid(node); return m_type[node].is_key_squo(); }
    C4_ALWAYS_INLINE bool is_val_squo(id_type node) const { _ryml_chkid(node); return m_type[node].is_val_squo(); }
    C4_ALWAYS_INLINE bool is_key_dquo(id_type node) const { _ryml_chkid(node); return m_type[node].is_key_dquo(); }
    C4_ALWAYS_INLINE bool is_val_dquo(id_type node) const { _ryml_chkid(node); return m_type[node].is_val_dquo(); }
    C4_ALWAYS_INLINE bool is_key_plain(id_type node) const { _ryml_chkid(node); return m_type[node].is_key_plain(); }
    C4_ALWAYS_INLINE bool is_val_plain(id_type node) const { _ryml_chkid(node); return m_type[node].is_val_plain(); }
    C4_ALWAYS_INLINE bool is_key_quoted(id_type node) const { _ryml_chkid(node); return m_type[node].is_key_quoted(); }
    C4_ALWAYS_INLINE bool is_val_quoted(id_type node) const { _ryml_chkid(node); return m_type[node].is_val_quoted(); }
    C4_ALWAYS_INLINE bool is_quoted(id_type node) const { _ryml_chkid(node); return m_type[node].is_quoted(); }

    C4_ALWAYS_INLINE void set_container_style(id_type node, NodeType_e style) { _ryml_chkid(node); _RYML_CB_ASSERT(m_callbacks, is_container(node)); m_type[node].set_container_style(style); }
    C4_ALWAYS_INLINE void set_key_style(id_type node, NodeType_e style) { _ryml_chkid(node); _RYML_CB_ASSERT(m_callbacks, has_key(node)); m_type[node].set_key_style(style); }
    C4_ALWAYS_INLINE void set_val_style(id_type node, NodeType_e style) { _ryml_chkid(node); _RYML_CB_ASSERT(m_callbacks, has_val(node)); m_type[node].set_val_style(style); }

    /** @} */

public:

    /** @name node type modifiers */
    /** @{ */

    void to_keyval(id_type node, csubstr key, csubstr val, type_bits more_flags=0);
    void to_map(id_type node, csubstr key, type_bits more_flags=0);
    void to_seq(id_type node, csubstr key, type_bits more_flags=0);
    void to_val(id_type node, csubstr val, type_bits more_flags=0);
    void to_map(id_type node, type_bits more_flags=0);
    void to_seq(id_type node, type_bits more_flags=0);
    void to_doc(id_type node, type_bits more_flags=0);
    void to_stream(id_type node, type_bits more_flags=0);

    void set_key(id_type node, csubstr key) { _ryml_chkid(node); _RYML_CB_ASSERT(m_callbacks, has_key(node)); m_key[node] = key; }
    void set_val(id_type node, csubstr val) { _ryml_chkid(node); _RYML_CB_ASSERT(m_callbacks, has_val(node)); m_val[node] = val; }

    void set_key_tag(id_type node, csubstr tag) { _ryml_chkid(node); _RYML_CB_ASSERT(m_callbacks, has_key(node)); m_key_tag[node] = tag; _add_flags(node, KEYTAG); }
    void set_val_tag(id_type node, csubstr tag) { _ryml_chkid(node); _RYML_CB_ASSERT(m_callbacks, has_val(node) || is_container(node)); m_val_tag[node] = tag; _add_flags(node, VALTAG); }

    void set_key_anchor(id_type node, csubstr anchor) { _ryml_chkid(node); _RYML_CB_ASSERT(m_callbacks,  ! is_key_ref(node)); m_key_anchor[node] = anchor.triml('&'); _add_flags(node, KEYANCH); }
    void set_val_anchor(id_type node, csubstr anchor) { _ryml_chkid(node); _RYML_CB_ASSERT(m_callbacks,  ! is_val_ref(node)); m_val_anchor[node] = anchor.triml('&'); _add_flags(node, VALANCH); }
    void set_key_ref(id_type node, csubstr ref)
    {
        _ryml_chkid(node);
        _RYML_CB_ASSERT(m_callbacks, ! has_key_anchor(node));
        csubstr trimmed = ref.begins_with('*') ? ref.sub(1) : ref;
        m_key_anchor[node] = trimmed;
        if((!m_type[node].has_key()) || !m_key[node].ends_with(trimmed))
            m_key[node] = ref;
        _add_flags(node, KEY|KEYREF);
    }
    void set_val_ref(id_type node, csubstr ref)
    {
        _ryml_chkid(node);
        _RYML_CB_ASSERT(m_callbacks, ! has_val_anchor(node));
        csubstr trimmed = ref.begins_with('*') ? ref.sub(1) : ref;
        m_val_anchor[node] = trimmed;
        if((!m_type[node].has_val()) || !m_val[node].ends_with(trimmed))
            m_val[node] = ref;
        _add_flags(node, VAL|VALREF);
    }

    void rem_key_anchor(id_type node) { _ryml_chkid(node); m_key_anchor[node].clear(); _rem_flags(node, KEYANCH); }
    void rem_val_anchor(id_type node) { _ryml_chkid(node); m_val_anchor[node].clear(); _rem_flags(node, VALANCH); }
    void rem_key_ref   (id_type node) { _ryml_chkid(node); m_key_anchor[node].clear(); _rem_flags(node, KEYREF); }
    void rem_val_ref   (id_type node) { _ryml_chkid(node); m_val_anchor[node].clear(); _rem_flags(node, VALREF); }
    void rem_key_tag   (id_type node) { _ryml_chkid(node); m_key_anchor[node].clear(); _rem_flags(node, KEYTAG); }
    void rem_val_tag   (id_type node) { _ryml_chkid(node); m_val_anchor[node].clear(); _rem_flags(node, VALTAG); }
    void rem_anchor_ref(id_type node) { _ryml_chkid(node); m_key_anchor[node].clear(); m_val_anchor[node].clear(); _rem_flags(node, KEYANCH|VALANCH|KEYREF|VALREF); }

    /** @} */

public:

    /** @name tree modifiers */
    /** @{ */

    /** reorder the tree in memory so that all the nodes are stored
     * in a linear sequence when visited in depth-first order.
     * This will invalidate existing ids, since the node id is its
     * position in the tree's node array. */
    void reorder();

    /** Resolve references (aliases <- anchors) in the tree.
     *
     * Dereferencing is opt-in; after parsing, Tree::resolve() has to
     * be called explicitly for obtaining resolved references in the
     * tree. This method will @ref ReferenceResolver::resolve()
     * to resolve all references and substitute the anchored values in
     * place of the reference.
     *
     * This method first does a full traversal of the tree to gather all
     * anchors and references in a separate collection, then it goes through
     * that collection to locate the names, which it does by obeying the YAML
     * standard diktat that "an alias node refers to the most recent node in
     * the serialization having the specified anchor"
     *
     * So, depending on the number of anchor/alias nodes, this is a
     * potentially expensive operation, with a best-case linear complexity
     * (from the initial traversal). This potential cost is the reason for
     * requiring an explicit call.
     *
     * @see ReferenceResolver::resolve()
     */
    void resolve(ReferenceResolver *C4_RESTRICT rr);

    /** Resolve references using a throw-away resolver. */
    void resolve();

    /** @} */

public:

    /** @name tag directives */
    /** @{ */

    void resolve_tags();
    void normalize_tags();
    void normalize_tags_long();

    id_type num_tag_directives() const;
    bool add_tag_directive(csubstr directive);
    id_type add_tag_directive(TagDirective const& td);
    void clear_tag_directives();

    /** resolve the given tag, appearing at node_id. Write the result into output.
     * @return the number of characters required for the resolved tag */
    size_t resolve_tag(substr output, csubstr tag, id_type node_id) const;
    csubstr resolve_tag_sub(substr output, csubstr tag, id_type node_id) const
    {
        size_t needed = resolve_tag(output, tag, node_id);
        return needed <= output.len ? output.first(needed) : output;
    }

    TagDirective const* begin_tag_directives() const { return m_tag_directives; }
    TagDirective const* end_tag_directives() const { return m_tag_directives + num_tag_directives(); }
    c4::yml::TagDirectiveRange tag_directives() const { return c4::yml::TagDirectiveRange{begin_tag_directives(), end_tag_directives()}; }

    RYML_DEPRECATED("use c4::yml::tag_directive_const_iterator") typedef TagDirective const* tag_directive_const_iterator;
    RYML_DEPRECATED("use c4::yml::TagDirectiveRange") typedef c4::yml::TagDirectiveRange TagDirectiveProxy;

    /** @} */

public:

    /** @name modifying hierarchy */
    /** @{ */

    /** create and insert a new child of @p parent. insert after the (to-be)
     * sibling @p after, which must be a child of @p parent. To insert as the
     * first child, set after to NONE */
    C4_ALWAYS_INLINE id_type insert_child(id_type parent, id_type after)
    {
        _RYML_CB_ASSERT(m_callbacks, parent != NONE);
        _RYML_CB_ASSERT(m_callbacks, is_container(parent) || is_root(parent));
        _RYML_CB_ASSERT(m_callbacks, after == NONE || (m_parent[after] == parent));
        id_type child = _claim();
        _set_hierarchy(child, parent, after);
        return child;
    }
    /** create and insert a node as the first child of @p parent */
    C4_ALWAYS_INLINE id_type prepend_child(id_type parent) { return insert_child(parent, NONE); }
    /** create and insert a node as the last child of @p parent */
    C4_ALWAYS_INLINE id_type append_child(id_type parent) { return insert_child(parent, m_last_child[parent]); }
    C4_ALWAYS_INLINE id_type _append_child__unprotected(id_type parent)
    {
        id_type child = _claim();
        _set_hierarchy(child, parent, m_last_child[parent]);
        return child;
    }

public:

    //! create and insert a new sibling of n. insert after "after"
    C4_ALWAYS_INLINE id_type insert_sibling(id_type node, id_type after)
    {
        return insert_child(m_parent[node], after);
    }
    /** create and insert a node as the first node of @p parent */
    C4_ALWAYS_INLINE id_type prepend_sibling(id_type node) { return prepend_child(m_parent[node]); }
    C4_ALWAYS_INLINE id_type  append_sibling(id_type node) { return append_child(m_parent[node]); }

public:

    /** remove an entire branch at once: ie remove the children and the node itself */
    inline void remove(id_type node)
    {
        remove_children(node);
        _release(node);
    }

    /** remove all the node's children, but keep the node itself */
    void remove_children(id_type node);

    /** change the @p type of the node to one of MAP, SEQ or VAL.  @p
     * type must have one and only one of MAP,SEQ,VAL; @p type may
     * possibly have KEY, but if it does, then the @p node must also
     * have KEY. Changing to the same type is a no-op. Otherwise,
     * changing to a different type will initialize the node with an
     * empty value of the desired type: changing to VAL will
     * initialize with a null scalar (~), changing to MAP will
     * initialize with an empty map ({}), and changing to SEQ will
     * initialize with an empty seq ([]).
     * @return true when the type changed, false otherwise */
    bool change_type(id_type node, NodeType type);
    bool change_type(id_type node, type_bits type)
    {
        return change_type(node, (NodeType)type);
    }

public:

    /** change the node's position in the parent */
    void move(id_type node, id_type after);

    /** change the node's parent and position */
    void move(id_type node, id_type new_parent, id_type after);

    /** change the node's parent and position to a different tree
     * @return the index of the new node in the destination tree */
    id_type move(Tree * src, id_type node, id_type new_parent, id_type after);

    /** ensure the first node is a stream. Eg, change this tree
     *
     *  DOCMAP
     *    MAP
     *      KEYVAL
     *      KEYVAL
     *    SEQ
     *      VAL
     *
     * to
     *
     *  STREAM
     *    DOCMAP
     *      MAP
     *        KEYVAL
     *        KEYVAL
     *      SEQ
     *        VAL
     *
     * If the root is already a stream, this is a no-op.
     */
    void set_root_as_stream();

public:

    /** recursively duplicate a node from this tree into a new parent,
     * placing it after one of its children
     * @return the index of the copy */
    id_type duplicate(id_type node, id_type new_parent, id_type after);
    /** recursively duplicate a node from a different tree into a new parent,
     * placing it after one of its children
     * @return the index of the copy */
    id_type duplicate(Tree const* src, id_type node, id_type new_parent, id_type after);

    /** recursively duplicate the node's children (but not the node)
     * @return the index of the last duplicated child */
    id_type duplicate_children(id_type node, id_type parent, id_type after);
    /** recursively duplicate the node's children (but not the node), where
     * the node is from a different tree
     * @return the index of the last duplicated child */
    id_type duplicate_children(Tree const* src, id_type node, id_type parent, id_type after);

    void duplicate_contents(id_type node, id_type where);
    void duplicate_contents(Tree const* src, id_type node, id_type where);

    /** duplicate the node's children (but not the node) in a new parent, but
     * omit repetitions where a duplicated node has the same key (in maps) or
     * value (in seqs). If one of the duplicated children has the same key
     * (in maps) or value (in seqs) as one of the parent's children, the one
     * that is placed closest to the end will prevail. */
    id_type duplicate_children_no_rep(id_type node, id_type parent, id_type after);
    id_type duplicate_children_no_rep(Tree const* src, id_type node, id_type parent, id_type after);

public:

    void merge_with(Tree const* src, id_type src_node=NONE, id_type dst_root=NONE);

    /** @} */

public:

    /** @name internal string arena */
    /** @{ */

    /** get the current size of the tree's internal arena */
    RYML_DEPRECATED("use arena_size() instead") size_t arena_pos() const { return m_arena_pos; }
    /** get the current size of the tree's internal arena */
    inline size_t arena_size() const { return m_arena_pos; }
    /** get the current capacity of the tree's internal arena */
    inline size_t arena_capacity() const { return m_arena.len; }
    /** get the current slack of the tree's internal arena */
    inline size_t arena_slack() const { _RYML_CB_ASSERT(m_callbacks, m_arena.len >= m_arena_pos); return m_arena.len - m_arena_pos; }

    /** get the current arena */
    csubstr arena() const { return m_arena.first(m_arena_pos); }
    /** get the current arena */
    substr arena() { return m_arena.first(m_arena_pos); }

    /** return true if the given substring is part of the tree's string arena */
    bool in_arena(csubstr s) const
    {
        return m_arena.is_super(s);
    }

    /** serialize the given floating-point variable to the tree's
     * arena, growing it as needed to accomodate the serialization.
     *
     * @note Growing the arena may cause relocation of the entire
     * existing arena, and thus change the contents of individual
     * nodes, and thus cost O(numnodes)+O(arenasize). To avoid this
     * cost, ensure that the arena is reserved to an appropriate size
     * using .reserve_arena()
     *
     * @see alloc_arena() */
    template<class T>
    typename std::enable_if<std::is_floating_point<T>::value, csubstr>::type
    to_arena(T const& C4_RESTRICT a)
    {
        substr rem(m_arena.sub(m_arena_pos));
        size_t num = to_chars_float(rem, a);
        if(num > rem.len)
        {
            rem = _grow_arena(num);
            num = to_chars_float(rem, a);
            _RYML_CB_ASSERT(m_callbacks, num <= rem.len);
        }
        rem = _request_span(num);
        return rem;
    }

    /** serialize the given non-floating-point variable to the tree's
     * arena, growing it as needed to accomodate the serialization.
     *
     * @note Growing the arena may cause relocation of the entire
     * existing arena, and thus change the contents of individual
     * nodes, and thus cost O(numnodes)+O(arenasize). To avoid this
     * cost, ensure that the arena is reserved to an appropriate size
     * using .reserve_arena()
     *
     * @see alloc_arena() */
    template<class T>
    typename std::enable_if<!std::is_floating_point<T>::value, csubstr>::type
    to_arena(T const& C4_RESTRICT a)
    {
        substr rem(m_arena.sub(m_arena_pos));
        size_t num = to_chars(rem, a);
        if(num > rem.len)
        {
            rem = _grow_arena(num);
            num = to_chars(rem, a);
            _RYML_CB_ASSERT(m_callbacks, num <= rem.len);
        }
        rem = _request_span(num);
        return rem;
    }

    /** serialize the given csubstr to the tree's arena, growing the
     * arena as needed to accomodate the serialization.
     *
     * @note Growing the arena may cause relocation of the entire
     * existing arena, and thus change the contents of individual
     * nodes, and thus cost O(numnodes)+O(arenasize). To avoid this
     * cost, ensure that the arena is reserved to an appropriate size
     * using .reserve_arena()
     *
     * @see alloc_arena() */
    csubstr to_arena(csubstr a)
    {
        if(a.len > 0)
        {
            substr rem(m_arena.sub(m_arena_pos));
            size_t num = to_chars(rem, a);
            if(num > rem.len)
            {
                rem = _grow_arena(num);
                num = to_chars(rem, a);
                _RYML_CB_ASSERT(m_callbacks, num <= rem.len);
            }
            return _request_span(num);
        }
        else
        {
            if(a.str == nullptr)
            {
                return csubstr{};
            }
            else if(m_arena.str == nullptr)
            {
                // Arena is empty and we want to store a non-null
                // zero-length string.
                // Even though the string has zero length, we need
                // some "memory" to store a non-nullptr string
                _grow_arena(1);
            }
            return _request_span(0);
        }
    }
    C4_ALWAYS_INLINE csubstr to_arena(const char *s)
    {
        return to_arena(to_csubstr(s));
    }
    C4_ALWAYS_INLINE csubstr to_arena(std::nullptr_t)
    {
        return csubstr{};
    }

    /** copy the given substr to the tree's arena, growing it by the
     * required size
     *
     * @note Growing the arena may cause relocation of the entire
     * existing arena, and thus change the contents of individual
     * nodes, and thus cost O(numnodes)+O(arenasize). To avoid this
     * cost, ensure that the arena is reserved to an appropriate size
     * using .reserve_arena()
     *
     * @see alloc_arena() */
    substr copy_to_arena(csubstr s)
    {
        substr cp = alloc_arena(s.len);
        _RYML_CB_ASSERT(m_callbacks, cp.len == s.len);
        _RYML_CB_ASSERT(m_callbacks, !s.overlaps(cp));
        #if (!defined(__clang__)) && (defined(__GNUC__) && __GNUC__ >= 10)
        C4_SUPPRESS_WARNING_GCC_PUSH
        C4_SUPPRESS_WARNING_GCC("-Wstringop-overflow=") // no need for terminating \0
        C4_SUPPRESS_WARNING_GCC( "-Wrestrict") // there's an assert to ensure no violation of restrict behavior
        #endif
        if(s.len)
            memcpy(cp.str, s.str, s.len);
        #if (!defined(__clang__)) && (defined(__GNUC__) && __GNUC__ >= 10)
        C4_SUPPRESS_WARNING_GCC_POP
        #endif
        return cp;
    }

    /** grow the tree's string arena by the given size and return a substr
     * of the added portion
     *
     * @note Growing the arena may cause relocation of the entire
     * existing arena, and thus change the contents of individual
     * nodes, and thus cost O(numnodes)+O(arenasize). To avoid this
     * cost, ensure that the arena is reserved to an appropriate size
     * using .reserve_arena().
     *
     * @see reserve_arena() */
    substr alloc_arena(size_t sz)
    {
        if(sz > arena_slack())
            _grow_arena(sz - arena_slack());
        substr s = _request_span(sz);
        return s;
    }

    /** ensure the tree's internal string arena is at least the given capacity
     * @warning This operation may be expensive, with a potential complexity of O(numNodes)+O(arenasize).
     * @warning Growing the arena may cause relocation of the entire
     * existing arena, and thus change the contents of individual nodes. */
    void reserve_arena(size_t arena_cap)
    {
        if(arena_cap > m_arena.len)
        {
            substr buf;
            buf.str = (char*) m_callbacks.m_allocate(arena_cap, m_arena.str, m_callbacks.m_user_data);
            buf.len = arena_cap;
            if(m_arena.str)
            {
                _RYML_CB_ASSERT(m_callbacks, m_arena.len >= 0);
                _relocate(buf); // does a memcpy and changes nodes using the arena
                m_callbacks.m_free(m_arena.str, m_arena.len, m_callbacks.m_user_data);
            }
            m_arena = buf;
        }
    }

    /** @} */

private:

    substr _grow_arena(size_t more)
    {
        size_t cap = m_arena.len + more;
        cap = cap < 2 * m_arena.len ? 2 * m_arena.len : cap;
        cap = cap < 64 ? 64 : cap;
        reserve_arena(cap);
        return m_arena.sub(m_arena_pos);
    }

    substr _request_span(size_t sz)
    {
        _RYML_CB_ASSERT(m_callbacks, m_arena_pos + sz <= m_arena.len);
        substr s;
        s = m_arena.sub(m_arena_pos, sz);
        m_arena_pos += sz;
        return s;
    }

    substr _relocated(csubstr s, substr next_arena) const
    {
        _RYML_CB_ASSERT(m_callbacks, m_arena.is_super(s));
        _RYML_CB_ASSERT(m_callbacks, m_arena.sub(0, m_arena_pos).is_super(s));
        auto pos = (s.str - m_arena.str); // this is larger than 0 based on the assertions above
        substr r(next_arena.str + pos, s.len);
        _RYML_CB_ASSERT(m_callbacks, r.str - next_arena.str == pos);
        _RYML_CB_ASSERT(m_callbacks, next_arena.sub(0, m_arena_pos).is_super(r));
        return r;
    }

public:

    /** @name lookup */
    /** @{ */

    struct lookup_result
    {
        id_type  target;
        id_type  closest;
        size_t  path_pos;
        csubstr path;

        inline operator bool() const { return target != NONE; }

        lookup_result() : target(NONE), closest(NONE), path_pos(0), path() {}
        lookup_result(csubstr path_, id_type start) : target(NONE), closest(start), path_pos(0), path(path_) {}

        /** get the part ot the input path that was resolved */
        csubstr resolved() const;
        /** get the part ot the input path that was unresolved */
        csubstr unresolved() const;
    };

    /** for example foo.bar[0].baz */
    lookup_result lookup_path(csubstr path, id_type start=NONE) const;

    /** defaulted lookup: lookup @p path; if the lookup fails, recursively modify
     * the tree so that the corresponding lookup_path() would return the
     * default value.
     * @see lookup_path() */
    id_type lookup_path_or_modify(csubstr default_value, csubstr path, id_type start=NONE);

    /** defaulted lookup: lookup @p path; if the lookup fails, recursively modify
     * the tree so that the corresponding lookup_path() would return the
     * branch @p src_node (from the tree @p src).
     * @see lookup_path() */
    id_type lookup_path_or_modify(Tree const *src, id_type src_node, csubstr path, id_type start=NONE);

    /** @} */

private:

    struct _lookup_path_token
    {
        csubstr value;
        NodeType type;
        _lookup_path_token() : value(), type() {}
        _lookup_path_token(csubstr v, NodeType t) : value(v), type(t) {}
        inline operator bool() const { return type != NOTYPE; }
        bool is_index() const { return value.begins_with('[') && value.ends_with(']'); }
    };

    id_type _lookup_path_or_create(csubstr path, id_type start);

    void   _lookup_path       (lookup_result *r) const;
    void   _lookup_path_modify(lookup_result *r);

    id_type _next_node       (lookup_result *r, _lookup_path_token *parent) const;
    id_type _next_node_modify(lookup_result *r, _lookup_path_token *parent);

    void   _advance(lookup_result *r, size_t more) const;

    _lookup_path_token _next_token(lookup_result *r, _lookup_path_token const& parent) const;

private:

    void _clear();
    void _free();
    void _copy(Tree const& that);
    void _move(Tree      & that) noexcept;

    void _relocate(substr next_arena);

public:

    /** @cond dev*/

    #if ! RYML_USE_ASSERT
    C4_ALWAYS_INLINE void _check_next_flags(id_type, type_bits) {}
    #else
    void _check_next_flags(id_type node, type_bits f)
    {
        const type_bits o = m_type[node]; // old
        if(f & MAP)
        {
            RYML_ASSERT((f & SEQ) == 0); // cannot mark simultaneously as map and seq
            RYML_ASSERT((f & VAL) == 0); // cannot mark simultaneously as map and val
            RYML_ASSERT((o & SEQ) == 0); // cannot turn a seq into a map; clear first
            RYML_ASSERT((o & VAL) == 0); // cannot turn a val into a map; clear first
        }
        else if(f & SEQ)
        {
            RYML_ASSERT((f & MAP) == 0); // cannot mark simultaneously as seq and map
            RYML_ASSERT((f & VAL) == 0); // cannot mark simultaneously as seq and val
            RYML_ASSERT((o & MAP) == 0); // cannot turn a map into a seq; clear first
            RYML_ASSERT((o & VAL) == 0); // cannot turn a val into a seq; clear first
        }
        if(f & KEY)
        {
            _RYML_CB_ASSERT(m_callbacks, !is_root(node));
            const id_type pid = parent(node);
            _RYML_CB_ASSERT(m_callbacks, is_map(pid));
        }
        if((f & VAL) && !is_root(node))
        {
            const id_type pid = parent(node);
            _RYML_CB_ASSERT(m_callbacks, is_map(pid) || is_seq(pid));
        }
    }
    #endif

    C4_ALWAYS_INLINE void _set_flags(id_type node, NodeType_e f) { _check_next_flags(node, f); m_type[node] = f; }
    C4_ALWAYS_INLINE void _set_flags(id_type node, type_bits  f) { _check_next_flags(node, f); m_type[node] = f; }

    C4_ALWAYS_INLINE void _add_flags(id_type node, NodeType_e f) { type_bits fb = f |  m_type[node]; _check_next_flags(node, fb); m_type[node] = (NodeType_e) fb; }
    C4_ALWAYS_INLINE void _add_flags(id_type node, type_bits  f) {                f |= m_type[node]; _check_next_flags(node,  f); m_type[node] = f; }

    C4_ALWAYS_INLINE void _rem_flags(id_type node, NodeType_e f) { type_bits fb = m_type[node] & ~f; _check_next_flags(node, fb); m_type[node] = (NodeType_e) fb; }
    C4_ALWAYS_INLINE void _rem_flags(id_type node, type_bits  f) {            f = m_type[node] & ~f; _check_next_flags(node,  f); m_type[node] = f; }

    void _set_key(id_type node, csubstr key, type_bits more_flags=0)
    {
        m_key[node] = key;
        _add_flags(node, KEY|more_flags);
    }
    void _set_key(id_type node, NodeScalar const& key, type_bits more_flags=0)
    {
        m_key[node] = key.scalar;
        m_key_tag[node] = key.tag;
        m_key_anchor[node] = key.anchor;
        _add_flags(node, KEY|more_flags);
    }

    void _set_val(id_type node, csubstr val, type_bits more_flags=0)
    {
        _RYML_CB_ASSERT(m_callbacks, num_children(node) == 0);
        _RYML_CB_ASSERT(m_callbacks, !is_seq(node) && !is_map(node));
        m_val[node] = val;
        _add_flags(node, VAL|more_flags);
    }
    void _set_val(id_type node, NodeScalar const& val, type_bits more_flags=0)
    {
        _RYML_CB_ASSERT(m_callbacks, num_children(node) == 0);
        _RYML_CB_ASSERT(m_callbacks,  ! is_container(node));
        m_val[node] = val.scalar;
        m_val_tag[node] = val.tag;
        m_val_anchor[node] = val.anchor;
        _add_flags(node, VAL|more_flags);
    }

    void _set(id_type node, NodeInit const& i)
    {
        _RYML_CB_ASSERT(m_callbacks, i._check());
        _RYML_CB_ASSERT(m_callbacks, !has_key(node) || i.key.scalar.empty() || i.key.scalar == m_key[node]);
        if(!m_type[node].has_key())
        {
            if( ! i.key.scalar.empty())
            {
                _set_key(node, i.key.scalar);
            }
        }
        m_key_tag[node] = i.key.tag;
        m_key_anchor[node] = i.key.anchor;
        m_val[node] = i.val.scalar;
        m_val_tag[node] = i.val.tag;
        m_val_anchor[node] = i.val.anchor;
        _add_flags(node, i.type);
    }

    void _set_parent_as_container_if_needed(id_type node)
    {
        id_type parent = m_parent[node];
        if(parent != NONE)
        {
            if( ! (is_seq(parent) || is_map(parent)))
            {
                if((node == first_child(parent)) && (node == last_child(parent)))
                {
                    if(has_key(node))
                    {
                        _add_flags(parent, MAP);
                    }
                    else
                    {
                        _add_flags(parent, SEQ);
                    }
                }
            }
        }
    }

    void _seq2map(id_type node)
    {
        _RYML_CB_ASSERT(m_callbacks, is_seq(node));
        for(id_type i = first_child(node); i != NONE; i = next_sibling(i))
        {
            if(m_type[i].is_keyval())
                continue;
            m_type[i].add(KEY);
            m_key[i] = m_val[i];
        }
        m_type[node].rem(SEQ);
        m_type[node].add(MAP);
    }

    id_type _do_reorder(id_type *node, id_type count);

    void _swap(id_type n_, id_type m_);
    void _swap_props(id_type n_, id_type m_);
    void _swap_hierarchy(id_type n_, id_type m_);
    void _copy_hierarchy(id_type dst_, id_type src_);

    inline void _copy_props(id_type dst_, id_type src_)
    {
        _copy_props(dst_, this, src_);
    }

    inline void _copy_props_wo_key(id_type dst_, id_type src_)
    {
        _copy_props_wo_key(dst_, this, src_);
    }

    void _copy_props(id_type dst_, Tree const* that_tree, id_type src_)
    {
        m_type[dst_] = that_tree->m_type[src_];
        m_key[dst_]  = that_tree->m_key[src_];
        m_key_tag[dst_]  = that_tree->m_key_tag[src_];
        m_key_anchor[dst_]  = that_tree->m_key_anchor[src_];
        m_val[dst_]  = that_tree->m_val[src_];
        m_val_tag[dst_]  = that_tree->m_val_tag[src_];
        m_val_anchor[dst_]  = that_tree->m_val_anchor[src_];
    }

    void _copy_props(id_type dst_, Tree const* that_tree, id_type src_, type_bits src_mask)
    {
        m_type[dst_] = (that_tree->m_type[src_] & src_mask) | (m_type[dst_] & ~src_mask);
        m_key[dst_]  = that_tree->m_key[src_];
        m_key_tag[dst_]  = that_tree->m_key_tag[src_];
        m_key_anchor[dst_]  = that_tree->m_key_anchor[src_];
        m_val[dst_]  = that_tree->m_val[src_];
        m_val_tag[dst_]  = that_tree->m_val_tag[src_];
        m_val_anchor[dst_]  = that_tree->m_val_anchor[src_];
    }

    void _copy_props_wo_key(id_type dst_, Tree const* that_tree, id_type src_)
    {
        m_type[dst_] = (that_tree->m_type[src_] & ~_KEYMASK) | (m_type[dst_] & _KEYMASK);
        m_val[dst_]  = that_tree->m_val[src_];
        m_val_tag[dst_]  = that_tree->m_val_tag[src_];
        m_val_anchor[dst_]  = that_tree->m_val_anchor[src_];
    }

    void _copy_props_wo_key(id_type dst_, Tree const* that_tree, id_type src_, type_bits src_mask)
    {
        m_type[dst_] = (that_tree->m_type[src_] & ((~_KEYMASK)|src_mask)) | (m_type[dst_] & (_KEYMASK|~src_mask));
        m_val[dst_]  = that_tree->m_val[src_];
        m_val_tag[dst_]  = that_tree->m_val_tag[src_];
        m_val_anchor[dst_]  = that_tree->m_val_anchor[src_];
    }

    inline void _clear_type(id_type node)
    {
        m_type[node] = NOTYPE;
    }

    inline void _clear(id_type node)
    {
        m_type[node] = NOTYPE;
        //m_key[node].clear();
        //m_key_tag[node].clear();
        //m_key_anchor[node].clear();
        //m_val[node].clear();
        //m_val_tag[node].clear();
        //m_val_anchor[node].clear();
        m_parent[node] = NONE;
        m_first_child[node] = NONE;
        m_last_child[node] = NONE;
    }

    inline void _clear_key(id_type node)
    {
        //m_key[node].clear();
        //m_key_tag[node].clear();
        //m_key_anchor[node].clear();
        _rem_flags(node, KEY|KEYTAG|KEYREF|KEYANCH);
    }

    inline void _clear_val(id_type node)
    {
        //m_val[node].clear();
        //m_val_tag[node].clear();
        //m_val_anchor[node].clear();
        _rem_flags(node, VAL|VALTAG|VALREF|VALANCH);
    }

    /** @endcond */

private:

    void _clear_range(id_type first, id_type num);

public:
    id_type _claim();
private:
    void   _claim_root();
    void   _release(id_type node);
    void   _free_list_add(id_type node);
    void   _free_list_rem(id_type node);

    void _set_hierarchy(id_type node, id_type parent, id_type after_sibling);
    void _rem_hierarchy(id_type node);

public:

    // members are exposed, but you should NOT access them directly,
    // and you should NEVER modify them

    // formerly NodeData
    NodeType     *C4_RESTRICT m_type;

    csubstr      *C4_RESTRICT m_val;
    csubstr      *C4_RESTRICT m_val_tag;
    csubstr      *C4_RESTRICT m_val_anchor;
    csubstr      *C4_RESTRICT m_key;
    csubstr      *C4_RESTRICT m_key_tag;
    csubstr      *C4_RESTRICT m_key_anchor;

    id_type      *C4_RESTRICT m_parent;
    id_type      *C4_RESTRICT m_first_child;
    id_type      *C4_RESTRICT m_last_child;
    id_type      *C4_RESTRICT m_prev_sibling;
    id_type      *C4_RESTRICT m_next_sibling;

    id_type m_cap;
    id_type m_size;

    id_type m_free_head;
    id_type m_free_tail;

    substr m_arena;
    size_t m_arena_pos;

    Callbacks m_callbacks;

    TagDirective m_tag_directives[RYML_MAX_TAG_DIRECTIVES];

};

/** @} */

} // namespace yml
} // namespace c4


C4_SUPPRESS_WARNING_MSVC_POP
C4_SUPPRESS_WARNING_GCC_CLANG_POP


#endif /* _C4_YML_TREE_HPP_ */
