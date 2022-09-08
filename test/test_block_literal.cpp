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
        EXPECT_EQ(t[0].val(), csubstr(""));
        EXPECT_EQ(t[1].val(), csubstr(""));
        EXPECT_EQ(t[2].val(), csubstr("\n"));
    }
    {
        Tree t = parse_in_arena(R"(
- |
  
- |-
  
- |+
  
)");
        EXPECT_FALSE(t.empty());
        EXPECT_EQ(t[0].val(), csubstr(""));
        EXPECT_EQ(t[1].val(), csubstr(""));
        EXPECT_EQ(t[2].val(), csubstr("\n"));
    }
    {
        Tree t = parse_in_arena(R"(
- |
- |-
- |+
)");
        EXPECT_FALSE(t.empty());
        EXPECT_EQ(t[0].val(), csubstr(""));
        EXPECT_EQ(t[1].val(), csubstr(""));
        EXPECT_EQ(t[2].val(), csubstr(""));
    }
}


TEST(block_literal, emit_does_not_add_lines_to_multi_at_end_1)
{
    Tree t = parse_in_arena("[]");
    NodeRef r = t.rootref();
    r.append_child() = "\n\n";
    r.append_child() = "\n\n";
    r.append_child() = "last";
    std::string out = emitrs_yaml<std::string>(t);
    t.clear();
    t = parse_in_arena(to_csubstr(out));
    EXPECT_EQ(t[0].val(), csubstr("\n\n"));
    EXPECT_EQ(t[1].val(), csubstr("\n\n"));
    EXPECT_EQ(t[2].val(), csubstr("last"));
    out = emitrs_yaml<std::string>(t);
    t.clear();
    t = parse_in_arena(to_csubstr(out));
    EXPECT_EQ(t[0].val(), csubstr("\n\n"));
    EXPECT_EQ(t[1].val(), csubstr("\n\n"));
    EXPECT_EQ(t[2].val(), csubstr("last"));
    out = emitrs_yaml<std::string>(t);
    t.clear();
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
    std::string out = emitrs_yaml<std::string>(t);
    EXPECT_EQ(out, expected);
    t = parse_in_arena(to_csubstr(out));
    EXPECT_EQ(t.docref(0).val(), csubstr("ab\n\n \n"));
    out = emitrs_yaml<std::string>(t);
    EXPECT_EQ(out, expected);
    t = parse_in_arena(to_csubstr(out));
    EXPECT_EQ(t.docref(0).val(), csubstr("ab\n\n \n"));
    out = emitrs_yaml<std::string>(t);
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
    std::string out = emitrs_yaml<std::string>(t);
    EXPECT_EQ(out, expected);
    t = parse_in_arena(to_csubstr(out));
    EXPECT_EQ(t[0].val(), "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n");
    EXPECT_EQ(t[1].val(), "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n\n");
    out = emitrs_yaml<std::string>(t);
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
    auto emitted = emitrs_yaml<std::string>(t);
    #ifdef RYML_DBG
    __c4presc(emitted.data(), emitted.size());
    #endif
    Tree r = parse_in_arena(to_csubstr(emitted));
    EXPECT_EQ(t["with"].val(), "text\n \tlines\n");
    EXPECT_EQ(t["without"].val(), "text\n \tlines\n");
}

#ifdef JAVAI
TEST(block_literal, errors_on_tab_indents)
{
    Tree tree;
    ExpectError::do_check(&tree, [&]{
        parse_in_arena("foo: |4\n    this is foo\n    now with tab-\n \t \tmust not work\n", &tree);
    });
}
#endif

TEST(block_literal, test_suite_L24T_00)
{
    // this is double quoted, but will be emitted as a block literal
    csubstr yaml = R"(foo: "x\n \n"
)";
    test_check_emit_check(yaml, [](Tree const &t){
        EXPECT_EQ(t["foo"].val(), csubstr("x\n \n"));
    });
}

TEST(block_literal, error_on_bad_spec)
{
    Tree t;
    ExpectError::do_check(&t, [&t]{
        t = parse_in_arena("- |012abc\n  must have errors above\n");
    });
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


CASE_GROUP(BLOCK_LITERAL)
{
//
ADD_CASE_TO_GROUP("indentation requirements",
R"(---
|
hello
there
---
|
 hello
 there
---
|
  hello
  there
---
|
ciao
qua
---
|
    ciao
    qua
---
|
      ciao
      qua
---
- |
 hello
 there
- |
 ciao
 qua
---
foo: |
 hello
 there
bar: |
 ciao
 qua
)",
N(STREAM, L{
        N(DOCVAL|QV, "hello\nthere\n"),
        N(DOCVAL|QV, "hello\nthere\n"),
        N(DOCVAL|QV, "hello\nthere\n"),
        N(DOCVAL|QV, "ciao\nqua\n"),
        N(DOCVAL|QV, "ciao\nqua\n"),
        N(DOCVAL|QV, "ciao\nqua\n"),
        N(SEQ|DOC, L{N(QV, "hello\nthere\n"), N(QV, "ciao\nqua\n")}),
        N(MAP|DOC, L{N(QV, "foo", "hello\nthere\n"), N(QV, "bar", "ciao\nqua\n")}),
    }));

ADD_CASE_TO_GROUP("indentation requirements err seq", EXPECT_PARSE_ERROR,
R"(- |
hello
there
- |
ciao
qua
)",
N(L{N(QV, "hello\nthere\n"), N(QV, "ciao\nqua\n")}));

