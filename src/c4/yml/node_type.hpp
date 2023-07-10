#ifndef C4_YML_NODE_TYPE_HPP_
#define C4_YML_NODE_TYPE_HPP_

#ifndef _C4_YML_COMMON_HPP_
#include "c4/yml/common.hpp"
#endif

C4_SUPPRESS_WARNING_MSVC_PUSH
C4_SUPPRESS_WARNING_GCC_CLANG_PUSH
C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")

namespace c4 {
namespace yml {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


/** the integral type necessary to cover all the bits marking node types */
using type_bits = uint64_t;


/** a bit mask for marking node types */
typedef enum : type_bits {
    // a convenience define, undefined below
    #define c4bit(v) (type_bits(1) << v)
    NOTYPE  = 0,            ///< no node type is set
    VAL     = c4bit(0),     ///< a leaf node, has a (possibly empty) value
    KEY     = c4bit(1),     ///< is member of a map, must have non-empty key
    MAP     = c4bit(2),     ///< a map: a parent of keyvals
    SEQ     = c4bit(3),     ///< a seq: a parent of vals
    DOC     = c4bit(4),     ///< a document
    STREAM  = c4bit(5)|SEQ, ///< a stream: a seq of docs
    KEYREF  = c4bit(6),     ///< a *reference: the key references an &anchor
    VALREF  = c4bit(7),     ///< a *reference: the val references an &anchor
    KEYANCH = c4bit(8),     ///< the key has an &anchor
    VALANCH = c4bit(9),     ///< the val has an &anchor
    KEYTAG  = c4bit(10),    ///< the key has an explicit tag/type
    VALTAG  = c4bit(11),    ///< the val has an explicit tag/type
    _TYMASK = c4bit(12)-1,  // all the bits up to here
    VALQUO  = c4bit(12),    ///< the val is quoted by '', "", > or |
    KEYQUO  = c4bit(13),    ///< the key is quoted by '', "", > or |
    KEYVAL  = KEY|VAL,
    KEYSEQ  = KEY|SEQ,
    KEYMAP  = KEY|MAP,
    DOCMAP  = DOC|MAP,
    DOCSEQ  = DOC|SEQ,
    DOCVAL  = DOC|VAL,
    _KEYMASK = KEY | KEYQUO | KEYANCH | KEYREF | KEYTAG,
    _VALMASK = VAL | VALQUO | VALANCH | VALREF | VALTAG,
    // these flags are from a work in progress and should be used with care
    _WIP_STYLE_FLOW_SL = c4bit(14), ///< mark container with single-line flow format (seqs as '[val1,val2], maps as '{key: val, key2: val2}')
    _WIP_STYLE_FLOW_ML = c4bit(15), ///< mark container with multi-line flow format (seqs as '[val1,\nval2], maps as '{key: val,\nkey2: val2}')
    _WIP_STYLE_BLOCK   = c4bit(16), ///< mark container with block format (seqs as '- val\n', maps as 'key: val')
    _WIP_KEY_LITERAL   = c4bit(17), ///< mark key scalar as multiline, block literal |
    _WIP_VAL_LITERAL   = c4bit(18), ///< mark val scalar as multiline, block literal |
    _WIP_KEY_FOLDED    = c4bit(19), ///< mark key scalar as multiline, block folded >
    _WIP_VAL_FOLDED    = c4bit(20), ///< mark val scalar as multiline, block folded >
    _WIP_KEY_SQUO      = c4bit(21), ///< mark key scalar as single quoted
    _WIP_VAL_SQUO      = c4bit(22), ///< mark val scalar as single quoted
    _WIP_KEY_DQUO      = c4bit(23), ///< mark key scalar as double quoted
    _WIP_VAL_DQUO      = c4bit(24), ///< mark val scalar as double quoted
    _WIP_KEY_PLAIN     = c4bit(25), ///< mark key scalar as plain scalar (unquoted, even when multiline)
    _WIP_VAL_PLAIN     = c4bit(26), ///< mark val scalar as plain scalar (unquoted, even when multiline)
    _WIP_KEY_STYLE     = _WIP_KEY_LITERAL|_WIP_KEY_FOLDED|_WIP_KEY_SQUO|_WIP_KEY_DQUO|_WIP_KEY_PLAIN,
    _WIP_VAL_STYLE     = _WIP_VAL_LITERAL|_WIP_VAL_FOLDED|_WIP_VAL_SQUO|_WIP_VAL_DQUO|_WIP_VAL_PLAIN,
    _WIP_KEY_FT_NL     = c4bit(27), ///< features: mark key scalar as having \n in its contents
    _WIP_VAL_FT_NL     = c4bit(28), ///< features: mark val scalar as having \n in its contents
    _WIP_KEY_FT_SQ     = c4bit(29), ///< features: mark key scalar as having single quotes in its contents
    _WIP_VAL_FT_SQ     = c4bit(30), ///< features: mark val scalar as having single quotes in its contents
    _WIP_KEY_FT_DQ     = c4bit(31), ///< features: mark key scalar as having double quotes in its contents
    _WIP_VAL_FT_DQ     = c4bit(32), ///< features: mark val scalar as having double quotes in its contents
    #undef c4bit
} NodeType_e;


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/** wraps a NodeType_e element with some syntactic sugar and predicates */
struct NodeType
{
public:

    NodeType_e type;

public:

    C4_ALWAYS_INLINE NodeType() : type(NOTYPE) {}
    C4_ALWAYS_INLINE NodeType(NodeType_e t) : type(t) {}
    C4_ALWAYS_INLINE NodeType(type_bits t) : type((NodeType_e)t) {}

    C4_ALWAYS_INLINE const char *type_str() const { return type_str(type); }
    static const char* type_str(NodeType_e t);

    C4_ALWAYS_INLINE void set(NodeType_e t) { type = t; }
    C4_ALWAYS_INLINE void set(type_bits  t) { type = (NodeType_e)t; }

    C4_ALWAYS_INLINE void add(NodeType_e t) { type = (NodeType_e)(type|t); }
    C4_ALWAYS_INLINE void add(type_bits  t) { type = (NodeType_e)(type|t); }

    C4_ALWAYS_INLINE void rem(NodeType_e t) { type = (NodeType_e)(type & ~t); }
    C4_ALWAYS_INLINE void rem(type_bits  t) { type = (NodeType_e)(type & ~t); }

    C4_ALWAYS_INLINE void clear() { type = NOTYPE; }

public:

    C4_ALWAYS_INLINE operator NodeType_e      & C4_RESTRICT ()       { return type; }
    C4_ALWAYS_INLINE operator NodeType_e const& C4_RESTRICT () const { return type; }

    C4_ALWAYS_INLINE bool operator== (NodeType_e t) const { return type == t; }
    C4_ALWAYS_INLINE bool operator!= (NodeType_e t) const { return type != t; }

public:

    #if defined(__clang__)
    #   pragma clang diagnostic push
    #   pragma clang diagnostic ignored "-Wnull-dereference"
    #elif defined(__GNUC__)
    #   pragma GCC diagnostic push
    #   if __GNUC__ >= 6
    #       pragma GCC diagnostic ignored "-Wnull-dereference"
    #   endif
    #endif

