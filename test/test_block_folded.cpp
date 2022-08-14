#include "./test_group.hpp"

namespace c4 {
namespace yml {

TEST(block_folded, basic)
{
    {
        Tree t = parse_in_arena(R"(>
hello
there

got it


really
)");
        EXPECT_EQ(t.rootref().val(), csubstr("hello there\ngot it\n\nreally\n"));
    }
}

TEST(block_folded, empty_block)
{
    {
        Tree t = parse_in_arena(R"(- >
)");
        EXPECT_EQ(t[0].val(), csubstr(""));
    }
    {
        Tree t = parse_in_arena(R"(- >-
)");
        EXPECT_EQ(t[0].val(), csubstr(""));
    }
    {
        Tree t = parse_in_arena(R"(- >+
)");
        EXPECT_EQ(t[0].val(), csubstr(""));
    }
    {
        Tree t = parse_in_arena(R"(
- >

- >-

- >+

)");
        EXPECT_FALSE(t.empty());
        EXPECT_EQ(t[0].val(), csubstr(""));
        EXPECT_EQ(t[1].val(), csubstr(""));
        EXPECT_EQ(t[2].val(), csubstr("\n"));
    }
    {
        Tree t = parse_in_arena(R"(
- >
  
- >-
  
- >+
  
)");
        EXPECT_FALSE(t.empty());
        EXPECT_EQ(t[0].val(), csubstr(""));
        EXPECT_EQ(t[1].val(), csubstr(""));
        EXPECT_EQ(t[2].val(), csubstr("\n"));
    }
    {
        Tree t = parse_in_arena(R"(
- >
- >-
- >+
)");
        EXPECT_FALSE(t.empty());
        EXPECT_EQ(t[0].val(), csubstr(""));
        EXPECT_EQ(t[1].val(), csubstr(""));
        EXPECT_EQ(t[2].val(), csubstr(""));
    }
}

TEST(block_folded, empty_block0)
{
    Tree t = parse_in_arena(R"(- >
)");
    EXPECT_EQ(t[0].val(), csubstr(""));
    t = parse_in_arena(R"(- >-
)");
    EXPECT_EQ(t[0].val(), csubstr(""));
    t = parse_in_arena(R"(- >+
)");
    EXPECT_EQ(t[0].val(), csubstr(""));
}

TEST(block_folded, empty_block1)
{
    const Tree t = parse_in_arena(R"(
- >-
  a
- >-
  
- >-
   
- >-
  
  
  
- >-
  
   
  
)");
    EXPECT_EQ(t[0].val(), csubstr("a"));
    EXPECT_EQ(t[1].val(), csubstr(""));
    EXPECT_EQ(t[2].val(), csubstr(""));
    EXPECT_EQ(t[3].val(), csubstr(""));
    EXPECT_EQ(t[4].val(), csubstr(""));
}

TEST(block_folded, issue152_not_indented)
{
    const Tree t = parse_in_arena(R"(
ok:
  - |
    exec pg_isready -U "dog" -d "dbname=dog" -h 127.0.0.1 -p 5432
  - parses - yes
ok_parses: yes
err:
  - |
    exec pg_isready -U "dog" -d "dbname=dog" -h 127.0.0.1 -p 5432
err_parses: no
err2:
  - >
    exec pg_isready -U "dog" -d "dbname=dog" -h 127.0.0.1 -p 5432
err2_parses: no
err3:
  - >-
    exec pg_isready -U "dog" -d "dbname=dog" -h 127.0.0.1 -p 5432
err3_parses: no
)");
    EXPECT_EQ(t["ok"  ][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432\n"));
    EXPECT_EQ(t["err" ][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432\n"));
    EXPECT_EQ(t["err2"][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432\n"));
    EXPECT_EQ(t["err3"][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432"));
}

TEST(block_folded, issue152_indented_once)
{
    const Tree t = parse_in_arena(R"(
indented_once:
  ok:
    - |
      exec pg_isready -U "dog" -d "dbname=dog" -h 127.0.0.1 -p 5432
    - parses - yes
  ok_parses: yes
  err:
    - |
      exec pg_isready -U "dog" -d "dbname=dog" -h 127.0.0.1 -p 5432
  err_parses: no
  err2:
    - >
      exec pg_isready -U "dog" -d "dbname=dog" -h 127.0.0.1 -p 5432
  err2_parses: no
  err3:
    - >-
      exec pg_isready -U "dog" -d "dbname=dog" -h 127.0.0.1 -p 5432
  err3_parses: no
)");
    ConstNodeRef n = t["indented_once"];
    EXPECT_EQ(n["ok"  ][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432\n"));
    EXPECT_EQ(n["err" ][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432\n"));
    EXPECT_EQ(n["err2"][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432\n"));
    EXPECT_EQ(n["err3"][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432"));
}

TEST(block_folded, issue152_indented_twice)
{
    const Tree t = parse_in_arena(R"(
indented_once:
  indented_twice:
    ok:
      - |
        exec pg_isready -U "dog" -d "dbname=dog" -h 127.0.0.1 -p 5432
      - parses - yes
    ok_parses: yes
    err:
      - |
        exec pg_isready -U "dog" -d "dbname=dog" -h 127.0.0.1 -p 5432
    err_parses: no
    err2:
      - >
        exec pg_isready -U "dog" -d "dbname=dog" -h 127.0.0.1 -p 5432
    err2_parses: no
    err3:
      - >-
        exec pg_isready -U "dog" -d "dbname=dog" -h 127.0.0.1 -p 5432
    err3_parses: no
)");
    ConstNodeRef n = t["indented_once"]["indented_twice"];
    EXPECT_EQ(n["ok"  ][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432\n"));
    EXPECT_EQ(n["err" ][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432\n"));
    EXPECT_EQ(n["err2"][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432\n"));
    EXPECT_EQ(n["err3"][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432"));
}

TEST(block_folded, issue152_indented_thrice)
{
    const Tree t = parse_in_arena(R"(
indented_once:
  indented_twice:
    indented_thrice:
      ok:
        - |
          exec pg_isready -U "dog" -d "dbname=dog" -h 127.0.0.1 -p 5432
        - parses - yes
      ok_parses: yes
      err:
        - |
          exec pg_isready -U "dog" -d "dbname=dog" -h 127.0.0.1 -p 5432
      err_parses: no
      err2:
        - >
          exec pg_isready -U "dog" -d "dbname=dog" -h 127.0.0.1 -p 5432
      err2_parses: no
      err3:
        - >-
          exec pg_isready -U "dog" -d "dbname=dog" -h 127.0.0.1 -p 5432
      err3_parses: no
)");
    ConstNodeRef n = t["indented_once"]["indented_twice"]["indented_thrice"];
    EXPECT_EQ(n["ok"  ][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432\n"));
    EXPECT_EQ(n["err" ][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432\n"));
    EXPECT_EQ(n["err2"][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432\n"));
    EXPECT_EQ(n["err3"][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432"));
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(block_folded, test_suite_4QFQ)
{
    csubstr yaml = R"(
- |1
  child2
- |3
    child2
- ' child2

'
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_EQ(t[0].val(), csubstr(" child2\n"));
        EXPECT_EQ(t[1].val(), csubstr(" child2\n"));
        EXPECT_EQ(t[2].val(), csubstr(" child2\n"));
    });
}

TEST(block_folded, test_suite_4QFQ_pt2)
{
    csubstr yaml = R"(---
- |
 child0
- >
 
  
  # child1
- |1
  child2
- >
 child3
---
foo:
  - |
   child0
  - >
   
    
    # child1
  - |2
     child2
  - >
   child3
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_stream());
        ConstNodeRef doc = t.rootref().child(0);
        ASSERT_TRUE(doc.is_seq());
        ASSERT_EQ(doc.num_children(), 4u);
        EXPECT_EQ(doc[0].val(), csubstr("child0\n"));
        EXPECT_EQ(doc[1].val(), csubstr("\n\n# child1\n"));
        EXPECT_EQ(doc[2].val(), csubstr(" child2\n"));
        EXPECT_EQ(doc[3].val(), csubstr("child3\n"));
        doc = t.rootref().child(1);
        ASSERT_TRUE(doc.is_map());
        ASSERT_EQ(doc["foo"].num_children(), 4u);
        EXPECT_EQ(doc["foo"][0].val(), csubstr("child0\n"));
        EXPECT_EQ(doc["foo"][1].val(), csubstr("\n\n# child1\n"));
        EXPECT_EQ(doc["foo"][2].val(), csubstr(" child2\n"));
        EXPECT_EQ(doc["foo"][3].val(), csubstr("child3\n"));
    });
}

TEST(block_folded, test_suite_6VJK)
{
    csubstr yaml = R"(- >
 Sammy Sosa completed another
 fine season with great stats.

 63 Home Runs
 0.288 Batting Average

 What a year!
- >
 Sammy Sosa completed another
 fine season with great stats.
   63 Home Runs
   0.288 Batting Average
 What a year!
- >
 Sammy Sosa completed another
 fine season with great stats.

   63 Home Runs
   0.288 Batting Average

 What a year!
- >
 Sammy Sosa completed another
 fine season with great stats.


   63 Home Runs
   0.288 Batting Average


 What a year!
- >
 Sammy Sosa completed another
 fine season with great stats.



   63 Home Runs
   0.288 Batting Average



 What a year!
- >-
 No folding needed
- >
 No folding needed)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_EQ(t[0].val(), csubstr("Sammy Sosa completed another fine season with great stats.\n63 Home Runs 0.288 Batting Average\nWhat a year!\n"));
        EXPECT_EQ(t[1].val(), csubstr("Sammy Sosa completed another fine season with great stats.\n  63 Home Runs\n  0.288 Batting Average\nWhat a year!\n"));
        EXPECT_EQ(t[2].val(), csubstr("Sammy Sosa completed another fine season with great stats.\n\n  63 Home Runs\n  0.288 Batting Average\n\nWhat a year!\n"));
        EXPECT_EQ(t[3].val(), csubstr("Sammy Sosa completed another fine season with great stats.\n\n\n  63 Home Runs\n  0.288 Batting Average\n\n\nWhat a year!\n"));
        EXPECT_EQ(t[4].val(), csubstr("Sammy Sosa completed another fine season with great stats.\n\n\n\n  63 Home Runs\n  0.288 Batting Average\n\n\n\nWhat a year!\n"));
        EXPECT_EQ(t[5].val(), csubstr("No folding needed"));
        EXPECT_EQ(t[6].val(), csubstr("No folding needed\n"));
    });
}

TEST(block_folded, test_suite_7T8X)
{
    csubstr yaml = R"(>

 folded
 line

 next
 line
   * bullet

   * list
   * lines

 last
 line

# Comment
)";
    Tree t = parse_in_arena(yaml);
    EXPECT_EQ(t.rootref().val(), "\nfolded line\nnext line\n  * bullet\n\n  * list\n  * lines\n\nlast line\n");
}

TEST(block_folded, test_suite_A6F9)
{
    csubstr yaml = R"(
strip: |-
  text
clip: |
  text
keep: |+
  text
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_EQ(t["strip"].val(), "text");
        EXPECT_EQ(t["clip"].val(), "text\n");
        EXPECT_EQ(t["keep"].val(), "text\n");
    });
}

TEST(block_folded, test_suite_B3HG)
{
    csubstr yaml = R"(
--- >
 folded
 text


--- >
 folded
 text
--- >
  folded text
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_EQ(t.docref(0).val(), csubstr("folded text\n"));
        EXPECT_EQ(t.docref(1).val(), csubstr("folded text\n"));
        EXPECT_EQ(t.docref(2).val(), csubstr("folded text\n"));
    });
}

TEST(block_folded, test_suite_D83L)
{
    csubstr yaml = R"(
- |2-
  explicit indent and chomp
- |-2
  chomp and explicit indent
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_TRUE(t.rootref().is_seq());
        EXPECT_EQ(t[0].val(), csubstr("explicit indent and chomp"));
        EXPECT_EQ(t[1].val(), csubstr("chomp and explicit indent"));
    });
}

TEST(block_folded, test_suite_DWX9)
{
    csubstr yaml = R"(
|
 
  
  literal
   
  
  text

 # Comment
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_EQ(t.rootref().val(), csubstr("\n\nliteral\n \n\ntext\n"));
    });
}

TEST(block_folded, test_suite_F6MC)
{
    csubstr yaml = R"(
a: >2
   more indented
  regular
b: >2


   more indented
  regular
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_EQ(t["a"].val(), csubstr(" more indented\nregular\n"));
        EXPECT_EQ(t["b"].val(), csubstr("\n\n more indented\nregular\n"));
    });
}

TEST(block_folded, test_suite_K858)
{
    csubstr yaml = R"(---
# strip
- >-

# clip
- >

# keep
- |+

---
strip: >-

clip: >

keep: |+

)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_EQ(t.docref(0).num_children(), 3u);
        EXPECT_EQ(t.docref(0)[0].val(), csubstr{});
        EXPECT_EQ(t.docref(0)[1].val(), csubstr{});
        EXPECT_EQ(t.docref(0)[2].val(), csubstr("\n"));
        ASSERT_TRUE(t.docref(1).has_child("strip"));
        ASSERT_TRUE(t.docref(1).has_child("keep"));
        ASSERT_TRUE(t.docref(1).has_child("clip"));
        EXPECT_EQ(t.docref(1)["strip"].val(), csubstr{});
        EXPECT_EQ(t.docref(1)["clip"].val(), csubstr{});
        EXPECT_EQ(t.docref(1)["keep"].val(), csubstr("\n"));
    });
}


TEST(block_folded, test_suite_MJS9)
{
    csubstr yaml = R"(
- >
  foo 
 
  	 bar

  baz
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_EQ(t[0].val(), csubstr("foo \n\n\t bar\n\nbaz\n")); // "foo \n\n \t bar\n\nbaz\n"
    });
}

TEST(block_folded, test_suite_P2AD)
{
    csubstr yaml = R"(
- | # Empty header↓
 literal
- >1 # Indentation indicator↓
  folded
- |+ # Chomping indicator↓
 keep

- >1- # Both indicators↓
  strip
- >-1 # Both indicators↓
  strip
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_seq());
        ASSERT_EQ(t.rootref().num_children(), 5u);
        EXPECT_EQ(t[0].val(), csubstr("literal\n"));
        EXPECT_EQ(t[1].val(), csubstr(" folded\n"));
        EXPECT_EQ(t[2].val(), csubstr("keep\n\n"));
        EXPECT_EQ(t[3].val(), csubstr(" strip"));
        EXPECT_EQ(t[4].val(), csubstr(" strip"));
    });
}


TEST(block_folded, test_suite_R4YG)
{
    csubstr yaml = R"(
- |
 detected0
- >
 
  
  # detected1
- |1
  explicit2
- >
 	
 detected3
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_seq());
        ASSERT_EQ(t.rootref().num_children(), 4u);
        EXPECT_EQ(t[0].val(), csubstr("detected0\n"));
        EXPECT_EQ(t[1].val(), csubstr("\n\n# detected1\n"));
        EXPECT_EQ(t[2].val(), csubstr(" explicit2\n"));
        EXPECT_EQ(t[3].val(), csubstr("\t\ndetected3\n"));
    });
}


TEST(block_folded, test_suite_T26H)
{
    csubstr yaml = R"(
--- |
 
  
  literal
   
  
  text

 # Comment
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_stream());
        ASSERT_TRUE(t.rootref().first_child().is_doc());
        EXPECT_EQ(t.rootref().first_child().val(), csubstr("\n\nliteral\n \n\ntext\n"));
    });
}


TEST(block_folded, test_suite_T5N4)
{
    csubstr yaml = R"(
--- |
 literal
 	text


)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_stream());
        ASSERT_TRUE(t.rootref().first_child().is_doc());
        EXPECT_EQ(t.rootref().first_child().val(), csubstr("literal\n\ttext\n"));
    });
}


