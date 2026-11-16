#ifndef C4_YML_REFERENCE_RESOLVER_HPP_
#define C4_YML_REFERENCE_RESOLVER_HPP_


#ifndef C4_YML_DETAIL_KNOWN_KEYS_HPP_
#include "c4/yml/detail/known_keys.hpp"
#endif


#ifndef RYML_TREE_RESOLVER_SSO_SIZE
/// SSO size of the maps used in the
#define RYML_TREE_RESOLVER_SSO_SIZE (32)
#endif


namespace c4 {
namespace yml {

/** @addtogroup doc_ref_utils
 * @{
 */


/** Reusable object to resolve references/aliases or merge map keys in
 * a @ref Tree. See @ref ResolveOptions . */
struct ReferenceResolver
{
    ReferenceResolver() noexcept : ReferenceResolver(get_callbacks()) {}
    RYML_EXPORT ReferenceResolver(Callbacks const& cb);

    /** Resolve references and/or merge repeated map keys.
     *
     * @p tree the subject tree
     *
     * @p opts options to control resolve behavior: see @ref ResolveOptions
     *
     */
    RYML_EXPORT void resolve(Tree *tree, ResolveOptions const& opts={}, ResolveParams const& params={});

    /** @cond dev */

    struct AnchorData
    {
        id_type node;
        id_type visit_count;
        bool val_anchor;
    };
    using AnchorMap = detail::small_map<csubstr, AnchorData, RYML_TREE_RESOLVER_SSO_SIZE, /*stable*/true>;
    using BranchSizeMem = detail::stack<uint32_t, 2>; // we always expect this to be large
    using key_status = detail::KnownKeys::status_bits;

private:

    void reset_(Tree *t_, ResolveOptions const& opts, ResolveParams const& params);
    id_type resolve_(id_type node, id_type count);
    id_type branch_size_(id_type node);
    void grow_branch_size_mem_();
    void check_size_limit_(id_type node);

    bool has_merge_elms_(id_type map) const;
    bool requires_merge_(id_type dst_map, id_type visit_count);
    bool maybe_merge_keys_(id_type dst_map, id_type visit_count);
    bool insert_without_merge_(id_type dst_map, id_type visit_count);
    id_type insert_merge_node_(id_type dst_map, id_type ref_node, id_type after, id_type visit_count);

    bool is_merge_elm_(id_type child) const;
    C4_NODISCARD bool add_keys_mergedin_(id_type node, id_type visit_count);
    id_type gather_anchors_(id_type node, id_type count, bool *hasrefs);
    void add_anchor_(csubstr anch, id_type node, id_type count, bool val_anchor)
    {
        AnchorData &d = m_anchors.insert(anch);
        d.node = node;
        d.visit_count = count;
        d.val_anchor = val_anchor;

    }
    AnchorData const* lookup_anchor_(id_type ref_node, id_type count, csubstr ref);
    void handle_key_ref_(id_type ref_node, AnchorData const& anchor);
    void handle_val_ref_(id_type ref_node, AnchorData const& anchor);
    void clear_anchors_(id_type node);

public: // members exposed for testing. do not modify in client code!

    AnchorMap         m_anchors;
    BranchSizeMem     m_branch_size; // memoize branch size
    detail::KnownKeys m_known_keys; // to merge map keys
    Tree *C4_RESTRICT m_tree;
    ResolveOptions    m_opts;
    id_type           m_size_limit;

public:

    RYML_DEPRECATED("pass ResolveOptions instead of bool")
    void resolve(Tree *tree, bool clear_anchors)
    {
        resolve(tree, ResolveOptions{}.clear_anchors(clear_anchors));
    }

    /** @endcond */
};

/** @} */


/** @cond dev */
// extract pos to be used in the stable sort
C4_ALWAYS_INLINE id_type getpos(ReferenceResolver::AnchorData const& val) noexcept
{
    return val.visit_count;
}
/** @endcond */

} // namespace ryml
} // namespace c4


#endif // C4_YML_REFERENCE_RESOLVER_HPP_
