#include "c4/yml/preprocess.hpp"


namespace c4 {
namespace yml {


namespace {
struct _SubstrWriter
{
    substr buf;
    size_t pos;
    _SubstrWriter(substr buf_, size_t pos_=0) : buf(buf_), pos(pos_) {}
    void operator()(csubstr s)
    {
        C4_ASSERT(!s.overlaps(buf));
        if(pos + s.len <= buf.len)
        {
            memcpy(buf.str + pos, s.str, s.len);
        }
        pos += s.len;
    }
    void operator()(char c)
    {
        if(pos < buf.len)
        {
            buf.str[pos] = c;
        }
        ++pos;
    }
    csubstr curr() const { return pos <= buf.len ? buf.first(pos) : buf; }
    size_t excess() const { return pos > buf.len ? pos - buf.len : 0; }
};

bool _is_idchar(char c)
{
    return (c >= 'a' && c <= 'z')
        || (c >= 'A' && c <= 'Z')
        || (c >= '0' && c <= '9')
        || (c == '_' || c == '-' || c == '~' || c == '$');
}

typedef enum { kReadPending = 0, kKeyPending = 1, kValPending = 2 } _ppstate;
_ppstate _next(_ppstate s)
{
    int n = (int)s + 1;
    return (_ppstate)(n <= (int)kValPending ? n : 0);
}
} // empty namespace


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

size_t preproc_json(csubstr s, substr buf)
{
    _SubstrWriter _append(buf);
    //size_t prev = 0;
    //for(size_t i = 0; i < s.len; ++i)
    //{
    //    char c = s[i];
    //}
    _append(s);
    return _append.pos;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

size_t preproc_relaxed_map(csubstr s, substr buf)
{
    _SubstrWriter _append(buf);
    _ppstate state = kReadPending;
    size_t last = 0;

    if(s.begins_with('{'))
    {
        RYML_CHECK(s.ends_with('}'));
        s = s.offs(1, 1);
    }

    _append('{');

    for(size_t i = 0; i < s.len; ++i)
    {
        const char curr = s[i];
        const char next = i+1 < s.len ? s[i+1] : '\0';

        if(curr == '\'' || curr == '"')
        {
            csubstr ss = s.sub(i).pair_range_esc(curr, '\\');
            i += ss.end() - (s.str + i);
            state = _next(state);
        }
        else if(state == kReadPending && _is_idchar(curr))
        {
            state = _next(state);
        }

        switch(state)
        {
        case kKeyPending:
        {
            if(curr == ':' && next == ' ')
            {
                state = _next(state);
            }
            else if(curr == ',' && next == ' ')
            {
                _append(s.range(last, i));
                _append(": 1, ");
                last = i + 2;
            }
            break;
        }
        case kValPending:
        {
            if(curr == '[' || curr == '{' || curr == '(')
            {
                csubstr ss = s.sub(i).pair_range_nested(curr, '\\');
                i += ss.end() - (s.str + i);
                state = _next(state);
            }
            else if(curr == ',' && next == ' ')
            {
                state = _next(state);
            }
            break;
        }
        default:
            // nothing to do
            break;
        }
    }

    _append(s.sub(last));
    if(state == kKeyPending) _append(": 1");
    _append('}');

    return _append.pos;
}


} // namespace yml
} // namespace c4
