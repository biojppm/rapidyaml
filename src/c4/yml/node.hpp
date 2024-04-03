#ifndef _C4_YML_NODE_HPP_
#define _C4_YML_NODE_HPP_

/** @file node.hpp
 * @see NodeRef */

#include <cstddef>

#include "c4/yml/tree.hpp"
#include "c4/base64.hpp"

#ifdef __clang__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wtype-limits"
#   pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wtype-limits"
#   pragma GCC diagnostic ignored "-Wold-style-cast"
#elif defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable: 4251/*needs to have dll-interface to be used by clients of struct*/)
#   pragma warning(disable: 4296/*expression is always 'boolean_value'*/)
#endif

namespace c4 {
namespace yml {

template<class K> struct Key { K & k; };
template<> struct Key<fmt::const_base64_wrapper> { fmt::const_base64_wrapper wrapper; };
template<> struct Key<fmt::base64_wrapper> { fmt::base64_wrapper wrapper; };

template<class K> C4_ALWAYS_INLINE Key<K> key(K & k) { return Key<K>{k}; }
C4_ALWAYS_INLINE Key<fmt::const_base64_wrapper> key(fmt::const_base64_wrapper w) { return {w}; }
C4_ALWAYS_INLINE Key<fmt::base64_wrapper> key(fmt::base64_wrapper w) { return {w}; }

template<class T> void write(NodeRef *n, T const& v);

template<class T>
typename std::enable_if< ! std::is_floating_point<T>::value, bool>::type
read(NodeRef const& n, T *v);

template<class T>
typename std::enable_if<   std::is_floating_point<T>::value, bool>::type
read(NodeRef const& n, T *v);


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// forward decls
class NodeRef;
class ConstNodeRef;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

namespace detail {

template<class NodeRefType>
struct child_iterator
{
    using value_type = NodeRefType;
    using tree_type = typename NodeRefType::tree_type;

    tree_type * C4_RESTRICT m_tree;
    size_t m_child_id;

    child_iterator(tree_type * t, size_t id) : m_tree(t), m_child_id(id) {}

    child_iterator& operator++ () { RYML_ASSERT(m_child_id != NONE); m_child_id = m_tree->next_sibling(m_child_id); return *this; }
    child_iterator& operator-- () { RYML_ASSERT(m_child_id != NONE); m_child_id = m_tree->prev_sibling(m_child_id); return *this; }

    NodeRefType operator*  () const { return NodeRefType(m_tree, m_child_id); }
    NodeRefType operator-> () const { return NodeRefType(m_tree, m_child_id); }

    bool operator!= (child_iterator that) const { RYML_ASSERT(m_tree == that.m_tree); return m_child_id != that.m_child_id; }
    bool operator== (child_iterator that) const { RYML_ASSERT(m_tree == that.m_tree); return m_child_id == that.m_child_id; }
};

template<class NodeRefType>
struct children_view_
{
    using n_iterator = child_iterator<NodeRefType>;

    n_iterator b, e;

    inline children_view_(n_iterator const& C4_RESTRICT b_,
                          n_iterator const& C4_RESTRICT e_) : b(b_), e(e_) {}

    inline n_iterator begin() const { return b; }
    inline n_iterator end  () const { return e; }
};

template<class NodeRefType, class Visitor>
bool _visit(NodeRefType &node, Visitor fn, size_t indentation_level, bool skip_root=false)
{
    size_t increment = 0;
    if( ! (node.is_root() && skip_root))
    {
        if(fn(node, indentation_level))
            return true;
        ++increment;
    }
    if(node.has_children())
    {
        for(auto ch : node.children())
        {
            if(_visit(ch, fn, indentation_level + increment, false)) // no need to forward skip_root as it won't be root
            {
                return true;
            }
        }
    }
    return false;
}

template<class NodeRefType, class Visitor>
bool _visit_stacked(NodeRefType &node, Visitor fn, size_t indentation_level, bool skip_root=false)
{
    size_t increment = 0;
    if( ! (node.is_root() && skip_root))
    {
        if(fn(node, indentation_level))
        {
            return true;
        }
        ++increment;
    }
    if(node.has_children())
    {
        fn.push(node, indentation_level);
        for(auto ch : node.children())
        {
            if(_visit_stacked(ch, fn, indentation_level + increment, false)) // no need to forward skip_root as it won't be root
            {
                fn.pop(node, indentation_level);
                return true;
            }
        }
        fn.pop(node, indentation_level);
    }
    return false;
}


//-----------------------------------------------------------------------------

/** a CRTP base for read-only node methods */
template<class Impl, class ConstImpl>
struct RoNodeMethods
{
    C4_SUPPRESS_WARNING_GCC_CLANG_WITH_PUSH("-Wcast-align")
    // helper CRTP macros, undefined at the end
    #define tree_ ((ConstImpl const* C4_RESTRICT)this)->m_tree
    #define id_ ((ConstImpl const* C4_RESTRICT)this)->m_id
    #define tree__ ((Impl const* C4_RESTRICT)this)->m_tree
    #define id__ ((Impl const* C4_RESTRICT)this)->m_id
    // require readable: this is a precondition for reading from the
    // tree using this object.
    #define _C4RV()                                       \
        RYML_ASSERT(tree_ != nullptr);                    \
        _RYML_CB_ASSERT(tree_->m_callbacks, id_ != NONE); \
        _RYML_CB_ASSERT(tree_->m_callbacks, (!(((Impl const* C4_RESTRICT)this)->is_seed())));
    #define _C4_IF_MUTABLE(ty) typename std::enable_if<!std::is_same<U, ConstImpl>::value, ty>::type

public:

    /** @name node property getters */
    /** @{ */

    /** returns the data or null when the id is NONE */
    C4_ALWAYS_INLINE NodeData const* get() const RYML_NOEXCEPT { return ((Impl const*)this)->readable() ? tree_->get(id_) : nullptr; }
    /** returns the data or null when the id is NONE */
    template<class U=Impl>
    C4_ALWAYS_INLINE auto get() RYML_NOEXCEPT -> _C4_IF_MUTABLE(NodeData*) { return ((Impl const*)this)->readable() ? tree__->get(id__) : nullptr; }

    C4_ALWAYS_INLINE NodeType    type() const RYML_NOEXCEPT { _C4RV(); return tree_->type(id_); }
    C4_ALWAYS_INLINE const char* type_str() const RYML_NOEXCEPT { _C4RV(); return tree_->type_str(id_); }

    C4_ALWAYS_INLINE csubstr key()        const RYML_NOEXCEPT { _C4RV(); return tree_->key(id_); }
    C4_ALWAYS_INLINE csubstr key_tag()    const RYML_NOEXCEPT { _C4RV(); return tree_->key_tag(id_); }
    C4_ALWAYS_INLINE csubstr key_ref()    const RYML_NOEXCEPT { _C4RV(); return tree_->key_ref(id_); }
    C4_ALWAYS_INLINE csubstr key_anchor() const RYML_NOEXCEPT { _C4RV(); return tree_->key_anchor(id_); }