TEST(block_folded, test_suite_W4TN)
{
    csubstr yaml = R"(
--- |
 %!PS-Adobe-2.0
...
--- >
 %!PS-Adobe-2.0
...
--- |
%!PS-Adobe-2.0
...
--- >
%!PS-Adobe-2.0
...
---
# Empty
...
--- |
 %!PS-Adobe-2.0
--- >
 %!PS-Adobe-2.0
--- |
%!PS-Adobe-2.0
--- >
%!PS-Adobe-2.0
---
# empty
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ConstNodeRef r = t.rootref();
        ASSERT_TRUE(r.is_stream());
        ASSERT_EQ(r.num_children(), 10u);
        ASSERT_TRUE(r.doc(0).is_doc());
        ASSERT_TRUE(r.doc(0).is_val());
        EXPECT_EQ(r.doc(0).val(), csubstr("%!PS-Adobe-2.0\n"));
        ASSERT_TRUE(r.doc(1).is_doc());
        ASSERT_TRUE(r.doc(1).is_val());
        EXPECT_EQ(r.doc(1).val(), csubstr("%!PS-Adobe-2.0\n"));
        ASSERT_TRUE(r.doc(2).is_doc());
        ASSERT_TRUE(r.doc(2).is_val());
        EXPECT_EQ(r.doc(2).val(), csubstr("%!PS-Adobe-2.0\n"));
        ASSERT_TRUE(r.doc(3).is_doc());
        ASSERT_TRUE(r.doc(3).is_val());
        EXPECT_EQ(r.doc(3).val(), csubstr("%!PS-Adobe-2.0\n"));
        ASSERT_TRUE(r.doc(4).is_doc());
        ASSERT_TRUE(r.doc(4).is_val());
        EXPECT_EQ(r.doc(4).val(), csubstr{});
        ASSERT_TRUE(r.doc(5).is_doc());
        ASSERT_TRUE(r.doc(5).is_val());
        EXPECT_EQ(r.doc(5).val(), csubstr("%!PS-Adobe-2.0\n"));
        ASSERT_TRUE(r.doc(6).is_doc());
        ASSERT_TRUE(r.doc(6).is_val());
        EXPECT_EQ(r.doc(6).val(), csubstr("%!PS-Adobe-2.0\n"));
        ASSERT_TRUE(r.doc(7).is_doc());
        ASSERT_TRUE(r.doc(7).is_val());
        EXPECT_EQ(r.doc(7).val(), csubstr("%!PS-Adobe-2.0\n"));
        ASSERT_TRUE(r.doc(8).is_doc());
        ASSERT_TRUE(r.doc(8).is_val());
        EXPECT_EQ(r.doc(8).val(), csubstr("%!PS-Adobe-2.0\n"));
        ASSERT_TRUE(r.doc(4).is_doc());
        ASSERT_TRUE(r.doc(4).is_val());
        EXPECT_EQ(r.doc(4).val(), csubstr{});
    });
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


