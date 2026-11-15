#ifndef TEST_TREE_AND_INTS_HPP_
#define TEST_TREE_AND_INTS_HPP_

#include <test_lib/test_helpers_tree.hpp>
#include <test_lib/test_helpers_ints.hpp>

namespace c4 {
namespace yml {

using IntBufs = extra::ievt::TestBuffers;
using IntBufsCR = IntBufs const&;

struct TreeAndInts
{
    Tree    tree;
    IntBufs ints;
    txtbuf  src_ints;
    TreeAndInts(csubstr src, ParserOptions const& opts={});
    void parse(csubstr src, ParserOptions const& opts={});
    TreeAndInts() noexcept : tree(), ints(), src_ints() {}
    TreeAndInts(TreeAndInts &&that) noexcept
        : tree(std::move(that.tree))
        , ints(std::move(that.ints))
        , src_ints(std::move(that.src_ints))
    {
        ints.src = src_ints;
    }
    ~TreeAndInts() = default;
    TreeAndInts(TreeAndInts const& that) = delete;
    TreeAndInts& operator= (TreeAndInts &&that) noexcept = delete;
    TreeAndInts& operator= (TreeAndInts const& that) noexcept = delete;
};


#define test_parse_err_yaml_tree_(...) SCOPED_CALL(test_parse_err_yaml_tree, (__VA_ARGS__))
#define test_parse_err_yaml_ints_(...) SCOPED_CALL(test_parse_err_yaml_ints, (__VA_ARGS__))
#define test_parse_err_yaml_(...) SCOPED_CALL(test_parse_err_yaml, (__VA_ARGS__))

#define test_resolve_tree_(...) SCOPED_CALL(test_resolve_tree, (__VA_ARGS__))
#define test_resolve_ints_(...) SCOPED_CALL(test_resolve_ints, (__VA_ARGS__))
#define test_resolve_(...) SCOPED_CALL(test_resolve, (__VA_ARGS__))
#define test_resolve_err_tree_(...) SCOPED_CALL(test_resolve_err_tree, (__VA_ARGS__))
#define test_resolve_err_ints_(...) SCOPED_CALL(test_resolve_err_ints, (__VA_ARGS__))
#define test_resolve_err_(...) SCOPED_CALL(test_resolve_err, (__VA_ARGS__))
#define test_emit_resolve_err_(...) SCOPED_CALL(test_emit_resolve_err, (__VA_ARGS__))


#define test_emit_yaml_(...) SCOPED_CALL(test_emit_yaml, (__VA_ARGS__))
#define test_emit_yaml_tree_(...) SCOPED_CALL(test_emit_yaml_tree, (__VA_ARGS__))
#define test_emit_yaml_ints_(...) SCOPED_CALL(test_emit_yaml_ints, (__VA_ARGS__))
#define test_emit_yaml_same_ints_(...) SCOPED_CALL(test_emit_yaml_same_ints, (__VA_ARGS__))
#define test_emit_error_yaml_(...) SCOPED_CALL(test_emit_error_yaml, (__VA_ARGS__))
#define test_emit_error_yaml_tree_(...) SCOPED_CALL(test_emit_error_yaml_tree, (__VA_ARGS__))
#define test_emit_error_yaml_ints_(...) SCOPED_CALL(test_emit_error_yaml_ints, (__VA_ARGS__))


#define test_emit_json_(...) SCOPED_CALL(test_emit_json, (__VA_ARGS__))
#define test_emit_json_tree_(...) SCOPED_CALL(test_emit_json_tree, (__VA_ARGS__))
#define test_emit_json_ints_(...) SCOPED_CALL(test_emit_json_ints, (__VA_ARGS__))
#define test_emit_json_same_ints_(...) SCOPED_CALL(test_emit_json_same_ints, (__VA_ARGS__))
#define test_emit_error_json_(...) SCOPED_CALL(test_emit_error_json, (__VA_ARGS__))
#define test_emit_error_json_tree_(...) SCOPED_CALL(test_emit_error_json_tree, (__VA_ARGS__))
#define test_emit_error_json_ints_(...) SCOPED_CALL(test_emit_error_json_ints, (__VA_ARGS__))



//-----------------------------------------------------------------------------
void test_parse_err_yaml_tree(csubstr yaml);
void test_parse_err_yaml_ints(csubstr yaml);
void test_parse_err_yaml(csubstr yaml);


//-----------------------------------------------------------------------------

void test_resolve_tree(Tree &tree, ResolveOptions const& opts={}, ResolveParams const& params={});
void test_resolve_ints(IntBufs &ints, ResolveOptions const& opts={}, ResolveParams const& params={});
void test_resolve(TreeAndInts &ti, ResolveOptions const& opts={}, ResolveParams const& params={});


void test_resolve_err_tree(Tree &tree, ResolveOptions const& opts={}, ResolveParams const& params={});
void test_resolve_err_ints(IntBufs &ints, ResolveOptions const& opts={}, ResolveParams const& params={});
void test_resolve_err(TreeAndInts &ti, ResolveOptions const& opts={}, ResolveParams const& params={});


//-----------------------------------------------------------------------------

void test_emit_yaml_tree(ConstNodeRef n, std::string const& expected, EmitOptions const& opts={});
void test_emit_yaml_tree(ConstNodeRef n, EmitOptions const& opts, std::string const& expected);


void test_emit_yaml_ints(IntBufsCR ints, extra::ievt::evt_size pos, std::string const& expected, EmitOptions const& opts={});
void test_emit_yaml_ints(IntBufsCR ints, extra::ievt::evt_size pos, EmitOptions const& opts, std::string const& expected);


void test_emit_yaml_same_ints(ConstNodeRef n, std::string const& expected, EmitOptions const& opts={});

void test_emit_yaml(ConstNodeRef n, IntBufsCR ints, extra::ievt::evt_size pos, std::string const& expected, EmitOptions const& opts={});
void test_emit_yaml(ConstNodeRef n, IntBufsCR ints, extra::ievt::evt_size pos, EmitOptions const& opts, std::string const& expected);
void test_emit_yaml(TreeAndInts const& ti, std::string const& expected, EmitOptions const& opts={});
void test_emit_yaml(TreeAndInts const& ti, EmitOptions const& opts, std::string const& expected);


void test_emit_json_tree(ConstNodeRef n, std::string const& expected, EmitOptions const& opts={});
void test_emit_json_ints(IntBufsCR ints, extra::ievt::evt_size pos, std::string const& expected, EmitOptions const& opts={});
void test_emit_json_same_ints(ConstNodeRef n, std::string const& expected, EmitOptions const& opts={});
void test_emit_json_tree(ConstNodeRef n, EmitOptions const& opts, std::string const& expected);
void test_emit_json(IntBufsCR ints, extra::ievt::evt_size pos, EmitOptions const& opts, std::string const& expected);
void test_emit_json(ConstNodeRef n, IntBufsCR ints, extra::ievt::evt_size pos, std::string const& expected, EmitOptions const& opts={});
void test_emit_json(ConstNodeRef n, IntBufsCR ints, extra::ievt::evt_size pos, EmitOptions const& opts, std::string const& expected);
void test_emit_json(TreeAndInts const& ti, std::string const& expected, EmitOptions const& opts={});
void test_emit_json(TreeAndInts const& ti, EmitOptions const& opts, std::string const& expected);



void test_emit_error_yaml_tree(NodeRef n, EmitOptions const& opts={});
void test_emit_error_yaml_ints(IntBufsCR ints, extra::ievt::evt_size pos, EmitOptions const& opts={});
void test_emit_error_yaml_ints(IntBufsCR ints, EmitOptions const& opts={});

void test_emit_error_yaml(NodeRef n, IntBufsCR ints, extra::ievt::evt_size pos, EmitOptions const& opts={});
void test_emit_error_yaml(NodeRef n, IntBufsCR ints, EmitOptions const& opts={});
void test_emit_error_yaml(TreeAndInts & ti, EmitOptions const& opts={});

void test_emit_error_json_tree(NodeRef n, EmitOptions const& opts={});
void test_emit_error_json_ints(IntBufsCR ints, extra::ievt::evt_size pos, EmitOptions const& opts={});
void test_emit_error_json_ints(IntBufsCR ints, EmitOptions const& opts={});

void test_emit_error_json(NodeRef n, IntBufsCR ints, extra::ievt::evt_size pos, EmitOptions const& opts={});
void test_emit_error_json(NodeRef n, IntBufsCR ints, EmitOptions const& opts={});
void test_emit_error_json(TreeAndInts & ti, EmitOptions const& opts={});


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void test_resolve(ResolveOptions opts, ResolveParams params, std::string const& yaml, std::string const& resolved={});
void test_resolve(ResolveOptions opts, std::string const& yaml, std::string const& resolved={});
void test_resolve(std::string const& yaml, std::string const& resolved={});

void test_resolve_err(std::string const& yaml, ResolveOptions opts={}, ResolveParams params={});
void test_resolve_err(ResolveOptions opts, ResolveParams params, std::string const& yaml);
void test_resolve_err(ResolveOptions opts, std::string const& yaml);

void test_emit_resolve_err(std::string const& yaml, std::string const& json, ResolveOptions opts={}, ResolveParams params={});
void test_emit_resolve(std::string const& src, std::string const& yaml, std::string const& json, ResolveOptions opts={}, ResolveParams params={});

} // namespace yml
} // namespace c4

#endif /* TEST_TREE_AND_INTS_HPP_ */
