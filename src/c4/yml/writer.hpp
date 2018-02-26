#ifndef _C4_YML_WRITER_HPP_
#define _C4_YML_WRITER_HPP_

#include <stdio.h> // fwrite(), fputc()
#include <string.h> // memcpy()

#ifndef _C4_YML_COMMON_HPP_
#include "./common.hpp"
#endif
#ifndef _C4_YML_SPAN_HPP_
#include "./span.hpp"
#endif

namespace c4 {
namespace yml {


/** A character to be written a number of times. */
struct RepC
{
    char c;
    size_t num_times;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/** A writer that outputs to a file. Defaults to stdout. */
struct WriterFile
{
    FILE *m_file;
    size_t m_pos;

    WriterFile(FILE *f = nullptr) : m_file(f ? f : stdout), m_pos(0) {}

    inline span _get(bool /*error_on_excess*/)
    {
        span sp;
        sp.str = nullptr;
        sp.len = m_pos;
        return sp;
    }

    inline void _do_write(const char c)
    {
        fwrite(&c, sizeof(char), 1, m_file);
        ++m_pos;
    }

    inline void _do_write(RepC const rc)
    {
        //fwrite(&rc.c, sizeof(char), rc.num_times, m_file); // this fails... need to investigate
        for(size_t i = 0; i < rc.num_times; ++i)
        {
            fputc(rc.c, m_file);
        }
        m_pos += rc.num_times;
    }

    inline void _do_write(cspan const& sp)
    {
        if(sp.empty()) return;
        if(m_file)
        {
            fwrite(sp.str, sp.len, 1, m_file);
        }
        m_pos += sp.len;
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/** a writer to a string span */
struct WriterSpan
{
    span   m_span;
    size_t m_pos;

    WriterSpan(span const& sp) : m_span(sp), m_pos(0) {}

    inline span _get(bool error_on_excess)
    {
        span sp = m_span;
        if(m_pos > sp.len)
        {
            sp.len = m_pos;
            sp.str = nullptr;
            if(error_on_excess)
            {
                c4::yml::error("not enough space in the given span");
            }
            return sp;
        }
        sp = sp.range(0, m_pos);
        return sp;
    }

    inline void _do_write(const char c)
    {
        if(m_pos + 1 <= m_span.len)
        {
            m_span[m_pos] = c;
        }
        ++m_pos;
    }

    inline void _do_write(RepC const rc)
    {
        if(m_pos + rc.num_times <= m_span.len)
        {
            for(size_t i = 0; i < rc.num_times; ++i)
            {
                m_span[m_pos + i] = rc.c;
            }
        }
        m_pos += rc.num_times;
    }

    inline void _do_write(cspan const& sp)
    {
        if(sp.empty()) return;
        if(m_pos + sp.len <= m_span.len)
        {
            memcpy(&(m_span[m_pos]), sp.str, sp.len);
        }
        m_pos += sp.len;
    }
};


} // namespace yml
} // namespace c4

#endif /* _C4_YML_WRITER_HPP_ */
