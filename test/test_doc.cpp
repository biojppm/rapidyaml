#include "./test_lib/test_group.hpp"
#include "./test_lib/test_group.def.hpp"
#include <c4/utf.hpp>

namespace c4 {
namespace yml {


TEST(simple_doc, issue_251)
{
    {
        csubstr yaml = R"(
...
)";
        test_check_emit_check(yaml, [](Tree const &t){
            EXPECT_EQ(t.rootref().type(), NOTYPE);
            ASSERT_EQ(t.rootref().num_children(), 0u);
        });
    }
    {
        Tree tree;
        NodeRef root = tree.rootref();
        root |= MAP;
        root["test"] = "...";
        root["test"] |= VAL_SQUO;
        std::string s = emitrs_yaml<std::string>(tree);
        test_check_emit_check(to_csubstr(s), [](Tree const &t){
            EXPECT_EQ(t["test"].val(), "...");
        });
    }
}


TEST(simple_doc, test_suite_XLQ9)
{
    csubstr yaml = R"(
---
scalar
%YAML 1.2
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_stream());
        ASSERT_EQ(t.rootref().num_children(), 1u);
        ASSERT_TRUE(t.docref(0).is_doc());
        ASSERT_TRUE(t.docref(0).is_val());
        EXPECT_EQ(t.docref(0).val(), csubstr("scalar %YAML 1.2"));
    });
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

