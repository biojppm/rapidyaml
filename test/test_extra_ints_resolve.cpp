#include "test_lib/test_case.hpp"
#include "test_lib/test_helpers_ints.hpp"
#include <c4/yml/extra/event_handler_ints.hpp>

RYML_DEFINE_TEST_MAIN()

// NOLINTBEGIN(hicpp-signed-bitwise)

namespace c4 {
namespace yml {
namespace extra {



#define test_cyclic_reference_(...) SCOPED_CALL(test_cyclic_reference, (__VA_ARGS__))
void test_cyclic_reference(ievt::TestBuffers const& ints, bool cyclic,
                           ievt::evt_bits anchor_bits, ievt::evt_size anchor_pos, csubstr anchor,
                           ievt::evt_bits ref_bits, ievt::evt_size ref_pos, csubstr ref)
{
    ASSERT_TRUE(ievt::detail::hasall(ints.evts.ptr[anchor_pos], anchor_bits|ievt::ANCH));
    ASSERT_EQ(ints.getstr(anchor_pos), anchor);
    ASSERT_TRUE(ievt::detail::hasall(ints.evts.ptr[ref_pos], ref_bits|ievt::ALIA));
    ASSERT_EQ(ints.getstr(ref_pos), ref);
    EXPECT_EQ(cyclic, ievt::detail::is_cyclic_ref_(ints.evts.ptr, ints.evts.len, ref_pos, anchor_pos));
}


TEST(ints, cyclic_references)
{
    std::string src =
        "- &x [a,*x]\n"
        "- &y [b,*y]\n"
        "- {&z x: *z}\n"  // non cyclic!
        "- &a0\n"
        "  - *a0\n"
        "- &a1\n"
        "  - - - - a\n"
        "        - *a1\n"
        "";
    ievt::TestBuffers ints;
    parse_ints(to_substr(src), &ints);
    bool cyclic = true, noncyc/*lic*/ = false;
    //
    test_cyclic_reference_(ints, cyclic, ievt::VAL_, 3, "x", ievt::VAL_, 10, "x");
    test_cyclic_reference_(ints, cyclic, ievt::VAL_, 14, "y", ievt::VAL_, 21, "y");
    test_cyclic_reference_(ints, noncyc, ievt::KEY_, 26, "z", ievt::VAL_, 32, "z");
    test_cyclic_reference_(ints, cyclic, ievt::VAL_, 36, "a0", ievt::VAL_, 40, "a0");
    test_cyclic_reference_(ints, cyclic, ievt::VAL_, 44, "a1", ievt::VAL_, 54, "a1");
    //
    test_cyclic_reference_(ints, noncyc, ievt::VAL_, 3, "x", ievt::VAL_, 21, "y");
    test_cyclic_reference_(ints, noncyc, ievt::VAL_, 3, "x", ievt::VAL_, 32, "z");
    test_cyclic_reference_(ints, noncyc, ievt::VAL_, 3, "x", ievt::VAL_, 40, "a0");
    test_cyclic_reference_(ints, noncyc, ievt::VAL_, 3, "x", ievt::VAL_, 54, "a1");
    //
    test_cyclic_reference_(ints, noncyc, ievt::VAL_, 14, "y", ievt::VAL_, 10, "x");
    test_cyclic_reference_(ints, noncyc, ievt::VAL_, 14, "y", ievt::VAL_, 32, "z");
    test_cyclic_reference_(ints, noncyc, ievt::VAL_, 14, "y", ievt::VAL_, 40, "a0");
    test_cyclic_reference_(ints, noncyc, ievt::VAL_, 14, "y", ievt::VAL_, 54, "a1");
    //
    test_cyclic_reference_(ints, noncyc, ievt::KEY_, 26, "z", ievt::VAL_, 10, "x");
    test_cyclic_reference_(ints, noncyc, ievt::KEY_, 26, "z", ievt::VAL_, 21, "y");
    test_cyclic_reference_(ints, noncyc, ievt::KEY_, 26, "z", ievt::VAL_, 40, "a0");
    test_cyclic_reference_(ints, noncyc, ievt::KEY_, 26, "z", ievt::VAL_, 54, "a1");
    //
    test_cyclic_reference_(ints, noncyc, ievt::VAL_, 36, "a0", ievt::VAL_, 10, "x");
    test_cyclic_reference_(ints, noncyc, ievt::VAL_, 36, "a0", ievt::VAL_, 21, "y");
    test_cyclic_reference_(ints, noncyc, ievt::VAL_, 36, "a0", ievt::VAL_, 32, "z");
    test_cyclic_reference_(ints, noncyc, ievt::VAL_, 36, "a0", ievt::VAL_, 54, "a1");
    //
    test_cyclic_reference_(ints, noncyc, ievt::VAL_, 44, "a1", ievt::VAL_, 10, "x");
    test_cyclic_reference_(ints, noncyc, ievt::VAL_, 44, "a1", ievt::VAL_, 21, "y");
    test_cyclic_reference_(ints, noncyc, ievt::VAL_, 44, "a1", ievt::VAL_, 32, "z");
    test_cyclic_reference_(ints, noncyc, ievt::VAL_, 44, "a1", ievt::VAL_, 40, "a0");
    //
}
#undef test_cyclic_reference_


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(ints_resolve, empty)
{
    ievt::TestBuffers ints;
    ASSERT_EQ(ints.evts.ptr, nullptr);
    ASSERT_EQ(ints.evts.len, 0);
    ints.resolve();
    ASSERT_EQ(ints.evts.ptr, nullptr);
    ASSERT_EQ(ints.evts.len, 0);
}


#define test_ints_resolve(...) SCOPED_CALL(test_ints_resolve_, (__VA_ARGS__))
void test_ints_resolve_(std::string & src,
                        std::string const& emitted_yaml,
                        std::string const& emitted_json,
                        std::string const& resolved_yaml,
                        std::string const& resolved_json,
                        cspan<ievt::evt_bits> evts_expected,
                        cspan<ievt::evt_bits> evts_resolved,
                        cspan<ievt::evt_size> pos_seq={},
                        cspan<ievt::evt_size> pos_seq_resolved={})
{
    SCOPED_TRACE("test ints resolve");
    using namespace ievt;
    TestBuffers actual;
    parse_ints(to_substr(src), &actual);
    {
        SCOPED_TRACE("unresolved");
        if(pos_seq.empty())
        {
            SCOPED_CALL(verify_visit_seq, (evts_expected));
            SCOPED_CALL(verify_visit_seq, (actual.evts));
        }
        else
        {
            SCOPED_CALL(verify_visit_seq, (evts_expected, pos_seq));
            SCOPED_CALL(verify_visit_seq, (actual.evts, pos_seq));
        }
        TestBuffers expected = mkevts(actual, evts_expected);
        SCOPED_CALL(expected.test_invariants, ());
        SCOPED_CALL(actual.test_invariants, ());
        SCOPED_CALL(actual.test_compare, (expected));
        if(testing::Test::HasFailure())
        {
            printf("--------------\nexpected:");
            expected.print();
            printf("--------------\nactual:");
            actual.print();
            return;
        }
        {
            SCOPED_TRACE("yaml");
            std::string expect_emitted = expected.emitrs_yaml<std::string>();
            std::string actual_emitted = actual.emitrs_yaml<std::string>();
            EXPECT_EQ(emitted_yaml, actual_emitted);
            EXPECT_EQ(expect_emitted, actual_emitted);
            if(testing::Test::HasFailure())
            {
                printf("--------------\nactual:");
                actual.print();
                printf("--------------\nactual emitted:");
                printf("%.*s\n", (int)actual_emitted.size(), actual_emitted.data());
                printf("--------------\nemitted:");
                printf("%.*s\n", (int)emitted_yaml.size(), emitted_yaml.data());
                return;
            }
        }
        if(!emitted_json.empty())
        {
            SCOPED_TRACE("json");
            std::string expect_emitted = expected.emitrs_json<std::string>();
            std::string actual_emitted = actual.emitrs_json<std::string>();
            EXPECT_EQ(emitted_json, actual_emitted);
            EXPECT_EQ(expect_emitted, actual_emitted);
            if(testing::Test::HasFailure())
            {
                printf("--------------\nactual:");
                actual.print();
                printf("--------------\nactual emitted:");
                printf("%.*s\n", (int)actual_emitted.size(), actual_emitted.data());
                printf("--------------\nemitted:");
                printf("%.*s\n", (int)emitted_yaml.size(), emitted_yaml.data());
                return;
            }
        }
    }
    {
        SCOPED_TRACE("resolved");
        if(pos_seq_resolved.empty())
        {
            SCOPED_CALL(verify_visit_seq, (evts_resolved));
        }
        else
        {
            SCOPED_CALL(verify_visit_seq, (evts_resolved, pos_seq_resolved));
        }
        TestBuffers expected = mkevts(actual, evts_resolved);
        actual.resolve();
        SCOPED_CALL(expected.test_invariants, ());
        SCOPED_CALL(actual.test_invariants, ());
        actual.test_compare(expected);
        if(testing::Test::HasFailure())
        {
            printf("--------------\nexpected:");
            expected.print();
            printf("--------------\nactual:");
            actual.print();
            return;
        }
        {
            SCOPED_TRACE("yaml");
            std::string expect_resolved_emitted = expected.emitrs_yaml<std::string>();
            std::string actual_resolved_emitted = actual.emitrs_yaml<std::string>();
            EXPECT_EQ(resolved_yaml, actual_resolved_emitted);
            EXPECT_EQ(expect_resolved_emitted, actual_resolved_emitted);
        }
        if(!resolved_json.empty())
        {
            SCOPED_TRACE("json");
            std::string expect_resolved_emitted = expected.emitrs_json<std::string>();
            std::string actual_resolved_emitted = actual.emitrs_json<std::string>();
            EXPECT_EQ(resolved_json, actual_resolved_emitted);
            EXPECT_EQ(expect_resolved_emitted, actual_resolved_emitted);
        }
        if(testing::Test::HasFailure())
        {
            actual.print();
        }
    }
}


TEST(ints_resolve, 0_billion_laughs)
{
    using namespace ievt;
    std::string src = ""
        "l0: &l0 [x,x]\n"
        "l1: &l1 [*l0,*l0]\n"
        "l2: &l2 [*l1,*l1]\n"
        "";
    std::string resolved = ""
        "l0: [x,x]\n"
        "l1: [[x,x],[x,x]]\n"
        "l2: [[[x,x],[x,x]],[[x,x],[x,x]]]\n"
        "";
    std::string json = ""
        "{\n"
        "  \"l0\": [\"x\",\"x\"],\n"
        "  \"l1\": [\"*l0\",\"*l0\"],\n"
        "  \"l2\": [\"*l1\",\"*l1\"]\n"
        "}\n"
        "";
    std::string resolved_json = ""
        "{\n"
        "  \"l0\": [\"x\",\"x\"],\n"
        "  \"l1\": [[\"x\",\"x\"],[\"x\",\"x\"]],\n"
        "  \"l2\": [[[\"x\",\"x\"],[\"x\",\"x\"]],[[\"x\",\"x\"],[\"x\",\"x\"]]]\n"
        "}\n"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,2, /*l0*/
        /*[4][6]*/       VAL_|ANCH|PEX2, 5,2, /*l0*/
        /*[5][9]*/       VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[6][10]*/         VAL_|SCLR|PLAI, 9,1, /*x*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 11,1, /*x*/
        /*[8][16]*/       ESEQ|PEX2,
        /*[9][17]*/       KEY_|SCLR|PLAI, 14,2, /*l1*/
        /*[10][20]*/       VAL_|ANCH|PEX2, 19,2, /*l1*/
        /*[11][23]*/       VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[12][24]*/         VAL_|ALIA, 24,2, /*l0*/
        /*[13][27]*/         VAL_|ALIA|PEX2, 28,2, /*l0*/
        /*[14][30]*/       ESEQ|PEX2,
        /*[15][31]*/       KEY_|SCLR|PLAI, 32,2, /*l2*/
        /*[16][34]*/       VAL_|ANCH|PEX2, 37,2, /*l2*/
        /*[17][37]*/       VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[18][38]*/         VAL_|ALIA, 42,2, /*l1*/
        /*[19][41]*/         VAL_|ALIA|PEX2, 46,2, /*l1*/
        /*[20][44]*/       ESEQ|PEX2,
        /*[21][45]*/     EMAP,
        /*[22][46]*/   EDOC,
        /*[23][47]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,2, /*l0*/
        /*[4][6]*/       VAL_|PEX2, VAL_,VAL_, /*l0*/
        /*[5][9]*/       VAL_|BSEQ|FLOW|FSL_,
        /*[6][10]*/         VAL_|SCLR|PLAI, 9,1, /*x*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 11,1, /*x*/
        /*[8][16]*/       ESEQ|PEX2,
        /*[9][17]*/       KEY_|SCLR|PLAI, 14,2, /*l1*/
        /*[10][20]*/       VAL_|PEX2, VAL_,VAL_, /*l1*/
        /*[11][23]*/       VAL_|BSEQ|FLOW|FSL_,
        /*[12][24]*/         VAL_|RREF, 9,17, /*l0*/
        /*[13][27]*/         VAL_|RREF|PEX2, 9,17, /*l0*/
        /*[14][30]*/       ESEQ|PEX2,
        /*[15][31]*/       KEY_|SCLR|PLAI, 32,2, /*l2*/
        /*[16][34]*/       VAL_|PEX2, VAL_,VAL_, /*l2*/
        /*[17][37]*/       VAL_|BSEQ|FLOW|FSL_,
        /*[18][38]*/         VAL_|RREF, 23,31, /*l1*/
        /*[19][41]*/         VAL_|RREF|PEX2, 23,31, /*l1*/
        /*[20][44]*/       ESEQ|PEX2,
        /*[21][45]*/     EMAP,
        /*[22][46]*/   EDOC,
        /*[23][47]*/ ESTR,
    };
    const evt_size pos[] = {
        0,1,2,3,6,9,10,13,16,17,20,23,24,27,30,31,34,37,38,41,44,45,46,47,
    };
    const evt_size pos_resolved[] = {
        0,1,2,3,6,7,8,9,10,13,16,17,20,21,22,23,/*rref@24*/
                      9,10,13,16,/*rref@27*/
                      9,10,13,16,
                      30,31,34,35,36,37,/*rref@38*/
                      23,/*rref@24*/
                      9,10,13,16,/*rref@27*/
                      9,10,13,16,
                      30,
                      23,/*rref@24*/
                      9,10,13,16,/*rref@27*/
                      9,10,13,16,
                      30,
                      44,45,46,47,};
    test_ints_resolve(src, src, json, resolved, resolved_json, evts_expected, evts_resolved, pos, pos_resolved);
}

TEST(ints_resolve, 1_to_scalar_seq_block)
{
    using namespace ievt;
    std::string src = "- &anc foo\n- *anc\n";
    std::string resolved = "- foo\n- foo\n";
    std::string json = "[\n  \"foo\",\n  \"*anc\"\n]\n";
    std::string resolved_json = "[\n  \"foo\",\n  \"foo\"\n]\n";
    const evt_bits evts_expected[] = {
        BSTR,
        BDOC,
        VAL_|BSEQ,
        VAL_|ANCH,            3, 3,
        VAL_|SCLR|PLAI|PEX2,  7, 3,
        VAL_|ALIA|PEX2,      14, 3,
        ESEQ|PEX2,
        EDOC,
        ESTR,
    };
    const evt_bits evts_resolved[] = {
        BSTR,
        BDOC,
        VAL_|BSEQ,
        VAL_, VAL_, VAL_,
        VAL_|SCLR|PLAI,  7, 3,
        VAL_|RREF|PEX2,  6, 9, // this was resolved
        ESEQ|PEX2,
        EDOC,
        ESTR,
    };
    const evt_size pos[] = {0,1,2,3,6,9,12,13,14};
    const evt_size pos_resolved[] = {0,1,2,3,4,5,6,6,12,13,14};
    test_ints_resolve(src, src, json, resolved, resolved_json, evts_expected, evts_resolved, pos, pos_resolved);
}

TEST(ints_resolve, 1_to_scalar_seq_flow_ml)
{
    using namespace ievt;
    std::string src = "[\n  &anc foo,\n  *anc\n]\n";
    std::string resolved = "[\n  foo,\n  foo\n]\n";
    std::string json = "[\n  \"foo\",\n  \"*anc\"\n]\n";
    std::string resolved_json = "[\n  \"foo\",\n  \"foo\"\n]\n";
    const evt_bits evts_expected[] = {
        /*0*/BSTR,
        /*1*/BDOC,
        /*2*/VAL_|BSEQ|FLOW|FML1,
        /*3*/VAL_|ANCH,            5, 3,
        /*6*/VAL_|SCLR|PLAI|PEX2,  9, 3,
        /*9*/VAL_|ALIA|PEX2,      17, 3,
        /*12*/ESEQ|PEX2,
        /*13*/EDOC,
        /*14*/ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*0*/BSTR,
        /*1*/BDOC,
        /*2*/VAL_|BSEQ|FLOW|FML1,
        /*3*/VAL_, VAL_, VAL_,
        /*6*/VAL_|SCLR|PLAI,  9, 3,
        /*9*/VAL_|RREF|PEX2,  6, 9, // this was resolved
        /*12*/ESEQ|PEX2,
        /*13*/EDOC,
        /*14*/ESTR,
    };
    const evt_size pos[] = {0,1,2,3,6,9,12,13,14};
    const evt_size pos_resolved[] = {0,1,2,3,4,5,6,6,12,13,14};
    test_ints_resolve(src, src, json, resolved, resolved_json, evts_expected, evts_resolved, pos, pos_resolved);
}

TEST(ints_resolve, 1_to_scalar_seq_flow_sl)
{
    using namespace ievt;
    std::string src = "[&anc foo,*anc]";
    std::string resolved = "[foo,foo]";
    std::string json = "[\"foo\",\"*anc\"]";
    std::string resolved_json = "[\"foo\",\"foo\"]";
    const evt_bits evts_expected[] = {
        BSTR,
        BDOC,
        VAL_|BSEQ|FLOW|FSL_,
        VAL_|ANCH,            2, 3,
        VAL_|SCLR|PLAI|PEX2,  6, 3,
        VAL_|ALIA|PEX2,      11, 3,
        ESEQ|PEX2,
        EDOC,
        ESTR,
    };
    const evt_bits evts_resolved[] = {
        BSTR,
        BDOC,
        VAL_|BSEQ|FLOW|FSL_,
        VAL_, VAL_, VAL_,
        VAL_|SCLR|PLAI,  6, 3,
        VAL_|RREF|PEX2,  6, 9, // this was resolved
        ESEQ|PEX2,
        EDOC,
        ESTR,
    };
    const evt_size pos[] = {0,1,2,3,6,9,12,13,14};
    const evt_size pos_resolved[] = {0,1,2,3,4,5,6,6,12,13,14};
    test_ints_resolve(src, src, json, resolved, resolved_json, evts_expected, evts_resolved, pos, pos_resolved);
}

TEST(ints_resolve, 2_to_container_seq_blck)
{
    using namespace ievt;
    std::string src = "- &foo [0,1,2,3]\n- *foo\n";
    std::string resolved = "- [0,1,2,3]\n- [0,1,2,3]\n";
    std::string json = "[\n  [0,1,2,3],\n  \"*foo\"\n]\n";
    std::string resolved_json = "[\n  [0,1,2,3],\n  [0,1,2,3]\n]\n";
    const evt_bits evts_expected[] = {
        /* 0*/BSTR,
        /* 1*/BDOC,
        /* 2*/VAL_|BSEQ,
        /* 3*/VAL_|ANCH,            3, 3,
        /* 6*/VAL_|BSEQ|FLOW|FSL_|PEX2,
        /* 7*/VAL_|SCLR|PLAI,       8, 1,
        /*10*/VAL_|SCLR|PLAI|PEX2, 10, 1,
        /*13*/VAL_|SCLR|PLAI|PEX2, 12, 1,
        /*16*/VAL_|SCLR|PLAI|PEX2, 14, 1,
        /*19*/ESEQ|PEX2,
        /*20*/VAL_|ALIA,           20, 3,
        /*23*/ESEQ|PEX2,
        /*24*/EDOC,
        /*25*/ESTR,
    };
    const evt_size pos[] = {0,1,2,3,6,7,10,13,16,19,20,23,24,25};
    const evt_bits evts_resolved[] = {
        /* 0*/BSTR,
        /* 1*/BDOC,
        /* 2*/VAL_|BSEQ,
        /* 3*/VAL_, VAL_, VAL_,
        /* 6*/VAL_|BSEQ|FLOW|FSL_,
        /* 7*/VAL_|SCLR|PLAI,       8, 1,
        /*10*/VAL_|SCLR|PLAI|PEX2, 10, 1,
        /*13*/VAL_|SCLR|PLAI|PEX2, 12, 1,
        /*16*/VAL_|SCLR|PLAI|PEX2, 14, 1,
        /*19*/ESEQ|PEX2,
        /*20*/VAL_|RREF,            6,20, // this was resolved
        /*23*/ESEQ|PEX2,
        /*24*/EDOC,
        /*25*/ESTR,
    };
    const evt_size pos_resolved[] = {0,1,2,3,4,5,6,7,10,13,16,19,6,7,10,13,16,19,23,24,25};
    test_ints_resolve(src, src, json, resolved, resolved_json, evts_expected, evts_resolved, pos, pos_resolved);
}

TEST(ints_resolve, 2_to_container_seq_flow_ml)
{
    using namespace ievt;
    std::string src = "[\n&foo [0,1,2,3],\n *foo\n]\n";
    std::string emitted = "[\n  &foo [0,1,2,3],\n  *foo\n]\n";
    std::string resolved = "[\n  [0,1,2,3],\n  [0,1,2,3]\n]\n";
    std::string json = "[\n  [0,1,2,3],\n  \"*foo\"\n]\n";
    const evt_bits evts_expected[] = {
        /* 0*/BSTR,
        /* 1*/BDOC,
        /* 2*/VAL_|BSEQ|FLOW|FML1,
        /* 3*/VAL_|ANCH,            3, 3,
        /* 6*/VAL_|BSEQ|FLOW|FSL_|PEX2,
        /* 7*/VAL_|SCLR|PLAI,       8, 1,
        /*10*/VAL_|SCLR|PLAI|PEX2, 10, 1,
        /*13*/VAL_|SCLR|PLAI|PEX2, 12, 1,
        /*16*/VAL_|SCLR|PLAI|PEX2, 14, 1,
        /*19*/ESEQ|PEX2,
        /*20*/VAL_|ALIA,           20, 3,
        /*23*/ESEQ|PEX2,
        /*24*/EDOC,
        /*25*/ESTR,
    };
    const evt_bits evts_resolved[] = {
        /* 0*/BSTR,
        /* 1*/BDOC,
        /* 2*/VAL_|BSEQ|FLOW|FML1,
        /* 3*/VAL_, VAL_, VAL_,
        /* 6*/VAL_|BSEQ|FLOW|FSL_,
        /* 7*/VAL_|SCLR|PLAI,       8, 1,
        /*10*/VAL_|SCLR|PLAI|PEX2, 10, 1,
        /*13*/VAL_|SCLR|PLAI|PEX2, 12, 1,
        /*16*/VAL_|SCLR|PLAI|PEX2, 14, 1,
        /*19*/ESEQ|PEX2,
        /*20*/VAL_|RREF,            6,20, // this was resolved
        /*23*/ESEQ|PEX2,
        /*24*/EDOC,
        /*25*/ESTR,
    };
    const evt_size pos[] = {0,1,2,3,6,7,10,13,16,19,20,23,24,25};
    const evt_size pos_resolved[] = {0,1,2,3,4,5,6,7,10,13,16,19,6,7,10,13,16,19,23,24,25};
    test_ints_resolve(src, emitted, json, resolved, resolved, evts_expected, evts_resolved, pos, pos_resolved);
}

TEST(ints_resolve, 2_to_container_seq_flow_sl)
{
    using namespace ievt;
    std::string src = "[ &foo [0,1,2,3],  *foo ]";
    std::string emitted = "[&foo [0,1,2,3],*foo]";
    std::string resolved = "[[0,1,2,3],[0,1,2,3]]";
    std::string json = "[[0,1,2,3],\"*foo\"]";
    const evt_bits evts_expected[] = {
        /* 0*/BSTR,
        /* 1*/BDOC,
        /* 2*/VAL_|BSEQ|FLOW|FSL_,
        /* 3*/VAL_|ANCH,            3, 3,
        /* 6*/VAL_|BSEQ|FLOW|FSL_|PEX2,
        /* 7*/VAL_|SCLR|PLAI,       8, 1,
        /*10*/VAL_|SCLR|PLAI|PEX2, 10, 1,
        /*13*/VAL_|SCLR|PLAI|PEX2, 12, 1,
        /*16*/VAL_|SCLR|PLAI|PEX2, 14, 1,
        /*19*/ESEQ|PEX2,
        /*20*/VAL_|ALIA,           20, 3,
        /*23*/ESEQ|PEX2,
        /*24*/EDOC,
        /*25*/ESTR,
    };
    const evt_bits evts_resolved[] = {
        /* 0*/BSTR,
        /* 1*/BDOC,
        /* 2*/VAL_|BSEQ|FLOW|FSL_,
        /* 3*/VAL_, VAL_, VAL_,
        /* 6*/VAL_|BSEQ|FLOW|FSL_,
        /* 7*/VAL_|SCLR|PLAI,       8, 1,
        /*10*/VAL_|SCLR|PLAI|PEX2, 10, 1,
        /*13*/VAL_|SCLR|PLAI|PEX2, 12, 1,
        /*16*/VAL_|SCLR|PLAI|PEX2, 14, 1,
        /*19*/ESEQ|PEX2,
        /*20*/VAL_|RREF,            6,20, // this was resolved
        /*23*/ESEQ|PEX2,
        /*24*/EDOC,
        /*25*/ESTR,
    };
    const evt_size pos[] = {0,1,2,3,6,7,10,13,16,19,20,23,24,25};
    const evt_size pos_resolved[] = {0,1,2,3,4,5,6,7,10,13,16,19,6,7,10,13,16,19,23,24,25};
    test_ints_resolve(src, emitted, json, resolved, resolved, evts_expected, evts_resolved, pos, pos_resolved);
}


TEST(ints_resolve, 3_key_vs_seq)
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
        /*[4][6]*/       KEY_|SCLR|PLAI|PEX2, 11,3,  /*key*/
        /*[5][9]*/       VAL_|ANCH|PEX2, 17,9,  /*valscalar*/
        /*[6][12]*/       VAL_|SCLR|PLAI|PEX2, 27,3,  /*val*/
        /*[7][15]*/       KEY_|ANCH|PEX2, 32,6,  /*keyseq*/
        /*[8][18]*/       KEY_|BSEQ|FLOW|FSL_|PEX2,
        /*[9][19]*/         VAL_|SCLR|PLAI, 40,1,  /*a*/
        /*[10][22]*/         VAL_|SCLR|PLAI|PEX2, 43,1,  /*b*/
        /*[11][25]*/       ESEQ|PEX2,
        /*[12][26]*/       VAL_|ANCH, 48,6,  /*valseq*/
        /*[13][29]*/       VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[14][30]*/         VAL_|SCLR|PLAI, 56,1,  /*c*/
        /*[15][33]*/         VAL_|SCLR|PLAI|PEX2, 59,1,  /*d*/
        /*[16][36]*/       ESEQ|PEX2,
        /*[17][37]*/       KEY_|ANCH, 63,6,  /*keymap*/
        /*[18][40]*/       KEY_|BMAP|FLOW|FSL_|PEX2,
        /*[19][41]*/         KEY_|SCLR|PLAI, 71,1,  /*e*/
        /*[20][44]*/         VAL_|SCLR|PLAI|PEX2, 74,1,  /*f*/
        /*[21][47]*/       EMAP|PEX2,
        /*[22][48]*/       VAL_|ANCH, 79,6,  /*valmap*/
        /*[23][51]*/       VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[24][52]*/         KEY_|SCLR|PLAI, 87,1,  /*g*/
        /*[25][55]*/         VAL_|SCLR|PLAI|PEX2, 90,1,  /*h*/
        /*[26][58]*/       EMAP|PEX2,
        /*[27][59]*/       KEY_|ALIA, 94,9,  /*valscalar*/
        /*[28][62]*/       VAL_|ALIA|PEX2, 107,9,  /*keyscalar*/
        /*[29][65]*/       KEY_|ALIA|PEX2, 118,6,  /*valseq*/
        /*[30][68]*/       VAL_|ALIA|PEX2, 128,6,  /*keyseq*/
        /*[31][71]*/       KEY_|ALIA|PEX2, 136,6,  /*valmap*/
        /*[32][74]*/       VAL_|ALIA|PEX2, 146,6,  /*keymap*/
        /*[33][77]*/     EMAP|PEX2,
        /*[34][78]*/   EDOC,
        /*[35][79]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_,KEY_,KEY_,  /*keyscalar*/
        /*[4][6]*/       KEY_|SCLR|PLAI, 11,3,  /*key*/
        /*[5][9]*/       VAL_|PEX2, VAL_,VAL_,  /*valscalar*/
        /*[6][12]*/       VAL_|SCLR|PLAI, 27,3,  /*val*/
        /*[7][15]*/       KEY_|PEX2, KEY_,KEY_,  /*keyseq*/
        /*[8][18]*/       KEY_|BSEQ|FLOW|FSL_,
        /*[9][19]*/         VAL_|SCLR|PLAI, 40,1,  /*a*/
        /*[10][22]*/         VAL_|SCLR|PLAI|PEX2, 43,1,  /*b*/
        /*[11][25]*/       ESEQ|PEX2,
        /*[12][26]*/       VAL_, VAL_,VAL_,  /*valseq*/
        /*[13][29]*/       VAL_|BSEQ|FLOW|FSL_,
        /*[14][30]*/         VAL_|SCLR|PLAI, 56,1,  /*c*/
        /*[15][33]*/         VAL_|SCLR|PLAI|PEX2, 59,1,  /*d*/
        /*[16][36]*/       ESEQ|PEX2,
        /*[17][37]*/       KEY_, KEY_,KEY_,  /*keymap*/
        /*[18][40]*/       KEY_|BMAP|FLOW|FSL_,
        /*[19][41]*/         KEY_|SCLR|PLAI, 71,1,  /*e*/
        /*[20][44]*/         VAL_|SCLR|PLAI|PEX2, 74,1,  /*f*/
        /*[21][47]*/       EMAP|PEX2,
        /*[22][48]*/       VAL_, VAL_,VAL_,  /*valmap*/
        /*[23][51]*/       VAL_|BMAP|FLOW|FSL_,
        /*[24][52]*/         KEY_|SCLR|PLAI, 87,1,  /*g*/
        /*[25][55]*/         VAL_|SCLR|PLAI|PEX2, 90,1,  /*h*/
        /*[26][58]*/       EMAP|PEX2,
        /*[27][59]*/       KEY_|RREF, 12,15,  /*valscalar*/
        /*[28][62]*/       VAL_|RREF|PEX2, 6,9,  /*keyscalar*/
        /*[29][65]*/       KEY_|RREF|PEX2, 29,37,  /*valseq*/
        /*[30][68]*/       VAL_|RREF|PEX2, 18,26,  /*keyseq*/
        /*[31][71]*/       KEY_|RREF|PEX2, 51,59,  /*valmap*/
        /*[32][74]*/       VAL_|RREF|PEX2, 40,48,  /*keymap*/
        /*[33][77]*/     EMAP|PEX2,
        /*[34][78]*/   EDOC,
        /*[35][79]*/ ESTR,
    };
    test_ints_resolve(src, emitted, {}, resolved, {}, evts_expected, evts_resolved);
}

