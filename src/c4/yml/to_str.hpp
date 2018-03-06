#ifndef _C4_YML_TO_STR_HPP_
#define _C4_YML_TO_STR_HPP_

#include <stdio.h>
#include <inttypes.h>
#include <type_traits>

#include "./subs.hpp"

namespace c4 {
namespace yml {

//-----------------------------------------------------------------------------
/** serialize the arguments to the given span.
 * @return the number of characters written to the buffer. */
template< class Arg, class... Args >
size_t cat(subs buf, Arg const& a, Args const& ...more)
{
    size_t num = to_str(buf, a);
    buf = buf.len >= num ? buf.sub(num) : subs{};
    num += cat(buf, more...);
    return num;
}

inline size_t cat(subs /*buf*/)
{
    return 0;
}

/** deserialize the arguments from the given span.
 *
 * @return the number of characters read from the buffer. If a
 * conversion was not successful, return npos. */
template< class Arg, class... Args >
size_t uncat(csubs buf, Arg & a, Args & ...more)
{
    size_t num = from_str_untrimmed(buf, &a);
    if(num == npos) return npos;
    buf = buf.len >= num ? buf.sub(num) : subs{};
    num += uncat(buf, more...);
    return num;
}

inline size_t uncat(csubs /*buf*/)
{
    return 0;
}

//-----------------------------------------------------------------------------

template< class Sep, class Arg, class... Args >
size_t catsep(subs buf, Sep const& sep, Arg const& a, Args const& ...more)
{
    size_t num = to_str(buf, sep);
    buf = buf.len >= num ? buf.sub(num) : subs{};
    num += to_str(buf, a);
    buf = buf.len >= num ? buf.sub(num) : subs{};
    num += catsep(buf, sep, more...);
    return num;
}

template< class Sep >
inline size_t catsep(subs /*buf*/, Sep const& /*sep*/)
{
    return 0;
}


//-----------------------------------------------------------------------------

template< class CharOwningContainer, class... Args >
inline void catrs(CharOwningContainer *cont, Args const& ...args)
{
    subs buf = to_subs(*cont);
    size_t ret = cat(buf, args...);
    cont->resize(ret);
    if(ret > buf.len)
    {
        buf = to_subs(*cont);
        ret = cat(buf, args...);
    }
}

template< class CharOwningContainer, class Sep, class... Args >
inline void catseprs(CharOwningContainer *cont, Sep const& sep, Args const& ...args)
{
    subs buf = to_subs(*cont);
    size_t ret = catsep(buf, sep, args...);
    cont->resize(ret);
    if(ret > buf.len)
    {
        buf = to_subs(*cont);
        ret = catsep(buf, sep, args...);
    }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// Helper macro: appends chars to the buffer, without overflow. Always counts.
#define _c4append(c) { if(pos < buf.len) { buf.str[pos++] = (c); } else { ++pos; } }

template< class T >
size_t itoa(subs buf, T v)
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
        buf.reverse_range(1, pos <= buf.len ? pos : buf.len);
    }
    else
    {
        do {
            _c4append((char)(v % 10) + '0');
            v /= 10;
        } while(v);
        buf.reverse_range(0, pos <= buf.len ? pos : buf.len);
    }
    return pos;
}


//-----------------------------------------------------------------------------
template< class T >
size_t utoa(subs buf, T v)
{
    static_assert(std::is_integral<T>::value, "must be integral type");
    size_t pos = 0;
    do {
        _c4append((char)(v % 10) + '0');
        v /= 10;
    } while(v);
    buf.reverse_range(0, pos <= buf.len ? pos : buf.len);
    return pos;
}

#undef _c4append


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template< class T >
inline bool atoi(csubs str, T *v)
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

template< class T >
inline size_t atoi_untrimmed(csubs str, T *v)
{
    csubs trimmed = str.first_int_span();
    if(trimmed.len == 0) return npos;
    if(atoi(trimmed, v)) return trimmed.end() - str.begin();
    return npos;
}

//-----------------------------------------------------------------------------
template< class T >
inline bool atou(csubs str, T *v)
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


template< class T >
inline size_t atou_untrimmed(csubs str, T *v)
{
    csubs trimmed = str.first_uint_span();
    if(trimmed.len == 0) return npos;
    if(atou(trimmed, v)) return trimmed.end() - str.begin();
    return npos;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#define _C4_DEFINE_TO_FROM_STR_TOA(ty, id)          \
                                                    \
inline size_t to_str(subs buf, ty v)                \
{                                                   \
    return id##toa<ty>(buf, v);                     \
}                                                   \
                                                    \
inline bool from_str(csubs buf, ty *v)              \
{                                                   \
    return ato##id<ty>(buf, v);                     \
}                                                   \
                                                    \
inline size_t from_str_untrimmed(csubs buf, ty *v)  \
{                                                   \
    return ato##id##_untrimmed<ty>(buf, v);         \
}

#ifdef _MSC_VER
#   pragma warning(push)
#   pragma warning(disable: 4800) //'int': forcing value to bool 'true' or 'false' (performance warning)
#   pragma warning(disable: 4996) // snprintf/scanf: this function or variable may be unsafe
#endif

template< class T >
inline subs to_str_span(subs buf, T const& v)
{
    size_t sz = to_str(buf, v);
    return buf.left_of(sz <= buf.len ? sz : buf.len);
}

#ifdef _MSC_VER
#define _C4_DEFINE_TO_STR(ty, pri_fmt)                                  \
inline size_t to_str(subs buf, ty v)                                    \
{                                                                       \
    /** use _snprintf() to prevent early termination of the output      \
     * for writing the null character at the last position              \
     * @see https://msdn.microsoft.com/en-us/library/2ts7cx93.aspx */   \
    int iret = _snprintf(buf.str, buf.len, "%" pri_fmt, v);             \
    if(iret < 0)                                                        \
    {                                                                   \
        /* when buf.len is not enough, VS returns a negative value.     \
         * so call it again with a negative value for getting an        \
         * actual length of the string */                               \
        iret = snprintf(nullptr, 0, "%" pri_fmt, v);                    \
        C4_ASSERT(iret > 0);                                            \
    }                                                                   \
    size_t ret = (size_t) iret;                                         \
    return ret;                                                         \
}
#else
#define _C4_DEFINE_TO_STR(ty, pri_fmt)\
inline size_t to_str(subs buf, ty v)                                    \
{                                                                       \
    int iret = snprintf(buf.str, buf.len, "%" pri_fmt, v);              \
    C4_ASSERT(iret >= 0);                                               \
    if(iret == buf.len)                                                 \
    {                                                                   \
        ++iret; /* snprintf() reserves the last character to write \0 */\
    }                                                                   \
    return ret;                                                         \
}
#endif

/** this macro defines to_str()/from_str() pairs for intrinsic types. */
#define _C4_DEFINE_TO_FROM_STR(ty, pri_fmt, scn_fmt)                    \
                                                                        \
_C4_DEFINE_TO_STR(ty, pri_fmt)                                          \
                                                                        \
inline size_t from_str_untrimmed(csubs buf, ty *v)                      \
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
    /* this is the actual format we'll use for scanning */              \
    char fmt[12];                                                       \
    /* write the length into it. Eg "%12d" for an int (scn_fmt="d").    \
     * Also, get the number of characters read from the string.         \
     * So the final format ends up as "%12d%n"*/                        \
    int ret = snprintf(fmt, sizeof(fmt), "%%""%zu" scn_fmt "%%n", buf.len);\
    /* no nasty surprises, please! */                                   \
    C4_ASSERT(size_t(ret) < sizeof(fmt));                               \
    /* now we scan with confidence that the span length is respected */ \
    int num_chars;                                                      \
    ret = sscanf(buf.str, fmt, v, &num_chars);                          \
    /* scanf returns the number of successful conversions */            \
    if(ret != 1) return npos;                                           \
    return (size_t)(num_chars);                                         \
}                                                                       \
                                                                        \
inline bool from_str(csubs buf, ty *v)                                  \
{                                                                       \
    size_t num = from_str_untrimmed(buf, v);                            \
    return (num != npos);                                               \
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


//-----------------------------------------------------------------------------
inline size_t to_str(subs buf, bool v)
{
    int val = v;
    return to_str(buf, val);
}

inline bool from_str(csubs buf, bool *v)
{
    int val;
    bool ret = from_str(buf, &val);
    *v = (bool)val;
    return ret;
}

inline size_t from_str_untrimmed(csubs buf, bool *v)
{
    int val;
    size_t ret = from_str_untrimmed(buf, &val);
    *v = (bool)val;
    return ret;
}

#ifdef _MSC_VER
#   pragma warning(pop)
#endif

//-----------------------------------------------------------------------------
inline size_t to_str(subs buf, char v)
{
    if(buf.len > 0) buf[0] = v;
    return 1;
}

inline bool from_str(csubs buf, char *v)
{
    if(buf.len != 1) return false;
    *v = buf[0];
    return true;
}

inline size_t from_str_untrimmed(csubs buf, char *v)
{
    if(buf.len < 1) return npos;
    *v = buf[0];
    return 1;
}

//-----------------------------------------------------------------------------
inline size_t to_str(subs buf, csubs const& v)
{
    size_t len = buf.len < v.len ? buf.len : v.len;
    memcpy(buf.str, v.str, len);
    return v.len;
}

inline bool from_str(csubs buf, csubs *v)
{
    *v = buf;
    return true;
}

inline size_t from_str_untrimmed(subs buf, csubs *v)
{
    csubs trimmed = buf.first_non_empty_span();
    if(trimmed.len == 0) return npos;
    *v = trimmed;
    return trimmed.end() - buf.begin();
}

//-----------------------------------------------------------------------------
inline size_t to_str(subs buf, subs const& v)
{
    size_t len = buf.len < v.len ? buf.len : v.len;
    memcpy(buf.str, v.str, len);
    return v.len;
}

inline bool from_str(csubs buf, subs *v)
{
    size_t len = buf.len > v->len ? v->len : buf.len;
    memcpy(v->str, buf.str, len);
    return buf.len <= v->len;
}

inline size_t from_str_untrimmed(csubs buf, subs *v)
{
    csubs trimmed = buf.first_non_empty_span();
    if(trimmed.len == 0) return npos;
    size_t len = trimmed.len > v->len ? v->len : trimmed.len;
    memcpy(v->str, trimmed.str, len);
    if(trimmed.len > v->len) return npos;
    return trimmed.end() - buf.begin();
}

//-----------------------------------------------------------------------------

template< size_t N >
inline size_t to_str(subs buf, const char (&v)[N])
{
    csubs sp(v);
    return to_str(buf, sp);
}

inline size_t to_str(subs buf, const char *v)
{
    return to_str(buf, to_csubs(v));
}

} // namespace yml
} // namespace c4

#endif /* _C4_YML_TO_STR_HPP_ */