    C4_ALWAYS_INLINE csubstr val()        const RYML_NOEXCEPT { _C4RV(); return tree_->val(id_); }
    C4_ALWAYS_INLINE csubstr val_tag()    const RYML_NOEXCEPT { _C4RV(); return tree_->val_tag(id_); }
    C4_ALWAYS_INLINE csubstr val_ref()    const RYML_NOEXCEPT { _C4RV(); return tree_->val_ref(id_); }
    C4_ALWAYS_INLINE csubstr val_anchor() const RYML_NOEXCEPT { _C4RV(); return tree_->val_anchor(id_); }

    C4_ALWAYS_INLINE NodeScalar const& keysc() const RYML_NOEXCEPT { _C4RV(); return tree_->keysc(id_); }
    C4_ALWAYS_INLINE NodeScalar const& valsc() const RYML_NOEXCEPT { _C4RV(); return tree_->valsc(id_); }

    C4_ALWAYS_INLINE bool key_is_null() const RYML_NOEXCEPT { _C4RV(); return tree_->key_is_null(id_); }
    C4_ALWAYS_INLINE bool val_is_null() const RYML_NOEXCEPT { _C4RV(); return tree_->val_is_null(id_); }

    /** @} */

public:

    /** @name node property predicates */
    /** @{ */

    C4_ALWAYS_INLINE bool empty()            const RYML_NOEXCEPT { _C4RV(); return tree_->empty(id_); }
    C4_ALWAYS_INLINE bool is_stream()        const RYML_NOEXCEPT { _C4RV(); return tree_->is_stream(id_); }
    C4_ALWAYS_INLINE bool is_doc()           const RYML_NOEXCEPT { _C4RV(); return tree_->is_doc(id_); }
    C4_ALWAYS_INLINE bool is_container()     const RYML_NOEXCEPT { _C4RV(); return tree_->is_container(id_); }
    C4_ALWAYS_INLINE bool is_map()           const RYML_NOEXCEPT { _C4RV(); return tree_->is_map(id_); }
    C4_ALWAYS_INLINE bool is_seq()           const RYML_NOEXCEPT { _C4RV(); return tree_->is_seq(id_); }
    C4_ALWAYS_INLINE bool has_val()          const RYML_NOEXCEPT { _C4RV(); return tree_->has_val(id_); }
    C4_ALWAYS_INLINE bool has_key()          const RYML_NOEXCEPT { _C4RV(); return tree_->has_key(id_); }
    C4_ALWAYS_INLINE bool is_val()           const RYML_NOEXCEPT { _C4RV(); return tree_->is_val(id_); }
    C4_ALWAYS_INLINE bool is_keyval()        const RYML_NOEXCEPT { _C4RV(); return tree_->is_keyval(id_); }
    C4_ALWAYS_INLINE bool has_key_tag()      const RYML_NOEXCEPT { _C4RV(); return tree_->has_key_tag(id_); }
    C4_ALWAYS_INLINE bool has_val_tag()      const RYML_NOEXCEPT { _C4RV(); return tree_->has_val_tag(id_); }
    C4_ALWAYS_INLINE bool has_key_anchor()   const RYML_NOEXCEPT { _C4RV(); return tree_->has_key_anchor(id_); }
    C4_ALWAYS_INLINE bool is_key_anchor()    const RYML_NOEXCEPT { _C4RV(); return tree_->is_key_anchor(id_); }
    C4_ALWAYS_INLINE bool has_val_anchor()   const RYML_NOEXCEPT { _C4RV(); return tree_->has_val_anchor(id_); }
    C4_ALWAYS_INLINE bool is_val_anchor()    const RYML_NOEXCEPT { _C4RV(); return tree_->is_val_anchor(id_); }
    C4_ALWAYS_INLINE bool has_anchor()       const RYML_NOEXCEPT { _C4RV(); return tree_->has_anchor(id_); }
    C4_ALWAYS_INLINE bool is_anchor()        const RYML_NOEXCEPT { _C4RV(); return tree_->is_anchor(id_); }
    C4_ALWAYS_INLINE bool is_key_ref()       const RYML_NOEXCEPT { _C4RV(); return tree_->is_key_ref(id_); }
    C4_ALWAYS_INLINE bool is_val_ref()       const RYML_NOEXCEPT { _C4RV(); return tree_->is_val_ref(id_); }
    C4_ALWAYS_INLINE bool is_ref()           const RYML_NOEXCEPT { _C4RV(); return tree_->is_ref(id_); }
    C4_ALWAYS_INLINE bool is_anchor_or_ref() const RYML_NOEXCEPT { _C4RV(); return tree_->is_anchor_or_ref(id_); }
    C4_ALWAYS_INLINE bool is_key_quoted()    const RYML_NOEXCEPT { _C4RV(); return tree_->is_key_quoted(id_); }
    C4_ALWAYS_INLINE bool is_val_quoted()    const RYML_NOEXCEPT { _C4RV(); return tree_->is_val_quoted(id_); }
    C4_ALWAYS_INLINE bool is_quoted()        const RYML_NOEXCEPT { _C4RV(); return tree_->is_quoted(id_); }
    C4_ALWAYS_INLINE bool parent_is_seq()    const RYML_NOEXCEPT { _C4RV(); return tree_->parent_is_seq(id_); }
    C4_ALWAYS_INLINE bool parent_is_map()    const RYML_NOEXCEPT { _C4RV(); return tree_->parent_is_map(id_); }

    /** @} */

public:

    /** @name hierarchy predicates */
    /** @{ */

    C4_ALWAYS_INLINE bool is_root()    const RYML_NOEXCEPT { _C4RV(); return tree_->is_root(id_); }
    C4_ALWAYS_INLINE bool has_parent() const RYML_NOEXCEPT { _C4RV(); return tree_->has_parent(id_); }

    C4_ALWAYS_INLINE bool has_child(ConstImpl const& n) const RYML_NOEXCEPT { _C4RV(); return n.readable() ? tree_->has_child(id_, n.m_id) : false; }
    C4_ALWAYS_INLINE bool has_child(size_t node) const RYML_NOEXCEPT { _C4RV(); return tree_->has_child(id_, node); }
    C4_ALWAYS_INLINE bool has_child(csubstr name) const RYML_NOEXCEPT { _C4RV(); return tree_->has_child(id_, name); }
    C4_ALWAYS_INLINE bool has_children() const RYML_NOEXCEPT { _C4RV(); return tree_->has_children(id_); }

    C4_ALWAYS_INLINE bool has_sibling(ConstImpl const& n) const RYML_NOEXCEPT { _C4RV(); return n.readable() ? tree_->has_sibling(id_, n.m_id) : false; }
    C4_ALWAYS_INLINE bool has_sibling(size_t node) const RYML_NOEXCEPT { _C4RV(); return tree_->has_sibling(id_, node); }
    C4_ALWAYS_INLINE bool has_sibling(csubstr name) const RYML_NOEXCEPT { _C4RV(); return tree_->has_sibling(id_, name); }
    /** does not count with this */
    C4_ALWAYS_INLINE bool has_other_siblings() const RYML_NOEXCEPT { _C4RV(); return tree_->has_other_siblings(id_); }

    /** counts with this */
    RYML_DEPRECATED("use has_other_siblings()") bool has_siblings() const RYML_NOEXCEPT { _C4RV(); return tree_->has_siblings(id_); }

