#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

namespace c4 {
namespace yml {

//-----------------------------------------------------------------------------

ENGINE_TEST(Qmrk0,
            (HAS_CONTAINER_KEYS, Location(41,3,19),
             "\n"
             "a simple key: a value\n"
             "? an explicit key: another value\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :a simple key\n"
            "=VAL :a value\n"
            "+MAP\n"
            "=VAL :an explicit key\n"
            "=VAL :another value\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a simple key"));
    ___(ps.set_val_scalar_plain("a value"));
    ___(ps.add_sibling());
    ___(ps.begin_map_key_block());
        ___(ps.set_key_scalar_plain("an explicit key"));
        ___(ps.set_val_scalar_plain("another value"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk1_0,
            ("? an explicit key\n"
             "a simple key: a value\n",
             "an explicit key: \n"
             "a simple key: a value\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :an explicit key\n"
            "=VAL :\n"
            "=VAL :a simple key\n"
            "=VAL :a value\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("an explicit key"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("a simple key"));
    ___(ps.set_val_scalar_plain("a value"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk1_1,
            (HAS_CONTAINER_KEYS, Location(19,2,19),
             "\n"
             "? an explicit key: another value\n"
             "a simple key: a value\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL :an explicit key\n"
            "=VAL :another value\n"
            "-MAP\n"
            "=VAL :\n"
            "=VAL :a simple key\n"
            "=VAL :a value\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
        ___(ps.set_key_scalar_plain("an explicit key"));
        ___(ps.set_val_scalar_plain("another value"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("a simple key"));
    ___(ps.set_val_scalar_plain("a value"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk1_2,
            (HAS_CONTAINER_KEYS, Location(25,2,21),
             "map:\n"
             "  ? an explicit key: another value\n"
             "  a simple key: a value\n"
             "? an explicit key deindented: its value\n"
             "? more: siblings\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :map\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL :an explicit key\n"
            "=VAL :another value\n"
            "-MAP\n"
            "=VAL :\n"
            "=VAL :a simple key\n"
            "=VAL :a value\n"
            "-MAP\n"
            "+MAP\n"
            "=VAL :an explicit key deindented\n"
            "=VAL :its value\n"
            "-MAP\n"
            "=VAL :\n"
            "+MAP\n"
            "=VAL :more\n"
            "=VAL :siblings\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
        ___(ps.set_key_scalar_plain("map"));
        ___(ps.begin_map_val_block());
            ___(ps.begin_map_key_block());
                ___(ps.set_key_scalar_plain("an explicit key"));
                ___(ps.set_val_scalar_plain("another value"));
            ___(ps.end_map());
            ___(ps.set_val_scalar_plain({}));
            ___(ps.add_sibling());
            ___(ps.set_key_scalar_plain("a simple key"));
            ___(ps.set_val_scalar_plain("a value"));
        ___(ps.end_map());
        ___(ps.add_sibling());
        ___(ps.begin_map_key_block());
            ___(ps.set_key_scalar_plain("an explicit key deindented"));
            ___(ps.set_key_scalar_plain("its value"));
        ___(ps.end_map());
        ___(ps.set_val_scalar_plain({}));
        ___(ps.add_sibling());
        ___(ps.begin_map_key_block());
            ___(ps.set_key_scalar_plain("more"));
            ___(ps.set_key_scalar_plain("siblings"));
        ___(ps.end_map());
        ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk2_0,
            ("? an explicit key\n"
             "...\n"
             "? another explicit key\n",
             "---\n"
             "an explicit key: \n"
             "---\n"
             "another explicit key: \n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :an explicit key\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC ...\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :another explicit key\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("an explicit key"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc_expl());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("another explicit key"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk2_1,
            ("? an explicit key\n"
             "---\n"
             "? another explicit key\n",
             "---\n"
             "an explicit key: \n"
             "---\n"
             "another explicit key: \n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :an explicit key\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "+DOC ---\n"
            "+MAP\n"
            "=VAL :another explicit key\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("an explicit key"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.begin_doc_expl());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("another explicit key"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk3,
            ("{ ? an explicit key, ? foo,? bar,?baz:,?bat}",
             "{an explicit key: ,foo: ,bar: ,?baz: ,?bat: }"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :an explicit key\n"
            "=VAL :\n"
            "=VAL :foo\n"
            "=VAL :\n"
            "=VAL :bar\n"
            "=VAL :\n"
            "=VAL :?baz\n"
            "=VAL :\n"
            "=VAL :?bat\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("an explicit key"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("bar"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("?baz"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("?bat"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk4_0,
            ("[?baz:,]",
             "[{?baz: }]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :?baz\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("?baz"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk4,
            ("[ ? an explicit key, ? foo,? bar,?baz:,?bat]",
             "[{an explicit key: },{foo: },{bar: },{?baz: },?bat]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :an explicit key\n"
            "=VAL :\n"
            "-MAP\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL :\n"
            "-MAP\n"
            "+MAP {}\n"
            "=VAL :bar\n"
            "=VAL :\n"
            "-MAP\n"
            "+MAP {}\n"
            "=VAL :?baz\n"
            "=VAL :\n"
            "-MAP\n"
            "=VAL :?bat\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("an explicit key"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("foo"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("bar"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("?baz"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("?bat"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk5,
            (HAS_CONTAINER_KEYS, Location(4,1,5),
             "? a: b\n"
             "?\n"
             "?\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "=VAL :\n"
            "=VAL :\n"
            "=VAL :\n"
            "=VAL :\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain({}));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain({}));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST(Qmrk6,
            (HAS_CONTAINER_KEYS, Location(21,2,21),
             "\n"
             "- ? an explicit key: another value\n"
             "  a simple key: a value\n"
             "- ? another explicit key: its value\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL :an explicit key\n"
            "=VAL :another value\n"
            "-MAP\n"
            "=VAL :\n"
            "=VAL :a simple key\n"
            "=VAL :a value\n"
            "-MAP\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL :another explicit key\n"
            "=VAL :its value\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
        ___(ps.begin_map_val_block());
            ___(ps.begin_map_key_block());
                ___(ps.set_key_scalar_plain("an explicit key"));
                ___(ps.set_val_scalar_plain("another value"));
            ___(ps.end_map());
            ___(ps.set_val_scalar_plain({}));
            ___(ps.add_sibling());
            ___(ps.set_key_scalar_plain("a simple key"));
            ___(ps.set_val_scalar_plain("a value"));
        ___(ps.end_map());
        ___(ps.add_sibling());
        ___(ps.begin_map_val_block());
            ___(ps.begin_map_key_block());
                ___(ps.set_key_scalar_plain("another explicit key"));
                ___(ps.set_val_scalar_plain("its value"));
            ___(ps.end_map());
            ___(ps.set_val_scalar_plain({}));
        ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk7,
            (HAS_CONTAINER_KEYS, Location(5,2,5),
             "\n"
             "- ? : x\n"
             "- ? : \n"
             "- ? :\n"),
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL :\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL :\n"
            "=VAL :\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "+MAP\n"
            "+MAP\n"
            "=VAL :\n"
            "=VAL :\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
        ___(ps.begin_map_val_block());
            ___(ps.begin_map_key_block());
                ___(ps.set_key_scalar_plain({}));
                ___(ps.set_val_scalar_plain("x"));
            ___(ps.end_map());
            ___(ps.set_val_scalar_plain({}));
        ___(ps.end_map());
        ___(ps.add_sibling());
        ___(ps.begin_map_val_block());
            ___(ps.begin_map_key_block());
                ___(ps.set_key_scalar_plain({}));
                ___(ps.set_val_scalar_plain({}));
            ___(ps.end_map());
            ___(ps.set_val_scalar_plain({}));
        ___(ps.end_map());
        ___(ps.add_sibling());
        ___(ps.begin_map_val_block());
            ___(ps.begin_map_key_block());
                ___(ps.set_key_scalar_plain({}));
                ___(ps.set_val_scalar_plain({}));
            ___(ps.end_map());
            ___(ps.set_val_scalar_plain({}));
        ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkWithTags,
            (HAS_CONTAINER_KEYS, Location(18,3,6),
             "a1: b1\n"
             "? !at\n"
             " !bt : b2\n"
             ": c3\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :a1\n"
            "=VAL :b1\n"
            "+MAP <!at>\n"
            "=VAL <!bt> :\n"
            "=VAL :b2\n"
            "-MAP\n"
            "=VAL :c3\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("a1"));
    ___(ps.set_val_scalar_plain("b1"));
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!at"));
    ___(ps.begin_map_key_block());
        ___(ps.set_key_tag("!bt"));
        ___(ps.set_key_scalar_plain({}));
        ___(ps.set_val_scalar_plain("b2"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain("c3"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST(QmrkGithub524_0,
            ("? outer\n"
             ": inner1: 1\n"
             "  inner2: 2\n",
             "outer:\n"
             "  inner1: 1\n"
             "  inner2: 2\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :outer\n"
            "+MAP\n"
            "=VAL :inner1\n"
            "=VAL :1\n"
            "=VAL :inner2\n"
            "=VAL :2\n"
            "-MAP\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("outer"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("inner1"));
    ___(ps.set_val_scalar_plain("1"));
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("inner2"));
    ___(ps.set_val_scalar_plain("2"));
    ___(ps.end_map());
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkGithub524_1,
            ("? outer\n"
             ": inner1: 1\n"
             "inner2: 2\n"
             ,
             "outer:\n"
             "  inner1: 1\n"
             "inner2: 2\n"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "=VAL :outer\n"
            "+MAP\n"
            "=VAL :inner1\n"
            "=VAL :1\n"
            "-MAP\n"
            "=VAL :inner2\n"
            "=VAL :2\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
            )
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("outer"));
    ___(ps.begin_map_val_block());
    ___(ps.set_key_scalar_plain("inner1"));
    ___(ps.set_val_scalar_plain("1"));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain("inner2"));
    ___(ps.set_val_scalar_plain("2"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(QmrkFlow0,
            ("{ ? }", "{: }"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain({}));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1,
            ("{ ? , }", "{: }"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain({}));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1Anch,
            ("{ ? &anch , }", "{&anch : }"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL &anch :\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anch"));
    ___(ps.set_key_scalar_plain({}));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1Tag,
            ("{ ? !tag , }", "{!tag : }"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL <!tag> :\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_tag("!tag"));
    ___(ps.set_key_scalar_plain({}));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1Squo,
            ("{ ? 'squo', }", "{'squo': }"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL 'squo\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_squoted("squo"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1AnchSquo,
            ("{ ? &anch 'squo', }", "{&anch 'squo': }"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL &anch 'squo\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anch"));
    ___(ps.set_key_scalar_squoted("squo"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1TagSquo,
            ("{ ? !tag 'squo', }", "{!tag 'squo': }"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL <!tag> 'squo\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_tag("!tag"));
    ___(ps.set_key_scalar_squoted("squo"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1Dquo,
            ("{ ? \"dquo\", }", "{\"dquo\": }"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL \"dquo\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_dquoted("dquo"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1AnchDquo,
            ("{ ? &anch \"dquo\", }", "{&anch \"dquo\": }"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL &anch \"dquo\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anch"));
    ___(ps.set_key_scalar_dquoted("dquo"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1TagDquo,
            ("{ ? !tag \"dquo\", }", "{!tag \"dquo\": }"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL <!tag> \"dquo\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_tag("!tag"));
    ___(ps.set_key_scalar_dquoted("dquo"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1Ref,
            ("{ ? *ref , }", "{*ref : }"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=ALI *ref\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_ref("*ref"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1Seq,
            (HAS_CONTAINER_KEYS,
             "{ ? [a, b]: c , }", "{? [a, b] : c}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+SEQ []\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-SEQ\n"
            "=VAL :c\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_seq());
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1SeqAnchor,
            (HAS_CONTAINER_KEYS,
             "{ ? &anchor [a, b]: c , }", "{? &anchor [a, b] : c}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+SEQ [] &anchor\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-SEQ\n"
            "=VAL :c\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anchor"));
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_seq());
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1SeqTag,
            (HAS_CONTAINER_KEYS,
             "{ ? !tag [a, b]: c , }", "{? !tag [a, b] : c}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+SEQ [] <!tag>\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-SEQ\n"
            "=VAL :c\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_tag("!tag"));
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_seq());
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1Map,
            (HAS_CONTAINER_KEYS,
             "{ ? {a: b}: c , }", "{? {a: b} : c}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "=VAL :c\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1MapAnchor,
            (HAS_CONTAINER_KEYS,
             "{ ? &anchor {a: b}: c , }", "{? &anchor {a: b} : c}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+MAP {} &anchor\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "=VAL :c\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anchor"));
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkFlow1MapTag,
            (HAS_CONTAINER_KEYS,
             "{ ? !tag {a: b}: c , }", "{? !tag {a: b} : c}"),
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "+MAP {} <!tag>\n"
            "=VAL :a\n"
            "=VAL :b\n"
            "-MAP\n"
            "=VAL :c\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_tag("!tag"));
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain("c"));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

#ifdef TODO_FIXME // this is the only failing suite test
ENGINE_TEST(QmrkTestSuiteM2N8_01_0,
            (HAS_CONTAINER_KEYS,
             "? []: x"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+SEQ []\n"
            "-SEQ\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_seq_key_flow());
    ___(ps.end_seq());
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(QmrkTestSuiteM2N8_01_1,
            (HAS_CONTAINER_KEYS,
             "? {}: x"),
            "+STR\n"
            "+DOC\n"
            "+MAP\n"
            "+MAP\n"
            "+MAP {}\n"
            "-MAP\n"
            "=VAL :x\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n"
)
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_block());
    ___(ps.begin_map_key_block());
    ___(ps.begin_map_key_flow());
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain("x"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
#endif

} // namespace yml
} // namespace c4


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// this is needed to use the test case library

#ifndef RYML_SINGLE_HEADER
#include "c4/substr.hpp"
#endif

namespace c4 {
namespace yml {
struct Case;
Case const* get_case(csubstr /*name*/)
{
    return nullptr;
}
} // namespace yml
} // namespace c4
