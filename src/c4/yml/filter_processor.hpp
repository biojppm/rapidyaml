#ifndef _C4_YML_FILTER_PROCESSOR_HPP_
#define _C4_YML_FILTER_PROCESSOR_HPP_

#include "c4/yml/common.hpp"

#ifdef RYML_DBG
#include "c4/charconv.hpp"
#include "c4/yml/detail/parser_dbg.hpp"
#endif

namespace c4 {
namespace yml {

/** @name filter_processors filter processors */
/** @{ */

//-----------------------------------------------------------------------------

/** Filters an input string into a different output string */
struct FilterProcessorSrcDst
{
    csubstr src;
    substr dst;
    size_t rpos; ///< read position
    size_t wpos; ///< write position
    Callbacks const* m_callbacks;

    C4_ALWAYS_INLINE FilterProcessorSrcDst(csubstr src_, substr dst_, Callbacks const* callbacks) noexcept
        : src(src_)
        , dst(dst_)
        , rpos(0)
        , wpos(0)
        , m_callbacks(callbacks)
    {
        _RYML_CB_ASSERT(*m_callbacks, !dst.overlaps(src));
    }

    C4_ALWAYS_INLINE FilterProcessorSrcDst(csubstr src_, substr dst_) noexcept
        : src(src_)
        , dst(dst_)
        , rpos(0)
        , wpos(0)
        , m_callbacks(&get_callbacks())
    {
        _RYML_CB_ASSERT(*m_callbacks, !dst.overlaps(src));
    }

    C4_ALWAYS_INLINE bool has_more_chars() const noexcept { return rpos < src.len; }

    C4_ALWAYS_INLINE csubstr rem() const noexcept { return src.sub(rpos); }
    C4_ALWAYS_INLINE csubstr sofar() const noexcept { return csubstr(dst.str, wpos <= dst.len ? wpos : dst.len); }
    C4_ALWAYS_INLINE csubstr result() const noexcept { csubstr ret; ret.str = wpos <= dst.len ? dst.str : nullptr; ret.len = wpos; return ret; }

    C4_ALWAYS_INLINE char curr() const noexcept { _RYML_CB_ASSERT(*m_callbacks, rpos < src.len); return src[rpos]; }
    C4_ALWAYS_INLINE char next() const noexcept { return rpos+1 < src.len ? src[rpos+1] : '\0'; }
    C4_ALWAYS_INLINE bool skipped_chars() const noexcept { return wpos != rpos; }

    C4_ALWAYS_INLINE void skip() noexcept { ++rpos; }
    C4_ALWAYS_INLINE void skip(size_t num) noexcept { rpos += num; }

    C4_ALWAYS_INLINE void set(char c) noexcept
    {
        if(wpos < dst.len)
            dst.str[wpos] = c;
        ++wpos;
    }
    C4_ALWAYS_INLINE void set(char c, size_t num) noexcept
    {
        _RYML_CB_ASSERT(*m_callbacks, num > 0);
        if(wpos + num <= dst.len)
            memset(dst.str + wpos, c, num);
        wpos += num;
    }

    C4_ALWAYS_INLINE void copy() noexcept
    {
        _RYML_CB_ASSERT(*m_callbacks, rpos < src.len);
        if(wpos < dst.len)
            dst.str[wpos] = src.str[rpos];
        ++wpos;
        ++rpos;
    }
    C4_ALWAYS_INLINE void copy(size_t num) noexcept
    {
        _RYML_CB_ASSERT(*m_callbacks, num);
        _RYML_CB_ASSERT(*m_callbacks, rpos+num <= src.len);
        if(wpos + num <= dst.len)
            memcpy(dst.str + wpos, src.str + rpos, num);
        wpos += num;
        rpos += num;
    }

