#include "test_lib/test_case.hpp"
#include "./test_helpers_ints.hpp"

C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")

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

void test_events_ints_compare(ievt::Buffers const& expect, ievt::Buffers const& actual, evt_bits mask_ignore)
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
            evt_bits diff = evt_actual ^ evt_expect;
            if(diff & (~mask_ignore))
            {
                csubstr as = ievt::to_str_sub(abuf, evt_actual);
                csubstr es = ievt::to_str_sub(ebuf, evt_expect);
                EXPECT_EQ(evt_actual, evt_expect) << SHOWINFOSTR;
            }
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
        #undef _test_eq
    }
    RYML_TRACE_FMT("input:[{}]~~~{}~~~\n"
                   "parsed:[{}]~~~{}~~~\n",
                   "arena:[{}]~~~{}~~~\n",
                   yaml.len, yaml,
                   parsed_source.len, parsed_source,
                   arena.len, arena);
    EXPECT_TRUE(status);
}



//-----------------------------------------------------------------------------

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


//-----------------------------------------------------------------------------

namespace {
void fill_visit_seq_fwd(evt_bits const* evts, evt_size evts_sz,
                        evt_size begpos, evt_size endpos,
                        std::vector<evt_size> *seq)
{
    for(evt_size pos = begpos; pos != endpos; pos = nextpos(evts, pos))
    {
        if(evts[pos] & RREF)
        {
            evt_size first = evts[pos + 1];
            evt_size last = evts[pos + 2];
            fill_visit_seq_fwd(evts, evts_sz, first, last, seq);
        }
        else
        {
            seq->push_back(pos);
        }
        if(evts[pos] == ESTR)
            break;
    }
}

void verify_visit_seq_fwd(evt_bits const* evts, evt_size evts_sz,
                          evt_size const* expected, evt_size expected_sz,
                          evt_size begpos, evt_size endpos,
                          evt_size &count)
{
    RYML_TRACE_FMT("verify_visit_seq_fwd({},{},{})", begpos, endpos, count);
    ASSERT_GT(evts_sz, 1);
    ASSERT_GT(endpos, 0);
    ASSERT_NE(endpos, begpos);
    ASSERT_LT(begpos, endpos);
    ASSERT_LT(begpos, evts_sz);
    ASSERT_LT(endpos, evts_sz);
    for(evt_size pos = begpos; pos != endpos; pos = nextpos(evts, pos))
    {
        RYML_TRACE_FMT("pos={} count={}", pos, count);
        ASSERT_LT(pos, evts_sz);
        ASSERT_LT(count, expected_sz);
        if(evts[pos] & RREF)
        {
            RYML_TRACE_FMT("RREF @ pos={}", pos);
            ASSERT_LT(pos + 2, evts_sz);
            evt_size first = evts[pos + 1];
            evt_size last = evts[pos + 2];
            verify_visit_seq_fwd(evts, evts_sz, expected, expected_sz, first, last, count);
        }
        else
        {
            ASSERT_EQ(pos, expected[count]);
            ++count;
        }
        if(evts[pos] == ESTR)
            break;
    }
}

void verify_visit_seq_bwd(evt_bits const* evts, evt_size evts_sz,
                          evt_size const* expected, evt_size expected_sz,
                          evt_size begpos, evt_size endpos,
                          evt_size &count)
{
    RYML_TRACE_FMT("verify_visit_seq_bwd({},{},{})", begpos, endpos, count);
    ASSERT_GT(evts_sz, 1);
    ASSERT_GT(endpos, 0);
    ASSERT_LT(begpos, evts_sz);
    ASSERT_LT(endpos, evts_sz);
    evt_size const* end = expected + (expected_sz - 1);
    for(evt_size pos = endpos; true; pos = prevpos(evts, pos))
    {
        RYML_TRACE_FMT("pos={} count={}", pos, count);
        ASSERT_LT(pos, evts_sz);
        ASSERT_LT(count, expected_sz);
        if(evts[pos] & RREF)
        {
            RYML_TRACE_FMT("RREF @ pos={}", pos);
            evt_size first = evts[pos + 1];
            evt_size last = evts[pos + 2];
            ASSERT_LE(first, last);
            last = prevpos(evts, last);
            verify_visit_seq_bwd(evts, evts_sz, expected, expected_sz, first, last, count);
        }
        else
        {
            ASSERT_EQ(pos, end[count]);
            --count;
        }
        if(evts[pos] == BSTR)
            break;
        if(pos == begpos)
            break;
    }
}

void print_arr(evt_size const* evts, evt_size sz)
{
    printf("{");
    for(evt_size i = 0; i < sz; ++i)
        printf("%d,", evts[i]);
    printf("}");
}
} // namespace