CASE_GROUP(BLOCK_FOLDED)
{
//
ADD_CASE_TO_GROUP("indentation requirements",
R"(---
>
hello
there
---
>
 hello
 there
---
>
  hello
  there
---
>
ciao
qua
---
>
    ciao
    qua
---
>
      ciao
      qua
---
- >
 hello
 there
- >
 ciao
 qua
---
foo: >
 hello
 there
bar: >
 ciao
 qua
)",
N(STREAM, L{
        N(DOCVAL|QV, "hello there\n"),
        N(DOCVAL|QV, "hello there\n"),
        N(DOCVAL|QV, "hello there\n"),
        N(DOCVAL|QV, "ciao qua\n"),
        N(DOCVAL|QV, "ciao qua\n"),
        N(DOCVAL|QV, "ciao qua\n"),
        N(SEQ|DOC, L{N(QV, "hello there\n"), N(QV, "ciao qua\n")}),
        N(MAP|DOC, L{N(QV, "foo", "hello there\n"), N(QV, "bar", "ciao qua\n")}),
    }));

ADD_CASE_TO_GROUP("indentation requirements err seq", EXPECT_PARSE_ERROR,
R"(- >
hello
there
- >
ciao
qua
)",
N(L{N(QV, "hello there"), N(QV, "ciao qua\n")}));

