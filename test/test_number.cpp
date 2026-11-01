#include "./test_lib/test_group.hpp"
#include "./test_lib/test_group.def.hpp"
#ifdef RYML_SINGLE_HEADER
#include <ryml_all.hpp>
#else
#include <c4/charconv.hpp>
#endif

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

TEST(asdasd,  asasldkjas)
{
    csubstr sstr = "ptr";
    /* */ char mbuf[] = "ptr";
    const char cbuf[] = "ptr";
    EXPECT_EQ(sstr, mbuf);
    EXPECT_EQ(sstr, cbuf);
    EXPECT_EQ(mbuf, sstr);
    EXPECT_EQ(cbuf, sstr);
    EXPECT_EQ(sstr, (const char*)mbuf);
    EXPECT_EQ(sstr, (const char*)cbuf);
    EXPECT_EQ((const char*)mbuf, sstr);
    EXPECT_EQ((const char*)cbuf, sstr);
    EXPECT_EQ(sstr, (char*)mbuf);
    EXPECT_EQ((char*)mbuf, sstr);
    EXPECT_EQ(sstr, "ptr");
    EXPECT_EQ("ptr", sstr);
}

template<class I>
auto mkvals() -> typename std::enable_if<std::is_signed<I>::value, std::vector<I>>::type
{
    return std::vector<I>({std::numeric_limits<I>::min(), I(-10), I(-5), I(-1), I(0), I(1), I(5), I(10), std::numeric_limits<I>::max(),});
}
template<class I>
auto mkvals() -> typename std::enable_if<!std::is_signed<I>::value, std::vector<I>>::type
{
    return std::vector<I>({I(0), I(1), I(5), I(10), std::numeric_limits<I>::max(),});
}
template<class I>
C4_NO_UBSAN_IOVRFLW
void test_ints()
{
    C4_SUPPRESS_WARNING_GCC_WITH_PUSH("-Wuseless-cast")
    std::vector<I> values = mkvals<I>();
    Tree t = parse_in_arena("{dec: [], hex: [], bin: [], oct: [], versions: ['0.1', 0.1.2, 0.1.2.3, 0.1.2.3.4]}");
    NodeRef r = t.rootref();
    auto notval = [](I val) { return val <= 0 ? (I)(val + I(1)) : (I)(val - I(1)); }; // try not to overflow
    for(I val : values)
    {
        I out;
        r["dec"].append_child().save(val);
        r["hex"].append_child().save(fmt::hex(val));
        r["bin"].append_child().save(fmt::bin(val));
        r["oct"].append_child().save(fmt::oct(val));
        out = notval(val);
        r["dec"].last_child().load(&out);
        EXPECT_EQ(out, val);
        out = notval(val);
        r["hex"].last_child().load(&out);
        EXPECT_EQ(out, val);
        out = notval(val);
        r["bin"].last_child().load(&out);
        EXPECT_EQ(out, val);
        out = notval(val);
        r["oct"].last_child().load(&out);
        EXPECT_EQ(out, val);
    }
    {
        std::string emitted = emitrs_yaml<std::string>(t);
        Tree parsed = parse_in_place(to_substr(emitted));
        ASSERT_EQ(parsed["dec"].num_children(), values.size());
        ASSERT_EQ(parsed["hex"].num_children(), values.size());
        ASSERT_EQ(parsed["bin"].num_children(), values.size());
        ASSERT_EQ(parsed["oct"].num_children(), values.size());
        ASSERT_EQ(parsed["versions"].num_children(), 4u);
        id_type pos = 0;
        for(I val : values)
        {
            I out = notval(val);
            parsed["dec"][pos].load(&out);
            EXPECT_EQ(out, val);
            out = notval(val);
            parsed["hex"][pos].load(&out);
            EXPECT_EQ(out, val);
            out = notval(val);
            parsed["bin"][pos].load(&out);
            EXPECT_EQ(out, val);
            out = notval(val);
            parsed["oct"][pos].load(&out);
            EXPECT_EQ(out, val);
            ++pos;
        }
        EXPECT_EQ(parsed["versions"][0], "0.1");
        EXPECT_EQ(parsed["versions"][1], "0.1.2");
        EXPECT_EQ(parsed["versions"][2], "0.1.2.3");
        EXPECT_EQ(parsed["versions"][3], "0.1.2.3.4");
    }
    {
        std::string emitted = emitrs_json<std::string>(t);
        Tree parsed = parse_in_place(to_substr(emitted));
        ASSERT_EQ(parsed["dec"].num_children(), values.size());
        ASSERT_EQ(parsed["hex"].num_children(), values.size());
        ASSERT_EQ(parsed["bin"].num_children(), values.size());
        ASSERT_EQ(parsed["oct"].num_children(), values.size());
        ASSERT_EQ(parsed["versions"].num_children(), 4u);
        id_type pos = 0;
        for(I val : values)
        {
            I out = notval(val);
            parsed["dec"][pos].load(&out);
            EXPECT_EQ(out, val);
            out = notval(val);
            parsed["hex"][pos].load(&out);
            EXPECT_EQ(out, val);
            out = notval(val);
            parsed["bin"][pos].load(&out);
            EXPECT_EQ(out, val);
            out = notval(val);
            parsed["oct"][pos].load(&out);
            EXPECT_EQ(out, val);
            ++pos;
        }
        EXPECT_EQ(parsed["versions"][0], "0.1");
        EXPECT_EQ(parsed["versions"][1], "0.1.2");
        EXPECT_EQ(parsed["versions"][2], "0.1.2.3");
        EXPECT_EQ(parsed["versions"][3], "0.1.2.3.4");
    }
    C4_SUPPRESS_WARNING_GCC_POP
}

