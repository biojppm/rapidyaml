#include "./test_group.hpp"

namespace c4 {
namespace yml {

TEST(block_literal, empty_block)
{
    {
        Tree t = parse_in_arena(R"(- |
)");
        EXPECT_EQ(t[0].val(), csubstr(""));
    }
    {
        Tree t = parse_in_arena(R"(- |-
)");
        EXPECT_EQ(t[0].val(), csubstr(""));
    }
    {
        Tree t = parse_in_arena(R"(- |+
)");
        EXPECT_EQ(t[0].val(), csubstr(""));
    }
    {
        Tree t = parse_in_arena(R"(# no indentation: fails!
- |

- |-

- |+

)");
        EXPECT_FALSE(t.empty());
        // FAILS! EXPECT_EQ(t[0].val(), csubstr(""));
        // FAILS! EXPECT_EQ(t[1].val(), csubstr(""));
        // FAILS! EXPECT_EQ(t[2].val(), csubstr(""));
    }
    {
        Tree t = parse_in_arena(R"(# fails!
- |
  
- |-
  
- |+
  
)");
        EXPECT_FALSE(t.empty());
        // FAILS! EXPECT_EQ(t[0].val(), csubstr(" \n"));
        // FAILS! EXPECT_EQ(t[1].val(), csubstr(" "));
        // FAILS! EXPECT_EQ(t[2].val(), csubstr("\n"));
    }
    {
        Tree t = parse_in_arena(R"(# fails!
- |
- |-
- |+
)");
        EXPECT_FALSE(t.empty());
        // FAILS! EXPECT_EQ(t[0].val(), csubstr(""));
        // FAILS! EXPECT_EQ(t[1].val(), csubstr(""));
        // FAILS! EXPECT_EQ(t[2].val(), csubstr(""));
    }
}


TEST(block_literal, emit_does_not_add_lines_to_multi_at_end_1)
{
    Tree t = parse_in_arena("[]");
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
    t = parse_in_arena(to_csubstr(out));
    EXPECT_EQ(t[0].val(), csubstr("\n\n"));
    EXPECT_EQ(t[1].val(), csubstr("\n\n"));
    EXPECT_EQ(t[2].val(), csubstr("last"));
    EXPECT_EQ(csubstr("ab\n\n \n").trimr(" \t\n"), csubstr("ab"));
}

TEST(block_literal, emit_does_not_add_lines_to_multi_at_end_2)
{
    Tree t = parse_in_arena(R"(--- |+
 ab
 
  
)");
    EXPECT_EQ(t.docref(0).val(), csubstr("ab\n\n \n"));
    std::string expected = R"(--- |
  ab
  
   

)";
    std::string out = emitrs<std::string>(t);
    EXPECT_EQ(out, expected);
    t = parse_in_arena(to_csubstr(out));
    EXPECT_EQ(t.docref(0).val(), csubstr("ab\n\n \n"));
    out = emitrs<std::string>(t);
    EXPECT_EQ(out, expected);
    t = parse_in_arena(to_csubstr(out));
    EXPECT_EQ(t.docref(0).val(), csubstr("ab\n\n \n"));
    out = emitrs<std::string>(t);
    EXPECT_EQ(out, expected);
    t = parse_in_arena(to_csubstr(out));
    EXPECT_EQ(t.docref(0).val(), csubstr("ab\n\n \n"));
}

TEST(block_literal, emit_does_not_add_lines_to_multi_at_end_3)
{
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
    std::string expected = R"(- |
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
    Tree t = parse_in_arena(to_csubstr(yaml));
    EXPECT_EQ(t[0].val(), "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n");
    EXPECT_EQ(t[1].val(), "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n\n");
    std::string out = emitrs<std::string>(t);
    EXPECT_EQ(out, expected);
    t = parse_in_arena(to_csubstr(out));
    EXPECT_EQ(t[0].val(), "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n");
    EXPECT_EQ(t[1].val(), "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n\n");
    out = emitrs<std::string>(t);
    EXPECT_EQ(out, expected);
}

TEST(block_literal, carriage_return)
{
    std::string yaml = "with: |\r\n"
"  text\r\n"
"   	lines\r\n"
"without: |\n"
"  text\n"
"   	lines\n";
    Tree t = parse_in_arena(to_csubstr(yaml));
    EXPECT_EQ(t["with"].val(), "text\n \tlines\n");
    EXPECT_EQ(t["without"].val(), "text\n \tlines\n");
    auto emitted = emitrs<std::string>(t);
    #ifdef RYML_DBG
    __c4presc(emitted.data(), emitted.size());
    #endif
    Tree r = parse_in_arena(to_csubstr(emitted));
    EXPECT_EQ(t["with"].val(), "text\n \tlines\n");
    EXPECT_EQ(t["without"].val(), "text\n \tlines\n");
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


CASE_GROUP(BLOCK_LITERAL)
{

ADD_CASE_TO_GROUP("block literal as seq val, implicit indentation 2",
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
);

ADD_CASE_TO_GROUP("block literal as seq val, implicit indentation 2, chomp=keep",
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
);

ADD_CASE_TO_GROUP("block literal as seq val, implicit indentation 2, chomp=strip",
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
);

ADD_CASE_TO_GROUP("block literal as seq val at eof, implicit indentation 2",
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
);

ADD_CASE_TO_GROUP("block literal as seq val at eof, implicit indentation 4",
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
);

ADD_CASE_TO_GROUP("block literal as map val, implicit indentation 2",
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
);

ADD_CASE_TO_GROUP("block literal as map val, explicit indentation 2",
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
);

ADD_CASE_TO_GROUP("block literal as map val, explicit indentation 2, chomp=keep",
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
);

ADD_CASE_TO_GROUP("block literal as map val, explicit indentation 2, chomp=strip",
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
);

ADD_CASE_TO_GROUP("block literal as map val, implicit indentation 3",
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
);

ADD_CASE_TO_GROUP("block literal as map val, explicit indentation 3",
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
);

ADD_CASE_TO_GROUP("block literal as map val, implicit indentation 4",
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
);

ADD_CASE_TO_GROUP("block literal as map val, explicit indentation 4",
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
);

ADD_CASE_TO_GROUP("block literal as map val at eof, implicit indentation 2",
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
);

ADD_CASE_TO_GROUP("block literal as map val at eof, implicit indentation 4",
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
);

ADD_CASE_TO_GROUP("block literal as map val, implicit indentation 9",
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
);

ADD_CASE_TO_GROUP("block literal as map val, explicit indentation 9",
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
);

ADD_CASE_TO_GROUP("block literal with empty unindented lines, without quotes",
    R"(tpl:
  src: |
    #include <{{hdr.filename}}>
  
    {{src.gencode}}
)",
  L{
    N("tpl", L{N(QV, "src", "#include <{{hdr.filename}}>\n\n{{src.gencode}}\n")})
  }
);

ADD_CASE_TO_GROUP("block literal with empty unindented lines, with double quotes",
    R"(tpl:
  src: |
    #include "{{hdr.filename}}"
  
    {{src.gencode}}
)",
  L{
    N("tpl", L{N(QV, "src", "#include \"{{hdr.filename}}\"\n\n{{src.gencode}}\n")})
  }
);

ADD_CASE_TO_GROUP("block literal with empty unindented lines, with single quotes",
    R"(tpl:
  src: |
    #include '{{hdr.filename}}'
  
    {{src.gencode}}
)",
  L{
    N("tpl", L{N(QV, "src", "#include '{{hdr.filename}}'\n\n{{src.gencode}}\n")})
  }
);