TEST(ints_resolve, 3_key_vs_seq_flow_ml)
{
    using namespace ievt;
    std::string src = ""
        "{\n"
        "  &keyscalar key: &valscalar val,\n"
        "  &keyseq [a, b]: &valseq [c, d],\n"
        "  &keymap {e: f}: &valmap {g: h},\n"
        "  *valscalar : *keyscalar,\n"
        "  *valseq : *keyseq,\n"
        "  *valmap : *keymap,\n"
        "}\n"
        "";
    std::string emitted = ""
        "{\n"
        "  &keyscalar key: &valscalar val,\n"
        "  ? &keyseq [a,b]: &valseq [c,d],\n"
        "  ? &keymap {e: f}: &valmap {g: h},\n"
        "  *valscalar : *keyscalar,\n"
        "  *valseq : *keyseq,\n"
        "  *valmap : *keymap\n"
        "}\n"
        "";
    std::string resolved = ""
        "{\n"
        "  key: val,\n"
        "  ? [a,b]: [c,d],\n"
        "  ? {e: f}: {g: h},\n"
        "  val: key,\n"
        "  ? [c,d]: [a,b],\n"
        "  ? {g: h}: {e: f}\n"
        "}\n"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP|FLOW|FML1,
        /*[3][3]*/       KEY_|ANCH, 5,9, /*keyscalar*/
        /*[4][6]*/       KEY_|SCLR|PLAI|PEX2, 15,3, /*key*/
        /*[5][9]*/       VAL_|ANCH|PEX2, 21,9, /*valscalar*/
        /*[6][12]*/       VAL_|SCLR|PLAI|PEX2, 31,3, /*val*/
        /*[7][15]*/       KEY_|ANCH|PEX2, 39,6, /*keyseq*/
        /*[8][18]*/       KEY_|BSEQ|FLOW|FSL_|PEX2,
        /*[9][19]*/         VAL_|SCLR|PLAI, 47,1, /*a*/
        /*[10][22]*/         VAL_|SCLR|PLAI|PEX2, 50,1, /*b*/
        /*[11][25]*/       ESEQ|PEX2,
        /*[12][26]*/       VAL_|ANCH, 55,6, /*valseq*/
        /*[13][29]*/       VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[14][30]*/         VAL_|SCLR|PLAI, 63,1, /*c*/
        /*[15][33]*/         VAL_|SCLR|PLAI|PEX2, 66,1, /*d*/
        /*[16][36]*/       ESEQ|PEX2,
        /*[17][37]*/       KEY_|ANCH, 73,6, /*keymap*/
        /*[18][40]*/       KEY_|BMAP|FLOW|FSL_|PEX2,
        /*[19][41]*/         KEY_|SCLR|PLAI, 81,1, /*e*/
        /*[20][44]*/         VAL_|SCLR|PLAI|PEX2, 84,1, /*f*/
        /*[21][47]*/       EMAP|PEX2,
        /*[22][48]*/       VAL_|ANCH, 89,6, /*valmap*/
        /*[23][51]*/       VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[24][52]*/         KEY_|SCLR|PLAI, 97,1, /*g*/
        /*[25][55]*/         VAL_|SCLR|PLAI|PEX2, 100,1, /*h*/
        /*[26][58]*/       EMAP|PEX2,
        /*[27][59]*/       KEY_|ALIA, 107,9, /*valscalar*/
        /*[28][62]*/       VAL_|ALIA|PEX2, 120,9, /*keyscalar*/
        /*[29][65]*/       KEY_|ALIA|PEX2, 134,6, /*valseq*/
        /*[30][68]*/       VAL_|ALIA|PEX2, 144,6, /*keyseq*/
        /*[31][71]*/       KEY_|ALIA|PEX2, 155,6, /*valmap*/
        /*[32][74]*/       VAL_|ALIA|PEX2, 165,6, /*keymap*/
        /*[33][77]*/     EMAP|PEX2,
        /*[34][78]*/   EDOC,
        /*[35][79]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP|FLOW|FML1,
        /*[3][3]*/       KEY_, KEY_,KEY_, /*keyscalar*/
        /*[4][6]*/       KEY_|SCLR|PLAI, 15,3, /*key*/
        /*[5][9]*/       VAL_|PEX2, VAL_,VAL_, /*valscalar*/
        /*[6][12]*/       VAL_|SCLR|PLAI, 31,3, /*val*/
        /*[7][15]*/       KEY_|PEX2, KEY_,KEY_, /*keyseq*/
        /*[8][18]*/       KEY_|BSEQ|FLOW|FSL_,
        /*[9][19]*/         VAL_|SCLR|PLAI, 47,1, /*a*/
        /*[10][22]*/         VAL_|SCLR|PLAI|PEX2, 50,1, /*b*/
        /*[11][25]*/       ESEQ|PEX2,
        /*[12][26]*/       VAL_, VAL_,VAL_, /*valseq*/
        /*[13][29]*/       VAL_|BSEQ|FLOW|FSL_,
        /*[14][30]*/         VAL_|SCLR|PLAI, 63,1, /*c*/
        /*[15][33]*/         VAL_|SCLR|PLAI|PEX2, 66,1, /*d*/
        /*[16][36]*/       ESEQ|PEX2,
        /*[17][37]*/       KEY_, KEY_,KEY_, /*keymap*/
        /*[18][40]*/       KEY_|BMAP|FLOW|FSL_,
        /*[19][41]*/         KEY_|SCLR|PLAI, 81,1, /*e*/
        /*[20][44]*/         VAL_|SCLR|PLAI|PEX2, 84,1, /*f*/
        /*[21][47]*/       EMAP|PEX2,
        /*[22][48]*/       VAL_, VAL_,VAL_, /*valmap*/
        /*[23][51]*/       VAL_|BMAP|FLOW|FSL_,
        /*[24][52]*/         KEY_|SCLR|PLAI, 97,1, /*g*/
        /*[25][55]*/         VAL_|SCLR|PLAI|PEX2, 100,1, /*h*/
        /*[26][58]*/       EMAP|PEX2,
        /*[27][59]*/       KEY_|RREF, 12,15,  /*valscalar*/
        /*[28][62]*/       VAL_|RREF|PEX2, 6,9,  /*keyscalar*/
        /*[29][65]*/       KEY_|RREF|PEX2, 29,37,  /*valseq*/
        /*[30][68]*/       VAL_|RREF|PEX2, 18,26,  /*keyseq*/
        /*[31][71]*/       KEY_|RREF|PEX2, 51,59,  /*valmap*/
        /*[32][74]*/       VAL_|RREF|PEX2, 40,48,  /*keymap*/
        /*[33][77]*/     EMAP|PEX2,
        /*[34][78]*/   EDOC,
        /*[35][79]*/ ESTR,
    };
    test_ints_resolve(src, emitted, {}, resolved, {}, evts_expected, evts_resolved);
}