struct bomspec
{
    csubstr name; Encoding_e encoding; csubstr bom;
    void checkchars() const
    {
        if(name == "NOBOM")
            return;
        if(!name.begins_with('!'))
        {
            EXPECT_EQ(c4::first_non_bom(bom), bom.len);
            EXPECT_EQ(c4::get_bom(bom), bom);
            EXPECT_EQ(c4::skip_bom(bom), "");
        }
        else
        {
            EXPECT_EQ(c4::first_non_bom(bom), 0u);
            EXPECT_EQ(c4::get_bom(bom), "");
            EXPECT_EQ(c4::skip_bom(bom), bom);
        }
        switch(encoding)
        {
        case UTF32BE:
            ASSERT_EQ(bom.len, 4u);
            if(!name.begins_with('!'))
            {
                EXPECT_EQ(bom.str[0], '\x00');
                EXPECT_EQ(bom.str[1], '\x00');
                EXPECT_EQ(bom.str[2], '\xfe');
                EXPECT_EQ(bom.str[3], '\xff');
            }
            else
            {
                EXPECT_EQ(bom.str[0], '\x00');
                EXPECT_EQ(bom.str[1], '\x00');
                EXPECT_EQ(bom.str[2], '\x00');
                EXPECT_GE(bom.str[3], 0);
                EXPECT_LE(bom.str[3], 0x7f);
            }
            break;
        case UTF32LE:
            ASSERT_EQ(bom.len, 4u);
            if(!name.begins_with('!'))
            {
                EXPECT_EQ(bom.str[0], '\xff');
                EXPECT_EQ(bom.str[1], '\xfe');
                EXPECT_EQ(bom.str[2], '\x00');
                EXPECT_EQ(bom.str[3], '\x00');
            }
            else
            {
                EXPECT_GE(bom.str[0], 0);
                EXPECT_LE(bom.str[0], 0x7f);
                EXPECT_EQ(bom.str[1], '\x00');
                EXPECT_EQ(bom.str[2], '\x00');
                EXPECT_EQ(bom.str[3], '\x00');
            }
            break;
        case UTF16BE:
            ASSERT_EQ(bom.len, 2u);
            if(!name.begins_with('!'))
            {
                EXPECT_EQ(bom.str[0], '\xfe');
                EXPECT_EQ(bom.str[1], '\xff');
            }
            else
            {
                EXPECT_EQ(bom.str[0], '\x00');
                EXPECT_GE(bom.str[1], 0);
                EXPECT_LE(bom.str[1], 0x7f);
            }
            break;
        case UTF16LE:
            ASSERT_EQ(bom.len, 2u);
            if(!name.begins_with('!'))
            {
                EXPECT_EQ(bom.str[0], '\xff');
                EXPECT_EQ(bom.str[1], '\xfe');
            }
            else
            {
                EXPECT_GE(bom.str[0], 0);
                EXPECT_LE(bom.str[0], 0x7f);
                EXPECT_EQ(bom.str[1], '\x00');
            }
            break;
        case UTF8:
            ASSERT_EQ(bom.len, 3u);
            EXPECT_EQ(bom.str[0], '\xef');
            EXPECT_EQ(bom.str[1], '\xbb');
            EXPECT_EQ(bom.str[2], '\xbf');
            break;
        default:
            break;
        }
    }
};
const bomspec boms[] = {
    {"NOBOM"       , UTF8, ""},
    {"UTF8"        , UTF8, "\xef\xbb\xbf"},
    {"UTF16BE"     , UTF16BE, "\xfe\xff"},
    {"!UTF16BE-a"  , UTF16BE, csubstr("\x00""a", 2)}, // bare string causes problems in gcc5 and earlier
    {"!UTF16BE-b"  , UTF16BE, csubstr("\x00""b", 2)}, // bare string causes problems in gcc5 and earlier
    {"!UTF16BE-0"  , UTF16BE, csubstr("\x00""0", 2)}, // bare string causes problems in gcc5 and earlier
    {"UTF16LE"     , UTF16LE, "\xff\xfe"},
    {"!UTF16LE-a"  , UTF16LE, csubstr("a""\x00" , 2)}, // bare string causes problems in gcc5 and earlier
    {"!UTF16LE-b"  , UTF16LE, csubstr("b""\x00" , 2)}, // bare string causes problems in gcc5 and earlier
    {"!UTF16LE-0"  , UTF16LE, csubstr("0""\x00" , 2)}, // bare string causes problems in gcc5 and earlier
    {"UTF32BE"     , UTF32BE, csubstr("\x00\x00\xfe\xff", 4)}, // bare string causes problems in gcc5 and earlier
    {"!UTF32BE-a"  , UTF32BE, csubstr("\x00\x00\x00""a" , 4)}, // bare string causes problems in gcc5 and earlier
    {"!UTF32BE-b"  , UTF32BE, csubstr("\x00\x00\x00""b" , 4)}, // bare string causes problems in gcc5 and earlier
    {"!UTF32BE-0"  , UTF32BE, csubstr("\x00\x00\x00""0" , 4)}, // bare string causes problems in gcc5 and earlier
    {"UTF32LE"     , UTF32LE, csubstr("\xff\xfe\x00\x00", 4)}, // bare string causes problems in gcc5 and earlier
    {"!UTF32LE-a"  , UTF32LE, csubstr("a""\x00\x00\x00" , 4)}, // bare string causes problems in gcc5 and earlier
    {"!UTF32LE-b"  , UTF32LE, csubstr("b""\x00\x00\x00" , 4)}, // bare string causes problems in gcc5 and earlier
    {"!UTF32LE-0"  , UTF32LE, csubstr("0""\x00\x00\x00" , 4)}, // bare string causes problems in gcc5 and earlier
};
template<class CreateFn, class TestFn>
void test_boms(CreateFn &&createfn, TestFn &&testfn)
{
    Parser::handler_type handler;
    Parser parser(&handler);
    for(const bomspec bom : boms)
    {
        std::string buf = std::forward<CreateFn>(createfn)(bom);
        SCOPED_TRACE(bom.name);
        SCOPED_TRACE(buf);
        bom.checkchars();
        Tree tree = parse_in_arena(&parser, to_csubstr(buf));
        std::forward<TestFn>(testfn)(parser, tree, bom);
    }
}
template<class CreateFn, class TestFn>
void test_boms_json(CreateFn &&createfn, TestFn &&testfn)
{
    Parser::handler_type handler;
    Parser parser(&handler);
    for(const bomspec bom : boms)
    {
        std::string buf = std::forward<CreateFn>(createfn)(bom);
        SCOPED_TRACE(bom.name);
        SCOPED_TRACE(buf);
        bom.checkchars();
        Tree tree = parse_json_in_arena(&parser, to_csubstr(buf));
        std::forward<TestFn>(testfn)(parser, tree, bom);
    }
}
template<class CreateFn, class TestFn>
void test_boms2(CreateFn &&createfn, TestFn &&testfn)
{
    for(const bomspec bom1 : boms)
    {
        SCOPED_TRACE(bom1.name);
        bom1.checkchars();
        for(const bomspec bom2 : boms)
        {
            SCOPED_TRACE(bom2.name);
            bom2.checkchars();
            std::string buf = std::forward<CreateFn>(createfn)(bom1, bom2);
            SCOPED_TRACE(buf);
            if(bom1.encoding == bom2.encoding || bom2.bom.empty())
            {
                Parser::handler_type handler;
                Parser parser(&handler);
                Tree tree = parse_in_arena(&parser, to_csubstr(buf));
                std::forward<TestFn>(testfn)(parser, tree, bom1);
            }
            else
            {
                pfn_error orig = get_callbacks().m_error;
                ExpectError::check_error([&]{
                    Tree tree;
                    Parser::handler_type handler;
                    Parser parser(&handler);
                    ASSERT_EQ((pfn_error)tree.callbacks().m_error, (pfn_error)parser.callbacks().m_error);
                    ASSERT_NE((pfn_error)tree.callbacks().m_error, orig);
                    parse_in_arena(&parser, to_csubstr(buf), &tree);
                });
            }
        }
    }
}