ADD_CASE_TO_GROUP("block literal with same indentation level 0",
R"(
aaa: |2
  xxx
bbb: |
  yyy
)",
  L{N(QV, "aaa", "xxx\n"), N(QV, "bbb", "yyy\n")}
    );

ADD_CASE_TO_GROUP("block literal with same indentation level 1",
R"(
- aaa: |2
    xxx
  bbb: |
    yyy
)",
  L{N(L{N(QV, "aaa", "xxx\n"), N(QV, "bbb", "yyy\n")})}
    );

ADD_CASE_TO_GROUP("block literal with empty docval 1",
R"(|)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 2",
R"(|
)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 3",
R"(|
  )",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 4",
R"(|
  
)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 5",
R"(|
    
)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 6",
R"(|
       	  )",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 7",
R"(|
       	  
)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 8",
R"(|


)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 9",
R"(|



)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 10",
R"(|




)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 10",
R"(|
 
  
   
    )",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 11",
R"(|
 
  
   
    
)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 12",
R"(|
 
  

   

    

)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 0",
R"(|
  asd)",
  N(DOCVAL|VALQUO, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 1",
R"(|
  asd
)",
  N(DOCVAL|VALQUO, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 2",
R"(|
  asd

)",
  N(DOCVAL|VALQUO, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 3",
R"(|
  asd
  )",
  N(DOCVAL|VALQUO, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 4",
R"(|
  asd
  
  )",
  N(DOCVAL|VALQUO, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 5",
R"(|
     asd
   
  )",
  N(DOCVAL|VALQUO, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 6",
R"(|
     asd
      )",
  N(DOCVAL|VALQUO, "asd\n \n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 7",
R"(|
     asd
      
)",
  N(DOCVAL|VALQUO, "asd\n \n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 8",
R"(|
     asd
       )",
  N(DOCVAL|VALQUO, "asd\n  \n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 9",
R"(|
     asd
       
)",
  N(DOCVAL|VALQUO, "asd\n  \n")
    );

ADD_CASE_TO_GROUP("block literal as map entry",
R"(
data: |
   There once was a short man from Ealing
   Who got on a bus to Darjeeling
       It said on the door
       "Please don't spit on the floor"
   So he carefully spat on the ceiling
)",
  N(MAP, {
     N(KEYVAL|VALQUO, "data", "There once was a short man from Ealing\nWho got on a bus to Darjeeling\n    It said on the door\n    \"Please don't spit on the floor\"\nSo he carefully spat on the ceiling\n")
      })
);

ADD_CASE_TO_GROUP("block literal and two scalars",
R"(
example: >
        HTML goes into YAML without modification
message: |
        <blockquote style="font: italic 12pt Times">
        <p>"Three is always greater than two,
           even for large values of two"</p>
        <p>--Author Unknown</p>
        </blockquote>
date: 2007-06-01
)",
     N(MAP, L{
          N(KEYVAL|VALQUO, "example", "HTML goes into YAML without modification\n"),
          N(KEYVAL|VALQUO, "message", R"(<blockquote style="font: italic 12pt Times">
<p>"Three is always greater than two,
   even for large values of two"</p>
<p>--Author Unknown</p>
</blockquote>
)"),
          N("date", "2007-06-01"),
              })
);

ADD_CASE_TO_GROUP("block literal no chomp, no indentation",
R"(example: |
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:

  plus another line at the end.

another: text
)",
     N(MAP, L{
      N(KEYVAL|VALQUO, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
      N("another", "text"),
          })
);
}


} // namespace yml
} // namespace c4
