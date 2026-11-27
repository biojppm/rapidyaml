#include "./test_emits_common.hpp"

#ifndef RYML_SINGLE_HEADER
#include "c4/yml/parse.hpp"
C4_SUPPRESS_WARNING_PUSH
#if defined(__GNUC__) && (__GNUC__ >= 6)
C4_SUPPRESS_WARNING_GCC("-Wnull-dereference")
#endif
#include "c4/yml/std/std.hpp"
C4_SUPPRESS_WARNING_POP
#include "c4/yml/emit.hpp"
#include <c4/format.hpp>
#include <c4/yml/detail/checks.hpp>
#include <c4/yml/detail/print.hpp>
#include <c4/yml/writer.hpp>
#endif
#include <c4/fs/fs.hpp>

#include "./test_lib/tree_and_ints.hpp"

#include <gtest/gtest.h>


namespace c4 {
namespace yml {

// NOLINTBEGIN(*-signed-bitwise)

namespace {
struct tmpfile
{
    std::string filename{};
    FILE *f{};
    ~tmpfile() { clear(); }
    C4_NO_INLINE void clear()
    {
        if(f)
        {
            (void)fclose(f); //NOLINT
            f = nullptr;
        }
        if(!filename.empty())
        {
           if(fs::file_exists(filename.c_str()))
           {
               fs::rmfile(filename.c_str());
           }
           filename.clear();
        }
    }
    template<class Emit>
    C4_NO_INLINE std::string execget(Emit &&fn)
    {
        C4_SUPPRESS_WARNING_MSVC_WITH_PUSH(4996) // fopen unsafe
        C4_SUPPRESS_WARNING_CLANG_WITH_PUSH("-Wdeprecated-declarations") // fopen is deprecated
        static uint32_t count = 0, countopen = 0;
        ++count;
        ++countopen;
        clear();
        filename = fs::tmpnam<std::string>("_c4fs_tmpname_XXXXXXXX.test_emit.tmp");
        const char *name = filename.c_str();
        f = fopen(name, "wb"); // NOLINT
        C4_CHECK_MSG(f != nullptr, "count=%u open=%u\nbase=%s\ndir=%s", count, countopen, name, fs::cwd<std::string>().c_str());
        std::forward<Emit>(fn)(f);
        (void)fflush(f); //NOLINT
        (void)fclose(f); //NOLINT
        f = nullptr;
        --countopen;
        std::string result = fs::file_get_contents<std::string>(name);
        fs::rmfile(filename.c_str());
        C4_CHECK_MSG(!fs::file_exists(name), "count=%u open=%u\nbase=%s\ndir=%s", count, countopen, name, fs::cwd<std::string>().c_str());
        filename = {};
        return result;
        C4_SUPPRESS_WARNING_CLANG_POP
        C4_SUPPRESS_WARNING_MSVC_POP
    }
};
tmpfile tmpf_ = {};
} // namespace


template<class Emit>
C4_NO_INLINE std::string emit2file(Emit &&fn)
{
    SCOPED_TRACE("emit2file");
    std::string result = tmpf_.execget(std::forward<Emit>(fn));
    _c4dbgpf("emit result: [{}]~~~{}~~~", result.size(), to_csubstr(result));
    tmpf_.clear();
    return result;
}

template<class Emit>
C4_NO_INLINE std::string emit2stream(Emit &&fn)
{
    SCOPED_TRACE("emit2stream");
    std::ostringstream ss;
    std::forward<Emit>(fn)(ss);
    std::string result = ss.str();
    _c4dbgpf("emit result: [{}]~~~{}~~~", result.size(), to_csubstr(result));
    return result;
}

template<class Emit>
C4_NO_INLINE std::string emit2buf(Emit &&fn)
{
    SCOPED_TRACE("emit2buf");
    std::string buf;
    buf.resize(2048);
    substr out = std::forward<Emit>(fn)(to_substr(buf));
    buf.resize(out.len);
    if(out.len > buf.size())
        out = fn(to_substr(buf));
    _c4dbgpf("emit result: [{}]~~~{}~~~", buf.size(), to_csubstr(buf));
    return buf;
}

template<class Emit>
C4_NO_INLINE std::string emitrs_append(csubstr first_part, Emit &&fn)
{
    SCOPED_TRACE("emitrs_append");
    std::string buf(first_part.begin(), first_part.end());
    std::forward<Emit>(fn)(&buf);
    _c4dbgpf("emit result: [{}]~~~{}~~~", buf.size(), to_csubstr(buf));
    return buf;
}


#define retonfail() do { if(testing::Test::HasFailure()) return; } while(0)
#define bailonfail() do { if(testing::Test::HasFailure()) goto failure; /* NOLINT */ } while(0)
C4_NO_INLINE void test_emits_tree(Tree const* tree, id_type id, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts)
{
    retonfail();
    RYML_TRACE_FMT("id={}", id);
    std::string append_prefix = "#before\n";
    ConstNodeRef node{tree, id};
    ASSERT_EQ(node.id(), id);
    ASSERT_EQ(node.tree(), tree);
    bool isroot = id == NONE || (!tree->empty() && id == tree->root_id());
    if(!tree)
    {
        ASSERT_EQ(id, NONE);
        if(opts == EmitOptions{})
        {
            EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(EmitOptions{}, buf); em.emit_as(EMIT_YAML, tree); return em.get_result(/*error_on_excess*/true); }), expected_yaml);
            bailonfail();
            EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(EmitOptions{}, buf); em.emit_as(EMIT_JSON, tree); return em.get_result(/*error_on_excess*/true); }), expected_json);
            EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(EmitOptions{}, f); em.emit_as(EMIT_YAML, tree); }), expected_yaml);
            EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(EmitOptions{}, f); em.emit_as(EMIT_JSON, tree); }), expected_json);
            EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em(EmitOptions{}, &oss); em.emit_as(EMIT_YAML, tree); }), expected_yaml);;
            EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em(EmitOptions{}, &oss); em.emit_as(EMIT_JSON, tree); }), expected_json);;
            EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_yaml(node); }), expected_yaml);
            EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_json(node); }), expected_json);
        }
        EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(opts, buf); em.emit_as(EMIT_YAML, tree); return em.get_result(/*error_on_excess*/true); }), expected_yaml);
        bailonfail();
        EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(opts, buf); em.emit_as(EMIT_JSON, tree); return em.get_result(/*error_on_excess*/true); }), expected_json);
        EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(opts, f); em.emit_as(EMIT_YAML, tree); }), expected_yaml);
        EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(opts, f); em.emit_as(EMIT_JSON, tree); }), expected_json);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em(opts, &oss); em.emit_as(EMIT_YAML, tree); }), expected_yaml);;
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em(opts, &oss); em.emit_as(EMIT_JSON, tree); }), expected_json);;
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_yaml(node, opts); }), expected_yaml);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_json(node, opts); }), expected_json);
        return;
    }
    if(opts == EmitOptions{})
    {
        EXPECT_EQ(emit2buf([&](substr buf){ return emit_yaml(*tree, id, buf); }), expected_yaml);
        bailonfail();
        EXPECT_EQ(emit2buf([&](substr buf){ return emit_json(*tree, id, buf); }), expected_json);
        bailonfail();
        EXPECT_EQ(emit2buf([&](substr buf){ return emit_yaml(node, buf); }), expected_yaml);
        EXPECT_EQ(emit2buf([&](substr buf){ return emit_json(node, buf); }), expected_json);
        EXPECT_EQ(emit2file([&](FILE *f){ return emit_yaml(*tree, id, f); }), expected_yaml);
        EXPECT_EQ(emit2file([&](FILE *f){ return emit_json(*tree, id, f); }), expected_json);
        EXPECT_EQ(emit2file([&](FILE *f){ return emit_yaml(node, f); }), expected_yaml);
        EXPECT_EQ(emit2file([&](FILE *f){ return emit_json(node, f); }), expected_json);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_yaml(*tree, id); }), expected_yaml);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_json(*tree, id); }), expected_json);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss <<         node; }),  expected_yaml);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_yaml(node); }), expected_yaml);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_json(node); }), expected_json);
        EXPECT_EQ(emitrs_yaml<std::string>(*tree, id), expected_yaml);
        EXPECT_EQ(emitrs_json<std::string>(*tree, id), expected_json);
        EXPECT_EQ(emitrs_yaml<std::string>(node), expected_yaml);
        EXPECT_EQ(emitrs_json<std::string>(node), expected_json);
        EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_yaml(*tree, id, s, /*append*/true); } ), append_prefix + expected_yaml);
        EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_json(*tree, id, s, /*append*/true); } ), append_prefix + expected_json);
        EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_yaml(node, s, /*append*/true); } ), append_prefix + expected_yaml);
        EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_json(node, s, /*append*/true); } ), append_prefix + expected_json);
        bailonfail();
        if(isroot)
        {
            EXPECT_EQ(emit2buf([&](substr buf){ return emit_yaml(*tree, buf); }), expected_yaml);
            bailonfail();
            EXPECT_EQ(emit2buf([&](substr buf){ return emit_json(*tree, buf); }), expected_json);
            bailonfail();
            EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(EmitOptions{}, buf); em.emit_as(EMIT_YAML, tree); return em.get_result(/*error_on_excess*/true); }), expected_yaml);
            EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(EmitOptions{}, buf); em.emit_as(EMIT_JSON, tree); return em.get_result(/*error_on_excess*/true); }), expected_json);
            EXPECT_EQ(emit2file([&](FILE *f){ return emit_yaml(*tree, f); }), expected_yaml);
            EXPECT_EQ(emit2file([&](FILE *f){ return emit_json(*tree, f); }), expected_json);
            EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(EmitOptions{}, f); em.emit_as(EMIT_YAML, tree); }), expected_yaml);
            EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(EmitOptions{}, f); em.emit_as(EMIT_JSON, tree); }), expected_json);
            EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << *tree; }), expected_yaml);
            EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_yaml(*tree); }), expected_yaml);
            EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_json(*tree); }), expected_json);
            EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_yaml(node); }), expected_yaml);
            EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_json(node); }), expected_json);
            EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em(EmitOptions{}, &oss); em.emit_as(EMIT_YAML, tree); }), expected_yaml);;
            EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em(EmitOptions{}, &oss); em.emit_as(EMIT_JSON, tree); }), expected_json);;
            EXPECT_EQ(emitrs_yaml<std::string>(*tree), expected_yaml);
            EXPECT_EQ(emitrs_json<std::string>(*tree), expected_json);
            EXPECT_EQ(emitrs_yaml<std::string>(node), expected_yaml);
            EXPECT_EQ(emitrs_json<std::string>(node), expected_json);
            EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_yaml(*tree, s, /*append*/true); } ), append_prefix + expected_yaml);
            EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_json(*tree, s, /*append*/true); } ), append_prefix + expected_json);
            bailonfail();
        }
    }
    EXPECT_EQ(emit2buf([&](substr buf){ return emit_yaml(*tree, id, opts, buf); }), expected_yaml);
    bailonfail();
    EXPECT_EQ(emit2buf([&](substr buf){ return emit_json(*tree, id, opts, buf); }), expected_json);
    bailonfail();
    EXPECT_EQ(emit2buf([&](substr buf){ return emit_yaml(node, opts, buf); }), expected_yaml);
    EXPECT_EQ(emit2buf([&](substr buf){ return emit_json(node, opts, buf); }), expected_json);
    EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(opts, buf); em.emit_as(EMIT_YAML, tree, id); return em.get_result(/*error_on_excess*/true); }), expected_yaml);
    EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(opts, buf); em.emit_as(EMIT_JSON, tree, id); return em.get_result(/*error_on_excess*/true); }), expected_json);
    EXPECT_EQ(emit2file([&](FILE *f){ return emit_yaml(*tree, id, opts, f); }), expected_yaml);
    EXPECT_EQ(emit2file([&](FILE *f){ return emit_json(*tree, id, opts, f); }), expected_json);
    EXPECT_EQ(emit2file([&](FILE *f){ return emit_yaml(node, opts, f); }), expected_yaml);
    EXPECT_EQ(emit2file([&](FILE *f){ return emit_json(node, opts, f); }), expected_json);
    EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(opts, f); em.emit_as(EMIT_YAML, tree, id); }), expected_yaml);
    EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(opts, f); em.emit_as(EMIT_JSON, tree, id); }), expected_json);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_yaml(*tree, id, opts); }), expected_yaml);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_json(*tree, id, opts); }), expected_json);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_yaml(node, opts); }), expected_yaml);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_json(node, opts); }), expected_json);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em(opts, &oss); em.emit_as(EMIT_YAML, tree, id); }), expected_yaml);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em(opts, &oss); em.emit_as(EMIT_JSON, tree, id); }), expected_json);
    EXPECT_EQ(emitrs_yaml<std::string>(*tree, id, opts), expected_yaml);
    EXPECT_EQ(emitrs_json<std::string>(*tree, id, opts), expected_json);
    EXPECT_EQ(emitrs_yaml<std::string>(node, opts), expected_yaml);
    EXPECT_EQ(emitrs_json<std::string>(node, opts), expected_json);
    EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_yaml(*tree, id, opts, s, /*append*/true); } ), append_prefix + expected_yaml);
    EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_json(*tree, id, opts, s, /*append*/true); } ), append_prefix + expected_json);
    EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_yaml(node, opts, s, /*append*/true); } ), append_prefix + expected_yaml);
    EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_json(node, opts, s, /*append*/true); } ), append_prefix + expected_json);
    bailonfail();
    if(isroot)
    {
        EXPECT_EQ(emit2buf([&](substr buf){ return emit_yaml(*tree, opts, buf); }), expected_yaml);
        bailonfail();
        EXPECT_EQ(emit2buf([&](substr buf){ return emit_json(*tree, opts, buf); }), expected_json);
        bailonfail();
        EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(opts, buf); em.emit_as(EMIT_YAML, tree); return em.get_result(/*error_on_excess*/true); }), expected_yaml);
        EXPECT_EQ(emit2buf([&](substr buf){ EmitterBuf em(opts, buf); em.emit_as(EMIT_JSON, tree); return em.get_result(/*error_on_excess*/true); }), expected_json);
        EXPECT_EQ(emit2file([&](FILE *f){ return emit_yaml(*tree, opts, f); }), expected_yaml);
        EXPECT_EQ(emit2file([&](FILE *f){ return emit_json(*tree, opts, f); }), expected_json);
        EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(opts, f); return em.emit_as(EMIT_YAML, tree); }), expected_yaml);
        EXPECT_EQ(emit2file([&](FILE *f){ EmitterFile em(opts, f); return em.emit_as(EMIT_JSON, tree); }), expected_json);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_yaml(*tree, opts); }), expected_yaml);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ oss << as_json(*tree, opts); }), expected_json);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em(opts, &oss); em.emit_as(EMIT_YAML, tree); }), expected_yaml);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ EmitterOStream<std::ostringstream> em(opts, &oss); em.emit_as(EMIT_JSON, tree); }), expected_json);
        EXPECT_EQ(emitrs_yaml<std::string>(*tree, opts), expected_yaml);
        EXPECT_EQ(emitrs_json<std::string>(*tree, opts), expected_json);
        EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_yaml(*tree, opts, s, /*append*/true); } ), append_prefix + expected_yaml);
        EXPECT_EQ(emitrs_append(to_csubstr(append_prefix), [&](std::string *s) { emitrs_json(*tree, opts, s, /*append*/true); } ), append_prefix + expected_json);
        bailonfail();
    }
    if(!tree->empty() && tree->depth_desc(id != NONE ? id : tree->root_id()) > 1)
    {
        // error on max depth
        EmitOptions optsd = opts;
        optsd = optsd.max_depth(0);
        RYML_EXPECT_ERROR(check_error_visit(tree, [&]{ return emit2buf([&](substr buf){ return emit_yaml(*tree, id, optsd, buf); }); }));
        bailonfail();
        RYML_EXPECT_ERROR(check_error_visit(tree, [&]{ return emit2buf([&](substr buf){ return emit_json(*tree, id, optsd, buf); }); }));
        bailonfail();
        RYML_EXPECT_ERROR(check_error_visit(tree, [&]{ return emit2buf([&](substr buf){ return emit_yaml(node, optsd, buf); }); }));
        RYML_EXPECT_ERROR(check_error_visit(tree, [&]{ return emit2buf([&](substr buf){ return emit_json(node, optsd, buf); }); }));
        RYML_EXPECT_ERROR(check_error_visit(tree, [&]{ return emit2file([&](FILE *f){ emit_yaml(*tree, id, optsd, f); }); }));
        RYML_EXPECT_ERROR(check_error_visit(tree, [&]{ return emit2file([&](FILE *f){ emit_json(*tree, id, optsd, f); }); }));
        RYML_EXPECT_ERROR(check_error_visit(tree, [&]{ return emit2file([&](FILE *f){ emit_yaml(node, optsd, f); }); }));
        RYML_EXPECT_ERROR(check_error_visit(tree, [&]{ return emit2file([&](FILE *f){ emit_json(node, optsd, f); }); }));
        RYML_EXPECT_ERROR(check_error_visit(tree, [&]{ return emit2stream([&](std::ostringstream &oss){ oss << as_yaml(*tree, id, optsd); }); }));
        RYML_EXPECT_ERROR(check_error_visit(tree, [&]{ return emit2stream([&](std::ostringstream &oss){ oss << as_json(*tree, id, optsd); }); }));
        RYML_EXPECT_ERROR(check_error_visit(tree, [&]{ return emit2stream([&](std::ostringstream &oss){ oss << as_yaml(node, optsd); }); }));
        RYML_EXPECT_ERROR(check_error_visit(tree, [&]{ return emit2stream([&](std::ostringstream &oss){ oss << as_json(node, optsd); }); }));
        RYML_EXPECT_ERROR(check_error_visit(tree, [&]{ return emitrs_yaml<std::string>(*tree, id, optsd); }));
        RYML_EXPECT_ERROR(check_error_visit(tree, [&]{ return emitrs_json<std::string>(*tree, id, optsd); }));
        RYML_EXPECT_ERROR(check_error_visit(tree, [&]{ return emitrs_yaml<std::string>(node, optsd); }));
        RYML_EXPECT_ERROR(check_error_visit(tree, [&]{ return emitrs_json<std::string>(node, optsd); }));
        bailonfail();
        if(id == NONE || id == tree->root_id())
        {
            RYML_EXPECT_ERROR(check_error_visit(tree, [&]{ return emit2buf([&](substr buf){ return emit_yaml(*tree, optsd, buf); }); }));
            bailonfail();
            RYML_EXPECT_ERROR(check_error_visit(tree, [&]{ return emit2buf([&](substr buf){ return emit_json(*tree, optsd, buf); }); }));
            bailonfail();
            RYML_EXPECT_ERROR(check_error_visit(tree, [&]{ return emit2file([&](FILE *f){ emit_yaml(*tree, optsd, f); }); }));
            RYML_EXPECT_ERROR(check_error_visit(tree, [&]{ return emit2file([&](FILE *f){ emit_json(*tree, optsd, f); }); }));
            RYML_EXPECT_ERROR(check_error_visit(tree, [&]{ return emit2stream([&](std::ostringstream &oss){ oss << as_yaml(ConstNodeRef(tree), optsd); }); }));
            RYML_EXPECT_ERROR(check_error_visit(tree, [&]{ return emit2stream([&](std::ostringstream &oss){ oss << as_json(ConstNodeRef(tree), optsd); }); }));
            RYML_EXPECT_ERROR(check_error_visit(tree, [&]{ return emitrs_yaml<std::string>(*tree, optsd); }));
            RYML_EXPECT_ERROR(check_error_visit(tree, [&]{ return emitrs_json<std::string>(*tree, optsd); }));
            bailonfail();
        }
    }
    tmpf_.clear();
    return;
