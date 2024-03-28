#pragma once
#ifndef C4_RYML_TEST_GROUP_HPP_
#define C4_RYML_TEST_GROUP_HPP_

#include "./test_lib/test_case.hpp"
#include "c4/span.hpp"
#include <algorithm>

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable: 4068/*unknown pragma*/)
#elif defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#elif defined(__GNUC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wunknown-pragmas"
//#   pragma GCC diagnostic ignored "-Wpragma-system-header-outside-header"
#endif

#if defined(RYML_WITH_TAB_TOKENS)
#define _RYML_WITH_TAB_TOKENS(...) __VA_ARGS__
#define _RYML_WITHOUT_TAB_TOKENS(...)
#define _RYML_WITH_OR_WITHOUT_TAB_TOKENS(with, without) with
#else
#define _RYML_WITH_TAB_TOKENS(...)
#define _RYML_WITHOUT_TAB_TOKENS(...) __VA_ARGS__
#define _RYML_WITH_OR_WITHOUT_TAB_TOKENS(with, without) without
#endif

namespace c4 {
namespace yml {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// a fixture for running the tests
struct YmlTestCase : public ::testing::TestWithParam<csubstr>
{
    csubstr const name;
    Case const* c;
    CaseData * d;

    YmlTestCase() : name(to_csubstr(GetParam()))
    {
        c = get_case(name);
        d = get_data(name);
    }

    void SetUp() override
    {
        _show_origin();
    }

    void TearDown() override
    {
        #ifdef RYML_DBG
        _show_origin();
        #endif
    }

    void _show_origin()
    {
        std::cout << "-------------------------------------------\n";
        std::cout << c->filelinebuf << ": " << name << "'\n";
        std::cout << "-------------------------------------------\n";
    }

    void _test_parse_using_ryml(CaseDataLineEndings *cd);

    void _test_emit_yml_stdout(CaseDataLineEndings *cd);
    void _test_emit_json_stdout(CaseDataLineEndings *cd);

    void _test_emit_yml_cout(CaseDataLineEndings *cd);
    void _test_emit_json_cout(CaseDataLineEndings *cd);

    void _test_emit_yml_stringstream(CaseDataLineEndings *cd);
    void _test_emit_json_stringstream(CaseDataLineEndings *cd);

    void _test_emit_yml_ofstream(CaseDataLineEndings *cd);
    void _test_emit_json_ofstream(CaseDataLineEndings *cd);

    void _test_emit_yml_string(CaseDataLineEndings *cd);
    void _test_emit_json_string(CaseDataLineEndings *cd);

    void _test_emitrs(CaseDataLineEndings *cd);
    void _test_emitrs_json(CaseDataLineEndings *cd);

    void _test_emitrs_cfile(CaseDataLineEndings *cd);
    void _test_emitrs_json_cfile(CaseDataLineEndings *cd);

    void _test_complete_round_trip(CaseDataLineEndings *cd);
    void _test_complete_round_trip_json(CaseDataLineEndings *cd);

    void _test_recreate_from_ref(CaseDataLineEndings *cd);

    void _ensure_parse(CaseDataLineEndings *cd)
    {
        if(cd->parsed_tree.empty())
            parse_json_in_place(c->fileline, cd->src, &cd->parsed_tree);
    }
    void _ensure_emit(CaseDataLineEndings *cd)
    {
        _ensure_parse(cd);
        if(cd->emit_buf.empty())
        {
            cd->emitted_yml = emitrs_yaml(cd->parsed_tree, &cd->emit_buf);
            ASSERT_EQ(cd->emitted_yml.size(), cd->emit_buf.size());
            if(cd->emitted_yml.size())
            {
                ASSERT_EQ(cd->emitted_yml.data(), cd->emit_buf.data());
            }
        }
    }
    void _ensure_emit_json(CaseDataLineEndings *cd)
    {
        _ensure_parse(cd);
        if(cd->emitjson_buf.empty())
        {
            cd->emitted_json = emitrs_json(cd->parsed_tree, &cd->emitjson_buf);
            ASSERT_EQ(cd->emitted_json.size(), cd->emitjson_buf.size());
            if(cd->emitted_json.size())
            {
                ASSERT_EQ(cd->emitted_json.data(), cd->emitjson_buf.data());
            }
        }
    }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// facilities for declaring test data

using N = TestCaseNode;
using L = TestCaseNode::iseqmap;
using TS = TaggedScalar;
using TL = TestCaseNode::TaggedList;
using AR = AnchorRef;

C4_SUPPRESS_WARNING_GCC_PUSH
#if __GNUC__ > 5
C4_SUPPRESS_WARNING_GCC("-Wunused-const-variable")
#endif

constexpr const NodeType_e KP = (KEY|_WIP_KEY_PLAIN);   ///< key, plain scalar
constexpr const NodeType_e KS = (KEY|_WIP_KEY_SQUO);    ///< key, single-quoted scalar
constexpr const NodeType_e KD = (KEY|_WIP_KEY_DQUO);    ///< key, double-quoted scalar
constexpr const NodeType_e KL = (KEY|_WIP_KEY_LITERAL); ///< key, block-literal scalar
constexpr const NodeType_e KF = (KEY|_WIP_KEY_FOLDED);  ///< key, block-folded scalar

constexpr const NodeType_e VP = (VAL|_WIP_VAL_PLAIN);   ///< val, plain scalar
constexpr const NodeType_e VS = (VAL|_WIP_VAL_SQUO);    ///< val, single-quoted scalar
constexpr const NodeType_e VD = (VAL|_WIP_VAL_DQUO);    ///< val, double-quoted scalar
constexpr const NodeType_e VL = (VAL|_WIP_VAL_LITERAL); ///< val, block-literal scalar
constexpr const NodeType_e VF = (VAL|_WIP_VAL_FOLDED);  ///< val, block-folded scalar

constexpr const NodeType_e SB = (SEQ|_WIP_STYLE_BLOCK);    ///< sequence, block-style
constexpr const NodeType_e SFS = (SEQ|_WIP_STYLE_FLOW_SL); ///< sequence, flow-style
constexpr const NodeType_e SMS = (SEQ|_WIP_STYLE_FLOW_ML); ///< sequence, flow-style

constexpr const NodeType_e MB = (MAP|_WIP_STYLE_BLOCK);    ///< map, flow-style
constexpr const NodeType_e MFS = (MAP|_WIP_STYLE_FLOW_SL); ///< map, flow-style
constexpr const NodeType_e MMS = (MAP|_WIP_STYLE_FLOW_ML); ///< map, flow-style

constexpr const NodeType_e QK = (VAL | KEYQUO);
constexpr const NodeType_e QV = (VAL | VALQUO);
constexpr const NodeType_e QKV = (VAL | KEYQUO | VALQUO);

C4_SUPPRESS_WARNING_GCC_POP


#ifdef __GNUC__
#if __GNUC__ == 4 && __GNUC_MINOR__ >= 8
struct CaseAdder
{
    std::vector<Case> *group_cases;
    const csubstr file;
    const int line;