TEST(ints_resolve, 3_key_vs_seq_flow_sl)
{
    using namespace ievt;
    std::string src = ""
        "{ "
        "  &keyscalar key: &valscalar val, "
        "  &keyseq [a, b]: &valseq [c, d], "
        "  &keymap {e: f}: &valmap {g: h}, "
        "  *valscalar : *keyscalar, "
        "  *valseq : *keyseq, "
        "  *valmap : *keymap, "
        "}\n"
        "";
    std::string emitted = ""
        "{"
        "&keyscalar key: &valscalar val,"
        "? &keyseq [a,b]: &valseq [c,d],"
        "? &keymap {e: f}: &valmap {g: h},"
        "*valscalar : *keyscalar,"
        "*valseq : *keyseq,"
        "*valmap : *keymap"
        "}"
        "";
    std::string resolved = ""
        "{"
        "key: val,"
        "? [a,b]: [c,d],"
        "? {e: f}: {g: h},"
        "val: key,"
        "? [c,d]: [a,b],"
        "? {g: h}: {e: f}"
        "}"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP|FLOW|FSL_,
        /*[3][3]*/       KEY_|ANCH, 5,9, /*keyscalar*/
        /*[4][6]*/       KEY_|SCLR|PLAI|PEX2, 15,3, /*key*/
        /*[5][9]*/       VAL_|ANCH|PEX2, 21,9, /*valscalar*/
        /*[6][12]*/       VAL_|SCLR|PLAI|PEX2, 31,3, /*val*/
        /*[7][15]*/       KEY_|ANCH|PEX2, 39,6, /*keyseq*/
        /*[8][18]*/       KEY_|BSEQ|FLOW|FSL_|PEX2,
        /*[9][19]*/         VAL_|SCLR|PLAI, 47,1, /*a*/
        /*[10][22]*/         VAL_|SCLR|PLAI|PEX2, 50,1, /*b*/
        /*[11][25]*/       ESEQ|PEX2,
        /*[12][26]*/       VAL_|ANCH, 55,6, /*valseq*/
        /*[13][29]*/       VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[14][30]*/         VAL_|SCLR|PLAI, 63,1, /*c*/
        /*[15][33]*/         VAL_|SCLR|PLAI|PEX2, 66,1, /*d*/
        /*[16][36]*/       ESEQ|PEX2,
        /*[17][37]*/       KEY_|ANCH, 73,6, /*keymap*/
        /*[18][40]*/       KEY_|BMAP|FLOW|FSL_|PEX2,
        /*[19][41]*/         KEY_|SCLR|PLAI, 81,1, /*e*/
        /*[20][44]*/         VAL_|SCLR|PLAI|PEX2, 84,1, /*f*/
        /*[21][47]*/       EMAP|PEX2,
        /*[22][48]*/       VAL_|ANCH, 89,6, /*valmap*/
        /*[23][51]*/       VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[24][52]*/         KEY_|SCLR|PLAI, 97,1, /*g*/
        /*[25][55]*/         VAL_|SCLR|PLAI|PEX2, 100,1, /*h*/
        /*[26][58]*/       EMAP|PEX2,
        /*[27][59]*/       KEY_|ALIA, 107,9, /*valscalar*/
        /*[28][62]*/       VAL_|ALIA|PEX2, 120,9, /*keyscalar*/
        /*[29][65]*/       KEY_|ALIA|PEX2, 134,6, /*valseq*/
        /*[30][68]*/       VAL_|ALIA|PEX2, 144,6, /*keyseq*/
        /*[31][71]*/       KEY_|ALIA|PEX2, 155,6, /*valmap*/
        /*[32][74]*/       VAL_|ALIA|PEX2, 165,6, /*keymap*/
        /*[33][77]*/     EMAP|PEX2,
        /*[34][78]*/   EDOC,
        /*[35][79]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP|FLOW|FSL_,
        /*[3][3]*/       KEY_, KEY_,KEY_, /*keyscalar*/
        /*[4][6]*/       KEY_|SCLR|PLAI, 15,3, /*key*/
        /*[5][9]*/       VAL_|PEX2, VAL_,VAL_, /*valscalar*/
        /*[6][12]*/       VAL_|SCLR|PLAI, 31,3, /*val*/
        /*[7][15]*/       KEY_|PEX2, KEY_,KEY_, /*keyseq*/
        /*[8][18]*/       KEY_|BSEQ|FLOW|FSL_,
        /*[9][19]*/         VAL_|SCLR|PLAI, 47,1, /*a*/
        /*[10][22]*/         VAL_|SCLR|PLAI|PEX2, 50,1, /*b*/
        /*[11][25]*/       ESEQ|PEX2,
        /*[12][26]*/       VAL_, VAL_,VAL_, /*valseq*/
        /*[13][29]*/       VAL_|BSEQ|FLOW|FSL_,
        /*[14][30]*/         VAL_|SCLR|PLAI, 63,1, /*c*/
        /*[15][33]*/         VAL_|SCLR|PLAI|PEX2, 66,1, /*d*/
        /*[16][36]*/       ESEQ|PEX2,
        /*[17][37]*/       KEY_, KEY_,KEY_, /*keymap*/
        /*[18][40]*/       KEY_|BMAP|FLOW|FSL_,
        /*[19][41]*/         KEY_|SCLR|PLAI, 81,1, /*e*/
        /*[20][44]*/         VAL_|SCLR|PLAI|PEX2, 84,1, /*f*/
        /*[21][47]*/       EMAP|PEX2,
        /*[22][48]*/       VAL_, VAL_,VAL_, /*valmap*/
        /*[23][51]*/       VAL_|BMAP|FLOW|FSL_,
        /*[24][52]*/         KEY_|SCLR|PLAI, 97,1, /*g*/
        /*[25][55]*/         VAL_|SCLR|PLAI|PEX2, 100,1, /*h*/
        /*[26][58]*/       EMAP|PEX2,
        /*[27][59]*/       KEY_|RREF, 12,15,  /*valscalar*/
        /*[28][62]*/       VAL_|RREF|PEX2, 6,9,  /*keyscalar*/
        /*[29][65]*/       KEY_|RREF|PEX2, 29,37,  /*valseq*/
        /*[30][68]*/       VAL_|RREF|PEX2, 18,26,  /*keyseq*/
        /*[31][71]*/       KEY_|RREF|PEX2, 51,59,  /*valmap*/
        /*[32][74]*/       VAL_|RREF|PEX2, 40,48,  /*keymap*/
        /*[33][77]*/     EMAP|PEX2,
        /*[34][78]*/   EDOC,
        /*[35][79]*/ ESTR,
    };
    test_ints_resolve(src, emitted, {}, resolved, {}, evts_expected, evts_resolved);
}

