#ifndef RYML_SINGLE_HEADER
#include "c4/yml/detail/print.hpp"
#endif
#include "test_group.hpp"
#include "test_case.hpp"
#include <c4/fs/fs.hpp>
#include <fstream>
#include <stdexcept>

#define RYML_NFO (RYML_DBG || 0)

//-----------------------------------------------------------------------------
namespace c4 {
namespace yml {

void YmlTestCase::_test_parse_using_ryml(CaseDataLineEndings *cd)
{
    #ifdef RYML_NFO
    std::cout << "---------------\n";
    std::cout << c->src;
    std::cout << "---------------\n";
    #endif

    if(c->flags & EXPECT_PARSE_ERROR)
    {
        auto flags = c->flags;
        ExpectError::do_check(&cd->parsed_tree, [this, cd, flags](){
            parse_in_place(c->fileline, cd->src, &cd->parsed_tree);
            if(flags & RESOLVE_REFS)
                cd->parsed_tree.resolve();
            #ifdef RYML_DBG
            // if this point was reached, then it means that the expected
            // error failed to occur. So print debugging info.
            std::cout << "failed to catch expected error while parsing.\nPARSED TREE:\n";
            print_tree(cd->parsed_tree);
            #endif
        }, c->expected_location);
        return;
    }

    cd->parsed_tree.clear();
    parse_in_place(c->fileline, cd->src, &cd->parsed_tree);

    #ifdef RYML_NFO
    std::cout << "REF TREE:\n";
    print_tree(c->root);
    std::cout << "PARSED TREE:\n";
    print_tree(cd->parsed_tree);
    #endif

    {
        SCOPED_TRACE("checking tree invariants of unresolved parsed tree");
        test_invariants(cd->parsed_tree);
    }
    {
        SCOPED_TRACE("checking node invariants of unresolved parsed tree");
        test_invariants(cd->parsed_tree.rootref());
    }

    if(c->flags & RESOLVE_REFS)
    {
        cd->parsed_tree.resolve();
        #ifdef RYML_NFO
        std::cout << "resolved tree!!!\n";
        print_tree(cd->parsed_tree);
        #endif
        {
            SCOPED_TRACE("checking tree invariants of resolved parsed tree");
            test_invariants(cd->parsed_tree);
        }
        {
            SCOPED_TRACE("checking node invariants of resolved parsed tree");
            test_invariants(cd->parsed_tree.rootref());
        }
    }

    {
        SCOPED_TRACE("comparing parsed tree to ref tree");
        EXPECT_GE(cd->parsed_tree.capacity(), c->root.reccount());
        EXPECT_EQ(cd->parsed_tree.size(), c->root.reccount());
        c->root.compare(cd->parsed_tree.rootref());
    }

    if(c->flags & RESOLVE_REFS)
    {
        cd->parsed_tree.reorder();
        #ifdef RYML_NFO
        std::cout << "reordered tree!!!\n";
        print_tree(cd->parsed_tree);
        #endif
        {
            SCOPED_TRACE("checking tree invariants of reordered parsed tree after resolving");
            test_invariants(cd->parsed_tree);
        }
        {
            SCOPED_TRACE("checking node invariants of reordered parsed tree after resolving");
            test_invariants(cd->parsed_tree.rootref());
        }

        {
            SCOPED_TRACE("comparing parsed tree to ref tree");
            EXPECT_GE(cd->parsed_tree.capacity(), c->root.reccount());
            EXPECT_EQ(cd->parsed_tree.size(), c->root.reccount());
            c->root.compare(cd->parsed_tree.rootref());
        }
    }
}


//-----------------------------------------------------------------------------
void YmlTestCase::_test_emit_yml_stdout(CaseDataLineEndings *cd)
{
    if(c->flags & EXPECT_PARSE_ERROR)
        return;
    if(cd->parsed_tree.empty())
        parse_in_place(c->fileline, cd->src, &cd->parsed_tree);
    if(cd->emit_buf.empty())
        cd->emitted_yml = emitrs(cd->parsed_tree, &cd->emit_buf);
    cd->numbytes_stdout = emit(cd->parsed_tree);
}

//-----------------------------------------------------------------------------
void YmlTestCase::_test_emit_yml_cout(CaseDataLineEndings *cd)
{
    if(c->flags & EXPECT_PARSE_ERROR)
        return;
    if(cd->parsed_tree.empty())
        parse_in_place(c->fileline, cd->src, &cd->parsed_tree);
    if(cd->emit_buf.empty())
        cd->emitted_yml = emitrs(cd->parsed_tree, &cd->emit_buf);
    std::cout << cd->parsed_tree;
}


//-----------------------------------------------------------------------------
void YmlTestCase::_test_emit_yml_stringstream(CaseDataLineEndings *cd)
{
    if(c->flags & EXPECT_PARSE_ERROR)
        return;
    if(cd->parsed_tree.empty())
        parse_in_place(c->fileline, cd->src, &cd->parsed_tree);
    if(cd->emit_buf.empty())
        cd->emitted_yml = emitrs(cd->parsed_tree, &cd->emit_buf);
    {
        std::stringstream ss;
        ss << cd->parsed_tree;
        std::string actual = ss.str();
        EXPECT_EQ(actual, cd->emitted_yml);
    }
    {
        std::stringstream ss;
        ss << cd->parsed_tree.rootref();
        std::string actual = ss.str();
        EXPECT_EQ(actual, cd->emitted_yml);
    }
}

//-----------------------------------------------------------------------------
void YmlTestCase::_test_emit_yml_ofstream(CaseDataLineEndings *cd)
{
    if(c->flags & EXPECT_PARSE_ERROR)
        return;
    if(cd->parsed_tree.empty())
        parse_in_place(c->fileline, cd->src, &cd->parsed_tree);
    if(cd->emit_buf.empty())
        cd->emitted_yml = emitrs(cd->parsed_tree, &cd->emit_buf);
    auto fn = fs::tmpnam<std::string>();
    {
        std::ofstream f(fn, std::ios::binary);
        f << cd->parsed_tree;
    }
    auto actual = fs::file_get_contents<std::string>(fn.c_str());
    fs::rmfile(fn.c_str());
    EXPECT_EQ(actual, cd->emitted_yml);
}

//-----------------------------------------------------------------------------
void YmlTestCase::_test_emit_yml_string(CaseDataLineEndings *cd)
{
    if(c->flags & EXPECT_PARSE_ERROR)
        return;
    auto em = emitrs(cd->parsed_tree, &cd->emit_buf);
    EXPECT_EQ(em.len, cd->emit_buf.size());
    EXPECT_EQ(em.len, cd->numbytes_stdout);
    cd->emitted_yml = em;
    #ifdef RYML_NFO
    std::cout << em;
    #endif
}

//-----------------------------------------------------------------------------
void YmlTestCase::_test_emitrs(CaseDataLineEndings *cd)
{
    if(c->flags & EXPECT_PARSE_ERROR)
        return;
    using vtype = std::vector<char>;
    using stype = std::string;
    vtype vv, v = emitrs<vtype>(cd->parsed_tree);
    stype ss, s = emitrs<stype>(cd->parsed_tree);
    EXPECT_EQ(to_csubstr(v), to_csubstr(s));
    csubstr svv = emitrs(cd->parsed_tree, &vv);
    csubstr sss = emitrs(cd->parsed_tree, &ss);
    EXPECT_EQ(svv, sss);
}

//-----------------------------------------------------------------------------
void YmlTestCase::_test_emitrs_cfile(CaseDataLineEndings *cd)
{
    if(c->flags & EXPECT_PARSE_ERROR)
        return;
    auto s = emitrs<std::string>(cd->parsed_tree);
    std::string r;
    {
        c4::fs::ScopedTmpFile f;
        emit(cd->parsed_tree, f.m_file);
        fflush(f.m_file);
        r = f.contents<std::string>();
    }
    EXPECT_EQ(s, r);
}


//-----------------------------------------------------------------------------
void YmlTestCase::_test_complete_round_trip(CaseDataLineEndings *cd)
{
    if(c->flags & EXPECT_PARSE_ERROR)
        return;
    if(cd->parsed_tree.empty())
        parse_in_place(c->fileline, cd->src, &cd->parsed_tree);
    if(cd->emit_buf.empty())
        cd->emitted_yml = emitrs(cd->parsed_tree, &cd->emit_buf);
    {
        SCOPED_TRACE("parsing emitted yml");
        cd->parse_buf = cd->emit_buf;
        cd->parsed_yml = to_substr(cd->parse_buf);
        parse_in_place(c->fileline, cd->parsed_yml, &cd->emitted_tree);
    }
    #ifdef RYML_NFO
    std::cout << "~~~~~~~~~~~~~~ src yml:\n";
    _c4presc(cd->src);
    std::cout << "~~~~~~~~~~~~~~ parsed tree:\n";
    print_tree(cd->parsed_tree);
    std::cout << "~~~~~~~~~~~~~~ emitted yml:\n";
    _c4presc(cd->emitted_yml);
    std::cout << "~~~~~~~~~~~~~~ emitted tree:\n";
    print_tree(cd->emitted_tree);
    std::cout << "~~~~~~~~~~~~~~\n";
    #endif
    {
        SCOPED_TRACE("checking node invariants of emitted tree");
        test_invariants(cd->parsed_tree.rootref());
    }
    {
        SCOPED_TRACE("checking node invariants of emitted tree");
        test_invariants(cd->emitted_tree.rootref());
    }
    {
        SCOPED_TRACE("comparing emitted and parsed tree");
        test_compare(cd->emitted_tree, cd->parsed_tree);
    }
    {
        SCOPED_TRACE("checking tree invariants of emitted tree");
        test_invariants(cd->emitted_tree);
    }
    {
        SCOPED_TRACE("comparing parsed tree to ref tree");
        EXPECT_GE(cd->parsed_tree.capacity(), c->root.reccount());
        EXPECT_EQ(cd->parsed_tree.size(), c->root.reccount());
        c->root.compare(cd->parsed_tree.rootref());
    }
    {
        SCOPED_TRACE("comparing emitted tree to ref tree");
        EXPECT_GE(cd->emitted_tree.capacity(), c->root.reccount());
        EXPECT_EQ(cd->emitted_tree.size(), c->root.reccount());
        // in this case, we can ignore whether scalars are quoted.
        // Because it can happen that a scalar was quoted in the
        // original file, but the re-emitted data does not quote the
        // scalars.
        c->root.compare(cd->emitted_tree.rootref(), true);
    }
}

//-----------------------------------------------------------------------------
void YmlTestCase::_test_recreate_from_ref(CaseDataLineEndings *cd)
{
    if(c->flags & EXPECT_PARSE_ERROR)
        return;
    if(cd->parsed_tree.empty())
        parse_in_place(c->fileline, cd->src, &cd->parsed_tree);
    if(cd->emit_buf.empty())
        cd->emitted_yml = emitrs(cd->parsed_tree, &cd->emit_buf);
    {
        SCOPED_TRACE("recreating a new tree from the ref tree");
        cd->recreated.reserve(cd->parsed_tree.size());
        NodeRef r = cd->recreated.rootref();
        c->root.recreate(&r);
    }
    #ifdef RYML_NFO
    std::cout << "REF TREE:\n";
    print_tree(c->root);
    std::cout << "RECREATED TREE:\n";
    print_tree(cd->recreated);
    #endif
    {
        SCOPED_TRACE("checking node invariants of recreated tree");
        test_invariants(cd->recreated.rootref());
    }
    {
        SCOPED_TRACE("checking tree invariants of recreated tree");
        test_invariants(cd->recreated);
    }
    {
        SCOPED_TRACE("comparing recreated tree to ref tree");
        c->root.compare(cd->recreated.rootref());
    }
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, parse_unix)
{
    SCOPED_TRACE("unix style");
    _test_parse_using_ryml(&d->unix_style);
}

TEST_P(YmlTestCase, parse_windows)
{
    SCOPED_TRACE("windows style");
    _test_parse_using_ryml(&d->windows_style);
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, emit_yml_unix_stdout)
{
    SCOPED_TRACE("unix style");
    _test_emit_yml_stdout(&d->unix_style);
}

TEST_P(YmlTestCase, emit_yml_windows_stdout)
{
    SCOPED_TRACE("windows style");
    _test_emit_yml_stdout(&d->windows_style);
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, emit_yml_unix_cout)
{
    SCOPED_TRACE("unix style");
    _test_emit_yml_cout(&d->unix_style);
}

TEST_P(YmlTestCase, emit_yml_windows_cout)
{
    SCOPED_TRACE("windows style");
    _test_emit_yml_cout(&d->windows_style);
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, emit_yml_unix_stringstream)
{
    SCOPED_TRACE("unix style");
    _test_emit_yml_stringstream(&d->unix_style);
}

TEST_P(YmlTestCase, emit_yml_windows_stringstream)
{
    SCOPED_TRACE("windows style");
    _test_emit_yml_stringstream(&d->windows_style);
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, emit_yml_unix_ofstream)
{
    SCOPED_TRACE("unix style");
    _test_emit_yml_ofstream(&d->unix_style);
}

TEST_P(YmlTestCase, emit_yml_windows_ofstream)
{
    SCOPED_TRACE("windows style");
    _test_emit_yml_ofstream(&d->windows_style);
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, emit_yml_unix_string)
{
    SCOPED_TRACE("unix style");
    _test_emit_yml_string(&d->unix_style);
}

TEST_P(YmlTestCase, emit_yml_windows_string)
{
    SCOPED_TRACE("windows style");
    _test_emit_yml_string(&d->windows_style);
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, unix_emitrs)
{
    SCOPED_TRACE("unix style");
    _test_emitrs(&d->unix_style);
}

TEST_P(YmlTestCase, windows_emitrs)
{
    SCOPED_TRACE("windows style");
    _test_emitrs(&d->windows_style);
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, unix_emitrs_cfile)
{
    SCOPED_TRACE("unix style");
    _test_emitrs_cfile(&d->unix_style);
}

TEST_P(YmlTestCase, windows_emitrs_cfile)
{
    SCOPED_TRACE("windows style");
    _test_emitrs_cfile(&d->windows_style);
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, complete_unix_round_trip)
{
    SCOPED_TRACE("unix style");
    _test_complete_round_trip(&d->unix_style);
}

TEST_P(YmlTestCase, complete_windows_round_trip)
{
    SCOPED_TRACE("windows style");
    _test_complete_round_trip(&d->windows_style);
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, unix_recreate_from_ref)
{
    SCOPED_TRACE("unix style");
    _test_recreate_from_ref(&d->unix_style);
}

TEST_P(YmlTestCase, windows_recreate_from_ref)
{
    SCOPED_TRACE("windows style");
    _test_recreate_from_ref(&d->windows_style);
}


} // namespace yml
} // namespace c4
