#ifndef _C4_YML_WRITER_FILE_HPP_
#define _C4_YML_WRITER_FILE_HPP_

/** @file writer_file.hpp */

#ifndef _C4_YML_ERROR_HPP_
#include "c4/yml/error.hpp"
#endif

#include <stdio.h>  // fwrite(), fputc()

namespace c4 {
namespace yml {

/** A writer that outputs to a C file handle, defaulting to
 * stdout. This writer is *much* faster than @ref WriterOStream and
 * should be preferred to it.
 * @ingroup doc_writers
 * @ingroup doc_emit_to_file
 */
struct RYML_EXPORT WriterFile
{
    FILE * m_file;

    WriterFile(FILE *f = nullptr) : m_file(f ? f : stdout) {}

    template<size_t N>
    void _do_write(const char (&a)[N]) noexcept
    {
        static_assert(N > 1, "empty string");
        (void)fwrite(a, sizeof(char), N - 1, m_file);
    }

    void _do_write(csubstr s) noexcept
    {
        if(s.len)
        {
            C4_SUPPRESS_WARNING_GCC_CLANG_WITH_PUSH("-Wsign-conversion")
            (void)fwrite(s.str, sizeof(csubstr::char_type), s.len, m_file);
            C4_SUPPRESS_WARNING_GCC_CLANG_POP
        }
    }

    void _do_write(const char c) noexcept
    {
        (void)fputc(c, m_file);
    }

    void _do_write(const char c, size_t num_times) noexcept
    {
        for(size_t i = 0; i < num_times; ++i)
            (void)fputc(c, m_file);
    }
};

} // namespace yml
} // namespace c4

#endif /* _C4_YML_WRITER_FILE_HPP_ */
