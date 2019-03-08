#include "./test_group.hpp"

namespace c4 {
namespace yml {

#define INDENTATION_CASES \
    "4 chars",\
    "2 chars + 4 chars, ex0",\
    "2 chars + 4 chars, ex1",\
    "2 chars + 4 chars, ex2",\
    "non-indented blank lines",\
    "unnecessary indentation"

CASE_GROUP(INDENTATION)
{
    APPEND_CASES(

C("4 chars",
R"(
key:
     value
another_key:
    sub_key0:
      - val0
      - val1
    sub_key1:
      - val2
      - val3
    sub_key2:
      - val4
      - val5
)",
L{
    N("key", "value"),
    N("another_key", L{
        N("sub_key0", L{N("val0"), N("val1")}),
        N("sub_key1", L{N("val2"), N("val3")}),
        N("sub_key2", L{N("val4"), N("val5")}),
    })
}),

C("2 chars + 4 chars, ex0",
R"(
key:
     value
another_key:
    sub_key0:
        - val0
        - val1
    sub_key1:
      - val2
      - val3
    sub_key2:
      - val4
      - val5
)",
L{
    N("key", "value"),
    N("another_key", L{
        N("sub_key0", L{N("val0"), N("val1")}),
        N("sub_key1", L{N("val2"), N("val3")}),
        N("sub_key2", L{N("val4"), N("val5")}),
    })
}),

C("2 chars + 4 chars, ex1",
R"(
key:
     value
another_key:
    sub_key0:
      - val0
      - val1
    sub_key1:
        - val2
        - val3
    sub_key2:
      - val4
      - val5
)",
L{
    N("key", "value"),
    N("another_key", L{
        N("sub_key0", L{N("val0"), N("val1")}),
        N("sub_key1", L{N("val2"), N("val3")}),
        N("sub_key2", L{N("val4"), N("val5")}),
    })
}),

C("2 chars + 4 chars, ex2",
R"(
key:
     value
another_key:
    sub_key0:
      - val0
      - val1
    sub_key1:
      - val2
      - val3
    sub_key2:
        - val4
        - val5
)",
L{
    N("key", "value"),
    N("another_key", L{
        N("sub_key0", L{N("val0"), N("val1")}),
        N("sub_key1", L{N("val2"), N("val3")}),
        N("sub_key2", L{N("val4"), N("val5")}),
    })
}),

C("non-indented blank lines",
R"(
matrix:

  include:  # next line is blank

    - env01
    - env02
    - env03
    - env04  # next line has one space
 
    - env11
    - env12
    - env13
    - env14  # next line has two spaces
  
    - env21
    - env22
    - env23
    - env24  # next line has three spaces
   
    - env31
    - env32
    - env33
    - env34  # next line has four spaces
    
    - env41
    - env42
    - env43
    - env44  # next line has five spaces
     
    - env51
    - env52
    - env53
    - env54  # next line has six spaces
      
    - env61
    - env62
    - env63
    - env64  # next line has five spaces
)",
L{N("matrix", L{
    N("include", L{
      N("env01"), N("env02"), N("env03"), N("env04"), 
      N("env11"), N("env12"), N("env13"), N("env14"), 
      N("env21"), N("env22"), N("env23"), N("env24"), 
      N("env31"), N("env32"), N("env33"), N("env34"), 
      N("env41"), N("env42"), N("env43"), N("env44"), 
      N("env51"), N("env52"), N("env53"), N("env54"), 
      N("env61"), N("env62"), N("env63"), N("env64"), 
        }
  )})
}),

C("unnecessary indentation",
R"(
skip_commits:
  files:
                - a
                - b
                - c
                - d
                - e
                - f
  more_files:
           - a
           - b
  even_more_files:
     - a
     - b
more_skip:
  files:
          - a
          - b
          - c
          - d
          - e
          - f
  more_files:
    - a
    - b
)",
L{
  N("skip_commits", L{
    N("files", L{N("a"), N("b"), N("c"), N("d"), N("e"), N("f"),}),
    N("more_files", L{N("a"), N("b"),}),
    N("even_more_files", L{N("a"), N("b"),}),
  }),
  N("more_skip", L{
    N("files", L{N("a"), N("b"), N("c"), N("d"), N("e"), N("f"),}),
    N("more_files", L{N("a"), N("b"),}),
  })
}),
  )
}

INSTANTIATE_GROUP(INDENTATION);

} // namespace yml
} // namespace c4
