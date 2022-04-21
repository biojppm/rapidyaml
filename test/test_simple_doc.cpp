#include "./test_group.hpp"

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
        root["test"] |= VALQUO;

        std::string s = emitrs<std::string>(tree);
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
        ASSERT_TRUE(t.rootref().first_child().is_doc());
        ASSERT_TRUE(t.rootref().first_child().is_val());
        EXPECT_EQ(t.rootref().first_child().val(), csubstr("scalar %YAML 1.2"));
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
    N(STREAM, L{DOCVAL})
);

ADD_CASE_TO_GROUP("one empty doc, explicit termination",
R"(---
...
)",
    N(STREAM, L{DOCVAL})
);

ADD_CASE_TO_GROUP("two empty docs",
R"(---
---
)",
    N(STREAM, L{DOCVAL, DOCVAL})
);

ADD_CASE_TO_GROUP("two empty docs, with termination",
R"(---
...
---
)",
    N(STREAM, L{DOCVAL, DOCVAL})
);

ADD_CASE_TO_GROUP("doc with single scalar",
R"(a scalar
)",
N(DOCVAL, "a scalar")
);

ADD_CASE_TO_GROUP("doc with single scalar, explicit",
R"(--- a scalar
)",
N(STREAM, L{N(DOCVAL, "a scalar")})
);

ADD_CASE_TO_GROUP("simple doc, empty docs",
R"(---
---
---
---
)",
    N(STREAM, L{DOCVAL, DOCVAL, DOCVAL, DOCVAL})
);

ADD_CASE_TO_GROUP("simple doc, empty docs, indented",
R"(    ---
    ---
    ---
    ---
)",
    N(STREAM, L{DOCVAL, DOCVAL, DOCVAL, DOCVAL})
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
    N(STREAM, L{DOCVAL, DOCVAL, DOCVAL, DOCVAL})
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
    N(STREAM, L{DOCVAL, DOCVAL, DOCVAL, DOCVAL})
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
  N(DOCSEQ, L{N("a plain scalar with several lines")}),
  N(DOCSEQ, L{N("a second plain scalar with several lines")}),
}));

ADD_CASE_TO_GROUP("simple doc, single scalar, implicit doc",
R"(a scalar with some spaces inside
)",
  N(DOCVAL, "a scalar with some spaces inside")
);

ADD_CASE_TO_GROUP("simple doc, single scalar, implicit doc, indented",
R"(    a scalar with some spaces inside
)",
  N(DOCVAL,"a scalar with some spaces inside")
);

ADD_CASE_TO_GROUP("simple doc, multi scalar, implicit doc",
R"(a scalar with some spaces inside,
and yet another one with more spaces inside,
and it doesn't really stop
)",
    N(L{
     N("a scalar with some spaces inside"),
     N("and yet another one with more spaces inside"),
     N("and it doesn't really stop"),
   })
);

ADD_CASE_TO_GROUP("simple doc, multi scalar, implicit doc, indented",
R"(
    a scalar with some spaces inside,
    and yet another one with more spaces inside,
    and it doesn't really stop
)",
    N(L{
     N("a scalar with some spaces inside"),
     N("and yet another one with more spaces inside"),
     N("and it doesn't really stop"),
   })
);

ADD_CASE_TO_GROUP("simple doc, single scalar, explicit doc, implicit termination",
R"(---
a scalar with some spaces inside
)",
    N(STREAM, L{N(DOCVAL, "a scalar with some spaces inside")})
);

ADD_CASE_TO_GROUP("simple doc, single scalar, explicit doc, implicit termination, indented",
R"(    ---
    a scalar with some spaces inside
)",
    N(STREAM, L{N(DOCVAL, "a scalar with some spaces inside")})
);

ADD_CASE_TO_GROUP("simple doc, single scalar, explicit doc, explicit termination",
R"(---
a scalar with some spaces inside
...
)",
    N(STREAM, L{N(DOCVAL, "a scalar with some spaces inside")})
);

ADD_CASE_TO_GROUP("simple doc, single scalar, explicit doc, explicit termination, indented",
R"(    ---
    a scalar with some spaces inside
    ...
)",
    N(STREAM, L{N(DOCVAL, "a scalar with some spaces inside")})
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
        N(DOCSEQ, L{N("a"), N("b"), N("c")}),
        N(DOCMAP, L{N("a", "0"), N("b", "1"), N("c", "2")})
    })
);

ADD_CASE_TO_GROUP("simple doc, multi doc, seq-map, indented",
R"(    ---
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
        N(DOCSEQ, L{N("a"), N("b"), N("c")}),
        N(DOCMAP, L{N("a", "0"), N("b", "1"), N("c", "2")})
    })
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
        N(DOCSEQ, L{N("a"), N("b"), N("c")}),
        N(DOCMAP, L{N("a", "0"), N("b", "1"), N("c", "2")})
    })
);

