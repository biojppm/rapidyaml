#include "./test_lib/test_group.hpp"
#include "./test_lib/test_group.def.hpp"
#ifdef RYML_SINGLE_HEADER
#include <ryml_all.hpp>
#else
#include <c4/charconv.hpp>
#endif

namespace c4 {
namespace yml {

template<class I>
auto mkvals() -> typename std::enable_if<std::is_signed<I>::value, std::vector<I>>::type
{
    return std::vector<I>({std::numeric_limits<I>::min(), -10, -5, -1, 0, 1, 5, 10, std::numeric_limits<I>::max(),});
}
template<class I>
auto mkvals() -> typename std::enable_if<!std::is_signed<I>::value, std::vector<I>>::type
{
    return std::vector<I>({0, 1, 5, 10, std::numeric_limits<I>::max(),});
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
        r["dec"].append_child() << val;
        r["hex"].append_child() << fmt::hex(val);
        r["bin"].append_child() << fmt::bin(val);
        r["oct"].append_child() << fmt::oct(val);
        out = notval(val);
        r["dec"].last_child() >> out;
        EXPECT_EQ(out, val);
        out = notval(val);
        r["hex"].last_child() >> out;
        EXPECT_EQ(out, val);
        out = notval(val);
        r["bin"].last_child() >> out;
        EXPECT_EQ(out, val);
        out = notval(val);
        r["oct"].last_child() >> out;
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
            parsed["dec"][pos] >> out;
            EXPECT_EQ(out, val);
            out = notval(val);
            parsed["hex"][pos] >> out;
            EXPECT_EQ(out, val);
            out = notval(val);
            parsed["bin"][pos]>> out;
            EXPECT_EQ(out, val);
            out = notval(val);
            parsed["oct"][pos] >> out;
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
            parsed["dec"][pos] >> out;
            EXPECT_EQ(out, val);
            out = notval(val);
            parsed["hex"][pos] >> out;
            EXPECT_EQ(out, val);
            out = notval(val);
            parsed["bin"][pos]>> out;
            EXPECT_EQ(out, val);
            out = notval(val);
            parsed["oct"][pos] >> out;
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

ADD_CASE_TO_GROUP("floating point numbers, flow", JSON_WRITE,
R"([-2.0, -2.1, 0.1, .1, -.2, -2.e+6, -3e-6, 1.12345e+011])",
N(SFS, L{N(VP, "-2.0"), N(VP, "-2.1"), N(VP, "0.1"), N(VP, ".1"), N(VP, "-.2"), N(VP, "-2.e+6"), N(VP, "-3e-6"), N(VP, "1.12345e+011")})
);

ADD_CASE_TO_GROUP("floating point numbers, block", JSON_WRITE,
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

ADD_CASE_TO_GROUP("hex floating point numbers, block", JSON_WRITE,
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
