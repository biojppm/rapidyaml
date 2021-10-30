#include "./test_group.hpp"

namespace c4 {
namespace yml {


TEST(block_literal, emit_does_not_add_lines_to_multi_at_end)
{
    Tree t = parse("[]");
    NodeRef r = t.rootref();
    r.append_child() = "\n\n";
    r.append_child() = "\n\n";
    r.append_child() = "last";
    std::string out = emitrs<std::string>(t);
    std::string expected = R"(- |+
  
  
- |+
  
  
- last
)";
    EXPECT_EQ(out, expected);
    t = parse(to_csubstr(out));
    EXPECT_EQ(t[0].val(), csubstr("\n\n"));
    EXPECT_EQ(t[1].val(), csubstr("\n\n"));
    EXPECT_EQ(t[2].val(), csubstr("last"));

    ASSERT_EQ(csubstr("ab\n\n \n").trimr(" \t\n"), csubstr("ab"));

    t = parse(R"(--- |+
 ab
 
  
)");
    EXPECT_EQ(t.docref(0).val(), csubstr("ab\n\n \n"));
    expected = R"(--- |
  ab
  
   

)";
    out = emitrs<std::string>(t);
    EXPECT_EQ(out, expected);
    t = parse(to_csubstr(out));
    EXPECT_EQ(t.docref(0).val(), csubstr("ab\n\n \n"));
    out = emitrs<std::string>(t);
    EXPECT_EQ(out, expected);
    t = parse(to_csubstr(out));
    EXPECT_EQ(t.docref(0).val(), csubstr("ab\n\n \n"));
    out = emitrs<std::string>(t);
    EXPECT_EQ(out, expected);
    t = parse(to_csubstr(out));
    EXPECT_EQ(t.docref(0).val(), csubstr("ab\n\n \n"));

    std::string yaml = R"(
- |
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:

  plus another line at the end.



- |+
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:

  plus another line at the end.

- last
)";
    expected = R"(- |
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.

- |+
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
- last
)";
    t = parse(to_csubstr(yaml));
    EXPECT_EQ(t[0].val(), "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n");
    EXPECT_EQ(t[1].val(), "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n\n");
    out = emitrs<std::string>(t);
    EXPECT_EQ(out, expected);
    t = parse(to_csubstr(out));
    EXPECT_EQ(t[0].val(), "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n");
    EXPECT_EQ(t[1].val(), "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n\n");
    out = emitrs<std::string>(t);
    EXPECT_EQ(out, expected);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#define BLOCK_LITERAL_CASES \
    "block literal as seq val, implicit indentation 2",\
    "block literal as seq val, implicit indentation 2, chomp=keep",\
    "block literal as seq val, implicit indentation 2, chomp=strip",\
    "block literal as seq val at eof, implicit indentation 2",\
    "block literal as seq val at eof, implicit indentation 4",\
    "block literal as map val, implicit indentation 2",\
    "block literal as map val, implicit indentation 3",\
    "block literal as map val, implicit indentation 4",\
    "block literal as map val, implicit indentation 9",\
    "block literal as map val, explicit indentation 2",\
    "block literal as map val, explicit indentation 2, chomp=keep",\
    "block literal as map val, explicit indentation 2, chomp=strip",\
    "block literal as map val at eof, implicit indentation 2",\
    "block literal as map val at eof, implicit indentation 4",\
    "block literal as map val, explicit indentation 3",\
    "block literal as map val, explicit indentation 4",\
    "block literal as map val, explicit indentation 9",\
    "block literal with empty unindented lines, without quotes",\
    "block literal with empty unindented lines, with double quotes",\
    "block literal with empty unindented lines, with single quotes"


CASE_GROUP(BLOCK_LITERAL)
{
    APPEND_CASES(

C("block literal as seq val, implicit indentation 2",
R"(
- |
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
- another val
)",
  L{
    N(QV, "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N("another val")
  }
),

C("block literal as seq val, implicit indentation 2, chomp=keep",
R"(
- |+
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
- another val
)",
  L{
    N(QV, "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n\n\n"),
    N("another val")
  }
),

C("block literal as seq val, implicit indentation 2, chomp=strip",
R"(
- |-
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
- another val
)",
  L{
    N(QV, "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end."),
    N("another val")
  }
),

