#include "./test_tree.hpp"

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

TEST(Tree, empty_ctor)
{
    Tree tree;
    EXPECT_EQ(tree.callbacks(), get_callbacks());
    EXPECT_EQ(tree.empty(), RYML_DEFAULT_TREE_CAPACITY == 0);
    EXPECT_EQ(tree.capacity(), RYML_DEFAULT_TREE_CAPACITY);
    EXPECT_EQ(tree.size(), RYML_DEFAULT_TREE_CAPACITY != 0); // the root
    EXPECT_EQ(tree.slack(), RYML_DEFAULT_TREE_CAPACITY ? RYML_DEFAULT_TREE_CAPACITY - 1u : 0);
    EXPECT_EQ(tree.arena_capacity(), 0u);
    EXPECT_EQ(tree.arena_slack(), 0u);
    EXPECT_EQ(tree.arena().empty(), true);
}

TEST(Tree, node_cap_ctor)
{
    {
        Tree tree(0u);
        EXPECT_EQ(tree.callbacks(), get_callbacks());
        EXPECT_EQ(tree.empty(), true);
        EXPECT_EQ(tree.capacity(), 0u);
        EXPECT_EQ(tree.arena_capacity(), 0u);
        EXPECT_EQ(tree.arena_slack(), 0u);
        EXPECT_EQ(tree.size(), 0u);
        EXPECT_EQ(tree.slack(), 0u);
        EXPECT_EQ(tree.arena().empty(), true);
    }
    {
        Tree tree(10u);
        EXPECT_EQ(tree.callbacks(), get_callbacks());
        EXPECT_EQ(tree.empty(), false); // we have the root
        EXPECT_EQ(tree.capacity(), 10u);
        EXPECT_EQ(tree.arena_capacity(), 0u);
        EXPECT_EQ(tree.arena_slack(), 0u);
        EXPECT_EQ(tree.arena().empty(), true);
        EXPECT_EQ(tree.size(), 1u); // we have the root
        EXPECT_EQ(tree.slack(), 9u);
    }
    {
        Tree tree(10u, 20u);
        EXPECT_EQ(tree.callbacks(), get_callbacks());
        EXPECT_EQ(tree.empty(), false); // we have the root
        EXPECT_EQ(tree.capacity(), 10u);
        EXPECT_EQ(tree.arena_capacity(), 20u);
        EXPECT_EQ(tree.arena().empty(), true);
        EXPECT_EQ(tree.size(), 1u); // we have the root
        EXPECT_EQ(tree.slack(), 9u);
    }
    {
        Tree tree(0u, 20u);
        EXPECT_EQ(tree.callbacks(), get_callbacks());
        EXPECT_EQ(tree.empty(), true);
        EXPECT_EQ(tree.capacity(), 0u);
        EXPECT_EQ(tree.arena_capacity(), 20u);
        EXPECT_EQ(tree.arena_slack(), 20u);
        EXPECT_EQ(tree.arena().empty(), true);
        EXPECT_EQ(tree.size(), 0u);
        EXPECT_EQ(tree.slack(), 0u);
    }
}

Tree get_test_tree(CallbacksTester *cbt=nullptr)
{
    Parser::handler_type evt_handler(cbt ? cbt->callbacks() : get_callbacks());
    Parser parser(&evt_handler);
    Tree t = parse_in_arena(&parser, "", "{a: b, c: d, e: [0, 1, 2, 3]}");
    // make sure the tree has strings in its arena
    NodeRef n = t.rootref();
    NodeRef ch = n.append_child();
    ch << key("serialized_key");
    ch << 89;
    return t;
}

TEST(Tree, test_tree_has_arena)
{
    {
        Tree t = get_test_tree();
        ASSERT_GT(t.arena().size(), 0u);
    }
    {
        CallbacksTester cbt;
        Tree t = get_test_tree(&cbt);
        ASSERT_GT(t.arena().size(), 0u);
    }
}

//-------------------------------------------

