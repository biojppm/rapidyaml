#include "./test_engine.hpp"



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


void test_expected_error_events_from_yaml(std::string const& parsed_yaml, Location const& expected_error_location)
{
    ExpectError::check_error([&]{
        EventHandlerYamlStd::EventSink sink;
        EventHandlerYamlStd handler(&sink);
        handler.reset();
        ParseEngine<EventHandlerYamlStd> parser(&handler);
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
    }, expected_error_location);
}

void test_new_parser_events_from_yaml(ReferenceYaml const& yaml, std::string const& expected_events)
{
    EventHandlerYamlStd::EventSink sink;
    EventHandlerYamlStd handler(&sink);
    handler.reset();
    ParseEngine<EventHandlerYamlStd> parser(&handler);
    std::string copy = yaml.parsed;
    parser.parse_in_place_ev("(testyaml)", to_substr(copy));
    csubstr result = sink;
    _c4dbgpf("~~~\n{}~~~\n", result);
    EXPECT_EQ(std::string(result.str, result.len), expected_events);
}

void test_new_parser_tree_from_yaml(ReferenceYaml const& yaml)
{
    if(yaml.test_case_flags & HAS_CONTAINER_KEYS)
    {
        test_expected_error_tree_from_yaml(yaml.parsed, yaml.expected_error_location);
    }
    else
    {
        Tree tree = {};
        EventHandlerTree handler(&tree, tree.root_id());
        ASSERT_EQ(&tree, handler.m_tree);
        ParseEngine<EventHandlerTree> parser(&handler);
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
}

void test_new_parser_events_from_yaml_with_comments(ReferenceYaml const& yaml, std::string const& expected_events)
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
        test_new_parser_events_from_yaml(transformed, expected_events);
    }
}

void test_new_parser_tree_from_yaml_with_comments(ReferenceYaml const& yaml)
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
        test_new_parser_tree_from_yaml(transformed);
    }
}

} // namespace yml
} // namespace c4
