#ifndef _C4_YML_NODE_TYPE_HPP_
#define _C4_YML_NODE_TYPE_HPP_

#ifndef _C4_YML_COMMON_HPP_
#include "c4/yml/common.hpp"
#endif

C4_SUPPRESS_WARNING_MSVC_PUSH
C4_SUPPRESS_WARNING_GCC_CLANG_PUSH
C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")
#if defined(__GNUC__) && __GNUC__ >= 6
C4_SUPPRESS_WARNING_GCC("-Wnull-dereference")
#endif
// NOLINTBEGIN(modernize-avoid-c-style-cast)

namespace c4 {
namespace yml {

/** @addtogroup doc_node_type
 *
 * @{
 */


/** the integral type necessary to cover all the bits for NodeType_e */
using type_bits = uint32_t;


/** a bit mask for marking node types and styles */
typedef enum : type_bits { // NOLINT
    #define __(v) (type_bits(1) << v) // a convenience define, undefined below // NOLINT
    NOTYPE  = 0,         ///< no node type or style is set
    KEY     = __(0),     ///< the scalar to the left of `:` in a map's member
    VAL     = __(1),     ///< a scalar: has a scalar (ie string) value, possibly empty. must be a leaf node, and cannot be MAP or SEQ
    MAP     = __(2),     ///< a map: a parent of KEYVAL/KEYSEQ/KEYMAP nodes
    SEQ     = __(3),     ///< a seq: a parent of VAL/SEQ/MAP nodes
    DOC     = __(4),     ///< a document
    STREAM  = __(5)|SEQ, ///< a stream: a seq of docs
    KEYREF  = __(6),     ///< a *reference: the key references an &anchor
    VALREF  = __(7),     ///< a *reference: the val references an &anchor
    KEYANCH = __(8),     ///< the key has an &anchor
    VALANCH = __(9),     ///< the val has an &anchor
    KEYTAG  = __(10),    ///< the key has a tag
    VALTAG  = __(11),    ///< the val has a tag
    KEYNIL  = __(12),    ///< the key is null (eg `{ : b}` results in a null key)
    VALNIL  = __(13),    ///< the val is null (eg `{a : }` results in a null val)
    _TYMASK = __(14)-1,  ///< all the bits up to here
    //
    // unfiltered flags:
    //
    KEY_UNFILT  = __(14), ///< the key scalar was left unfiltered; the parser was set not to filter. @see @ref ParserOptions::scalar_filtering()
    VAL_UNFILT  = __(15), ///< the val scalar was left unfiltered; the parser was set not to filter. @see @ref ParserOptions::scalar_filtering()
    //
    // style flags:
    //
    FLOW_SL     = __(16), ///< mark container with single-line flow style
                          ///<  - seqs as
                          ///<    @code{yaml}
                          ///<    [val1,val2]
                          ///<    @endcode
                          ///<    when @ref FLOW_SPC is not set, or
                          ///<    @code{yaml}
                          ///<    [val1, val2]
                          ///<    @endcode
                          ///<    when @ref FLOW_SPC is set (or @ref EmitOptions::force_flow_spc() is set)
                          ///<  - maps as
                          ///<    @code{yaml}
                          ///<    {key1: val1,key2: val2}
                          ///<    @endcode
                          ///<    when @ref FLOW_SPC is not set, or
                          ///<    @code{yaml}
                          ///<    {key1: val1, key2: val2}
                          ///<    @endcode
                          ///<    when @ref FLOW_SPC is set (or @ref EmitOptions::force_flow_spc() is set)
    FLOW_ML1    = __(17), ///< mark container with multi-line flow style, 1 element per line
                          ///<  - seqs as
                          ///<    @code{yaml}
                          ///<    [
                          ///<      val1,
                          ///<      val2
                          ///<    ]
                          ///<    @endcode
                          ///<  - maps as
                          ///<    @code{yaml}
                          ///<    {
                          ///<      key: val,
                          ///<      key2: val2
                          ///<    }
                          ///<    @endcode
    FLOW_MLN    = __(18), ///< mark container with multi-line flow style, n elements per line,
                          ///< wrapped (as set by @ref EmitOptions::max_cols()):
                          ///<  - seqs as
                          ///<    @code{yaml}
                          ///<    [
                          ///<      val,val,...
                          ///<      val,val,...
                          ///<      val,val,...
                          ///<      ...
                          ///<      val
                          ///<    ]
                          ///<    @endcode
                          ///<    when @ref FLOW_SPC is not set, or
                          ///<    @code{yaml}
                          ///<    [
                          ///<      val, val,...
                          ///<      val, val,...
                          ///<      val, val,...
                          ///<      ...
                          ///<      val
                          ///<    ]
                          ///<    @endcode
                          ///<    when @ref FLOW_SPC is set (or @ref EmitOptions::force_flow_spc() is set)
                          ///<  - maps as
                          ///<    @code{yaml}
                          ///<    {
                          ///<      key: val,key: val,...
                          ///<      key: val,key: val,...
                          ///<      ...
                          ///<      key: val
                          ///<    }
                          ///<    @endcode
                          ///<    when @ref FLOW_SPC is not set, or
                          ///<    @code{yaml}
                          ///<    {
                          ///<      key: val, key: val,...
                          ///<      key: val, key: val,...
                          ///<      ...
                          ///<      key: val
                          ///<    }
                          ///<    @endcode
                          ///<    when @ref FLOW_SPC is set (or @ref EmitOptions::force_flow_spc() is set)
    FLOW_SPC    = __(19), ///< mark container with spaces after comma when in flow mode.
                          ///< Applies to both @ref FLOW_SL and @ref FLOW_MLN (but not
                          ///< to @ref FLOW_ML1), and can be overriden globally by
                          ///< @ref EmitOptions::force_flow_spc().
    BLOCK       = __(20), ///< mark container with block style
                          ///<  - seqs as
                          ///<    @code{yaml}
                          ///<    - val1
                          ///<    - val2
                          ///<    @endcode
                          ///<  - maps as
                          ///<    @code{yaml}
                          ///<    key1: val1
                          ///<    key2: val2
                          ///<    @endcode
    KEY_LITERAL = __(21), ///< mark key scalar as multiline, block literal |
    VAL_LITERAL = __(22), ///< mark val scalar as multiline, block literal |
    KEY_FOLDED  = __(23), ///< mark key scalar as multiline, block folded >
    VAL_FOLDED  = __(24), ///< mark val scalar as multiline, block folded >
    KEY_SQUO    = __(25), ///< mark key scalar as single quoted '
    VAL_SQUO    = __(26), ///< mark val scalar as single quoted '
    KEY_DQUO    = __(27), ///< mark key scalar as double quoted "
    VAL_DQUO    = __(28), ///< mark val scalar as double quoted "
    KEY_PLAIN   = __(29), ///< mark key scalar as plain scalar (unquoted, even when multiline)
    VAL_PLAIN   = __(30), ///< mark val scalar as plain scalar (unquoted, even when multiline)
    //
    // type combination masks:
    //
    KEYVAL  = KEY|VAL, ///< mask of @ref KEY|@ref VAL
    KEYSEQ  = KEY|SEQ, ///< mask of @ref KEY|@ref SEQ
    KEYMAP  = KEY|MAP, ///< mask of @ref KEY|@ref MAP
    DOCMAP  = DOC|MAP, ///< mask of @ref DOC|@ref MAP
    DOCSEQ  = DOC|SEQ, ///< mask of @ref DOC|@ref SEQ
    DOCVAL  = DOC|VAL, ///< mask of @ref DOC|@ref VAL
    //
    // style combination masks:
    //
    SCALAR_LITERAL = KEY_LITERAL|VAL_LITERAL,  ///< mask of @ref KEY_LITERAL|@ref VAL_LITERAL,
    SCALAR_FOLDED  = KEY_FOLDED|VAL_FOLDED,  ///< mask of @ref KEY_FOLDED|@ref VAL_FOLDED,
    SCALAR_SQUO    = KEY_SQUO|VAL_SQUO,  ///< mask of @ref KEY_SQUO|@ref VAL_SQUO,
    SCALAR_DQUO    = KEY_DQUO|VAL_DQUO,  ///< mask of @ref KEY_DQUO|@ref VAL_DQUO,
    SCALAR_PLAIN   = KEY_PLAIN|VAL_PLAIN,  ///< mask of @ref KEY_PLAIN|@ref VAL_PLAIN,
    KEYQUO         = KEY_SQUO|KEY_DQUO|KEY_FOLDED|KEY_LITERAL, ///< key style is one of `'">|`. mask of @ref KEY_SQUO|@ref KEY_DQUO|@ref KEY_FOLDED|@ref KEY_LITERAL
    VALQUO         = VAL_SQUO|VAL_DQUO|VAL_FOLDED|VAL_LITERAL, ///< val style is one of `'">|`. mask of @ref VAL_SQUO|@ref VAL_DQUO|@ref VAL_FOLDED|@ref VAL_LITERAL
    KEY_STYLE      = KEYQUO|KEY_PLAIN, ///< mask of @ref KEYQUO|@ref KEY_PLAIN : all the key scalar styles for key (not container styles!)
    VAL_STYLE      = VALQUO|VAL_PLAIN, ///< mask of @ref VALQUO|@ref VAL_PLAIN : all the val scalar styles for val (not container styles!)
    SCALAR_STYLE   = KEY_STYLE|VAL_STYLE, ///< mask of @ref KEY_STYLE|@ref VAL_STYLE : all the key+val scalar styles
    FLOW_MLX       = FLOW_ML1|FLOW_MLN, ///< mask of @ref FLOW_ML1|@ref FLOW_MLN : all the flow multiline styles
    CONTAINER_STYLE_FLOW  = FLOW_SL|FLOW_MLX|FLOW_SPC, ///< mask of @ref FLOW_SL|@ref FLOW_MLX|@ref FLOW_SPC : all flow flags
    CONTAINER_STYLE_BLOCK = BLOCK, ///< alias to @ref BLOCK
    CONTAINER_STYLE       = CONTAINER_STYLE_FLOW|CONTAINER_STYLE_BLOCK, ///< mask of @ref CONTAINER_STYLE_FLOW|@ref CONTAINER_STYLE_BLOCK : all container style flags
    STYLE          = SCALAR_STYLE | CONTAINER_STYLE, ///< mask of @ref SCALAR_STYLE | @ref CONTAINER_STYLE : all style flags
    /** @cond dev */
    //
    // mixed masks
    _KEYMASK = KEY | KEYQUO | KEYANCH | KEYREF | KEYTAG,
    _VALMASK = VAL | VALQUO | VALANCH | VALREF | VALTAG,
    #undef __
    #if C4_CPP >= 17                                  \
        || (defined(__GNUC__) && __GNUC__ >= 6)       \
        || (defined(_MSC_VER) && !defined(__clang__))
    #define RYML_HAS_DEPRECATED_ENUMS__
    FLOW_ML RYML_DEPRECATED("use one of FLOW_ML{1,N,X}") = FLOW_ML1,
    #endif
    /** @endcond */
} NodeType_e;
/** @cond dev */
#if !defined(RYML_HAS_DEPRECATED_ENUMS__)
// defined here because the current c++ standard / compiler cannot
// handle deprecated enums
RYML_DEPRECATED("use one of FLOW_ML{1,N,X}")
constexpr const NodeType_e FLOW_ML = FLOW_ML1;
#endif
/** @endcond */

constexpr C4_ALWAYS_INLINE C4_CONST NodeType_e operator|  (NodeType_e lhs, NodeType_e rhs) noexcept { return (NodeType_e)(((type_bits)lhs) | ((type_bits)rhs)); }
constexpr C4_ALWAYS_INLINE C4_CONST NodeType_e operator&  (NodeType_e lhs, NodeType_e rhs) noexcept { return (NodeType_e)(((type_bits)lhs) & ((type_bits)rhs)); }
constexpr C4_ALWAYS_INLINE C4_CONST NodeType_e operator>> (NodeType_e bits, uint32_t n) noexcept { return (NodeType_e)(((type_bits)bits) >> n); }
constexpr C4_ALWAYS_INLINE C4_CONST NodeType_e operator<< (NodeType_e bits, uint32_t n) noexcept { return (NodeType_e)(((type_bits)bits) << n); }
constexpr C4_ALWAYS_INLINE C4_CONST NodeType_e operator~  (NodeType_e bits) noexcept { return (NodeType_e)(~(type_bits)bits); }
C4_ALWAYS_INLINE NodeType_e& operator&= (NodeType_e &subject, NodeType_e bits) noexcept { subject = (NodeType_e)((type_bits)subject & (type_bits)bits); return subject; }
C4_ALWAYS_INLINE NodeType_e& operator|= (NodeType_e &subject, NodeType_e bits) noexcept { subject = (NodeType_e)((type_bits)subject | (type_bits)bits); return subject; }


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/** wraps a NodeType_e element with some syntactic sugar and predicates */
struct RYML_EXPORT NodeType
{
public:

    NodeType_e type;

public:

    C4_ALWAYS_INLINE NodeType() noexcept : type(NOTYPE) {}
    C4_ALWAYS_INLINE NodeType(NodeType_e t) noexcept : type(t) {}
    C4_ALWAYS_INLINE NodeType(type_bits t) noexcept : type((NodeType_e)t) {}

    C4_ALWAYS_INLINE bool has_any(NodeType_e t) const noexcept { return (type & t) != 0u; }
    C4_ALWAYS_INLINE bool has_all(NodeType_e t) const noexcept { return (type & t) == t; }
    C4_ALWAYS_INLINE bool has_none(NodeType_e t) const noexcept { return (type & t) == 0; }

    C4_ALWAYS_INLINE void set(NodeType_e t) noexcept { type = t; }
    C4_ALWAYS_INLINE void add(NodeType_e t) noexcept { type = (type|t); }
    C4_ALWAYS_INLINE void rem(NodeType_e t) noexcept { type = (type & ~t); }
    C4_ALWAYS_INLINE void addrem(NodeType_e bits_to_add, NodeType_e bits_to_remove) noexcept { type |= bits_to_add; type &= ~bits_to_remove; }

    C4_ALWAYS_INLINE void clear() noexcept { type = NOTYPE; }

public:

    C4_ALWAYS_INLINE operator NodeType_e      & C4_RESTRICT ()       noexcept { return type; }
    C4_ALWAYS_INLINE operator NodeType_e const& C4_RESTRICT () const noexcept { return type; }

public:

