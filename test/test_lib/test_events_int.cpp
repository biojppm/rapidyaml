#include "test_lib/test_case.hpp"
#include "./test_events_int.hpp"

namespace c4 {
namespace yml {
namespace extra {


void test_events_ints(IntEventWithScalar const* expected, size_t expected_sz,
                      ievt::DataType const* actual, size_t actual_sz,
                      csubstr yaml,
                      csubstr parsed_source,
                      const char *file, int line)
{
    RYML_TRACE_FMT("defined in:\n{}:{}:\n", file, line);
    int status = true;
    size_t num_ints_expected = num_ints(expected, expected_sz);

    EXPECT_EQ(actual_sz, num_ints_expected);
    status = (actual_sz == num_ints_expected);

    char actualbuf[100];(void)actualbuf;
    char expectedbuf[100];(void)expectedbuf;
    for(size_t ia = 0, ie = 0; ie < expected_sz; ++ie)
    {
        EXPECT_LT(ia, actual_sz);
        if (ia >= actual_sz)
            break;
#define _test_eq(lhs, rhs, fmt, ...)                            \
    do                                                          \
    {                                                           \
        _c4dbgpf("status={} cmp={} ie={} ia={}: {}={} == {}={} " fmt, \
            status, (lhs == rhs), ie, ia, #lhs, lhs, rhs, #rhs, __VA_ARGS__); \
        status &= int(lhs == rhs);                              \
        EXPECT_EQ(lhs, rhs);                                    \
    } while(0)
        csubstr sactual = mkstring(actual[ia], actualbuf);
        csubstr sexpect = mkstring(expected[ie].flags, expectedbuf);
        _test_eq(actual[ia], expected[ie].flags, "", 0);
        _test_eq(sactual, sexpect, "", 0);
        if((expected[ie].flags & ievt::WSTR) && (actual[ia] & ievt::WSTR))
        {
            _test_eq(expected[ie].str_start, actual[ia + 1], "", 0);
            _test_eq(expected[ie].str_len, actual[ia + 2], "", 0);
            bool safeactual = (ia + 2 < actual_sz) && (actual[ia + 1] < (int)parsed_source.len && actual[ia + 1] + actual[ia + 2] <= (int)parsed_source.len);
            bool safeexpected = (expected[ie].str_start < (int)parsed_source.len && expected[ie].str_start + expected[ie].str_len <= (int)parsed_source.len);
            _test_eq(safeactual, true, "", 0);
            _test_eq(safeactual, safeexpected, "", 0);
            if(safeactual && safeexpected)
            {
                csubstr evtstr = parsed_source.sub((size_t)expected[ie].str_start, (size_t)expected[ie].str_len);
                csubstr actualstr = parsed_source.sub((size_t)actual[ia + 1], (size_t)actual[ia + 2]);
                _test_eq(expected[ie].scalar, actualstr,
                         "   ref=[{}]~~~{}~~~ vs act=[{}]~~~{}~~~",
                         expected[ie].scalar.len, expected[ie].scalar,
                         actualstr.len, actualstr);
                if( ! expected[ie].needs_filter)
                {
                    _test_eq(evtstr, actualstr,
                             "   exp=[{}]~~~{}~~~ vs act=[{}]~~~{}~~~",
                             evtstr.len, evtstr,
                             actualstr.len, actualstr);
                }
            }
        }
        ia += (actual[ia] & ievt::WSTR) ? 3u : 1u;
    }
    RYML_TRACE_FMT("input:[{}]~~~{}~~~\n"
                   "parsed:[{}]~~~{}~~~\n",
                   yaml.len, yaml,
                   parsed_source.len, parsed_source);
    EXPECT_TRUE(status);
}


void test_events_ints_invariants(csubstr parsed_yaml,
                                 ievt::DataType const* evts_ints,
                                 ievt::DataType evts_ints_sz)
{
    EXPECT_GT(evts_ints_sz, 0);
    for(ievt::DataType i = 0; i < evts_ints_sz; i += ((evts_ints[i] & ievt::WSTR) ? 3 : 1))
    {
        ievt::DataType evt = evts_ints[i];
        if (evt & ievt::BSTR)
        {
            EXPECT_EQ(evt & ievt::ESTR, 0);
            EXPECT_EQ(evt & ievt::EXPL, 0);
            EXPECT_EQ(evt & ievt::WSTR, 0);
            EXPECT_EQ(evt & ievt::SCLR, 0);
            EXPECT_EQ(evt & ievt::ALIA, 0);
            EXPECT_EQ(evt & ievt::ANCH, 0);
            EXPECT_EQ(evt & ievt::TAG_, 0);
            EXPECT_EQ(evt & ievt::KEY_, 0);
            EXPECT_EQ(evt & ievt::VAL_, 0);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), 0);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), 0);
            EXPECT_EQ(evt & (ievt::BSEQ|ievt::ESEQ), 0);
            EXPECT_EQ(evt & (ievt::FLOW|ievt::BLCK), 0);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0);
        }
        if (evt & ievt::ESTR)
        {
            EXPECT_EQ(evt & ievt::BSTR, 0);
            EXPECT_EQ(evt & ievt::EXPL, 0);
            EXPECT_EQ(evt & ievt::WSTR, 0);
            EXPECT_EQ(evt & ievt::SCLR, 0);
            EXPECT_EQ(evt & ievt::ALIA, 0);
            EXPECT_EQ(evt & ievt::ANCH, 0);
            EXPECT_EQ(evt & ievt::TAG_, 0);
            EXPECT_EQ(evt & ievt::KEY_, 0);
            EXPECT_EQ(evt & ievt::VAL_, 0);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), 0);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), 0);
            EXPECT_EQ(evt & (ievt::BSEQ|ievt::ESEQ), 0);
            EXPECT_EQ(evt & (ievt::FLOW|ievt::BLCK), 0);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0);
        }
        if (evt & ievt::BDOC)
        {
            EXPECT_EQ(evt & ievt::EDOC, 0);
            EXPECT_EQ(evt & ievt::WSTR, 0);
            EXPECT_EQ(evt & ievt::SCLR, 0);
            EXPECT_EQ(evt & ievt::ALIA, 0);
            EXPECT_EQ(evt & ievt::ANCH, 0);
            EXPECT_EQ(evt & ievt::TAG_, 0);
            EXPECT_EQ(evt & ievt::KEY_, 0);
            EXPECT_EQ(evt & ievt::VAL_, 0);
            EXPECT_EQ(evt & (ievt::BSTR|ievt::ESTR), 0);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), 0);
            EXPECT_EQ(evt & (ievt::BSEQ|ievt::ESEQ), 0);
            EXPECT_EQ(evt & (ievt::FLOW|ievt::BLCK), 0);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0);
        }
        if (evt & ievt::EDOC)
        {
            EXPECT_EQ(evt & ievt::BDOC, 0);
            EXPECT_EQ(evt & ievt::WSTR, 0);
            EXPECT_EQ(evt & ievt::SCLR, 0);
            EXPECT_EQ(evt & ievt::ALIA, 0);
            EXPECT_EQ(evt & ievt::ANCH, 0);
            EXPECT_EQ(evt & ievt::TAG_, 0);
            EXPECT_EQ(evt & ievt::KEY_, 0);
            EXPECT_EQ(evt & ievt::VAL_, 0);
            EXPECT_EQ(evt & (ievt::BSTR|ievt::ESTR), 0);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), 0);
            EXPECT_EQ(evt & (ievt::BSEQ|ievt::ESEQ), 0);
            EXPECT_EQ(evt & (ievt::FLOW|ievt::BLCK), 0);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0);
        }
        if (evt & ievt::BSEQ)
        {
            EXPECT_EQ(evt & ievt::ESEQ, 0);
            EXPECT_EQ(evt & ievt::EXPL, 0);
            EXPECT_EQ(evt & ievt::WSTR, 0);
            EXPECT_EQ(evt & ievt::SCLR, 0);
            EXPECT_EQ(evt & ievt::ALIA, 0);
            EXPECT_EQ(evt & ievt::ANCH, 0);
            EXPECT_EQ(evt & ievt::TAG_, 0);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), 0);
            EXPECT_EQ(evt & (ievt::BSTR|ievt::ESTR), 0);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), 0);
            EXPECT_NE(evt & (ievt::FLOW|ievt::BLCK), 0);
            EXPECT_NE(evt & (ievt::FLOW|ievt::BLCK), ievt::FLOW|ievt::BLCK);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), 0);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), ievt::KEY_|ievt::VAL_);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0);
        }
        if (evt & ievt::ESEQ)
        {
            EXPECT_EQ(evt & ievt::BSEQ, 0);
            EXPECT_EQ(evt & ievt::EXPL, 0);
            EXPECT_EQ(evt & ievt::WSTR, 0);
            EXPECT_EQ(evt & ievt::SCLR, 0);
            EXPECT_EQ(evt & ievt::ALIA, 0);
            EXPECT_EQ(evt & ievt::ANCH, 0);
            EXPECT_EQ(evt & ievt::TAG_, 0);
            EXPECT_EQ(evt & ievt::KEY_, 0);
            EXPECT_EQ(evt & ievt::VAL_, 0);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), 0);
            EXPECT_EQ(evt & (ievt::BSTR|ievt::ESTR), 0);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), 0);
            EXPECT_EQ(evt & (ievt::FLOW|ievt::BLCK), 0);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0);
        }
        if (evt & ievt::BMAP)
        {
            EXPECT_EQ(evt & ievt::EMAP, 0);
            EXPECT_EQ(evt & ievt::EXPL, 0);
            EXPECT_EQ(evt & ievt::WSTR, 0);
            EXPECT_EQ(evt & ievt::SCLR, 0);
            EXPECT_EQ(evt & ievt::ALIA, 0);
            EXPECT_EQ(evt & ievt::ANCH, 0);
            EXPECT_EQ(evt & ievt::TAG_, 0);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), 0);
            EXPECT_EQ(evt & (ievt::BSTR|ievt::ESTR), 0);
            EXPECT_EQ(evt & (ievt::BSEQ|ievt::ESEQ), 0);
            EXPECT_NE(evt & (ievt::FLOW|ievt::BLCK), 0);
            EXPECT_NE(evt & (ievt::FLOW|ievt::BLCK), ievt::FLOW|ievt::BLCK);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), 0);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), ievt::KEY_|ievt::VAL_);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0);
        }
        if (evt & ievt::EMAP)
        {
            EXPECT_EQ(evt & ievt::BSEQ, 0);
            EXPECT_EQ(evt & ievt::EXPL, 0);
            EXPECT_EQ(evt & ievt::WSTR, 0);
            EXPECT_EQ(evt & ievt::SCLR, 0);
            EXPECT_EQ(evt & ievt::ALIA, 0);
            EXPECT_EQ(evt & ievt::ANCH, 0);
            EXPECT_EQ(evt & ievt::TAG_, 0);
            EXPECT_EQ(evt & ievt::KEY_, 0);
            EXPECT_EQ(evt & ievt::VAL_, 0);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), 0);
            EXPECT_EQ(evt & (ievt::BSTR|ievt::ESTR), 0);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), 0);
            EXPECT_EQ(evt & (ievt::FLOW|ievt::BLCK), 0);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0);
        }
        if (evt & ievt::SCLR)
        {
            EXPECT_EQ(evt & ievt::EXPL, 0);
            EXPECT_NE(evt & ievt::WSTR, 0);
            EXPECT_EQ(evt & ievt::ALIA, 0);
            EXPECT_EQ(evt & ievt::ANCH, 0);
            EXPECT_EQ(evt & ievt::TAG_, 0);
            EXPECT_NE(evt & ievt::KEY_, 0);
            EXPECT_NE(evt & ievt::VAL_, 0);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), 0);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), ievt::KEY_|ievt::VAL_);
            EXPECT_EQ(evt & (ievt::BSTR|ievt::ESTR), 0);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), 0);
            EXPECT_EQ(evt & (ievt::BSEQ|ievt::ESEQ), 0);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), 0);
            EXPECT_EQ(evt & (ievt::FLOW|ievt::BLCK), 0);
            ievt::DataType style = evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD);
            EXPECT_NE(style, 0);
            EXPECT_EQ((style & (style << 1)), 0);
            #define _test_str_in_buffer(i)                              \
                do {                                                    \
                    EXPECT_LE(i + 3, evts_ints_sz);                     \
                    if (i + 3 <= evts_ints_sz)                          \
                    {                                                   \
                        EXPECT_GE(evts_ints[i + 1], 0);                 \
                        EXPECT_GE(evts_ints[i + 2], 0);                 \
                        if (evts_ints[i + 1] >= 0 && evts_ints[i + 2] >= 0) \
                        {                                               \
                            size_t offset = (size_t)evts_ints[i + 1];   \
                            size_t len = (size_t)evts_ints[i + 2];      \
                            EXPECT_LT(offset, parsed_yaml.len);         \
                            EXPECT_LE(offset + len, parsed_yaml.len);   \
                        }                                               \
                    }                                                   \
                } while(0)
            _test_str_in_buffer(i);
        }
        if (evt & ievt::ALIA)
        {
            EXPECT_EQ(evt & ievt::EXPL, 0);
            EXPECT_NE(evt & ievt::WSTR, 0);
            EXPECT_EQ(evt & ievt::SCLR, 0);
            EXPECT_EQ(evt & ievt::ANCH, 0);
            EXPECT_EQ(evt & ievt::TAG_, 0);
            EXPECT_NE(evt & ievt::KEY_, 0);
            EXPECT_NE(evt & ievt::VAL_, 0);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), 0);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), ievt::KEY_|ievt::VAL_);
            EXPECT_EQ(evt & (ievt::BSTR|ievt::ESTR), 0);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), 0);
            EXPECT_EQ(evt & (ievt::BSEQ|ievt::ESEQ), 0);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), 0);
            EXPECT_EQ(evt & (ievt::FLOW|ievt::BLCK), 0);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0);
            _test_str_in_buffer(i);
        }
        if (evt & ievt::ANCH)
        {
            EXPECT_EQ(evt & ievt::EXPL, 0);
            EXPECT_NE(evt & ievt::WSTR, 0);
            EXPECT_EQ(evt & ievt::SCLR, 0);
            EXPECT_EQ(evt & ievt::ALIA, 0);
            EXPECT_EQ(evt & ievt::TAG_, 0);
            EXPECT_NE(evt & ievt::KEY_, 0);
            EXPECT_NE(evt & ievt::VAL_, 0);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), 0);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), ievt::KEY_|ievt::VAL_);
            EXPECT_EQ(evt & (ievt::BSTR|ievt::ESTR), 0);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), 0);
            EXPECT_EQ(evt & (ievt::BSEQ|ievt::ESEQ), 0);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), 0);
            EXPECT_EQ(evt & (ievt::FLOW|ievt::BLCK), 0);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0);
            _test_str_in_buffer(i);
        }
        if (evt & ievt::TAG_)
        {
            EXPECT_EQ(evt & ievt::EXPL, 0);
            EXPECT_NE(evt & ievt::WSTR, 0);
            EXPECT_EQ(evt & ievt::SCLR, 0);
            EXPECT_EQ(evt & ievt::ALIA, 0);
            EXPECT_EQ(evt & ievt::ANCH, 0);
            EXPECT_NE(evt & ievt::KEY_, 0);
            EXPECT_NE(evt & ievt::VAL_, 0);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), 0);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), ievt::KEY_|ievt::VAL_);
            EXPECT_EQ(evt & (ievt::BSTR|ievt::ESTR), 0);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), 0);
            EXPECT_EQ(evt & (ievt::BSEQ|ievt::ESEQ), 0);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), 0);
            EXPECT_EQ(evt & (ievt::FLOW|ievt::BLCK), 0);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0);
            _test_str_in_buffer(i);
        }
    }
}