TEST(Tree, copy_ctor_empty)
{
    CallbacksTester cbt;
    {
        Tree src(cbt.callbacks());
        test_invariants(src);
        {
            Tree dst(src);
            test_invariants(dst);
            test_compare(dst, src);
            test_arena_not_shared(dst, src);
            EXPECT_EQ(dst.callbacks(), src.callbacks());
        }
    }
    {
        Tree src(cbt.callbacks());
        test_invariants(src);
        {
            Tree dst = src;
            test_invariants(dst);
            test_compare(dst, src);
            test_arena_not_shared(dst, src);
            EXPECT_EQ(dst.callbacks(), src.callbacks());
        }
    }
}

TEST(Tree, copy_ctor)
{
    CallbacksTester cbt;
    {
        Tree src = get_test_tree(&cbt);
        test_invariants(src);
        {
            Tree dst(src);
            test_invariants(dst);
            test_compare(dst, src);
            test_arena_not_shared(dst, src);
            EXPECT_EQ(dst.callbacks(), src.callbacks());
        }
    }
}

//-------------------------------------------
TEST(Tree, move_ctor)
{
    CallbacksTester cbt;
    Tree src = get_test_tree(&cbt);
    test_invariants(src);
    Tree save(src);
    test_invariants(save);
    test_compare(save, src);
    {
        Tree dst(std::move(src));
        EXPECT_EQ(src.m_size, 0u);
        EXPECT_EQ(src.m_arena_pos, 0u);
        EXPECT_EQ(dst.size(), save.size());
        EXPECT_EQ(dst.arena(), save.arena());
        test_invariants(src);
        test_invariants(dst);
        test_compare(dst, save);
        test_arena_not_shared(src, dst);
        test_arena_not_shared(save, dst);
    }
}

//-------------------------------------------
TEST(Tree, copy_assign_same_callbacks)
{
    CallbacksTester cbt;
    {
        Tree src = get_test_tree(&cbt);
        test_invariants(src);
        {
            Tree dst(cbt.callbacks());
            EXPECT_EQ(dst.callbacks(), src.callbacks());
            test_invariants(dst);
            dst = src;
            test_invariants(dst);
            test_compare(dst, src);
            test_arena_not_shared(dst, src);
            EXPECT_EQ(dst.callbacks(), src.callbacks());
        }
    }
}

TEST(Tree, copy_assign_diff_callbacks)
{
    CallbacksTester cbsrc("src");
    CallbacksTester cbdst("dst");
    {
        Tree src = get_test_tree(&cbsrc);
        EXPECT_EQ(src.callbacks(), cbsrc.callbacks());
        test_invariants(src);
        {
            Tree dst = get_test_tree(&cbdst);
            EXPECT_EQ(dst.callbacks(), cbdst.callbacks());
            test_invariants(dst);
            dst = src;
            test_invariants(dst);
            test_compare(dst, src);
            test_arena_not_shared(dst, src);
            EXPECT_EQ(dst.callbacks(), src.callbacks());
        }
    }
}

//-------------------------------------------
TEST(Tree, move_assign_same_callbacks)
{
    CallbacksTester cbt;
    Tree src = get_test_tree(&cbt);
    test_invariants(src);
    Tree save(src);
    EXPECT_EQ(save.callbacks(), src.callbacks());
    test_invariants(save);
    test_compare(save, src);
    {
        Tree dst = get_test_tree(&cbt);
        EXPECT_NE(dst.empty(), true);
        EXPECT_NE(dst.size(), 0u);
        EXPECT_NE(dst.arena().empty(), true);
        dst = std::move(src);
        EXPECT_EQ(src.m_size, 0u);
        EXPECT_EQ(src.m_arena_pos, 0u);
        EXPECT_EQ(src.m_callbacks, cbt.callbacks());
        EXPECT_EQ(dst.size(), save.size());
        EXPECT_EQ(dst.arena(), save.arena());
        EXPECT_EQ(dst.callbacks(), save.callbacks());
        test_invariants(src);
        test_invariants(dst);
        test_compare(dst, save);
        test_arena_not_shared(src, dst);
        test_arena_not_shared(save, dst);
    }
}