    /** @name convert to string
     * @{ */

    /** return a preset string based on the node type */
    C4_ALWAYS_INLINE const char *type_str() const noexcept { return type_str(type); }
    /** return a preset string based on the node type */
    static const char* type_str(NodeType_e t) noexcept;

    /** fill a string with the node type flags. */
    C4_ALWAYS_INLINE size_t type_str(substr buf) const noexcept { return type_str(buf, type); }
    /** fill a string with the node type flags. */
    static size_t type_str(substr buf, NodeType_e t) noexcept;

    /** fill a string with the node type flags. If the string is small, returns {null, len} */
    C4_ALWAYS_INLINE csubstr type_str_sub(substr buf) const noexcept { return type_str_sub(buf, type); }
    /** fill a string with the node type flags. If the string is small, returns {null, len}  */
    static csubstr type_str_sub(substr buf, NodeType_e t) noexcept
    {
        csubstr ret;
        ret.len = type_str(buf, t);
        ret.str = ret.len < buf.len ? buf.str : nullptr;
        return ret;
    }

    /** @} */

public:

    /** @name node type queries
     * @{ */

    C4_ALWAYS_INLINE bool is_notype()         const noexcept { return type == NOTYPE; }
    C4_ALWAYS_INLINE bool is_stream()         const noexcept { return ((type & STREAM) == STREAM) != 0; }
    C4_ALWAYS_INLINE bool is_doc()            const noexcept { return (type & DOC) != 0; }
    C4_ALWAYS_INLINE bool is_container()      const noexcept { return (type & (MAP|SEQ|STREAM)) != 0; }
    C4_ALWAYS_INLINE bool is_map()            const noexcept { return (type & MAP) != 0; }
    C4_ALWAYS_INLINE bool is_seq()            const noexcept { return (type & SEQ) != 0; }
    C4_ALWAYS_INLINE bool has_key()           const noexcept { return (type & KEY) != 0; }
    C4_ALWAYS_INLINE bool has_val()           const noexcept { return (type & VAL) != 0; }
    C4_ALWAYS_INLINE bool is_val()            const noexcept { return (type & KEYVAL) == VAL; }
    C4_ALWAYS_INLINE bool is_keyval()         const noexcept { return (type & KEYVAL) == KEYVAL; }
    C4_ALWAYS_INLINE bool key_is_null()       const noexcept { return (type & KEYNIL) != 0; }
    C4_ALWAYS_INLINE bool val_is_null()       const noexcept { return (type & VALNIL) != 0; }
    C4_ALWAYS_INLINE bool has_key_tag()       const noexcept { return (type & KEYTAG) != 0; }
    C4_ALWAYS_INLINE bool has_val_tag()       const noexcept { return (type & VALTAG) != 0; }
    C4_ALWAYS_INLINE bool has_key_anchor()    const noexcept { return (type & KEYANCH) != 0; }
    C4_ALWAYS_INLINE bool has_val_anchor()    const noexcept { return (type & VALANCH) != 0; }
    C4_ALWAYS_INLINE bool has_anchor()        const noexcept { return (type & (KEYANCH|VALANCH)) != 0; }
    C4_ALWAYS_INLINE bool is_key_ref()        const noexcept { return (type & KEYREF) != 0; }
    C4_ALWAYS_INLINE bool is_val_ref()        const noexcept { return (type & VALREF) != 0; }
    C4_ALWAYS_INLINE bool is_ref()            const noexcept { return (type & (KEYREF|VALREF)) != 0; }