TEST(ints_resolve, 4_inherit_block)
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
    std::string json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": \"val0\",\n"
        "    \"key1\": \"val1\"\n"
        "  },\n"
        "  \"inherit\": {\n"
        "    \"<<\": \"*map0\"\n"
        "  }\n"
        "}\n"
        "";
    std::string resolved_json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": \"val0\",\n"
        "    \"key1\": \"val1\"\n"
        "  },\n"
        "  \"inherit\": {\n"
        "    \"key0\": \"val0\",\n"
        "    \"key1\": \"val1\"\n"
        "  }\n"
        "}\n"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4, /*map0*/
        /*[4][6]*/       VAL_|ANCH|PEX2, 7,4, /*map0*/
        /*[5][9]*/       VAL_|BMAP|PEX2,
        /*[6][10]*/         KEY_|SCLR|PLAI, 14,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 20,4, /*val0*/
        /*[8][16]*/         KEY_|SCLR|PLAI|PEX2, 27,4, /*key1*/
        /*[9][19]*/         VAL_|SCLR|PLAI|PEX2, 33,4, /*val1*/
        /*[10][22]*/       EMAP|PEX2,
        /*[11][23]*/       KEY_|SCLR|PLAI, 38,7, /*inherit*/
        /*[12][26]*/       VAL_|BMAP|PEX2,
        /*[13][27]*/         KEY_|SCLR|PLAI, 49,2, /*<<*/
        /*[14][30]*/         VAL_|ALIA|PEX2, 54,4, /*map0*/
        /*[15][33]*/       EMAP|PEX2,
        /*[16][34]*/     EMAP,
        /*[17][35]*/   EDOC,
        /*[18][36]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4, /*map0*/
        /*[4][6]*/       VAL_|PEX2, VAL_,VAL_, /*map0*/
        /*[5][9]*/       VAL_|BMAP,
        /*[6][10]*/         KEY_|SCLR|PLAI, 14,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 20,4, /*val0*/
        /*[8][16]*/         KEY_|SCLR|PLAI|PEX2, 27,4, /*key1*/
        /*[9][19]*/         VAL_|SCLR|PLAI|PEX2, 33,4, /*val1*/
        /*[10][22]*/       EMAP|PEX2,
        /*[11][23]*/       KEY_|SCLR|PLAI, 38,7, /*inherit*/
        /*[12][26]*/       VAL_|BMAP|PEX2,
        /*[13][27]*/         KEY_,KEY_,KEY_, /*<<*/
        /*[14][30]*/         VAL_|RREF, 10,22, /*map0*/
        /*[15][33]*/       EMAP|PEX2,
        /*[16][34]*/     EMAP,
        /*[17][35]*/   EDOC,
        /*[18][36]*/ ESTR,
    };
    test_ints_resolve(src, src, json, resolved, resolved_json, evts_expected, evts_resolved);
}

