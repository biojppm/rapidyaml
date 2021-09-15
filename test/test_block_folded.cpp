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
##- >
##
## folded
## line
##
## next
## line
##   * bullet
##
##   * list
##   * lines
##
## last
## line
##
### Comment
##
)",
  L{
    N(QV, "\nfolded line\nnext line\n  * bullet\n\n  * list\n  * lines\n\nlast line\n"),
    //N(QV, "\nfolded line\nnext line\n  * bullet\n\n  * list\n  * lines\n\nlast line\n"),
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
  
  
another: val
)",
  L{
    N(QV, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n\n\n"),
    N("another", "val")
  }
),

C("block folded as map val, explicit indentation 2, chomp=strip",
R"(
example: >-2
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
