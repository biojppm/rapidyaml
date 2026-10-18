#pragma once
#ifndef C4_RYML_TEST_GROUP_HPP_
#define C4_RYML_TEST_GROUP_HPP_

#include "./test_lib/test_case.hpp"
#include "c4/span.hpp"
#include <algorithm>

C4_SUPPRESS_WARNING_PUSH
C4_SUPPRESS_WARNING_MSVC(4068/*unknown pragma*/)
C4_SUPPRESS_WARNING_MSVC(4702/*unreachable code*/)
C4_SUPPRESS_WARNING_CLANG("-Wgnu-zero-variadic-macro-arguments")
C4_SUPPRESS_WARNING_GCC("-Wunknown-pragmas")
#if defined(__GNUC__) && (__GNUC__ > 5)
C4_SUPPRESS_WARNING_GCC("-Wunused-const-variable")
//C4_SUPPRESS_WARNING_GCC("-Wpragma-system-header-outside-header")
#endif

#if defined(RYML_WITH_TAB_TOKENS)
#define RYML_WITH_TAB_TOKENS_(...) __VA_ARGS__
#define RYML_WITHOUT_TAB_TOKENS_(...)
#define RYML_WITH_OR_WITHOUT_TAB_TOKENS_(with, without) with
#else
#define RYML_WITH_TAB_TOKENS_(...)
#define RYML_WITHOUT_TAB_TOKENS_(...) __VA_ARGS__
#define RYML_WITH_OR_WITHOUT_TAB_TOKENS_(with, without) without
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

    void _test_parse_yaml_to_tree(CaseDataLineEndings *cd);
    void _test_parse_yaml_to_ints_noresize(CaseDataLineEndings *cd);
    void _test_parse_yaml_to_ints_resize(CaseDataLineEndings *cd);

    void _test_roundtrip_yaml_tree(CaseDataLineEndings *cd);
    void _test_roundtrip_yaml_ints_resize(CaseDataLineEndings *cd);
    void _test_roundtrip_yaml_ints_noresize(CaseDataLineEndings *cd);

    void _test_roundtrip_json_tree(CaseDataLineEndings *cd);
    void _test_roundtrip_json_ints_resize(CaseDataLineEndings *cd);
    void _test_roundtrip_json_ints_noresize(CaseDataLineEndings *cd);

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

constexpr const type_bits KP = (KEY|KEY_PLAIN);   ///< key, plain scalar
constexpr const type_bits KN = (KEY|KEY_PLAIN|KEYNIL); ///< key, plain scalar, nil
constexpr const type_bits KS = (KEY|KEY_SQUO);    ///< key, single-quoted scalar
constexpr const type_bits KD = (KEY|KEY_DQUO);    ///< key, double-quoted scalar
constexpr const type_bits KL = (KEY|KEY_LITERAL); ///< key, block-literal scalar
constexpr const type_bits KF = (KEY|KEY_FOLDED);  ///< key, block-folded scalar

constexpr const type_bits VP = (VAL|VAL_PLAIN);   ///< val, plain scalar
constexpr const type_bits VN = (VAL|VAL_PLAIN|VALNIL); ///< val, plain scalar, nil
constexpr const type_bits VS = (VAL|VAL_SQUO);    ///< val, single-quoted scalar
constexpr const type_bits VD = (VAL|VAL_DQUO);    ///< val, double-quoted scalar
constexpr const type_bits VL = (VAL|VAL_LITERAL); ///< val, block-literal scalar
constexpr const type_bits VF = (VAL|VAL_FOLDED);  ///< val, block-folded scalar

constexpr const type_bits SB = (SEQ|BLOCK);       ///< sequence, block-style
constexpr const type_bits SFS = (SEQ|FLOW_SL);    ///< sequence, flow-style, single-line
constexpr const type_bits SFM = (SEQ|FLOW_ML1);   ///< sequence, flow-style, multi-line

constexpr const type_bits MB = (MAP|BLOCK);       ///< map, flow-style
constexpr const type_bits MFS = (MAP|FLOW_SL);    ///< map, flow-style, single-line
constexpr const type_bits MFM = (MAP|FLOW_ML1);   ///< map, flow-style, multi-line

C4_SUPPRESS_WARNING_GCC_POP


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// utilities to create the parameterized cases for each test group

#if !(defined(__GNUC__) && (__GNUC__ == 4) && (__GNUC_MINOR__ >= 8))

/** use this macro to add a case (of type @ref Case) to the test group. */
#define ADD_CASE_TO_GROUP(...)                  \
    group_cases__->emplace_back(csubstr(__FILE__), __LINE__+1, __VA_ARGS__)

#else

struct CaseAdderGcc4_8
{
    std::vector<Case> *group_cases;
    const csubstr file;
    const int line;

    template<typename... Args>
    void operator ()(Args &&... parameters) const {
        group_cases->emplace_back(csubstr(file), line, std::forward<Args>(parameters)...);
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

C4_SUPPRESS_WARNING_PUSH

#endif // C4_RYML_TEST_GROUP_HPP_