TEST(ints_resolve, 4_inherit_flow_ml)
{
    using namespace ievt;
    std::string src = ""
        "{\n"
        "  map0: &map0 {\n"
        "    key0: val0,\n"
        "    key1: val1\n"
        "  },\n"
        "  inherit: {\n"
        "    <<: *map0\n"
        "  }\n"
        "}\n"
        "";
    std::string resolved = ""
        "{\n"
        "  map0: {\n"
        "    key0: val0,\n"
        "    key1: val1\n"
        "  },\n"
        "  inherit: {\n"
        "    key0: val0,\n"
        "    key1: val1\n"
        "  }\n"
        "}\n"
        "";
    std::string json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": \"val0\",\n"
        "    \"key1\": \"val1\"\n"
        "  },\n"
        "  \"inherit\": {\n"
        "    \"<<\": \"*map0\"\n"
        "  }\n"
        "}\n"
        "";
    std::string resolved_json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": \"val0\",\n"
        "    \"key1\": \"val1\"\n"
        "  },\n"
        "  \"inherit\": {\n"
        "    \"key0\": \"val0\",\n"
        "    \"key1\": \"val1\"\n"
        "  }\n"
        "}\n"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP|FLOW|FML1,
        /*[3][3]*/       KEY_|SCLR|PLAI, 4,4, /*map0*/
        /*[4][6]*/       VAL_|ANCH|PEX2, 11,4, /*map0*/
        /*[5][9]*/       VAL_|BMAP|FLOW|FML1|PEX2,
        /*[6][10]*/         KEY_|SCLR|PLAI, 22,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 28,4, /*val0*/
        /*[8][16]*/         KEY_|SCLR|PLAI|PEX2, 38,4, /*key1*/
        /*[9][19]*/         VAL_|SCLR|PLAI|PEX2, 44,4, /*val1*/
        /*[10][22]*/       EMAP|PEX2,
        /*[11][23]*/       KEY_|SCLR|PLAI, 56,7, /*inherit*/
        /*[12][26]*/       VAL_|BMAP|FLOW|FML1|PEX2,
        /*[13][27]*/         KEY_|SCLR|PLAI, 71,2, /*<<*/
        /*[14][30]*/         VAL_|ALIA|PEX2, 76,4, /*map0*/
        /*[15][33]*/       EMAP|PEX2,
        /*[16][34]*/     EMAP,
        /*[17][35]*/   EDOC,
        /*[18][36]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP|FLOW|FML1,
        /*[3][3]*/       KEY_|SCLR|PLAI, 4,4, /*map0*/
        /*[4][6]*/       VAL_|PEX2, VAL_,VAL_, /*map0*/
        /*[5][9]*/       VAL_|BMAP|FLOW|FML1,
        /*[6][10]*/         KEY_|SCLR|PLAI, 22,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 28,4, /*val0*/
        /*[8][16]*/         KEY_|SCLR|PLAI|PEX2, 38,4, /*key1*/
        /*[9][19]*/         VAL_|SCLR|PLAI|PEX2, 44,4, /*val1*/
        /*[10][22]*/       EMAP|PEX2,
        /*[11][23]*/       KEY_|SCLR|PLAI, 56,7, /*inherit*/
        /*[12][26]*/       VAL_|BMAP|FLOW|FML1|PEX2,
        /*[13][27]*/         KEY_,KEY_,KEY_, /*<<*/
        /*[14][30]*/         VAL_|RREF, 10,22, /*map0*/
        /*[15][33]*/       EMAP|PEX2,
        /*[16][34]*/     EMAP,
        /*[17][35]*/   EDOC,
        /*[18][36]*/ ESTR,
    };
    test_ints_resolve(src, src, json, resolved, resolved_json, evts_expected, evts_resolved);
}

TEST(ints_resolve, 4_inherit_flow_sl)
{
    using namespace ievt;
    std::string src = ""
        "{ "
        "  map0: &map0 { "
        "    key0: val0, "
        "    key1: val1 "
        "  }, "
        "  inherit: { "
        "    <<: *map0 "
        "  } "
        "} "
        "";
    std::string emitted = ""
        "{map0: &map0 {key0: val0,key1: val1},inherit: {<<: *map0}}";
    std::string resolved = ""
        "{map0: {key0: val0,key1: val1},inherit: {key0: val0,key1: val1}}";
    std::string json = ""
        "{"
        "\"map0\": {"
        "\"key0\": \"val0\","
        "\"key1\": \"val1\""
        "},"
        "\"inherit\": {"
        "\"<<\": \"*map0\""
        "}"
        "}"
        "";
    std::string resolved_json = ""
        "{"
        "\"map0\": {"
        "\"key0\": \"val0\","
        "\"key1\": \"val1\""
        "},"
        "\"inherit\": {"
        "\"key0\": \"val0\","
        "\"key1\": \"val1\""
        "}"
        "}"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP|FLOW|FSL_,
        /*[3][3]*/       KEY_|SCLR|PLAI, 4,4, /*map0*/
        /*[4][6]*/       VAL_|ANCH|PEX2, 11,4, /*map0*/
        /*[5][9]*/       VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[6][10]*/         KEY_|SCLR|PLAI, 22,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 28,4, /*val0*/
        /*[8][16]*/         KEY_|SCLR|PLAI|PEX2, 38,4, /*key1*/
        /*[9][19]*/         VAL_|SCLR|PLAI|PEX2, 44,4, /*val1*/
        /*[10][22]*/       EMAP|PEX2,
        /*[11][23]*/       KEY_|SCLR|PLAI, 56,7, /*inherit*/
        /*[12][26]*/       VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[13][27]*/         KEY_|SCLR|PLAI, 71,2, /*<<*/
        /*[14][30]*/         VAL_|ALIA|PEX2, 76,4, /*map0*/
        /*[15][33]*/       EMAP|PEX2,
        /*[16][34]*/     EMAP,
        /*[17][35]*/   EDOC,
        /*[18][36]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP|FLOW|FSL_,
        /*[3][3]*/       KEY_|SCLR|PLAI, 4,4, /*map0*/
        /*[4][6]*/       VAL_|PEX2, VAL_,VAL_, /*map0*/
        /*[5][9]*/       VAL_|BMAP|FLOW|FSL_,
        /*[6][10]*/         KEY_|SCLR|PLAI, 22,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 28,4, /*val0*/
        /*[8][16]*/         KEY_|SCLR|PLAI|PEX2, 38,4, /*key1*/
        /*[9][19]*/         VAL_|SCLR|PLAI|PEX2, 44,4, /*val1*/
        /*[10][22]*/       EMAP|PEX2,
        /*[11][23]*/       KEY_|SCLR|PLAI, 56,7, /*inherit*/
        /*[12][26]*/       VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[13][27]*/         KEY_,KEY_,KEY_, /*<<*/
        /*[14][30]*/         VAL_|RREF, 10,22, /*map0*/
        /*[15][33]*/       EMAP|PEX2,
        /*[16][34]*/     EMAP,
        /*[17][35]*/   EDOC,
        /*[18][36]*/ ESTR,
    };
    test_ints_resolve(src, emitted, json, resolved, resolved_json, evts_expected, evts_resolved);
}



