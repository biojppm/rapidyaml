#include "test_lib/test_case.hpp"
#include "test_lib/test_events_ints_helpers.hpp"
#include <c4/yml/extra/event_handler_ints.hpp>

RYML_DEFINE_TEST_MAIN()

// NOLINTBEGIN(hicpp-signed-bitwise)

namespace c4 {
namespace yml {
namespace extra {


TEST(flags, to_chars)
{
    using namespace ievt;
    char buf1_[1]; substr buf1 = buf1_;
    char buf_[200]; substr buf = buf_;
    #define _(flags, str)                                   \
    {                                                       \
        ievt::evt_bits flags_{flags};                       \
        csubstr actual(str);                                \
        EXPECT_EQ(ievt::to_str(buf1, flags_), actual.len);  \
        size_t ret = ievt::to_str(buf, flags_);             \
        ASSERT_LE(ret, buf.len);                            \
        EXPECT_EQ(buf.first(ret), actual);                  \
        buf.fill(0);                                        \
        csubstr r = ievt::to_str_sub(buf, flags_);          \
        ASSERT_LE(r.len, buf.len);                          \
        EXPECT_EQ(r, actual);                               \
    }
    _(0, "NONE");
    _(KEY_, "KEY_");
    _(VAL_, "VAL_");
    _(KEY_|VAL_, "KEY_|VAL_");
    #undef _
}

TEST(ints, find_matching_open_close_next_entry)
{
    #define CHECK_MATCHING_OPEN_CLOSE(open, close) \
        do {                                                            \
            ASSERT_GE(open, 0);                                         \
            ASSERT_LT(open, ints.evts.len);                             \
            EXPECT_EQ(open, ievt::detail::find_matching_open_(ints.evts.ptr, close)); \
            ASSERT_GE(close, 0);                                        \
            ASSERT_LT(close, ints.evts.len);                            \
            EXPECT_EQ(close, ievt::detail::find_matching_close_(ints.evts.ptr, ints.evts.len, open)); \
        } while(0)
    ievt::TestBuffers ints;
    std::string src = "[{a: b}: [c, d]]: {{e: f}: [g: h]}";
    parse_ints(to_substr(src), &ints);
    CHECK_MATCHING_OPEN_CLOSE(0, 45);
    CHECK_MATCHING_OPEN_CLOSE(1, 44);
    CHECK_MATCHING_OPEN_CLOSE(2, 43);
    CHECK_MATCHING_OPEN_CLOSE(3, 22);
    CHECK_MATCHING_OPEN_CLOSE(4, 21);
    CHECK_MATCHING_OPEN_CLOSE(5, 12);
    CHECK_MATCHING_OPEN_CLOSE(13, 20);
    CHECK_MATCHING_OPEN_CLOSE(23, 42);
    CHECK_MATCHING_OPEN_CLOSE(24, 31);
    CHECK_MATCHING_OPEN_CLOSE(32, 41);
    CHECK_MATCHING_OPEN_CLOSE(33, 40);
    if(testing::Test::HasFailure())
        ints.print();
    #undef CHECK_MATCHING_OPEN_CLOSE
}

TEST(ints, grow_evts)
{
    using Handler = ievt::EventHandlerInts<true>;
    auto test_grow_evts = [&](ievt::evt_size cap, csubstr yaml, csubstr expected){
        RYML_TRACE_FMT("cap={}", cap);
        if(!expected.len && yaml.len)
            expected = yaml;
        std::string src(yaml.str, yaml.len);
        Handler handler;
        ParseEngine<Handler> parser(&handler);
        handler.reset(to_substr(src));
        ASSERT_EQ(handler.m_evt.cap, 0);
        handler._grow_evts_exact(cap);
        ASSERT_EQ(handler.m_evt.cap, cap);
        parser.parse_in_place_ev("(testyaml)", to_substr(src));
        ievt::evt_size finalcap = handler.m_evt.cap;
        ASSERT_LE(handler.m_evt.len, handler.m_evt.cap);
        bool ok = true;
        if(handler.m_evt.len <= cap)
        {
            ok = false;
            EXPECT_GT(handler.m_evt.cap, cap);
        }
        ievt::TestBuffers ints;
        handler.get_buffers(&ints, true);
        ASSERT_EQ(ints.evts.cap, finalcap);
        std::string actual = ints.emitrs_yaml<std::string>();
        if(expected != actual)
        {
            RYML_TRACE_FMT("yaml=~~~{}~~~", yaml);
            EXPECT_EQ(expected, actual);
            ok = false;
        }
        if(!ok)
            ints.print();
    };
    #define test_grow_evts_(...) { SCOPED_TRACE("call"); test_grow_evts(__VA_ARGS__); }
    test_grow_evts_(1, "a\n", {});
    test_grow_evts_(1, "[a]", {});
    test_grow_evts_(2, "[a]", {});
    test_grow_evts_(3, "[a,b]", {});
    test_grow_evts_(3, "[a: b]", "[{a: b}]");
    test_grow_evts_(6, "[a: b]", "[{a: b}]");
    test_grow_evts_(7, "[a: b]", "[{a: b}]");
    test_grow_evts_(3, "[[a,b]: {c: d}]", "[{? [a,b]: {c: d}}]");
    test_grow_evts_(6, "[[a,b]: {c: d}]", "[{? [a,b]: {c: d}}]");
    test_grow_evts_(11, "[[a,b]: {c: d}]", "[{? [a,b]: {c: d}}]");
}


TEST(ints, grow_arena)
{
    using Handler = ievt::EventHandlerInts<true>;
    {
        Handler handler;
        ASSERT_EQ(handler.m_arena.len, 0);
        ASSERT_EQ(handler.m_arena.str, nullptr);
        ASSERT_EQ(handler.arena(), handler.m_arena);
    }
    {
        Handler handler = {};
        ASSERT_EQ(handler.m_arena.len, 0);
        ASSERT_EQ(handler.m_arena.str, nullptr);
        ASSERT_EQ(handler.arena(), handler.m_arena);
    }
    Handler handler;
    ASSERT_EQ(handler.arena().len, 0);
    std::string sofar;
    #define grow_arena(s_)                              \
        do                                              \
        {                                               \
            SCOPED_TRACE("grow_arena");                 \
            csubstr s = to_csubstr(s_);                 \
            size_t lenprev = handler.arena().len;       \
            ASSERT_EQ(sofar, handler.arena());          \
            substr ret = handler.alloc_arena(s.len);    \
            ASSERT_EQ(ret.len, s.len);                  \
            ASSERT_EQ(handler.arena().len, lenprev + s.len); \
            sofar.append(s.str, s.len);                 \
            if(s.len)                                   \
                memcpy(ret.str, s.str, s.len);          \
            ASSERT_EQ(sofar, handler.arena());          \
       } while(0)
    ASSERT_EQ(handler.m_arena.len, 0);
    grow_arena("0123");
    ASSERT_EQ(handler.m_arena.len, 256);
    grow_arena("456789");
    ASSERT_EQ(handler.m_arena.len, 256);
    std::string growit;
    growit.assign(256, '-');
    ASSERT_EQ(growit.size(), 256);
    grow_arena(growit);
    ASSERT_GT(handler.m_arena.len, 256);
    #undef grow_arena
}



void verify_visit_seq(ievt::evt_bits const* evts, ievt::evt_size evts_sz,
                      ievt::evt_size const* expected, ievt::evt_size expected_sz,
                      ievt::evt_size begpos, ievt::evt_size endpos,
                      ievt::evt_size &count_fwd, ievt::evt_size &count_bwd)
{
    RYML_TRACE_FMT("verify_visit_seq({},{},{})", begpos, endpos, count_fwd, count_bwd);
    ASSERT_GT(evts_sz, 1);
    ASSERT_GT(endpos, 0);
    ASSERT_NE(endpos, begpos);
    ASSERT_LT(begpos, endpos);
    ASSERT_LT(begpos, evts_sz);
    ASSERT_LT(endpos, evts_sz);
    {
        ievt::evt_size pos = begpos;
        while(pos != endpos)
        {
            RYML_TRACE_FMT("pos={} count={}", pos, count_fwd);
            ASSERT_LT(pos, evts_sz);
            ASSERT_LT(count_fwd, expected_sz);
            if(evts[pos] == ievt::ESTR)
                break;
            if(evts[pos] & ievt::RREF)
            {
                RYML_TRACE_FMT("RREF @ pos={}", pos);
                ASSERT_LT(pos + 2, evts_sz);
                ievt::evt_size first = evts[pos + 1];
                ievt::evt_size last = evts[pos + 2];
                verify_visit_seq(evts, evts_sz, expected, expected_sz, first, last, count_fwd, count_bwd);
            }
            else
            {
                ASSERT_EQ(pos, expected[count_fwd]);
                ++count_fwd;
            }
            pos = ievt::nextpos(evts, pos);
        }
    }
}
void verify_visit_seq(cspan<ievt::evt_bits> evts, cspan<ievt::evt_size> expected)
{
    ievt::evt_size count_fwd = 0;
    ievt::evt_size count_bwd = 0;
    auto sz = (ievt::evt_size)evts.size();
    verify_visit_seq(evts.data(), sz,
                     expected.data(), (ievt::evt_size)expected.size(),
                     0, (sz-1),
                     count_fwd, count_bwd);
}
void verify_visit_seq(ievt::evtbuf evts, cspan<ievt::evt_size> expected)
{
    auto evts_ = cspan<ievt::evt_bits>{evts.ptr, (size_t)evts.len};
    verify_visit_seq(evts_, expected);
}

void tweak_evts(ievt::TestBuffers * ints,
                ievt::TestBuffers const& actual,
                ievt::evt_bits const* evts, ievt::evt_size sz)
{
    ints->destroy();
    *ints = {};
    ints->src = actual.src;
    ints->arena = actual.arena;
    ints->evts.ptr = const_cast<ievt::evt_size*>(evts); // NOLINT
    ints->evts.len = sz;
    ints->owned = false;
}
void tweak_evts(ievt::TestBuffers * ints, ievt::TestBuffers const& actual, cspan<ievt::evt_size> evts)
{
    tweak_evts(ints, actual, evts.data(), (ievt::evt_size)evts.size());
}
ievt::TestBuffers tweak_evts(ievt::TestBuffers const& actual, cspan<ievt::evt_size> evts)
{
    ievt::TestBuffers ints;
    tweak_evts(&ints, actual, evts.data(), (ievt::evt_size)evts.size());
    return ints;
}

void test_ints_resolve_(std::string & src,
                        std::string const& emitted,
                        std::string const& resolved,
                        cspan<ievt::evt_bits> evts_expected,
                        cspan<ievt::evt_bits> evts_resolved,
                        cspan<ievt::evt_size> pos_fwd={},
                        cspan<ievt::evt_size> pos_fwd_resolved={})
{
    SCOPED_TRACE("test ints resolve");
    using namespace ievt;
    TestBuffers actual;
    parse_ints(to_substr(src), &actual);
    {
        SCOPED_TRACE("unresolved");
        if(!pos_fwd.empty())
        {
            verify_visit_seq(evts_expected, pos_fwd);
            verify_visit_seq(actual.evts, pos_fwd);
        }
        TestBuffers expected = tweak_evts(actual, evts_expected);
        actual.test_compare(expected);
        EXPECT_EQ(emitted, actual.emitrs_yaml<std::string>());
        EXPECT_EQ(expected.emitrs_yaml<std::string>(), actual.emitrs_yaml<std::string>());
    }
    if(testing::Test::HasFailure())
    {
        actual.print();
        return;
    }
    {
        SCOPED_TRACE("resolved");
        if(!pos_fwd_resolved.empty())
        {
            verify_visit_seq(evts_resolved, pos_fwd_resolved);
        }
        TestBuffers expected = tweak_evts(actual, evts_resolved);
        actual.resolve();
        actual.test_compare(expected);
        EXPECT_EQ(resolved, actual.emitrs_yaml<std::string>());
        EXPECT_EQ(expected.emitrs_yaml<std::string>(), actual.emitrs_yaml<std::string>());
    }
    if(testing::Test::HasFailure())
    {
        actual.print();
    }
}
#define test_ints_resolve(...) \
    {                          \
        SCOPED_TRACE("call");  \
        test_ints_resolve_(__VA_ARGS__);         \
    }

TEST(ints, resolve_refs_1_to_scalar)
{
    using namespace ievt;
    std::string src = "- &anc foo\n- *anc\n";
    std::string resolved = "- foo\n- foo\n";
    const evt_size pos[] = {0,1,2,3,6,9,12,13,14};
    const evt_bits evts_expected[] = {
        BSTR,
        BDOC,
        VAL_|BSEQ,
        VAL_|ANCH,            3, 3,
        VAL_|SCLR|PLAI|PSTR,  7, 3,
        VAL_|ALIA|PSTR,      14, 3,
        ESEQ|PSTR,
        EDOC,
        ESTR,
    };
    const evt_bits evts_resolved[] = {
        BSTR,
        BDOC,
        VAL_|BSEQ,
        VAL_, 0, 0,
        VAL_|SCLR|PLAI,  7, 3,
        VAL_|RREF|PSTR,  6, 9, // this was resolved
        ESEQ|PRREF,
        EDOC,
        ESTR,
    };
    const evt_size pos_resolved[] = {0,1,2,3,4,5,6,6,12,13,14};
    test_ints_resolve(src, src, resolved, evts_expected, evts_resolved, pos, pos_resolved);
}

TEST(ints, resolve_refs_2_to_container)
{
    using namespace ievt;
    std::string src = "- &foo [0,1,2,3]\n- *foo\n";
    std::string resolved = "- [0,1,2,3]\n- [0,1,2,3]\n";
    const evt_bits evts_expected[] = {
        /* 0*/BSTR,
        /* 1*/BDOC,
        /* 2*/VAL_|BSEQ,
        /* 3*/VAL_|ANCH,            3, 3,
        /* 6*/VAL_|BSEQ|FLOW|FSL_|PSTR,
        /* 7*/VAL_|SCLR|PLAI,       8, 1,
        /*10*/VAL_|SCLR|PLAI|PSTR, 10, 1,
        /*13*/VAL_|SCLR|PLAI|PSTR, 12, 1,
        /*16*/VAL_|SCLR|PLAI|PSTR, 14, 1,
        /*19*/ESEQ|PSTR,
        /*20*/VAL_|ALIA,           20, 3,
        /*23*/ESEQ|PSTR,
        /*24*/EDOC,
        /*25*/ESTR,
    };
    const evt_size pos[] = {0,1,2,3,6,7,10,13,16,19,20,23,24,25};
    const evt_bits evts_resolved[] = {
        /* 0*/BSTR,
        /* 1*/BDOC,
        /* 2*/VAL_|BSEQ,
        /* 3*/VAL_, 0, 0,
        /* 6*/VAL_|BSEQ|FLOW|FSL_,
        /* 7*/VAL_|SCLR|PLAI,       8, 1,
        /*10*/VAL_|SCLR|PLAI|PSTR, 10, 1,
        /*13*/VAL_|SCLR|PLAI|PSTR, 12, 1,
        /*16*/VAL_|SCLR|PLAI|PSTR, 14, 1,
        /*19*/ESEQ|PSTR,
        /*20*/VAL_|RREF,            6,20, // this was resolved
        /*23*/ESEQ|PRREF,
        /*24*/EDOC,
        /*25*/ESTR,
    };
    const evt_size pos_resolved[] = {0,1,2,3,4,5,6,7,10,13,16,19,6,7,10,13,16,19,23,24,25};
    test_ints_resolve(src, src, resolved, evts_expected, evts_resolved, pos, pos_resolved);
}


TEST(ints, resolve_refs_3_key_vs_seq)
{
    using namespace ievt;
    std::string src = ""
        "&keyscalar key: &valscalar val\n"
        "&keyseq [a, b]: &valseq [c, d]\n"
        "&keymap {e: f}: &valmap {g: h}\n"
        "*valscalar : *keyscalar\n"
        "*valseq : *keyseq\n"
        "*valmap : *keymap\n"
        "";
    std::string emitted = ""
        "&keyscalar key: &valscalar val\n"
        "? &keyseq [a,b]\n"
        ": &valseq [c,d]\n"
        "? &keymap {e: f}\n"
        ": &valmap {g: h}\n"
        "*valscalar : *keyscalar\n"
        "*valseq : *keyseq\n"
        "*valmap : *keymap\n"
        "";
    std::string resolved = ""
        "key: val\n"
        "? [a,b]\n"
        ": [c,d]\n"
        "? {e: f}\n"
        ": {g: h}\n"
        "val: key\n"
        "? [c,d]\n"
        ": [a,b]\n"
        "? {g: h}\n"
        ": {e: f}\n"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|ANCH, 1,9,  /*keyscalar*/
        /*[4][6]*/       KEY_|SCLR|PLAI|PSTR, 11,3,  /*key*/
        /*[5][9]*/       VAL_|ANCH|PSTR, 17,9,  /*valscalar*/
        /*[6][12]*/       VAL_|SCLR|PLAI|PSTR, 27,3,  /*val*/
        /*[7][15]*/       KEY_|ANCH|PSTR, 32,6,  /*keyseq*/
        /*[8][18]*/       KEY_|BSEQ|FLOW|FSL_|PSTR,
        /*[9][19]*/         VAL_|SCLR|PLAI, 40,1,  /*a*/
        /*[10][22]*/         VAL_|SCLR|PLAI|PSTR, 43,1,  /*b*/
        /*[11][25]*/       ESEQ|PSTR,
        /*[12][26]*/       VAL_|ANCH, 48,6,  /*valseq*/
        /*[13][29]*/       VAL_|BSEQ|FLOW|FSL_|PSTR,
        /*[14][30]*/         VAL_|SCLR|PLAI, 56,1,  /*c*/
        /*[15][33]*/         VAL_|SCLR|PLAI|PSTR, 59,1,  /*d*/
        /*[16][36]*/       ESEQ|PSTR,
        /*[17][37]*/       KEY_|ANCH, 63,6,  /*keymap*/
        /*[18][40]*/       KEY_|BMAP|FLOW|FSL_|PSTR,
        /*[19][41]*/         KEY_|SCLR|PLAI, 71,1,  /*e*/
        /*[20][44]*/         VAL_|SCLR|PLAI|PSTR, 74,1,  /*f*/
        /*[21][47]*/       EMAP|PSTR,
        /*[22][48]*/       VAL_|ANCH, 79,6,  /*valmap*/
        /*[23][51]*/       VAL_|BMAP|FLOW|FSL_|PSTR,
        /*[24][52]*/         KEY_|SCLR|PLAI, 87,1,  /*g*/
        /*[25][55]*/         VAL_|SCLR|PLAI|PSTR, 90,1,  /*h*/
        /*[26][58]*/       EMAP|PSTR,
        /*[27][59]*/       KEY_|ALIA, 94,9,  /*valscalar*/
        /*[28][62]*/       VAL_|ALIA|PSTR, 107,9,  /*keyscalar*/
        /*[29][65]*/       KEY_|ALIA|PSTR, 118,6,  /*valseq*/
        /*[30][68]*/       VAL_|ALIA|PSTR, 128,6,  /*keyseq*/
        /*[31][71]*/       KEY_|ALIA|PSTR, 136,6,  /*valmap*/
        /*[32][74]*/       VAL_|ALIA|PSTR, 146,6,  /*keymap*/
        /*[33][77]*/     EMAP|PSTR,
        /*[34][78]*/   EDOC,
        /*[35][79]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_,0,0,  /*keyscalar*/
        /*[4][6]*/       KEY_|SCLR|PLAI, 11,3,  /*key*/
        /*[5][9]*/       VAL_|PSTR, 0,0,  /*valscalar*/
        /*[6][12]*/       VAL_|SCLR|PLAI, 27,3,  /*val*/
        /*[7][15]*/       KEY_|PSTR, 0,0,  /*keyseq*/
        /*[8][18]*/       KEY_|BSEQ|FLOW|FSL_,
        /*[9][19]*/         VAL_|SCLR|PLAI, 40,1,  /*a*/
        /*[10][22]*/         VAL_|SCLR|PLAI|PSTR, 43,1,  /*b*/
        /*[11][25]*/       ESEQ|PSTR,
        /*[12][26]*/       VAL_, 0,0,  /*valseq*/
        /*[13][29]*/       VAL_|BSEQ|FLOW|FSL_,
        /*[14][30]*/         VAL_|SCLR|PLAI, 56,1,  /*c*/
        /*[15][33]*/         VAL_|SCLR|PLAI|PSTR, 59,1,  /*d*/
        /*[16][36]*/       ESEQ|PSTR,
        /*[17][37]*/       KEY_, 0,0,  /*keymap*/
        /*[18][40]*/       KEY_|BMAP|FLOW|FSL_,
        /*[19][41]*/         KEY_|SCLR|PLAI, 71,1,  /*e*/
        /*[20][44]*/         VAL_|SCLR|PLAI|PSTR, 74,1,  /*f*/
        /*[21][47]*/       EMAP|PSTR,
        /*[22][48]*/       VAL_, 0,0,  /*valmap*/
        /*[23][51]*/       VAL_|BMAP|FLOW|FSL_,
        /*[24][52]*/         KEY_|SCLR|PLAI, 87,1,  /*g*/
        /*[25][55]*/         VAL_|SCLR|PLAI|PSTR, 90,1,  /*h*/
        /*[26][58]*/       EMAP|PSTR,
        /*[27][59]*/       KEY_|RREF, 12,15,  /*valscalar*/
        /*[28][62]*/       VAL_|RREF|PRREF, 6,9,  /*keyscalar*/
        /*[29][65]*/       KEY_|RREF|PRREF, 29,37,  /*valseq*/
        /*[30][68]*/       VAL_|RREF|PRREF, 18,26,  /*keyseq*/
        /*[31][71]*/       KEY_|RREF|PRREF, 51,59,  /*valmap*/
        /*[32][74]*/       VAL_|RREF|PRREF, 40,48,  /*keymap*/
        /*[33][77]*/     EMAP|PRREF,
        /*[34][78]*/   EDOC,
        /*[35][79]*/ ESTR,
    };
    test_ints_resolve(src, emitted, resolved, evts_expected, evts_resolved);
}

TEST(ints, resolve_refs_4_inherit)
{
    using namespace ievt;
    std::string src = ""
        "map0: &map0\n"
        "  key0: val0\n"
        "  key1: val1\n"
        "inherit:\n"
        "  <<: *map0\n"
        "";
    std::string resolved = ""
        "map0:\n"
        "  key0: val0\n"
        "  key1: val1\n"
        "inherit:\n"
        "  key0: val0\n"
        "  key1: val1\n"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4, /*map0*/
        /*[4][6]*/       VAL_|ANCH|PSTR, 7,4, /*map0*/
        /*[5][9]*/       VAL_|BMAP|PSTR,
        /*[6][10]*/         KEY_|SCLR|PLAI, 14,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PSTR, 20,4, /*val0*/
        /*[8][16]*/         KEY_|SCLR|PLAI|PSTR, 27,4, /*key1*/
        /*[9][19]*/         VAL_|SCLR|PLAI|PSTR, 33,4, /*val1*/
        /*[10][22]*/       EMAP|PSTR,
        /*[11][23]*/       KEY_|SCLR|PLAI, 38,7, /*inherit*/
        /*[12][26]*/       VAL_|BMAP|PSTR,
        /*[13][27]*/         KEY_|SCLR|PLAI, 49,2, /*<<*/
        /*[14][30]*/         VAL_|ALIA|PSTR, 54,4, /*map0*/
        /*[15][33]*/       EMAP|PSTR,
        /*[16][34]*/     EMAP,
        /*[17][35]*/   EDOC,
        /*[18][36]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4, /*map0*/
        /*[4][6]*/       VAL_|PSTR, 0,0, /*map0*/
        /*[5][9]*/       VAL_|BMAP,
        /*[6][10]*/         KEY_|SCLR|PLAI, 14,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PSTR, 20,4, /*val0*/
        /*[8][16]*/         KEY_|SCLR|PLAI|PSTR, 27,4, /*key1*/
        /*[9][19]*/         VAL_|SCLR|PLAI|PSTR, 33,4, /*val1*/
        /*[10][22]*/       EMAP|PSTR,
        /*[11][23]*/       KEY_|SCLR|PLAI, 38,7, /*inherit*/
        /*[12][26]*/       VAL_|BMAP|PSTR,
        /*[13][27]*/         KEY_,0,0, /*<<*/
        /*[14][30]*/         VAL_|RREF, 10,22, /*map0*/
        /*[15][33]*/       EMAP|PRREF,
        /*[16][34]*/     EMAP,
        /*[17][35]*/   EDOC,
        /*[18][36]*/ ESTR,
    };
    test_ints_resolve(src, src, resolved, evts_expected, evts_resolved);
}

TEST(ints, resolve_refs_5_inherit_seq)
{
    using namespace ievt;
    std::string src = ""
        "map0: &map0\n"
        "  key0: val0\n"
        "map1: &map1\n"
        "  key1: val1\n"
        "map2: &map2\n"
        "  key2: val2\n"
        "inherit:\n"
        "  <<: [*map0,*map1,*map2]\n"
        "";
    std::string resolved = ""
        "map0:\n"
        "  key0: val0\n"
        "map1:\n"
        "  key1: val1\n"
        "map2:\n"
        "  key2: val2\n"
        "inherit:\n"
        "  key0: val0\n"
        "  key1: val1\n"
        "  key2: val2\n"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4, /*map0*/
        /*[4][6]*/       VAL_|ANCH|PSTR, 7,4, /*map0*/
        /*[5][9]*/       VAL_|BMAP|PSTR,
        /*[6][10]*/         KEY_|SCLR|PLAI, 14,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PSTR, 20,4, /*val0*/
        /*[8][16]*/       EMAP|PSTR,
        /*[9][17]*/       KEY_|SCLR|PLAI, 25,4, /*map1*/
        /*[10][20]*/       VAL_|ANCH|PSTR, 32,4, /*map1*/
        /*[11][23]*/       VAL_|BMAP|PSTR,
        /*[12][24]*/         KEY_|SCLR|PLAI, 39,4, /*key1*/
        /*[13][27]*/         VAL_|SCLR|PLAI|PSTR, 45,4, /*val1*/
        /*[14][30]*/       EMAP|PSTR,
        /*[15][31]*/       KEY_|SCLR|PLAI, 50,4, /*map2*/
        /*[16][34]*/       VAL_|ANCH|PSTR, 57,4, /*map2*/
        /*[17][37]*/       VAL_|BMAP|PSTR,
        /*[18][38]*/         KEY_|SCLR|PLAI, 64,4, /*key2*/
        /*[19][41]*/         VAL_|SCLR|PLAI|PSTR, 70,4, /*val2*/
        /*[20][44]*/       EMAP|PSTR,
        /*[21][45]*/       KEY_|SCLR|PLAI, 75,7, /*inherit*/
        /*[22][48]*/       VAL_|BMAP|PSTR,
        /*[23][49]*/         KEY_|SCLR|PLAI, 86,2, /*<<*/
        /*[24][52]*/         VAL_|BSEQ|FLOW|FSL_|PSTR,
        /*[25][53]*/           VAL_|ALIA, 92,4, /*map0*/
        /*[26][56]*/           VAL_|ALIA|PSTR, 98,4, /*map1*/
        /*[27][59]*/           VAL_|ALIA|PSTR, 104,4, /*map2*/
        /*[28][62]*/         ESEQ|PSTR,
        /*[29][63]*/       EMAP,
        /*[30][64]*/     EMAP,
        /*[31][65]*/   EDOC,
        /*[32][66]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4, /*map0*/
        /*[4][6]*/       VAL_|PSTR, 0,0, /*map0*/
        /*[5][9]*/       VAL_|BMAP,
        /*[6][10]*/         KEY_|SCLR|PLAI, 14,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PSTR, 20,4, /*val0*/
        /*[8][16]*/       EMAP|PSTR,
        /*[9][17]*/       KEY_|SCLR|PLAI, 25,4, /*map1*/
        /*[10][20]*/       VAL_|PSTR, 0,0, /*map1*/
        /*[11][23]*/       VAL_|BMAP,
        /*[12][24]*/         KEY_|SCLR|PLAI, 39,4, /*key1*/
        /*[13][27]*/         VAL_|SCLR|PLAI|PSTR, 45,4, /*val1*/
        /*[14][30]*/       EMAP|PSTR,
        /*[15][31]*/       KEY_|SCLR|PLAI, 50,4, /*map2*/
        /*[16][34]*/       VAL_|PSTR, 0,0, /*map2*/
        /*[17][37]*/       VAL_|BMAP,
        /*[18][38]*/         KEY_|SCLR|PLAI, 64,4, /*key2*/
        /*[19][41]*/         VAL_|SCLR|PLAI|PSTR, 70,4, /*val2*/
        /*[20][44]*/       EMAP|PSTR,
        /*[21][45]*/       KEY_|SCLR|PLAI, 75,7, /*inherit*/
        /*[22][48]*/       VAL_|BMAP|PSTR,
        /*[23][49]*/         KEY_, 0,0, /*<<*/
        /*[24][52]*/         VAL_,
        /*[25][53]*/         VAL_|RREF, 10,16, /*map0*/
        /*[26][56]*/         VAL_|RREF|PRREF, 24,30, /*map1*/
        /*[27][59]*/         VAL_|RREF|PRREF, 38,44, /*map2*/
        /*[28][62]*/         PRREF,
        /*[29][63]*/       EMAP,
        /*[30][64]*/     EMAP,
        /*[31][65]*/   EDOC,
        /*[32][66]*/ ESTR,
    };
    test_ints_resolve(src, src, resolved, evts_expected, evts_resolved);
}