    /** @} */

public:

    /** @name hierarchy getters */
    /** @{ */

    /** succeeds even when the node may have invalid or seed id */
    template<class U=Impl>
    C4_ALWAYS_INLINE auto doc(size_t num) RYML_NOEXCEPT -> _C4_IF_MUTABLE(Impl) { RYML_ASSERT(tree_); return {tree__, tree__->doc(num)}; }
    /** succeeds even when the node may have invalid or seed id */
    C4_ALWAYS_INLINE ConstImpl doc(size_t num) const RYML_NOEXCEPT { RYML_ASSERT(tree_); return {tree_, tree_->doc(num)}; }

    template<class U=Impl>
    C4_ALWAYS_INLINE auto parent() RYML_NOEXCEPT -> _C4_IF_MUTABLE(Impl) { _C4RV(); return {tree__, tree__->parent(id__)}; }
    C4_ALWAYS_INLINE ConstImpl parent() const RYML_NOEXCEPT { _C4RV(); return {tree_, tree_->parent(id_)}; }

    template<class U=Impl>
    C4_ALWAYS_INLINE auto first_child() RYML_NOEXCEPT -> _C4_IF_MUTABLE(Impl) { _C4RV(); return {tree__, tree__->first_child(id__)}; }
    C4_ALWAYS_INLINE ConstImpl first_child() const RYML_NOEXCEPT { _C4RV(); return {tree_, tree_->first_child(id_)}; }

    template<class U=Impl>
    C4_ALWAYS_INLINE auto last_child() RYML_NOEXCEPT -> _C4_IF_MUTABLE(Impl) { _C4RV(); return {tree__, tree__->last_child(id__)}; }
    C4_ALWAYS_INLINE ConstImpl last_child () const RYML_NOEXCEPT { _C4RV(); return {tree_, tree_->last_child (id_)}; }

    template<class U=Impl>
    C4_ALWAYS_INLINE auto child(size_t pos) RYML_NOEXCEPT -> _C4_IF_MUTABLE(Impl) { _C4RV(); return {tree__, tree__->child(id__, pos)}; }
    C4_ALWAYS_INLINE ConstImpl child(size_t pos) const RYML_NOEXCEPT { _C4RV(); return {tree_, tree_->child(id_, pos)}; }

    template<class U=Impl>
    C4_ALWAYS_INLINE auto find_child(csubstr name)  RYML_NOEXCEPT -> _C4_IF_MUTABLE(Impl) { _C4RV(); return {tree__, tree__->find_child(id__, name)}; }
    C4_ALWAYS_INLINE ConstImpl find_child(csubstr name) const RYML_NOEXCEPT { _C4RV(); return {tree_, tree_->find_child(id_, name)}; }

    template<class U=Impl>
    C4_ALWAYS_INLINE auto prev_sibling() RYML_NOEXCEPT -> _C4_IF_MUTABLE(Impl) { _C4RV(); return {tree__, tree__->prev_sibling(id__)}; }
    C4_ALWAYS_INLINE ConstImpl prev_sibling() const RYML_NOEXCEPT { _C4RV(); return {tree_, tree_->prev_sibling(id_)}; }

    template<class U=Impl>
    C4_ALWAYS_INLINE auto next_sibling() RYML_NOEXCEPT -> _C4_IF_MUTABLE(Impl) { _C4RV(); return {tree__, tree__->next_sibling(id__)}; }
    C4_ALWAYS_INLINE ConstImpl next_sibling() const RYML_NOEXCEPT { _C4RV(); return {tree_, tree_->next_sibling(id_)}; }

    template<class U=Impl>
    C4_ALWAYS_INLINE auto first_sibling() RYML_NOEXCEPT -> _C4_IF_MUTABLE(Impl) { _C4RV(); return {tree__, tree__->first_sibling(id__)}; }
    C4_ALWAYS_INLINE ConstImpl first_sibling() const RYML_NOEXCEPT { _C4RV(); return {tree_, tree_->first_sibling(id_)}; }

    template<class U=Impl>
    C4_ALWAYS_INLINE auto last_sibling() RYML_NOEXCEPT -> _C4_IF_MUTABLE(Impl) { _C4RV(); return {tree__, tree__->last_sibling(id__)}; }
    C4_ALWAYS_INLINE ConstImpl last_sibling () const RYML_NOEXCEPT { _C4RV(); return {tree_, tree_->last_sibling(id_)}; }

    template<class U=Impl>
    C4_ALWAYS_INLINE auto sibling(size_t pos) RYML_NOEXCEPT -> _C4_IF_MUTABLE(Impl) { _C4RV(); return {tree__, tree__->sibling(id__, pos)}; }
    C4_ALWAYS_INLINE ConstImpl sibling(size_t pos) const RYML_NOEXCEPT { _C4RV(); return {tree_, tree_->sibling(id_, pos)}; }

    template<class U=Impl>
    C4_ALWAYS_INLINE auto find_sibling(csubstr name) RYML_NOEXCEPT -> _C4_IF_MUTABLE(Impl) { _C4RV(); return {tree__, tree__->find_sibling(id__, name)}; }
    C4_ALWAYS_INLINE ConstImpl find_sibling(csubstr name) const RYML_NOEXCEPT { _C4RV(); return {tree_, tree_->find_sibling(id_, name)}; }

    /** O(#num_children) */
    C4_ALWAYS_INLINE size_t child_pos(ConstImpl const& n) const RYML_NOEXCEPT { _C4RV(); _RYML_CB_ASSERT(tree_->m_callbacks, n.readable()); return tree_->child_pos(id_, n.m_id); }

    /** O(#num_children) */
    C4_ALWAYS_INLINE size_t num_children() const RYML_NOEXCEPT { _C4RV(); return tree_->num_children(id_); }

    /** O(#num_siblings) */
    C4_ALWAYS_INLINE size_t num_siblings() const RYML_NOEXCEPT { _C4RV(); return tree_->num_siblings(id_); }
    C4_ALWAYS_INLINE size_t num_other_siblings() const RYML_NOEXCEPT { _C4RV(); return tree_->num_other_siblings(id_); }
    C4_ALWAYS_INLINE size_t sibling_pos(ConstImpl const& n) const RYML_NOEXCEPT { _C4RV(); _RYML_CB_ASSERT(tree_->callbacks(), n.readable()); return tree_->child_pos(tree_->parent(id_), n.m_id); }

    /** Find child by key; complexity is O(num_children).
     *
     * Returns the requested node, or an object in seed state if no
     * such child is found (see @ref NodeRef for an explanation of
     * what is seed state). When the object is in seed state, using it
     * to read from the tree is UB. The seed node can be used to write
     * to the tree provided that its create() method is called prior
     * to writing, which happens in most modifying methods in
     * NodeRef. It is the caller's responsibility to verify that the
     * returned node is readable before subsequently using it.
     *
     * @warning the calling object must be readable. This precondition
     * is asserted. This assertion is performed only if @ref
     * RYML_USE_ASSERT is set to true. As with the non-const overload,
     * it is UB to call this method if the node is not readable.
     *
     * @see https://github.com/biojppm/rapidyaml/issues/389 */
    template<class U=Impl>
    C4_ALWAYS_INLINE auto operator[] (csubstr key) RYML_NOEXCEPT -> _C4_IF_MUTABLE(Impl)
    {
        _C4RV();
        size_t ch = tree__->find_child(id__, key);
        return ch != NONE ? Impl(tree__, ch) : Impl(tree__, id__, key);
    }