    C4_ALWAYS_INLINE bool is_key_unfiltered() const noexcept { return (type & (KEY_UNFILT)) != 0; }
    C4_ALWAYS_INLINE bool is_val_unfiltered() const noexcept { return (type & (VAL_UNFILT)) != 0; }

    /** @} */

public:

    /** @name style functions
     * @{ */

    C4_ALWAYS_INLINE bool is_container_styled() const noexcept { return (type & (CONTAINER_STYLE)) != 0; }
    C4_ALWAYS_INLINE bool is_block() const noexcept { return (type & (BLOCK)) != 0; }
    C4_ALWAYS_INLINE bool is_flow_sl() const noexcept { return (type & (FLOW_SL)) != 0; }
    C4_ALWAYS_INLINE bool is_flow_ml1() const noexcept { return (type & (FLOW_ML1)) != 0; }
    C4_ALWAYS_INLINE bool is_flow_mln() const noexcept { return (type & (FLOW_MLN)) != 0; }
    C4_ALWAYS_INLINE bool is_flow_mlx() const noexcept { return (type & (FLOW_ML1|FLOW_MLN)) != 0; }
    C4_ALWAYS_INLINE bool is_flow() const noexcept { return (type & (FLOW_ML1|FLOW_MLN|FLOW_SL)) != 0; }
    C4_ALWAYS_INLINE bool has_flow_space() const noexcept { return (type & (FLOW_SPC)) != 0; }

