#include "./test_lib/test_group.hpp"
#include "./test_lib/test_group.def.hpp"

namespace c4 {
namespace yml {

TEST(github, 455_0_ok)
{
    Tree t;
    ExpectError::check_success([&t]{
        parse_in_arena(R"(
  processors:
    - simple: a
  sampler:
    trace_id_ratio_based:
      ratio:
)", &t);
    });
    ConstNodeRef sampler = t["sampler"];
    ASSERT_TRUE(sampler.is_map());
    ASSERT_TRUE(sampler.has_child("trace_id_ratio_based"));
    ASSERT_TRUE(sampler["trace_id_ratio_based"].has_child("ratio"));
}

TEST(github, 455_0)
{
    Tree t;
    ExpectError::check_success([&t]{
        parse_in_arena(R"(
  processors:
    - simple:
  sampler:
    trace_id_ratio_based:
      ratio:
)", &t);
    });
    ConstNodeRef sampler = t["sampler"];
    ASSERT_TRUE(sampler.is_map());
    ASSERT_TRUE(sampler.has_child("trace_id_ratio_based"));
    ASSERT_TRUE(sampler["trace_id_ratio_based"].has_child("ratio"));
}

TEST(github, 455_1)
{
    Tree t;
    ExpectError::check_success([&t]{
        parse_in_arena(R"(
file_format: 0.0
tracer_provider:
  processors:
    - simple:
        exporter:
          console:
  sampler:
    trace_id_ratio_based:
      ratio:
    )", &t);
    });
    ConstNodeRef sampler = t["tracer_provider"]["sampler"];
    ASSERT_TRUE(sampler.is_map());
    ASSERT_TRUE(sampler.has_child("trace_id_ratio_based"));
    ASSERT_TRUE(sampler["trace_id_ratio_based"].has_child("ratio"));
}


//-----------------------------------------------------------------------------

TEST(github, 268)
{
    Tree tree = parse_in_arena(R"(
        list:
            - &bar bar
        map:
            node: *bar
    )");
    tree.resolve();
    ConstNodeRef root = tree.crootref();
    ASSERT_TRUE(root["map"].is_map());
    ASSERT_TRUE(root["map"].has_child("node"));
    ASSERT_EQ(root["map"]["node"], "bar");
}

TEST(github, 277)
{
    Tree tree = parse_in_arena(R"(
    A: &A
        V: 3
        W: 4
    B:
        <<: *A
        V: 5
        X: 6
    )");
    const char *keys[] = {"V", "W", "X"};
    const char *vals[] = {"5", "4", "6"};
    #ifdef RYML_DBG
    print_tree("parsed", tree);
    #endif
    test_invariants(tree);
    tree.resolve();
    #ifdef RYML_DBG
    print_tree("resolved", tree);
    #endif
    ConstNodeRef root = tree.crootref();
    ASSERT_TRUE(root["B"].is_map());
    id_type num_childs = root["B"].num_children();
    id_type child = 0;
    ASSERT_EQ(num_childs, 3);
    for (const auto node : root["B"].children())
    {
        EXPECT_EQ(node.key(), csubstr(keys[child], 1));
        EXPECT_EQ(node.val(), csubstr(vals[child], 1));
        child++;
    }
    // test whether the tree is corrupted
    test_invariants(tree);
    child = num_childs;
    for (id_type n = tree.last_child(root["B"].id()); n != NONE; n = tree.prev_sibling(n))
    {
        ASSERT_NE(child, 0);
        EXPECT_EQ(tree.key(n), csubstr(keys[child - 1], 1));
        child--;
    }
}


TEST(github, 78)
{
    Tree t = parse_in_arena("{foo: 1, bar: [2, 3]}");
    EXPECT_EQ(t["foo"].val(), "1");
    EXPECT_EQ(t["bar"][0].val(), "2");
    EXPECT_EQ(t["bar"][1].val(), "3");
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(github, 60)
{
    Tree tree = parse_in_arena(R"(
    traits:
        roleBonuses:
        -   bonus: 5
            bonusText:
                de: Bonus auf die Virusstärke von <a href=showinfo:22177>Relikt-</a>
                    und <a href=showinfo:22175>Datenanalysatoren</a>
                en: bonus to <a href=showinfo:22177>Relic</a> and <a href=showinfo:22175>Data
                    Analyzer</a> virus strength
                fr: de bonus à la puissance du virus des <a href=showinfo:22177>analyseurs
                    de reliques</a> et des <a href=showinfo:22175>analyseurs de données</a>
                ja: <a href=showinfo:22177>遺物アナライザー</a>と<a href=showinfo:22175>データアナライザー</a>のウイルス強度が増加
                ru: повышается степень опасности вирусов, применяемых в <a href=showinfo:22175>комплексах
                    анализа данных</a> и <a href=showinfo:22177>комплексах анализа
                    артефактов</a>
                zh: <a href="showinfo:22177">遗迹分析仪</a>和<a href="showinfo:22175">数据分析仪</a>病毒强度加成
            importance: 1
            unitID: 139
)");
    auto root = tree.rootref();
    ASSERT_TRUE(root.is_map());
    ASSERT_TRUE(root.has_child("traits"));
    auto rb = root["traits"]["roleBonuses"][0];
    ASSERT_FALSE(rb.invalid());
    ASSERT_TRUE(rb.readable());
    EXPECT_EQ(rb["bonus"].val(), "5");
    auto txt = rb["bonusText"];
    ASSERT_TRUE(txt.readable());
    ASSERT_TRUE(txt.is_map());
    EXPECT_TRUE(txt.has_child("de"));
    EXPECT_TRUE(txt.has_child("en"));
    EXPECT_TRUE(txt.has_child("fr"));
    EXPECT_TRUE(txt.has_child("ja"));
    EXPECT_TRUE(txt.has_child("ru"));
    EXPECT_TRUE(txt.has_child("zh"));
    EXPECT_EQ(txt["de"].val(), "Bonus auf die Virusstärke von <a href=showinfo:22177>Relikt-</a> und <a href=showinfo:22175>Datenanalysatoren</a>");
    EXPECT_EQ(txt["en"].val(), "bonus to <a href=showinfo:22177>Relic</a> and <a href=showinfo:22175>Data Analyzer</a> virus strength");
    EXPECT_EQ(txt["fr"].val(), "de bonus à la puissance du virus des <a href=showinfo:22177>analyseurs de reliques</a> et des <a href=showinfo:22175>analyseurs de données</a>");
    EXPECT_EQ(txt["ja"].val(), "<a href=showinfo:22177>遺物アナライザー</a>と<a href=showinfo:22175>データアナライザー</a>のウイルス強度が増加");
    EXPECT_EQ(txt["ru"].val(), "повышается степень опасности вирусов, применяемых в <a href=showinfo:22175>комплексах анализа данных</a> и <a href=showinfo:22177>комплексах анализа артефактов</a>");
    EXPECT_EQ(txt["zh"].val(), "<a href=\"showinfo:22177\">遗迹分析仪</a>和<a href=\"showinfo:22175\">数据分析仪</a>病毒强度加成");


    tree = parse_in_arena(R"(208:
    basePrice: 3000.0
    description:
        de: Ursprünglich als Rakete für den Fangschuss entworfen, um einem beschädigten
            Schiff den Todesstoß zu geben, hat die Inferno Heavy Missile seither eine
            Reihe technischer Upgrades durchlaufen. Die neueste Version hat eine leichtere
            Sprengladung als das Original, aber stark verbesserte Lenksysteme.
        en: Originally designed as a 'finisher' - the killing blow to a crippled ship
            - the Inferno heavy missile has since gone through various technological
            upgrades. The latest version has a lighter payload than the original,
            but much improved guidance systems.
        fr: Conçu à l'origine pour donner le coup de grâce, le missile lourd Inferno
            a depuis subi de nombreuses améliorations techniques. La dernière version
            emporte une charge utile réduite par rapport à l'originale, mais est dotée
            de systèmes de guidage améliorés.
        ja: 元々「フィニッシャー」―大破した船にとどめを刺す兵器として設計されたインフェルノヘビーミサイルは、以来各種の技術改良を経てきた。現行型は初期型より軽い弾頭を採用しているが、それを補って余りある優れた誘導システムを持つ。
        ru: Тяжелая ракета Inferno изначально была спроектирована как «оружие последнего
            удара» для уничтожения подбитых кораблей. С тех пор было выпущено несколько
            ее модификаций. В последней модификации используется заряд меньшей мощности,
            но более совершенная система наведения.
        zh: 炼狱重型导弹历经多种技术改良，原本被设计为给予落魄敌舰最后一击的“终结者”角色。相比原型，最新版导弹载荷较轻，但装配了大幅改进的制导系统。
    graphicID: 20048
    groupID: 385
    iconID: 188
    marketGroupID: 924
    mass: 1000.0
    name:
        de: Inferno Heavy Missile
        en: Inferno Heavy Missile
        fr: Missile lourd Inferno
        ja: インフェルノヘビーミサイル
        ru: Inferno Heavy Missile
        zh: 炼狱重型导弹
    portionSize: 100
    published: true
    radius: 300.0
    volume: 0.03
)");
    root = tree.rootref()["208"];
    EXPECT_EQ(root["description"]["ja"].val(), "元々「フィニッシャー」―大破した船にとどめを刺す兵器として設計されたインフェルノヘビーミサイルは、以来各種の技術改良を経てきた。現行型は初期型より軽い弾頭を採用しているが、それを補って余りある優れた誘導システムを持つ。");
    EXPECT_EQ(root["name"]["ja"].val(), "インフェルノヘビーミサイル");
    EXPECT_EQ(root["name"]["zh"].val(), "炼狱重型导弹");
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
        NodeRef lumi = plist.append_child();
        lumi << "Lumi";
        EXPECT_TRUE(lumi.is_val());
    }

    {
        NodeRef lumi = plist.append_child();
        lumi |= MAP;
        lumi["value"] << 1;
        lumi["relErr"] << 0.1;
        EXPECT_TRUE(lumi.is_map());
    }

    {
        ExpectError::check_assertion(&tree, [&](){
            NodeRef lumi = plist.append_child();
            lumi << "Lumi";
            lumi |= MAP;
        });
    }

    {
        ExpectError::check_assertion(&tree, [&](){
            NodeRef lumi = plist.append_child();
            lumi << "Lumi";
            lumi |= SEQ;
        });
    }

    {
        ExpectError::check_assertion(&tree, [&](){
            NodeRef lumi = plist.append_child();
            lumi |= MAP;
            lumi << "Lumi";
        });
    }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

CASE_GROUP(GITHUB_ISSUES)
{

ADD_CASE_TO_GROUP("github3-problem1",
R"(
translation: [-2, -2, 5])",
N(MB, L{N(KP|SFS, "translation", L{N(VP, "-2"), N(VP, "-2"), N(VP, "5")})})
);

// these must work without quotes
ADD_CASE_TO_GROUP("github3-problem2-ex1",
R"(
audio resource:
)",
N(MB, L{N(KP|VP, "audio resource", /*"~"*/{})})
);

ADD_CASE_TO_GROUP("github3-problem2-ex2",
R"(
audio resource:
more:
  example: y
)",
N(MB, L{N(KP|VP, "audio resource", /*"~"*/{}), N(KP|MB, "more", L{N(KP|VP, "example", "y")})})
);

ADD_CASE_TO_GROUP("github3-problem3",
R"(component:
  type: perspective camera component
  some_data: {}  # this was working
  data:
    {}           # but this was not working
)",
N(MB, L{
  N(KP|MB, "component", L{
    N(KP|VP, "type", "perspective camera component"),
    N(KP|MFS, "some_data", L{}),
    N(KP|MFS, "data", L{})
    }
)})
);

