#ifndef _C4_YML_ESCAPE_SCALAR_HPP_
#define _C4_YML_ESCAPE_SCALAR_HPP_

#ifndef _C4_YML_COMMON_HPP_
#include "c4/yml/common.hpp"
#endif

namespace c4 {
namespace yml {


/** Iterate through a scalar and escape special characters in it. This
 * function takes a callback (which accepts a single parameter of
 * csubstr type) and, while processing, calls this callback as
 * appropriate, passing ranges of the scalar and/or escaped
 * characters.
 *
 * @param fn a sink function receiving a csubstr
 * @param scalar the scalar to be escaped
 * @param keep_newlines when true, `\n` will be escaped as `\\n\n` instead of just `\\n`
 *
 * Example usage:
 *
 * ```c++
 * // escape to stdout
 * void escape_scalar(FILE *file, csubstr scalar)
 * {
 *     auto print_ = [](csubstr repl){
 *         fwrite(repl.len, 1, repl.str, file);
 *     };
 *     escape_scalar_fn(std::ref(print_), scalar);
 * }
 *
 * // escape to a different buffer and return the required buffer size
 * size_t escape_scalar(substr buffer, csubstr scalar)
 * {
 *     C4_ASSERT(!buffer.overlaps(scalar));
 *     size_t pos = 0;
 *     auto _append = [&](csubstr repl){
 *         if(repl.len && (pos + repl.len <= buffer.len))
 *             memcpy(buffer.str + pos, repl.str, repl.len);
 *         pos += repl.len;
 *     };
 *     escape_scalar_fn(std::ref(_append), scalar);
 *     return pos;
 * }
 * ```
 */
template<class Fn>
void escape_scalar_fn(Fn &&fn, csubstr scalar, bool keep_newlines=false)
{
    size_t prev = 0;   // the last position that was flushed
    size_t skip = 0;   // how much to add to prev
    csubstr repl;      // replacement string
    bool newl = false; // to add a newline
    // cast to u8 to avoid having to deal with negative
    // signed chars (which are present some platforms)
    uint8_t const* C4_RESTRICT s = reinterpret_cast<uint8_t const*>(scalar.str); // NOLINT(*-reinterpret-cast)
    // NOLINTBEGIN(*-goto)
    for(size_t i = 0; i < scalar.len; ++i)
    {
        switch(s[i])
        {
        case UINT8_C(0x0a): // \n
            repl = "\\n";
            skip = 1;
            if(keep_newlines)
                newl = true;
            goto flush_now;
        case UINT8_C(0x5c): // '\\'
            repl = "\\\\";
            skip = 1;
            goto flush_now;
        case UINT8_C(0x09): // \t
            repl = "\\t";
            skip = 1;
            goto flush_now;
        case UINT8_C(0x0d): // \r
            repl = "\\r";
            skip = 1;
            goto flush_now;
        case UINT8_C(0x00): // \0
            repl = "\\0";
            skip = 1;
            goto flush_now;
        case UINT8_C(0x0c): // \f (form feed)
            repl = "\\f";
            skip = 1;
            goto flush_now;
        case UINT8_C(0x08): // \b (backspace)
            repl = "\\b";
            skip = 1;
            goto flush_now;
        case UINT8_C(0x07): // \a (bell)
            repl = "\\a";
            skip = 1;
            goto flush_now;
        case UINT8_C(0x0b): // \v (vertical tab)
            repl = "\\v";
            skip = 1;
            goto flush_now;
        case UINT8_C(0x1b): // \e (escape)
            repl = "\\e";
            skip = 1;
            goto flush_now;
        case UINT8_C(0xc2): // AKA -0x3e
            if(i+1 < scalar.len)
            {
                if(s[i+1] == UINT8_C(0xa0)) // AKA -0x60
                {
                    repl = "\\_";
                    skip = 2;
                    goto flush_now;
                }
                else if(s[i+1] == UINT8_C(0x85)) // AKA -0x7b
                {
                    repl = "\\N";
                    skip = 2;
                    goto flush_now;
                }
            }
            continue;
        case UINT8_C(0xe2): // AKA -0x1e
            if(i+2 < scalar.len)
            {
                if(s[i+1] == UINT8_C(0x80)) // AKA -0x80
                {
                    if(s[i+2] == UINT8_C(0xa8)) // AKA -0x58
                    {
                        repl = "\\L";
                        skip = 3;
                        goto flush_now;
                    }
                    else if(s[i+2] == UINT8_C(0xa9)) // AKA -0x57
                    {
                        repl = "\\P";
                        skip = 3;
                        goto flush_now;
                    }
                }
            }
            continue;
        default:
            continue;
        }
    flush_now:
        std::forward<Fn>(fn)(scalar.range(prev, i));
        std::forward<Fn>(fn)(repl);
        if(newl)
        {
            std::forward<Fn>(fn)("\n");
            newl = false;
        }
        prev = i + skip;
    }
    // flush the rest
    if(scalar.len > prev)
        std::forward<Fn>(fn)(scalar.sub(prev));
    // NOLINTEND(*-goto)
}


C4_SUPPRESS_WARNING_GCC_WITH_PUSH("-Wattributes")
/** Escape a scalar to an existing buffer, using @ref escape_scalar_fn
 *
 * @note This is a utility/debugging functions, so it is provided in this
 * (optional) header. For this reason, we inline it to obey to the
 * One-Definition Rule. But then we set the noinline attribute to
 * ensure they are not inlined in calling code. */
inline C4_NO_INLINE size_t escape_scalar(substr buffer, csubstr scalar, bool keep_newlines=false)
{
    size_t pos = 0;
    auto _append = [&pos, &buffer](csubstr repl){
        if(repl.len && (pos + repl.len <= buffer.len))
            memcpy(buffer.str + pos, repl.str, repl.len);
        pos += repl.len;
    };
    escape_scalar_fn(_append, scalar, keep_newlines);
    return pos;
}
C4_SUPPRESS_WARNING_GCC_POP


/** formatting helper to escape a scalar with @ref escape_scalar()x */
struct escaped_scalar
{
    escaped_scalar(csubstr s, bool keep_newl=false) : scalar(s), keep_newlines(keep_newl) {}
    csubstr scalar;
    bool keep_newlines;
};

/** formatting implementation to escape a scalar with @ref escape_scalar()x */
inline size_t to_chars(substr buf, escaped_scalar e)
{
    return escape_scalar(buf, e.scalar, e.keep_newlines);
}


} // namespace yml
} // namespace c4

#endif /* _C4_YML_ESCAPE_SCALAR_HPP_ */