TEST(ints, resolve_refs_6_inherit_seq_with_more_members)
{
    using namespace ievt;
    std::string src = ""
        "map0: &map0\n"
        "  key0: val0\n"
        "map1: &map1\n"
        "  key1: val1\n"
        "map2: &map2\n"
        "  key2: val2\n"
        "inherit:\n"
        "  <<: [*map0,*map1,*map2]\n"
        "another: member\n"
        "";
    std::string resolved = ""
        "map0:\n"
        "  key0: val0\n"
        "map1:\n"
        "  key1: val1\n"
        "map2:\n"
        "  key2: val2\n"
        "inherit:\n"
        "  key0: val0\n"
        "  key1: val1\n"
        "  key2: val2\n"
        "another: member\n"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4, /*map0*/
        /*[4][6]*/       VAL_|ANCH|PSTR, 7,4, /*map0*/
        /*[5][9]*/       VAL_|BMAP|PSTR,
        /*[6][10]*/         KEY_|SCLR|PLAI, 14,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PSTR, 20,4, /*val0*/
        /*[8][16]*/       EMAP|PSTR,
        /*[9][17]*/       KEY_|SCLR|PLAI, 25,4, /*map1*/
        /*[10][20]*/       VAL_|ANCH|PSTR, 32,4, /*map1*/
        /*[11][23]*/       VAL_|BMAP|PSTR,
        /*[12][24]*/         KEY_|SCLR|PLAI, 39,4, /*key1*/
        /*[13][27]*/         VAL_|SCLR|PLAI|PSTR, 45,4, /*val1*/
        /*[14][30]*/       EMAP|PSTR,
        /*[15][31]*/       KEY_|SCLR|PLAI, 50,4, /*map2*/
        /*[16][34]*/       VAL_|ANCH|PSTR, 57,4, /*map2*/
        /*[17][37]*/       VAL_|BMAP|PSTR,
        /*[18][38]*/         KEY_|SCLR|PLAI, 64,4, /*key2*/
        /*[19][41]*/         VAL_|SCLR|PLAI|PSTR, 70,4, /*val2*/
        /*[20][44]*/       EMAP|PSTR,
        /*[21][45]*/       KEY_|SCLR|PLAI, 75,7, /*inherit*/
        /*[22][48]*/       VAL_|BMAP|PSTR,
        /*[23][49]*/         KEY_|SCLR|PLAI, 86,2, /*<<*/
        /*[24][52]*/         VAL_|BSEQ|FLOW|FSL_|PSTR,
        /*[25][53]*/           VAL_|ALIA, 92,4, /*map0*/
        /*[26][56]*/           VAL_|ALIA|PSTR, 98,4, /*map1*/
        /*[27][59]*/           VAL_|ALIA|PSTR, 104,4, /*map2*/
        /*[28][62]*/         ESEQ|PSTR,
        /*[29][63]*/       EMAP,
        /*[30][64]*/       KEY_|SCLR|PLAI, 110,7, /*another*/
        /*[31][67]*/       VAL_|SCLR|PLAI|PSTR, 119,6, /*member*/
        /*[32][70]*/     EMAP|PSTR,
        /*[33][71]*/   EDOC,
        /*[34][72]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4, /*map0*/
        /*[4][6]*/       VAL_|PSTR, 0,0, /*map0*/
        /*[5][9]*/       VAL_|BMAP,
        /*[6][10]*/         KEY_|SCLR|PLAI, 14,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PSTR, 20,4, /*val0*/
        /*[8][16]*/       EMAP|PSTR,
        /*[9][17]*/       KEY_|SCLR|PLAI, 25,4, /*map1*/
        /*[10][20]*/       VAL_|PSTR, 0,0, /*map1*/
        /*[11][23]*/       VAL_|BMAP,
        /*[12][24]*/         KEY_|SCLR|PLAI, 39,4, /*key1*/
        /*[13][27]*/         VAL_|SCLR|PLAI|PSTR, 45,4, /*val1*/
        /*[14][30]*/       EMAP|PSTR,
        /*[15][31]*/       KEY_|SCLR|PLAI, 50,4, /*map2*/
        /*[16][34]*/       VAL_|PSTR, 0,0, /*map2*/
        /*[17][37]*/       VAL_|BMAP,
        /*[18][38]*/         KEY_|SCLR|PLAI, 64,4, /*key2*/
        /*[19][41]*/         VAL_|SCLR|PLAI|PSTR, 70,4, /*val2*/
        /*[20][44]*/       EMAP|PSTR,
        /*[21][45]*/       KEY_|SCLR|PLAI, 75,7, /*inherit*/
        /*[22][48]*/       VAL_|BMAP|PSTR,
        /*[23][49]*/         KEY_, 0,0, /*<<*/
        /*[24][52]*/         VAL_,
        /*[25][53]*/         VAL_|RREF, 10,16, /*map0*/
        /*[26][56]*/         VAL_|RREF|PRREF, 24,30, /*map1*/
        /*[27][59]*/         VAL_|RREF|PRREF, 38,44, /*map2*/
        /*[28][62]*/         PRREF,
        /*[29][63]*/       EMAP,
        /*[30][64]*/       KEY_|SCLR|PLAI, 110,7, /*another*/
        /*[31][67]*/       VAL_|SCLR|PLAI|PSTR, 119,6, /*member*/
        /*[32][70]*/     EMAP|PSTR,
        /*[33][71]*/   EDOC,
        /*[34][72]*/ ESTR,
    };
    test_ints_resolve(src, src, resolved, evts_expected, evts_resolved);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

