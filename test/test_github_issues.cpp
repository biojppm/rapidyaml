#include "./test_lib/test_helpers_tree.hpp"
#ifndef RYML_SINGLE_HEADER
#include "c4/yml/parse.hpp"
#include "c4/yml/emit.hpp"
#endif

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

TEST(github, 475_0_space)
{
    Tree t;
    ExpectError::check_success(&t, [&t]{
        parse_in_arena(R"(
test:
- {a: 1}
# next line has a trailing space
- {b: 2} 
# next line has a trailing space
- [0, {c: 3}] 
)", &t);
    });
    ConstNodeRef test = t["test"];
    ASSERT_TRUE(test.is_seq());
    ASSERT_EQ(test.num_children(), 3u);
}

TEST(github, 475_1_space_indented)
{
    Tree t;
    ExpectError::check_success(&t, [&t]{
        parse_in_arena(R"(
test:
  - {a: 1}
  # next line has a trailing space
  - {b: 2} 
  # next line has a trailing space
  - [0, {c: 3}] 
)", &t);
    });
    ConstNodeRef test = t["test"];
    ASSERT_TRUE(test.is_seq());
    ASSERT_EQ(test.num_children(), 3u);
}

TEST(github, 475_2_tab)
{
    Tree t;
    ExpectError::check_success(&t, [&t]{
        parse_in_arena(R"(
test:
- {a: 1}
# next line has a trailing tab
- {b: 2}	
# next line has a trailing tab
- [0, {c: 3}]	
)", &t);
    });
    ConstNodeRef test = t["test"];
    ASSERT_TRUE(test.is_seq());
    ASSERT_EQ(test.num_children(), 3u);
}

TEST(github, 475_3_tab_indented)
{
    Tree t;
    ExpectError::check_success(&t, [&t]{
        parse_in_arena(R"(
test:
  - {a: 1}
  # next line has a trailing tab
  - {b: 2}	
  # next line has a trailing tab
  - [0, {c: 3}]	
)", &t);
    });
    ConstNodeRef test = t["test"];
    ASSERT_TRUE(test.is_seq());
    ASSERT_EQ(test.num_children(), 3u);
}

TEST(github, 475_4_space_map)
{
    Tree t;
    ExpectError::check_success(&t, [&t]{
        parse_in_arena(R"(
test:
  0: {a: 1}
  # next line has a trailing space
  1: {b: 2} 
  # next line has a trailing space
  2: [0, {c: 3}] 
)", &t);
    });
    ConstNodeRef test = t["test"];
    ASSERT_TRUE(test.is_map());
    ASSERT_EQ(test.num_children(), 3u);
}

TEST(github, 475_5_tab_map)
{
    Tree t;
    ExpectError::check_success(&t, [&t]{
        parse_in_arena(R"(
test:
  0: {a: 1}
  # next line has a trailing tab
  1: {b: 2}	
  # next line has a trailing tab
  2: [0, {c: 3}]	
)", &t);
    });
    ConstNodeRef test = t["test"];
    ASSERT_TRUE(test.is_map());
    ASSERT_EQ(test.num_children(), 3u);
}


//-----------------------------------------------------------------------------

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
    ASSERT_EQ(root["map"]["node"].val(), "bar");
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
    r.set_map();

    auto meas = r["meas"];
    meas.set_map();

    auto plist = meas["createParameterList"];
    plist.set_seq();

    {
        NodeRef lumi = plist.append_child();
        lumi.save("Lumi");
        EXPECT_TRUE(lumi.is_val());
    }

    {
        NodeRef lumi = plist.append_child();
        lumi.set_map();
        lumi["value"].save(1);
        lumi["relErr"].save(0.1);
        EXPECT_TRUE(lumi.is_map());
    }

    {
        ExpectError::check_assert_visit(&tree, [&](){
            NodeRef lumi = plist.append_child();
            lumi.save("Lumi");
            lumi.set_map();
        });
    }

    {
        ExpectError::check_assert_visit(&tree, [&](){
            NodeRef lumi = plist.append_child();
            lumi.save("Lumi");
            lumi.set_seq();
        });
    }

    {
        ExpectError::check_assert_visit(&tree, [&](){
            NodeRef lumi = plist.append_child();
            lumi.set_map();
            lumi.save("Lumi");
        });
    }
}

} // namespace yml
} // namespace c4
