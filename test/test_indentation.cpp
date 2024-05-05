#include "./test_lib/test_group.hpp"
#include "./test_lib/test_group.def.hpp"

namespace c4 {
namespace yml {

CASE_GROUP(INDENTATION)
{

ADD_CASE_TO_GROUP("plain scalar indentation, 0", R"(
plain
 scalar
 follows
)",
N(VP, "plain scalar follows")
);

ADD_CASE_TO_GROUP("plain scalar indentation, 0.1", R"(
plain
scalar
follows
)",
N(VP, "plain scalar follows")
);

ADD_CASE_TO_GROUP("plain scalar indentation, 1",
R"(a: plain
 scalar
 follows
)",
N(MB, L{N(KP|VP, "a", "plain scalar follows")})
);

ADD_CASE_TO_GROUP("plain scalar indentation, 1, err", EXPECT_PARSE_ERROR,
R"(a: plain
scalar
follows
)",
LineCol(2, 7)
);

ADD_CASE_TO_GROUP("plain scalar indentation, 2",
R"(- plain
 scalar
 follows
)",
N(SB, L{N(VP, "plain scalar follows")})
);

ADD_CASE_TO_GROUP("plain scalar indentation, 2, err", EXPECT_PARSE_ERROR,
R"(- plain
scalar
follows
)",
LineCol(2, 1)
);

ADD_CASE_TO_GROUP("plain scalar indentation, 3",
R"(a: [plain
 scalar
 follows
 ]
)",
N(MB, L{N(KP|SFS, "a", L{N(VP, "plain scalar follows")})})
);

// FIXME: this should be a parse error
ADD_CASE_TO_GROUP("plain scalar indentation, 3.1",
R"(a: [plain
 scalar
 follows
]
b: c
)",
N(MB, L{
  N(KP|SFS, "a", L{N(VP, "plain scalar follows")}),
  N(KP|VP, "b", "c"),
})
);

ADD_CASE_TO_GROUP("4 chars",
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
N(MB, L{
    N(KP|VP, "key", "value"),
    N(KP|MB, "another_key", L{
        N(KP|SB, "sub_key0", L{N(VP, "val0"), N(VP, "val1")}),
        N(KP|SB, "sub_key1", L{N(VP, "val2"), N(VP, "val3")}),
        N(KP|SB, "sub_key2", L{N(VP, "val4"), N(VP, "val5")}),
    })
})
);

ADD_CASE_TO_GROUP("2 chars + 4 chars, ex0",
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
N(MB, L{
    N(KP|VP, "key", "value"),
    N(KP|MB, "another_key", L{
        N(KP|SB, "sub_key0", L{N(VP, "val0"), N(VP, "val1")}),
        N(KP|SB, "sub_key1", L{N(VP, "val2"), N(VP, "val3")}),
        N(KP|SB, "sub_key2", L{N(VP, "val4"), N(VP, "val5")}),
    })
})
);

ADD_CASE_TO_GROUP("2 chars + 4 chars, ex1",
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
N(MB, L{
    N(KP|VP, "key", "value"),
    N(KP|MB, "another_key", L{
        N(KP|SB, "sub_key0", L{N(VP, "val0"), N(VP, "val1")}),
        N(KP|SB, "sub_key1", L{N(VP, "val2"), N(VP, "val3")}),
        N(KP|SB, "sub_key2", L{N(VP, "val4"), N(VP, "val5")}),
    })
})
);

ADD_CASE_TO_GROUP("2 chars + 4 chars, ex2",
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
N(MB, L{
    N(KP|VP, "key", "value"),
    N(KP|MB, "another_key", L{
        N(KP|SB, "sub_key0", L{N(VP, "val0"), N(VP, "val1")}),
        N(KP|SB, "sub_key1", L{N(VP, "val2"), N(VP, "val3")}),
        N(KP|SB, "sub_key2", L{N(VP, "val4"), N(VP, "val5")}),
    })
})
);

