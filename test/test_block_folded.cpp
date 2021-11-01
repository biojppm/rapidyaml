#include "./test_group.hpp"

namespace c4 {
namespace yml {

TEST(block_folded, issue152_not_indented)
{
    const Tree t = parse(R"(
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
    const Tree t = parse(R"(
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
    const NodeRef n = t["indented_once"];
    EXPECT_EQ(n["ok"  ][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432\n"));
    EXPECT_EQ(n["err" ][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432\n"));
    EXPECT_EQ(n["err2"][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432\n"));
    EXPECT_EQ(n["err3"][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432"));
}

TEST(block_folded, issue152_indented_twice)
{
    const Tree t = parse(R"(
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
    const NodeRef n = t["indented_once"]["indented_twice"];
    EXPECT_EQ(n["ok"  ][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432\n"));
    EXPECT_EQ(n["err" ][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432\n"));
    EXPECT_EQ(n["err2"][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432\n"));
    EXPECT_EQ(n["err3"][0].val(), csubstr("exec pg_isready -U \"dog\" -d \"dbname=dog\" -h 127.0.0.1 -p 5432"));
}

TEST(block_folded, issue152_indented_thrice)
{
    const Tree t = parse(R"(
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
    const NodeRef n = t["indented_once"]["indented_twice"]["indented_thrice"];
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
    yaml = R"(---
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
  - |3
    child2
  - >
   child3
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_stream());
        NodeRef doc = t.rootref().child(0);
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
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_EQ(t[0].val(), csubstr("Sammy Sosa completed another fine season with great stats.\n\n  63 Home Runs\n  0.288 Batting Average\n\nWhat a year!\n"));
        EXPECT_EQ(t[1].val(), csubstr("Sammy Sosa completed another fine season with great stats.\n\n\n  63 Home Runs\n  0.288 Batting Average\n\n\nWhat a year!\n"));
        EXPECT_EQ(t[2].val(), csubstr("Sammy Sosa completed another fine season with great stats.\n\n\n\n  63 Home Runs\n  0.288 Batting Average\n\n\n\nWhat a year!\n"));
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
    Tree t = parse(yaml);
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
  folded text
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_TRUE(t.rootref().is_stream());
        const NodeRef doc = t.rootref().first_child();
        ASSERT_TRUE(doc.is_doc());
        ASSERT_TRUE(doc.is_val());
        EXPECT_EQ(doc.val(), csubstr("folded text\n"));
        const NodeRef doc2 = t.rootref().last_child();
        ASSERT_TRUE(doc2.is_doc());
        ASSERT_TRUE(doc2.is_val());
        EXPECT_EQ(doc2.val(), csubstr("folded text\n"));
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

#ifdef TEST_SUITE_WIP
TEST(block_folded, test_suite_K858)
{
    csubstr yaml = R"(
strip: >-

clip: >

keep: |+

)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().has_child("strip"));
        ASSERT_TRUE(t.rootref().has_child("keep"));
        ASSERT_TRUE(t.rootref().has_child("clip"));
        EXPECT_EQ(t["strip"].val(), csubstr{});
        EXPECT_EQ(t["clip"].val(), csubstr{});
        EXPECT_EQ(t["keep"].val(), csubstr("\n"));
    });
}
#endif


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
 detected
- >
 
  
  # detected
- |1
  explicit
- >
 	
 detected
)";
    test_check_emit_check(yaml, [](Tree const &t){
        ASSERT_TRUE(t.rootref().is_seq());
        ASSERT_EQ(t.rootref().num_children(), 4u);
        EXPECT_EQ(t[0].val(), csubstr("detected\n"));
        EXPECT_EQ(t[1].val(), csubstr("\n\n# detected\n"));
        EXPECT_EQ(t[2].val(), csubstr(" explicit\n"));
        EXPECT_EQ(t[3].val(), csubstr("\t\ndetected\n"));
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
        NodeRef const r = t.rootref();
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

#define BLOCK_FOLDED_CASES \
    "7T8X",                                            \
    "block folded as seq val, implicit indentation 2", \
    "block folded as map val, implicit indentation 2",\
    "block folded as map val, implicit indentation 2, chomp=keep",\
    "block folded as map val, implicit indentation 2, chomp=strip",\
    "block folded as map val, implicit indentation 3",\
    "block folded as map val, implicit indentation 4",\
    "block folded as map val, implicit indentation 9",\
    "block folded as map val, explicit indentation 2",\
    "block folded as map val, explicit indentation 2, chomp=keep",\
    "block folded as map val, explicit indentation 2, chomp=strip",\
    "block folded as map val, explicit indentation 3",\
    "block folded as map val, explicit indentation 4",\
    "block folded as map val, explicit indentation 9"


CASE_GROUP(BLOCK_FOLDED)
{
    APPEND_CASES(

C("7T8X",
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

)",
  L{
    N(QV, "\nfolded line\nnext line\n  * bullet\n\n  * list\n  * lines\n\nlast line\n"),
    N(QV, "\nfolded line\nnext line\n  * bullet\n\n  * list\n  * lines\n\nlast line\n"),
  }
),


C("block folded as seq val, implicit indentation 2",
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
),

C("block folded as map val, implicit indentation 2",
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
),

C("block folded as map val, implicit indentation 2, chomp=keep",
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
),

C("block folded as map val, implicit indentation 2, chomp=strip",
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
),

C("block folded as map val, explicit indentation 2",
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
),

C("block folded as map val, explicit indentation 2, chomp=keep",
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
),

C("block folded as map val, explicit indentation 2, chomp=strip",
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
),

C("block folded as map val, implicit indentation 3",
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
),

C("block folded as map val, explicit indentation 3",
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
),

C("block folded as map val, implicit indentation 4",
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
),

C("block folded as map val, explicit indentation 4",
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
),

C("block folded as map val, implicit indentation 9",
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
),

C("block folded as map val, explicit indentation 9",
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
),
    )
}

INSTANTIATE_GROUP(BLOCK_FOLDED)

} // namespace yml
} // namespace c4
