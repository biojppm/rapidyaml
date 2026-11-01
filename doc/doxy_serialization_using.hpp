
#include "c4/yml/tree.hpp"
#include "c4/yml/node.hpp"
#include "c4/yml/scalar_charconv.hpp"

namespace c4 {
namespace yml {

/** @addtogroup doc_serialization_using

The top-level serialization functions are @ref NodeRef::save() / @ref
ConstNodeRef::load() to deserialize, and its tree equivalents @ref
Tree::save() / @ref Tree::load():

@code{c++}
ryml::NodeRef node = ...;
// serialize
node.save(42);
CHECK(node.val() == "42");
// deserialize
int answer = 0;
node.load(&answer);
CHECK(answer == 42);
@endcode
or, if you prefer the tree API,
@code{c++}
ryml::Tree tree;
ryml::id_type node_id = ...;
// serialize
tree.save(node_id, 42);
CHECK(tree.val(node_id) == "42");
// deserialize
int answer = 0;
tree.load(node_id, &answer);
CHECK(answer == 42);
@endcode

The functions for serializing keys are @ref NodeRef::save_key() to
serialize and @ref ConstNodeRef::load_key() to deserialize, and its
tree equivalents @ref Tree::save_key() and @ref Tree::load_key(). But
note that the Tree cannot handle container keys, so they must
serialized as a scalar.


<br>
<hr>
## Setting styles while serializing

When serializing scalars, it is advised to set explicit scalar styles when building
a tree for subsequently emitting. This is because YAML has constraints
on which styles can be used for a particular scalar.

When the scalar is not marked with an explicit style, the ryml emitter
adheres to these constraints at run time by scanning each scalar to
choose a style for it. On the other hand, if the scalar is marked
with an explicit style, the emitter does not have to do the scan.

So explicitly setting the style saves the emitter from having to scan
each scalar while emitting.

@code{c++}
ryml::Tree tree = parse_in_arena("map:");
ryml::NodeRef map = tree["map"];
// serialize with explicit style
map["single quoted"].save("scalar", ryml::VAL_PLAIN);
map["single quoted"].save("scalar", ryml::VAL_SQUO);
map["double quoted"].save("scalar", ryml::VAL_DQUO);
map["literal"].save("scalar", ryml::VAL_LITERAL);
map["folded"].save("scalar", ryml::VAL_FOLDED);
CHECK(emitrs_yaml<std::string>(tree),
    "map:\n"
    "plain: scalar\n"
    "single quoted: 'scalar'\n"
    "double quoted: \"scalar\"\n"
    "literal: |-\n
    "  \"scalar\"\n"
    "folded: >-\n
    "  \"scalar\"\n"
    );
@endcode

The optional style parameter can and should be also used with all
the tree building functions (eg, @ref NodeRef::set_val(), etc).


<br>
<hr>

## Checks: .load() vs  .deserialize()

`.load()` does a lot of hand-holding to ensure the preconditions are
met before attempting to read from the node, and it then checks the
success of the deserialization, triggering an error if anything
is wrong. To be clear, these are checks and not assertions, so they
are done regardless of build type.

This results generally in safer user code, but comes at a slight cost.
You can avoid this cost, if you wish. First, `.load()` has a parameter
that turns the readability check into an assertion, but keeps the
deserialization check:

@code{c++}
ryml::ConstNodeRef node = ...;
node.load(&answer, false); // disable readability check
@endcode
or, if you prefer the tree API
@code{c++}
ryml::Tree tree = ...;
ryml::id_type node_id = ...;
bool check_readable = false
tree.load(node_id, &answer, check_readable); // assumes the node is ok
@endcode

And further, you can avoid exceptional flow in the code by using
instead @ref ConstNodeRef::deserialize() or @ref
Tree::deserialize(). These functions attempt to deserialize and return
a `[[nodiscard]]` boolean with the deserialization status:

@code{c++}
ryml::ConstNodeRef node = ...;
if(... node is ok ...)
    if(!node.deserialize(&answer))
        ... // deserialization failed, react accordingly
@endcode
or, if you prefer the tree API
@code{c++}
ryml::Tree tree = ...;
ryml::id_type node_id = ...;
if(... node is ok ...)
    if(!tree.deserialize(node_id, &answer))
        ... // deserialization failed, react accordingly
@endcode

Note that `.deserialize()` still asserts readability.


<br>
<hr>

## Examples

 - See @ref sample_serialize_basic() and @ref doc_serialization_using
   for an overview of how use serialization in ryml.
 - See @ref sample_fundamental_types() for basic examples
   of serialization of fundamental types.
 - See @ref sample_empty_null_values() for different ways
   to serialize and deserialize empty and null values/
 - When serializing floating point values in C++ earlier than
   17, be aware that there may be a truncation of the precision
   with the default float/double implementations of @ref
   doc_to_chars. To enforce a particular precision, use for
   example @ref c4::fmt::real, or call directly @ref c4::ftoa() or
   @ref c4::dtoa(), or any other method (remember that ryml only
   stores the final string in the tree, so nothing prevents you from
   creating it in whatever way is most suitable). See the relevant
   sample: @ref sample_float_precision().
 - You can also serialize and deserialize base64: see @ref
   doc_base64 and @ref sample_base64

That's it! ryml provides built-in serialization/deserialization
utilities for all fundamental data types, so you're good to go if
that's all you are using. However, read on if you want to implement
serialization with your own types.

See now the basic sample @ref sample_serialize_basic() for a concrete
working example. And if you want to use serialization with your custom
user types, proceed to @ref doc_serialization_user_types.
*/

} // namespace yml
} // namespace c4
