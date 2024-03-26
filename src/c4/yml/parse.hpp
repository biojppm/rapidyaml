#ifndef _C4_YML_PARSE_HPP_
#define _C4_YML_PARSE_HPP_

#ifndef _C4_YML_COMMON_HPP_
#include "c4/yml/common.hpp"
#endif

namespace c4 {
namespace yml {

struct EventHandlerTree;
template<class EventHandler> class ParseEngine;
class NodeRef;
class Tree;

/** This is the main parser, where the parser events are handled to
 * create a ryml tree. */
using Parser = ParseEngine<EventHandlerTree>;


//-----------------------------------------------------------------------------

/** @name parse_in_place__with_existing_parser
 *
 * @desc parse a mutable YAML source buffer. Scalars requiring
 * filtering are mutated in place (except in the rare cases where the
 * filtered scalar is longer than the original scalar, or where
 * filtering was disabled before the call). These overloads accept an
 * existing parser object, and provide the opportunity to use special
 * parser options.
 *
 * @see ParserOptions
 *
 * @{
 */

void parse_in_place(Parser *parser, csubstr filename, substr yaml, Tree *t, size_t node_id); /**< (1) parse YAML into an existing tree node.
                                                                                              *
                                                                                              * The filename will be used in any error messages
                                                                                              * arising during the parse. The callbacks in the
                                                                                              * tree are kept, and used to allocate
                                                                                              * the tree members, if any allocation is required. */
void parse_in_place(Parser *parser,                   substr yaml, Tree *t, size_t node_id); /**< (2) like (1) but no filename will be reported */
void parse_in_place(Parser *parser, csubstr filename, substr yaml, Tree *t                ); /**< (3) parse YAML into an existing tree, into its root node.
                                                                                              *
                                                                                              * The filename will be used in any error messages
                                                                                              * arising during the parse. The callbacks in the
                                                                                              * tree are kept, and used to allocate
                                                                                              * the tree members, if any allocation is required. */
void parse_in_place(Parser *parser,                   substr yaml, Tree *t                ); /**< (4) like (3) but no filename will be reported */
void parse_in_place(Parser *parser, csubstr filename, substr yaml, NodeRef node           ); /**< (5) like (1) but the node is given as a NodeRef */
void parse_in_place(Parser *parser,                   substr yaml, NodeRef node           ); /**< (6) like (5) but no filename will be reported */
Tree parse_in_place(Parser *parser, csubstr filename, substr yaml                         ); /**< (7) create a new tree, and parse YAML into its root node.
                                                                                              *
                                                                                              * The filename will be used in any error messages
                                                                                              * arising during the parse. The tree is created with
                                                                                              * the callbacks currently in the parser.
                                                                                              */
Tree parse_in_place(Parser *parser,                   substr yaml                         ); /**< (8) like (7) but no filename will be reported */


void parse_json_in_place(Parser *parser, csubstr filename, substr json, Tree *t, size_t node_id); ///< (1) parse JSON into an existing tree node. The filename will be used in any error messages arising during the parse.
void parse_json_in_place(Parser *parser,                   substr json, Tree *t, size_t node_id); ///< (2) like (1) but no filename will be reported
void parse_json_in_place(Parser *parser, csubstr filename, substr json, Tree *t                ); ///< (3) parse JSON into an existing tree, into its root node.
void parse_json_in_place(Parser *parser,                   substr json, Tree *t                ); ///< (4) like (3) but no filename will be reported
void parse_json_in_place(Parser *parser, csubstr filename, substr json, NodeRef node           ); ///< (5) like (1) but the node is given as a NodeRef
void parse_json_in_place(Parser *parser,                   substr json, NodeRef node           ); ///< (6) like (5) but no filename will be reported
Tree parse_json_in_place(Parser *parser, csubstr filename, substr json                         ); ///< (7) create a new tree, and parse JSON into its root node.
Tree parse_json_in_place(Parser *parser,                   substr json                         ); ///< (8) like (7) but no filename will be reported

/** @} */


//-----------------------------------------------------------------------------

/** @name parse_in_place___with_temporary_parser
 *
 * @desc parse a mutable YAML source buffer. Scalars requiring
 * filtering are mutated in place (except in the rare cases where the
 * filtered scalar is longer than the original scalar).
 *
 * @note These freestanding functions use a temporary parser object,
 * and are convenience functions to enable the user to easily parse
 * YAML without the need to explicitly instantiate a parser and event
 * handler. Note that some properties (notably node locations in the
 * original source code) are only available through the parser
 * class. If you need access to any of these properties, use
 * the appropriate overload from @ref parse_in_place__with_existing_parser
 *
 * @{
 */

void parse_in_place(csubstr filename, substr yaml, Tree *t, size_t node_id); ///< (1) parse YAML into an existing tree node. The filename will be used in any error messages arising during the parse.
void parse_in_place(                  substr yaml, Tree *t, size_t node_id); ///< (2) like (1) but no filename will be reported
void parse_in_place(csubstr filename, substr yaml, Tree *t                ); ///< (3) parse YAML into an existing tree, into its root node.
void parse_in_place(                  substr yaml, Tree *t                ); ///< (4) like (3) but no filename will be reported
void parse_in_place(csubstr filename, substr yaml, NodeRef node           ); ///< (5) like (1) but the node is given as a NodeRef
void parse_in_place(                  substr yaml, NodeRef node           ); ///< (6) like (5) but no filename will be reported
Tree parse_in_place(csubstr filename, substr yaml                         ); ///< (7) create a new tree, and parse YAML into its root node.
Tree parse_in_place(                  substr yaml                         ); ///< (8) like (7) but no filename will be reported

void parse_json_in_place(csubstr filename, substr json, Tree *t, size_t node_id); ///< (1) parse JSON into an existing tree node. The filename will be used in any error messages arising during the parse.
void parse_json_in_place(                  substr json, Tree *t, size_t node_id); ///< (2) like (1) but no filename will be reported
void parse_json_in_place(csubstr filename, substr json, Tree *t                ); ///< (3) parse JSON into an existing tree, into its root node.
void parse_json_in_place(                  substr json, Tree *t                ); ///< (4) like (3) but no filename will be reported
void parse_json_in_place(csubstr filename, substr json, NodeRef node           ); ///< (5) like (1) but the node is given as a NodeRef
void parse_json_in_place(                  substr json, NodeRef node           ); ///< (6) like (5) but no filename will be reported
Tree parse_json_in_place(csubstr filename, substr json                         ); ///< (7) create a new tree, and parse JSON into its root node.
Tree parse_json_in_place(                  substr json                         ); ///< (8) like (7) but no filename will be reported

/** @} */


//-----------------------------------------------------------------------------


/** @name parse_in_arena__with_existing_parser
 *
 * @desc parse a read-only (immutable) YAML source buffer. This is
 * achieved by first copying the contents of the buffer to the tree's
 * arena, and then calling @ref parse_in_arena() . All the resulting
 * scalars will be filtered in the arena. These overloads accept an
 * existing parser object, and provide the opportunity to use special
 * parser options.
 *
 * @see ParserOptions
 *
 *
 * @note These freestanding functions use a temporary parser object,
 * and are convenience functions to easily parse YAML without the need
 * to instantiate a separate parser. Note that some properties
 * (notably node locations in the original source code) are only
 * available through the parser class. If you need access to any of
 * these properties, use the appropriate overload from @ref
 * parse_json_in_place__with_existing_parser
 *
 * @note overloads receiving a substr YAML buffer are intentionally
 * left undefined, such that calling parse_in_arena() with a substr
 * will cause a linker error. This is to prevent an accidental copy of
 * the source buffer to the tree's arena, because substr (which is
 * mutable) is implicitly convertible to csubstr (which is
 * immutable). If you really intend to parse a mutable buffer in the
 * tree's arena, convert it first to immutable by assigning the substr
 * to a csubstr prior to calling parse_in_arena(). This is not needed
 * for parse_in_place() because csubstr is not implicitly convertible
 * to substr. To be clear:
 *
 * ```c++
 * substr mutable_buffer = ...;
 * parser.parse_in_arena(mutable_buffer); // linker error
 *
 * csubstr immutable_buffer = ...;
 * parser.parse_in_arena(immutable_buffer); // ok
 * ```
 *
 * @{
 */

#define RYML_DONT_PARSE_SUBSTR_IN_ARENA ""                      \
    "Do not pass a (mutable) substr to parse_in_arena(); "      \
    "if you have a substr, it should be parsed in place. "      \
    "Consider using parse_in_place() instead, or convert "      \
    "the buffer to csubstr prior to calling. This function "    \
    " is deliberately left undefined, so that calling it "      \
    "will cause a linker error."

void parse_in_arena(Parser *parser, csubstr filename, csubstr yaml, Tree *t, size_t node_id); ///< (1) parse YAML into an existing tree node. The filename will be used in any error messages arising during the parse.
void parse_in_arena(Parser *parser,                   csubstr yaml, Tree *t, size_t node_id); ///< (2) like (1) but no filename will be reported
void parse_in_arena(Parser *parser, csubstr filename, csubstr yaml, Tree *t                ); ///< (3) parse YAML into an existing tree, into its root node.
void parse_in_arena(Parser *parser,                   csubstr yaml, Tree *t                ); ///< (4) like (3) but no filename will be reported
void parse_in_arena(Parser *parser, csubstr filename, csubstr yaml, NodeRef node           ); ///< (5) like (1) but the node is given as a NodeRef
void parse_in_arena(Parser *parser,                   csubstr yaml, NodeRef node           ); ///< (6) like (5) but no filename will be reported
Tree parse_in_arena(Parser *parser, csubstr filename, csubstr yaml                         ); ///< (7) create a new tree, and parse YAML into its root node.
Tree parse_in_arena(Parser *parser,                   csubstr yaml                         ); ///< (8) like (7) but no filename will be reported

void parse_json_in_arena(Parser *parser, csubstr filename, csubstr json, Tree *t, size_t node_id); ///< (1) parse JSON into an existing tree node. The filename will be used in any error messages arising during the parse.
void parse_json_in_arena(Parser *parser,                   csubstr json, Tree *t, size_t node_id); ///< (2) like (1) but no filename will be reported
void parse_json_in_arena(Parser *parser, csubstr filename, csubstr json, Tree *t                ); ///< (3) parse JSON into an existing tree, into its root node.
void parse_json_in_arena(Parser *parser,                   csubstr json, Tree *t                ); ///< (4) like (3) but no filename will be reported
void parse_json_in_arena(Parser *parser, csubstr filename, csubstr json, NodeRef node           ); ///< (5) like (1) but the node is given as a NodeRef
void parse_json_in_arena(Parser *parser,                   csubstr json, NodeRef node           ); ///< (6) like (5) but no filename will be reported
Tree parse_json_in_arena(Parser *parser, csubstr filename, csubstr json                         ); ///< (7) create a new tree, and parse JSON into its root node.
Tree parse_json_in_arena(Parser *parser,                   csubstr json                         ); ///< (8) like (7) but no filename will be reported

/* READ THE DEPRECATION NOTE!
 *
 * All of the functions below are intentionally left undefined, to
 * prevent them being used.
 *
 */
RYML_DEPRECATED(RYML_DONT_PARSE_SUBSTR_IN_ARENA) void parse_in_arena(Parser *parser,                   substr yaml, Tree *t, size_t node_id);
RYML_DEPRECATED(RYML_DONT_PARSE_SUBSTR_IN_ARENA) void parse_in_arena(Parser *parser, csubstr filename, substr yaml, Tree *t, size_t node_id);
RYML_DEPRECATED(RYML_DONT_PARSE_SUBSTR_IN_ARENA) void parse_in_arena(Parser *parser,                   substr yaml, Tree *t                );
RYML_DEPRECATED(RYML_DONT_PARSE_SUBSTR_IN_ARENA) void parse_in_arena(Parser *parser, csubstr filename, substr yaml, Tree *t                );
RYML_DEPRECATED(RYML_DONT_PARSE_SUBSTR_IN_ARENA) void parse_in_arena(Parser *parser,                   substr yaml, NodeRef node           );
RYML_DEPRECATED(RYML_DONT_PARSE_SUBSTR_IN_ARENA) void parse_in_arena(Parser *parser, csubstr filename, substr yaml, NodeRef node           );
RYML_DEPRECATED(RYML_DONT_PARSE_SUBSTR_IN_ARENA) Tree parse_in_arena(Parser *parser,                   substr yaml                         );
RYML_DEPRECATED(RYML_DONT_PARSE_SUBSTR_IN_ARENA) Tree parse_in_arena(Parser *parser, csubstr filename, substr yaml                         );
RYML_DEPRECATED(RYML_DONT_PARSE_SUBSTR_IN_ARENA) void parse_json_in_arena(Parser *parser,                   substr json, Tree *t, size_t node_id);
RYML_DEPRECATED(RYML_DONT_PARSE_SUBSTR_IN_ARENA) void parse_json_in_arena(Parser *parser, csubstr filename, substr json, Tree *t, size_t node_id);
RYML_DEPRECATED(RYML_DONT_PARSE_SUBSTR_IN_ARENA) void parse_json_in_arena(Parser *parser,                   substr json, Tree *t                );
RYML_DEPRECATED(RYML_DONT_PARSE_SUBSTR_IN_ARENA) void parse_json_in_arena(Parser *parser, csubstr filename, substr json, Tree *t                );
RYML_DEPRECATED(RYML_DONT_PARSE_SUBSTR_IN_ARENA) void parse_json_in_arena(Parser *parser,                   substr json, NodeRef node           );
RYML_DEPRECATED(RYML_DONT_PARSE_SUBSTR_IN_ARENA) void parse_json_in_arena(Parser *parser, csubstr filename, substr json, NodeRef node           );
RYML_DEPRECATED(RYML_DONT_PARSE_SUBSTR_IN_ARENA) Tree parse_json_in_arena(Parser *parser,                   substr json                         );
RYML_DEPRECATED(RYML_DONT_PARSE_SUBSTR_IN_ARENA) Tree parse_json_in_arena(Parser *parser, csubstr filename, substr json                         );

/** @} */


//-----------------------------------------------------------------------------


/** @name parse_in_arena__with_temporary_parser
 *
 * @desc parse a read-only (immutable) YAML source buffer. This is
 * achieved by first copying the contents of the buffer to the tree's
 * arena, and then calling @ref parse_in_arena() .
 *
 * @note These freestanding functions use a temporary parser object,
 * and are convenience functions to easily parse YAML without the need
 * to instantiate a separate parser. Note that some properties
 * (notably node locations in the original source code) are only
 * available through the parser class. If you need access to any of
 * these properties, use the appropriate overload from @ref
 * parse_in_arena__with_existing_parser
 *
 * @note overloads receiving a substr YAML buffer are intentionally
 * left undefined, such that calling parse_in_arena() with a substr
 * will cause a linker error. This is to prevent an accidental copy of
 * the source buffer to the tree's arena, because substr (which is
 * mutable) is implicitly convertible to csubstr (which is
 * immutable). If you really intend to parse a mutable buffer in the
 * tree's arena, convert it first to immutable by assigning the substr
 * to a csubstr prior to calling parse_in_arena(). This is not needed
 * for parse_in_place() because csubstr is not implicitly convertible
 * to substr. To be clear:
 *
 * ```c++
 * substr mutable_buffer = ...;
 * parser.parse_in_arena(mutable_buffer); // linker error
 *
 * csubstr immutable_buffer = ...;
 * parser.parse_in_arena(immutable_buffer); // ok
 * ```
 *
 * @{
 */

void parse_in_arena(csubstr filename, csubstr yaml, Tree *t, size_t node_id); ///< (1) parse YAML into an existing tree node. The filename will be used in any error messages arising during the parse.
void parse_in_arena(                  csubstr yaml, Tree *t, size_t node_id); ///< (2) like (1) but no filename will be reported
void parse_in_arena(csubstr filename, csubstr yaml, Tree *t                ); ///< (3) parse YAML into an existing tree, into its root node.
void parse_in_arena(                  csubstr yaml, Tree *t                ); ///< (4) like (3) but no filename will be reported
void parse_in_arena(csubstr filename, csubstr yaml, NodeRef node           ); ///< (5) like (1) but the node is given as a NodeRef
void parse_in_arena(                  csubstr yaml, NodeRef node           ); ///< (6) like (5) but no filename will be reported
Tree parse_in_arena(csubstr filename, csubstr yaml                         ); ///< (7) create a new tree, and parse YAML into its root node.
Tree parse_in_arena(                  csubstr yaml                         ); ///< (8) like (7) but no filename will be reported

void parse_json_in_arena(csubstr filename, csubstr json, Tree *t, size_t node_id); ///< (1) parse JSON into an existing tree node. The filename will be used in any error messages arising during the parse.
void parse_json_in_arena(                  csubstr json, Tree *t, size_t node_id); ///< (2) like (1) but no filename will be reported
void parse_json_in_arena(csubstr filename, csubstr json, Tree *t                ); ///< (3) parse JSON into an existing tree, into its root node.
void parse_json_in_arena(                  csubstr json, Tree *t                ); ///< (4) like (3) but no filename will be reported
void parse_json_in_arena(csubstr filename, csubstr json, NodeRef node           ); ///< (5) like (1) but the node is given as a NodeRef
void parse_json_in_arena(                  csubstr json, NodeRef node           ); ///< (6) like (5) but no filename will be reported
Tree parse_json_in_arena(csubstr filename, csubstr json                         ); ///< (7) create a new tree, and parse JSON into its root node.
Tree parse_json_in_arena(                  csubstr json                         ); ///< (8) like (7) but no filename will be reported

/* READ THE DEPRECATION NOTE!
 *
 * All of the functions below are intentionally left undefined, to
 * prevent them being used.
 */
RYML_DEPRECATED(RYML_DONT_PARSE_SUBSTR_IN_ARENA) void parse_in_arena(                  substr yaml, Tree *t, size_t node_id);
RYML_DEPRECATED(RYML_DONT_PARSE_SUBSTR_IN_ARENA) void parse_in_arena(csubstr filename, substr yaml, Tree *t, size_t node_id);
RYML_DEPRECATED(RYML_DONT_PARSE_SUBSTR_IN_ARENA) void parse_in_arena(                  substr yaml, Tree *t                );
RYML_DEPRECATED(RYML_DONT_PARSE_SUBSTR_IN_ARENA) void parse_in_arena(csubstr filename, substr yaml, Tree *t                );
RYML_DEPRECATED(RYML_DONT_PARSE_SUBSTR_IN_ARENA) void parse_in_arena(                  substr yaml, NodeRef node           );
RYML_DEPRECATED(RYML_DONT_PARSE_SUBSTR_IN_ARENA) void parse_in_arena(csubstr filename, substr yaml, NodeRef node           );
RYML_DEPRECATED(RYML_DONT_PARSE_SUBSTR_IN_ARENA) Tree parse_in_arena(                  substr yaml                         );
RYML_DEPRECATED(RYML_DONT_PARSE_SUBSTR_IN_ARENA) Tree parse_in_arena(csubstr filename, substr yaml                         );
RYML_DEPRECATED(RYML_DONT_PARSE_SUBSTR_IN_ARENA) void parse_json_in_arena(                  substr json, Tree *t, size_t node_id);
RYML_DEPRECATED(RYML_DONT_PARSE_SUBSTR_IN_ARENA) void parse_json_in_arena(csubstr filename, substr json, Tree *t, size_t node_id);
RYML_DEPRECATED(RYML_DONT_PARSE_SUBSTR_IN_ARENA) void parse_json_in_arena(                  substr json, Tree *t                );
RYML_DEPRECATED(RYML_DONT_PARSE_SUBSTR_IN_ARENA) void parse_json_in_arena(csubstr filename, substr json, Tree *t                );
RYML_DEPRECATED(RYML_DONT_PARSE_SUBSTR_IN_ARENA) void parse_json_in_arena(                  substr json, NodeRef node           );
RYML_DEPRECATED(RYML_DONT_PARSE_SUBSTR_IN_ARENA) void parse_json_in_arena(csubstr filename, substr json, NodeRef node           );
RYML_DEPRECATED(RYML_DONT_PARSE_SUBSTR_IN_ARENA) Tree parse_json_in_arena(                  substr json                         );
RYML_DEPRECATED(RYML_DONT_PARSE_SUBSTR_IN_ARENA) Tree parse_json_in_arena(csubstr filename, substr json                         );

/** @} */

} // namespace yml
} // namespace c4

#endif /* _C4_YML_PARSE_HPP_ */
