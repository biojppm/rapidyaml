#ifdef RYML_SINGLE_HEADER
#include "ryml_all.hpp"
#else
#include "c4/yml/event_handler_tree.hpp"
#include "c4/yml/parse_engine.hpp"
#include "c4/yml/parse.hpp"
#include "c4/yml/node.hpp"
#endif
#include <gtest/gtest.h>
#include "./test_lib/callbacks_tester.hpp"
#include "./test_lib/test_case.hpp"


namespace c4 {
namespace yml {

C4_SUPPRESS_WARNING_GCC_CLANG_WITH_PUSH("-Wold-style-cast")

// TODO: add this as a method to csubstr
bool is_same(csubstr lhs, csubstr rhs)
{
    return lhs.str == rhs.str && lhs.len == rhs.len;
}

void mklarge(Parser *p, Callbacks const& cb)
{
    ASSERT_TRUE(p->m_evt_handler);
    Parser::handler_type *evt_handler = p->m_evt_handler;
    p->~Parser();
    evt_handler->m_stack.m_callbacks = cb;
    new ((void*)p) Parser(evt_handler, p->options());
    p->reserve_stack(20); // cause an allocation
    p->reserve_locations(128); // cause an allocation
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(Parser, empty_ctor)
{
    Parser::handler_type evt_handler = {};
    EXPECT_EQ(evt_handler.m_stack.m_callbacks, get_callbacks());
    Parser parser(&evt_handler);
    EXPECT_EQ(parser.callbacks(), get_callbacks());
}

TEST(Parser, callbacks_ctor)
{
    CallbacksTester cbt;
    {
        Parser::handler_type evt_handler = {cbt.callbacks()};
        Parser parser(&evt_handler);
        EXPECT_EQ(parser.callbacks(), cbt.callbacks());
    }
    EXPECT_EQ(cbt.num_allocs, 0u);
    EXPECT_EQ(cbt.num_deallocs, 0u);
}

TEST(Parser, reserve_capacity)
{
    CallbacksTester cbt("test", 30000/*Bytes*/);
    {
        Parser::handler_type evt_handler = {cbt.callbacks()};
        Parser parser(&evt_handler);
        EXPECT_EQ(cbt.num_allocs, 0u);
        EXPECT_EQ(cbt.num_deallocs, 0u);
        parser.reserve_stack(18);
        EXPECT_EQ(cbt.num_allocs, 1u);
        EXPECT_EQ(cbt.num_deallocs, 0u);
        parser.reserve_stack(24);
        EXPECT_EQ(cbt.num_allocs, 2u);
        EXPECT_EQ(cbt.num_deallocs, 1u);
        parser.reserve_stack(28);
        EXPECT_EQ(cbt.num_allocs, 3u);
        EXPECT_EQ(cbt.num_deallocs, 2u);
    }
    EXPECT_EQ(cbt.num_allocs, 3u);
    EXPECT_EQ(cbt.num_deallocs, 3u);
    cbt.check();
}

TEST(Parser, reserve_locations)
{
    CallbacksTester ts;
    {
        Parser::handler_type evt_handler = {ts.callbacks()};
        Parser parser(&evt_handler);
        EXPECT_EQ(parser.callbacks(), ts.callbacks());
        EXPECT_EQ(ts.num_allocs, 0u);
        EXPECT_EQ(ts.num_deallocs, 0u);
        parser.reserve_locations(128);
        EXPECT_EQ(ts.num_allocs, 1u);
        EXPECT_EQ(ts.num_deallocs, 0u);
        EXPECT_EQ(ts.alloc_size, 128u * sizeof(size_t));
        EXPECT_EQ(ts.dealloc_size, 0u);
    }
    EXPECT_EQ(ts.num_allocs, 1u);
    EXPECT_EQ(ts.num_deallocs, 1u);
    EXPECT_EQ(ts.alloc_size, 128u * sizeof(size_t));
    EXPECT_EQ(ts.dealloc_size, 128u * sizeof(size_t));
}

TEST(Parser, copy_ctor_0)
{
    Parser::handler_type evt_handler = {};
    EXPECT_EQ(evt_handler.m_stack.m_callbacks, get_callbacks());
    Parser src(&evt_handler);
    mklarge(&src, evt_handler.m_stack.m_callbacks);
    EXPECT_EQ(src.callbacks(), get_callbacks());
    Parser dst(src);
    EXPECT_EQ(src.callbacks(), get_callbacks());
    EXPECT_EQ(dst.callbacks(), get_callbacks());
}

TEST(Parser, copy_ctor_1)
{
    CallbacksTester ts;
    {
        Parser::handler_type evt_handler = {ts.callbacks()};
        Parser src(&evt_handler);
        mklarge(&src, evt_handler.m_stack.m_callbacks);
        ASSERT_EQ(src.callbacks(), ts.callbacks());
        size_t nbefore = ts.num_allocs;
        EXPECT_GT(ts.num_allocs, 0u);
        Parser dst(src);
        ASSERT_EQ(src.callbacks(), ts.callbacks());
        ASSERT_EQ(dst.callbacks(), ts.callbacks());
        EXPECT_GT(ts.num_allocs, nbefore);
    }
    EXPECT_EQ(ts.num_allocs, ts.num_deallocs);
    EXPECT_EQ(ts.alloc_size, ts.dealloc_size);
}

TEST(Parser, move_ctor_0)
{
    Parser::handler_type evt_handler = {};
    Parser src(&evt_handler);
    mklarge(&src, evt_handler.m_stack.m_callbacks);
    EXPECT_EQ(src.callbacks(), get_callbacks());
    Parser dst(std::move(src));
    //EXPECT_EQ(src.callbacks(), get_callbacks());
    EXPECT_EQ(dst.callbacks(), get_callbacks());
}
TEST(Parser, move_ctor_1)
{
    CallbacksTester ts;
    {
        Parser::handler_type evt_handler = {ts.callbacks()};
        Parser src(&evt_handler);
        mklarge(&src, evt_handler.m_stack.m_callbacks);
        ASSERT_EQ(src.callbacks(), ts.callbacks());
        size_t nbefore = ts.num_allocs;
        EXPECT_GT(ts.num_allocs, 0u);
        Parser dst(std::move(src));
        //ASSERT_EQ(src.callbacks(), ts.callbacks());
        ASSERT_EQ(dst.callbacks(), ts.callbacks());
        EXPECT_EQ(ts.num_allocs, nbefore);
    }
    EXPECT_EQ(ts.num_allocs, ts.num_deallocs);
    EXPECT_EQ(ts.alloc_size, ts.dealloc_size);
}

TEST(Parser, copy_assign_same_callbacks)
{
    CallbacksTester ts;
    {
        Parser::handler_type evt_handler_src = {ts.callbacks()};
        Parser::handler_type evt_handler_dst = {ts.callbacks()};
        Parser src(&evt_handler_src);
        Parser dst(&evt_handler_dst);
        mklarge(&src, ts.callbacks());
        mklarge(&dst, ts.callbacks());
        ASSERT_EQ(src.callbacks(), ts.callbacks());
        ASSERT_EQ(dst.callbacks(), ts.callbacks());
        size_t nbefore = ts.num_allocs;
        EXPECT_GT(ts.num_allocs, 0u);
        EXPECT_GT(ts.num_allocs, 0u);
        dst = src;
        ASSERT_EQ(src.callbacks(), ts.callbacks());
        ASSERT_EQ(dst.callbacks(), ts.callbacks());
        EXPECT_GT(ts.num_allocs, nbefore);
    }
    EXPECT_EQ(ts.num_allocs, ts.num_deallocs);
    EXPECT_EQ(ts.alloc_size, ts.dealloc_size);
}

TEST(Parser, copy_assign_diff_callbacks)
{
    CallbacksTester ts("src");
    CallbacksTester td("dst");
    {
        Parser::handler_type evt_handler_src = {ts.callbacks()};
        Parser::handler_type evt_handler_dst = {td.callbacks()};
        Parser src(&evt_handler_src);
        Parser dst(&evt_handler_dst);
        mklarge(&src, ts.callbacks());
        mklarge(&dst, td.callbacks());
        ASSERT_EQ(src.callbacks(), ts.callbacks());
        ASSERT_EQ(dst.callbacks(), td.callbacks());
        size_t nbefore = ts.num_allocs;
        EXPECT_GT(ts.num_allocs, 0u);
        EXPECT_GT(td.num_allocs, 0u);
        dst = src;
        ASSERT_EQ(src.callbacks(), ts.callbacks());
        ASSERT_EQ(dst.callbacks(), ts.callbacks());
        EXPECT_GT(ts.num_allocs, nbefore);
        EXPECT_EQ(td.num_allocs, nbefore);
    }
    EXPECT_EQ(ts.num_allocs, ts.num_deallocs);
    EXPECT_EQ(ts.alloc_size, ts.dealloc_size);
    EXPECT_EQ(td.num_allocs, td.num_deallocs);
    EXPECT_EQ(td.alloc_size, td.dealloc_size);
}

TEST(Parser, move_assign_same_callbacks)
{
    CallbacksTester ts;
    {
        Parser::handler_type evt_handler_src = {ts.callbacks()};
        Parser::handler_type evt_handler_dst = {ts.callbacks()};
        Parser src(&evt_handler_src);
        Parser dst(&evt_handler_dst);
        mklarge(&src, ts.callbacks());
        mklarge(&dst, ts.callbacks());
        ASSERT_EQ(src.callbacks(), ts.callbacks());
        ASSERT_EQ(dst.callbacks(), ts.callbacks());
        size_t nbefore = ts.num_allocs;
        EXPECT_GT(ts.num_allocs, 0u);
        EXPECT_GT(ts.num_allocs, 0u);
        dst = std::move(src);
        //ASSERT_EQ(src.callbacks(), ts.callbacks());
        ASSERT_EQ(dst.callbacks(), ts.callbacks());
        EXPECT_EQ(ts.num_allocs, nbefore);
    }
    EXPECT_EQ(ts.num_allocs, ts.num_deallocs);
    EXPECT_EQ(ts.alloc_size, ts.dealloc_size);
}

TEST(Parser, move_assign_diff_callbacks)
{
    CallbacksTester ts("src");
    CallbacksTester td("dst");
    {
        Parser::handler_type evt_handler_src = {ts.callbacks()};
        Parser::handler_type evt_handler_dst = {td.callbacks()};
        Parser src(&evt_handler_src);
        Parser dst(&evt_handler_dst);
        mklarge(&src, ts.callbacks());
        mklarge(&dst, td.callbacks());
        ASSERT_EQ(src.callbacks(), ts.callbacks());
        ASSERT_EQ(dst.callbacks(), td.callbacks());
        size_t nbefore = ts.num_allocs;
        EXPECT_GT(ts.num_allocs, 0u);
        EXPECT_GT(td.num_allocs, 0u);
        dst = std::move(src);
        //ASSERT_EQ(src.callbacks(), ts.callbacks());
        ASSERT_EQ(dst.callbacks(), ts.callbacks());
        EXPECT_EQ(ts.num_allocs, nbefore);
        EXPECT_EQ(ts.num_allocs, nbefore);
    }
    EXPECT_EQ(ts.num_allocs, ts.num_deallocs);
    EXPECT_EQ(ts.alloc_size, ts.dealloc_size);
    EXPECT_EQ(td.num_allocs, td.num_deallocs);
    EXPECT_EQ(td.alloc_size, td.dealloc_size);
}

TEST(Parser, new_tree_receives_callbacks)
{
    char src_[] = "{a: b}";
    substr src = src_;
    csubstr csrc = src_;
    {
        {
            Parser::handler_type evt_handler = {};
            Parser parser(&evt_handler);
            EXPECT_EQ(parser.callbacks(), get_callbacks());
            Tree t = parse_in_arena(&parser, "file0", csrc);
            EXPECT_EQ(t.callbacks(), get_callbacks());
        }
        CallbacksTester cbt("test", 20000/*Bytes*/);
        {
            Parser::handler_type evt_handler = {cbt.callbacks()};
            Parser parser(&evt_handler);
            EXPECT_EQ(parser.callbacks(), cbt.callbacks());
            Tree t = parse_in_arena(&parser, "file1", csrc);
            EXPECT_EQ(t.callbacks(), cbt.callbacks());
        }
        cbt.check();
    }
    {
        {
            Parser::handler_type evt_handler = {};
            Parser parser(&evt_handler);
            EXPECT_EQ(parser.callbacks(), get_callbacks());
            Tree t = parse_in_place(&parser, "file", src);
            EXPECT_EQ(t.callbacks(), get_callbacks());
        }
        CallbacksTester cbt("test", 20000/*Bytes*/);
        {
            Parser::handler_type evt_handler = {cbt.callbacks()};
            Parser parser(&evt_handler);
            EXPECT_EQ(parser.callbacks(), cbt.callbacks());
            Tree t = parse_in_place(&parser, "file", src);
            EXPECT_EQ(t.callbacks(), cbt.callbacks());
        }
        cbt.check();
    }
}

TEST(Parser, existing_tree_overwrites_parser_callbacks)
{
    char src_[] = "{a: b}";
    substr src = src_;
    csubstr csrc = src_;
    {
        CallbacksTester cbp("parser");
        CallbacksTester cbt("tree");
        {
            Tree tree(cbt.callbacks());
            EXPECT_EQ(tree.callbacks(), cbt.callbacks());
            Parser::handler_type evt_handler(cbp.callbacks());
            EXPECT_EQ(evt_handler.m_stack.m_callbacks, cbp.callbacks());
            Parser parser(&evt_handler);
            EXPECT_EQ(parser.callbacks(), cbp.callbacks());
            parse_in_arena(&parser, "file", csrc, &tree);
            EXPECT_EQ(tree.callbacks(), cbt.callbacks());
            EXPECT_EQ(parser.callbacks(), cbp.callbacks());
        }
        cbp.check();
        cbt.check();
    }
    {
        CallbacksTester cbp("parser");
        CallbacksTester cbt("tree");
        {
            Tree tree(cbt.callbacks());
            EXPECT_EQ(tree.callbacks(), cbt.callbacks());
            Parser::handler_type evt_handler(cbp.callbacks());
            Parser parser(&evt_handler);
            EXPECT_EQ(parser.callbacks(), cbp.callbacks());
            parse_in_place(&parser, "file", src, &tree);
            EXPECT_EQ(tree.callbacks(), cbt.callbacks());
            EXPECT_EQ(parser.callbacks(), cbp.callbacks());
        }
        cbp.check();
        cbt.check();
    }
}

TEST(Parser, filename_and_buffer_are_stored)
{
    char src_[] = "{a: b}";
    substr src = src_;
    csubstr csrc = src_;
    Parser::handler_type evt_handler = {};
    Parser parser(&evt_handler);
    EXPECT_EQ(parser.filename(), csubstr{});
    {
        Tree tree = parse_in_place(&parser, "file0", src);
        EXPECT_EQ(parser.filename(), "file0");
        EXPECT_TRUE(is_same(parser.source(), src));
    }
    {
        Tree tree = parse_in_arena(&parser, "file1", csrc);
        EXPECT_EQ(parser.filename(), "file1");
        EXPECT_TRUE(!is_same(parser.source(), src));
    }
    {
        Tree tree = parse_in_place(&parser, "file2", src);
        EXPECT_EQ(parser.filename(), "file2");
        EXPECT_TRUE(is_same(parser.source(), src));
    }
    {
        Tree tree = parse_in_arena(&parser, csrc);
        EXPECT_EQ(parser.filename(), csubstr{});
        EXPECT_TRUE(!is_same(parser.source(), src));
    }
}

TEST(Parser, estimate_tree_capacity)
{
    EXPECT_EQ(2, estimate_tree_capacity(R"([])"));
    EXPECT_EQ(2, estimate_tree_capacity(R"([a])"));
    EXPECT_EQ(3, estimate_tree_capacity(R"([a, b])"));
    EXPECT_EQ(4, estimate_tree_capacity(R"([a, b, c])"));
    EXPECT_EQ(5, estimate_tree_capacity(R"([a, b, c, d])"));
    EXPECT_EQ(2, estimate_tree_capacity(R"({})"));
    EXPECT_EQ(2, estimate_tree_capacity(R"({a: 0})"));
    EXPECT_EQ(3, estimate_tree_capacity(R"({a: 0, b: 1})"));
    EXPECT_EQ(4, estimate_tree_capacity(R"({a: 0, b: 1, c: 2})"));
    EXPECT_EQ(5, estimate_tree_capacity(R"({a: 0, b: 1, c: 2, d: 3})"));
    EXPECT_EQ(9, estimate_tree_capacity(R"(- {a: 0, b: 1, c: 2, d: 3}
- a
- b
- c
)"));
}

TEST(Parser, alloc_arena)
{
    Tree tree;
    Parser::handler_type evt_handler = {};
    int data = 0;
    auto relocate = [](void*, csubstr prev, substr next_arena){
        EXPECT_FALSE(prev.overlaps(next_arena));
    };
    evt_handler.reset(&tree, tree.root_id());
    evt_handler.start_parse("filename", relocate, &data);
    substr bufa = evt_handler.alloc_arena(64);
    bufa.fill('a');
    csubstr prev = bufa;
    csubstr prev_arena = tree.arena();
    substr bufb = evt_handler.alloc_arena(64, &bufa);
    csubstr curr_arena = tree.arena();
    EXPECT_NE(prev_arena.str, curr_arena.str);
    EXPECT_NE(prev.str, bufa.str);
    EXPECT_EQ(bufa.first_not_of('a'), npos);
    (void)bufb;
}

TEST(parse_in_place, overloads)
{
    char src1_[] = "{a: b}";
    char src2_[] = "{c: d, e: {}}";
    {
        Tree tree = parse_in_place(src1_);
        EXPECT_EQ(tree["a"].val(), "b");
    }
    {
        Tree tree = parse_in_place("src1", src1_);
        EXPECT_EQ(tree["a"].val(), "b");
    }
    {
        Tree tree;
        parse_in_place(src1_, &tree);
        EXPECT_EQ(tree["a"].val(), "b");
    }
    {
        Tree tree;
        parse_in_place("src1", src1_, &tree);
        EXPECT_EQ(tree["a"].val(), "b");
    }
    {
        Tree tree = parse_in_place(src2_);
        EXPECT_EQ(tree["c"].val(), "d");
        EXPECT_EQ(tree["e"].is_map(), true);
        EXPECT_EQ(tree["e"].has_children(), false);
        id_type e = tree.find_child(tree.root_id(), "e");
        ASSERT_NE(e, (id_type)NONE);
        parse_in_place(src1_, &tree, e);
        EXPECT_EQ(tree["c"].val(), "d");
        EXPECT_EQ(tree["e"].has_children(), true);
        EXPECT_EQ(tree["e"]["a"].val(), "b");
    }
    {
        Tree tree = parse_in_place("src2", src2_);
        EXPECT_EQ(tree["c"].val(), "d");
        EXPECT_EQ(tree["e"].is_map(), true);
        EXPECT_EQ(tree["e"].has_children(), false);
        id_type e = tree.find_child(tree.root_id(), "e");
        ASSERT_NE(e, (id_type)NONE);
        parse_in_place("src1", src1_, &tree, e);
        EXPECT_EQ(tree["c"].val(), "d");
        EXPECT_EQ(tree["e"].has_children(), true);
        EXPECT_EQ(tree["e"]["a"].val(), "b");
    }
    {
        Tree tree = parse_in_place(src2_);
        EXPECT_EQ(tree["c"].val(), "d");
        EXPECT_EQ(tree["e"].is_map(), true);
        EXPECT_EQ(tree["e"].has_children(), false);
        parse_in_place(src1_, tree["e"]);
        EXPECT_EQ(tree["c"].val(), "d");
        EXPECT_EQ(tree["e"].has_children(), true);
        EXPECT_EQ(tree["e"]["a"].val(), "b");
    }
    {
        Tree tree = parse_in_place("src2", src2_);
        EXPECT_EQ(tree["c"].val(), "d");
        EXPECT_EQ(tree["e"].is_map(), true);
        EXPECT_EQ(tree["e"].has_children(), false);
        parse_in_place("src1", src1_, tree["e"]);
        EXPECT_EQ(tree["c"].val(), "d");
        EXPECT_EQ(tree["e"].has_children(), true);
        EXPECT_EQ(tree["e"]["a"].val(), "b");
    }
}

TEST(parse_in_arena, overloads)
{
    csubstr src1 = "{a: b}";
    csubstr src2 = "{c: d, e: {}}";
    {
        Tree tree = parse_in_arena(src1);
        EXPECT_EQ(tree["a"].val(), "b");
        EXPECT_FALSE(tree.arena().empty());
        EXPECT_NE(tree.arena().find(src1), npos);
    }
    {
        Tree tree = parse_in_arena("src1", src1);
        EXPECT_EQ(tree["a"].val(), "b");
        EXPECT_FALSE(tree.arena().empty());
        EXPECT_NE(tree.arena().find(src1), npos);
    }
    {
        Tree tree;
        parse_in_arena(src1, &tree);
        EXPECT_EQ(tree["a"].val(), "b");
        EXPECT_FALSE(tree.arena().empty());
        EXPECT_NE(tree.arena().find(src1), npos);
    }
    {
        Tree tree;
        parse_in_arena("src1", src1, &tree);
        EXPECT_EQ(tree["a"].val(), "b");
        EXPECT_FALSE(tree.arena().empty());
        EXPECT_NE(tree.arena().find(src1), npos);
    }
    {
        Tree tree = parse_in_arena(src2);
        EXPECT_EQ(tree["c"].val(), "d");
        EXPECT_EQ(tree["e"].is_map(), true);
        EXPECT_EQ(tree["e"].has_children(), false);
        EXPECT_FALSE(tree.arena().empty());
        EXPECT_NE(tree.arena().find(src2), npos);
        id_type e = tree.find_child(tree.root_id(), "e");
        ASSERT_NE(e, (id_type)NONE);
        parse_in_arena(src1, &tree, e);
        EXPECT_EQ(tree["c"].val(), "d");
        EXPECT_EQ(tree["e"].has_children(), true);
        EXPECT_EQ(tree["e"]["a"].val(), "b");
        EXPECT_FALSE(tree.arena().empty());
        EXPECT_NE(tree.arena().find(src1), npos);
        EXPECT_NE(tree.arena().find(src2), npos);
    }
    {
        Tree tree = parse_in_arena("src2", src2);
        EXPECT_EQ(tree["c"].val(), "d");
        EXPECT_EQ(tree["e"].is_map(), true);
        EXPECT_EQ(tree["e"].has_children(), false);
        EXPECT_FALSE(tree.arena().empty());
        EXPECT_NE(tree.arena().find(src2), npos);
        id_type e = tree.find_child(tree.root_id(), "e");
        ASSERT_NE(e, (id_type)NONE);
        parse_in_arena("src1", src1, &tree, e);
        EXPECT_EQ(tree["c"].val(), "d");
        EXPECT_EQ(tree["e"].has_children(), true);
        EXPECT_EQ(tree["e"]["a"].val(), "b");
        EXPECT_FALSE(tree.arena().empty());
        EXPECT_NE(tree.arena().find(src1), npos);
        EXPECT_NE(tree.arena().find(src2), npos);
    }
    {
        Tree tree = parse_in_arena(src2);
        EXPECT_EQ(tree["c"].val(), "d");
        EXPECT_EQ(tree["e"].is_map(), true);
        EXPECT_EQ(tree["e"].has_children(), false);
        EXPECT_FALSE(tree.arena().empty());
        EXPECT_NE(tree.arena().find(src2), npos);
        parse_in_arena(src1, tree["e"]);
        EXPECT_EQ(tree["c"].val(), "d");
        EXPECT_EQ(tree["e"].has_children(), true);
        EXPECT_EQ(tree["e"]["a"].val(), "b");
        EXPECT_FALSE(tree.arena().empty());
        EXPECT_NE(tree.arena().find(src1), npos);
        EXPECT_NE(tree.arena().find(src2), npos);
    }
    {
        Tree tree = parse_in_arena("src2", src2);
        EXPECT_EQ(tree["c"].val(), "d");
        EXPECT_EQ(tree["e"].is_map(), true);
        EXPECT_EQ(tree["e"].has_children(), false);
        EXPECT_FALSE(tree.arena().empty());
        EXPECT_NE(tree.arena().find(src2), npos);
        parse_in_arena("src1", src1, tree["e"]);
        EXPECT_EQ(tree["c"].val(), "d");
        EXPECT_EQ(tree["e"].has_children(), true);
        EXPECT_EQ(tree["e"]["a"].val(), "b");
        EXPECT_FALSE(tree.arena().empty());
        EXPECT_NE(tree.arena().find(src1), npos);
        EXPECT_NE(tree.arena().find(src2), npos);
    }
}

TEST(Parser, error_without_handler)
{
    ExpectError::check_error([]{
        Parser parser{nullptr};
    });
}

TEST(parse_in_place, error_without_handler)
{
    ExpectError::check_error([]{
        EventHandlerTree evth;
        Parser parser{&evth};
        parser.m_evt_handler = nullptr;
        char yaml[] = "{foo: bar}";
        Tree t = parse_in_place(&parser, yaml);
    });
}

TEST(parse_in_arena, error_without_handler)
{
    ExpectError::check_error([]{
        EventHandlerTree evth;
        Parser parser{&evth};
        parser.m_evt_handler = nullptr;
        const char yaml[] = "{foo: bar}";
        Tree t = parse_in_arena(&parser, yaml);
    });
}


//-----------------------------------------------------------------------------

class ParseOverloadYamlTest : public testing::Test
{
protected:

    Tree ref;
    std::string yaml_ = "{this: is, a: [yaml, example]}";
    substr yaml = to_substr(yaml_);
    csubstr cyaml = to_csubstr(yaml_);
    csubstr filename = "example.yml";

    EventHandlerTree evt_handler;
    Parser parser = {&evt_handler};

    void SetUp() override
    {
        NodeRef root = ref.rootref();
        root |= MAP;
        root |= FLOW_SL;
        NodeRef keyval = root.append_child();
        NodeRef keyseq = root.append_child();
        keyval << key("this") << "is";
        keyseq << key("a");
        keyseq |= SEQ;
        keyseq.append_child() << "yaml";
        keyseq.append_child() << "example";
    }

    void check_tree(Tree const& actual, csubstr expected_arena)
    {
        ASSERT_TRUE(actual.rootref().is_map());
        ASSERT_TRUE(actual["a"].is_seq());
        EXPECT_EQ(actual["this"].key(), "this");
        EXPECT_EQ(actual["this"].val(), "is");
        EXPECT_EQ(actual["a"].key(), "a");
        EXPECT_EQ(actual["a"][0].val(), "yaml");
        EXPECT_EQ(actual["a"][1].val(), "example");
        EXPECT_TRUE(actual["this"].key().is_sub(expected_arena));
        EXPECT_TRUE(actual["this"].val().is_sub(expected_arena));
        EXPECT_TRUE(actual["a"].key().is_sub(expected_arena));
        EXPECT_TRUE(actual["a"][0].val().is_sub(expected_arena));
        EXPECT_TRUE(actual["a"][1].val().is_sub(expected_arena));
        if(expected_arena.str != actual.arena().str)
        {
            EXPECT_TRUE(actual.arena().empty());
        }
    }
};

TEST_F(ParseOverloadYamlTest, in_place_noparser_1_1)
{
    const Tree actual = parse_in_place(yaml);
    test_compare(actual, ref);
    check_tree(actual, yaml);
}
TEST_F(ParseOverloadYamlTest, in_place_noparser_1_2)
{
    const Tree actual = parse_in_place(filename, yaml);
    test_compare(actual, ref);
    check_tree(actual, yaml);
}
TEST_F(ParseOverloadYamlTest, in_place_noparser_2_1)
{
    Tree actual;
    parse_in_place(yaml, &actual);
    test_compare(actual, ref);
    check_tree(actual, yaml);
}
TEST_F(ParseOverloadYamlTest, in_place_noparser_2_2)
{
    Tree actual;
    parse_in_place(filename, yaml, &actual);
    test_compare(actual, ref);
    check_tree(actual, yaml);
}
TEST_F(ParseOverloadYamlTest, in_place_noparser_3_1)
{
    Tree actual;
    parse_in_place(yaml, &actual, actual.root_id());
    test_compare(actual, ref);
    check_tree(actual, yaml);
}
TEST_F(ParseOverloadYamlTest, in_place_noparser_3_2)
{
    Tree actual;
    parse_in_place(filename, yaml, &actual, actual.root_id());
    test_compare(actual, ref);
    check_tree(actual, yaml);
}
TEST_F(ParseOverloadYamlTest, in_place_noparser_4_1)
{
    Tree actual;
    parse_in_place(yaml, actual.rootref());
    test_compare(actual, ref);
    check_tree(actual, yaml);
}
TEST_F(ParseOverloadYamlTest, in_place_noparser_4_2)
{
    Tree actual;
    parse_in_place(filename, yaml, actual.rootref());
    test_compare(actual, ref);
    check_tree(actual, yaml);
}

TEST_F(ParseOverloadYamlTest, in_place_parser_1_1)
{
    const Tree actual = parse_in_place(&parser, yaml);
    test_compare(actual, ref);
    check_tree(actual, yaml);
}
TEST_F(ParseOverloadYamlTest, in_place_parser_1_2)
{
    const Tree actual = parse_in_place(&parser, filename, yaml);
    test_compare(actual, ref);
    check_tree(actual, yaml);
}
TEST_F(ParseOverloadYamlTest, in_place_parser_2_1)
{
    Tree actual;
    parse_in_place(&parser, yaml, &actual);
    test_compare(actual, ref);
    check_tree(actual, yaml);
}
TEST_F(ParseOverloadYamlTest, in_place_parser_2_2)
{
    Tree actual;
    parse_in_place(&parser, filename, yaml, &actual);
    test_compare(actual, ref);
    check_tree(actual, yaml);
}
TEST_F(ParseOverloadYamlTest, in_place_parser_3_1)
{
    Tree actual;
    parse_in_place(&parser, yaml, &actual, actual.root_id());
    test_compare(actual, ref);
    check_tree(actual, yaml);
}
TEST_F(ParseOverloadYamlTest, in_place_parser_3_2)
{
    Tree actual;
    parse_in_place(&parser, filename, yaml, &actual, actual.root_id());
    test_compare(actual, ref);
    check_tree(actual, yaml);
}
TEST_F(ParseOverloadYamlTest, in_place_parser_4_1)
{
    Tree actual;
    parse_in_place(&parser, yaml, actual.rootref());
    test_compare(actual, ref);
    check_tree(actual, yaml);
}
TEST_F(ParseOverloadYamlTest, in_place_parser_4_2)
{
    Tree actual;
    parse_in_place(&parser, filename, yaml, actual.rootref());
    test_compare(actual, ref);
    check_tree(actual, yaml);
}

TEST_F(ParseOverloadYamlTest, in_arena_noparser_1_1)
{
    const Tree actual = parse_in_arena(cyaml);
    test_compare(actual, ref);
    check_tree(actual, actual.arena());
}
TEST_F(ParseOverloadYamlTest, in_arena_noparser_1_2)
{
    const Tree actual = parse_in_arena(filename, cyaml);
    test_compare(actual, ref);
    check_tree(actual, actual.arena());
}
TEST_F(ParseOverloadYamlTest, in_arena_noparser_2_1)
{
    Tree actual;
    parse_in_arena(cyaml, &actual);
    test_compare(actual, ref);
    check_tree(actual, actual.arena());
}
TEST_F(ParseOverloadYamlTest, in_arena_noparser_2_2)
{
    Tree actual;
    parse_in_arena(filename, cyaml, &actual);
    test_compare(actual, ref);
    check_tree(actual, actual.arena());
}
TEST_F(ParseOverloadYamlTest, in_arena_noparser_3_1)
{
    Tree actual;
    parse_in_arena(cyaml, &actual, actual.root_id());
    test_compare(actual, ref);
    check_tree(actual, actual.arena());
}
TEST_F(ParseOverloadYamlTest, in_arena_noparser_3_2)
{
    Tree actual;
    parse_in_arena(filename, cyaml, &actual, actual.root_id());
    test_compare(actual, ref);
    check_tree(actual, actual.arena());
}
TEST_F(ParseOverloadYamlTest, in_arena_noparser_4_1)
{
    Tree actual;
    parse_in_arena(cyaml, actual.rootref());
    test_compare(actual, ref);
    check_tree(actual, actual.arena());
}
TEST_F(ParseOverloadYamlTest, in_arena_noparser_4_2)
{
    Tree actual;
    parse_in_arena(filename, cyaml, actual.rootref());
    test_compare(actual, ref);
    check_tree(actual, actual.arena());
}

TEST_F(ParseOverloadYamlTest, in_arena_parser_1_1)
{
    const Tree actual = parse_in_arena(&parser, cyaml);
    test_compare(actual, ref);
    check_tree(actual, actual.arena());
}
TEST_F(ParseOverloadYamlTest, in_arena_parser_1_2)
{
    const Tree actual = parse_in_arena(&parser, filename, cyaml);
    test_compare(actual, ref);
    check_tree(actual, actual.arena());
}
TEST_F(ParseOverloadYamlTest, in_arena_parser_2_1)
{
    Tree actual;
    parse_in_arena(&parser, cyaml, &actual);
    test_compare(actual, ref);
    check_tree(actual, actual.arena());
}
TEST_F(ParseOverloadYamlTest, in_arena_parser_2_2)
{
    Tree actual;
    parse_in_arena(&parser, filename, cyaml, &actual);
    test_compare(actual, ref);
    check_tree(actual, actual.arena());
}
TEST_F(ParseOverloadYamlTest, in_arena_parser_3_1)
{
    Tree actual;
    parse_in_arena(&parser, cyaml, &actual, actual.root_id());
    test_compare(actual, ref);
    check_tree(actual, actual.arena());
}
TEST_F(ParseOverloadYamlTest, in_arena_parser_3_2)
{
    Tree actual;
    parse_in_arena(&parser, filename, cyaml, &actual, actual.root_id());
    test_compare(actual, ref);
    check_tree(actual, actual.arena());
}
TEST_F(ParseOverloadYamlTest, in_arena_parser_4_1)
{
    Tree actual;
    parse_in_arena(&parser, cyaml, actual.rootref());
    test_compare(actual, ref);
    check_tree(actual, actual.arena());
}
TEST_F(ParseOverloadYamlTest, in_arena_parser_4_2)
{
    Tree actual;
    parse_in_arena(&parser, filename, cyaml, actual.rootref());
    test_compare(actual, ref);
    check_tree(actual, actual.arena());
}


//-----------------------------------------------------------------------------

class ParseOverloadJsonTest : public testing::Test
{
protected:

    Tree ref;
    std::string json_ = "{\"this\": \"is\", \"a\": [\"json\", \"example\"]}";
    substr json = to_substr(json_);
    csubstr cjson = to_csubstr(json_);
    csubstr filename = "example.yml";

    EventHandlerTree evt_handler;
    Parser parser = {&evt_handler};

    void SetUp() override
    {
        NodeRef root = ref.rootref();
        root |= MAP;
        root |= FLOW_SL;
        NodeRef keyval = root.append_child();
        NodeRef keyseq = root.append_child();
        keyval << key("this") << "is";
        keyseq << key("a");
        keyseq |= SEQ;
        keyseq.append_child() << "json";
        keyseq.append_child() << "example";
    }

    void check_tree(Tree const& actual, csubstr expected_arena)
    {
        ASSERT_TRUE(actual.rootref().is_map());
        ASSERT_TRUE(actual["a"].is_seq());
        EXPECT_EQ(actual["this"].key(), "this");
        EXPECT_EQ(actual["this"].val(), "is");
        EXPECT_EQ(actual["a"].key(), "a");
        EXPECT_EQ(actual["a"][0].val(), "json");
        EXPECT_EQ(actual["a"][1].val(), "example");
        EXPECT_TRUE(actual["this"].key().is_sub(expected_arena));
        EXPECT_TRUE(actual["this"].val().is_sub(expected_arena));
        EXPECT_TRUE(actual["a"].key().is_sub(expected_arena));
        EXPECT_TRUE(actual["a"][0].val().is_sub(expected_arena));
        EXPECT_TRUE(actual["a"][1].val().is_sub(expected_arena));
        if(expected_arena.str != actual.arena().str)
        {
            EXPECT_TRUE(actual.arena().empty());
        }
    }
};

TEST_F(ParseOverloadJsonTest, in_place_noparser_1_1)
{
    const Tree actual = parse_json_in_place(json);
    test_compare(actual, ref);
    check_tree(actual, json);
}
TEST_F(ParseOverloadJsonTest, in_place_noparser_1_2)
{
    const Tree actual = parse_json_in_place(filename, json);
    test_compare(actual, ref);
    check_tree(actual, json);
}
TEST_F(ParseOverloadJsonTest, in_place_noparser_2_1)
{
    Tree actual;
    parse_json_in_place(json, &actual);
    test_compare(actual, ref);
    check_tree(actual, json);
}
TEST_F(ParseOverloadJsonTest, in_place_noparser_2_2)
{
    Tree actual;
    parse_json_in_place(filename, json, &actual);
    test_compare(actual, ref);
    check_tree(actual, json);
}
TEST_F(ParseOverloadJsonTest, in_place_noparser_3_1)
{
    Tree actual;
    parse_json_in_place(json, &actual, actual.root_id());
    test_compare(actual, ref);
    check_tree(actual, json);
}
TEST_F(ParseOverloadJsonTest, in_place_noparser_3_2)
{
    Tree actual;
    parse_json_in_place(filename, json, &actual, actual.root_id());
    test_compare(actual, ref);
    check_tree(actual, json);
}
TEST_F(ParseOverloadJsonTest, in_place_noparser_4_1)
{
    Tree actual;
    parse_json_in_place(json, actual.rootref());
    test_compare(actual, ref);
    check_tree(actual, json);
}
TEST_F(ParseOverloadJsonTest, in_place_noparser_4_2)
{
    Tree actual;
    parse_json_in_place(filename, json, actual.rootref());
    test_compare(actual, ref);
    check_tree(actual, json);
}

TEST_F(ParseOverloadJsonTest, in_place_parser_1_1)
{
    const Tree actual = parse_json_in_place(&parser, json);
    test_compare(actual, ref);
    check_tree(actual, json);
}
TEST_F(ParseOverloadJsonTest, in_place_parser_1_2)
{
    const Tree actual = parse_json_in_place(&parser, filename, json);
    test_compare(actual, ref);
    check_tree(actual, json);
}
TEST_F(ParseOverloadJsonTest, in_place_parser_2_1)
{
    Tree actual;
    parse_json_in_place(&parser, json, &actual);
    test_compare(actual, ref);
    check_tree(actual, json);
}
TEST_F(ParseOverloadJsonTest, in_place_parser_2_2)
{
    Tree actual;
    parse_json_in_place(&parser, filename, json, &actual);
    test_compare(actual, ref);
    check_tree(actual, json);
}
TEST_F(ParseOverloadJsonTest, in_place_parser_3_1)
{
    Tree actual;
    parse_json_in_place(&parser, json, &actual, actual.root_id());
    test_compare(actual, ref);
    check_tree(actual, json);
}
TEST_F(ParseOverloadJsonTest, in_place_parser_3_2)
{
    Tree actual;
    parse_json_in_place(&parser, filename, json, &actual, actual.root_id());
    test_compare(actual, ref);
    check_tree(actual, json);
}
TEST_F(ParseOverloadJsonTest, in_place_parser_4_1)
{
    Tree actual;
    parse_json_in_place(&parser, json, actual.rootref());
    test_compare(actual, ref);
    check_tree(actual, json);
}
TEST_F(ParseOverloadJsonTest, in_place_parser_4_2)
{
    Tree actual;
    parse_json_in_place(&parser, filename, json, actual.rootref());
    test_compare(actual, ref);
    check_tree(actual, json);
}

TEST_F(ParseOverloadJsonTest, in_arena_noparser_1_1)
{
    const Tree actual = parse_json_in_arena(cjson);
    test_compare(actual, ref);
    check_tree(actual, actual.arena());
}
TEST_F(ParseOverloadJsonTest, in_arena_noparser_1_2)
{
    const Tree actual = parse_json_in_arena(filename, cjson);
    test_compare(actual, ref);
    check_tree(actual, actual.arena());
}
TEST_F(ParseOverloadJsonTest, in_arena_noparser_2_1)
{
    Tree actual;
    parse_json_in_arena(cjson, &actual);
    test_compare(actual, ref);
    check_tree(actual, actual.arena());
}
TEST_F(ParseOverloadJsonTest, in_arena_noparser_2_2)
{
    Tree actual;
    parse_json_in_arena(filename, cjson, &actual);
    test_compare(actual, ref);
    check_tree(actual, actual.arena());
}
TEST_F(ParseOverloadJsonTest, in_arena_noparser_3_1)
{
    Tree actual;
    parse_json_in_arena(cjson, &actual, actual.root_id());
    test_compare(actual, ref);
    check_tree(actual, actual.arena());
}
TEST_F(ParseOverloadJsonTest, in_arena_noparser_3_2)
{
    Tree actual;
    parse_json_in_arena(filename, cjson, &actual, actual.root_id());
    test_compare(actual, ref);
    check_tree(actual, actual.arena());
}
TEST_F(ParseOverloadJsonTest, in_arena_noparser_4_1)
{
    Tree actual;
    parse_json_in_arena(cjson, actual.rootref());
    test_compare(actual, ref);
    check_tree(actual, actual.arena());
}
TEST_F(ParseOverloadJsonTest, in_arena_noparser_4_2)
{
    Tree actual;
    parse_json_in_arena(filename, cjson, actual.rootref());
    test_compare(actual, ref);
    check_tree(actual, actual.arena());
}

TEST_F(ParseOverloadJsonTest, in_arena_parser_1_1)
{
    const Tree actual = parse_json_in_arena(&parser, cjson);
    test_compare(actual, ref);
    check_tree(actual, actual.arena());
}
TEST_F(ParseOverloadJsonTest, in_arena_parser_1_2)
{
    const Tree actual = parse_json_in_arena(&parser, filename, cjson);
    test_compare(actual, ref);
    check_tree(actual, actual.arena());
}
TEST_F(ParseOverloadJsonTest, in_arena_parser_2_1)
{
    Tree actual;
    parse_json_in_arena(&parser, cjson, &actual);
    test_compare(actual, ref);
    check_tree(actual, actual.arena());
}
TEST_F(ParseOverloadJsonTest, in_arena_parser_2_2)
{
    Tree actual;
    parse_json_in_arena(&parser, filename, cjson, &actual);
    test_compare(actual, ref);
    check_tree(actual, actual.arena());
}
TEST_F(ParseOverloadJsonTest, in_arena_parser_3_1)
{
    Tree actual;
    parse_json_in_arena(&parser, cjson, &actual, actual.root_id());
    test_compare(actual, ref);
    check_tree(actual, actual.arena());
}
TEST_F(ParseOverloadJsonTest, in_arena_parser_3_2)
{
    Tree actual;
    parse_json_in_arena(&parser, filename, cjson, &actual, actual.root_id());
    test_compare(actual, ref);
    check_tree(actual, actual.arena());
}
TEST_F(ParseOverloadJsonTest, in_arena_parser_4_1)
{
    Tree actual;
    parse_json_in_arena(&parser, cjson, actual.rootref());
    test_compare(actual, ref);
    check_tree(actual, actual.arena());
}
TEST_F(ParseOverloadJsonTest, in_arena_parser_4_2)
{
    Tree actual;
    parse_json_in_arena(&parser, filename, cjson, actual.rootref());
    test_compare(actual, ref);
    check_tree(actual, actual.arena());
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class ParseToNodeTest : public testing::Test
{
protected:

    EventHandlerTree evt_handler;
    Parser parser = {&evt_handler};

    std::string val_plain = "this is a plain val";
    std::string val_plain_anchor = "&anchor this is a plain val";
    std::string val_plain_tag = "!!str this is a plain val";

    std::string val_squo = "'this is a squo val'";
    std::string val_squo_anchor = "&anchor 'this is a squo val'";
    std::string val_squo_tag = "!!str 'this is a squo val'";

    std::string val_dquo = "\"this is a dquo val\"";
    std::string val_dquo_anchor = "&anchor \"this is a dquo val\"";
    std::string val_dquo_tag = "!!str \"this is a dquo val\"";

    std::string val_literal = "|-\n this is a literal val";
    std::string val_literal_anchor = "&anchor |-\n this is a literal val";
    std::string val_literal_tag = "!!str |-\n this is a literal val";

    std::string val_folded = ">-\n this is a folded val";
    std::string val_folded_anchor = "&anchor >-\n this is a folded val";
    std::string val_folded_tag = "!!str >-\n this is a folded val";

    std::string val_ref = "*ref";

    std::string seq_flow = "  [seq, flow, yes, it is]";
    std::string seq_flow_anchor = "  &anchor [seq, flow, yes, it is]";
    std::string seq_flow_tag = "  !!seq [seq, flow, yes, it is]";

    std::string seq_blck = "- seq\n- block\n- yes\n- it is";
    std::string seq_blck_anchor = "&anchor\n- seq\n- block\n- yes\n- it is";
    std::string seq_blck_tag = "!!seq\n- seq\n- block\n- yes\n- it is";

    std::string map_flow = "  {map: flow, yes: it is}";
    std::string map_flow_anchor = "  &anchor {map: flow, yes: it is}";
    std::string map_flow_tag = "  !!map {map: flow, yes: it is}";

    std::string map_blck_keyless = ": block\nyes: it is";
    std::string map_blck_plain = "map: block\nyes: it is";
    std::string map_blck_squo = "'map': block\nyes: it is";
    std::string map_blck_dquo = "\"map\": block\nyes: it is";
    std::string map_blck_literal = "? |-\n  map\n: block\nyes: it is";
    std::string map_blck_folded = "? >-\n  map\n: block\nyes: it is";
    std::string map_blck_anchor = "&anchor\nmap: block\nyes: it is";
    std::string map_blck_tag = "!!map\nmap: block\nyes: it is";
    std::string map_blck_ref = "*map : block\nyes: it is";

    std::string empty = "";
    std::string whitespace = "     ";

    Tree dst_empty;
    Tree dst_val;
    Tree dst_map_flow;
    Tree dst_map_blck;
    Tree dst_seq_flow;
    Tree dst_seq_blck;

    void SetUp() override
    {
        dst_val.rootref().set_val("val");
        dst_map_flow.rootref() |= MAP|FLOW_SL;
        dst_map_blck.rootref() |= MAP|BLOCK;
        dst_seq_flow.rootref() |= SEQ|FLOW_SL;
        dst_seq_blck.rootref() |= SEQ|BLOCK;
    }

};


//-----------------------------------------------------------------------------

class ParseToSeqFlowTest : public ParseToNodeTest {};



TEST_F(ParseToSeqFlowTest, empty__to__seq_flow__root)
{
    NodeRef dst = dst_seq_flow.rootref();
    parse_in_arena(to_csubstr(empty), dst);
    EXPECT_TRUE(dst.empty());
}

TEST_F(ParseToSeqFlowTest, empty__to__seq_flow__new_child)
{
    NodeRef dst = dst_seq_flow.rootref().append_child();
    parse_in_arena(to_csubstr(empty), dst);
    EXPECT_FALSE(dst.has_val());
}


TEST_F(ParseToSeqFlowTest, whitespace__to__seq_flow__root)
{
    NodeRef dst = dst_seq_flow.rootref();
    parse_in_arena(to_csubstr(whitespace), dst);
    EXPECT_TRUE(dst.empty());
}

TEST_F(ParseToSeqFlowTest, whitespace__to__seq_flow__new_child)
{
    NodeRef dst = dst_seq_flow.rootref().append_child();
    parse_in_arena(to_csubstr(whitespace), dst);
    EXPECT_FALSE(dst.has_val());
}


TEST_F(ParseToSeqFlowTest, val_plain__to__seq_flow__root)
{
    NodeRef dst = dst_seq_flow.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_plain), dst);
    });
    _c4dbg_tree("actual", dst_seq_flow);
}

TEST_F(ParseToSeqFlowTest, val_plain__to__seq_flow__new_child)
{
    NodeRef dst = dst_seq_flow.rootref().append_child();
    parse_in_arena(to_csubstr(val_plain), dst);
    const Tree expected = parse_in_arena("[this is a plain val]");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_seq_flow);
    test_compare(dst_seq_flow, expected);
}


