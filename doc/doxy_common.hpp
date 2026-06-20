namespace c4 {
namespace yml {


/** @defgroup doc_quickstart Quickstart
 *
 * Example code for every feature.
 */


/** @defgroup doc_parse Parse utilities
 * @see @ref sample_parse_in_place()
 * @see @ref sample_parse_in_arena()
 * @see @ref sample_parse_file()
 * @see @ref sample_parse_reuse_tree()
 * @see @ref sample_parse_reuse_parser()
 * @see @ref sample_parse_reuse_tree_and_parser()
 * @see @ref sample_location_tracking()
 */



/** @defgroup doc_node_type Node types
 */


/** @defgroup doc_tree Tree utilities
 * @see @ref sample_quick_overview()
 * @see @ref sample_iterate_tree()
 * @see @ref sample_create_tree()
 * @see @ref sample_tree_arena()
 *
 * @see @ref sample_static_trees()
 * @see @ref sample_location_tracking()
 *
 * @see @ref sample_docs()
 * @see @ref sample_anchors_and_aliases()
 * @see @ref sample_tags()
 */


/** @defgroup doc_node_classes Node classes
 *
 * High-level node classes.
 *
 * @see @ref sample_quick_overview()
 * @see @ref sample_iterate_tree()
 * @see @ref sample_create_tree()
 * @see @ref sample_create_tree_style()
 * @see @ref sample_tree_arena()
 */


/** @defgroup doc_emit Emit utilities
 *
 * Utilities to emit YAML and JSON, either to a memory buffer or to a
 * file or ostream-like class.
 *
 * @see @ref sample_emit_to_container()
 * @see @ref sample_emit_to_stream()
 * @see @ref sample_emit_to_file()
 * @see @ref sample_emit_nested_node()
 * @see @ref sample_style()
 */

/** @defgroup doc_emit_to_buffer Emit to a memory buffer
 * @ingroup doc_emit
 */
/** @defgroup doc_emit_to_buffer_from_root Emit full tree
 * @ingroup doc_emit_to_buffer
 */
/** @defgroup doc_emit_to_buffer_from_node_id Emit from nested node id
 * @ingroup doc_emit_to_buffer
 */
/** @defgroup doc_emit_to_buffer_from_noderef Emit from ConstNodeRef
 * @ingroup doc_emit_to_buffer
 */

/** @defgroup doc_emit_to_container Emit to a container
 * Overloads to emit to contiguous memory container of char like std::string or std::vector<char>
 * @ingroup doc_emit
 */
/** @defgroup doc_emit_to_container_from_root Emit full tree
 * @ingroup doc_emit_to_container
 */
/** @defgroup doc_emit_to_container_from_node_id Emit from nested node id
 * @ingroup doc_emit_to_container
 */
/** @defgroup doc_emit_to_container_from_noderef Emit from ConstNodeRef
 * @ingroup doc_emit_to_container
 */

/** @defgroup doc_emit_to_file Emit to file
 * @ingroup doc_emit
 */
/** @defgroup doc_emit_to_file_from_root Emit full tree
 * @ingroup doc_emit_to_file
 */
/** @defgroup doc_emit_to_file_from_node_id Emit from nested node id
 * @ingroup doc_emit_to_file
 */
/** @defgroup doc_emit_to_file_from_noderef Emit from ConstNodeRef
 * @ingroup doc_emit_to_file
 */

/** @defgroup doc_emit_to_ostream Emit to an STL-like ostream
 * @ingroup doc_emit
 */

/** @defgroup doc_writers Writer objects to use with an Emitter
 * @ingroup doc_emit
 */



/** @defgroup doc_serialization Serialization/deserialization
 */
/** @defgroup doc_serialization_using How to use (de)serialization
 *
 * Shows basic use of the calls to serialize and deserialize.
 *
 * @ingroup doc_serialization
 * @see @ref sample_serialize_basic()
 * @see @ref doc_serialization_user_types to implement for user types
 */
// don't wrap markdown links! doxygen won't render them.
/** @defgroup doc_serialization_user_types Serialization of user types
 * @ingroup doc_serialization
 *
 * Shows how to implement serialization for custom user types.
 * rapidyaml provides a serialization implementation for all
 * fundamental types, and optionally for some STL containers. To
 * enable use with any type, rapidyaml uses
 * [ADL to dispatch to type-specific](http://en.cppreference.com/w/cpp/language/adl)
 * function overloads. For serializing or deserializing custom user
 * types, you only need to provide the appropriate overloads, which
 * are explained here.
 *
 * @see @ref doc_serialization_overview to find how the user-provided functions fit into rapidyaml's serialization pipeline.
 */
/** @defgroup doc_serialization_overview Serialization overview
 * Shows how user-provided functions fit into ryml's structure.
 * @ingroup doc_serialization
 */

/** @defgroup doc_serialization_tree Serialization for Tree
 * Shows ryml's default implementation of tree serialization
 * @ingroup doc_serialization
 */
/** @defgroup doc_serialization_tree_read Read from Tree
 * Implementation of deserialization from a tree
 * @ingroup doc_serialization_tree
 */
/** @defgroup doc_serialization_tree_write Write to Tree
 * Implementation of serialization to a tree
 * @ingroup doc_serialization_tree
 */
/** @defgroup doc_serialization_tree_write_arena Arena utils
 * Implementation utils for serializing scalars in a tree's arena.
 *
 * These functions are implementation helpers used by @ref
 * Tree::to_arena() and @ref NodeRef::to_arena() to grow the tree
 * arena as needed and serialize scalars in it. There should be no
 * reason to call these functions directly.
 *
 * They can be overriden by the user to customize behavior for a user
 * type, but generally there should be no reason to do so -- if you
 * think there is, please open an issue in github.
 *
 * @ingroup doc_serialization_tree_write
 */

/** @defgroup doc_serialization_node Serialization for nodes
 * Shows ryml's default implementation of node serialization
 * @ingroup doc_serialization
 */
/** @defgroup doc_serialization_node_write Write to NodeRef
 * @ingroup doc_serialization_node
 */
/** @defgroup doc_serialization_node_read Read from ConstNodeRef
 * @ingroup doc_serialization_node
 */


/** @defgroup doc_ref_utils Anchor/Reference utilities
 *
 * @see @ref sample_anchors_and_aliases()
 * @see @ref sample_anchors_and_aliases_create()
 * */

/** @defgroup doc_tag_utils Tag utilities
 * @see @ref sample_tags()
 */

/** @defgroup doc_file_utils File utils
 *
 * Functions for loading/saving a file from/to disk.
 */


/** @defgroup doc_scalar Scalar utilities
 */
/** @defgroup doc_scalar_style Scalar style helpers
 * These functions are used by the emitter to choose a scalar style when a scalar does not have it set.
 * @ingroup doc_scalar
 * @ingroup doc_emit
 */
/** @defgroup doc_scalar_charconv Scalar charconv helpers
 * These functions are used by the parser and emitter to convert scalars to and from chars.
 * @ingroup doc_scalar
 * @ingroup doc_emit
 * @ingroup doc_parse
 * @ingroup doc_serialization
 */
/** @defgroup doc_scalar_filter Scalar filter processors
 *
 * These are internal utilities used by @ref ParseEngine to filter
 * YAML scalars; normally there is no reason for a user to be manually
 * using these classes.
 *
 * @ingroup doc_scalar
 * @ingroup doc_parse */


/** @defgroup doc_error_handling Error handling
 *
 * Utilities to report handle errors, and to build and report error
 * messages.
 *
 * @see @ref sample_error_handler()
 */

/** @defgroup doc_callbacks Callbacks for errors and allocation
 *
 * Functions called by ryml to allocate/free memory and to report
 * errors.
 *
 * @see @ref sample_error_handler()
 * @see @ref sample_global_allocator()
 * @see @ref sample_per_tree_allocator()
 */


// document macros for doxygen

/** define this macro with a boolean value to enable/disable
 * assertions to check preconditions and assumptions throughout the
 * codebase; this causes a slowdown of the code, and larger code
 * size. By default, this macro is defined unless NDEBUG is defined
 * (see C4_USE_ASSERT); as a result, by default this macro is truthy
 * only in debug builds. */
#   define RYML_USE_ASSERT

/** (Undefined by default) Define this macro to disable ryml's default
 * implementation of the callback functions. See @ref doc_callbacks.  */
#   define RYML_NO_DEFAULT_CALLBACKS

/** (Undefined by default) When this macro is defined (and
 * @ref RYML_NO_DEFAULT_CALLBACKS is not defined), the default error
 * handler will throw exceptions. See @ref doc_error_handling. */
#   define RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS

/** Conditionally expands to `noexcept` when @ref RYML_USE_ASSERT is 0 and
 * is empty otherwise. The user is unable to override this macro. */
#   define RYML_NOEXCEPT

/** (Undefined by default) Use shorter error message from
 * checks/asserts: do not show the check condition in the error
 * message. */
#   defined RYML_SHORT_CHECK_MSG


} // namespace yml
} // namespace c4