TEST(Tree, move_assign_diff_callbacks)
{
    CallbacksTester cbsrc("src");
    CallbacksTester cbdst("dst");
    Tree src = get_test_tree(&cbsrc);
    test_invariants(src);
    Tree save(src);
    test_invariants(save);
    test_compare(save, src);
    {
        Tree dst = get_test_tree(&cbdst);
        EXPECT_NE(dst.empty(), true);
        EXPECT_NE(dst.size(), 0u);
        EXPECT_NE(dst.arena().empty(), true);
        EXPECT_EQ(dst.callbacks(), cbdst.callbacks());
        dst = std::move(src);
        EXPECT_EQ(src.m_size, 0u);
        EXPECT_EQ(src.m_arena_pos, 0u);
        EXPECT_EQ(src.m_callbacks, cbsrc.callbacks());
        EXPECT_EQ(dst.size(), save.size());
        EXPECT_EQ(dst.arena(), save.arena());
        EXPECT_NE(dst.callbacks(), cbdst.callbacks());
        EXPECT_EQ(dst.callbacks(), save.callbacks());
        test_invariants(src);
        test_invariants(dst);
        test_compare(dst, save);
        test_arena_not_shared(src, dst);
        test_arena_not_shared(save, dst);
    }
}

TEST(Tree, std_interop)
{
    CallbacksTester cbt;
    std::vector<Tree> forest;
    for(size_t i = 0; i < 3; ++i)
    {
        forest.emplace_back(cbt.callbacks());
        parse_in_arena("{foo: bar}", &forest.back());
    }
}


//-------------------------------------------
TEST(Tree, reserve)
{
    Tree t(16, 64);
    EXPECT_EQ(t.capacity(), 16);
    EXPECT_EQ(t.slack(), 15);
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t.arena_capacity(), 64);
    EXPECT_EQ(t.arena_slack(), 64);
    EXPECT_EQ(t.arena_size(), 0);
    test_invariants(t);

    auto buf = t.m_buf;
    t.reserve(16);
    t.reserve_arena(64);
    EXPECT_EQ(t.m_buf, buf);
    EXPECT_EQ(t.capacity(), 16);
    EXPECT_EQ(t.slack(), 15);
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t.arena_capacity(), 64);
    EXPECT_EQ(t.arena_slack(), 64);
    EXPECT_EQ(t.arena_size(), 0);
    test_invariants(t);

    t.reserve(32);
    t.reserve_arena(128);
    EXPECT_EQ(t.capacity(), 32);
    EXPECT_EQ(t.slack(), 31);
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t.arena_capacity(), 128);
    EXPECT_EQ(t.arena_slack(), 128);
    EXPECT_EQ(t.arena_size(), 0);
    test_invariants(t);

    buf = t.m_buf;
    parse_in_arena("[a, b, c, d, e, f]", &t);
    EXPECT_EQ(t.m_buf, buf);
    EXPECT_EQ(t.capacity(), 32);
    EXPECT_EQ(t.slack(), 25);
    EXPECT_EQ(t.size(), 7);
    EXPECT_EQ(t.arena_capacity(), 128);
    EXPECT_EQ(t.arena_slack(), 110);
    EXPECT_EQ(t.arena_size(), 18);
    test_invariants(t);

    t.reserve(64);
    t.reserve_arena(256);
    EXPECT_EQ(t.capacity(), 64);
    EXPECT_EQ(t.slack(), 57);
    EXPECT_EQ(t.size(), 7);
    EXPECT_EQ(t.arena_capacity(), 256);
    EXPECT_EQ(t.arena_slack(), 238);
    EXPECT_EQ(t.arena_size(), 18);
    test_invariants(t);
}