TEST_F(ParseToSeqFlowTest, val_squo__to__seq_flow__root)
{
    NodeRef dst = dst_seq_flow.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_squo), dst);
    });
    _c4dbg_tree("actual", dst_seq_flow);
}

TEST_F(ParseToSeqFlowTest, val_squo__to__seq_flow__new_child)
{
    NodeRef dst = dst_seq_flow.rootref().append_child();
    parse_in_arena(to_csubstr(val_squo), dst);
    const Tree expected = parse_in_arena("[this is a squo val]");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_seq_flow);
    test_compare(dst_seq_flow, expected);
}


TEST_F(ParseToSeqFlowTest, val_dquo__to__seq_flow__root)
{
    NodeRef dst = dst_seq_flow.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_dquo), dst);
    });
    _c4dbg_tree("actual", dst_seq_flow);
}

TEST_F(ParseToSeqFlowTest, val_dquo__to__seq_flow__new_child)
{
    NodeRef dst = dst_seq_flow.rootref().append_child();
    parse_in_arena(to_csubstr(val_dquo), dst);
    const Tree expected = parse_in_arena("[this is a dquo val]");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_seq_flow);
    test_compare(dst_seq_flow, expected);
}


TEST_F(ParseToSeqFlowTest, val_literal__to__seq_flow__root)
{
    NodeRef dst = dst_seq_flow.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_literal), dst);
    });
    _c4dbg_tree("actual", dst_seq_flow);
}

