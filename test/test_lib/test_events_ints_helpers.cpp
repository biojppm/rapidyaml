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

void test_events_ints_compare(ievt::Buffers const& expect, ievt::Buffers const& actual)
{
    bool samelen = actual.evts.len == expect.evts.len;
    int  samemem = 0 == memcmp(actual.evts.ptr, expect.evts.ptr, (size_t)actual.evts.len * sizeof(actual.evts.ptr[0]));
    if(samelen && samemem)
        return;
    EXPECT_EQ(actual.evts.len, expect.evts.len);
    EXPECT_EQ(0, samemem);
    char abuf[200];(void)abuf;
    char ebuf[200];(void)ebuf;
    for(evt_size pos_actual = 0, pos_expect = 0, count = 0;
        pos_actual < actual.evts.len && pos_expect < expect.evts.len;
        ++count,
            pos_actual = nextpos(actual.evts.ptr, pos_actual),
            pos_expect = nextpos(expect.evts.ptr, pos_expect))
    {
        const evt_bits evt_actual = actual.evts.ptr[pos_actual];
        const evt_bits evt_expect = expect.evts.ptr[pos_expect];
        #define SHOWINFO                                    \
                       " evtid=" << count << "\n"           \
                    << " pos_actual=" << pos_actual << "\n" \
                    << " pos_expect=" << pos_expect << "\n"
        #define SHOWINFOSTR SHOWINFO \
                << " actual=" << (testing::Message() << as) << "\n" \
                << " expected=" << (testing::Message() << es) << "\n"
        if(evt_actual != evt_expect)
        {
            csubstr as = ievt::to_str_sub(abuf, evt_actual);
            csubstr es = ievt::to_str_sub(ebuf, evt_expect);
            EXPECT_EQ(evt_actual, evt_expect) << SHOWINFOSTR;
            break;
        }
        if(evt_actual & ievt::WSTR)
        {
            const csubstr as = actual.getstr(pos_actual);
            const csubstr es = expect.getstr(pos_expect);
            if(as != es)
            {
                EXPECT_EQ(as, es) << SHOWINFOSTR;
                break;
            }
        }
        if(evt_actual & ievt::RREF)
        {
            evt_bits const *as = actual.evts.ptr + pos_actual;
            evt_bits const *es = expect.evts.ptr + pos_expect;
            if(as[1] != es[1] || as[2] != es[2])
            {
                EXPECT_EQ(as[1], es[1]) << SHOWINFO;
                EXPECT_EQ(as[2], es[2]) << SHOWINFO;
                break;
            }
        }
        if(evt_actual == ESTR)
            break;
        if(evt_expect == ESTR)
            break;
        #undef SHOWINFO
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

enum : evt_bits { // NOLINT
    style_scalar = ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD,
    scope = ievt::MAP_|ievt::SEQ_|ievt::DOC_|ievt::STRM,
    directives = ievt::YAML|ievt::TAGH|ievt::TAGP,
    wstr = directives|ievt::SCLR|ievt::TAG_|ievt::ANCH|ievt::ALIA,
};

evt_size next_sibling(evt_bits const* evts, evt_size sz, evt_size pos)
{
    if(evts[pos] & ievt::BEG_)
    {
        evt_size close = detail::find_matching_close_(evts, sz, pos);
        return ievt::nextpos(evts, close);
    }
    return ievt::nextpos(evts, pos);
}
template<class Fn>
static void iter_children(evt_bits const* evts, evt_size sz, evt_size pos, Fn const& fn)
{
    const evt_size close = detail::find_matching_close_(evts, sz, pos);
    ++pos;
    while(pos != close)
    {
        RYML_TRACE_FMT("child={}", pos);
        fn(pos, evts[pos]);
        if(testing::Test::HasFailure())
            break;
        pos = next_sibling(evts, sz, pos);
    }
}

void test_events_ints_invariants(csubstr parsed_yaml,
                                 csubstr arena,
                                 evt_bits const* evts,
                                 evt_bits evts_sz)
{
    char bufpos[200];
    char bufprev[200];
    EXPECT_GT(evts_sz, 0);
    std::vector<evt_size> path;
    path.reserve(32);
    for(evt_bits evtpos = 0, evtnumber = 0;
        evtpos < evts_sz;
        ++evtnumber,
            evtpos = nextpos(evts, evtpos))
    {
        bool ok = true;
        evt_bits evt = evts[evtpos];
        evt_bits prev = {};
        evt_bits nextpos = evtpos + ((evt & ievt::WSTR) ? 3 : 1);
        evt_bits next = {};
        evt_bits parentpos = path.empty() ? -1 : path.back();
        evt_bits parent = path.empty() ? evt_bits{} : evts[parentpos];
        SCOPED_TRACE(ievt::to_str_sub(bufpos, evt));
        RYML_TRACE_FMT("evt #={} evtpos={} parentpos={}", evtnumber, evtpos, parentpos);
        if(evtpos)
            prev = (evt & ievt::PSTR) ? evts[evtpos - 3] : evts[evtpos - 1];
        if(nextpos < evts_sz)
            next = evts[nextpos];
        if(evt & ievt::BEG_)
        {
            path.push_back(evtpos);
        }
        if(evt & ievt::END_)
        {
            ASSERT_TRUE(!path.empty());
            path.pop_back();
        }
        // check general rules
        if(evt & ievt::WSTR)
        {
            EXPECT_NE(evt & wstr, 0) << (ok = false);
            EXPECT_NE(next & ievt::PSTR, 0) << (ok = false);
            EXPECT_LE(evtpos + 3, evts_sz) << (ok = false);
            if(evtpos + 3 <= evts_sz)
            {
                bool in_arena = evts[evtpos] & ievt::AREN;
                csubstr buf = !in_arena ? parsed_yaml : arena;
                RYML_TRACE_FMT("in_arena={}", in_arena);
                EXPECT_GE(evts[evtpos + 1], 0) << (ok = false);
                EXPECT_GE(evts[evtpos + 2], 0) << (ok = false);
                if(evts[evtpos + 1] >= 0 &&
                   evts[evtpos + 2] >= 0)
                {
                    size_t offset = (size_t)evts[evtpos + 1];
                    size_t len = (size_t)evts[evtpos + 2];
                    EXPECT_LE(offset, buf.len) << (ok = false);
                    EXPECT_LE(len, buf.len) << (ok = false);
                    EXPECT_LE(offset + len, buf.len) << (ok = false);
                }
            }
        }
        if(evt & ievt::PSTR)
        {
            SCOPED_TRACE(ievt::to_str_sub(bufprev, prev));
            EXPECT_GT(evtnumber, 0) << (ok = false);
            EXPECT_GE(evtpos, 3) << (ok = false);
            EXPECT_NE(prev & ievt::WSTR, 0) << (ok = false);
        }
        if(evt & (ievt::BEG_|ievt::END_))
        {
            EXPECT_NE(evt & scope, 0) << (ok = false);
            EXPECT_EQ(evt & style_scalar, 0) << (ok = false);
            EXPECT_EQ(evt & directives, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::WSTR, 0) << (ok = false);
        }
        if(evt & (ievt::FLOW))
        {
            EXPECT_NE(evt & (ievt::DOC_|ievt::MAP_|ievt::SEQ_), 0) << (ok = false);
            EXPECT_EQ(evt & style_scalar, 0) << (ok = false);
        }
        if(evt & (ievt::AREN))
        {
            EXPECT_NE(evt & ievt::WSTR, 0) << (ok = false);
            EXPECT_EQ(evt & directives, 0) << (ok = false);
        }
        if(evt & (ievt::KEY_|ievt::VAL_))
        {
            EXPECT_EQ(evt & (ievt::DOC_|ievt::STRM|ievt::TAGP|ievt::TAGH|ievt::YAML), 0) << (ok = false);
            EXPECT_EQ(evt & directives, 0) << (ok = false);
            EXPECT_NE(parent & (ievt::SEQ_|ievt::MAP_|ievt::DOC_), 0);
            EXPECT_EQ(parent & ievt::BEG_, ievt::BEG_);
        }
        if(evt & ievt::KEY_)
        {
            EXPECT_EQ(parent & ievt::MAP_, ievt::MAP_);
        }
        if(evt & ievt::FLOW)
        {
            if(evt & (ievt::MAP_|ievt::SEQ_))
            {
                EXPECT_EQ(evt & ievt::BEG_, ievt::BEG_) << (ok = false);
                EXPECT_EQ(evt & ievt::END_, 0) << (ok = false);
            }
            if(evt & ievt::DOC_)
            {
                EXPECT_NE(evt & (ievt::BEG_|ievt::END_), 0) << (ok = false);
            }
            EXPECT_NE(evt & (ievt::MAP_|ievt::SEQ_|ievt::DOC_), 0) << (ok = false);
        }
        if(evt & (ievt::FSL_|ievt::FML1|ievt::FMLN|ievt::FMLX|ievt::FSPC))
        {
            EXPECT_EQ(evt & ievt::FLOW, ievt::FLOW) << (ok = false);
            EXPECT_EQ(evt & ievt::BEG_, ievt::BEG_) << (ok = false);
            EXPECT_EQ(evt & ievt::END_, 0) << (ok = false);
            EXPECT_NE(evt & (ievt::MAP_|ievt::SEQ_), 0) << (ok = false);
        }
        // now check each flag
        if(evt & ievt::YAML)
        {
            EXPECT_EQ(parent & ievt::BSTR, ievt::BSTR);
            EXPECT_EQ(parentpos, 0);
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
            EXPECT_EQ(evt & (ievt::FLOW|ievt::FSL_|ievt::FMLX|ievt::FSPC), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0) << (ok = false);
            EXPECT_EQ(next & ievt::PSTR, ievt::PSTR) << (ok = false);
        }
        if(evt & ievt::TAGH)
        {
            EXPECT_EQ(parent & ievt::BSTR, ievt::BSTR);
            EXPECT_EQ(parentpos, 0);
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
            EXPECT_EQ(evt & (ievt::FLOW|ievt::FSL_|ievt::FMLX|ievt::FSPC), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0) << (ok = false);
            EXPECT_EQ(next & ievt::PSTR, ievt::PSTR) << (ok = false);
            EXPECT_EQ(next & ievt::TAGP, ievt::TAGP) << (ok = false);
        }
        if(evt & ievt::TAGP)
        {
            EXPECT_EQ(parent & ievt::BSTR, ievt::BSTR);
            EXPECT_EQ(parentpos, 0);
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
            EXPECT_EQ(evt & (ievt::FLOW|ievt::FSL_|ievt::FMLX|ievt::FSPC), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0) << (ok = false);
            EXPECT_EQ(next & ievt::PSTR, ievt::PSTR) << (ok = false);
            EXPECT_EQ(prev & ievt::TAGH, ievt::TAGH) << (ok = false);
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
            EXPECT_EQ(evt & (ievt::FLOW|ievt::FSL_|ievt::FMLX|ievt::FSPC), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0) << (ok = false);
            EXPECT_EQ(next & ievt::PSTR, 0) << (ok = false);
            iter_children(evts, evts_sz, evtpos, [&](evt_size, evt_bits child_evt){
                EXPECT_NE(child_evt & (ievt::DOC_|ievt::YAML|ievt::TAGH|ievt::TAGP), 0) << (ok = false);
                EXPECT_EQ(child_evt & (ievt::SEQ_|ievt::MAP_|ievt::TAG_|ievt::ANCH|ievt::ALIA), 0) << (ok = false);
            });
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
            EXPECT_EQ(evt & (ievt::FLOW|ievt::FSL_|ievt::FMLX|ievt::FSPC), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0) << (ok = false);
            EXPECT_EQ(next & ievt::PSTR, 0) << (ok = false);
        }
        if((evt & ievt::BDOC) == ievt::BDOC)
        {
            EXPECT_EQ(parentpos, 0);
            EXPECT_EQ(parent & ievt::BSTR, ievt::BSTR);
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
            EXPECT_EQ(evt & (ievt::FSL_|ievt::FMLX|ievt::FSPC), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0) << (ok = false);
            EXPECT_EQ(next & ievt::PSTR, 0) << (ok = false);
            iter_children(evts, evts_sz, evtpos, [&](evt_size, evt_bits child_evt){
                EXPECT_NE(child_evt & (ievt::SEQ_|ievt::MAP_|ievt::SCLR|ievt::TAG_|ievt::ANCH|ievt::ALIA), 0) << (ok = false);
            });
        }
        if((evt & ievt::EDOC) == ievt::EDOC)
        {
            EXPECT_EQ(parent & ievt::BDOC, ievt::BDOC);
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
            EXPECT_EQ(evt & (ievt::FSL_|ievt::FMLX|ievt::FSPC), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0) << (ok = false);
            EXPECT_EQ(next & ievt::PSTR, 0) << (ok = false);
        }
        if((evt & ievt::BSEQ) == ievt::BSEQ)
        {
            EXPECT_EQ(evt & ievt::ESEQ, ievt::SEQ_) << (ok = false);
            EXPECT_EQ(evt & ievt::EMAP, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::WSTR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::SCLR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ALIA, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ANCH, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::TAG_, 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), ievt::BEG_) << (ok = false);
            EXPECT_EQ(evt & (ievt::BSTR|ievt::ESTR), ievt::BEG_) << (ok = false);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), ievt::BEG_) << (ok = false);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), 0) << (ok = false);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), ievt::KEY_|ievt::VAL_) << (ok = false);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0) << (ok = false);
            EXPECT_EQ(next & ievt::PSTR, 0) << (ok = false);
            iter_children(evts, evts_sz, evtpos, [&](evt_size, evt_bits child_evt){
                if(child_evt)
                {
                    EXPECT_NE(child_evt & (ievt::SEQ_|ievt::MAP_|ievt::SCLR|ievt::TAG_|ievt::ANCH|ievt::ALIA|ievt::RREF), 0) << (ok = false);
                    EXPECT_EQ(child_evt & ievt::KEY_, 0) << (ok = false);
                }
            });
        }
        if((evt & ievt::ESEQ) == ievt::ESEQ)
        {
            EXPECT_EQ(evt & ievt::BSEQ, ievt::SEQ_) << (ok = false);
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
            EXPECT_EQ(evt & (ievt::FLOW), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0) << (ok = false);
            EXPECT_EQ(next & ievt::PSTR, 0) << (ok = false);
        }
        if((evt & ievt::BMAP) == ievt::BMAP)
        {
            EXPECT_EQ(evt & ievt::EMAP, ievt::MAP_) << (ok = false);
            EXPECT_EQ(evt & ievt::WSTR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::SCLR, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ALIA, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::ANCH, 0) << (ok = false);
            EXPECT_EQ(evt & ievt::TAG_, 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), ievt::BEG_) << (ok = false);
            EXPECT_EQ(evt & (ievt::BSTR|ievt::ESTR), ievt::BEG_) << (ok = false);
            EXPECT_EQ(evt & (ievt::BSEQ|ievt::ESEQ), ievt::BEG_) << (ok = false);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), 0) << (ok = false);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), ievt::KEY_|ievt::VAL_) << (ok = false);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0) << (ok = false);
            EXPECT_EQ(next & ievt::PSTR, 0) << (ok = false);
            bool key_state = true;
            iter_children(evts, evts_sz, evtpos, [&](evt_size, evt_bits child_evt){
                EXPECT_NE(child_evt & (ievt::SEQ_|ievt::MAP_|ievt::SCLR|ievt::TAG_|ievt::ANCH|ievt::ALIA|ievt::RREF), 0) << (ok = false);
                if(key_state)
                {
                    EXPECT_EQ(child_evt & ievt::KEY_, ievt::KEY_) << (ok = false);
                    EXPECT_EQ(child_evt & ievt::VAL_, 0) << (ok = false);
                }
                else
                {
                    EXPECT_EQ(child_evt & ievt::KEY_, 0) << (ok = false);
                    EXPECT_EQ(child_evt & ievt::VAL_, ievt::VAL_) << (ok = false);
                }
                if(child_evt & (ievt::SEQ_|ievt::MAP_|ievt::SCLR|ievt::ALIA|ievt::RREF))
                    key_state = !key_state;
            });
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
            EXPECT_EQ(evt & (ievt::FLOW), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0) << (ok = false);
            EXPECT_EQ(next & ievt::PSTR, 0) << (ok = false);
        }
        if(evt & ievt::SCLR)
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
            EXPECT_EQ(evt & (ievt::FLOW|ievt::FSL_|ievt::FMLX|ievt::FSPC), 0) << (ok = false);
            EXPECT_EQ(next & ievt::PSTR, ievt::PSTR) << (ok = false);
            evt_bits estyle = evt & style_scalar;
            EXPECT_EQ((estyle & (estyle << 1)), 0) << (ok = false);
        }
        if(evt & ievt::ALIA)
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
            EXPECT_EQ(evt & (ievt::FLOW|ievt::FSL_|ievt::FMLX|ievt::FSPC), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0) << (ok = false);
            EXPECT_EQ(next & ievt::PSTR, ievt::PSTR) << (ok = false);
        }
        if(evt & ievt::ANCH)
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
            EXPECT_EQ(evt & (ievt::FLOW|ievt::FSL_|ievt::FMLX|ievt::FSPC), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0) << (ok = false);
            EXPECT_EQ(next & ievt::PSTR, ievt::PSTR) << (ok = false);
        }
        if(evt & ievt::TAG_)
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
            EXPECT_EQ(evt & (ievt::FLOW|ievt::FSL_|ievt::FMLX|ievt::FSPC), 0) << (ok = false);
            EXPECT_EQ(evt & (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD), 0) << (ok = false);
            EXPECT_EQ(next & ievt::PSTR, ievt::PSTR) << (ok = false);
        }
        if(!ok)
            break;
    }
}