TEST(Tree, reserve_empty_and_reused)
{
    {
        Tree t(0, 0);
        EXPECT_EQ(t.capacity(), 0);
        t.reserve(100);
        EXPECT_EQ(t.capacity(), 100);
        EXPECT_EQ(t.size(), 1);
        EXPECT_EQ(t.slack(), 99);
        test_invariants(t);
        parse_in_arena("[a, b, c]", &t);
        EXPECT_EQ(t.size(), 4);
        EXPECT_EQ(t.capacity(), 100);
        test_invariants(t);
    }
    {
        Tree t;
        parse_in_arena("{a: [0, 1, 2], b: [3, 4, 5], c: [6, 7, 8], d: [9, 10, 11]}", &t);
        EXPECT_EQ(t.size(), 17);
        EXPECT_EQ(t.capacity(), 32);
        t.clear();
        t.clear_arena();
        t.reserve(16);
        EXPECT_EQ(t.capacity(), 32);
        EXPECT_EQ(t.size(), 1);
        t.reserve(1000);
        EXPECT_EQ(t.capacity(), 1000);
        EXPECT_EQ(t.size(), 1);
        EXPECT_EQ(t.slack(), 999);
        test_invariants(t);
        parse_in_arena("[a, b, c]", &t);
        EXPECT_EQ(t.size(), 4);
        EXPECT_EQ(t.capacity(), 1000);
        EXPECT_EQ(emitrs_yaml<std::string>(t), "[a,b,c]");
        test_invariants(t);
    }
    {
        Tree t;
        parse_in_arena("[a, b, c]", &t);
        t.reserve(5000);
        EXPECT_EQ(t.capacity(), 5000);
        EXPECT_EQ(t.size(), 4);
        EXPECT_EQ(emitrs_yaml<std::string>(t), "[a,b,c]");
        test_invariants(t);
    }
}

namespace {
struct MakeResidentCall
{
    void *mem;
    size_t len;
    bool freed;
};
std::vector<MakeResidentCall> s_make_resident_calls;
pfn_free s_free_before = nullptr;
void make_resident_counting(void *mem, size_t len, void *user_data)
{
    s_make_resident_calls.push_back(MakeResidentCall{mem, len, false});
    make_resident(mem, len, user_data);
}
// a freed block's address may be given to a later allocation, so
// the calls on it must not be counted for the new block
void free_counting(void *mem, size_t len, void *user_data)
{
    for(MakeResidentCall &call : s_make_resident_calls)
        if(call.mem == mem)
            call.freed = true;
    s_free_before(mem, len, user_data);
}
Callbacks counting_callbacks(Callbacks cb)
{
    s_free_before = cb.m_free;
    cb.set_make_resident(&make_resident_counting);
    cb.set_free(&free_counting);
    return cb;
}
size_t num_make_resident_calls_on(void const* mem)
{
    size_t num = 0;
    for(MakeResidentCall const& call : s_make_resident_calls)
        num += (call.mem == mem && !call.freed);
    return num;
}
} // namespace

TEST(Tree, reserve_without_make_resident)
{
    Tree t(0, 0);
    EXPECT_EQ(t.callbacks().m_make_resident, nullptr);
    t.reserve(100);
    EXPECT_EQ(t.capacity(), 100);
    parse_in_arena("[a, b, c]", &t);
    EXPECT_EQ(emitrs_yaml<std::string>(t), "[a,b,c]");
}

TEST(Tree, reserve_calls_make_resident)
{
    s_make_resident_calls.clear();
    Callbacks cb = counting_callbacks(get_callbacks());
    Tree t(0, 0, cb);
    EXPECT_TRUE(s_make_resident_calls.empty());
    t.reserve(100);
    ASSERT_EQ(s_make_resident_calls.size(), 1u);
    EXPECT_EQ(s_make_resident_calls[0].mem, static_cast<void*>(t.get(0)));
    EXPECT_EQ(s_make_resident_calls[0].len, 100u * sizeof(NodeData));
    t.reserve(50); // no allocation, no call
    EXPECT_EQ(s_make_resident_calls.size(), 1u);
    parse_in_arena("[a, b, c]", &t);
    EXPECT_EQ(num_make_resident_calls_on(t.get(0)), 1u); // the nodes did not grow
    EXPECT_EQ(emitrs_yaml<std::string>(t), "[a,b,c]");
    // growth inside the parse also goes through reserve()
    std::string src = "[";
    for(int i = 0; i < 200; ++i)
        src += "x, ";
    src += "y]";
    t.clear();
    t.clear_arena();
    const size_t before = s_make_resident_calls.size();
    parse_in_arena(to_csubstr(src), &t);
    EXPECT_EQ(num_make_resident_calls_on(t.get(0)), 1u);
    EXPECT_GT(s_make_resident_calls.size(), before);
    bool found = false;
    for(MakeResidentCall const& call : s_make_resident_calls)
        if(call.mem == static_cast<void*>(t.get(0)) && !call.freed)
            found = (call.len == static_cast<size_t>(t.capacity()) * sizeof(NodeData));
    EXPECT_TRUE(found);
    EXPECT_EQ(t.rootref().num_children(), 201);
    test_invariants(t);
    s_make_resident_calls.clear();
}