TEST(byte_order_mark, only_bom)
{
    test_boms(
        [](bomspec bom){
            return std::string(bom.bom.str, bom.bom.len);
        },
        [](Parser const& parser, Tree const&, bomspec bom){
            EXPECT_EQ(parser.encoding(), bom.encoding);
        });
}

TEST(byte_order_mark, bom_and_scalar)
{
    test_boms(
        [](bomspec bom){
            std::string yaml(bom.bom.str, bom.bom.len);
            yaml.append("this is a scalar");
            return yaml;
        },
        [](Parser const& parser, Tree const& tree, bomspec bom){
            EXPECT_EQ(parser.encoding(), bom.encoding);
            EXPECT_EQ(tree.rootref().val(), "this is a scalar");
        });
}

TEST(byte_order_mark, scalar_and_bom)
{
    auto mkyaml = [](bomspec bom){
        std::string yaml("this is a scalar");
        yaml.append(bom.bom.str, bom.bom.len);
        return yaml;
    };
    test_boms(mkyaml,
        [&](Parser const& parser, Tree const& tree, bomspec bom){
            EXPECT_EQ(parser.encoding(), UTF8);
            EXPECT_EQ(tree.rootref().val(), mkyaml(bom));
        });
}

TEST(byte_order_mark, scalar_bom_scalar)
{
    auto mkyaml = [](bomspec bom){
        std::string yaml("this is a scalar");
        yaml.append(bom.bom.str, bom.bom.len);
        yaml.append("and it continues");
        return yaml;
    };
    test_boms(mkyaml,
        [&](Parser const& parser, Tree const& tree, bomspec bom){
            EXPECT_EQ(parser.encoding(), UTF8);
            EXPECT_EQ(tree.rootref().val(), mkyaml(bom));
        });
}

TEST(byte_order_mark, bom_and_seq)
{
    auto mkyaml = [](bomspec bom){
        std::string yaml(bom.bom.str, bom.bom.len);
        yaml.append("[]");
        return yaml;
    };
    auto test = [&](Parser const& parser, Tree const&, bomspec bom){
        EXPECT_EQ(parser.encoding(), bom.encoding);
    };
    test_boms(mkyaml, test);
    test_boms_json(mkyaml, test);
}

