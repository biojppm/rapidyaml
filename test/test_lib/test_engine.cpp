#include "./test_engine.hpp"
#include "testsuite/testsuite_events.hpp"
#include "c4/yml/extra/ints_utils.hpp"
#include "c4/yml/extra/ints_to_testsuite.hpp"



namespace c4 {
namespace yml {

namespace {
// inject comments on every line
std::vector<std::string> inject_comments(std::string const& src_)
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


void test_expected_error_testsuite_from_yaml(std::string const& parsed_yaml, Location const& expected_error_location)
{
    ExpectError::check_error([&]{
        extra::EventHandlerTestSuite::EventSink sink;
        extra::EventHandlerTestSuite handler(&sink);
        handler.reset();
        ParseEngine<extra::EventHandlerTestSuite> parser(&handler);
        std::string copy = parsed_yaml;
        parser.parse_in_place_ev("(testyaml)", to_substr(copy));
    }, expected_error_location);
}

void test_expected_error_ints_from_yaml(std::string const& parsed_yaml, Location const& expected_error_location)
{
    ExpectError::check_error([&]{
        extra::EventHandlerInts handler{};
        handler.reset(to_csubstr(parsed_yaml), substr{}, nullptr, 0);
        ParseEngine<extra::EventHandlerInts> parser(&handler);
        std::string copy = parsed_yaml;
        parser.parse_in_place_ev("(testyaml)", to_substr(copy));
    }, expected_error_location);
}

void test_expected_error_tree_from_yaml(std::string const& parsed_yaml, Location const& expected_error_location)
{
    Tree tree = {};
    ExpectError::check_error(&tree, [&]{
        EventHandlerTree handler(&tree, tree.root_id());
        ASSERT_EQ(&tree, handler.m_tree);
        ParseEngine<EventHandlerTree> parser(&handler);
        ASSERT_EQ(&handler, parser.m_evt_handler);
        ASSERT_EQ(&tree, parser.m_evt_handler->m_tree);
        std::string copy = parsed_yaml;
        parser.parse_in_place_ev("(testyaml)", to_substr(copy));
        tree.resolve_tags();
    }, expected_error_location);
}


void test_engine_testsuite_from_yaml(ReferenceYaml const& yaml, std::string const& expected_events, ParserOptions opts)
{
    extra::EventHandlerTestSuite::EventSink sink;
    extra::EventHandlerTestSuite handler(&sink);
    handler.reset();
    ParseEngine<extra::EventHandlerTestSuite> parser(&handler, opts);
    std::string copy = yaml.parsed;
    parser.parse_in_place_ev("(testyaml)", to_substr(copy));
    csubstr result = sink;
    _c4dbgpf("~~~\n{}~~~\n", result);
    EXPECT_EQ(std::string(result.str, result.len), expected_events);
}

void test_engine_ints_from_yaml(ReferenceYaml const& yaml, std::string const& expected_events, ParserOptions opts)
{
    extra::EventHandlerInts handler{};
    using IntType = extra::ievt::DataType;
    //NOTE! crashes in MIPS64 Debug c++20 (but not c++11) when size is 0:
    //std::vector<IntType> actual_evts(empty.size());
    std::vector<IntType> actual_evts; // DO THIS!
    size_t size_reference = num_ints(yaml.expected_ints.data(), yaml.expected_ints.size());
    int size_estimated = extra::estimate_events_ints_size(to_csubstr(yaml.parsed));
    // there was an error in gcc<5 where the copy buffer was NOT
    // assigned when using a std::string:
    //std::string copy = yaml.parsed; gcc<5 ERROR, see below
    std::vector<char> copy(yaml.parsed.begin(), yaml.parsed.end());
    _c4dbgpf("parsing: [{}]{}", copy.size(), c4::fmt::hex(copy.data()));
    std::vector<char> arena(copy.size());
    handler.reset(to_csubstr(copy), to_substr(arena), actual_evts.data(), (IntType)actual_evts.size());
    ParseEngine<extra::EventHandlerInts> parser(&handler, opts);
    parser.parse_in_place_ev("(testyaml)", to_substr(copy));
    EXPECT_GE(size_estimated, handler.required_size_events());
    if(yaml.expected_ints_enabled)
    {
        EXPECT_EQ(size_reference, handler.required_size_events());
    }
    size_t sz = (size_t)handler.required_size_events();
    if (!handler.fits_buffers())
    {
        if(sz > actual_evts.size())
            actual_evts.resize(sz);
        copy.assign(yaml.parsed.begin(), yaml.parsed.end());
        if(handler.required_size_arena() > arena.size())
            arena.resize(handler.required_size_arena());
        _c4dbgpf("parsing again: (before) [{}]{}", copy.size(), c4::fmt::hex(copy.data()));
        //copy = yaml.parsed; ERROR: bad assignment in gcc<5
        _c4dbgpf("parsing again: (after) [{}]{}", copy.size(), c4::fmt::hex(copy.data()));
        handler.reset(to_csubstr(copy), to_substr(arena), actual_evts.data(), (IntType)actual_evts.size());
        parser.parse_in_place_ev("(testyaml)", to_substr(copy));
    }
    actual_evts.resize(sz);
    #ifdef RYML_DBG
    extra::events_ints_print(to_csubstr(copy), to_csubstr(arena), actual_evts.data(), (IntType)actual_evts.size());
    #endif
    {
        RYML_TRACE_FMT("invariants", 0);
        extra::test_events_ints_invariants(to_csubstr(copy), to_csubstr(arena), actual_evts.data(), (IntType)actual_evts.size());
    }
    if (yaml.expected_ints_enabled)
    {
        RYML_TRACE_FMT("here", 0);
        test_events_ints(yaml.expected_ints.data(), yaml.expected_ints.size(),
                         actual_evts.data(), actual_evts.size(),
                         to_csubstr(yaml.parsed), to_csubstr(copy), to_csubstr(arena));
    }
    {
        RYML_TRACE_FMT("cmp", 0);
        std::string actual_test_suite_evts = extra::events_ints_to_testsuite<std::string>(to_csubstr(copy), to_csubstr(arena), actual_evts.data(), (IntType)actual_evts.size());
        test_compare_events(to_csubstr(expected_events),
                            to_csubstr(actual_test_suite_evts),
                            /*ignore_doc_style*/false,
                            /*ignore_container_style*/false,
                            /*ignore_scalar_style*/false,
                            /*ignore_tag_normalization*/true);
    }
}

void test_engine_tree_from_yaml(ReferenceYaml const& yaml, ParserOptions opts)
{
    if(yaml.test_case_flags & HAS_CONTAINER_KEYS)
    {
        test_expected_error_tree_from_yaml(yaml.parsed, yaml.expected_error_location);
        return;
    }
    Tree tree = {};
    EventHandlerTree handler(&tree, tree.root_id());
    ASSERT_EQ(&tree, handler.m_tree);
    ParseEngine<EventHandlerTree> parser(&handler, opts);
    ASSERT_EQ(&handler, parser.m_evt_handler);
    ASSERT_EQ(&tree, parser.m_evt_handler->m_tree);
    std::string copy = yaml.parsed;
    parser.parse_in_place_ev("(testyaml)", to_substr(copy));
    #ifdef RYML_DBG
    print_tree(tree);
    #endif
    std::string actual = emitrs_yaml<std::string>(tree);
    _c4dbgpf("~~~\n{}~~~\n", actual);
    EXPECT_EQ(actual, yaml.emitted);
}


void test_engine_testsuite_from_yaml_with_comments(ReferenceYaml const& yaml, std::string const& expected_events, ParserOptions opts)
{
    if(yaml.test_case_flags & HAS_CONTAINER_KEYS)
        return;
    if(yaml.test_case_flags & HAS_MULTILINE_SCALAR)
        return;
    ReferenceYaml transformed = yaml;
    const auto injected_comments = inject_comments(yaml.parsed);
    for(size_t i = 0; i < injected_comments.size(); ++i)
    {
        const auto & transformed_str = injected_comments[i];
        _c4dbgpf("transformed[{}/{}]=~~~[{}]\n{}\n~~~", i, injected_comments.size(), transformed_str.size(), to_csubstr(transformed_str));
        SCOPED_TRACE(transformed_str);
        SCOPED_TRACE("commented");
        transformed.parsed = transformed_str;
        test_engine_testsuite_from_yaml(transformed, expected_events, opts);
    }
}

void test_engine_ints_from_yaml_with_comments(ReferenceYaml const& yaml, std::string const& expected_events, ParserOptions opts)
{
    if(yaml.test_case_flags & HAS_MULTILINE_SCALAR)
        return;
    ReferenceYaml transformed = yaml;
    const auto injected_comments = inject_comments(yaml.parsed);
    for(size_t i = 0; i < injected_comments.size(); ++i)
    {
        const auto & transformed_str = injected_comments[i];
        _c4dbgpf("transformed[{}/{}]=~~~[{}]\n{}\n~~~", i, injected_comments.size(), transformed_str.size(), to_csubstr(transformed_str));
        SCOPED_TRACE(transformed_str);
        SCOPED_TRACE("commented");
        transformed.parsed = transformed_str;
        test_engine_ints_from_yaml(transformed, expected_events, opts);
    }
}

void test_engine_tree_from_yaml_with_comments(ReferenceYaml const& yaml, ParserOptions opts)
{
    if(yaml.test_case_flags & HAS_CONTAINER_KEYS)
        return;
    if(yaml.test_case_flags & HAS_MULTILINE_SCALAR)
        return;
    ReferenceYaml transformed = yaml;
    const auto injected_comments = inject_comments(yaml.parsed);
    for(size_t i = 0; i < injected_comments.size(); ++i)
    {
        const auto & transformed_str = injected_comments[i];
        _c4dbgpf("transformed[{}/{}]=~~~[{}]\n{}\n~~~", i, injected_comments.size(), transformed_str.size(), to_csubstr(transformed_str));
        SCOPED_TRACE(transformed_str);
        SCOPED_TRACE("commented");
        transformed.parsed = transformed_str;
        test_engine_tree_from_yaml(transformed, opts);
    }
}

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
        RYML_ASSERT(pos != (size_t)csubstr::npos);
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
