#include "./test_group/test_group.hpp"
#include "./test_group/test_group.def.hpp"

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

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
        N(DOC|VF, "hello there\n"),
        N(DOC|VF, "hello there\n"),
        N(DOC|VF, "hello there\n"),
        N(DOC|VF, "ciao qua\n"),
        N(DOC|VF, "ciao qua\n"),
        N(DOC|VF, "ciao qua\n"),
        N(DOC|SB, L{N(VF, "hello there\n"), N(VF, "ciao qua\n")}),
        N(DOC|MB, L{N(KP|VF, "foo", "hello there\n"), N(KP|VF, "bar", "ciao qua\n")}),
    }));

ADD_CASE_TO_GROUP("indentation requirements err seq", EXPECT_PARSE_ERROR,
R"(- >
hello
there
- >
ciao
qua
)");

ADD_CASE_TO_GROUP("indentation requirements err map", EXPECT_PARSE_ERROR,
R"(foo: >
hello
there
bar: >
ciao
qua
)"
);

ADD_CASE_TO_GROUP("indentation requirements err level", EXPECT_PARSE_ERROR,
R"(--- >2
 hello
 there
)");

ADD_CASE_TO_GROUP("foo without space after",
R"(>
  foo
)",
N(VF, "foo\n"));

ADD_CASE_TO_GROUP("foo with space after",
R"(>
  foo
  
)",
N(VF, "foo\n"));

ADD_CASE_TO_GROUP("simple with indents",
R"(>
  foo
     
    bar
)",
N(VF, "foo\n   \n  bar\n"));


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
N(SB, L{
    N(VF, "\nfolded line\nnext line\n  * bullet\n\n  * list\n  * lines\n\nlast line\n"),
    N(VF, "\nfolded line\nnext line\n  * bullet\n\n  * list\n  * lines\n\nlast line\n"),
    N(VF, "\nfolded line\nnext line\n  * bullet\n\n  * list\n  * lines\n\nlast line\n"),
  })
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
N(SB, L{
    N(VF, "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N(VP,"another val")
  })
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
N(MB, L{
    N(KP|VF, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N(KP|VP, "another", "val")
  })
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
N(MB, L{
    N(KP|VF, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n\n\n"),
    N(KP|VP, "another", "val")
  })
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
N(MB, L{
    N(KP|VF, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end."),
    N(KP|VP, "another", "val")
  })
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
N(MB, L{
    N(KP|VF, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N(KP|VP, "another", "val")
  })
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
N(MB, L{
    N(KP|VF, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n\n\n"),
    N(KP|VF, "example2", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n\n\n"),
  })
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
N(MB, L{
    N(KP|VF, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end."),
    N(KP|VF, "example2", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end."),
  })
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
N(MB, L{
    N(KP|VF, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N(KP|VP, "another", "val")
  })
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
N(MB, L{
    N(KP|VF, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N(KP|VP, "another", "val")
  })
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
N(MB, L{
    N(KP|VF, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N(KP|VP, "another", "val")
  })
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
N(MB, L{
    N(KP|VF, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N(KP|VP, "another", "val")
  })
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
N(MB, L{
    N(KP|VF, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N(KP|VP, "another", "val")
  })
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
N(MB, L{
    N(KP|VF, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
    N(KP|VP, "another", "val")
  })
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
  N(MB, L{N(KP|VF, "data", "Wrapped text will be folded into a single paragraph\nBlank lines denote paragraph breaks\n")})
);

ADD_CASE_TO_GROUP("block folded, no chomp, no indentation",
R"(example: >
  Several lines of text,
  with some "quotes" of various 'types',
  and also a blank line:

  plus another line at the end.

another: text
)",
  N(MB, L{
      N(KP|VF, "example", "Several lines of text, with some \"quotes\" of various 'types', and also a blank line:\nplus another line at the end.\n"),
      N(KP|VP, "another", "text"),
      })
);

ADD_CASE_TO_GROUP("block folded with tab and spaces",
R"(>
       	  )",
  N(VF, "\t  \n")
    );


ADD_CASE_TO_GROUP("block folded with empty docval 1",
R"(>)",
  N(VF, "")
    );

ADD_CASE_TO_GROUP("block folded with empty docval 2",
R"(>
)",
  N(VF, "")
    );

ADD_CASE_TO_GROUP("block folded with empty docval 3",
R"(>
  )",
  N(VF, "")
    );

ADD_CASE_TO_GROUP("block folded with empty docval 4",
R"(>
  
)",
  N(VF, "")
    );

ADD_CASE_TO_GROUP("block folded with empty docval 5",
R"(>
    
)",
  N(VF, "")
    );

ADD_CASE_TO_GROUP("block folded with empty docval 8",
R"(>

)",
  N(VF, "")
    );

ADD_CASE_TO_GROUP("block folded with empty docval 9",
R"(>



)",
  N(VF, "")
    );

ADD_CASE_TO_GROUP("block folded with empty docval 10",
R"(>




)",
  N(VF, "")
    );

ADD_CASE_TO_GROUP("block folded with empty docval 11",
R"(>
 
  
   
    )",
  N(VF, "")
    );

ADD_CASE_TO_GROUP("block folded with empty docval 12",
R"(>
 
  
   
    
     
      
    
   
 
)",
  N(VF, "")
    );

ADD_CASE_TO_GROUP("block folded with empty docval 13",
R"(>
 
  

   

    

)",
  N(VF, "")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 0",
R"(>
  asd)",
  N(VF, "asd\n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 1",
R"(>
  asd
)",
  N(VF, "asd\n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 2",
R"(>
  asd

)",
  N(VF, "asd\n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 3",
R"(>
  asd
  )",
  N(VF, "asd\n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 4",
R"(>
  asd
  
  )",
  N(VF, "asd\n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 5",
R"(>
     asd
   
  )",
  N(VF, "asd\n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 5.1",
R"(>
       asd
     
   
     
  
 
  )",
  N(VF, "asd\n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 5.2",
R"(>
       asd
     
   
       
  
 
  )",
  N(VF, "asd\n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 5.3",
R"(>
       asd
     
   
        
  
 
  )",
  N(VF, "asd\n\n\n \n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 6",
R"(>
     asd
      )",
  N(VF, "asd\n \n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 7",
R"(>
     asd
      
)",
  N(VF, "asd\n \n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 8",
R"(>
     asd
       )",
  N(VF, "asd\n  \n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 9",
R"(>
     asd
       
)",
  N(VF, "asd\n  \n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 10",
R"(>
     asd
     	 )",
  N(VF, "asd\n\t \n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 11",
R"(>
     asd
      	 )",
  N(VF, "asd\n \t \n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 12",
R"(>
     asd
     	 
)",
  N(VF, "asd\n\t \n")
    );

ADD_CASE_TO_GROUP("block folded with docval no newlines at end 13",
R"(>
     asd
      	 
)",
  N(VF, "asd\n \t \n")
    );