    /** Find child by position; complexity is O(pos).
     *
     * Returns the requested node, or an object in seed state if no
     * such child is found (see @ref NodeRef for an explanation of
     * what is seed state). When the object is in seed state, using it
     * to read from the tree is UB. The seed node can be used to write
     * to the tree provided that its create() method is called prior
     * to writing, which happens in most modifying methods in
     * NodeRef. It is the caller's responsibility to verify that the
     * returned node is readable before subsequently using it.
     *
     * @warning the calling object must be readable. This precondition
     * is asserted. This assertion is performed only if @ref
     * RYML_USE_ASSERT is set to true. As with the non-const overload,
     * it is UB to call this method if the node is not readable.
     *
     * @see https://github.com/biojppm/rapidyaml/issues/389 */
    template<class U=Impl>
    C4_ALWAYS_INLINE auto operator[] (size_t pos) RYML_NOEXCEPT -> _C4_IF_MUTABLE(Impl)
    {
        _C4RV();
        size_t ch = tree__->child(id__, pos);
        return ch != NONE ? Impl(tree__, ch) : Impl(tree__, id__, pos);
    }

    /** Find a child by key; complexity is O(num_children).
     *
     * Behaves similar to the non-const overload, but further asserts
     * that the returned node is readable (because it can never be in
     * a seed state). This assertion is performed only if @ref
     * RYML_USE_ASSERT is set to true. As with the non-const overload,
     * it is UB to use the return value if it is not valid.
     *
     * @see https://github.com/biojppm/rapidyaml/issues/389  */
    C4_ALWAYS_INLINE ConstImpl operator[] (csubstr key) const RYML_NOEXCEPT
    {
        _C4RV();
        size_t ch = tree_->find_child(id_, key);
        _RYML_CB_ASSERT(tree_->m_callbacks, ch != NONE);
        return {tree_, ch};
    }

    /** Find a child by position; complexity is O(pos).
     *
     * Behaves similar to the non-const overload, but further asserts
     * that the returned node is readable (because it can never be in
     * a seed state). This assertion is performed only if @ref
     * RYML_USE_ASSERT is set to true. As with the non-const overload,
     * it is UB to use the return value if it is not valid.
     *
     * @see https://github.com/biojppm/rapidyaml/issues/389  */
    C4_ALWAYS_INLINE ConstImpl operator[] (size_t pos) const RYML_NOEXCEPT
    {
        _C4RV();
        size_t ch = tree_->child(id_, pos);
        _RYML_CB_ASSERT(tree_->m_callbacks, ch != NONE);
        return {tree_, ch};
    }

    /** Get a child by name, with error checking; complexity is
     * O(num_children).
     *
     * Behaves as operator[](csubstr) const, but always raises an
     * error (even when RYML_USE_ASSERT is set to false) when the
     * returned node does not exist, or when this node is not
     * readable, or when it is not a map. This behaviour is similar to
     * std::vector::at(), but the error consists in calling the error
     * callback instead of directly raising an exception. */
    ConstImpl at(csubstr key) const
    {
        RYML_CHECK(tree_ != nullptr);
        _RYML_CB_CHECK(tree_->m_callbacks, ((Impl const*)this)->readable());
        _RYML_CB_CHECK(tree_->m_callbacks, ((Impl const*)this)->is_map());
        size_t ch = tree_->find_child(id_, key);
        _RYML_CB_CHECK(tree_->m_callbacks, ch != NONE);
        return {tree_, ch};
    }

    /** Get a child by position, with error checking; complexity is
     * O(pos).
     *
     * Behaves as operator[](size_t) const, but always raises an error
     * (even when RYML_USE_ASSERT is set to false) when the returned
     * node does not exist, or when this node is not readable, or when
     * it is not a map. This behaviour is similar to
     * std::vector::at(), but the error consists in calling the error
     * callback instead of directly raising an exception. */
    ConstImpl at(size_t pos) const
    {
        RYML_CHECK(tree_ != nullptr);
        _RYML_CB_CHECK(tree_->m_callbacks, ((Impl const*)this)->readable());
        _RYML_CB_CHECK(tree_->m_callbacks, ((Impl const*)this)->is_container());
        size_t ch = tree_->child(id_, pos);
        _RYML_CB_ASSERT(tree_->m_callbacks, ch != NONE);
        return {tree_, ch};
    }

    /** @} */

public:

    /** deserialization */
    /** @{ */

    template<class T>
    ConstImpl const& operator>> (T &v) const
    {
        _C4RV();
        if( ! read((ConstImpl const&)*this, &v))
            _RYML_CB_ERR(tree_->m_callbacks, "could not deserialize value");
        return *((ConstImpl const*)this);
    }

    /** deserialize the node's key to the given variable */
    template<class T>
    ConstImpl const& operator>> (Key<T> v) const
    {
        _C4RV();
        if( ! from_chars(key(), &v.k))
            _RYML_CB_ERR(tree_->m_callbacks, "could not deserialize key");
        return *((ConstImpl const*)this);
    }

    /** deserialize the node's key as base64 */
    ConstImpl const& operator>> (Key<fmt::base64_wrapper> w) const
    {
        deserialize_key(w.wrapper);
        return *((ConstImpl const*)this);
    }

    /** deserialize the node's val as base64 */
    ConstImpl const& operator>> (fmt::base64_wrapper w) const
    {
        deserialize_val(w);
        return *((ConstImpl const*)this);
    }

    /** decode the base64-encoded key and assign the
     * decoded blob to the given buffer/
     * @return the size of base64-decoded blob */
    size_t deserialize_key(fmt::base64_wrapper v) const
    {
        _C4RV();
        return from_chars(key(), &v);
    }
    /** decode the base64-encoded key and assign the
     * decoded blob to the given buffer/
     * @return the size of base64-decoded blob */
    size_t deserialize_val(fmt::base64_wrapper v) const
    {
        _C4RV();
        return from_chars(val(), &v);
    };

    template<class T>
    bool get_if(csubstr name, T *var) const
    {
        _C4RV();
        ConstImpl ch = find_child(name);
        if(!ch.valid())
            return false;
        ch >> *var;
        return true;
    }

    template<class T>
    bool get_if(csubstr name, T *var, T const& fallback) const
    {
        _C4RV();
        ConstImpl ch = find_child(name);
        if(ch.valid())
        {
            ch >> *var;
            return true;
        }
        else
        {
            *var = fallback;
            return false;
        }
    }

    /** @} */

public:

    #if defined(__clang__)
    #   pragma clang diagnostic push
    #   pragma clang diagnostic ignored "-Wnull-dereference"
    #elif defined(__GNUC__)
    #   pragma GCC diagnostic push
    #   if __GNUC__ >= 6
    #       pragma GCC diagnostic ignored "-Wnull-dereference"
    #   endif
    #endif

    /** @name iteration */
    /** @{ */

