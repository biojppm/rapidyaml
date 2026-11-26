#include "./test_group/test_group.hpp"
#include "./test_group/test_group.def.hpp"

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

CASE_GROUP(BLOCK_LITERAL)
{

ADD_CASE_TO_GROUP("trailing newlines 0_0",
R"(- |
- |

- |


- |



- |




- |





)",
N(SB, L{
  N(VL, ""),
  N(VL, ""),
  N(VL, ""),
  N(VL, ""),
  N(VL, ""),
  N(VL, ""),
})
);

ADD_CASE_TO_GROUP("trailing newlines 0_1",
R"(- |-
- |-

- |-


- |-



- |-




- |-





)",
N(SB, L{
  N(VL, ""),
  N(VL, ""),
  N(VL, ""),
  N(VL, ""),
  N(VL, ""),
  N(VL, ""),
})
);

ADD_CASE_TO_GROUP("trailing newlines 0_2",
R"(- |+
- |+

- |+


- |+



- |+




- |+





)",
N(SB, L{
  N(VL, ""),
  N(VL, "\n"),
  N(VL, "\n\n"),
  N(VL, "\n\n\n"),
  N(VL, "\n\n\n\n"),
  N(VL, "\n\n\n\n\n"),
})
);

ADD_CASE_TO_GROUP("trailing newlines 1_0",
R"(- |
  a
- |
  b

- |
  c


- |
  d



- |
  e




- |
  f





)",
N(SB, L{
  N(VL, "a\n"),
  N(VL, "b\n"),
  N(VL, "c\n"),
  N(VL, "d\n"),
  N(VL, "e\n"),
  N(VL, "f\n"),
})
);

ADD_CASE_TO_GROUP("trailing newlines 1_1",
R"(- |-
  a
- |-
  b

- |-
  c


- |-
  d



- |-
  e




- |-
  f





)",
N(SB, L{
  N(VL, "a"),
  N(VL, "b"),
  N(VL, "c"),
  N(VL, "d"),
  N(VL, "e"),
  N(VL, "f"),
})
);

ADD_CASE_TO_GROUP("trailing newlines 1_2",
R"(- |+
  a
- |+
  b

- |+
  c


- |+
  d



- |+
  e




- |+
  f





)",
N(SB, L{
  N(VL, "a\n"),
  N(VL, "b\n\n"),
  N(VL, "c\n\n\n"),
  N(VL, "d\n\n\n\n"),
  N(VL, "e\n\n\n\n\n"),
  N(VL, "f\n\n\n\n\n\n"),
})
);

ADD_CASE_TO_GROUP("trailing whitespace", R"(
|
 a
  
   
    
)",
N(VL, "a\n \n  \n   \n")
);

ADD_CASE_TO_GROUP("indentation requirements",
R"(---
|
hello0
there
---
|
 hello1
 there
---
|
  hello2
  there
---
|
ciao3
qua
---
|
    ciao4
    qua
---
|
      ciao5
      qua
---
- |
 hello6
 there
- |
 ciao7
 qua
---
foo: |
 hello8
 there
bar: |
 ciao9
 qua
)",
N(STREAM, L{
        N(DOC|VL, "hello0\nthere\n"),
        N(DOC|VL, "hello1\nthere\n"),
        N(DOC|VL, "hello2\nthere\n"),
        N(DOC|VL, "ciao3\nqua\n"),
        N(DOC|VL, "ciao4\nqua\n"),
        N(DOC|VL, "ciao5\nqua\n"),
        N(DOC|SB, L{N(VL, "hello6\nthere\n"), N(VL, "ciao7\nqua\n")}),
        N(DOC|MB, L{N(KP|VL, "foo", "hello8\nthere\n"), N(KP|VL, "bar", "ciao9\nqua\n")}),
    }));

ADD_CASE_TO_GROUP("indentation requirements err, 0", EXPECT_PARSE_ERROR,
R"(|0)",
Location(1, 1)
);

