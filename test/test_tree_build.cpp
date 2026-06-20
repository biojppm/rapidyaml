#include "./test_lib/test_case.hpp"
#include "gtest/gtest-spi.h"

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {


#ifndef RYML_DBG
#define _RYML_WITH_DBG(...)
#define _RYML_WITHOUT_DBG(...) __VA_ARGS__
#define BUILDTRACE(...) RYML_TRACE_FMT(__VA_ARGS__, 0)
#else
#define _RYML_WITH_DBG(...) __VA_ARGS__
#define _RYML_WITHOUT_DBG(...)
#define BUILDTRACE(...)                         \
    RYML_TRACE_FMT(__VA_ARGS__, 0);             \
    _c4dbgpf(__VA_ARGS__, 0)
#endif

// https://stackoverflow.com/questions/24037390/how-can-i-expect-multiple-failures-in-google-test
#define EXPECT_NONFATAL_FAILURE_ANY_NUMBER(statement)                 \
  do {                                                                \
    ::testing::TestPartResultArray gtest_failures;                    \
    {                                                                 \
      ::testing::ScopedFakeTestPartResultReporter gtest_reporter(     \
          ::testing::ScopedFakeTestPartResultReporter::               \
              INTERCEPT_ONLY_CURRENT_THREAD,                          \
          &gtest_failures);                                           \
      statement;                                                      \
      {                                                               \
          SCOPED_TRACE("check at least one failure");                 \
          ASSERT_NE(gtest_failures.size(), 0);                        \
      }                                                               \
    }                                                                 \
  } while (0)


namespace {
using SetupFn = std::function<NodeRef(Tree&)>;
using NodeFn = std::function<void(NodeRef)>;
using TreeFn = std::function<void(Tree *,id_type)>;

void test_assertion_(SetupFn const& setup, NodeFn const& cause_assertion, id_type err_node_id=npos)
{
    Tree tree;
    NodeRef subject = setup(tree);
    if(err_node_id == npos)
        err_node_id = subject.id();
    ExpectError::check_assert_visit(&tree, [&]{ cause_assertion(subject); }, err_node_id);
    #if ! RYML_USE_ASSERT
    {
        SCOPED_TRACE("here");
        EXPECT_NONFATAL_FAILURE_ANY_NUMBER(test_invariants(tree));
    }
    #endif
}
void test_assertion_(SetupFn const& setup, TreeFn const& cause_assertion, id_type err_node_id=npos)
{
    Tree tree;
    NodeRef subject = setup(tree);
    if(err_node_id == npos)
        err_node_id = subject.id();
    ExpectError::check_assert_visit(&tree, [&]{ cause_assertion(subject.tree(), subject.id()); }, err_node_id);
    #if ! RYML_USE_ASSERT
    {
        SCOPED_TRACE("here");
        EXPECT_NONFATAL_FAILURE_ANY_NUMBER(test_invariants(tree));
    }
    #endif
}
void test_success_(SetupFn const& setup, NodeFn const& action)
{
    Tree tree;
    NodeRef subject = setup(tree);
    ExpectError::check_success(&tree, [&]{ action(subject); });
}
void test_success_(SetupFn const& setup, TreeFn const& action)
{
    Tree tree;
    NodeRef subject = setup(tree);
    ExpectError::check_success(&tree, [&]{ action(subject.tree(), subject.id()); });
}
#define test_assertion(...)                     \
    {                                           \
        SCOPED_TRACE("here");                   \
        test_assertion_(__VA_ARGS__);           \
    }
#define test_success(...)                       \
    {                                           \
        SCOPED_TRACE("here");                   \
        test_success_(__VA_ARGS__);             \
    }

NodeRef mkmap(NodeRef n) { n.set_map(); return n; }
NodeRef mkseq(NodeRef n) { n.set_seq(); return n; }
NodeRef mkval(NodeRef n) { n.set_val("scalar"); return n; }
NodeRef mkrootmap(Tree &tree) { return mkmap(tree.rootref()); }
NodeRef mkrootseq(Tree &tree) { return mkseq(tree.rootref()); }
NodeRef mkrootval(Tree &tree) { return mkval(tree.rootref()); }

} // namespace

TEST(build_errors, rootval_then_stream)
{
    test_assertion(mkrootval, [](Tree *t, id_type id){ t->set_stream(id); });
    test_assertion(mkrootval, [](NodeRef n){ n.set_stream(); });
}
TEST(build_errors, rootval_then_doc)
{
    test_success(mkrootval, [](Tree *t, id_type id){ t->set_doc(id); });
    test_success(mkrootval, [](NodeRef n){ n.set_doc(); });
}
TEST(build_errors, rootval_then_seq)
{
    test_assertion(mkrootval, [](Tree *t, id_type id){ t->set_seq(id); });
    test_assertion(mkrootval, [](NodeRef n){ n.set_seq(); });
}
TEST(build_errors, rootval_then_map)
{
    test_assertion(mkrootval, [](Tree *t, id_type id){ t->set_map(id); });
    test_assertion(mkrootval, [](NodeRef n){ n.set_map(); });
}
TEST(build_errors, rootval_then_val)
{
    test_success(mkrootval, [](Tree *t, id_type id){ t->set_val(id, "a"); });
    test_success(mkrootval, [](NodeRef n){ n.set_val("a"); });
}

TEST(build_errors, rootseq_then_stream)
{
    test_success(mkrootseq, [](NodeRef n){ n.tree()->set_stream(n.id()); });
    test_success(mkrootseq, [](NodeRef n){ n.set_stream(); });
}
TEST(build_errors, rootseq_then_doc)
{
    test_success(mkrootseq, [](NodeRef n){ n.tree()->set_doc(n.id()); });
    test_success(mkrootseq, [](NodeRef n){ n.set_doc(); });
}
TEST(build_errors, rootseq_then_val)
{
    test_assertion(mkrootseq, [](NodeRef n){ n.tree()->set_val(n.id(), "a"); });
    test_assertion(mkrootseq, [](NodeRef n){ n.set_val("a"); });
}

TEST(build_errors, rootmap_then_val)
{
    test_assertion(mkrootmap, [](NodeRef n){ n.tree()->set_val(n.id(), "a"); });
    test_assertion(mkrootmap, [](NodeRef n){ n.set_val("a"); });
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// exhaustive combinatorial tests:


struct BuilderVariant
{
    const char* name;
    NodeFn buildfn; ///< a function that modifies a node
    NodeFn testfn;  ///< a function that tests the node (may be null)
    int line;
};

/// A collection of variants which perform the same task in different ways
struct BuilderVariants
{
    const char* name;
    NodeFn testfn;
    std::vector<BuilderVariant> variants;
};

/// What to do after a block's variant has modified a node to prepare
/// the next block
typedef enum { // NOLINT
    nothing_else,  ///< finish
    same_node,     ///< stay on the same subject node
    first_child,
    last_child,
    all_children,
} Subject_e;

/// A building block, containing the variants together with iteration
/// state
struct BuilderBlock
{
    BuilderBlock(BuilderVariants &&action_, Subject_e then_, bool check_invariants=true)
        : action(std::move(action_))
        , check_invariants_at_end(check_invariants)
        , then(then_)
        , variant_index()
    {
    }
    BuilderVariants action;
    bool check_invariants_at_end;
    Subject_e then;
    size_t variant_index;
    void reset() { variant_index = 0; }
    bool done() const { return (variant_index >= action.variants.size()); }
    bool next() { ++variant_index; return !done(); }
};

// reuse the objects to reduce allocations
struct BuilderWorkspace
{
    Tree tree, parsed;
    std::string emitted_yaml;
    EventHandlerTree handler = {};
    Parser parser{&handler};
    void reset()
    {
        tree.reserve(128);
        tree.reserve_arena(512);
        tree.clear();
        parsed.clear();
        emitted_yaml.clear();
    }
};

void run_case(cspan<BuilderBlock> blocks, BuilderWorkspace *ws)
{
    ws->reset();
    id_type id = ws->tree.root_id();
    EXPECT_EQ(ws->tree.type(id), NodeType{});
    bool do_all_children = false;
    for(size_t iblock = 0; iblock < blocks.size(); ++iblock)
    {
        BuilderBlock const& b = blocks[iblock];
        BuilderVariants const& variants = b.action;
        ASSERT_LT(b.variant_index, variants.variants.size());
        BuilderVariant const& variant = variants.variants[b.variant_index];
        _c4dbgpf("block[{}/{}]: ({}) applying variant[{}/{}]: ...", iblock+1, blocks.size(), b.action.name, b.variant_index+1, variants.variants.size());
        auto doit = [&](id_type subject_id){
            RYML_TRACE_FMT("here:\n{}:{}: id={}: {}[{}]", __FILE__, variant.line, subject_id, b.variant_index+1, variants.variants.size(), variants.name, variant.name);
            _RYML_WITH_DBG(_dbg_printf("{}:{}: id={}: block[{}/{}] variant[{}/{}] {}[{}]\n", __FILE__, variant.line, subject_id, iblock+1, blocks.size(), b.variant_index+1, variants.variants.size(), variants.name, variant.name));
            //printf("%s:%d: id=%zu: block[%zu/%zu] variant[%zu/%zu] %s[%s]\n", __FILE__, variant.line, subject_id, iblock+1, blocks.size(), b.variant_index+1, variants.variants.size(), variants.name.str, variant.name); // NOLINT
            NodeRef n(&ws->tree, subject_id);
            variant.buildfn(n);
            _RYML_WITH_DBG(print_tree(ws->tree));
            if(variant.testfn)
                variant.testfn(n);
            if(b.action.testfn)
                b.action.testfn(n);
        };
        if(!do_all_children)
        {
            doit(id);
        }
        else
        {
            BUILDTRACE("id={} all children!", id);
            for(id_type child = ws->tree.first_child(id); child != NONE; child = ws->tree.next_sibling(child))
                doit(child);
        }
        if(b.check_invariants_at_end)
        {
            SCOPED_TRACE("tree invariants");
            test_invariants(ws->tree);
        }
        if(testing::Test::HasFailure())
            break;
        if(do_all_children)
            id = ws->tree.first_child(id);
        do_all_children = false;
        switch(b.then)
        {
        case same_node:
            break;
        case all_children:
            do_all_children = true;
            break;
        case first_child:
            id = ws->tree.first_child(id);
            break;
        case last_child:
            ws->tree = ws->tree.last_child(id);
            break;
        case nothing_else:
        default:
            break;
        }
    }
    _RYML_WITHOUT_DBG(if(testing::Test::HasFailure()))
    {
        print_tree(ws->tree);
        _RYML_WITHOUT_DBG(return);
    }
    // now do a roundtrip
    emitrs_yaml(ws->tree, &ws->emitted_yaml);
    parse_in_place(&ws->parser, to_substr(ws->emitted_yaml), &ws->parsed);
    {
        BUILDTRACE("compare trees");
        test_compare(ws->parsed, ws->tree, "after_roundtrip", "tree");
    }
    _RYML_WITHOUT_DBG(if(testing::Test::HasFailure()))
    {
        print_tree("tree", ws->tree);
        printf("emitted_yaml: [%zu]~~~\n%s~~~\n", ws->emitted_yaml.size(), ws->emitted_yaml.c_str());
        print_tree("after_roundtrip", ws->parsed);
    }
}


void apply(span<BuilderBlock> blocks)
{
    static size_t total_cases_all_tests = 0;
    if(blocks.empty())
        return;
    size_t num_cases = 1; (void)num_cases;
    for(BuilderBlock &b : blocks)
    {
        b.reset();
        num_cases *= b.action.variants.size();
    }
    BuilderBlock * first = &blocks.front();
    BuilderBlock * last = &blocks.back();
    auto id = [&](BuilderBlock *b){ return 1 + b - first; }; (void)id;
    size_t curr_case = 0; (void)curr_case;
    BuilderWorkspace ws{};
    while(true)
    {
        ++curr_case;
        _c4dbgpf("\n--------------------------------\n"
                 "running case {}/{}", curr_case, num_cases);
        run_case(blocks, &ws);
        if(testing::Test::HasFailure())
            return;
        _c4dbgpf("finished case {}/{}", curr_case, num_cases);
        if(!last->next())
        {
            _c4dbgpf("block[{}/{}] ({}) finished {} jobs!", id(last), blocks.size(), last->action.name, last->action.variants.size());
            last->reset();
            if(last == first)
                goto finish; // NOLINT
            for(BuilderBlock *b = last - 1; b >= first; --b)
            {
                if(b->next())
                {
                    _c4dbgpf("block[{}/{}] ({}) move to job {}/{}!", id(b), blocks.size(), b->action.name, b->variant_index+1, b->action.variants.size());
                    break;
                }
                else
                {
                    _c4dbgpf("block[{}/{}] ({}) finished {} jobs!", id(b), blocks.size(), b->action.name, b->action.variants.size());
                    if(b == first)
                    {
                        _c4dbgpf("block[{}/{}] is the first. DONE!", id(b), blocks.size());
                        goto finish; // NOLINT
                    }
                    b->reset();
                }
            }
        }
    }
    ASSERT_EQ(curr_case, num_cases);
finish:
    total_cases_all_tests += num_cases;
    printf("ran a total of %zu cases. so far: %zu\n", num_cases, total_cases_all_tests);
}


//-----------------------------------------------------------------------------

#define mkb(name, fn) BuilderVariant{name, fn, {}, __LINE__}
#define mkb2(name, fn, check) BuilderVariant{name, fn, check, __LINE__}

BuilderVariants set_to_stream()
{
    return BuilderVariants{"set_to_stream",
        #define _(node) [](NodeRef node)
        _(n){
            BUILDTRACE("check seq");
            EXPECT_TRUE(n.is_stream());
            EXPECT_TRUE(n.tree()->is_stream(n.id()));
        }, {
        mkb("to stream tree", _(n){
            n.tree()->set_stream(n.id());
        }),
        mkb("to stream node", _(n){
            n.set_stream();
        }),
        #undef _
    }};
}

BuilderVariants set_scalar_val(csubstr scalar, NodeType extra={})
{
    return BuilderVariants{"on_val_scalar",
        #define _(node) [scalar, extra](NodeRef node)
        _(n){
            BUILDTRACE("check scalar val");
            EXPECT_TRUE(n.type().has_val());
            EXPECT_EQ(n.val(), scalar);
            EXPECT_EQ(n.type() & extra, extra);
        }, {
        mkb("tree set", _(n){
            if(extra)
                n.tree()->set_val(n.id(), scalar, extra);
            else
                n.tree()->set_val(n.id(), scalar);
        }),
        mkb("node set", _(n){
            if(extra)
                n.set_val(scalar, extra);
            else
                n.set_val(scalar);
        }),
        mkb("node assign", _(n){
            n.set_val(scalar, extra);
        }),
        #undef _
    }};
}
BuilderVariants set_scalar_val_all(csubstr scalar, NodeType extra={})
{
    return BuilderVariants{"on_val_scalar",
        #define _(node) [scalar, extra](NodeRef node)
        _(n){
            BUILDTRACE("check scalar val");
            EXPECT_TRUE(n.has_val());
            EXPECT_TRUE(n.tree()->has_val(n.id()));
            EXPECT_EQ(n.type() & extra, extra);
            EXPECT_EQ(n.val(), scalar);
        }, {
        mkb("tree set 1", _(n){
            n.tree()->set_val(n.id(), scalar, extra);
        }),
        mkb("tree set 2", _(n){
            n.tree()->set_val(n.id(), scalar);
            n.tree()->_add_flags(n.id(), extra);
        }),
        mkb("tree set 3", _(n){
            n.tree()->_add_flags(n.id(), extra);
            n.tree()->set_val(n.id(), scalar);
        }),
        mkb("tree serialized 1", _(n){
            n.tree()->set_serialized(n.id(), scalar, extra);
        }),
        mkb("tree serialized 2", _(n){
            n.tree()->_add_flags(n.id(), extra);
            n.tree()->set_serialized(n.id(), scalar);
        }),
        mkb("tree serialized 3", _(n){
            n.tree()->set_serialized(n.id(), scalar);
            n.tree()->_add_flags(n.id(), extra);
        }),
        mkb("tree save 1", _(n){
            n.tree()->save(n.id(), scalar, extra);
        }),
        mkb("tree save 2", _(n){
            n.tree()->_add_flags(n.id(), extra);
            n.tree()->save(n.id(), scalar);
        }),
        mkb("tree save 3", _(n){
            n.tree()->save(n.id(), scalar);
            n.tree()->_add_flags(n.id(), extra);
        }),
        mkb("node set 1.1", _(n){
            n.set_val(scalar, extra);
        }),
        mkb("node set 1.2", _(n){
            n.set_val(scalar);
            n._add_flags(extra);
        }),
        mkb("node set 2.2", _(n){
            n._add_flags(extra);
            n.set_val(scalar);
        }),
        mkb("node serialized 2.1", _(n){
            n.set_serialized(scalar);
            n._add_flags(extra);
        }),
        mkb("node serialized 2.2", _(n){
            n._add_flags(extra);
            n.set_serialized(scalar);
        }),
        mkb("node serialized 2.3", _(n){
            n.set_serialized(scalar, extra);
        }),
        mkb("node save 1.1", _(n){
            n.save(scalar);
            n._add_flags(extra);
        }),
        mkb("node save 1.2", _(n){
            n._add_flags(extra);
            n.save(scalar);
        }),
        mkb("node save 1.3", _(n){
            n.save(scalar, extra);
        }),
        #undef _
    }};
}

BuilderVariants set_scalar_keyval(csubstr key, csubstr val, NodeType extra={})
{
    return BuilderVariants{"on_key_scalar",
        #define _(node) [key, val, extra](NodeRef node)
        _(n){
            BUILDTRACE("check keyval");
            EXPECT_TRUE(n.has_key());
            EXPECT_TRUE(n.has_val());
            EXPECT_TRUE(n.tree()->has_key(n.id()));
            EXPECT_TRUE(n.tree()->has_val(n.id()));
            EXPECT_EQ(n.key(), key);
            EXPECT_EQ(n.val(), val);
            EXPECT_EQ(n.type() & extra, extra);
        }, {
        mkb("tree set", _(n){
            if(extra)
            {
                n.tree()->set_key(n.id(), key, extra);
                n.tree()->set_key(n.id(), val, extra);
            }
            else
            {
                n.tree()->set_key(n.id(), key);
                n.tree()->set_val(n.id(), val);
            }
        }),
        mkb("node set", _(n){
            n.set_key(key);
            n.set_val(val);
            if(extra)
                n._add_flags(extra);
        }),
        #undef _
    }};
}
BuilderVariants set_scalar_keyval_all(csubstr key, csubstr val, NodeType extra={})
{
    return BuilderVariants{"on_key_scalar",
        #define _(node) [key, val, extra](NodeRef node)
        _(n){
            BUILDTRACE("check keyval");
            EXPECT_TRUE(n.has_key());
            EXPECT_TRUE(n.has_val());
            EXPECT_TRUE(n.tree()->has_key(n.id()));
            EXPECT_TRUE(n.tree()->has_val(n.id()));
            EXPECT_EQ(n.key(), key);
            EXPECT_EQ(n.val(), val);
            EXPECT_EQ(n.type() & extra, extra);
        }, {
        mkb("tree set 1", _(n){
            n.tree()->set_key(n.id(), key, extra);
            n.tree()->set_val(n.id(), val, extra);
        }),
        mkb("tree set 2", _(n){
            n.tree()->set_key(n.id(), key);
            n.tree()->set_val(n.id(), val);
            n.tree()->_add_flags(n.id(), extra);
        }),
        mkb("tree set 3", _(n){
            n.tree()->_add_flags(n.id(), extra);
            n.tree()->set_key(n.id(), key);
            n.tree()->set_val(n.id(), val);
        }),
        mkb("tree serialized 1", _(n){
            n.tree()->set_key_serialized(n.id(), key, extra);
            n.tree()->set_serialized(n.id(), val, extra);
        }),
        mkb("tree serialized 2", _(n){
            n.tree()->_add_flags(n.id(), extra);
            n.tree()->set_key_serialized(n.id(), key);
            n.tree()->set_serialized(n.id(), val);
        }),
        mkb("tree serialized 3", _(n){
            n.tree()->set_key_serialized(n.id(), key);
            n.tree()->set_serialized(n.id(), val);
            n.tree()->_add_flags(n.id(), extra);
        }),
        mkb("tree save 1", _(n){
            n.tree()->save_key(n.id(), key, extra);
            n.tree()->save(n.id(), val, extra);
        }),
        mkb("tree save 2", _(n){
            n.tree()->_add_flags(n.id(), extra);
            n.tree()->save_key(n.id(), key);
            n.tree()->save(n.id(), val);
        }),
        mkb("tree serialized 3", _(n){
            n.tree()->save_key(n.id(), key);
            n.tree()->save(n.id(), val);
            n.tree()->_add_flags(n.id(), extra);
        }),
        mkb("node set 2.1", _(n){
            n.set_key(key);
            n.set_val(val);
            n._add_flags(extra);
        }),
        mkb("node set 2.2", _(n){
            n._add_flags(extra);
            n.set_key(key);
            n.set_val(val);
        }),
        mkb("node serialized 1.1", _(n){
            n.save_key(key);
            n.save(val);
            n._add_flags(extra);
        }),
        mkb("node serialized 1.2", _(n){
            n._add_flags(extra);
            n.save_key(key);
            n.save(val);
        }),
        mkb("node serialized 1.3", _(n){
            n.save_key(key);
            n.save(val, extra);
        }),
        mkb("node serialized 1.4", _(n){
            n.save_key(key, extra);
            n.save(val);
        }),
        mkb("node serialized 2.1", _(n){
            n.set_key_serialized(key);
            n.set_serialized(val);
            n._add_flags(extra);
        }),
        mkb("node serialized 2.2", _(n){
            n._add_flags(extra);
            n.set_key_serialized(key);
            n.set_serialized(val);
        }),
        #undef _
    }};
}

BuilderVariants set_val_style()
{
    return BuilderVariants{"set_val_style",
        #define _(node) [](NodeRef node)
        _(n){
            (void)n; // do nothing
        }, {
        mkb2("squo, tree", _(n){
            n.tree()->set_val_style(n.id(), VAL_SQUO);
        }, _(n){
            EXPECT_EQ(n.val_style(), VAL_SQUO);
        }),
        mkb2("squo, node", _(n){
            n.set_val_style(VAL_SQUO);
        }, _(n){
            EXPECT_EQ(n.val_style(), VAL_SQUO);
        }),
        #undef _
    }};
}
BuilderVariants set_val_style_all()
{
    return BuilderVariants{"set_val_style",
        #define _(node) [](NodeRef node)
        _(n){
            (void)n; // do nothing
        }, {
        mkb2("no style", _(n){
            (void)n; // do nothing
        }, _(n){
            EXPECT_EQ(n.val_style(), NodeType{});
        }),
        mkb2("plain, tree", _(n){
            n.tree()->set_val_style(n.id(), VAL_PLAIN);
        }, _(n){
            EXPECT_EQ(n.val_style(), VAL_PLAIN);
        }),
        mkb2("plain, node", _(n){
            n.set_val_style(VAL_PLAIN);
        }, _(n){
            EXPECT_EQ(n.val_style(), VAL_PLAIN);
        }),
        mkb2("squo, tree", _(n){
            n.tree()->set_val_style(n.id(), VAL_SQUO);
        }, _(n){
            EXPECT_EQ(n.val_style(), VAL_SQUO);
        }),
        mkb2("squo, node", _(n){
            n.set_val_style(VAL_SQUO);
        }, _(n){
            EXPECT_EQ(n.val_style(), VAL_SQUO);
        }),
        mkb2("dquo, tree", _(n){
            n.tree()->set_val_style(n.id(), VAL_DQUO);
        }, _(n){
            EXPECT_EQ(n.val_style(), VAL_DQUO);
        }),
        mkb2("dquo, node", _(n){
            n.set_val_style(VAL_DQUO);
        }, _(n){
            EXPECT_EQ(n.val_style(), VAL_DQUO);
        }),
        mkb2("literal, tree", _(n){
            n.tree()->set_val_style(n.id(), VAL_LITERAL);
        }, _(n){
            EXPECT_EQ(n.val_style(), VAL_LITERAL);
        }),
        mkb2("literal, node", _(n){
            n.set_val_style(VAL_LITERAL);
        }, _(n){
            EXPECT_EQ(n.val_style(), VAL_LITERAL);
        }),
        mkb2("folded, tree", _(n){
            n.tree()->set_val_style(n.id(), VAL_FOLDED);
        }, _(n){
            EXPECT_EQ(n.val_style(), VAL_FOLDED);
        }),
        mkb2("folded, node", _(n){
            n.set_val_style(VAL_FOLDED);
        }, _(n){
            EXPECT_EQ(n.val_style(), VAL_FOLDED);
        }),
        #undef _
    }};
}

BuilderVariants set_keyval_style()
{
    return BuilderVariants{"set_key_style",
        #define _(node) [](NodeRef node)
        _(n){
            (void)n; // do nothing
        }, {
        mkb2("squo, tree", _(n){
            n.tree()->set_key_style(n.id(), KEY_SQUO);
            n.tree()->set_val_style(n.id(), VAL_SQUO);
        }, _(n){
            EXPECT_EQ(n.key_style(), KEY_SQUO);
            EXPECT_EQ(n.val_style(), VAL_SQUO);
        }),
        mkb2("squo, node", _(n){
            n.set_key_style(KEY_FOLDED);
            n.set_val_style(VAL_FOLDED);
        }, _(n){
            EXPECT_EQ(n.key_style(), KEY_SQUO);
            EXPECT_EQ(n.val_style(), VAL_SQUO);
        }),
        #undef _
    }};
}
BuilderVariants set_keyval_style_all()
{
    return BuilderVariants{"set_key_style",
        #define _(node) [](NodeRef node)
        _(n){
            (void)n; // do nothing
        }, {
        mkb2("no style", _(n){
            (void)n; // do nothing
        }, _(n){
            EXPECT_EQ(n.key_style(), NodeType{});
            EXPECT_EQ(n.val_style(), NodeType{});
        }),
        mkb2("plain, tree", _(n){
            n.tree()->set_key_style(n.id(), KEY_PLAIN);
            n.tree()->set_val_style(n.id(), VAL_PLAIN);
        }, _(n){
            EXPECT_EQ(n.key_style(), KEY_PLAIN);
            EXPECT_EQ(n.key_style(), VAL_PLAIN);
        }),
        mkb2("plain, node", _(n){
            n.set_key_style(KEY_PLAIN);
            n.set_val_style(VAL_PLAIN);
        }, _(n){
            EXPECT_EQ(n.key_style(), KEY_PLAIN);
            EXPECT_EQ(n.val_style(), VAL_PLAIN);
        }),
        mkb2("squo, tree", _(n){
            n.tree()->set_key_style(n.id(), KEY_SQUO);
            n.tree()->set_val_style(n.id(), VAL_SQUO);
        }, _(n){
            EXPECT_EQ(n.key_style(), KEY_SQUO);
            EXPECT_EQ(n.val_style(), VAL_SQUO);
        }),
        mkb2("squo, node", _(n){
            n.set_key_style(KEY_SQUO);
            n.set_val_style(VAL_SQUO);
        }, _(n){
            EXPECT_EQ(n.key_style(), KEY_SQUO);
            EXPECT_EQ(n.val_style(), VAL_SQUO);
        }),
        mkb2("dquo, tree", _(n){
            n.tree()->set_key_style(n.id(), KEY_DQUO);
            n.tree()->set_val_style(n.id(), VAL_DQUO);
        }, _(n){
            EXPECT_EQ(n.key_style(), KEY_DQUO);
            EXPECT_EQ(n.val_style(), VAL_DQUO);
        }),
        mkb2("dquo, node", _(n){
            n.set_key_style(KEY_DQUO);
            n.set_val_style(VAL_DQUO);
        }, _(n){
            EXPECT_EQ(n.key_style(), KEY_DQUO);
            EXPECT_EQ(n.val_style(), VAL_DQUO);
        }),
        mkb2("literal, tree", _(n){
            n.tree()->set_key_style(n.id(), KEY_LITERAL);
            n.tree()->set_val_style(n.id(), VAL_LITERAL);
        }, _(n){
            EXPECT_EQ(n.key_style(), KEY_LITERAL);
            EXPECT_EQ(n.val_style(), VAL_LITERAL);
        }),
        mkb2("literal, node", _(n){
            n.set_key_style(KEY_LITERAL);
            n.set_val_style(VAL_LITERAL);
        }, _(n){
            EXPECT_EQ(n.key_style(), KEY_LITERAL);
            EXPECT_EQ(n.val_style(), VAL_LITERAL);
        }),
        mkb2("folded, tree", _(n){
            n.tree()->set_key_style(n.id(), KEY_FOLDED);
            n.tree()->set_val_style(n.id(), VAL_FOLDED);
        }, _(n){
            EXPECT_EQ(n.key_style(), KEY_FOLDED);
            EXPECT_EQ(n.val_style(), VAL_FOLDED);
        }),
        mkb2("folded, node", _(n){
            n.set_key_style(KEY_FOLDED);
            n.set_val_style(VAL_FOLDED);
        }, _(n){
            EXPECT_EQ(n.key_style(), KEY_FOLDED);
            EXPECT_EQ(n.val_style(), VAL_FOLDED);
        }),
        #undef _
    }};
}

BuilderVariants set_to_seq(NodeType extra={})
{
    return BuilderVariants{"set_to_seq",
        #define _(node) [extra](NodeRef node)
        _(n){
            SCOPED_TRACE("check seq");
            EXPECT_TRUE(n.is_seq());
            EXPECT_TRUE(n.tree()->is_seq(n.id()));
            EXPECT_EQ(n.type() & extra, extra);
        }, {
        mkb("tree set A", _(n){
            n.tree()->set_seq(n.id(), extra);
        }),
        mkb("tree set B", _(n){
            n.tree()->set_seq(n.id());
            n.tree()->_add_flags(n.id(), extra);
        }),
        mkb("node set A", _(n){
            n.set_seq();
            n._add_flags(extra);
        }),
        mkb("node set B", _(n){
            n._add_flags(extra);
            n.set_seq();
        }),
        #undef _
    }};
}

BuilderVariants set_to_map(NodeType extra={})
{
    return BuilderVariants{"set_to_map",
        #define _(node) [extra](NodeRef node)
        _(n){
            SCOPED_TRACE("check map");
            EXPECT_TRUE(n.is_map());
            EXPECT_TRUE(n.tree()->is_map(n.id()));
            EXPECT_EQ(n.type() & extra, extra);
        }, {
        mkb("tree set A", _(n){
            n.tree()->set_map(n.id(), extra);
        }),
        mkb("tree set B", _(n){
            n.tree()->set_map(n.id());
            n.tree()->_add_flags(n.id(), extra);
        }),
        mkb("node set A", _(n){
            n.set_map();
            n._add_flags(extra);
        }),
        mkb("node set B", _(n){
            n._add_flags(extra);
            n.set_map();
        }),
        #undef _
    }};
}

BuilderVariants add_to_seq(NodeType extra={})
{
    return BuilderVariants{"add_to_seq",
        #define _(node) [extra](NodeRef node)
        _(n){
            BUILDTRACE("check seq");
            EXPECT_TRUE(n.is_seq());
            EXPECT_TRUE(n.tree()->is_seq(n.id()));
            for(ConstNodeRef child : n.children())
                EXPECT_EQ(child.type() & extra, extra);
        }, {
        mkb("single child, tree", _(n){
            id_type id = n.tree()->append_child(n.id());
            n.tree()->_add_flags(id, extra);
        }),
        mkb("two children, tree", _(n){
            id_type id0 = n.tree()->append_child(n.id());
            id_type id1 = n.tree()->append_child(n.id());
            n.tree()->_add_flags(id0, extra);
            n.tree()->_add_flags(id1, extra);
        }),
        mkb("three children, tree", _(n){
            id_type id0 = n.tree()->append_child(n.id());
            id_type id1 = n.tree()->append_child(n.id());
            id_type id2 = n.tree()->append_child(n.id());
            n.tree()->_add_flags(id0, extra);
            n.tree()->_add_flags(id1, extra);
            n.tree()->_add_flags(id2, extra);
        }),
        mkb("single child, node", _(n){
            NodeRef child0 = n.append_child();
            child0._add_flags(extra);
        }),
        mkb("two children, node", _(n){
            NodeRef child0 = n.append_child();
            NodeRef child1 = n.append_child();
            child0._add_flags(extra);
            child1._add_flags(extra);
        }),
        mkb("three children, node", _(n){
            NodeRef child0 = n.append_child();
            NodeRef child1 = n.append_child();
            NodeRef child2 = n.append_child();
            child0._add_flags(extra);
            child1._add_flags(extra);
            child2._add_flags(extra);
        }),
        #undef _
    }};
}

BuilderVariants add_to_map(NodeType extra={})
{
    return BuilderVariants{"add_to_map",
        #define _(node) [extra](NodeRef node)
        _(n){
            BUILDTRACE("check map");
            EXPECT_TRUE(n.is_map());
            EXPECT_TRUE(n.tree()->is_map(n.id()));
            for(ConstNodeRef child : n.children())
                EXPECT_EQ(child.type() & extra, extra);
        }, {
        mkb("single child, tree", _(n){
            id_type id0 = n.tree()->append_child(n.id());
            n.tree()->set_key(id0, "key0", extra);
        }),
        mkb("two children, tree", _(n){
            id_type id0 = n.tree()->append_child(n.id());
            id_type id1 = n.tree()->append_child(n.id());
            n.tree()->set_key(id0, "key0", extra);
            n.tree()->set_key(id1, "key1", extra);
        }),
        mkb("three children, tree", _(n){
            id_type id0 = n.tree()->append_child(n.id());
            id_type id1 = n.tree()->append_child(n.id());
            id_type id2 = n.tree()->append_child(n.id());
            n.tree()->set_key(id0, "key0", extra);
            n.tree()->set_key(id1, "key1", extra);
            n.tree()->set_key(id2, "key2", extra);
        }),
        mkb("single child, node", _(n){
            NodeRef child0 = n.append_child();
            child0.set_key("key0");
            child0._add_flags(extra);
        }),
        mkb("two children, node", _(n){
            NodeRef child0 = n.append_child();
            NodeRef child1 = n.append_child();
            child0.set_key("key0");
            child1.set_key("key1");
            child0._add_flags(extra);
            child1._add_flags(extra);
        }),
        mkb("three children, node", _(n){
            NodeRef child0 = n.append_child();
            NodeRef child1 = n.append_child();
            NodeRef child2 = n.append_child();
            child0.set_key("key0");
            child1.set_key("key1");
            child2.set_key("key2");
            child0._add_flags(extra);
            child1._add_flags(extra);
            child2._add_flags(extra);
        }),
        #undef _
    }};
}


#define RUN_BUILDER_TEST_CASE(...) \
    {                              \
        BUILDTRACE("test case");   \
        BuilderBlock blocks[] = {  \
            __VA_ARGS__            \
        };                         \
        apply(blocks);             \
    }
#define blk(...) BuilderBlock(__VA_ARGS__)
bool no_check_invariants = false;


//-----------------------------------------------------------------------------

TEST(build, stream_doc)
{
    RUN_BUILDER_TEST_CASE(
        blk(set_to_stream(), same_node),
        blk(add_to_seq(DOC|VAL|VAL_SQUO), nothing_else)
    );
}

TEST(build, root_scalar)
{
    RUN_BUILDER_TEST_CASE(
        blk(set_scalar_val("foo"), nothing_else)
    );
}
TEST(build, root_scalar_all)
{
    RUN_BUILDER_TEST_CASE(
        blk(set_scalar_val_all("foo"), nothing_else)
    );
}

TEST(build, root_scalar_with_style)
{
    RUN_BUILDER_TEST_CASE(
        blk(set_scalar_val("foo"), same_node),
        blk(set_val_style(), nothing_else)
    );
}

TEST(build, root_scalar_with_style_all)
{
    RUN_BUILDER_TEST_CASE(
        blk(set_scalar_val_all("foo"), same_node),
        blk(set_val_style_all(), nothing_else)
    );
}

TEST(build, root_seq_empty)
{
    RUN_BUILDER_TEST_CASE(
        blk(set_to_seq(), nothing_else)
    );
}

TEST(build, root_map_empty)
{
    RUN_BUILDER_TEST_CASE(
        blk(set_to_map(), nothing_else)
    );
}

TEST(build, root_seq)
{
    RUN_BUILDER_TEST_CASE(
        blk(set_to_seq(), same_node),
        blk(add_to_seq(VAL|VALNIL|VAL_PLAIN), nothing_else)
    );
}

TEST(build, root_map)
{
    RUN_BUILDER_TEST_CASE(
        blk(set_to_map(), same_node),
        blk(add_to_map(VAL|VALNIL|VAL_PLAIN), nothing_else)
    );
}

TEST(build, root_seq_val)
{
    RUN_BUILDER_TEST_CASE(
        blk(set_to_seq(), same_node),
        blk(add_to_seq(), all_children),
        blk(set_scalar_val("foo"), all_children),
        blk(set_val_style(), nothing_else),
    );
}
TEST(build, root_seq_val_all)
{
    RUN_BUILDER_TEST_CASE(
        blk(set_to_seq(), same_node),
        blk(add_to_seq(), all_children),
        blk(set_scalar_val_all("foo"), all_children),
        blk(set_val_style_all(), nothing_else),
    );
}

TEST(build, root_map_val)
{
    RUN_BUILDER_TEST_CASE(
        blk(set_to_map(), same_node),
        blk(add_to_map(), all_children, no_check_invariants),
        blk(set_scalar_keyval("foo", "bar"), all_children),
        blk(set_keyval_style(), nothing_else),
    );
}
TEST(build, root_map_val_all)
{
    RUN_BUILDER_TEST_CASE(
        blk(set_to_map(), same_node),
        blk(add_to_map(), all_children, no_check_invariants),
        blk(set_scalar_keyval_all("foo", "bar"), all_children),
        blk(set_keyval_style_all(), nothing_else),
    );
}

TEST(build, nested_seq_val)
{
    RUN_BUILDER_TEST_CASE(
        blk(set_to_seq(), same_node),
        blk(add_to_seq(), all_children),
        blk(set_to_seq(), same_node),
        blk(add_to_seq(), all_children),
        blk(set_scalar_val("foo"), nothing_else),
    );
}
TEST(build, nested_map_val)
{
    RUN_BUILDER_TEST_CASE(
        blk(set_to_map(), same_node),
        blk(add_to_map(), all_children, no_check_invariants),
        blk(set_to_map(), same_node),
        blk(add_to_map(), all_children, no_check_invariants),
        blk(set_scalar_keyval("foo", "bar"), nothing_else),
    );
}

TEST(build, map_nested_in_seq_val)
{
    RUN_BUILDER_TEST_CASE(
        blk(set_to_seq(), same_node),
        blk(add_to_seq(), all_children),
        blk(set_to_map(), same_node),
        blk(add_to_map(), all_children, no_check_invariants),
        blk(set_scalar_keyval("foo", "bar"), nothing_else),
    );
}
TEST(build, seq_nested_in_map)
{
    RUN_BUILDER_TEST_CASE(
        blk(set_to_map(), same_node),
        blk(add_to_map(), all_children, no_check_invariants),
        blk(set_to_seq(), same_node),
        blk(add_to_seq(), all_children),
        blk(set_scalar_val("foo"), nothing_else),
    );
}


//-------------------------------------------
// this is needed to use the test case library
Case const* get_case(csubstr /*name*/)
{
    return nullptr;
}

} // namespace yml
} // namespace c4