TEST(Tree, arena_calls_make_resident)
{
    s_make_resident_calls.clear();
    Callbacks cb = counting_callbacks(get_callbacks());
    Tree t(0, 0, cb);
    t.reserve_arena(64);
    ASSERT_EQ(s_make_resident_calls.size(), 1u);
    EXPECT_EQ(s_make_resident_calls[0].mem, static_cast<void const*>(t.arena().str));
    EXPECT_EQ(s_make_resident_calls[0].len, 64u);
    s_make_resident_calls.clear();
}

TEST(Tree, copy_calls_make_resident)
{
    s_make_resident_calls.clear();
    Callbacks cb = counting_callbacks(get_callbacks());
    Tree t(cb);
    parse_in_arena("{a: b, c: [d, e]}", &t);
    s_make_resident_calls.clear();
    Tree copy(t); // NOLINT(performance-unnecessary-copy-initialization)
    EXPECT_EQ(num_make_resident_calls_on(copy.get(0)), 1u);
    EXPECT_EQ(num_make_resident_calls_on(copy.arena().str), 1u);
    EXPECT_EQ(emitrs_yaml<std::string>(copy), emitrs_yaml<std::string>(t));
    s_make_resident_calls.clear();
}

TEST(Tree, global_callbacks_make_resident)
{
    s_make_resident_calls.clear();
    const Callbacks before = get_callbacks();
    set_callbacks(counting_callbacks(before));
    {
        // deep enough for the parser to grow its stack
        std::string src;
        for(int i = 0; i < 100; ++i)
            src += "[";
        for(int i = 0; i < 100; ++i)
            src += "]";
        Tree t = parse_in_arena(to_csubstr(src));
        EXPECT_EQ(num_make_resident_calls_on(t.get(0)), 1u);
        EXPECT_EQ(num_make_resident_calls_on(t.arena().str), 1u);
        EXPECT_GT(s_make_resident_calls.size(), 2u); // the parser stack too
    }
    set_callbacks(before);
    s_make_resident_calls.clear();
}

// https://github.com/biojppm/rapidyaml/issues/288
TEST(Tree, reserve_arena_issue288)
{
    Tree t;
    EXPECT_EQ(t.arena_slack(), 0u);
    EXPECT_EQ(t.arena_capacity(), 0u);
    EXPECT_EQ(t.arena_size(), 0u);
    t.reserve_arena(3u);
    EXPECT_EQ(t.arena_slack(), 3u);
    EXPECT_GE(t.arena_capacity(), 3u);
    EXPECT_EQ(t.arena_size(), 0u);
    // longer than the slack to cause another call to _grow_arena()
    std::string stars(2 * t.arena_slack(), '*');
    t.copy_to_arena(to_csubstr(stars));
    EXPECT_GE(t.arena_capacity(), stars.size());
    EXPECT_EQ(t.arena_size(), stars.size());
    EXPECT_EQ(t.arena(), to_csubstr(stars));
    // again
    std::string pluses(2 * t.arena_slack(), '+');
    t.copy_to_arena(to_csubstr(pluses));
    EXPECT_GE(t.arena_capacity(), stars.size() + pluses.size());
    EXPECT_EQ(t.arena_size(), stars.size() + pluses.size());
    EXPECT_EQ(t.arena().first(stars.size()), to_csubstr(stars));
    EXPECT_EQ(t.arena().last(pluses.size()), to_csubstr(pluses));
}