TEST(ints_resolve, 5_inherit_seq_block)
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
    std::string json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": \"val0\"\n"
        "  },\n"
        "  \"map1\": {\n"
        "    \"key1\": \"val1\"\n"
        "  },\n"
        "  \"map2\": {\n"
        "    \"key2\": \"val2\"\n"
        "  },\n"
        "  \"inherit\": {\n"
        "    \"<<\": [\"*map0\",\"*map1\",\"*map2\"]\n"
        "  }\n"
        "}\n"
        "";
    std::string resolved_json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": \"val0\"\n"
        "  },\n"
        "  \"map1\": {\n"
        "    \"key1\": \"val1\"\n"
        "  },\n"
        "  \"map2\": {\n"
        "    \"key2\": \"val2\"\n"
        "  },\n"
        "  \"inherit\": {\n"
        "    \"key0\": \"val0\",\n"
        "    \"key1\": \"val1\",\n"
        "    \"key2\": \"val2\"\n"
        "  }\n"
        "}\n"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4, /*map0*/
        /*[4][6]*/       VAL_|ANCH|PEX2, 7,4, /*map0*/
        /*[5][9]*/       VAL_|BMAP|PEX2,
        /*[6][10]*/         KEY_|SCLR|PLAI, 14,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 20,4, /*val0*/
        /*[8][16]*/       EMAP|PEX2,
        /*[9][17]*/       KEY_|SCLR|PLAI, 25,4, /*map1*/
        /*[10][20]*/       VAL_|ANCH|PEX2, 32,4, /*map1*/
        /*[11][23]*/       VAL_|BMAP|PEX2,
        /*[12][24]*/         KEY_|SCLR|PLAI, 39,4, /*key1*/
        /*[13][27]*/         VAL_|SCLR|PLAI|PEX2, 45,4, /*val1*/
        /*[14][30]*/       EMAP|PEX2,
        /*[15][31]*/       KEY_|SCLR|PLAI, 50,4, /*map2*/
        /*[16][34]*/       VAL_|ANCH|PEX2, 57,4, /*map2*/
        /*[17][37]*/       VAL_|BMAP|PEX2,
        /*[18][38]*/         KEY_|SCLR|PLAI, 64,4, /*key2*/
        /*[19][41]*/         VAL_|SCLR|PLAI|PEX2, 70,4, /*val2*/
        /*[20][44]*/       EMAP|PEX2,
        /*[21][45]*/       KEY_|SCLR|PLAI, 75,7, /*inherit*/
        /*[22][48]*/       VAL_|BMAP|PEX2,
        /*[23][49]*/         KEY_|SCLR|PLAI, 86,2, /*<<*/
        /*[24][52]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[25][53]*/           VAL_|ALIA, 92,4, /*map0*/
        /*[26][56]*/           VAL_|ALIA|PEX2, 98,4, /*map1*/
        /*[27][59]*/           VAL_|ALIA|PEX2, 104,4, /*map2*/
        /*[28][62]*/         ESEQ|PEX2,
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
        /*[4][6]*/       VAL_|PEX2, VAL_,VAL_, /*map0*/
        /*[5][9]*/       VAL_|BMAP,
        /*[6][10]*/         KEY_|SCLR|PLAI, 14,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 20,4, /*val0*/
        /*[8][16]*/       EMAP|PEX2,
        /*[9][17]*/       KEY_|SCLR|PLAI, 25,4, /*map1*/
        /*[10][20]*/       VAL_|PEX2, VAL_,VAL_, /*map1*/
        /*[11][23]*/       VAL_|BMAP,
        /*[12][24]*/         KEY_|SCLR|PLAI, 39,4, /*key1*/
        /*[13][27]*/         VAL_|SCLR|PLAI|PEX2, 45,4, /*val1*/
        /*[14][30]*/       EMAP|PEX2,
        /*[15][31]*/       KEY_|SCLR|PLAI, 50,4, /*map2*/
        /*[16][34]*/       VAL_|PEX2, VAL_,VAL_, /*map2*/
        /*[17][37]*/       VAL_|BMAP,
        /*[18][38]*/         KEY_|SCLR|PLAI, 64,4, /*key2*/
        /*[19][41]*/         VAL_|SCLR|PLAI|PEX2, 70,4, /*val2*/
        /*[20][44]*/       EMAP|PEX2,
        /*[21][45]*/       KEY_|SCLR|PLAI, 75,7, /*inherit*/
        /*[22][48]*/       VAL_|BMAP|PEX2,
        /*[23][49]*/         KEY_, KEY_,KEY_, /*<<*/
        /*[24][52]*/         VAL_,
        /*[25][53]*/         VAL_|RREF, 10,16, /*map0*/
        /*[26][56]*/         VAL_|RREF|PEX2, 24,30, /*map1*/
        /*[27][59]*/         VAL_|RREF|PEX2, 38,44, /*map2*/
        /*[28][62]*/         PEX2,
        /*[29][63]*/       EMAP,
        /*[30][64]*/     EMAP,
        /*[31][65]*/   EDOC,
        /*[32][66]*/ ESTR,
    };
    test_ints_resolve(src, src, json, resolved, resolved_json, evts_expected, evts_resolved);
}

TEST(ints_resolve, 5_inherit_seq_flowml)
{
    using namespace ievt;
    std::string src = ""
        "{\n"
        "  map0: &map0 {\n"
        "    key0: val0\n"
        "  },\n"
        "  map1: &map1 {\n"
        "    key1: val1\n"
        "  },\n"
        "  map2: &map2 {\n"
        "    key2: val2\n"
        "  },\n"
        "  inherit: {\n"
        "    <<: [*map0,*map1,*map2]\n"
        "  }\n"
        "}\n"
        "";
    std::string resolved = ""
        "{\n"
        "  map0: {\n"
        "    key0: val0\n"
        "  },\n"
        "  map1: {\n"
        "    key1: val1\n"
        "  },\n"
        "  map2: {\n"
        "    key2: val2\n"
        "  },\n"
        "  inherit: {\n"
        "    key0: val0,\n"
        "    key1: val1,\n"
        "    key2: val2\n"
        "  }\n"
        "}\n"
        "";
    std::string json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": \"val0\"\n"
        "  },\n"
        "  \"map1\": {\n"
        "    \"key1\": \"val1\"\n"
        "  },\n"
        "  \"map2\": {\n"
        "    \"key2\": \"val2\"\n"
        "  },\n"
        "  \"inherit\": {\n"
        "    \"<<\": [\"*map0\",\"*map1\",\"*map2\"]\n"
        "  }\n"
        "}\n"
        "";
    std::string resolved_json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": \"val0\"\n"
        "  },\n"
        "  \"map1\": {\n"
        "    \"key1\": \"val1\"\n"
        "  },\n"
        "  \"map2\": {\n"
        "    \"key2\": \"val2\"\n"
        "  },\n"
        "  \"inherit\": {\n"
        "    \"key0\": \"val0\",\n"
        "    \"key1\": \"val1\",\n"
        "    \"key2\": \"val2\"\n"
        "  }\n"
        "}\n"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP|FLOW|FML1,
        /*[3][3]*/       KEY_|SCLR|PLAI, 4,4, /*map0*/
        /*[4][6]*/       VAL_|ANCH|PEX2, 11,4, /*map0*/
        /*[5][9]*/       VAL_|BMAP|FLOW|FML1|PEX2,
        /*[6][10]*/         KEY_|SCLR|PLAI, 22,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 28,4, /*val0*/
        /*[8][16]*/       EMAP|PEX2,
        /*[9][17]*/       KEY_|SCLR|PLAI, 40,4, /*map1*/
        /*[10][20]*/       VAL_|ANCH|PEX2, 47,4, /*map1*/
        /*[11][23]*/       VAL_|BMAP|FLOW|FML1|PEX2,
        /*[12][24]*/         KEY_|SCLR|PLAI, 58,4, /*key1*/
        /*[13][27]*/         VAL_|SCLR|PLAI|PEX2, 64,4, /*val1*/
        /*[14][30]*/       EMAP|PEX2,
        /*[15][31]*/       KEY_|SCLR|PLAI, 76,4, /*map2*/
        /*[16][34]*/       VAL_|ANCH|PEX2, 83,4, /*map2*/
        /*[17][37]*/       VAL_|BMAP|FLOW|FML1|PEX2,
        /*[18][38]*/         KEY_|SCLR|PLAI, 94,4, /*key2*/
        /*[19][41]*/         VAL_|SCLR|PLAI|PEX2, 100,4, /*val2*/
        /*[20][44]*/       EMAP|PEX2,
        /*[21][45]*/       KEY_|SCLR|PLAI, 112,7, /*inherit*/
        /*[22][48]*/       VAL_|BMAP|FLOW|FML1|PEX2,
        /*[23][49]*/         KEY_|SCLR|PLAI, 127,2, /*<<*/
        /*[24][52]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[25][53]*/           VAL_|ALIA, 133,4, /*map0*/
        /*[26][56]*/           VAL_|ALIA|PEX2, 139,4, /*map1*/
        /*[27][59]*/           VAL_|ALIA|PEX2, 145,4, /*map2*/
        /*[28][62]*/         ESEQ|PEX2,
        /*[29][63]*/       EMAP,
        /*[30][64]*/     EMAP,
        /*[31][65]*/   EDOC,
        /*[32][66]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP|FLOW|FML1,
        /*[3][3]*/       KEY_|SCLR|PLAI, 4,4, /*map0*/
        /*[4][6]*/       VAL_|PEX2, VAL_,VAL_, /*map0*/
        /*[5][9]*/       VAL_|BMAP|FLOW|FML1,
        /*[6][10]*/         KEY_|SCLR|PLAI, 22,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 28,4, /*val0*/
        /*[8][16]*/       EMAP|PEX2,
        /*[9][17]*/       KEY_|SCLR|PLAI, 40,4, /*map1*/
        /*[10][20]*/       VAL_|PEX2, VAL_,VAL_, /*map1*/
        /*[11][23]*/       VAL_|BMAP|FLOW|FML1,
        /*[12][24]*/         KEY_|SCLR|PLAI, 58,4, /*key1*/
        /*[13][27]*/         VAL_|SCLR|PLAI|PEX2, 64,4, /*val1*/
        /*[14][30]*/       EMAP|PEX2,
        /*[15][31]*/       KEY_|SCLR|PLAI, 76,4, /*map2*/
        /*[16][34]*/       VAL_|PEX2, VAL_,VAL_, /*map2*/
        /*[17][37]*/       VAL_|BMAP|FLOW|FML1,
        /*[18][38]*/         KEY_|SCLR|PLAI, 94,4, /*key2*/
        /*[19][41]*/         VAL_|SCLR|PLAI|PEX2, 100,4, /*val2*/
        /*[20][44]*/       EMAP|PEX2,
        /*[21][45]*/       KEY_|SCLR|PLAI, 112,7, /*inherit*/
        /*[22][48]*/       VAL_|BMAP|FLOW|FML1|PEX2,
        /*[23][49]*/         KEY_, KEY_,KEY_, /*<<*/
        /*[24][52]*/         VAL_,
        /*[25][53]*/         VAL_|RREF, 10,16, /*map0*/
        /*[26][56]*/         VAL_|RREF|PEX2, 24,30, /*map1*/
        /*[27][59]*/         VAL_|RREF|PEX2, 38,44, /*map2*/
        /*[28][62]*/         PEX2,
        /*[29][63]*/       EMAP,
        /*[30][64]*/     EMAP,
        /*[31][65]*/   EDOC,
        /*[32][66]*/ ESTR,
    };
    test_ints_resolve(src, src, json, resolved, resolved_json, evts_expected, evts_resolved);
}