TEST(byte_order_mark, bom_and_map)
{
    auto mkyaml = [](bomspec bom){
        std::string yaml(bom.bom.str, bom.bom.len);
        yaml.append("{}");
        return yaml;
    };
    auto test = [&](Parser const& parser, Tree const&, bomspec bom){
        EXPECT_EQ(parser.encoding(), bom.encoding);
    };
    test_boms(mkyaml, test);
    test_boms_json(mkyaml, test);
}

TEST(byte_order_mark, bom_and_doc)
{
    auto mkyaml = [](bomspec bom){
        std::string yaml(bom.bom.str, bom.bom.len);
        yaml.append("---\nabc");
        return yaml;
    };
    auto test = [&](Parser const& parser, Tree const& tree, bomspec bom){
        EXPECT_EQ(parser.encoding(), bom.encoding);
        EXPECT_EQ(tree.docref(0).val(), "abc");
    };
    test_boms(mkyaml, test);
}

TEST(byte_order_mark, bom_doc_bom)
{
    auto mkyaml = [](bomspec bom1, bomspec bom2){
        std::string yaml(bom1.bom.str, bom1.bom.len);
        yaml.append("---\n");
        yaml.append(bom2.bom.str, bom2.bom.len);
        yaml.append("abc");
        std::cout << bom1.name << " vs " << bom2.name << "\n" << yaml  << "\n";
        return yaml;
    };
    auto test = [&](Parser const& parser, Tree const& tree, bomspec bom){
        EXPECT_EQ(parser.encoding(), bom.encoding);
        EXPECT_EQ(tree.docref(0).val(), "abc");
    };
    test_boms2(mkyaml, test);
}

TEST(byte_order_mark, bom_scalar_doc_bom_scalar)
{
    auto mkyaml = [](bomspec bom1, bomspec bom2){
        std::string yaml(bom1.bom.str, bom1.bom.len);
        yaml.append("abc\n");
        yaml.append("---\n");
        yaml.append(bom2.bom.str, bom2.bom.len);
        yaml.append("def\n");
        std::cout << bom1.name << " vs " << bom2.name << "\n" << yaml  << "\n";
        return yaml;
    };
    auto test = [&](Parser const& parser, Tree const& tree, bomspec bom){
        EXPECT_EQ(parser.encoding(), bom.encoding);
        ASSERT_EQ(tree.rootref().num_children(), 2);
        EXPECT_EQ(tree.docref(0).val(), "abc");
        EXPECT_EQ(tree.docref(1).val(), "def");
    };
    test_boms2(mkyaml, test);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

CASE_GROUP(SIMPLE_DOC)
{

ADD_CASE_TO_GROUP("one empty doc",
R"(---
)",
    N(STREAM, L{DOC|VN})
);

ADD_CASE_TO_GROUP("one empty doc, indented",
R"( ---
)",
    N(VP, "---")
);

ADD_CASE_TO_GROUP("one termination",
R"(...
)",
    NOTYPE
);

ADD_CASE_TO_GROUP("one termination, indented",
R"( ...
)",
    N(VP, "...")
);

ADD_CASE_TO_GROUP("two terminations",
R"(...
...
)",
    NOTYPE
);

ADD_CASE_TO_GROUP("two terminations, indented",
R"( ...
 ...
)",
    N(VP, "... ...")
);

ADD_CASE_TO_GROUP("three terminations",
R"(...
...
...
)",
    NOTYPE
);

ADD_CASE_TO_GROUP("three terminations and one explicit, v0",
R"(...
...
...
---
)",
    N(STREAM, L{DOC|VN})
);

ADD_CASE_TO_GROUP("three terminations and one explicit, v1",
R"(...
...
---
...
)",
    N(STREAM, L{DOC|VN})
);

ADD_CASE_TO_GROUP("three terminations and one explicit, v2",
R"(...
---
...
...
)",
    N(STREAM, L{DOC|VN})
);

ADD_CASE_TO_GROUP("three terminations and one explicit, v3",
R"(---
...
...
...
)",
    N(STREAM, L{DOC|VN})
);

