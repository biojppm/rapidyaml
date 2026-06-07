#ifndef _C4_YML_WRITER_HPP_
#define _C4_YML_WRITER_HPP_

#ifndef _C4_YML_ERROR_HPP_
#include "c4/yml/error.hpp"
#endif

#include <stdio.h>  // fwrite(), fputc()
#include <string.h> // memcpy()


namespace c4 {
namespace yml {

/** @addtogroup doc_emit
 * @{
 */

/** @defgroup doc_writers Writer objects to use with an Emitter
 * @see Emitter
 * @{
 */


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/** A writer that outputs to a C file handle, defaulting to
 * stdout. This writer is *much* faster than @ref WriterOStream and
 * should be preferred to it. */
struct WriterFile
{
    FILE * m_file;
    size_t m_pos;

    WriterFile(FILE *f = nullptr) : m_file(f ? f : stdout), m_pos(0) {}

    substr _get(bool /*error_on_excess*/) const
    {
        substr sp;
        sp.str = nullptr;
        sp.len = m_pos;
        return sp;
    }

    template<size_t N>
    void _do_write(const char (&a)[N]) noexcept
    {
        static_assert(N > 1, "empty string");
        (void)fwrite(a, sizeof(char), N - 1, m_file);
        m_pos += N - 1;
    }

    void _do_write(csubstr s) noexcept
    {
        if(s.len)
        {
            C4_SUPPRESS_WARNING_GCC_CLANG_WITH_PUSH("-Wsign-conversion")
            (void)fwrite(s.str, sizeof(csubstr::char_type), s.len, m_file);
            m_pos += s.len;
            C4_SUPPRESS_WARNING_GCC_CLANG_POP
        }
    }

    void _do_write(const char c) noexcept
    {
        (void)fputc(c, m_file);
        ++m_pos;
    }

    void _do_write(const char c, size_t num_times) noexcept
    {
        for(size_t i = 0; i < num_times; ++i)
            (void)fputc(c, m_file);
        m_pos += num_times;
    }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/** A writer that outputs to an STL-like ostream.
 *
 * @warning This writer is *much* slower than @ref WriterFile, and
 * @ref WriterFile should be preferred to this. The slowness is due to
 * how std::ostream works, and not because of anything in the code of
 * this class. */
template<class OStream>
struct WriterOStream
{
    OStream* m_stream;
    size_t   m_pos;

    WriterOStream(OStream &s) : m_stream(&s), m_pos(0) {}

    substr _get(bool /*error_on_excess*/) const noexcept
    {
        substr sp;
        sp.str = nullptr;
        sp.len = m_pos;
        return sp;
    }

    template<size_t N>
    void _do_write(const char (&a)[N]) noexcept
    {
        static_assert(N > 1, "empty string");
        m_stream->write(a, N - 1);
        m_pos += N - 1;
    }

    void _do_write(csubstr s) noexcept
    {
        if(s.len)
        {
            C4_SUPPRESS_WARNING_GCC_CLANG_WITH_PUSH("-Wsign-conversion")
            m_stream->write(s.str, s.len);
            m_pos += s.len;
            C4_SUPPRESS_WARNING_GCC_CLANG_POP
        }
    }

    void _do_write(const char c) noexcept
    {
        m_stream->put(c);
        ++m_pos;
    }

    void _do_write(const char c, size_t num_times) noexcept
    {
        for(size_t i = 0; i < num_times; ++i)
            m_stream->put(c);
        m_pos += num_times;
    }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/** A writer to a memory buffer, in the form of a @ref substr */
struct WriterBuf
{
    substr m_buf;
    size_t m_pos;

    WriterBuf(substr sp) noexcept : m_buf(sp), m_pos(0) {}

    substr _get(bool error_on_excess) const
    {
        if(m_pos <= m_buf.len)
            return m_buf.first(m_pos);
        else if(error_on_excess)
            _RYML_ERR_BASIC("not enough space in the given buffer");
        substr sp;
        sp.str = nullptr;
        sp.len = m_pos;
        return sp;
    }

    template<size_t N>
    void _do_write(const char (&a)[N]) noexcept
    {
        static_assert(N > 1, "empty string");
        _RYML_ASSERT_BASIC( ! m_buf.overlaps(a));
        if(m_pos + N-1 <= m_buf.len)
            memcpy(m_buf.str + m_pos, a, N-1);
        m_pos += N-1;
    }

    void _do_write(csubstr s) noexcept
    {
        _RYML_ASSERT_BASIC( ! s.overlaps(m_buf));
        if(s.len && m_pos + s.len <= m_buf.len)
            memcpy(m_buf.str + m_pos, s.str, s.len);
        m_pos += s.len;
    }

    void _do_write(const char c) noexcept
    {
        if(m_pos + 1 <= m_buf.len)
            m_buf.str[m_pos] = c;
        ++m_pos;
    }

    void _do_write(const char c, size_t num_times) noexcept
    {
        if(m_pos + num_times <= m_buf.len)
            memset(m_buf.str + m_pos, c, num_times);
        m_pos += num_times;
    }
};

/** @ } */

/** @ } */


} // namespace yml
} // namespace c4

#endif /* _C4_YML_WRITER_HPP_ */