TEST(number, idec)
{
    test_ints<int8_t>();
    test_ints<int16_t>();
    test_ints<int32_t>();
    test_ints<int64_t>();
    test_ints<uint8_t>();
    test_ints<uint16_t>();
    test_ints<uint32_t>();
    test_ints<uint64_t>();
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

template<class T>
static bool fleq(T a, T b)
{
    return 0 == memcmp(&a, &b, sizeof(T));
}

TEST(number, nan_0)
{
    Tree t;
    t.rootref().set_seq();
    t[0].save(std::numeric_limits<float>::quiet_NaN());
    t[1].save(std::numeric_limits<double>::quiet_NaN());
    EXPECT_EQ(t[0].val(), ".nan");
    EXPECT_EQ(t[1].val(), ".nan");
    EXPECT_EQ(emitrs_yaml<std::string>(t),
              R"(- .nan
- .nan
)");
    EXPECT_EQ(emitrs_json<std::string>(t),
              R"([".nan",".nan"])");
}

TEST(number, nan_1)
{
    csubstr yaml = R"(
good:
 .nan: .nan
 .nan:   .nan
 .NaN: .NaN
 .NAN: .NAN
 nan: nan
 .nan: .nan
set:
  nothing0: nothing
  nothing1: nothing
)";
    test_check_emit_check(yaml, [](Tree const& t){
        EXPECT_EQ(t["good"][0].val(), ".nan");
        EXPECT_EQ(t["good"][1].val(), ".nan");
        EXPECT_EQ(t["good"][2].val(), ".NaN");
        EXPECT_EQ(t["good"][3].val(), ".NAN");
        EXPECT_EQ(t["good"][4].val(), "nan");
        EXPECT_EQ(t["good"][5].val(), ".nan");
        for(ConstNodeRef ch : t["good"]){
            SCOPED_TRACE(ch.key());
            {
                float f = 0.f;
                double d = 0.;
                EXPECT_FALSE(std::isnan(f));
                EXPECT_FALSE(std::isnan(d));
                ch.load(&f);
                ch.load(&d);
                EXPECT_TRUE(std::isnan(f));
                EXPECT_TRUE(std::isnan(d));
            }
            {
                float f = 0.f;
                double d = 0.;
                EXPECT_FALSE(std::isnan(f));
                EXPECT_FALSE(std::isnan(d));
                ch.load_key(&f);
                ch.load_key(&d);
                EXPECT_TRUE(std::isnan(f));
                EXPECT_TRUE(std::isnan(d));
            }
        }
        for(ConstNodeRef ch : t["set"]){
            SCOPED_TRACE(ch.key());
            float f = 0.f;
            double d = 0.;
            ExpectError::check_error_visit(&t, [&]{ ch.load(&f); });
            ExpectError::check_error_visit(&t, [&]{ ch.load(&d); });
            ExpectError::check_error_visit(&t, [&]{ ch.load_key(&f); });
            ExpectError::check_error_visit(&t, [&]{ ch.load_key(&d); });
        }
        EXPECT_EQ(emitrs_yaml<std::string>(t),
                  R"(good:
  .nan: .nan
  .nan: .nan
  .NaN: .NaN
  .NAN: .NAN
  nan: nan
  .nan: .nan
set:
  nothing0: nothing
  nothing1: nothing
)");
        EXPECT_EQ(emitrs_json<std::string>(t),
                  R"({
  "good": {
    ".nan": ".nan",
    ".nan": ".nan",
    ".nan": ".nan",
    ".nan": ".nan",
    ".nan": ".nan",
    ".nan": ".nan"
  },
  "set": {
    "nothing0": "nothing",
    "nothing1": "nothing"
  }
}
)");
    });
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(number, inf_0)
{
    Tree t;
    t.rootref().set_seq();
    const float finf = std::numeric_limits<float>::infinity();
    const double dinf = std::numeric_limits<double>::infinity();
    t[0].save(finf);
    t[1].save(dinf);
    t[2].save(-finf);
    t[3].save(-dinf);
    EXPECT_EQ(t[0].val(), ".inf");
    EXPECT_EQ(t[1].val(), ".inf");
    EXPECT_EQ(t[2].val(), "-.inf");
    EXPECT_EQ(t[3].val(), "-.inf");
    EXPECT_EQ(scalar_style_choose_flow("-.inf"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose_block("-.inf"), SCALAR_PLAIN);
    EXPECT_EQ(emitrs_yaml<std::string>(t),
              R"(- .inf
- .inf
- -.inf
- -.inf
)");
    EXPECT_TRUE(scalar_is_special_json(".inf"));
    EXPECT_TRUE(scalar_is_special_json("-.inf"));
    EXPECT_TRUE(scalar_is_special_json("+.inf"));
    EXPECT_EQ(emitrs_json<std::string>(t),
              R"([".inf",".inf","-.inf","-.inf"])");
}

TEST(number, inf_1)
{
    csubstr yaml = R"(
good:
  .inf: .inf
  .inf:   .inf
  .Inf: .Inf
  .INF: .INF
  inf: inf
  infinity: infinity
  .inf:
   .inf
set:
  nothing0: nothing
  nothing1: nothing
)";
    test_check_emit_check(yaml, [](Tree const& t){
        float finf = std::numeric_limits<float>::infinity();
        double dinf = std::numeric_limits<double>::infinity();
        EXPECT_EQ(t["good"][0].key(), ".inf");
        EXPECT_EQ(t["good"][0].val(), ".inf");
        EXPECT_EQ(t["good"][1].key(), ".inf");
        EXPECT_EQ(t["good"][1].val(), ".inf");
        EXPECT_EQ(t["good"][2].key(), ".Inf");
        EXPECT_EQ(t["good"][2].val(), ".Inf");
        EXPECT_EQ(t["good"][3].key(), ".INF");
        EXPECT_EQ(t["good"][3].val(), ".INF");
        EXPECT_EQ(t["good"][4].key(), "inf");
        EXPECT_EQ(t["good"][4].val(), "inf");
        EXPECT_EQ(t["good"][5].key(), "infinity");
        EXPECT_EQ(t["good"][5].val(), "infinity");
        EXPECT_EQ(t["good"][6].key(), ".inf");
        EXPECT_EQ(t["good"][6].val(), ".inf");
        for(ConstNodeRef ch : t["good"]){
            SCOPED_TRACE(ch.key());
            {
                float f = 0.f;
                double d = 0.;
                EXPECT_FALSE(fleq(f, finf));
                EXPECT_FALSE(fleq(d, dinf));
                EXPECT_FALSE(std::isinf(f));
                EXPECT_FALSE(std::isinf(d));
                ch.load(&f);
                ch.load(&d);
                EXPECT_TRUE(fleq(f, finf));
                EXPECT_TRUE(fleq(d, dinf));
                EXPECT_TRUE(std::isinf(f));
                EXPECT_TRUE(std::isinf(d));
            }
            {
                float f = 0.f;
                double d = 0.;
                EXPECT_FALSE(fleq(f, finf));
                EXPECT_FALSE(fleq(d, dinf));
                EXPECT_FALSE(std::isinf(f));
                EXPECT_FALSE(std::isinf(d));
                ch.load_key(&f);
                ch.load_key(&d);
                EXPECT_TRUE(fleq(f, finf));
                EXPECT_TRUE(fleq(d, dinf));
                EXPECT_TRUE(std::isinf(f));
                EXPECT_TRUE(std::isinf(d));
            }
        }
        for(ConstNodeRef ch : t["set"]){
            SCOPED_TRACE(ch.key());
            float f = 0.f;
            double d = 0.;
            ExpectError::check_error_visit(&t, [&]{ ch.load(&f); });
            ExpectError::check_error_visit(&t, [&]{ ch.load(&d); });
            ExpectError::check_error_visit(&t, [&]{ ch.load_key(&f); });
            ExpectError::check_error_visit(&t, [&]{ ch.load_key(&d); });
        }
        EXPECT_EQ(emitrs_yaml<std::string>(t),
                  R"(good:
  .inf: .inf
  .inf: .inf
  .Inf: .Inf
  .INF: .INF
  inf: inf
  infinity: infinity
  .inf: .inf
set:
  nothing0: nothing
  nothing1: nothing
)");
        EXPECT_EQ(emitrs_json<std::string>(t),
                  R"({
  "good": {
    ".inf": ".inf",
    ".inf": ".inf",
    ".inf": ".inf",
    ".inf": ".inf",
    ".inf": ".inf",
    ".inf": ".inf",
    ".inf": ".inf"
  },
  "set": {
    "nothing0": "nothing",
    "nothing1": "nothing"
  }
}
)");
    });
}