ADD_CASE_TO_GROUP("indentation requirements err map", EXPECT_PARSE_ERROR,
R"(foo: >
hello
there
bar: >
ciao
qua
)",
N(L{N(QV, "foo", "hello there\n"), N(QV, "bar" "ciao qua\n")}));

ADD_CASE_TO_GROUP("indentation requirements err level", EXPECT_PARSE_ERROR,
R"(--- >2
 hello
 there
)",
N(NOTYPE));

ADD_CASE_TO_GROUP("foo without space after",
R"(>
  foo
)",
N(DOCVAL|QV, "foo\n"));

ADD_CASE_TO_GROUP("foo with space after",
R"(>
  foo
  
)",
N(DOCVAL|QV, "foo\n"));

ADD_CASE_TO_GROUP("simple with indents",
R"(>
  foo
     
    bar
)",
N(DOCVAL|QV, "foo\n   \n  bar\n"));


ADD_CASE_TO_GROUP("7T8X",
R"(- >
    
    folded
    line
    
    next
    line
      * bullet
    
      * list
      * lines
    
    last
    line

# Comment

##### this is the original scalar:
- >

 folded
 line

 next
 line
   * bullet

   * list
   * lines

 last
 line

# Comment

##### without any indentation
- >

    folded
    line

    next
    line
      * bullet

      * list
      * lines

    last
    line

# Comment
)",
  L{
    N(QV, "\nfolded line\nnext line\n  * bullet\n\n  * list\n  * lines\n\nlast line\n"),
    N(QV, "\nfolded line\nnext line\n  * bullet\n\n  * list\n  * lines\n\nlast line\n"),
    N(QV, "\nfolded line\nnext line\n  * bullet\n\n  * list\n  * lines\n\nlast line\n"),
  }
);


