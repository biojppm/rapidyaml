
// DANGER: Keep markdown []() links in a single line!!!
//
// doxygen is broken and fails to render the markdown links when
// they span multi lines.


#include "c4/yml/tree.hpp"
#include "c4/yml/node.hpp"
#include "c4/yml/scalar_charconv.hpp"

namespace c4 {
namespace yml {


/** @addtogroup doc_serialization_user_types

<br>
<hr>
## Serialization type categories

There are two distinct type categories to consider regarding YAML
serialization:

  - **Container types**. These represent a hierarchy of values (or
    containers) and must converted to/from a YAML map (@ref MAP) or
    sequence (@ref SEQ).

  - **Scalar types**. These types are encoded as scalars, but need
    to be transformed from/to their string representation in the
    YAML buffer.


A container type will always require child nodes in the tree. A scalar
type will always be a leaf (childless) node in the tree. Most of the
time, a scalar will be converted to string and not require any meta
info (like tags) or style flags set in the tree, but occasionally this
will be needed.

So in fact, from the implementation point of view, the categories are
the following:

  - **General types**. Require extra structure/info from the tree:
    child nodes (required by containers) and/or tags or extra @ref
    NodeType flags (required by some scalars).

  - **Scalar types**. These merely need to be converted to string and
    then set as scalars on the tree, without needing to set any tags
    or extra @ref NodeType flags.


To have rapidyaml interact with your types, you need to define functions
where this is done, and then the compiler will have rapidyaml call your
functions because of [C++'s ADL rules](http://en.cppreference.com/w/cpp/language/adl).

Briefly stated, these are the functions you need to implement, **under
your type's namespace**:

@code{c++}
// IMPORTANT: define under the namespace of T. Read note below.
namespace your_namespace {

// tree API implementation for general types (containers
// or scalars requiring extra info from the tree):
//
// needed only if you're deserializing T:
c4::yml::ReadResult read(c4::yml::Tree const *tree, c4::yml::id_type node_id, T* var);
// needed only if you're serializing T:
void write(c4::yml::Tree * tree, c4::yml::id_type node_id, T const& var);

// or...

// special case for scalars not needing interaction with the tree:
//
// needed only if you're deserializing T:
bool from_chars(c4::yml::csubstr str, T* var);
// needed only if you're serializing T:
size_t to_chars(c4::yml::substr buffer, T const& var);
// optional:
c4::yml::type_bits scalar_flags_val(T const& var); // set extra style flags on T vals
c4::yml::type_bits scalar_flags_key(T const& var); // set extra style flags on T keys

// or...

// special case for writing string scalars: no need to convert to chars!
// mark as string
template<> struct c4::is_string<T> : std::true_type {};
// instead of to_chars()
c4::yml::csubstr to_csubstr(T const& var);
// rest as above for scalars

} // namespace
@endcode


@important Because of [C++'s ADL
rules](http://en.cppreference.com/w/cpp/language/adl), **it is
required to overload these functions in the namespace of the type**
you're serializing. Here's an [example of an issue](https://github.com/biojppm/rapidyaml/issues/424)
where failing to do this was causing problems in some platforms.


You may also implement %read/write() using the node API instead of the
tree API (but read the following section for details):

@code{c++}
// IMPORTANT: define %read() under the namespace of T. Read note above.
namespace your_namespace {

// node API implementation for general types (old approach)
// needed only if you're deserializing T:
c4::yml::ReadResult read(c4::yml::ConstNodeRef node, T* var);
// needed only if you're serializing T:
void write(c4::yml::NodeRef * node, T const& var);

} // namespace
@endcode

@note For maximum flexibility you should prefer implementing the
tree %read/write.


Read on for details.


// <br>
// <hr>

## Why you should prefer implementing with tree API

You may have noticed above that there are two sets of functions: one
for the node API and another for the tree API. You don't need to
implement both. Simply put, the choice on which one to implement comes
down to which one you want to use, but for maximum flexibility
the **default advice is to implement the tree %read/write functions**.

Here are the key considerations:

  - If you trigger the deserialization from a particular API, it will
    directly call the corresponding %read/write() function. Further,
    rapidyaml's default implementation of node is calling into the tree
    %read/write(), so that if you only implement this one, it is
    automagically picked even if you're calling from nodes. For
    example:

    @code{c++}
    T var;

    // tree calls
    Tree tree = ...;
    id_type node_id = ...;
    node.load(&var)                  // calls read(Tree const*,id_type,T*)
    if(!node.deserialize(&var)) ...; // calls read(Tree const*,id_type,T*)
    tree.save(var);                  // calls write(Tree*,id_type,T const&)
    tree.set_serialized(&var);       // calls write(Tree*,id_type,T const&)

    // node calls - forwarding to tree by default
    NodeRef node = ...;
    node.load(&var);                 // calls read(ConstNodeRef const&,T*)
                                     //    -> rapidyaml calls read(Tree const*,id_type,T*)
    if(!node.deserialize(&var)) ...; // calls read(ConstNodeRef const&,T*)
                                     //    -> rapidyaml calls read(Tree const*,id_type,T*)
    node.save(var);                  // calls write(NodeRef*,T const&)
                                     //    -> rapidyaml calls write(Tree*,id_type,T const&)
    node.set_serialized(&var);       // calls write(NodeRef*,T const&)
                                     //    -> rapidyaml calls write(Tree*,id_type,T const&)
    @endcode

  - By default, a tree %read/write() impl will get called from a node
    call. rapidyaml's node impl calls into the tree impl. This means that
    if you implement the tree %read/write(), rapidyaml will pick it up
    **even if you are triggering it with the node API**.

  - If you implement node %read/write(), they will be picked up by a
    node call, but not by a tree call. Further, if you also implement
    tree %read/writes, they will only be picked up by a tree call.

  - If you implement node %read/write(), it hides rapidyaml's default
    implementation of calling the tree %read/write(), so if you then
    want to call tree deserialization, you will also need to implement
    tree %read/write().

So again, it is best to choose to implement the tree %read/write() functions.



// <br>
// <hr>

## Implementation notes: general types

As explained above, general types are those that require child nodes
(in the case of containers), or are scalars that require extra @ref
NodeType flags to be set along with it. For each type, the functions
you will to implement depend on whether you're reading or writing from
the tree/node.



// <br>
### Writing general types

When writing general types to YAML, you need to define the following
function:

@code{c++}
// implement these functions for T ...
namespace your_namespace {  // IMPORTANT read note about namespace above
void write(c4::yml::Tree *tree, c4::yml::id_type node_id, T const& var);
// or, if you want to use the node API,
void write(c4::yml::NodeRef *scalar, T const& var);
} // namespace
@endcode

Likewise, for writing keys you need to define the following function
(but note the key MUST be a scalar):

@code{c++}
// implement these functions for T ...
namespace your_namespace {  // IMPORTANT read note about namespace above
void write_key(c4::yml::Tree *tree, c4::yml::id_type node_id, T const& var);
// or, if you want to use the node API,
void write_key(c4::yml::NodeRef *scalar, T const& var);
} // namespace
@endcode

The requirements for `%write()` are less numerous than with
%read(). Inside `%write()`, you may assume the node is valid, as rapidyaml
will have made the required checks before calling your function, as
specified by the call triggering the %write (as described in @ref
doc_serialization_using).

As for what you can do inside `%write()`: generally you should only be
setting/adding things to the node, and not to its key (that
will generally have been dealt with elsewhere), typically with one of
[.set_seq()](@ref Tree::set_seq()) or
[.set_map()](@ref Tree::set_map()) for containers,
or [.set_val()](@ref Tree::set_val()) or
[.set_serialized()](@ref Tree::set_serialized()). Following this, for
containers you should create and populate the children, with further
calls to any of these functions, but now with child nodes and data
structures as the targets.


@note See examples of `%write()` implementations:
  - @ref doc_serialization_tree_write
  - @ref doc_serialization_node_write
  - see the [vector write implementation](@ref src/c4/yml/std/vector.hpp)
  - see the [map write implementation](@ref src/c4/yml/std/map.hpp).
  - see the sample @ref sample_user_container_types
  - see the sample @ref sample_std_types



// <br>
### Reading general types

To enable reading (deserialization) of a custom user type T falling
into the general category, you need to define the following function:

@code{c++}
// IMPORTANT: define read() under the namespace of T. Read warning above.
namespace your_namespace {
c4::yml::ReadResult read(c4::yml::Tree const *tree, c4::yml::id_type node_id, T* var);
// and/or, if you prefer the node API
c4::yml::ReadResult read(c4::yml::ConstNodeRef node, T* var);
} // namespace
@endcode

Likewise, for reading keys you need to define the following function:
@code{c++}
// IMPORTANT: define %read_key() under the namespace of T. Read warning above.
namespace your_namespace {
c4::yml::ReadResult read_key(c4::yml::Tree const *tree, c4::yml::id_type node_id, T* var);
// and/or, if you prefer the node API
c4::yml::ReadResult read_key(c4::yml::ConstNodeRef node, T* var);
} // namespace
@endcode


Then when you call any of @ref NodeRef::load(), @ref
NodeRef::deserialize(), @ref Tree::load() or @ref Tree::deserialize()
(as described in @ref doc_serialization_using), rapidyaml will call
your `%read()` function through the magic of C++ ADL / Koenig
lookup. And likewise, when you call any of @ref NodeRef::load_key(),
@ref NodeRef::deserialize_key(), @ref Tree::load_key() or @ref
Tree::deserialize_key() (as described in @ref
doc_serialization_using), rapidyaml will call your `%read_key()`
function. (**But note the rapidyaml tree cannot accept containers as
keys!**)


The @ref ReadResult return type is a lightweight truthy type, used to
enable reporting either of success or of the offending node, when an
error happens in nested reads. It evaluates as true
(empty-initialized) when there is no error, or as false on error, and
has the innermost node causing the error. This enables accurate error
reporting, and is very useful on large YAML files; see also @ref
sample_location_tracking() to find the original source location of the
offending node.



To start with an example, here is the rapidyaml implementation of `%read()` for
`std::map`:

@code{c++}
template<class K, class V, class Less, class Alloc>
c4::yml::ReadResult read(c4::yml::Tree const* tree, c4::yml::id_type id, std::map<K, V, Less, Alloc> * m)
{
    // RULE 0. you may assume tree and id are valid.
    if(!tree->is_map(id))  // RULE 1. check node type
        return c4::yml::ReadResult(id); // report error on this id
    for(id_type child = tree->first_child(id); child != NONE; child = tree->next_sibling(child))
    {
        K k{};
        // RULE 2. use .deserialize(), not .load()
        c4::yml::ReadResult result = tree->deserialize_key(child, &k);
        if((!result))
            return result; // RULE 3. early exit on error
        result = tree->deserialize(child, &(*m)[std::move(k)]);
        if(!result)
            return result; // may refer to a deeply nested node!
    }
    return ReadResult{}; // report success
}
@endcode


<br>
The beginning rule is actually an assumption:

@important Rule 0. Inside your implementation of `%read()` or
`%read_key()`, you may assume the node is valid (ie, that the tree and
node_id are valid).

rapidyaml will already have checked for this as specified by the
triggering call (see @ref doc_serialization_using).


<br>
Now the first rule:

@important Rule 1. Inside `%read()`, **start with a node type check**:
must be exactly one of @ref VAL (for scalars), @ref SEQ (for sequence
types) or @ref MAP (for dictionary types). `%read_key()` *does not
require* a @ref KEY check.

This is needed to ensure that the node type matches the type of the
destination variable. Concretely:

   - If you're reading a scalar type like a number or a string, the
     node must be @ref VAL, ie it must verify @ref NodeType::has_val().

   - If you're reading a sequence type like a vector, the node must be
     a @ref SEQ, ie it should verify @ref NodeType::is_seq().

   - If you're reading a map type, the node should be a @ref VAL, ie
     it should verify @ref NodeType::is_map().

Why can't rapidyaml do this check for you before calling your `%read()`
function? Well, in the general case, it is impossible to know what type
of node to expect, so rapidyaml can only check that the node is one of
the @ref VAL|@ref SEQ|@ref MAP cases above, but not concretely which
one. It is up to the `%read()` implementation for a type to specify
which one.

However, note that inside `%read_key()` you do not need a type check,
as the rapidyaml tree requires that these are scalars (ie @ref KEY),
so rapidyaml does this check for you before calling `%read_key()`.


<br>
Now the next rule:

@important Rule 2. Inside `%read()`, **use
[.deserialize()](@ref Tree::deserialize()) and not
[.load()](@ref Tree::load())**, to play nice with `.deserialize()`
callers calling your function. For `%read_key()` it should be
[.deserialize_key()](@ref Tree::deserialize_key()) instead
of [.load_key()](@ref Tree::load_key()).


`.load()` triggers an error, while `.deserialize()` just returns, so
you don't want to have a `.deserialize()` caller being aborted by a
nested `.load()` call in your function. Let the top-level `.load()`
caller trigger the error.


<br>
Finally,

@important Rule 3. **Check every read and do early exit on error**,
adequately filling the @ref ReadResult return type.

Your implementation of `%read()` or `%read_key()` **must return a
truthy type to signify success of deserialization**. The type should
preferably be a @ref ReadResult to enable accurate error reporting.

If the type is not @ref ReadResult (like the legacy bool), rapidyaml
will still work -- although with the inconvenience of pointing only at the
outer-most node instead of the actual error-causing node.

With this return value, rapidyaml will continue on success; on failure
it will either return this value to the caller (with `.deserialize()`)
or with `.load()` trigger a visit error on the reported node, as
instructed by the triggering call (see @ref doc_serialization_using).

That's it for `%read()`!

@note See examples of `%read()` implementations:
  - @ref doc_serialization_tree_read
  - @ref doc_serialization_node_read
  - see the [vector read implementation](@ref src/c4/yml/std/vector.hpp)
  - see the [map read implementation](@ref src/c4/yml/std/map.hpp).
  - see the sample @ref sample_user_container_types
  - see the sample @ref sample_std_types




<br>
<hr>

## Implementation notes: scalars

When a scalar type does not require any style or tags to be set in the
tree, instead of defining `%read()` / `%write()` you can just define
the direct serialization functions `%from_chars()` and/or
`%to_chars()` to transform the scalar from/to its string
representation.

@note Please take note of the following pitfall when using scalar
serialization functions: you may have to include the header with your
`%from_chars()` / `%to_chars()` implementation before any other headers
that use functions from it.


<br>
### Reading scalars

To implement reading (deserialization) of scalar types, you
need to define the following function:

@code{c++}
namespace your_namespace {
bool from_chars(c4::yml::csubstr str, T* var); // if you want to read from YAML
} // namespace
@endcode

The function receives a string fitted to the scalar, and must convert
the string to the argument. To achieve this, you may find it useful to
use the utilities in @ref doc_charconv or @ref doc_format, which are
very fast and efficient, and play nice with this approach. But that's
not mandatory -- you are also free to use any other conversion method
you choose, such as fmtlib (but please do not use stringstreams; their
performance is really bad).

Finally, you must return a boolean success status. rapidyaml will then
react to this status in accordance with the call triggering the read.

@note See examples of `%from_chars()` implementations:
  - for `std::string`: @ref ext/c4core/src/c4/std/string.hpp
  - for `std::vector<char>`: @ref ext/c4core/src/c4/std/vector.hpp
  - for `std::span<char>`: @ref ext/c4core/src/c4/std/span.hpp
  - see the several from_chars overloads in @ref doc_charconv
  - see the several from_chars overloads in @ref doc_format


<br>
### Writing scalars

To implement writing (serialization) of scalar types, you
need to define the following function:

@code{c++}
namespace your_namespace {
size_t to_chars(c4::yml::substr buffer, T const& var); // if you want to write to YAML
} // namespace
@endcode

This function receives a buffer on which it is to write the
serialization of var. Importantly, inside your function **you cannot
assume the buffer is large enough** to fit the serialization of
var. You must always check against its size.

You must return the number of bytes required to fit the serialization
of var. Importantly, this size must not depend on the size of the
buffer, which means **you cannot do an early exit** when you find the
buffer is too small. The returned size must be invariant.

Upon returning, the caller will compare the returned size with the
current buffer size. If the returned size is >= than the buffer size,
it means the serialization succeeded, and we're done. Otherwise, it
means the buffer was too small; then rapidyaml will resize the buffer
and call the function again. For an example of this call pattern, see
eg @ref serialize_to_arena_scalar().

A typical implementation of `%to_chars()` will look like this:

@code{c++}
namespace your_namespace {
size_t to_chars(c4::yml::substr buffer, T const& var)
{
    size_t pos = 0;
    for(... var) // iterate over var, adding characters to the buffer
    {
        // append another char to the buffer: only if possible!
        // BUT do not break the loop if the buffer is too small.
        // Continue doing a blank loop until the end, to count
        // the needed characters
        if(pos < buffer.len)
            buffer[pos] = ...;
        ++pos; // keep counting, even if we already know
               // the buffer is small!
    }
    return pos; // now we know the required size, return it
}
} // namespace
@endcode

For instance, if your T is a string type, you could do:

@code{c++}
namespace your_namespace {
size_t to_chars(c4::yml::substr buffer, T const& var)
{
    size_t sz = var.size();
    if(sz && sz <= buffer.len)
        memcpy(buffer.str, var.data(), sz);
    return sz;
}
} // namespace
@endcode

@note See examples of `%to_chars()` implementations:
  - for `std::string`: @ref ext/c4core/src/c4/std/string.hpp
  - for `std::string_view`: @ref ext/c4core/src/c4/std/string_view.hpp
  - for `std::vector<char>`: @ref ext/c4core/src/c4/std/vector.hpp
  - for `std::span<char>`: @ref ext/c4core/src/c4/std/span.hpp
  - see the several to_chars overloads in @ref doc_charconv
  - see the several to_chars overloads in @ref doc_format


<br>
### Further reading for scalar serialization

  - See the sample @ref sample_user_scalar_types
  - See the sample @ref sample_formatting for examples
    of functions from @ref doc_format_utils that will be very
    helpful in implementing custom @ref to_chars() / @ref from_chars()
    functions.
  - See @ref doc_charconv for the example implementations of
    @ref to_chars() / @ref from_chars() for the fundamental types.
  - See @ref doc_substr and @ref sample_substr() for the
    many useful utilities in the substring class.
  - See quickstart examples on how to @ref doc_sample_scalar_types

*/


} // namespace yml
} // namespace c4