namespace {
std::string filter_emitted_yaml_ints(csubstr em)
{
    std::string filtered;
    filtered.reserve(em.len);
    size_t line_start = 0;
    size_t line_end = em.find('\n');
    while(true)
    {
        if(line_end != csubstr::npos)
            ++line_end;
        csubstr line = em.range(line_start, line_end);
        // skip YAML directives
        if(line.begins_with("%YAML "))
            goto skip; // NOLINT
        // skip expl end docs if they're not followed by directives
        else if(line == "...\n")
        {
            csubstr next_line = em.sub(line_end);
            if(!next_line.begins_with("%TAG"))
                goto skip; // NOLINT
        }
        filtered.append(line.str, line.len);
    skip:
        if(line_end == csubstr::npos || line_end >= em.len)
            break;
        line_start = line_end;
        line_end = em.find('\n', line_start);
    }
    return filtered;
}

bool compare_emitted_yaml_ints(csubstr emitted_, csubstr expected_)
{
    if(expected_.begins_with("---\n") || expected_.begins_with("--- "))
    {
        if(expected_.sub(4) == emitted_)
            return true;
    }
    if(emitted_.ends_with("...\n"))
    {
        if(expected_ == emitted_.offs(0, 4))
            return true;
    }
    return false;
}
} // namespace

void test_compare_emitted_yaml_ints(std::string const& emitted, std::string const& expected)
{
    SCOPED_TRACE("compare_emitted_yaml_ints");
    if(emitted == expected)
        return;
    // they differ. work around some edge cases.
    if(compare_emitted_yaml_ints(to_csubstr(emitted), to_csubstr(expected)))
        return;
    // remove internal ...\n
    std::string filtered = filter_emitted_yaml_ints(to_csubstr(emitted));
    if(filtered == expected)
        return;
    if(compare_emitted_yaml_ints(to_csubstr(filtered), to_csubstr(expected)))
        return;
    RYML_TRACE_FMT("filtered=~~~\n{}~~~\n", filtered);
    EXPECT_EQ(expected, emitted);
}

} // namespace ievt
} // namespace extra
} // namespace yml
} // namespace c4

// NOLINTEND(hicpp-signed-bitwise)
