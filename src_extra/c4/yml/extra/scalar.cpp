#ifdef RYML_SINGLE_HEADER_INTS
    #ifndef _RYML_SINGLE_HEADER_AMALGAMATED_HPP_
        #include <ryml_ints.hpp>
    #endif
#elif defined(RYML_SINGLE_HEADER)
    #ifndef _RYML_SINGLE_HEADER_AMALGAMATED_HPP_
        #include <ryml_all.hpp>
    #endif
#endif

#ifndef _C4_YML_EXTRA_SCALAR_HPP_
#include <c4/yml/extra/scalar.hpp>
#endif


namespace c4 {
namespace yml {
namespace extra {

size_t escape_scalar(substr buffer, csubstr val)
{
    size_t pos = 0;
    #define _append(repl)                                       \
        do {                                                    \
            if(repl.len && (pos + repl.len <= buffer.len))      \
                memcpy(buffer.str + pos, repl.str, repl.len);   \
            pos += repl.len;                                    \
        } while(0)
    #define _c4flush_use_instead(i, repl, skip)  \
        do {                                     \
            _append(val.range(prev, i));         \
            _append(csubstr(repl));              \
            prev = i + skip;                     \
        }                                        \
        while(0)
    uint8_t const* C4_RESTRICT s = reinterpret_cast<uint8_t const*>(val.str);
    size_t prev = 0;
    for(size_t i = 0; i < val.len; ++i)
    {
        switch(s[i])
        {
        case UINT8_C(0x0a): // \n
            _c4flush_use_instead(i, "\\n", 1); break;
        case UINT8_C(0x5c): // '\\'
            _c4flush_use_instead(i, "\\\\", 1); break;
        case UINT8_C(0x09): // \t
            _c4flush_use_instead(i, "\\t", 1); break;
        case UINT8_C(0x0d): // \r
            _c4flush_use_instead(i, "\\r", 1); break;
        case UINT8_C(0x00): // \0
            _c4flush_use_instead(i, "\\0", 1); break;
        case UINT8_C(0x0c): // \f (form feed)
            _c4flush_use_instead(i, "\\f", 1); break;
        case UINT8_C(0x08): // \b (backspace)
            _c4flush_use_instead(i, "\\b", 1); break;
        case UINT8_C(0x07): // \a (bell)
            _c4flush_use_instead(i, "\\a", 1); break;
        case UINT8_C(0x0b): // \v (vertical tab)
            _c4flush_use_instead(i, "\\v", 1); break;
        case UINT8_C(0x1b): // \e (escape)
            _c4flush_use_instead(i, "\\e", 1); break;
        case UINT8_C(0xc2):
            if(i+1 < val.len)
            {
                const uint8_t np1 = s[i+1];
                if(np1 == UINT8_C(0xa0))
                    _c4flush_use_instead(i, "\\_", 2);
                else if(np1 == UINT8_C(0x85))
                    _c4flush_use_instead(i, "\\N", 2);
            }
            break;
        case UINT8_C(0xe2):
            if(i+2 < val.len)
            {
                if(s[i+1] == UINT8_C(0x80))
                {
                    if(s[i+2] == UINT8_C(0xa8))
                        _c4flush_use_instead(i, "\\L", 3);
                    else if(s[i+2] == UINT8_C(0xa9))
                        _c4flush_use_instead(i, "\\P", 3);
                }
            }
            break;
        }
    }
    // flush the rest
    _append(val.sub(prev));
    #undef _c4flush_use_instead
    #undef _append
    return pos;
}

} // namespace extra
} // namespace yml
} // namespace c4