failure:
    print_tree(tree);
    tmpf_.clear();
}
void test_emits_tree(ConstNodeRef node, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts)
{
    test_emits_tree(node.tree(), node.id(), expected_yaml, expected_json, opts);
}


//-----------------------------------------------------------------------------

C4_NO_INLINE uint32_t getdepth(evt_bits const* evts, evt_size pos)
{
    using namespace extra;
    uint32_t count = 0;
    uint32_t maxcount = 0;
    while(evts[pos] != ievt::ESTR)
    {
        evt_bits evt = evts[pos];
        if(evt & (ievt::SEQ_|ievt::MAP_))
        {
            if(evt & ievt::BEG_)
            {
                if(++count > maxcount)
                    maxcount = count;
            }
            else if(evt & ievt::END_)
            {
                if(count)
                    --count;
                else
                    break;
            }
        }
        pos = ievt::nextpos(evts, pos);
    }
    return maxcount;
}
C4_NO_INLINE void test_emits_ints(IntBufsCR ints, evt_size pos, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts, bool with_json)
{
    uint32_t depth = 0;
    retonfail();
    RYML_TRACE_FMT("startpos={}", pos);
    EXPECT_EQ(ints.emitrs_yaml<std::string>(opts, pos), expected_yaml);
    bailonfail();
    EXPECT_EQ(emit2buf([&](substr buf){ size_t sz = ints.emit_yaml(buf, opts, pos); buf.str = nullptr; buf.len = sz ; return buf; }), expected_yaml);
    EXPECT_EQ(emit2file([&](FILE *f){ return ints.emit_yaml(f, opts, pos); }), expected_yaml);
    EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ ints.emit_yaml_stream(oss, opts, pos); }), expected_yaml);
    bailonfail();
    depth = getdepth(ints.evts.ptr, pos);
    if(depth > 1)
    {
        EmitOptions optsd = opts;
        optsd = optsd.max_depth(0);
        RYML_TRACE_FMT("depth={}", depth);
        RYML_EXPECT_ERROR(check_error_basic([&]{ ints.emitrs_yaml<std::string>(optsd, pos); }));
        bailonfail();
        RYML_EXPECT_ERROR(check_error_basic([&]{ emit2buf([&](substr buf){ size_t sz = ints.emit_yaml(buf, optsd, pos); buf.str = nullptr; buf.len = sz ; return buf; }); }));
        RYML_EXPECT_ERROR(check_error_basic([&]{ emit2file([&](FILE *f){ return ints.emit_yaml(f, optsd, pos); }); }));
        RYML_EXPECT_ERROR(check_error_basic([&]{ emit2stream([&](std::ostringstream &oss){ ints.emit_yaml_stream(oss, optsd, pos); }); }));
    }
    if(with_json)
    {
        EXPECT_EQ(ints.emitrs_json<std::string>(opts, pos), expected_json);
        bailonfail();
        EXPECT_EQ(emit2buf([&](substr buf){ size_t sz = ints.emit_json(buf, opts, pos); buf.str = nullptr; buf.len = sz ; return buf; }), expected_json);
        EXPECT_EQ(emit2file([&](FILE *f){ return ints.emit_json(f, opts, pos); }), expected_json);
        EXPECT_EQ(emit2stream([&](std::ostringstream &oss){ ints.emit_json_stream(oss, opts, pos); }), expected_json);
        if(depth > 1)
        {
            EmitOptions optsd = opts;
            optsd = optsd.max_depth(0);
            RYML_TRACE_FMT("depth={}", depth);
            RYML_EXPECT_ERROR(check_error_basic([&]{ ints.emitrs_json<std::string>(optsd, pos); }));
            bailonfail();
            RYML_EXPECT_ERROR(check_error_basic([&]{ emit2buf([&](substr buf){ size_t sz = ints.emit_json(buf, optsd, pos); buf.str = nullptr; buf.len = sz ; return buf; }); }));
            RYML_EXPECT_ERROR(check_error_basic([&]{ emit2file([&](FILE *f){ return ints.emit_json(f, optsd, pos); }); }));
            RYML_EXPECT_ERROR(check_error_basic([&]{ emit2stream([&](std::ostringstream &oss){ ints.emit_json_stream(oss, optsd, pos); }); }));
        }
    }
    tmpf_.clear();
    return;