ADD_CASE_TO_GROUP("simple doc, multi doc, seq-map, no term, indented",
R"(
    ---
    - a
    - b
    - c
    ---
    a: 0
    b: 1
    c: 2
)",
    N(STREAM, L{
        N(DOCSEQ, L{N("a"), N("b"), N("c")}),
        N(DOCMAP, L{N("a", "0"), N("b", "1"), N("c", "2")})
    })
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
        N(DOCMAP, L{N("a", "0"), N("b", "1"), N("c", "2")}),
        N(DOCSEQ, L{N("a"), N("b"), N("c")})
    })
);

ADD_CASE_TO_GROUP("simple doc, multi doc, map-seq, indented",
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
    N(STREAM, L{
        N(DOCMAP, L{N("a", "0"), N("b", "1"), N("c", "2")}),
        N(DOCSEQ, L{N("a"), N("b"), N("c")})
    })
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
        N(DOCMAP, L{N("a", "0"), N("b", "1"), N("c", "2")}),
        N(DOCSEQ, L{N("a"), N("b"), N("c")})
    })
);

ADD_CASE_TO_GROUP("simple doc, multi doc, map-seq, no term, indented",
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
    N(STREAM, L{
        N(DOCMAP, L{N("a", "0"), N("b", "1"), N("c", "2")}),
        N(DOCSEQ, L{N("a"), N("b"), N("c")})
    })
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
        N(DOCSEQ, L{N("a"), N("b"), N("c")}),
        N(DOCMAP, L{N("a", "0"), N("b", "1"), N("c", "2")})
    })
);

ADD_CASE_TO_GROUP("simple doc, multi doc, impl seq-map, indented",
R"(
    ---
    [a, b, c]
    ...
    ---
    {a: 0, b: 1, c: 2}
    ...
)",
    N(STREAM, L{
        N(DOCSEQ, L{N("a"), N("b"), N("c")}),
        N(DOCMAP, L{N("a", "0"), N("b", "1"), N("c", "2")})
    })
);

ADD_CASE_TO_GROUP("simple doc, multi doc, impl seq-map, no term",
R"(---
[a, b, c]
---
{a: 0, b: 1, c: 2}
)",
    N(STREAM, L{
        N(DOCSEQ, L{N("a"), N("b"), N("c")}),
        N(DOCMAP, L{N("a", "0"), N("b", "1"), N("c", "2")})
    })
);

ADD_CASE_TO_GROUP("simple doc, multi doc, impl seq-map, no term, indented",
R"(
    ---
    [a, b, c]
    ---
    {a: 0, b: 1, c: 2}
)",
    N(STREAM, L{
        N(DOCSEQ, L{N("a"), N("b"), N("c")}),
        N(DOCMAP, L{N("a", "0"), N("b", "1"), N("c", "2")})
    })
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
        N(DOCMAP, L{N("a", "0"), N("b", "1"), N("c", "2")}),
        N(DOCSEQ, L{N("a"), N("b"), N("c")})
    })
);

ADD_CASE_TO_GROUP("simple doc, multi doc, impl map-seq, indented",
R"(
    ---
    {a: 0, b: 1, c: 2}
    ...
    ---
    [a, b, c]
    ...
)",
    N(STREAM, L{
        N(DOCMAP, L{N("a", "0"), N("b", "1"), N("c", "2")}),
        N(DOCSEQ, L{N("a"), N("b"), N("c")})
    })
);

ADD_CASE_TO_GROUP("simple doc, multi doc, impl map-seq, no term",
R"(---
{a: 0, b: 1, c: 2}
---
[a, b, c]
)",
    N(STREAM, L{
        N(DOCMAP, L{N("a", "0"), N("b", "1"), N("c", "2")}),
        N(DOCSEQ, L{N("a"), N("b"), N("c")})
    })
);

ADD_CASE_TO_GROUP("simple doc, multi doc, impl map-seq, no term, indented",
R"(
    ---
    {a: 0, b: 1, c: 2}
    ---
    [a, b, c]
)",
    N(STREAM, L{
        N(DOCMAP, L{N("a", "0"), N("b", "1"), N("c", "2")}),
        N(DOCSEQ, L{N("a"), N("b"), N("c")})
    })
);

ADD_CASE_TO_GROUP("simple doc, indented with empty lines",
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
    N(STREAM, L{
        N(DOCMAP, L{N("a", "0"), N("b", "1"), N("c", "2"), N("d", "some scalar")}),
        N(DOCMAP, L{N("a", "0"), N("b", "1"), N("c", "2"), N("d", "some scalar")}),
    })
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
  N(DOCVAL|VALQUO, TS("!foo", "bar")),
  // strict YAML should result in this for the second doc:
  //N(DOCVAL|VALQUO, TS("<tag:example.com,2000:app/foo>", "bar")),
  // but since we don't do lookup, it should result in:
  N(DOCVAL|VALQUO, TS("!foo", "bar")),
})
);
}

} // namespace yml
} // namespace c4
