#ifndef TEST_LIB_TEST_CASE_HPP_
#define TEST_LIB_TEST_CASE_HPP_

#include "test_lib/test_utils.hpp"
#include "test_lib/test_case_node.hpp"
#include "test_lib/test_helpers_tree.hpp"
#include "test_lib/test_helpers_ints.hpp"
#ifndef RYML_SINGLE_HEADER
#include "c4/yml/parse.hpp"
#include "c4/yml/emit.hpp"
#endif

C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")

namespace c4 {
namespace yml {

struct Case;
struct CaseData;

Case const* get_case(csubstr name);
CaseData* get_data(csubstr name);


struct Case
{
    std::string filelinebuf;
    csubstr fileline;
    csubstr name;
    csubstr src;
    TestCaseNode root;
    TestCaseFlags_e flags;
    Location expected_location;

    //! create a test case with an error on an expected location
    Case(csubstr file, int line, const char *name_, int f_, const char *src_, Location const& loc={}) : filelinebuf(catrs<std::string>(file, ':', line)), fileline(to_csubstr(filelinebuf)), name(to_csubstr(name_)), src(to_csubstr(src_)), root(), flags((TestCaseFlags_e)f_), expected_location(name, loc.line, loc.col) {}
    //! create a standard test case: name, source and expected CaseNode structure
    Case(csubstr file, int line, const char *name_,         const char *src_, TestCaseNode&& node) : /* */filelinebuf(catrs<std::string>(file, ':', line)), fileline(to_csubstr(filelinebuf)), name(to_csubstr(name_)), src(to_csubstr(src_)), root(std::move(node)), flags(), expected_location() {}
    //! create a test case with explicit flags: name, source flags, and expected CaseNode structure
    Case(csubstr file, int line, const char *name_, int f_, const char *src_, TestCaseNode&& node) : /* */filelinebuf(catrs<std::string>(file, ':', line)), fileline(to_csubstr(filelinebuf)), name(to_csubstr(name_)), src(to_csubstr(src_)), root(std::move(node)), flags((TestCaseFlags_e)f_), expected_location()  {}
};


//-----------------------------------------------------------------------------

template<class T>
struct src_and_val
{
    resultdep<txtbuf> orig; // original source
    resultdep<txtbuf> src; // mutated by the parse
    resultdep<T> val;
    void reset() {  orig.done = src.done = val.done = false; }
    void assign(csubstr src_)
    {
        if(src.ensure([&]{ src.result.assign(src_.begin(), src_.end()); }))
            orig = src;
    }
};
using SrcTree = src_and_val<Tree>;
using SrcInts = src_and_val<extra::ievt::TestBuffers>;


// a persistent data store to avoid repeating operations on every test
struct CaseDataLineEndings
{
    void assign(csubstr src_orig)
    {
        if(src_orig == src && src_orig.len)
            return;
        tree.reset();
        tree_roundtrip_yaml.reset();
        tree_roundtrip_json.reset();
        tree.val.result.clear();
        tree_roundtrip_yaml.val.result.clear();
        tree_roundtrip_json.val.result.clear();
        ints_resize.reset();
        ints_resize_roundtrip_yaml.reset();
        ints_resize_roundtrip_json.reset();
        ints_noresize.reset();
        ints_noresize_roundtrip_yaml.reset();
        ints_noresize_roundtrip_json.reset();
        const char *b = src_orig.begin();
        const char *e = src_orig.end();
        src.assign(b, e);
        tree.assign(src_orig);
        ints_resize.assign(src_orig);
        ints_noresize.assign(src_orig);
    }

    txtbuf src;

    SrcTree tree;
    SrcTree tree_roundtrip_yaml;
    SrcTree tree_roundtrip_json;

    SrcInts ints_resize;
    SrcInts ints_resize_roundtrip_yaml;
    SrcInts ints_resize_roundtrip_json;

    SrcInts ints_noresize;
    SrcInts ints_noresize_roundtrip_yaml;
    SrcInts ints_noresize_roundtrip_json;

    void ensure_tree_parse(Case const *c);
    void ensure_tree_emit_yaml(Case const *c);
    void ensure_tree_emit_json(Case const *c);
    void ensure_tree_roundtrip_yaml(Case const *c);
    void ensure_tree_roundtrip_json(Case const *c);

    void ensure_ints_parse(Case const *c, SrcInts *dst);
    void ensure_ints_emit(Case const *c, SrcInts *ints, SrcInts *roundtrip, EmitType_e type);
    void ensure_ints_roundtrip(Case const *c, SrcInts *ints, SrcInts *roundtrip, EmitType_e type);

    void ensure_ints_resize_emit_yaml(Case const *c);
    void ensure_ints_resize_emit_json(Case const *c);
    void ensure_ints_resize_roundtrip_yaml(Case const *c);
    void ensure_ints_resize_roundtrip_json(Case const *c);

    void ensure_ints_noresize_emit_yaml(Case const *c);
    void ensure_ints_noresize_emit_json(Case const *c);
    void ensure_ints_noresize_roundtrip_yaml(Case const *c);
    void ensure_ints_noresize_roundtrip_json(Case const *c);
};


struct CaseData
{
    CaseDataLineEndings unix_style;
    CaseDataLineEndings windows_style;

    resultdep<Tree> reftree;
    void ensure_reftree(Case const *c);
};

} // namespace yml
} // namespace c4

#endif /* TEST_LIB_TEST_CASE_HPP_ */