ADD_CASE_TO_GROUP("non-indented blank lines",
R"(matrix:

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
N(MB, L{N(KP|MB, "matrix", L{
    N(KP|SB, "include", L{
      N(VP, "env01"), N(VP, "env02"), N(VP, "env03"), N(VP, "env04"),
      N(VP, "env11"), N(VP, "env12"), N(VP, "env13"), N(VP, "env14"),
      N(VP, "env21"), N(VP, "env22"), N(VP, "env23"), N(VP, "env24"),
      N(VP, "env31"), N(VP, "env32"), N(VP, "env33"), N(VP, "env34"),
      N(VP, "env41"), N(VP, "env42"), N(VP, "env43"), N(VP, "env44"),
      N(VP, "env51"), N(VP, "env52"), N(VP, "env53"), N(VP, "env54"),
      N(VP, "env61"), N(VP, "env62"), N(VP, "env63"), N(VP, "env64"),
    })
  })
})
);

ADD_CASE_TO_GROUP("unnecessary indentation",
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
N(MB, L{
  N(KP|MB, "skip_commits", L{
    N(KP|SB, "files", L{N(VP, "a"), N(VP, "b"), N(VP, "c"), N(VP, "d"), N(VP, "e"), N(VP, "f"),}),
    N(KP|SB, "more_files", L{N(VP, "a"), N(VP, "b"),}),
    N(KP|SB, "even_more_files", L{N(VP, "a"), N(VP, "b"),}),
  }),
  N(KP|MB, "more_skip", L{
    N(KP|SB, "files", L{N(VP, "a"), N(VP, "b"), N(VP, "c"), N(VP, "d"), N(VP, "e"), N(VP, "f"),}),
    N(KP|SB, "more_files", L{N(VP, "a"), N(VP, "b"),}),
  })
})
);


ADD_CASE_TO_GROUP("blank lines indented, 1 - at same scope",
R"(
skip_commits:
  files:
                - a  # next line has 22 spaces (aligns with -)
                
                - b  # next line has 23 spaces (aligns with #)
                     
                - c  # next line has 3 spaces
   
                - d
)",
N(MB, L{
  N(KP|MB, "skip_commits", L{
    N(KP|SB, "files", L{N(VP, "a"), N(VP, "b"), N(VP, "c"), N(VP, "d"),}),
  }),
})
);

ADD_CASE_TO_GROUP("indentation at start",
R"(
      foo:
        - a
        - b
      bar:
        - c
        - d
)",
N(MB, L{
  N(KP|SB, "foo", L{N(VP, "a"), N(VP, "b"),}),
  N(KP|SB, "bar", L{N(VP, "c"), N(VP, "d"),}),
})
);

ADD_CASE_TO_GROUP("unaligned comments",
R"(
      stand2sit:
        map: mirror
        dat:
          - a
          - b
#
          - b1
 #
          - b2
  #
   #
    #
          - b3
     #
      #
       #
          - b4
        #
         # - c
          #- d
          - b5
           #- d2
            #- d3
             #- d4
          - b6
              #- d41
               #
          - b61
                 #
                   #
          - b62
                     #
                       #
                         #
          - b63
                           #
          - b64
                           #
          - b65
                         #
                       #
                     #
          - b66
                     #
                   #
                 #
               #
              #- d41
             #- d5
            #- d6
           #- d7
          - b7
          #- d8
         #
        #
       #
          - b8
      #
     #
    #
          - b9
   #
  #
          - b10
 #
#
          - e
          - f
          - g
)",
N(MB, L{
  N(KP|MB, "stand2sit", L{
    N(KP|VP, "map", "mirror"),
    N(KP|SB, "dat", L{N(VP, "a"), N(VP, "b"), N(VP, "b1"), N(VP, "b2"), N(VP, "b3"), N(VP, "b4"), N(VP, "b5"), N(VP, "b6"), N(VP, "b61"), N(VP, "b62"), N(VP, "b63"), N(VP, "b64"), N(VP, "b65"), N(VP, "b66"), N(VP, "b7"), N(VP, "b8"), N(VP, "b9"), N(VP, "b10"), N(VP, "e"), N(VP, "f"), N(VP, "g")}),
  }),
}));

ADD_CASE_TO_GROUP("issue83",
R"(
e:
  - f
g: h
a:
  - b
  
c: d
)",
N(MB, L{
N(KP|SB, "e", L{N(VP, "f")}),
N(KP|VP, "g", "h"),
N(KP|SB, "a", L{N(VP, "b")}),
N(KP|VP, "c", "d"),
})
);

}

} // namespace yml
} // namespace c4
