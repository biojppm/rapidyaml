#ifndef RYML_SINGLE_HEADER
#include "c4/yml/std/std.hpp"
#include "c4/yml/parse.hpp"
#include "c4/yml/emit.hpp"
#include <c4/format.hpp>
#include <c4/yml/detail/checks.hpp>
#include <c4/yml/detail/print.hpp>
#include <c4/yml/detail/dbgprint.hpp>
#include <c4/yml/escape_scalar.hpp>
#include <c4/yml/extra/event_ints.hpp>
#endif
#include "./test_lib/test_case.hpp"
#include "./test_lib/test_helpers_ints.hpp"

#include <gtest/gtest.h>
#include "./test_lib/callbacks_tester.hpp"

RYML_DEFINE_TEST_MAIN()
#if defined(__clang__) && (__clang_major__ >= 13)
C4_SUPPRESS_WARNING_CLANG("-Wreserved-identifier")
#endif


C4_SUPPRESS_WARNING_GCC_CLANG_PUSH
C4_SUPPRESS_WARNING_MSVC_PUSH
C4_SUPPRESS_WARNING_GCC_CLANG("-Wdeprecated-declarations")
C4_SUPPRESS_WARNING_MSVC(4996) // deprecated
// NOLINTBEGIN(*-signed-bitwise)

