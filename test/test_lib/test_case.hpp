#ifndef TEST_CASE_HPP_
#define TEST_CASE_HPP_

#ifdef RYML_SINGLE_HEADER
#include <ryml_all.hpp>
#else
#include "c4/std/vector.hpp"
#include "c4/std/string.hpp"
#include "c4/format.hpp"
#include <c4/yml/yml.hpp>
#include <c4/yml/detail/dbgprint.hpp>
#include <c4/yml/escape_scalar.hpp>
#include <c4/yml/detail/print.hpp>
#include <c4/yml/extra/event_ints.hpp>
#endif
#include "c4/span.hpp"
#include <test_lib/test_events_ints_helpers.hpp>
#include <gtest/gtest.h>
#include <functional>


// no pragma push for these warnings! they will be suppressed in the
// files including this header (most test files)
C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")
#if defined(__clang__) && (__clang_major__ >= 13)
C4_SUPPRESS_WARNING_CLANG("-Wreserved-identifier")
#endif


C4_SUPPRESS_WARNING_PUSH
C4_SUPPRESS_WARNING_GCC_CLANG("-Wtype-limits")
C4_SUPPRESS_WARNING_MSVC(4296/*expression is always 'boolean_value'*/)
C4_SUPPRESS_WARNING_MSVC(4389/*'==': signed/unsigned mismatch*/)
C4_SUPPRESS_WARNING_MSVC(4702/*unreachable code*/)
#if defined(_MSC_VER) && (C4_MSVC_VERSION != C4_MSVC_VERSION_2017)
C4_SUPPRESS_WARNING_MSVC(4800/*'int': forcing value to bool 'true' or 'false' (performance warning)*/)
#endif

#ifdef RYML_DBG
#   include <c4/yml/detail/print.hpp>
#endif
#include "test_lib/test_case_node.hpp"
#include "test_lib/test_main.hpp"


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
            csubstr ltype = NodeType::type_str_sub(ltypebuf, (type_bits)lhs); \
            csubstr rtype = NodeType::type_str_sub(rtypebuf, (type_bits)rhs); \
            EXPECT_##testop(lhs, rhs);                                  \
            std::cout << __FILE__  << ":" << __LINE__ << ": ...\n";     \
            if(ltype.str && rtype.str)                                  \
            {                                                           \
                std::cout                                               \
                    << "  " << ltype << " (" << (lhs)  << ")" << "=" << #lhs \
                    << "\n"                                             \
                    << "  " << rtype << " (" << (rhs)  << ")" << "=" << #rhs \
                    << "\n";                                            \
            }                                                           \
            else                                                        \
            {                                                           \
                std::cout                                               \
                    << "(type too large to fit print buffer)"           \
                    << "\n";                                            \
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
inline void PrintTo(NodeTypeBits ty, ::std::ostream* os)
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
        << "error_basic." << RYML_GNUC_EXTENSION (void*)cb.m_error_basic << '}'
        << "error_parse." << RYML_GNUC_EXTENSION (void*)cb.m_error_parse << '}'
        << "error_visit." << RYML_GNUC_EXTENSION (void*)cb.m_error_visit << '}';
#undef RYML_GNUC_EXTENSION
}

struct Case;
struct TestCaseNode;
struct CaseData;

Case const* get_case(csubstr name);
CaseData* get_data(csubstr name);


void test_compare(ConstNodeRef const& actual, ConstNodeRef const& expected,
                  const char *actual_name="actual", const char *expected_name="expected",
                  type_bits cmp_mask=TYMASK_);
void test_compare(Tree const& actual, Tree const& expected,
                  const char *actual_name="actual", const char *expected_name="expected",
                  type_bits cmp_mask=TYMASK_);
void test_compare(Tree const& actual, id_type node_actual,
                  Tree const& expected, id_type node_expected,
                  id_type level=0, const char *actual_name="actual", const char *expected_name="expected",
                  type_bits cmp_mask=TYMASK_);

