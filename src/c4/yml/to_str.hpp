#ifndef _C4_YML_TO_STR_HPP_
#define _C4_YML_TO_STR_HPP_

#include <stdio.h>
#include <inttypes.h>

#include "./span.hpp"

namespace c4 {
namespace yml {

/** this macro defines a to_str()/from_str() pairs for intrinsic types. */
#define _C4_DEFINE_TO_FROM_STR(ty, pri_fmt, scn_fmt)                    \
                                                                        \
inline size_t to_str(span buf, ty v)                                    \
{                                                                       \
    return snprintf(buf.str, buf.len, "%" pri_fmt, v);                  \
}                                                                       \
                                                                        \
inline bool from_str(cspan buf, ty *v)                                  \
{                                                                       \
    /* snscanf() is absolutely needed here as we must be sure that      \
     * buf.len is strictly respected, because the span string is        \
     * generally not null-terminated.                                   \
     *                                                                  \
     * Alas, there is no snscanf().                                     \
     *                                                                  \
     * So we fake it by using a dynamic format with an explicit         \
     * field size set to the length of the given span.                  \
     * This trick is taken from:                                        \
     * https://stackoverflow.com/a/18368910/5875572 */                  \
                                                                        \
    /* this is the actual format used for scanning */                   \
    char fmt[8];                                                        \
    /* write the length into it. Eg "%12d" for an int (scn_fmt="d") */  \
    int ret = snprintf(fmt, sizeof(fmt), "%%""%zu" scn_fmt, buf.len);   \
    /* no nasty surprises, please! */                                   \
    C4_ASSERT(size_t(ret) < sizeof(fmt));                               \
    /* now we scan with confidence that the span length is respected */ \
    ret = sscanf(buf.str, fmt, v);                                      \
    /* scanf returns the number of successful conversions */            \
    return ret == 1;                                                    \
}

_C4_DEFINE_TO_FROM_STR(void*   , "p"             , "p"             )
_C4_DEFINE_TO_FROM_STR(char    , "c"             , "c"             )
_C4_DEFINE_TO_FROM_STR(double  , "lg"            , "lg"            )
_C4_DEFINE_TO_FROM_STR(float   , "g"             , "g"             )
_C4_DEFINE_TO_FROM_STR(  int8_t, PRId8 /*"%hhd"*/, SCNd8 /*"%hhd"*/)
_C4_DEFINE_TO_FROM_STR( uint8_t, PRIu8 /*"%hhu"*/, SCNu8 /*"%hhu"*/)
_C4_DEFINE_TO_FROM_STR( int16_t, PRId16/*"%hd" */, SCNd16/*"%hd" */)
_C4_DEFINE_TO_FROM_STR(uint16_t, PRIu16/*"%hu" */, SCNu16/*"%hu" */)
_C4_DEFINE_TO_FROM_STR( int32_t, PRId32/*"%d"  */, SCNd32/*"%d"  */)
_C4_DEFINE_TO_FROM_STR(uint32_t, PRIu32/*"%u"  */, SCNu32/*"%u"  */)
_C4_DEFINE_TO_FROM_STR( int64_t, PRId64/*"%lld"*/, SCNd64/*"%lld"*/)
_C4_DEFINE_TO_FROM_STR(uint64_t, PRIu64/*"%llu"*/, SCNu64/*"%llu"*/)

#undef _C4_DEFINE_TO_FROM_STR

inline size_t to_str(span buf, cspan const& v)
{
    size_t len = buf.len < v.len ? buf.len : v.len;
    memcpy(buf.str, v.str, len);
    return v.len;
}
inline bool from_str(cspan buf, span *v)
{
    size_t len = buf.len > v->len ? v->len : buf.len;
    memcpy(v->str, buf.str, len);
    return buf.len <= v->len;
}

template< size_t N >
inline size_t to_str(span buf, const char (&v)[N])
{
    cspan sp;
    sp.assign<N>(v);
    return to_str(buf, sp);
}

inline int to_str(span buf, const char *v)
{
    cspan sp(v);
    return to_str(buf, sp);
}

} // namespace yml
} // namespace c4

#endif /* _C4_YML_TO_STR_HPP_ */
