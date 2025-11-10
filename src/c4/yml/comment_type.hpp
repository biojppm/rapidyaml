#ifndef C4_YML_COMMENT_TYPE_HPP_
#define C4_YML_COMMENT_TYPE_HPP_

#include <stdint.h>

#ifdef RYML_WITH_COMMENTS

#ifndef _C4_YML_COMMON_HPP_
#include "c4/yml/common.hpp"
#endif

namespace c4 {
namespace yml {

/** @addtogroup doc_node_type
 *
 * @{
 */

using comment_data_type = uint32_t;

/**
 *
 *
 *
 *
 * Flow seq:
 * ```yaml
 * # LEADING
 * [
 *    #                  LEADING
 *    val1, #            TRAILING
 *    #                  LEADING
 *    val2 #             TRAILING
 * ] # TRAILING
 * ```
 *
 * Flow map:
 * ```yaml
 * {
 *    #                  LEADING
 *    key1: val1, #      TRAILING
 *    #                  LEADING
 *    key2: val2 #       TRAILING
 * }
 * ```
 *
 * Block seq:
 * ```yaml
 * #                     LEADING
 * - val1 #              TRAILING
 * #                     LEADING
 * - val2 #              TRAILING
 * ```
 *
 * Block map:
 * ```yaml
 * #                     LEADING
 * key1: val1 #          TRAILING
 * #                     LEADING
 * key2: val2 #          TRAILING
 * ```
 *
 *
 * ```yaml
 * --- # Flow map        TRAILING (node 4)
 * #                     LEADING (container node)
 * { #                   VAL_TRAILING_OPEN (container node)
 *
 *   #                   LEADING
 *   key: val, #         TRAILING
 *
 *   #                   LEADING
 *   key: #              COLON_TRAILING
 *      #                VAL_LEADING
 *      val, #           TRAILING
 *      #                VAL_FOOTER
 *
 *   #                   LEADING
 *   key #               KEY_TRAILING
 *   #                   KEY_FOOTER
 *   : #                 COLON_TRAILING
 *      #                VAL_LEADING
 *      val, #           TRAILING
 *
 *   #                   LEADING
 *   key #               KEY_TRAILING
 *   #                   KEY_FOOTER
 *   : #                 COLON_TRAILING
 *      #                VAL_LEADING
 *      val #            VAL_TRAILING
 *      #                COMMA_LEADING
 *      , #              TRAILING
 *
 *   # 1                 LEADING
 *   # 2                 KEY_TAG_LEADING
 *   !keytag # 3         KEY_TAG_TRAILING
 *   # 4                 KEY_ANCHOR_LEADING
 *   &keyanchor # 5      KEY_ANCHOR_TRAILING
 *   # 6                 KEY_LEADING
 *   key # 7             KEY_TRAILING
 *   # 8                 COLON_LEADING
 *   : # 9               COLON_TRAILING
 *   # 10                VAL_LEADING
 *   # 11                VAL_TAG_LEADING
 *   !valtag # 12        VAL_TAG_TRAILING
 *   # 13                VAL_ANCHOR_LEADING
 *   &valanchor # 14     VAL_ANCHOR_TRAILING
 *   # 15                VAL_LEADING2
 *   val # 16            TRAILING
 *   # 17                FOOTER (child node)
 * } #                   TRAILING (container node)
 * #                     VAL_FOOTER (container node)
 * ```
 */
typedef enum : comment_data_type {
#define _RYML_DEFINE_COMMENTS(macro)                        \
    /* STREAM */                                            \
    macro(DOC_TRAILING, 0)         /* trailing ---  */      \
    macro(LEADING, 1)              /* leading, GENERAL */   \
    macro(KEY_TRAILING_QMRK, 2)    /* trailing ? */         \
    macro(KEY_FOOTER_QMRK, 3)      /* footer ? */           \
    macro(KEY_ANCHOR_LEADING, 4)                            \
    macro(KEY_ANCHOR_TRAILING, 5)                           \
    macro(KEY_TAG_LEADING, 6)                               \
    macro(KEY_TAG_TRAILING, 7)                              \
    macro(KEY_LEADING, 8)                                   \
    macro(KEY_BRACKET_TRAILING, 9)                          \
    macro(KEY_TRAILING, 10)                                 \
    macro(KEY_FOOTER, 11)                                   \
    macro(COLON_LEADING, 12)                                \
    macro(COLON_TRAILING, 13)                               \
    macro(VAL_DASH_TRAILING, 14)                            \
    macro(VAL_LEADING, 15)                                  \
    macro(VAL_ANCHOR_LEADING, 16)                           \
    macro(VAL_ANCHOR_TRAILING, 17)                          \
    macro(VAL_TAG_LEADING, 18)                              \
    macro(VAL_TAG_TRAILING, 19)                             \
    macro(VAL_LEADING2, 20)                                 \
    macro(VAL_BRACKET_TRAILING, 21)                         \
    macro(VAL_BRACKET_LEADING, 22)                          \
    macro(VAL_TRAILING, 23)                                 \
    macro(COMMA_LEADING, 24)                                \
    macro(TRAILING, 25)                                     \
    macro(FOOTER, 26)

    #define _c4comm(comm_symbol, bit) COMM_##comm_symbol = (UINT32_C(1) << UINT32_C(bit)),
    _RYML_DEFINE_COMMENTS(_c4comm)
    _c4comm(LAST_, 26)
    COMM_ANY = (COMM_LAST_ << 1u) - 1u,
    COMM_NONE = 0,
    #undef _c4comm
} CommentType_e;


const char *comment_type_str(CommentType_e type);

/** @} */

} // namespace yml
} // namespace c4

#endif // RYML_WITH_COMMENTS

#endif /* C4_YML_COMMENT_TYPE_HPP_ */
