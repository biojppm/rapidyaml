#include "./test_engine.hpp"
#include "c4/yml/extra/ints_to_testsuite.hpp"



namespace c4 {
namespace yml {


#ifdef RYML_DBG
void _print_handler_info(EventHandlerTree const& ps, csubstr stmt, const char *file, int line)
{
    if(ps.m_parent)
        _dbg_printf("{}:{}: parent.id={} curr.id={}  {}\n",
                    file, line, ps.m_parent->node_id, ps.m_curr->node_id, stmt);
    else
        _dbg_printf("{}:{}: parent.id=-- curr.id={}  {}\n",
                    file, line, ps.m_curr->node_id, stmt);
    print_tree(*ps.m_tree);
}

void _print_handler_info(extra::EventHandlerInts const& ps, csubstr stmt, const char *file, int line)
{
    _dbg_printf("{}:{}: {}\n", file, line, stmt);
    (void)ps;
}
#endif


//-----------------------------------------------------------------------------

namespace {
struct IntsBuffers
{
    // prefer std::vector<char> to std::string because g++ 4.8
    // sometimes fails the assignment operator
    std::vector<char> src;
    std::vector<char> arena;
    std::vector<int>  ints;

public:

    void prepare_yaml(extra::EventHandlerInts &handler,
                      EngineEvtTestCase const& test_case, std::string const& parsed_yaml,
                      extra::ievt::DataType ints_size=-1, size_t arena_size=npos)
    {
        if(ints_size == -1)
            ints_size = extra::estimate_events_ints_size(to_csubstr(parsed_yaml));
        if(arena_size == npos)
            arena_size = test_case.expected_emitted.size();
        _c4dbgpf("ints: setting buffer sizes: src={} emitted={} ints={} arena={}", parsed_yaml.size(), test_case.expected_emitted.size(), ints_size, arena_size);
        ints.resize((size_t)ints_size);
        arena.resize(arena_size);
        src.assign(parsed_yaml.begin(), parsed_yaml.end());
        handler.reset(to_substr(src), to_substr(arena), ints.data(), (int)ints.size());
    }
    bool resize_post_yaml(extra::EventHandlerInts &handler, std::string const& parsed_yaml)
    {
        bool ret = false;
        ints.resize((size_t)handler.required_size_events());
        if(!handler.fits_buffers())
        {
            ints.resize((size_t)handler.required_size_events());
            arena.resize(handler.required_size_arena());
            src.assign(parsed_yaml.begin(), parsed_yaml.end());
            handler.reset(to_substr(src), to_substr(arena), ints.data(), (int)ints.size());
            ret = true;
        }
        return ret;
    }

public:

    void prepare_events(EventHandlerIntsTr &handler_tr,
                        EngineEvtTestCase const& test_case, std::string const& parsed_yaml,
                        extra::ievt::DataType ints_size=-1, size_t arena_size=npos)
    {
        prepare_yaml(handler_tr.handler, test_case, parsed_yaml, ints_size, arena_size);
        handler_tr.transformer.src = to_csubstr(src);
    }
    bool resize_post_events(EventHandlerIntsTr &events_tr, std::string const& parsed_yaml)
    {
        if(resize_post_yaml(events_tr.handler, parsed_yaml))
        {
            events_tr.transformer.src = to_csubstr(src);
            return true;
        }
        return false;
    }

public:

    void print(bool print_all=true) const
    {
        size_t sz = print_all ? ints.size() : num_ints();
        extra::events_ints_print(to_csubstr(src), to_csubstr(arena), ints.data(), (int)sz);
    }
    void test(const EngineEvtTestCase& test_case) const
    {
        {
            SCOPED_TRACE("test_invariants");
            extra::test_events_ints_invariants(to_csubstr(src), to_csubstr(arena), ints.data(), (int)ints.size());
        }
        if(test_case.expected_ints_enabled)
        {
            SCOPED_TRACE("compare_ints");
            extra::test_events_ints(test_case.expected_ints.data(), test_case.expected_ints.size(),
                                    ints.data(), ints.size(),
                                    to_csubstr(test_case.yaml),
                                    to_csubstr(src),
                                    to_csubstr(arena));
        }
        {
            std::string actual_testsuite_events = extra::events_ints_to_testsuite<std::string>(to_csubstr(src), to_csubstr(arena), ints.data(), (int)ints.size());
            _c4dbgpf("~~~\n{}~~~\n", actual_testsuite_events);
            test_compare_events(to_csubstr(test_case.expected_events),
                                to_csubstr(actual_testsuite_events),
                                /*ignore_doc_style*/false,
                                /*ignore_container_style*/false,
                                /*ignore_scalar_style*/false,
                                /*ignore_tag_normalization*/true);
        }
        if(testing::Test::HasFailure())
        {
            print();
        }
    }
    size_t num_ints() const
    {
        size_t sz = ints.size();
        for(size_t i = 0; i < sz; ++i)
        {
            if(ints[i] & extra::ievt::WSTR) // NOLINT
                i += 2; // NOLINT
            else if(!ints[i])
                return i;
        }
        return sz;
    }
};
} // namespace


void test_engine_error_ints_from_events(const EngineEvtTestCase& test_case, EventProducerInts event_producer)
{
    SCOPED_TRACE("error_ints_from_events");
    IntsBuffers buffers;
    ExpectError::check_error_parse([&]{
        EventHandlerIntsTr events_tr;
        buffers.prepare_events(events_tr, test_case, test_case.yaml, -1, test_case.expected_emitted.size());
        event_producer(events_tr);
    });
    if(testing::Test::HasFailure())
        buffers.print();
}

void test_engine_ints_from_events(EngineEvtTestCase const& test_case, EventProducerInts event_producer)
{
    SCOPED_TRACE("ints_from_events");
    EventHandlerIntsTr events_tr;
    IntsBuffers buffers;
    buffers.prepare_events(events_tr, test_case, test_case.yaml, -1, test_case.expected_emitted.size());
    event_producer(events_tr);
    if(buffers.resize_post_events(events_tr, test_case.yaml))
        event_producer(events_tr);
    ASSERT_TRUE(events_tr.handler.fits_buffers());
    buffers.test(test_case);
}

void test_expected_error_ints_from_yaml(EngineEvtTestCase const& test_case, ExpectedErrorType errtype)
{
    SCOPED_TRACE("error_ints_from_yaml");
    IntsBuffers buffers;
    ExpectError::check_error(errtype, [&]{
        extra::EventHandlerInts handler{};
        ParseEngine<extra::EventHandlerInts> parser(&handler, test_case.opts);
        buffers.prepare_yaml(handler, test_case, test_case.yaml);
        parser.parse_in_place_ev(test_case.fileline, to_substr(buffers.src));
    }, test_case.expected_error_location);
    if(testing::Test::HasFailure())
        buffers.print(/*all*/false);
}

void test_engine_ints_from_yaml(EngineEvtTestCase const& test_case, std::string const& parsed_yaml)
{
    SCOPED_TRACE("test_engine_ints_from_yaml");
    extra::EventHandlerInts handler{};
    ParseEngine<extra::EventHandlerInts> parser(&handler, test_case.opts);
    IntsBuffers buffers;
    int size_estimated = extra::estimate_events_ints_size(to_csubstr(parsed_yaml));
    int reqsz_evts = 0;
    size_t reqsz_arena = 0;
    {
        SCOPED_TRACE("empty buffers");
        // try first with empty buffers
        buffers.prepare_yaml(handler, test_case, parsed_yaml, 0, 0);
        parser.parse_in_place_ev(test_case.fileline, to_substr(buffers.src));
        EXPECT_GE(size_estimated, handler.required_size_events());
    }
    reqsz_evts = handler.required_size_events();
    reqsz_arena = handler.required_size_arena();
    {
        SCOPED_TRACE("small buffers");
        buffers.prepare_yaml(handler, test_case, parsed_yaml, reqsz_evts / 2, reqsz_arena / 2);
        parser.parse_in_place_ev(test_case.fileline, to_substr(buffers.src));
        EXPECT_EQ(handler.required_size_events(), reqsz_evts);
        EXPECT_EQ(handler.required_size_arena(), reqsz_arena);
    }
    if(test_case.expected_ints_enabled)
    {
        size_t size_reference = num_ints(test_case.expected_ints.data(), test_case.expected_ints.size());
        EXPECT_EQ(size_reference, handler.required_size_events());
    }
    EXPECT_TRUE(buffers.resize_post_yaml(handler, parsed_yaml));
    {
        SCOPED_TRACE("buffers ok");
        parser.parse_in_place_ev(test_case.fileline, to_substr(buffers.src));
        EXPECT_FALSE(buffers.resize_post_yaml(handler, parsed_yaml));
        EXPECT_EQ(handler.required_size_events(), reqsz_evts);
        EXPECT_EQ(handler.required_size_arena(), reqsz_arena);
        ASSERT_TRUE(handler.fits_buffers());
        buffers.test(test_case);
    }
}


//-----------------------------------------------------------------------------

void test_engine_error_tree_from_events(const EngineEvtTestCase& test_case, EventProducerTree event_producer)
{
    (void)test_case;
    Tree tree = {};
    ExpectError::check_error_parse(&tree, [&]{
        EventHandlerTree handler(&tree, tree.root_id());
        event_producer(handler);
    });
}

void test_engine_tree_from_events(EngineEvtTestCase const& test_case, EventProducerTree event_producer)
{
    if(test_case.test_case_flags & HAS_CONTAINER_KEYS)
    {
        Tree tree = {};
        ExpectError::check_error_parse(&tree, [&]{
            EventHandlerTree handler(&tree, tree.root_id());
            event_producer(handler);
        });
    }
    else
    {
        Tree tree = {};
        EventHandlerTree handler(&tree, tree.root_id());
        event_producer(handler);
        test_invariants(tree);
        #ifdef RYML_DBG
        print_tree(tree);
        #endif
        std::string actual = emitrs_yaml<std::string>(tree);
        _c4dbgpf("~~~\n{}~~~\n", actual);
        EXPECT_EQ(test_case.expected_emitted, actual);
    }
}

void test_expected_error_tree_from_yaml(EngineEvtTestCase const& test_case, ExpectedErrorType errtype)
{
    SCOPED_TRACE("test_expected_error_tree_from_yaml");
    if(test_case.test_case_flags & (HAS_CONTAINER_KEYS))
        return;
    else
    {
        ExpectError::check_error(errtype, [&]{
            Tree tree = {};
            std::vector<char> copy(test_case.yaml.begin(), test_case.yaml.end()); // g++ 4.8 fails with std::string
            EventHandlerTree handler(&tree, tree.root_id());
            ASSERT_EQ(&tree, handler.m_tree);
            ParseEngine<EventHandlerTree> parser(&handler, test_case.opts);
            ASSERT_EQ(&handler, parser.m_evt_handler);
            ASSERT_EQ(&tree, parser.m_evt_handler->m_tree);
            parser.parse_in_place_ev(test_case.fileline, to_substr(copy));
        }, test_case.expected_error_location);
    }
}

void test_engine_tree_from_yaml(EngineEvtTestCase const& test_case, std::string const& yaml)
{
    SCOPED_TRACE("test_engine_tree_from_yaml");
    if(test_case.test_case_flags & HAS_CONTAINER_KEYS)
    {
        test_expected_error_tree_from_yaml(test_case, ExpectedErrorType::err_parse);
        return;
    }
    Tree tree = {};
    EventHandlerTree handler(&tree, tree.root_id());
    ASSERT_EQ(&tree, handler.m_tree);
    ParseEngine<EventHandlerTree> parser(&handler, test_case.opts);
    ASSERT_EQ(&handler, parser.m_evt_handler);
    ASSERT_EQ(&tree, parser.m_evt_handler->m_tree);
    std::vector<char> copy(yaml.begin(), yaml.end()); // g++ 4.8 fails with std::string
    parser.parse_in_place_ev(test_case.fileline, to_substr(copy));
    #ifdef RYML_DBG
    print_tree("parsed_tree", tree);
    #endif
    std::string actual = emitrs_yaml<std::string>(tree);
    if(!(test_case.test_case_flags & NO_COMPARE_EMITTED))
    {
        _c4dbgpf("~~~\n{}~~~\n", actual);
        EXPECT_EQ(test_case.expected_emitted, actual);
    }
}


//-----------------------------------------------------------------------------

void test_engine_roundtrip_from_events(EngineEvtTestCase const& test_case, EventProducerTree event_producer)
{
    if(test_case.test_case_flags & HAS_CONTAINER_KEYS)
        return;
    SCOPED_TRACE("roundtrip_from_events");
    Tree event_tree = {};
    EventHandlerTree handler(&event_tree, event_tree.root_id());
    event_producer(handler);
    {
        SCOPED_TRACE("test_invariants_orig");
        test_invariants(event_tree);
    }
    const std::string emitted0 = emitrs_yaml<std::string>(event_tree);
    EXPECT_EQ(test_case.expected_emitted, emitted0);
    std::string copy = emitted0;
    Tree after_roundtrip = parse_in_place(test_case.fileline, to_substr(copy), test_case.opts);
    {
        SCOPED_TRACE("test_invariants_after_roundtrip");
        test_invariants(after_roundtrip);
    }
    {
        SCOPED_TRACE("compare_trees");
        test_compare(after_roundtrip, event_tree, "after_roundtrip", "event_tree");
    }
    std::string emitted1 = emitrs_yaml<std::string>(after_roundtrip);
    EXPECT_EQ(test_case.expected_emitted, emitted1);
    if(testing::Test::HasFailure())
    {
        printf("source: ~~~\n%.*s~~~\n", (int)test_case.yaml.size(), test_case.yaml.data());
        print_tree("event_tree", event_tree);
        printf("event_tree_emitted: ~~~\n%.*s~~~\n", (int)emitted0.size(), emitted0.data());
        print_tree("after_roundtrip", after_roundtrip);
        printf("after_roundtrip_emitted: ~~~\n%.*s~~~\n", (int)emitted1.size(), emitted1.data());
    }
}

void test_engine_roundtrip_from_yaml(EngineEvtTestCase const& test_case, std::string const& yaml)
{
    if(test_case.test_case_flags & HAS_CONTAINER_KEYS) // NOLINT
        return;
    SCOPED_TRACE("test_engine_roundtrip_from_yaml");
    std::string copy = yaml;
    const Tree parsed_tree = parse_in_place(test_case.fileline, to_substr(copy), test_case.opts);
    #ifdef RYML_DBG
    print_tree("parsed_tree", parsed_tree);
    #endif
    {
        SCOPED_TRACE("invariants_after_parse");
        test_invariants(parsed_tree);
    }
    const std::string parsed_tree_emitted = emitrs_yaml<std::string>(parsed_tree);
    if(!(test_case.test_case_flags & NO_COMPARE_EMITTED))
    {
        EXPECT_EQ(test_case.expected_emitted, parsed_tree_emitted);
    }
    std::string emitted0_copy = parsed_tree_emitted;
    const Tree after_roundtrip = parse_in_place(test_case.fileline, to_substr(emitted0_copy), test_case.opts);
    {
        SCOPED_TRACE("invariants_after_roundtrip");
        test_invariants(after_roundtrip);
    }
    {
        SCOPED_TRACE("compare_trees");
        test_compare(after_roundtrip, parsed_tree,
                     "after_roundtrip", "parsed_tree");
    }
    const std::string after_roundtrip_emitted = emitrs_yaml<std::string>(after_roundtrip);
    if(!(test_case.test_case_flags & NO_COMPARE_EMITTED))
    {
        EXPECT_EQ(test_case.expected_emitted, after_roundtrip_emitted);
    }
    if(testing::Test::HasFailure())
    {
        printf("source: ~~~\n%.*s~~~\n", (int)yaml.size(), yaml.data());
        print_tree("parsed_tree", parsed_tree);
        printf("parsed_tree_emitted: ~~~\n%.*s~~~\n", (int)parsed_tree_emitted.size(), parsed_tree_emitted.data());
        print_tree("after_roundtrip", after_roundtrip);
        printf("after_roundtrip_emitted: ~~~\n%.*s~~~\n", (int)after_roundtrip_emitted.size(), after_roundtrip_emitted.data());
    }
}


//-----------------------------------------------------------------------------

namespace {
// inject comments on every line
std::vector<std::string> inject_comments_in_src(std::string const& src_)
{
    std::vector<std::string> result;
    csubstr src = to_csubstr(src_);
    csubstr comment = "   # this is a comment\n";
    // inject a comment before the contents
    {
        std::string curr;
        curr.append(comment.str, comment.len);
        curr.append(src.str, src.len);
        result.emplace_back(std::move(curr));
    }
    // inject a comment after each newline
    size_t pos = src.find('\n');
    do
    {
        csubstr before = src.first(pos);
        csubstr after = pos != npos ? src.sub(pos) : src.last(0);
        std::string curr;
        curr.append(before.str, before.len);
        curr += '\n';
        curr.append(comment.str, comment.len);
        curr.append(after.str, after.len);
        result.emplace_back(std::move(curr));
        pos = src.find('\n', pos+1);
    } while(pos != npos);
    return result;
}
} // anon

void test_engine_ints_from_yaml_with_comments(EngineEvtTestCase const& test_case)
{
    SCOPED_TRACE("test_engine_ints_from_yaml_with_comments");
    if(test_case.test_case_flags & HAS_MULTILINE_SCALAR)
        return;
    const auto injected_comment_cases = inject_comments_in_src(test_case.yaml);
    for(size_t i = 0; i < injected_comment_cases.size(); ++i)
    {
        const std::string& transformed_str = injected_comment_cases[i];
        RYML_TRACE_FMT("transformed[{}/{}]=~~~[{}]\n{}\n~~~", i, injected_comment_cases.size(), transformed_str.size(), to_csubstr(transformed_str));
        SCOPED_TRACE(transformed_str);
        SCOPED_TRACE("commented");
        test_engine_ints_from_yaml(test_case, transformed_str);
        if(testing::Test::HasFailure())
            break;
    }
}

void test_engine_tree_from_yaml_with_comments(EngineEvtTestCase const& test_case)
{
    SCOPED_TRACE("test_engine_tree_from_yaml_with_comments");
    if(test_case.test_case_flags & HAS_CONTAINER_KEYS)
        return;
    if(test_case.test_case_flags & HAS_MULTILINE_SCALAR)
        return;
    const auto injected_comment_cases = inject_comments_in_src(test_case.yaml);
    for(size_t i = 0; i < injected_comment_cases.size(); ++i)
    {
        const std::string& transformed_str = injected_comment_cases[i];
        RYML_TRACE_FMT("transformed[{}/{}]=~~~[{}]\n{}\n~~~", i, injected_comment_cases.size(), transformed_str.size(), to_csubstr(transformed_str));
        SCOPED_TRACE(transformed_str);
        SCOPED_TRACE("commented");
        test_engine_tree_from_yaml(test_case, transformed_str);
        if(testing::Test::HasFailure())
            break;
    }
}

void test_engine_roundtrip_from_yaml_with_comments(EngineEvtTestCase const& test_case)
{
    SCOPED_TRACE("test_engine_roundtrip_from_yaml_with_comments");
    if(test_case.test_case_flags & HAS_CONTAINER_KEYS)
        return;
    if(test_case.test_case_flags & HAS_MULTILINE_SCALAR)
        return;
    if(test_case.test_case_flags & NO_COMPARE_EMITTED)
        return;
    const auto injected_comment_cases = inject_comments_in_src(test_case.yaml);
    for(size_t i = 0; i < injected_comment_cases.size(); ++i)
    {
        const std::string& transformed_str = injected_comment_cases[i];
        RYML_TRACE_FMT("transformed[{}/{}]=~~~[{}]\n{}\n~~~", i, injected_comment_cases.size(), transformed_str.size(), to_csubstr(transformed_str));
        SCOPED_TRACE(transformed_str);
        SCOPED_TRACE("commented");
        test_engine_roundtrip_from_yaml(test_case, transformed_str);
        if(testing::Test::HasFailure())
            break;
    }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

csubstr parse_anchor_and_tag(csubstr tokens, OptionalScalar *anchor, OptionalScalar *tag)
{
    *anchor = OptionalScalar{};
    *tag = OptionalScalar{};
    if(tokens.begins_with('&'))
    {
        size_t pos = tokens.first_of(' ');
        if(pos == (size_t)csubstr::npos)
        {
            *anchor = tokens.sub(1);
            tokens = {};
        }
        else
        {
            *anchor = tokens.first(pos).sub(1);
            tokens = tokens.right_of(pos);
        }
        _c4dbgpf("anchor: {}", anchor->get());
    }
    if(tokens.begins_with('<'))
    {
        size_t pos = tokens.find('>');
        _RYML_ASSERT_BASIC(pos != (size_t)csubstr::npos);
        *tag = tokens.first(pos + 1);
        tokens = tokens.right_of(pos).triml(' ');
        _c4dbgpf("tag: {}", tag->maybe_get());
    }
    return tokens;
}

void test_compare_events(csubstr ref_evts,
                         csubstr emt_evts,
                         bool ignore_doc_style,
                         bool ignore_container_style,
                         bool ignore_scalar_style,
                         bool ignore_tag_normalization)
{
    RYML_TRACE_FMT("actual=~~~\n{}~~~", emt_evts);
    RYML_TRACE_FMT("expected=~~~\n{}~~~", ref_evts);
    auto test_anchor_tag = [&](csubstr &ref, csubstr &emt){
        OptionalScalar reftag = {}, refanchor = {};
        OptionalScalar emttag = {}, emtanchor = {};
        ref = parse_anchor_and_tag(ref, &refanchor, &reftag).triml(' ');
        emt = parse_anchor_and_tag(emt, &emtanchor, &emttag).triml(' ');
        EXPECT_EQ(bool(refanchor), bool(emtanchor));
        if(bool(refanchor) && bool(emtanchor))
        {
            EXPECT_EQ(refanchor.get(), emtanchor.get());
        }
        EXPECT_EQ(bool(reftag), bool(emttag));
        if(reftag && emttag)
        {
            if(!ignore_tag_normalization)
            {
                EXPECT_EQ(reftag.get(), emttag.get());
            }
        }
    };
    auto test_doc = [&](csubstr ref, csubstr emt){
        EXPECT_TRUE(ref.begins_with("+DOC") || ref.begins_with("-DOC"));
        EXPECT_TRUE(emt.begins_with("+DOC") || emt.begins_with("-DOC"));
        EXPECT_EQ(emt.begins_with("+DOC"), ref.begins_with("+DOC"));
        EXPECT_EQ(emt.begins_with("-DOC"), ref.begins_with("-DOC"));
        if(ignore_doc_style)
        {
            if(ref.begins_with("+DOC"))
            {
                ref = ref.stripr("---").trimr(' ');
                emt = emt.stripr("---").trimr(' ');
            }
            else
            {
                ASSERT_TRUE(ref.begins_with("-DOC"));
                ref = ref.stripr("...").trimr(' ');
                emt = emt.stripr("...").trimr(' ');
            }
        }
        EXPECT_EQ(ref, emt);
    };
    auto test_container = [&](csubstr ref, csubstr emt){
        RYML_TRACE_FMT("expected={}", ref);
        RYML_TRACE_FMT("actual={}", emt);
        EXPECT_TRUE(ref.begins_with("+MAP") || ref.begins_with("+SEQ"));
        EXPECT_TRUE(emt.begins_with("+MAP") || emt.begins_with("+SEQ"));
        EXPECT_EQ(emt.begins_with("+MAP"), ref.begins_with("+MAP"));
        EXPECT_EQ(emt.begins_with("+SEQ"), ref.begins_with("+SEQ"));
        csubstr rest_ref = ref.sub(4).triml(' ');
        csubstr rest_emt = emt.sub(4).triml(' ');
        if(rest_ref.begins_with("{}"))
        {
            if(!ignore_container_style)
            {
                EXPECT_TRUE(rest_emt.begins_with("{}"));
            }
        }
        else if(rest_ref.begins_with("[]"))
        {
            if(!ignore_container_style)
            {
                EXPECT_TRUE(rest_emt.begins_with("[]"));
            }
        }
        test_anchor_tag(ref, emt);
    };
    auto test_val_with_scalar_wildcard = [&](csubstr ref, csubstr emt){
        ASSERT_TRUE(ref.begins_with("=VAL "));
        ASSERT_TRUE(emt.begins_with("=VAL "));
        ref = ref.sub(5);
        emt = emt.sub(5);
        test_anchor_tag(ref, emt);
        ASSERT_GE(ref.len, 0);
        ASSERT_GE(emt.len, 0);
        EXPECT_TRUE(ref[0] == ':' || ref[0] == '\'' || ref[0] == '"' || ref[0] == '|' || ref[0] == '>');
        EXPECT_TRUE(emt[0] == ':' || emt[0] == '\'' || emt[0] == '"' || emt[0] == '|' || emt[0] == '>');
        if(!ignore_scalar_style)
        {
            EXPECT_EQ(ref[0], emt[0]);
        }
        ref = ref.sub(1);
        emt = emt.sub(1);
        EXPECT_EQ(ref, emt);
    };
    EXPECT_EQ(bool(ref_evts.len), bool(emt_evts.len));
    size_t posref = 0;
    size_t posemt = 0;
    while(posref < ref_evts.len && posemt < emt_evts.len)
    {
        const size_t endref = ref_evts.find('\n', posref);
        const size_t endemt = emt_evts.find('\n', posemt);
        ASSERT_FALSE((endref == npos || endemt == npos) && (endref != endemt));
        csubstr ref = ref_evts.range(posref, endref);
        csubstr emt = emt_evts.range(posemt, endemt);
        if(ref != emt)
        {
            if(ref.begins_with("+DOC") || emt.begins_with("-DOC"))
            {
                test_doc(ref, emt);
            }
            else if(ref.begins_with("+MAP") || ref.begins_with("+SEQ"))
            {
                test_container(ref, emt);
            }
            else if(ref.begins_with("=VAL"))
            {
                test_val_with_scalar_wildcard(ref, emt);
            }
            else
            {
                ASSERT_EQ(ref, emt);
            }
        }
        posref = endref + 1u;
        posemt = endemt + 1u;
    }
}

} // namespace yml
} // namespace c4