ADD_CASE_TO_GROUP("indentation requirements err map", EXPECT_PARSE_ERROR,
R"(foo: |
hello
there
bar: |
ciao
qua
)",
N(L{N(QV, "foo", "hello\nthere\n"), N(QV, "bar" "ciao\nqua\n")}));

ADD_CASE_TO_GROUP("indentation requirements err level", EXPECT_PARSE_ERROR,
R"(--- |2
 hello
 there
)",
N(NOTYPE));

ADD_CASE_TO_GROUP("empty, specs only 2G84_02",
"--- |1-",
N(STREAM, L{N(DOCVAL|VALQUO, {})}));

ADD_CASE_TO_GROUP("empty, specs only 2G84_03",
"--- |1+",
N(STREAM, L{N(DOCVAL|VALQUO, {})}));

ADD_CASE_TO_GROUP("empty, specs only 2G84_xx",
"--- |+",
N(STREAM, L{N(DOCVAL|VALQUO, {})}));

ADD_CASE_TO_GROUP("empty, specs only 2G84_02_1",
"|1-",
N(DOCVAL|VALQUO, {}));

ADD_CASE_TO_GROUP("empty, specs only 2G84_03_1",
"|1+",
N(DOCVAL|VALQUO, {}));

ADD_CASE_TO_GROUP("empty, specs only 2G84_xx_1",
"|+",
N(DOCVAL|VALQUO, {}));

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

ADD_CASE_TO_GROUP("block literal with empty docval 13",
R"(|
 
  

   

    

)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.0",
R"(- |+
)",
  N(SEQ, L{N(VALQUO, "")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.0.1",
R"(- |+
 )",
  N(SEQ, L{N(VALQUO, "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.0.2",
R"(- |+
   )",
  N(SEQ, L{N(VALQUO, "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.1",
R"(foo: |+
)",
  N(MAP, L{N(VALQUO, "foo", "")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.1.1",
R"(foo: |+
 )",
  N(MAP, L{N(VALQUO, "foo", "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.1.2",
R"(foo: |+
  )",
  N(MAP, L{N(VALQUO, "foo", "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.2",
R"(|+
)",
  N(DOCVAL|VALQUO, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.2.1",
R"(|+
 )",
  N(DOCVAL|VALQUO, "\n")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.2.2",
R"(|+
   )",
  N(DOCVAL|VALQUO, "\n")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 15.0",
R"(- |+

)",
  N(SEQ, L{N(VALQUO, "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 15.0.1",
R"(- |+

  )",
  N(SEQ, L{N(VALQUO, "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 15.1",
R"(foo: |+

)",
  N(MAP, L{N(VALQUO, "foo", "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 15.1.1",
R"(foo: |+

  )",
  N(MAP, L{N(VALQUO, "foo", "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 15.2",
R"(|+

)",
  N(DOCVAL|VALQUO, "\n")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 15.2.1",
R"(|+

  )",
  N(DOCVAL|VALQUO, "\n")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 16",
R"(|+


)",
  N(DOCVAL|VALQUO, "\n\n")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 16.1",
R"(foo: |+


)",
  N(MAP, L{N(VALQUO, "foo", "\n\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 16.2",
R"(- |+


)",
  N(SEQ, L{N(VALQUO, "\n\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 17",
R"(|+



)",
  N(DOCVAL|VALQUO, "\n\n\n")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 17.1",
R"(foo: |+



)",
  N(MAP, L{N(VALQUO, "foo", "\n\n\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 17.2",
R"(- |+



)",
  N(SEQ, L{N(VALQUO, "\n\n\n")})
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

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 1.1",
R"(|
  asd
  )",
  N(DOCVAL|VALQUO, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 1.2",
R"(|+
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

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 5.1",
R"(|
       asd
     
   
     
  
 
  )",
  N(DOCVAL|VALQUO, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 5.2",
R"(|
       asd
     
   
       
  
 
  )",
  N(DOCVAL|VALQUO, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 5.3",
R"(|
       asd
     
   
        
  
 
  )",
  N(DOCVAL|VALQUO, "asd\n\n\n \n")
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

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 10",
R"(|
     asd
     	 )",
  N(DOCVAL|VALQUO, "asd\n\t \n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 11",
R"(|
     asd
      	 )",
  N(DOCVAL|VALQUO, "asd\n \t \n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 12",
R"(|
     asd
     	 
)",
  N(DOCVAL|VALQUO, "asd\n\t \n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 13",
R"(|
     asd
      	 
)",
  N(DOCVAL|VALQUO, "asd\n \t \n")
    );

ADD_CASE_TO_GROUP("block literal, empty block vals in seq 0",
R"(- |+
  
- |+
  )",
N(L{N(QV, "\n"), N(QV, "\n"),}));

ADD_CASE_TO_GROUP("block literal, empty block vals in seq 1",
R"(- |+
  
- |+
  
)",
N(L{N(QV, "\n"), N(QV, "\n"),}));

}


} // namespace yml
} // namespace c4