TEST(ints_resolve, 5_inherit_seq_flowsl)
{
    using namespace ievt;
    std::string src = ""
        "{ "
        "  map0: &map0 { "
        "    key0: val0 "
        "  }, "
        "  map1: &map1 { "
        "    key1: val1 "
        "  }, "
        "  map2: &map2 { "
        "    key2: val2 "
        "  }, "
        "  inherit: { "
        "    <<: [*map0,*map1,*map2] "
        "  } "
        "} "
        "";
    std::string emitted = ""
        "{"
        "map0: &map0 {key0: val0},"
        "map1: &map1 {key1: val1},"
        "map2: &map2 {key2: val2},"
        "inherit: {<<: [*map0,*map1,*map2]}"
        "}"
        "";
    std::string resolved = ""
        "{"
        "map0: {key0: val0},"
        "map1: {key1: val1},"
        "map2: {key2: val2},"
        "inherit: {key0: val0,key1: val1,key2: val2}"
        "}"
        "";
    std::string json = ""
        "{"
        "\"map0\": {"
        "\"key0\": \"val0\""
        "},"
        "\"map1\": {"
        "\"key1\": \"val1\""
        "},"
        "\"map2\": {"
        "\"key2\": \"val2\""
        "},"
        "\"inherit\": {"
        "\"<<\": [\"*map0\",\"*map1\",\"*map2\"]"
        "}"
        "}"
        "";
    std::string resolved_json = ""
        "{"
        "\"map0\": {"
        "\"key0\": \"val0\""
        "},"
        "\"map1\": {"
        "\"key1\": \"val1\""
        "},"
        "\"map2\": {"
        "\"key2\": \"val2\""
        "},"
        "\"inherit\": {"
        "\"key0\": \"val0\","
        "\"key1\": \"val1\","
        "\"key2\": \"val2\""
        "}"
        "}"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP|FLOW|FSL_,
        /*[3][3]*/       KEY_|SCLR|PLAI, 4,4, /*map0*/
        /*[4][6]*/       VAL_|ANCH|PEX2, 11,4, /*map0*/
        /*[5][9]*/       VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[6][10]*/         KEY_|SCLR|PLAI, 22,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 28,4, /*val0*/
        /*[8][16]*/       EMAP|PEX2,
        /*[9][17]*/       KEY_|SCLR|PLAI, 40,4, /*map1*/
        /*[10][20]*/       VAL_|ANCH|PEX2, 47,4, /*map1*/
        /*[11][23]*/       VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[12][24]*/         KEY_|SCLR|PLAI, 58,4, /*key1*/
        /*[13][27]*/         VAL_|SCLR|PLAI|PEX2, 64,4, /*val1*/
        /*[14][30]*/       EMAP|PEX2,
        /*[15][31]*/       KEY_|SCLR|PLAI, 76,4, /*map2*/
        /*[16][34]*/       VAL_|ANCH|PEX2, 83,4, /*map2*/
        /*[17][37]*/       VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[18][38]*/         KEY_|SCLR|PLAI, 94,4, /*key2*/
        /*[19][41]*/         VAL_|SCLR|PLAI|PEX2, 100,4, /*val2*/
        /*[20][44]*/       EMAP|PEX2,
        /*[21][45]*/       KEY_|SCLR|PLAI, 112,7, /*inherit*/
        /*[22][48]*/       VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[23][49]*/         KEY_|SCLR|PLAI, 127,2, /*<<*/
        /*[24][52]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[25][53]*/           VAL_|ALIA, 133,4, /*map0*/
        /*[26][56]*/           VAL_|ALIA|PEX2, 139,4, /*map1*/
        /*[27][59]*/           VAL_|ALIA|PEX2, 145,4, /*map2*/
        /*[28][62]*/         ESEQ|PEX2,
        /*[29][63]*/       EMAP,
        /*[30][64]*/     EMAP,
        /*[31][65]*/   EDOC,
        /*[32][66]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP|FLOW|FSL_,
        /*[3][3]*/       KEY_|SCLR|PLAI, 4,4, /*map0*/
        /*[4][6]*/       VAL_|PEX2, VAL_,VAL_, /*map0*/
        /*[5][9]*/       VAL_|BMAP|FLOW|FSL_,
        /*[6][10]*/         KEY_|SCLR|PLAI, 22,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 28,4, /*val0*/
        /*[8][16]*/       EMAP|PEX2,
        /*[9][17]*/       KEY_|SCLR|PLAI, 40,4, /*map1*/
        /*[10][20]*/       VAL_|PEX2, VAL_,VAL_, /*map1*/
        /*[11][23]*/       VAL_|BMAP|FLOW|FSL_,
        /*[12][24]*/         KEY_|SCLR|PLAI, 58,4, /*key1*/
        /*[13][27]*/         VAL_|SCLR|PLAI|PEX2, 64,4, /*val1*/
        /*[14][30]*/       EMAP|PEX2,
        /*[15][31]*/       KEY_|SCLR|PLAI, 76,4, /*map2*/
        /*[16][34]*/       VAL_|PEX2, VAL_,VAL_, /*map2*/
        /*[17][37]*/       VAL_|BMAP|FLOW|FSL_,
        /*[18][38]*/         KEY_|SCLR|PLAI, 94,4, /*key2*/
        /*[19][41]*/         VAL_|SCLR|PLAI|PEX2, 100,4, /*val2*/
        /*[20][44]*/       EMAP|PEX2,
        /*[21][45]*/       KEY_|SCLR|PLAI, 112,7, /*inherit*/
        /*[22][48]*/       VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[23][49]*/         KEY_, KEY_,KEY_, /*<<*/
        /*[24][52]*/         VAL_,
        /*[25][53]*/         VAL_|RREF, 10,16, /*map0*/
        /*[26][56]*/         VAL_|RREF|PEX2, 24,30, /*map1*/
        /*[27][59]*/         VAL_|RREF|PEX2, 38,44, /*map2*/
        /*[28][62]*/         PEX2,
        /*[29][63]*/       EMAP,
        /*[30][64]*/     EMAP,
        /*[31][65]*/   EDOC,
        /*[32][66]*/ ESTR,
    };
    test_ints_resolve(src, emitted, json, resolved, resolved_json, evts_expected, evts_resolved);
}

TEST(ints_resolve, 6_inherit_seq_with_more_members)
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
    std::string json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": \"val0\"\n"
        "  },\n"
        "  \"map1\": {\n"
        "    \"key1\": \"val1\"\n"
        "  },\n"
        "  \"map2\": {\n"
        "    \"key2\": \"val2\"\n"
        "  },\n"
        "  \"inherit\": {\n"
        "    \"<<\": [\"*map0\",\"*map1\",\"*map2\"]\n"
        "  },\n"
        "  \"another\": \"member\"\n"
        "}\n"
        "";
    std::string resolved_json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": \"val0\"\n"
        "  },\n"
        "  \"map1\": {\n"
        "    \"key1\": \"val1\"\n"
        "  },\n"
        "  \"map2\": {\n"
        "    \"key2\": \"val2\"\n"
        "  },\n"
        "  \"inherit\": {\n"
        "    \"key0\": \"val0\",\n"
        "    \"key1\": \"val1\",\n"
        "    \"key2\": \"val2\"\n"
        "  },\n"
        "  \"another\": \"member\"\n"
        "}\n"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4, /*map0*/
        /*[4][6]*/       VAL_|ANCH|PEX2, 7,4, /*map0*/
        /*[5][9]*/       VAL_|BMAP|PEX2,
        /*[6][10]*/         KEY_|SCLR|PLAI, 14,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 20,4, /*val0*/
        /*[8][16]*/       EMAP|PEX2,
        /*[9][17]*/       KEY_|SCLR|PLAI, 25,4, /*map1*/
        /*[10][20]*/       VAL_|ANCH|PEX2, 32,4, /*map1*/
        /*[11][23]*/       VAL_|BMAP|PEX2,
        /*[12][24]*/         KEY_|SCLR|PLAI, 39,4, /*key1*/
        /*[13][27]*/         VAL_|SCLR|PLAI|PEX2, 45,4, /*val1*/
        /*[14][30]*/       EMAP|PEX2,
        /*[15][31]*/       KEY_|SCLR|PLAI, 50,4, /*map2*/
        /*[16][34]*/       VAL_|ANCH|PEX2, 57,4, /*map2*/
        /*[17][37]*/       VAL_|BMAP|PEX2,
        /*[18][38]*/         KEY_|SCLR|PLAI, 64,4, /*key2*/
        /*[19][41]*/         VAL_|SCLR|PLAI|PEX2, 70,4, /*val2*/
        /*[20][44]*/       EMAP|PEX2,
        /*[21][45]*/       KEY_|SCLR|PLAI, 75,7, /*inherit*/
        /*[22][48]*/       VAL_|BMAP|PEX2,
        /*[23][49]*/         KEY_|SCLR|PLAI, 86,2, /*<<*/
        /*[24][52]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[25][53]*/           VAL_|ALIA, 92,4, /*map0*/
        /*[26][56]*/           VAL_|ALIA|PEX2, 98,4, /*map1*/
        /*[27][59]*/           VAL_|ALIA|PEX2, 104,4, /*map2*/
        /*[28][62]*/         ESEQ|PEX2,
        /*[29][63]*/       EMAP,
        /*[30][64]*/       KEY_|SCLR|PLAI, 110,7, /*another*/
        /*[31][67]*/       VAL_|SCLR|PLAI|PEX2, 119,6, /*member*/
        /*[32][70]*/     EMAP|PEX2,
        /*[33][71]*/   EDOC,
        /*[34][72]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4, /*map0*/
        /*[4][6]*/       VAL_|PEX2, VAL_,VAL_, /*map0*/
        /*[5][9]*/       VAL_|BMAP,
        /*[6][10]*/         KEY_|SCLR|PLAI, 14,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 20,4, /*val0*/
        /*[8][16]*/       EMAP|PEX2,
        /*[9][17]*/       KEY_|SCLR|PLAI, 25,4, /*map1*/
        /*[10][20]*/       VAL_|PEX2, VAL_,VAL_, /*map1*/
        /*[11][23]*/       VAL_|BMAP,
        /*[12][24]*/         KEY_|SCLR|PLAI, 39,4, /*key1*/
        /*[13][27]*/         VAL_|SCLR|PLAI|PEX2, 45,4, /*val1*/
        /*[14][30]*/       EMAP|PEX2,
        /*[15][31]*/       KEY_|SCLR|PLAI, 50,4, /*map2*/
        /*[16][34]*/       VAL_|PEX2, VAL_,VAL_, /*map2*/
        /*[17][37]*/       VAL_|BMAP,
        /*[18][38]*/         KEY_|SCLR|PLAI, 64,4, /*key2*/
        /*[19][41]*/         VAL_|SCLR|PLAI|PEX2, 70,4, /*val2*/
        /*[20][44]*/       EMAP|PEX2,
        /*[21][45]*/       KEY_|SCLR|PLAI, 75,7, /*inherit*/
        /*[22][48]*/       VAL_|BMAP|PEX2,
        /*[23][49]*/         KEY_, KEY_,KEY_, /*<<*/
        /*[24][52]*/         VAL_,
        /*[25][53]*/         VAL_|RREF, 10,16, /*map0*/
        /*[26][56]*/         VAL_|RREF|PEX2, 24,30, /*map1*/
        /*[27][59]*/         VAL_|RREF|PEX2, 38,44, /*map2*/
        /*[28][62]*/         PEX2,
        /*[29][63]*/       EMAP,
        /*[30][64]*/       KEY_|SCLR|PLAI, 110,7, /*another*/
        /*[31][67]*/       VAL_|SCLR|PLAI|PEX2, 119,6, /*member*/
        /*[32][70]*/     EMAP|PEX2,
        /*[33][71]*/   EDOC,
        /*[34][72]*/ ESTR,
    };
    test_ints_resolve(src, src, json, resolved, resolved_json, evts_expected, evts_resolved);
}


