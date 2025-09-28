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
#include "c4/yml/extra/scalar.hpp"
#endif

#ifndef _C4_YML_EXTRA_INTS_UTILS_HPP_
#include "c4/yml/extra/ints_utils.hpp"
#endif

#ifndef _C4_BITMASK_HPP_
#include "c4/bitmask.hpp"
#endif


C4_SUPPRESS_WARNING_GCC_WITH_PUSH("-Wold-style-cast")
C4_SUPPRESS_WARNING_CLANG_WITH_PUSH("-Wold-style-cast")
// NOLINTBEGIN(hicpp-signed-bitwise)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

namespace c4 {
namespace yml {
namespace extra {

size_t events_ints_to_testsuite(csubstr parsed_yaml,
                                        csubstr arena,
                                        ievt::DataType const* evts_ints,
                                        ievt::DataType evts_ints_sz,
                                        substr evts_test_suite)
{
    auto getstr = [&](ievt::DataType i){
        bool in_arena = evts_ints[i] & ievt::AREN;
        csubstr region = !in_arena ? parsed_yaml : arena;
        return region.sub((size_t)evts_ints[i+1], (size_t)evts_ints[i+2]);
    };
    size_t sz = 0;
    auto append = [&](csubstr s){
        size_t next = sz + s.len;
        if (s.len && (next <= evts_test_suite.len && evts_test_suite.len))
            memcpy(evts_test_suite.str + sz, s.str, s.len);
        sz = next;
    };
    bool has_tag = false;
    csubstr tag;
    auto maybe_append_tag = [&]{
        if(has_tag)
        {
            #ifdef RYML_NO_COVERAGE__TO_BE_DELETED
            if(tag.begins_with('<'))
            {
                append(" ");
                append(tag);
            }
            else
            #endif
            if(tag.begins_with("!<"))
            {
                append(" ");
                append(tag.sub(1));
            }
            else if(tag.begins_with('!'))
            {
                append(" <");
                append(tag);
                append(">");
            }
            else
            {
                append(" <!");
                append(tag);
                append(">");
            }
        }
        has_tag = false;
    };
    bool has_anchor = false;
    csubstr anchor;
    auto maybe_append_anchor = [&]{
        if(has_anchor)
        {
            append(" &");
            append(anchor);
        }
        has_anchor = false;
    };
    auto append_cont = [&](csubstr evt, csubstr style){
        append(evt);
        if(style.len)
        {
            append(" ");
            append(style);
        }
        maybe_append_anchor();
        maybe_append_tag();
        append("\n");
    };
    auto append_val = [&](csubstr evt, csubstr val){
        append("=VAL");
        maybe_append_anchor();
        maybe_append_tag();
        append(" ");
        append(evt);
        substr buf = sz <= evts_test_suite.len ? evts_test_suite.sub(sz) : evts_test_suite.last(0);
        sz += escape_scalar(buf, val);
        append("\n");
    };
    for(ievt::DataType i = 0; i < evts_ints_sz; )
    {
        ievt::DataType evt = evts_ints[i];
        if(evt & ievt::SCLR)
        {
            csubstr s = getstr(i);
            if(evt & ievt::SQUO)
                append_val("'", s);
            else if(evt & ievt::DQUO)
                append_val("\"", s);
            else if(evt & ievt::LITL)
                append_val("|", s);
            else if(evt & ievt::FOLD)
                append_val(">", s);
            else //if(evt & ievt::PLAI)
                append_val(":", s);
        }
        else if(evt & ievt::BSEQ)
        {
            if(evt & ievt::FLOW)
                append_cont("+SEQ", "[]");
            else
                append_cont("+SEQ", "");
        }
        else if(evt & ievt::ESEQ)
        {
            append("-SEQ\n");
        }
        else if(evt & ievt::BMAP)
        {
            if(evt & ievt::FLOW)
                append_cont("+MAP", "{}");
            else
                append_cont("+MAP", "");
        }
        else if(evt & ievt::EMAP)
        {
            append("-MAP\n");
        }
        else if(evt & ievt::ALIA)
        {
            append("=ALI *");
            append(getstr(i));
            append("\n");
        }
        else if(evt & ievt::TAG_)
        {
            has_tag = true;
            tag = getstr(i);
        }
        else if(evt & ievt::ANCH)
        {
            has_anchor = true;
            anchor = getstr(i);
        }
        else if(evt & ievt::BDOC)
        {
            if(evt & ievt::EXPL)
                append("+DOC ---\n");
            else
                append("+DOC\n");
        }
        else if(evt & ievt::EDOC)
        {
            if(evt & ievt::EXPL)
                append("-DOC ...\n");
            else
                append("-DOC\n");
        }
        else if(evt & ievt::BSTR)
        {
             append("+STR\n");
        }
        else if(evt & ievt::ESTR)
        {
            append("-STR\n");
        }

        i += (evt & ievt::WSTR) ? 3 : 1;
    }
    return sz;
}

} // namespace extra
} // namespace yml
} // namespace c4

// NOLINTEND(hicpp-signed-bitwise)
C4_SUPPRESS_WARNING_CLANG_POP
C4_SUPPRESS_WARNING_GCC_POP
