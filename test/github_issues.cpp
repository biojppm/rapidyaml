#include "./test_group.hpp"

namespace c4 {
namespace yml {

TEST(github, 58)
{
    Tree tree;
    NodeRef root;

    tree = parse("{a}");
    root = tree.rootref();
    EXPECT_EQ(root["a"].val(), "~");

    tree = parse("{a, b, c}");
    root = tree.rootref();
    EXPECT_EQ(root["a"].val(), "~");
    EXPECT_EQ(root["b"].val(), "~");
    EXPECT_EQ(root["c"].val(), "~");

    tree = parse("{a: 1, b: 2, c}");
    root = tree.rootref();
    EXPECT_EQ(root["a"].val(), "1");
    EXPECT_EQ(root["b"].val(), "2");
    EXPECT_EQ(root["c"].val(), "~");

    tree = parse("{a: 1, b, c: 2}");
    root = tree.rootref();
    EXPECT_EQ(root["a"].val(), "1");
    EXPECT_EQ(root["b"].val(), "~");
    EXPECT_EQ(root["c"].val(), "2");

    tree = parse("{a, b: 1, c: 2}");
    root = tree.rootref();
    EXPECT_EQ(root["a"].val(), "~");
    EXPECT_EQ(root["b"].val(), "1");
    EXPECT_EQ(root["c"].val(), "2");
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(github, 54)
{
    Tree tree = parse(R"(
foo   : crl
keyA :
    keyA.B : test value
"key C"     : val C
'key D'     : val D
elm2     :
    "key C"     : val C
    'key D'     : val D
    key E       : val E
elm3     :
    'key D'     : val D
    "key C"     : val C
    key E       : val E
elm4     :
    key E       : val E
    'key D'     : val D
    "key C"     : val C
)");
    EXPECT_TRUE(!tree.empty());
    auto root = tree.rootref();
    EXPECT_EQ(root.type(), MAP);
    EXPECT_EQ(root.num_children(), 7u);

    EXPECT_TRUE(root.has_child("keyA"));
    auto keyA = root["keyA"];
    EXPECT_TRUE(keyA.is_map());
    EXPECT_EQ(keyA.num_children(), 1u);
    EXPECT_TRUE(keyA.has_child("keyA.B"));
    auto keyA_B = keyA["keyA.B"];
    EXPECT_EQ(keyA_B.type(), KEYVAL);
    EXPECT_EQ(keyA_B.val(), "test value");

    EXPECT_EQ(root["key C"].val(), "val C");
    EXPECT_EQ(root["key D"].val(), "val D");

    EXPECT_TRUE(root.has_child("elm2"));
    auto elm2 = root["elm2"];
    EXPECT_TRUE(elm2.is_map());
    EXPECT_EQ(elm2.num_children(), 3u);
    EXPECT_TRUE(elm2.has_child("key C"));
    EXPECT_EQ(elm2["key C"].val(), "val C");
    EXPECT_TRUE(elm2.has_child("key D"));
    EXPECT_EQ(elm2["key D"].val(), "val D");
    EXPECT_TRUE(elm2.has_child("key E"));
    EXPECT_EQ(elm2["key E"].val(), "val E");

    EXPECT_TRUE(root.has_child("elm3"));
    auto elm3 = root["elm3"];
    EXPECT_TRUE(elm3.is_map());
    EXPECT_EQ(elm3.num_children(), 3u);
    EXPECT_TRUE(elm3.has_child("key C"));
    EXPECT_EQ(elm3["key C"].val(), "val C");
    EXPECT_TRUE(elm3.has_child("key D"));
    EXPECT_EQ(elm3["key D"].val(), "val D");
    EXPECT_TRUE(elm3.has_child("key E"));
    EXPECT_EQ(elm3["key E"].val(), "val E");

    EXPECT_TRUE(root.has_child("elm4"));
    auto elm4 = root["elm4"];
    EXPECT_TRUE(elm4.is_map());
    EXPECT_EQ(elm4.num_children(), 3u);
    EXPECT_TRUE(elm4.has_child("key C"));
    EXPECT_EQ(elm4["key C"].val(), "val C");
    EXPECT_TRUE(elm4.has_child("key D"));
    EXPECT_EQ(elm4["key D"].val(), "val D");
    EXPECT_TRUE(elm4.has_child("key E"));
    EXPECT_EQ(elm4["key E"].val(), "val E");
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(github, 31)
{
    Tree tree;
    NodeRef r = tree.rootref();
    r |= MAP;

    auto meas = r["meas"];
    meas |= MAP;

    auto plist = meas["createParameterList"];
    plist |= SEQ;

    {
        auto lumi = plist.append_child();
        lumi << "Lumi";
        EXPECT_TRUE(lumi.is_val());
    }

    {
        auto lumi = plist.append_child();
        lumi |= MAP;
        lumi["value"] << 1;
        lumi["relErr"] << 0.1;
        EXPECT_TRUE(lumi.is_map());
    }

    {
        #if RYML_USE_ASSERT
        ExpectError::do_check([&](){
            auto lumi = plist.append_child();
            lumi << "Lumi";
            lumi |= MAP;
        });
        #endif // RYML_USE_ASSERT
    }

    {
        #if RYML_USE_ASSERT
        ExpectError::do_check([&](){
            auto lumi = plist.append_child();
            lumi << "Lumi";
            lumi |= SEQ;
        });
        #endif // RYML_USE_ASSERT
    }

    {
        #if RYML_USE_ASSERT
        ExpectError::do_check([&](){
            auto lumi = plist.append_child();
            lumi |= MAP;
            lumi << "Lumi";
        });
        #endif // RYML_USE_ASSERT
    }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#define GITHUB_ISSUES_CASES \
        "github3-problem1",\
        "github3-problem2-ex1",\
        "github3-problem2-ex2",\
        "github3-problem3",\
        /*"github3",*/\
        "github6-problem1",\
        "github6",\
        "github34/ex1",\
        "github34/ex2",\
        "github34",\
        "github35/expected_error1",\
        "github35/expected_error2"


CASE_GROUP(GITHUB_ISSUES)
{
    APPEND_CASES(

C("github3-problem1",
R"(
translation: [-2, -2, 5])",
L{N("translation", L{N("-2"), N("-2"), N("5")})}
),

// these must work without quotes
C("github3-problem2-ex1",
R"(
audio resource: 
)",
L{N("audio resource", "~")}
),
C("github3-problem2-ex2",
R"(
audio resource: 
more:
  example: y
)",
L{N("audio resource", "~"), N("more", L{N("example", "y")})}
),

C("github3-problem3",
R"(component:
  type: perspective camera component
  some_data: {}  # this was working
  data:
    {}           # but this was not working
)",
L{N("component", L{
        N("type", "perspective camera component"),
        N(KEYMAP, "some_data", L{}),
        N(KEYMAP, "data", L{})
    }
)}
),

/* THIS IS CAUSING VS TO CRASH OUT OF HEAP SPACE
C("github3",
R"(
universe:
  objects:
    object:
      uuid: A7AB039C0EF3A74480A1B398247039A7
      components:
        - component:
            type: name component
            data:
              object name: Root Node
        - component:
            type: transform component
            data:
              translation: [-2, -2, 5]
              rotation: [0, 0, 0, 1]
              scaling: [1, 1, 1]
        - component:
            type: perspective camera component
            data:
             {}
        - component:
            type: mesh component
            data:
              mesh resource: TODO
        - component:
            type: lua script component
            data:
             {}
        - component:
            type: audio component
            data:
              audio resource: ''
              type: 0
              current sample: 184102
              spatialized: true
      children:
        - object:
            uuid: E1C364A925D649408E83C8EEF5179A87
            components:
              - component:
                  type: name component
                  data:
                    object name: Prepend
            children:
              []
        - object:
            uuid: 377DBA885AF4CD42B8A56BB3471F60E5
            components:
              - component:
                  type: name component
                  data:
                    object name: pivot
            children:
              []
        - object:
            uuid: 6DD1835797DADB4F95232CE7E9DE41BA
            components:
              - component:
                  type: name component
                  data:
                    object name: Append
            children:
              []
)",
  L{N("universe", L{
        N("objects", L{
            N("object", L{
                N("uuid", "A7AB039C0EF3A74480A1B398247039A7"),
                N("components", L{
                    N(L{N("component", L{N("type", "name component"), N("data", L{N("object name", "Root Node")}), }), }),
                    N(L{N("component", L{N("type", "transform component"), N("data", L{N("translation", L{N("-2"), N("-2"), N("5")}), N("rotation", L{N("0"), N("0"), N("0"), N("1")}), N("scaling", L{N("1"), N("1"), N("1")}),}), }), }),
                    N(L{N("component", L{N("type", "perspective camera component"), N(KEYMAP, "data", L{}), }), }),
                    N(L{N("component", L{N("type", "mesh component"), N("data", L{N("mesh resource", "TODO")}), }), }),
                    N(L{N("component", L{N("type", "lua script component"), N(KEYMAP, "data", L{}), }), }),
                    N(L{N("component", L{N("type", "audio component"), N("data", L{N("audio resource", ""), N("type", "0"), N("current sample", "184102"), N("spatialized", "true"), }), }), }), // component
                  }), // components
                N("children", L{
                    N(L{N("object", L{
                        N("uuid", "E1C364A925D649408E83C8EEF5179A87"),
                        N("components", L{N(L{N("component", L{N("type", "name component"), N("data", L{N("object name", "Prepend")}), }), }), }),
                        N(KEYSEQ, "children", L{}),
                    }), }), // object
                    N(L{N("object", L{
                        N("uuid", "377DBA885AF4CD42B8A56BB3471F60E5"),
                        N("components", L{N(L{N("component", L{N("type", "name component"), N("data", L{N("object name", "pivot")}), }), }), }),
                        N(KEYSEQ, "children", L{}),
                    }), }), // object
                    N(L{N("object", L{
                        N("uuid", "6DD1835797DADB4F95232CE7E9DE41BA"),
                        N("components", L{N(L{N("component", L{N("type", "name component"), N("data", L{N("object name", "Append")}), }), }), }),
                        N(KEYSEQ, "children", L{}),
                    }), }), // object
                  }), // children
                }), // object
              }) // objects
          }) // universe
      }
),
*/

C("github6-problem1",
R"(
- UQxRibHKEDI:
  - 0.mp4
  - 1.mp4
  - 2.mp4
  - 3.mp4
- DcYsg8VFdC0:
  - 0.mp4
  - 1.mp4
  - 2.mp4
  - 3.mp4
- Yt3ymqZXzLY:
  - 0.mp4
  - 1.mp4
  - 2.mp4
  - 3.mp4
)",
L{
N(L{N("UQxRibHKEDI", L{N("0.mp4"), N("1.mp4"), N("2.mp4"), N("3.mp4")})}),
N(L{N("DcYsg8VFdC0", L{N("0.mp4"), N("1.mp4"), N("2.mp4"), N("3.mp4")})}),
N(L{N("Yt3ymqZXzLY", L{N("0.mp4"), N("1.mp4"), N("2.mp4"), N("3.mp4")})}),
}
),

C("github6",
R"(videos:
- UQxRibHKEDI:
  - 0.mp4
  - 1.mp4
  - 2.mp4
  - 3.mp4
- DcYsg8VFdC0:
  - 0.mp4
  - 1.mp4
  - 2.mp4
  - 3.mp4
- Yt3ymqZXzLY:
  - 0.mp4
  - 1.mp4
  - 2.mp4
  - 3.mp4
)",
L{N("videos", L{
N(L{N("UQxRibHKEDI", L{N("0.mp4"), N("1.mp4"), N("2.mp4"), N("3.mp4")})}),
N(L{N("DcYsg8VFdC0", L{N("0.mp4"), N("1.mp4"), N("2.mp4"), N("3.mp4")})}),
N(L{N("Yt3ymqZXzLY", L{N("0.mp4"), N("1.mp4"), N("2.mp4"), N("3.mp4")})}),
})}
),

C("github34/ex1",
R"(
# correct:
MessageID1: 'MapRegion_HyrulePrairie'
MessageID2: "MapRegion_HyrulePrairie"
MessageID3: 'MapRegion_HyrulePrairie'
MessageID4: "MapRegion_HyrulePrairie"
# incorrect: uninitialised memory?
MessageID5:  'MapRegion_HyrulePrairie'
MessageID6:  "MapRegion_HyrulePrairie"
MessageID7:   'MapRegion_HyrulePrairie'
MessageID8:   "MapRegion_HyrulePrairie"
MessageID9:          'MapRegion_HyrulePrairie'
MessageID0:          "MapRegion_HyrulePrairie"
)",
L{
  N("MessageID1", "MapRegion_HyrulePrairie"),
  N("MessageID2", "MapRegion_HyrulePrairie"),
  N("MessageID3", "MapRegion_HyrulePrairie"),
  N("MessageID4", "MapRegion_HyrulePrairie"),
  N("MessageID5", "MapRegion_HyrulePrairie"),
  N("MessageID6", "MapRegion_HyrulePrairie"),
  N("MessageID7", "MapRegion_HyrulePrairie"),
  N("MessageID8", "MapRegion_HyrulePrairie"),
  N("MessageID9", "MapRegion_HyrulePrairie"),
  N("MessageID0", "MapRegion_HyrulePrairie"),
}
),