/* THIS IS CAUSING VS TO CRASH OUT OF HEAP SPACE
ADD_CASE_TO_GROUP("github3",
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
);
*/

ADD_CASE_TO_GROUP("github6-problem1",
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
N(SB, L{
N(MB, L{N(KP|SB, "UQxRibHKEDI", L{N(VP,"0.mp4"), N(VP,"1.mp4"), N(VP,"2.mp4"), N(VP,"3.mp4")})}),
N(MB, L{N(KP|SB, "DcYsg8VFdC0", L{N(VP,"0.mp4"), N(VP,"1.mp4"), N(VP,"2.mp4"), N(VP,"3.mp4")})}),
N(MB, L{N(KP|SB, "Yt3ymqZXzLY", L{N(VP,"0.mp4"), N(VP,"1.mp4"), N(VP,"2.mp4"), N(VP,"3.mp4")})}),
})
);

ADD_CASE_TO_GROUP("github6",
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
N(MB, L{
  N(KP|SB, "videos", L{
    N(MB, L{N(KP|SB, "UQxRibHKEDI", L{N(VP,"0.mp4"), N(VP,"1.mp4"), N(VP,"2.mp4"), N(VP,"3.mp4")})}),
    N(MB, L{N(KP|SB, "DcYsg8VFdC0", L{N(VP,"0.mp4"), N(VP,"1.mp4"), N(VP,"2.mp4"), N(VP,"3.mp4")})}),
    N(MB, L{N(KP|SB, "Yt3ymqZXzLY", L{N(VP,"0.mp4"), N(VP,"1.mp4"), N(VP,"2.mp4"), N(VP,"3.mp4")})}),
  })
})
);