ADD_CASE_TO_GROUP("one empty doc, explicit termination",
R"(---
...
)",
    N(STREAM, L{DOC|VN})
);

ADD_CASE_TO_GROUP("one empty doc, explicit termination, first indented",
R"( ---
...
)",
    N(VP, "---")
);

ADD_CASE_TO_GROUP("two empty docs",
R"(---
---
)",
    N(STREAM, L{DOC|VN, DOC|VN})
);

ADD_CASE_TO_GROUP("two empty docs, with termination",
R"(---
...
---
)",
    N(STREAM, L{DOC|VN, DOC|VN})
);

ADD_CASE_TO_GROUP("doc with single scalar",
R"(a scalar
)",
N(VP, "a scalar")
);

ADD_CASE_TO_GROUP("doc with single scalar, explicit",
R"(--- a scalar
)",
N(STREAM, L{N(DOC|VP, "a scalar")})
);

ADD_CASE_TO_GROUP("simple doc, empty docs",
R"(---
---
---
---
)",
    N(STREAM, L{DOC|VN, DOC|VN, DOC|VN, DOC|VN})
);

ADD_CASE_TO_GROUP("simple doc, empty docs, indented",
R"(    ---
    ---
    ---
    ---
)",
    N(VP, "--- --- --- ---")
);

ADD_CASE_TO_GROUP("simple doc, empty docs, term",
R"(---
...


---
...
---
...
---
...
)",
    N(STREAM, L{DOC|VN, DOC|VN, DOC|VN, DOC|VN})
);

ADD_CASE_TO_GROUP("simple doc, empty docs, term, indented",
R"(
    ---

    ...
    ---
    ...

    ---
    ...
    ---
    ...
)",
    N(VP, "---\n... --- ...\n--- ... --- ...")
);

ADD_CASE_TO_GROUP("simple doc, plain scalar, multiple docs, implicit 2nd doc",
R"(---
- a plain scalar
    with several lines
...
- a second plain scalar
    with several lines
)",
N(STREAM, L{
  N(DOC|SB, L{N(VP, "a plain scalar with several lines")}),
  N(DOC|SB, L{N(VP, "a second plain scalar with several lines")}),
}));

ADD_CASE_TO_GROUP("simple doc, single scalar, implicit doc",
R"(a scalar with some spaces inside
)",
  N(VP, "a scalar with some spaces inside")
);

ADD_CASE_TO_GROUP("simple doc, single scalar, implicit doc, indented",
R"(    a scalar with some spaces inside
)",
  N(VP,"a scalar with some spaces inside")
);

ADD_CASE_TO_GROUP("simple doc, multi scalar, implicit doc",
R"(a scalar with some spaces inside,
and yet another one with more spaces inside,
and it doesn't really stop
)",
    N(VP, "a scalar with some spaces inside, and yet another one with more spaces inside, and it doesn't really stop")
);

ADD_CASE_TO_GROUP("simple doc, multi scalar, implicit doc, indented",
R"(
    a scalar with some spaces inside,
    and yet another one with more spaces inside,
    and it doesn't really stop
)",
    N(VP, "a scalar with some spaces inside, and yet another one with more spaces inside, and it doesn't really stop")
);

ADD_CASE_TO_GROUP("simple doc, single scalar, explicit doc, implicit termination",
R"(---
a scalar with some spaces inside
)",
    N(STREAM, L{N(DOC|VP, "a scalar with some spaces inside")})
);

ADD_CASE_TO_GROUP("simple doc, single scalar, explicit doc, implicit termination, indented",
R"(    ---
    a scalar with some spaces inside
)",
    N(VP, "--- a scalar with some spaces inside")
);

ADD_CASE_TO_GROUP("simple doc, single scalar, explicit doc, explicit termination",
R"(---
a scalar with some spaces inside
...
)",
    N(STREAM, L{N(DOC|VP, "a scalar with some spaces inside")})
);