ADD_CASE_TO_GROUP("block folded as seq val, implicit indentation 2",
R"(
- >
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:

  plus another line at the end.


- another val
)",
  L{
    N(QV, "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N("another val")
  }
);

ADD_CASE_TO_GROUP("block folded as map val, implicit indentation 2",
R"(
example: >
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
another: val
)",
  L{
    N(QV, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N("another", "val")
  }
);

ADD_CASE_TO_GROUP("block folded as map val, implicit indentation 2, chomp=keep",
R"(
example: >+
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
another: val
)",
  L{
    N(QV, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n\n\n"),
    N("another", "val")
  }
);

ADD_CASE_TO_GROUP("block folded as map val, implicit indentation 2, chomp=strip",
R"(
example: >-
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
another: val
)",
  L{
    N(QV, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end."),
    N("another", "val")
  }
);

ADD_CASE_TO_GROUP("block folded as map val, explicit indentation 2",
R"(
example: >2
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
another: val
)",
  L{
    N(QV, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N("another", "val")
  }
);

ADD_CASE_TO_GROUP("block folded as map val, explicit indentation 2, chomp=keep",
R"(
example: >+2
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
example2: >2+
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
)",
  L{
    N(QV, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n\n\n"),
    N(QV, "example2", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n\n\n"),
  }
);

ADD_CASE_TO_GROUP("block folded as map val, explicit indentation 2, chomp=strip",
R"(
example: >-2
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
example2: >2-
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
)",
  L{
    N(QV, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end."),
    N(QV, "example2", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end."),
  }
);

ADD_CASE_TO_GROUP("block folded as map val, implicit indentation 3",
R"(
example: >
   Several lines of text,
   with some "quotes" of various 'types',
   and also a blank line:
   
   plus another line at the end.
   
   
another: val
)",
  L{
    N(QV, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N("another", "val")
  }
);

ADD_CASE_TO_GROUP("block folded as map val, explicit indentation 3",
R"(
example: >3
   Several lines of text,
   with some "quotes" of various 'types',
   and also a blank line:
   
   plus another line at the end.
   
   
another: val
)",
  L{
    N(QV, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N("another", "val")
  }
);

ADD_CASE_TO_GROUP("block folded as map val, implicit indentation 4",
R"(
example: >
    Several lines of text,
    with some "quotes" of various 'types',
    and also a blank line:
    
    plus another line at the end.
    
    
another: val
)",
  L{
    N(QV, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N("another", "val")
  }
);

ADD_CASE_TO_GROUP("block folded as map val, explicit indentation 4",
R"(
example: >4
    Several lines of text,
    with some "quotes" of various 'types',
    and also a blank line:
    
    plus another line at the end.
    
    
another: val
)",
  L{
    N(QV, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N("another", "val")
  }
);

ADD_CASE_TO_GROUP("block folded as map val, implicit indentation 9",
R"(
example: >
         Several lines of text,
         with some "quotes" of various 'types',
         and also a blank line:
         
         plus another line at the end.
         
         
another: val
)",
  L{
    N(QV, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N("another", "val")
  }
);

ADD_CASE_TO_GROUP("block folded as map val, explicit indentation 9",
R"(
example: >9
         Several lines of text,
         with some "quotes" of various 'types',
         and also a blank line:
         
         plus another line at the end.
         
         
another: val
)",
  L{
    N(QV, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N("another", "val")
  }
);


ADD_CASE_TO_GROUP("block folded as map entry",
R"(
data: >
   Wrapped text
   will be folded
   into a single
   paragraph

   Blank lines denote
   paragraph breaks
)",
  N(L{N(KEYVAL|VALQUO, "data", "Wrapped text will be folded into a single paragraph\nBlank lines denote paragraph breaks\n")})
);

ADD_CASE_TO_GROUP("block folded, no chomp, no indentation",
R"(example: >
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:

  plus another line at the end.

another: text
)",
  N(L{
      N(KEYVAL|VALQUO, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
      N("another", "text"),
      })
);

ADD_CASE_TO_GROUP("block folded with empty docval 1",
R"(>)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block folded with empty docval 2",
R"(>
)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block folded with empty docval 3",
R"(>
  )",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block folded with empty docval 4",
