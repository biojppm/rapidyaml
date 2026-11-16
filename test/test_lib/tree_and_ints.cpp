#include <test_lib/tree_and_ints.hpp>

#ifdef RYML_SINGLE_HEADER
#include <ryml_all.hpp>
#else
#include <c4/yml/parse.hpp>
#include <c4/yml/parse_engine.hpp>
#include <c4/yml/emit.hpp>
#endif
#include <test_lib/test_utils.hpp>
#include <gtest/gtest.h>

namespace c4 {
namespace yml {

TreeAndInts::TreeAndInts(csubstr src, ParserOptions const& opts)
    : tree()
    , ints()
    , src_ints()
{
    parse(src, opts);
}
void TreeAndInts::parse(csubstr src, ParserOptions const& opts)
{
    parse_in_arena(src, &tree, opts);
    src_ints.assign(src.str, src.len);
    parse_ints(src_ints, &ints, opts);
}


//-----------------------------------------------------------------------------
void test_parse_err_yaml_tree(csubstr yaml)
{
    SCOPED_TRACE("test_parse_err_tree");
    bool failed = false;
    Tree tree;
    RYML_EXPECT_ERROR(check_error_parse(&tree, [&]{
        parse_in_arena(yaml);
        // if we reach this, it's a failure
        failed = true;
    }, /*only_basic*/false));
    if(failed)
        print_tree(tree);
}
void test_parse_err_yaml_ints(csubstr yaml)
{
    SCOPED_TRACE("test_parse_err_ints");
    bool failed = false;
    IntBufs ints;
    txtbuf src(yaml.str, yaml.len);
    RYML_EXPECT_ERROR(check_error_parse([&]{
        parse_ints(src, &ints);
        // if we reach this, it's a failure
        failed = true;
    }, /*only_basic*/false));
    if(failed)
        ints.print();
}
void test_parse_err_yaml(csubstr yaml)
{
    test_parse_err_yaml_tree_(yaml);
    test_parse_err_yaml_ints_(yaml);
}



//-----------------------------------------------------------------------------


void test_resolve_tree(Tree &tree, ResolveOptions const& opts, ResolveParams const& params)
{
    SCOPED_TRACE("test_resolve_tree");
    {
        SCOPED_TRACE("invariants before");
        test_invariants(tree);
    }
    RYML_EXPECT_ERROR(check_success(&tree, [&]{
        tree.resolve(opts, params);
    }));
    {
        SCOPED_TRACE("invariants after");
        test_invariants(tree);
    }
    if(testing::Test::HasFailure())
        print_tree(tree);
}
void test_resolve_ints(IntBufs &ints, ResolveOptions const& opts, ResolveParams const& params)
{
    SCOPED_TRACE("test_resolve_ints");
    {
        SCOPED_TRACE("invariants before");
        ints.test_invariants();
    }
    if(testing::Test::HasFailure())
    {
        ints.print();
        return;
    }
    RYML_EXPECT_ERROR(check_success([&]{
        ints.resolve(opts, params);
    }));
    if(!testing::Test::HasFailure())
    {
        SCOPED_TRACE("invariants after");
        ints.test_invariants();
    }
    if(testing::Test::HasFailure())
        ints.print();
}
void test_resolve(TreeAndInts &ti, ResolveOptions const& opts, ResolveParams const& params)
{
    test_resolve_tree(ti.tree, opts, params);
    test_resolve_ints(ti.ints, opts, params);
}


void test_resolve_err_tree(Tree &tree, ResolveOptions const& opts, ResolveParams const& params)
{
    SCOPED_TRACE("test_resolve_err_tree");
    bool failed = false;
    RYML_EXPECT_ERROR(check_error_basic(&tree, [&]{
        tree.resolve(opts, params);
        // if we reach this, it's a failure
        failed = true;
    }, /*only_basic*/false));
    if(failed)
        print_tree(tree);
}
void test_resolve_err_ints(IntBufs &ints, ResolveOptions const& opts, ResolveParams const& params)
{
    SCOPED_TRACE("test_resolve_err_ints");
    bool failed = false;
    RYML_EXPECT_ERROR(check_error_basic([&]{
        ints.resolve(opts, params);
        // if we reach this, it's a failure
        failed = true;
    }));
    if(failed)
        ints.print();
}
void test_resolve_err(TreeAndInts &ti, ResolveOptions const& opts, ResolveParams const& params)
{
    test_resolve_err_tree(ti.tree, opts, params);
    test_resolve_err_ints(ti.ints, opts, params);
}


//-----------------------------------------------------------------------------

void test_emit_yaml_tree(ConstNodeRef n, std::string const& expected, EmitOptions const& opts)
{
    if(!testing::Test::HasFailure())
    {
        RYML_TRACE_FMT("tree: id", n.id());
        EXPECT_EQ(emitrs_yaml<std::string>(n, opts), expected);
        if(testing::Test::HasFailure())
            print_tree(*n.tree());
    }
}
void test_emit_yaml_tree(ConstNodeRef n, EmitOptions const& opts, std::string const& expected)
{
    test_emit_yaml_tree(n, expected, opts);
}


void test_emit_yaml_ints(IntBufsCR ints, extra::ievt::evt_size pos, std::string const& expected, EmitOptions const& opts)
{
    if(!testing::Test::HasFailure())
    {
        RYML_TRACE_FMT("ints: pos", pos);
        EXPECT_EQ(ints.emitrs_yaml<std::string>(opts, pos), expected);
        if(testing::Test::HasFailure())
            ints.print();
    }
}
void test_emit_yaml_ints(IntBufsCR ints, extra::ievt::evt_size pos, EmitOptions const& opts, std::string const& expected)
{
    test_emit_yaml_ints(ints, pos, expected, opts);
}


void test_emit_yaml_same_ints(ConstNodeRef n, std::string const& expected, EmitOptions const& opts)
{
    if(!testing::Test::HasFailure())
    {
        RYML_TRACE_FMT("tree: id", n.id());
        std::string emitted_tree = emitrs_yaml<std::string>(n, opts);
        EXPECT_EQ(emitted_tree, expected);
        if(testing::Test::HasFailure())
        {
            print_tree(*n.tree());
            return;
        }
        {
            SCOPED_TRACE("here");
            IntBufs ints;
            parse_ints(to_substr(emitted_tree), &ints);
            test_emit_yaml_ints(ints, 0, expected, opts);
        }
    }
}

void test_emit_yaml(ConstNodeRef n, IntBufsCR ints, extra::ievt::evt_size pos, std::string const& expected, EmitOptions const& opts)
{
    SCOPED_CALL(test_emit_yaml_tree, (n, expected, opts));
    SCOPED_CALL(test_emit_yaml_ints, (ints, pos, expected, opts));
}
void test_emit_yaml(ConstNodeRef n, IntBufsCR ints, extra::ievt::evt_size pos, EmitOptions const& opts, std::string const& expected)
{
    test_emit_yaml(n, ints, pos, expected, opts);
}
void test_emit_yaml(TreeAndInts const& ti, std::string const& expected, EmitOptions const& opts)
{
    test_emit_yaml(ti.tree.rootref(), ti.ints, 0, expected, opts);
}
void test_emit_yaml(TreeAndInts const& ti, EmitOptions const& opts, std::string const& expected)
{
    test_emit_yaml(ti.tree.rootref(), ti.ints, 0, expected, opts);
}


void test_emit_json_tree(ConstNodeRef n, std::string const& expected, EmitOptions const& opts)
{
    if(!testing::Test::HasFailure())
    {
        RYML_TRACE_FMT("tree: id", n.id());
        EXPECT_EQ(emitrs_json<std::string>(n, opts), expected);
        if(testing::Test::HasFailure())
            print_tree(*n.tree());
    }
}
void test_emit_json_ints(IntBufsCR ints, extra::ievt::evt_size pos, std::string const& expected, EmitOptions const& opts)
{
    if(!testing::Test::HasFailure())
    {
        RYML_TRACE_FMT("ints: pos", pos);
        EXPECT_EQ(ints.emitrs_json<std::string>(opts, pos), expected);
        if(testing::Test::HasFailure())
            ints.print();
    }
}
void test_emit_json_same_ints(ConstNodeRef n, std::string const& expected, EmitOptions const& opts)
{
    if(!testing::Test::HasFailure())
    {
        RYML_TRACE_FMT("tree: id", n.id());
        std::string emitted_tree = emitrs_json<std::string>(n, opts);
        EXPECT_EQ(emitted_tree, expected);
        if(testing::Test::HasFailure())
        {
            print_tree(*n.tree());
            return;
        }
        {
            SCOPED_TRACE("here");
            IntBufs ints;
            parse_ints(to_substr(emitted_tree), &ints);
            test_emit_json_ints(ints, 0, expected, opts);
        }
    }
}
void test_emit_json_tree(ConstNodeRef n, EmitOptions const& opts, std::string const& expected)
{
    test_emit_json_tree(n, expected, opts);
}
void test_emit_json(IntBufsCR ints, extra::ievt::evt_size pos, EmitOptions const& opts, std::string const& expected)
{
    test_emit_json_ints(ints, pos, expected, opts);
}
void test_emit_json(ConstNodeRef n, IntBufsCR ints, extra::ievt::evt_size pos, std::string const& expected, EmitOptions const& opts)
{
    test_emit_json_tree(n, expected, opts);
    test_emit_json_ints(ints, pos, expected, opts);
}
void test_emit_json(ConstNodeRef n, IntBufsCR ints, extra::ievt::evt_size pos, EmitOptions const& opts, std::string const& expected)
{
    test_emit_json(n, ints, pos, expected, opts);
}
void test_emit_json(TreeAndInts const& ti, std::string const& expected, EmitOptions const& opts)
{
    test_emit_json(ti.tree.rootref(), ti.ints, 0, expected, opts);
}
void test_emit_json(TreeAndInts const& ti, EmitOptions const& opts, std::string const& expected)
{
    test_emit_json(ti.tree.rootref(), ti.ints, 0, opts, expected);
}



void test_emit_error_yaml_tree(NodeRef n, EmitOptions const& opts)
{
    RYML_TRACE_FMT("tree: id", n.id());
    RYML_EXPECT_ERROR(check_error_visit(n.tree(), [&]{
        emitrs_yaml<std::string>(n, opts);
        // if we reach this, it's a failure
        print_tree(*n.tree());
        GTEST_FAIL();
    }));
}
void test_emit_error_yaml_ints(IntBufsCR ints, extra::ievt::evt_size pos, EmitOptions const& opts)
{
    RYML_TRACE_FMT("ints: pos", pos);
    RYML_EXPECT_ERROR(check_error_basic([&]{
        ints.emitrs_yaml<std::string>(opts, pos);
        // if we reach this, it's a failure
        ints.print();
        GTEST_FAIL();
    }));
}
void test_emit_error_yaml_ints(IntBufsCR ints, EmitOptions const& opts)
{
    test_emit_error_yaml_ints(ints, 0, opts);
}

void test_emit_error_yaml(NodeRef n, IntBufsCR ints, extra::ievt::evt_size pos, EmitOptions const& opts)
{
    test_emit_error_yaml_tree(n, opts);
    test_emit_error_yaml_ints(ints, pos, opts);
}
void test_emit_error_yaml(NodeRef n, IntBufsCR ints, EmitOptions const& opts)
{
    test_emit_error_yaml(n, ints, 0, opts);
}
void test_emit_error_yaml(TreeAndInts & ti, EmitOptions const& opts)
{
    test_emit_error_yaml(ti.tree.rootref(), ti.ints, 0, opts);
}

void test_emit_error_json_tree(NodeRef n, EmitOptions const& opts)
{
    RYML_TRACE_FMT("tree: id", n.id());
    RYML_EXPECT_ERROR(check_error_visit(n.tree(), [&]{
        emitrs_json<std::string>(n, opts);
        // if we reach this, it's a failure
        print_tree(*n.tree());
    }));
}
void test_emit_error_json_ints(IntBufsCR ints, extra::ievt::evt_size pos, EmitOptions const& opts)
{
    RYML_TRACE_FMT("ints: pos", pos);
    RYML_EXPECT_ERROR(check_error_basic([&]{
        ints.emitrs_json<std::string>(opts, pos);
        // if we reach this, it's a failure
        ints.print();
    }));
}
void test_emit_error_json_ints(IntBufsCR ints, EmitOptions const& opts)
{
    test_emit_error_json_ints(ints, 0, opts);
}

void test_emit_error_json(NodeRef n, IntBufsCR ints, extra::ievt::evt_size pos, EmitOptions const& opts)
{
    test_emit_error_json_tree(n, opts);
    test_emit_error_json_ints(ints, pos, opts);
}
void test_emit_error_json(NodeRef n, IntBufsCR ints, EmitOptions const& opts)
{
    test_emit_error_json(n, ints, 0, opts);
}
void test_emit_error_json(TreeAndInts & ti, EmitOptions const& opts)
{
    test_emit_error_json(ti.tree.rootref(), ti.ints, 0, opts);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void test_resolve(ResolveOptions opts, ResolveParams params, std::string const& yaml, std::string const& resolved)
{
    TreeAndInts ti(to_csubstr(yaml));
    test_emit_yaml_(ti, yaml);
    test_resolve_(ti, opts, params);
    if(!resolved.empty())
        test_emit_yaml_(ti, resolved);
    else
        test_emit_yaml_(ti, yaml);
}
void test_resolve(ResolveOptions opts, std::string const& yaml, std::string const& resolved)
{
    test_resolve_(opts, ResolveParams{}, yaml, resolved);
}
void test_resolve(std::string const& yaml, std::string const& resolved)
{
    test_resolve_(ResolveOptions{}, ResolveParams{}, yaml, resolved);
}

void test_resolve_err(std::string const& yaml, ResolveOptions opts, ResolveParams params)
{
    TreeAndInts ti(to_csubstr(yaml));
    test_resolve_err_(ti, opts, params);
}
void test_resolve_err(ResolveOptions opts, ResolveParams params, std::string const& yaml)
{
    test_resolve_err(yaml, opts, params);
}
void test_resolve_err(ResolveOptions opts, std::string const& yaml)
{
    test_resolve_err(yaml, opts, ResolveParams{});
}

void test_emit_resolve_err(std::string const& yaml, std::string const& json, ResolveOptions opts, ResolveParams params)
{
    TreeAndInts ti(to_csubstr(yaml));
    test_emit_yaml_(ti, yaml);
    test_emit_json_(ti, json);
    test_resolve_err_(ti, opts, params);
}
void test_emit_resolve(std::string const& src, std::string const& yaml, std::string const& json, ResolveOptions opts, ResolveParams params)
{
    TreeAndInts ti(to_csubstr(src));
    test_resolve_(ti, opts, params);
    test_emit_yaml_(ti, yaml);
    test_emit_json_(ti, json);
}

} // namespace yml
} // namespace c4