TEST(number, inf_2)
{
    csubstr yaml = R"(
good:
  -.inf: -.inf
  -.inf:   -.inf
  -.Inf: -.Inf
  -.INF: -.INF
  -inf: -inf
  -infinity: -infinity
  -.inf:
    -.inf
set:
  nothing0: nothing
  nothing1: nothing
)";
    test_check_emit_check(yaml, [](Tree const& t){
        float finf = std::numeric_limits<float>::infinity();
        double dinf = std::numeric_limits<double>::infinity();
        EXPECT_EQ(t["good"][0].val(), "-.inf");
        EXPECT_EQ(t["good"][1].val(), "-.inf");
        EXPECT_EQ(t["good"][2].val(), "-.Inf");
        EXPECT_EQ(t["good"][3].val(), "-.INF");
        EXPECT_EQ(t["good"][4].val(), "-inf");
        EXPECT_EQ(t["good"][5].val(), "-infinity");
        EXPECT_EQ(t["good"][6].val(), "-.inf");
        EXPECT_EQ(t["good"][0].key(), "-.inf");
        EXPECT_EQ(t["good"][1].key(), "-.inf");
        EXPECT_EQ(t["good"][2].key(), "-.Inf");
        EXPECT_EQ(t["good"][3].key(), "-.INF");
        EXPECT_EQ(t["good"][4].key(), "-inf");
        EXPECT_EQ(t["good"][5].key(), "-infinity");
        EXPECT_EQ(t["good"][6].key(), "-.inf");
        for(ConstNodeRef ch : t["good"]){
            SCOPED_TRACE(ch.key());
            {
                float f = 0.f;
                double d = 0.;
                ch.load(&f);
                ch.load(&d);
                EXPECT_TRUE(fleq(f, -finf));
                EXPECT_TRUE(fleq(d, -dinf));
            }
            {
                float f = 0.f;
                double d = 0.;
                ch.load_key(&f);
                ch.load_key(&d);
                EXPECT_TRUE(fleq(f, -finf));
                EXPECT_TRUE(fleq(d, -dinf));
            }
        }
        for(ConstNodeRef ch : t["set"]){
            SCOPED_TRACE(ch.key());
            float f = 0.f;
            double d = 0.;
            ExpectError::check_error_visit(&t, [&]{ ch.load(&f); });
            ExpectError::check_error_visit(&t, [&]{ ch.load(&d); });
            ExpectError::check_error_visit(&t, [&]{ ch.load_key(&f); });
            ExpectError::check_error_visit(&t, [&]{ ch.load_key(&d); });
        }
        EXPECT_EQ(emitrs_yaml<std::string>(t),
                  R"(good:
  -.inf: -.inf
  -.inf: -.inf
  -.Inf: -.Inf
  -.INF: -.INF
  -inf: -inf
  -infinity: -infinity
  -.inf: -.inf
set:
  nothing0: nothing
  nothing1: nothing
)");
        EXPECT_EQ(emitrs_json<std::string>(t),
                  R"({
  "good": {
    "-.inf": "-.inf",
    "-.inf": "-.inf",
    "-.inf": "-.inf",
    "-.inf": "-.inf",
    "-.inf": "-.inf",
    "-.inf": "-.inf",
    "-.inf": "-.inf"
  },
  "set": {
    "nothing0": "nothing",
    "nothing1": "nothing"
  }
}
)");
    });
}

