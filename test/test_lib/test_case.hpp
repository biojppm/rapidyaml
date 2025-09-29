#ifndef _TEST_CASE_HPP_
#define _TEST_CASE_HPP_

#ifdef RYML_SINGLE_HEADER
#include <ryml_all.hpp>
#else
#include "c4/std/vector.hpp"
#include "c4/std/string.hpp"
#include "c4/format.hpp"
#include <c4/yml/yml.hpp>
#include <c4/yml/detail/dbgprint.hpp>
#include <c4/yml/detail/print.hpp>
#endif

#include <gtest/gtest.h>
#include <functional>


// no pragma push for these warnings! they will be suppressed in the
// files including this header (most test files)
#ifdef __clang__
#   pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#   pragma GCC diagnostic ignored "-Wold-style-cast"
#endif


#ifdef __clang__
#   pragma clang diagnostic push
#elif defined(__GNUC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wtype-limits"
#elif defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable: 4296/*expression is always 'boolean_value'*/)
#   pragma warning(disable: 4389/*'==': signed/unsigned mismatch*/)
#   pragma warning(disable: 4702/*unreachable code*/)
#   if C4_MSVC_VERSION != C4_MSVC_VERSION_2017
#       pragma warning(disable: 4800/*'int': forcing value to bool 'true' or 'false' (performance warning)*/)
#   endif
#endif

#ifdef RYML_DBG
#   include <c4/yml/detail/print.hpp>
#endif
#include "test_lib/test_case_node.hpp"

/** @todo use a matcher and EXPECT_THAT():
 * see http://google.github.io/googletest/reference/assertions.html#EXPECT_THAT
 * see http://google.github.io/googletest/gmock_cook_book.html#NewMatchers
 */
#define RYML_COMPARE_NODE_TYPE(lhs, rhs, op, testop)                    \
    do                                                                  \
    {                                                                   \
        if(!((lhs) op (rhs)))                                           \
        {                                                               \
            char ltypebuf[256];                                         \
            char rtypebuf[256];                                         \
            csubstr ltype = NodeType::type_str(ltypebuf, (NodeType_e)lhs); \
            csubstr rtype = NodeType::type_str(rtypebuf, (NodeType_e)rhs); \
            if(ltype.str && rtype.str)                                  \
            {                                                           \
                EXPECT_##testop(lhs, rhs)                               \
                    << "  " << ltype.str << " (" << (lhs)  << ")" << "=" << #lhs \
                    << "\n"                                             \
                    << "  " << rtype.str << " (" << (rhs)  << ")" << "=" << #rhs; \
            }                                                           \
            else                                                        \
            {                                                           \
                EXPECT_##testop(lhs, rhs)                               \
                    << "(type too large to fit print buffer)";          \
            }                                                           \
        }                                                               \
    } while(0)