// https://github.com/biojppm/rapidyaml/issues/564
namespace issue564 {
struct AdjacentSizedBlocks {
    std::vector<char> mem;
    char* start;
    char* next;
};
using AvailableBlocks = std::unordered_map<size_t, AdjacentSizedBlocks>;
static void free(void* , size_t size, void* user_data) {
    (*static_cast<AvailableBlocks *>(user_data))[size].next -= size;
}
static void* alloc(size_t len, void* /*hint*/, void* user_data) {
    auto& blocks = (*static_cast<AvailableBlocks*>(user_data))[len];
    if (blocks.start == nullptr) {
        blocks.mem.resize(10 * len);
        blocks.start = blocks.next = blocks.mem.data();
    }
    void *ret = blocks.next;
    blocks.next += len;
    return ret;
}
} // namespace issue564
TEST(Tree, issue564_relocate_arena_0)
{
    issue564::AvailableBlocks blocks_by_size;
    Callbacks cb{};
    cb.m_user_data = &blocks_by_size;
    cb.m_allocate = issue564::alloc;
    cb.m_free = issue564::free;
    Tree dest(cb);
    Tree source(cb);
    parse_in_arena("[]", &dest);
    parse_in_arena("[]", &source);
    // Shallow copy `source` as a child of `dest`
    NodeRef child = dest.rootref().append_child();
    dest.duplicate_contents(&source, source.root_id(), child.id());
    // trigger a relocation in dest
    std::string loong(10000, ' ');
    ExpectError::check_success([&]{
        dest.to_arena(to_csubstr(loong));  // error
    });
}

id_type free_list_length(Tree const& t)
{
    id_type n = 0;
    for(id_type i = t.m_free_head; i != NONE; i = t.m_buf[i].m_next_sibling)
        ++n;
    return n;
}

TEST(Tree, unused_tail)
{
    Tree t(16);
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(free_list_length(t), 0);
    test_invariants(t);
    NodeRef root = t.rootref();
    root.set_seq();
    for(int i = 0; i < 5; ++i)
        root.append_child() << i;
    EXPECT_EQ(t.size(), 6);
    EXPECT_EQ(free_list_length(t), 0);
    test_invariants(t);
    const id_type second = root[1].id();
    const id_type fourth = root[3].id();
    t.remove(second);
    t.remove(fourth);
    EXPECT_EQ(t.size(), 4);
    EXPECT_EQ(free_list_length(t), 2);
    test_invariants(t);
    EXPECT_EQ(root.append_child().id(), fourth);
    EXPECT_EQ(root.append_child().id(), second);
    EXPECT_EQ(free_list_length(t), 0);
    EXPECT_EQ(root.append_child().id(), 6);
    EXPECT_EQ(t.size(), 7);
    test_invariants(t);
    t.reserve(64);
    EXPECT_EQ(t.capacity(), 64);
    EXPECT_EQ(free_list_length(t), 0);
    test_invariants(t);
    for(int i = 0; t.size() < 40; ++i)
        root.append_child() << i;
    EXPECT_EQ(t.size(), 40);
    EXPECT_EQ(t.last_child(t.root_id()), 39);
    t.remove(5);
    EXPECT_EQ(free_list_length(t), 1);
    test_invariants(t);
    {
        Tree cp(t);
        EXPECT_EQ(cp.capacity(), 64);
        EXPECT_EQ(cp.size(), 39);
        EXPECT_EQ(free_list_length(cp), 1);
        test_invariants(cp);
        EXPECT_EQ(emitrs_yaml<std::string>(cp), emitrs_yaml<std::string>(t));
        Tree mv(std::move(cp));
        EXPECT_EQ(mv.size(), 39);
        EXPECT_EQ(free_list_length(mv), 1);
        test_invariants(mv);
        EXPECT_EQ(mv.append_child(mv.root_id()), 5);
        EXPECT_EQ(mv.append_child(mv.root_id()), 40);
        test_invariants(mv);
    }
    t.clear();
    EXPECT_EQ(t.capacity(), 64);
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(free_list_length(t), 0);
    test_invariants(t);
    parse_in_arena("[a, b, c]", &t);
    EXPECT_EQ(t.size(), 4);
    EXPECT_EQ(t.last_child(t.root_id()), 3);
    EXPECT_EQ(t.capacity(), 64);
    test_invariants(t);
    t.reorder();
    test_invariants(t);
}

