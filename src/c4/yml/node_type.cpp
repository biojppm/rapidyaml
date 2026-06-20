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

} // namespace yml
} // namespace c4
