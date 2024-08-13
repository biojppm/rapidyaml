#pragma once
#ifndef C4_RYML_TEST_GROUP_HPP_
#define C4_RYML_TEST_GROUP_HPP_

#include "./test_lib/test_case.hpp"
#include "c4/span.hpp"
#include <algorithm>

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable: 4068/*unknown pragma*/)
#   pragma warning(disable: 4702/*unreachable code*/)
#elif defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#elif defined(__GNUC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wunknown-pragmas"
#   if __GNUC__ > 5
#       pragma GCC diagnostic ignored "-Wunused-const-variable"
#   endif
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
            parse_in_place(c->fileline, cd->src, &cd->parsed_tree);
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

#if defined(__GNUC__) && (__GNUC__ > 5)
C4_SUPPRESS_WARNING_GCC("-Wunused-const-variable")
#endif

constexpr const NodeType_e KP = (KEY|KEY_PLAIN);   ///< key, plain scalar
constexpr const NodeType_e KS = (KEY|KEY_SQUO);    ///< key, single-quoted scalar
constexpr const NodeType_e KD = (KEY|KEY_DQUO);    ///< key, double-quoted scalar
constexpr const NodeType_e KL = (KEY|KEY_LITERAL); ///< key, block-literal scalar
constexpr const NodeType_e KF = (KEY|KEY_FOLDED);  ///< key, block-folded scalar

constexpr const NodeType_e VP = (VAL|VAL_PLAIN);   ///< val, plain scalar
constexpr const NodeType_e VS = (VAL|VAL_SQUO);    ///< val, single-quoted scalar
constexpr const NodeType_e VD = (VAL|VAL_DQUO);    ///< val, double-quoted scalar
constexpr const NodeType_e VL = (VAL|VAL_LITERAL); ///< val, block-literal scalar
constexpr const NodeType_e VF = (VAL|VAL_FOLDED);  ///< val, block-folded scalar

constexpr const NodeType_e SB = (SEQ|BLOCK);       ///< sequence, block-style
constexpr const NodeType_e SFS = (SEQ|FLOW_SL);    ///< sequence, flow-style, single-line
constexpr const NodeType_e SFM = (SEQ|FLOW_ML);    ///< sequence, flow-style, multi-line

constexpr const NodeType_e MB = (MAP|BLOCK);       ///< map, flow-style
constexpr const NodeType_e MFS = (MAP|FLOW_SL);    ///< map, flow-style, single-line
constexpr const NodeType_e MFM = (MAP|FLOW_ML);    ///< map, flow-style, multi-line

C4_SUPPRESS_WARNING_GCC_POP


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// utilities to create the parameterized cases for each test group

#if !(defined(__GNUC__) && (__GNUC__ == 4) && (__GNUC_MINOR__ >= 8))

/** use this macro to add a case to the test group. */
#define ADD_CASE_TO_GROUP(...)                  \
    group_cases__->emplace_back(csubstr(__FILE__), __LINE__+1, __VA_ARGS__)

#else

struct CaseAdderGcc4_8
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
#define ADD_CASE_TO_GROUP CaseAdderGcc4_8{group_cases__, csubstr(__FILE__), __LINE__+1}

#endif

/** declares a function where we can call ADD_CASE_TO_GROUP()
 * to populate the group */
#define CASE_GROUP(group_name)                                          \
                                                                        \
                                                                        \
/* fwd-declare a function to fill a container of case data */           \
void add_cases_##group_name(std::vector<Case> *group_cases);            \
                                                                        \
                                                                        \
/* container with cases data. not the parameterized container */        \
std::vector<Case> const& get_cases_##group_name()                       \
{                                                                       \
    static std::vector<Case> cases_##group_name;                        \
    if(cases_##group_name.empty())                                      \
        add_cases_##group_name(&cases_##group_name);                    \
    return cases_##group_name;                                          \
}                                                                       \
                                                                        \
                                                                        \
/* container with case names. this is the parameterized container. */   \
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
                                                                        \
/* used by the fixture to obtain a case by name */                      \
Case const* get_case(csubstr name)                                      \
{                                                                       \
    for(Case const& c : get_cases_##group_name())                       \
        if(c.name == name)                                              \
            return &c;                                                  \
    printf("case not found: '%.*s' defs_included=%d\n",                 \
           (int)name.len, name.str,                                     \
           /*call this function to ensure the tests were included*/     \
           YmlTestCaseDefsWereIncluded());                              \
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
#   pragma warning(disable: 4702/*unreachable code*/)
#elif defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__GNUC__)
#   pragma GCC diagnostic pop
#endif

#endif // C4_RYML_TEST_GROUP_HPP_