TEST_F(ParseToSeqFlowTest, val_literal_anchor__to__seq_flow__new_child)
{
    NodeRef dst = dst_seq_flow.rootref().append_child();
    parse_in_arena(to_csubstr(val_literal), dst);
    const Tree expected = parse_in_arena("[this is a literal val]");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_seq_flow);
    test_compare(dst_seq_flow, expected);
}


TEST_F(ParseToSeqFlowTest, val_folded__to__seq_flow__root)
{
    NodeRef dst = dst_seq_flow.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_folded), dst);
    });
    _c4dbg_tree("actual", dst_seq_flow);
}

TEST_F(ParseToSeqFlowTest, val_folded__to__seq_flow__new_child)
{
    NodeRef dst = dst_seq_flow.rootref().append_child();
    parse_in_arena(to_csubstr(val_folded), dst);
    const Tree expected = parse_in_arena("[this is a folded val]");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_seq_flow);
    test_compare(dst_seq_flow, expected);
}


TEST_F(ParseToSeqFlowTest, val_ref__to__seq_flow__root)
{
    NodeRef dst = dst_seq_flow.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_ref), dst);
    });
    _c4dbg_tree("actual", dst_seq_flow);
}

TEST_F(ParseToSeqFlowTest, val_ref__to__seq_flow__new_child)
{
    NodeRef dst = dst_seq_flow.rootref().append_child();
    parse_in_arena(to_csubstr(val_ref), dst);
    const Tree expected = parse_in_arena("[*ref]");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_seq_flow);
    test_compare(dst_seq_flow, expected);
}