    C4_ALWAYS_INLINE void translate_esc(char c) noexcept
    {
        if(wpos < dst.len)
            dst.str[wpos] = c;
        ++wpos;
        rpos += 2;
    }
    C4_ALWAYS_INLINE void translate_esc(const char *C4_RESTRICT s, size_t nw, size_t nr) noexcept
    {
        _RYML_CB_ASSERT(*m_callbacks, nw > 0);
        _RYML_CB_ASSERT(*m_callbacks, nr > 0);
        _RYML_CB_ASSERT(*m_callbacks, rpos+nr <= src.len);
        if(wpos+nw <= dst.len)
            memcpy(dst.str + wpos, s, nw);
        wpos += nw;
        rpos += 1 + nr;
    }
};


//-----------------------------------------------------------------------------
// filter in place

// debugging scaffold
#if 0
#define _c4dbgip(...) _c4dbgpf(__VA_ARGS__);
#else
#define _c4dbgip(...)
#endif

struct FilterProcessorInplace
{
    substr src;  ///< the subject string
    size_t wcap; ///< write capacity - the capacity of the subject string's buffer
    size_t rpos; ///< read position
    size_t wpos; ///< write position
    bool unfiltered_chars; ///< whether there are characters left unfiltered (eg because the string was too small)
    Callbacks const* m_callbacks;

    C4_ALWAYS_INLINE FilterProcessorInplace(substr src_, size_t wcap_, Callbacks const* callbacks) noexcept
        : src(src_)
        , wcap(wcap_)
        , rpos(0)
        , wpos(0)
        , unfiltered_chars(false)
        , m_callbacks(callbacks)
    {
        _RYML_CB_ASSERT(*m_callbacks, wcap >= src.len);
    }

    C4_ALWAYS_INLINE FilterProcessorInplace(substr src_, size_t wcap_) noexcept
        : src(src_)
        , wcap(wcap_)
        , rpos(0)
        , wpos(0)
        , unfiltered_chars(false)
        , m_callbacks(&get_callbacks())
    {
        _RYML_CB_ASSERT(*m_callbacks, wcap >= src.len);
    }

    C4_ALWAYS_INLINE bool has_more_chars() const noexcept { return rpos < src.len; }

    C4_ALWAYS_INLINE csubstr result() const noexcept
    {
        _c4dbgip("inplace: wpos={} wcap={} unfiltered={}", this->wpos, this->wcap, this->unfiltered_chars);
        csubstr ret;
        ret.str = (wpos <= wcap && !unfiltered_chars) ? src.str : nullptr;
        ret.len = wpos;
        return ret;
    }
    C4_ALWAYS_INLINE csubstr sofar() const noexcept { return csubstr(src.str, wpos <= wcap ? wpos : wcap); }
    C4_ALWAYS_INLINE csubstr rem() const noexcept { return src.sub(rpos); }

    C4_ALWAYS_INLINE char curr() const noexcept { _RYML_CB_ASSERT(*m_callbacks, rpos < src.len); return src[rpos]; }
    C4_ALWAYS_INLINE char next() const noexcept { return rpos+1 < src.len ? src[rpos+1] : '\0'; }

    C4_ALWAYS_INLINE void skip() noexcept { ++rpos; }
    C4_ALWAYS_INLINE void skip(size_t num) noexcept { rpos += num; }

    C4_ALWAYS_INLINE void set(char c) noexcept
    {
        if(wpos < wcap)  // respect write-capacity
        {
            if(wpos <= rpos)
                src.str[wpos] = c;
        }
        else
        {
            _c4dbgip("inplace: set unfiltered {}->1!", unfiltered_chars);
            unfiltered_chars = true;
        }
        ++wpos;
    }
    C4_ALWAYS_INLINE void set(char c, size_t num) noexcept
    {
        _RYML_CB_ASSERT(*m_callbacks, num);
        if(wpos + num <= wcap)  // respect write-capacity
        {
            if(wpos <= rpos)
                memset(src.str + wpos, c, num);
        }
        else
        {
            _c4dbgip("inplace: set unfiltered {}->1!", unfiltered_chars);
            unfiltered_chars = true;
        }
        wpos += num;
    }