    using iterator = detail::child_iterator<Impl>;
    using const_iterator = detail::child_iterator<ConstImpl>;
    using children_view = detail::children_view_<Impl>;
    using const_children_view = detail::children_view_<ConstImpl>;

    template<class U=Impl>
    C4_ALWAYS_INLINE auto begin() RYML_NOEXCEPT -> _C4_IF_MUTABLE(iterator) { _C4RV(); return iterator(tree__, tree__->first_child(id__)); }
    C4_ALWAYS_INLINE const_iterator begin() const RYML_NOEXCEPT { _C4RV(); return const_iterator(tree_, tree_->first_child(id_)); }
    C4_ALWAYS_INLINE const_iterator cbegin() const RYML_NOEXCEPT { _C4RV(); return const_iterator(tree_, tree_->first_child(id_)); }

    template<class U=Impl>
    C4_ALWAYS_INLINE auto end() RYML_NOEXCEPT -> _C4_IF_MUTABLE(iterator) { _C4RV(); return iterator(tree__, NONE); }
    C4_ALWAYS_INLINE const_iterator end() const RYML_NOEXCEPT { _C4RV(); return const_iterator(tree_, NONE); }
    C4_ALWAYS_INLINE const_iterator cend() const RYML_NOEXCEPT { _C4RV(); return const_iterator(tree_, tree_->first_child(id_)); }

    /** get an iterable view over children */
    template<class U=Impl>
    C4_ALWAYS_INLINE auto children() RYML_NOEXCEPT -> _C4_IF_MUTABLE(children_view) { _C4RV(); return children_view(begin(), end()); }
    /** get an iterable view over children */
    C4_ALWAYS_INLINE const_children_view children() const RYML_NOEXCEPT { _C4RV(); return const_children_view(begin(), end()); }
    /** get an iterable view over children */
    C4_ALWAYS_INLINE const_children_view cchildren() const RYML_NOEXCEPT { _C4RV(); return const_children_view(begin(), end()); }

    /** get an iterable view over all siblings (including the calling node) */
    template<class U=Impl>
    C4_ALWAYS_INLINE auto siblings() RYML_NOEXCEPT -> _C4_IF_MUTABLE(children_view)
    {
        _C4RV();
        NodeData const *nd = tree__->get(id__);
        return (nd->m_parent != NONE) ? // does it have a parent?
            children_view(iterator(tree__, tree_->get(nd->m_parent)->m_first_child), iterator(tree__, NONE))
            :
            children_view(end(), end());
    }
    /** get an iterable view over all siblings (including the calling node) */
    C4_ALWAYS_INLINE const_children_view siblings() const RYML_NOEXCEPT
    {
        _C4RV();
        NodeData const *nd = tree_->get(id_);
        return (nd->m_parent != NONE) ? // does it have a parent?
            const_children_view(const_iterator(tree_, tree_->get(nd->m_parent)->m_first_child), const_iterator(tree_, NONE))
            :
            const_children_view(end(), end());
    }
    /** get an iterable view over all siblings (including the calling node) */
    C4_ALWAYS_INLINE const_children_view csiblings() const RYML_NOEXCEPT { return siblings(); }

    /** visit every child node calling fn(node) */
    template<class Visitor>
    bool visit(Visitor fn, size_t indentation_level=0, bool skip_root=true) const RYML_NOEXCEPT
    {
        _C4RV();
        return detail::_visit(*(ConstImpl const*)this, fn, indentation_level, skip_root);
    }
    /** visit every child node calling fn(node) */
    template<class Visitor, class U=Impl>
    auto visit(Visitor fn, size_t indentation_level=0, bool skip_root=true) RYML_NOEXCEPT
        -> _C4_IF_MUTABLE(bool)
    {
        _C4RV();
        return detail::_visit(*(Impl*)this, fn, indentation_level, skip_root);
    }

    /** visit every child node calling fn(node, level) */
    template<class Visitor>
    bool visit_stacked(Visitor fn, size_t indentation_level=0, bool skip_root=true) const RYML_NOEXCEPT
    {
        _C4RV();
        return detail::_visit_stacked(*(ConstImpl const*)this, fn, indentation_level, skip_root);
    }
    /** visit every child node calling fn(node, level) */
    template<class Visitor, class U=Impl>
    auto visit_stacked(Visitor fn, size_t indentation_level=0, bool skip_root=true) RYML_NOEXCEPT
        -> _C4_IF_MUTABLE(bool)
    {
        _C4RV();
        return detail::_visit_stacked(*(Impl*)this, fn, indentation_level, skip_root);
    }

    /** @} */

    #if defined(__clang__)
    #   pragma clang diagnostic pop
    #elif defined(__GNUC__)
    #   pragma GCC diagnostic pop
    #endif

    #undef _C4_IF_MUTABLE
    #undef _C4RV
    #undef tree_
    #undef tree__
    #undef id_
    #undef id__

    C4_SUPPRESS_WARNING_GCC_CLANG_POP
};

} // namespace detail


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class RYML_EXPORT ConstNodeRef : public detail::RoNodeMethods<ConstNodeRef, ConstNodeRef>
{
public:

    using tree_type = Tree const;

public:

    Tree const* C4_RESTRICT m_tree;
    size_t m_id;

    friend NodeRef;
    friend struct detail::RoNodeMethods<ConstNodeRef, ConstNodeRef>;

public:

    /** @name construction */
    /** @{ */

    ConstNodeRef() : m_tree(nullptr), m_id(NONE) {}
    ConstNodeRef(Tree const &t) : m_tree(&t), m_id(t .root_id()) {}
    ConstNodeRef(Tree const *t) : m_tree(t ), m_id(t->root_id()) {}
    ConstNodeRef(Tree const *t, size_t id) : m_tree(t), m_id(id) {}
    ConstNodeRef(std::nullptr_t) : m_tree(nullptr), m_id(NONE) {}

    ConstNodeRef(ConstNodeRef const&) = default;
    ConstNodeRef(ConstNodeRef     &&) = default;

    ConstNodeRef(NodeRef const&);
    ConstNodeRef(NodeRef     &&);

    /** @} */

public:

    /** @name assignment */
    /** @{ */

    ConstNodeRef& operator= (std::nullptr_t) { m_tree = nullptr; m_id = NONE; return *this; }

    ConstNodeRef& operator= (ConstNodeRef const&) = default;
    ConstNodeRef& operator= (ConstNodeRef     &&) = default;

    ConstNodeRef& operator= (NodeRef const&);
    ConstNodeRef& operator= (NodeRef     &&);


    /** @} */

public:

    /** @name state queries */
    /** @{ */

    C4_ALWAYS_INLINE C4_PURE bool valid() const noexcept { return m_tree != nullptr && m_id != NONE; }
    /** because this is a const method, readable() has the same meaning as valid() */
    C4_ALWAYS_INLINE C4_PURE bool readable() const noexcept { return m_tree != nullptr && m_id != NONE; }
    /** because a ConstNodeRef cannot be used to write to the tree, it can never be a seed.
     * This method is provided for API equivalence between ConstNodeRef and NodeRef. */
    constexpr static C4_ALWAYS_INLINE C4_PURE bool is_seed() noexcept { return false; }

    /** @} */

public:

    /** @name member getters */
    /** @{ */

    C4_ALWAYS_INLINE C4_PURE Tree const* tree() const noexcept { return m_tree; }
    C4_ALWAYS_INLINE C4_PURE size_t id() const noexcept { return m_id; }

    /** @} */

public:

    /** @name comparisons */
    /** @{ */

    C4_ALWAYS_INLINE bool operator== (ConstNodeRef const& that) const RYML_NOEXCEPT { return that.m_tree == m_tree && m_id == that.m_id; }
    C4_ALWAYS_INLINE bool operator!= (ConstNodeRef const& that) const RYML_NOEXCEPT { return ! this->operator== (that); }

    C4_ALWAYS_INLINE C4_PURE bool operator== (std::nullptr_t) const noexcept { return m_tree == nullptr || m_id == NONE; }
    C4_ALWAYS_INLINE C4_PURE bool operator!= (std::nullptr_t) const noexcept { return ! this->operator== (nullptr); }

    C4_ALWAYS_INLINE bool operator== (csubstr val) const RYML_NOEXCEPT { RYML_ASSERT(m_tree); _RYML_CB_ASSERT(m_tree->m_callbacks, m_id != NONE); return m_tree->val(m_id) == val; }
    C4_ALWAYS_INLINE bool operator!= (csubstr val) const RYML_NOEXCEPT { RYML_ASSERT(m_tree); _RYML_CB_ASSERT(m_tree->m_callbacks, m_id != NONE); return m_tree->val(m_id) != val; }

    /** @} */

};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/** a reference to a node in an existing yaml tree, offering a more
 * convenient API than the index-based API used in the tree. This
 * reference can be used to modify the tree. Individual objects may be */
class RYML_EXPORT NodeRef : public detail::RoNodeMethods<NodeRef, ConstNodeRef>
{
public:

