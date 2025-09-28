#include "test_lib/test_case.hpp"
#include "./test_events_ints_helpers.hpp"

// NOLINTBEGIN(hicpp-signed-bitwise)

namespace c4 {
namespace yml {
namespace extra {

size_t num_ints(IntEventWithScalar const *evt, size_t evt_size)
{
    size_t sz = 0;
    for(size_t i = 0; i < evt_size; ++i)
        sz += evt[i].required_size();
    return sz;
}


void test_events_ints(IntEventWithScalar const* expected, size_t expected_sz,
                      ievt::DataType const* actual, size_t actual_sz,
                      csubstr yaml,
                      csubstr parsed_source,
                      csubstr arena)
{
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
        csubstr sactual = ievt::to_chars_sub(actualbuf, actual[ia]);
        csubstr sexpect = ievt::to_chars_sub(expectedbuf, expected[ie].flags);
        _test_eq(actual[ia], expected[ie].flags, "", 0);
        _test_eq(sactual, sexpect, "", 0);
        if((expected[ie].flags & ievt::WSTR) && (actual[ia] & ievt::WSTR))
        {
            _test_eq(expected[ie].str_start, actual[ia + 1], "", 0);
            _test_eq(expected[ie].str_len, actual[ia + 2], "", 0);
            bool in_arena = actual[ia] & ievt::AREN;
            bool safeexpected = !in_arena ?
                (expected[ie].str_start < (int)parsed_source.len && expected[ie].str_start + expected[ie].str_len <= (int)parsed_source.len)
                :
                (expected[ie].str_start < (int)arena.len && expected[ie].str_start + expected[ie].str_len <= (int)arena.len);
            bool safeactual = !in_arena ?
                (ia + 2 < actual_sz) && (actual[ia + 1] < (int)parsed_source.len && actual[ia + 1] + actual[ia + 2] <= (int)parsed_source.len)
                :
                (ia + 2 < actual_sz) && (actual[ia + 1] < (int)arena.len && actual[ia + 1] + actual[ia + 2] <= (int)arena.len)                ;
            _test_eq(safeactual, true, "", 0);
            _test_eq(safeactual, safeexpected, "", 0);
            if(safeactual && safeexpected)
            {
                csubstr expectedstr = !in_arena ?
                    parsed_source.sub((size_t)expected[ie].str_start, (size_t)expected[ie].str_len)
                    :
                    arena.sub((size_t)expected[ie].str_start, (size_t)expected[ie].str_len);
                csubstr actualstr = !in_arena ?
                    parsed_source.sub((size_t)actual[ia + 1], (size_t)actual[ia + 2])
                    :
                    arena.sub((size_t)actual[ia + 1], (size_t)actual[ia + 2]);
                _test_eq(expected[ie].scalar, actualstr,
                         "   ref=[{}]~~~{}~~~ vs act=[{}]~~~{}~~~",
                         expected[ie].scalar.len, expected[ie].scalar,
                         actualstr.len, actualstr);
                if( ! expected[ie].needs_filter)
                {
                    _test_eq(expectedstr, actualstr,
                             "   exp=[{}]~~~{}~~~ vs act=[{}]~~~{}~~~",
                             expectedstr.len, expectedstr,
                             actualstr.len, actualstr);
                }
            }
        }
        ia += (actual[ia] & ievt::WSTR) ? 3u : 1u;
    }
    RYML_TRACE_FMT("input:[{}]~~~{}~~~\n"
                   "parsed:[{}]~~~{}~~~\n",
                   "arena:[{}]~~~{}~~~\n",
                   yaml.len, yaml,
                   parsed_source.len, parsed_source,
                   arena.len, arena);
    EXPECT_TRUE(status);
#undef _test_eq
}


void test_events_ints_invariants(csubstr parsed_yaml,
                                 csubstr arena,
                                 ievt::DataType const* evts,
                                 ievt::DataType evts_sz)
{
    char bufpos[100];
    char bufprev[100];
    EXPECT_GT(evts_sz, 0);
    for(ievt::DataType evtpos = 0, evtnumber = 0;
        evtpos < evts_sz;
        ++evtnumber,
            evtpos += ((evts[evtpos] & ievt::WSTR) ? 3 : 1))
    {
        ievt::DataType evt = evts[evtpos];
        ievt::DataType prev = {};
        ievt::DataType nextpos = evtpos + ((evt & ievt::WSTR) ? 3 : 1);
        ievt::DataType next = {};
        SCOPED_TRACE(evtpos); // position in the array
        SCOPED_TRACE(evtnumber); // event number
        SCOPED_TRACE(ievt::to_chars_sub(bufpos, evt));
        if(evtpos)
            prev = (evt & ievt::PSTR) ? evts[evtpos - 3] : evts[evtpos - 1];
        if(nextpos < evts_sz)
            next = evts[nextpos];
        if(evt & ievt::PSTR)
        {
            EXPECT_GT(evtnumber, 0);
            EXPECT_GE(evtpos, 3);
            SCOPED_TRACE(ievt::to_chars_sub(bufprev, prev));
            EXPECT_NE(prev & ievt::WSTR, 0);
        }
        #define _test_str_in_buffer(i)                              \
            do {                                                    \
                EXPECT_LE(i + 3, evts_sz);                          \
                if (i + 3 <= evts_sz)                               \
                {                                                   \
                    bool in_arena = evts[i] & ievt::AREN;           \
                    csubstr buf = !in_arena ? parsed_yaml : arena;  \
                    EXPECT_GE(evts[i + 1], 0);                      \
                    EXPECT_GE(evts[i + 2], 0);                      \
                    if (evts[i + 1] >= 0 && evts[i + 2] >= 0)       \
                    {                                               \
                        size_t offset = (size_t)evts[i + 1];        \
                        size_t len = (size_t)evts[i + 2];           \
                        EXPECT_LE(offset, buf.len);                 \
                        EXPECT_LE(len, buf.len);                    \
                        EXPECT_LE(offset + len, buf.len);   \
                    }                                               \
                }                                                   \
            } while(0)
        if (evt & ievt::YAML)
        {
            EXPECT_EQ(evt & ievt::TAGD, 0);
            EXPECT_EQ(evt & ievt::TAGV, 0);
            EXPECT_EQ(evt & ievt::BSTR, 0);
            EXPECT_EQ(evt & ievt::ESTR, 0);
            EXPECT_EQ(evt & ievt::EXPL, 0);
            EXPECT_EQ(evt & ievt::WSTR, ievt::YAML);
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
            EXPECT_EQ(next & ievt::PSTR, ievt::PSTR);
        }
        if (evt & ievt::TAGD)
        {
            EXPECT_EQ(evt & ievt::YAML, 0);
            EXPECT_EQ(evt & ievt::TAGV, 0);
            EXPECT_EQ(evt & ievt::BSTR, 0);
            EXPECT_EQ(evt & ievt::ESTR, 0);
            EXPECT_EQ(evt & ievt::EXPL, 0);
            EXPECT_EQ(evt & ievt::WSTR, ievt::TAGD);
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
            EXPECT_EQ(next & ievt::PSTR, ievt::PSTR);
        }
        if (evt & ievt::TAGV)
        {
            EXPECT_EQ(evt & ievt::YAML, 0);
            EXPECT_EQ(evt & ievt::TAGD, 0);
            EXPECT_EQ(evt & ievt::BSTR, 0);
            EXPECT_EQ(evt & ievt::ESTR, 0);
            EXPECT_EQ(evt & ievt::EXPL, 0);
            EXPECT_EQ(evt & ievt::WSTR, ievt::TAGV);
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
            EXPECT_EQ(next & ievt::PSTR, ievt::PSTR);
        }
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
            EXPECT_EQ(next & ievt::PSTR, 0);
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
            EXPECT_EQ(next & ievt::PSTR, 0);
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
            EXPECT_EQ(next & ievt::PSTR, 0);
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
            EXPECT_EQ(next & ievt::PSTR, 0);
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
            EXPECT_EQ(next & ievt::PSTR, 0);
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
            EXPECT_EQ(next & ievt::PSTR, 0);
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
            EXPECT_EQ(next & ievt::PSTR, 0);
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
            EXPECT_EQ(evt & (ievt::BSEQ|ievt::ESEQ), 0);
            EXPECT_EQ(evt & (ievt::FLOW|ievt::BLCK), 0);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0);
            EXPECT_EQ(next & ievt::PSTR, 0);
        }
        if (evt & ievt::SCLR)
        {
            EXPECT_EQ(evt & ievt::EXPL, 0);
            EXPECT_NE(evt & ievt::WSTR, 0);
            EXPECT_EQ(evt & ievt::ALIA, 0);
            EXPECT_EQ(evt & ievt::ANCH, 0);
            EXPECT_EQ(evt & ievt::TAG_, 0);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), 0);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), ievt::KEY_|ievt::VAL_);
            EXPECT_EQ(evt & (ievt::BSTR|ievt::ESTR), 0);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), 0);
            EXPECT_EQ(evt & (ievt::BSEQ|ievt::ESEQ), 0);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), 0);
            EXPECT_EQ(evt & (ievt::FLOW|ievt::BLCK), 0);
            EXPECT_EQ(next & ievt::PSTR, ievt::PSTR);
            ievt::DataType style = evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD);
            EXPECT_NE(style, 0);
            EXPECT_EQ((style & (style << 1)), 0);
            _test_str_in_buffer(evtpos);
        }
        if (evt & ievt::ALIA)
        {
            EXPECT_EQ(evt & ievt::EXPL, 0);
            EXPECT_NE(evt & ievt::WSTR, 0);
            EXPECT_EQ(evt & ievt::SCLR, 0);
            EXPECT_EQ(evt & ievt::ANCH, 0);
            EXPECT_EQ(evt & ievt::TAG_, 0);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), 0);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), ievt::KEY_|ievt::VAL_);
            EXPECT_EQ(evt & (ievt::BSTR|ievt::ESTR), 0);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), 0);
            EXPECT_EQ(evt & (ievt::BSEQ|ievt::ESEQ), 0);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), 0);
            EXPECT_EQ(evt & (ievt::FLOW|ievt::BLCK), 0);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0);
            EXPECT_EQ(next & ievt::PSTR, ievt::PSTR);
            _test_str_in_buffer(evtpos);
        }
        if (evt & ievt::ANCH)
        {
            EXPECT_EQ(evt & ievt::EXPL, 0);
            EXPECT_NE(evt & ievt::WSTR, 0);
            EXPECT_EQ(evt & ievt::SCLR, 0);
            EXPECT_EQ(evt & ievt::ALIA, 0);
            EXPECT_EQ(evt & ievt::TAG_, 0);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), 0);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), ievt::KEY_|ievt::VAL_);
            EXPECT_EQ(evt & (ievt::BSTR|ievt::ESTR), 0);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), 0);
            EXPECT_EQ(evt & (ievt::BSEQ|ievt::ESEQ), 0);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), 0);
            EXPECT_EQ(evt & (ievt::FLOW|ievt::BLCK), 0);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0);
            EXPECT_EQ(next & ievt::PSTR, ievt::PSTR);
            _test_str_in_buffer(evtpos);
        }
        if (evt & ievt::TAG_)
        {
            EXPECT_EQ(evt & ievt::EXPL, 0);
            EXPECT_NE(evt & ievt::WSTR, 0);
            EXPECT_EQ(evt & ievt::SCLR, 0);
            EXPECT_EQ(evt & ievt::ALIA, 0);
            EXPECT_EQ(evt & ievt::ANCH, 0);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), 0);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), ievt::KEY_|ievt::VAL_);
            EXPECT_EQ(evt & (ievt::BSTR|ievt::ESTR), 0);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), 0);
            EXPECT_EQ(evt & (ievt::BSEQ|ievt::ESEQ), 0);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), 0);
            EXPECT_EQ(evt & (ievt::FLOW|ievt::BLCK), 0);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0);
            EXPECT_EQ(next & ievt::PSTR, ievt::PSTR);
            _test_str_in_buffer(evtpos);
        }
        #undef _test_str_in_buffer
    }
}

} // namespace extra
} // namespace yml
} // namespace c4

// NOLINTEND(hicpp-signed-bitwise)