failure:
    tmpf_.clear();
    ints.print();
}
void test_emits_ints_nojson(IntBufsCR ints, evt_size pos, std::string const& expected_yaml, EmitOptions const& opts)
{
    test_emits_ints(ints, pos, expected_yaml, "", opts, false);
}

void test_emits_ints(evt_size pos, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts)
{
    retonfail();
    IntBufs ints;
    std::string yaml = expected_yaml;
    parse_ints(to_substr(yaml), &ints);
    test_emits_ints(ints, pos, expected_yaml, expected_json, opts);
}

void test_emits(Tree const& t, id_type id, evt_size pos, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts)
{
    test_emits_tree(&t, id, expected_yaml, expected_json, opts);
    test_emits_ints(pos, expected_yaml, expected_json, opts);
}
void test_emits(TreeAndInts const& ti, id_type id, evt_size pos, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts)
{
    test_emits_tree(&ti.tree, id, expected_yaml, expected_json, opts);
    test_emits_ints(ti.ints, pos, expected_yaml, expected_json, opts);
}
void test_emits(Tree const& t, id_type id, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts)
{
    test_emits_tree(&t, id, expected_yaml, expected_json, opts);
    if(id == NONE || id == 0)
        test_emits_ints(0, expected_yaml, expected_json, opts);
}
void test_emits(TreeAndInts const& ti, id_type id, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts)
{
    test_emits_tree(&ti.tree, id, expected_yaml, expected_json, opts);
    if(id == NONE || id == 0)
        test_emits_ints(ti.ints, 0, expected_yaml, expected_json, opts);
}