TEST_F(ParseToSeqFlowTest, seq_flow__to__seq_flow__root)
{
    NodeRef dst = dst_seq_flow.rootref();
    parse_in_arena(to_csubstr(seq_flow), dst);
    const Tree expected = parse_in_arena("[seq, flow, yes, it is]");
    _c4dbg_tree("actual", dst_seq_flow);
    _c4dbg_tree("expected", expected);
    test_compare(dst_seq_flow, expected);
}

TEST_F(ParseToSeqFlowTest, seq_flow__to__seq_flow__new_child)
{
    NodeRef dst = dst_seq_flow.rootref().append_child();
    parse_in_arena(to_csubstr(seq_flow), dst);
    const Tree expected = parse_in_arena("- [seq, flow, yes, it is]");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_seq_flow);
    test_compare(dst_seq_flow, expected);
}


TEST_F(ParseToSeqFlowTest, seq_blck__to__seq_flow__root)
{
    NodeRef dst = dst_seq_flow.rootref();
    parse_in_arena(to_csubstr(seq_blck), dst);
    const Tree expected = parse_in_arena("[seq, block, yes, it is]");
    _c4dbg_tree("actual", dst_seq_flow);
    _c4dbg_tree("expected", expected);
    test_compare(dst_seq_flow, expected);
}