    C4_ALWAYS_INLINE bool is_notype() const { return type == NOTYPE; }
    C4_ALWAYS_INLINE bool is_stream() const { return ((type & STREAM) == STREAM) != 0; }
    C4_ALWAYS_INLINE bool is_doc() const { return (type & DOC) != 0; }
    C4_ALWAYS_INLINE bool is_container() const { return (type & (MAP|SEQ|STREAM)) != 0; }
    C4_ALWAYS_INLINE bool is_map() const { return (type & MAP) != 0; }
    C4_ALWAYS_INLINE bool is_seq() const { return (type & SEQ) != 0; }
    C4_ALWAYS_INLINE bool has_key() const { return (type & KEY) != 0; }
    C4_ALWAYS_INLINE bool has_val() const { return (type & VAL) != 0; }
    C4_ALWAYS_INLINE bool is_val() const { return (type & KEYVAL) == VAL; }
    C4_ALWAYS_INLINE bool is_keyval() const { return (type & KEYVAL) == KEYVAL; }
    C4_ALWAYS_INLINE bool has_key_tag() const { return (type & (KEY|KEYTAG)) == (KEY|KEYTAG); }
    C4_ALWAYS_INLINE bool has_val_tag() const { return ((type & VALTAG) && (type & (VAL|MAP|SEQ))); }
    C4_ALWAYS_INLINE bool has_key_anchor() const { return (type & (KEY|KEYANCH)) == (KEY|KEYANCH); }
    C4_ALWAYS_INLINE bool is_key_anchor() const { return (type & (KEY|KEYANCH)) == (KEY|KEYANCH); }
    C4_ALWAYS_INLINE bool has_val_anchor() const { return (type & VALANCH) != 0 && (type & (VAL|SEQ|MAP)) != 0; }
    C4_ALWAYS_INLINE bool is_val_anchor() const { return (type & VALANCH) != 0 && (type & (VAL|SEQ|MAP)) != 0; }
    C4_ALWAYS_INLINE bool has_anchor() const { return (type & (KEYANCH|VALANCH)) != 0; }
    C4_ALWAYS_INLINE bool is_anchor() const { return (type & (KEYANCH|VALANCH)) != 0; }
    C4_ALWAYS_INLINE bool is_key_ref() const { return (type & KEYREF) != 0; }
    C4_ALWAYS_INLINE bool is_val_ref() const { return (type & VALREF) != 0; }
    C4_ALWAYS_INLINE bool is_ref() const { return (type & (KEYREF|VALREF)) != 0; }
    C4_ALWAYS_INLINE bool is_anchor_or_ref() const { return (type & (KEYANCH|VALANCH|KEYREF|VALREF)) != 0; }
    C4_ALWAYS_INLINE bool is_key_quoted() const { return (type & (KEY|KEYQUO)) == (KEY|KEYQUO); }
    C4_ALWAYS_INLINE bool is_val_quoted() const { return (type & (VAL|VALQUO)) == (VAL|VALQUO); }
    C4_ALWAYS_INLINE bool is_quoted() const { return (type & (KEY|KEYQUO)) == (KEY|KEYQUO) || (type & (VAL|VALQUO)) == (VAL|VALQUO); }

    // these predicates are a work in progress and subject to change. Don't use yet.
    C4_ALWAYS_INLINE bool default_block() const { return (type & (_WIP_STYLE_BLOCK|_WIP_STYLE_FLOW_ML|_WIP_STYLE_FLOW_SL)) == 0; }
    C4_ALWAYS_INLINE bool marked_block() const { return (type & (_WIP_STYLE_BLOCK)) != 0; }
    C4_ALWAYS_INLINE bool marked_flow_sl() const { return (type & (_WIP_STYLE_FLOW_SL)) != 0; }
    C4_ALWAYS_INLINE bool marked_flow_ml() const { return (type & (_WIP_STYLE_FLOW_ML)) != 0; }
    C4_ALWAYS_INLINE bool marked_flow() const { return (type & (_WIP_STYLE_FLOW_ML|_WIP_STYLE_FLOW_SL)) != 0; }
    C4_ALWAYS_INLINE bool key_marked_literal() const { return (type & (_WIP_KEY_LITERAL)) != 0; }
    C4_ALWAYS_INLINE bool val_marked_literal() const { return (type & (_WIP_VAL_LITERAL)) != 0; }
    C4_ALWAYS_INLINE bool key_marked_folded() const { return (type & (_WIP_KEY_FOLDED)) != 0; }
    C4_ALWAYS_INLINE bool val_marked_folded() const { return (type & (_WIP_VAL_FOLDED)) != 0; }
    C4_ALWAYS_INLINE bool key_marked_squo() const { return (type & (_WIP_KEY_SQUO)) != 0; }
    C4_ALWAYS_INLINE bool val_marked_squo() const { return (type & (_WIP_VAL_SQUO)) != 0; }
    C4_ALWAYS_INLINE bool key_marked_dquo() const { return (type & (_WIP_KEY_DQUO)) != 0; }
    C4_ALWAYS_INLINE bool val_marked_dquo() const { return (type & (_WIP_VAL_DQUO)) != 0; }
    C4_ALWAYS_INLINE bool key_marked_plain() const { return (type & (_WIP_KEY_PLAIN)) != 0; }
    C4_ALWAYS_INLINE bool val_marked_plain() const { return (type & (_WIP_VAL_PLAIN)) != 0; }

    #if defined(__clang__)
    #   pragma clang diagnostic pop
    #elif defined(__GNUC__)
    #   pragma GCC diagnostic pop
    #endif

};


} // namespace yml
} // namespace c4

C4_SUPPRESS_WARNING_MSVC_POP
C4_SUPPRESS_WARNING_GCC_CLANG_POP

#endif /* C4_YML_NODE_TYPE_HPP_ */