void test_emits(Tree const& t, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts)
{
    if(t.empty())
        test_emits_tree(&t, NONE, expected_yaml, expected_json, opts);
    else
        test_emits_tree(&t, t.root_id(), expected_yaml, expected_json, opts);
    test_emits_ints(0, expected_yaml, expected_json, opts);
}
void test_emits(TreeAndInts const& ti, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts)
{
    if(ti.tree.empty())
        test_emits_tree(&ti.tree, NONE, expected_yaml, expected_json, opts);
    else
        test_emits_tree(&ti.tree, ti.tree.root_id(), expected_yaml, expected_json, opts);
    test_emits_ints(ti.ints, 0, expected_yaml, expected_json, opts);
}

void test_emits(ConstNodeRef node, evt_size pos, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts)
{
    test_emits_tree(node.tree(), node.id(), expected_yaml, expected_json, opts);
    test_emits_ints(pos, expected_yaml, expected_json, opts);
}
void test_emits(TreeAndInts const& ti, ConstNodeRef node, evt_size pos, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts)
{
    ASSERT_EQ(node.tree(), &ti.tree);
    test_emits_tree(node.tree(), node.id(), expected_yaml, expected_json, opts);
    test_emits_ints(ti.ints, pos, expected_yaml, expected_json, opts);
}
void test_emits(ConstNodeRef node, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts)
{
    test_emits_tree(node.tree(), node.id(), expected_yaml, expected_json, opts);
    if(node.id() == NONE || (node.tree() && node.id() == node.tree()->root_id_maybe()))
        test_emits_ints(0, expected_yaml, expected_json, opts);
}
void test_emits(TreeAndInts const& ti, ConstNodeRef node, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts)
{
    ASSERT_EQ(node.tree(), &ti.tree);
    test_emits_tree(node.tree(), node.id(), expected_yaml, expected_json, opts);
    if(node.id() == NONE || (node.tree() && node.id() == node.tree()->root_id_maybe()))
        test_emits_ints(ti.ints, 0, expected_yaml, expected_json, opts);
}