ADD_CASE_TO_GROUP("simple doc, single scalar, explicit doc, explicit termination, indented",
R"(    ---
    a scalar with some spaces inside
    ...
)",
    N(VP, "--- a scalar with some spaces inside ...")
);

ADD_CASE_TO_GROUP("simple doc, multi doc, seq-map",
R"(---
- a
- b
- c
...
---
a: 0
b: 1
c: 2
)",
    N(STREAM, L{
        N(DOC|SB, L{N(VP, "a"), N(VP, "b"), N(VP, "c")}),
        N(DOC|MB, L{N(KP|VP, "a", "0"), N(KP|VP, "b", "1"), N(KP|VP, "c", "2")})
    })
);

ADD_CASE_TO_GROUP("simple doc, multi doc, seq-map, indented",
R"(
    ---
    - a
    - b
    - c
    ...
    ---
)",
N(VP, "--- - a - b - c ... ---")
);

ADD_CASE_TO_GROUP("simple doc, 2XXW",
R"(
--- !!set
? Mark McGwire
? Sammy Sosa
? Ken Griff
)",
    N(STREAM, L{
      N(DOC|MB, TL("!!set", L{
        N(KP|VN, "Mark McGwire", {}),
        N(KP|VN, "Sammy Sosa", {}),
        N(KP|VN, "Ken Griff", {}),
      }))
   })
);

ADD_CASE_TO_GROUP("simple doc, 2XXW, indented",
R"(
    --- !!set
    ? Mark McGwire
    ? Sammy Sosa
    ? Ken Griff
)",
N(VP, "--- !!set ? Mark McGwire ? Sammy Sosa ? Ken Griff")
);

ADD_CASE_TO_GROUP("simple doc, multi doc, seq-map, no term",
R"(---
- a
- b
- c
---
a: 0
b: 1
c: 2
)",
N(STREAM, L{
  N(DOC|SB, L{N(VP, "a"), N(VP, "b"), N(VP, "c")}),
  N(DOC|MB, L{N(KP|VP, "a", "0"), N(KP|VP, "b", "1"), N(KP|VP, "c", "2")})
})
);

ADD_CASE_TO_GROUP("simple doc, multi doc, seq-map, no term, indented", EXPECT_PARSE_ERROR,
R"(# the first : should cause a parse error
    ---
    - a
    - b
    - c
    ---
    a: 0
    b: 1
    c: 2
)",
   LineCol(7, 6)
);

ADD_CASE_TO_GROUP("simple doc, multi doc, map-seq",
R"(---
a: 0
b: 1
c: 2
...
---
- a
- b
- c
...
)",
N(STREAM, L{
  N(DOC|MB, L{N(KP|VP, "a", "0"), N(KP|VP, "b", "1"), N(KP|VP, "c", "2")}),
  N(DOC|SB, L{N(VP, "a"), N(VP, "b"), N(VP, "c")}),
})
);

ADD_CASE_TO_GROUP("simple doc, multi doc, map-seq, indented", EXPECT_PARSE_ERROR,
R"(
    ---
    a: 0
    b: 1
    c: 2
    ...
    ---
    - a
    - b
    - c
    ...
)",
  LineCol(3, 6)
);

ADD_CASE_TO_GROUP("simple doc, multi doc, map-seq, no term",
R"(---
a: 0
b: 1
c: 2
---
- a
- b
- c
)",
N(STREAM, L{
  N(DOC|MB, L{N(KP|VP, "a", "0"), N(KP|VP, "b", "1"), N(KP|VP, "c", "2")}),
  N(DOC|SB, L{N(VP, "a"), N(VP, "b"), N(VP, "c")}),
})
);

ADD_CASE_TO_GROUP("simple doc, multi doc, map-seq, no term, indented", EXPECT_PARSE_ERROR,
R"(
    ---
    a: 0
    b: 1
    c: 2
    ---
    - a
    - b
    - c
)",
  LineCol(3, 6)
);