ADD_CASE_TO_GROUP("indentation requirements err, 0.1", EXPECT_PARSE_ERROR,
R"(|0
)",
Location(1, 1)
);

ADD_CASE_TO_GROUP("indentation requirements err seq", EXPECT_PARSE_ERROR,
R"(- |
hello
there
- |
ciao
qua
)",
Location(2, 1)
);

ADD_CASE_TO_GROUP("indentation requirements err map", EXPECT_PARSE_ERROR,
R"(foo: |
hello
there
bar: |
ciao
qua
)",
Location(2, 6)
);

ADD_CASE_TO_GROUP("indentation requirements err level", EXPECT_PARSE_ERROR,
R"(--- |2
 hello
 there
)");

ADD_CASE_TO_GROUP("empty, specs only 2G84_02",
"--- |1-",
N(STREAM, L{N(DOC|VL, {})}));

ADD_CASE_TO_GROUP("empty, specs only 2G84_03",
"--- |1+",
N(STREAM, L{N(DOC|VL, {})}));

ADD_CASE_TO_GROUP("empty, specs only 2G84_xx",
"--- |+",
N(STREAM, L{N(DOC|VL, {})}));

ADD_CASE_TO_GROUP("empty, specs only 2G84_02_1",
"|1-",
N(VL, {}));

ADD_CASE_TO_GROUP("empty, specs only 2G84_03_1",
"|1+",
N(VL, {}));

ADD_CASE_TO_GROUP("empty, specs only 2G84_xx_1",
"|+",
N(VL, {}));