C("block literal as seq val at eof, implicit indentation 2",
R"(
- |
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
)",
  L{
    N(QV, "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
  }
),

C("block literal as seq val at eof, implicit indentation 4",
R"(
- |
    Several lines of text,
    with some "quotes" of various 'types',
    and also a blank line:
    
    plus another line at the end.
    
    
)",
  L{
    N(QV, "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
  }
),

C("block literal as map val, implicit indentation 2",
R"(
example: |
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
another: val
)",
  L{
    N(QV, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N("another", "val")
  }
),

C("block literal as map val, explicit indentation 2",
R"(
example: |2
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
another: val
)",
  L{
    N(QV, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N("another", "val")
  }
),

C("block literal as map val, explicit indentation 2, chomp=keep",
R"(
example: |+2
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
another: val
)",
  L{
    N(QV, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n\n\n"),
    N("another", "val")
  }
),

C("block literal as map val, explicit indentation 2, chomp=strip",
R"(
example: |-2
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
another: val
)",
  L{
    N(QV, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end."),
    N("another", "val")
  }
),

C("block literal as map val, implicit indentation 3",
R"(
example: |
   Several lines of text,
   with some "quotes" of various 'types',
   and also a blank line:
   
   plus another line at the end.
   
   
another: val
)",
  L{
    N(QV, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N("another", "val")
  }
),

C("block literal as map val, explicit indentation 3",
R"(
example: |3
   Several lines of text,
   with some "quotes" of various 'types',
   and also a blank line:
   
   plus another line at the end.
   
   
another: val
)",
  L{
    N(QV, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N("another", "val")
  }
),

C("block literal as map val, implicit indentation 4",
R"(
example: |
    Several lines of text,
    with some "quotes" of various 'types',
    and also a blank line:
    
    plus another line at the end.
    
    
another: val
)",
  L{
    N(QV, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N("another", "val")
  }
),

C("block literal as map val, explicit indentation 4",
R"(
example: |4
    Several lines of text,
    with some "quotes" of various 'types',
    and also a blank line:
    
    plus another line at the end.
    
    
another: val
)",
  L{
    N(QV, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N("another", "val")
  }
),

C("block literal as map val at eof, implicit indentation 2",
R"(
example: |
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
)",
  L{
    N(QV, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
  }
),

C("block literal as map val at eof, implicit indentation 4",
R"(
example: |
    Several lines of text,
    with some "quotes" of various 'types',
    and also a blank line:
    
    plus another line at the end.
    
    
)",
  L{
    N(QV, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
  }
),

C("block literal as map val, implicit indentation 9",
R"(
example: |
         Several lines of text,
         with some "quotes" of various 'types',
         and also a blank line:
         
         plus another line at the end.
         
         
another: val
)",
  L{
    N(QV, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N("another", "val")
  }
),

C("block literal as map val, explicit indentation 9",
R"(
example: |9
         Several lines of text,
         with some "quotes" of various 'types',
         and also a blank line:
         
         plus another line at the end.
         
         
another: val
)",
  L{
    N(QV, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N("another", "val")
  }
),

C("block literal with empty unindented lines, without quotes",
    R"(tpl:
  src: |
    #include <{{hdr.filename}}>
  
    {{src.gencode}}
)",
  L{
    N("tpl", L{N(QV, "src", "#include <{{hdr.filename}}>\n\n{{src.gencode}}\n")})
  }
),

C("block literal with empty unindented lines, with double quotes",
    R"(tpl:
  src: |
    #include "{{hdr.filename}}"
  
    {{src.gencode}}
)",
  L{
    N("tpl", L{N(QV, "src", "#include \"{{hdr.filename}}\"\n\n{{src.gencode}}\n")})
  }
),

C("block literal with empty unindented lines, with single quotes",
    R"(tpl:
  src: |
    #include '{{hdr.filename}}'
  
    {{src.gencode}}
)",
  L{
    N("tpl", L{N(QV, "src", "#include '{{hdr.filename}}'\n\n{{src.gencode}}\n")})
  }
),
    )
}


INSTANTIATE_GROUP(BLOCK_LITERAL)

} // namespace yml
} // namespace c4
