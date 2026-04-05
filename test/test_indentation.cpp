#include "./test_lib/test_group.hpp"
#include "./test_lib/test_group.def.hpp"

RYML_DEFINE_TEST_MAIN()

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
Location(2, 7)
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
Location(2, 1)
);

ADD_CASE_TO_GROUP("plain scalar indentation, 3",
R"(a: [plain
 scalar
 follows
 ]
)",
N(MB, L{N(KP|SFM, "a", L{N(VP, "plain scalar follows")})})
);

ADD_CASE_TO_GROUP("plain scalar indentation, 3.1", EXPECT_PARSE_ERROR,
R"(a: [plain
 scalar
 follows
]
b: c
)");

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


//-----------------------------------------------------------------------------

ADD_CASE_TO_GROUP("leading tabs comments",
R"(
   	# asd
-
   	# asd
  a
   	# asd
)",
N(SB, L{N(VP, "a")})
);


//-----------------------------------------------------------------------------

ADD_CASE_TO_GROUP("leading tabs plain unk 0",
R"(
		foo
)",
N(VP, "foo")
);
ADD_CASE_TO_GROUP("leading tabs plain unk 1",
R"(
 		foo
)",
N(VP, "foo")
);
ADD_CASE_TO_GROUP("leading tabs plain unk 2",
R"(
   		foo
)",
N(VP, "foo")
);


//-----------------------------------------------------------------------------

ADD_CASE_TO_GROUP("leading tabs plain seq 0 RVAL", EXPECT_PARSE_ERROR,
                  R"(
-
		foo
)",
Location(3, 1)
);
ADD_CASE_TO_GROUP("leading tabs plain seq 0 RNXT",
R"(
-
 		foo
		
- bar
)",
N(SB, L{N(VP, "foo"), N(VP, "bar")})
);
ADD_CASE_TO_GROUP("leading tabs plain seq 1 RVAL",
R"(
-
 		foo
)",
N(SB, L{N(VP, "foo")})
);
ADD_CASE_TO_GROUP("leading tabs plain seq 1 RNXT",
R"(
-
 		foo
 		
- bar
)",
N(SB, L{N(VP, "foo"), N(VP, "bar")})
);
ADD_CASE_TO_GROUP("leading tabs plain seq 1 RNXT 2", EXPECT_PARSE_ERROR,
R"(
    - foo
  		- bar
)",
Location(3, 1));
ADD_CASE_TO_GROUP("leading tabs plain seq 1 RNXT 3", EXPECT_PARSE_ERROR,
R"(
    - foo
		  - bar
)",
Location(3, 5)
);
ADD_CASE_TO_GROUP("leading tabs plain seq 1 RNXT 4", EXPECT_PARSE_ERROR,
R"(
    - foo
 		 - bar
)",
Location(3, 1)
);
ADD_CASE_TO_GROUP("leading tabs plain seq 1 RNXT 5", EXPECT_PARSE_ERROR,
R"(
    - foo
    	 - bar
)",
Location(3, 5)
);
ADD_CASE_TO_GROUP("leading tabs plain seq 1 RNXT 6",
R"(
    - foo
     	  - bar
)",
N(SB, L{N(VP, "foo - bar")})
);
ADD_CASE_TO_GROUP("leading tabs plain seq 2 RVAL",
R"(
-
   		foo
)",
N(SB, L{N(VP, "foo")})
);
ADD_CASE_TO_GROUP("leading tabs plain seq 2 RNXT",
R"(
-
   		foo
   		
- bar
)",
N(SB, L{N(VP, "foo"), N(VP, "bar")})
);

ADD_CASE_TO_GROUP("leading tabs plain map 0", EXPECT_PARSE_ERROR,
R"(
a:
		foo
)",
Location(3, 1)
);
ADD_CASE_TO_GROUP("leading tabs plain map 1",
R"(
a:
 		foo
)",
N(MB, L{N(KP|VP, "a", "foo")})
);
ADD_CASE_TO_GROUP("leading tabs plain map 2",
R"(
a:
   		foo
)",
N(MB, L{N(KP|VP, "a", "foo")})
);


//-----------------------------------------------------------------------------

ADD_CASE_TO_GROUP("leading tabs squo unk 0",
R"(
		'foo'
)",
N(VS, "foo")
);
ADD_CASE_TO_GROUP("leading tabs squo unk 1",
R"(
 		'foo'
)",
N(VS, "foo")
);
ADD_CASE_TO_GROUP("leading tabs squo unk 2",
R"(
   		'foo'
)",
N(VS, "foo")
);


