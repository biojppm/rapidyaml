#ifndef _C4_YML_EMIT_OSTREAM_HPP_
#define _C4_YML_EMIT_OSTREAM_HPP_

/** @file emit_ostream.hpp */

#ifndef _C4_YML_EMITTER_HPP_
#include "c4/yml/emitter.hpp"
#endif

#ifndef _C4_YML_WRITER_OSTREAM_HPP_
#include "c4/yml/writer_ostream.hpp"
#endif
#ifndef _C4_YML_TREE_HPP_
#include "c4/yml/tree.hpp"
#endif
#ifndef _C4_YML_NODE_HPP_
#include "c4/yml/node.hpp"
#endif

#ifndef _C4_YML_EMITTER_DEF_HPP_
#include "c4/yml/emitter.def.hpp"
#endif


namespace c4 {
namespace yml {


/** @addtogroup doc_emit_to_ostream
 *
 * @{
 */

template<class OStream>
using EmitterOStream = Emitter<WriterOStream<OStream>>;


/** tag type to mark a tree or node to be emitted as yaml when using
 * @ref operator<<, with options. For example:
 *
 * ```cpp
 * Tree t = parse_in_arena("{foo: bar}");
 * std::cout << t; // emits YAML
 * std::cout << as_yaml(t); // emits YAML, same as above
 * std::cout << as_yaml(t, EmitOptions{}.flow_spc(true)); // emits YAML forcing spaces after flow commas
 * ```
 *
 * @see @ref operator<< */
struct as_json
{
    Tree const* tree;
    id_type node;
    EmitOptions options;
    as_json(Tree const& t, EmitOptions const& opts={}) : tree(&t), node(t.empty() ? NONE : t.root_id()), options(opts)  {}
    as_json(Tree const& t, id_type id, EmitOptions const& opts={}) : tree(&t), node(id), options(opts)  {}
    as_json(ConstNodeRef const& n, EmitOptions const& opts={}) : tree(n.tree()), node(n.id()), options(opts) {}
};

/** tag type to mark a tree or node to be emitted as yaml when using
 * @ref operator<< . For example:
 *
 * ```cpp
 * Tree t = parse_in_arena("{foo: bar}");
 * std::cout << t; // emits YAML
 * std::cout << as_json(t); // emits JSON
 * std::cout << as_json(t, EmitOptions{}.flow_spc(true)); // emits JSON forcing spaces after flow commas
 * ```
 *
 * @see @ref operator<< */
struct as_yaml
{
    Tree const* tree;
    id_type node;
    EmitOptions options;
    as_yaml(Tree const& t, EmitOptions const& opts={}) : tree(&t), node(t.empty() ? NONE : t.root_id()), options(opts)  {}
    as_yaml(Tree const& t, id_type id, EmitOptions const& opts={}) : tree(&t), node(id), options(opts)  {}
    as_yaml(ConstNodeRef const& n, EmitOptions const& opts={}) : tree(n.tree()), node(n.id()), options(opts) {}
};


/** emit YAML to an STL-like ostream */
template<class OStream>
inline OStream& operator<< (OStream& stream, Tree const& tree)
{
    EmitterOStream<OStream> em(EmitOptions{}, &stream);
    em.emit_as(EMIT_YAML, &tree);
    return stream;
}

/** emit YAML to an STL-like ostream
 * @overload */
template<class OStream>
inline OStream& operator<< (OStream& stream, ConstNodeRef const& node)
{
    if(!node.readable())
        return stream;
    EmitterOStream<OStream> em(EmitOptions{}, &stream);
    em.emit_as(EMIT_YAML, node.tree(), node.id());
    return stream;
}

/** emit JSON to an STL-like stream */
template<class OStream>
inline OStream& operator<< (OStream& stream, as_json const& json_spec)
{
    if(!json_spec.tree || json_spec.tree->empty())
        return stream;
    EmitterOStream<OStream> em(json_spec.options, &stream);
    id_type node_id = json_spec.node != NONE ? json_spec.node : json_spec.tree->root_id();
    em.emit_as(EMIT_JSON, json_spec.tree, node_id);
    return stream;
}

/** emit YAML to an STL-like stream */
template<class OStream>
inline OStream& operator<< (OStream& stream, as_yaml const& yaml_spec)
{
    if(!yaml_spec.tree || yaml_spec.tree->empty())
        return stream;
    EmitterOStream<OStream> em(yaml_spec.options, &stream);
    id_type node_id = yaml_spec.node != NONE ? yaml_spec.node : yaml_spec.tree->root_id();
    em.emit_as(EMIT_YAML, yaml_spec.tree, node_id);
    return stream;
}

/** @} */


} // namespace yml
} // namespace c4

#endif /* _C4_YML_EMIT_OSTREAM_HPP_ */
