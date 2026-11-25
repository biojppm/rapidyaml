#include "./test_lib/test_case.hpp"
#ifndef RYML_SINGLE_HEADER
#include "c4/yml/common.hpp"
#include "c4/yml/error.def.hpp"
#include "c4/dump.hpp"
#endif
#include <stdexcept>
#include <csetjmp>
#include <cstring>
#include "c4/yml/extra/event_handler_ints.hpp"


C4_SUPPRESS_WARNING_MSVC_WITH_PUSH(4996) // fopen: This function or variable may be unsafe
C4_SUPPRESS_WARNING_CLANG_WITH_PUSH("-Wdeprecated-declarations") // fopen is deprecated

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {
bool print_msg = false;

struct CustomLengthArg { size_t len; };
size_t to_chars(substr buf, CustomLengthArg const& arg)
{
    size_t len = (arg.len < buf.len) ? arg.len : buf.len;
    for(size_t i = 0; i < len; ++i)
        buf.str[i] = (char)('0' + (i % 9));
    return arg.len;
}

struct Dumper
{
    char errmsg[RYML_ERRMSG_SIZE] = {0};
    detail::SubstrWriter_ writer{errmsg};
    void operator()(csubstr s)
    {
        writer.append(s);
    }
};

TEST(_to_chars_limited, arg_not_requiring_argbuf)
{
    char argbuf_[RYML_LOGBUF_SIZE];
    substr argbuf(argbuf_);
    ASSERT_EQ(argbuf.len + 1, RYML_LOGBUF_SIZE);
    {
        SCOPED_TRACE("smaller size");
        std::string arg(argbuf.len - 1, '-');
        const std::string expected = catrs<std::string>(arg);
        csubstr actual = detail::_to_chars_limited(argbuf, to_csubstr(arg));
        EXPECT_EQ(expected.size(), arg.size());
        EXPECT_EQ(actual, expected);
    }
    {
        SCOPED_TRACE("same size");
        std::string arg(argbuf.len, '-');
        const std::string expected = catrs<std::string>(arg);
        csubstr actual = detail::_to_chars_limited(argbuf, to_csubstr(arg));
        EXPECT_EQ(expected.size(), arg.size());
        EXPECT_EQ(actual, expected);
    }
    {
        SCOPED_TRACE("larger size");
        std::string arg(argbuf.len + 1, '-');
        std::string expected = catrs<std::string>(arg);
        csubstr actual = detail::_to_chars_limited(argbuf, to_csubstr(arg));
        EXPECT_EQ(expected.size(), arg.size());
        EXPECT_EQ(actual, expected);
    }
}

TEST(_to_chars_limited, arg_requiring_argbuf)
{
    char argbuf_[RYML_LOGBUF_SIZE];
    substr argbuf(argbuf_);
    ASSERT_EQ(argbuf.len + 1, RYML_LOGBUF_SIZE);
    {
        SCOPED_TRACE("smaller size");
        CustomLengthArg arg{argbuf.len - 1};
        const std::string expected = catrs<std::string>(arg);
        csubstr actual = detail::_to_chars_limited(argbuf, arg);
        EXPECT_EQ(expected.size(), arg.len);
        EXPECT_EQ(actual, expected);
    }
    {
        SCOPED_TRACE("same size");
        CustomLengthArg arg{argbuf.len};
        const std::string expected = catrs<std::string>(arg);
        csubstr actual = detail::_to_chars_limited(argbuf, arg);
        EXPECT_EQ(expected.size(), arg.len);
        EXPECT_EQ(actual, expected);
    }
    {
        SCOPED_TRACE("larger size");
        CustomLengthArg arg{argbuf.len + 1};
        std::string expected = catrs<std::string>(arg);
        csubstr actual = detail::_to_chars_limited(argbuf, arg);
        EXPECT_EQ(expected.size(), arg.len);
        EXPECT_NE(actual, expected);
        EXPECT_GT(arg.len, argbuf.len);
        EXPECT_GT(expected.size(), argbuf.len);
        EXPECT_EQ(actual.len, argbuf.len);
        EXPECT_TRUE(actual.ends_with("...")) << actual;
        csubstr expected_with_ellipsis = detail::_maybe_add_ellipsis(to_substr(expected).first(argbuf.len), arg.len);
        EXPECT_TRUE(expected_with_ellipsis.ends_with("...")) << expected;
        EXPECT_EQ(actual, expected_with_ellipsis);
    }
}

TEST(_mk_err_msg, smaller_size)
{
    size_t item_size = RYML_ERRMSG_SIZE / 10;
    std::string arg(item_size, '-');
    char buf_[RYML_ERRMSG_SIZE];
    substr buf(buf_);
    std::string expected = c4::formatrs<std::string>("{}", to_csubstr(arg));
    EXPECT_EQ(expected.size(), item_size);
    csubstr actual = detail::_mk_err_msg(buf, "{}", to_csubstr(arg));
    EXPECT_LT(actual.len, buf.len);
    EXPECT_EQ(actual, expected);
}

TEST(_mk_err_msg, larger_size_not_requiring_argbuf)
{
    size_t item_size = RYML_ERRMSG_SIZE * 2;
    std::string arg_(item_size, '-');
    csubstr arg = to_csubstr(arg_);
    EXPECT_EQ(arg.size(), item_size);
    EXPECT_EQ(arg.len, item_size);
    char buf_[RYML_ERRMSG_SIZE] = {0};
    substr buf(buf_);
    std::string expected = c4::formatrs<std::string>("{}", arg);
    EXPECT_EQ(expected.size(), arg.len);
    csubstr actual = detail::_mk_err_msg(buf, "{}", arg);
    EXPECT_EQ(actual.len, buf.len);
    EXPECT_NE(actual, expected);
    EXPECT_TRUE(actual.ends_with("..."));
}

TEST(_mk_err_msg, larger_size_requiring_argbuf)
{
    size_t item_size = RYML_ERRMSG_SIZE * 2;
    CustomLengthArg arg{item_size};
    EXPECT_EQ(arg.len, item_size);
    char buf_[RYML_ERRMSG_SIZE];
    substr buf(buf_);
    std::string expected = c4::formatrs<std::string>("{}", arg);
    EXPECT_EQ(expected.size(), arg.len);
    csubstr actual = detail::_mk_err_msg(buf, "{}", arg);
    EXPECT_EQ(actual.len, RYML_LOGBUF_SIZE-1);
    EXPECT_NE(actual, expected);
    EXPECT_TRUE(actual.ends_with("..."));
}

TEST(_mk_err_msg, trailing_fmt)
{
    char buf_[RYML_ERRMSG_SIZE];
    substr buf(buf_);
    std::string expected = c4::formatrs<std::string>("{} trailing after fmt", 2);
    csubstr actual = detail::_mk_err_msg(buf, "{} trailing after fmt", 2);
    EXPECT_EQ(actual, expected);
}

TEST(_mk_err_msg, no_args)
{
    char buf_[RYML_ERRMSG_SIZE];
    substr buf(buf_);
    std::string expected = c4::formatrs<std::string>("look, no args!");
    csubstr actual = detail::_mk_err_msg(buf, "look, no args!");
    EXPECT_EQ(actual, expected);
}


TEST(_get_text_region, basic)
{
    #define check_same(a, b)                                        \
        {                                                           \
            csubstr a_ = (a);                                       \
            csubstr b_ = (b);                                       \
            if(a_.str != b_.str || a_.len != b_.len)                \
            {                                                       \
                std::cout << __FILE__ << ":" << __LINE__ << ": "    \
                          << "[" << a_.len << "]~~~";               \
                std::cout.write(a_.str, (std::streamsize)a_.len);   \
                std::cout << "~~~=" #a "\n";                        \
                std::cout << __FILE__ << ":" << __LINE__ << ": "    \
                          << "[" << b_.len << "]~~~";               \
                std::cout.write(b_.str, (std::streamsize)b_.len);   \
                std::cout << "~~~=" #b "\n";                        \
            }                                                       \
            EXPECT_EQ(a_.str, b_.str);                              \
            EXPECT_EQ(a_.len, b_.len);                              \
        }
    using detail::_get_text_region;
    {
        csubstr txt = "";
        check_same(txt, _get_text_region(txt, 0, 0, 0));
        check_same(txt, _get_text_region(txt, 0, 2, 2));
        check_same(txt, _get_text_region(txt, 1, 2, 2));
        check_same(txt, _get_text_region(txt, 2, 2, 2));
    }
    {
        csubstr txt = "a\nb\nc\n";
        //
        check_same(txt.first(1), _get_text_region(txt, 0, 0, 0));
        check_same(txt.first(1), _get_text_region(txt, 1, 0, 0));
        check_same(txt.range(2, 3), _get_text_region(txt, 2, 0, 0));
        check_same(txt.range(2, 3), _get_text_region(txt, 3, 0, 0));
        check_same(txt.range(4, 5), _get_text_region(txt, 4, 0, 0));
        check_same(txt.range(4, 5), _get_text_region(txt, 5, 0, 0));
        check_same(txt.range(6, 6), _get_text_region(txt, 6, 0, 0));
        // 1 line before
        check_same(txt.first(1), _get_text_region(txt, 0, 1, 0));
        check_same(txt.first(1), _get_text_region(txt, 1, 1, 0));
        check_same(txt.range(0, 3), _get_text_region(txt, 2, 1, 0));
        check_same(txt.range(0, 3), _get_text_region(txt, 3, 1, 0));
        check_same(txt.range(2, 5), _get_text_region(txt, 4, 1, 0));
        check_same(txt.range(2, 5), _get_text_region(txt, 5, 1, 0));
        check_same(txt.range(4, 6), _get_text_region(txt, 6, 1, 0));
        // 2 lines before
        check_same(txt.first(1), _get_text_region(txt, 0, 2, 0));
        check_same(txt.first(1), _get_text_region(txt, 1, 2, 0));
        check_same(txt.range(0, 3), _get_text_region(txt, 2, 2, 0));
        check_same(txt.range(0, 3), _get_text_region(txt, 3, 2, 0));
        check_same(txt.range(0, 5), _get_text_region(txt, 4, 2, 0));
        check_same(txt.range(0, 5), _get_text_region(txt, 5, 2, 0));
        check_same(txt.range(2, 6), _get_text_region(txt, 6, 2, 0));
        // 1 line after
        check_same(txt.first(3), _get_text_region(txt, 0, 0, 1));
        check_same(txt.first(3), _get_text_region(txt, 1, 0, 1));
        check_same(txt.range(2, 5), _get_text_region(txt, 2, 0, 1));
        check_same(txt.range(2, 5), _get_text_region(txt, 3, 0, 1));
        check_same(txt.range(4, 6), _get_text_region(txt, 4, 0, 1));
        check_same(txt.range(4, 6), _get_text_region(txt, 5, 0, 1));
        check_same(txt.range(6, 6), _get_text_region(txt, 6, 0, 1));
        // 2 lines after
        check_same(txt.first(5), _get_text_region(txt, 0, 0, 2));
        check_same(txt.first(5), _get_text_region(txt, 1, 0, 2));
        check_same(txt.range(2, 6), _get_text_region(txt, 2, 0, 2));
        check_same(txt.range(2, 6), _get_text_region(txt, 3, 0, 2));
        check_same(txt.range(4, 6), _get_text_region(txt, 4, 0, 2));
        check_same(txt.range(4, 6), _get_text_region(txt, 5, 0, 2));
        check_same(txt.range(6, 6), _get_text_region(txt, 6, 0, 2));
    }
    #undef check_same
}


struct msgs { std::string nocall, withcall; };
msgs mkmsg_(csubstr txt, const char* file, size_t line, size_t col, size_t offset, size_t firstcol=0, size_t lastcol=0, size_t num_lines_before=0, size_t num_lines_after = 0, size_t maxlen=80u)
{
    Location loc;
    loc.name = to_csubstr(file);
    loc.line = line;
    loc.col = col;
    loc.offset = offset;
    msgs ret;
    auto dumpfn1 = [&ret](csubstr s) { ret.nocall.append(s.str, s.len); };
    location_format_with_context(dumpfn1, loc, txt, csubstr{}, firstcol, lastcol, num_lines_before, num_lines_after, maxlen);
    if(print_msg)
    {
        std::cout << "----\n";
        std::cout << ret.nocall;
    }
    auto dumpfn2 = [&ret](csubstr s) { ret.withcall.append(s.str, s.len); };
    location_format_with_context(dumpfn2, loc, txt, "error", firstcol, lastcol, num_lines_before, num_lines_after, maxlen);
    if(print_msg)
    {
        std::cout << "....\n";
        std::cout << ret.withcall;
    }
    return ret;
}

TEST(format_region, basic)
{
#define mkmsg(args, nocall_, withcall_)                 \
    do {                                                \
        SCOPED_TRACE(#args);                            \
        msgs actual = mkmsg_ args;                      \
        msgs expected{nocall_, withcall_};              \
        EXPECT_EQ(actual.nocall, expected.nocall);      \
        EXPECT_EQ(actual.withcall, expected.withcall);  \
    } while(0)

    csubstr txt = "aaa\nbbb\nccc\nddd\neee\n";
    //     src  file     line  col   offset
    mkmsg((txt, nullptr, npos, npos, npos), "", "");
    mkmsg((txt, nullptr, npos, npos, 0), "(0B): aaa\n", "(0B): error:\nerror:\nerror:     aaa\n"); // FIXME
    mkmsg((txt, nullptr, npos, npos, 1), "(1B): aaa\n", "(1B): error:\nerror:\nerror:     aaa\n"); // FIXME
    mkmsg((txt, nullptr, npos, npos, 2), "(2B): aaa\n", "(2B): error:\nerror:\nerror:     aaa\n"); // FIXME
    mkmsg((txt, nullptr, npos, npos, 3), "(3B): aaa\n", "(3B): error:\nerror:\nerror:     aaa\n"); // FIXME
    mkmsg((txt, nullptr, npos, 0   , npos), "col=0: \n", "col=0: error:\nerror:\nerror:     \n");
    mkmsg((txt, nullptr, npos, 0   , 0), "col=0 (0B): aaa\n            |\n            (here)\n", "col=0 (0B): error:\nerror:\nerror:     aaa\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, npos, 1   , 1), "col=1 (1B): aaa\n             |\n             (here)\n", "col=1 (1B): error:\nerror:\nerror:     aaa\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, nullptr, npos, 2   , 2), "col=2 (2B): aaa\n              |\n              (here)\n", "col=2 (2B): error:\nerror:\nerror:     aaa\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, nullptr, npos, 3   , 3), "col=3 (3B): aaa\n               |\n               (here)\n", "col=3 (3B): error:\nerror:\nerror:     aaa\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, nullptr, npos, npos, npos), "", "");
    mkmsg((txt, nullptr, npos, npos, 4+0), "(4B): bbb\n", "(4B): error:\nerror:\nerror:     bbb\n");
    mkmsg((txt, nullptr, npos, npos, 4+1), "(5B): bbb\n", "(5B): error:\nerror:\nerror:     bbb\n");
    mkmsg((txt, nullptr, npos, npos, 4+2), "(6B): bbb\n", "(6B): error:\nerror:\nerror:     bbb\n");
    mkmsg((txt, nullptr, npos, npos, 4+3), "(7B): bbb\n", "(7B): error:\nerror:\nerror:     bbb\n");
    mkmsg((txt, nullptr, npos, 0   , npos), "col=0: \n", "col=0: error:\nerror:\nerror:     \n");
    mkmsg((txt, nullptr, npos, 0   , 4+0), "col=0 (4B): bbb\n            |\n            (here)\n", "col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, npos, 1   , 4+1), "col=1 (5B): bbb\n             |\n             (here)\n", "col=1 (5B): error:\nerror:\nerror:     bbb\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, nullptr, npos, 2   , 4+2), "col=2 (6B): bbb\n              |\n              (here)\n", "col=2 (6B): error:\nerror:\nerror:     bbb\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, nullptr, npos, 3   , 4+3), "col=3 (7B): bbb\n               |\n               (here)\n", "col=3 (7B): error:\nerror:\nerror:     bbb\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, nullptr, npos, npos, npos, 1), "", "");
    mkmsg((txt, nullptr, npos, npos, 4+0 , 1), "(4B): bbb\n(4B): see region:\naaa\nbbb\n", "(4B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, nullptr, npos, npos, 4+1 , 1), "(5B): bbb\n(5B): see region:\naaa\nbbb\n", "(5B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, nullptr, npos, npos, 4+2 , 1), "(6B): bbb\n(6B): see region:\naaa\nbbb\n", "(6B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, nullptr, npos, npos, 4+3 , 1), "(7B): bbb\n(7B): see region:\naaa\nbbb\n", "(7B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, nullptr, npos, 0   , npos, 1), "col=0: \ncol=0: see region:\n", "col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, nullptr, npos, 0   , 4+0 , 1), "col=0 (4B): bbb\n            |\n            (here)\ncol=0 (4B): see region:\naaa\nbbb\n|\n(here)\n", "col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, npos, 1   , 4+1 , 1), "col=1 (5B): bbb\n             |\n             (here)\ncol=1 (5B): see region:\naaa\nbbb\n |\n (here)\n", "col=1 (5B): error:\nerror:\nerror:     bbb\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, nullptr, npos, 2   , 4+2 , 1), "col=2 (6B): bbb\n              |\n              (here)\ncol=2 (6B): see region:\naaa\nbbb\n  |\n  (here)\n", "col=2 (6B): error:\nerror:\nerror:     bbb\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, nullptr, npos, 3   , 4+3 , 1), "col=3 (7B): bbb\n               |\n               (here)\ncol=3 (7B): see region:\naaa\nbbb\n   |\n   (here)\n", "col=3 (7B): error:\nerror:\nerror:     bbb\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, nullptr, npos, npos, npos, 2), "", "");
    mkmsg((txt, nullptr, npos, npos, 4+0 , 2), "(4B): bbb\n(4B): see region:\naaa\nbbb\n", "(4B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, nullptr, npos, npos, 4+1 , 2), "(5B): bbb\n(5B): see region:\naaa\nbbb\n", "(5B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, nullptr, npos, npos, 4+2 , 2), "(6B): bbb\n(6B): see region:\naaa\nbbb\n", "(6B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, nullptr, npos, npos, 4+3 , 2), "(7B): bbb\n(7B): see region:\naaa\nbbb\n", "(7B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, nullptr, npos, 0   , npos, 2), "col=0: \ncol=0: see region:\n", "col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, nullptr, npos, 0   , 4+0 , 2), "col=0 (4B): bbb\n            |\n            (here)\ncol=0 (4B): see region:\naaa\nbbb\n|\n(here)\n", "col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, npos, 1   , 4+1 , 2), "col=1 (5B): bbb\n             |\n             (here)\ncol=1 (5B): see region:\naaa\nbbb\n |\n (here)\n", "col=1 (5B): error:\nerror:\nerror:     bbb\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, nullptr, npos, 2   , 4+2 , 2), "col=2 (6B): bbb\n              |\n              (here)\ncol=2 (6B): see region:\naaa\nbbb\n  |\n  (here)\n", "col=2 (6B): error:\nerror:\nerror:     bbb\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, nullptr, npos, 3   , 4+3 , 2), "col=3 (7B): bbb\n               |\n               (here)\ncol=3 (7B): see region:\naaa\nbbb\n   |\n   (here)\n", "col=3 (7B): error:\nerror:\nerror:     bbb\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, nullptr, npos, npos, npos), "", "");
    mkmsg((txt, nullptr, npos, npos, 8+0), "(8B): ccc\n", "(8B): error:\nerror:\nerror:     ccc\n");
    mkmsg((txt, nullptr, npos, npos, 8+1), "(9B): ccc\n", "(9B): error:\nerror:\nerror:     ccc\n");
    mkmsg((txt, nullptr, npos, npos, 8+2), "(10B): ccc\n", "(10B): error:\nerror:\nerror:     ccc\n");
    mkmsg((txt, nullptr, npos, npos, 8+3), "(11B): ccc\n", "(11B): error:\nerror:\nerror:     ccc\n");
    mkmsg((txt, nullptr, npos, 0   , npos), "col=0: \n", "col=0: error:\nerror:\nerror:     \n");
    mkmsg((txt, nullptr, npos, 0   , 8+0), "col=0 (8B): ccc\n            |\n            (here)\n", "col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, npos, 1   , 8+1), "col=1 (9B): ccc\n             |\n             (here)\n", "col=1 (9B): error:\nerror:\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, nullptr, npos, 2   , 8+2), "col=2 (10B): ccc\n               |\n               (here)\n", "col=2 (10B): error:\nerror:\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, nullptr, npos, 3   , 8+3), "col=3 (11B): ccc\n                |\n                (here)\n", "col=3 (11B): error:\nerror:\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, nullptr, npos, npos, npos, 1), "", "");
    mkmsg((txt, nullptr, npos, npos, 8+0 , 1), "(8B): ccc\n(8B): see region:\nbbb\nccc\n", "(8B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, npos, npos, 8+1 , 1), "(9B): ccc\n(9B): see region:\nbbb\nccc\n", "(9B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, npos, npos, 8+2 , 1), "(10B): ccc\n(10B): see region:\nbbb\nccc\n", "(10B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, npos, npos, 8+3 , 1), "(11B): ccc\n(11B): see region:\nbbb\nccc\n", "(11B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, npos, 0   , npos, 1), "col=0: \ncol=0: see region:\n", "col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, nullptr, npos, 0   , 8+0 , 1), "col=0 (8B): ccc\n            |\n            (here)\ncol=0 (8B): see region:\nbbb\nccc\n|\n(here)\n", "col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, npos, 1   , 8+1 , 1), "col=1 (9B): ccc\n             |\n             (here)\ncol=1 (9B): see region:\nbbb\nccc\n |\n (here)\n", "col=1 (9B): error:\nerror:\nerror:     ccc\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, nullptr, npos, 2   , 8+2 , 1), "col=2 (10B): ccc\n               |\n               (here)\ncol=2 (10B): see region:\nbbb\nccc\n  |\n  (here)\n", "col=2 (10B): error:\nerror:\nerror:     ccc\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, nullptr, npos, 3   , 8+3 , 1), "col=3 (11B): ccc\n                |\n                (here)\ncol=3 (11B): see region:\nbbb\nccc\n   |\n   (here)\n", "col=3 (11B): error:\nerror:\nerror:     ccc\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, nullptr, npos, npos, npos, 2), "", "");
    mkmsg((txt, nullptr, npos, npos, 8+0 , 2), "(8B): ccc\n(8B): see region:\naaa\nbbb\nccc\n", "(8B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, npos, npos, 8+1 , 2), "(9B): ccc\n(9B): see region:\naaa\nbbb\nccc\n", "(9B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, npos, npos, 8+2 , 2), "(10B): ccc\n(10B): see region:\naaa\nbbb\nccc\n", "(10B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, npos, npos, 8+3 , 2), "(11B): ccc\n(11B): see region:\naaa\nbbb\nccc\n", "(11B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, npos, 0   , npos, 2), "col=0: \ncol=0: see region:\n", "col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, nullptr, npos, 0   , 8+0 , 2), "col=0 (8B): ccc\n            |\n            (here)\ncol=0 (8B): see region:\naaa\nbbb\nccc\n|\n(here)\n", "col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, npos, 1   , 8+1 , 2), "col=1 (9B): ccc\n             |\n             (here)\ncol=1 (9B): see region:\naaa\nbbb\nccc\n |\n (here)\n", "col=1 (9B): error:\nerror:\nerror:     ccc\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, nullptr, npos, 2   , 8+2 , 2), "col=2 (10B): ccc\n               |\n               (here)\ncol=2 (10B): see region:\naaa\nbbb\nccc\n  |\n  (here)\n", "col=2 (10B): error:\nerror:\nerror:     ccc\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, nullptr, npos, 3   , 8+3 , 2), "col=3 (11B): ccc\n                |\n                (here)\ncol=3 (11B): see region:\naaa\nbbb\nccc\n   |\n   (here)\n", "col=3 (11B): error:\nerror:\nerror:     ccc\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, nullptr, npos, npos, npos, 3), "", "");
    mkmsg((txt, nullptr, npos, npos, 8+0 , 3), "(8B): ccc\n(8B): see region:\naaa\nbbb\nccc\n", "(8B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, npos, npos, 8+1 , 3), "(9B): ccc\n(9B): see region:\naaa\nbbb\nccc\n", "(9B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, npos, npos, 8+2 , 3), "(10B): ccc\n(10B): see region:\naaa\nbbb\nccc\n", "(10B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, npos, npos, 8+3 , 3), "(11B): ccc\n(11B): see region:\naaa\nbbb\nccc\n", "(11B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, npos, 0   , npos, 3), "col=0: \ncol=0: see region:\n", "col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, nullptr, npos, 0   , 8+0 , 3), "col=0 (8B): ccc\n            |\n            (here)\ncol=0 (8B): see region:\naaa\nbbb\nccc\n|\n(here)\n", "col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, npos, 1   , 8+1 , 3), "col=1 (9B): ccc\n             |\n             (here)\ncol=1 (9B): see region:\naaa\nbbb\nccc\n |\n (here)\n", "col=1 (9B): error:\nerror:\nerror:     ccc\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, nullptr, npos, 2   , 8+2 , 3), "col=2 (10B): ccc\n               |\n               (here)\ncol=2 (10B): see region:\naaa\nbbb\nccc\n  |\n  (here)\n", "col=2 (10B): error:\nerror:\nerror:     ccc\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, nullptr, npos, 3   , 8+3 , 3), "col=3 (11B): ccc\n                |\n                (here)\ncol=3 (11B): see region:\naaa\nbbb\nccc\n   |\n   (here)\n", "col=3 (11B): error:\nerror:\nerror:     ccc\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    //
    //     src  file     line col  offset
    mkmsg((txt, nullptr, 10, npos, npos), "line=10: \n", "line=10: error:\nerror:\nerror:     \n");
    mkmsg((txt, nullptr, 11, npos, 0), "line=11: (0B): aaa\n", "line=11: (0B): error:\nerror:\nerror:     aaa\n");
    mkmsg((txt, nullptr, 12, npos, 1), "line=12: (1B): aaa\n", "line=12: (1B): error:\nerror:\nerror:     aaa\n");
    mkmsg((txt, nullptr, 13, npos, 2), "line=13: (2B): aaa\n", "line=13: (2B): error:\nerror:\nerror:     aaa\n");
    mkmsg((txt, nullptr, 14, npos, 3), "line=14: (3B): aaa\n", "line=14: (3B): error:\nerror:\nerror:     aaa\n");
    mkmsg((txt, nullptr, 10, 0   , npos), "line=10 col=0: \n", "line=10 col=0: error:\nerror:\nerror:     \n");
    mkmsg((txt, nullptr, 11, 0   , 0), "line=11 col=0 (0B): aaa\n                    |\n                    (here)\n", "line=11 col=0 (0B): error:\nerror:\nerror:     aaa\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, 12, 1   , 1), "line=12 col=1 (1B): aaa\n                     |\n                     (here)\n", "line=12 col=1 (1B): error:\nerror:\nerror:     aaa\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, nullptr, 13, 2   , 2), "line=13 col=2 (2B): aaa\n                      |\n                      (here)\n", "line=13 col=2 (2B): error:\nerror:\nerror:     aaa\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, nullptr, 14, 3   , 3), "line=14 col=3 (3B): aaa\n                       |\n                       (here)\n", "line=14 col=3 (3B): error:\nerror:\nerror:     aaa\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, nullptr, 10, npos, npos), "line=10: \n", "line=10: error:\nerror:\nerror:     \n");
    mkmsg((txt, nullptr, 11, npos, 4+0), "line=11: (4B): bbb\n", "line=11: (4B): error:\nerror:\nerror:     bbb\n");
    mkmsg((txt, nullptr, 12, npos, 4+1), "line=12: (5B): bbb\n", "line=12: (5B): error:\nerror:\nerror:     bbb\n");
    mkmsg((txt, nullptr, 13, npos, 4+2), "line=13: (6B): bbb\n", "line=13: (6B): error:\nerror:\nerror:     bbb\n");
    mkmsg((txt, nullptr, 14, npos, 4+3), "line=14: (7B): bbb\n", "line=14: (7B): error:\nerror:\nerror:     bbb\n");
    mkmsg((txt, nullptr, 10, 0   , 4+0), "line=10 col=0 (4B): bbb\n                    |\n                    (here)\n", "line=10 col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, 12, 0   , 4+1), "line=12 col=0 (5B): bbb\n                    |\n                    (here)\n", "line=12 col=0 (5B): error:\nerror:\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, 12, 1   , 4+1), "line=12 col=1 (5B): bbb\n                     |\n                     (here)\n", "line=12 col=1 (5B): error:\nerror:\nerror:     bbb\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, nullptr, 13, 2   , 4+2), "line=13 col=2 (6B): bbb\n                      |\n                      (here)\n", "line=13 col=2 (6B): error:\nerror:\nerror:     bbb\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, nullptr, 14, 3   , 4+3), "line=14 col=3 (7B): bbb\n                       |\n                       (here)\n", "line=14 col=3 (7B): error:\nerror:\nerror:     bbb\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, nullptr, 10, npos, npos, 1), "line=10: \nline=10: see region:\n", "line=10: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, nullptr, 11, npos, 4+0 , 1), "line=11: (4B): bbb\nline=11: (4B): see region:\naaa\nbbb\n", "line=11: (4B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, nullptr, 12, npos, 4+1 , 1), "line=12: (5B): bbb\nline=12: (5B): see region:\naaa\nbbb\n", "line=12: (5B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, nullptr, 13, npos, 4+2 , 1), "line=13: (6B): bbb\nline=13: (6B): see region:\naaa\nbbb\n", "line=13: (6B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, nullptr, 14, npos, 4+3 , 1), "line=14: (7B): bbb\nline=14: (7B): see region:\naaa\nbbb\n", "line=14: (7B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, nullptr, 10, 0   , npos, 1), "line=10 col=0: \nline=10 col=0: see region:\n", "line=10 col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, nullptr, 11, 0   , 4+0 , 1), "line=11 col=0 (4B): bbb\n                    |\n                    (here)\nline=11 col=0 (4B): see region:\naaa\nbbb\n|\n(here)\n", "line=11 col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, 12, 1   , 4+1 , 1), "line=12 col=1 (5B): bbb\n                     |\n                     (here)\nline=12 col=1 (5B): see region:\naaa\nbbb\n |\n (here)\n", "line=12 col=1 (5B): error:\nerror:\nerror:     bbb\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, nullptr, 13, 2   , 4+2 , 1), "line=13 col=2 (6B): bbb\n                      |\n                      (here)\nline=13 col=2 (6B): see region:\naaa\nbbb\n  |\n  (here)\n", "line=13 col=2 (6B): error:\nerror:\nerror:     bbb\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, nullptr, 14, 3   , 4+3 , 1), "line=14 col=3 (7B): bbb\n                       |\n                       (here)\nline=14 col=3 (7B): see region:\naaa\nbbb\n   |\n   (here)\n", "line=14 col=3 (7B): error:\nerror:\nerror:     bbb\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, nullptr, 10, npos, npos, 2), "line=10: \nline=10: see region:\n", "line=10: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, nullptr, 11, npos, 4+0 , 2), "line=11: (4B): bbb\nline=11: (4B): see region:\naaa\nbbb\n", "line=11: (4B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, nullptr, 12, npos, 4+1 , 2), "line=12: (5B): bbb\nline=12: (5B): see region:\naaa\nbbb\n", "line=12: (5B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, nullptr, 13, npos, 4+2 , 2), "line=13: (6B): bbb\nline=13: (6B): see region:\naaa\nbbb\n", "line=13: (6B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, nullptr, 14, npos, 4+3 , 2), "line=14: (7B): bbb\nline=14: (7B): see region:\naaa\nbbb\n", "line=14: (7B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, nullptr, 10, 0   , npos, 2), "line=10 col=0: \nline=10 col=0: see region:\n", "line=10 col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, nullptr, 11, 0   , 4+0 , 2), "line=11 col=0 (4B): bbb\n                    |\n                    (here)\nline=11 col=0 (4B): see region:\naaa\nbbb\n|\n(here)\n", "line=11 col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, 12, 1   , 4+1 , 2), "line=12 col=1 (5B): bbb\n                     |\n                     (here)\nline=12 col=1 (5B): see region:\naaa\nbbb\n |\n (here)\n", "line=12 col=1 (5B): error:\nerror:\nerror:     bbb\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, nullptr, 13, 2   , 4+2 , 2), "line=13 col=2 (6B): bbb\n                      |\n                      (here)\nline=13 col=2 (6B): see region:\naaa\nbbb\n  |\n  (here)\n", "line=13 col=2 (6B): error:\nerror:\nerror:     bbb\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, nullptr, 14, 3   , 4+3 , 2), "line=14 col=3 (7B): bbb\n                       |\n                       (here)\nline=14 col=3 (7B): see region:\naaa\nbbb\n   |\n   (here)\n", "line=14 col=3 (7B): error:\nerror:\nerror:     bbb\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, nullptr, 10, npos, npos), "line=10: \n", "line=10: error:\nerror:\nerror:     \n");
    mkmsg((txt, nullptr, 11, npos, 8+0), "line=11: (8B): ccc\n", "line=11: (8B): error:\nerror:\nerror:     ccc\n");
    mkmsg((txt, nullptr, 12, npos, 8+1), "line=12: (9B): ccc\n", "line=12: (9B): error:\nerror:\nerror:     ccc\n");
    mkmsg((txt, nullptr, 13, npos, 8+2), "line=13: (10B): ccc\n", "line=13: (10B): error:\nerror:\nerror:     ccc\n");
    mkmsg((txt, nullptr, 14, npos, 8+3), "line=14: (11B): ccc\n", "line=14: (11B): error:\nerror:\nerror:     ccc\n");
    mkmsg((txt, nullptr, 10, 0   , npos), "line=10 col=0: \n", "line=10 col=0: error:\nerror:\nerror:     \n");
    mkmsg((txt, nullptr, 11, 0   , 8+0), "line=11 col=0 (8B): ccc\n                    |\n                    (here)\n", "line=11 col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, 12, 1   , 8+1), "line=12 col=1 (9B): ccc\n                     |\n                     (here)\n", "line=12 col=1 (9B): error:\nerror:\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, nullptr, 13, 2   , 8+2), "line=13 col=2 (10B): ccc\n                       |\n                       (here)\n", "line=13 col=2 (10B): error:\nerror:\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, nullptr, 14, 3   , 8+3), "line=14 col=3 (11B): ccc\n                        |\n                        (here)\n", "line=14 col=3 (11B): error:\nerror:\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, nullptr, 10, npos, npos, 1), "line=10: \nline=10: see region:\n", "line=10: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, nullptr, 11, npos, 8+0 , 1), "line=11: (8B): ccc\nline=11: (8B): see region:\nbbb\nccc\n", "line=11: (8B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, 12, npos, 8+1 , 1), "line=12: (9B): ccc\nline=12: (9B): see region:\nbbb\nccc\n", "line=12: (9B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, 13, npos, 8+2 , 1), "line=13: (10B): ccc\nline=13: (10B): see region:\nbbb\nccc\n", "line=13: (10B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, 14, npos, 8+3 , 1), "line=14: (11B): ccc\nline=14: (11B): see region:\nbbb\nccc\n", "line=14: (11B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, 10, 0   , npos, 1), "line=10 col=0: \nline=10 col=0: see region:\n", "line=10 col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, nullptr, 11, 0   , 8+0 , 1), "line=11 col=0 (8B): ccc\n                    |\n                    (here)\nline=11 col=0 (8B): see region:\nbbb\nccc\n|\n(here)\n", "line=11 col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, 12, 1   , 8+1 , 1), "line=12 col=1 (9B): ccc\n                     |\n                     (here)\nline=12 col=1 (9B): see region:\nbbb\nccc\n |\n (here)\n", "line=12 col=1 (9B): error:\nerror:\nerror:     ccc\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, nullptr, 13, 2   , 8+2 , 1), "line=13 col=2 (10B): ccc\n                       |\n                       (here)\nline=13 col=2 (10B): see region:\nbbb\nccc\n  |\n  (here)\n", "line=13 col=2 (10B): error:\nerror:\nerror:     ccc\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, nullptr, 14, 3   , 8+3 , 1), "line=14 col=3 (11B): ccc\n                        |\n                        (here)\nline=14 col=3 (11B): see region:\nbbb\nccc\n   |\n   (here)\n", "line=14 col=3 (11B): error:\nerror:\nerror:     ccc\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, nullptr, 10, npos, npos, 2), "line=10: \nline=10: see region:\n", "line=10: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, nullptr, 11, npos, 8+0 , 2), "line=11: (8B): ccc\nline=11: (8B): see region:\naaa\nbbb\nccc\n", "line=11: (8B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, 12, npos, 8+1 , 2), "line=12: (9B): ccc\nline=12: (9B): see region:\naaa\nbbb\nccc\n", "line=12: (9B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, 13, npos, 8+2 , 2), "line=13: (10B): ccc\nline=13: (10B): see region:\naaa\nbbb\nccc\n", "line=13: (10B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, 14, npos, 8+3 , 2), "line=14: (11B): ccc\nline=14: (11B): see region:\naaa\nbbb\nccc\n", "line=14: (11B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, 10, 0   , npos, 2), "line=10 col=0: \nline=10 col=0: see region:\n", "line=10 col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, nullptr, 11, 0   , 8+0 , 2), "line=11 col=0 (8B): ccc\n                    |\n                    (here)\nline=11 col=0 (8B): see region:\naaa\nbbb\nccc\n|\n(here)\n", "line=11 col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, 12, 1   , 8+1 , 2), "line=12 col=1 (9B): ccc\n                     |\n                     (here)\nline=12 col=1 (9B): see region:\naaa\nbbb\nccc\n |\n (here)\n", "line=12 col=1 (9B): error:\nerror:\nerror:     ccc\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, nullptr, 13, 2   , 8+2 , 2), "line=13 col=2 (10B): ccc\n                       |\n                       (here)\nline=13 col=2 (10B): see region:\naaa\nbbb\nccc\n  |\n  (here)\n", "line=13 col=2 (10B): error:\nerror:\nerror:     ccc\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, nullptr, 14, 3   , 8+3 , 2), "line=14 col=3 (11B): ccc\n                        |\n                        (here)\nline=14 col=3 (11B): see region:\naaa\nbbb\nccc\n   |\n   (here)\n", "line=14 col=3 (11B): error:\nerror:\nerror:     ccc\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, nullptr, 10, npos, npos, 3), "line=10: \nline=10: see region:\n", "line=10: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, nullptr, 11, npos, 8+0 , 3), "line=11: (8B): ccc\nline=11: (8B): see region:\naaa\nbbb\nccc\n", "line=11: (8B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, 12, npos, 8+1 , 3), "line=12: (9B): ccc\nline=12: (9B): see region:\naaa\nbbb\nccc\n", "line=12: (9B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, 13, npos, 8+2 , 3), "line=13: (10B): ccc\nline=13: (10B): see region:\naaa\nbbb\nccc\n", "line=13: (10B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, 14, npos, 8+3 , 3), "line=14: (11B): ccc\nline=14: (11B): see region:\naaa\nbbb\nccc\n", "line=14: (11B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, nullptr, 10, 0   , npos, 3), "line=10 col=0: \nline=10 col=0: see region:\n", "line=10 col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, nullptr, 11, 0   , 8+0 , 3), "line=11 col=0 (8B): ccc\n                    |\n                    (here)\nline=11 col=0 (8B): see region:\naaa\nbbb\nccc\n|\n(here)\n", "line=11 col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, nullptr, 12, 1   , 8+1 , 3), "line=12 col=1 (9B): ccc\n                     |\n                     (here)\nline=12 col=1 (9B): see region:\naaa\nbbb\nccc\n |\n (here)\n", "line=12 col=1 (9B): error:\nerror:\nerror:     ccc\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, nullptr, 13, 2   , 8+2 , 3), "line=13 col=2 (10B): ccc\n                       |\n                       (here)\nline=13 col=2 (10B): see region:\naaa\nbbb\nccc\n  |\n  (here)\n", "line=13 col=2 (10B): error:\nerror:\nerror:     ccc\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, nullptr, 14, 3   , 8+3 , 3), "line=14 col=3 (11B): ccc\n                        |\n                        (here)\nline=14 col=3 (11B): see region:\naaa\nbbb\nccc\n   |\n   (here)\n", "line=14 col=3 (11B): error:\nerror:\nerror:     ccc\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    //
    //     src  file      line     col   offset
    mkmsg((txt, "file.yml", npos, npos, npos), "file.yml: \n", "file.yml: error:\nerror:\nerror:     \n");
    mkmsg((txt, "file.yml", npos, npos, 0), "file.yml: (0B): aaa\n", "file.yml: (0B): error:\nerror:\nerror:     aaa\n");
    mkmsg((txt, "file.yml", npos, npos, 1), "file.yml: (1B): aaa\n", "file.yml: (1B): error:\nerror:\nerror:     aaa\n");
    mkmsg((txt, "file.yml", npos, npos, 2), "file.yml: (2B): aaa\n", "file.yml: (2B): error:\nerror:\nerror:     aaa\n");
    mkmsg((txt, "file.yml", npos, npos, 3), "file.yml: (3B): aaa\n", "file.yml: (3B): error:\nerror:\nerror:     aaa\n");
    mkmsg((txt, "file.yml", npos, 0   , npos), "file.yml: col=0: \n", "file.yml: col=0: error:\nerror:\nerror:     \n");
    mkmsg((txt, "file.yml", npos, 0   , 0), "file.yml: col=0 (0B): aaa\n                      |\n                      (here)\n", "file.yml: col=0 (0B): error:\nerror:\nerror:     aaa\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", npos, 1   , 1), "file.yml: col=1 (1B): aaa\n                       |\n                       (here)\n", "file.yml: col=1 (1B): error:\nerror:\nerror:     aaa\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", npos, 2   , 2), "file.yml: col=2 (2B): aaa\n                        |\n                        (here)\n", "file.yml: col=2 (2B): error:\nerror:\nerror:     aaa\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", npos, 3   , 3), "file.yml: col=3 (3B): aaa\n                         |\n                         (here)\n", "file.yml: col=3 (3B): error:\nerror:\nerror:     aaa\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", npos, npos, npos), "file.yml: \n", "file.yml: error:\nerror:\nerror:     \n");
    mkmsg((txt, "file.yml", npos, npos, 4+0), "file.yml: (4B): bbb\n", "file.yml: (4B): error:\nerror:\nerror:     bbb\n");
    mkmsg((txt, "file.yml", npos, npos, 4+1), "file.yml: (5B): bbb\n", "file.yml: (5B): error:\nerror:\nerror:     bbb\n");
    mkmsg((txt, "file.yml", npos, npos, 4+2), "file.yml: (6B): bbb\n", "file.yml: (6B): error:\nerror:\nerror:     bbb\n");
    mkmsg((txt, "file.yml", npos, npos, 4+3), "file.yml: (7B): bbb\n", "file.yml: (7B): error:\nerror:\nerror:     bbb\n");
    mkmsg((txt, "file.yml", npos, 0   , 4+0), "file.yml: col=0 (4B): bbb\n                      |\n                      (here)\n", "file.yml: col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", npos, 0   , 4+1), "file.yml: col=0 (5B): bbb\n                      |\n                      (here)\n", "file.yml: col=0 (5B): error:\nerror:\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", npos, 1   , 4+1), "file.yml: col=1 (5B): bbb\n                       |\n                       (here)\n", "file.yml: col=1 (5B): error:\nerror:\nerror:     bbb\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", npos, 2   , 4+2), "file.yml: col=2 (6B): bbb\n                        |\n                        (here)\n", "file.yml: col=2 (6B): error:\nerror:\nerror:     bbb\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", npos, 3   , 4+3), "file.yml: col=3 (7B): bbb\n                         |\n                         (here)\n", "file.yml: col=3 (7B): error:\nerror:\nerror:     bbb\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", npos, npos, npos, 1), "file.yml: \nfile.yml: see region:\n", "file.yml: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", npos, npos, 4+0 , 1), "file.yml: (4B): bbb\nfile.yml: (4B): see region:\naaa\nbbb\n", "file.yml: (4B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, "file.yml", npos, npos, 4+1 , 1), "file.yml: (5B): bbb\nfile.yml: (5B): see region:\naaa\nbbb\n", "file.yml: (5B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, "file.yml", npos, npos, 4+2 , 1), "file.yml: (6B): bbb\nfile.yml: (6B): see region:\naaa\nbbb\n", "file.yml: (6B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, "file.yml", npos, npos, 4+3 , 1), "file.yml: (7B): bbb\nfile.yml: (7B): see region:\naaa\nbbb\n", "file.yml: (7B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, "file.yml", npos, 0   , npos, 1), "file.yml: col=0: \nfile.yml: col=0: see region:\n", "file.yml: col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", npos, 0   , 4+0 , 1), "file.yml: col=0 (4B): bbb\n                      |\n                      (here)\nfile.yml: col=0 (4B): see region:\naaa\nbbb\n|\n(here)\n", "file.yml: col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", npos, 1   , 4+1 , 1), "file.yml: col=1 (5B): bbb\n                       |\n                       (here)\nfile.yml: col=1 (5B): see region:\naaa\nbbb\n |\n (here)\n", "file.yml: col=1 (5B): error:\nerror:\nerror:     bbb\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", npos, 2   , 4+2 , 1), "file.yml: col=2 (6B): bbb\n                        |\n                        (here)\nfile.yml: col=2 (6B): see region:\naaa\nbbb\n  |\n  (here)\n", "file.yml: col=2 (6B): error:\nerror:\nerror:     bbb\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", npos, 3   , 4+3 , 1), "file.yml: col=3 (7B): bbb\n                         |\n                         (here)\nfile.yml: col=3 (7B): see region:\naaa\nbbb\n   |\n   (here)\n", "file.yml: col=3 (7B): error:\nerror:\nerror:     bbb\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", npos, npos, npos, 2), "file.yml: \nfile.yml: see region:\n", "file.yml: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", npos, npos, 4+0 , 2), "file.yml: (4B): bbb\nfile.yml: (4B): see region:\naaa\nbbb\n", "file.yml: (4B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, "file.yml", npos, npos, 4+1 , 2), "file.yml: (5B): bbb\nfile.yml: (5B): see region:\naaa\nbbb\n", "file.yml: (5B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, "file.yml", npos, npos, 4+2 , 2), "file.yml: (6B): bbb\nfile.yml: (6B): see region:\naaa\nbbb\n", "file.yml: (6B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, "file.yml", npos, npos, 4+3 , 2), "file.yml: (7B): bbb\nfile.yml: (7B): see region:\naaa\nbbb\n", "file.yml: (7B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, "file.yml", npos, 0   , npos, 2), "file.yml: col=0: \nfile.yml: col=0: see region:\n", "file.yml: col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", npos, 0   , 4+0 , 2), "file.yml: col=0 (4B): bbb\n                      |\n                      (here)\nfile.yml: col=0 (4B): see region:\naaa\nbbb\n|\n(here)\n", "file.yml: col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", npos, 1   , 4+1 , 2), "file.yml: col=1 (5B): bbb\n                       |\n                       (here)\nfile.yml: col=1 (5B): see region:\naaa\nbbb\n |\n (here)\n", "file.yml: col=1 (5B): error:\nerror:\nerror:     bbb\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", npos, 2   , 4+2 , 2), "file.yml: col=2 (6B): bbb\n                        |\n                        (here)\nfile.yml: col=2 (6B): see region:\naaa\nbbb\n  |\n  (here)\n", "file.yml: col=2 (6B): error:\nerror:\nerror:     bbb\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", npos, 3   , 4+3 , 2), "file.yml: col=3 (7B): bbb\n                         |\n                         (here)\nfile.yml: col=3 (7B): see region:\naaa\nbbb\n   |\n   (here)\n", "file.yml: col=3 (7B): error:\nerror:\nerror:     bbb\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", npos, npos, npos), "file.yml: \n", "file.yml: error:\nerror:\nerror:     \n");
    mkmsg((txt, "file.yml", npos, npos, 8+0), "file.yml: (8B): ccc\n", "file.yml: (8B): error:\nerror:\nerror:     ccc\n");
    mkmsg((txt, "file.yml", npos, npos, 8+1), "file.yml: (9B): ccc\n", "file.yml: (9B): error:\nerror:\nerror:     ccc\n");
    mkmsg((txt, "file.yml", npos, npos, 8+2), "file.yml: (10B): ccc\n", "file.yml: (10B): error:\nerror:\nerror:     ccc\n");
    mkmsg((txt, "file.yml", npos, npos, 8+3), "file.yml: (11B): ccc\n", "file.yml: (11B): error:\nerror:\nerror:     ccc\n");
    mkmsg((txt, "file.yml", npos, 0   , npos), "file.yml: col=0: \n", "file.yml: col=0: error:\nerror:\nerror:     \n");
    mkmsg((txt, "file.yml", npos, 0   , 8+0), "file.yml: col=0 (8B): ccc\n                      |\n                      (here)\n", "file.yml: col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", npos, 1   , 8+1), "file.yml: col=1 (9B): ccc\n                       |\n                       (here)\n", "file.yml: col=1 (9B): error:\nerror:\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", npos, 2   , 8+2), "file.yml: col=2 (10B): ccc\n                         |\n                         (here)\n", "file.yml: col=2 (10B): error:\nerror:\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", npos, 3   , 8+3), "file.yml: col=3 (11B): ccc\n                          |\n                          (here)\n", "file.yml: col=3 (11B): error:\nerror:\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", npos, npos, npos, 1), "file.yml: \nfile.yml: see region:\n", "file.yml: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", npos, npos, 8+0 , 1), "file.yml: (8B): ccc\nfile.yml: (8B): see region:\nbbb\nccc\n", "file.yml: (8B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", npos, npos, 8+1 , 1), "file.yml: (9B): ccc\nfile.yml: (9B): see region:\nbbb\nccc\n", "file.yml: (9B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", npos, npos, 8+2 , 1), "file.yml: (10B): ccc\nfile.yml: (10B): see region:\nbbb\nccc\n", "file.yml: (10B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", npos, npos, 8+3 , 1), "file.yml: (11B): ccc\nfile.yml: (11B): see region:\nbbb\nccc\n", "file.yml: (11B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", npos, 0   , npos, 1), "file.yml: col=0: \nfile.yml: col=0: see region:\n", "file.yml: col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", npos, 0   , 8+0 , 1), "file.yml: col=0 (8B): ccc\n                      |\n                      (here)\nfile.yml: col=0 (8B): see region:\nbbb\nccc\n|\n(here)\n", "file.yml: col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", npos, 1   , 8+1 , 1), "file.yml: col=1 (9B): ccc\n                       |\n                       (here)\nfile.yml: col=1 (9B): see region:\nbbb\nccc\n |\n (here)\n", "file.yml: col=1 (9B): error:\nerror:\nerror:     ccc\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", npos, 2   , 8+2 , 1), "file.yml: col=2 (10B): ccc\n                         |\n                         (here)\nfile.yml: col=2 (10B): see region:\nbbb\nccc\n  |\n  (here)\n", "file.yml: col=2 (10B): error:\nerror:\nerror:     ccc\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", npos, 3   , 8+3 , 1), "file.yml: col=3 (11B): ccc\n                          |\n                          (here)\nfile.yml: col=3 (11B): see region:\nbbb\nccc\n   |\n   (here)\n", "file.yml: col=3 (11B): error:\nerror:\nerror:     ccc\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", npos, npos, npos, 2), "file.yml: \nfile.yml: see region:\n", "file.yml: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", npos, npos, 8+0 , 2), "file.yml: (8B): ccc\nfile.yml: (8B): see region:\naaa\nbbb\nccc\n", "file.yml: (8B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", npos, npos, 8+1 , 2), "file.yml: (9B): ccc\nfile.yml: (9B): see region:\naaa\nbbb\nccc\n", "file.yml: (9B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", npos, npos, 8+2 , 2), "file.yml: (10B): ccc\nfile.yml: (10B): see region:\naaa\nbbb\nccc\n", "file.yml: (10B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", npos, npos, 8+3 , 2), "file.yml: (11B): ccc\nfile.yml: (11B): see region:\naaa\nbbb\nccc\n", "file.yml: (11B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", npos, 0   , npos, 2), "file.yml: col=0: \nfile.yml: col=0: see region:\n", "file.yml: col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", npos, 0   , 8+0 , 2), "file.yml: col=0 (8B): ccc\n                      |\n                      (here)\nfile.yml: col=0 (8B): see region:\naaa\nbbb\nccc\n|\n(here)\n", "file.yml: col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", npos, 1   , 8+1 , 2), "file.yml: col=1 (9B): ccc\n                       |\n                       (here)\nfile.yml: col=1 (9B): see region:\naaa\nbbb\nccc\n |\n (here)\n", "file.yml: col=1 (9B): error:\nerror:\nerror:     ccc\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", npos, 2   , 8+2 , 2), "file.yml: col=2 (10B): ccc\n                         |\n                         (here)\nfile.yml: col=2 (10B): see region:\naaa\nbbb\nccc\n  |\n  (here)\n", "file.yml: col=2 (10B): error:\nerror:\nerror:     ccc\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", npos, 3   , 8+3 , 2), "file.yml: col=3 (11B): ccc\n                          |\n                          (here)\nfile.yml: col=3 (11B): see region:\naaa\nbbb\nccc\n   |\n   (here)\n", "file.yml: col=3 (11B): error:\nerror:\nerror:     ccc\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", npos, npos, npos, 3), "file.yml: \nfile.yml: see region:\n", "file.yml: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", npos, npos, 8+0 , 3), "file.yml: (8B): ccc\nfile.yml: (8B): see region:\naaa\nbbb\nccc\n", "file.yml: (8B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", npos, npos, 8+1 , 3), "file.yml: (9B): ccc\nfile.yml: (9B): see region:\naaa\nbbb\nccc\n", "file.yml: (9B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", npos, npos, 8+2 , 3), "file.yml: (10B): ccc\nfile.yml: (10B): see region:\naaa\nbbb\nccc\n", "file.yml: (10B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", npos, npos, 8+3 , 3), "file.yml: (11B): ccc\nfile.yml: (11B): see region:\naaa\nbbb\nccc\n", "file.yml: (11B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", npos, 0   , npos, 3), "file.yml: col=0: \nfile.yml: col=0: see region:\n", "file.yml: col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", npos, 0   , 8+0 , 3), "file.yml: col=0 (8B): ccc\n                      |\n                      (here)\nfile.yml: col=0 (8B): see region:\naaa\nbbb\nccc\n|\n(here)\n", "file.yml: col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", npos, 1   , 8+1 , 3), "file.yml: col=1 (9B): ccc\n                       |\n                       (here)\nfile.yml: col=1 (9B): see region:\naaa\nbbb\nccc\n |\n (here)\n", "file.yml: col=1 (9B): error:\nerror:\nerror:     ccc\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", npos, 2   , 8+2 , 3), "file.yml: col=2 (10B): ccc\n                         |\n                         (here)\nfile.yml: col=2 (10B): see region:\naaa\nbbb\nccc\n  |\n  (here)\n", "file.yml: col=2 (10B): error:\nerror:\nerror:     ccc\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", npos, 3   , 8+3 , 3), "file.yml: col=3 (11B): ccc\n                          |\n                          (here)\nfile.yml: col=3 (11B): see region:\naaa\nbbb\nccc\n   |\n   (here)\n", "file.yml: col=3 (11B): error:\nerror:\nerror:     ccc\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    //
    //     src  file      line  col   offset
    mkmsg((txt, "file.yml", 10, npos, npos), "file.yml:10: \n", "file.yml:10: error:\nerror:\nerror:     \n");
    mkmsg((txt, "file.yml", 11, npos, 0), "file.yml:11: (0B): aaa\n", "file.yml:11: (0B): error:\nerror:\nerror:     aaa\n");
    mkmsg((txt, "file.yml", 12, npos, 1), "file.yml:12: (1B): aaa\n", "file.yml:12: (1B): error:\nerror:\nerror:     aaa\n");
    mkmsg((txt, "file.yml", 13, npos, 2), "file.yml:13: (2B): aaa\n", "file.yml:13: (2B): error:\nerror:\nerror:     aaa\n");
    mkmsg((txt, "file.yml", 14, npos, 3), "file.yml:14: (3B): aaa\n", "file.yml:14: (3B): error:\nerror:\nerror:     aaa\n");
    mkmsg((txt, "file.yml", 10, 0   , npos), "file.yml:10: col=0: \n", "file.yml:10: col=0: error:\nerror:\nerror:     \n");
    mkmsg((txt, "file.yml", 11, 0   , 0), "file.yml:11: col=0 (0B): aaa\n                         |\n                         (here)\n", "file.yml:11: col=0 (0B): error:\nerror:\nerror:     aaa\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 1), "file.yml:12: col=1 (1B): aaa\n                          |\n                          (here)\n", "file.yml:12: col=1 (1B): error:\nerror:\nerror:     aaa\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 2), "file.yml:13: col=2 (2B): aaa\n                           |\n                           (here)\n", "file.yml:13: col=2 (2B): error:\nerror:\nerror:     aaa\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 3), "file.yml:14: col=3 (3B): aaa\n                            |\n                            (here)\n", "file.yml:14: col=3 (3B): error:\nerror:\nerror:     aaa\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 11, 0   , 0, 0, 0, 0, 2), "file.yml:11: col=0 (0B): aaa\n                         ~~  (cols 0-2/3)\n                         |\n                         (here)\n", "file.yml:11: col=0 (0B): error:\nerror:\nerror:     aaa\nerror:     ~~  (cols 0-2/3)\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 1, 0, 0, 0, 2), "file.yml:12: col=1 (1B): aaa\n                         ~~  (cols 0-2/3)\n                          |\n                          (here)\n", "file.yml:12: col=1 (1B): error:\nerror:\nerror:     aaa\nerror:     ~~  (cols 0-2/3)\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 2, 0, 0, 0, 2), "file.yml:13: col=2 (2B): aaa\n                         ~~  (cols 0-2/3)\n                           |\n                           (here)\n", "file.yml:13: col=2 (2B): error:\nerror:\nerror:     aaa\nerror:     ~~  (cols 0-2/3)\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 3, 0, 0, 0, 2), "file.yml:14: col=3 (3B): aaa\n                         ~~  (cols 0-2/3)\n                            |\n                            (here)\n", "file.yml:14: col=3 (3B): error:\nerror:\nerror:     aaa\nerror:     ~~  (cols 0-2/3)\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 11, 0   , 0, 0, 0, 1, 3), "file.yml:11: col=0 (0B): aaa\n                          ~~  (cols 1-3/3)\n                         |\n                         (here)\n", "file.yml:11: col=0 (0B): error:\nerror:\nerror:     aaa\nerror:     ~~  (cols 1-3/3)\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 1, 0, 0, 1, 3), "file.yml:12: col=1 (1B): aaa\n                          ~~  (cols 1-3/3)\n                          |\n                          (here)\n", "file.yml:12: col=1 (1B): error:\nerror:\nerror:     aaa\nerror:     ~~  (cols 1-3/3)\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 2, 0, 0, 1, 3), "file.yml:13: col=2 (2B): aaa\n                          ~~  (cols 1-3/3)\n                           |\n                           (here)\n", "file.yml:13: col=2 (2B): error:\nerror:\nerror:     aaa\nerror:     ~~  (cols 1-3/3)\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 3, 0, 0, 1, 3), "file.yml:14: col=3 (3B): aaa\n                          ~~  (cols 1-3/3)\n                            |\n                            (here)\n", "file.yml:14: col=3 (3B): error:\nerror:\nerror:     aaa\nerror:     ~~  (cols 1-3/3)\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 11, 0   , 0, 0, 0, 0, 3), "file.yml:11: col=0 (0B): aaa\n                         ~~~  (cols 0-3/3)\n                         |\n                         (here)\n", "file.yml:11: col=0 (0B): error:\nerror:\nerror:     aaa\nerror:     ~~~  (cols 0-3/3)\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 1, 0, 0, 0, 3), "file.yml:12: col=1 (1B): aaa\n                         ~~~  (cols 0-3/3)\n                          |\n                          (here)\n", "file.yml:12: col=1 (1B): error:\nerror:\nerror:     aaa\nerror:     ~~~  (cols 0-3/3)\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 2, 0, 0, 0, 3), "file.yml:13: col=2 (2B): aaa\n                         ~~~  (cols 0-3/3)\n                           |\n                           (here)\n", "file.yml:13: col=2 (2B): error:\nerror:\nerror:     aaa\nerror:     ~~~  (cols 0-3/3)\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 3, 0, 0, 0, 3), "file.yml:14: col=3 (3B): aaa\n                         ~~~  (cols 0-3/3)\n                            |\n                            (here)\n", "file.yml:14: col=3 (3B): error:\nerror:\nerror:     aaa\nerror:     ~~~  (cols 0-3/3)\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 10, npos, npos), "file.yml:10: \n", "file.yml:10: error:\nerror:\nerror:     \n");
    mkmsg((txt, "file.yml", 11, npos, 4+0), "file.yml:11: (4B): bbb\n", "file.yml:11: (4B): error:\nerror:\nerror:     bbb\n");
    mkmsg((txt, "file.yml", 12, npos, 4+1), "file.yml:12: (5B): bbb\n", "file.yml:12: (5B): error:\nerror:\nerror:     bbb\n");
    mkmsg((txt, "file.yml", 13, npos, 4+2), "file.yml:13: (6B): bbb\n", "file.yml:13: (6B): error:\nerror:\nerror:     bbb\n");
    mkmsg((txt, "file.yml", 14, npos, 4+3), "file.yml:14: (7B): bbb\n", "file.yml:14: (7B): error:\nerror:\nerror:     bbb\n");
    mkmsg((txt, "file.yml", 10, 0   , 4+0), "file.yml:10: col=0 (4B): bbb\n                         |\n                         (here)\n", "file.yml:10: col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 0   , 4+1), "file.yml:12: col=0 (5B): bbb\n                         |\n                         (here)\n", "file.yml:12: col=0 (5B): error:\nerror:\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 4+1), "file.yml:12: col=1 (5B): bbb\n                          |\n                          (here)\n", "file.yml:12: col=1 (5B): error:\nerror:\nerror:     bbb\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 4+2), "file.yml:13: col=2 (6B): bbb\n                           |\n                           (here)\n", "file.yml:13: col=2 (6B): error:\nerror:\nerror:     bbb\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 4+3), "file.yml:14: col=3 (7B): bbb\n                            |\n                            (here)\n", "file.yml:14: col=3 (7B): error:\nerror:\nerror:     bbb\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 10, 0   , 4+0, 0, 0, 0, 2), "file.yml:10: col=0 (4B): bbb\n                         ~~  (cols 0-2/3)\n                         |\n                         (here)\n", "file.yml:10: col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     ~~  (cols 0-2/3)\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 0   , 4+1, 0, 0, 0, 2), "file.yml:12: col=0 (5B): bbb\n                         ~~  (cols 0-2/3)\n                         |\n                         (here)\n", "file.yml:12: col=0 (5B): error:\nerror:\nerror:     bbb\nerror:     ~~  (cols 0-2/3)\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 4+1, 0, 0, 0, 2), "file.yml:12: col=1 (5B): bbb\n                         ~~  (cols 0-2/3)\n                          |\n                          (here)\n", "file.yml:12: col=1 (5B): error:\nerror:\nerror:     bbb\nerror:     ~~  (cols 0-2/3)\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 4+2, 0, 0, 0, 2), "file.yml:13: col=2 (6B): bbb\n                         ~~  (cols 0-2/3)\n                           |\n                           (here)\n", "file.yml:13: col=2 (6B): error:\nerror:\nerror:     bbb\nerror:     ~~  (cols 0-2/3)\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 4+3, 0, 0, 0, 2), "file.yml:14: col=3 (7B): bbb\n                         ~~  (cols 0-2/3)\n                            |\n                            (here)\n", "file.yml:14: col=3 (7B): error:\nerror:\nerror:     bbb\nerror:     ~~  (cols 0-2/3)\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 10, 0   , 4+0, 0, 0, 1, 3), "file.yml:10: col=0 (4B): bbb\n                          ~~  (cols 1-3/3)\n                         |\n                         (here)\n", "file.yml:10: col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     ~~  (cols 1-3/3)\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 0   , 4+1, 0, 0, 1, 3), "file.yml:12: col=0 (5B): bbb\n                          ~~  (cols 1-3/3)\n                         |\n                         (here)\n", "file.yml:12: col=0 (5B): error:\nerror:\nerror:     bbb\nerror:     ~~  (cols 1-3/3)\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 4+1, 0, 0, 1, 3), "file.yml:12: col=1 (5B): bbb\n                          ~~  (cols 1-3/3)\n                          |\n                          (here)\n", "file.yml:12: col=1 (5B): error:\nerror:\nerror:     bbb\nerror:     ~~  (cols 1-3/3)\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 4+2, 0, 0, 1, 3), "file.yml:13: col=2 (6B): bbb\n                          ~~  (cols 1-3/3)\n                           |\n                           (here)\n", "file.yml:13: col=2 (6B): error:\nerror:\nerror:     bbb\nerror:     ~~  (cols 1-3/3)\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 4+3, 0, 0, 1, 3), "file.yml:14: col=3 (7B): bbb\n                          ~~  (cols 1-3/3)\n                            |\n                            (here)\n", "file.yml:14: col=3 (7B): error:\nerror:\nerror:     bbb\nerror:     ~~  (cols 1-3/3)\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 10, 0   , 4+0, 0, 0, 0, 3), "file.yml:10: col=0 (4B): bbb\n                         ~~~  (cols 0-3/3)\n                         |\n                         (here)\n", "file.yml:10: col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     ~~~  (cols 0-3/3)\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 0   , 4+1, 0, 0, 0, 3), "file.yml:12: col=0 (5B): bbb\n                         ~~~  (cols 0-3/3)\n                         |\n                         (here)\n", "file.yml:12: col=0 (5B): error:\nerror:\nerror:     bbb\nerror:     ~~~  (cols 0-3/3)\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 4+1, 0, 0, 0, 3), "file.yml:12: col=1 (5B): bbb\n                         ~~~  (cols 0-3/3)\n                          |\n                          (here)\n", "file.yml:12: col=1 (5B): error:\nerror:\nerror:     bbb\nerror:     ~~~  (cols 0-3/3)\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 4+2, 0, 0, 0, 3), "file.yml:13: col=2 (6B): bbb\n                         ~~~  (cols 0-3/3)\n                           |\n                           (here)\n", "file.yml:13: col=2 (6B): error:\nerror:\nerror:     bbb\nerror:     ~~~  (cols 0-3/3)\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 4+3, 0, 0, 0, 3), "file.yml:14: col=3 (7B): bbb\n                         ~~~  (cols 0-3/3)\n                            |\n                            (here)\n", "file.yml:14: col=3 (7B): error:\nerror:\nerror:     bbb\nerror:     ~~~  (cols 0-3/3)\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 10, npos, npos, 1), "file.yml:10: \nfile.yml:10: see region:\n", "file.yml:10: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", 11, npos, 4+0 , 1), "file.yml:11: (4B): bbb\nfile.yml:11: (4B): see region:\naaa\nbbb\n", "file.yml:11: (4B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, "file.yml", 12, npos, 4+1 , 1), "file.yml:12: (5B): bbb\nfile.yml:12: (5B): see region:\naaa\nbbb\n", "file.yml:12: (5B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, "file.yml", 13, npos, 4+2 , 1), "file.yml:13: (6B): bbb\nfile.yml:13: (6B): see region:\naaa\nbbb\n", "file.yml:13: (6B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, "file.yml", 14, npos, 4+3 , 1), "file.yml:14: (7B): bbb\nfile.yml:14: (7B): see region:\naaa\nbbb\n", "file.yml:14: (7B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, "file.yml", 10, 0   , npos, 1), "file.yml:10: col=0: \nfile.yml:10: col=0: see region:\n", "file.yml:10: col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", 11, 0   , 4+0 , 1), "file.yml:11: col=0 (4B): bbb\n                         |\n                         (here)\nfile.yml:11: col=0 (4B): see region:\naaa\nbbb\n|\n(here)\n", "file.yml:11: col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 4+1 , 1), "file.yml:12: col=1 (5B): bbb\n                          |\n                          (here)\nfile.yml:12: col=1 (5B): see region:\naaa\nbbb\n |\n (here)\n", "file.yml:12: col=1 (5B): error:\nerror:\nerror:     bbb\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 4+2 , 1), "file.yml:13: col=2 (6B): bbb\n                           |\n                           (here)\nfile.yml:13: col=2 (6B): see region:\naaa\nbbb\n  |\n  (here)\n", "file.yml:13: col=2 (6B): error:\nerror:\nerror:     bbb\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 4+3 , 1), "file.yml:14: col=3 (7B): bbb\n                            |\n                            (here)\nfile.yml:14: col=3 (7B): see region:\naaa\nbbb\n   |\n   (here)\n", "file.yml:14: col=3 (7B): error:\nerror:\nerror:     bbb\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 11, 0   , 4+0 , 1, 0, 0, 3), "file.yml:11: col=0 (4B): bbb\n                         ~~~  (cols 0-3/3)\n                         |\n                         (here)\nfile.yml:11: col=0 (4B): see region:\naaa\nbbb\n|\n(here)\n", "file.yml:11: col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     ~~~  (cols 0-3/3)\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 4+1 , 1, 0, 0, 3), "file.yml:12: col=1 (5B): bbb\n                         ~~~  (cols 0-3/3)\n                          |\n                          (here)\nfile.yml:12: col=1 (5B): see region:\naaa\nbbb\n |\n (here)\n", "file.yml:12: col=1 (5B): error:\nerror:\nerror:     bbb\nerror:     ~~~  (cols 0-3/3)\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 4+2 , 1, 0, 0, 3), "file.yml:13: col=2 (6B): bbb\n                         ~~~  (cols 0-3/3)\n                           |\n                           (here)\nfile.yml:13: col=2 (6B): see region:\naaa\nbbb\n  |\n  (here)\n", "file.yml:13: col=2 (6B): error:\nerror:\nerror:     bbb\nerror:     ~~~  (cols 0-3/3)\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 4+3 , 1, 0, 0, 3), "file.yml:14: col=3 (7B): bbb\n                         ~~~  (cols 0-3/3)\n                            |\n                            (here)\nfile.yml:14: col=3 (7B): see region:\naaa\nbbb\n   |\n   (here)\n", "file.yml:14: col=3 (7B): error:\nerror:\nerror:     bbb\nerror:     ~~~  (cols 0-3/3)\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 10, npos, npos, 2), "file.yml:10: \nfile.yml:10: see region:\n", "file.yml:10: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", 11, npos, 4+0 , 2), "file.yml:11: (4B): bbb\nfile.yml:11: (4B): see region:\naaa\nbbb\n", "file.yml:11: (4B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, "file.yml", 12, npos, 4+1 , 2), "file.yml:12: (5B): bbb\nfile.yml:12: (5B): see region:\naaa\nbbb\n", "file.yml:12: (5B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, "file.yml", 13, npos, 4+2 , 2), "file.yml:13: (6B): bbb\nfile.yml:13: (6B): see region:\naaa\nbbb\n", "file.yml:13: (6B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, "file.yml", 14, npos, 4+3 , 2), "file.yml:14: (7B): bbb\nfile.yml:14: (7B): see region:\naaa\nbbb\n", "file.yml:14: (7B): error:\nerror:\nerror:     bbb\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\n");
    mkmsg((txt, "file.yml", 10, 0   , npos, 2), "file.yml:10: col=0: \nfile.yml:10: col=0: see region:\n", "file.yml:10: col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", 11, 0   , 4+0 , 2), "file.yml:11: col=0 (4B): bbb\n                         |\n                         (here)\nfile.yml:11: col=0 (4B): see region:\naaa\nbbb\n|\n(here)\n", "file.yml:11: col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 4+1 , 2), "file.yml:12: col=1 (5B): bbb\n                          |\n                          (here)\nfile.yml:12: col=1 (5B): see region:\naaa\nbbb\n |\n (here)\n", "file.yml:12: col=1 (5B): error:\nerror:\nerror:     bbb\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 4+2 , 2), "file.yml:13: col=2 (6B): bbb\n                           |\n                           (here)\nfile.yml:13: col=2 (6B): see region:\naaa\nbbb\n  |\n  (here)\n", "file.yml:13: col=2 (6B): error:\nerror:\nerror:     bbb\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 4+3 , 2), "file.yml:14: col=3 (7B): bbb\n                            |\n                            (here)\nfile.yml:14: col=3 (7B): see region:\naaa\nbbb\n   |\n   (here)\n", "file.yml:14: col=3 (7B): error:\nerror:\nerror:     bbb\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 11, 0   , 4+0 , 2, 0, 0, 3), "file.yml:11: col=0 (4B): bbb\n                         ~~~  (cols 0-3/3)\n                         |\n                         (here)\nfile.yml:11: col=0 (4B): see region:\naaa\nbbb\n|\n(here)\n", "file.yml:11: col=0 (4B): error:\nerror:\nerror:     bbb\nerror:     ~~~  (cols 0-3/3)\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 4+1 , 2, 0, 0, 3), "file.yml:12: col=1 (5B): bbb\n                         ~~~  (cols 0-3/3)\n                          |\n                          (here)\nfile.yml:12: col=1 (5B): see region:\naaa\nbbb\n |\n (here)\n", "file.yml:12: col=1 (5B): error:\nerror:\nerror:     bbb\nerror:     ~~~  (cols 0-3/3)\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 4+2 , 2, 0, 0, 3), "file.yml:13: col=2 (6B): bbb\n                         ~~~  (cols 0-3/3)\n                           |\n                           (here)\nfile.yml:13: col=2 (6B): see region:\naaa\nbbb\n  |\n  (here)\n", "file.yml:13: col=2 (6B): error:\nerror:\nerror:     bbb\nerror:     ~~~  (cols 0-3/3)\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 4+3 , 2, 0, 0, 3), "file.yml:14: col=3 (7B): bbb\n                         ~~~  (cols 0-3/3)\n                            |\n                            (here)\nfile.yml:14: col=3 (7B): see region:\naaa\nbbb\n   |\n   (here)\n", "file.yml:14: col=3 (7B): error:\nerror:\nerror:     bbb\nerror:     ~~~  (cols 0-3/3)\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 10, npos, npos), "file.yml:10: \n", "file.yml:10: error:\nerror:\nerror:     \n");
    mkmsg((txt, "file.yml", 11, npos, 8+0), "file.yml:11: (8B): ccc\n", "file.yml:11: (8B): error:\nerror:\nerror:     ccc\n");
    mkmsg((txt, "file.yml", 12, npos, 8+1), "file.yml:12: (9B): ccc\n", "file.yml:12: (9B): error:\nerror:\nerror:     ccc\n");
    mkmsg((txt, "file.yml", 13, npos, 8+2), "file.yml:13: (10B): ccc\n", "file.yml:13: (10B): error:\nerror:\nerror:     ccc\n");
    mkmsg((txt, "file.yml", 14, npos, 8+3), "file.yml:14: (11B): ccc\n", "file.yml:14: (11B): error:\nerror:\nerror:     ccc\n");
    mkmsg((txt, "file.yml", 10, 0   , npos), "file.yml:10: col=0: \n", "file.yml:10: col=0: error:\nerror:\nerror:     \n");
    mkmsg((txt, "file.yml", 11, 0   , 8+0), "file.yml:11: col=0 (8B): ccc\n                         |\n                         (here)\n", "file.yml:11: col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 8+1), "file.yml:12: col=1 (9B): ccc\n                          |\n                          (here)\n", "file.yml:12: col=1 (9B): error:\nerror:\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 8+2), "file.yml:13: col=2 (10B): ccc\n                            |\n                            (here)\n", "file.yml:13: col=2 (10B): error:\nerror:\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 8+3), "file.yml:14: col=3 (11B): ccc\n                             |\n                             (here)\n", "file.yml:14: col=3 (11B): error:\nerror:\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 10, npos, npos, 1), "file.yml:10: \nfile.yml:10: see region:\n", "file.yml:10: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", 11, npos, 8+0 , 1), "file.yml:11: (8B): ccc\nfile.yml:11: (8B): see region:\nbbb\nccc\n", "file.yml:11: (8B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", 12, npos, 8+1 , 1), "file.yml:12: (9B): ccc\nfile.yml:12: (9B): see region:\nbbb\nccc\n", "file.yml:12: (9B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", 13, npos, 8+2 , 1), "file.yml:13: (10B): ccc\nfile.yml:13: (10B): see region:\nbbb\nccc\n", "file.yml:13: (10B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", 14, npos, 8+3 , 1), "file.yml:14: (11B): ccc\nfile.yml:14: (11B): see region:\nbbb\nccc\n", "file.yml:14: (11B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", 10, 0   , npos, 1), "file.yml:10: col=0: \nfile.yml:10: col=0: see region:\n", "file.yml:10: col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", 11, 0   , 8+0 , 1), "file.yml:11: col=0 (8B): ccc\n                         |\n                         (here)\nfile.yml:11: col=0 (8B): see region:\nbbb\nccc\n|\n(here)\n", "file.yml:11: col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 8+1 , 1), "file.yml:12: col=1 (9B): ccc\n                          |\n                          (here)\nfile.yml:12: col=1 (9B): see region:\nbbb\nccc\n |\n (here)\n", "file.yml:12: col=1 (9B): error:\nerror:\nerror:     ccc\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 8+2 , 1), "file.yml:13: col=2 (10B): ccc\n                            |\n                            (here)\nfile.yml:13: col=2 (10B): see region:\nbbb\nccc\n  |\n  (here)\n", "file.yml:13: col=2 (10B): error:\nerror:\nerror:     ccc\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 8+3 , 1), "file.yml:14: col=3 (11B): ccc\n                             |\n                             (here)\nfile.yml:14: col=3 (11B): see region:\nbbb\nccc\n   |\n   (here)\n", "file.yml:14: col=3 (11B): error:\nerror:\nerror:     ccc\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 11, 0   , 8+0 , 1, 0, 0, 3), "file.yml:11: col=0 (8B): ccc\n                         ~~~  (cols 0-3/3)\n                         |\n                         (here)\nfile.yml:11: col=0 (8B): see region:\nbbb\nccc\n|\n(here)\n", "file.yml:11: col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     ~~~  (cols 0-3/3)\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 8+1 , 1, 0, 0, 3), "file.yml:12: col=1 (9B): ccc\n                         ~~~  (cols 0-3/3)\n                          |\n                          (here)\nfile.yml:12: col=1 (9B): see region:\nbbb\nccc\n |\n (here)\n", "file.yml:12: col=1 (9B): error:\nerror:\nerror:     ccc\nerror:     ~~~  (cols 0-3/3)\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 8+2 , 1, 0, 0, 3), "file.yml:13: col=2 (10B): ccc\n                          ~~~  (cols 0-3/3)\n                            |\n                            (here)\nfile.yml:13: col=2 (10B): see region:\nbbb\nccc\n  |\n  (here)\n", "file.yml:13: col=2 (10B): error:\nerror:\nerror:     ccc\nerror:     ~~~  (cols 0-3/3)\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 8+3 , 1, 0, 0, 3), "file.yml:14: col=3 (11B): ccc\n                          ~~~  (cols 0-3/3)\n                             |\n                             (here)\nfile.yml:14: col=3 (11B): see region:\nbbb\nccc\n   |\n   (here)\n", "file.yml:14: col=3 (11B): error:\nerror:\nerror:     ccc\nerror:     ~~~  (cols 0-3/3)\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     bbb\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 10, npos, npos, 2), "file.yml:10: \nfile.yml:10: see region:\n", "file.yml:10: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", 11, npos, 8+0 , 2), "file.yml:11: (8B): ccc\nfile.yml:11: (8B): see region:\naaa\nbbb\nccc\n", "file.yml:11: (8B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", 12, npos, 8+1 , 2), "file.yml:12: (9B): ccc\nfile.yml:12: (9B): see region:\naaa\nbbb\nccc\n", "file.yml:12: (9B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", 13, npos, 8+2 , 2), "file.yml:13: (10B): ccc\nfile.yml:13: (10B): see region:\naaa\nbbb\nccc\n", "file.yml:13: (10B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", 14, npos, 8+3 , 2), "file.yml:14: (11B): ccc\nfile.yml:14: (11B): see region:\naaa\nbbb\nccc\n", "file.yml:14: (11B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", 10, 0   , npos, 2), "file.yml:10: col=0: \nfile.yml:10: col=0: see region:\n", "file.yml:10: col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", 11, 0   , 8+0 , 2), "file.yml:11: col=0 (8B): ccc\n                         |\n                         (here)\nfile.yml:11: col=0 (8B): see region:\naaa\nbbb\nccc\n|\n(here)\n", "file.yml:11: col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 8+1 , 2), "file.yml:12: col=1 (9B): ccc\n                          |\n                          (here)\nfile.yml:12: col=1 (9B): see region:\naaa\nbbb\nccc\n |\n (here)\n", "file.yml:12: col=1 (9B): error:\nerror:\nerror:     ccc\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 8+2 , 2), "file.yml:13: col=2 (10B): ccc\n                            |\n                            (here)\nfile.yml:13: col=2 (10B): see region:\naaa\nbbb\nccc\n  |\n  (here)\n", "file.yml:13: col=2 (10B): error:\nerror:\nerror:     ccc\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 8+3 , 2), "file.yml:14: col=3 (11B): ccc\n                             |\n                             (here)\nfile.yml:14: col=3 (11B): see region:\naaa\nbbb\nccc\n   |\n   (here)\n", "file.yml:14: col=3 (11B): error:\nerror:\nerror:     ccc\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 11, 0   , 8+0 , 2, 0, 0, 3), "file.yml:11: col=0 (8B): ccc\n                         ~~~  (cols 0-3/3)\n                         |\n                         (here)\nfile.yml:11: col=0 (8B): see region:\naaa\nbbb\nccc\n|\n(here)\n", "file.yml:11: col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     ~~~  (cols 0-3/3)\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 8+1 , 2, 0, 0, 3), "file.yml:12: col=1 (9B): ccc\n                         ~~~  (cols 0-3/3)\n                          |\n                          (here)\nfile.yml:12: col=1 (9B): see region:\naaa\nbbb\nccc\n |\n (here)\n", "file.yml:12: col=1 (9B): error:\nerror:\nerror:     ccc\nerror:     ~~~  (cols 0-3/3)\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 8+2 , 2, 0, 0, 3), "file.yml:13: col=2 (10B): ccc\n                          ~~~  (cols 0-3/3)\n                            |\n                            (here)\nfile.yml:13: col=2 (10B): see region:\naaa\nbbb\nccc\n  |\n  (here)\n", "file.yml:13: col=2 (10B): error:\nerror:\nerror:     ccc\nerror:     ~~~  (cols 0-3/3)\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 8+3 , 2, 0, 0, 3), "file.yml:14: col=3 (11B): ccc\n                          ~~~  (cols 0-3/3)\n                             |\n                             (here)\nfile.yml:14: col=3 (11B): see region:\naaa\nbbb\nccc\n   |\n   (here)\n", "file.yml:14: col=3 (11B): error:\nerror:\nerror:     ccc\nerror:     ~~~  (cols 0-3/3)\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 10, npos, npos, 3), "file.yml:10: \nfile.yml:10: see region:\n", "file.yml:10: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", 11, npos, 8+0 , 3), "file.yml:11: (8B): ccc\nfile.yml:11: (8B): see region:\naaa\nbbb\nccc\n", "file.yml:11: (8B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", 12, npos, 8+1 , 3), "file.yml:12: (9B): ccc\nfile.yml:12: (9B): see region:\naaa\nbbb\nccc\n", "file.yml:12: (9B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", 13, npos, 8+2 , 3), "file.yml:13: (10B): ccc\nfile.yml:13: (10B): see region:\naaa\nbbb\nccc\n", "file.yml:13: (10B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", 14, npos, 8+3 , 3), "file.yml:14: (11B): ccc\nfile.yml:14: (11B): see region:\naaa\nbbb\nccc\n", "file.yml:14: (11B): error:\nerror:\nerror:     ccc\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\n");
    mkmsg((txt, "file.yml", 10, 0   , npos, 3), "file.yml:10: col=0: \nfile.yml:10: col=0: see region:\n", "file.yml:10: col=0: error:\nerror:\nerror:     \nerror:\nerror: see region:\nerror:\n");
    mkmsg((txt, "file.yml", 11, 0   , 8+0 , 3), "file.yml:11: col=0 (8B): ccc\n                         |\n                         (here)\nfile.yml:11: col=0 (8B): see region:\naaa\nbbb\nccc\n|\n(here)\n", "file.yml:11: col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 8+1 , 3), "file.yml:12: col=1 (9B): ccc\n                          |\n                          (here)\nfile.yml:12: col=1 (9B): see region:\naaa\nbbb\nccc\n |\n (here)\n", "file.yml:12: col=1 (9B): error:\nerror:\nerror:     ccc\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 8+2 , 3), "file.yml:13: col=2 (10B): ccc\n                            |\n                            (here)\nfile.yml:13: col=2 (10B): see region:\naaa\nbbb\nccc\n  |\n  (here)\n", "file.yml:13: col=2 (10B): error:\nerror:\nerror:     ccc\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 8+3 , 3), "file.yml:14: col=3 (11B): ccc\n                             |\n                             (here)\nfile.yml:14: col=3 (11B): see region:\naaa\nbbb\nccc\n   |\n   (here)\n", "file.yml:14: col=3 (11B): error:\nerror:\nerror:     ccc\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 11, 0   , 8+0 , 3, 0, 0, 3), "file.yml:11: col=0 (8B): ccc\n                         ~~~  (cols 0-3/3)\n                         |\n                         (here)\nfile.yml:11: col=0 (8B): see region:\naaa\nbbb\nccc\n|\n(here)\n", "file.yml:11: col=0 (8B): error:\nerror:\nerror:     ccc\nerror:     ~~~  (cols 0-3/3)\nerror:     |\nerror:     (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:     |\nerror:     (here)\n");
    mkmsg((txt, "file.yml", 12, 1   , 8+1 , 3, 0, 0, 3), "file.yml:12: col=1 (9B): ccc\n                         ~~~  (cols 0-3/3)\n                          |\n                          (here)\nfile.yml:12: col=1 (9B): see region:\naaa\nbbb\nccc\n |\n (here)\n", "file.yml:12: col=1 (9B): error:\nerror:\nerror:     ccc\nerror:     ~~~  (cols 0-3/3)\nerror:      |\nerror:      (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:      |\nerror:      (here)\n");
    mkmsg((txt, "file.yml", 13, 2   , 8+2 , 3, 0, 0, 3), "file.yml:13: col=2 (10B): ccc\n                          ~~~  (cols 0-3/3)\n                            |\n                            (here)\nfile.yml:13: col=2 (10B): see region:\naaa\nbbb\nccc\n  |\n  (here)\n", "file.yml:13: col=2 (10B): error:\nerror:\nerror:     ccc\nerror:     ~~~  (cols 0-3/3)\nerror:       |\nerror:       (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:       |\nerror:       (here)\n");
    mkmsg((txt, "file.yml", 14, 3   , 8+3 , 3, 0, 0, 3), "file.yml:14: col=3 (11B): ccc\n                          ~~~  (cols 0-3/3)\n                             |\n                             (here)\nfile.yml:14: col=3 (11B): see region:\naaa\nbbb\nccc\n   |\n   (here)\n", "file.yml:14: col=3 (11B): error:\nerror:\nerror:     ccc\nerror:     ~~~  (cols 0-3/3)\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     aaa\nerror:     bbb\nerror:     ccc\nerror:        |\nerror:        (here)\n");

    txt = R"(