TEST(number, inf_3)
{
    csubstr yaml = R"(
good:
  +.inf: +.inf
  +.inf:   +.inf
  +.Inf: +.Inf
  +.INF: +.INF
  +inf: +inf
  +infinity: +infinity
  infinity: infinity
  +.inf:
    +.inf
set:
  nothing0: nothing
  nothing1: nothing
)";
    test_check_emit_check(yaml, [](Tree const& t){
        float finf = std::numeric_limits<float>::infinity();
        double dinf = std::numeric_limits<double>::infinity();
        EXPECT_EQ(t["good"][0].val(), "+.inf");
        EXPECT_EQ(t["good"][1].val(), "+.inf");
        EXPECT_EQ(t["good"][2].val(), "+.Inf");
        EXPECT_EQ(t["good"][3].val(), "+.INF");
        EXPECT_EQ(t["good"][4].val(), "+inf");
        EXPECT_EQ(t["good"][5].val(), "+infinity");
        EXPECT_EQ(t["good"][6].val(), "infinity");
        EXPECT_EQ(t["good"][7].val(), "+.inf");
        EXPECT_EQ(t["good"][0].key(), "+.inf");
        EXPECT_EQ(t["good"][1].key(), "+.inf");
        EXPECT_EQ(t["good"][2].key(), "+.Inf");
        EXPECT_EQ(t["good"][3].key(), "+.INF");
        EXPECT_EQ(t["good"][4].key(), "+inf");
        EXPECT_EQ(t["good"][5].key(), "+infinity");
        EXPECT_EQ(t["good"][6].key(), "infinity");
        EXPECT_EQ(t["good"][7].key(), "+.inf");
        for(ConstNodeRef ch : t["good"]){
            SCOPED_TRACE(ch.key());
            {
                float f = 0.f;
                double d = 0.;
                ch.load(&f);
                ch.load(&d);
                EXPECT_TRUE(fleq(f, finf));
                EXPECT_TRUE(fleq(d, dinf));
            }
            {
                float f = 0.f;
                double d = 0.;
                ch.load_key(&f);
                ch.load_key(&d);
                EXPECT_TRUE(fleq(f, finf));
                EXPECT_TRUE(fleq(d, dinf));
            }
        }
        for(ConstNodeRef ch : t["set"]){
            SCOPED_TRACE(ch.key());
            float f = 0.f;
            double d = 0.;
            ExpectError::check_error_visit(&t, [&]{ ch.load(&f); });
            ExpectError::check_error_visit(&t, [&]{ ch.load(&d); });
            ExpectError::check_error_visit(&t, [&]{ ch.load_key(&f); });
            ExpectError::check_error_visit(&t, [&]{ ch.load_key(&d); });
        }
        EXPECT_EQ(emitrs_yaml<std::string>(t),
                  R"(good:
  +.inf: +.inf
  +.inf: +.inf
  +.Inf: +.Inf
  +.INF: +.INF
  +inf: +inf
  +infinity: +infinity
  infinity: infinity
  +.inf: +.inf
set:
  nothing0: nothing
  nothing1: nothing
)");
        EXPECT_EQ(emitrs_json<std::string>(t),
                  R"({
  "good": {
    ".inf": ".inf",
    ".inf": ".inf",
    ".inf": ".inf",
    ".inf": ".inf",
    ".inf": ".inf",
    ".inf": ".inf",
    ".inf": ".inf",
    ".inf": ".inf"
  },
  "set": {
    "nothing0": "nothing",
    "nothing1": "nothing"
  }
}
)");
    });
}



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

