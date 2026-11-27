#ifndef RYML_SINGLE_HEADER
#include "c4/yml/std/std.hpp"
#include "c4/yml/parse.hpp"
#include "c4/yml/emit.hpp"
#include <c4/format.hpp>
#include <c4/format_base64.hpp>
#include <c4/yml/detail/checks.hpp>
#include <c4/yml/detail/print.hpp>
#endif
#include "./test_lib/test_helpers_tree.hpp"

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable: 4389) // signed/unsigned mismatch
#elif defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wdollar-in-identifier-extension"
#elif defined(__GNUC__)
#   pragma GCC diagnostic push
#endif

// inf and nan are tested in test_number.cpp

RYML_DEFINE_TEST_MAIN()

using substr = c4::substr;
using csubstr = c4::csubstr;

//-------------------------------------------

namespace c4 {
namespace yml {

TEST(serialize, base64_2)
{
    csubstr yaml = R"(str: Brevity is the soul of wit.
base64: QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu
QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu: base64
)";
    Tree t = parse_in_arena(yaml);
    NodeRef mr = t;
    ConstNodeRef r = t;
    ASSERT_TRUE(r.type().is_map());
    ASSERT_EQ(r[0].key(), "str");
    ASSERT_EQ(r[0].val(), "Brevity is the soul of wit.");
    ASSERT_EQ(r[1].key(), "base64");
    ASSERT_EQ(r[1].val(), "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
    ASSERT_EQ(r[2].key(), "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
    ASSERT_EQ(r[2].val(), "base64");
    {
        std::string s, fallback = "fallback";
        s = {}; ASSERT_TRUE( r[0].deserialize_key(&s)); EXPECT_EQ(s,  r[0].key()); EXPECT_EQ(s, "str");
        s = {}; ASSERT_TRUE(mr[0].deserialize_key(&s)); EXPECT_EQ(s, mr[0].key()); EXPECT_EQ(s, "str");
        s = {}; ASSERT_TRUE( r[0].deserialize(&s));     EXPECT_EQ(s,  r[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE(mr[0].deserialize(&s));     EXPECT_EQ(s, mr[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE( r[0].parent().deserialize_child(0, &s)); EXPECT_EQ(s,  r[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE(mr[0].parent().deserialize_child(0, &s)); EXPECT_EQ(s, mr[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE( r[0].parent().deserialize_child(0, &s, fallback)); EXPECT_EQ(s,  r[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE(mr[0].parent().deserialize_child(0, &s, fallback)); EXPECT_EQ(s, mr[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE( r[0].parent().deserialize_child(20, &s, fallback)); EXPECT_NE(s,  r[0].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE(mr[0].parent().deserialize_child(20, &s, fallback)); EXPECT_NE(s, mr[0].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE( r[0].parent().deserialize_child("str", &s)); EXPECT_EQ(s,  r[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE(mr[0].parent().deserialize_child("str", &s)); EXPECT_EQ(s, mr[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE( r[0].parent().deserialize_child("str", &s, fallback)); EXPECT_EQ(s,  r[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE(mr[0].parent().deserialize_child("str", &s, fallback)); EXPECT_EQ(s, mr[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE( r[0].parent().deserialize_child("!!!", &s, fallback)); EXPECT_NE(s,  r[0].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE(mr[0].parent().deserialize_child("!!!", &s, fallback)); EXPECT_NE(s, mr[0].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE( r[1].deserialize_key(&s)); EXPECT_EQ(s,  r[1].key()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(mr[1].deserialize_key(&s)); EXPECT_EQ(s, mr[1].key()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE( r[1].deserialize(&s));     EXPECT_EQ(s,  r[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(mr[1].deserialize(&s));     EXPECT_EQ(s, mr[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE( r[1].parent().deserialize_child(1, &s)); EXPECT_EQ(s,  r[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(mr[1].parent().deserialize_child(1, &s)); EXPECT_EQ(s, mr[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE( r[1].parent().deserialize_child(1, &s, fallback)); EXPECT_EQ(s,  r[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(mr[1].parent().deserialize_child(1, &s, fallback)); EXPECT_EQ(s, mr[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE( r[1].parent().deserialize_child(20, &s, fallback)); EXPECT_NE(s,  r[1].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE(mr[1].parent().deserialize_child(20, &s, fallback)); EXPECT_NE(s, mr[1].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE( r[1].parent().deserialize_child("base64", &s)); EXPECT_EQ(s,  r[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(mr[1].parent().deserialize_child("base64", &s)); EXPECT_EQ(s, mr[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE( r[1].parent().deserialize_child("base64", &s, fallback)); EXPECT_EQ(s,  r[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(mr[1].parent().deserialize_child("base64", &s, fallback)); EXPECT_EQ(s, mr[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE( r[1].parent().deserialize_child("!!!!!!", &s, fallback)); EXPECT_NE(s,  r[1].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE(mr[1].parent().deserialize_child("!!!!!!", &s, fallback)); EXPECT_NE(s, mr[1].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE( r[2].deserialize_key(&s)); EXPECT_EQ(s,  r[2].key()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(mr[2].deserialize_key(&s)); EXPECT_EQ(s, mr[2].key()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE( r[2].deserialize(&s));     EXPECT_EQ(s,  r[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(mr[2].deserialize(&s));     EXPECT_EQ(s, mr[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE( r[2].parent().deserialize_child(2, &s)); EXPECT_EQ(s,  r[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(mr[2].parent().deserialize_child(2, &s)); EXPECT_EQ(s, mr[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE( r[2].parent().deserialize_child(2, &s, fallback)); EXPECT_EQ(s,  r[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(mr[2].parent().deserialize_child(2, &s, fallback)); EXPECT_EQ(s, mr[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE( r[2].parent().deserialize_child(20, &s, fallback)); EXPECT_NE(s,  r[2].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE(mr[2].parent().deserialize_child(20, &s, fallback)); EXPECT_NE(s, mr[2].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE( r[2].parent().deserialize_child("QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu", &s)); EXPECT_EQ(s,  r[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(mr[2].parent().deserialize_child("QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu", &s)); EXPECT_EQ(s, mr[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE( r[2].parent().deserialize_child("QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu", &s, fallback)); EXPECT_EQ(s,  r[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(mr[2].parent().deserialize_child("QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu", &s, fallback)); EXPECT_EQ(s, mr[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE( r[2].parent().deserialize_child("!!!!dml0eSBpcyB0aGUgc291bCBvZiB3aXQu", &s, fallback)); EXPECT_NE(s,  r[2].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE(mr[2].parent().deserialize_child("!!!!dml0eSBpcyB0aGUgc291bCBvZiB3aXQu", &s, fallback)); EXPECT_NE(s, mr[2].val()); EXPECT_EQ(s, "fallback");
        //
        s = {}; ASSERT_TRUE(t.deserialize_key( r[0].id(), &s)); EXPECT_EQ(s,  r[0].key()); EXPECT_EQ(s, "str");
        s = {}; ASSERT_TRUE(t.deserialize_key(mr[0].id(), &s)); EXPECT_EQ(s, mr[0].key()); EXPECT_EQ(s, "str");
        s = {}; ASSERT_TRUE(t.deserialize( r[0].id(), &s));     EXPECT_EQ(s,  r[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE(t.deserialize(mr[0].id(), &s));     EXPECT_EQ(s, mr[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[0].parent().id(), 0, &s)); EXPECT_EQ(s,  r[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[0].parent().id(), 0, &s)); EXPECT_EQ(s, mr[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[0].parent().id(), 0, &s, fallback)); EXPECT_EQ(s,  r[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[0].parent().id(), 0, &s, fallback)); EXPECT_EQ(s, mr[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[0].parent().id(), 20, &s, fallback)); EXPECT_NE(s,  r[0].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[0].parent().id(), 20, &s, fallback)); EXPECT_NE(s, mr[0].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[0].parent().id(), "str", &s)); EXPECT_EQ(s,  r[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[0].parent().id(), "str", &s)); EXPECT_EQ(s, mr[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[0].parent().id(), "str", &s, fallback)); EXPECT_EQ(s,  r[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[0].parent().id(), "str", &s, fallback)); EXPECT_EQ(s, mr[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[0].parent().id(), "!!!", &s, fallback)); EXPECT_NE(s,  r[0].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[0].parent().id(), "!!!", &s, fallback)); EXPECT_NE(s, mr[0].val()); EXPECT_EQ(s, "fallback");
        //
        s = {}; ASSERT_TRUE(t.deserialize_key( r[1].id(), &s)); EXPECT_EQ(s,  r[1].key()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(t.deserialize_key(mr[1].id(), &s)); EXPECT_EQ(s, mr[1].key()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(t.deserialize( r[1].id(), &s));     EXPECT_EQ(s,  r[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(t.deserialize(mr[1].id(), &s));     EXPECT_EQ(s, mr[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[1].parent().id(), 1, &s)); EXPECT_EQ(s,  r[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[1].parent().id(), 1, &s)); EXPECT_EQ(s, mr[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[1].parent().id(), 1, &s, fallback)); EXPECT_EQ(s,  r[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[1].parent().id(), 1, &s, fallback)); EXPECT_EQ(s, mr[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[1].parent().id(), 20, &s, fallback)); EXPECT_NE(s,  r[1].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[1].parent().id(), 20, &s, fallback)); EXPECT_NE(s, mr[1].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[1].parent().id(), "base64", &s)); EXPECT_EQ(s,  r[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[1].parent().id(), "base64", &s)); EXPECT_EQ(s, mr[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[1].parent().id(), "base64", &s, fallback)); EXPECT_EQ(s,  r[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[1].parent().id(), "base64", &s, fallback)); EXPECT_EQ(s, mr[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[1].parent().id(), "!!!!!!", &s, fallback)); EXPECT_NE(s,  r[1].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[1].parent().id(), "!!!!!!", &s, fallback)); EXPECT_NE(s, mr[1].val()); EXPECT_EQ(s, "fallback");
        //
        s = {}; ASSERT_TRUE(t.deserialize_key( r[2].id(), &s)); EXPECT_EQ(s,  r[2].key()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(t.deserialize_key(mr[2].id(), &s)); EXPECT_EQ(s, mr[2].key()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; ASSERT_TRUE(t.deserialize( r[2].id(), &s));     EXPECT_EQ(s,  r[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(t.deserialize(mr[2].id(), &s));     EXPECT_EQ(s, mr[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[2].parent().id(), 2, &s)); EXPECT_EQ(s,  r[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[2].parent().id(), 2, &s)); EXPECT_EQ(s, mr[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[2].parent().id(), 2, &s, fallback)); EXPECT_EQ(s,  r[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[2].parent().id(), 2, &s, fallback)); EXPECT_EQ(s, mr[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[2].parent().id(), 21, &s, fallback)); EXPECT_NE(s,  r[2].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[2].parent().id(), 21, &s, fallback)); EXPECT_NE(s, mr[2].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[2].parent().id(), "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu", &s)); EXPECT_EQ(s,  r[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[2].parent().id(), "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu", &s)); EXPECT_EQ(s, mr[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[2].parent().id(), "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu", &s, fallback)); EXPECT_EQ(s,  r[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[2].parent().id(), "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu", &s, fallback)); EXPECT_EQ(s, mr[2].val()); EXPECT_EQ(s, "base64");
        s = {}; ASSERT_TRUE(t.deserialize_child( r[2].parent().id(), "!!!!dml0eSBpcyB0aGUgc291bCBvZiB3aXQu", &s, fallback)); EXPECT_NE(s,  r[2].val()); EXPECT_EQ(s, "fallback");
        s = {}; ASSERT_TRUE(t.deserialize_child(mr[2].parent().id(), "!!!!dml0eSBpcyB0aGUgc291bCBvZiB3aXQu", &s, fallback)); EXPECT_NE(s, mr[2].val()); EXPECT_EQ(s, "fallback");
        //
        s = {};  r[0].load_key(&s); EXPECT_EQ(s,  r[0].key()); EXPECT_EQ(s, "str");
        s = {}; mr[0].load_key(&s); EXPECT_EQ(s, mr[0].key()); EXPECT_EQ(s, "str");
        s = {};  r[0].load(&s);     EXPECT_EQ(s,  r[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; mr[0].load(&s);     EXPECT_EQ(s, mr[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {};  r[1].load_key(&s); EXPECT_EQ(s,  r[1].key()); EXPECT_EQ(s, "base64");
        s = {}; mr[1].load_key(&s); EXPECT_EQ(s, mr[1].key()); EXPECT_EQ(s, "base64");
        s = {};  r[1].load(&s);     EXPECT_EQ(s,  r[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; mr[1].load(&s);     EXPECT_EQ(s, mr[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {};  r[2].load_key(&s); EXPECT_EQ(s,  r[2].key()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; mr[2].load_key(&s); EXPECT_EQ(s, mr[2].key()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {};  r[2].load(&s);     EXPECT_EQ(s,  r[2].val()); EXPECT_EQ(s, "base64");
        s = {}; mr[2].load(&s);     EXPECT_EQ(s, mr[2].val()); EXPECT_EQ(s, "base64");
        s = {}; t.load_key( r[0].id(), &s); EXPECT_EQ(s,  r[0].key()); EXPECT_EQ(s, "str");
        s = {}; t.load_key(mr[0].id(), &s); EXPECT_EQ(s, mr[0].key()); EXPECT_EQ(s, "str");
        s = {}; t.load( r[0].id(), &s);     EXPECT_EQ(s,  r[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; t.load(mr[0].id(), &s);     EXPECT_EQ(s, mr[0].val()); EXPECT_EQ(s, "Brevity is the soul of wit.");
        s = {}; t.load_key( r[1].id(), &s); EXPECT_EQ(s,  r[1].key()); EXPECT_EQ(s, "base64");
        s = {}; t.load_key(mr[1].id(), &s); EXPECT_EQ(s, mr[1].key()); EXPECT_EQ(s, "base64");
        s = {}; t.load( r[1].id(), &s);     EXPECT_EQ(s,  r[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; t.load(mr[1].id(), &s);     EXPECT_EQ(s, mr[1].val()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; t.load_key( r[2].id(), &s); EXPECT_EQ(s,  r[2].key()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; t.load_key(mr[2].id(), &s); EXPECT_EQ(s, mr[2].key()); EXPECT_EQ(s, "QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu");
        s = {}; t.load( r[2].id(), &s);     EXPECT_EQ(s,  r[2].val()); EXPECT_EQ(s, "base64");
        s = {}; t.load(mr[2].id(), &s);     EXPECT_EQ(s, mr[2].val()); EXPECT_EQ(s, "base64");
    }
    std::string buf;
    size_t szk = 0;
    #define testonbuf(...) { SCOPED_TRACE("here"); testonbuf_(__VA_ARGS__); }
    auto testonbuf_ = [&](std::function<void()> const& fn){
        to_substr(buf).fill('a');
        EXPECT_NE(buf, "Brevity is the soul of wit.");
        fn();
        EXPECT_TRUE(r[1].deserialize(fmt::base64(buf, &szk)));
        EXPECT_EQ(buf, "Brevity is the soul of wit.");
    };
    testonbuf([&]{ EXPECT_TRUE(t.deserialize_child(r[1].parent().id(), 1, fmt::base64(buf, &szk))); });
    testonbuf([&]{ EXPECT_TRUE(t.deserialize_child(r[1].parent().id(), r[1].key(), fmt::base64(buf, &szk))); });
    testonbuf([&]{ EXPECT_TRUE(t.deserialize(r[1].id(), fmt::base64(buf, &szk))); });
    testonbuf([&]{ EXPECT_TRUE(t.deserialize_key(r[2].id(), fmt::base64(buf, &szk))); });
    testonbuf([&]{ t.load(r[1].id(), fmt::base64(buf, &szk)); });
    testonbuf([&]{ t.load(r[1].id(), fmt::base64(to_substr(buf), &szk)); });
    testonbuf([&]{ t.load(mr[1].id(), fmt::base64(buf, &szk)); });
    testonbuf([&]{ t.load(mr[1].id(), fmt::base64(to_substr(buf), &szk)); });
    testonbuf([&]{ t.load_key(r[2].id(), fmt::base64(buf, &szk)); });
    testonbuf([&]{ t.load_key(r[2].id(), fmt::base64(to_substr(buf), &szk)); });
    testonbuf([&]{ t.load_key(mr[2].id(), fmt::base64(buf, &szk)); });
    testonbuf([&]{ t.load_key(mr[2].id(), fmt::base64(to_substr(buf), &szk)); });
    //
    testonbuf([&]{ EXPECT_TRUE(r[1].parent().deserialize_child(1, fmt::base64(buf, &szk))); });
    testonbuf([&]{ EXPECT_TRUE(r[1].parent().deserialize_child(r[1].key(), fmt::base64(buf, &szk))); });
    testonbuf([&]{ EXPECT_TRUE(r[1].deserialize(fmt::base64(buf, &szk))); });
    testonbuf([&]{ EXPECT_TRUE(r[2].deserialize_key(fmt::base64(buf, &szk))); });
    testonbuf([&]{ r[1].load(fmt::base64(buf, &szk)); });
    testonbuf([&]{ r[1].load(fmt::base64(to_substr(buf), &szk)); });
    testonbuf([&]{ mr[1].load(fmt::base64(buf, &szk)); });
    testonbuf([&]{ mr[1].load(fmt::base64(to_substr(buf), &szk)); });
    testonbuf([&]{ r[2].load_key(fmt::base64(buf, &szk)); });
    testonbuf([&]{ r[2].load_key(fmt::base64(to_substr(buf), &szk)); });
    testonbuf([&]{ mr[2].load_key(fmt::base64(buf, &szk)); });
    testonbuf([&]{ mr[2].load_key(fmt::base64(to_substr(buf), &szk)); });
    testonbuf([&]{ EXPECT_TRUE(read(r[1], fmt::base64(buf, &szk))); });
    testonbuf([&]{ EXPECT_TRUE(read(mr[1], fmt::base64(buf, &szk))); });
    testonbuf([&]{ EXPECT_TRUE(read_key(r[2], fmt::base64(buf, &szk))); });
    testonbuf([&]{ EXPECT_TRUE(read_key(mr[2], fmt::base64(buf, &szk))); });
    testonbuf([&]{ EXPECT_TRUE(read(r[1].tree(), r[1].id(), fmt::base64(buf, &szk))); });
    testonbuf([&]{ EXPECT_TRUE(read(mr[1].tree(), mr[1].id(), fmt::base64(buf, &szk))); });
    testonbuf([&]{ EXPECT_TRUE(read_key(r[2].tree(), r[2].id(), fmt::base64(buf, &szk))); });
    testonbuf([&]{ EXPECT_TRUE(read_key(mr[2].tree(), mr[2].id(), fmt::base64(buf, &szk))); });
    {
        auto w = fmt::base64(buf, &szk);
        testonbuf([&]{ EXPECT_TRUE(read(r[1], w)); });
        testonbuf([&]{ EXPECT_TRUE(read(r[1], &w)); });
        testonbuf([&]{ EXPECT_TRUE(read(mr[1], w)); });
        testonbuf([&]{ EXPECT_TRUE(read(mr[1], &w)); });
        testonbuf([&]{ EXPECT_TRUE(read_key(r[2], w)); });
        testonbuf([&]{ EXPECT_TRUE(read_key(r[2], &w)); });
        testonbuf([&]{ EXPECT_TRUE(read_key(mr[2], w)); });
        testonbuf([&]{ EXPECT_TRUE(read_key(mr[2], &w)); });
        testonbuf([&]{ EXPECT_TRUE(read(r[1].tree(), r[1].id(), w)); });
        testonbuf([&]{ EXPECT_TRUE(read(r[1].tree(), r[1].id(), &w)); });
        testonbuf([&]{ EXPECT_TRUE(read(mr[1].tree(), mr[1].id(), w)); });
        testonbuf([&]{ EXPECT_TRUE(read(mr[1].tree(), mr[1].id(), &w)); });
        testonbuf([&]{ EXPECT_TRUE(read_key(r[2].tree(), r[2].id(), w)); });
        testonbuf([&]{ EXPECT_TRUE(read_key(r[2].tree(), r[2].id(), &w)); });
        testonbuf([&]{ EXPECT_TRUE(read_key(mr[2].tree(), mr[2].id(), w)); });
        testonbuf([&]{ EXPECT_TRUE(read_key(mr[2].tree(), mr[2].id(), &w)); });
    }
    #undef testonbuf
    //
    auto checkerr_key = [&](ConstNodeRef node){
        ASSERT_EQ(node.tree(), &t);
        ASSERT_TRUE(node.type().has_key());
        id_type id = node.id();
        auto w = fmt::base64(buf, &szk);
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t.load_key(node.id(), &w); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t.load_key(node.id(), &w); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t.load_key(node.id(), fmt::base64(buf)); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t.load_key(node.id(), fmt::base64(buf, &szk)); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t.load_key(node.id(), fmt::base64(to_substr(buf))); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t.load_key(node.id(), fmt::base64(to_substr(buf), &szk)); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ node.load_key(&w); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ node.load_key(&w); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ node.load_key(fmt::base64(buf)); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ node.load_key(fmt::base64(buf, &szk)); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ node.load_key(fmt::base64(to_substr(buf))); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ node.load_key(fmt::base64(to_substr(buf), &szk)); }, id));
        EXPECT_FALSE(t.deserialize_key(id, w));
        EXPECT_FALSE(t.deserialize_key(id, &w));
        EXPECT_FALSE(t.deserialize_key(id, fmt::base64(buf)));
        EXPECT_FALSE(t.deserialize_key(id, fmt::base64(buf, &szk)));
        EXPECT_FALSE(t.deserialize_key(id, fmt::base64(to_substr(buf))));
        EXPECT_FALSE(t.deserialize_key(id, fmt::base64(to_substr(buf), &szk)));
        EXPECT_FALSE(node.deserialize_key(w));
        EXPECT_FALSE(node.deserialize_key(&w));
        EXPECT_FALSE(node.deserialize_key(fmt::base64(buf)));
        EXPECT_FALSE(node.deserialize_key(fmt::base64(buf, &szk)));
        EXPECT_FALSE(node.deserialize_key(fmt::base64(to_substr(buf))));
        EXPECT_FALSE(node.deserialize_key(fmt::base64(to_substr(buf), &szk)));
        EXPECT_FALSE(read_key(node, w));
        EXPECT_FALSE(read_key(node, &w));
        EXPECT_FALSE(read_key(node, fmt::base64(buf)));
        EXPECT_FALSE(read_key(node, fmt::base64(buf, &szk)));
        EXPECT_FALSE(read_key(node, fmt::base64(to_substr(buf))));
        EXPECT_FALSE(read_key(node, fmt::base64(to_substr(buf), &szk)));
        EXPECT_FALSE(read_key(&t, id, w));
        EXPECT_FALSE(read_key(&t, id, &w));
        EXPECT_FALSE(read_key(&t, id, fmt::base64(buf)));
        EXPECT_FALSE(read_key(&t, id, fmt::base64(buf, &szk)));
        EXPECT_FALSE(read_key(&t, id, fmt::base64(to_substr(buf))));
        EXPECT_FALSE(read_key(&t, id, fmt::base64(to_substr(buf), &szk)));
    };
    auto checkerr_val = [&](ConstNodeRef node){
        ASSERT_EQ(node.tree(), &t);
        id_type id = node.id();
        auto w = fmt::base64(buf, &szk);
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t.load(node.id(), w); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t.load(node.id(), &w); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t.load(node.id(), fmt::base64(buf)); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t.load(node.id(), fmt::base64(buf, &szk)); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t.load(node.id(), fmt::base64(to_substr(buf))); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ t.load(node.id(), fmt::base64(to_substr(buf), &szk)); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ node.load(w); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ node.load(&w); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ node.load(fmt::base64(buf)); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ node.load(fmt::base64(buf, &szk)); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ node.load(fmt::base64(to_substr(buf))); }, id));
        RYML_EXPECT_ERROR(check_error_visit(&t, [&]{ node.load(fmt::base64(to_substr(buf), &szk)); }, id));
        EXPECT_FALSE(t.deserialize(id, w));
        EXPECT_FALSE(t.deserialize(id, &w));
        EXPECT_FALSE(t.deserialize(id, fmt::base64(buf)));
        EXPECT_FALSE(t.deserialize(id, fmt::base64(buf, &szk)));
        EXPECT_FALSE(t.deserialize(id, fmt::base64(to_substr(buf))));
        EXPECT_FALSE(t.deserialize(id, fmt::base64(to_substr(buf), &szk)));
        if(t.has_parent(id))
        {
            id_type parent = t.parent(id), pos = t.child_pos(parent, id);
            csubstr key = t.key(id);
            EXPECT_FALSE(t.deserialize_child(parent, pos, w));
            EXPECT_FALSE(t.deserialize_child(parent, key, w));
            EXPECT_FALSE(t.deserialize_child(parent, pos, &w));
            EXPECT_FALSE(t.deserialize_child(parent, key, &w));
            EXPECT_FALSE(t.deserialize_child(parent, pos, fmt::base64(buf)));
            EXPECT_FALSE(t.deserialize_child(parent, key, fmt::base64(buf)));
            EXPECT_FALSE(t.deserialize_child(parent, pos, fmt::base64(buf, &szk)));
            EXPECT_FALSE(t.deserialize_child(parent, key, fmt::base64(buf, &szk)));
            EXPECT_FALSE(t.deserialize_child(parent, pos, fmt::base64(to_substr(buf))));
            EXPECT_FALSE(t.deserialize_child(parent, key, fmt::base64(to_substr(buf))));
            EXPECT_FALSE(t.deserialize_child(parent, pos, fmt::base64(to_substr(buf), &szk)));
            EXPECT_FALSE(t.deserialize_child(parent, key, fmt::base64(to_substr(buf), &szk)));
        }
        EXPECT_FALSE(node.deserialize(w));
        EXPECT_FALSE(node.deserialize(&w));
        EXPECT_FALSE(node.deserialize(fmt::base64(buf)));
        EXPECT_FALSE(node.deserialize(fmt::base64(buf, &szk)));
        EXPECT_FALSE(node.deserialize(fmt::base64(to_substr(buf))));
        EXPECT_FALSE(node.deserialize(fmt::base64(to_substr(buf), &szk)));
        if(t.has_parent(id))
        {
            ConstNodeRef nparent = node.parent();
            id_type pos = nparent.child_pos(node);
            csubstr key = t.key(id);
            EXPECT_FALSE(nparent.deserialize_child(pos, w));
            EXPECT_FALSE(nparent.deserialize_child(key, w));
            EXPECT_FALSE(nparent.deserialize_child(pos, &w));
            EXPECT_FALSE(nparent.deserialize_child(key, &w));
            EXPECT_FALSE(nparent.deserialize_child(pos, fmt::base64(buf)));
            EXPECT_FALSE(nparent.deserialize_child(key, fmt::base64(buf)));
            EXPECT_FALSE(nparent.deserialize_child(pos, fmt::base64(buf, &szk)));
            EXPECT_FALSE(nparent.deserialize_child(key, fmt::base64(buf, &szk)));
            EXPECT_FALSE(nparent.deserialize_child(pos, fmt::base64(to_substr(buf))));
            EXPECT_FALSE(nparent.deserialize_child(key, fmt::base64(to_substr(buf))));
            EXPECT_FALSE(nparent.deserialize_child(pos, fmt::base64(to_substr(buf), &szk)));
            EXPECT_FALSE(nparent.deserialize_child(key, fmt::base64(to_substr(buf), &szk)));
        }
        EXPECT_FALSE(read(node, w));
        EXPECT_FALSE(read(node, &w));
        EXPECT_FALSE(read(node, fmt::base64(buf)));
        EXPECT_FALSE(read(node, fmt::base64(buf, &szk)));
        EXPECT_FALSE(read(node, fmt::base64(to_substr(buf))));
        EXPECT_FALSE(read(node, fmt::base64(to_substr(buf), &szk)));
        EXPECT_FALSE(read(node.tree(), id, w));
        EXPECT_FALSE(read(node.tree(), id, &w));
        EXPECT_FALSE(read(node.tree(), id, fmt::base64(buf)));
        EXPECT_FALSE(read(node.tree(), id, fmt::base64(buf, &szk)));
        EXPECT_FALSE(read(node.tree(), id, fmt::base64(to_substr(buf))));
        EXPECT_FALSE(read(node.tree(), id, fmt::base64(to_substr(buf), &szk)));
    };
    {
        SCOPED_TRACE("r[1] key is not base64");
        EXPECT_FALSE(base64_valid(r[1].key().str, r[1].key().len));
        checkerr_key(r[1]);
    }
    {
        SCOPED_TRACE("r[2] val is not base64");
        EXPECT_FALSE(base64_valid(r[2].val().str, r[2].val().len));
        checkerr_val(r[2]);
    }
    {
        SCOPED_TRACE("container as base64");
        checkerr_val(r);
    }
}

} // namespace yml
} // namespace c4

#if defined(_MSC_VER)
#   pragma warning(pop)
#elif defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__GNUC__)
#   pragma GCC diagnostic pop
#endif