namespace c4 {
namespace yml {

namespace {
struct scalar_style_spec
{
    csubstr scalar;
    NodeType style_flow;
    NodeType style_block;
    NodeType style_json;
    int elm;
    int line;
    int entry;
};
csubstr mknullstr() { csubstr s; s.str = nullptr; s.len = 0; return s; }
const csubstr zerolen("", size_t(0));

const NodeType P = SCALAR_PLAIN;
const NodeType S = SCALAR_SQUO;
const NodeType D = SCALAR_DQUO;
int elmcount = 0;
int entrycount = 0;
const scalar_style_spec scalars[] = {
    #define _(s, flow, block, json) scalar_style_spec{csubstr(s), flow, block, json, elmcount++, __LINE__, entrycount++}
    #define __(s, flow, block, json) scalar_style_spec{csubstr(s), flow, block, json, elmcount++, __LINE__, (entrycount = 0, entrycount++)}
    #define ___(s, flow, block, json) scalar_style_spec{s, flow, block, json, elmcount++, __LINE__, entrycount++}
    // special
    ___(mknullstr(), P, P, D),
    ___(zerolen, S, S, D),
    // regular scalar
    __("foo", P, P, D),
    __("foo", P, P, D),
    __("bar", P, P, D),
    __("foo space", P, P, D),
    __("bar space", P, P, D),
    // whitespace
    __(" ", S, S, D),
    __("\t", S, S, D),
    __("\n", S, S, D),
    // whitespace next to a newline would be folded away
    __("a\n b", D, D, D), _("a\n\tb", D, D, D), _("a\n\n b", D, D, D),
    __("a \nb", D, D, D), _("a\t\nb", D, D, D), _("a \n", D, D, D),
    __(">a\n b", D, D, D), _("- a\n b", D, D, D), _("#a \nb", D, D, D),
    // numbers
    __("0", P, P, P), _("00", P, P, D), _("000", P, P, D),
    __("1", P, P, P), _("01", P, P, D), _("001", P, P, D),
    __("10", P, P, P), _("010", P, P, D), _("0010", P, P, D),
    __("0.1", P, P, P), _("-0.1", P, P, P), _("+0.1", P, P, P),
    __(".1", P, P, P), _("-.1", P, P, P), _("+.1", P, P, P),
    __("01", P, P, D),  _("0x1", P, P, D), _("0o1", P, P, D), _("0b1", P, P, D), _("0x1", P, P, D), _("0o1", P, P, D), _("0b1", P, P, D),
    __("01", P, P, D),  _("0X1", P, P, D), _("0O1", P, P, D), _("0B1", P, P, D), _("0X1", P, P, D), _("0O1", P, P, D), _("0B1", P, P, D),
    __("-01", P, P, P), _("-0x1", P, P, P), _("-0o1", P, P, P), _("-0b1", P, P, P), _("-0x1", P, P, P), _("-0o1", P, P, P), _("-0b1", P, P, P),
    __("-01", P, P, P), _("-0X1", P, P, P), _("-0O1", P, P, P), _("-0B1", P, P, P), _("-0X1", P, P, P), _("-0O1", P, P, P), _("-0B1", P, P, P),
    __("+01", P, P, P), _("+0x1", P, P, P), _("+0o1", P, P, P), _("+0b1", P, P, P), _("+0x1", P, P, P), _("+0o1", P, P, P), _("+0b1", P, P, P),
    __("+01", P, P, P), _("+0X1", P, P, P), _("+0O1", P, P, P), _("+0B1", P, P, P), _("+0X1", P, P, P), _("+0O1", P, P, P), _("+0B1", P, P, P),
    __("2.35e-10", P, P, P),
    __("-2.35e-10", P, P, P),
    __("+2.35e-10", P, P, P),
    __("0.034", P, P, P),
    __("0.034", P, P, P),
    __("-0.034", P, P, P),
    __("-0.034", P, P, P),
    __("+0.034", P, P, P),
    __("+0.034", P, P, P),
    __(".034x", P, P, D),
    __("-.0123", P, P, P),
    __("+.0123", P, P, P),
    __("-.034x", P, P, D),
    __("+.034x", P, P, D),
    // inf
    __(".inf", P, P, P), _(".INF", P, P, P), _(".Inf", P, P, P),
    __("-.inf", P, P, P), _("-.INF", P, P, P), _("-.Inf", P, P, P),
    __("+.inf", P, P, P), _("+.INF", P, P, P), _("+.Inf", P, P, P),
    __(".infnot", P, P, D), _(".INFnot", P, P, D), _(".Infnot", P, P, D),
    __("-.infnot", P, P, D), _("-.INFnot", P, P, D), _("-.Infnot", P, P, D),
    __("+.infnot", P, P, D), _("+.INFnot", P, P, D), _("+.Infnot", P, P, D),
    __(".notinf", P, P, D), _(".NOTINF", P, P, D), _(".NotInf", P, P, D),
    __("-.notinf", P, P, D), _("-.NOTINF", P, P, D), _("-.NotInf", P, P, D),
    __("+.notinf", P, P, D), _("+.NOTINF", P, P, D), _("+.NotInf", P, P, D),
    __(".not", P, P, D), _(".NOT", P, P, D), _(".Not", P, P, D),
    __(".oot", P, P, D), _(".OOT", P, P, D), _(".Oot", P, P, D),
    // nan
    __(".nan", P, P, P), _(".NAN", P, P, P), _(".Nan", P, P, P), _(".NaN", P, P, P),
    __("-.nan", P, P, D), _("-.NAN", P, P, D), _("-.Nan", P, P, D), _("-.NaN", P, P, D),
    __("+.nan", P, P, D), _("+.NAN", P, P, D), _("+.Nan", P, P, D), _("+.NaN", P, P, D),
    __(".nannot", P, P, D), _(".NANnot", P, P, D), _(".Nannot", P, P, D),
    __(".notnan", P, P, D), _(".NOTNAN", P, P, D), _(".NotNan", P, P, D), _(".NoTNaN", P, P, D),
    __("-.notnan", P, P, D), _("-.NOTNAN", P, P, D), _("-.NotNan", P, P, D), _("-.NoTNaN", P, P, D),
    __("+.notnan", P, P, D), _("+.NOTNAN", P, P, D), _("+.NotNan", P, P, D), _("+.NoTNaN", P, P, D),
    // bool
    __("true", P, P, P),
    __("false", P, P, P),
    __("True", P, P, D),
    __("False", P, P, D),
    // dubious
    __("+.notinf", P, P, D), _("+.:", S, S, D),
    __("-.notinf", P, P, D), _("-:", S, S, D),
    __(":", S, S, D), _(": ", S, S, D), _(":\t", S, S, D), _(":\n", S, S, D), _(":\r\n", S, S, D),
    __("-", S, S, D), _("- ", S, S, D), _("-\t", S, S, D), _("-\n", S, S, D), _("-\r\n", S, S, D),
    __("?", S, S, D), _("? ", S, S, D), _("?\t", S, S, D), _("?\n", S, S, D), _("?\r\n", S, S, D),
    __(":a", P, P, D), _(":ab", P, P, D), _(":a b", P, P, D), _(":a\tb", P, P, D),
    __("-a", P, P, D), _("-ab", P, P, D), _("-a b", P, P, D), _("-a\tb", P, P, D),
    __("?a", P, P, D), _("?ab", P, P, D), _("?a b", P, P, D), _("?a\tb", P, P, D),
    __(":a,", S, P, D), _(":ab,", S, P, D), _(":a\tb,", S, P, D),
    __("-a,", S, P, D), _("-ab,", S, P, D), _("-a\tb,", S, P, D),
    __("?a,", S, P, D), _("?ab,", S, P, D), _("?a\tb,", S, P, D),
    __("b[c]d", S, P, D), _("b{c}d", S, P, D), _("b,c,d", S, P, D),
    __("(0,1)", S, P, D), _("-0,1", S, P, D),
    __("::", S, S, D), _(":: ", S, S, D), _("::\t", S, S, D),
    __("--", P, P, D), _("-- ", S, S, D), _("--\t", S, S, D),
    __("??", P, P, D), _("?? ", S, S, D), _("??\t", S, S, D),
    // tags
    __("!!str", S, S, D),
    __("!str", S, S, D),
    __("!<!foo>", S, S, D),
    __("<!foo>", P, P, D),
    __("<!foo> scalar", P, P, D),
    // anchors
    __("&notanchor", S, S, D),
    // invalid characters at beginning
    __("%invalidplain", S, S, D), _("but%validplain", P, P, D),
    __("`invalidplain", S, S, D), _("but`validplain", P, P, D),
    __("@invalidplain", S, S, D), _("but@validplain", P, P, D),
    __("#invalidplain", S, S, D), _("but#validplain", P, P, D),
    __(",invalidplain", S, S, D), _("but,validplain", S, P, D),
    __("- invalidplain", S, S, D), _("but- validplain",     P, P, D), _("and -validplain", P, P, D),
    /*                          */__("but-\nvalidplain",    P, P, D), _("and\n-validplain", P, P, D),
    __(": invalidplain", S, S, D), _("also: invalidplain",  S, S, D), _("while :validplain", P, P, D),
    /*                          */__("also:\ninvalidplain", S, S, D), _("while\n:validplain", P, P, D),
    __("? invalidplain", S, S, D), _("but? validplain",     P, P, D), _("also ?validplain", P, P, D),
    /*                          */__("but?\nvalidplain",    P, P, D), _("also\n?validplain", P, P, D),
    __("# invalidplain", S, S, D), _("but# validplain",     P, P, D), _("while #invalidplain", S, S, D),
    /*                          */__("but#\nvalidplain",    P, P, D), _("while\n#invalidplain", S, S, D),
};
std::string namefor(scalar_style_spec const& param)
{
    return c4::formatrs<std::string>("elm{}__line{}__entry{}",
                                     fmt::zpad(param.elm, 3),
                                     fmt::zpad(param.line, 3),
                                     param.entry);
}
template<class T>
std::string namefor(testing::TestParamInfo<T> const& pinfo)
{
    return namefor(pinfo.param);
}

struct TestScalarStyle : public testing::TestWithParam<scalar_style_spec> {};

INSTANTIATE_TEST_SUITE_P(_, TestScalarStyle, testing::ValuesIn(scalars), namefor<scalar_style_spec>);

struct showtype_ { NodeType ty; };
showtype_ showtype(NodeType ty) { return {ty}; }
size_t to_chars(substr buf, showtype_ s)
{
    size_t pos = s.ty.type_str(buf);
    buf = buf.sub(pos < buf.len ? pos : buf.len);
    pos += format(buf, "({})", s.ty.m_bits);
    return pos;
}

const csubstr plain_flow_invalid_at_beginning[] = {
    csubstr(" "), csubstr("\t"), csubstr("\r"), csubstr("\n"),
    csubstr("!"), csubstr("&"), csubstr("*"), csubstr(","),
    csubstr("\""), csubstr("'"), csubstr("{"), csubstr("}"),
};
const csubstr plain_flow_invalid_in_bulk[] = {
    csubstr(": "), csubstr(":\t"), csubstr(":\n"), csubstr(":\r"),
    csubstr(" #"), csubstr("\t#"), csubstr("\n#"), csubstr("\r#"),
    csubstr(","), csubstr("{"), csubstr("}"), csubstr("["), csubstr("]"),
};
const csubstr plain_flow_valid_in_bulk[] = {
    csubstr(" :"), csubstr("\t:"), csubstr("\n:"), csubstr("\r\n:"),
    csubstr("# "), csubstr("#\t"), csubstr("#\n"), csubstr("#\r"),
};
const csubstr plain_flow_invalid_at_end[] = {
    csubstr(" "), csubstr("\t"), csubstr("\r\n"), csubstr("\n"),
    csubstr(","), csubstr("{"), csubstr("}"), csubstr("["), csubstr("]"),
    csubstr("# "), csubstr("#\t"), csubstr("#"),
    csubstr(": "), csubstr(":\t"), csubstr(":"),
};

const csubstr plain_block_invalid_at_beginning[] = {
    csubstr(" "), csubstr("\t"), csubstr("\r"), csubstr("\n"),
    csubstr("!"), csubstr("&"), csubstr("*"), csubstr(","),
    csubstr("\""), csubstr("'"), csubstr("{"), csubstr("}"),
};
const csubstr plain_block_invalid_in_bulk[] = {
    csubstr(": "), csubstr(":\t"), csubstr(":\n"), csubstr(":\r"),
    csubstr(" #"), csubstr("\t#"), csubstr("\n#"), csubstr("\r#"),
};
const csubstr plain_block_valid_in_bulk[] = {
    csubstr(" :"), csubstr("\t:"), csubstr("\n:"), csubstr("\r:"),
    csubstr("# "), csubstr("#\t"), csubstr("#\n"), csubstr("#\r"),
};
const csubstr plain_block_invalid_at_end[] = {
    csubstr(" "), csubstr("\t"), csubstr("\r"), csubstr("\n"),
    csubstr("# "), csubstr("#\t"), csubstr("#"),
    csubstr(": "), csubstr(":\t"), csubstr(":"),
};


using ScalarBuildFn = bool (*)(std::string *buf, csubstr str, csubstr scalar, bool expect_valid);

bool make_case_at_beginning(std::string *buf, csubstr str, csubstr scalar, bool expect_valid)
{
    (void)expect_valid;
    catrs(buf, str, scalar);
    return true;
}

bool make_case_in_bulk(std::string *buf, csubstr str, csubstr scalar, bool expect_valid)
{
    (void)expect_valid;
    catrs(buf, scalar, str, scalar);
    return true;
}

bool make_case_at_end(std::string *buf, csubstr str, csubstr scalar, bool expect_valid)
{
    (void)expect_valid;
    catrs(buf, scalar, str);
    return true;
}


#define SHOWPARAM(p, fmt, ...)                                      \
    RYML_TRACE_FMT("test\n"                                         \
                   "{}:{}: scalar=[{}]~~~{}~~~\n"                   \
                   "{}:{}: " fmt,                                   \
                   to_csubstr(__FILE__), (p).line,                  \
                   (p).scalar.len, escaped_scalar((p).scalar),      \
                   to_csubstr(__FILE__), (p).line, __VA_ARGS__)

void test_plain_valid(scalar_style_spec const& p, bool flow, cspan<csubstr> cases, bool expected, ScalarBuildFn fn)
{
    if(p.style_flow != P || !p.scalar.len || p.scalar.first_not_of(" \n\t\r") == npos)
        return;
    SHOWPARAM(p, "expected_flow={}", showtype(p.style_flow.m_bits));
    std::string buf;
    size_t i = 0;
    for(csubstr c : cases)
    {
        if(!fn(&buf, c, p.scalar, expected))
            continue;
        csubstr str = to_csubstr(buf);
        bool ok = (expected == flow ? scalar_style_query_plain_flow(str)
                   : scalar_style_query_plain_block(str));
        ok = ok && (expected == scalar_style_query_plain(str, flow));
        if(ok)
            continue;
        RYML_TRACE_FMT("case[{}/{}]='{}' scalar='{}' str='{}'", i++, cases.size(), escaped_scalar(c), escaped_scalar(p.scalar), escaped_scalar(str));
        if(flow)
        {
            EXPECT_EQ(scalar_style_query_plain_flow(str), expected);
            EXPECT_EQ(scalar_style_query_plain_flow(str),
                      scalar_style_query_plain(str, flow));
        }
        else
        {
            EXPECT_EQ(scalar_style_query_plain_block(str), expected);
            EXPECT_EQ(scalar_style_query_plain_block(str),
                      scalar_style_query_plain(str, !flow));
        }
        break;
    }
}

substr erase(substr buf, char c)
{
    size_t len = 0;
    bool skip = false;
    for(size_t i = 0; i < buf.len; ++i)
    {
        if(buf.str[i] != c)
        {
            if(skip)
                buf.str[len++] = buf.str[i];
        }
        else
        {
            len = i;
            skip = true;
        }
    }
    buf.len = len;
    return buf;
}

void compare_scalar(csubstr expected, csubstr actual)
{
    if(expected != actual)
    {
        RYML_TRACE_FMT("actual  ={}", escaped_scalar(actual));
        RYML_TRACE_FMT("expected={}", escaped_scalar(expected));
        if(expected.find('\r') == csubstr::npos)
        {
            EXPECT_EQ(expected, actual);
        }
        else
        {
            std::string filtered_(expected.begin(), expected.end());
            csubstr filtered = erase(to_substr(filtered_), '\r');
            RYML_TRACE_FMT("filtered={}", escaped_scalar(filtered));
            EXPECT_EQ(filtered, actual);
        }
    }
}

using namespace extra;
ievt::evt_bits to_ievt(NodeType ty)
{
    if(ty == P || (ty & (KEY_PLAIN|VAL_PLAIN)))
        return ievt::PLAI;
    else if(ty == D || (ty & (KEY_DQUO|VAL_DQUO)))
        return ievt::DQUO;
    else if(ty == S || (ty & (KEY_SQUO|VAL_SQUO)))
        return ievt::SQUO;
    RYML_ERR_BASIC_("unknown type");
}

#ifndef SCOPED_CALL
#define SCOPED_CALL(fn, ...) do { SCOPED_TRACE(#fn); fn __VA_ARGS__; } while(0)
#endif
using pfn_ints_check = std::function<void(ievt::TestBuffers const&)>;
void test_ints_roundtrip(txtbuf const& emitted, pfn_ints_check const& check)
{
    SCOPED_TRACE("ints");
    txtbuf orig_buf = emitted;
    ievt::TestBuffers orig = {};
    parse_ints(orig_buf, &orig);
    SCOPED_CALL(orig.test_invariants, ());
    SCOPED_CALL(check, (orig));
    if(testing::Test::HasFailure())
    {
        printf("emitted=~~~\n%s\n~~~\n", catrs<txtbuf>(escaped_scalar(emitted)).c_str());
        orig.print();
        return;
    }
    const txtbuf roundtrip_buf = orig.emitrs_yaml<txtbuf>();
    txtbuf emitted_parsed = emitted;
    ievt::TestBuffers roundtrip;
    parse_ints(emitted_parsed, &roundtrip);
    SCOPED_CALL(roundtrip.test_invariants, ());
    SCOPED_CALL(check, (orig));
    SCOPED_CALL(roundtrip.test_compare, (orig));
    if(testing::Test::HasFailure())
    {
        printf("emitted=~~~\n%s\n~~~\n", catrs<txtbuf>(escaped_scalar(emitted)).c_str());
        orig.print();
        printf("roundtrip=~~~\n%s\n~~~\n", roundtrip_buf.c_str());
        roundtrip.print();
        return;
    }
}

void test_scalar_roundtrip_as_root(scalar_style_spec const& p, NodeType expected_style)
{
    SHOWPARAM(p, "roundtrip_as_root", 0);
    csubstr scalar = p.scalar;
    if(!scalar.str || !scalar.len || scalar.first_not_of(" \n\t\r") == npos)
        return;
    txtbuf emitted;
    {
        SCOPED_TRACE("tree");
        Tree orig;
        orig.rootref().set_val(scalar);
        emitted = emitrs_yaml<txtbuf>(orig);
        const Tree parsed = parse_in_arena(to_csubstr(emitted));
        ConstNodeRef r = parsed.rootref();
        bool fail = !r.has_val();
        if(!fail)
        {
            compare_scalar(scalar, r.val());
            EXPECT_TRUE(r.type().is_val_styled()); // style is set on the roundtrip
            NodeType actual_style = r.type() & expected_style;
            RYML_COMPARE_NODE_TYPE(expected_style, actual_style, ==, EQ);
        }
        if(fail || testing::Test::HasFailure())
        {
            print_tree("orig", orig);
            printf("emitted=~~~\n%s\n~~~\n", emitted.c_str());
            print_tree("roundtrip", parsed);
        }
        if(fail)
            FAIL() << "not a val: ~~~" << emitted << "~~~";
    }
    if(testing::Test::HasFailure())
        return;
    {
        SCOPED_TRACE("ints");
        SCOPED_CALL(test_ints_roundtrip, (emitted, [&](ievt::TestBuffers const& ints){
            ASSERT_EQ(ints.evts.len, 7);
            ASSERT_EQ(ints.evts.ptr[2] & ievt::SCLR, ievt::SCLR);
            SCOPED_CALL(compare_scalar, (scalar, ints.getstr(2)));
        }));
    }
}

void test_scalar_roundtrip_on_seq(scalar_style_spec const& p, NodeType seq_style, NodeType expected_style)
{
    csubstr scalar = p.scalar;
    if(!scalar.len)
        return;
    SHOWPARAM(p, "roundtrip_on_seq", 0);
    size_t num_vals = 4;
    txtbuf emitted;
    {
        SCOPED_TRACE("tree");
        Tree orig;
        NodeRef r = orig;
        r.set_seq(seq_style);
        for(size_t i = 0; i < num_vals; ++i)
        {
            r.append_child().set_val(scalar);
            EXPECT_FALSE(r.type().is_val_styled()); // no style is set
        }
        emitted = emitrs_yaml<txtbuf>(orig);
        const Tree roundtrip = parse_in_arena(to_csubstr(emitted));
        for(ConstNodeRef ch : roundtrip.rootref().children())
        {
            compare_scalar(scalar, ch.val());
            EXPECT_TRUE(ch.type().is_val_styled()); // style is set on the roundtrip
            NodeType actual_style = ch.type() & expected_style;
            RYML_COMPARE_NODE_TYPE(expected_style, actual_style, ==, EQ);
            RYML_COMPARE_NODE_TYPE(expected_style, ch.type().val_style() & expected_style, ==, EQ);
            RYML_COMPARE_NODE_TYPE(expected_style, roundtrip.type(ch.id()).val_style() & expected_style, ==, EQ);
            RYML_COMPARE_NODE_TYPE(expected_style, roundtrip.val_style(ch.id()) & expected_style, ==, EQ);
        }
        if(testing::Test::HasFailure())
        {
            print_tree("orig", orig);
            printf("emitted=~~~\n%s\n~~~\n", catrs<txtbuf>(escaped_scalar(to_csubstr(emitted))).c_str());
            print_tree("roundtrip", roundtrip);
        }
    }
    if(testing::Test::HasFailure())
        return;
    {
        SCOPED_TRACE("ints");
        auto check = [&](ievt::TestBuffers const& ints){
            ASSERT_EQ(ints.evts.len, 18);
            ASSERT_EQ(ints.evts.ptr[2] & ievt::BSEQ, ievt::BSEQ);
            using namespace ievt;
            for(evt_size i = 0; i < (evt_size)num_vals; ++i)
            {
                evt_size pos = 3 + i * 3;
                evt_bits evt = ints.evts.ptr[pos];
                evt_bits expected_bits = to_ievt(expected_style);
                RYML_TRACE_FMT("pos={} evt={}", pos, evt);
                ASSERT_EQ(evt & ievt::SCLR, ievt::SCLR);
                EXPECT_EQ(evt & ~(ievt::KEY_|ievt::VAL_|ievt::SCLR|ievt::PEX2), expected_bits);
                SCOPED_CALL(compare_scalar, (scalar, ints.getstr(pos)));
            }
        };
        SCOPED_CALL(test_ints_roundtrip, (emitted, check));
    }
}

void test_scalar_roundtrip_on_map(scalar_style_spec const& p,
                                  NodeType map_style,
                                  NodeType expected_style_key,
                                  NodeType expected_style_val)
{
    csubstr scalar = p.scalar;
    if(!scalar.str)
        return;
    SHOWPARAM(p, "roundtrip_on_map", 0);
    csubstr names[] = {"0", "1", "2", "3"};
    bool valid_as_key = (npos == p.scalar.find('\n'));
    txtbuf emitted;
    {
        Tree orig;
        NodeRef r = orig;
        r.set_map(map_style);
        NodeRef vals = r["vals"];
        NodeRef keys = r["keys"];
        vals.set_map(map_style);
        keys.set_map(map_style);
        for(csubstr name : names)
        {
            NodeRef k = keys.append_child();
            NodeRef v = vals.append_child();
            k.set_key(valid_as_key ? scalar : name);
            v.set_key(name);
            k.set_val(name);
            v.set_val(scalar);
            EXPECT_FALSE(k.type().is_key_styled()); // no style is set
            EXPECT_FALSE(v.type().is_val_styled()); // no style is set
        }
        emitted = emitrs_yaml<txtbuf>(orig);
        _c4dbgpf("scalar=~~~{}~~~", escaped_scalar(to_csubstr(scalar), true));
        _c4dbgpf("emitted=~~~\n{}\n~~~", escaped_scalar(to_csubstr(emitted), true));
        const Tree parsed = parse_in_arena(to_csubstr(emitted));
        if(valid_as_key)
        {
            for(ConstNodeRef ch : parsed["keys"].children())
            {
                compare_scalar(scalar, ch.key());
                EXPECT_TRUE(ch.type().is_key_styled()); // style is set on the roundtrip
                const NodeType actual_style = ch.type() & expected_style_key;
                RYML_COMPARE_NODE_TYPE(expected_style_key, actual_style, ==, EQ);
                RYML_COMPARE_NODE_TYPE(expected_style_key, ch.type().key_style() & expected_style_key, ==, EQ);
                RYML_COMPARE_NODE_TYPE(expected_style_key, parsed.type(ch.id()).key_style() & expected_style_key, ==, EQ);
                RYML_COMPARE_NODE_TYPE(expected_style_key, parsed.key_style(ch.id()) & expected_style_key, ==, EQ);
            }
        }
        for(ConstNodeRef ch : parsed["vals"].children())
        {
            compare_scalar(scalar, ch.val());
            EXPECT_TRUE(ch.type().is_val_styled()); // style is set on the roundtrip
            const NodeType actual_style = ch.type() & expected_style_val;
            RYML_COMPARE_NODE_TYPE(expected_style_val, actual_style, ==, EQ);
            RYML_COMPARE_NODE_TYPE(expected_style_val, ch.type().val_style() & expected_style_val, ==, EQ);
            RYML_COMPARE_NODE_TYPE(expected_style_val, parsed.type(ch.id()).val_style() & expected_style_val, ==, EQ);
            RYML_COMPARE_NODE_TYPE(expected_style_val, parsed.val_style(ch.id()) & expected_style_val, ==, EQ);
        }
        if(testing::Test::HasFailure())
        {
            printf("scalar=~~~\n%s\n~~~\n", catrs<txtbuf>(escaped_scalar(scalar)).c_str());
            print_tree("orig", orig);
            printf("emitted=~~~\n%s\n~~~\n", catrs<txtbuf>(escaped_scalar(to_csubstr(emitted))).c_str());
            print_tree("roundtrip", parsed);
        }
    }
    if(testing::Test::HasFailure())
        return;
    {
        SCOPED_TRACE("ints");
        using namespace ievt;
        auto check_scalars_on_map = [&](ievt::TestBuffers const& ints,
                                        evt_size pos_map,
                                        evt_bits key_or_val,
                                        NodeType expected_style){
            RYML_TRACE_FMT("pos_map={}", pos_map);
            ASSERT_EQ(ints.evts.ptr[pos_map] & ievt::BMAP, ievt::BMAP);
            bool toval = key_or_val & ievt::VAL_;
            bool use_scalar = valid_as_key || toval;
            evt_size pos = pos_map + 1 + (toval ? 3 : 0);
            for(csubstr name : names)
            {
                evt_bits evt = ints.evts.ptr[pos];
                evt_bits expected_bits = to_ievt(expected_style);
                csubstr expected_scalar = use_scalar ? scalar : name;
                RYML_TRACE_FMT("pos={} evt={} name={} expected={}", pos, evt, name, expected_scalar);
                ASSERT_EQ(evt & (key_or_val|ievt::SCLR), key_or_val|ievt::SCLR);
                if(use_scalar)
                {
                    EXPECT_EQ(evt & ~(key_or_val|ievt::SCLR|ievt::PEX2), expected_bits);
                }
                else
                {
                    EXPECT_EQ(evt & ~(key_or_val|ievt::SCLR|ievt::PEX2), ievt::PLAI);
                }
                SCOPED_CALL(compare_scalar, (expected_scalar, ints.getstr(pos)));
                pos += 6;
            }
        };
        auto check = [&](ievt::TestBuffers const& ints){
            ASSERT_EQ(ints.evts.len, 64);
            SCOPED_CALL(check_scalars_on_map, (ints, 6, ievt::VAL_, expected_style_val));
            SCOPED_CALL(check_scalars_on_map, (ints, 35, ievt::KEY_, expected_style_key));
        };
        SCOPED_CALL(test_ints_roundtrip, (emitted, check));
    }
}

bool flow = true;
bool block = false;
} // namespace


//-----------------------------------------------------------------------------

TEST_P(TestScalarStyle, query_plain_flow)
{
    scalar_style_spec const& p = GetParam();
    SHOWPARAM(p, "expected_flow={}", showtype(p.style_flow.m_bits));
    EXPECT_EQ(scalar_style_query_plain_flow(p.scalar), p.style_flow == P);
    EXPECT_EQ(scalar_style_query_plain_flow(p.scalar),
              scalar_style_query_plain(p.scalar, flow));
}

TEST_P(TestScalarStyle, query_plain_flow_invalid_at_beginning)
{
    test_plain_valid(GetParam(), flow, plain_flow_invalid_at_beginning, false,
                     make_case_at_beginning);
}

TEST_P(TestScalarStyle, query_plain_flow_invalid_in_bulk)
{
    test_plain_valid(GetParam(), flow, plain_flow_invalid_in_bulk, false,
                     make_case_in_bulk);
}

TEST_P(TestScalarStyle, query_plain_flow_valid_in_bulk)
{
    test_plain_valid(GetParam(), flow, plain_flow_valid_in_bulk, true,
                     make_case_in_bulk);
}

TEST_P(TestScalarStyle, query_plain_flow_invalid_at_end)
{
    test_plain_valid(GetParam(), flow, plain_flow_invalid_at_end, false,
                     make_case_at_end);
}


//-----------------------------------------------------------------------------
// block

TEST_P(TestScalarStyle, query_plain_block)
{
    scalar_style_spec const& p = GetParam();
    SHOWPARAM(p, "expected_block={}", showtype(p.style_block.m_bits));
    EXPECT_EQ(scalar_style_query_plain_block(p.scalar), p.style_block == P);
    EXPECT_EQ(scalar_style_query_plain_block(p.scalar),
              scalar_style_query_plain(p.scalar, block));
}

TEST_P(TestScalarStyle, query_plain_block_invalid_at_beginning)
{
    test_plain_valid(GetParam(), block, plain_block_invalid_at_beginning, false,
                     make_case_at_beginning);
}

TEST_P(TestScalarStyle, query_plain_block_invalid_in_bulk)
{
    test_plain_valid(GetParam(), block, plain_block_invalid_in_bulk, false,
                     make_case_in_bulk);
}

TEST_P(TestScalarStyle, query_plain_block_valid_in_bulk)
{
    test_plain_valid(GetParam(), block, plain_block_valid_in_bulk, true,
                     make_case_in_bulk);
}

TEST_P(TestScalarStyle, query_plain_block_invalid_at_end)
{
    test_plain_valid(GetParam(), block, plain_block_invalid_at_end, false,
                     make_case_at_end);
}


//-----------------------------------------------------------------------------
// choose

TEST_P(TestScalarStyle, choose_flow)
{
    scalar_style_spec const& p = GetParam();
    NodeType actual = scalar_style_choose_flow(p.scalar);
    NodeType expected = p.style_flow.m_bits;
    SHOWPARAM(p, "\n  actual  ={}\n  expected={}", showtype(actual), showtype(expected));
    RYML_COMPARE_NODE_TYPE(actual, expected, ==, EQ);
    EXPECT_EQ(extra::ievt::detail::scalar_style_choose_flow_ievt(p.scalar),
              to_ievt(p.style_flow));
}

TEST_P(TestScalarStyle, choose_block)
{
    scalar_style_spec const& p = GetParam();
    NodeType actual = scalar_style_choose_block(p.scalar);
    NodeType expected = p.style_block.m_bits;
    SHOWPARAM(p, "\n  actual  ={}\n  expected={}", showtype(actual), showtype(expected));
    RYML_COMPARE_NODE_TYPE(actual, expected, ==, EQ);
    EXPECT_EQ(extra::ievt::detail::scalar_style_choose_block_ievt(p.scalar),
              to_ievt(p.style_block));
}

TEST_P(TestScalarStyle, choose_json)
{
    scalar_style_spec const& p = GetParam();
    NodeType actual = scalar_style_json_choose(p.scalar);
    NodeType expected = p.style_json.m_bits;
    SHOWPARAM(p, "\n  actual  ={}\n  expected={}", showtype(actual), showtype(expected));
    RYML_COMPARE_NODE_TYPE(actual, expected, ==, EQ);
    EXPECT_EQ(extra::ievt::detail::scalar_style_choose_json_ievt(p.scalar),
              to_ievt(p.style_json));
}


//-----------------------------------------------------------------------------

TEST_P(TestScalarStyle, roundtrip_as_root)
{
    scalar_style_spec const& p = GetParam();
    test_scalar_roundtrip_as_root(p, p.style_block & VAL_STYLE);
}

TEST_P(TestScalarStyle, roundtrip_seq_flow_sl)
{
    scalar_style_spec const& p = GetParam();
    test_scalar_roundtrip_on_seq(p, FLOW_SL, p.style_flow & VAL_STYLE);
}

TEST_P(TestScalarStyle, roundtrip_seq_flow_ml)
{
    scalar_style_spec const& p = GetParam();
    test_scalar_roundtrip_on_seq(p, FLOW_ML, p.style_flow & VAL_STYLE);
}

TEST_P(TestScalarStyle, roundtrip_seq_block)
{
    scalar_style_spec const& p = GetParam();
    test_scalar_roundtrip_on_seq(p, BLOCK, p.style_block & VAL_STYLE);
}

TEST_P(TestScalarStyle, roundtrip_map_flow_sl)
{
    scalar_style_spec const& p = GetParam();
    test_scalar_roundtrip_on_map(p, FLOW_SL,
                                 p.style_flow & KEY_STYLE,
                                 p.style_flow & VAL_STYLE);
}

TEST_P(TestScalarStyle, roundtrip_map_flow_ml)
{
    scalar_style_spec const& p = GetParam();
    test_scalar_roundtrip_on_map(p, FLOW_ML,
                                 p.style_flow & KEY_STYLE,
                                 p.style_flow & VAL_STYLE);
}

TEST_P(TestScalarStyle, roundtrip_map_block)
{
    scalar_style_spec const& p = GetParam();
    test_scalar_roundtrip_on_map(p, BLOCK,
                                 p.style_block & KEY_STYLE,
                                 p.style_block & VAL_STYLE);
}

// NOLINTEND(*-signed-bitwise)
C4_SUPPRESS_WARNING_POP

} // namespace yml
} // namespace c4