R"(>
  
)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block folded with empty docval 5",
R"(>
    
)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block folded with empty docval 6",
R"(>
       	  )",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block folded with empty docval 7",
R"(>
       	  
)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block folded with empty docval 8",
R"(>


)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block folded with empty docval 9",
R"(>



)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block folded with empty docval 10",
R"(>




)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block folded with empty docval 11",
R"(>
 
  
   
    )",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block folded with empty docval 12",
R"(>
 
  
   
    
     
      
    
   
 
)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block folded with empty docval 13",
R"(>
 
  

   

    

)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 0",
R"(>
  asd)",
  N(DOCVAL|VALQUO, "asd\n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 1",
R"(>
  asd
)",
  N(DOCVAL|VALQUO, "asd\n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 2",
R"(>
  asd

)",
  N(DOCVAL|VALQUO, "asd\n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 3",
R"(>
  asd
  )",
  N(DOCVAL|VALQUO, "asd\n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 4",
R"(>
  asd
  
  )",
  N(DOCVAL|VALQUO, "asd\n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 5",
R"(>
     asd
   
  )",
  N(DOCVAL|VALQUO, "asd\n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 5.1",
R"(>
       asd
     
   
     
  
 
  )",
  N(DOCVAL|VALQUO, "asd\n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 5.2",
R"(>
       asd
     
   
       
  
 
  )",
  N(DOCVAL|VALQUO, "asd\n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 5.3",
R"(>
       asd
     
   
        
  
 
  )",
  N(DOCVAL|VALQUO, "asd\n\n\n \n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 6",
R"(>
     asd
      )",
  N(DOCVAL|VALQUO, "asd\n \n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 7",
R"(>
     asd
      
)",
  N(DOCVAL|VALQUO, "asd\n \n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 8",
R"(>
     asd
       )",
  N(DOCVAL|VALQUO, "asd\n  \n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 9",
R"(>
     asd
       
)",
  N(DOCVAL|VALQUO, "asd\n  \n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 10",
R"(>
     asd
     	 )",
  N(DOCVAL|VALQUO, "asd\n\t \n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 11",
R"(>
     asd
      	 )",
  N(DOCVAL|VALQUO, "asd\n \t \n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 12",
R"(>
     asd
     	 
)",
  N(DOCVAL|VALQUO, "asd\n\t \n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 13",
R"(>
     asd
      	 
)",
  N(DOCVAL|VALQUO, "asd\n \t \n")
    );