TEST_F(ParseToSeqFlowTest, seq_blck__to__seq_flow__new_child)
{
    NodeRef dst = dst_seq_flow.rootref().append_child();
    parse_in_arena(to_csubstr(seq_blck), dst);
    const Tree expected = parse_in_arena("- [seq, block, yes, it is]");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_seq_flow);
    test_compare(dst_seq_flow, expected);
}


TEST_F(ParseToSeqFlowTest, map_flow__to__seq_flow__root)
{
    NodeRef dst = dst_seq_flow.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(map_flow), dst);
    });
    _c4dbg_tree("actual", dst_seq_flow);
}

TEST_F(ParseToSeqFlowTest, map_flow__to__seq_flow__new_child)
{
    NodeRef dst = dst_seq_flow.rootref().append_child();
    parse_in_arena(to_csubstr(map_flow), dst);
    const Tree expected = parse_in_arena("- {map: flow, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_seq_flow);
    test_compare(dst_seq_flow, expected);
}


TEST_F(ParseToSeqFlowTest, map_blck_plain__to__seq_blck__root)
{
    NodeRef dst = dst_seq_flow.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(map_blck_plain), dst);
    });
    _c4dbg_tree("actual", dst_seq_flow);
}

TEST_F(ParseToSeqFlowTest, map_blck_plain__to__seq_blck__new_child)
{
    NodeRef dst = dst_seq_flow.rootref().append_child();
    parse_in_arena(to_csubstr(map_blck_plain), dst);
    const Tree expected = parse_in_arena("- {map: block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_seq_flow);
    test_compare(dst_seq_flow, expected);
}


TEST_F(ParseToSeqFlowTest, map_blck_squo__to__seq_blck__root)
{
    NodeRef dst = dst_seq_flow.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(map_blck_squo), dst);
    });
    _c4dbg_tree("actual", dst_seq_flow);
}

