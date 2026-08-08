#include "./test_engine.hpp"
#include "./test_events_ints_helpers.hpp"
#include "c4/yml/extra/ints_to_testsuite.hpp"


namespace c4 {
namespace yml {


#ifdef RYML_DBG
void print_handler_info_(EventHandlerTree const& ps, csubstr stmt, const char *file, int line)
{
    if(ps.m_parent)
        dbg_printf_("{}:{}: parent.id={} curr.id={}  {}\n",
                    file, line, ps.m_parent->node_id, ps.m_curr->node_id, stmt);
    else
        dbg_printf_("{}:{}: parent.id=-- curr.id={}  {}\n",
                    file, line, ps.m_curr->node_id, stmt);
    print_tree(*ps.m_tree);
}

void print_handler_info_(extra::EventHandlerInts const& ps, csubstr stmt, const char *file, int line)
{
    dbg_printf_("{}:{}: {}\n", file, line, stmt);
    (void)ps;
}
#endif


//-----------------------------------------------------------------------------

namespace {

struct EngineTestIntBuffers
{
    extra::IntBuffers buf;
public:

    void prepare_parse(extra::EventHandlerInts &handler,
                      std::string const& parsed_yaml,
                      extra::evt_bits ints_size=-1, size_t arena_size=npos)
    {
        buf.prepare_parse(handler, parsed_yaml, ints_size, arena_size);
    }
    bool resize_post_parse(extra::EventHandlerInts &handler, std::string const& parsed_yaml)
    {
        return buf.resize_post_parse(handler, parsed_yaml);
    }

public:

    void prepare_events(EventHandlerIntsTr &handler_tr,
                        std::string const& parsed_yaml,
                        extra::evt_bits ints_size=-1, size_t arena_size=npos)
    {
        prepare_parse(handler_tr.handler, parsed_yaml, ints_size, arena_size);
        handler_tr.transformer.src = to_csubstr(buf.src);
    }
    bool resize_post_events(EventHandlerIntsTr &events_tr, std::string const& parsed_yaml)
    {
        if(resize_post_parse(events_tr.handler, parsed_yaml))
        {
            events_tr.transformer.src = to_csubstr(buf.src);
            return true;
        }
        return false;
    }

public:

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

};
} // namespace


void test_engine_error_ints_from_events(const EngineEvtTestCase& test_case, EventProducerInts event_producer)
{
    SCOPED_TRACE("error_ints_from_events");
    EngineTestIntBuffers buffers;
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
    EngineTestIntBuffers buffers;
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
    EngineTestIntBuffers buffers;
    ExpectError::check_error(errtype, [&]{
        extra::EventHandlerInts handler{};
        ParseEngine<extra::EventHandlerInts> parser(&handler, test_case.opts);
        buffers.prepare_parse(handler, test_case, test_case.yaml);
        parser.parse_in_place_ev(test_case.fileline, to_substr(buffers.src));
    }, test_case.expected_error_location);
    if(testing::Test::HasFailure())
        buffers.print(/*all*/false);
}

static void test_engine_ints_from_yaml(EngineTestIntBuffers& buffers, EngineEvtTestCase const& test_case, std::string const& parsed_yaml)
{
    SCOPED_TRACE("test_engine_ints_from_yaml");
    extra::EventHandlerInts handler{};
    ParseEngine<extra::EventHandlerInts> parser(&handler, test_case.opts);
    int size_estimated = extra::estimate_events_ints_size(to_csubstr(parsed_yaml));
    int reqsz_evts = 0;
    size_t reqsz_arena = 0;
    {
        SCOPED_TRACE("empty buffers");
        // try first with empty buffers
        buffers.prepare_parse(handler, test_case, parsed_yaml, 0, 0);
        parser.parse_in_place_ev(test_case.fileline, to_substr(buffers.src));
        EXPECT_GE(size_estimated, handler.required_size_events());
    }
    reqsz_evts = handler.required_size_events();
    reqsz_arena = handler.required_size_arena();
    {
        SCOPED_TRACE("small buffers");
        buffers.prepare_parse(handler, test_case, parsed_yaml, reqsz_evts / 2, reqsz_arena / 2);
        parser.parse_in_place_ev(test_case.fileline, to_substr(buffers.src));
        EXPECT_EQ(handler.required_size_events(), reqsz_evts);
        EXPECT_EQ(handler.required_size_arena(), reqsz_arena);
    }
    if(test_case.expected_ints_enabled)
    {
        size_t size_reference = num_ints(test_case.expected_ints.data(), test_case.expected_ints.size());
        EXPECT_EQ(size_reference, handler.required_size_events());
    }
    EXPECT_TRUE(buffers.resize_post_parse(handler, parsed_yaml));
    {
        SCOPED_TRACE("buffers ok");
        parser.parse_in_place_ev(test_case.fileline, to_substr(buffers.src));
        EXPECT_FALSE(buffers.resize_post_parse(handler, parsed_yaml));
        EXPECT_EQ(handler.required_size_events(), reqsz_evts);
        EXPECT_EQ(handler.required_size_arena(), reqsz_arena);
        ASSERT_TRUE(handler.fits_buffers());
        buffers.test(test_case);
    }
}

void test_engine_ints_from_yaml(EngineEvtTestCase const& test_case, std::string const& parsed_yaml)
{
    EngineTestIntBuffers buffers;
    test_engine_ints_from_yaml(buffers, test_case, parsed_yaml);
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
    Tree parsed_tree = {};
    EventHandlerTree handler(&parsed_tree, parsed_tree.root_id());
    ASSERT_EQ(&parsed_tree, handler.m_tree);
    ParseEngine<EventHandlerTree> parser(&handler, test_case.opts);
    ASSERT_EQ(&handler, parser.m_evt_handler);
    ASSERT_EQ(&parsed_tree, parser.m_evt_handler->m_tree);
    std::vector<char> copy(yaml.begin(), yaml.end()); // g++ 4.8 fails with std::string
    parser.parse_in_place_ev(test_case.fileline, to_substr(copy));
    std::string actual_emitted = emitrs_yaml<std::string>(parsed_tree);
    auto show_info = [&]{
        printf("source: ~~~\n%.*s~~~\n", (int)test_case.yaml.size(), test_case.yaml.data());
        print_tree("parsed_tree", parsed_tree);
        printf("emitted: ~~~\n%.*s~~~\n", (int)actual_emitted.size(), actual_emitted.data());
    };
    {
        SCOPED_TRACE("test parsed tree");
        test_invariants(parsed_tree);
        if(!(test_case.test_case_flags & NO_COMPARE_EMITTED))
        {
            EXPECT_EQ(test_case.expected_emitted, actual_emitted);
        }
    }
    if(testing::Test::HasFailure())
    {
        show_info();
        return;
    }
    // test also Tree::resolve_tags(): if the parser options are set
    // to resolve, then parse again into an unresolved tree, and
    // compare to the original (resolved) tree from above
    if(!test_case.opts.resolve_tags())
        return;
    ParserOptions unresolved_opts = test_case.opts;
    bool resolve_all = test_case.opts.resolve_tags_all();
    unresolved_opts.resolve_tags(false);
    Tree unresolved_tree;
    EventHandlerTree unresolved_handler(&unresolved_tree, unresolved_tree.root_id());
    ParseEngine<EventHandlerTree> unresolved_parser(&unresolved_handler, unresolved_opts);
    std::vector<char> unresolved_copy(yaml.begin(), yaml.end()); // g++ 4.8 fails with std::string
    unresolved_parser.parse_in_place_ev(test_case.fileline, to_substr(unresolved_copy));
    TagCache tag_cache;
    Tree resolved_tree = unresolved_tree;
    resolved_tree.resolve_tags(tag_cache, resolve_all);
    std::string resolved_emitted = emitrs_yaml<std::string>(resolved_tree);
    {
        SCOPED_TRACE("test Tree::resolve_tags()");
        test_invariants(unresolved_tree);
        test_invariants(resolved_tree);
        test_compare(resolved_tree, parsed_tree, "resolved_tree", "tree");
        if(!(test_case.test_case_flags & NO_COMPARE_EMITTED))
        {
            EXPECT_EQ(test_case.expected_emitted, resolved_emitted);
        }
    }
    if(testing::Test::HasFailure())
    {
        show_info();
        print_tree("unresolved", unresolved_tree);
        printf("resolve_all=%d\n", resolve_all);
        print_tree("resolved", resolved_tree);
        printf("resolved_emitted: ~~~\n%.*s~~~\n", (int)resolved_emitted.size(), resolved_emitted.data());
    }
}


//-----------------------------------------------------------------------------

void test_engine_roundtrip_tree_from_events(EngineEvtTestCase const& test_case, EventProducerTree event_producer)
{
    if(test_case.test_case_flags & HAS_CONTAINER_KEYS)
        return;
    SCOPED_TRACE("roundtrip_tree_from_events");
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

void test_engine_roundtrip_ints_from_events(EngineEvtTestCase const& test_case, EventProducerInts event_producer)
{
    SCOPED_TRACE("roundtrip_ints_from_events");
    (void)test_case;
    (void)event_producer;
}

void test_engine_roundtrip_tree_from_yaml(EngineEvtTestCase const& test_case, std::string const& yaml)
{
    if(test_case.test_case_flags & HAS_CONTAINER_KEYS) // NOLINT
        return;
    SCOPED_TRACE("roundtrip_tree_from_yaml");
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

void test_engine_roundtrip_ints_from_yaml(EngineEvtTestCase const& test_case, std::string const& yaml)
{
    SCOPED_TRACE("roundtrip_ints_from_yaml");
    EngineTestIntBuffers buffers1;
    {
        SCOPED_TRACE("roundtrip_parse1");
        test_engine_ints_from_yaml(buffers1, test_case, yaml);
    }
    const std::string parsed_ints_emitted = buffers1.emit_yaml<std::string>();
std::cout << parsed_ints_emitted << "\n";
    {
        EngineTestIntBuffers buffers2;
        SCOPED_TRACE("roundtrip_parse2");
        test_engine_ints_from_yaml(buffers2, test_case, parsed_ints_emitted);
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

void test_engine_roundtrip_tree_from_yaml_with_comments(EngineEvtTestCase const& test_case)
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
        test_engine_roundtrip_tree_from_yaml(test_case, transformed_str);
        if(testing::Test::HasFailure())
            break;
    }
}

void test_engine_roundtrip_ints_from_yaml_with_comments(EngineEvtTestCase const& test_case)
{
    SCOPED_TRACE("test_engine_roundtrip_from_yaml_with_comments");
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
        test_engine_roundtrip_ints_from_yaml(test_case, transformed_str);
        if(testing::Test::HasFailure())
            break;
    }
}


} // namespace yml
} // namespace c4