ADD_CASE_TO_GROUP("block folded, keep, empty docval trailing 0",
R"(>+)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block folded, keep, empty docval trailing 1",
R"(>+
)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block folded, keep, empty docval trailing 1.1",
R"(>+
  )",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block folded, keep, empty docval trailing 1.2",
R"(>+
  asd)",
  N(DOCVAL|VALQUO, "asd")
    );

ADD_CASE_TO_GROUP("block folded, keep, empty docval trailing 1.3",
R"(>+
  asd
)",
  N(DOCVAL|VALQUO, "asd\n")
    );

ADD_CASE_TO_GROUP("block folded, keep, empty docval trailing 1.4",
R"(>+
  asd
  
)",
  N(DOCVAL|VALQUO, "asd\n\n")
    );

ADD_CASE_TO_GROUP("block folded, keep, empty docval trailing 2",
R"(>+

)",
  N(DOCVAL|VALQUO, "\n")
    );

ADD_CASE_TO_GROUP("block folded, keep, empty docval trailing 2.1",
R"(>+

  )",
  N(DOCVAL|VALQUO, "\n")
    );

ADD_CASE_TO_GROUP("block folded, keep, empty docval trailing 3",
R"(>+


)",
  N(DOCVAL|VALQUO, "\n\n")
    );

ADD_CASE_TO_GROUP("block folded, keep, empty docval trailing 4",
R"(>+



)",
  N(DOCVAL|VALQUO, "\n\n\n")
    );

ADD_CASE_TO_GROUP("block folded, keep, empty docval trailing 5",
R"(>+




)",
  N(DOCVAL|VALQUO, "\n\n\n\n")
    );

ADD_CASE_TO_GROUP("block folded, empty block vals in seq 0",
R"(- >+
  
- >+
  )",
N(L{N(QV, "\n"), N(QV, ""),}));

ADD_CASE_TO_GROUP("block folded, empty block vals in seq 1",
R"(- >+
  
- >+
  
)",
N(L{N(QV, "\n"), N(QV, "\n"),}));

}

} // namespace yml
} // namespace c4