struct IntEventsCase
{
    const char *file;
    const int line;
    ParserOptions opts;
    csubstr yaml;
    const std::vector<ievt::IntEventWithScalar> evt;

    void testeq(csubstr parsed_source, csubstr arena, ievt::evt_bits const* actual, size_t actual_size) const
    {
        RYML_TRACE_FMT("defined in:\n{}:{}: (here)\n", file, line);
        extra::ievt::test_events_ints_invariants(parsed_source, arena, actual, (ievt::evt_bits)actual_size);
        test_events_ints(evt.data(), evt.size(), actual, actual_size, yaml, parsed_source, arena);
    }
};
// this is required to work around a valgrind problem in gtest's
// printing of the test byte contents. We use the opportunity to print
// a line showing the location where the test case was defined.
std::ostream& operator<<(std::ostream& os, const IntEventsCase& e)
{
    return os << "line[" << e.line << "]:\n" << e.file << ":" << e.line << ": (here)";
}


//-----------------------------------------------------------------------------
#define DECLARE_CSUBSTR_FROM_CHAR_ARR(name, ...) \
    const char name##_[] = { __VA_ARGS__ }; \
    csubstr name = {name##_, C4_COUNTOF(name##_)}

DECLARE_CSUBSTR_FROM_CHAR_ARR(dqesc_L6,
         RYML_CHCONST_(-0x1e, 0xe2), RYML_CHCONST_(-0x80, 0x80), RYML_CHCONST_(-0x58, 0xa8),
         RYML_CHCONST_(-0x1e, 0xe2), RYML_CHCONST_(-0x80, 0x80), RYML_CHCONST_(-0x58, 0xa8),
         RYML_CHCONST_(-0x1e, 0xe2), RYML_CHCONST_(-0x80, 0x80), RYML_CHCONST_(-0x58, 0xa8),
         RYML_CHCONST_(-0x1e, 0xe2), RYML_CHCONST_(-0x80, 0x80), RYML_CHCONST_(-0x58, 0xa8),
         RYML_CHCONST_(-0x1e, 0xe2), RYML_CHCONST_(-0x80, 0x80), RYML_CHCONST_(-0x58, 0xa8),
         RYML_CHCONST_(-0x1e, 0xe2), RYML_CHCONST_(-0x80, 0x80), RYML_CHCONST_(-0x58, 0xa8),
    );
DECLARE_CSUBSTR_FROM_CHAR_ARR(dqesc_P6,
         RYML_CHCONST_(-0x1e, 0xe2), RYML_CHCONST_(-0x80, 0x80), RYML_CHCONST_(-0x57, 0xa9),
         RYML_CHCONST_(-0x1e, 0xe2), RYML_CHCONST_(-0x80, 0x80), RYML_CHCONST_(-0x57, 0xa9),
         RYML_CHCONST_(-0x1e, 0xe2), RYML_CHCONST_(-0x80, 0x80), RYML_CHCONST_(-0x57, 0xa9),
         RYML_CHCONST_(-0x1e, 0xe2), RYML_CHCONST_(-0x80, 0x80), RYML_CHCONST_(-0x57, 0xa9),
         RYML_CHCONST_(-0x1e, 0xe2), RYML_CHCONST_(-0x80, 0x80), RYML_CHCONST_(-0x57, 0xa9),
         RYML_CHCONST_(-0x1e, 0xe2), RYML_CHCONST_(-0x80, 0x80), RYML_CHCONST_(-0x57, 0xa9),
    );

using namespace ievt;
const bool needs_filter = true;
const IntEventsCase test_cases[] = {
    // make the declarations shorter
    #define e(...) IntEventWithScalar{__VA_ARGS__}
    #define tc_(opts, ys, ...) IntEventsCase{__FILE__, __LINE__, opts.resolve_tags(true), ys, std::initializer_list<IntEventWithScalar>(__VA_ARGS__)}
    #define tc(ys, ...) tc_(ParserOptions{}.resolve_tags(true), ys, __VA_ARGS__)
    // case -------------------------------------------------
    tc("!yamlscript/v0/bare\n--- !code\n42\n",
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|TAG_, 0, 19, "!yamlscript/v0/bare"),
           e(VAL_|SCLR|PLAI|PSTR, 0, 0, ""),
           e(EDOC|PSTR),
           e(BDOC|EXPL),
           e(VAL_|TAG_, 24, 5, "!code"),
           e(VAL_|SCLR|PLAI|PSTR, 30, 2, "42"),
           e(EDOC|PSTR),
           e(ESTR),
       }),
    // case -------------------------------------------------
    tc("a: 1",
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BMAP),
           e(KEY_|SCLR|PLAI, 0, 1, "a"),
           e(VAL_|SCLR|PLAI|PSTR, 3, 1, "1"),
           e(EMAP|PSTR),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    tc("say: 2 + 2",
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BMAP),
           e(KEY_|SCLR|PLAI, 0, 3, "say"),
           e(VAL_|SCLR|PLAI|PSTR, 5, 5, "2 + 2"),
           e(EMAP|PSTR),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    tc("𝄞: ✅",
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BMAP),
           e(KEY_|SCLR|PLAI, 0, 4, "𝄞"),
           e(VAL_|SCLR|PLAI|PSTR, 6, 3, "✅"),
           e(EMAP|PSTR),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    tc("[a, b, c]",
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BSEQ|FLOW|FSL_),
           e(VAL_|SCLR|PLAI, 1, 1, "a"),
           e(VAL_|SCLR|PLAI|PSTR, 4, 1, "b"),
           e(VAL_|SCLR|PLAI|PSTR, 7, 1, "c"),
           e(ESEQ|PSTR),
           e(EDOC),
           e(ESTR),
       }),
    // case ------------------------------
    tc("[a: b]",
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BSEQ|FLOW|FSL_),
           e(VAL_|BMAP|FLOW|FSL_),
           e(KEY_|SCLR|PLAI, 1, 1, "a"),
           e(VAL_|SCLR|PLAI|PSTR, 4, 1, "b"),
           e(EMAP|PSTR),
           e(ESEQ),
           e(EDOC),
           e(ESTR),
       }),
    // case ------------------------------
    tc(""
       "--- !yamlscript/v0\n"
       "foo: !\n"
       "- {x: y}\n"
       "- [x, y]\n"
       "- foo\n"
       "- 'foo'\n"
       "- \"foo\"\n"
       "- |\n"
       "  foo\n"
       "- >\n"
       "  foo\n"
       "- [1, 2, true, false, null]\n"
       "- &anchor-1 !tag-1 foobar\n"
       "---\n"
       "another: doc\n"
       "",
       {
           e(BSTR),
           e(BDOC|EXPL),
           e(VAL_|TAG_, 4, 14, "!yamlscript/v0"),
           e(VAL_|BMAP|PSTR),
           e(KEY_|SCLR|PLAI, 19, 3, "foo"),
           e(VAL_|TAG_|PSTR, 24, 1, "!"),
           e(VAL_|BSEQ|PSTR),
           e(VAL_|BMAP|FLOW|FSL_),
           e(KEY_|SCLR|PLAI, 29, 1, "x"),
           e(VAL_|SCLR|PLAI|PSTR, 32, 1, "y"),
           e(EMAP|PSTR),
           e(VAL_|BSEQ|FLOW|FSL_),
           e(VAL_|SCLR|PLAI, 38, 1, "x"),
           e(VAL_|SCLR|PLAI|PSTR, 41, 1, "y"),
           e(ESEQ|PSTR),
           e(VAL_|SCLR|PLAI, 46, 3, "foo"),
           e(VAL_|SCLR|SQUO|PSTR, 53, 3, "foo"),
           e(VAL_|SCLR|DQUO|PSTR, 61, 3, "foo"),
           e(VAL_|SCLR|LITL|PSTR, 70, 4, "foo\n", needs_filter),
           e(VAL_|SCLR|FOLD|PSTR, 80, 4, "foo\n", needs_filter),
           e(VAL_|BSEQ|FLOW|FSL_|PSTR),
           e(VAL_|SCLR|PLAI, 89, 1, "1"),
           e(VAL_|SCLR|PLAI|PSTR, 92, 1, "2"),
           e(VAL_|SCLR|PLAI|PSTR, 95, 4, "true"),
           e(VAL_|SCLR|PLAI|PSTR, 101, 5, "false"),
           e(VAL_|SCLR|PLAI|PSTR, 108, 4, "null"),
           e(ESEQ|PSTR),
           e(VAL_|ANCH, 117, 8, "anchor-1"),
           e(VAL_|TAG_|PSTR, 126, 6, "!tag-1"),
           e(VAL_|SCLR|PLAI|PSTR, 133, 6, "foobar"),
           e(ESEQ|PSTR),
           e(EMAP),
           e(EDOC),
           e(BDOC|EXPL),
           e(VAL_|BMAP),
           e(KEY_|SCLR|PLAI, 144, 7, "another"),
           e(VAL_|SCLR|PLAI|PSTR, 153, 3, "doc"),
           e(EMAP|PSTR),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    tc("plain: well\n"
       "  a\n"
       "  b\n"
       "  c\n"
       "squo: 'single''quote'\n"
       "dquo: \"x\\t\\ny\"\n"
       "lit: |\n"
       "     X\n"
       "     Y\n"
       "     Z\n"
       "fold: >\n"
       "     U\n"
       "     V\n"
       "     W\n"
       ,
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BMAP),
           e(KEY_|SCLR|PLAI, 0, 5, "plain"),
           e(VAL_|SCLR|PLAI|PSTR, 7, 10, "well a b c"),
           e(KEY_|SCLR|PLAI|PSTR, 24, 4, "squo"),
           e(VAL_|SCLR|SQUO|PSTR, 31, 12, "single'quote", needs_filter),
           e(KEY_|SCLR|PLAI|PSTR, 46, 4, "dquo"),
           e(VAL_|SCLR|DQUO|PSTR, 53, 4, "x\t\ny", needs_filter),
           e(KEY_|SCLR|PLAI|PSTR, 61, 3, "lit"),
           e(VAL_|SCLR|LITL|PSTR, 68, 6, "X\nY\nZ\n", needs_filter),
           e(KEY_|SCLR|PLAI|PSTR, 89, 4, "fold"),
           e(VAL_|SCLR|FOLD|PSTR, 97, 6, "U V W\n", needs_filter),
           e(EMAP|PSTR),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    tc("- !!seq []",
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BSEQ),
           e(VAL_|TAG_, 2, 5, "!!seq"),
           e(VAL_|BSEQ|FLOW|FSL_|PSTR),
           e(ESEQ),
           e(ESEQ),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    tc(""
       "defn run(prompt session=nil):\n"
       "  when session:\n"
       "    write session _ :append true: |+\n"
       "      Q: $(orig-prompt:trim)\n"
       "      A ($api-model):\n"
       "      $(answer:trim)\n"
       ""
       ,
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BMAP),
           e(KEY_|SCLR|PLAI, 0, 28, "defn run(prompt session=nil)"),
           e(VAL_|BMAP|PSTR),
           e(KEY_|SCLR|PLAI, 32, 12, "when session"),
           e(VAL_|BMAP|PSTR),
           e(KEY_|SCLR|PLAI, 50, 28, "write session _ :append true"),
           e(VAL_|SCLR|LITL|PSTR, 83, 54, "Q: $(orig-prompt:trim)\nA ($api-model):\n$(answer:trim)\n", needs_filter),
           e(EMAP|PSTR),
           e(EMAP),
           e(EMAP),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    tc("#!/usr/bin/env ys-0\n"
       "\n"
       "defn run(prompt session=nil):\n"
       "  session-text =:\n"
       "    when session && session:fs-e:\n"
       "\n"
       "  answer =:\n"
       "    cond:\n"
       "      api-model =~ /^dall-e/:\n"
       "        openai-image(prompt).data.0.url\n"
       "      api-model.in?(anthropic-models):\n"
       "        anthropic(prompt):anthropic-message:format\n"
       "      api-model.in?(groq-models):\n"
       "        groq(prompt).choices.0.message.content:format\n"
       "      api-model.in?(openai-models):\n"
       "        openai-chat(prompt).choices.0.message.content:format\n"
       "      else: die()\n"
       "\n"
       "  say: answer\n"
       "\n"
       "  when session:\n"
       "    write session _ :append true: |+\n"
       "      Q: $(orig-prompt:trim)\n"
       "      A ($api-model):\n"
       "      $(answer:trim)\n"
       "\n"
       ,
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BMAP),
           e(KEY_|SCLR|PLAI, 21, 28, "defn run(prompt session=nil)"),
           e(VAL_|BMAP|PSTR),
           e(KEY_|SCLR|PLAI, 53, 14, "session-text ="),
           e(VAL_|BMAP|PSTR),
           e(KEY_|SCLR|PLAI, 73, 28, "when session && session:fs-e"),
           e(VAL_|SCLR|PLAI|PSTR, 0, 0, ""), // note empty scalar pointing at the front
           e(EMAP|PSTR),
           e(KEY_|SCLR|PLAI, 106, 8, "answer ="),
           e(VAL_|BMAP|PSTR),
           e(KEY_|SCLR|PLAI, 120, 4, "cond"),
           e(VAL_|BMAP|PSTR),
           e(KEY_|SCLR|PLAI, 132, 22, "api-model =~ /^dall-e/"),
           e(VAL_|SCLR|PLAI|PSTR, 164, 31, "openai-image(prompt).data.0.url"),
           e(KEY_|SCLR|PLAI|PSTR, 202, 31, "api-model.in?(anthropic-models)"),
           e(VAL_|SCLR|PLAI|PSTR, 243, 42, "anthropic(prompt):anthropic-message:format"),
           e(KEY_|SCLR|PLAI|PSTR, 292, 26, "api-model.in?(groq-models)"),
           e(VAL_|SCLR|PLAI|PSTR, 328, 45, "groq(prompt).choices.0.message.content:format"),
           e(KEY_|SCLR|PLAI|PSTR, 380, 28, "api-model.in?(openai-models)"),
           e(VAL_|SCLR|PLAI|PSTR, 418, 52, "openai-chat(prompt).choices.0.message.content:format"),
           e(KEY_|SCLR|PLAI|PSTR, 477, 4, "else"),
           e(VAL_|SCLR|PLAI|PSTR, 483, 5, "die()"),
           e(EMAP|PSTR),
           e(EMAP),
           e(KEY_|SCLR|PLAI, 492, 3, "say"),
           e(VAL_|SCLR|PLAI|PSTR, 497, 6, "answer"),
           e(KEY_|SCLR|PLAI|PSTR, 507, 12, "when session"),
           e(VAL_|BMAP|PSTR),
           e(KEY_|SCLR|PLAI, 525, 28, "write session _ :append true"),
           e(VAL_|SCLR|LITL|PSTR, 558, 55, "Q: $(orig-prompt:trim)\nA ($api-model):\n$(answer:trim)\n\n", needs_filter),
           e(EMAP|PSTR),
           e(EMAP),
           e(EMAP),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    tc(""
       "---\n"
       "plain: a\n"
       " b\n"
       "\n"
       " c\n"
       ,
       {
           e(BSTR),
           e(BDOC|EXPL),
           e(VAL_|BMAP),
           e(KEY_|SCLR|PLAI, 4, 5, "plain"),
           e(VAL_|SCLR|PLAI|PSTR, 11, 5, "a b\nc"),
           e(EMAP|PSTR),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    tc("{key: map}: [seq, val]"
       ,
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BMAP),
           e(KEY_|BMAP|FLOW|FSL_),
           e(KEY_|SCLR|PLAI, 1, 3, "key"),
           e(VAL_|SCLR|PLAI|PSTR, 6, 3, "map"),
           e(EMAP|PSTR),
           e(VAL_|BSEQ|FLOW|FSL_),
           e(VAL_|SCLR|PLAI, 13, 3, "seq"),
           e(VAL_|SCLR|PLAI|PSTR, 18, 3, "val"),
           e(ESEQ|PSTR),
           e(EMAP),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    tc("[key, seq]: {map: val}"
       ,
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BMAP),
           e(KEY_|BSEQ|FLOW|FSL_),
           e(VAL_|SCLR|PLAI, 1, 3, "key"),
           e(VAL_|SCLR|PLAI|PSTR, 6, 3, "seq"),
           e(ESEQ|PSTR),
           e(VAL_|BMAP|FLOW|FSL_),
           e(KEY_|SCLR|PLAI, 13, 3, "map"),
           e(VAL_|SCLR|PLAI|PSTR, 18, 3, "val"),
           e(EMAP|PSTR),
           e(EMAP),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    tc("[b,c]: d"
       ,
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BMAP),
           e(KEY_|BSEQ|FLOW|FSL_),
           e(VAL_|SCLR|PLAI, 1, 1, "b"),
           e(VAL_|SCLR|PLAI|PSTR, 3, 1, "c"),
           e(ESEQ|PSTR),
           e(VAL_|SCLR|PLAI, 7, 1, "d"),
           e(EMAP|PSTR),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    tc("[[b,c]]: d"
       ,
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BMAP),
           e(KEY_|BSEQ|FLOW|FSL_),
           e(VAL_|BSEQ|FLOW|FSL_),
           e(VAL_|SCLR|PLAI, 2, 1, "b"),
           e(VAL_|SCLR|PLAI|PSTR, 4, 1, "c"),
           e(ESEQ|PSTR),
           e(ESEQ),
           e(VAL_|SCLR|PLAI, 9, 1, "d"),
           e(EMAP|PSTR),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    tc("[ a, [ [[b,c]]: d, e]]: 23"
       ,
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BMAP),
           e(KEY_|BSEQ|FLOW|FSL_),
           e(VAL_|SCLR|PLAI, 2, 1, "a"),
           e(VAL_|BSEQ|FLOW|FSL_|PSTR),
           e(VAL_|BMAP|FLOW|FSL_),
           e(KEY_|BSEQ|FLOW|FSL_),
           e(VAL_|BSEQ|FLOW|FSL_),
           e(VAL_|SCLR|PLAI, 9, 1, "b"),
           e(VAL_|SCLR|PLAI|PSTR, 11, 1, "c"),
           e(ESEQ|PSTR),
           e(ESEQ),
           e(VAL_|SCLR|PLAI, 16, 1, "d"),
           e(EMAP|PSTR),
           e(VAL_|SCLR|PLAI, 19, 1, "e"),
           e(ESEQ|PSTR),
           e(ESEQ),
           e(VAL_|SCLR|PLAI, 24, 2, "23"),
           e(EMAP|PSTR),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    // this requires the arena:
    // every filtered \L or \P expands 2->3 bytes
    tc("[\"\\L\\L\\L\\L\\L\\L\", \"\\P\\P\\P\\P\\P\\P\"]",
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BSEQ|FLOW|FSL_),
           e(VAL_|SCLR|DQUO|AREN, 0, 18, dqesc_L6),
           e(VAL_|SCLR|DQUO|AREN|PSTR, 18, 18, dqesc_P6),
           e(ESEQ|PSTR),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    // this requires the arena: directives cause the tag to expand
    tc("%YAML 1.2\n"
       "---\n"
       "- !light fluorescent\n"
       "- !m something\n"
       "...\n"
       "%TAG !m! !extraordinary-\n"
       "---\n"
       "- !m!light green\n"
       "- !!int 2\n"
       ,
       {
           e(BSTR),
           e(YAML,                  6,  3, "1.2"),
           e(BDOC|EXPL|PSTR),
           e(VAL_|BSEQ),
           e(VAL_|TAG_,            16,  6, "!light"),
           e(VAL_|SCLR|PLAI|PSTR,  23, 11, "fluorescent"),
           e(VAL_|TAG_|PSTR,       37,  2, "!m"),
           e(VAL_|SCLR|PLAI|PSTR,  40,  9, "something"),
           e(ESEQ|PSTR),
           e(EDOC|EXPL),
           e(TAGH,                 59,  3, "!m!"),
           e(TAGP|PSTR,            63, 15, "!extraordinary-"),
           e(BDOC|EXPL|PSTR),
           e(VAL_|BSEQ),
           e(VAL_|TAG_|AREN,        0, 22, "<!extraordinary-light>"),
           e(VAL_|SCLR|PLAI|PSTR,  94,  5, "green"),
           e(VAL_|TAG_|PSTR,      102,  5, "!!int"),
           e(VAL_|SCLR|PLAI|PSTR, 108,  1, "2"),
           e(ESEQ|PSTR),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    tc("\"\\\"foo\\\"\": \"\\\"bar\\\"\"",
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BMAP),
           e(KEY_|SCLR|DQUO,       1, 5, "\"foo\""),
           e(VAL_|SCLR|DQUO|PSTR, 12, 5, "\"bar\""),
           e(EMAP|PSTR),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    tc_(ParserOptions{}.scalar_filtering(false),
       "\"\\\"foo\\\"\": \"\\\"bar\\\"\"",
       {
           e(BSTR),
           e(BDOC),
           e(VAL_|BMAP),
           e(KEY_|SCLR|DQUO|UNFILT,       1, 7, "\\\"foo\\\""),
           e(VAL_|SCLR|DQUO|UNFILT|PSTR, 12, 7, "\\\"bar\\\""),
           e(EMAP|PSTR),
           e(EDOC),
           e(ESTR),
       }),
    // case -------------------------------------------------
    // tests an extending scalar
    tc("|\n"
       "abc"  // no newline at end
       ,
       {
           e(BSTR),
           e(BDOC),
           // instead of this (in the arena):
           //e(VAL_|SCLR|LITL|AREN, 0, 4, "abc\n"), // result has additional newline
           // we get this (shifted left by 1, in the source code)
           e(VAL_|SCLR|LITL, 1, 4, "abc\n"), // result has additional newline
           e(EDOC|PSTR),
           e(ESTR),
       }),
    // case -------------------------------------------------
    // tests an extending scalar
    tc(">\n"
       "abc"  // no newline at end
       ,
       {
           e(BSTR),
           e(BDOC),
           // instead of this (in the arena):
           //e(VAL_|SCLR|FOLD|AREN, 0, 4, "abc\n"), // result has additional newline
           // we get this (shifted left by 1, in the source code)
           e(VAL_|SCLR|FOLD, 1, 4, "abc\n"), // result has additional newline
           e(EDOC|PSTR),
           e(ESTR),
       }),
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

constexpr bool dynamic_size = true;
constexpr bool fixed_size = false;

struct IntEventsTestHelper
{
    IntEventsCase const& ec;
    size_t required_size_expected;
    size_t required_size_actual;
    extra::ievt::EventHandlerInts<fixed_size> handler;
    ParseEngine<extra::ievt::EventHandlerInts<fixed_size>> parser;
    std::string src_copy;
    std::vector<evt_bits> actual;
    std::string arena;
    IntEventsTestHelper(IntEventsCase const& ec_)
        : ec(ec_)
        , required_size_expected(num_ints(ec.evt.data(), ec.evt.size()))
        , handler()
        , parser(&handler, ec_.opts)
        , src_copy()
        , actual()
    {
    }
    void run_with_size(size_t sz, size_t arena_sz)
    {
        src_copy.assign(ec.yaml.str, ec.yaml.len);
        actual.resize(sz);
        arena.resize(arena_sz);
        handler.reset(to_substr(src_copy), to_substr(arena), actual.data(), (int)actual.size());
        parser.parse_in_place_ev("(testyaml)", to_substr(src_copy));
        required_size_actual = (size_t)handler.required_size_events();
    }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

struct IntEventsTest : public testing::TestWithParam<IntEventsCase>
{
    // force an ascii name (some characters in the parameter are UTF8)
    static std::string name2str(const testing::TestParamInfo<ParamType>& info)
    {
        std::string s = c4::catrs<std::string>("line_", info.param.line);
        for (char &c : s)
            if (!std::isalnum(c))
                c = '_';
        return s;
    }
};

TEST_P(IntEventsTest, fixed_size_large_enough)
{
    IntEventsTestHelper h(GetParam());
    h.run_with_size(2u * h.required_size_expected, 2u * h.ec.yaml.len);
    ASSERT_LT(h.required_size_actual, h.actual.size());
    ASSERT_LT(h.handler.required_size_arena(), h.arena.size());
    ASSERT_TRUE(h.handler.fits_buffers());
    h.ec.testeq(to_csubstr(h.src_copy), to_csubstr(h.arena), h.actual.data(), h.required_size_actual);
}

TEST_P(IntEventsTest, fixed_size_too_small)
{
    IntEventsTestHelper h(GetParam());
    size_t small = h.required_size_expected / 2u;
    h.run_with_size(small, 0u);
    ASSERT_EQ(h.actual.size(), small);
    ASSERT_EQ(h.arena.size(), 0u);
    ASSERT_GT(h.required_size_actual, h.actual.size());
    ASSERT_FALSE(h.handler.fits_buffers());
    _c4dbgpf("retry! reqbuf={} reqarena={}", h.required_size_actual, h.handler.required_size_arena());
    h.run_with_size(h.required_size_actual, h.handler.required_size_arena());
    ASSERT_EQ(h.actual.size(), h.handler.required_size_events());
    ASSERT_EQ(h.arena.size(), h.handler.required_size_arena());
    ASSERT_TRUE(h.handler.fits_buffers());
    h.ec.testeq(to_csubstr(h.src_copy), to_csubstr(h.arena), h.actual.data(), h.required_size_actual);
}

TEST_P(IntEventsTest, fixed_size_null)
{
    IntEventsTestHelper h(GetParam());
    h.run_with_size(0, 0);
    ASSERT_GT(h.required_size_actual, h.actual.size());
    h.run_with_size(h.required_size_actual, h.handler.required_size_arena());
    ASSERT_TRUE(h.handler.fits_buffers());
    h.ec.testeq(to_csubstr(h.src_copy), to_csubstr(h.arena), h.actual.data(), h.required_size_actual);
}


void test_dynamic_size(IntEventsCase const& ec, bool transfer_ownership)
{
    extra::ievt::EventHandlerInts<dynamic_size> handler;
    ParseEngine<extra::ievt::EventHandlerInts<dynamic_size>> parser(&handler, ec.opts);
    std::vector<char> src(ec.yaml.begin(), ec.yaml.end());
    handler.reset(to_substr(src));
    parser.parse_in_place_ev("(testyaml)", to_substr(src));
    extra::ievt::Buffers buf;
    handler.get_buffers(&buf, transfer_ownership);
    ec.testeq(to_csubstr(src), buf.arena, buf.evts.ptr, (size_t)buf.evts.len);
}

TEST_P(IntEventsTest, dynamic_size_notransfer)
{
    test_dynamic_size(GetParam(), false);
}

TEST_P(IntEventsTest, dynamic_size_transfer)
{
    test_dynamic_size(GetParam(), true);
}

INSTANTIATE_TEST_SUITE_P(IntEvents, IntEventsTest, testing::ValuesIn(test_cases), &IntEventsTest::name2str);


} // namespace extra


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// The other test executables are written to contain the declarative-style
// YmlTestCases. This executable does not have any but the build setup
// assumes it does, and links with the test lib, which requires an existing
// get_case() function. So this is here to act as placeholder until (if?)
// proper test cases are added here. This was detected in #47 (thanks
// @cburgard).
Case const* get_case(csubstr)
{
    return nullptr;
}

} // namespace yml
} // namespace c4

// NOLINTEND(hicpp-signed-bitwise)