    C4_ALWAYS_INLINE void copy() noexcept
    {
        _RYML_CB_ASSERT(*m_callbacks, rpos < src.len);
        if(wpos < wcap)  // respect write-capacity
        {
            if(wpos < rpos)  // write only if wpos is behind rpos
                src.str[wpos] = src.str[rpos];
        }
        else
        {
            _c4dbgip("inplace: set unfiltered {}->1 (wpos={}!=rpos={})={}  (wpos={}<wcap={})!", unfiltered_chars, wpos, rpos, wpos!=rpos, wpos, wcap, wpos<wcap);
            unfiltered_chars = true;
        }
        ++wpos;
        ++rpos;
    }
    C4_ALWAYS_INLINE void copy(size_t num) noexcept
    {
        _RYML_CB_ASSERT(*m_callbacks, num);
        _RYML_CB_ASSERT(*m_callbacks, rpos+num <= src.len);
        if(wpos + num <= wcap)  // respect write-capacity
        {
            if(wpos < rpos)  // write only if wpos is behind rpos
            {
                if(wpos + num <= rpos) // there is no overlap
                    memcpy(src.str + wpos, src.str + rpos, num);
                else                   // there is overlap
                    memmove(src.str + wpos, src.str + rpos, num);
            }
        }
        else
        {
            _c4dbgip("inplace: set unfiltered {}->1 (wpos={}<rpos={})={}  (wpos={}<wcap={})!", unfiltered_chars, wpos, rpos, wpos<rpos, wpos, wcap, wpos<wcap);
            unfiltered_chars = true;
        }
        wpos += num;
        rpos += num;
    }

    C4_ALWAYS_INLINE void translate_esc(char c) noexcept
    {
        if(wpos < wcap) // respect write-capacity
        {
            if(wpos <= rpos)
                src.str[wpos] = c;
        }
        else
        {
            _c4dbgip("inplace: set unfiltered {}->1!", unfiltered_chars);
            unfiltered_chars = true;
        }
        ++wpos;
        rpos += 2;
    }

    C4_NO_INLINE void translate_esc(const char *C4_RESTRICT s, size_t nw, size_t nr) noexcept
    {
        _RYML_CB_ASSERT(*m_callbacks, nw > 0);
        _RYML_CB_ASSERT(*m_callbacks, nr > 0);
        _RYML_CB_ASSERT(*m_callbacks, rpos+nr <= src.len);
        const size_t wpos_next = wpos + nw;
        const size_t rpos_next = rpos + 1 + nr;
        if(wpos_next <= rpos_next) // read and write do not overlap. just do a vanilla copy.
        {
            if(wpos_next <= wcap)
                memcpy(src.str + wpos, s, nw);
            wpos = wpos_next;
            rpos = rpos_next;
        }
        else // there is overlap. move the (to-be-read) string to the right.
        {
            const size_t excess = wpos_next - rpos_next;
            if(src.len + excess <= wcap) // ensure we do not go past the end.
            {
                _RYML_CB_ASSERT(*m_callbacks, rpos+nr+excess <= src.len);
                if(wpos_next <= wcap)
                {
                    memmove(src.str + wpos_next, src.str + rpos_next, src.len - rpos_next);
                    memcpy(src.str + wpos, s, nw);
                    rpos = wpos_next; // wpos, not rpos
                }
                else
                {
                    rpos = rpos_next;
                    _c4dbgip("inplace: set unfiltered {}->1!", unfiltered_chars);
                    unfiltered_chars = true;
                }
                // extend the string up to capacity
                src.len += excess;
            }
            else
            {
                _c4dbgip("inplace: set unfiltered {}->1!", unfiltered_chars);
                rpos = rpos_next;
                unfiltered_chars = true;
            }
            wpos = wpos_next;
        }
    }
};

#undef _c4dbgip


/** @} */

} // namespace yml
} // namespace c4

#endif /* _C4_YML_FILTER_PROCESSOR_HPP_ */