ADD_CASE_TO_GROUP("github34/ex1",
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
N(MB, L{
  N(KP|VS, "MessageID1", "MapRegion_HyrulePrairie"),
  N(KP|VD, "MessageID2", "MapRegion_HyrulePrairie"),
  N(KP|VS, "MessageID3", "MapRegion_HyrulePrairie"),
  N(KP|VD, "MessageID4", "MapRegion_HyrulePrairie"),
  N(KP|VS, "MessageID5", "MapRegion_HyrulePrairie"),
  N(KP|VD, "MessageID6", "MapRegion_HyrulePrairie"),
  N(KP|VS, "MessageID7", "MapRegion_HyrulePrairie"),
  N(KP|VD, "MessageID8", "MapRegion_HyrulePrairie"),
  N(KP|VS, "MessageID9", "MapRegion_HyrulePrairie"),
  N(KP|VD, "MessageID0", "MapRegion_HyrulePrairie"),
})
);

ADD_CASE_TO_GROUP("github34/ex2",
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
N(SB, L{
  N(MB, L{N(KP|VS, "MessageID1", "MapRegion_HyrulePrairie")}),
  N(MB, L{N(KP|VD, "MessageID2", "MapRegion_HyrulePrairie")}),
  N(MB, L{N(KP|VS, "MessageID3", "MapRegion_HyrulePrairie")}),
  N(MB, L{N(KP|VD, "MessageID4", "MapRegion_HyrulePrairie")}),
  N(MB, L{N(KP|VS, "MessageID5", "MapRegion_HyrulePrairie")}),
  N(MB, L{N(KP|VD, "MessageID6", "MapRegion_HyrulePrairie")}),
  N(MB, L{N(KP|VS, "MessageID7", "MapRegion_HyrulePrairie")}),
  N(MB, L{N(KP|VD, "MessageID8", "MapRegion_HyrulePrairie")}),
  N(MB, L{N(KP|VS, "MessageID9", "MapRegion_HyrulePrairie")}),
  N(MB, L{N(KP|VD, "MessageID0", "MapRegion_HyrulePrairie")}),
})
);

