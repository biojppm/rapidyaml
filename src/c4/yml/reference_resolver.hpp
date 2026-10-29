#ifndef C4_YML_REFERENCE_RESOLVER_HPP_
#define C4_YML_REFERENCE_RESOLVER_HPP_


#ifndef C4_YML_DETAIL_KNOWN_KEYS_HPP_
#include "c4/yml/detail/known_keys.hpp"
#endif
#ifndef C4_YML_NODE_TYPE_HPP_
#include "c4/yml/node_type.hpp"
#endif


namespace c4 {
namespace yml {

/** @addtogroup doc_ref_utils
 * @{
 */


//-----------------------------------------------------------------------------

/** Reusable object to resolve references/aliases in a @ref Tree. See
 * @ref ResolveOptions . */
struct ReferenceResolver
{
    ReferenceResolver() noexcept : ReferenceResolver(get_callbacks()) {}
    ReferenceResolver(Callbacks const& cb);

    /** Resolve references: for each reference, look for a matching
     * anchor, and copy its contents to the ref node.
     *
     * @p tree the subject tree
     *
     * @p opts options to control resolve behavior: see @ref ResolveOptions
     *
     * This method first does a full traversal of the tree to gather
     * all anchors and references in a separate collection, then it
     * goes through that collection to locate the names, which it does
     * by obeying the YAML standard diktat that "an alias node refers
     * to the most recent node in the serialization having the
     * specified anchor"
     *
     * So, depending on the number of anchor/alias nodes, this is a
     * potentially expensive operation, with a best-case linear
     * complexity (from the initial traversal). This potential cost is
     * one of the reasons for requiring an explicit call.
     *
     * The @ref Tree has a @ref Tree::resolve() overload set
     * forwarding here. Previously this operation was done there,
     * using a discarded object; using this separate class offers
     * opportunity for reuse of the object.
     *
     * @warning resolving references opens an attack vector when the
     * data is malicious or severely malformed, as the tree can expand
     * exponentially. See for example the [Billion Laughs
     * Attack](https://en.wikipedia.org/wiki/Billion_laughs_attack).
     *
     */
    RYML_EXPORT void resolve(Tree *tree, ResolveOptions const& opts={});

public:

    /** @cond dev */
    RYML_DEPRECATED("use the overload receiving ResolveOptions")
    void resolve(Tree *tree, bool clear_anchors)
    {
        resolve(tree, ResolveOptions{}.clear_anchors(clear_anchors));
    }
    /** @endcond */

public:

    /** @cond dev */

    struct RefData
    {
        NodeType type;
        id_type node;
        id_type prev_anchor;
        id_type target;
        id_type parent_ref;
    };
    struct AnchorData
    {
        id_type node;
        id_type visit_count;
        bool val_anchor;
    };
    using key_status = detail::KnownKeys::status_bits;

    void reset_(Tree *t_, ResolveOptions const& opts);
    id_type resolve_(id_type node, id_type count);
    id_type branch_size_(id_type node);
    void check_size_limit_(id_type node);

    bool requires_merge_(id_type dst_map, id_type visit_count);
    bool maybe_merge_keys_(id_type dst_map, id_type visit_count);

    bool is_merge_elm_(id_type child) const;
    void add_keys_mergedin_(id_type node, id_type visit_count);
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

    detail::small_map<csubstr, AnchorData> m_anchors;
    detail::KnownKeys m_known_keys;
    Tree *C4_RESTRICT m_tree;
    ResolveOptions m_opts;
    id_type m_size_limit;

    /** @endcond */
};

/** @} */

} // namespace ryml
} // namespace c4


#endif // C4_YML_REFERENCE_RESOLVER_HPP_
