#ifndef _C4_RYML_SPAN_HPP_
#define _C4_RYML_SPAN_HPP_

#include <string.h>
#include "./common.hpp"

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
#define _c4prsp(sp) ((int)(sp).len), (sp).str


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

    // convert automatically to span of const C
    operator basic_span< const C > () { basic_span< const C > s(str, len); return s; }
    operator bool () const { return has_str(); }

    using iterator = C*;
    using const_iterator = C const*;

public:

    basic_span() : str(nullptr), len(0) {}
    basic_span(C *s_) : str(s_), len(strlen(s_)) {}
    basic_span(C *s_, size_t len_) : str(s_), len(len_) { C4_ASSERT(str || !len_); }
    template< size_t N > basic_span(C const (&s_)[N]) : str(s_), len(N-1) {}

    void clear() { str = nullptr; len = 0; }

    bool has_str() const { return ! empty() && str[0] != C(0); }
    bool empty() const { return (len == 0 || str == nullptr); }
    size_t size() const { return len; }

    iterator begin() { return str; }
    iterator end  () { return str + len; }

    const_iterator begin() const { return str; }
    const_iterator end  () const { return str + len; }

    inline C      & operator[] (size_t i)       { C4_ASSERT(i >= 0 && i < len); return str[i]; }
    inline C const& operator[] (size_t i) const { C4_ASSERT(i >= 0 && i < len); return str[i]; }

    bool operator== (basic_span const& that) const
    {
        if(len != that.len) return false;
        return (str == that.str || (strncmp(str, that.str, len) == 0));
    }
    bool operator== (C const* s) const
    {
        C4_ASSERT(s != nullptr);
        return (strncmp(str, s, len) == 0);
    }
    bool operator== (C const c) const
    {
        return len == 1 && *str == c;
    }

    bool operator<  (basic_span const& that) { size_t n = len < that.len ? len : that.len; return strncmp(str, that.str, n) < 0; }
    bool operator>  (basic_span const& that) { size_t n = len < that.len ? len : that.len; return strncmp(str, that.str, n) > 0; }
    bool operator<= (basic_span const& that) { size_t n = len < that.len ? len : that.len; return strncmp(str, that.str, n) <= 0; }
    bool operator>= (basic_span const& that) { size_t n = len < that.len ? len : that.len; return strncmp(str, that.str, n) >= 0; }

    basic_span subspan(size_t first, size_t num = npos) const
    {
        size_t rnum = num != npos ? num : len - first;
        C4_ASSERT((first >= 0 && first + rnum <= len) || num == 0);
        return basic_span(str + first, rnum);
    }

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

    /** trim left */
    basic_span triml(const C c) const
    {
        return right_of(first_not_of(c), /*include_pos*/true);
    }
    /** trim left ANY of the characters */
    basic_span triml(basic_span< const C > const& chars) const
    {
        return right_of(first_not_of(chars), /*include_pos*/true);
    }

    /** trim right */
    basic_span trimr(const C c) const
    {
        return left_of(last_not_of(c), /*include_pos*/true);
    }
    /** trim right ANY of the characters */
    basic_span trimr(basic_span< const C > const& chars) const
    {
        return left_of(last_not_of(chars), /*include_pos*/true);
    }

    /** trim left and right */
    basic_span trim(const C c) const
    {
        return triml(c).trimr(c);
    }
    /** trim left and right ANY of the characters */
    basic_span trim(basic_span< const C > const& chars) const
    {
        return triml(chars).trimr(chars);
    }

    inline size_t find(const C c) const
    {
        return first_of(c);
    }
    inline size_t find(basic_span< const C > const& chars) const
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
                }
            }
            if(gotit)
            {
                return i;
            }
        }
        return npos;
    }

    inline bool begins_with(const C c) const
    {
        return len > 0 ? str[0] == c : false;
    }
    inline bool begins_with(basic_span< const C > const& pattern) const
    {
        if(len < pattern.len) return false;
        for(size_t i = 0; i < pattern.len; ++i)
        {
            if(str[i] != pattern[i]) return false;
        }
        return true;
    }

    inline bool ends_with(const C c) const
    {
        return len > 0 ? str[len-1] == c : false;
    }
    inline bool ends_with(basic_span< const C > const& pattern) const
    {
        if(len < pattern.len) return false;
        for(size_t i = len - pattern.len; i < len; ++i)
        {
            if(str[i] != pattern[i]) return false;
        }
        return true;
    }

    inline size_t first_of(const C c) const
    {
        for(size_t i = 0; i < len; ++i)
        {
            if(str[i] == c) return i;
        }
        return npos;
    }
    inline size_t first_of(basic_span< const C > const& chars) const
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

    inline size_t first_not_of(const C c) const
    {
        for(size_t i = 0; i < len; ++i)
        {
            if(str[i] != c) return i;
        }
        return npos;
    }
    inline size_t first_not_of(basic_span< const C > const& chars) const
    {
        for(size_t i = 0; i < len; ++i)
        {
            for(size_t j = 0; j < chars.len; ++j)
            {
                if(str[i] != chars[j]) return i;
            }
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
    inline size_t last_of(basic_span< const C > const& chars) const
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

    inline size_t last_not_of(const C c) const
    {
        for(size_t i = len-1; i != size_t(-1); --i)
        {
            if(str[i] != c) return i;
        }
        return npos;
    }
    inline size_t last_not_of(basic_span< const C > const& chars) const
    {
        for(size_t i = len-1; i != size_t(-1); --i)
        {
            bool gotit = false;
            for(size_t j = 0; j < chars.len; ++j)
            {
                if(str[i] == chars[j])
                {
                    gotit = true;
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

}; // template class basic_span

} // namespace yml
} // namespace c4

#endif /* _C4_RYML_SPAN_HPP_ */