ADD_CASE_TO_GROUP("github34",
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
N(SB, L{
  N(MB, L{N(KP|VS, "MessageID1",  "MapRegion_HyrulePrairie")}),
  N(MB, L{N(KP|VD, "MessageID2",  "MapRegion_HyrulePrairie")}),
  N(MB, L{N(KP|VS, "MessageID3",  "MapRegion_HyrulePrairie ")}),
  N(MB, L{N(KP|VD, "MessageID4",  "MapRegion_HyrulePrairie ")}),
  N(MB, L{N(KP|VS, "MessageID5",  "MapRegion_HyrulePrairie ")}),
  N(MB, L{N(KP|VS, "MessageID6",  "MapRegion_HyrulePrairie ")}),
  N(MB, L{N(KP|VD, "MessageID7",  "MapRegion_HyrulePrairie ")}),
  N(MB, L{N(KP|VD, "MessageID8",  "MapRegion_HyrulePrairie ")}),
  N(MB, L{N(KP|VS, "MessageID9",  "MapRegion_HyrulePrairie ")}),
  N(MB, L{N(KP|VD, "MessageID10", "MapRegion_HyrulePrairie ")}),
  N(MB, L{N(KP|VS, "MessageID11", "MapRegion_HyrulePrairie")}),
  N(MB, L{N(KP|VD, "MessageID12", "MapRegion_HyrulePrairie")}),
  N(MB, L{N(KP|VP, "key1", "true1"), N(KP|VS, "MessageID1", "MapRegion_HyrulePrairie1 ")}),
  N(MB, L{N(KP|VP, "key2", "true2"), N(KP|VD, "MessageID2", "MapRegion_HyrulePrairie2 ")}),
})
);