namespace {
template<class T>
struct val_and_str
{
    csubstr str;
    T val;
};
template<class T>
Tree make_tree(val_and_str<T> const* vals, size_t num_vals)
{
    Tree tree;
    NodeRef r = tree;
    r.set_map();
    auto append = [&](csubstr number) {
        NodeRef ch = r.append_child();
        ch .set_key_serialized(number);
        ch .set_serialized(number);
    };
    for(size_t i = 0; i < num_vals; ++i)
    {
        append(vals[i].str);
    }
    return tree;
}
template<class T, class Cmp>
void check_vals(val_and_str<T> const* vals, size_t num_vals, Cmp const& cmp)
{
    Tree tree = make_tree(vals, num_vals);
    NodeRef tmp = tree["tmp"];
    for(size_t i = 0; i < num_vals; ++i)
    {
        auto const& vs = vals[i];
        T val = {};
        ConstNodeRef node = tree[vs.str];
        ReadResult rr = node.deserialize(&val);
        {
            RYML_TRACE_FMT("i={}  vs.str={}  vs.val={}   val={}", i, vs.str, vs.val, val);
            EXPECT_TRUE(rr);
            EXPECT_TRUE(cmp(vs.val, val));
        }
        tmp.save(val);
        val = {};
        rr = tmp.deserialize(&val);
        {
            RYML_TRACE_FMT("i={}  vs.str={}  vs.val={}   val={}", i, vs.str, vs.val, val);
            EXPECT_TRUE(rr);
            EXPECT_TRUE(cmp(vs.val, val));
        }
    }
}

template<class T> bool cmpints(T lhs, T rhs) { return lhs == rhs; }
template<class T> bool cmpfloat(T lhs, T rhs, T eps) { return std::abs(lhs - rhs) < eps; }

#define int_cases_bin(lcase, ucase, _)\
            _(lcase, ucase, "0", 0),\
            _(lcase, ucase, "1", 1),\
            _(lcase, ucase, "10", 2),\
            _(lcase, ucase, "010", 2),\
            _(lcase, ucase, "0010", 2),\
            _(lcase, ucase, "11", 3),\
            _(lcase, ucase, "011", 3),\
            _(lcase, ucase, "0011", 3),\
            _(lcase, ucase, "100", 4),\
            _(lcase, ucase, "0100", 4),\
            _(lcase, ucase, "00100", 4),\
            _(lcase, ucase, "101", 5),\
            _(lcase, ucase, "0101", 5),\
            _(lcase, ucase, "00101", 5),
#define int_cases_oct(lcase, ucase, _)\
            _(lcase, ucase, "0", 0),\
            _(lcase, ucase, "1", 1),\
            _(lcase, ucase, "10", 8),\
            _(lcase, ucase, "010", 8),\
            _(lcase, ucase, "0010", 8),\
            _(lcase, ucase, "11", 9),\
            _(lcase, ucase, "011", 9),\
            _(lcase, ucase, "0011", 9),\
            _(lcase, ucase, "100", 64),\
            _(lcase, ucase, "0100", 64),\
            _(lcase, ucase, "00100", 64),\
            _(lcase, ucase, "101", 65),\
            _(lcase, ucase, "0101", 65),\
            _(lcase, ucase, "00101", 65),
#define int_cases_hex(lcase, ucase, _)\
            _(lcase, ucase, "0", 0),\
            _(lcase, ucase, "1", 1),\
            _(lcase, ucase, "10", 16),\
            _(lcase, ucase, "010", 16),\
            _(lcase, ucase, "0010", 16),\
            _(lcase, ucase, "11", 17),\
            _(lcase, ucase, "011", 17),\
            _(lcase, ucase, "0011", 17),\
            _(lcase, ucase, "100", 256),\
            _(lcase, ucase, "0100", 256),\
            _(lcase, ucase, "00100", 256),\
            _(lcase, ucase, "101", 257),\
            _(lcase, ucase, "0101", 257),\
            _(lcase, ucase, "00101", 257),

#define signed_case(lcase, ucase, str, val)                     \
    {csubstr("" lcase str), val}, {csubstr("" ucase str), val},\
    {csubstr("+" lcase str), val}, {csubstr("+" ucase str), val},\
    {csubstr("-" lcase str), -(val)}, {csubstr("-" ucase str), -(val)}
#define unsigned_case(lcase, ucase, str, val)                     \
    {csubstr("" lcase str), val}, {csubstr("" ucase str), val},   \
    {csubstr("+" lcase str), val}, {csubstr("+" ucase str), val}

template<class T>
auto test_ints_bin() -> typename std::enable_if<std::is_signed<T>::value, void>::type // NOLINT
{
    const val_and_str<T> vals[] = { int_cases_bin("0b", "0B", signed_case) };
    check_vals(vals, C4_COUNTOF(vals), cmpints<T>);
}
template<class T>
auto test_ints_bin() -> typename std::enable_if<std::is_unsigned<T>::value, void>::type // NOLINT
{
    const val_and_str<T> vals[] = { int_cases_bin("0b", "0B", unsigned_case) };
    check_vals(vals, C4_COUNTOF(vals), cmpints<T>);
}

template<class T>
auto test_ints_oct() -> typename std::enable_if<std::is_signed<T>::value, void>::type // NOLINT
{
    const val_and_str<T> vals[] = { int_cases_oct("0o", "0O", signed_case) };
    check_vals(vals, C4_COUNTOF(vals), cmpints<T>);
}
template<class T>
auto test_ints_oct() -> typename std::enable_if<std::is_unsigned<T>::value, void>::type // NOLINT
{
    const val_and_str<T> vals[] = { int_cases_oct("0o", "0O", unsigned_case) };
    check_vals(vals, C4_COUNTOF(vals), cmpints<T>);
}

template<class T>
auto test_ints_hex() -> typename std::enable_if<std::is_signed<T>::value, void>::type // NOLINT
{
    const val_and_str<T> vals[] = { int_cases_hex("0x", "0X", signed_case) };
    check_vals(vals, C4_COUNTOF(vals), cmpints<T>);
}
template<class T>
auto test_ints_hex() -> typename std::enable_if<std::is_unsigned<T>::value, void>::type // NOLINT
{
    const val_and_str<T> vals[] = { int_cases_hex("0x", "0X", unsigned_case) };
    check_vals(vals, C4_COUNTOF(vals), cmpints<T>);
}


template<class T>
void test_hex_float(T epsilon)
{
    const val_and_str<T> vals[] = {
        #define _(str, val)                                 \
            {csubstr("0x" str), val}, {csubstr("0X" str), val},\
            {csubstr("+0x" str), val}, {csubstr("+0X" str), val},\
            {csubstr("-0x" str), -(val)}, {csubstr("-0X" str), -(val)}
        _("0p+0", 0),
        _("1p+0", 1),
        _("2p+0", 2),
        _("3p+0", 3),
        #undef _
    };
    auto cmp = [epsilon](T lhs, T rhs){ return cmpfloat(lhs, rhs, epsilon); };
    check_vals(vals, C4_COUNTOF(vals), cmp);
}


TEST(number, bin_ints)
{
    test_ints_bin<int8_t>();
    test_ints_bin<int16_t>();
    test_ints_bin<int32_t>();
    test_ints_bin<int64_t>();
    test_ints_bin<uint8_t>();
    test_ints_bin<uint16_t>();
    test_ints_bin<uint32_t>();
    test_ints_bin<uint64_t>();
}

TEST(number, oct_ints)
{
    test_ints_oct<int8_t>();
    test_ints_oct<int16_t>();
    test_ints_oct<int32_t>();
    test_ints_oct<int64_t>();
    test_ints_oct<uint8_t>();
    test_ints_oct<uint16_t>();
    test_ints_oct<uint32_t>();
    test_ints_oct<uint64_t>();
}

TEST(number, hex_ints)
{
    //test_ints_hex<int8_t>();
    test_ints_hex<int16_t>();
    test_ints_hex<int32_t>();
    test_ints_hex<int64_t>();
    //test_ints_hex<uint8_t>();
    test_ints_hex<uint16_t>();
    test_ints_hex<uint32_t>();
    test_ints_hex<uint64_t>();
}

} // namespace