ADD_CASE_TO_GROUP("leading tabs squo seq 0", EXPECT_PARSE_ERROR,
R"(
-
		'foo'
)",
Location(3, 1)
);
ADD_CASE_TO_GROUP("leading tabs squo seq 1",
R"(
-
 		'foo'
)",
N(SB, L{N(VS, "foo")})
);
ADD_CASE_TO_GROUP("leading tabs squo seq 2",
R"(
-
   		'foo'
)",
N(SB, L{N(VS, "foo")})
);

ADD_CASE_TO_GROUP("leading tabs squo map 0", EXPECT_PARSE_ERROR,
R"(
a:
		'foo'
)",
Location(3, 1)
);
ADD_CASE_TO_GROUP("leading tabs squo map 1",
R"(
a:
 		'foo'
)",
N(MB, L{N(KP|VS, "a", "foo")})
);
ADD_CASE_TO_GROUP("leading tabs squo map 2",
R"(
a:
   		'foo'
)",
N(MB, L{N(KP|VS, "a", "foo")})
);



//-----------------------------------------------------------------------------

ADD_CASE_TO_GROUP("leading tabs literal unk 0",
R"(
		|
 foo
)",
N(VL, "foo\n")
);
ADD_CASE_TO_GROUP("leading tabs literal unk 1",
R"(
 		|
   foo
)",
N(VL, "foo\n")
);
ADD_CASE_TO_GROUP("leading tabs literal unk 2",
R"(
   		|
   foo
)",
N(VL, "foo\n")
);


ADD_CASE_TO_GROUP("leading tabs literal seq 0", EXPECT_PARSE_ERROR,
R"(
-
		|
   foo
)",
Location(3, 1)
);
ADD_CASE_TO_GROUP("leading tabs literal seq 1",
R"(
-
 		|
   foo
)",
N(SB, L{N(VL, "foo\n")})
);
ADD_CASE_TO_GROUP("leading tabs literal seq 2",
R"(
-
   		|
   foo
)",
N(SB, L{N(VL, "foo\n")})
);

ADD_CASE_TO_GROUP("leading tabs literal map 0", EXPECT_PARSE_ERROR,
R"(
a:
		|
   foo
)",
Location(3, 1)
);
ADD_CASE_TO_GROUP("leading tabs literal map 1",
R"(
a:
 		|
   foo
)",
N(MB, L{N(KP|VL, "a", "foo\n")})
);
ADD_CASE_TO_GROUP("leading tabs literal map 2",
R"(
a:
   		|
   foo
)",
N(MB, L{N(KP|VL, "a", "foo\n")})
);


//-----------------------------------------------------------------------------

ADD_CASE_TO_GROUP("leading tabs folded unk 0",
R"(
		>
 foo
)",
N(VF, "foo\n")
);
ADD_CASE_TO_GROUP("leading tabs folded unk 1",
R"(
 		>
   foo
)",
N(VF, "foo\n")
);
ADD_CASE_TO_GROUP("leading tabs folded unk 2",
R"(
   		>
   foo
)",
N(VF, "foo\n")
);


ADD_CASE_TO_GROUP("leading tabs folded seq 0", EXPECT_PARSE_ERROR,
R"(
-
		>
   foo
)",
Location(3, 1)
);
ADD_CASE_TO_GROUP("leading tabs folded seq 1",
R"(
-
 		>
   foo
)",
N(SB, L{N(VF, "foo\n")})
);
ADD_CASE_TO_GROUP("leading tabs folded seq 2",
R"(
-
   		>
   foo
)",
N(SB, L{N(VF, "foo\n")})
);

ADD_CASE_TO_GROUP("leading tabs folded map 0", EXPECT_PARSE_ERROR,
R"(
a:
		>
   foo
)",
Location(3, 1)
);
ADD_CASE_TO_GROUP("leading tabs folded map 1",
R"(
a:
 		>
   foo
)",
N(MB, L{N(KP|VF, "a", "foo\n")})
);
ADD_CASE_TO_GROUP("leading tabs folded map 2",
R"(
a:
   		>
   foo
)",
N(MB, L{N(KP|VF, "a", "foo\n")})
);


//-----------------------------------------------------------------------------