//-----------------------------------------------------------------------------

TEST(ints_resolve, 7_inherit_seq_merge)
{
    using namespace ievt;
    std::string src = ""
        "map0: &map0\n"
        "  key0: val0\n"
        "map1: &map1\n"
        "  key0: val01\n"
        "  key1: val1\n"
        "map2: &map2\n"
        "  key0: val02\n"
        "  key1: val12\n"
        "  key2: val2\n"
        "inherit:\n"
        "  <<: [*map0,*map1,*map2]\n"
        "another: member\n"
        "";
    std::string resolved = ""
        "map0:\n"
        "  key0: val0\n"
        "map1:\n"
        "  key0: val01\n"
        "  key1: val1\n"
        "map2:\n"
        "  key0: val02\n"
        "  key1: val12\n"
        "  key2: val2\n"
        "inherit:\n"
        "  key0: val0\n"
        "  key1: val1\n"
        "  key2: val2\n"
        "another: member\n"
        "";
    std::string json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": \"val0\"\n"
        "  },\n"
        "  \"map1\": {\n"
        "    \"key0\": \"val01\",\n"
        "    \"key1\": \"val1\"\n"
        "  },\n"
        "  \"map2\": {\n"
        "    \"key0\": \"val02\",\n"
        "    \"key1\": \"val12\",\n"
        "    \"key2\": \"val2\"\n"
        "  },\n"
        "  \"inherit\": {\n"
        "    \"<<\": [\"*map0\",\"*map1\",\"*map2\"]\n"
        "  },\n"
        "  \"another\": \"member\"\n"
        "}\n"
        "";
    std::string resolved_json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": \"val0\"\n"
        "  },\n"
        "  \"map1\": {\n"
        "    \"key0\": \"val01\",\n"
        "    \"key1\": \"val1\"\n"
        "  },\n"
        "  \"map2\": {\n"
        "    \"key0\": \"val02\",\n"
        "    \"key1\": \"val12\",\n"
        "    \"key2\": \"val2\"\n"
        "  },\n"
        "  \"inherit\": {\n"
        "    \"key0\": \"val0\",\n"
        "    \"key1\": \"val1\",\n"
        "    \"key2\": \"val2\"\n"
        "  },\n"
        "  \"another\": \"member\"\n"
        "}\n"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4,/*map0*/
        /*[4][6]*/       VAL_|ANCH|PEX2, 7,4,/*map0*/
        /*[5][9]*/       VAL_|BMAP|PEX2,
        /*[6][10]*/         KEY_|SCLR|PLAI, 14,4,/*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 20,4,/*val0*/
        /*[8][16]*/       EMAP|PEX2,
        /*[9][17]*/       KEY_|SCLR|PLAI, 25,4,/*map1*/
        /*[10][20]*/       VAL_|ANCH|PEX2, 32,4,/*map1*/
        /*[11][23]*/       VAL_|BMAP|PEX2,
        /*[12][24]*/         KEY_|SCLR|PLAI, 39,4,/*key0*/
        /*[13][27]*/         VAL_|SCLR|PLAI|PEX2, 45,5,/*val01*/
        /*[14][30]*/         KEY_|SCLR|PLAI|PEX2, 53,4,/*key1*/
        /*[15][33]*/         VAL_|SCLR|PLAI|PEX2, 59,4,/*val1*/
        /*[16][36]*/       EMAP|PEX2,
        /*[17][37]*/       KEY_|SCLR|PLAI, 64,4,/*map2*/
        /*[18][40]*/       VAL_|ANCH|PEX2, 71,4,/*map2*/
        /*[19][43]*/       VAL_|BMAP|PEX2,
        /*[20][44]*/         KEY_|SCLR|PLAI, 78,4,/*key0*/
        /*[21][47]*/         VAL_|SCLR|PLAI|PEX2, 84,5,/*val02*/
        /*[22][50]*/         KEY_|SCLR|PLAI|PEX2, 92,4,/*key1*/
        /*[23][53]*/         VAL_|SCLR|PLAI|PEX2, 98,5,/*val12*/
        /*[24][56]*/         KEY_|SCLR|PLAI|PEX2, 106,4,/*key2*/
        /*[25][59]*/         VAL_|SCLR|PLAI|PEX2, 112,4,/*val2*/
        /*[26][62]*/       EMAP|PEX2,
        /*[27][63]*/       KEY_|SCLR|PLAI, 117,7,/*inherit*/
        /*[28][66]*/       VAL_|BMAP|PEX2,
        /*[29][67]*/         KEY_|SCLR|PLAI, 128,2,/*<<*/
        /*[30][70]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[31][71]*/           VAL_|ALIA, 134,4,/*map0*/
        /*[32][74]*/           VAL_|ALIA|PEX2, 140,4,/*map1*/
        /*[33][77]*/           VAL_|ALIA|PEX2, 146,4,/*map2*/
        /*[34][80]*/         ESEQ|PEX2,
        /*[35][81]*/       EMAP,
        /*[36][82]*/       KEY_|SCLR|PLAI, 152,7,/*another*/
        /*[37][85]*/       VAL_|SCLR|PLAI|PEX2, 161,6,/*member*/
        /*[38][88]*/     EMAP|PEX2,
        /*[39][89]*/   EDOC,
        /*[40][90]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4,/*map0*/
        /*[4][6]*/       VAL_|PEX2,
        /*[5][7]*/       VAL_,
        /*[6][8]*/       VAL_,
        /*[7][9]*/       VAL_|BMAP,
        /*[8][10]*/         KEY_|SCLR|PLAI, 14,4,/*key0*/
        /*[9][13]*/         VAL_|SCLR|PLAI|PEX2, 20,4,/*val0*/
        /*[10][16]*/       EMAP|PEX2,
        /*[11][17]*/       KEY_|SCLR|PLAI, 25,4,/*map1*/
        /*[12][20]*/       VAL_|PEX2,
        /*[13][21]*/       VAL_,
        /*[14][22]*/       VAL_,
        /*[15][23]*/       VAL_|BMAP,
        /*[16][24]*/         KEY_|SCLR|PLAI, 39,4,/*key0*/
        /*[17][27]*/         VAL_|SCLR|PLAI|PEX2, 45,5,/*val01*/
        /*[18][30]*/         KEY_|SCLR|PLAI|PEX2, 53,4,/*key1*/
        /*[19][33]*/         VAL_|SCLR|PLAI|PEX2, 59,4,/*val1*/
        /*[20][36]*/       EMAP|PEX2,
        /*[21][37]*/       KEY_|SCLR|PLAI, 64,4,/*map2*/
        /*[22][40]*/       VAL_|PEX2,
        /*[23][41]*/       VAL_,
        /*[24][42]*/       VAL_,
        /*[25][43]*/       VAL_|BMAP,
        /*[26][44]*/         KEY_|SCLR|PLAI, 78,4,/*key0*/
        /*[27][47]*/         VAL_|SCLR|PLAI|PEX2, 84,5,/*val02*/
        /*[28][50]*/         KEY_|SCLR|PLAI|PEX2, 92,4,/*key1*/
        /*[29][53]*/         VAL_|SCLR|PLAI|PEX2, 98,5,/*val12*/
        /*[30][56]*/         KEY_|SCLR|PLAI|PEX2, 106,4,/*key2*/
        /*[31][59]*/         VAL_|SCLR|PLAI|PEX2, 112,4,/*val2*/
        /*[32][62]*/       EMAP|PEX2,
        /*[33][63]*/       KEY_|SCLR|PLAI, 117,7,/*inherit*/
        /*[34][66]*/       VAL_|BMAP|PEX2,
        /*[35][67]*/         KEY_,
        /*[36][68]*/         KEY_,
        /*[37][69]*/         KEY_,
        /*[38][70]*/         VAL_,
        /*[39][71]*/         VAL_|RREF, 10,16,
        /*[40][74]*/         VAL_|RREF|PEX2, 91,97,
        /*[41][77]*/         VAL_|RREF|PEX2, 97,103,
        /*[42][80]*/         PEX2,
        /*[43][81]*/       EMAP,
        /*[44][82]*/       KEY_|SCLR|PLAI, 152,7,/*another*/
        /*[45][85]*/       VAL_|SCLR|PLAI|PEX2, 161,6,/*member*/
        /*[46][88]*/     EMAP|PEX2,
        /*[47][89]*/   EDOC,
        /*[48][90]*/ ESTR,
        // these entries had to be added while merging:
        /*[49][91]*/         KEY_|SCLR|PLAI, 53,4,/*key1*/
        /*[50][94]*/         VAL_|SCLR|PLAI|PEX2, 59,4,/*val1*/
        /*[51][97]*/         KEY_|SCLR|PLAI|PEX2, 106,4,/*key2*/
        /*[52][100]*/         VAL_|SCLR|PLAI|PEX2, 112,4,/*val2*/
        /*[53][103]*/         KEY_|PEX2,
    };
    test_ints_resolve(src, src, json, resolved, resolved_json, evts_expected, evts_resolved);
}

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