void verify_visit_seq(evt_bits const* evts, evt_size evtsz,
                      evt_size const* expected, evt_size expsz)
{
    SCOPED_TRACE("verify_visit_seq");
    ASSERT_GE(evtsz, 2);
    evt_size pos_estr = evtsz - 1;
    while(evts[pos_estr] != ievt::ESTR && evts[pos_estr] != ievt::BSTR)
        pos_estr = ievt::prevpos(evts, pos_estr);
    ASSERT_EQ(evts[pos_estr], ievt::ESTR);
    {
        SCOPED_TRACE("fwd");
        ievt::evt_size count_fwd = 0;
        verify_visit_seq_fwd(evts, evtsz, expected, expsz, 0, pos_estr, count_fwd);
    }
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("bwd");
        ievt::evt_size count_bwd = 0;
        verify_visit_seq_bwd(evts, evtsz, expected, expsz, 0, pos_estr, count_bwd);
    }
    if(testing::Test::HasFailure())
    {
        printf("%s:%d: see arr:\n", __FILE__, __LINE__);
        print_arr(expected, expsz);
        printf("\n");
    }
}

void verify_visit_seq(evt_bits const* evts, evt_size evtsz)
{
    SCOPED_TRACE("verify_visit_seq");
    std::vector<evt_size> seq;
    seq.reserve(static_cast<size_t>(evtsz));
    fill_visit_seq_fwd(evts, evtsz, 0, evtsz, &seq);
    verify_visit_seq(evts, evtsz, seq.data(), static_cast<evt_size>(seq.size()));
}


//-----------------------------------------------------------------------------
// invariants


namespace {
#define style_scalar (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD)
#define style_container_flow (ievt::FSL_|ievt::FMLX|ievt::FSPC)
#define scope (ievt::MAP_|ievt::SEQ_|ievt::DOC_|ievt::STRM|ievt::BEG_|ievt::END_)
#define directives (ievt::YAML|ievt::TAGH|ievt::TAGP)

template<class Fn>
void iter_children(evt_bits const* evts, evt_size sz, evt_size pos, Fn const& fn)
{
    const evt_size close = detail::find_matching_close_(evts, sz, pos);
    ++pos;
    while(pos != close)
    {
        RYML_TRACE_FMT("child={}", pos);
        fn(pos, evts[pos]);
        if(testing::Test::HasFailure())
            break;
        pos = detail::find_next_sibling(evts, sz, pos);
    }
}

std::string showflags(csubstr lhs_name, evt_bits lhs,
                      csubstr rhs_name, evt_bits rhs)
{
    char lhsbuf[200];
    char rhsbuf[200];
    return formatrs<std::string>("{}={}({})  vs {}={}({})",
                                 lhs_name, ievt::to_str_sub(lhsbuf, lhs), c4::fmt::hex(lhs),
                                 rhs_name, ievt::to_str_sub(rhsbuf, rhs), c4::fmt::hex(rhs));
}
bool is(evt_bits evt, evt_bits bits) { return evt == bits; }
bool hasall(evt_bits evt, evt_bits bits) { return (evt & bits) == bits; }
bool hassome(evt_bits evt, evt_bits bits) { return (evt & bits) != 0; }
bool hasnone(evt_bits evt, evt_bits bits) { return (evt & bits) == 0; }
bool hasone(evt_bits evt, evt_bits bits) { evt &= bits; return (evt != 0) && (evt & (evt - 1)) == 0; }
bool hasone_or_none(evt_bits evt, evt_bits bits) { return hasone(evt, bits) || hasnone(evt, bits); }
} // namespace


