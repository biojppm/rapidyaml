#include "./test_group.hpp"
#include "c4/yml/detail/print.hpp"
#include <c4/fs/fs.hpp>
#include <fstream>
#include <stdexcept>

// this is needed to include yaml-cpp (!)
#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable: 4127/*conditional expression is constant*/)
#   pragma warning(disable: 4389/*'==': signed/unsigned mismatch*/)
#elif defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wshadow"
#   pragma clang diagnostic ignored "-Wfloat-equal"
#   pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined(__GNUC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wshadow"
#   pragma GCC diagnostic ignored "-Wfloat-equal"
#   pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
#include <yaml-cpp/yaml.h>
#if defined(_MSC_VER)
#   pragma warning(pop)
#elif defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__GNUC__)
#   pragma GCC diagnostic pop
#endif


//-----------------------------------------------------------------------------
namespace c4 {
namespace yml {

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, parse_using_ryml)
{
#ifdef RYML_DBG
    std::cout << "---------------\n";
    std::cout << c->src;
    std::cout << "---------------\n";
#endif

    if(c->flags & EXPECT_PARSE_ERROR)
    {
        ExpectError::do_check([this](){
            parse(d->src, &d->parsed_tree);
        }, c->expected_location);
        return;
    }

    parse(d->src, &d->parsed_tree);
    {
        SCOPED_TRACE("checking tree invariants of unresolved parsed tree");
        test_invariants(d->parsed_tree);
    }
#ifdef RYML_DBG
    std::cout << "REF TREE:\n";
    print_tree(c->root);
    std::cout << "PARSED TREE:\n";
    print_tree(d->parsed_tree);
#endif
    {
        SCOPED_TRACE("checking node invariants of unresolved parsed tree");
        test_invariants(d->parsed_tree.rootref());
    }

    if(c->flags & RESOLVE_REFS)
    {
        d->parsed_tree.resolve();
#ifdef RYML_DBG
        std::cout << "resolved tree!!!\n";
        print_tree(d->parsed_tree);
#endif
        {
            SCOPED_TRACE("checking tree invariants of resolved parsed tree");
            test_invariants(d->parsed_tree);
        }
        {
            SCOPED_TRACE("checking node invariants of resolved parsed tree");
            test_invariants(d->parsed_tree.rootref());
        }
    }

    {
        SCOPED_TRACE("comparing parsed tree to ref tree");
        EXPECT_GE(d->parsed_tree.capacity(), c->root.reccount());
        EXPECT_EQ(d->parsed_tree.size(), c->root.reccount());
        c->root.compare(d->parsed_tree.rootref());
    }

    if(c->flags & RESOLVE_REFS)
    {
        d->parsed_tree.reorder();
#ifdef RYML_DBG
        std::cout << "reordered tree!!!\n";
        print_tree(d->parsed_tree);
#endif
        {
            SCOPED_TRACE("checking tree invariants of reordered parsed tree after resolving");
            test_invariants(d->parsed_tree);
        }
        {
            SCOPED_TRACE("checking node invariants of reordered parsed tree after resolving");
            test_invariants(d->parsed_tree.rootref());
        }

        {
            SCOPED_TRACE("comparing parsed tree to ref tree");
            EXPECT_GE(d->parsed_tree.capacity(), c->root.reccount());
            EXPECT_EQ(d->parsed_tree.size(), c->root.reccount());
            c->root.compare(d->parsed_tree.rootref());
        }
    }
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, emit_yml_stdout)
{
    if(c->flags & EXPECT_PARSE_ERROR) return;
    if(d->parsed_tree.empty())
    {
        parse(d->src, &d->parsed_tree);
    }
    if(d->emit_buf.empty())
    {
        d->emitted_yml = emitrs(d->parsed_tree, &d->emit_buf);
    }

    d->numbytes_stdout = emit(d->parsed_tree);
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, emit_yml_cout)
{
    if(c->flags & EXPECT_PARSE_ERROR) return;
    if(d->parsed_tree.empty())
    {
        parse(d->src, &d->parsed_tree);
    }
    if(d->emit_buf.empty())
    {
        d->emitted_yml = emitrs(d->parsed_tree, &d->emit_buf);
    }

    std::cout << d->parsed_tree;
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, emit_yml_stringstream)
{
    if(c->flags & EXPECT_PARSE_ERROR) return;

    std::string s;
    std::vector<char> v;
    csubstr sv = emitrs(d->parsed_tree, &v);

    {
        std::stringstream ss;
        ss << d->parsed_tree;
        s = ss.str();
        EXPECT_EQ(sv, s);
    }

    {
        std::stringstream ss;
        ss << d->parsed_tree.rootref();
        s = ss.str();

        csubstr sv2 = emitrs(d->parsed_tree, &v);
        EXPECT_EQ(sv2, s);
    }
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, emit_ofstream)
{
    if(c->flags & EXPECT_PARSE_ERROR) return;
    auto s = emitrs<std::string>(d->parsed_tree);
    auto fn = c4::fs::tmpnam<std::string>();
    {
        std::ofstream f(fn);
        f << d->parsed_tree;
    }
    auto r = c4::fs::file_get_contents<std::string>(fn.c_str());
    c4::fs::delete_file(fn.c_str());
    // using ofstream will use \r\n. So delete it.
    std::string filtered;
    filtered.reserve(r.size());
    for(auto c_ : r)
    {
        if(c_ != '\r') filtered += c_;
    }
    EXPECT_EQ(s, filtered);
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, emit_yml_string)
{
    if(c->flags & EXPECT_PARSE_ERROR) return;
    auto em = emitrs(d->parsed_tree, &d->emit_buf);
    EXPECT_EQ(em.len, d->emit_buf.size());
    EXPECT_EQ(em.len, d->numbytes_stdout);
    d->emitted_yml = em;

#ifdef RYML_DBG
    std::cout << em;
#endif
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, emitrs)
{
    if(c->flags & EXPECT_PARSE_ERROR) return;
    using vtype = std::vector<char>;
    using stype = std::string;

    vtype vv, v = emitrs<vtype>(d->parsed_tree);
    stype ss, s = emitrs<stype>(d->parsed_tree);
    EXPECT_EQ(to_csubstr(v), to_csubstr(s));

    csubstr svv = emitrs(d->parsed_tree, &vv);
    csubstr sss = emitrs(d->parsed_tree, &ss);
    EXPECT_EQ(svv, sss);
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, emitrs_cfile)
{
    if(c->flags & EXPECT_PARSE_ERROR) return;
    auto s = emitrs<std::string>(d->parsed_tree);
    std::string r;
    {
        c4::fs::ScopedTmpFile f;
        emit(d->parsed_tree, f.m_file);
        fflush(f.m_file);
        r = f.contents<std::string>();
    }
    EXPECT_EQ(s, r);
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, complete_round_trip)
{
    if(c->flags & EXPECT_PARSE_ERROR) return;
    if(d->parsed_tree.empty())
    {
        parse(d->src, &d->parsed_tree);
    }
    if(d->emit_buf.empty())
    {
        d->emitted_yml = emitrs(d->parsed_tree, &d->emit_buf);
    }

#ifdef RYML_DBG
    print_tree(d->parsed_tree);
    std::cout << d->emitted_yml;
#endif

    {
        SCOPED_TRACE("parsing emitted yml");
        d->parse_buf = d->emit_buf;
        d->parsed_yml.assign(d->parse_buf.data(), d->parse_buf.size());
        parse(d->parsed_yml, &d->emitted_tree);
#ifdef RYML_DBG
        print_tree(d->emitted_tree);
#endif
    }

    {
        SCOPED_TRACE("checking node invariants of parsed tree");
        test_invariants(d->emitted_tree.rootref());
    }

    {
        SCOPED_TRACE("checking tree invariants of parsed tree");
        test_invariants(d->emitted_tree);
    }

    {
        SCOPED_TRACE("comparing parsed tree to ref tree");
        EXPECT_GE(d->emitted_tree.capacity(), c->root.reccount());
        EXPECT_EQ(d->emitted_tree.size(), c->root.reccount());
        c->root.compare(d->emitted_tree.rootref());
    }
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, recreate_from_ref)
{
    if(c->flags & EXPECT_PARSE_ERROR) return;

    if(d->parsed_tree.empty())
    {
        parse(d->src, &d->parsed_tree);
    }
    if(d->emit_buf.empty())
    {
        d->emitted_yml = emitrs(d->parsed_tree, &d->emit_buf);
    }

    {
        SCOPED_TRACE("recreating a new tree from the ref tree");
        d->recreated.reserve(d->parsed_tree.size());
        NodeRef r = d->recreated.rootref();
        c->root.recreate(&r);
    }

    {
        SCOPED_TRACE("checking node invariants of recreated tree");
        test_invariants(d->recreated.rootref());
    }

    {
        SCOPED_TRACE("checking tree invariants of recreated tree");
        test_invariants(d->recreated);
    }

    {
        SCOPED_TRACE("comparing recreated tree to ref tree");
        c->root.compare(d->recreated.rootref());
    }
}

} // namespace yml
} // namespace c4