size_t emit_events_test_suite_from_ints(
    csubstr parsed_yaml,
    ievt::DataType const* evts_ints,
    ievt::DataType evts_ints_sz,
    substr evts_test_suite)
{
    size_t sz = 0;
    auto append = [&](csubstr s){
        size_t next = sz + s.len;
        if (s.len && (next <= evts_test_suite.len && evts_test_suite.len))
            memcpy(evts_test_suite.str + sz, s.str, s.len);
        sz = next;
    };
    auto getstr = [&](ievt::DataType i){
        return parsed_yaml.range((size_t)evts_ints[i+1], (size_t)evts_ints[i+2]);
    };
    auto append_str = [&](csubstr evt, csubstr str){
        append(evt);
        append(str);
    };
    auto append_val = [&](csubstr evt, csubstr val){
        append(evt);
        substr buf = sz <= evts_test_suite.len ? evts_test_suite.sub(sz) : evts_test_suite.last(0);
        sz += append_escaped(buf, val);
    };
    for(ievt::DataType i = 0; i < evts_ints_sz; )
    {
        ievt::DataType evt = evts_ints[i];
        if (evt & ievt::BSTR)
        {
            append("+STR\n");
        }
        else if (evt & ievt::ESTR)
        {
            append("-STR\n");
        }
        else if (evt & ievt::BDOC)
        {
            if (evt & ievt::EXPL)
                append("+DOC ---\n");
            else
                append("+DOC\n");
        }
        else if (evt & ievt::EDOC)
        {
            if (evt & ievt::EXPL)
                append("-DOC ...\n");
            else
                append("-DOC\n");
        }
        else if (evt & ievt::BSEQ)
        {
            if (evt & ievt::FLOW)
                append("+SEQ []\n");
            else
                append("+SEQ\n");
        }
        else if (evt & ievt::ESEQ)
        {
            append("-SEQ\n");
        }
        else if (evt & ievt::BMAP)
        {
            if (evt & ievt::FLOW)
                append("+MAP []\n");
            else
                append("+MAP\n");
        }
        else if (evt & ievt::EMAP)
        {
            append("-MAP\n");
        }
        else if (evt & ievt::SCLR)
        {
            csubstr s = getstr(i);
            if (evt & ievt::SQUO)
                append_val("+VAL '", s);
            else if (evt & ievt::DQUO)
                append_val("+VAL \"", s);
            else if (evt & ievt::LITL)
                append_val("+VAL |", s);
            else if (evt & ievt::FOLD)
                append_val("+VAL >", s);
            else //if (evt & ievt::PLAI)
                append_val("+VAL :", s);
        }
        else if (evt & ievt::ALIA)
        {
            append_str("=ALI :", getstr(i));
        }

        i += (evt & ievt::WSTR) ? 3 : 1;
    }
    return sz;
}

} // namespace extra
} // namespace yml
} // namespace c4