ADD_CASE_TO_GROUP("github35/expected_error11", EXPECT_PARSE_ERROR,
R"(
# *segfault* // not anymore!
- key1: true1
 MessageID1:          'MapRegion_HyrulePrairie1 '
)",
  LineCol(4, 1)
);

ADD_CASE_TO_GROUP("github35/expected_error12", EXPECT_PARSE_ERROR,
R"(
# *segfault* // not anymore!
- key2: true2
 MessageID2:          "MapRegion_HyrulePrairie2 "
)",
  LineCol(4, 1)
);

ADD_CASE_TO_GROUP("github35/expected_error21", EXPECT_PARSE_ERROR,
R"(
# *segfault* // not anymore!
- key1: true1
    MessageID1:          'MapRegion_HyrulePrairie1 '
)",
  LineCol(4, 15)
);

ADD_CASE_TO_GROUP("github35/expected_error22", EXPECT_PARSE_ERROR,
R"(
# *segfault* // not anymore!
- key2: true2
    MessageID2:          "MapRegion_HyrulePrairie2 "
)",
  LineCol(4, 15)
);

ADD_CASE_TO_GROUP("github128/1", RESOLVE_REFS | EXPECT_PARSE_ERROR, "a: *invalid");
ADD_CASE_TO_GROUP("github128/2", RESOLVE_REFS | EXPECT_PARSE_ERROR, "*");
ADD_CASE_TO_GROUP("github128/3", RESOLVE_REFS | EXPECT_PARSE_ERROR, "*abc");
ADD_CASE_TO_GROUP("github128/4", "*abc", N(VAL, "*abc", AR(VALREF, "*abc")));

ADD_CASE_TO_GROUP("github129", RESOLVE_REFS, R"(
ref: &ref ref_val
a: *ref   # resolve the reference
b: '*ref' # don't resolve, it's just a string
c: "*ref" # don't resolve, it's just a string
d: >      # don't resolve, it's just a string
  *ref
e: >-     # don't resolve, it's just a string
  *ref
f: >+     # don't resolve, it's just a string
  *ref
g: |      # don't resolve, it's just a string
  *ref
h: |-     # don't resolve, it's just a string
  *ref
i: |+     # don't resolve, it's just a string
  *ref
)",
N(MB, L{
  N(KP|VP, "ref", "ref_val"),
  N(KP|VP, "a", "ref_val"),    // this should be resolved
  N(KP|VS, "b", "*ref"),   // this should not be resolved (just a string)
  N(KP|VD, "c", "*ref"),   // this should not be resolved (just a string)
  N(KP|VF, "d", "*ref\n"), // this should not be resolved (just a string)
  N(KP|VF, "e", "*ref"),   // this should not be resolved (just a string)
  N(KP|VF, "f", "*ref\n"), // this should not be resolved (just a string)
  N(KP|VL, "g", "*ref\n"), // this should not be resolved (just a string)
  N(KP|VL, "h", "*ref"),   // this should not be resolved (just a string)
  N(KP|VL, "i", "*ref\n"),   // this should not be resolved (just a string)
})
);

}

} // namespace yml
} // namespace c4
