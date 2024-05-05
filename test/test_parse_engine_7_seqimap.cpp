#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

namespace c4 {
namespace yml {


//-----------------------------------------------------------------------------

ENGINE_TEST(SeqIMap0Ref,
            ("[val0,val1,{key: val2},wtf]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :val0\n"
            "=VAL :val1\n"
            "+MAP {}\n"
            "=VAL :key\n"
            "=VAL :val2\n"
            "-MAP\n"
            "=VAL :wtf\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("val0"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("key"));
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("wtf"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap1,
            ("[val0,val1,key: val2,wtf]", "[val0,val1,{key: val2},wtf]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :val0\n"
            "=VAL :val1\n"
            "+MAP {}\n"
            "=VAL :key\n"
            "=VAL :val2\n"
            "-MAP\n"
            "=VAL :wtf\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("val0"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
        // first this is read as a val because the parent is read as a
        // seq...
    ___(ps.set_val_scalar_plain("key"));
        // ... then this will create a flow map, and move the existing
        // val to the key of the first child
    ___(ps.actually_val_is_first_key_of_new_map_flow());
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("wtf"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap2,
            ("[val0,val1,key: [foo: bar],wtf]", "[val0,val1,{key: [{foo: bar}]},wtf]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :val0\n"
            "=VAL :val1\n"
            "+MAP {}\n"
            "=VAL :key\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "-SEQ\n"
            "-MAP\n"
            "=VAL :wtf\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
      ___(ps.set_val_scalar_plain("val0"));
      ___(ps.add_sibling());
      ___(ps.set_val_scalar_plain("val1"));
      ___(ps.add_sibling());
      ___(ps.set_val_scalar_plain("key"));
      ___(ps.actually_val_is_first_key_of_new_map_flow());
        ___(ps.begin_seq_val_flow());
        ___(ps.set_val_scalar_plain("foo"));
        ___(ps.actually_val_is_first_key_of_new_map_flow());
        ___(ps.set_val_scalar_plain("bar"));
        ___(ps.end_map());
        ___(ps.end_seq());
      ___(ps.end_map());
      ___(ps.add_sibling());
      ___(ps.set_val_scalar_plain("wtf"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST(SeqIMap2Nested,
            ("[val: [0: [1: [2: [3: [4: [5: [6: 7]]]]]]], :]",
             "[{val: [{0: [{1: [{2: [{3: [{4: [{5: [{6: 7}]}]}]}]}]}]}]},{: }]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :val\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :0\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :1\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :2\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :3\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :4\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :5\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :6\n"
            "=VAL :7\n"
            "-MAP\n"
            "-SEQ\n"
            "-MAP\n"
            "-SEQ\n"
            "-MAP\n"
            "-SEQ\n"
            "-MAP\n"
            "-SEQ\n"
            "-MAP\n"
            "-SEQ\n"
            "-MAP\n"
            "-SEQ\n"
            "-MAP\n"
            "-SEQ\n"
            "-MAP\n"
            "+MAP {}\n"
            "=VAL :\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
      ___(ps.begin_seq_val_flow());
        ___(ps.set_val_scalar_plain("val"));
        ___(ps.actually_val_is_first_key_of_new_map_flow());
          ___(ps.begin_seq_val_flow());
            ___(ps.set_val_scalar_plain("0"));
            ___(ps.actually_val_is_first_key_of_new_map_flow());
              ___(ps.begin_seq_val_flow());
                ___(ps.set_val_scalar_plain("1"));
                ___(ps.actually_val_is_first_key_of_new_map_flow());
                  ___(ps.begin_seq_val_flow());
                    ___(ps.set_val_scalar_plain("2"));
                    ___(ps.actually_val_is_first_key_of_new_map_flow());
                      ___(ps.begin_seq_val_flow());
                        ___(ps.set_val_scalar_plain("3"));
                        ___(ps.actually_val_is_first_key_of_new_map_flow());
                          ___(ps.begin_seq_val_flow());
                            ___(ps.set_val_scalar_plain("4"));
                            ___(ps.actually_val_is_first_key_of_new_map_flow());
                              ___(ps.begin_seq_val_flow());
                                ___(ps.set_val_scalar_plain("5"));
                                ___(ps.actually_val_is_first_key_of_new_map_flow());
                                  ___(ps.begin_seq_val_flow());
                                    ___(ps.set_val_scalar_plain("6"));
                                    ___(ps.actually_val_is_first_key_of_new_map_flow());
                                      ___(ps.set_val_scalar_plain("7"));
                                    ___(ps.end_map());
                                  ___(ps.end_seq());
                                ___(ps.end_map());
                              ___(ps.end_seq());
                            ___(ps.end_map());
                          ___(ps.end_seq());
                        ___(ps.end_map());
                      ___(ps.end_seq());
                    ___(ps.end_map());
                  ___(ps.end_seq());
                ___(ps.end_map());
              ___(ps.end_seq());
            ___(ps.end_map());
          ___(ps.end_seq());
        ___(ps.end_map());
        ___(ps.add_sibling());
        ___(ps.set_val_scalar_plain({}));
        ___(ps.actually_val_is_first_key_of_new_map_flow());
          ___(ps.set_val_scalar_plain({}));
        ___(ps.end_map());
      ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


ENGINE_TEST(SeqIMap3EmptyKey,
            ("[val0, : wtf]", "[val0,{: wtf}]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :val0\n"
            "+MAP {}\n"
            "=VAL :\n"
            "=VAL :wtf\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("val0"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.actually_val_is_first_key_of_new_map_flow());
    ___(ps.set_val_scalar_plain("wtf"));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap3EmptyVal,
            ("[val0, wtf: ]", "[val0,{wtf: }]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :val0\n"
            "+MAP {}\n"
            "=VAL :wtf\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("val0"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("wtf"));
    ___(ps.actually_val_is_first_key_of_new_map_flow());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap3EmptyKeyVal,
            ("[val0, : ]", "[val0,{: }]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :val0\n"
            "+MAP {}\n"
            "=VAL :\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("val0"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.actually_val_is_first_key_of_new_map_flow());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap3EmptyKeyValNested,
            ("[val0: val1, : ]", "[{val0: val1},{: }]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :val0\n"
            "=VAL :val1\n"
            "-MAP\n"
            "+MAP {}\n"
            "=VAL :\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("val0"));
    ___(ps.actually_val_is_first_key_of_new_map_flow());
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.actually_val_is_first_key_of_new_map_flow());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap3EmptyKeyValNested2,
            ("[val0: [val1: val2], : ]", "[{val0: [{val1: val2}]},{: }]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :val0\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :val1\n"
            "=VAL :val2\n"
            "-MAP\n"
            "-SEQ\n"
            "-MAP\n"
            "+MAP {}\n"
            "=VAL :\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("val0"));
    ___(ps.actually_val_is_first_key_of_new_map_flow());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.actually_val_is_first_key_of_new_map_flow());
    ___(ps.set_val_scalar_plain("val2"));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.actually_val_is_first_key_of_new_map_flow());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap4Ref,
            ("[val0,val1,[rat,rot],wtf]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :val0\n"
            "=VAL :val1\n"
            "+SEQ []\n"
            "=VAL :rat\n"
            "=VAL :rot\n"
            "-SEQ\n"
            "=VAL :wtf\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("val0"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("rat"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("rot"));
    ___(ps.end_seq());
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("wtf"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap4,
            (HAS_CONTAINER_KEYS, Location(20,1,21), "[val0,val1,[rat,rot]: [foo: bar],wtf]", "[val0,val1,{[rat,rot]: [{foo: bar}]},wtf]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :val0\n"
            "=VAL :val1\n"
            "+MAP {}\n"
            "+SEQ []\n"
            "=VAL :rat\n"
            "=VAL :rot\n"
            "-SEQ\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "-SEQ\n"
            "-MAP\n"
            "=VAL :wtf\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_val_scalar_plain("val0"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("val1"));
    ___(ps.add_sibling());
    ___(ps.begin_map_val_flow());
      ___(ps.begin_seq_key_flow());
      ___(ps.set_val_scalar_plain("rat"));
      ___(ps.add_sibling());
      ___(ps.set_val_scalar_plain("rot"));
      ___(ps.end_seq());
      ___(ps.begin_seq_val_flow());
        ___(ps.begin_map_val_flow());
          ___(ps.set_key_scalar_plain("foo"));
          ___(ps.set_val_scalar_plain("bar"));
        ___(ps.end_map());
      ___(ps.end_seq());
    ___(ps.end_map());
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("wtf"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap4_actually,
            (HAS_CONTAINER_KEYS, Location(20,1,21), "[val0,val1,[rat,rot]: [foo: bar],wtf]", "[val0,val1,{[rat,rot]: [{foo: bar}]},wtf]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "=VAL :val0\n"
            "=VAL :val1\n"
            "+MAP {}\n"
            "+SEQ []\n"
            "=VAL :rat\n"
            "=VAL :rot\n"
            "-SEQ\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :foo\n"
            "=VAL :bar\n"
            "-MAP\n"
            "-SEQ\n"
            "-MAP\n"
            "=VAL :wtf\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
       ___(ps.set_val_scalar_plain("val0"));
       ___(ps.add_sibling());
       ___(ps.set_val_scalar_plain("val1"));
       ___(ps.add_sibling());
       ___(ps.begin_seq_val_flow());
           ___(ps.set_val_scalar_plain("rat"));
           ___(ps.add_sibling());
           ___(ps.set_val_scalar_plain("rot"));
       ___(ps.end_seq());
       ___(ps.actually_val_is_first_key_of_new_map_flow());
           ___(ps.begin_seq_val_flow());
               ___(ps.set_val_scalar_plain("foo"));
               ___(ps.actually_val_is_first_key_of_new_map_flow());
                   ___(ps.set_val_scalar_plain("bar"));
               ___(ps.end_map());
           ___(ps.end_seq());
       ___(ps.end_map());
       ___(ps.add_sibling());
       ___(ps.set_val_scalar_plain("wtf"));
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST(SeqIMap5Squo,
            ("[a: &anchor 'b']", "[{a: &anchor 'b'}]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL &anchor 'b\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_anchor("anchor"));
    ___(ps.set_val_scalar_squoted("b"));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap5Dquo,
            ("[a: &anchor \"b\"]", "[{a: &anchor \"b\"}]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=VAL &anchor \"b\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_anchor("anchor"));
    ___(ps.set_val_scalar_dquoted("b"));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap5Ref,
            ("[a: *ref]", "[{a: *ref}]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=ALI *ref\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_ref("*ref"));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap5QmrkNone0,
            ("[? ,]", "[{: }]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :\n"
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
    ___(ps.set_key_scalar_plain({}));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap5QmrkNone1,
            ("[? ]", "[{: }]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :\n"
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
    ___(ps.set_key_scalar_plain({}));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap5QmrkSquo1,
            ("[? &anchor 'squo' ]", "[{&anchor 'squo': }]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL &anchor 'squo\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_key_tag("!tag"));
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anchor"));
    ___(ps.set_key_scalar_squoted("squo"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap5QmrkDquo1,
            ("[? &anchor \"dquo\" ]", "[{&anchor \"dquo\": }]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL &anchor \"dquo\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_key_tag("!tag"));
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anchor"));
    ___(ps.set_key_scalar_dquoted("dquo"));
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap5QmrkRef1,
            ("[? a: *ref]", "[{a: *ref}]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :a\n"
            "=ALI *ref\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_ref("*ref"));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap5QmrkRef2,
            ("[? *ref: b]", "[{*ref : b}]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=ALI *ref\n"
            "=VAL :b\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_ref("*ref"));
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap5QmrkSeq,
            (HAS_CONTAINER_KEYS,
             "[? &anchor [a, seq]: ]", "[{&anchor [a, seq]: }]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "+SEQ [] &anchor\n"
            "=VAL :a\n"
            "=VAL :seq\n"
            "-SEQ\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_key_tag("!tag"));
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anchor"));
    ___(ps.begin_seq_key_flow());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("seq"));
    ___(ps.end_seq());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(SeqIMap5QmrkMap,
            (HAS_CONTAINER_KEYS,
             "[? &anchor {a: map}: ]", "[{&anchor {a: map}: }]"),
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "+MAP {} &anchor\n"
            "=VAL :a\n"
            "=VAL :map\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.set_key_tag("!tag"));
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anchor"));
    ___(ps.begin_map_key_flow());
    ___(ps.set_key_scalar_plain("a"));
    ___(ps.set_val_scalar_plain("map"));
    ___(ps.end_map());
    ___(ps.set_val_scalar_plain({}));
    ___(ps.end_map());
    ___(ps.end_seq());
    ___(ps.end_doc());
    ___(ps.end_stream());
}

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