TEST_F(ParseToSeqFlowTest, map_blck_squo__to__seq_blck__new_child)
{
    NodeRef dst = dst_seq_flow.rootref().append_child();
    parse_in_arena(to_csubstr(map_blck_squo), dst);
    const Tree expected = parse_in_arena("- {map: block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_seq_flow);
    test_compare(dst_seq_flow, expected);
}


TEST_F(ParseToSeqFlowTest, map_blck_dquo__to__seq_blck__root)
{
    NodeRef dst = dst_seq_flow.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(map_blck_dquo), dst);
    });
    _c4dbg_tree("actual", dst_seq_flow);
}

TEST_F(ParseToSeqFlowTest, map_blck_dquo__to__seq_blck__new_child)
{
    NodeRef dst = dst_seq_flow.rootref().append_child();
    parse_in_arena(to_csubstr(map_blck_dquo), dst);
    const Tree expected = parse_in_arena("- {map: block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_seq_flow);
    test_compare(dst_seq_flow, expected);
}


TEST_F(ParseToSeqFlowTest, map_blck_literal__to__seq_blck__root)
{
    NodeRef dst = dst_seq_flow.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(map_blck_literal), dst);
    });
    _c4dbg_tree("actual", dst_seq_flow);
}

TEST_F(ParseToSeqFlowTest, map_blck_literal__to__seq_blck__new_child)
{
    NodeRef dst = dst_seq_flow.rootref().append_child();
    parse_in_arena(to_csubstr(map_blck_literal), dst);
    const Tree expected = parse_in_arena("- {map: block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_seq_flow);
    test_compare(dst_seq_flow, expected);
}


TEST_F(ParseToSeqFlowTest, map_blck_folded__to__seq_blck__root)
{
    NodeRef dst = dst_seq_flow.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(map_blck_folded), dst);
    });
    _c4dbg_tree("actual", dst_seq_flow);
}

TEST_F(ParseToSeqFlowTest, map_blck_folded__to__seq_blck__new_child)
{
    NodeRef dst = dst_seq_flow.rootref().append_child();
    parse_in_arena(to_csubstr(map_blck_folded), dst);
    const Tree expected = parse_in_arena("- {map: block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_seq_flow);
    test_compare(dst_seq_flow, expected);
}



//-----------------------------------------------------------------------------

class ParseToMapFlowTest : public ParseToNodeTest {};


TEST_F(ParseToMapFlowTest, empty__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    parse_in_arena(to_csubstr(empty), dst);
    EXPECT_TRUE(dst.empty());
}

TEST_F(ParseToMapFlowTest, empty__to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(empty), dst);
    EXPECT_FALSE(dst.has_val());
}


TEST_F(ParseToMapFlowTest, whitespace__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    parse_in_arena(to_csubstr(whitespace), dst);
    EXPECT_TRUE(dst.empty());
}

TEST_F(ParseToMapFlowTest, whitespace__to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(whitespace), dst);
    EXPECT_FALSE(dst.has_val());
}


TEST_F(ParseToMapFlowTest, val_plain__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_plain), dst);
    });
    _c4dbg_tree("actual", dst_map_flow);
}

TEST_F(ParseToMapFlowTest, val_plain__to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(val_plain), dst);
    const Tree expected = parse_in_arena("dst: this is a plain val");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}


TEST_F(ParseToMapFlowTest, val_plain_anchor__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_plain_anchor), dst);
    });
    _c4dbg_tree("actual", dst_map_flow);
}

TEST_F(ParseToMapFlowTest, val_plain_anchor__to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(val_plain_anchor), dst);
    const Tree expected = parse_in_arena("dst: &anchor this is a plain val");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}


TEST_F(ParseToMapFlowTest, val_plain_tag__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_plain_tag), dst);
    });
    _c4dbg_tree("actual", dst_map_flow);
}

TEST_F(ParseToMapFlowTest, val_plain_tag__to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(val_plain_tag), dst);
    const Tree expected = parse_in_arena("dst: !!str this is a plain val");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}


TEST_F(ParseToMapFlowTest, val_squo__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_squo), dst);
    });
    _c4dbg_tree("actual", dst_map_flow);
}

TEST_F(ParseToMapFlowTest, val_squo__to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(val_squo), dst);
    const Tree expected = parse_in_arena("dst: this is a squo val");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}


TEST_F(ParseToMapFlowTest, val_squo_anchor__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_squo_anchor), dst);
    });
    _c4dbg_tree("actual", dst_map_flow);
}

TEST_F(ParseToMapFlowTest, val_squo_anchor__to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(val_squo_anchor), dst);
    const Tree expected = parse_in_arena("dst: &anchor this is a squo val");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}


TEST_F(ParseToMapFlowTest, val_squo_tag__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_squo_tag), dst);
    });
    _c4dbg_tree("actual", dst_map_flow);
}

TEST_F(ParseToMapFlowTest, val_squo_tag__to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(val_squo_tag), dst);
    const Tree expected = parse_in_arena("dst: !!str this is a squo val");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}


TEST_F(ParseToMapFlowTest, val_dquo__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_dquo), dst);
    });
    _c4dbg_tree("actual", dst_map_flow);
}

TEST_F(ParseToMapFlowTest, val_dquo__to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(val_dquo), dst);
    const Tree expected = parse_in_arena("dst: this is a dquo val");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}


TEST_F(ParseToMapFlowTest, val_dquo_anchor__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_dquo_anchor), dst);
    });
    _c4dbg_tree("actual", dst_map_flow);
}

TEST_F(ParseToMapFlowTest, val_dquo_anchor__to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(val_dquo_anchor), dst);
    const Tree expected = parse_in_arena("dst: &anchor this is a dquo val");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}


TEST_F(ParseToMapFlowTest, val_dquo_tag__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_dquo_tag), dst);
    });
    _c4dbg_tree("actual", dst_map_flow);
}

TEST_F(ParseToMapFlowTest, val_dquo_tag__to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(val_dquo_tag), dst);
    const Tree expected = parse_in_arena("dst: !!str this is a dquo val");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}


TEST_F(ParseToMapFlowTest, val_literal__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_literal), dst);
    });
    _c4dbg_tree("actual", dst_map_flow);
}

TEST_F(ParseToMapFlowTest, val_literal__to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(val_literal), dst);
    const Tree expected = parse_in_arena("dst: this is a literal val");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}


TEST_F(ParseToMapFlowTest, val_literal_anchor__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_literal_anchor), dst);
    });
    _c4dbg_tree("actual", dst_map_flow);
}

TEST_F(ParseToMapFlowTest, val_literal_anchor__to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(val_literal_anchor), dst);
    const Tree expected = parse_in_arena("dst: &anchor this is a literal val");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}


TEST_F(ParseToMapFlowTest, val_literal_tag__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_literal_tag), dst);
    });
    _c4dbg_tree("actual", dst_map_flow);
}

TEST_F(ParseToMapFlowTest, val_literal_tag__to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(val_literal_tag), dst);
    const Tree expected = parse_in_arena("dst: !!str this is a literal val");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}


TEST_F(ParseToMapFlowTest, val_folded__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_folded), dst);
    });
    _c4dbg_tree("actual", dst_map_flow);
}

TEST_F(ParseToMapFlowTest, val_folded__to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(val_folded), dst);
    const Tree expected = parse_in_arena("dst: this is a folded val");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}


TEST_F(ParseToMapFlowTest, val_folded_anchor__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_folded_anchor), dst);
    });
    _c4dbg_tree("actual", dst_map_flow);
}

TEST_F(ParseToMapFlowTest, val_folded_anchor__to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(val_folded_anchor), dst);
    const Tree expected = parse_in_arena("dst: &anchor this is a folded val");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}


TEST_F(ParseToMapFlowTest, val_folded_tag__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_folded_tag), dst);
    });
    _c4dbg_tree("actual", dst_map_flow);
}

TEST_F(ParseToMapFlowTest, val_folded_tag__to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(val_folded_tag), dst);
    const Tree expected = parse_in_arena("dst: !!str this is a folded val");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}


TEST_F(ParseToMapFlowTest, val_ref__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_ref), dst);
    });
    _c4dbg_tree("actual", dst_map_flow);
}

TEST_F(ParseToMapFlowTest, val_ref_to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(val_ref), dst);
    const Tree expected = parse_in_arena("dst: *ref");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}


TEST_F(ParseToMapFlowTest, seq_flow__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(seq_flow), dst);
    });
}

TEST_F(ParseToMapFlowTest, seq_flow__to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(seq_flow), dst);
    const Tree expected = parse_in_arena("dst: [seq, flow, yes, it is]");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}


TEST_F(ParseToMapFlowTest, seq_flow_anchor__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(seq_flow_anchor), dst);
    });
}

TEST_F(ParseToMapFlowTest, seq_flow_anchor__to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(seq_flow_anchor), dst);
    const Tree expected = parse_in_arena("dst: &anchor [seq, flow, yes, it is]");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}


TEST_F(ParseToMapFlowTest, seq_flow_tag__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(seq_flow_tag), dst);
    });
}

TEST_F(ParseToMapFlowTest, seq_flow_tag__to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(seq_flow_tag), dst);
    const Tree expected = parse_in_arena("dst: !!seq [seq, flow, yes, it is]");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}


TEST_F(ParseToMapFlowTest, seq_blck__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(seq_blck), dst);
    });
}

TEST_F(ParseToMapFlowTest, seq_blck__to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(seq_blck), dst);
    const Tree expected = parse_in_arena("dst: [seq, block, yes, it is]");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}