    using tree_type = Tree;
    using base_type = detail::RoNodeMethods<NodeRef, ConstNodeRef>;

private:

    Tree *C4_RESTRICT m_tree;
    size_t m_id;

    /** This member is used to enable lazy operator[] writing. When a child
     * with a key or index is not found, m_id is set to the id of the parent
     * and the asked-for key or index are stored in this member until a write
     * does happen. Then it is given as key or index for creating the child.
     * When a key is used, the csubstr stores it (so the csubstr's string is
     * non-null and the csubstr's size is different from NONE). When an index is
     * used instead, the csubstr's string is set to null, and only the csubstr's
     * size is set to a value different from NONE. Otherwise, when operator[]
     * does find the child then this member is empty: the string is null and
     * the size is NONE. */
    csubstr m_seed;

    friend ConstNodeRef;
    friend struct detail::RoNodeMethods<NodeRef, ConstNodeRef>;

    // require valid: a helper macro, undefined at the end
    #define _C4RV()                                                         \
        RYML_ASSERT(m_tree != nullptr);                                     \
        _RYML_CB_ASSERT(m_tree->m_callbacks, m_id != NONE && !is_seed())
    // require id: a helper macro, undefined at the end
    #define _C4RID()                                                        \
        RYML_ASSERT(m_tree != nullptr);                                     \
        _RYML_CB_ASSERT(m_tree->m_callbacks, m_id != NONE)

public:

    /** @name construction */
    /** @{ */

    NodeRef() : m_tree(nullptr), m_id(NONE), m_seed() { _clear_seed(); }
    NodeRef(Tree &t) : m_tree(&t), m_id(t .root_id()), m_seed() { _clear_seed(); }
    NodeRef(Tree *t) : m_tree(t ), m_id(t->root_id()), m_seed() { _clear_seed(); }
    NodeRef(Tree *t, size_t id) : m_tree(t), m_id(id), m_seed() { _clear_seed(); }
    NodeRef(Tree *t, size_t id, size_t seed_pos) : m_tree(t), m_id(id), m_seed() { m_seed.str = nullptr; m_seed.len = seed_pos; }
    NodeRef(Tree *t, size_t id, csubstr  seed_key) : m_tree(t), m_id(id), m_seed(seed_key) {}
    NodeRef(std::nullptr_t) : m_tree(nullptr), m_id(NONE), m_seed() {}

    /** @} */

public:

    /** @name assignment */
    /** @{ */

    NodeRef(NodeRef const&) = default;
    NodeRef(NodeRef     &&) = default;

    NodeRef& operator= (NodeRef const&) = default;
    NodeRef& operator= (NodeRef     &&) = default;

    /** @} */

public:

    /** @name state queries */
    /** @{ */

    inline bool valid() const { return m_tree != nullptr && m_id != NONE; }
    inline bool is_seed() const { return m_seed.str != nullptr || m_seed.len != NONE; }
    inline bool readable() const { return valid() && !is_seed(); }

    inline void _clear_seed() { /*do the following manually or an assert is triggered: */ m_seed.str = nullptr; m_seed.len = NONE; }

    /** @} */

public:

    /** @name comparisons */
    /** @{ */

    inline bool operator== (NodeRef const& that) const
    {
        if(m_tree == that.m_tree && m_id == that.m_id)
        {
            if(is_seed() == that.is_seed())
            {
                if(is_seed())
                {
                    return (m_seed.len == that.m_seed.len) && (m_seed.str == that.m_seed.str || m_seed == that.m_seed);
                }
                return true;
            }
        }
        return false;
    }
    inline bool operator!= (NodeRef const& that) const { return ! this->operator==(that); }

    inline bool operator== (ConstNodeRef const& that) const { return m_tree == that.m_tree && m_id == that.m_id && !is_seed(); }
    inline bool operator!= (ConstNodeRef const& that) const { return ! this->operator==(that); }

    inline bool operator== (std::nullptr_t) const { return m_tree == nullptr || m_id == NONE || is_seed(); }
    inline bool operator!= (std::nullptr_t) const { return ! this->operator==(nullptr); }

    inline bool operator== (csubstr val) const { _C4RV(); _RYML_CB_ASSERT(m_tree->m_callbacks, has_val()); return m_tree->val(m_id) == val; }
    inline bool operator!= (csubstr val) const { _C4RV(); _RYML_CB_ASSERT(m_tree->m_callbacks, has_val()); return m_tree->val(m_id) != val; }

    /** @} */

public:

    /** @name node property getters */
    /** @{ */

    C4_ALWAYS_INLINE C4_PURE Tree * tree() noexcept { return m_tree; }
    C4_ALWAYS_INLINE C4_PURE Tree const* tree() const noexcept { return m_tree; }

    C4_ALWAYS_INLINE C4_PURE size_t id() const noexcept { return m_id; }

    /** @} */

public:

    /** @name node modifiers */
    /** @{ */

    void create() { _apply_seed(); }

    void change_type(NodeType t) { _C4RV(); m_tree->change_type(m_id, t); }