namespace c4 {

inline void PrintTo(substr  s, ::std::ostream* os) { *os << "'"; os->write(s.str, (std::streamsize)s.len); *os << "'"; }
inline void PrintTo(csubstr s, ::std::ostream* os) { *os << "'"; os->write(s.str, (std::streamsize)s.len); *os << "'"; }

namespace yml {

#define RYML_TRACE_FMT(fmt, ...) SCOPED_TRACE([&]{ return formatrs<std::string>(fmt, __VA_ARGS__); }())

inline void PrintTo(NodeType ty, ::std::ostream* os)
{
    *os << ty.type_str();
}
inline void PrintTo(NodeType_e ty, ::std::ostream* os)
{
    *os << NodeType::type_str(ty);
}

inline void PrintTo(Callbacks const& cb, ::std::ostream* os)
{
#ifdef __GNUC__
#define RYML_GNUC_EXTENSION __extension__
#else
#define RYML_GNUC_EXTENSION
#endif
    *os << '{'
        << "userdata." << (void*)cb.m_user_data << ','
        << "allocate." << RYML_GNUC_EXTENSION (void*)cb.m_allocate << ','
        << "free." << RYML_GNUC_EXTENSION (void*)cb.m_free << ','
        << "error." << RYML_GNUC_EXTENSION (void*)cb.m_error << '}';
#undef RYML_GNUC_EXTENSION
}

struct Case;
struct TestCaseNode;
struct CaseData;

Case const* get_case(csubstr name);
CaseData* get_data(csubstr name);

void test_compare(Tree const& actual, Tree const& expected);
void test_compare(Tree const& actual, id_type node_actual,
     Tree const& expected, id_type node_expected,
     id_type level=0);

void test_arena_not_shared(Tree const& a, Tree const& b);

void test_invariants(Tree const& t);
void test_invariants(ConstNodeRef const& n);

void print_test_node(TestCaseNode const& t, int level=0);
void print_test_tree(TestCaseNode const& p, int level=0);
void print_test_tree(const char *message, TestCaseNode const& t);
void print_path(ConstNodeRef const& p);


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

template<class CheckFn>
void test_check_emit_check_with_parser(Tree const& t, Parser &parser, CheckFn &&check_fn)
{
    #ifdef RYML_DBG
    print_tree(t);
    #endif
    {
        SCOPED_TRACE("original yaml");
        test_invariants(t);
        std::forward<CheckFn>(check_fn)(t, parser);
    }
    auto emit_and_parse = [&](Tree const& tp, const char* identifier){
        SCOPED_TRACE(identifier);
        std::string emitted = emitrs_yaml<std::string>(tp);
        #ifdef RYML_DBG
        printf("~~~%s~~~[%zu]\n%.*s", identifier, emitted.size(), (int)emitted.size(), emitted.data());
        #endif
        Tree cp = parse_in_arena(&parser, to_csubstr(emitted));
        #ifdef RYML_DBG
        print_tree(cp);
        #endif
        test_invariants(cp);
        std::forward<CheckFn>(check_fn)(cp, parser);
        return cp;
    };
    Tree cp = emit_and_parse(t, "emitted 1");
    cp = emit_and_parse(cp, "emitted 2");
    cp = emit_and_parse(cp, "emitted 3");
}
template<class CheckFn>
void test_check_emit_check(Tree const& t, Parser &parser, CheckFn &&check_fn)
{
    test_check_emit_check_with_parser(t, parser, [&check_fn](Tree const& t_, Parser const&){
        std::forward<CheckFn>(check_fn)(t_);
    });
}


template<class CheckFn>
void test_check_emit_check_with_parser(csubstr yaml, ParserOptions opts, CheckFn check_fn)
{
    Parser::handler_type evt_handler = {};
    Parser parser(&evt_handler, opts);
    const Tree t = parse_in_arena(&parser, yaml);
    test_check_emit_check_with_parser(t, parser, std::forward<CheckFn>(check_fn));
}
template<class CheckFn>
void test_check_emit_check(csubstr yaml, ParserOptions opts, CheckFn &&check_fn)
{
    Parser::handler_type evt_handler = {};
    Parser parser(&evt_handler, opts);
    const Tree t = parse_in_arena(&parser, yaml);
    test_check_emit_check(t, parser, std::forward<CheckFn>(check_fn));
}


template<class CheckFn>
void test_check_emit_check_with_parser(Tree const& t, CheckFn &&check_fn)
{
    Parser::handler_type evt_handler = {};
    Parser parser(&evt_handler, ParserOptions());
    test_check_emit_check_with_parser(t, parser, check_fn);
}
template<class CheckFn>
void test_check_emit_check(Tree const& t, CheckFn &&check_fn)
{
    Parser::handler_type evt_handler = {};
    Parser parser(&evt_handler, ParserOptions());
    test_check_emit_check(t, parser, std::forward<CheckFn>(check_fn));
}


template<class CheckFn>
void test_check_emit_check_with_parser(csubstr yaml, CheckFn &&check_fn)
{
    test_check_emit_check_with_parser(yaml, ParserOptions(), std::forward<CheckFn>(check_fn));
}
template<class CheckFn>
void test_check_emit_check(csubstr yaml, CheckFn &&check_fn)
{
    test_check_emit_check(yaml, ParserOptions(), std::forward<CheckFn>(check_fn));
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


inline c4::substr replace_all(c4::csubstr pattern, c4::csubstr repl, c4::csubstr subject, std::string *dst)
{
    RYML_CHECK(!subject.overlaps(to_csubstr(*dst)));
    size_t ret = subject.replace_all(to_substr(*dst), pattern, repl);
    if(ret != dst->size())
    {
        dst->resize(ret);
        ret = subject.replace_all(to_substr(*dst), pattern, repl);
    }
    RYML_CHECK(ret == dst->size());
    return c4::to_substr(*dst);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

struct ExpectError
{
    bool m_got_an_error;
    Tree *m_tree;
    c4::yml::Callbacks m_glob_prev;
    c4::yml::Callbacks m_tree_prev;
    Location expected_location;

    ExpectError(Location loc={}) : ExpectError(nullptr, loc) {}
    ExpectError(Tree *tree, Location loc={});
    ~ExpectError();

    static void check_error(            std::function<void()> fn, Location expected={}) { check_error((const Tree*)nullptr, fn, expected); }
    static void check_error(Tree *tree, std::function<void()> fn, Location expected={});
    static void check_error(Tree const *tree, std::function<void()> fn, Location expected={});
    static void check_assertion(            std::function<void()> fn, Location expected={}) { check_assertion(nullptr, fn, expected); }
    static void check_assertion(Tree *tree, std::function<void()> fn, Location expected={});
    static void check_success(            std::function<void()> fn) { check_success(nullptr, fn); };
    static void check_success(Tree *tree, std::function<void()> fn);
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef enum {
    EXPECT_PARSE_ERROR = (1<<0),
    RESOLVE_REFS = (1<<1),
    EXPECT_RESOLVE_ERROR = (1<<2),
    JSON_WRITE = (1<<3), // TODO: make it the opposite: opt-out instead of opt-in
    JSON_READ = (1<<4),
    HAS_CONTAINER_KEYS = (1<<5),
    HAS_MULTILINE_SCALAR = (1<<6),
} TestCaseFlags_e;


struct Case
{
    std::string filelinebuf;
    csubstr fileline;
    csubstr name;
    csubstr src;
    TestCaseNode root;
    TestCaseFlags_e flags;
    Location expected_location;

    //! create a standard test case: name, source and expected CaseNode structure
    template<class... Args> Case(csubstr file, int line, const char *name_,         const char *src_, Args&& ...args) : filelinebuf(catrs<std::string>(file, ':', line)), fileline(to_csubstr(filelinebuf)), name(to_csubstr(name_)), src(to_csubstr(src_)), root(std::forward<Args>(args)...), flags(), expected_location() {}
    //! create a test case with explicit flags: name, source flags, and expected CaseNode structure
    template<class... Args> Case(csubstr file, int line, const char *name_, int f_, const char *src_, Args&& ...args) : filelinebuf(catrs<std::string>(file, ':', line)), fileline(to_csubstr(filelinebuf)), name(to_csubstr(name_)), src(to_csubstr(src_)), root(std::forward<Args>(args)...), flags((TestCaseFlags_e)f_), expected_location()  {}
    //! create a test case with an error on an expected location
                            Case(csubstr file, int line, const char *name_, int f_, const char *src_, LineCol loc) : filelinebuf(catrs<std::string>(file, ':', line)), fileline(to_csubstr(filelinebuf)), name(to_csubstr(name_)), src(to_csubstr(src_)), root(), flags((TestCaseFlags_e)f_), expected_location(name, loc.line, loc.col) {}
};

//-----------------------------------------------------------------------------

// a persistent data store to avoid repeating operations on every test
struct CaseDataLineEndings
{
    void assign(csubstr src_orig)
    {
        parse_buf_ints.assign(src_orig.begin(), src_orig.end());
        src_buf.assign(src_orig.begin(), src_orig.end());
        src = to_substr(src_buf);
    }

    std::vector<char> src_buf;
    substr src;

    Tree parsed_tree;

    size_t numbytes_stdout;
    size_t numbytes_stdout_json;

    std::string emit_buf;
    csubstr emitted_yml;

    std::string emitjson_buf;
    csubstr emitted_json;

    std::string parse_buf;
    substr parsed_yml;

    std::string parse_buf_json;
    substr parsed_json;

    Tree emitted_tree;
    Tree emitted_tree_json;

    Tree recreated;

    std::string parse_buf_ints;
    std::vector<int> parsed_ints;
    std::vector<char> arena_ints;
};


struct CaseData
{
    CaseDataLineEndings unix_style;
    CaseDataLineEndings unix_style_json;
    CaseDataLineEndings windows_style;
    CaseDataLineEndings windows_style_json;
};


} // namespace yml
} // namespace c4

#ifdef __clang__
#   pragma clang diagnostic pop
#elif defined(__GNUC__)
#   pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#   pragma warning(pop)
#endif

#endif /* _TEST_CASE_HPP_ */