    C4_ALWAYS_INLINE bool is_key_styled() const noexcept { return (type & (KEY_STYLE)) != 0; }
    C4_ALWAYS_INLINE bool is_val_styled() const noexcept { return (type & (VAL_STYLE)) != 0; }
    C4_ALWAYS_INLINE bool is_key_literal() const noexcept { return (type & (KEY_LITERAL)) != 0; }
    C4_ALWAYS_INLINE bool is_val_literal() const noexcept { return (type & (VAL_LITERAL)) != 0; }
    C4_ALWAYS_INLINE bool is_key_folded() const noexcept { return (type & (KEY_FOLDED)) != 0; }
    C4_ALWAYS_INLINE bool is_val_folded() const noexcept { return (type & (VAL_FOLDED)) != 0; }
    C4_ALWAYS_INLINE bool is_key_squo() const noexcept { return (type & (KEY_SQUO)) != 0; }
    C4_ALWAYS_INLINE bool is_val_squo() const noexcept { return (type & (VAL_SQUO)) != 0; }
    C4_ALWAYS_INLINE bool is_key_dquo() const noexcept { return (type & (KEY_DQUO)) != 0; }
    C4_ALWAYS_INLINE bool is_val_dquo() const noexcept { return (type & (VAL_DQUO)) != 0; }
    C4_ALWAYS_INLINE bool is_key_plain() const noexcept { return (type & (KEY_PLAIN)) != 0; }
    C4_ALWAYS_INLINE bool is_val_plain() const noexcept { return (type & (VAL_PLAIN)) != 0; }
    C4_ALWAYS_INLINE bool is_key_quoted() const noexcept { return (type & KEYQUO) != 0; }
    C4_ALWAYS_INLINE bool is_val_quoted() const noexcept { return (type & VALQUO) != 0; }
    C4_ALWAYS_INLINE bool is_quoted() const noexcept { return (type & (KEYQUO|VALQUO)) != 0; }