TEST_F(ParseToMapFlowTest, map_flow__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    parse_in_arena(to_csubstr(map_flow), dst);
    const Tree expected = parse_in_arena("{map: flow, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}

TEST_F(ParseToMapFlowTest, map_flow__to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(map_flow), dst);
    const Tree expected = parse_in_arena("{dst: {map: flow, yes: it is}}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}

TEST_F(ParseToMapFlowTest, map_flow__to__map_flow__new_child_no_key)
{
    NodeRef dst = dst_map_flow.rootref().append_child();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(map_flow), dst);
    });
}


TEST_F(ParseToMapFlowTest, map_flow_anchor__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    parse_in_arena(to_csubstr(map_flow_anchor), dst);
    const Tree expected = parse_in_arena("&anchor\nmap: flow\nyes: it is\n");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}

TEST_F(ParseToMapFlowTest, map_flow_anchor__to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(map_flow_anchor), dst);
    const Tree expected = parse_in_arena("dst: &anchor\n  map: flow\n  yes: it is\n");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}


TEST_F(ParseToMapFlowTest, map_flow_tag__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    parse_in_arena(to_csubstr(map_flow_tag), dst);
    const Tree expected = parse_in_arena("!!tag\nmap: flow\nyes: it is\n");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}

TEST_F(ParseToMapFlowTest, map_flow_tag__to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(map_flow_tag), dst);
    const Tree expected = parse_in_arena("dst: !!tag\n  map: flow\n  yes: it is\n");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}


TEST_F(ParseToMapFlowTest, map_block_keyless__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    parse_in_arena(to_csubstr(map_blck_keyless), dst);
    const Tree expected = parse_in_arena("{ : block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}

TEST_F(ParseToMapFlowTest, map_block_keyless__to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(map_blck_keyless), dst);
    const Tree expected = parse_in_arena("dst : { : block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}


TEST_F(ParseToMapFlowTest, map_block_plain__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    parse_in_arena(to_csubstr(map_blck_plain), dst);
    const Tree expected = parse_in_arena("{map: block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}

TEST_F(ParseToMapFlowTest, map_block_plain__to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(map_blck_plain), dst);
    const Tree expected = parse_in_arena("dst: {map: block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}


TEST_F(ParseToMapFlowTest, map_block_squo__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    parse_in_arena(to_csubstr(map_blck_squo), dst);
    const Tree expected = parse_in_arena("{map: block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}

TEST_F(ParseToMapFlowTest, map_block_squo__to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(map_blck_squo), dst);
    const Tree expected = parse_in_arena("dst: {map: block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}


TEST_F(ParseToMapFlowTest, map_block_dquo__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    parse_in_arena(to_csubstr(map_blck_dquo), dst);
    const Tree expected = parse_in_arena("{map: block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}

TEST_F(ParseToMapFlowTest, map_block_dquo__to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(map_blck_dquo), dst);
    const Tree expected = parse_in_arena("dst: {map: block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}


TEST_F(ParseToMapFlowTest, map_block_literal__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    parse_in_arena(to_csubstr(map_blck_literal), dst);
    const Tree expected = parse_in_arena("{map: block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}

TEST_F(ParseToMapFlowTest, map_block_literal__to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(map_blck_literal), dst);
    const Tree expected = parse_in_arena("dst: {map: block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}


TEST_F(ParseToMapFlowTest, map_block_folded__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    parse_in_arena(to_csubstr(map_blck_folded), dst);
    const Tree expected = parse_in_arena("{map: block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}

TEST_F(ParseToMapFlowTest, map_block_folded__to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(map_blck_folded), dst);
    const Tree expected = parse_in_arena("dst: {map: block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}


TEST_F(ParseToMapFlowTest, map_block_anchor__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    parse_in_arena(to_csubstr(map_blck_anchor), dst);
    const Tree expected = parse_in_arena("&anchor {map: block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}

TEST_F(ParseToMapFlowTest, map_block_anchor__to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(map_blck_anchor), dst);
    const Tree expected = parse_in_arena("dst: &anchor {map: block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}


TEST_F(ParseToMapFlowTest, map_block_tag__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    parse_in_arena(to_csubstr(map_blck_tag), dst);
    const Tree expected = parse_in_arena("!!tag {map: block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}

TEST_F(ParseToMapFlowTest, map_block_tag__to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(map_blck_tag), dst);
    const Tree expected = parse_in_arena("dst: !!tag {map: block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}


TEST_F(ParseToMapFlowTest, map_block_ref__to__map_flow__root)
{
    NodeRef dst = dst_map_flow.rootref();
    parse_in_arena(to_csubstr(map_blck_ref), dst);
    const Tree expected = parse_in_arena("{*map : block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}

TEST_F(ParseToMapFlowTest, map_block_ref__to__map_flow__new_child)
{
    NodeRef dst = dst_map_flow.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(map_blck_ref), dst);
    const Tree expected = parse_in_arena("dst: {*map : block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_flow);
    test_compare(dst_map_flow, expected);
}



//-----------------------------------------------------------------------------

class ParseToMapBlockTest : public ParseToNodeTest {};


TEST_F(ParseToMapBlockTest, val_plain__to__map_flow__root)
{
    NodeRef dst = dst_map_blck.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_plain), dst);
    });
    _c4dbg_tree("actual", dst_map_blck);
}

TEST_F(ParseToMapBlockTest, val_plain__to__map_flow__new_child)
{
    NodeRef dst = dst_map_blck.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(val_plain), dst);
    const Tree expected = parse_in_arena("dst: this is a plain val");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}


TEST_F(ParseToMapBlockTest, val_plain_anchor__to__map_flow__root)
{
    NodeRef dst = dst_map_blck.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_plain_anchor), dst);
    });
    _c4dbg_tree("actual", dst_map_blck);
}

TEST_F(ParseToMapBlockTest, val_plain_anchor__to__map_flow__new_child)
{
    NodeRef dst = dst_map_blck.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(val_plain_anchor), dst);
    const Tree expected = parse_in_arena("dst: &anchor this is a plain val");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}


TEST_F(ParseToMapBlockTest, val_plain_tag__to__map_flow__root)
{
    NodeRef dst = dst_map_blck.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_plain_tag), dst);
    });
    _c4dbg_tree("actual", dst_map_blck);
}

TEST_F(ParseToMapBlockTest, val_plain_tag__to__map_flow__new_child)
{
    NodeRef dst = dst_map_blck.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(val_plain_tag), dst);
    const Tree expected = parse_in_arena("dst: !!str this is a plain val");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}


TEST_F(ParseToMapBlockTest, val_squo__to__map_flow__root)
{
    NodeRef dst = dst_map_blck.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_squo), dst);
    });
    _c4dbg_tree("actual", dst_map_blck);
}

TEST_F(ParseToMapBlockTest, val_squo__to__map_flow__new_child)
{
    NodeRef dst = dst_map_blck.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(val_squo), dst);
    const Tree expected = parse_in_arena("dst: this is a squo val");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}


TEST_F(ParseToMapBlockTest, val_squo_anchor__to__map_flow__root)
{
    NodeRef dst = dst_map_blck.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_squo_anchor), dst);
    });
    _c4dbg_tree("actual", dst_map_blck);
}

TEST_F(ParseToMapBlockTest, val_squo_anchor__to__map_flow__new_child)
{
    NodeRef dst = dst_map_blck.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(val_squo_anchor), dst);
    const Tree expected = parse_in_arena("dst: &anchor this is a squo val");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}


TEST_F(ParseToMapBlockTest, val_squo_tag__to__map_flow__root)
{
    NodeRef dst = dst_map_blck.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_squo_tag), dst);
    });
    _c4dbg_tree("actual", dst_map_blck);
}

TEST_F(ParseToMapBlockTest, val_squo_tag__to__map_flow__new_child)
{
    NodeRef dst = dst_map_blck.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(val_squo_tag), dst);
    const Tree expected = parse_in_arena("dst: !!str this is a squo val");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}


TEST_F(ParseToMapBlockTest, val_dquo__to__map_flow__root)
{
    NodeRef dst = dst_map_blck.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_dquo), dst);
    });
    _c4dbg_tree("actual", dst_map_blck);
}

TEST_F(ParseToMapBlockTest, val_dquo__to__map_flow__new_child)
{
    NodeRef dst = dst_map_blck.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(val_dquo), dst);
    const Tree expected = parse_in_arena("dst: this is a dquo val");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}


TEST_F(ParseToMapBlockTest, val_dquo_anchor__to__map_flow__root)
{
    NodeRef dst = dst_map_blck.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_dquo_anchor), dst);
    });
    _c4dbg_tree("actual", dst_map_blck);
}

TEST_F(ParseToMapBlockTest, val_dquo_anchor__to__map_flow__new_child)
{
    NodeRef dst = dst_map_blck.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(val_dquo_anchor), dst);
    const Tree expected = parse_in_arena("dst: &anchor this is a dquo val");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}


TEST_F(ParseToMapBlockTest, val_dquo_tag__to__map_flow__root)
{
    NodeRef dst = dst_map_blck.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_dquo_tag), dst);
    });
    _c4dbg_tree("actual", dst_map_blck);
}

TEST_F(ParseToMapBlockTest, val_dquo_tag__to__map_flow__new_child)
{
    NodeRef dst = dst_map_blck.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(val_dquo_tag), dst);
    const Tree expected = parse_in_arena("dst: !!str this is a dquo val");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}


TEST_F(ParseToMapBlockTest, val_literal__to__map_flow__root)
{
    NodeRef dst = dst_map_blck.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_literal), dst);
    });
    _c4dbg_tree("actual", dst_map_blck);
}

TEST_F(ParseToMapBlockTest, val_literal__to__map_flow__new_child)
{
    NodeRef dst = dst_map_blck.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(val_literal), dst);
    const Tree expected = parse_in_arena("dst: this is a literal val");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}


TEST_F(ParseToMapBlockTest, val_literal_anchor__to__map_flow__root)
{
    NodeRef dst = dst_map_blck.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_literal_anchor), dst);
    });
    _c4dbg_tree("actual", dst_map_blck);
}

TEST_F(ParseToMapBlockTest, val_literal_anchor__to__map_flow__new_child)
{
    NodeRef dst = dst_map_blck.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(val_literal_anchor), dst);
    const Tree expected = parse_in_arena("dst: &anchor this is a literal val");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}


TEST_F(ParseToMapBlockTest, val_literal_tag__to__map_flow__root)
{
    NodeRef dst = dst_map_blck.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_literal_tag), dst);
    });
    _c4dbg_tree("actual", dst_map_blck);
}

TEST_F(ParseToMapBlockTest, val_literal_tag__to__map_flow__new_child)
{
    NodeRef dst = dst_map_blck.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(val_literal_tag), dst);
    const Tree expected = parse_in_arena("dst: !!str this is a literal val");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}


TEST_F(ParseToMapBlockTest, val_folded__to__map_flow__root)
{
    NodeRef dst = dst_map_blck.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_folded), dst);
    });
    _c4dbg_tree("actual", dst_map_blck);
}

