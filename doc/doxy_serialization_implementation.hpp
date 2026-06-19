
#include "c4/yml/tree.hpp"
#include "c4/yml/node.hpp"
#include "c4/yml/scalar_charconv.hpp"

namespace c4 {
namespace yml {


// DANGER: Keep markdown []() links in a single line!!!
//
// doxygen is broken and fails to render the markdown links when
// they span multi lines.
//

/** @addtogroup doc_serialization_overview


# Implementation overview


@note If you are implementing serialization functions for your type,
you should first skim over the info here, and then remember to return
to this if you're in trouble.

This describes how ryml implements the serialization pipeline. This is
useful to understand how the user-provided read() and write()
functions fit and interact with ryml's implementation.


Note also that in a pinch, you can override any non-member function
mentioned below; however, there shouldn't be any reason to do so
unless explicitly noted. If you do feel the need to override other
functions, please submit also an issue at
https://github.com/biojppm/rapidyaml/issues to discuss whether this is
an API oversight or maybe a misunderstanding, and hopefully this
documentation can be improved.


// <br>
// <hr>

## Serialization (write)


ryml implements serialization in the following way:

 - The user-facing @ref NodeRef functions are
   [.save()](@ref c4::yml::NodeRef::save()) and
   [.save_key()](@ref c4::yml::NodeRef::save_key())
   (where the node is checked), and
   [.set_serialized()](@ref c4::yml::NodeRef::set_serialized()) and
   [.set_key_serialized()](@ref c4::yml::NodeRef::set_key_serialized())
   (where the node is merely asserted). After the validity check/assert,
   these functions then call...

 - ... the [node write() overloads](@ref doc_serialization_node_write). **These
   functions can be overrided for user types**. The default
   implementations (provided by ryml) merely forward to...:

 - ... the user-facing @ref Tree functions, which are
   [.save()](@ref c4::yml::Tree::save()) and
   [.save_key()](@ref c4::yml::Tree::save_key()), as well as
   [.set_serialized()](@ref c4::yml::Tree::set_serialized()) and
   [.set_key_serialized()](@ref c4::yml::Tree::set_key_serialized()).
   Like their node counterparts, these do a value existence check and then call...

 - ... the [tree write()/write_key() overloads](@ref doc_serialization_tree_write),
   which **again can be overrided for user types**. The default
   implementations of these functions now get into the nitty-gritty
   of serialization by calling...

   - [serialize_to_arena()](@ref c4::yml::serialize_to_arena())
     which uses trait @ref c4::is_string<T> to dispatch to...

     - either [serialize_to_arena_str()](@ref c4::yml::serialize_to_arena_str()) for strings,
       which grows the arena and copies the string to the arena

     - or [serialize_to_arena_scalar()](@ref c4::yml::serialize_to_arena_scalar()),
       which grows the arena and calls...

       - [scalar_serialize()](@ref c4::yml::scalar_serialize()) to dispatch to...

         - either [to_chars()](@ref c4::yml::to_chars()) for
           non-floating scalar types, **including user scalar
           types**. **This function can be overrided for scalar
           user-types **

         - or [to_chars_float()](@ref c4::yml::to_chars_float()) for
           floating scalar types, which trims leading +, checks for
           special YAML value strings eg `.nan` or `.inf`, and then
           dispatches to @ref c4::ftoa() or @ref c4::dtoa()



// <br>
// <hr>

## Deserialization (read)

ryml implements deserialization in the following way:

 - User facing @ref ConstNodeRef functions
   [.load()](@ref c4::yml::ConstNodeRef::load()) and
   [.load_key()](@ref c4::yml::ConstNodeRef::load_key())
   (which check node and deserialization), and
   [.deserialize()](@ref c4::yml::ConstNodeRef::deserialize()) and
   [.deserialize_key()](@ref c4::yml::ConstNodeRef::deserialize_key())
   (which assert node and
   check serialization). After the validity check/assert, these functions call...

 - ... the [node read() overloads](@ref doc_serialization_node_read). **These
   functions can be overrided for user types**. The default
   implementations (provided by ryml) merely forward to...:

 - ... the user-facing @ref Tree functions, which are
   [.load()](@ref c4::yml::Tree::load()) and
   [.load_key()](@ref c4::yml::Tree::load_key()) (which check node and deserialization),
   as well as [.deserialize()](@ref c4::yml::Tree::deserialize()) and
   [.deserialize_key()](@ref c4::yml::Tree::deserialize_key())
   (which assert node and check serialization).
   Like their node counterparts, these do a value existence check
   and then call...

 - ... the [tree read()/read_key() overloads](@ref doc_serialization_tree_read),
   which **again can be overrided for user types**. The default
   implementations of these functions first check that the node
   [.has_val()](@ref c4::yml::Tree::has_val()) (or key) and then
   get into the nitty-gritty off deserialization by calling...

     - [scalar_deserialize()](@ref c4::yml::scalar_deserialize()) for
       standard scalars passed as pointers to read(), which is a
       dispatcher to...

        - either [from_chars_float()](@ref c4::yml::from_chars_float())
          for floating types, which trims leading `+` and compares against
          special YAML float strings such as `.nan` or `.inf`, and finally
          calls @ref c4::from_chars() which dispatches to either
          @ref c4::atoi() or @ref c4::atof(), or ...

        - [from_chars_integral()](@ref c4::yml::from_chars_integral())
          for integral (arithmetic, non-float) types, which trims
          leading `+` and finally calls @ref c4::from_chars() wrapping
          either @ref c4::atoi() or @ref c4::atou()

        - and for all other types call from_chars(). **This function
          can be overrided for scalar user-types**.

     - or [from_chars()](@ref from_chars()) directly for wrapper types
       like eg @ref c4::fmt::base64() that wrap the destination with
       some meta-info related with deserialization. **This function
       can be overrided for wrapper user-types**.

*/


} // namespace yml
} // namespace c4