ADD_CASE_TO_GROUP("leading tabs, seq block unk 0", EXPECT_PARSE_ERROR,
R"(
  		 - a
  		 - b
  		 - c
)",
Location(2, 6)
);
ADD_CASE_TO_GROUP("leading tabs, seq block unk 1", EXPECT_PARSE_ERROR,
R"(
 		 - a
 		 - b
 		 - c
)",
Location(2, 5)
);
ADD_CASE_TO_GROUP("leading tabs, seq block unk 2", EXPECT_PARSE_ERROR,
R"(
		 - a
		 - b
		 - c
)",
Location(2, 4)
);


//-----------------------------------------------------------------------------

ADD_CASE_TO_GROUP("leading tabs, map block unk 0", EXPECT_PARSE_ERROR,
R"(
  		 a: v
  		 b: v
  		 c: v
)",
Location(2, 9)
);
ADD_CASE_TO_GROUP("leading tabs, map block unk 1", EXPECT_PARSE_ERROR,
R"(
 		 a: v
 		 b: v
 		 c: v
)",
Location(2, 8)
);
ADD_CASE_TO_GROUP("leading tabs, map block unk 2", EXPECT_PARSE_ERROR,
R"(
		 a: v
		 b: v
		 c: v
)",
Location(2, 7)
);


//-----------------------------------------------------------------------------

ADD_CASE_TO_GROUP("leading tabs, seq block nested in seq 0", EXPECT_PARSE_ERROR,
R"(
-
  		 - a
  		 - b
  		 - c
)",
Location(3, 6)
);
ADD_CASE_TO_GROUP("leading tabs, seq block nested in seq 1", EXPECT_PARSE_ERROR,
R"(
-
 		 - a
 		 - b
 		 - c
)",
Location(3, 5)
);
ADD_CASE_TO_GROUP("leading tabs, seq block nested in seq 2", EXPECT_PARSE_ERROR,
R"(
-
		 - a
		 - b
		 - c
)",
Location(3, 1)
);
ADD_CASE_TO_GROUP("leading tabs, seq block nested in seq 3", EXPECT_PARSE_ERROR,
R"(
-
 	 - a
 	 - b
 	 - c
)",
Location(3, 4)
);
ADD_CASE_TO_GROUP("leading tabs, seq block nested in seq 4", EXPECT_PARSE_ERROR,
R"(
-
  	 - a
  	 - b
  	 - c
)",
Location(3, 5)
);
ADD_CASE_TO_GROUP("leading tabs, seq block nested in seq 5", EXPECT_PARSE_ERROR,
R"(
-
   	 - a
   	 - b
   	 - c
)",
Location(3, 6)
);
ADD_CASE_TO_GROUP("leading tabs, seq block nested in seq 6",
R"(
-
    - a
    - b
    - c
)",
N(SB, L{N(SB, L{N(VP, "a"), N(VP, "b"), N(VP, "c")})})
);

ADD_CASE_TO_GROUP("leading tabs, seq block nested in map 0", EXPECT_PARSE_ERROR,
R"(
map:
  		 - a
  		 - b
  		 - c
)",
Location(3, 6)
);
ADD_CASE_TO_GROUP("leading tabs, seq block nested in map 1", EXPECT_PARSE_ERROR,
R"(
map:
 		 - a
 		 - b
 		 - c
)",
Location(3, 5)
);
ADD_CASE_TO_GROUP("leading tabs, seq block nested in map 2", EXPECT_PARSE_ERROR,
R"(
map:
		 - a
		 - b
		 - c
)",
Location(3, 1)
);




//-----------------------------------------------------------------------------

ADD_CASE_TO_GROUP("leading tabs, map block nested in seq 0", EXPECT_PARSE_ERROR,
R"(
-
  		 a: v
  		 b: v
  		 c: v
)",
Location(3, 9)
);
ADD_CASE_TO_GROUP("leading tabs, map block nested in seq 1", EXPECT_PARSE_ERROR,
R"(
-
 		 a: v
 		 b: v
 		 c: v
)",
Location(3, 8)
);
ADD_CASE_TO_GROUP("leading tabs, map block nested in seq 2", EXPECT_PARSE_ERROR,
R"(
-
		 a: v
		 b: v
		 c: v
)",
Location(3, 1)
);
ADD_CASE_TO_GROUP("leading tabs, map block nested in seq 3", EXPECT_PARSE_ERROR,
R"(
-
 	 a: v
 	 b: v
 	 c: v
)",
Location(3, 7)
);
ADD_CASE_TO_GROUP("leading tabs, map block nested in seq 4", EXPECT_PARSE_ERROR,
R"(
-
  	 a: v
  	 b: v
  	 c: v
)",
Location(3, 8)
);
ADD_CASE_TO_GROUP("leading tabs, map block nested in seq 5", EXPECT_PARSE_ERROR,
R"(
-
   	 a: v
   	 b: v
   	 c: v
)",
Location(3, 9)
);
ADD_CASE_TO_GROUP("leading tabs, map block nested in seq 6",
R"(
-
    a: v
    b: v
    c: v
)",
N(SB, L{N(MB, L{N(KP|VP, "a", "v"), N(KP|VP, "b", "v"), N(KP|VP, "c", "v")})})
);