C("github34/ex2",
R"(
# correct:
- MessageID1: 'MapRegion_HyrulePrairie'
- MessageID2: "MapRegion_HyrulePrairie"
-  MessageID3: 'MapRegion_HyrulePrairie'
-  MessageID4: "MapRegion_HyrulePrairie"
# incorrect: uninitialised memory?
- MessageID5:  'MapRegion_HyrulePrairie'
- MessageID6:  "MapRegion_HyrulePrairie"
- MessageID7:   'MapRegion_HyrulePrairie'
- MessageID8:   "MapRegion_HyrulePrairie"
-  MessageID9:          'MapRegion_HyrulePrairie'
-  MessageID0:          "MapRegion_HyrulePrairie"
)",
L{
  N(L{N("MessageID1", "MapRegion_HyrulePrairie")}),
  N(L{N("MessageID2", "MapRegion_HyrulePrairie")}),
  N(L{N("MessageID3", "MapRegion_HyrulePrairie")}),
  N(L{N("MessageID4", "MapRegion_HyrulePrairie")}),
  N(L{N("MessageID5", "MapRegion_HyrulePrairie")}),
  N(L{N("MessageID6", "MapRegion_HyrulePrairie")}),
  N(L{N("MessageID7", "MapRegion_HyrulePrairie")}),
  N(L{N("MessageID8", "MapRegion_HyrulePrairie")}),
  N(L{N("MessageID9", "MapRegion_HyrulePrairie")}),
  N(L{N("MessageID0", "MapRegion_HyrulePrairie")}),
}
),