0123456789112345678921234567893123456789412345678951234567896123456789
0a234567891a234567892a234567893a234567894a234567895a234567896a23456789
)";
    //                      line   offset
    //                      |  col |   linesbefore
    //                      |  |   |   |  linesafter
    //                      |  |   |   |  |  hlfirstcol
    //                      |  |   |   |  |  |  hllastcol
    //                      |  |   |   |  |  |  |  maxlen
    //                      |  |   |   |  |  |  |  |
    mkmsg((txt, "file.yml", 2,  3, 73, 0, 0, 0, 0, 20), "file.yml:2: col=3 (73B): 0a234567891a23456789[...] (showing columns 0-20/70)\n                            |\n                            (here)\n", "file.yml:2: col=3 (73B): error:\nerror:\nerror:     0a234567891a23456789[...] (showing columns 0-20/70)\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 2, 23, 93, 0, 0, 0, 0, 30), "file.yml:2: col=23 (93B): 0a234567891a234567892a23456789[...] (showing columns 0-30/70)\n                                                 |\n                                                 (here)\n", "file.yml:2: col=23 (93B): error:\nerror:\nerror:     0a234567891a234567892a23456789[...] (showing columns 0-30/70)\nerror:                            |\nerror:                            (here)\n");
    mkmsg((txt, "file.yml", 2, 23, 93, 0, 0, 0, 0, 20), "file.yml:2: col=23 (93B): [...]4567891a234567892a23[...] (showing columns 4-24/70)\n                                                  |\n                                                  (here)\n", "file.yml:2: col=23 (93B): error:\nerror:\nerror:     [...]4567891a234567892a23[...] (showing columns 4-24/70)\nerror:                             |\nerror:                             (here)\n");
    mkmsg((txt, "file.yml", 2, 23, 93, 0, 0, 0, 0, 15), "file.yml:2: col=23 (93B): [...]91a234567892a23[...] (showing columns 9-24/70)\n                                             |\n                                             (here)\n", "file.yml:2: col=23 (93B): error:\nerror:\nerror:     [...]91a234567892a23[...] (showing columns 9-24/70)\nerror:                        |\nerror:                        (here)\n");
    mkmsg((txt, "file.yml", 2, 23, 93, 0, 0, 0, 0, 10), "file.yml:2: col=23 (93B): [...]4567892a23[...] (showing columns 14-24/70)\n                                        |\n                                        (here)\n", "file.yml:2: col=23 (93B): error:\nerror:\nerror:     [...]4567892a23[...] (showing columns 14-24/70)\nerror:                   |\nerror:                   (here)\n");

    txt = R"(
..........++++++++++0123456789
..........0123456789
0123456789
0a234567891a234567892a234567893a234567894a234567895a234567896a23456789
)";
    //                      line   offset
    //                      |  col |   linesbefore
    //                      |  |   |   |  linesafter
    //                      |  |   |   |  |  hlfirstcol
    //                      |  |   |   |  |  |  hllastcol
    //                      |  |   |   |  |  |  |  maxlen
    //                      |  |   |   |  |  |  |  |
    mkmsg((txt, "file.yml", 4,  3, 67, 3, 0, 0, 0, 20), "file.yml:4: col=3 (67B): 0a234567891a23456789[...] (showing columns 0-20/70)\n                            |\n                            (here)\nfile.yml:4: col=3 (67B): see region:\n..........++++++++++[...] (showing columns 0-20/30)\n..........0123456789\n0123456789\n0a234567891a23456789[...] (showing columns 0-20/70)\n   |\n   (here)\n", "file.yml:4: col=3 (67B): error:\nerror:\nerror:     0a234567891a23456789[...] (showing columns 0-20/70)\nerror:        |\nerror:        (here)\nerror:\nerror: see region:\nerror:\nerror:     ..........++++++++++[...] (showing columns 0-20/30)\nerror:     ..........0123456789\nerror:     0123456789\nerror:     0a234567891a23456789[...] (showing columns 0-20/70)\nerror:        |\nerror:        (here)\n");
    mkmsg((txt, "file.yml", 4, 23, 87, 3, 0, 0, 0, 30), "file.yml:4: col=23 (87B): 0a234567891a234567892a23456789[...] (showing columns 0-30/70)\n                                                 |\n                                                 (here)\nfile.yml:4: col=23 (87B): see region:\n..........++++++++++0123456789\n..........0123456789\n0123456789\n0a234567891a234567892a23456789[...] (showing columns 0-30/70)\n                       |\n                       (here)\n", "file.yml:4: col=23 (87B): error:\nerror:\nerror:     0a234567891a234567892a23456789[...] (showing columns 0-30/70)\nerror:                            |\nerror:                            (here)\nerror:\nerror: see region:\nerror:\nerror:     ..........++++++++++0123456789\nerror:     ..........0123456789\nerror:     0123456789\nerror:     0a234567891a234567892a23456789[...] (showing columns 0-30/70)\nerror:                            |\nerror:                            (here)\n");
    mkmsg((txt, "file.yml", 4, 23, 87, 3, 0, 0, 0, 20), "file.yml:4: col=23 (87B): [...]4567891a234567892a23[...] (showing columns 4-24/70)\n                                                  |\n                                                  (here)\nfile.yml:4: col=23 (87B): see region:\n[...]......++++++++++0123[...] (showing columns 4-24/30)\n[...]......0123456789          (showing columns 4-20/20)\n[...]456789                    (showing columns 4-10/10)\n[...]4567891a234567892a23[...] (showing columns 4-24/70)\n                        |\n                        (here)\n", "file.yml:4: col=23 (87B): error:\nerror:\nerror:     [...]4567891a234567892a23[...] (showing columns 4-24/70)\nerror:                             |\nerror:                             (here)\nerror:\nerror: see region:\nerror:\nerror:     [...]......++++++++++0123[...] (showing columns 4-24/30)\nerror:     [...]......0123456789          (showing columns 4-20/20)\nerror:     [...]456789                    (showing columns 4-10/10)\nerror:     [...]4567891a234567892a23[...] (showing columns 4-24/70)\nerror:                             |\nerror:                             (here)\n");
    mkmsg((txt, "file.yml", 4, 23, 87, 3, 0, 0, 0, 17), "file.yml:4: col=23 (87B): [...]7891a234567892a23[...] (showing columns 7-24/70)\n                                               |\n                                               (here)\nfile.yml:4: col=23 (87B): see region:\n[...]...++++++++++0123[...] (showing columns 7-24/30)\n[...]...0123456789          (showing columns 7-20/20)\n[...]789                    (showing columns 7-10/10)\n[...]7891a234567892a23[...] (showing columns 7-24/70)\n                     |\n                     (here)\n", "file.yml:4: col=23 (87B): error:\nerror:\nerror:     [...]7891a234567892a23[...] (showing columns 7-24/70)\nerror:                          |\nerror:                          (here)\nerror:\nerror: see region:\nerror:\nerror:     [...]...++++++++++0123[...] (showing columns 7-24/30)\nerror:     [...]...0123456789          (showing columns 7-20/20)\nerror:     [...]789                    (showing columns 7-10/10)\nerror:     [...]7891a234567892a23[...] (showing columns 7-24/70)\nerror:                          |\nerror:                          (here)\n");
    mkmsg((txt, "file.yml", 4, 23, 87, 3, 0, 0, 0, 16), "file.yml:4: col=23 (87B): [...]891a234567892a23[...] (showing columns 8-24/70)\n                                              |\n                                              (here)\nfile.yml:4: col=23 (87B): see region:\n[...]..++++++++++0123[...] (showing columns 8-24/30)\n[...]..0123456789          (showing columns 8-20/20)\n[...]89                    (showing columns 8-10/10)\n[...]891a234567892a23[...] (showing columns 8-24/70)\n                    |\n                    (here)\n", "file.yml:4: col=23 (87B): error:\nerror:\nerror:     [...]891a234567892a23[...] (showing columns 8-24/70)\nerror:                         |\nerror:                         (here)\nerror:\nerror: see region:\nerror:\nerror:     [...]..++++++++++0123[...] (showing columns 8-24/30)\nerror:     [...]..0123456789          (showing columns 8-20/20)\nerror:     [...]89                    (showing columns 8-10/10)\nerror:     [...]891a234567892a23[...] (showing columns 8-24/70)\nerror:                         |\nerror:                         (here)\n");
    mkmsg((txt, "file.yml", 4, 23, 87, 3, 0, 0, 0, 15), "file.yml:4: col=23 (87B): [...]91a234567892a23[...] (showing columns 9-24/70)\n                                             |\n                                             (here)\nfile.yml:4: col=23 (87B): see region:\n[...].++++++++++0123[...] (showing columns 9-24/30)\n[...].0123456789          (showing columns 9-20/20)\n[...]9                    (showing columns 9-10/10)\n[...]91a234567892a23[...] (showing columns 9-24/70)\n                   |\n                   (here)\n", "file.yml:4: col=23 (87B): error:\nerror:\nerror:     [...]91a234567892a23[...] (showing columns 9-24/70)\nerror:                        |\nerror:                        (here)\nerror:\nerror: see region:\nerror:\nerror:     [...].++++++++++0123[...] (showing columns 9-24/30)\nerror:     [...].0123456789          (showing columns 9-20/20)\nerror:     [...]9                    (showing columns 9-10/10)\nerror:     [...]91a234567892a23[...] (showing columns 9-24/70)\nerror:                        |\nerror:                        (here)\n");
    mkmsg((txt, "file.yml", 4, 23, 87, 3, 0, 0, 0, 10), "file.yml:4: col=23 (87B): [...]4567892a23[...] (showing columns 14-24/70)\n                                        |\n                                        (here)\nfile.yml:4: col=23 (87B): see region:\n[...]++++++0123[...] (showing columns 14-24/30)\n[...]456789          (showing columns 14-20/20)\n[...]                (not showing, columns=10)\n[...]4567892a23[...] (showing columns 14-24/70)\n              |\n              (here)\n", "file.yml:4: col=23 (87B): error:\nerror:\nerror:     [...]4567892a23[...] (showing columns 14-24/70)\nerror:                   |\nerror:                   (here)\nerror:\nerror: see region:\nerror:\nerror:     [...]++++++0123[...] (showing columns 14-24/30)\nerror:     [...]456789          (showing columns 14-20/20)\nerror:     [...]                (not showing, columns=10)\nerror:     [...]4567892a23[...] (showing columns 14-24/70)\nerror:                   |\nerror:                   (here)\n");
    mkmsg((txt, "file.yml", 4, 23, 87, 3, 0, 0, 0,  5), "file.yml:4: col=23 (87B): [...]92a23[...] (showing columns 19-24/70)\n                                   |\n                                   (here)\nfile.yml:4: col=23 (87B): see region:\n[...]+0123[...] (showing columns 19-24/30)\n[...]9          (showing columns 19-20/20)\n[...]           (not showing, columns=10)\n[...]92a23[...] (showing columns 19-24/70)\n         |\n         (here)\n", "file.yml:4: col=23 (87B): error:\nerror:\nerror:     [...]92a23[...] (showing columns 19-24/70)\nerror:              |\nerror:              (here)\nerror:\nerror: see region:\nerror:\nerror:     [...]+0123[...] (showing columns 19-24/30)\nerror:     [...]9          (showing columns 19-20/20)\nerror:     [...]           (not showing, columns=10)\nerror:     [...]92a23[...] (showing columns 19-24/70)\nerror:              |\nerror:              (here)\n");
}

} // namespace yml
} // namespace c4

C4_SUPPRESS_WARNING_MSVC_POP
C4_SUPPRESS_WARNING_CLANG_POP