TEST(Tree, clear_empty)
{
    Tree t(0, 0);
    EXPECT_EQ(t.capacity(), 0);
    EXPECT_EQ(t.slack(), 0);
    EXPECT_EQ(t.size(), 0);
    EXPECT_EQ(t.arena_capacity(), 0);
    EXPECT_EQ(t.arena_slack(), 0);
    EXPECT_EQ(t.arena_size(), 0);
    test_invariants(t);

    t.clear();
    EXPECT_EQ(t.capacity(), 0);
    EXPECT_EQ(t.slack(), 0);
    EXPECT_EQ(t.size(), 0);
    EXPECT_EQ(t.arena_capacity(), 0);
    EXPECT_EQ(t.arena_slack(), 0);
    EXPECT_EQ(t.arena_size(), 0);
    test_invariants(t);
}

TEST(Tree, clear)
{
    Tree t(16, 64);
    EXPECT_EQ(t.capacity(), 16);
    EXPECT_EQ(t.slack(), 15);
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t.arena_capacity(), 64);
    EXPECT_EQ(t.arena_slack(), 64);
    EXPECT_EQ(t.arena_size(), 0);
    test_invariants(t);

    t.clear();
    t.clear_arena();
    EXPECT_EQ(t.capacity(), 16);
    EXPECT_EQ(t.slack(), 15);
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t.arena_capacity(), 64);
    EXPECT_EQ(t.arena_slack(), 64);
    EXPECT_EQ(t.arena_size(), 0);
    test_invariants(t);

    NodeData *buf = t.m_buf;
    t.reserve(16);
    t.reserve_arena(64);
    EXPECT_EQ(t.m_buf, buf);
    EXPECT_EQ(t.capacity(), 16);
    EXPECT_EQ(t.slack(), 15);
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t.arena_capacity(), 64);
    EXPECT_EQ(t.arena_slack(), 64);
    EXPECT_EQ(t.arena_size(), 0);
    test_invariants(t);

    t.reserve(32);
    t.reserve_arena(128);
    EXPECT_EQ(t.capacity(), 32);
    EXPECT_EQ(t.slack(), 31);
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t.arena_capacity(), 128);
    EXPECT_EQ(t.arena_slack(), 128);
    EXPECT_EQ(t.arena_size(), 0);
    test_invariants(t);

    buf = t.m_buf;
    parse_in_arena("[a, b, c, d, e, f]", &t);
    EXPECT_EQ(t.m_buf, buf);
    EXPECT_EQ(t.capacity(), 32);
    EXPECT_EQ(t.slack(), 25);
    EXPECT_EQ(t.size(), 7);
    EXPECT_EQ(t.arena_capacity(), 128);
    EXPECT_EQ(t.arena_slack(), 110);
    EXPECT_EQ(t.arena_size(), 18);
    test_invariants(t);

    t.clear();
    t.clear_arena();
    EXPECT_EQ(t.capacity(), 32);
    EXPECT_EQ(t.slack(), 31);
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t.arena_capacity(), 128);
    EXPECT_EQ(t.arena_slack(), 128);
    EXPECT_EQ(t.arena_size(), 0);
    test_invariants(t);
}

TEST(Tree, relocate)
{
    // create a tree with anchors and refs, and copy it to ensure the
    // relocation also applies to the anchors and refs. Ensure to put
    // the source in the arena so that it gets relocated.
    Tree tree = parse_in_arena(R"(&keyanchor key: val
key2: &valanchor val2
keyref: *keyanchor
*valanchor : was val anchor
!!int 0: !!str foo
!!str doe: !!str a deer a female deer
ray: a drop of golden sun
me: a name I call myself
far: a long long way to run
)");
    Tree copy = tree;
    EXPECT_EQ(copy.size(), tree.size());
    EXPECT_EQ(emitrs_yaml<std::string>(copy), R"(&keyanchor key: val
key2: &valanchor val2
keyref: *keyanchor
*valanchor : was val anchor
!!int 0: !!str foo
!!str doe: !!str a deer a female deer
ray: a drop of golden sun
me: a name I call myself
far: a long long way to run
)");
    //
    Tree copy2 = copy;
    EXPECT_EQ(copy.size(), tree.size());
    copy2.resolve();
    EXPECT_EQ(emitrs_yaml<std::string>(copy2), R"(key: val
key2: val2
keyref: key
val2: was val anchor
!!int 0: !!str foo
!!str doe: !!str a deer a female deer
ray: a drop of golden sun
me: a name I call myself
far: a long long way to run
)");
}

} // namespace yml
} // namespace c4