TEST(number, hex_double)
{
    test_hex_float<double>(1.e-10);
}

TEST(number, hex_float)
{
    test_hex_float<float>(1.e-6f);
}


namespace {
template<class T>
void test_hex_float_error()
{
    csubstr vals[] = {
        //"++0xap0", // FIXME
        //"+-0xap0", // FIXME
        "-+0xap0",
        "--0xap0",
        "+0xap++0",
        "-0xap++0",
        //"+0xap+-0", //FIXME
        //"-0xap+-0", //FIXME
        "+0xap-+0",
        "-0xap-+0",
        "+0xap--0",
        "-0xap--0",
        "+0xrp0",
        "-0xrp0",
        "+0x+ap0",
        "-0x+ap0",
        "+0x-ap0",
        "-0x-ap0",
    };
    for(csubstr v : vals)
    {
        SCOPED_TRACE(v);
        T var = {};
        EXPECT_FALSE(scalar_deserialize(v, &var));
    }
}
} // namespace

TEST(number, hex_error_float)
{
    test_hex_float_error<float>();
}

TEST(number, hex_error_double)
{
    test_hex_float_error<double>();
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

namespace {
template<class T>
static void checkinf_(ConstNodeRef const& n)
{
    csubstr val = n.val();
    SCOPED_TRACE(val);
    T fa = T(0);
    EXPECT_FALSE(std::isinf(fa));
    n.load(&fa);
    EXPECT_TRUE(std::isinf(fa));
}
#define checkinf(n) do { SCOPED_TRACE(#n); checkinf_<float>(n);  checkinf_<double>(n); } while(0)
template<class T>
static void checknan_(ConstNodeRef const& n)
{
    csubstr val = n.val();
    SCOPED_TRACE(val);
    T fa = T(0);
    EXPECT_FALSE(std::isnan(fa));
    n.load(&fa);
    EXPECT_TRUE(std::isnan(fa));
}
#define checknan(n) do { SCOPED_TRACE(#n); checknan_<float>(n);  checknan_<double>(n); } while(0)
} // namespace


TEST(number, github_312__proposed_8e888_cannot_be_converted)
{
    float f = {};
    double d = {};
    EXPECT_FALSE(from_chars("8e888", &f));
    EXPECT_FALSE(from_chars("8e888", &d));
    EXPECT_FALSE(from_chars("-8e888", &f));
    EXPECT_FALSE(from_chars("-8e888", &d));
    EXPECT_FALSE(from_chars_float("8e888", &f));
    EXPECT_FALSE(from_chars_float("8e888", &d));
    EXPECT_FALSE(from_chars_float("-8e888", &f));
    EXPECT_FALSE(from_chars_float("-8e888", &d));
    Tree t = parse_in_arena("8e888");
    ExpectError::check_error_visit(&t, [&]{ t.rootref().load(&f); });
    ExpectError::check_error_visit(&t, [&]{ t.rootref().load(&d); });
}

TEST(number, github_312_535__json_styles_for_special_values)
{
    EXPECT_EQ(scalar_style_choose_json("nan"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose_json("inf"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose_json("infinity"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose_json("-infinity"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose_json(".nan"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose_json(".NaN"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose_json(".NAN"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose_json("inf"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose_json("-inf"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose_json(".inf"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose_json("-.inf"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose_json(".Inf"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose_json("-.Inf"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose_json(".INF"), SCALAR_PLAIN);
    EXPECT_EQ(scalar_style_choose_json("-.INF"), SCALAR_PLAIN);
}

TEST(number, github_312_535)
{
    const Tree tree = parse_in_arena(R"({
  inf: [inf, .inf, .Inf, .INF, -inf, -.inf, -.Inf, -.INF, infinity, -infinity],
  nan: [nan, .nan, .NaN, .NAN],
  dot: [.1, 1., .2e2, 10., -.2, -2.],
  zero: [10, 01],
  normal: [0.1, 0.2e3, 4.e5],
})");
    const std::string expected_yaml = R"({
  inf: [inf,.inf,.Inf,.INF,-inf,-.inf,-.Inf,-.INF,infinity,-infinity],
  nan: [nan,.nan,.NaN,.NAN],
  dot: [.1,1.,.2e2,10.,-.2,-2.],
  zero: [10,01],
  normal: [0.1,0.2e3,4.e5]
}
)";
    const std::string expected_json = R"({
  ".inf": [".inf",".inf",".inf",".inf","-.inf","-.inf","-.inf","-.inf",".inf","-.inf"],
  ".nan": [".nan",".nan",".nan",".nan"],
  "dot": [0.1,1.0,0.2e2,10.0,-0.2,-2.0],
  "zero": [10,"01"],
  "normal": [0.1,0.2e3,4.e5]
}
)";
    enum : int { is_yaml, is_json };
    C4_SUPPRESS_WARNING_GCC_PUSH
    #if defined(__GNUC__) && (__GNUC__ >= 7)
    C4_SUPPRESS_WARNING_GCC("-Wduplicated-branches")
    #endif
    auto checktree_ = [](Tree const& t, int type){
        C4_SUPPRESS_WARNING_GCC_PUSH
        #if defined(__GNUC__) && __GNUC__>= 7
        C4_SUPPRESS_WARNING_GCC("-Wduplicated-branches")
        #endif
        // inf
        ConstNodeRef inf = type == is_yaml ? t["inf"] : t[".inf"];
        EXPECT_EQ(inf[0].val(), type == is_yaml ? "inf" : ".inf"); checkinf(inf[0]);
        EXPECT_EQ(inf[1].val(), type == is_yaml ? ".inf" : ".inf"); checkinf(inf[1]);
        EXPECT_EQ(inf[2].val(), type == is_yaml ? ".Inf" : ".inf"); checkinf(inf[2]);
        EXPECT_EQ(inf[3].val(), type == is_yaml ? ".INF" : ".inf"); checkinf(inf[3]);
        EXPECT_EQ(inf[4].val(), type == is_yaml ? "-inf" : "-.inf"); checkinf(inf[4]);
        EXPECT_EQ(inf[5].val(), type == is_yaml ? "-.inf" : "-.inf"); checkinf(inf[5]);
        EXPECT_EQ(inf[6].val(), type == is_yaml ? "-.Inf" : "-.inf"); checkinf(inf[6]);
        EXPECT_EQ(inf[7].val(), type == is_yaml ? "-.INF" : "-.inf"); checkinf(inf[7]);
        EXPECT_EQ(inf[8].val(), type == is_yaml ? "infinity" : ".inf"); checkinf(inf[8]);
        EXPECT_EQ(inf[9].val(), type == is_yaml ? "-infinity" : "-.inf"); checkinf(inf[9]);
        // nan
        ConstNodeRef nan = type == is_yaml ? t["nan"] : t[".nan"];
        EXPECT_EQ(nan[0].val(), type == is_yaml ? "nan" : ".nan"); checknan(nan[0]);
        EXPECT_EQ(nan[1].val(), type == is_yaml ? ".nan" : ".nan"); checknan(nan[1]);
        EXPECT_EQ(nan[2].val(), type == is_yaml ? ".NaN" : ".nan"); checknan(nan[2]);
        EXPECT_EQ(nan[3].val(), type == is_yaml ? ".NAN" : ".nan"); checknan(nan[3]);
        // dot: adds zero in json
        EXPECT_EQ(t["dot"][0].val(), type == is_yaml ? ".1" : "0.1"); // add zero in json
        EXPECT_EQ(t["dot"][1].val(), type == is_yaml ? "1." : "1.0"); // add zero in json
        EXPECT_EQ(t["dot"][2].val(), type == is_yaml ? ".2e2" : "0.2e2"); // add zero in json
        EXPECT_EQ(t["dot"][3].val(), type == is_yaml ? "10." : "10.0"); // add zero in json
        EXPECT_EQ(t["dot"][4].val(), type == is_yaml ? "-.2" : "-0.2"); // add zero in json
        EXPECT_EQ(t["dot"][5].val(), type == is_yaml ? "-2." : "-2.0"); // add zero in json
        // zero
        EXPECT_EQ(t["zero"][0].val(), "10");
        EXPECT_EQ(t["zero"][1].val(), "01");
        EXPECT_EQ(t["zero"][1].is_quoted(), type == is_json); // adds quotes in json
        // normal
        EXPECT_EQ(t["normal"][0].val(), "0.1");
        EXPECT_EQ(t["normal"][1].val(), "0.2e3");
        EXPECT_EQ(t["normal"][2].val(), "4.e5");
        C4_SUPPRESS_WARNING_GCC_POP
    };
    C4_SUPPRESS_WARNING_GCC_POP
    #define checktree(...) do { SCOPED_TRACE("here"); checktree_(__VA_ARGS__); } while(0)
    checktree(tree, is_yaml);
    std::string yaml = emitrs_yaml<std::string>(tree);
    std::string json = emitrs_json<std::string>(tree);
    for(size_t i = 0; i < 4; ++i) // test over several consecutive emits
    {
        RYML_TRACE_FMT("emit level={}", i);
        RYML_TRACE_FMT("yaml={}", yaml);
        RYML_TRACE_FMT("json={}", json);
        EXPECT_EQ(expected_yaml, yaml);
        EXPECT_EQ(expected_json, json);
        const Tree tree_yaml = parse_in_arena(to_csubstr(yaml));
        const Tree tree_json = parse_in_arena(to_csubstr(json));
        checktree(tree_yaml, is_yaml);
        checktree(tree_json, is_json);
        yaml = emitrs_yaml<std::string>(tree_yaml);
        json = emitrs_json<std::string>(tree_json);
    }
    #undef checktree
}
#undef checknan
#undef checkinf


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

CASE_GROUP(NUMBER)
{

ADD_CASE_TO_GROUP("integer numbers, flow", JSON_WRITE,
R"(translation: [-2, -2, 5, 0xa, -0xb, 0XA, -0XA, 0b10, -0b10, 0B10, -0B10, 0o17, -0o17, 0O17, -0O17])",
N(MB, L{
  N(KP|SFS, "translation", L{
    N(VP, "-2"), N(VP, "-2"), N(VP, "5"),
    N(VP, "0xa"), N(VP, "-0xb"),
    N(VP, "0XA"), N(VP, "-0XA"),
    N(VP, "0b10"), N(VP, "-0b10"),
    N(VP, "0B10"), N(VP, "-0B10"),
    N(VP, "0o17"), N(VP, "-0o17"),
    N(VP, "0O17"), N(VP, "-0O17"),
  })
})
);

ADD_CASE_TO_GROUP("integer numbers, block", JSON_WRITE,
R"(translation:
  - -2
  - -2
  - -5
)",
N(MB, L{N(KP|SB, "translation", L{N(VP, "-2"), N(VP, "-2"), N(VP, "-5")})})
);

ADD_CASE_TO_GROUP("floating point numbers, flow",
R"([-2.0, -2.1, 0.1, .1, -.2, -2.e+6, -3e-6, 1.12345e+011])",
N(SFS, L{N(VP, "-2.0"), N(VP, "-2.1"), N(VP, "0.1"), N(VP, ".1"), N(VP, "-.2"), N(VP, "-2.e+6"), N(VP, "-3e-6"), N(VP, "1.12345e+011")})
);

ADD_CASE_TO_GROUP("floating point numbers, flow jsonified", JSON_WRITE,
R"([-2.0, -2.1, 0.1, 0.1, -0.2, -2.e+6, -3e-6, 1.12345e+011])",
N(SFS, L{N(VP, "-2.0"), N(VP, "-2.1"), N(VP, "0.1"), N(VP, "0.1"), N(VP, "-0.2"), N(VP, "-2.e+6"), N(VP, "-3e-6"), N(VP, "1.12345e+011")})
);

ADD_CASE_TO_GROUP("floating point numbers, block",
R"(
- -2.0
- -2.1
- 0.1
- .1
- -.2
- -2.e+6
- -3e-6
- 1.12345e+011
)",
N(SB, L{N(VP, "-2.0"), N(VP, "-2.1"), N(VP, "0.1"), N(VP, ".1"), N(VP, "-.2"), N(VP, "-2.e+6"), N(VP, "-3e-6"), N(VP, "1.12345e+011")})
);

ADD_CASE_TO_GROUP("floating point numbers, block jsonified", JSON_WRITE,
R"(
- -2.0
- -2.1
- 0.1
- 0.1
- -0.2
- -2.e+6
- -3e-6
- 1.12345e+011
)",
N(SB, L{N(VP, "-2.0"), N(VP, "-2.1"), N(VP, "0.1"), N(VP, "0.1"), N(VP, "-0.2"), N(VP, "-2.e+6"), N(VP, "-3e-6"), N(VP, "1.12345e+011")})
);

ADD_CASE_TO_GROUP("hex floating point numbers, block",
R"(
- -2.0
- -2.1
- 0.1
- .1
- -.2
- -2.e+6
- -3e-6
- 1.12345e+011
)",
N(SB, L{N(VP, "-2.0"), N(VP, "-2.1"), N(VP, "0.1"), N(VP, ".1"), N(VP, "-.2"), N(VP, "-2.e+6"), N(VP, "-3e-6"), N(VP, "1.12345e+011")})
);

ADD_CASE_TO_GROUP("hex floating point numbers, block jsonified", JSON_WRITE,
R"(
- -2.0
- -2.1
- 0.1
- 0.1
- -0.2
- -2.e+6
- -3e-6
- 1.12345e+011
)",
N(SB, L{N(VP, "-2.0"), N(VP, "-2.1"), N(VP, "0.1"), N(VP, "0.1"), N(VP, "-0.2"), N(VP, "-2.e+6"), N(VP, "-3e-6"), N(VP, "1.12345e+011")})
);

ADD_CASE_TO_GROUP("version numbers", JSON_WRITE,
R"(
- 1.2.3
- 1.2.3.4
- [1.2.3, 4.5.6]
- [1.2.3.4, 4.5.6.7]
- - 1.2.3
  - 4.5.6
- - 1.2.3.4
  - 4.5.6.7
- a: 1.2.3
- a: 1.2.3.4
- {a: 1.2.3}
- {a: 1.2.3.4}
- a: 1.2.3
  b: 4.5.6
- a: 1.2.3.4
  b: 4.5.6.7
- {a: 1.2.3, b: 4.5.6}
- {a: 1.2.3.4, b: 4.5.6.7}
)",
N(SB, L{
    N(VP, "1.2.3"),
    N(VP, "1.2.3.4"),
    N(SFS, L{N(VP, "1.2.3"), N(VP, "4.5.6")}),
    N(SFS, L{N(VP, "1.2.3.4"), N(VP, "4.5.6.7")}),
    N(SB, L{N(VP, "1.2.3"), N(VP, "4.5.6")}),
    N(SB, L{N(VP, "1.2.3.4"), N(VP, "4.5.6.7")}),
    N(MB, L{N(KP|VP, "a", "1.2.3")}),
    N(MB, L{N(KP|VP, "a", "1.2.3.4")}),
    N(MFS, L{N(KP|VP, "a", "1.2.3")}),
    N(MFS, L{N(KP|VP, "a", "1.2.3.4")}),
    N(MB, L{N(KP|VP, "a", "1.2.3"), N(KP|VP, "b", "4.5.6")}),
    N(MB, L{N(KP|VP, "a", "1.2.3.4"), N(KP|VP, "b", "4.5.6.7")}),
    N(MFS, L{N(KP|VP, "a", "1.2.3"), N(KP|VP, "b", "4.5.6")}),
    N(MFS, L{N(KP|VP, "a", "1.2.3.4"), N(KP|VP, "b", "4.5.6.7")}),
}));
}

} // namespace yml
} // namespace c4
