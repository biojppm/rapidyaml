#ifndef RYML_SINGLE_HEADER
#include "c4/yml/detail/print.hpp"
#endif
#include "test_lib/test_group.hpp"
#include "test_lib/test_case.hpp"
#include "test_lib/test_events_ints_helpers.hpp"
#include <c4/yml/extra/event_handler_ints.hpp>
#include <c4/yml/extra/ints_utils.hpp>
#include <c4/yml/extra/ints_to_testsuite.hpp>
#include <c4/fs/fs.hpp>
#include <fstream>
#include <stdexcept>


//-----------------------------------------------------------------------------
namespace c4 {
namespace yml {

void YmlTestCase::_test_parse_using_ryml(CaseDataLineEndings *cd)
{
    #ifdef RYML_DBG
    if(_dbg_enabled())
        printf("---------------\n%.*s\n---------------\n", (int)c->src.len, c->src.str);
    #endif

    if(c->flags & (EXPECT_PARSE_ERROR|HAS_CONTAINER_KEYS))
    {
        ExpectError::check_error(&cd->parsed_tree, [this, cd]{
            parse_in_place(c->fileline, cd->src, &cd->parsed_tree);
            // if this point was reached, then it means that the expected
            // error failed to occur. So print debugging info.
            _c4dbg_tree("UNEXPECTED PARSED TREE", cd->parsed_tree);
        }, c->expected_location);
        return;
    }

    cd->parsed_tree.clear();
    parse_in_place(c->fileline, cd->src, &cd->parsed_tree);

    #ifdef RYML_DBG
    if(_dbg_enabled())
    {
        print_test_tree("REF TREE", c->root);
        _c4dbg_tree("PARSED TREE", cd->parsed_tree);
    }
    #endif

    if(!(c->flags & EXPECT_RESOLVE_ERROR))
    {
        SCOPED_TRACE("checking tree invariants of unresolved parsed tree");
        test_invariants(cd->parsed_tree);
    }
    if(!(c->flags & EXPECT_RESOLVE_ERROR))
    {
        SCOPED_TRACE("checking node invariants of unresolved parsed tree");
        test_invariants(cd->parsed_tree.rootref());
    }

    if(c->flags & RESOLVE_REFS)
    {
        if(c->flags & EXPECT_RESOLVE_ERROR)
        {
            ExpectError::check_error(&cd->parsed_tree, [&]{
                cd->parsed_tree.resolve();
                // if this point was reached, then it means that the expected
                // error failed to occur. So print debugging info.
                _c4dbg_tree("UNEXPECTED RESOLVED TREE", cd->parsed_tree);
            }, c->expected_location);
            return;
        }

        cd->parsed_tree.resolve();
        _c4dbg_tree("resolved tree!!!", cd->parsed_tree);
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
        _c4dbg_tree("reordered tree!!!", cd->parsed_tree);
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

static void _parse_events_ints(csubstr name, substr src, std::vector<int> *ints, std::vector<char> *arena)
{
    SCOPED_TRACE("parse_ints");
    using I = extra::ievt::DataType;
    using Handler = extra::EventHandlerInts;
    int estimated_size = extra::estimate_events_ints_size(src);
    ints->resize((size_t)estimated_size);
    arena->resize(src.len);
    Handler handler;
    handler.reset(src, to_substr(*arena), ints->data(), (I)ints->size());
    ParseEngine<Handler> parser(&handler);
    parser.parse_in_place_ev(name, src);
    EXPECT_GT(handler.required_size_events(), 0);
    ASSERT_GE(estimated_size, handler.required_size_events());
    ints->resize((size_t)handler.required_size_events());
    ASSERT_TRUE(handler.fits_buffers());
}

void YmlTestCase::_test_parse_using_ints(CaseDataLineEndings *cd)
{
    SCOPED_TRACE("test_parse_ints");

    #ifdef RYML_DBG
    if(_dbg_enabled())
        printf("---------------\n%.*s\n---------------\n", (int)c->src.len, c->src.str);
    #endif

    substr s = to_substr(cd->parse_buf_ints);
    auto printints = [&]{
        std::cout << extra::events_ints_to_testsuite<std::string>(s, to_csubstr(cd->arena_ints), cd->parsed_ints.data(), (int)cd->parsed_ints.size()) << "\n";
    };

    if(c->flags & EXPECT_PARSE_ERROR)
    {
        SCOPED_TRACE("expect error");
        ExpectError::check_error(&cd->parsed_tree, [&]{
            _parse_events_ints(c->fileline, s, &cd->parsed_ints, &cd->arena_ints);
            printints(); // error failed to occur. So print debugging info.
        }, c->expected_location);
        return;
    }

    _parse_events_ints(c->fileline, s, &cd->parsed_ints, &cd->arena_ints);

    #ifdef RYML_DBG
    if(_dbg_enabled())
        printints();
    #endif

    {
        SCOPED_TRACE("checking invariants");
        extra::test_events_ints_invariants(s, to_csubstr(cd->arena_ints), cd->parsed_ints.data(), (int)cd->parsed_ints.size());
    }
}


//-----------------------------------------------------------------------------
void YmlTestCase::_test_emit_yml_stdout(CaseDataLineEndings *cd)
{
    if(c->flags & (EXPECT_PARSE_ERROR|HAS_CONTAINER_KEYS))
        return;
    _ensure_parse(cd);
    _ensure_emit(cd);
    cd->numbytes_stdout = emit_yaml(cd->parsed_tree);
    EXPECT_EQ(cd->numbytes_stdout, cd->emitted_yml.size());
}

//-----------------------------------------------------------------------------
void YmlTestCase::_test_emit_json_stdout(CaseDataLineEndings *cd)
{
    if(!(c->flags & JSON_WRITE))
        return;
    if(c->flags & (EXPECT_PARSE_ERROR|HAS_CONTAINER_KEYS))
        return;
    _ensure_parse(cd);
    _ensure_emit_json(cd);
    cd->numbytes_stdout_json = emit_json(cd->parsed_tree);
    EXPECT_EQ(cd->numbytes_stdout_json, cd->emitted_json.size());
}

//-----------------------------------------------------------------------------
void YmlTestCase::_test_emit_yml_cout(CaseDataLineEndings *cd)
{
    if(c->flags & (EXPECT_PARSE_ERROR|HAS_CONTAINER_KEYS))
        return;
    _ensure_parse(cd);
    _ensure_emit(cd);
    std::cout << cd->parsed_tree;
    std::cout << cd->parsed_tree.rootref();
}

//-----------------------------------------------------------------------------
void YmlTestCase::_test_emit_json_cout(CaseDataLineEndings *cd)
{
    if(!(c->flags & JSON_WRITE))
        return;
    if(c->flags & (EXPECT_PARSE_ERROR|HAS_CONTAINER_KEYS))
        return;
    _ensure_parse(cd);
    _ensure_emit_json(cd);
    std::cout << as_json(cd->parsed_tree);
    std::cout << as_json(cd->parsed_tree.rootref());
}


//-----------------------------------------------------------------------------
void YmlTestCase::_test_emit_yml_stringstream(CaseDataLineEndings *cd)
{
    if(c->flags & (EXPECT_PARSE_ERROR|HAS_CONTAINER_KEYS))
        return;
    _ensure_parse(cd);
    _ensure_emit(cd);
    {
        std::stringstream ss;
        ss << cd->parsed_tree;
        std::string actual = ss.str();
        EXPECT_EQ(actual, cd->emit_buf);
    }
    {
        std::stringstream ss;
        ss << cd->parsed_tree.rootref();
        std::string actual = ss.str();
        EXPECT_EQ(actual, cd->emit_buf);
    }
}

//-----------------------------------------------------------------------------
void YmlTestCase::_test_emit_json_stringstream(CaseDataLineEndings *cd)
{
    if(!(c->flags & JSON_WRITE))
        return;
    if(c->flags & (EXPECT_PARSE_ERROR|HAS_CONTAINER_KEYS))
        return;
    _ensure_parse(cd);
    _ensure_emit_json(cd);
    {
        std::stringstream ss;
        ss << as_json(cd->parsed_tree);
        std::string actual = ss.str();
        EXPECT_EQ(actual, cd->emitjson_buf);
    }
    {
        std::stringstream ss;
        ss << as_json(cd->parsed_tree.rootref());
        std::string actual = ss.str();
        EXPECT_EQ(actual, cd->emitjson_buf);
    }
}

//-----------------------------------------------------------------------------
void YmlTestCase::_test_emit_yml_ofstream(CaseDataLineEndings *cd)
{
    if(c->flags & (EXPECT_PARSE_ERROR|HAS_CONTAINER_KEYS))
        return;
    _ensure_parse(cd);
    _ensure_emit(cd);
    {
        auto fn = fs::tmpnam<std::string>();
        {
            std::ofstream f(fn, std::ios::binary);
            f << cd->parsed_tree;
        }
        auto actual = fs::file_get_contents<std::string>(fn.c_str());
        EXPECT_EQ(actual, cd->emit_buf);
        fs::rmfile(fn.c_str());
    }
    {
        auto fn = fs::tmpnam<std::string>();
        {
            std::ofstream f(fn, std::ios::binary);
            f << cd->parsed_tree.rootref();
        }
        auto actual = fs::file_get_contents<std::string>(fn.c_str());
        EXPECT_EQ(actual, cd->emit_buf);
        fs::rmfile(fn.c_str());
    }
}

//-----------------------------------------------------------------------------
void YmlTestCase::_test_emit_json_ofstream(CaseDataLineEndings *cd)
{
    if(!(c->flags & JSON_WRITE))
        return;
    if(c->flags & (EXPECT_PARSE_ERROR|HAS_CONTAINER_KEYS))
        return;
    _ensure_parse(cd);
    _ensure_emit_json(cd);
    {
        auto fn = fs::tmpnam<std::string>();
        {
            std::ofstream f(fn, std::ios::binary);
            f << as_json(cd->parsed_tree);
        }
        auto actual = fs::file_get_contents<std::string>(fn.c_str());
        EXPECT_EQ(actual, cd->emitjson_buf);
        fs::rmfile(fn.c_str());
    }
    {
        auto fn = fs::tmpnam<std::string>();
        {
            std::ofstream f(fn, std::ios::binary);
            f << as_json(cd->parsed_tree.rootref());
        }
        auto actual = fs::file_get_contents<std::string>(fn.c_str());
        EXPECT_EQ(actual, cd->emitjson_buf);
        fs::rmfile(fn.c_str());
    }
}

//-----------------------------------------------------------------------------
void YmlTestCase::_test_emit_yml_string(CaseDataLineEndings *cd)
{
    if(c->flags & (EXPECT_PARSE_ERROR|HAS_CONTAINER_KEYS))
        return;
    _ensure_parse(cd);
    _ensure_emit(cd);
    csubstr emitted = emitrs_yaml(cd->parsed_tree, &cd->emit_buf);
    EXPECT_EQ(emitted.len, cd->emit_buf.size());
    EXPECT_EQ(emitted.len, cd->numbytes_stdout);
    #ifdef RYML_DBG
    printf("%.*s", (int)emitted.len, emitted.str);
    #endif
}

//-----------------------------------------------------------------------------
void YmlTestCase::_test_emit_json_string(CaseDataLineEndings *cd)
{
    if(!(c->flags & JSON_WRITE))
        return;
    if(c->flags & (EXPECT_PARSE_ERROR|HAS_CONTAINER_KEYS))
        return;
    _ensure_parse(cd);
    _ensure_emit_json(cd);
    auto emitted = emitrs_json(cd->parsed_tree, &cd->emit_buf);
    EXPECT_EQ(emitted.len, cd->emitjson_buf.size());
    EXPECT_EQ(emitted.len, cd->numbytes_stdout_json);
    #ifdef RYML_DBG
    printf("%.*s", (int)emitted.len, emitted.str);
    #endif
}

//-----------------------------------------------------------------------------
void YmlTestCase::_test_emitrs(CaseDataLineEndings *cd)
{
    if(c->flags & (EXPECT_PARSE_ERROR|HAS_CONTAINER_KEYS))
        return;
    _ensure_parse(cd);
    using vtype = std::vector<char>;
    using stype = std::string;
    vtype vv, v = emitrs_yaml<vtype>(cd->parsed_tree);
    stype ss, s = emitrs_yaml<stype>(cd->parsed_tree);
    EXPECT_EQ(to_csubstr(v), to_csubstr(s));
    csubstr svv = emitrs_yaml(cd->parsed_tree, &vv);
    csubstr sss = emitrs_yaml(cd->parsed_tree, &ss);
    EXPECT_EQ(svv, sss);
}

//-----------------------------------------------------------------------------
void YmlTestCase::_test_emitrs_json(CaseDataLineEndings *cd)
{
    if(!(c->flags & JSON_WRITE))
        return;
    if(c->flags & (EXPECT_PARSE_ERROR|HAS_CONTAINER_KEYS))
        return;
    _ensure_parse(cd);
    using vtype = std::vector<char>;
    using stype = std::string;
    vtype vv, v = emitrs_json<vtype>(cd->parsed_tree);
    stype ss, s = emitrs_json<stype>(cd->parsed_tree);
    EXPECT_EQ(to_csubstr(v), to_csubstr(s));
    csubstr svv = emitrs_json(cd->parsed_tree, &vv);
    csubstr sss = emitrs_json(cd->parsed_tree, &ss);
    EXPECT_EQ(svv, sss);
}

//-----------------------------------------------------------------------------
void YmlTestCase::_test_emitrs_cfile(CaseDataLineEndings *cd)
{
    if(c->flags & (EXPECT_PARSE_ERROR|HAS_CONTAINER_KEYS))
        return;
    _ensure_parse(cd);
    auto s = emitrs_yaml<std::string>(cd->parsed_tree);
    std::string r;
    {
        c4::fs::ScopedTmpFile f;
        emit_yaml(cd->parsed_tree, f.m_file);
        fflush(f.m_file);
        r = f.contents<std::string>();
    }
    EXPECT_EQ(s, r);
}

//-----------------------------------------------------------------------------
void YmlTestCase::_test_emitrs_json_cfile(CaseDataLineEndings *cd)
{
    if(!(c->flags & JSON_WRITE))
        return;
    if(c->flags & (EXPECT_PARSE_ERROR|HAS_CONTAINER_KEYS))
        return;
    _ensure_parse(cd);
    auto s = emitrs_json<std::string>(cd->parsed_tree);
    std::string r;
    {
        c4::fs::ScopedTmpFile f;
        emit_json(cd->parsed_tree, f.m_file);
        fflush(f.m_file);
        r = f.contents<std::string>();
    }
    EXPECT_EQ(s, r);
}


//-----------------------------------------------------------------------------
void YmlTestCase::_test_complete_round_trip(CaseDataLineEndings *cd)
{
    if(c->flags & (EXPECT_PARSE_ERROR|HAS_CONTAINER_KEYS|EXPECT_RESOLVE_ERROR))
        return;
    _ensure_parse(cd);
    _ensure_emit(cd);
    #ifdef RYML_DBG
    printf("~~~~~~~~~~~~~~ emitted yml:\n");
    _c4presc(cd->emitted_yml, /*keep_newlines*/true);
    printf("~~~~~~~~~~~~~~\n");
    #endif
    {
        SCOPED_TRACE("parsing emitted yml");
        cd->parse_buf = cd->emit_buf;
        cd->parsed_yml = to_substr(cd->parse_buf);
        parse_in_place(c->fileline, cd->parsed_yml, &cd->emitted_tree);
    }
    #ifdef RYML_DBG
    printf("~~~~~~~~~~~~~~ src yml:\n");
    _c4presc(cd->src, /*keep_newlines*/true);
    printf("~~~~~~~~~~~~~~ parsed tree:\n");
    print_tree(cd->parsed_tree);
    printf("~~~~~~~~~~~~~~ emitted yml:\n");
    _c4presc(cd->emitted_yml, /*keep_newlines*/true);
    printf("~~~~~~~~~~~~~~ emitted tree:\n");
    print_tree(cd->emitted_tree);
    printf("~~~~~~~~~~~~~~\n");
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
        // scalars. FIXME!
        c->root.compare(cd->emitted_tree.rootref(), true);
    }
}


//-----------------------------------------------------------------------------
void YmlTestCase::_test_complete_round_trip_json(CaseDataLineEndings *cd)
{
    if(!(c->flags & JSON_WRITE))
        return;
    if(c->flags & (EXPECT_PARSE_ERROR|HAS_CONTAINER_KEYS))
        return;
    _ensure_parse(cd);
    _ensure_emit_json(cd);
    #ifdef RYML_DBG
    printf("~~~~~~~~~~~~~~ emitted json:\n");
    _c4presc(cd->emitted_json);
    printf("~~~~~~~~~~~~~~\n");
    #endif
    {
        SCOPED_TRACE("parsing emitted json");
        cd->parse_buf_json = cd->emitjson_buf;
        cd->parsed_json = to_substr(cd->parse_buf_json);
        parse_json_in_place(c->fileline, cd->parsed_json, &cd->emitted_tree_json);
    }
    #ifdef RYML_DBG
    printf("~~~~~~~~~~~~~~ src yml:\n");
    _c4presc(cd->src);
    printf("~~~~~~~~~~~~~~ parsed tree:\n");
    print_tree(cd->parsed_tree);
    printf("~~~~~~~~~~~~~~ emitted json:\n");
    _c4presc(cd->emitted_json);
    printf("~~~~~~~~~~~~~~ emitted json tree:\n");
    print_tree(cd->emitted_tree_json);
    printf("~~~~~~~~~~~~~~\n");
    #endif
    {
        SCOPED_TRACE("checking node invariants of emitted tree");
        test_invariants(cd->parsed_tree.rootref());
    }
    {
        SCOPED_TRACE("checking node invariants of emitted json tree");
        test_invariants(cd->emitted_tree_json.rootref());
    }
    {
        SCOPED_TRACE("comparing emitted json and parsed tree");
        test_compare(cd->emitted_tree_json, cd->parsed_tree);
    }
    {
        SCOPED_TRACE("checking tree invariants of emitted json tree");
        test_invariants(cd->emitted_tree_json);
    }
    {
        SCOPED_TRACE("comparing parsed tree to ref tree");
        EXPECT_GE(cd->parsed_tree.capacity(), c->root.reccount());
        EXPECT_EQ(cd->parsed_tree.size(), c->root.reccount());
        c->root.compare(cd->parsed_tree.rootref());
    }
    {
        SCOPED_TRACE("comparing emitted tree to ref tree");
        EXPECT_GE(cd->emitted_tree_json.capacity(), c->root.reccount());
        EXPECT_EQ(cd->emitted_tree_json.size(), c->root.reccount());
        // in this case, we can ignore whether scalars are quoted.
        // Because it can happen that a scalar was quoted in the
        // original file, but the re-emitted data does not quote the
        // scalars.
        c->root.compare(cd->emitted_tree_json.rootref(), true);
    }
}

//-----------------------------------------------------------------------------
void YmlTestCase::_test_recreate_from_ref(CaseDataLineEndings *cd)
{
    if(c->flags & (EXPECT_PARSE_ERROR|HAS_CONTAINER_KEYS))
        return;
    if(cd->parsed_tree.empty())
        parse_in_place(c->fileline, cd->src, &cd->parsed_tree);
    if(cd->emit_buf.empty())
        cd->emitted_yml = emitrs_yaml(cd->parsed_tree, &cd->emit_buf);
    {
        SCOPED_TRACE("recreating a new tree from the ref tree");
        cd->recreated.reserve(cd->parsed_tree.size());
        NodeRef r = cd->recreated.rootref();
        c->root.recreate(&r);
    }
    #ifdef RYML_DBG
    printf("REF TREE:\n");
    print_test_tree(c->root);
    printf("RECREATED TREE:\n");
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

} // namespace yml
} // namespace c4