ADD_CASE_TO_GROUP("simple doc, multi doc, impl seq-map",
R"(---
[a, b, c]
...
---
{a: 0, b: 1, c: 2}
...
)",
N(STREAM, L{
  N(DOC|SFS, L{N(VP, "a"), N(VP, "b"), N(VP, "c")}),
  N(DOC|MFS, L{N(KP|VP, "a", "0"), N(KP|VP, "b", "1"), N(KP|VP, "c", "2")}),
})
);

ADD_CASE_TO_GROUP("simple doc, multi doc, impl seq-map, indented", EXPECT_PARSE_ERROR,
R"(
    ---
    [a, b, c]
    ...
    ---
    {a: 0, b: 1, c: 2}
    ...
)",
  LineCol(6, 7)
);

ADD_CASE_TO_GROUP("simple doc, multi doc, impl seq-map, no term",
R"(---
[a, b, c]
---
{a: 0, b: 1, c: 2}
)",
N(STREAM, L{
  N(DOC|SFS, L{N(VP, "a"), N(VP, "b"), N(VP, "c")}),
  N(DOC|MFS, L{N(KP|VP, "a", "0"), N(KP|VP, "b", "1"), N(KP|VP, "c", "2")}),
})
);

ADD_CASE_TO_GROUP("simple doc, multi doc, impl seq-map, no term, indented", EXPECT_PARSE_ERROR,
R"(
    ---
    [a, b, c]
    ---
    {a: 0, b: 1, c: 2}
)",
  LineCol(5, 7)
);

ADD_CASE_TO_GROUP("simple doc, multi doc, impl map-seq",
R"(---
{a: 0, b: 1, c: 2}
...
---
[a, b, c]
...
)",
N(STREAM, L{
  N(DOC|MFS, L{N(KP|VP, "a", "0"), N(KP|VP, "b", "1"), N(KP|VP, "c", "2")}),
  N(DOC|SFS, L{N(VP, "a"), N(VP, "b"), N(VP, "c")}),
})
);

ADD_CASE_TO_GROUP("simple doc, multi doc, impl map-seq, indented", EXPECT_PARSE_ERROR,
R"(
    ---
    {a: 0, b: 1, c: 2}
    ...
    ---
    [a, b, c]
    ...
)",
  LineCol(3, 7)
);

ADD_CASE_TO_GROUP("simple doc, multi doc, impl map-seq, no term",
R"(---
{a: 0, b: 1, c: 2}
---
[a, b, c]
)",
N(STREAM, L{
  N(DOC|MFS, L{N(KP|VP, "a", "0"), N(KP|VP, "b", "1"), N(KP|VP, "c", "2")}),
  N(DOC|SFS, L{N(VP, "a"), N(VP, "b"), N(VP, "c")}),
})
);

ADD_CASE_TO_GROUP("simple doc, multi doc, impl map-seq, no term, indented", EXPECT_PARSE_ERROR,
R"(
    ---
    {a: 0, b: 1, c: 2}
    ---
    [a, b, c]
)",
  LineCol(3, 7)
);

ADD_CASE_TO_GROUP("simple doc, indented with empty lines", EXPECT_PARSE_ERROR,
R"(
    ---
    {a: 0, b: 1, c: 2,
    

 
  
   
    

    d:
       some scalar
    }
    ---
    a: 0
    b: 1
    c: 2
    

 
  
   
    

    d:
       some scalar
)",
  LineCol(3, 7)
);


ADD_CASE_TO_GROUP("simple doc, tags at global scope, 9WXW",
R"(# Private
!foo "bar"
...
# Global
%TAG ! tag:example.com,2000:app/
---
!foo "bar"
)",
N(STREAM, L{
  N(DOC|VD, TS("!foo", "bar")),
  // strict YAML should result in this for the second doc:
  //N(DOC|VD, TS("<tag:example.com,2000:app/foo>", "bar")),
  // but since we don't do lookup, it should result in:
  N(DOC|VD, TS("!foo", "bar")),
})
);
}

} // namespace yml
} // namespace c4