void test_arena_not_shared(Tree const& a, Tree const& b);

void test_invariants(NodeType ty);
void test_invariants(Tree const& t);
void test_invariants(ConstNodeRef const& n);

void print_test_node(TestCaseNode const& t, int level=0);
void print_test_tree(TestCaseNode const& p, int level=0);
void print_test_tree(const char *message, TestCaseNode const& t);


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

template<class CheckFn>
void test_check_emit_check_with_parser(Tree const& t, Parser &parser, CheckFn &&check_fn)
{
    {
        SCOPED_TRACE("original yaml");
        test_invariants(t);
        std::forward<CheckFn>(check_fn)(t, parser);
        if(testing::Test::HasFailure())
        {
            print_tree(t);
        }
    }
    auto emit_and_parse = [&](Tree const& tp, Tree *out, const char* identifier){
        SCOPED_TRACE(identifier);
        std::string emitted = emitrs_yaml<std::string>(tp);
        parse_in_arena(&parser, to_csubstr(emitted), out);
        test_invariants(*out);
        std::forward<CheckFn>(check_fn)(*out, parser);
        if(testing::Test::HasFailure())
        {
            printf("~~~%s~~~[%zu]\n%.*s", identifier, emitted.size(), (int)emitted.size(), emitted.data());
            print_tree(*out);
        }
    };
    if(!testing::Test::HasFailure())
    {
        Tree cp1;
        SCOPED_TRACE("level 1");
        emit_and_parse(t, &cp1, "level 1");
        if(!testing::Test::HasFailure())
        {
            Tree cp2;
            SCOPED_TRACE("level 2");
            emit_and_parse(cp1, &cp2, "level 2");
            if(!testing::Test::HasFailure())
            {
                Tree cp3;
                SCOPED_TRACE("level 3");
                emit_and_parse(cp2, &cp3, "level 3");
            }
        }
    }
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
    test_check_emit_check_with_parser(t, parser, std::forward<CheckFn>(check_fn));
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
    RYML_CHECK_BASIC_(!subject.overlaps(to_csubstr(*dst)));
    size_t ret = subject.replace_all(to_substr(*dst), pattern, repl);
    if(ret != dst->size())
    {
        dst->resize(ret);
        ret = subject.replace_all(to_substr(*dst), pattern, repl);
    }
    RYML_CHECK_BASIC_(ret == dst->size());
    return c4::to_substr(*dst);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

enum class ExpectedErrorType : int { err_none = 0, err_basic = 1, err_parse = 2, err_visit = 3, err_any = 7 };

#define RYML_EXPECT_ERROR(...)                  \
    do {                                        \
        SCOPED_TRACE("call");                   \
        ExpectError:: __VA_ARGS__ ;             \
    } while(0)

struct ExpectError
{
    ExpectedErrorType m_error;
    ExpectedErrorType m_expected_error;
    Tree *m_tree;
    c4::yml::Callbacks m_glob_prev;
    c4::yml::Callbacks m_tree_prev;
    Location expected_location;

    ExpectError(ExpectedErrorType errtype, Location loc={}) : ExpectError(errtype, nullptr, loc) {}
    ExpectError(ExpectedErrorType errtype, Tree *tree, Location loc={});
    ~ExpectError();

    using fntestref = std::function<void()> const&;

    static void check_success(            fntestref fn) { check_success(nullptr, fn); };
    static void check_success(Tree *tree, fntestref fn);

    static void check_error(ExpectedErrorType errtype,             fntestref fn, Location const& loc={}) { check_error(errtype, nullptr, fn, loc); };
    static void check_error(ExpectedErrorType errtype, Tree *tree, fntestref fn, Location const& loc={});

    static void check_assert(ExpectedErrorType errtype,             fntestref fn, Location const& loc={}) { check_error(errtype, nullptr, fn, loc); };
    static void check_assert(ExpectedErrorType errtype, Tree *tree, fntestref fn, Location const& loc={});

    static void check_error_basic(            fntestref fn, bool only_basic=true) { check_error_basic((Tree*)nullptr, fn, only_basic); }
    static void check_error_basic(Tree *tree, fntestref fn, bool only_basic=true);
    static void check_error_basic(Tree const *tree, fntestref fn, bool only_basic=true);
    static void check_assert_basic(            fntestref fn, bool only_basic=true) { check_assert_parse((Tree*)nullptr, fn, only_basic); }
    static void check_assert_basic(Tree *tree, fntestref fn, bool only_basic=true);
    static void check_assert_basic(Tree const* tree, fntestref fn, bool only_basic=true);

    static void check_error_parse(            fntestref fn, Location const& expected={}) { check_error_parse((Tree*)nullptr, fn, expected); }
    static void check_error_parse(Tree *tree, fntestref fn, Location const& expected={});
    static void check_error_parse(Tree const *tree, fntestref fn, Location const& expected={});
    static void check_assert_parse(            fntestref fn, Location const& expected={}) { check_assert_parse((Tree*)nullptr, fn, expected); }
    static void check_assert_parse(Tree *tree, fntestref fn, Location const& expected={});
    static void check_assert_parse(Tree const *tree, fntestref fn, Location const& expected={});

    static void check_error_visit(            fntestref fn, id_type id=NONE) { check_error_visit((Tree*)nullptr, fn, id); }
    static void check_error_visit(Tree *tree, fntestref fn, id_type id=NONE);
    static void check_error_visit(Tree const *tree, fntestref fn, id_type id=NONE);
    static void check_assert_visit(            fntestref fn, id_type id=NONE) { check_assert_visit((Tree*)nullptr, fn, id); }
    static void check_assert_visit(Tree *tree, fntestref fn, id_type id=NONE);
    static void check_assert_visit(Tree const *tree, fntestref fn, id_type id=NONE);
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
    NO_COMPARE_EMITTED = (1<<7),
    NO_COMPARE_EMITTED_INTS = (1<<8),
    NO_COMPARE_EMITTED_INTS_JSON = (1<<9),
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

    //! create a test case with an error on an expected location
    Case(csubstr file, int line, const char *name_, int f_, const char *src_, Location const& loc={}) : filelinebuf(catrs<std::string>(file, ':', line)), fileline(to_csubstr(filelinebuf)), name(to_csubstr(name_)), src(to_csubstr(src_)), root(), flags((TestCaseFlags_e)f_), expected_location(name, loc.line, loc.col) {}
    //! create a standard test case: name, source and expected CaseNode structure
    Case(csubstr file, int line, const char *name_,         const char *src_, TestCaseNode&& node) : /* */filelinebuf(catrs<std::string>(file, ':', line)), fileline(to_csubstr(filelinebuf)), name(to_csubstr(name_)), src(to_csubstr(src_)), root(std::move(node)), flags(), expected_location() {}
    //! create a test case with explicit flags: name, source flags, and expected CaseNode structure
    Case(csubstr file, int line, const char *name_, int f_, const char *src_, TestCaseNode&& node) : /* */filelinebuf(catrs<std::string>(file, ':', line)), fileline(to_csubstr(filelinebuf)), name(to_csubstr(name_)), src(to_csubstr(src_)), root(std::move(node)), flags((TestCaseFlags_e)f_), expected_location()  {}
};


//-----------------------------------------------------------------------------

struct txtbuf : std::string
{
    operator c4::substr() noexcept { return c4::substr{&(*this)[0], size()}; }//NOLINT
    operator c4::csubstr() const noexcept { return c4::csubstr{&(*this)[0], size()}; }//NOLINT
};

template<class T>
struct resultdep
{
    T result;
    bool done = false;
    template<class Fn>
    bool ensure(Fn && fn)
    {
        if(done)
            return false;
        std::forward<Fn>(fn)();
        done = true;
        return true;
    }
};
template<class T>
struct src_and_val
{
    resultdep<txtbuf> orig; // original source
    resultdep<txtbuf> src; // mutated by the parse
    resultdep<T> val;
    void reset() {  orig.done = src.done = val.done = false; }
    void assign(csubstr src_)
    {
        if(src.ensure([&]{ src.result.assign(src_.begin(), src_.end()); }))
            orig = src;
    }
};
using SrcTree = src_and_val<Tree>;
using SrcInts = src_and_val<extra::ievt::TestBuffers>;


// a persistent data store to avoid repeating operations on every test
struct CaseDataLineEndings
{
    void assign(csubstr src_orig)
    {
        if(src_orig == src && src_orig.len)
            return;
        tree.reset();
        tree_roundtrip_yaml.reset();
        tree_roundtrip_json.reset();
        tree.val.result.clear();
        tree_roundtrip_yaml.val.result.clear();
        tree_roundtrip_json.val.result.clear();
        ints_resize.reset();
        ints_resize_roundtrip_yaml.reset();
        ints_resize_roundtrip_json.reset();
        ints_noresize.reset();
        ints_noresize_roundtrip_yaml.reset();
        ints_noresize_roundtrip_json.reset();
        const char *b = src_orig.begin();
        const char *e = src_orig.end();
        src.assign(b, e);
        tree.assign(src_orig);
        ints_resize.assign(src_orig);
        ints_noresize.assign(src_orig);
    }

    txtbuf src;

    SrcTree tree;
    SrcTree tree_roundtrip_yaml;
    SrcTree tree_roundtrip_json;

    SrcInts ints_resize;
    SrcInts ints_resize_roundtrip_yaml;
    SrcInts ints_resize_roundtrip_json;

    SrcInts ints_noresize;
    SrcInts ints_noresize_roundtrip_yaml;
    SrcInts ints_noresize_roundtrip_json;

    void ensure_tree_parse(Case const *c);
    void ensure_tree_emit_yaml(Case const *c);
    void ensure_tree_emit_json(Case const *c);
    void ensure_tree_roundtrip_yaml(Case const *c);
    void ensure_tree_roundtrip_json(Case const *c);

    void ensure_ints_parse(Case const *c, SrcInts *dst);
    void ensure_ints_emit(Case const *c, SrcInts *ints, SrcInts *roundtrip, EmitType_e type);
    void ensure_ints_roundtrip(Case const *c, SrcInts *ints, SrcInts *roundtrip, EmitType_e type);

    void ensure_ints_resize_emit_yaml(Case const *c);
    void ensure_ints_resize_emit_json(Case const *c);
    void ensure_ints_resize_roundtrip_yaml(Case const *c);
    void ensure_ints_resize_roundtrip_json(Case const *c);

    void ensure_ints_noresize_emit_yaml(Case const *c);
    void ensure_ints_noresize_emit_json(Case const *c);
    void ensure_ints_noresize_roundtrip_yaml(Case const *c);
    void ensure_ints_noresize_roundtrip_json(Case const *c);
};


struct CaseData
{
    CaseDataLineEndings unix_style;
    CaseDataLineEndings windows_style;

    resultdep<Tree> reftree;
    void ensure_reftree(Case const *c);
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

struct bomspec
{
    csubstr name;
    Encoding_e encoding;
    bool supported;
    csubstr bom;
};
extern const cspan<bomspec> bomspecs;

inline std::string namefor(bomspec const& param)
{
    std::string s(param.name.str, param.name.len);
    substr ss = to_substr(s);
    ss.replace('!', '_');
    ss.replace('-', '_');
    return s;
}

} // namespace yml
} // namespace c4

C4_SUPPRESS_WARNING_POP

#endif /* TEST_CASE_HPP_ */