TEST_F(ParseToMapBlockTest, val_folded__to__map_flow__new_child)
{
    NodeRef dst = dst_map_blck.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(val_folded), dst);
    const Tree expected = parse_in_arena("dst: this is a folded val");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}


TEST_F(ParseToMapBlockTest, val_folded_anchor__to__map_flow__root)
{
    NodeRef dst = dst_map_blck.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_folded_anchor), dst);
    });
    _c4dbg_tree("actual", dst_map_blck);
}

TEST_F(ParseToMapBlockTest, val_folded_anchor__to__map_flow__new_child)
{
    NodeRef dst = dst_map_blck.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(val_folded_anchor), dst);
    const Tree expected = parse_in_arena("dst: &anchor this is a folded val");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}


TEST_F(ParseToMapBlockTest, val_folded_tag__to__map_flow__root)
{
    NodeRef dst = dst_map_blck.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_folded_tag), dst);
    });
    _c4dbg_tree("actual", dst_map_blck);
}

TEST_F(ParseToMapBlockTest, val_folded_tag__to__map_flow__new_child)
{
    NodeRef dst = dst_map_blck.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(val_folded_tag), dst);
    const Tree expected = parse_in_arena("dst: !!str this is a folded val");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}


TEST_F(ParseToMapBlockTest, val_ref__to__map_flow__root)
{
    NodeRef dst = dst_map_blck.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(val_ref), dst);
    });
    _c4dbg_tree("actual", dst_map_blck);
}

TEST_F(ParseToMapBlockTest, val_ref_to__map_flow__new_child)
{
    NodeRef dst = dst_map_blck.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(val_ref), dst);
    const Tree expected = parse_in_arena("dst: *ref");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}


TEST_F(ParseToMapBlockTest, seq_flow__to__map_flow__root)
{
    NodeRef dst = dst_map_blck.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(seq_flow), dst);
    });
}

TEST_F(ParseToMapBlockTest, seq_flow__to__map_flow__new_child)
{
    NodeRef dst = dst_map_blck.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(seq_flow), dst);
    const Tree expected = parse_in_arena("dst: [seq, flow, yes, it is]");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}


TEST_F(ParseToMapBlockTest, seq_flow_anchor__to__map_flow__root)
{
    NodeRef dst = dst_map_blck.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(seq_flow_anchor), dst);
    });
}

TEST_F(ParseToMapBlockTest, seq_flow_anchor__to__map_flow__new_child)
{
    NodeRef dst = dst_map_blck.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(seq_flow_anchor), dst);
    const Tree expected = parse_in_arena("dst: &anchor [seq, flow, yes, it is]");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}


TEST_F(ParseToMapBlockTest, seq_flow_tag__to__map_flow__root)
{
    NodeRef dst = dst_map_blck.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(seq_flow_tag), dst);
    });
}

TEST_F(ParseToMapBlockTest, seq_flow_tag__to__map_flow__new_child)
{
    NodeRef dst = dst_map_blck.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(seq_flow_tag), dst);
    const Tree expected = parse_in_arena("dst: !!seq [seq, flow, yes, it is]");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}


TEST_F(ParseToMapBlockTest, seq_blck__to__map_flow__root)
{
    NodeRef dst = dst_map_blck.rootref();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(seq_blck), dst);
    });
}

TEST_F(ParseToMapBlockTest, seq_blck__to__map_flow__new_child)
{
    NodeRef dst = dst_map_blck.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(seq_blck), dst);
    const Tree expected = parse_in_arena("dst: [seq, block, yes, it is]");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}


TEST_F(ParseToMapBlockTest, map_flow__to__map_flow__root)
{
    NodeRef dst = dst_map_blck.rootref();
    parse_in_arena(to_csubstr(map_flow), dst);
    const Tree expected = parse_in_arena("map: flow\nyes: it is\n");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}

TEST_F(ParseToMapBlockTest, map_flow__to__map_flow__new_child)
{
    NodeRef dst = dst_map_blck.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(map_flow), dst);
    const Tree expected = parse_in_arena("dst:\n  map: flow\n  yes: it is\n");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}

TEST_F(ParseToMapBlockTest, map_flow__to__map_flow__new_child_no_key)
{
    NodeRef dst = dst_map_blck.rootref().append_child();
    ExpectError::check_error(dst.tree(), [&]{
        parse_in_arena(to_csubstr(map_flow), dst);
    });
}


TEST_F(ParseToMapBlockTest, map_flow_anchor__to__map_flow__root)
{
    NodeRef dst = dst_map_blck.rootref();
    parse_in_arena(to_csubstr(map_flow_anchor), dst);
    const Tree expected = parse_in_arena("&anchor\nmap: flow\nyes: it is\n");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}

TEST_F(ParseToMapBlockTest, map_flow_anchor__to__map_flow__new_child)
{
    NodeRef dst = dst_map_blck.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(map_flow_anchor), dst);
    const Tree expected = parse_in_arena("dst: &anchor\n  map: flow\n  yes: it is\n");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}


TEST_F(ParseToMapBlockTest, map_flow_tag__to__map_flow__root)
{
    NodeRef dst = dst_map_blck.rootref();
    parse_in_arena(to_csubstr(map_flow_tag), dst);
    const Tree expected = parse_in_arena("!!tag\nmap: flow\nyes: it is\n");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}

TEST_F(ParseToMapBlockTest, map_flow_tag__to__map_flow__new_child)
{
    NodeRef dst = dst_map_blck.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(map_flow_tag), dst);
    const Tree expected = parse_in_arena("dst: !!tag\n  map: flow\n  yes: it is\n");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}


TEST_F(ParseToMapBlockTest, map_block_keyless__to__map_flow__root)
{
    NodeRef dst = dst_map_blck.rootref();
    parse_in_arena(to_csubstr(map_blck_keyless), dst);
    const Tree expected = parse_in_arena("{ : block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}

TEST_F(ParseToMapBlockTest, map_block_keyless__to__map_flow__new_child)
{
    NodeRef dst = dst_map_blck.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(map_blck_keyless), dst);
    const Tree expected = parse_in_arena("dst : { : block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}


TEST_F(ParseToMapBlockTest, map_block_plain__to__map_flow__root)
{
    NodeRef dst = dst_map_blck.rootref();
    parse_in_arena(to_csubstr(map_blck_plain), dst);
    const Tree expected = parse_in_arena("{map: block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}

TEST_F(ParseToMapBlockTest, map_block_plain__to__map_flow__new_child)
{
    NodeRef dst = dst_map_blck.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(map_blck_plain), dst);
    const Tree expected = parse_in_arena("dst: {map: block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}


TEST_F(ParseToMapBlockTest, map_block_squo__to__map_flow__root)
{
    NodeRef dst = dst_map_blck.rootref();
    parse_in_arena(to_csubstr(map_blck_squo), dst);
    const Tree expected = parse_in_arena("{map: block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}

TEST_F(ParseToMapBlockTest, map_block_squo__to__map_flow__new_child)
{
    NodeRef dst = dst_map_blck.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(map_blck_squo), dst);
    const Tree expected = parse_in_arena("dst: {map: block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}


TEST_F(ParseToMapBlockTest, map_block_dquo__to__map_flow__root)
{
    NodeRef dst = dst_map_blck.rootref();
    parse_in_arena(to_csubstr(map_blck_dquo), dst);
    const Tree expected = parse_in_arena("{map: block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}

TEST_F(ParseToMapBlockTest, map_block_dquo__to__map_flow__new_child)
{
    NodeRef dst = dst_map_blck.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(map_blck_dquo), dst);
    const Tree expected = parse_in_arena("dst: {map: block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}


TEST_F(ParseToMapBlockTest, map_block_literal__to__map_flow__root)
{
    NodeRef dst = dst_map_blck.rootref();
    parse_in_arena(to_csubstr(map_blck_literal), dst);
    const Tree expected = parse_in_arena("{map: block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}

TEST_F(ParseToMapBlockTest, map_block_literal__to__map_flow__new_child)
{
    NodeRef dst = dst_map_blck.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(map_blck_literal), dst);
    const Tree expected = parse_in_arena("dst: {map: block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}


TEST_F(ParseToMapBlockTest, map_block_folded__to__map_flow__root)
{
    NodeRef dst = dst_map_blck.rootref();
    parse_in_arena(to_csubstr(map_blck_folded), dst);
    const Tree expected = parse_in_arena("{map: block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}

TEST_F(ParseToMapBlockTest, map_block_folded__to__map_flow__new_child)
{
    NodeRef dst = dst_map_blck.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(map_blck_folded), dst);
    const Tree expected = parse_in_arena("dst: {map: block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}


TEST_F(ParseToMapBlockTest, map_block_anchor__to__map_flow__root)
{
    NodeRef dst = dst_map_blck.rootref();
    parse_in_arena(to_csubstr(map_blck_anchor), dst);
    const Tree expected = parse_in_arena("&anchor {map: block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}

TEST_F(ParseToMapBlockTest, map_block_anchor__to__map_flow__new_child)
{
    NodeRef dst = dst_map_blck.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(map_blck_anchor), dst);
    const Tree expected = parse_in_arena("dst: &anchor {map: block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}


TEST_F(ParseToMapBlockTest, map_block_tag__to__map_flow__root)
{
    NodeRef dst = dst_map_blck.rootref();
    parse_in_arena(to_csubstr(map_blck_tag), dst);
    const Tree expected = parse_in_arena("!!tag {map: block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}

TEST_F(ParseToMapBlockTest, map_block_tag__to__map_flow__new_child)
{
    NodeRef dst = dst_map_blck.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(map_blck_tag), dst);
    const Tree expected = parse_in_arena("dst: !!tag {map: block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}


TEST_F(ParseToMapBlockTest, map_block_ref__to__map_flow__root)
{
    NodeRef dst = dst_map_blck.rootref();
    parse_in_arena(to_csubstr(map_blck_ref), dst);
    const Tree expected = parse_in_arena("{*map : block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}

TEST_F(ParseToMapBlockTest, map_block_ref__to__map_flow__new_child)
{
    NodeRef dst = dst_map_blck.rootref().append_child({KEY, "dst"});
    parse_in_arena(to_csubstr(map_blck_ref), dst);
    const Tree expected = parse_in_arena("dst: {*map : block, yes: it is}");
    _c4dbg_tree("expected", expected);
    _c4dbg_tree("actual", dst_map_blck);
    test_compare(dst_map_blck, expected);
}




} // namespace yml
} // namespace c4


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// this is needed to use the test case library

#ifndef RYML_SINGLE_HEADER
#include "c4/substr.hpp"
#endif

namespace c4 {
namespace yml {
struct Case;
Case const* get_case(csubstr /*name*/)
{
    return nullptr;
}

C4_SUPPRESS_WARNING_GCC_CLANG_POP

} // namespace yml
} // namespace c4
