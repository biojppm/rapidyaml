#ifndef C4_YML_REFERENCE_RESOLVER_HPP_
#define C4_YML_REFERENCE_RESOLVER_HPP_


#ifndef C4_YML_RESOLVE_OPTIONS_HPP_
#include "c4/yml/resolve_options.hpp"
#endif
#ifndef C4_YML_DETAIL_STACK_HPP_
#include "c4/yml/detail/stack.hpp"
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
    ReferenceResolver() = default;

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
        id_type parent_ref_sibling;
    };

    void reset_(Tree *t_, ResolveOptions const& opts);
    void resolve_();
    void gather_anchors_and_refs_();
    void gather_anchors_and_refs_(id_type n);
    id_type count_anchors_and_refs_(id_type n);
    id_type branch_size_(id_type n);

    id_type lookup_(RefData const* C4_RESTRICT ra);

    detail::stack<RefData> m_refs; // We're using this purely as an array
    Tree *C4_RESTRICT m_tree;
    ResolveOptions m_opts;

    /** @endcond */
};

/** @} */

} // namespace ryml
} // namespace c4


#endif // C4_YML_REFERENCE_RESOLVER_HPP_
