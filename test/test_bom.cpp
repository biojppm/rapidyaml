#include "./test_lib/test_case.hpp"
#include <c4/utf.hpp>

namespace c4 {
namespace yml {

namespace {

struct bomspec
{
    csubstr name;
    Encoding_e encoding;
    csubstr bom;
};

const bomspec bomspecs[] = {
    //                        bare string causes problems in gcc5 and earlier
    {"NOBOM"       , UTF8   , csubstr("", size_t(0))},
    {"UTF8"        , UTF8   , csubstr("\xef\xbb\xbf", 3)},
    {"UTF16BE"     , UTF16BE, csubstr("\xfe\xff", 2)},
    {"!UTF16BE-a"  , UTF16BE, csubstr("\x00""a", 2)},
    {"!UTF16BE-b"  , UTF16BE, csubstr("\x00""b", 2)},
    {"!UTF16BE-0"  , UTF16BE, csubstr("\x00""0", 2)},
    {"UTF16LE"     , UTF16LE, csubstr("\xff\xfe", 2)},
    {"!UTF16LE-a"  , UTF16LE, csubstr("a""\x00" , 2)},
    {"!UTF16LE-b"  , UTF16LE, csubstr("b""\x00" , 2)},
    {"!UTF16LE-0"  , UTF16LE, csubstr("0""\x00" , 2)},
    {"UTF32BE"     , UTF32BE, csubstr("\x00\x00\xfe\xff", 4)},
    {"!UTF32BE-a"  , UTF32BE, csubstr("\x00\x00\x00""a" , 4)},
    {"!UTF32BE-b"  , UTF32BE, csubstr("\x00\x00\x00""b" , 4)},
    {"!UTF32BE-0"  , UTF32BE, csubstr("\x00\x00\x00""0" , 4)},
    {"UTF32LE"     , UTF32LE, csubstr("\xff\xfe\x00\x00", 4)},
    {"!UTF32LE-a"  , UTF32LE, csubstr("a""\x00\x00\x00" , 4)},
    {"!UTF32LE-b"  , UTF32LE, csubstr("b""\x00\x00\x00" , 4)},
    {"!UTF32LE-0"  , UTF32LE, csubstr("0""\x00\x00\x00" , 4)},
};

template<class CreateFn, class TestFn>
void test_bom(bomspec const& bom, CreateFn &&createfn, TestFn &&testfn, bool with_docs=true)
{
    Parser::handler_type handler;
    Parser parser(&handler);
    SCOPED_TRACE(bom.name);
    {
        std::string buf = std::forward<CreateFn>(createfn)(bom);
        SCOPED_TRACE(buf);
        SCOPED_TRACE("single instance");
        #ifdef RYML_DBG
        std::cout << "------------------\n" << bom.name << "\n" << buf << "\n";
        #endif
        Tree tree = parse_in_arena(&parser, to_csubstr(buf));
        std::forward<TestFn>(testfn)(parser, tree.crootref(), bom);
    }
    if(with_docs)
    {
        SCOPED_TRACE(bom.name);
        std::string buf = std::forward<CreateFn>(createfn)(bom);
        buf += "\n---\n";
        buf += std::forward<CreateFn>(createfn)(bom);
        SCOPED_TRACE(buf);
        SCOPED_TRACE("two docs");
        #ifdef RYML_DBG
        std::cout << "------------------\n" << bom.name << " x2\n" << buf << "\n";
        #endif
        Tree tree = parse_in_arena(&parser, to_csubstr(buf));
        {
            SCOPED_TRACE("doc 0");
            std::forward<TestFn>(testfn)(parser, tree.docref(0), bom);
        }
        if(tree.num_children(tree.root_id()) > 1)
        {
            SCOPED_TRACE("doc 1");
            std::forward<TestFn>(testfn)(parser, tree.docref(1), bom);
        }
    }
}

template<class CreateFn, class TestFn>
void test_bom_json(bomspec const& bom, CreateFn &&createfn, TestFn &&testfn)
{
    Parser::handler_type handler;
    Parser parser(&handler);
    SCOPED_TRACE(bom.name);
    std::string buf = std::forward<CreateFn>(createfn)(bom);
    SCOPED_TRACE(buf);
    #ifdef RYML_DBG
    std::cout << "------------------\n" << bom.name << "\n" << buf << "\n";
    #endif
    Tree tree = parse_json_in_arena(&parser, to_csubstr(buf));
    std::forward<TestFn>(testfn)(parser, tree.crootref(), bom);
}

typedef enum {
    bom2_err_none,
    bom2_err_vs,
    bom2_err_any
} bom2_err_e;
using bom2spec = std::tuple<bomspec,bomspec>;

template<class CreateFn, class TestFn>
void test_bom2(bom2spec const& spec, CreateFn &&createfn, TestFn &&testfn, bom2_err_e err=bom2_err_vs)
{
    bomspec const& bom1 = std::get<0>(spec);
    bomspec const& bom2 = std::get<1>(spec);
    SCOPED_TRACE(bom1.name);
    SCOPED_TRACE(bom2.name);
    std::string buf = std::forward<CreateFn>(createfn)(bom1, bom2);
    SCOPED_TRACE(buf);
    #ifdef RYML_DBG
    std::cout << "------------------\n" << bom1.name << " vs " << bom2.name << "\n" << buf << "\n";
    #endif
    if(err == bom2_err_none || (err == bom2_err_vs && (bom1.encoding == bom2.encoding || bom2.bom.empty())))
    {
        Parser::handler_type handler;
        Parser parser(&handler);
        Tree tree = parse_in_arena(&parser, to_csubstr(buf));
        std::forward<TestFn>(testfn)(parser, tree.crootref(), bom1, bom2);
        if(::testing::Test::HasFailure())
        {
            std::cout << "------------------\n" << bom1.name << " vs " << bom2.name << "\n" << buf << "\n";
            print_tree(tree);
        }
    }
    else
    {
        pfn_error orig = get_callbacks().m_error;
        ExpectError::check_error([&]{
            Parser::handler_type handler;
            Parser parser(&handler);
            Tree tree;
            ASSERT_EQ((pfn_error)tree.callbacks().m_error, (pfn_error)parser.callbacks().m_error);
            ASSERT_NE((pfn_error)tree.callbacks().m_error, orig);
            parse_in_arena(&parser, to_csubstr(buf), &tree);
            print_tree(tree);
        });
    }
}

std::string namefor(bomspec const& param)
{
    std::string s(param.name.str, param.name.len);
    substr ss = to_substr(s);
    ss.replace('!', '_');
    ss.replace('-', '_');
    return s;
}
std::string namefor(bom2spec const& param)
{
    return c4::catrs<std::string>(namefor(std::get<0>(param)), "__vs__", namefor(std::get<1>(param)));
}
template<class T>
std::string namefor(testing::TestParamInfo<T> const& pinfo)
{
    return namefor(pinfo.param);
}

template<class... Args>
std::string mkstr(Args&& ...args)
{
    return c4::formatrs<std::string>(std::forward<Args>(args)...);
}
} // namespace anon


struct TestBOM : public testing::TestWithParam<bomspec> {};
struct TestBOM2 : public testing::TestWithParam<bom2spec> {};

INSTANTIATE_TEST_SUITE_P(byte_order_mark, TestBOM,
                         testing::ValuesIn(bomspecs),
                         namefor<bomspec>);
INSTANTIATE_TEST_SUITE_P(byte_order_mark, TestBOM2,
                         testing::Combine(testing::ValuesIn(bomspecs), testing::ValuesIn(bomspecs)),
                         namefor<bom2spec>);



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST_P(TestBOM, specs)
{
    const bomspec p = GetParam();
    SCOPED_TRACE(p.name);

    if(p.name == "NOBOM")
        return;
    if(!p.name.begins_with('!'))
    {
        EXPECT_EQ(c4::first_non_bom(p.bom), p.bom.len);
        EXPECT_EQ(c4::get_bom(p.bom), p.bom);
        EXPECT_EQ(c4::skip_bom(p.bom), "");
    }
    else
    {
        EXPECT_EQ(c4::first_non_bom(p.bom), 0u);
        EXPECT_EQ(c4::get_bom(p.bom), "");
        EXPECT_EQ(c4::skip_bom(p.bom), p.bom);
    }
    switch(p.encoding)
    {
    case UTF32BE:
        ASSERT_EQ(p.bom.len, 4u);
        if(!p.name.begins_with('!'))
        {
            EXPECT_EQ(p.bom.str[0], '\x00');
            EXPECT_EQ(p.bom.str[1], '\x00');
            EXPECT_EQ(p.bom.str[2], '\xfe');
            EXPECT_EQ(p.bom.str[3], '\xff');
        }
        else
        {
            EXPECT_EQ(p.bom.str[0], '\x00');
            EXPECT_EQ(p.bom.str[1], '\x00');
            EXPECT_EQ(p.bom.str[2], '\x00');
            EXPECT_GE(p.bom.str[3], 0);
            EXPECT_LE(p.bom.str[3], 0x7f);
        }
        break;
    case UTF32LE:
        ASSERT_EQ(p.bom.len, 4u);
        if(!p.name.begins_with('!'))
        {
            EXPECT_EQ(p.bom.str[0], '\xff');
            EXPECT_EQ(p.bom.str[1], '\xfe');
            EXPECT_EQ(p.bom.str[2], '\x00');
            EXPECT_EQ(p.bom.str[3], '\x00');
        }
        else
        {
            EXPECT_GE(p.bom.str[0], 0);
            EXPECT_LE(p.bom.str[0], 0x7f);
            EXPECT_EQ(p.bom.str[1], '\x00');
            EXPECT_EQ(p.bom.str[2], '\x00');
            EXPECT_EQ(p.bom.str[3], '\x00');
        }
        break;
    case UTF16BE:
        ASSERT_EQ(p.bom.len, 2u);
        if(!p.name.begins_with('!'))
        {
            EXPECT_EQ(p.bom.str[0], '\xfe');
            EXPECT_EQ(p.bom.str[1], '\xff');
        }
        else
        {
            EXPECT_EQ(p.bom.str[0], '\x00');
            EXPECT_GE(p.bom.str[1], 0);
            EXPECT_LE(p.bom.str[1], 0x7f);
        }
        break;
    case UTF16LE:
        ASSERT_EQ(p.bom.len, 2u);
        if(!p.name.begins_with('!'))
        {
            EXPECT_EQ(p.bom.str[0], '\xff');
            EXPECT_EQ(p.bom.str[1], '\xfe');
        }
        else
        {
            EXPECT_GE(p.bom.str[0], 0);
            EXPECT_LE(p.bom.str[0], 0x7f);
            EXPECT_EQ(p.bom.str[1], '\x00');
        }
        break;
    case UTF8:
        ASSERT_EQ(p.bom.len, 3u);
        EXPECT_EQ(p.bom.str[0], '\xef');
        EXPECT_EQ(p.bom.str[1], '\xbb');
        EXPECT_EQ(p.bom.str[2], '\xbf');
        break;
    default:
        break;
    }
}

TEST_P(TestBOM, only_bom)
{
    test_bom(GetParam(),
             [](bomspec bom){
                 return mkstr("{}", bom.bom);
             },
             [](Parser const& parser, ConstNodeRef const&, bomspec const& bom){
                 EXPECT_EQ(parser.encoding(), bom.encoding);
             });
}

TEST_P(TestBOM, bom_and_scalar)
{
    test_bom(GetParam(),
             [](bomspec const& bom){
                 return mkstr("{}this is a scalar", bom.bom);
             },
             [](Parser const& parser, ConstNodeRef const& node, bomspec const& bom){
                 EXPECT_EQ(parser.encoding(), bom.encoding);
                 EXPECT_EQ(node.val(), "this is a scalar");
             });
}

TEST_P(TestBOM, scalar_and_bom)
{
    auto mkscalar = [](bomspec const& bom){
        return mkstr("this is a scalar{}", bom.bom);
    };
    test_bom(GetParam(),
             mkscalar,
             [&](Parser const& parser, ConstNodeRef const& node, bomspec const& bom){
                 EXPECT_EQ(parser.encoding(), UTF8);
                 EXPECT_EQ(node.val(), mkscalar(bom));
             });
}

TEST_P(TestBOM, scalar_bom_scalar)
{
    auto mkscalar = [](bomspec const& bom){
        return mkstr("this is a scalar{}and it continues", bom.bom);
    };
    test_bom(GetParam(),
             mkscalar,
             [&](Parser const& parser, ConstNodeRef const& node, bomspec const& bom){
                 EXPECT_EQ(parser.encoding(), UTF8);
                 EXPECT_EQ(node.val(), mkscalar(bom));
             });
}

TEST_P(TestBOM, bom_and_seq)
{
    auto mkyaml = [](bomspec const& bom){
        std::string yaml(bom.bom.str, bom.bom.len);
        yaml.append("[]");
        return yaml;
    };
    auto test = [](Parser const& parser, ConstNodeRef const& node, bomspec const& bom){
        EXPECT_EQ(parser.encoding(), bom.encoding);
        EXPECT_TRUE(node.is_seq());
        EXPECT_EQ(node.num_children(), 0);
    };
    test_bom(GetParam(), mkyaml, test);
    test_bom_json(GetParam(), mkyaml, test);
}

TEST_P(TestBOM, bom_and_map)
{
    auto mkyaml = [](bomspec const& bom){
        std::string yaml(bom.bom.str, bom.bom.len);
        yaml.append("{}");
        return yaml;
    };
    auto test = [](Parser const& parser, ConstNodeRef const& node, bomspec const& bom){
        EXPECT_EQ(parser.encoding(), bom.encoding);
        EXPECT_TRUE(node.is_map());
        EXPECT_EQ(node.num_children(), 0);
    };
    test_bom(GetParam(), mkyaml, test);
    test_bom_json(GetParam(), mkyaml, test);
}

TEST_P(TestBOM, bom_and_doc)
{
    auto mkyaml = [](bomspec const& bom){
        std::string yaml(bom.bom.str, bom.bom.len);
        yaml.append("---\nabc");
        return yaml;
    };
    auto test = [](Parser const& parser, ConstNodeRef const& node, bomspec const& bom){
        EXPECT_EQ(parser.encoding(), bom.encoding);
        EXPECT_EQ(node.doc(0).val(), "abc");
    };
    test_bom(GetParam(), mkyaml, test, /*with_docs*/true);
}

TEST_P(TestBOM, github559_1)
{
    auto mkyaml = [](bomspec const& bom1){
        std::string yaml(bom1.bom.str, bom1.bom.len);
        yaml.append("a: 1\nb: 2\n");
        return yaml;
    };
    auto test = [](Parser const& parser, ConstNodeRef const& node, bomspec const& bom){
        EXPECT_EQ(parser.encoding(), bom.encoding);
        ASSERT_TRUE(node.is_map());
        ASSERT_EQ(node.num_children(), 2);
        EXPECT_EQ(node["a"].val(), "1");
        EXPECT_EQ(node["b"].val(), "2");
    };
    test_bom(GetParam(), mkyaml, test);
}

TEST_P(TestBOM, github559_2)
{
    auto mkyaml = [](bomspec const& bom1){
        std::string yaml(bom1.bom.str, bom1.bom.len);
        yaml.append("- a\n- b\n");
        return yaml;
    };
    auto test = [](Parser const& parser, ConstNodeRef const& node, bomspec const& bom){
        EXPECT_EQ(parser.encoding(), bom.encoding);
        ASSERT_TRUE(node.is_seq());
        ASSERT_EQ(node.num_children(), 2);
        EXPECT_EQ(node[0].val(), "a");
        EXPECT_EQ(node[1].val(), "b");
    };
    test_bom(GetParam(), mkyaml, test);
}

TEST_P(TestBOM, github559_3)
{
    auto mkyaml = [](bomspec const& bom1){
        std::string yaml(bom1.bom.str, bom1.bom.len);
        yaml.append(R"(? multiline
  scalar
: c
)");
        return yaml;
    };
    auto test = [](Parser const& parser, ConstNodeRef const& node, bomspec const& bom){
        EXPECT_EQ(parser.encoding(), bom.encoding);
        ASSERT_TRUE(node.is_map());
        ASSERT_EQ(node.num_children(), 1);
        EXPECT_EQ(node[0].key(), "multiline scalar");
        EXPECT_EQ(node[0].val(), "c");
    };
    test_bom(GetParam(), mkyaml, test);
}

TEST_P(TestBOM, github559_4)
{
    auto mkyaml = [](bomspec const& bom1){
        std::string yaml(bom1.bom.str, bom1.bom.len);
        yaml.append(R"(|-
  multiline
  scalar
)");
        return yaml;
    };
    auto test = [](Parser const& parser, ConstNodeRef const& node, bomspec const& bom){
        EXPECT_EQ(parser.encoding(), bom.encoding);
        ASSERT_TRUE(node.is_val());
        EXPECT_EQ(node.val(), "multiline\nscalar");
    };
    test_bom(GetParam(), mkyaml, test);
}

TEST_P(TestBOM, github559_5)
{
    auto mkyaml = [](bomspec const& bom1){
        std::string yaml(bom1.bom.str, bom1.bom.len);
        yaml.append(R"(>-
  multiline
  scalar
)");
        return yaml;
    };
    auto test = [](Parser const& parser, ConstNodeRef const& node, bomspec const& bom){
        EXPECT_EQ(parser.encoding(), bom.encoding);
        ASSERT_TRUE(node.is_val());
        EXPECT_EQ(node.val(), "multiline scalar");
    };
    test_bom(GetParam(), mkyaml, test);
}


TEST_P(TestBOM, github559_6)
{
    auto mkyaml = [](bomspec const& bom1){
        std::string yaml(bom1.bom.str, bom1.bom.len);
        yaml.append("a:\n  - 1\n  - 2\n");
        return yaml;
    };
    auto test = [](Parser const& parser, ConstNodeRef const& node, bomspec const& bom){
        EXPECT_EQ(parser.encoding(), bom.encoding);
        ASSERT_TRUE(node.is_map());
        ASSERT_EQ(node.num_children(), 1);
        ASSERT_TRUE(node["a"].is_seq());
        ASSERT_EQ(node["a"].num_children(), 2);
        EXPECT_EQ(node["a"][0].val(), "1");
        EXPECT_EQ(node["a"][1].val(), "2");
    };
    test_bom(GetParam(), mkyaml, test);
}

TEST_P(TestBOM, github559_6_1)
{
    auto mkyaml = [](bomspec const& bom1){
        std::string yaml(bom1.bom.str, bom1.bom.len);
        yaml.append(" :\n  - 1\n  - 2\n");
        return yaml;
    };
    auto test = [](Parser const& parser, ConstNodeRef const& node, bomspec const& bom){
        EXPECT_EQ(parser.encoding(), bom.encoding);
        ASSERT_TRUE(node.is_map());
        ASSERT_EQ(node.num_children(), 1);
        ASSERT_TRUE(node[0].is_seq());
        EXPECT_EQ(node[0].key(), "");
        ASSERT_EQ(node[0].num_children(), 2);
        EXPECT_EQ(node[0][0].val(), "1");
        EXPECT_EQ(node[0][1].val(), "2");
    };
    test_bom(GetParam(), mkyaml, test);
}

TEST_P(TestBOM, github559_7)
{
    auto mkyaml = [](bomspec const& bom1){
        std::string yaml(bom1.bom.str, bom1.bom.len);
        yaml.append("- a: 1\n  b: 2\n");
        return yaml;
    };
    auto test = [](Parser const& parser, ConstNodeRef const& node, bomspec const& bom){
        EXPECT_EQ(parser.encoding(), bom.encoding);
        ASSERT_TRUE(node.is_seq());
        ASSERT_EQ(node.num_children(), 1);
        ASSERT_TRUE(node[0].is_map());
        ASSERT_EQ(node[0].num_children(), 2);
        EXPECT_EQ(node[0]["a"].val(), "1");
        EXPECT_EQ(node[0]["b"].val(), "2");
    };
    test_bom(GetParam(), mkyaml, test);
}

TEST_P(TestBOM, github559_8)
{
    auto mkyaml = [](bomspec const& bom1){
        std::string yaml(bom1.bom.str, bom1.bom.len);
        yaml.append("- : 1\n  b: 2\n");
        return yaml;
    };
    auto test = [](Parser const& parser, ConstNodeRef const& node, bomspec const& bom){
        EXPECT_EQ(parser.encoding(), bom.encoding);
        ASSERT_TRUE(node.is_seq());
        ASSERT_EQ(node.num_children(), 1);
        ASSERT_TRUE(node[0].is_map());
        ASSERT_EQ(node[0].num_children(), 2);
        EXPECT_EQ(node[0][0].key(), "");
        EXPECT_EQ(node[0][0].val(), "1");
        EXPECT_EQ(node[0][1].key(), "b");
        EXPECT_EQ(node[0][1].val(), "2");
    };
    test_bom(GetParam(), mkyaml, test);
}

TEST_P(TestBOM, github559_9)
{
    auto mkyaml = [](bomspec const& bom1){
        std::string yaml(bom1.bom.str, bom1.bom.len);
        yaml.append("- ? key\n  : val\n  ? key1\n  : val1\n");
        return yaml;
    };
    auto test = [](Parser const& parser, ConstNodeRef const& node, bomspec const& bom){
        EXPECT_EQ(parser.encoding(), bom.encoding);
        ASSERT_TRUE(node.is_seq());
        ASSERT_EQ(node.num_children(), 1);
        ASSERT_TRUE(node[0].is_map());
        ASSERT_EQ(node[0].num_children(), 2);
        EXPECT_EQ(node[0][0].key(), "key");
        EXPECT_EQ(node[0][0].val(), "val");
        EXPECT_EQ(node[0][1].key(), "key1");
        EXPECT_EQ(node[0][1].val(), "val1");
    };
    test_bom(GetParam(), mkyaml, test);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST_P(TestBOM2, bom_doc_bom_0)
{
    auto mkyaml = [](bomspec const& bom1, bomspec const& bom2){
        return mkstr("{}\n---\n{}abc", bom1.bom, bom2.bom);
    };
    auto test = [](Parser const& parser, ConstNodeRef const& node, bomspec const& bom1, bomspec const&){
        EXPECT_EQ(parser.encoding(), bom1.encoding);
        EXPECT_EQ(node[0].val(), "abc");
    };
    test_bom2(GetParam(), mkyaml, test);
}

TEST_P(TestBOM2, bom_doc_bom_1)
{
    auto mkyaml = [](bomspec const& bom1, bomspec const& bom2){
        return mkstr("{}---\n{}\nabc", bom1.bom, bom2.bom);
    };
    auto test = [](Parser const& parser, ConstNodeRef const& node, bomspec const& bom1, bomspec const&){
        EXPECT_EQ(parser.encoding(), bom1.encoding);
        EXPECT_EQ(node[0].val(), "abc");
    };
    test_bom2(GetParam(), mkyaml, test);
}

TEST_P(TestBOM2, bom_doc_bom_2)
{
    auto mkyaml = [](bomspec const& bom1, bomspec const& bom2){
        return mkstr("{}---\n{}\nabc", bom1.bom, bom2.bom);
    };
    auto test = [](Parser const& parser, ConstNodeRef const& node, bomspec const& bom1, bomspec const&){
        EXPECT_EQ(parser.encoding(), bom1.encoding);
        EXPECT_EQ(node[0].val(), "abc");
    };
    test_bom2(GetParam(), mkyaml, test);
}

TEST_P(TestBOM2, bom_doc_bom_3)
{
    auto mkyaml = [](bomspec const& bom1, bomspec const& bom2){
        return mkstr("{}---{} abc", bom1.bom, bom2.bom);
    };
    auto test = [](Parser const& parser, ConstNodeRef const& node, bomspec const& bom1, bomspec const& bom2){
        EXPECT_EQ(parser.encoding(), bom1.encoding);
        if(bom2.name == "NOBOM")
        {
            EXPECT_EQ(node[0].val(), "abc");
        }
        else
        {
            std::string expected = mkstr("---{} abc", bom2.bom);
            EXPECT_EQ(node.val(), expected);
        }
    };
    test_bom2(GetParam(), mkyaml, test, bom2_err_none);
}


//-----------------------------------------------------------------------------

TEST_P(TestBOM2, bom_scalar_doc_bom_scalar_1)
{
    auto mkyaml = [](bomspec const& bom1, bomspec const& bom2){
        return mkstr("{}abc\n---\n{}def\n", bom1.bom, bom2.bom);
    };
    auto test = [](Parser const& parser, ConstNodeRef const& node, bomspec const& bom1, bomspec const&){
        EXPECT_EQ(parser.encoding(), bom1.encoding);
        ASSERT_EQ(node.num_children(), 2);
        EXPECT_EQ(node[0].val(), "abc");
        EXPECT_EQ(node[1].val(), "def");
    };
    test_bom2(GetParam(), mkyaml, test);
}

namespace {
static void test_bom_scalar_doc_bom_scalar_2_fn(Parser const& parser, ConstNodeRef const& node, bomspec const& bom1, bomspec const& bom2)
{
    EXPECT_EQ(parser.encoding(), bom1.encoding);
    if(bom1.name == "NOBOM")
    {
        if(bom2.name == "NOBOM")
        {
            ASSERT_EQ(node.num_children(), 2);
            EXPECT_EQ(node[0].val(), "abc");
            EXPECT_EQ(node[1].val(), "def");
        }
        else
        {
            ASSERT_EQ(node.num_children(), 1);
            EXPECT_EQ(node[0].val(), mkstr("abc ---{} def", bom2.bom));
        }
    }
    else
    {
        if(bom2.name == "NOBOM")
        {
            ASSERT_EQ(node.num_children(), 2);
            EXPECT_EQ(node[0].val(), mkstr("---{} abc", bom1.bom));
            EXPECT_EQ(node[1].val(), "def");
        }
        else
        {
            ASSERT_TRUE(node.is_val());
            EXPECT_EQ(node.val(), mkstr("---{} abc ---{} def", bom1.bom, bom2.bom));
        }
    }
}
} // namespace anon

TEST_P(TestBOM2, bom_scalar_doc_bom_scalar_2)
{
    auto mkyaml = [](bomspec const& bom1, bomspec const& bom2){
        return mkstr("{}---{} abc\n---{} def\n", bom1.bom, bom1.bom, bom2.bom);
    };
    test_bom2(GetParam(), mkyaml, test_bom_scalar_doc_bom_scalar_2_fn, bom2_err_none);
}

TEST_P(TestBOM2, bom_scalar_doc_bom_scalar_3)
{
    auto mkyaml = [](bomspec const& bom1, bomspec const& bom2){
        return mkstr("{}---{}\nabc\n---{}\ndef\n", bom1.bom, bom1.bom, bom2.bom);
    };
    test_bom2(GetParam(), mkyaml, test_bom_scalar_doc_bom_scalar_2_fn, bom2_err_none);
}


//-------------------------------------------
// this is needed to use the test case library
Case const* get_case(csubstr /*name*/)
{
    return nullptr;
}

} // namespace yml
} // namespace c4