//-----------------------------------------------------------------------------

TmpStyle::TmpStyle(Tree *tree_, IntBufs *ints_, id_type id_, NodeType tmp, evt_size evt_pos_, evt_bits evt,
                   bool with_key_, evt_size evt_pos_key_)
    : tree(tree_)
    , id(id_)
    , prev()
    , ints(ints_)
    , evt_pos(evt_pos_)
    , evt_prev()
    , with_key(with_key_)
    , evt_pos_key(evt_pos_key_)
    , evt_prev_key()
{
    if(tree)
    {
        prev = tree_->type(id);
        (tree->_p(id)->m_type &= ~(CONTAINER_STYLE|SCALAR_STYLE)) |= tmp;
    }
    if(ints)
    {
        RYML_ASSERT_BASIC_(evt_pos < ints->evts.len);
        evt_prev = ints->evts.ptr[evt_pos];
        (ints->evts.ptr[evt_pos] &= ~(xievt::detail::styles_ievt_cont|xievt::detail::styles_ievt_sclr)) |= evt; // NOLINT
        if(with_key)
        {
            evt_prev_key = ints->evts.ptr[evt_pos_key];
            (ints->evts.ptr[evt_pos_key] &= ~(xievt::detail::styles_ievt_cont|xievt::detail::styles_ievt_sclr)) |= evt; // NOLINT
        }
    }
}

TmpStyle::~TmpStyle()
{
    if(tree)
        tree->_p(id)->m_type = prev;
    if(ints)
    {
        ints->evts.ptr[evt_pos] = evt_prev;
        if(with_key) ints->evts.ptr[evt_pos_key] = evt_prev_key;
    }
}

// NOLINTEND(*-signed-bitwise)

} // namespace yml
} // namespace c4
