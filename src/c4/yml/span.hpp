#ifndef _C4_YML_SPAN_HPP_
#define _C4_YML_SPAN_HPP_

#include <string.h>
#include "./common.hpp"
#include <type_traits>

namespace c4 {
namespace yml {

template< class C > class basic_span;

using span = basic_span< char >;
using cspan = basic_span< const char >;

template< class OStream, class C >
OStream& operator<< (OStream& s, basic_span< C > const& sp)
{
    s.write(sp.str, sp.len);
    return s;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/** a span of characters and a length. Works like a writeable string_view. */
template< class C >
class basic_span
{
public:

    C *str;
    size_t len;

public:

    using  CC = typename std::add_const< C >::type;
    using NCC = typename std::remove_const< C >::type;

    using basic_cspan  = basic_span<  CC >;
    using basic_ncspan = basic_span< NCC >;

    using char_type = C;

    using iterator = C*;
    using const_iterator = CC*;

    // SFINAE (undefed at the end)
    #define _C4_REQUIRE_CSPAN()  class T=C, class _require = typename std::enable_if< std::is_same<T , CC>::value >::type
    #define _C4_REQUIRE_NCSPAN() class T=C, class _require = typename std::enable_if< std::is_same<T, NCC>::value >::type
    // just to be sure
    #define _C4REQNULLWHENCONST(ty, buf, sz) C4_ASSERT(buf[sz-1] == '\0' || ! std::is_const<ty>::value)

public:

    /// convert automatically to span of const C
    operator basic_cspan () const { basic_cspan s(str, len); return s; }

public:

    basic_span() : str(nullptr), len(0) {}

    basic_span(basic_span const&) = default;
    basic_span(basic_span     &&) = default;

    basic_span& operator= (basic_span const&) = default;
    basic_span& operator= (basic_span     &&) = default;

    //basic_span& operator= (C *s_) { this->assign(s_); return *this; }
    template< size_t N >
    basic_span& operator= (C (&s_)[N]) { this->assign(s_); return *this; }

public:

    //basic_span(C *s_) : str(s_), len(s_ ? strlen(s_) : 0) {}
    /** the overload for receiving a single C* pointer will always
     * hide the array[N] overload. So it is disabled. If you want to
     * construct a span from a single C* pointer, you can call
     * c4::yml::to_span()/c4::yml::to_cspan().
     * @see c4::yml::to_span()
     * @see c4::yml::to_cspan() */
    template< size_t N >
    basic_span(C (&s_)[N]) : str(s_), len(N-1) { _C4REQNULLWHENCONST(C, s_, N); }
    basic_span(C *s_, size_t len_) : str(s_), len(len_) { C4_ASSERT(str || !len_); }
    basic_span(C *beg_, C *end_) : str(beg_), len(end_ - beg_) { C4_ASSERT(end_ >= beg_); }

    //void assign(C *s_) { str = (s_); len = (s_ ? strlen(s_) : 0); }
    /** the overload for receiving a single C* pointer will always
     * hide the array[N] overload. So it is disabled. If you want to
     * construct a span from a single C* pointer, you can call
     * c4::yml::to_span()/c4::yml::to_cspan().
     * @see c4::yml::to_span()
     * @see c4::yml::to_cspan() */
    template< size_t N >
    void assign(C (&s_)[N]) { _C4REQNULLWHENCONST(C, s_, N); str = (s_); len = (N-1); }
    void assign(C *s_, size_t len_) { str = s_; len = len_; C4_ASSERT(str || !len_); }
    void assign(C *beg_, C *end_) { C4_ASSERT(end_ >= beg_); str = (beg_); len = (end_ - beg_); }


public:

    // allow assignments from non-const chars when C is const char

    //template< _C4_REQUIRE_CSPAN() > basic_span(basic_ncspan const& s) : str(s.str), len(s.len) { }
    //template< _C4_REQUIRE_CSPAN() > basic_span& operator= (basic_ncspan const& s) { str = s.str; len = s.len; return *this; }

    //template<           _C4_REQUIRE_CSPAN() > explicit basic_span(NCC *s_) : str(s_), len(s_ ? strlen(s_) : 0) {}
    /** the overload for receiving a single C* pointer will always
     * hide the array[N] overload. So it is disabled. If you want to
     * construct a span from a single C* pointer, you can call
     * c4::yml::to_span()/c4::yml::to_cspan().
     * @see c4::yml::to_span()
     * @see c4::yml::to_cspan() */
    template< size_t N, _C4_REQUIRE_CSPAN() > explicit basic_span(NCC (&s_)[N]) : str(s_), len(N-1) {}
    template<           _C4_REQUIRE_CSPAN() > explicit basic_span(NCC *s_, size_t len_) : str(s_), len(len_) { C4_ASSERT(str || !len_); }
    template<           _C4_REQUIRE_CSPAN() > explicit basic_span(NCC *beg_, NCC *end_) : str(beg_), len(end_ - beg_) { C4_ASSERT(end_ >= beg_); }

public:

    void clear() { str = nullptr; len = 0; }

    bool has_str() const { return ! empty() && str[0] != C(0); }
    bool empty() const { return (len == 0 || str == nullptr); }
    size_t size() const { return len; }

    iterator begin() { return str; }
    iterator end  () { return str + len; }

    const_iterator begin() const { return str; }
    const_iterator end  () const { return str + len; }

    C      * data()       { return str; }
    C const* data() const { return str; }

    inline C      & operator[] (size_t i)       { C4_ASSERT(i >= 0 && i < len); return str[i]; }
    inline C const& operator[] (size_t i) const { C4_ASSERT(i >= 0 && i < len); return str[i]; }

public:

    bool operator== (C const* s) const
    {
        C4_ASSERT(s != nullptr);
        return (strncmp(str, s, len) == 0);
    }
    bool operator== (C const c) const
    {
        return len == 1 && *str == c;
    }
    template< class T >
    bool operator!= (T const& that) const
    {
        return ! (operator== (that));
    }

    bool operator== (basic_cspan const& that) const { return this->compare(that) == 0; }
    bool operator<  (basic_cspan const& that) const { return this->compare(that) <  0; }
    bool operator>  (basic_cspan const& that) const { return this->compare(that) >  0; }
    bool operator<= (basic_cspan const& that) const { return this->compare(that) <= 0; }
    bool operator>= (basic_cspan const& that) const { return this->compare(that) >= 0; }

    int compare(basic_cspan const& that) const
    {
        size_t n = len < that.len ? len : that.len;
        int ret = strncmp(str, that.str, n);
        if(ret == 0 && len != that.len)
        {
            ret = len < that.len ? -1 : 1;
        }
        return ret;
    }

public:

    /** return [first,first+num[ */
    basic_span subspan(size_t first, size_t num = npos) const
    {
        size_t rnum = num != npos ? num : len - first;
        C4_ASSERT((first >= 0 && first + rnum <= len) || num == 0);
        return basic_span(str + first, rnum);
    }

    /** return [first,last[ */
    basic_span range(size_t first, size_t last=npos) const
    {
        last = last != npos ? last : len;
        C4_ASSERT(first >= 0 && last <= len);
        return basic_span(str + first, last - first);
    }

    /** true if *this is a subspan of that */
    inline bool is_contained(basic_cspan const& super) const
    {
        return begin() >= super.begin() && end() <= super.end();
    }

    /** true if that is a subspan of this */
    inline bool contains(basic_cspan const& sub) const
    {
        return sub.begin() >= begin() && sub.end() <= end();
    }

public:

    basic_span right_of(size_t pos, bool include_pos = false) const
    {
        if(pos == npos) return subspan(0, 0);
        if( ! include_pos) ++pos;
        return subspan(pos);
    }

    basic_span left_of(size_t pos, bool include_pos = false) const
    {
        if(pos == npos) return *this;
        if( ! include_pos && pos > 0) --pos;
        return subspan(0, pos+1/* bump because this arg is a size, not a pos*/);
    }

public:

    basic_span left_of(basic_cspan const& ss) const
    {
        auto ssb = ss.begin();
        auto b = begin();
        auto e = end();
        if(ssb >= b && ssb <= e)
        {
            return subspan(0, ssb - b);
        }
        else
        {
            return subspan(0, 0);
        }
    }

    basic_span right_of(basic_cspan const& ss) const
    {
        auto sse = ss.end();
        auto b = begin();
        auto e = end();
        if(sse >= b && sse <= e)
        {
            return subspan(sse - b, e - sse);
        }
        else
        {
            return subspan(0, 0);
        }
    }

public:

    /** trim left */
    basic_span triml(const C c) const
    {
        return right_of(first_not_of(c), /*include_pos*/true);
    }
    /** trim left ANY of the characters */
    basic_span triml(basic_cspan const& chars) const
    {
        return right_of(first_not_of(chars), /*include_pos*/true);
    }

    /** trim right */
    basic_span trimr(const C c) const
    {
        return left_of(last_not_of(c), /*include_pos*/true);
    }
    /** trim right ANY of the characters */
    basic_span trimr(basic_cspan const& chars) const
    {
        return left_of(last_not_of(chars), /*include_pos*/true);
    }

    /** trim left and right */
    basic_span trim(const C c) const
    {
        return triml(c).trimr(c);
    }
    /** trim left and right ANY of the characters */
    basic_span trim(basic_cspan const& chars) const
    {
        return triml(chars).trimr(chars);
    }

public:

    inline size_t find(const C c) const
    {
        return first_of(c);
    }
    inline size_t find(basic_cspan const& chars) const
    {
        if(len < chars.len) return npos;
        for(size_t i = 0, e = len - chars.len + 1; i < e; ++i)
        {
            bool gotit = true;
            for(size_t j = 0; j < chars.len; ++j)
            {
                C4_ASSERT(i + j < len);
                if(str[i + j] != chars.str[j])
                {
                    gotit = false;
                    break;
                }
            }
            if(gotit)
            {
                return i;
            }
        }
        return npos;
    }

public:

    struct first_of_any_result
    {
        size_t which;
        size_t pos;
        inline operator bool() const { return which != NONE && pos != npos; }
    };

    first_of_any_result first_of_any(basic_cspan const& s0, basic_cspan const& s1) const
    {
        basic_cspan spans[2] = {s0, s1};
        return first_of_any(&spans[0], &spans[0] + 2);
    }

    first_of_any_result first_of_any(basic_cspan const& s0, basic_cspan const& s1, basic_cspan const& s2) const
    {
        basic_cspan spans[3] = {s0, s1, s2};
        return first_of_any(&spans[0], &spans[0] + 3);
    }

    first_of_any_result first_of_any(basic_cspan const& s0, basic_cspan const& s1, basic_cspan const& s2, basic_cspan const& s3) const
    {
        basic_cspan spans[4] = {s0, s1, s2, s3};
        return first_of_any(&spans[0], &spans[0] + 4);
    }

    first_of_any_result first_of_any(basic_cspan const& s0, basic_cspan const& s1, basic_cspan const& s2, basic_cspan const& s3, basic_cspan const& s4) const
    {
        basic_cspan spans[4] = {s0, s1, s2, s3, s4};
        return first_of_any(&spans[0], &spans[0] + 5);
    }

    template< class It >
    first_of_any_result first_of_any(It first_span, It last_span) const
    {
        for(size_t i = 0; i < len; ++i)
        {
            size_t curr = 0;
            for(It it = first_span; it != last_span; ++curr, ++it)
            {
                auto const& chars = *it;
                bool gotit = true;
                for(size_t j = 0; (j < chars.len) && (i+j < len); ++j)
                {
                    if(str[i + j] != chars[j])
                    {
                        gotit = false;
                        break;
                    }
                }
                if(gotit)
                {
                    return {curr, i};
                }
            }
        }
        return {NONE, npos};
    }

public:

    inline bool begins_with(const C c) const
    {
        return len > 0 ? str[0] == c : false;
    }
    inline bool begins_with(const C c, size_t num) const
    {
        if(len < num) return false;
        for(size_t i = 0; i < num; ++i)
        {
            if(str[i] != c) return false;
        }
        return true;
    }
    inline bool begins_with(basic_cspan const& pattern) const
    {
        if(len < pattern.len) return false;
        for(size_t i = 0; i < pattern.len; ++i)
        {
            if(str[i] != pattern[i]) return false;
        }
        return true;
    }
    inline bool begins_with_any(basic_cspan const& pattern) const
    {
        return first_of(pattern) == 0;
    }

    inline bool ends_with(const C c) const
    {
        return len > 0 ? str[len-1] == c : false;
    }
    inline bool ends_with(const C c, size_t num) const
    {
        if(len < num) return false;
        for(size_t i = len - num; i < len; ++i)
        {
            if(str[i] != c) return false;
        }
        return true;
    }
    inline bool ends_with(basic_cspan const& pattern) const
    {
        if(len < pattern.len) return false;
        for(size_t i = 0, s = len-pattern.len; i < pattern.len; ++i)
        {
            if(str[s+i] != pattern[i]) return false;
        }
        return true;
    }
    inline bool ends_with_any(basic_cspan const& chars) const
    {
        if(len == 0) return false;
        return last_of(chars) == len - 1;
    }

public:

    inline size_t first_of(const C c) const
    {
        for(size_t i = 0; i < len; ++i)
        {
            if(str[i] == c) return i;
        }
        return npos;
    }
    inline size_t last_of(const C c) const
    {
        for(size_t i = len-1; i != size_t(-1); --i)
        {
            if(str[i] == c) return i;
        }
        return npos;
    }

    inline size_t first_of(basic_cspan const& chars) const
    {
        for(size_t i = 0; i < len; ++i)
        {
            for(size_t j = 0; j < chars.len; ++j)
            {
                if(str[i] == chars[j]) return i;
            }
        }
        return npos;
    }
    inline size_t last_of(basic_cspan const& chars) const
    {
        for(size_t i = len-1; i != size_t(-1); --i)
        {
            for(size_t j = 0; j < chars.len; ++j)
            {
                if(str[i] == chars[j]) return i;
            }
        }
        return npos;
    }

public:

    inline size_t first_not_of(const C c) const
    {
        for(size_t i = 0; i < len; ++i)
        {
            if(str[i] != c) return i;
        }
        return npos;
    }
    inline size_t last_not_of(const C c) const
    {
        for(size_t i = len-1; i != size_t(-1); --i)
        {
            if(str[i] != c) return i;
        }
        return npos;
    }

    inline size_t first_not_of(basic_cspan const& chars) const
    {
        for(size_t i = 0; i < len; ++i)
        {
            bool gotit = true;
            for(size_t j = 0; j < chars.len; ++j)
            {
                if(str[i] == chars.str[j])
                {
                    gotit = false;
                    break;
                }
            }
            if(gotit)
            {
                return i;
            }
        }
        return npos;
    }

    inline size_t last_not_of(basic_cspan const& chars) const
    {
        for(size_t i = len-1; i != size_t(-1); --i)
        {
            bool gotit = true;
            for(size_t j = 0; j < chars.len; ++j)
            {
                if(str[i] == chars.str[j])
                {
                    gotit = false;
                    break;
                }
            }
            if(gotit)
            {
                return i;
            }
        }
        return npos;
    }

public:

    // this is SFINAE'd out for spans of const chars

    template< _C4_REQUIRE_NCSPAN() >
    void reverse()
    {
        if(len == 0) return;
        _do_reverse(str, str + len - 1);
    }

    template< _C4_REQUIRE_NCSPAN() >
    void reverse_subspan(size_t ifirst, size_t num)
    {
        C4_ASSERT(ifirst >= 0 && ifirst < len);
        C4_ASSERT(ifirst + num >= 0 && ifirst + num <= len);
        if(num == 0) return;
        _do_reverse(str + ifirst, ifirst + num - 1);
    }

    template< _C4_REQUIRE_NCSPAN() >
    void reverse_range(size_t ifirst, size_t ilast)
    {
        C4_ASSERT(ifirst >= 0 && ifirst <= len);
        C4_ASSERT(ilast  >= 0 && ilast  <= len);
        if(ifirst == ilast) return;
        _do_reverse(str + ifirst, str + ilast - 1);
    }

    template< _C4_REQUIRE_NCSPAN() >
    inline static void  _do_reverse(C *first, C* last)
    {
        while(last > first)
        {
            C tmp = *last;
            *last-- = *first;
            *first++ = tmp;
        }
    }

    #undef _C4_REQUIRE_CSPAN
    #undef _C4_REQUIRE_NCSPAN

public:

    /** get the first span consisting exclusively of non-empty characters */
    basic_span first_non_empty_span() const
    {
        basic_cspan empty_chars(" \n\r\t");
        size_t pos = first_not_of(empty_chars);
        if(pos == npos) return subspan(0, 0);
        auto ret = subspan(pos);
        pos = ret.first_of(empty_chars);
        return ret.subspan(0, pos);
    }

    basic_span first_uint_span() const
    {
        basic_span ne = first_non_empty_span();
        for(size_t i = 0; i < ne.len; ++i)
        {
            char c = ne.str[i];
            if(c < '0' || c > '9')
            {
                return ne.subspan(0, i);
            }
        }
        return ne;
    }

    basic_span first_int_span() const
    {
        basic_span ne = first_non_empty_span();
        for(size_t i = 0; i < ne.len; ++i)
        {
            char c = ne.str[i];
            if(c == '-' && i != 0)
            {
                return ne.subspan(0, i);
            }
            else if(c < '0' || c > '9')
            {
                return ne.subspan(0, i);
            }
        }
        return ne;
    }

    basic_span first_real_span() const
    {
        basic_span ne = first_non_empty_span();
        for(size_t i = 0; i < ne.len; ++i)
        {
            char c = ne.str[i];
            if(c == '-' || c == '+')
            {
                if(i == 0) // a leading signal is valid 
                {
                    continue;
                }
                else // we can also have a sign for the exponent
                {
                    char e = ne[i-1];
                    if(e == 'e' || e == 'E')
                    {
                        continue;
                    }
                    else
                    {
                        return ne.subspan(0, i);
                    }
                }
            }
            else if((c < '0' || c > '9') && (c != '.' && c != 'e' && c != 'E'))
            {
                return ne.subspan(0, i);
            }
        }
        return ne;
    }

}; // template class basic_span


/** Because of a C++ limitation, spans cannot provide simultaneous
 * overloads for constructing from a char[N] and a char*; the latter
 * will always be chosen by the compiler. So this specialization is
 * provided to simplify obtaining a span from a char*. Being a
 * function has the advantage of making evident the strlen() cost.
 *
 * @see For a more detailed explanation on why the overloads cannot
 * coexist, see http://cplusplus.bordoon.com/specializeForCharacterArrays.html */
inline span to_span(char *s)
{
    return span(s, s ? strlen(s) : 0);
}

/** Because of a C++ limitation, spans cannot provide simultaneous
 * overloads for constructing from a char[N] and a char*; the latter
 * will always be chosen by the compiler. So this specialization is
 * provided to simplify obtaining a span from a char*. Being a
 * function has the advantage of making evident the strlen() cost.
 *
 * @see For a more detailed explanation on why the overloads cannot
 * coexist, see http://cplusplus.bordoon.com/specializeForCharacterArrays.html */
inline cspan to_cspan(char *s)
{
    return cspan(s, s ? strlen(s) : 0);
}

/** Because of a C++ limitation, spans cannot provide simultaneous
 * overloads for constructing from a const char[N] and a const char*;
 * the latter will always be chosen by the compiler. So this
 * specialization is provided to simplify obtaining a span from a
 * char*. Being a function has the advantage of making evident the
 * strlen() cost.
 *
 * @see For a more detailed explanation on why the overloads cannot
 * coexist, see http://cplusplus.bordoon.com/specializeForCharacterArrays.html */
inline cspan to_cspan(const char *s)
{
    return cspan(s, s ? strlen(s) : 0);
}

} // namespace yml
} // namespace c4

#endif /* _C4_YML_SPAN_HPP_ */