ADD_CASE_TO_GROUP("block folded, keep, empty docval trailing 0",
R"(>+)",
  N(VF, "")
    );

ADD_CASE_TO_GROUP("block folded, keep, empty docval trailing 1",
R"(>+
)",
  N(VF, "")
    );

ADD_CASE_TO_GROUP("block folded, keep, empty docval trailing 1.1",
R"(>+
  )",
  N(VF, "\n")
    );

ADD_CASE_TO_GROUP("block folded, keep, empty docval trailing 1.2",
R"(>+
  asd)",
  N(VF, "asd")
    );

ADD_CASE_TO_GROUP("block folded, keep, empty docval trailing 1.3",
R"(>+
  asd
)",
  N(VF, "asd\n")
    );

ADD_CASE_TO_GROUP("block folded, keep, empty docval trailing 1.4",
R"(>+
  asd
  
)",
  N(VF, "asd\n\n")
    );

ADD_CASE_TO_GROUP("block folded, keep, empty docval trailing 2",
R"(>+

)",
  N(VF, "\n")
    );

ADD_CASE_TO_GROUP("block folded, keep, empty docval trailing 2.1",
R"(>+

  )",
  N(VF, "\n")
    );

ADD_CASE_TO_GROUP("block folded, keep, empty docval trailing 3",
R"(>+


)",
  N(VF, "\n\n")
    );

ADD_CASE_TO_GROUP("block folded, keep, empty docval trailing 4",
R"(>+



)",
  N(VF, "\n\n\n")
    );

ADD_CASE_TO_GROUP("block folded, keep, empty docval trailing 5",
R"(>+




)",
  N(VF, "\n\n\n\n")
    );

ADD_CASE_TO_GROUP("block folded, empty block vals in seq 0",
R"(- >+
  
- >+
  )",
N(SB, L{N(VF, "\n"), N(VF, "\n"),}));

ADD_CASE_TO_GROUP("block folded, empty block vals in seq 1",
R"(- >+
  
- >+
  
)",
N(SB, L{N(VF, "\n"), N(VF, "\n"),}));

} // CASE_GROOUP()

} // namespace yml
} // namespace c4
