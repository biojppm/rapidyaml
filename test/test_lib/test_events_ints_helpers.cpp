#include "test_lib/test_case.hpp"
#include "./test_events_ints_helpers.hpp"

// NOLINTBEGIN(hicpp-signed-bitwise)

namespace c4 {
namespace yml {
namespace extra {
namespace ievt {

size_t num_ints(IntEventWithScalar const *evt, size_t evt_sz)
{
    size_t sz = 0;
    for(size_t i = 0; i < evt_sz; ++i)
        sz += evt[i].required_size();
    return sz;
}

void test_events_ints_compare(ievt::Buffers const& expected, ievt::Buffers const& actual)
{
    bool samelen = actual.evts.len == expected.evts.len;
    int  samemem = memcmp(actual.evts.ptr, expected.evts.ptr, (size_t)actual.evts.len * sizeof(actual.evts.ptr[0]));
    if(samelen && samemem)
        return;
    EXPECT_EQ(actual.evts.len, expected.evts.len);
    EXPECT_EQ(0, samemem);
    const evt_size minsz = actual.evts.len < expected.evts.len ? actual.evts.len : expected.evts.len;
    char abuf[200];(void)abuf;
    char ebuf[200];(void)ebuf;
    for(evt_size i = 0, count = 0; i < minsz; ++i, ++count)
    {
        const evt_bits ai = actual.evts.ptr[i];
        const evt_bits ei = expected.evts.ptr[i];
        if(ai != ei)
        {
            csubstr as = ievt::to_str_sub(abuf, ai);
            csubstr es = ievt::to_str_sub(ebuf, ei);
            EXPECT_EQ(ai, ei) << "\n"
                << " evtid=" << count << "\n"
                << " evtpos=" << i << "\n"
                << " actual=" << (testing::Message() << as) << "\n"
                << " expected=" << (testing::Message() << es) << "\n"
                ;
            break;
        }
        if(ai & ievt::WSTR)
        {
            const csubstr as = actual.getstr(i);
            const csubstr es = expected.getstr(i);
            if(as != es)
            {
                EXPECT_EQ(as, es);
                break;
            }
            i += 2;
        }
    }
}

void test_events_ints(IntEventWithScalar const* expected, size_t expected_sz,
                      evt_bits const* actual, size_t actual_sz,
                      csubstr yaml,
                      csubstr parsed_source,
                      csubstr arena)
{
    int status = true;
    size_t num_ints_expected = num_ints(expected, expected_sz);

    EXPECT_EQ(actual_sz, num_ints_expected);
    status = (actual_sz == num_ints_expected);

    char actualbuf[200];(void)actualbuf;
    char expectedbuf[200];(void)expectedbuf;
    for(size_t ia = 0, ie = 0; ie < expected_sz; ++ie)
    {
        EXPECT_LT(ia, actual_sz);
        if (ia >= actual_sz)
            break;
        #define _test_eq(lhs, rhs, fmt, ...)                        \
        do                                                          \
        {                                                           \
            _c4dbgpf("status={} cmp={} ie={} ia={}: {}={} == {}={} " fmt, \
                status, (lhs == rhs), ie, ia, #lhs, lhs, rhs, #rhs, __VA_ARGS__); \
            status &= int(lhs == rhs);                              \
            EXPECT_EQ(lhs, rhs);                                    \
        } while(0)
        csubstr sactual = ievt::to_str_sub(actualbuf, actual[ia]);
        csubstr sexpect = ievt::to_str_sub(expectedbuf, expected[ie].flags);
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
                                 evt_bits const* evts,
                                 evt_bits evts_sz)
{
    char bufpos[200];
    char bufprev[200];
    EXPECT_GT(evts_sz, 0);
    for(evt_bits evtpos = 0, evtnumber = 0;
        evtpos < evts_sz;
        ++evtnumber,
            evtpos += ((evts[evtpos] & ievt::WSTR) ? 3 : 1))
    {
        bool ok = true;
        evt_bits evt = evts[evtpos];
        evt_bits prev = {};
        evt_bits nextpos = evtpos + ((evt & ievt::WSTR) ? 3 : 1);
        evt_bits next = {};
        SCOPED_TRACE(evtpos); // position in the array
        SCOPED_TRACE(evtnumber); // event number
        SCOPED_TRACE(ievt::to_str_sub(bufpos, evt));
        if(evtpos)
            prev = (evt & ievt::PSTR) ? evts[evtpos - 3] : evts[evtpos - 1];
        if(nextpos < evts_sz)
            next = evts[nextpos];
        #define _test_str_in_buffer(i)                                  \
        do {                                                            \
            EXPECT_LE((i) + 3, evts_sz) << (ok = false);                \
            if((i) + 3 <= evts_sz)                                      \
            {                                                           \
                bool in_arena = evts[i] & ievt::AREN;                   \
                csubstr buf = !in_arena ? parsed_yaml : arena;          \
                EXPECT_GE(evts[(i) + 1], 0) << (ok = false);            \
                EXPECT_GE(evts[(i) + 2], 0) << (ok = false);            \
                if(evts[(i) + 1] >= 0 && evts[(i) + 2] >= 0)            \
                {                                                       \
                    size_t offset = (size_t)evts[(i) + 1];              \
                    size_t len = (size_t)evts[(i) + 2];                 \
                    EXPECT_LE(offset, buf.len) << (ok = false);         \
                    EXPECT_LE(len, buf.len) << (ok = false);            \
                    EXPECT_LE(offset + len, buf.len) << (ok = false);   \
                }                                                       \
            }                                                           \
        } while(0)
        // check general rules
        if(evt & ievt::PSTR)
        {
            EXPECT_GT(evtnumber, 0) << (ok = false);
            EXPECT_GE(evtpos, 3) << (ok = false);
            SCOPED_TRACE(ievt::to_str_sub(bufprev, prev));
            EXPECT_NE(prev & ievt::WSTR, 0) << (ok = false);
        }
        constexpr const evt_bits style_scalar = ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD;
        constexpr const evt_bits scope = ievt::MAP_|ievt::SEQ_|ievt::DOC_|ievt::STRM;
        constexpr const evt_bits directives = ievt::YAML|ievt::TAGH|ievt::TAGP;
        if(evt & (ievt::BEG_|ievt::END_))
        {
            EXPECT_NE(evt & scope, 0) << (ok = false);
            EXPECT_EQ(evt & style_scalar, 0) << (ok = false);
            EXPECT_EQ(evt & directives, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::WSTR, 0) << (ok = false);
        }
        if(evt & (ievt::EXPL))
        {
            EXPECT_EQ(evt & ievt::DOC_, ievt::DOC_) << (ok = false);
            EXPECT_EQ(evt & style_scalar, 0) << (ok = false);
        }
        if(evt & (ievt::AREN))
        {
            EXPECT_NE(evt & ievt::WSTR, 0) << (ok = false);
            EXPECT_EQ(evt & directives, 0) << (ok = false);
        }
        if(evt & (ievt::KEY_|ievt::VAL_))
        {
            EXPECT_EQ(evt & (ievt::DOC_|ievt::STRM), 0) << (ok = false);
            EXPECT_EQ(evt & directives, 0) << (ok = false);
        }
        if(evt & (ievt::FLOW|ievt::BLCK))
        {
            EXPECT_EQ(evt & ievt::BEG_, ievt::BEG_) << (ok = false);
            EXPECT_EQ(evt & ievt::END_, 0) << (ok = false);
            EXPECT_NE(evt & (ievt::MAP_|ievt::SEQ_), 0) << (ok = false);
        }
        if(evt & (ievt::FSL_|ievt::FML1|ievt::FMLN|ievt::FMLX|ievt::FSPC))
        {
            EXPECT_EQ(evt & ievt::BLCK, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::FLOW, ievt::FLOW) << (ok = false);
            EXPECT_EQ(evt & ievt::BEG_, ievt::BEG_) << (ok = false);
            EXPECT_EQ(evt & ievt::END_, 0) << (ok = false);
            EXPECT_NE(evt & (ievt::MAP_|ievt::SEQ_), 0) << (ok = false);
        }
        // now check each flag
        if(evt & ievt::YAML)
        {
            EXPECT_EQ(evt & ievt::TAGH, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::TAGP, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::BSTR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ESTR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::EXPL, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::WSTR, ievt::YAML) << (ok = false);
            EXPECT_EQ(evt & ievt::SCLR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ALIA, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ANCH, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::TAG_, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::KEY_, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::VAL_, 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::BSEQ|ievt::ESEQ), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::FLOW|ievt::BLCK), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0) << (ok = false);
            EXPECT_EQ(next & ievt::PSTR, ievt::PSTR) << (ok = false);
        }
        if(evt & ievt::TAGH)
        {
            EXPECT_EQ(evt & ievt::YAML, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::TAGP, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::BSTR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ESTR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::EXPL, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::WSTR, ievt::TAGH) << (ok = false);
            EXPECT_EQ(evt & ievt::SCLR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ALIA, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ANCH, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::TAG_, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::KEY_, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::VAL_, 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::BSEQ|ievt::ESEQ), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::FLOW|ievt::BLCK), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0) << (ok = false);
            EXPECT_EQ(next & ievt::PSTR, ievt::PSTR) << (ok = false);
        }
        if(evt & ievt::TAGP)
        {
            EXPECT_EQ(evt & ievt::YAML, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::TAGH, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::BSTR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ESTR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::EXPL, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::WSTR, ievt::TAGP) << (ok = false);
            EXPECT_EQ(evt & ievt::SCLR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ALIA, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ANCH, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::TAG_, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::KEY_, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::VAL_, 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::BSEQ|ievt::ESEQ), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::FLOW|ievt::BLCK), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0) << (ok = false);
            EXPECT_EQ(next & ievt::PSTR, ievt::PSTR) << (ok = false);
        }
        if((evt & ievt::BSTR) == ievt::BSTR)
        {
            EXPECT_EQ(evt & ievt::ESTR, ievt::STRM) << (ok = false);
            EXPECT_EQ(evt & ievt::EXPL, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::WSTR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::SCLR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ALIA, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ANCH, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::TAG_, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::KEY_, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::VAL_, 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), ievt::BEG_) << (ok = false);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), ievt::BEG_) << (ok = false);
            EXPECT_EQ(evt & (ievt::BSEQ|ievt::ESEQ), ievt::BEG_) << (ok = false);
            EXPECT_EQ(evt & (ievt::FLOW|ievt::BLCK), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0) << (ok = false);
            EXPECT_EQ(next & ievt::PSTR, 0) << (ok = false);
        }
        if((evt & ievt::ESTR) == ievt::ESTR)
        {
            EXPECT_EQ(evt & ievt::BSTR, ievt::STRM) << (ok = false);
            EXPECT_EQ(evt & ievt::EXPL, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::WSTR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::SCLR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ALIA, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ANCH, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::TAG_, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::KEY_, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::VAL_, 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), ievt::END_) << (ok = false);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), ievt::END_) << (ok = false);
            EXPECT_EQ(evt & (ievt::BSEQ|ievt::ESEQ), ievt::END_) << (ok = false);
            EXPECT_EQ(evt & (ievt::FLOW|ievt::BLCK), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0) << (ok = false);
            EXPECT_EQ(next & ievt::PSTR, 0) << (ok = false);
        }
        if((evt & ievt::BDOC) == ievt::BDOC)
        {
            EXPECT_EQ(evt & ievt::EDOC, ievt::DOC_) << (ok = false);
            EXPECT_EQ(evt & ievt::WSTR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::SCLR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ALIA, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ANCH, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::TAG_, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::KEY_, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::VAL_, 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::BSTR|ievt::ESTR), ievt::BEG_) << (ok = false);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), ievt::BEG_) << (ok = false);
            EXPECT_EQ(evt & (ievt::BSEQ|ievt::ESEQ), ievt::BEG_) << (ok = false);
            EXPECT_EQ(evt & (ievt::FLOW|ievt::BLCK), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0) << (ok = false);
            EXPECT_EQ(next & ievt::PSTR, 0) << (ok = false);
        }
        if((evt & ievt::EDOC) == ievt::EDOC)
        {
            EXPECT_EQ(evt & ievt::BDOC, ievt::DOC_) << (ok = false);
            EXPECT_EQ(evt & ievt::WSTR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::SCLR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ALIA, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ANCH, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::TAG_, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::KEY_, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::VAL_, 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::BSTR|ievt::ESTR), ievt::END_) << (ok = false);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), ievt::END_) << (ok = false);
            EXPECT_EQ(evt & (ievt::BSEQ|ievt::ESEQ), ievt::END_) << (ok = false);
            EXPECT_EQ(evt & (ievt::FLOW|ievt::BLCK), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0) << (ok = false);
            EXPECT_EQ(next & ievt::PSTR, 0) << (ok = false);
        }
        if((evt & ievt::BSEQ) == ievt::BSEQ)
        {
            EXPECT_EQ(evt & ievt::ESEQ, ievt::SEQ_) << (ok = false);
            EXPECT_EQ(evt & ievt::EMAP, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::EXPL, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::WSTR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::SCLR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ALIA, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ANCH, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::TAG_, 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), ievt::BEG_) << (ok = false);
            EXPECT_EQ(evt & (ievt::BSTR|ievt::ESTR), ievt::BEG_) << (ok = false);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), ievt::BEG_) << (ok = false);
            EXPECT_NE(evt & (ievt::FLOW|ievt::BLCK), 0) << (ok = false);
            EXPECT_NE(evt & (ievt::FLOW|ievt::BLCK), ievt::FLOW|ievt::BLCK) << (ok = false);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), 0) << (ok = false);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), ievt::KEY_|ievt::VAL_) << (ok = false);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0) << (ok = false);
            EXPECT_EQ(next & ievt::PSTR, 0) << (ok = false);
        }
        if((evt & ievt::ESEQ) == ievt::ESEQ)
        {
            EXPECT_EQ(evt & ievt::BSEQ, ievt::SEQ_) << (ok = false);
            EXPECT_EQ(evt & ievt::EXPL, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::WSTR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::SCLR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ALIA, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ANCH, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::TAG_, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::KEY_, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::VAL_, 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), ievt::END_) << (ok = false);
            EXPECT_EQ(evt & (ievt::BSTR|ievt::ESTR), ievt::END_) << (ok = false);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), ievt::END_) << (ok = false);
            EXPECT_EQ(evt & (ievt::FLOW|ievt::BLCK), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0) << (ok = false);
            EXPECT_EQ(next & ievt::PSTR, 0) << (ok = false);
        }
        if((evt & ievt::BMAP) == ievt::BMAP)
        {
            EXPECT_EQ(evt & ievt::EMAP, ievt::MAP_) << (ok = false);
            EXPECT_EQ(evt & ievt::EXPL, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::WSTR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::SCLR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ALIA, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ANCH, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::TAG_, 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), ievt::BEG_) << (ok = false);
            EXPECT_EQ(evt & (ievt::BSTR|ievt::ESTR), ievt::BEG_) << (ok = false);
            EXPECT_EQ(evt & (ievt::BSEQ|ievt::ESEQ), ievt::BEG_) << (ok = false);
            EXPECT_NE(evt & (ievt::FLOW|ievt::BLCK), 0) << (ok = false);
            EXPECT_NE(evt & (ievt::FLOW|ievt::BLCK), ievt::FLOW|ievt::BLCK) << (ok = false);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), 0) << (ok = false);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), ievt::KEY_|ievt::VAL_) << (ok = false);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0) << (ok = false);
            EXPECT_EQ(next & ievt::PSTR, 0) << (ok = false);
        }
        if((evt & ievt::EMAP) == ievt::EMAP)
        {
            EXPECT_EQ(evt & ievt::BSEQ, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::EXPL, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::WSTR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::SCLR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ALIA, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ANCH, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::TAG_, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::KEY_, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::VAL_, 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), ievt::END_) << (ok = false);
            EXPECT_EQ(evt & (ievt::BSTR|ievt::ESTR), ievt::END_) << (ok = false);
            EXPECT_EQ(evt & (ievt::BSEQ|ievt::ESEQ), ievt::END_) << (ok = false);
            EXPECT_EQ(evt & (ievt::FLOW|ievt::BLCK), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0) << (ok = false);
            EXPECT_EQ(next & ievt::PSTR, 0) << (ok = false);
        }
        if (evt & ievt::SCLR)
        {
            EXPECT_EQ(evt & ievt::EXPL, 0) << (ok = false);
            EXPECT_NE(evt & ievt::WSTR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ALIA, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ANCH, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::TAG_, 0) << (ok = false);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), 0) << (ok = false);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), ievt::KEY_|ievt::VAL_) << (ok = false);
            EXPECT_EQ(evt & (ievt::BSTR|ievt::ESTR), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::BSEQ|ievt::ESEQ), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::FLOW|ievt::BLCK), 0) << (ok = false);
            EXPECT_EQ(next & ievt::PSTR, ievt::PSTR) << (ok = false);
            evt_bits estyle = evt & style_scalar << (ok = false);
            EXPECT_NE(estyle, 0) << (ok = false);
            EXPECT_EQ((estyle & (estyle << 1)), 0) << (ok = false);
            _test_str_in_buffer(evtpos);
        }
        if (evt & ievt::ALIA)
        {
            EXPECT_EQ(evt & ievt::EXPL, 0) << (ok = false);
            EXPECT_NE(evt & ievt::WSTR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::SCLR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ANCH, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::TAG_, 0) << (ok = false);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), 0) << (ok = false);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), ievt::KEY_|ievt::VAL_) << (ok = false);
            EXPECT_EQ(evt & (ievt::BSTR|ievt::ESTR), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::BSEQ|ievt::ESEQ), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::FLOW|ievt::BLCK), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0) << (ok = false);
            EXPECT_EQ(next & ievt::PSTR, ievt::PSTR) << (ok = false);
            _test_str_in_buffer(evtpos);
        }
        if (evt & ievt::ANCH)
        {
            EXPECT_EQ(evt & ievt::EXPL, 0) << (ok = false);
            EXPECT_NE(evt & ievt::WSTR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::SCLR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ALIA, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::TAG_, 0) << (ok = false);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), 0) << (ok = false);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), ievt::KEY_|ievt::VAL_) << (ok = false);
            EXPECT_EQ(evt & (ievt::BSTR|ievt::ESTR), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::BSEQ|ievt::ESEQ), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::FLOW|ievt::BLCK), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0) << (ok = false);
            EXPECT_EQ(next & ievt::PSTR, ievt::PSTR) << (ok = false);
            _test_str_in_buffer(evtpos);
        }
        if (evt & ievt::TAG_)
        {
            EXPECT_EQ(evt & ievt::EXPL, 0) << (ok = false);
            EXPECT_NE(evt & ievt::WSTR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::SCLR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ALIA, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ANCH, 0) << (ok = false);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), 0) << (ok = false);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), ievt::KEY_|ievt::VAL_) << (ok = false);
            EXPECT_EQ(evt & (ievt::BSTR|ievt::ESTR), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::BSEQ|ievt::ESEQ), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::FLOW|ievt::BLCK), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0) << (ok = false);
            EXPECT_EQ(next & ievt::PSTR, ievt::PSTR) << (ok = false);
            _test_str_in_buffer(evtpos);
        }
        if(!ok)
            break;
        #undef _test_str_in_buffer
    }
}

} // namespace ievt
} // namespace extra
} // namespace yml
} // namespace c4

// NOLINTEND(hicpp-signed-bitwise)
