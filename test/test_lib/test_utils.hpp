#ifndef TEST_LIB_TEST_UTILS_HPP_
#define TEST_LIB_TEST_UTILS_HPP_

#ifdef RYML_SINGLE_HEADER
#include <ryml_all.hpp>
#else
#include "c4/std/vector.hpp"
#include "c4/std/string.hpp"
#include "c4/format.hpp"
#include <c4/yml/detail/dbgprint.hpp>
#include <c4/yml/escape_scalar.hpp>
#include <c4/yml/detail/print.hpp>
#endif
#include "c4/span.hpp"
#include <gtest/gtest.h>
#include <functional>
#ifdef RYML_DBG
#   include <c4/yml/detail/print.hpp>
#endif

#include "test_lib/test_main.hpp"


// no pragma push for these warnings! they will be suppressed in the
// files including this header (most test files)

C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")
C4_SUPPRESS_WARNING_GCC("-Wuseless-cast")
#if defined(__clang__) && (__clang_major__ >= 13)
C4_SUPPRESS_WARNING_CLANG("-Wreserved-identifier")
#endif

// these warnings frequently have false positives. Disabling:
#if defined(__GNUC__) && (__GNUC__ >= 6)
C4_SUPPRESS_WARNING_GCC("-Wnull-dereference")
#endif
#if defined(__GNUC__) && (__GNUC__ >= 7)
C4_SUPPRESS_WARNING_GCC("-Wstringop-overflow")
#endif


namespace c4 {

inline void PrintTo(substr  s, ::std::ostream* os) { *os << "'"; os->write(s.str, (std::streamsize)s.len); *os << "'"; }
inline void PrintTo(csubstr s, ::std::ostream* os) { *os << "'"; os->write(s.str, (std::streamsize)s.len); *os << "'"; }

namespace yml {

#define SCOPED_CALL(fn, ...) do { SCOPED_TRACE(#fn); (fn) __VA_ARGS__; } while(0)
#define RYML_TRACE_FMT(fmt, ...) SCOPED_TRACE(formatrs<std::string>(fmt, __VA_ARGS__))

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


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

struct txtbuf : std::string
{
    using std::string::string;
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
    NO_COMPARE_RESOLVED_INTS = (1<<10),
    NO_COMPARE_ROUNDTRIP_INTS = (1<<11),
} TestCaseFlags_e;


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
        SCOPED_TRACE("expect error?");          \
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

#endif /* TEST_LIB_TEST_UTILS_HPP_ */
