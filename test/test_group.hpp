#pragma once
#ifndef C4_RYML_TEST_GROUP_HPP_
#define C4_RYML_TEST_GROUP_HPP_

#include "./test_case.hpp"
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
        // Code here will be called immediately after the constructor (right
        // before each test).
        std::cout << "-------------------------------------------\n";
        std::cout << "running test case '" << name << "'\n";
        std::cout << "-------------------------------------------\n";
    }

    void _test_parse_using_ryml(CaseDataLineEndings *cd);
    void _test_emit_yml_stdout(CaseDataLineEndings *cd);
    void _test_emit_yml_cout(CaseDataLineEndings *cd);
    void _test_emit_yml_stringstream(CaseDataLineEndings *cd);
    void _test_emit_yml_ofstream(CaseDataLineEndings *cd);
    void _test_emit_yml_string(CaseDataLineEndings *cd);
    void _test_emitrs(CaseDataLineEndings *cd);
    void _test_emitrs_cfile(CaseDataLineEndings *cd);
    void _test_complete_round_trip(CaseDataLineEndings *cd);
    void _test_recreate_from_ref(CaseDataLineEndings *cd);
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// facilities for declaring test data

using N = CaseNode;
using L = CaseNode::iseqmap;
using TS = TaggedScalar;
using TL = CaseNode::TaggedList;
using AR = AnchorRef;

constexpr const NodeType_e QK = (NodeType_e)(VAL | KEYQUO);
constexpr const NodeType_e QV = (NodeType_e)(VAL | VALQUO);
constexpr const NodeType_e QKV = (NodeType_e)(VAL | KEYQUO | VALQUO);

#ifdef __GNUC__
#if __GNUC__ == 4 && __GNUC_MINOR__ >= 8
struct CaseAdder {
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
/* fwd declaration to fill the container with cases */                  \
void add_cases_##group_name(std::vector<Case> *group_cases);            \
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
            if(cp[i] == cp[i+1])                                        \
                C4_ERROR("duplicate case name: '%.*s'", _c4prsp(cp[i])); \
    }                                                                   \
    return case_names_##group_name;                                     \
}                                                                       \
                                                                        \
INSTANTIATE_TEST_SUITE_P(group_name, YmlTestCase, ::testing::ValuesIn(get_case_names_##group_name())); \
GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(YmlTestCase);             \
                                                                        \
/* used by the fixture to obtain a case by name */                      \
Case const* get_case(csubstr name)                                      \
{                                                                       \
    for(Case const& c : get_cases_##group_name())                       \
        if(c.name == name)                                              \
            return &c;                                                  \
    C4_ERROR("case not found: '%.*s'", _c4prsp(name));                  \
    return nullptr;                                                     \
}                                                                       \
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