void test_events_ints_invariants(csubstr parsed_yaml,
                                 csubstr arena,
                                 evt_bits const* evts,
                                 evt_bits evts_sz)
{
    if(!evts_sz)
        return;
    SCOPED_TRACE("ints_invariants");
    ASSERT_GE(evts_sz, 2);
    EXPECT_EQ(evts[0], ievt::BSTR);
    if(evts_sz > 2) // if there is a BDOC, then there must be an EDOC
    {
        evt_size docstart = 1;
        while(evts[docstart] & (ievt::TAGH|ievt::TAGP|ievt::YAML))
            docstart = nextpos(evts, docstart);
        RYML_TRACE_FMT("docstart={}", docstart);
        EXPECT_GE(evts_sz, docstart + 2); // EDOC,
        EXPECT_TRUE(detail::hasall(evts[docstart], ievt::BDOC));
    }
    if(testing::Test::HasFailure())
        return;
    std::vector<evt_size> path;
    path.reserve(32);
    for(evt_bits evtpos = 0, evtnumber = 0;
        true;
        ++evtnumber,
            evtpos = nextpos(evts, evtpos))
    {
        bool ok = true;
        evt_bits evt = evts[evtpos];
        evt_bits prev = {};
        evt_bits nextpos = ievt::nextpos(evts, evtpos);
        evt_bits next = {};
        evt_bits parentpos = path.empty() ? -1 : path.back();
        evt_bits parent = path.empty() ? evt_bits{} : evts[parentpos];
        if(evtpos)
            prev = (evt & ievt::PEX2) ? evts[evtpos - 3] : evts[evtpos - 1];
        if(nextpos < evts_sz)
            next = evts[nextpos];
        RYML_TRACE_FMT("evt #={} evtpos={} parentpos={}", evtnumber, evtpos, parentpos); \
        if(evt & ievt::BEG_)
        {
            path.push_back(evtpos);
        }
        if(evt & ievt::END_)
        {
            ASSERT_TRUE(!path.empty());
            path.pop_back();
        }
        #define EXPECT_(cond, evt, bits)                            \
            if(!(cond))                                             \
            {                                                       \
                SCOPED_TRACE(showflags(#evt, evt, #bits, bits));    \
                EXPECT_TRUE(cond);                                  \
                ok = false;                                         \
            }
        #define EXPECT_IS(evt, bits) EXPECT_(is(evt, bits), evt, bits) // NOLINT
        #define EXPECT_NONE(evt, bits) EXPECT_(hasnone(evt, bits), evt, bits) // NOLINT
        #define EXPECT_SOME(evt, bits) EXPECT_(hassome(evt, bits), evt, bits) // NOLINT
        #define EXPECT_ONE(evt, bits) EXPECT_(hasone(evt, bits), evt, bits) // NOLINT
        #define EXPECT_ALL(evt, bits) EXPECT_(hasall(evt, bits), evt, bits) // NOLINT
        #define EXPECT_ONE_OR_NONE(evt, bits) EXPECT_(hasone_or_none(evt, bits), evt, bits) // NOLINT
        // check general rules
        if(evt & ievt::WSTR)
        {
            EXPECT_SOME(next, ievt::PEX2);
            EXPECT_LE(evtpos + 3, evts_sz);
            if(evtpos + 3 <= evts_sz)
            {
                bool in_arena = evts[evtpos] & ievt::AREN;
                csubstr buf = !in_arena ? parsed_yaml : arena;
                RYML_TRACE_FMT("in_arena={}", in_arena);
                EXPECT_GE(evts[evtpos + 1], 0);
                EXPECT_GE(evts[evtpos + 2], 0);
                if(evts[evtpos + 1] >= 0 &&
                   evts[evtpos + 2] >= 0)
                {
                    size_t offset = (size_t)evts[evtpos + 1];
                    size_t len = (size_t)evts[evtpos + 2];
                    EXPECT_LE(offset, buf.len);
                    EXPECT_LE(len, buf.len);
                    EXPECT_LE(offset + len, buf.len);
                }
            }
        }
        if(evt & ievt::PEX2)
        {
            EXPECT_GT(evtnumber, 0);
            EXPECT_GE(evtpos, 3);
            EXPECT_ONE(prev, ievt::EXT2);
        }
        if(evt & (ievt::BEG_|ievt::END_))
        {
            EXPECT_ONE(evt, ievt::BEG_|ievt::END_);
            EXPECT_ONE(evt, ievt::SEQ_|ievt::MAP_|ievt::DOC_|ievt::STRM);
            EXPECT_NONE(evt, ievt::SCLR|style_scalar|directives);
            EXPECT_NONE(evt, ievt::WSTR);
        }
        if(evt & (ievt::FLOW))
        {
            EXPECT_(hasall(evt, ievt::BEG_) || hasall(evt, ievt::EDOC), evt, ievt::BEG_);
            EXPECT_ONE(evt, ievt::DOC_|ievt::MAP_|ievt::SEQ_);
            EXPECT_NONE(evt, ievt::SCLR|style_scalar|directives);
            EXPECT_NONE(evt, ievt::WSTR);
        }
        if(evt & (ievt::AREN))
        {
            EXPECT_ONE(evt, ievt::WSTR);
            EXPECT_NONE(evt, directives|scope);
        }
        if(evt & (ievt::KEY_|ievt::VAL_))
        {
            EXPECT_ALL(parent, ievt::BEG_);
            EXPECT_ONE(parent, ievt::SEQ_|ievt::MAP_|ievt::DOC_);
            EXPECT_NONE(evt, ievt::DOC_|ievt::STRM|directives);
        }
        if(evt & ievt::KEY_)
        {
            EXPECT_NONE(parent, ievt::SEQ_|ievt::DOC_);
            EXPECT_ALL(parent, ievt::MAP_);
        }
        if(evt & ievt::FLOW)
        {
            EXPECT_ONE(evt, ievt::MAP_|ievt::SEQ_|ievt::DOC_);
            if(evt & (ievt::MAP_|ievt::SEQ_))
            {
                EXPECT_ALL(evt, ievt::BEG_);
                EXPECT_NONE(evt, ievt::END_);
            }
            if(evt & ievt::DOC_)
            {
                EXPECT_ONE(evt, ievt::BEG_|ievt::END_);
            }
        }
        if(evt & (ievt::FSL_|ievt::FML1|ievt::FMLN|ievt::FMLX|ievt::FSPC))
        {
            EXPECT_ALL(evt, ievt::FLOW|ievt::BEG_);
            EXPECT_NONE(evt, ievt::END_);
            EXPECT_ONE(evt, ievt::MAP_|ievt::SEQ_);
        }
        // now check each flag
        if(evt & ievt::YAML)
        {
            EXPECT_EQ(parentpos, 0);
            EXPECT_ALL(parent, ievt::BSTR);
            EXPECT_NONE(evt, ievt::BSTR|ievt::ESTR|ievt::EXPL);
            EXPECT_ONE(evt, directives);
            EXPECT_NONE(evt, ievt::SCLR);
            EXPECT_NONE(evt, ievt::ALIA);
            EXPECT_NONE(evt, ievt::ANCH);
            EXPECT_NONE(evt, ievt::TAG_);
            EXPECT_NONE(evt, ievt::KEY_);
            EXPECT_NONE(evt, ievt::VAL_);
            EXPECT_NONE(evt, (ievt::BDOC|ievt::EDOC));
            EXPECT_NONE(evt, (ievt::BMAP|ievt::EMAP));
            EXPECT_NONE(evt, (ievt::BSEQ|ievt::ESEQ));
            EXPECT_NONE(evt, (ievt::FLOW|ievt::FSL_|ievt::FMLX|ievt::FSPC));
            EXPECT_NONE(evt, (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD));
            EXPECT_ALL(next, ievt::PEX2);
        }
        if(evt & ievt::TAGH)
        {
            EXPECT_EQ(parent & ievt::BSTR, ievt::BSTR);
            EXPECT_EQ(parentpos, 0);
            EXPECT_NONE(evt, ievt::YAML);
            EXPECT_NONE(evt, ievt::TAGP);
            EXPECT_NONE(evt, ievt::BSTR);
            EXPECT_NONE(evt, ievt::ESTR);
            EXPECT_NONE(evt, ievt::EXPL);
            EXPECT_EQ(evt & ievt::WSTR, ievt::TAGH);
            EXPECT_NONE(evt, ievt::SCLR);
            EXPECT_NONE(evt, ievt::ALIA);
            EXPECT_NONE(evt, ievt::ANCH);
            EXPECT_NONE(evt, ievt::TAG_);
            EXPECT_NONE(evt, ievt::KEY_);
            EXPECT_NONE(evt, ievt::VAL_);
            EXPECT_NONE(evt, (ievt::BDOC|ievt::EDOC));
            EXPECT_NONE(evt, (ievt::BMAP|ievt::EMAP));
            EXPECT_NONE(evt, (ievt::BSEQ|ievt::ESEQ));
            EXPECT_NONE(evt, (ievt::FLOW|ievt::FSL_|ievt::FMLX|ievt::FSPC));
            EXPECT_NONE(evt, (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD));
            EXPECT_EQ(next & ievt::PEX2, ievt::PEX2);
            EXPECT_EQ(next & ievt::TAGP, ievt::TAGP);
        }
        if(evt & ievt::TAGP)
        {
            EXPECT_EQ(parent & ievt::BSTR, ievt::BSTR);
            EXPECT_EQ(parentpos, 0);
            EXPECT_NONE(evt, ievt::YAML);
            EXPECT_NONE(evt, ievt::TAGH);
            EXPECT_NONE(evt, ievt::BSTR);
            EXPECT_NONE(evt, ievt::ESTR);
            EXPECT_NONE(evt, ievt::EXPL);
            EXPECT_EQ(evt & ievt::WSTR, ievt::TAGP);
            EXPECT_NONE(evt, ievt::SCLR);
            EXPECT_NONE(evt, ievt::ALIA);
            EXPECT_NONE(evt, ievt::ANCH);
            EXPECT_NONE(evt, ievt::TAG_);
            EXPECT_NONE(evt, ievt::KEY_);
            EXPECT_NONE(evt, ievt::VAL_);
            EXPECT_NONE(evt, (ievt::BDOC|ievt::EDOC));
            EXPECT_NONE(evt, (ievt::BMAP|ievt::EMAP));
            EXPECT_NONE(evt, (ievt::BSEQ|ievt::ESEQ));
            EXPECT_NONE(evt, (ievt::FLOW|ievt::FSL_|ievt::FMLX|ievt::FSPC));
            EXPECT_NONE(evt, (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD));
            EXPECT_EQ(next & ievt::PEX2, ievt::PEX2);
            EXPECT_EQ(prev & ievt::TAGH, ievt::TAGH);
        }
        if(hasall(evt, ievt::BSTR))
        {
            EXPECT_IS(evt, ievt::BSTR);
            EXPECT_NONE(evt, ievt::END_);
            EXPECT_NONE(evt, ievt::EXPL);
            EXPECT_NONE(evt, ievt::WSTR);
            EXPECT_NONE(evt, ievt::SCLR);
            EXPECT_NONE(evt, ievt::ALIA);
            EXPECT_NONE(evt, ievt::ANCH);
            EXPECT_NONE(evt, ievt::TAG_);
            EXPECT_NONE(evt, ievt::KEY_);
            EXPECT_NONE(evt, ievt::VAL_);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), ievt::BEG_);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), ievt::BEG_);
            EXPECT_EQ(evt & (ievt::BSEQ|ievt::ESEQ), ievt::BEG_);
            EXPECT_NONE(evt, (ievt::FLOW|ievt::FSL_|ievt::FMLX|ievt::FSPC));
            EXPECT_NONE(evt, (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD));
            EXPECT_NONE(next, ievt::PEX2);
            iter_children(evts, evts_sz, evtpos, [&](evt_size, evt_bits child_evt){
                EXPECT_ONE(child_evt, ievt::DOC_|ievt::YAML|ievt::TAGH|ievt::TAGP);
                EXPECT_NONE(child_evt, ievt::SEQ_|ievt::MAP_|ievt::TAG_|ievt::ANCH|ievt::ALIA);
            });
        }
        if(hasall(evt, ievt::ESTR))
        {
            EXPECT_IS(evt, ievt::ESTR);
            EXPECT_EQ(evt & ievt::BSTR, ievt::STRM);
            EXPECT_NONE(evt, ievt::EXPL);
            EXPECT_NONE(evt, ievt::WSTR);
            EXPECT_NONE(evt, ievt::SCLR);
            EXPECT_NONE(evt, ievt::ALIA);
            EXPECT_NONE(evt, ievt::ANCH);
            EXPECT_NONE(evt, ievt::TAG_);
            EXPECT_NONE(evt, ievt::KEY_);
            EXPECT_NONE(evt, ievt::VAL_);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), ievt::END_);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), ievt::END_);
            EXPECT_EQ(evt & (ievt::BSEQ|ievt::ESEQ), ievt::END_);
            EXPECT_NONE(evt, (ievt::FLOW|ievt::FSL_|ievt::FMLX|ievt::FSPC));
            EXPECT_NONE(evt, (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD));
        }
        if((evt & ievt::BDOC) == ievt::BDOC)
        {
            EXPECT_EQ(parentpos, 0);
            EXPECT_EQ(parent & ievt::BSTR, ievt::BSTR);
            EXPECT_EQ(evt & ievt::EDOC, ievt::DOC_);
            EXPECT_NONE(evt, ievt::WSTR);
            EXPECT_NONE(evt, ievt::SCLR);
            EXPECT_NONE(evt, ievt::ALIA);
            EXPECT_NONE(evt, ievt::ANCH);
            EXPECT_NONE(evt, ievt::TAG_);
            EXPECT_NONE(evt, ievt::KEY_);
            EXPECT_NONE(evt, ievt::VAL_);
            EXPECT_EQ(evt & (ievt::BSTR|ievt::ESTR), ievt::BEG_);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), ievt::BEG_);
            EXPECT_EQ(evt & (ievt::BSEQ|ievt::ESEQ), ievt::BEG_);
            EXPECT_NONE(evt, (ievt::FSL_|ievt::FMLX|ievt::FSPC));
            EXPECT_NONE(evt, (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD));
            EXPECT_NONE(next, ievt::PEX2);
            iter_children(evts, evts_sz, evtpos, [&](evt_size, evt_bits child_evt){
                if(child_evt & (~ievt::VAL_))
                {
                    EXPECT_NE(child_evt & (ievt::SEQ_|ievt::MAP_|ievt::SCLR|ievt::TAG_|ievt::ANCH|ievt::ALIA), 0);
                }
            });
        }
        if((evt & ievt::EDOC) == ievt::EDOC)
        {
            EXPECT_EQ(parent & ievt::BDOC, ievt::BDOC);
            EXPECT_EQ(evt & ievt::BDOC, ievt::DOC_);
            EXPECT_NONE(evt, ievt::WSTR);
            EXPECT_NONE(evt, ievt::SCLR);
            EXPECT_NONE(evt, ievt::ALIA);
            EXPECT_NONE(evt, ievt::ANCH);
            EXPECT_NONE(evt, ievt::TAG_);
            EXPECT_NONE(evt, ievt::KEY_);
            EXPECT_NONE(evt, ievt::VAL_);
            EXPECT_EQ(evt & (ievt::BSTR|ievt::ESTR), ievt::END_);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), ievt::END_);
            EXPECT_EQ(evt & (ievt::BSEQ|ievt::ESEQ), ievt::END_);
            EXPECT_NONE(evt, (ievt::FSL_|ievt::FMLX|ievt::FSPC));
            EXPECT_NONE(evt, (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD));
            EXPECT_NONE(next, ievt::PEX2);
        }
        if((evt & ievt::BSEQ) == ievt::BSEQ)
        {
            EXPECT_EQ(evt & ievt::ESEQ, ievt::SEQ_);
            EXPECT_NONE(evt, ievt::EMAP);
            EXPECT_NONE(evt, ievt::WSTR);
            EXPECT_NONE(evt, ievt::SCLR);
            EXPECT_NONE(evt, ievt::ALIA);
            EXPECT_NONE(evt, ievt::ANCH);
            EXPECT_NONE(evt, ievt::TAG_);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), ievt::BEG_);
            EXPECT_EQ(evt & (ievt::BSTR|ievt::ESTR), ievt::BEG_);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), ievt::BEG_);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), 0);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), ievt::KEY_|ievt::VAL_);
            EXPECT_NONE(evt, (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD));
            EXPECT_NONE(next, ievt::PEX2);
            iter_children(evts, evts_sz, evtpos, [&](evt_size, evt_bits child_evt){
                //EXPECT_NE(child_evt & (ievt::SEQ_|ievt::MAP_|ievt::SCLR|ievt::TAG_|ievt::ANCH|ievt::ALIA|ievt::RREF), 0);
                EXPECT_NONE(child_evt, ievt::KEY_);
            });
        }
        if((evt & ievt::ESEQ) == ievt::ESEQ)
        {
            EXPECT_EQ(evt & ievt::BSEQ, ievt::SEQ_);
            EXPECT_NONE(evt, ievt::WSTR);
            EXPECT_NONE(evt, ievt::SCLR);
            EXPECT_NONE(evt, ievt::ALIA);
            EXPECT_NONE(evt, ievt::ANCH);
            EXPECT_NONE(evt, ievt::TAG_);
            EXPECT_NONE(evt, ievt::KEY_);
            EXPECT_NONE(evt, ievt::VAL_);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), ievt::END_);
            EXPECT_EQ(evt & (ievt::BSTR|ievt::ESTR), ievt::END_);
            EXPECT_EQ(evt & (ievt::BMAP|ievt::EMAP), ievt::END_);
            EXPECT_NONE(evt, (ievt::FLOW));
            EXPECT_NONE(evt, (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD));
            EXPECT_NONE(next, ievt::PEX2);
        }
        if((evt & ievt::BMAP) == ievt::BMAP)
        {
            EXPECT_EQ(evt & ievt::EMAP, ievt::MAP_);
            EXPECT_NONE(evt, ievt::WSTR);
            EXPECT_NONE(evt, ievt::SCLR);
            EXPECT_NONE(evt, ievt::ALIA);
            EXPECT_NONE(evt, ievt::ANCH);
            EXPECT_NONE(evt, ievt::TAG_);
            EXPECT_EQ(evt & (ievt::BDOC|ievt::EDOC), ievt::BEG_);
            EXPECT_EQ(evt & (ievt::BSTR|ievt::ESTR), ievt::BEG_);
            EXPECT_EQ(evt & (ievt::BSEQ|ievt::ESEQ), ievt::BEG_);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), 0);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), ievt::KEY_|ievt::VAL_);
            EXPECT_NONE(evt, (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD));
            EXPECT_NONE(next, ievt::PEX2);
            bool key_state = true;
            bool has_rref = false;
            iter_children(evts, evts_sz, evtpos, [&](evt_size, evt_bits child_evt){
                if(child_evt & ievt::RREF)
                    has_rref = true;
                if(has_rref || !child_evt)
                    return;
                if(child_evt & (ievt::SEQ_|ievt::MAP_|ievt::SCLR|ievt::ALIA))
                {
                    if(key_state)
                    {
                        EXPECT_EQ(child_evt & ievt::KEY_, ievt::KEY_);
                        EXPECT_NONE(child_evt, ievt::VAL_);
                    }
                    else
                    {
                        EXPECT_NONE(child_evt, ievt::KEY_);
                        EXPECT_EQ(child_evt & ievt::VAL_, ievt::VAL_);
                    }
                    key_state = !key_state;
                }
            });
        }
        if((evt & ievt::EMAP) == ievt::EMAP)
        {
            EXPECT_NONE(evt, ievt::BEG_);
            EXPECT_NONE(evt, ievt::EXPL|ievt::WSTR|ievt::SCLR|ievt::ALIA|ievt::ANCH|ievt::TAG_|ievt::KEY_|ievt::VAL_);
            EXPECT_NONE(evt, ievt::FLOW);
            EXPECT_NONE(evt, style_scalar|directives);
            EXPECT_NONE(evt, ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD);
            EXPECT_NONE(next, ievt::PEX2);
        }
        if(evt & ievt::SCLR)
        {
            EXPECT_NONE(evt, ievt::EXPL);
            EXPECT_NE(evt & ievt::WSTR, 0);
            EXPECT_NONE(evt, ievt::ALIA);
            EXPECT_NONE(evt, ievt::ANCH);
            EXPECT_NONE(evt, ievt::TAG_);
            EXPECT_ONE(evt, ievt::KEY_|ievt::VAL_);
            EXPECT_NONE(evt, (ievt::BSTR|ievt::ESTR));
            EXPECT_NONE(evt, (ievt::BDOC|ievt::EDOC));
            EXPECT_NONE(evt, (ievt::BSEQ|ievt::ESEQ));
            EXPECT_NONE(evt, (ievt::BMAP|ievt::EMAP));
            EXPECT_NONE(evt, (ievt::FLOW|ievt::FSL_|ievt::FMLX|ievt::FSPC));
            EXPECT_EQ(next & ievt::PEX2, ievt::PEX2);
            EXPECT_ONE_OR_NONE(evt, style_scalar);
        }
        if(evt & ievt::ALIA)
        {
            EXPECT_NONE(evt, ievt::EXPL);
            EXPECT_NE(evt & ievt::WSTR, 0);
            EXPECT_NONE(evt, ievt::SCLR);
            EXPECT_NONE(evt, ievt::ANCH);
            EXPECT_NONE(evt, ievt::TAG_);
            EXPECT_ONE(evt, ievt::KEY_|ievt::VAL_);
            EXPECT_NONE(evt, (ievt::BSTR|ievt::ESTR));
            EXPECT_NONE(evt, (ievt::BDOC|ievt::EDOC));
            EXPECT_NONE(evt, (ievt::BSEQ|ievt::ESEQ));
            EXPECT_NONE(evt, (ievt::BMAP|ievt::EMAP));
            EXPECT_NONE(evt, (ievt::FLOW|ievt::FSL_|ievt::FMLX|ievt::FSPC));
            EXPECT_NONE(evt, (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD));
            EXPECT_EQ(next & ievt::PEX2, ievt::PEX2);
        }
        if(evt & ievt::ANCH)
        {
            EXPECT_NONE(evt, ievt::EXPL);
            EXPECT_NE(evt & ievt::WSTR, 0);
            EXPECT_NONE(evt, ievt::SCLR);
            EXPECT_NONE(evt, ievt::ALIA);
            EXPECT_NONE(evt, ievt::TAG_);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), 0);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), ievt::KEY_|ievt::VAL_);
            EXPECT_NONE(evt, (ievt::BSTR|ievt::ESTR));
            EXPECT_NONE(evt, (ievt::BDOC|ievt::EDOC));
            EXPECT_NONE(evt, (ievt::BSEQ|ievt::ESEQ));
            EXPECT_NONE(evt, (ievt::BMAP|ievt::EMAP));
            EXPECT_NONE(evt, (ievt::FLOW|ievt::FSL_|ievt::FMLX|ievt::FSPC));
            EXPECT_NONE(evt, (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD));
            EXPECT_EQ(next & ievt::PEX2, ievt::PEX2);
            // must be followed either by an entry or by TAG_ then entry
            evt_size entrypos = evtpos + ((next & ievt::TAG_) ? 6 : 3);
            RYML_TRACE_FMT("evtpos={} next_has_tag={} entrypos={}", evtpos, next & ievt::TAG_, entrypos);
            ASSERT_LT(entrypos, evts_sz);
            EXPECT_TRUE(detail::isentry(evts[entrypos]));
            EXPECT_FALSE(evts[entrypos] & ievt::ALIA);
        }
        if(evt & ievt::TAG_)
        {
            EXPECT_NONE(evt, ievt::EXPL);
            EXPECT_NE(evt & ievt::WSTR, 0);
            EXPECT_NONE(evt, ievt::SCLR);
            EXPECT_NONE(evt, ievt::ALIA);
            EXPECT_NONE(evt, ievt::ANCH);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), 0);
            EXPECT_NE(evt & (ievt::KEY_|ievt::VAL_), ievt::KEY_|ievt::VAL_);
            EXPECT_NONE(evt, (ievt::BSTR|ievt::ESTR));
            EXPECT_NONE(evt, (ievt::BDOC|ievt::EDOC));
            EXPECT_NONE(evt, (ievt::BSEQ|ievt::ESEQ));
            EXPECT_NONE(evt, (ievt::BMAP|ievt::EMAP));
            EXPECT_NONE(evt, (ievt::FLOW|ievt::FSL_|ievt::FMLX|ievt::FSPC));
            EXPECT_NONE(evt, (ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD));
            EXPECT_EQ(next & ievt::PEX2, ievt::PEX2);
            // must be followed by an entry, never by an ANCH
            evt_size entrypos = evtpos + 3;
            RYML_TRACE_FMT("entrypos={}", entrypos);
            ASSERT_LT(entrypos, evts_sz);
            EXPECT_TRUE(detail::hasnone(evts[entrypos], ievt::ANCH));
            EXPECT_TRUE(detail::isentry(evts[entrypos]));
        }
        if(!ok || detail::hasall(evt, ievt::ESTR))
            break;
    }
}

} // namespace ievt
} // namespace extra
} // namespace yml
} // namespace c4

// NOLINTEND(hicpp-signed-bitwise)