    void set_type(NodeType t) { _apply_seed(); m_tree->_set_flags(m_id, t); }
    void set_key(csubstr key) { _apply_seed(); m_tree->_set_key(m_id, key); }
    void set_val(csubstr val) { _apply_seed(); m_tree->_set_val(m_id, val); }
    void set_key_tag(csubstr key_tag) { _apply_seed(); m_tree->set_key_tag(m_id, key_tag); }
    void set_val_tag(csubstr val_tag) { _apply_seed(); m_tree->set_val_tag(m_id, val_tag); }
    void set_key_anchor(csubstr key_anchor) { _apply_seed(); m_tree->set_key_anchor(m_id, key_anchor); }
    void set_val_anchor(csubstr val_anchor) { _apply_seed(); m_tree->set_val_anchor(m_id, val_anchor); }
    void set_key_ref(csubstr key_ref) { _apply_seed(); m_tree->set_key_ref(m_id, key_ref); }
    void set_val_ref(csubstr val_ref) { _apply_seed(); m_tree->set_val_ref(m_id, val_ref); }

    template<class T>
    size_t set_key_serialized(T const& C4_RESTRICT k)
    {
        _apply_seed();
        csubstr s = m_tree->to_arena(k);
        m_tree->_set_key(m_id, s);
        return s.len;
    }
    template<class T>
    size_t set_val_serialized(T const& C4_RESTRICT v)
    {
        _apply_seed();
        csubstr s = m_tree->to_arena(v);
        m_tree->_set_val(m_id, s);
        return s.len;
    }
    size_t set_val_serialized(std::nullptr_t)
    {
        _apply_seed();
        m_tree->_set_val(m_id, csubstr{});
        return 0;
    }

    /** encode a blob as base64 into the tree's arena, then assign the
     * result to the node's key @return the size of base64-encoded
     * blob */
    size_t set_key_serialized(fmt::const_base64_wrapper w);
    /** encode a blob as base64 into the tree's arena, then assign the
     * result to the node's val @return the size of base64-encoded
     * blob */
    size_t set_val_serialized(fmt::const_base64_wrapper w);

public:

    inline void clear()
    {
        if(is_seed())
            return;
        m_tree->remove_children(m_id);
        m_tree->_clear(m_id);
    }

    inline void clear_key()
    {
        if(is_seed())
            return;
        m_tree->_clear_key(m_id);
    }

    inline void clear_val()
    {
        if(is_seed())
            return;
        m_tree->_clear_val(m_id);
    }

    inline void clear_children()
    {
        if(is_seed())
            return;
        m_tree->remove_children(m_id);
    }

    inline void operator= (NodeType_e t)
    {
        _apply_seed();
        m_tree->_add_flags(m_id, t);
    }

    inline void operator|= (NodeType_e t)
    {
        _apply_seed();
        m_tree->_add_flags(m_id, t);
    }

    inline void operator= (NodeInit const& v)
    {
        _apply_seed();
        _apply(v);
    }

    inline void operator= (NodeScalar const& v)
    {
        _apply_seed();
        _apply(v);
    }

    inline void operator= (std::nullptr_t)
    {
        _apply_seed();
        _apply(csubstr{});
    }

    inline void operator= (csubstr v)
    {
        _apply_seed();
        _apply(v);
    }

    template<size_t N>
    inline void operator= (const char (&v)[N])
    {
        _apply_seed();
        csubstr sv;
        sv.assign<N>(v);
        _apply(sv);
    }

    /** @} */

public:

    /** @name serialization */
    /** @{ */

    /** serialize a variable to the arena */
    template<class T>
    inline csubstr to_arena(T const& C4_RESTRICT s)
    {
        RYML_ASSERT(m_tree); // no need for valid or readable
        return m_tree->to_arena(s);
    }

    /** serialize a variable, then assign the result to the node's val */
    inline NodeRef& operator<< (csubstr s)
    {
        // this overload is needed to prevent ambiguity (there's also
        // operator<< for writing a substr to a stream)
        _apply_seed();
        write(this, s);
        _RYML_CB_ASSERT(m_tree->m_callbacks, val() == s);
        return *this;
    }

    template<class T>
    inline NodeRef& operator<< (T const& C4_RESTRICT v)
    {
        _apply_seed();
        write(this, v);
        return *this;
    }

    /** serialize a variable, then assign the result to the node's key */
    template<class T>
    inline NodeRef& operator<< (Key<const T> const& C4_RESTRICT v)
    {
        _apply_seed();
        set_key_serialized(v.k);
        return *this;
    }

    /** serialize a variable, then assign the result to the node's key */
    template<class T>
    inline NodeRef& operator<< (Key<T> const& C4_RESTRICT v)
    {
        _apply_seed();
        set_key_serialized(v.k);
        return *this;
    }

    NodeRef& operator<< (Key<fmt::const_base64_wrapper> w)
    {
        set_key_serialized(w.wrapper);
        return *this;
    }

    NodeRef& operator<< (fmt::const_base64_wrapper w)
    {
        set_val_serialized(w);
        return *this;
    }

    /** @} */

private:

    void _apply_seed()
    {
        _C4RID();
        if(m_seed.str) // we have a seed key: use it to create the new child
        {
            m_id = m_tree->append_child(m_id);
            m_tree->_set_key(m_id, m_seed);
            m_seed.str = nullptr;
            m_seed.len = NONE;
        }
        else if(m_seed.len != NONE) // we have a seed index: create a child at that position
        {
            _RYML_CB_ASSERT(m_tree->m_callbacks, m_tree->num_children(m_id) == m_seed.len);
            m_id = m_tree->append_child(m_id);
            m_seed.str = nullptr;
            m_seed.len = NONE;
        }
        else
        {
            _RYML_CB_ASSERT(m_tree->m_callbacks, valid());
        }
    }

    inline void _apply(csubstr v)
    {
        m_tree->_set_val(m_id, v);
    }

    inline void _apply(NodeScalar const& v)
    {
        m_tree->_set_val(m_id, v);
    }

    inline void _apply(NodeInit const& i)
    {
        m_tree->_set(m_id, i);
    }

public:

    /** @name modification of hierarchy */
    /** @{ */

    inline NodeRef insert_child(NodeRef after)
    {
        _C4RV();
        _RYML_CB_ASSERT(m_tree->m_callbacks, after.m_tree == m_tree);
        NodeRef r(m_tree, m_tree->insert_child(m_id, after.m_id));
        return r;
    }

    inline NodeRef insert_child(NodeInit const& i, NodeRef after)
    {
        _C4RV();
        _RYML_CB_ASSERT(m_tree->m_callbacks, after.m_tree == m_tree);
        NodeRef r(m_tree, m_tree->insert_child(m_id, after.m_id));
        r._apply(i);
        return r;
    }

    inline NodeRef prepend_child()
    {
        _C4RV();
        NodeRef r(m_tree, m_tree->insert_child(m_id, NONE));
        return r;
    }

    inline NodeRef prepend_child(NodeInit const& i)
    {
        _C4RV();
        NodeRef r(m_tree, m_tree->insert_child(m_id, NONE));
        r._apply(i);
        return r;
    }

    inline NodeRef append_child()
    {
        _C4RV();
        NodeRef r(m_tree, m_tree->append_child(m_id));
        return r;
    }

    inline NodeRef append_child(NodeInit const& i)
    {
        _C4RV();
        NodeRef r(m_tree, m_tree->append_child(m_id));
        r._apply(i);
        return r;
    }

public:

    inline NodeRef insert_sibling(ConstNodeRef const& after)
    {
        _C4RV();
        _RYML_CB_ASSERT(m_tree->m_callbacks, after.m_tree == m_tree);
        NodeRef r(m_tree, m_tree->insert_sibling(m_id, after.m_id));
        return r;
    }

    inline NodeRef insert_sibling(NodeInit const& i, ConstNodeRef const& after)
    {
        _C4RV();
        _RYML_CB_ASSERT(m_tree->m_callbacks, after.m_tree == m_tree);
        NodeRef r(m_tree, m_tree->insert_sibling(m_id, after.m_id));
        r._apply(i);
        return r;
    }

    inline NodeRef prepend_sibling()
    {
        _C4RV();
        NodeRef r(m_tree, m_tree->prepend_sibling(m_id));
        return r;
    }

    inline NodeRef prepend_sibling(NodeInit const& i)
    {
        _C4RV();
        NodeRef r(m_tree, m_tree->prepend_sibling(m_id));
        r._apply(i);
        return r;
    }

    inline NodeRef append_sibling()
    {
        _C4RV();
        NodeRef r(m_tree, m_tree->append_sibling(m_id));
        return r;
    }

    inline NodeRef append_sibling(NodeInit const& i)
    {
        _C4RV();
        NodeRef r(m_tree, m_tree->append_sibling(m_id));
        r._apply(i);
        return r;
    }

public:

    inline void remove_child(NodeRef & child)
    {
        _C4RV();
        _RYML_CB_ASSERT(m_tree->m_callbacks, has_child(child));
        _RYML_CB_ASSERT(m_tree->m_callbacks, child.parent().id() == id());
        m_tree->remove(child.id());
        child.clear();
    }

    //! remove the nth child of this node
    inline void remove_child(size_t pos)
    {
        _C4RV();
        _RYML_CB_ASSERT(m_tree->m_callbacks, pos >= 0 && pos < num_children());
        size_t child = m_tree->child(m_id, pos);
        _RYML_CB_ASSERT(m_tree->m_callbacks, child != NONE);
        m_tree->remove(child);
    }

    //! remove a child by name
    inline void remove_child(csubstr key)
    {
        _C4RV();
        size_t child = m_tree->find_child(m_id, key);
        _RYML_CB_ASSERT(m_tree->m_callbacks, child != NONE);
        m_tree->remove(child);
    }

public:

    /** change the node's position within its parent, placing it after
     * @p after. To move to the first position in the parent, simply
     * pass an empty or default-constructed reference like this:
     * `n.move({})`. */
    inline void move(ConstNodeRef const& after)
    {
        _C4RV();
        m_tree->move(m_id, after.m_id);
    }

    /** move the node to a different @p parent (which may belong to a
     * different tree), placing it after @p after. When the
     * destination parent is in a new tree, then this node's tree
     * pointer is reset to the tree of the parent node. */
    inline void move(NodeRef const& parent, ConstNodeRef const& after)
    {
        _C4RV();
        if(parent.m_tree == m_tree)
        {
            m_tree->move(m_id, parent.m_id, after.m_id);
        }
        else
        {
            parent.m_tree->move(m_tree, m_id, parent.m_id, after.m_id);
            m_tree = parent.m_tree;
        }
    }

    /** duplicate the current node somewhere within its parent, and
     * place it after the node @p after. To place into the first
     * position of the parent, simply pass an empty or
     * default-constructed reference like this: `n.move({})`. */
    inline NodeRef duplicate(ConstNodeRef const& after) const
    {
        _C4RV();
        _RYML_CB_ASSERT(m_tree->m_callbacks, m_tree == after.m_tree || after.m_id == NONE);
        size_t dup = m_tree->duplicate(m_id, m_tree->parent(m_id), after.m_id);
        NodeRef r(m_tree, dup);
        return r;
    }

    /** duplicate the current node somewhere into a different @p parent
     * (possibly from a different tree), and place it after the node
     * @p after. To place into the first position of the parent,
     * simply pass an empty or default-constructed reference like
     * this: `n.move({})`. */
    inline NodeRef duplicate(NodeRef const& parent, ConstNodeRef const& after) const
    {
        _C4RV();
        _RYML_CB_ASSERT(m_tree->m_callbacks, parent.m_tree == after.m_tree || after.m_id == NONE);
        if(parent.m_tree == m_tree)
        {
            size_t dup = m_tree->duplicate(m_id, parent.m_id, after.m_id);
            NodeRef r(m_tree, dup);
            return r;
        }
        else
        {
            size_t dup = parent.m_tree->duplicate(m_tree, m_id, parent.m_id, after.m_id);
            NodeRef r(parent.m_tree, dup);
            return r;
        }
    }

    inline void duplicate_children(NodeRef const& parent, ConstNodeRef const& after) const
    {
        _C4RV();
        _RYML_CB_ASSERT(m_tree->m_callbacks, parent.m_tree == after.m_tree);
        if(parent.m_tree == m_tree)
        {
            m_tree->duplicate_children(m_id, parent.m_id, after.m_id);
        }
        else
        {
            parent.m_tree->duplicate_children(m_tree, m_id, parent.m_id, after.m_id);
        }
    }

    /** @} */

#undef _C4RV
};


//-----------------------------------------------------------------------------

inline ConstNodeRef::ConstNodeRef(NodeRef const& that)
    : m_tree(that.m_tree)
    , m_id(!that.is_seed() ? that.id() : NONE)
{
}

inline ConstNodeRef::ConstNodeRef(NodeRef && that)
    : m_tree(that.m_tree)
    , m_id(!that.is_seed() ? that.id() : NONE)
{
}


inline ConstNodeRef& ConstNodeRef::operator= (NodeRef const& that)
{
    m_tree = (that.m_tree);
    m_id = (!that.is_seed() ? that.id() : NONE);
    return *this;
}

inline ConstNodeRef& ConstNodeRef::operator= (NodeRef && that)
{
    m_tree = (that.m_tree);
    m_id = (!that.is_seed() ? that.id() : NONE);
    return *this;
}


//-----------------------------------------------------------------------------

template<class T>
inline void write(NodeRef *n, T const& v)
{
    n->set_val_serialized(v);
}

template<class T>
typename std::enable_if< ! std::is_floating_point<T>::value, bool>::type
inline read(NodeRef const& n, T *v)
{
    return from_chars(n.val(), v);
}
template<class T>
typename std::enable_if< ! std::is_floating_point<T>::value, bool>::type
inline read(ConstNodeRef const& n, T *v)
{
    return from_chars(n.val(), v);
}

template<class T>
typename std::enable_if<std::is_floating_point<T>::value, bool>::type
inline read(NodeRef const& n, T *v)
{
    return from_chars_float(n.val(), v);
}
template<class T>
typename std::enable_if<std::is_floating_point<T>::value, bool>::type
inline read(ConstNodeRef const& n, T *v)
{
    return from_chars_float(n.val(), v);
}


} // namespace yml
} // namespace c4



#ifdef __clang__
#   pragma clang diagnostic pop
#elif defined(__GNUC__)
#   pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#   pragma warning(pop)
#endif

#endif /* _C4_YML_NODE_HPP_ */