C("github34",
R"(
# incorrect: uninitialised memory?
-  MessageID1:          'MapRegion_HyrulePrairie'
-  MessageID2:          "MapRegion_HyrulePrairie"

# incorrect: uninitialised memory?
-  MessageID3:          'MapRegion_HyrulePrairie '
-  MessageID4:          "MapRegion_HyrulePrairie "

# incorrect: for some reason the ' is included in the string
-  MessageID5: 'MapRegion_HyrulePrairie '
- MessageID6: 'MapRegion_HyrulePrairie '
-  MessageID7: "MapRegion_HyrulePrairie "
- MessageID8: "MapRegion_HyrulePrairie "

# incorrect: same issue
- MessageID9: 'MapRegion_HyrulePrairie '
- MessageID10: "MapRegion_HyrulePrairie "

# incorrect: still has the trailing quote
- MessageID11: 'MapRegion_HyrulePrairie'
- MessageID12: "MapRegion_HyrulePrairie"

# the string is parsed correctly in this case
- key1: true1
  MessageID1:          'MapRegion_HyrulePrairie1 '
- key2: true2
  MessageID2:          "MapRegion_HyrulePrairie2 "
)",
L{
  N(L{N("MessageID1",  "MapRegion_HyrulePrairie")}),
  N(L{N("MessageID2",  "MapRegion_HyrulePrairie")}),
  N(L{N("MessageID3",  "MapRegion_HyrulePrairie ")}),
  N(L{N("MessageID4",  "MapRegion_HyrulePrairie ")}),
  N(L{N("MessageID5",  "MapRegion_HyrulePrairie ")}),
  N(L{N("MessageID6",  "MapRegion_HyrulePrairie ")}),
  N(L{N("MessageID7",  "MapRegion_HyrulePrairie ")}),
  N(L{N("MessageID8",  "MapRegion_HyrulePrairie ")}),
  N(L{N("MessageID9",  "MapRegion_HyrulePrairie ")}),
  N(L{N("MessageID10", "MapRegion_HyrulePrairie ")}),
  N(L{N("MessageID11", "MapRegion_HyrulePrairie")}),
  N(L{N("MessageID12", "MapRegion_HyrulePrairie")}),
  N(L{N("key1", "true1"), N("MessageID1", "MapRegion_HyrulePrairie1 ")}),
  N(L{N("key2", "true2"), N("MessageID2", "MapRegion_HyrulePrairie2 ")}),
}
),

C("github35/expected_error1", HAS_PARSE_ERROR,
R"(
# *segfault* // not anymore!
- key1: true1
 MessageID1:          'MapRegion_HyrulePrairie1 '
- key2: true2
 MessageID2:          "MapRegion_HyrulePrairie2 "
)",
L{
  N(L{N("key1", "true1"), N("MessageID1", "MapRegion_HyrulePrairie1 ")}),
  N(L{N("key2", "true2"), N("MessageID2", "MapRegion_HyrulePrairie2 ")}),
}
),

C("github35/expected_error2", HAS_PARSE_ERROR,
R"(
# *segfault* // not anymore!
- key1: true1
    MessageID1:          'MapRegion_HyrulePrairie1 '
- key2: true2
    MessageID2:          "MapRegion_HyrulePrairie2 "
)",
L{
  N(L{N("key1", "true1"), N("MessageID1", "MapRegion_HyrulePrairie1 ")}),
  N(L{N("key2", "true2"), N("MessageID2", "MapRegion_HyrulePrairie2 ")}),
}
),

    )
}

INSTANTIATE_GROUP(GITHUB_ISSUES)

} // namespace yml
} // namespace c4
