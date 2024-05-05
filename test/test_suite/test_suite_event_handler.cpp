#ifndef RYML_SINGLE_HEADER
#include <c4/yml/node.hpp>
#include <c4/yml/std/string.hpp>
#include <c4/yml/parse_engine.def.hpp>
#endif
#include "./test_suite_event_handler.hpp"


namespace c4 {
namespace yml {

// instantiate the template
template class ParseEngine<EventHandlerYamlStd>;


void EventHandlerYamlStd::EventSink::append_escaped(csubstr val)
{
    #define _c4flush_use_instead(repl, skip)  \
        do {                                  \
            this->append(val.range(prev, i)); \
            this->append(repl);               \
            prev = i + skip;                  \
        }                                     \
        while(0)
    uint8_t const* C4_RESTRICT s = reinterpret_cast<uint8_t const*>(val.str);
    size_t prev = 0;
    for(size_t i = 0; i < val.len; ++i)
    {
        switch(s[i])
        {
        case UINT8_C(0x0a): // \n
            _c4flush_use_instead("\\n", 1); break;
        case UINT8_C(0x5c): // '\\'
            _c4flush_use_instead("\\\\", 1); break;
        case UINT8_C(0x09): // \t
            _c4flush_use_instead("\\t", 1); break;
        case UINT8_C(0x0d): // \r
            _c4flush_use_instead("\\r", 1); break;
        case UINT8_C(0x00): // \0
            _c4flush_use_instead("\\0", 1); break;
        case UINT8_C(0x0c): // \f (form feed)
            _c4flush_use_instead("\\f", 1); break;
        case UINT8_C(0x08): // \b (backspace)
            _c4flush_use_instead("\\b", 1); break;
        case UINT8_C(0x07): // \a (bell)
            _c4flush_use_instead("\\a", 1); break;
        case UINT8_C(0x0b): // \v (vertical tab)
            _c4flush_use_instead("\\v", 1); break;
        case UINT8_C(0x1b): // \e (escape)
            _c4flush_use_instead("\\e", 1); break;
        case UINT8_C(0xc2):
            if(i+1 < val.len)
            {
                const uint8_t np1 = s[i+1];
                if(np1 == UINT8_C(0xa0))
                    _c4flush_use_instead("\\_", 2);
                else if(np1 == UINT8_C(0x85))
                    _c4flush_use_instead("\\N", 2);
            }
            break;
        case UINT8_C(0xe2):
            if(i+2 < val.len)
            {
                if(s[i+1] == UINT8_C(0x80))
                {
                    if(s[i+2] == UINT8_C(0xa8))
                        _c4flush_use_instead("\\L", 3);
                    else if(s[i+2] == UINT8_C(0xa9))
                        _c4flush_use_instead("\\P", 3);
                }
            }
            break;
        }
    }
    // flush the rest
    this->append(val.sub(prev));
    #undef _c4flush_use_instead
}

} // namespace yml
} // namespace c4
