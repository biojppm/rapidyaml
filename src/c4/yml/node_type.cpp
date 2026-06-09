#ifndef _C4_YML_NODE_TYPE_HPP_
#include "c4/yml/node_type.hpp"
#endif
#ifndef _C4_YML_ERROR_HPP_
#include "c4/yml/error.hpp"
#endif


namespace c4 {
namespace yml {

const char* NodeType::type_str(NodeType_e ty) noexcept
{
    switch(ty & _TYMASK)
    {
    case KEYVAL:
        return "KEYVAL";
    case KEY:
        return "KEY";
    case VAL:
        return "VAL";
    case MAP:
        return "MAP";
    case SEQ:
        return "SEQ";
    case KEYMAP:
        return "KEYMAP";
    case KEYSEQ:
        return "KEYSEQ";
    case DOCSEQ:
        return "DOCSEQ";
    case DOCMAP:
        return "DOCMAP";
    case DOCVAL:
        return "DOCVAL";
    case DOC:
        return "DOC";
    case STREAM:
        return "STREAM";
    case NOTYPE:
        return "NOTYPE";
    default:
        if((ty & KEYVAL) == KEYVAL)
            return "KEYVAL***";
        if((ty & KEYMAP) == KEYMAP)
            return "KEYMAP***";
        if((ty & KEYSEQ) == KEYSEQ)
            return "KEYSEQ***";
        if((ty & DOCSEQ) == DOCSEQ)
            return "DOCSEQ***";
        if((ty & DOCMAP) == DOCMAP)
            return "DOCMAP***";
        if((ty & DOCVAL) == DOCVAL)
            return "DOCVAL***";
        if(ty & KEY)
            return "KEY***";
        if(ty & VAL)
            return "VAL***";
        if(ty & MAP)
            return "MAP***";
        if(ty & SEQ)
            return "SEQ***";
        if(ty & DOC)
            return "DOC***";
        return "(unk)";
    }
}

size_t NodeType::type_str(substr buf, NodeType_e flags) noexcept
{
    size_t pos = 0;
    bool gotone = false;

    #define _prflag(fl, txt)                                    \
    do {                                                        \
        if((flags & (fl)) == (fl))                              \
        {                                                       \
            if(gotone)                                          \
            {                                                   \
                if(pos + 1 < buf.len)                           \
                    buf[pos] = '|';                             \
                ++pos;                                          \
            }                                                   \
            csubstr fltxt = txt;                                \
            if(pos + fltxt.len <= buf.len)                      \
                memcpy(buf.str + pos, fltxt.str, fltxt.len);    \
            pos += fltxt.len;                                   \
            gotone = true;                                      \
            flags = (flags & ~(fl)); /*remove the flag*/        \
        }                                                       \
    } while(0)

    _prflag(STREAM, "STREAM");
    _prflag(DOC, "DOC");
    // key properties
    _prflag(KEY, "KEY");
    _prflag(KEYNIL, "KNIL");
    _prflag(KEYTAG, "KTAG");
    _prflag(KEYANCH, "KANCH");
    _prflag(KEYREF, "KREF");
    _prflag(KEY_LITERAL, "KLITERAL");
    _prflag(KEY_FOLDED, "KFOLDED");
    _prflag(KEY_SQUO, "KSQUO");
    _prflag(KEY_DQUO, "KDQUO");
    _prflag(KEY_PLAIN, "KPLAIN");
    _prflag(KEY_UNFILT, "KUNFILT");
    // val properties
    _prflag(VAL, "VAL");
    _prflag(VALNIL, "VNIL");
    _prflag(VALTAG, "VTAG");
    _prflag(VALANCH, "VANCH");
    _prflag(VALREF, "VREF");
    _prflag(VAL_UNFILT, "VUNFILT");
    _prflag(VAL_LITERAL, "VLITERAL");
    _prflag(VAL_FOLDED, "VFOLDED");
    _prflag(VAL_SQUO, "VSQUO");
    _prflag(VAL_DQUO, "VDQUO");
    _prflag(VAL_PLAIN, "VPLAIN");
    _prflag(VAL_UNFILT, "VUNFILT");
    // container properties
    _prflag(MAP, "MAP");
    _prflag(SEQ, "SEQ");
    _prflag(FLOW_SL, "FLOWSL");
    _prflag(FLOW_ML1, "FLOWML1");
    _prflag(FLOW_MLN, "FLOWMLN");
    _prflag(FLOW_SPC, "FLOWSPC");
    _prflag(BLOCK, "BLCK");
    if(pos == 0)
        _prflag(NOTYPE, "NOTYPE");

    #undef _prflag

    return pos;
}


//-----------------------------------------------------------------------------

// see https://www.yaml.info/learn/quote.html#noplain
bool scalar_style_query_squo(csubstr s) noexcept
{
    // cannot have leading whitespace after a newline
    for(size_t i = 0; i < s.len; ++i)
    {
        if(s.str[i] == '\n' && i + 1 < s.len)
        {
            char next = s.str[i + 1];
            if(next == ' ' || next == '\t')
                return false;
        }
    }
    return true;
}

namespace {
bool _is_wsnl(char c) noexcept
{
    return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}
bool _is_valid_bulk(csubstr s, size_t i)
{
    C4_ASSERT(i >= 1 && i+1 < s.len);
    C4_ASSERT(s.str[i] == ':' || s.str[i] == '#');
    switch(s.str[i])
    {
    case ':': return !_is_wsnl(s.str[i+1]);
    case '#': return !_is_wsnl(s.str[i-1]);
    }
    C4_UNREACHABLE(); // LCOV_EXCL_LINE
}
} // namespace
// see https://www.yaml.info/learn/quote.html#noplain
bool scalar_style_query_plain_flow(csubstr s) noexcept
{
    if(!s.len)
        return !s.str;
    // first
    switch(s.str[0])
    {
    case ' ': case '\n': case '\t': case '\r':
    case '!': case '&': case '*': case ',':
    case '"': case '\'': case '|': case '>':
    case '{': case '}': case '[': case ']':
    case '#': case '`': case '%': case '@':
        return false;
    case '-': case ':': case '?':
        if(s.len == 1 || (s.str[1] == ' ' || s.str[1] == '\t'))
            return false;
        break;
    }
    // bulk
    for(size_t i = 1; i + 1 < s.len; ++i)
    {
        switch(s.str[i])
        {
        case ',': case '{': case '}': case '[': case ']':
            return false;
        case ':': case '#':
            if(!_is_valid_bulk(s, i))
                return false;
            break;
        }
    }
    // last
    if(s.len > 1)
    {
        switch(s.back())
        {
        case ' ': case '\n': case '\t': case '\r':
        case ',':
        case '{': case '}':
        case '[': case ']':
        case '#':
        case ':':
            return false;
        }
    }
    return true;
}

bool scalar_style_query_plain_block(csubstr s) noexcept
{
    if(!s.len)
        return !s.str;
    // first
    switch(s.str[0])
    {
    case ' ': case '\n': case '\t': case '\r':
    case '!': case '&': case '*': case ',':
    case '"': case '\'': case '|': case '>':
    case '{': case '}': case '[': case ']':
    case '#': case '`': case '%': case '@':
        return false;
    case '-': case ':': case '?':
        if (s.len == 1 || (s.str[1] == ' ' || s.str[1] == '\t'))
            return false;
        break;
    }
    // bulk
    for(size_t i = 1; i + 1 < s.len; ++i)
    {
        switch(s.str[i])
        {
        case ':': case '#':
            if(!_is_valid_bulk(s, i))
                return false;
            break;
        }
    }
    // last
    if(s.len > 1)
    {
        switch(s.back())
        {
        case ' ': case '\n': case '\t': case '\r':
        case '#':
        case ':':
            return false;
        }
    }
    return true;
}

NodeType_e scalar_style_choose_flow(csubstr s) noexcept
{
    if(s.len)
    {
        if(scalar_style_query_plain_flow(s))
            return SCALAR_PLAIN;
        else if(scalar_style_query_squo(s))
            return SCALAR_SQUO;
        return SCALAR_DQUO;
    }
    return s.str ? SCALAR_SQUO : SCALAR_PLAIN;
}

NodeType_e scalar_style_choose_block(csubstr s) noexcept
{
    if(s.len)
    {
        if(scalar_style_query_plain_block(s))
            return SCALAR_PLAIN;
        _RYML_ASSERT_BASIC(scalar_style_query_squo(s)
                           && "if this assertion fires, please submit an issue!");
        return SCALAR_SQUO;
    }
    return s.str ? SCALAR_SQUO : SCALAR_PLAIN;
}


bool scalar_is_null(csubstr s) noexcept
{
    return s.str == nullptr ||
        (s.len == 1 && (s.str[0] == '~')) ||
        (s.len == 4 && ((0 == memcmp("null", s.str, 4))
                        || (0 == memcmp("Null", s.str, 4))
                        || (0 == memcmp("NULL", s.str, 4))));
}


//-----------------------------------------------------------------------------

#define ryml_rest_is_(c1, c2) ((s.str[1] == (c1)) && (s.str[2] == (c2)))
bool scalar_is_inf3(csubstr s) noexcept
{
    _RYML_ASSERT_BASIC(s.len == 3);
    switch(s.str[0])
    {
    case 'i': return ryml_rest_is_('n', 'f');
    case 'I': return ryml_rest_is_('n', 'f') || ryml_rest_is_('N', 'F');
    }
    return false;
}
bool scalar_is_nan3(csubstr s) noexcept
{
    _RYML_ASSERT_BASIC(s.len == 3);
    switch(s.str[0])
    {
    case 'n': return ryml_rest_is_('a', 'n');
    case 'N': return ryml_rest_is_('a', 'N') || ryml_rest_is_('a', 'n') || ryml_rest_is_('A', 'N');
    }
    return false;
}
bool scalar_is_inf_or_nan3(csubstr s) noexcept
{
    _RYML_ASSERT_BASIC(s.len == 3);
    switch(s.str[0])
    {
    case 'i': return ryml_rest_is_('n', 'f');
    case 'I': return ryml_rest_is_('n', 'f') || ryml_rest_is_('N', 'F');
    case 'n': return ryml_rest_is_('a', 'n');
    case 'N': return ryml_rest_is_('a', 'N') || ryml_rest_is_('a', 'n') || ryml_rest_is_('A', 'N');
    }
    return false;
}
#undef ryml_rest_is_

bool scalar_is_plain_number_json(csubstr s) noexcept
{
    return s.is_number()
        &&
        (
            // quote integral numbers if they have a leading 0
            // https://github.com/biojppm/rapidyaml/issues/291
            (!(s.len > 1 && s.begins_with('0')))
            // do not quote reals with leading 0
            // https://github.com/biojppm/rapidyaml/issues/313
            || (s.find('.') != csubstr::npos)
        );
}

bool scalar_is_special_json(csubstr s) noexcept
{
    if(s.len == 4)
        return 0 == memcmp("true", s.str, 4)
            || 0 == memcmp("null", s.str, 4)
            || ((s[0] == '.') && scalar_is_inf_or_nan3(s.sub(1)))
            || ((s[0] == '-' || s[0] == '+') && scalar_is_inf3(s.sub(1)));
    else if(s.len == 5)
        return 0 == memcmp("false", s.str, 5)
            || ((s[0] == '-' || s[0] == '+') && s[1] == '.' && scalar_is_inf3(s.sub(2)));
    else if(s.len == 3)
        return scalar_is_inf_or_nan3(s);
    return false;
}

NodeType_e scalar_style_choose_json(csubstr s) noexcept
{
    // do not quote numbers or special scalars
    return scalar_is_plain_number_json(s) || scalar_is_special_json(s) ? SCALAR_PLAIN : SCALAR_DQUO;
}

} // namespace yml
} // namespace c4