ADD_CASE_TO_GROUP("block literal as map entry",
R"(
data: |
   There once was a short man from Ealing
   Who got on a bus to Darjeeling
       It said on the door
       "Please don't spit on the floor"
   So he carefully spat on the ceiling
)",
  N(MB, {
     N(KP|VL, "data", "There once was a short man from Ealing\nWho got on a bus to Darjeeling\n    It said on the door\n    \"Please don't spit on the floor\"\nSo he carefully spat on the ceiling\n")
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
     N(MB, L{
          N(KP|VF, "example", "HTML goes into YAML without modification\n"),
          N(KP|VL, "message", R"(<blockquote style="font: italic 12pt Times">
<p>"Three is always greater than two,
   even for large values of two"</p>
<p>--Author Unknown</p>
</blockquote>
)"),
          N(KP|VP, "date", "2007-06-01"),
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
     N(MB, L{
      N(KP|VL, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
      N(KP|VP, "another", "text"),
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
N(SB, L{
    N(VL, "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N(VP, "another val")
  })
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
N(SB, L{
    N(VL, "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n\n\n"),
    N(VP, "another val")
  })
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
N(SB, L{
    N(VL, "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end."),
    N(VP, "another val")
  })
);

ADD_CASE_TO_GROUP("block literal as seq val at eof, implicit indentation 2",
R"(
- |
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
)",
N(SB, L{
    N(VL, "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
  })
);

ADD_CASE_TO_GROUP("block literal as seq val at eof, implicit indentation 4",
R"(
- |
    Several lines of text,
    with some "quotes" of various 'types',
    and also a blank line:
    
    plus another line at the end.
    
    
)",
N(SB, L{
    N(VL, "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
  })
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
N(MB, L{
    N(KP|VL, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N(KP|VP, "another", "val")
  })
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
N(MB, L{
    N(KP|VL, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N(KP|VP, "another", "val")
  })
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
N(MB, L{
    N(KP|VL, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n\n\n"),
    N(KP|VP, "another", "val")
  })
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
N(MB, L{
    N(KP|VL, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end."),
    N(KP|VP, "another", "val")
  })
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
N(MB, L{
    N(KP|VL, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N(KP|VP, "another", "val")
  })
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
N(MB, L{
    N(KP|VL, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N(KP|VP, "another", "val")
  })
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
N(MB, L{
    N(KP|VL, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N(KP|VP, "another", "val")
  })
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
N(MB, L{
    N(KP|VL, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N(KP|VP, "another", "val")
  })
);

ADD_CASE_TO_GROUP("block literal as map val at eof, implicit indentation 2",
R"(
example: |
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:
  
  plus another line at the end.
  
  
)",
N(MB, L{
    N(KP|VL, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
  })
);

ADD_CASE_TO_GROUP("block literal as map val at eof, implicit indentation 4",
R"(
example: |
    Several lines of text,
    with some "quotes" of various 'types',
    and also a blank line:
    
    plus another line at the end.
    
    
)",
N(MB, L{
    N(KP|VL, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
  })
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
N(MB, L{
    N(KP|VL, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N(KP|VP, "another", "val")
  })
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
N(MB, L{
    N(KP|VL, "example", "Several lines of text,\nwith some \"quotes\" of various 'types',\nand also a blank line:\n\nplus another line at the end.\n"),
    N(KP|VP, "another", "val")
  })
);

ADD_CASE_TO_GROUP("block literal with empty unindented lines, without quotes",
    R"(tpl:
  src: |
    #include <{{hdr.filename}}>
  
    {{src.gencode}}
)",
N(MB, L{
    N(KP|MB, "tpl", L{N(KP|VL, "src", "#include <{{hdr.filename}}>\n\n{{src.gencode}}\n")})
  })
);

ADD_CASE_TO_GROUP("block literal with empty unindented lines, with double quotes",
    R"(tpl:
  src: |
    #include "{{hdr.filename}}"
  
    {{src.gencode}}
)",
N(MB, L{
    N(KP|MB, "tpl", L{N(KP|VL, "src", "#include \"{{hdr.filename}}\"\n\n{{src.gencode}}\n")})
  })
);

ADD_CASE_TO_GROUP("block literal with empty unindented lines, with single quotes",
    R"(tpl:
  src: |
    #include '{{hdr.filename}}'
  
    {{src.gencode}}
)",
N(MB, L{
    N(KP|MB, "tpl", L{N(KP|VL, "src", "#include '{{hdr.filename}}'\n\n{{src.gencode}}\n")})
  })
);

ADD_CASE_TO_GROUP("block literal with same indentation level 0",
R"(
aaa: |2
  xxx
bbb: |
  yyy
)",
N(MB, L{
  N(KP|VL, "aaa", "xxx\n"),
  N(KP|VL, "bbb", "yyy\n")
})
);

ADD_CASE_TO_GROUP("block literal with same indentation level 1",
R"(
- aaa: |2
    xxx
  bbb: |
    yyy
)",
N(SB, L{
  N(MB, L{
    N(KP|VL, "aaa", "xxx\n"),
    N(KP|VL, "bbb", "yyy\n"),
  })
})
);

ADD_CASE_TO_GROUP("block literal with tab and spaces",
R"(|
       	  )",
  N(VL, "\t  \n")
    );


ADD_CASE_TO_GROUP("block literal with empty docval 1",
R"(|)",
  N(VL, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 2",
R"(|
)",
  N(VL, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 3",
R"(|
  )",
  N(VL, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 4",
R"(|
  
)",
  N(VL, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 5",
R"(|
    
)",
  N(VL, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 8",
R"(|


)",
  N(VL, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 9",
R"(|



)",
  N(VL, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 10",
R"(|




)",
  N(VL, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 11",
R"(|
 
  
   
    )",
  N(VL, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 12",
R"(|
 
  
   
    
     
      
    
   
 
)",
  N(VL, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 13",
R"(|
 
  

   

    

)",
  N(VL, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.0",
R"(- |+
)",
  N(SB, L{N(VL, "")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.0.1",
R"(- |+
 )",
  N(SB, L{N(VL, "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.0.2",
R"(- |+
   )",
  N(SB, L{N(VL, "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.0.3",
R"(- |+

- |+

)",
  N(SB, L{N(VL, "\n"), N(VL, "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.0.4",
R"(- |+


- |+


)",
  N(SB, L{N(VL, "\n\n"), N(VL, "\n\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.0.5",
R"(- |+



- |+



)",
  N(SB, L{N(VL, "\n\n\n"), N(VL, "\n\n\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.1",
R"(foo: |+
)",
  N(MB, L{N(KP|VL, "foo", "")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.1.1",
R"(foo: |+
 )",
  N(MB, L{N(KP|VL, "foo", "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.1.2",
R"(foo: |+
  )",
  N(MB, L{N(KP|VL, "foo", "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.2",
R"(|+
)",
  N(VL, "")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.2.1",
R"(|+
 )",
  N(VL, "\n")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 14.2.2",
R"(|+
   )",
  N(VL, "\n")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 15.0",
R"(- |+

)",
  N(SB, L{N(VL, "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 15.0.1",
R"(- |+

  )",
  N(SB, L{N(VL, "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 15.1",
R"(foo: |+

)",
  N(MB, L{N(KP|VL, "foo", "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 15.1.1",
R"(foo: |+

  )",
  N(MB, L{N(KP|VL, "foo", "\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 15.2",
R"(|+

)",
  N(VL, "\n")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 15.2.1",
R"(|+

  )",
  N(VL, "\n")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 16",
R"(|+


)",
  N(VL, "\n\n")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 16.1",
R"(foo: |+


)",
  N(MB, L{N(KP|VL, "foo", "\n\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 16.2",
R"(- |+


)",
  N(SB, L{N(VL, "\n\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 17",
R"(|+



)",
  N(VL, "\n\n\n")
    );

ADD_CASE_TO_GROUP("block literal with empty docval 17.1",
R"(foo: |+



)",
  N(MB, L{N(KP|VL, "foo", "\n\n\n")})
    );

ADD_CASE_TO_GROUP("block literal with empty docval 17.2",
R"(- |+



)",
  N(SB, L{N(VL, "\n\n\n")})
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 0",
R"(|
  asd)",
  N(VL, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 1",
R"(|
  asd
)",
  N(VL, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 1.1",
R"(|
  asd
  )",
  N(VL, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 1.2",
R"(|+
  asd
  )",
  N(VL, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 2",
R"(|
  asd

)",
  N(VL, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 3",
R"(|
  asd
  )",
  N(VL, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 4",
R"(|
  asd
  
  )",
  N(VL, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 5",
R"(|
     asd
   
  )",
  N(VL, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 5.1",
R"(|
       asd
     
   
     
  
 
  )",
  N(VL, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 5.2",
R"(|
       asd
     
   
       
  
 
  )",
  N(VL, "asd\n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 5.3",
R"(|
       asd
     
   
        
  
 
  )",
  N(VL, "asd\n\n\n \n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 6",
R"(|
     asd
      )",
  N(VL, "asd\n \n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 7",
R"(|
     asd
      
)",
  N(VL, "asd\n \n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 8",
R"(|
     asd
       )",
  N(VL, "asd\n  \n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 9",
R"(|
     asd
       
)",
  N(VL, "asd\n  \n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 10",
R"(|
     asd
     	 )",
  N(VL, "asd\n\t \n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 11",
R"(|
     asd
      	 )",
  N(VL, "asd\n \t \n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 12",
R"(|
     asd
     	 
)",
  N(VL, "asd\n\t \n")
    );

ADD_CASE_TO_GROUP("block literal with docval no newlines at end 13",
R"(|
     asd
      	 
)",
  N(VL, "asd\n \t \n")
    );

ADD_CASE_TO_GROUP("block literal, empty block vals in seq 0",
R"(- |+
  
- |+
  )",
N(SB, L{N(VL, "\n"), N(VL, "\n"),}));

ADD_CASE_TO_GROUP("block literal, empty block vals in seq 1",
R"(- |+
  
- |+
  
)",
N(SB, L{N(VL, "\n"), N(VL, "\n"),}));

} // ADD_GROUP()

} // namespace yml
} // namespace c4