    template<typename... Args>
    void operator ()(Args... parameters) const {
        group_cases->emplace_back(csubstr(file), line, parameters...);
    }
};

/* all arguments are to the constructor of Case */
#define ADD_CASE_TO_GROUP CaseAdder{group_cases__, csubstr(__FILE__), __LINE__+1}
#endif
#endif

#ifndef ADD_CASE_TO_GROUP
#define ADD_CASE_TO_GROUP(...)                  \
    group_cases__->emplace_back(csubstr(__FILE__), __LINE__+1, __VA_ARGS__)
#endif

#define CASE_GROUP(group_name)                                          \
                                                                        \
                                                                        \
/* fwd declaration to fill the container with cases */                  \
void add_cases_##group_name(std::vector<Case> *group_cases);            \
                                                                        \
                                                                        \
/* container with the cases */                                          \
std::vector<Case> const& get_cases_##group_name()                       \
{                                                                       \
    static std::vector<Case> cases_##group_name;                        \
    if(cases_##group_name.empty())                                      \
        add_cases_##group_name(&cases_##group_name);                    \
    return cases_##group_name;                                          \
}                                                                       \
                                                                        \
                                                                        \
/* container with the case names */                                     \
std::vector<csubstr> const& get_case_names_##group_name()               \
{                                                                       \
    static std::vector<csubstr> case_names_##group_name;                \
    if(case_names_##group_name.empty())                                 \
    {                                                                   \
        for(auto const& c : get_cases_##group_name())                   \
            case_names_##group_name.emplace_back(c.name);               \
        /* check repetitions */                                         \
        std::vector<csubstr> cp = case_names_##group_name;              \
        std::sort(cp.begin(), cp.end());                                \
        for(size_t i = 0; i+1 < cp.size(); ++i)                         \
        {                                                               \
            if(cp[i] == cp[i+1])                                        \
            {                                                           \
                printf("duplicate case name: '%.*s'", (int)cp[i].len, cp[i].str);  \
                C4_ERROR("duplicate case name: '%.*s'", (int)cp[i].len, cp[i].str);  \
            }                                                           \
        }                                                               \
    }                                                                   \
    return case_names_##group_name;                                     \
}                                                                       \
                                                                        \
                                                                        \
INSTANTIATE_TEST_SUITE_P(group_name, YmlTestCase, ::testing::ValuesIn(get_case_names_##group_name())); \
                                                                        \
GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(YmlTestCase);             \
                                                                        \
                                                                        \
/* used by the fixture to obtain a case by name */                      \
Case const* get_case(csubstr name)                                      \
{                                                                       \
    for(Case const& c : get_cases_##group_name())                       \
        if(c.name == name)                                              \
            return &c;                                                  \
    printf("case not found: '%.*s'", (int)name.len, name.str);          \
    C4_ERROR("case not found: '%.*s'", (int)name.len, name.str);        \
    return nullptr;                                                     \
}                                                                       \
                                                                        \
                                                                        \
/* finally, define the cases by calling ADD_CASE_TO_GROUP() */          \
void add_cases_##group_name(std::vector<Case> *group_cases__)


} // namespace yml
} // namespace c4

#if defined(_MSC_VER)
#   pragma warning(pop)
#elif defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__GNUC__)
#   pragma GCC diagnostic pop
#endif

#endif // C4_RYML_TEST_GROUP_HPP_
