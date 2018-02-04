#ifndef _C4_RYML_TO_STR_HPP_
#define _C4_RYML_TO_STR_HPP_

#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "./common.hpp"

namespace c4 {
namespace yml {

#define _C4_DEFINE_TO_FROM_STR(ty, pri_fmt, scn_fmt)                    \
inline size_t to_str(span buf, ty v)                                    \
{                                                                       \
    return snprintf(buf.str, buf.len, "%" pri_fmt, v);                  \
}                                                                       \
inline bool from_str(cspan buf, ty *v)                                  \
{                                                                       \
    /* Alas, there's no snscanf, which is absolutely needed here. */    \
    /* We fake snscanf with a dynamic field delimiter; this trick is */ \
    /* taken from https://stackoverflow.com/a/18368910/5875572 */       \
    char fmt[8];                                                        \
    int ret = snprintf(fmt, sizeof(fmt), "%%" "%zu" scn_fmt, buf.len);  \
    C4_ASSERT(ret < sizeof(fmt));                                       \
    ret = sscanf(buf.str, fmt, v);                                      \
    return ret == 1;                                                    \
}

_C4_DEFINE_TO_FROM_STR(void*   , "p"             , "p"             )
_C4_DEFINE_TO_FROM_STR(char    , "c"             , "c"             )
_C4_DEFINE_TO_FROM_STR(double  , "lg"            , "lg"            )
_C4_DEFINE_TO_FROM_STR(float   , "g"             , "g"             )
_C4_DEFINE_TO_FROM_STR( int8_t , PRId8 /*"%hhd"*/, SCNd8 /*"%hhd"*/)
_C4_DEFINE_TO_FROM_STR(uint8_t , PRIu8 /*"%hhu"*/, SCNu8 /*"%hhu"*/)
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

#endif /* _C4_RYML_TO_STR_HPP_ */