    C4_ALWAYS_INLINE NodeType key_style() const noexcept { return (type & (KEY_STYLE)); }
    C4_ALWAYS_INLINE NodeType val_style() const noexcept { return (type & (VAL_STYLE)); }

    C4_ALWAYS_INLINE void set_container_style(NodeType_e style) noexcept { type = ((style & CONTAINER_STYLE) | (type & ~CONTAINER_STYLE)); }
    C4_ALWAYS_INLINE void set_key_style(NodeType_e style) noexcept { type = ((style & KEY_STYLE) | (type & ~KEY_STYLE)); }
    C4_ALWAYS_INLINE void set_val_style(NodeType_e style) noexcept { type = ((style & VAL_STYLE) | (type & ~VAL_STYLE)); }
    C4_ALWAYS_INLINE void clear_style() noexcept { type &= ~STYLE; }

    /** @} */

public: // deprecated methods

    /** @cond dev */ // LCOV_EXCL_START
    RYML_DEPRECATED("use has_key_anchor()")    bool is_key_anchor() const noexcept { return has_key_anchor(); }
    RYML_DEPRECATED("use has_val_anchor()")    bool is_val_anchor() const noexcept { return has_val_anchor(); }
    RYML_DEPRECATED("use has_anchor()")        bool is_anchor() const noexcept { return has_anchor(); }
    RYML_DEPRECATED("use has_anchor() || is_ref()") bool is_anchor_or_ref() const noexcept { return has_anchor() || is_ref(); }

    RYML_DEPRECATED("use one of .is_flow_ml{1,n,x}()")
    bool is_flow_ml() const noexcept { return (type & (FLOW_ML1)) != 0; }
    /** @endcond */ // LCOV_EXCL_STOP
};

/** @} */

} // namespace yml
} // namespace c4

// NOLINTEND(modernize-avoid-c-style-cast)
C4_SUPPRESS_WARNING_MSVC_POP
C4_SUPPRESS_WARNING_GCC_CLANG_POP

#endif /* _C4_YML_NODE_TYPE_HPP_ */
