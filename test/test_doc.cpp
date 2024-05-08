#include "./test_lib/test_group.hpp"
#include "./test_lib/test_group.def.hpp"

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

CASE_GROUP(SIMPLE_DOC)
{

ADD_CASE_TO_GROUP("one empty doc",
R"(---
)",
    N(STREAM, L{DOC|VP})
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
    N(STREAM, L{DOC|VP})
);

ADD_CASE_TO_GROUP("three terminations and one explicit, v1",
R"(...
...
---
...
)",
    N(STREAM, L{DOC|VP})
);

ADD_CASE_TO_GROUP("three terminations and one explicit, v2",
R"(...
---
...
...
)",
    N(STREAM, L{DOC|VP})
);

ADD_CASE_TO_GROUP("three terminations and one explicit, v3",
R"(---
...
...
...
)",
    N(STREAM, L{DOC|VP})
);

ADD_CASE_TO_GROUP("one empty doc, explicit termination",
R"(---
...
)",
    N(STREAM, L{DOC|VP})
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
    N(STREAM, L{DOC|VP, DOC|VP})
);

ADD_CASE_TO_GROUP("two empty docs, with termination",
R"(---
...
---
)",
    N(STREAM, L{DOC|VP, DOC|VP})
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
    N(STREAM, L{DOC|VP, DOC|VP, DOC|VP, DOC|VP})
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
    N(STREAM, L{DOC|VP, DOC|VP, DOC|VP, DOC|VP})
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
        N(KP|VP, "Mark McGwire", {}),
        N(KP|VP, "Sammy Sosa", {}),
        N(KP|VP, "Ken Griff", {}),
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
