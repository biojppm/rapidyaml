#include "./test_engine.hpp"
#include "testsuite/testsuite_events.hpp"
#include "c4/yml/extra/ints_utils.hpp"
#include "c4/yml/extra/ints_to_testsuite.hpp"



namespace c4 {
namespace yml {

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


#ifdef RYML_WITH_COMMENTS

id_type inject_comments_in_tree__blck_keyval(Tree *tree, id_type id, csubstr fmt, id_type count);
id_type inject_comments_in_tree__flow_keyval(Tree *tree, id_type id, csubstr fmt, id_type count);
id_type inject_comments_in_tree__blck_val(Tree *tree, id_type id, csubstr fmt, id_type count);
id_type inject_comments_in_tree__flow_val(Tree *tree, id_type id, csubstr fmt, id_type count);
id_type inject_comments_in_tree__flow_seq(Tree *tree, id_type id, csubstr fmt, id_type count);
id_type inject_comments_in_tree__flow_map(Tree *tree, id_type id, csubstr fmt, id_type count);
id_type inject_comments_in_tree__blck_seq(Tree *tree, id_type id, csubstr fmt, id_type count);
id_type inject_comments_in_tree__blck_map(Tree *tree, id_type id, csubstr fmt, id_type count);
id_type inject_comment(Tree *tree, id_type id, CommentType_e type, csubstr fmt, id_type count)
{
    struct symname { CommentType_e sym; csubstr name; };
    static const symname symbols[] = {
        #define _c4comm(comm_symbol, bit) {COMM_##comm_symbol, csubstr(#comm_symbol)},
        _RYML_DEFINE_COMMENTS(_c4comm)
        #undef _c4comm
    };
    substr comment = {};
    csubstr typestr = {};
    for(symname const& s : symbols)
        if(s.sym == type)
            typestr = s.name;
    _RYML_CHECK_BASIC(!typestr.empty());
    size_t len = c4::format(comment, fmt, count, id, typestr, count);
    comment = tree->alloc_arena(len);
    _RYML_CHECK_BASIC(comment.str != nullptr);
    _RYML_CHECK_BASIC(comment.len == len);
    size_t len2 = c4::format(comment, fmt, count, id, typestr, count);
    _RYML_CHECK_BASIC(len2 == len);
    _c4dbgpf("injecting comment: node={} type={} cmt=[{}]~~~{}~~~", id, typestr, comment.len, comment);
    tree->set_comment(id, type, comment);
    return count + 1;
}


id_type inject_comments_in_tree__blck_keyval(Tree *tree, id_type id, csubstr fmt, id_type count)
{
    (void)tree;
    (void)id;
    (void)fmt;
    (void)count;
    return count;
}
id_type inject_comments_in_tree__flow_keyval(Tree *tree, id_type id, csubstr fmt, id_type count)
{
    (void)tree;
    (void)id;
    (void)fmt;
    (void)count;
    return count;
}


id_type inject_comments_in_tree__blck_val(Tree *tree, id_type id, csubstr fmt, id_type count)
{
    (void)tree;
    (void)id;
    (void)fmt;
    (void)count;
    return count;
}
id_type inject_comments_in_tree__flow_val(Tree *tree, id_type id, csubstr fmt, id_type count)
{
    NodeType ty = tree->type(id);
    count = inject_comment(tree, id, COMM_LEADING, fmt, count);
    if(ty & VALANCH)
    {
        count = inject_comment(tree, id, COMM_VAL_ANCHOR_LEADING, fmt, count);
        count = inject_comment(tree, id, COMM_VAL_ANCHOR_TRAILING, fmt, count);
    }
    if(ty & VALTAG)
    {
        count = inject_comment(tree, id, COMM_VAL_TAG_LEADING, fmt, count);
        count = inject_comment(tree, id, COMM_VAL_TAG_TRAILING, fmt, count);
    }
    if(ty & (VALANCH|VALTAG))
    {
        count = inject_comment(tree, id, COMM_VAL_LEADING2, fmt, count);
    }
    if(ty & SEQ)
    {
        count = inject_comments_in_tree__flow_seq(tree, id, fmt, count);
    }
    else if(ty & MAP)
    {
        count = inject_comments_in_tree__flow_map(tree, id, fmt, count);
    }
    count = inject_comment(tree, id, COMM_TRAILING, fmt, count);
    return count;
}


id_type inject_comments_in_tree__flow_seq(Tree *tree, id_type id, csubstr fmt, id_type count)
{
    tree->set_style_conditionally(id, FLOW_SL, FLOW_SL, FLOW_ML);
    count = inject_comment(tree, id, COMM_VAL_BRACKET_TRAILING, fmt, count);
    id_type first = tree->first_child(id);
    for(id_type child = first; child != NONE; child = tree->next_sibling(child))
    {
        count = inject_comments_in_tree__flow_val(tree, child, fmt, count);
        count = inject_comment(tree, child, COMM_COMMA_LEADING, fmt, count);
    }
    count = inject_comment(tree, id, COMM_VAL_BRACKET_LEADING, fmt, count);
    return count;
}
id_type inject_comments_in_tree__flow_map(Tree *tree, id_type id, csubstr fmt, id_type count)
{
    tree->set_style_conditionally(id, FLOW_SL, FLOW_SL, FLOW_ML);
    (void)tree;
    (void)id;
    (void)fmt;
    (void)count;
    return count;
}


id_type inject_comments_in_tree__blck_seq(Tree *tree, id_type id, csubstr fmt, id_type count)
{
    (void)tree;
    (void)id;
    (void)fmt;
    (void)count;
    return count;
}
id_type inject_comments_in_tree__blck_map(Tree *tree, id_type id, csubstr fmt, id_type count)
{
    (void)tree;
    (void)id;
    (void)fmt;
    (void)count;
    return count;
}
#endif

} // anon


id_type inject_comments_in_tree(Tree *tree, csubstr fmt)
{
    #ifdef RYML_WITH_COMMENTS
    (void)inject_comments_in_tree__blck_keyval;
    (void)inject_comments_in_tree__flow_keyval;
    (void)inject_comments_in_tree__blck_val;
    auto kickstart = [&](id_type id, id_type count){
        NodeType ty = tree->type(id);
        if(ty & SEQ)
        {
            if(ty & (FLOW_SL|FLOW_ML))
                count = inject_comments_in_tree__flow_seq(tree, id, fmt, count);
            else
                count = inject_comments_in_tree__blck_seq(tree, id, fmt, count);
        }
        else if(ty & MAP)
        {
            if(ty & (FLOW_SL|FLOW_ML))
                count = inject_comments_in_tree__flow_map(tree, id, fmt, count);
            else
                count = inject_comments_in_tree__blck_map(tree, id, fmt, count);
        }
        else
        {
            _RYML_ASSERT_BASIC(ty & VAL);
            count = inject_comments_in_tree__blck_val(tree, id, fmt, count);
        }
        count = inject_comment(tree, id, COMM_FOOTER, fmt, count);
        return count;
    };
    id_type root = tree->root_id();
    if(!tree->is_stream(root))
        return kickstart(root, 0);
    // for streams
    id_type count = 0;
    for(id_type doc = tree->first_child(root); doc != NONE; doc = tree->next_sibling(doc))
    {
        count = inject_comment(tree, doc, COMM_DOC_TRAILING, fmt, count);
        count = kickstart(doc, count);
    }
    return count;
    #else
    (void)tree;
    (void)fmt;
    return 0;
    #endif
}


void test_expected_error_testsuite_from_yaml(std::string const& parsed_yaml, Location const& expected_error_location)
{
    ExpectError::check_error_parse([&]{
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
    ExpectError::check_error_parse([&]{
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
    ExpectError::check_error_parse(&tree, [&]{
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


void test_engine_testsuite_from_yaml(EngineEvtTestCase const& test_case, std::string const& parsed_yaml, ParserOptions opts)
{
    extra::EventHandlerTestSuite::EventSink sink;
    extra::EventHandlerTestSuite handler(&sink);
    handler.reset();
    ParseEngine<extra::EventHandlerTestSuite> parser(&handler, opts);
    std::string copy = parsed_yaml;
    parser.parse_in_place_ev("(testyaml)", to_substr(copy));
    csubstr result = sink;
    _c4dbgpf("~~~\n{}~~~\n", result);
    EXPECT_EQ(std::string(result.str, result.len), test_case.expected_events);
}

void test_engine_ints_from_yaml(EngineEvtTestCase const& test_case, std::string const& parsed_yaml, ParserOptions opts)
{
    extra::EventHandlerInts handler{};
    using IntType = extra::ievt::DataType;
    //NOTE! crashes in MIPS64 Debug c++20 (but not c++11) when size is 0:
    //std::vector<IntType> actual_evts(empty.size());
    std::vector<IntType> actual_evts; // DO THIS!
    size_t size_reference = num_ints(test_case.expected_ints.data(), test_case.expected_ints.size());
    int size_estimated = extra::estimate_events_ints_size(to_csubstr(parsed_yaml));
    // there was an error in gcc<5 where the copy buffer was NOT
    // assigned when using a std::string:
    //std::string copy = yaml.parsed; gcc<5 ERROR, see below
    std::vector<char> copy(parsed_yaml.begin(), parsed_yaml.end());
    _c4dbgpf("parsing: [{}]{}", copy.size(), c4::fmt::hex(copy.data()));
    std::vector<char> arena(copy.size());
    handler.reset(to_csubstr(copy), to_substr(arena), actual_evts.data(), (IntType)actual_evts.size());
    ParseEngine<extra::EventHandlerInts> parser(&handler, opts);
    parser.parse_in_place_ev("(testyaml)", to_substr(copy));
    EXPECT_GE(size_estimated, handler.required_size_events());
    if(test_case.expected_ints_enabled)
    {
        EXPECT_EQ(size_reference, handler.required_size_events());
    }
    size_t sz = (size_t)handler.required_size_events();
    if (!handler.fits_buffers())
    {
        if(sz > actual_evts.size())
            actual_evts.resize(sz);
        copy.assign(parsed_yaml.begin(), parsed_yaml.end());
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
    if (test_case.expected_ints_enabled)
    {
        RYML_TRACE_FMT("here", 0);
        test_events_ints(test_case.expected_ints.data(), test_case.expected_ints.size(),
                         actual_evts.data(), actual_evts.size(),
                         to_csubstr(parsed_yaml), to_csubstr(copy), to_csubstr(arena));
    }
    {
        RYML_TRACE_FMT("cmp", 0);
        std::string actual_test_suite_evts = extra::events_ints_to_testsuite<std::string>(to_csubstr(copy), to_csubstr(arena), actual_evts.data(), (IntType)actual_evts.size());
        test_compare_events(to_csubstr(test_case.expected_events),
                            to_csubstr(actual_test_suite_evts),
                            /*ignore_doc_style*/false,
                            /*ignore_container_style*/false,
                            /*ignore_scalar_style*/false,
                            /*ignore_tag_normalization*/true);
    }
}

void test_engine_tree_from_yaml(EngineEvtTestCase const& test_case, std::string const& yaml, ParserOptions opts)
{
    if(test_case.test_case_flags & HAS_CONTAINER_KEYS)
    {
        test_expected_error_tree_from_yaml(yaml, test_case.expected_error_location);
        return;
    }
    Tree tree = {};
    EventHandlerTree handler(&tree, tree.root_id());
    ASSERT_EQ(&tree, handler.m_tree);
    ParseEngine<EventHandlerTree> parser(&handler, opts);
    ASSERT_EQ(&handler, parser.m_evt_handler);
    ASSERT_EQ(&tree, parser.m_evt_handler->m_tree);
    std::string copy = yaml;
    parser.parse_in_place_ev("(testyaml)", to_substr(copy));
    #ifdef RYML_DBG
    print_tree("parsed_tree", tree);
    #endif
    std::string actual = emitrs_yaml<std::string>(tree);
    _c4dbgpf("~~~\n{}~~~\n", actual);
    EXPECT_EQ(test_case.expected_emitted, actual);
}

void test_engine_roundtrip_from_yaml(EngineEvtTestCase const& test_case, std::string const& yaml, ParserOptions opts)
{
    if(test_case.test_case_flags & HAS_CONTAINER_KEYS)
        return;
    csubstr filename = "(testyaml)";
    std::string copy = yaml;
    const Tree parsed_tree = parse_in_place(filename, to_substr(copy), opts);
    #ifdef RYML_DBG
    print_tree("parsed_tree", parsed_tree);
    #endif
    {
        SCOPED_TRACE("invariants_after_parse");
        test_invariants(parsed_tree);
    }
    const std::string parsed_tree_emitted = emitrs_yaml<std::string>(parsed_tree);
    EXPECT_EQ(test_case.expected_emitted, parsed_tree_emitted);
    std::string emitted0_copy = parsed_tree_emitted;
    const Tree after_roundtrip = parse_in_place(filename, to_substr(emitted0_copy), opts);
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
    EXPECT_EQ(test_case.expected_emitted, after_roundtrip_emitted);
    if(testing::Test::HasFailure())
    {
        printf("source: ~~~\n%.*s~~~\n", (int)yaml.size(), yaml.data());
        print_tree("parsed_tree", parsed_tree);
        printf("parsed_tree_emitted: ~~~\n%.*s~~~\n", (int)parsed_tree_emitted.size(), parsed_tree_emitted.data());
        print_tree("after_roundtrip", after_roundtrip);
        printf("after_roundtrip_emitted: ~~~\n%.*s~~~\n", (int)after_roundtrip_emitted.size(), after_roundtrip_emitted.data());
    }
}

void test_engine_roundtrip_from_yaml__tree_comments(EngineEvtTestCase const& test_case)
{
    if(test_case.test_case_flags & HAS_CONTAINER_KEYS)
        return;
    csubstr filename = "(testyaml)";
    std::string copy = test_case.yaml;
    ParserOptions without_comments = {};
    ParserOptions with_comments = {};
    without_comments.with_comments(false);
    with_comments.with_comments(true);
    const Tree parsed_tree = parse_in_place(filename, to_substr(copy), without_comments);
    {
        SCOPED_TRACE("invariants_after_parse");
        test_invariants(parsed_tree);
    }
    for(csubstr fmt : {
            csubstr(" {} node={} {} {}"),
            csubstr("\n {} node={} {}\n multiline\n with\n many lines\n {}\n"),
        })
    {
        RYML_TRACE_FMT("injected comment format={}", fmt);
        Tree injected_tree = parsed_tree;
        id_type num_comments = inject_comments_in_tree(&injected_tree, fmt);
        {
            SCOPED_TRACE("invariants_after_inject");
            test_invariants(parsed_tree);
        }
        EXPECT_EQ(num_comments, injected_tree.m_comments_size);
        const std::string injected_tree_emitted = emitrs_yaml<std::string>(injected_tree);
        std::string injected_tree_emitted_copy = injected_tree_emitted;
        const Tree after_roundtrip = parse_in_place(filename, to_substr(injected_tree_emitted_copy), with_comments);
        {
            SCOPED_TRACE("invariants_after_roundtrip");
            test_invariants(after_roundtrip);
        }
        const std::string roundtrip_tree_emitted = emitrs_yaml<std::string>(after_roundtrip);
        EXPECT_EQ(num_comments, after_roundtrip.m_comments_size);
        EXPECT_EQ(injected_tree_emitted, roundtrip_tree_emitted);
        {
            SCOPED_TRACE("compare_trees");
            test_compare(after_roundtrip, injected_tree,
                         "after_roundtrip", "injected_tree");
        }
        if(testing::Test::HasFailure())
        {
            printf("source: ~~~\n%.*s~~~\n", (int)test_case.yaml.size(), test_case.yaml.data());
            print_tree("parsed_tree", parsed_tree);
            print_tree("injected_tree", injected_tree);
            printf("injected_tree_emitted: ~~~\n%.*s~~~\n", (int)injected_tree_emitted.size(), injected_tree_emitted.data());
            print_tree("after_roundtrip", after_roundtrip);
            printf("roundtrip_tree_emitted: ~~~\n%.*s~~~\n", (int)roundtrip_tree_emitted.size(), roundtrip_tree_emitted.data());
            break;
        }
    }
}


void test_engine_testsuite_from_yaml__src_comments(EngineEvtTestCase const& test_case)
{
    if(test_case.opts.with_comments())
        return;
    if(test_case.test_case_flags & HAS_CONTAINER_KEYS)
        return;
    if(test_case.test_case_flags & HAS_MULTILINE_SCALAR)
        return;
    ParserOptions opts = test_case.opts;
    _RYML_WITH_COMMENTS(opts.with_comments(false);)
    const auto injected_comment_cases = inject_comments_in_src(test_case.yaml);
    for(size_t i = 0; i < injected_comment_cases.size(); ++i)
    {
        const std::string& transformed_str = injected_comment_cases[i];
        RYML_TRACE_FMT("transformed[{}/{}]=~~~[{}]\n{}\n~~~", i, injected_comment_cases.size(), transformed_str.size(), to_csubstr(transformed_str));
        SCOPED_TRACE(transformed_str);
        SCOPED_TRACE("commented");
        test_engine_testsuite_from_yaml(test_case, transformed_str, opts);
    }
}

void test_engine_ints_from_yaml__src_comments(EngineEvtTestCase const& test_case)
{
    if(test_case.opts.with_comments())
        return;
    if(test_case.test_case_flags & HAS_MULTILINE_SCALAR)
        return;
    ParserOptions opts = test_case.opts;
    _RYML_WITH_COMMENTS(opts.with_comments(false);)
    const auto injected_comment_cases = inject_comments_in_src(test_case.yaml);
    for(size_t i = 0; i < injected_comment_cases.size(); ++i)
    {
        const std::string& transformed_str = injected_comment_cases[i];
        RYML_TRACE_FMT("transformed[{}/{}]=~~~[{}]\n{}\n~~~", i, injected_comment_cases.size(), transformed_str.size(), to_csubstr(transformed_str));
        SCOPED_TRACE(transformed_str);
        SCOPED_TRACE("commented");
        test_engine_ints_from_yaml(test_case, transformed_str, opts);
    }
}

void test_engine_tree_from_yaml__src_comments(EngineEvtTestCase const& test_case)
{
    if(test_case.opts.with_comments())
        return;
    if(test_case.test_case_flags & HAS_CONTAINER_KEYS)
        return;
    if(test_case.test_case_flags & HAS_MULTILINE_SCALAR)
        return;
    ParserOptions opts = test_case.opts;
    _RYML_WITH_COMMENTS(opts.with_comments(false);)
    const auto injected_comment_cases = inject_comments_in_src(test_case.yaml);
    for(size_t i = 0; i < injected_comment_cases.size(); ++i)
    {
        const std::string& transformed_str = injected_comment_cases[i];
        RYML_TRACE_FMT("transformed[{}/{}]=~~~[{}]\n{}\n~~~", i, injected_comment_cases.size(), transformed_str.size(), to_csubstr(transformed_str));
        SCOPED_TRACE(transformed_str);
        SCOPED_TRACE("commented");
        test_engine_tree_from_yaml(test_case, transformed_str, opts);
    }
}

void test_engine_roundtrip_from_yaml__src_comments(EngineEvtTestCase const& test_case)
{
    if(test_case.opts.with_comments())
        return;
    if(test_case.test_case_flags & HAS_CONTAINER_KEYS)
        return;
    if(test_case.test_case_flags & HAS_MULTILINE_SCALAR)
        return;
    ParserOptions opts = test_case.opts;
    _RYML_WITH_COMMENTS(opts.with_comments(false);)
    const auto injected_comment_cases = inject_comments_in_src(test_case.yaml);
    for(size_t i = 0; i < injected_comment_cases.size(); ++i)
    {
        const std::string& transformed_str = injected_comment_cases[i];
        RYML_TRACE_FMT("transformed[{}/{}]=~~~[{}]\n{}\n~~~", i, injected_comment_cases.size(), transformed_str.size(), to_csubstr(transformed_str));
        SCOPED_TRACE(transformed_str);
        SCOPED_TRACE("commented");
        test_engine_roundtrip_from_yaml(test_case, transformed_str, opts);
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