ADD_CASE_TO_GROUP("leading tabs, map block nested in map 0", EXPECT_PARSE_ERROR,
R"(
map:
  		 a: v
  		 b: v
  		 c: v
)",
Location(3, 9)
);
ADD_CASE_TO_GROUP("leading tabs, map block nested in map 1", EXPECT_PARSE_ERROR,
R"(
map:
 		 a: v
 		 b: v
 		 c: v
)",
Location(3, 8)
);
ADD_CASE_TO_GROUP("leading tabs, map block nested in map 2", EXPECT_PARSE_ERROR,
R"(
map:
		 a: v
		 b: v
		 c: v
)",
Location(3, 1)
);


//-----------------------------------------------------------------------------

ADD_CASE_TO_GROUP("leading tabs, seq nested with tabs, 1.0", EXPECT_PARSE_ERROR,
R"(
- - a
- 	- a
)",
Location(3, 4)
);
ADD_CASE_TO_GROUP("leading tabs, seq nested with tabs, 1.1", EXPECT_PARSE_ERROR,
R"(
- - a
- 	 - a
)",
Location(3, 5)
);
ADD_CASE_TO_GROUP("leading tabs, seq nested with tabs, 1.2", EXPECT_PARSE_ERROR,
R"(
- - a
-  	  - a
)",
Location(3, 7)
);

ADD_CASE_TO_GROUP("leading tabs, seq nested with tabs, 2.0", EXPECT_PARSE_ERROR,
R"(
- - - a
- - 	- a
)",
Location(3, 6)
);
ADD_CASE_TO_GROUP("leading tabs, seq nested with tabs, 2.1", EXPECT_PARSE_ERROR,
R"(
- - - a
- - 	 - a
)",
Location(3, 7)
);
ADD_CASE_TO_GROUP("leading tabs, seq nested with tabs, 2.2", EXPECT_PARSE_ERROR,
R"(
- - - a
- -  	  - a
)",
Location(3, 9)
);

ADD_CASE_TO_GROUP("leading tabs, seq nested with tabs, 3.0", EXPECT_PARSE_ERROR,
R"(
- - - - a
- - - 	- a
)",
Location(3, 8)
);
ADD_CASE_TO_GROUP("leading tabs, seq nested with tabs, 3.1", EXPECT_PARSE_ERROR,
R"(
- - - - a
- - - 	 - a
)",
Location(3, 9)
);
ADD_CASE_TO_GROUP("leading tabs, seq nested with tabs, 3.2", EXPECT_PARSE_ERROR,
R"(
- - - - a
- - -  	  - a
)",
Location(3, 11)
);


//-----------------------------------------------------------------------------
ADD_CASE_TO_GROUP("test suite Y79Y/000", EXPECT_PARSE_ERROR,
R"(
foo: |
	
bar: 1
)",
Location(3,1)
);
ADD_CASE_TO_GROUP("test suite Y79Y/000 2",
R"(
foo: |
 	
bar: 1
)",
N(MB, L{
  N(KP|VL, "foo", "\t\n"),
  N(KP|VP, "bar", "1"),
})
);
ADD_CASE_TO_GROUP("test suite Y79Y/002",
R"(
- [
	
 foo
 ]
)",
N(SB, L{N(SFM, L{N(VP, "foo")})})
);
ADD_CASE_TO_GROUP("test suite Y79Y/003", EXPECT_PARSE_ERROR,
R"(
- [
	foo,
 foo
 ]
)",
Location(3, 1)
);


} // CASE_GROUP(INDENTATION)

} // namespace yml
} // namespace c4
