#ifndef _C4_YML_TO_STR_HPP_
#define _C4_YML_TO_STR_HPP_

#include <stdio.h>
#include <inttypes.h>
#include <type_traits>

#include "./span.hpp"

namespace c4 {
namespace yml {

//-----------------------------------------------------------------------------

template< class Arg, class... Args >
size_t cat(span buf, Arg const& a, Args const& ...more)
{
    size_t num = to_str(buf, a);
    buf = buf.len >= num ? buf.subspan(num) : span{};
    num += cat(buf, more...);
    return num;
}

inline size_t cat(span /*buf*/)
{
    return 0;
}

//-----------------------------------------------------------------------------

template< class Sep, class Arg, class... Args >
size_t catsep(span buf, Sep const& sep, Arg const& a, Args const& ...more)
{
    size_t num = to_str(buf, sep);
    buf = buf.len >= num ? buf.subspan(num) : span{};
    num += to_str(buf, a);
    buf = buf.len >= num ? buf.subspan(num) : span{};
    num += catsep(buf, sep, more...);
    return num;
}

template< class Sep >
inline size_t catsep(span /*buf*/, Sep const& /*sep*/)
{
    return 0;
}

//-----------------------------------------------------------------------------

template< class CharOwningContainer, class... Args >
inline void catrs(CharOwningContainer *cont, Args const& ...args)
{
    span buf(cont->empty() ? nullptr : &(*cont)[0], cont->size());
    size_t ret = cat(buf, args...);
    cont->resize(ret);
    if(ret > buf.len)
    {
        buf.assign(&(*cont)[0], cont->size());
        ret = cat(buf, args...);
    }
}

template< class CharOwningContainer, class Sep, class... Args >
inline void catseprs(CharOwningContainer *cont, Sep const& sep, Args const& ...args)
{
    span buf(cont->empty() ? nullptr : &(*cont)[0], cont->size());
    size_t ret = catsep(buf, sep, args...);
    cont->resize(ret);
    if(ret > buf.len)
    {
        buf.assign(&(*cont)[0], cont->size());
        ret = catsep(buf, sep, args...);
    }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// Helper macro: appends chars to the buffer, without overflow. Always counts.
#define _c4append(c) { if(pos < buf.len) { buf.str[pos++] = (c); } else { ++pos; } }

template< class T >
size_t itoa(span buf, T v)
{
    static_assert(std::is_integral<T>::value, "must be integral type");
    size_t pos = 0;
    if(v < 0)
    {
        _c4append('-');
        v = -v;
        do {
            _c4append((char)(v % 10) + '0');
            v /= 10;
        } while(v);
        if(pos <= buf.len)
        {
            buf.reverse_range(1, pos);
        }
    }
    else
    {
        do {
            _c4append((char)(v % 10) + '0');
            v /= 10;
        } while(v);
        if(pos <= buf.len)
        {
            buf.reverse_range(0, pos);
        }
    }
    return pos;
}

//-----------------------------------------------------------------------------
template< class T >
size_t utoa(span buf, T v)
{
    static_assert(std::is_integral<T>::value, "must be integral type");
    size_t pos = 0;
    do {
        _c4append((char)(v % 10) + '0');
        v /= 10;
    } while(v);
    if(pos <= buf.len)
    {
        buf.reverse_range(0, pos);
    }
    return pos;
}

#undef _c4append


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template< class T >
inline bool atoi(cspan str, T *v)
{
    static_assert(std::is_integral<T>::value, "must be integral type");
    C4_ASSERT(str.len > 0);
    C4_ASSERT(!str.begins_with(' '));
    C4_ASSERT(!str.ends_with(' '));
    T n = 0;
    if(str[0] != '-')
    {
        for(size_t i = 0; i < str.len; ++i)
        {
            char c = str.str[i];
            if(c < '0' || c > '9') return false;
            n = n*10 + (c-'0');
        }
    }
    else
    {
        for(size_t i = 1; i < str.len; ++i)
        {
            char c = str.str[i];
            if(c < '0' || c > '9') return false;
            n = n*10 + (c-'0');
        }
        n = -n;
    }
    *v = n;
    return true;
}

//-----------------------------------------------------------------------------
template< class T >
inline bool atou(cspan str, T *v)
{
    static_assert(std::is_integral<T>::value, "must be integral type");
    C4_ASSERT(str.len > 0);
    C4_ASSERT(!str.begins_with(' '));
    C4_ASSERT(!str.ends_with(' '));
    C4_ERROR_IF(str.str[0] == '-', "must be positive");
    T n = 0;
    for(size_t i = 0; i < str.len; ++i)
    {
        char c = str.str[i];
        if(c < '0' || c > '9') return false;
        n = n*10 + (c-'0');
    }
    *v = n;
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#define _C4_DEFINE_TO_FROM_STR_TOA(ty, id)      \
                                                \
inline size_t to_str(span buf, ty v)            \
{                                               \
    return id##toa<ty>(buf, v);                 \
}                                               \
                                                \
inline bool from_str(cspan buf, ty *v)          \
{                                               \
    return ato##id<ty>(buf, v);                 \
}

#ifdef _MSC_VER
#   pragma warning(push)
#   pragma warning(disable: 4996) // snprintf/scanf: this function or variable may be unsafe
#endif

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
_C4_DEFINE_TO_FROM_STR(double  , "lg"            , "lg"            )
_C4_DEFINE_TO_FROM_STR(float   , "g"             , "g"             )
//_C4_DEFINE_TO_FROM_STR(char    , "c"             , "c"             )
//_C4_DEFINE_TO_FROM_STR(  int8_t, PRId8 /*"%hhd"*/, SCNd8 /*"%hhd"*/)
//_C4_DEFINE_TO_FROM_STR( uint8_t, PRIu8 /*"%hhu"*/, SCNu8 /*"%hhu"*/)
//_C4_DEFINE_TO_FROM_STR( int16_t, PRId16/*"%hd" */, SCNd16/*"%hd" */)
//_C4_DEFINE_TO_FROM_STR(uint16_t, PRIu16/*"%hu" */, SCNu16/*"%hu" */)
//_C4_DEFINE_TO_FROM_STR( int32_t, PRId32/*"%d"  */, SCNd32/*"%d"  */)
//_C4_DEFINE_TO_FROM_STR(uint32_t, PRIu32/*"%u"  */, SCNu32/*"%u"  */)
//_C4_DEFINE_TO_FROM_STR( int64_t, PRId64/*"%lld"*/, SCNd64/*"%lld"*/)
//_C4_DEFINE_TO_FROM_STR(uint64_t, PRIu64/*"%llu"*/, SCNu64/*"%llu"*/)
_C4_DEFINE_TO_FROM_STR_TOA(  int8_t, i)
_C4_DEFINE_TO_FROM_STR_TOA( int16_t, i)
_C4_DEFINE_TO_FROM_STR_TOA( int32_t, i)
_C4_DEFINE_TO_FROM_STR_TOA( int64_t, i)
_C4_DEFINE_TO_FROM_STR_TOA( uint8_t, u)
_C4_DEFINE_TO_FROM_STR_TOA(uint16_t, u)
_C4_DEFINE_TO_FROM_STR_TOA(uint32_t, u)
_C4_DEFINE_TO_FROM_STR_TOA(uint64_t, u)

#undef _C4_DEFINE_TO_FROM_STR
#undef _C4_DEFINE_TO_FROM_STR_TOA

#ifdef _MSC_VER
#   pragma warning(pop)
#endif


inline size_t to_str(span buf, bool v)
{
    int val = v;
    return to_str(buf, val);
}

inline size_t from_str(cspan buf, bool *v)
{
    int val;
    size_t ret = from_str(buf, &val);
    *v = (bool)val;
    return ret;
}

inline size_t to_str(span buf, char v)
{
    if(buf.len > 0) buf[0] = v;
    return 1;
}

inline bool from_str(cspan buf, char *v)
{
    if(buf.len != 1) return false;
    *v = buf[0];
    return true;
}

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

inline bool from_str(cspan buf, cspan *v)
{
    *v = buf;
    return true;
}

template< size_t N >
inline size_t to_str(span buf, const char (&v)[N])
{
    cspan sp;
    sp.assign<N>(v);
    return to_str(buf, sp);
}

inline size_t to_str(span buf, const char *v)
{
    cspan sp(v);
    return to_str(buf, sp);
}

} // namespace yml
} // namespace c4

#endif /* _C4_YML_TO_STR_HPP_ */
