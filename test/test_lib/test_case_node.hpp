#ifndef _C4_YML_TEST_TEST_CASE_NODE_HPP_
#define _C4_YML_TEST_TEST_CASE_NODE_HPP_

#ifdef RYML_SINGLE_HEADER
#include <ryml_all.hpp>
#else
#include "c4/std/vector.hpp"
#include "c4/std/string.hpp"
#include "c4/format.hpp"
#include <c4/yml/yml.hpp>
#endif

namespace c4 {
namespace yml {

struct TaggedScalar
{
    csubstr tag;
    csubstr scalar;
    template<size_t N, size_t M>
    TaggedScalar(const char (&t)[N], const char (&s)[M]) : tag(t), scalar(s) {}
    template<size_t N>
    TaggedScalar(const char (&t)[N], std::nullptr_t) : tag(t), scalar() {}
};

struct AnchorRef
{
    NodeType_e type;
    csubstr str;
    AnchorRef() : type(NOTYPE), str() {}
    AnchorRef(NodeType_e t) : type(t), str() {}
    AnchorRef(NodeType_e t, csubstr v) : type(t), str(v) {}
};


/** a node class against which ryml structures are tested. Uses initializer
 * lists to facilitate minimal specification. */
struct TestCaseNode
{
public:

    using  seqmap = std::vector<TestCaseNode>;
    using iseqmap = std::initializer_list<TestCaseNode>;

    struct TaggedList
    {
        csubstr tag;
        iseqmap ilist;
        template<size_t N> TaggedList(const char (&t)[N], iseqmap l) : tag(t), ilist(l) {}
    };

public:

    NodeType       type;
    csubstr        key, key_tag; AnchorRef key_anchor;
    csubstr        val, val_tag; AnchorRef val_anchor;
    seqmap         children;
    TestCaseNode * parent;

public:

    TestCaseNode(TestCaseNode     && that) noexcept { _move(std::move(that)); }
    TestCaseNode(TestCaseNode const& that) noexcept { _copy(that); }

    TestCaseNode& operator= (TestCaseNode     && that) noexcept { _move(std::move(that)); return *this; }
    TestCaseNode& operator= (TestCaseNode const& that) noexcept { _copy(that); return *this; }

    ~TestCaseNode() = default;

public:

    // brace yourself: what you are about to see is ... crazy.

    TestCaseNode() : TestCaseNode(NOTYPE) {}
    TestCaseNode(NodeType_e t) : type(t), key(), key_tag(), key_anchor(), val(), val_tag(), val_anchor(), children(), parent(nullptr) { _set_parent(); }

    // val
    template<size_t N> explicit TestCaseNode(const char (&v)[N]   ) : type((VAL       )), key(), key_tag(), key_anchor(), val(v       ), val_tag(     ), val_anchor(), children(), parent(nullptr) { _set_parent(); }
                       explicit TestCaseNode(TaggedScalar const& v) : type((VAL|VALTAG)), key(), key_tag(), key_anchor(), val(v.scalar), val_tag(v.tag), val_anchor(), children(), parent(nullptr) { _set_parent(); }
                       explicit TestCaseNode(std::nullptr_t       ) : type((VAL       )), key(), key_tag(), key_anchor(), val(        ), val_tag(     ), val_anchor(), children(), parent(nullptr) { _set_parent(); }
    // val, with anchor/ref
    template<size_t N> explicit TestCaseNode(const char (&v)[N]   , AnchorRef const& arv) : type((arv.type|VAL       )), key(), key_tag(), key_anchor(), val(v       ), val_tag(     ), val_anchor(arv), children(), parent(nullptr) { _set_parent(); }
                       explicit TestCaseNode(TaggedScalar const& v, AnchorRef const& arv) : type((arv.type|VAL|VALTAG)), key(), key_tag(), key_anchor(), val(v.scalar), val_tag(v.tag), val_anchor(arv), children(), parent(nullptr) { _set_parent(); }
                       explicit TestCaseNode(std::nullptr_t       , AnchorRef const& arv) : type((arv.type|VAL       )), key(), key_tag(), key_anchor(), val(        ), val_tag(     ), val_anchor(arv), children(), parent(nullptr) { _set_parent(); }
                       explicit TestCaseNode(                       AnchorRef const& arv) : type((arv.type|VAL       )), key(), key_tag(), key_anchor(), val(arv.str ), val_tag(     ), val_anchor(arv), children(), parent(nullptr) { _set_parent(); RYML_ASSERT(arv.type == VALREF); }


    // val, explicit type
    template<size_t N> explicit TestCaseNode(NodeType t, const char (&v)[N]   ) : type((VAL|t       )), key(), key_tag(), key_anchor(), val(v       ), val_tag(     ), val_anchor(), children(), parent(nullptr) { _set_parent(); }
                       explicit TestCaseNode(NodeType t, TaggedScalar const& v) : type((VAL|VALTAG|t)), key(), key_tag(), key_anchor(), val(v.scalar), val_tag(v.tag), val_anchor(), children(), parent(nullptr) { _set_parent(); }
                       explicit TestCaseNode(NodeType t, std::nullptr_t       ) : type((VAL       |t)), key(), key_tag(), key_anchor(), val(        ), val_tag(     ), val_anchor(), children(), parent(nullptr) { _set_parent(); }
    // val, explicit type, with val anchor/ref
    template<size_t N> explicit TestCaseNode(NodeType t, const char (&v)[N]   , AnchorRef const& arv) : type((arv.type|VAL|t       )), key(), key_tag(), key_anchor(), val(v       ), val_tag(     ), val_anchor(arv), children(), parent(nullptr) { _set_parent(); }
                       explicit TestCaseNode(NodeType t, TaggedScalar const& v, AnchorRef const& arv) : type((arv.type|VAL|VALTAG|t)), key(), key_tag(), key_anchor(), val(v.scalar), val_tag(v.tag), val_anchor(arv), children(), parent(nullptr) { _set_parent(); }
                       explicit TestCaseNode(NodeType t, std::nullptr_t       , AnchorRef const& arv) : type((arv.type|VAL       |t)), key(), key_tag(), key_anchor(), val(        ), val_tag(     ), val_anchor(arv), children(), parent(nullptr) { _set_parent(); }


    // keyval
    template<size_t N, size_t M> explicit TestCaseNode(const char (&k)[N]   , const char (&v)[M]   ) : type((KEYVAL                  )), key(k       ), key_tag(     ), key_anchor(   ), val(v       ), val_tag(     ), val_anchor(   ), children(), parent(nullptr) { _set_parent(); }
    template<size_t M>           explicit TestCaseNode(std::nullptr_t       , const char (&v)[M]   ) : type((KEYVAL                  )), key(        ), key_tag(     ), key_anchor(   ), val(v       ), val_tag(     ), val_anchor(   ), children(), parent(nullptr) { _set_parent(); }
    template<size_t N>           explicit TestCaseNode(const char (&k)[N]   , std::nullptr_t       ) : type((KEYVAL                  )), key(k       ), key_tag(     ), key_anchor(   ), val(        ), val_tag(     ), val_anchor(   ), children(), parent(nullptr) { _set_parent(); }
    template<size_t N>           explicit TestCaseNode(const char (&k)[N]   , TaggedScalar const& v) : type((KEYVAL|VALTAG           )), key(k       ), key_tag(     ), key_anchor(   ), val(v.scalar), val_tag(v.tag), val_anchor(   ), children(), parent(nullptr) { _set_parent(); }
    template<size_t M>           explicit TestCaseNode(TaggedScalar const& k, const char (&v)[M]   ) : type((KEYVAL|KEYTAG           )), key(k.scalar), key_tag(k.tag), key_anchor(   ), val(v       ), val_tag(     ), val_anchor(   ), children(), parent(nullptr) { _set_parent(); }
                                 explicit TestCaseNode(TaggedScalar const& k, TaggedScalar const& v) : type((KEYVAL|KEYTAG|VALTAG    )), key(k.scalar), key_tag(k.tag), key_anchor(   ), val(v.scalar), val_tag(v.tag), val_anchor(   ), children(), parent(nullptr) { _set_parent(); }
                                 explicit TestCaseNode(std::nullptr_t       , TaggedScalar const& v) : type((KEYVAL       |VALTAG    )), key(        ), key_tag(     ), key_anchor(   ), val(v.scalar), val_tag(v.tag), val_anchor(   ), children(), parent(nullptr) { _set_parent(); }
                                 explicit TestCaseNode(TaggedScalar const& k, std::nullptr_t       ) : type((KEYVAL|KEYTAG           )), key(k.scalar), key_tag(k.tag), key_anchor(   ), val(        ), val_tag(     ), val_anchor(   ), children(), parent(nullptr) { _set_parent(); }
                                 explicit TestCaseNode(std::nullptr_t       , std::nullptr_t       ) : type((KEYVAL                  )), key(        ), key_tag(     ), key_anchor(   ), val(        ), val_tag(     ), val_anchor(   ), children(), parent(nullptr) { _set_parent(); }
                                 explicit TestCaseNode(AnchorRef  const& ark, AnchorRef  const& arv) : type((KEYVAL|ark.type|arv.type)), key(ark.str ), key_tag(     ), key_anchor(ark), val(arv.str ), val_tag(     ), val_anchor(arv), children(), parent(nullptr) { _set_parent(); RYML_ASSERT(ark.type == KEYREF); RYML_ASSERT(arv.type == VALREF); }
    // keyval, with val anchor/ref
    template<size_t N, size_t M> explicit TestCaseNode(const char (&k)[N]   , const char (&v)[M]   , AnchorRef const& arv) : type((arv.type|KEYVAL              )), key(k       ), key_tag(     ), key_anchor(), val(v       ), val_tag(     ), val_anchor(arv), children(), parent(nullptr) { _set_parent(); }
    template<size_t N>           explicit TestCaseNode(const char (&k)[N]   , TaggedScalar const& v, AnchorRef const& arv) : type((arv.type|KEYVAL|VALTAG       )), key(k       ), key_tag(     ), key_anchor(), val(v.scalar), val_tag(v.tag), val_anchor(arv), children(), parent(nullptr) { _set_parent(); }
    template<size_t M>           explicit TestCaseNode(TaggedScalar const& k, const char (&v)[M]   , AnchorRef const& arv) : type((arv.type|KEYVAL|KEYTAG       )), key(k.scalar), key_tag(k.tag), key_anchor(), val(v       ), val_tag(     ), val_anchor(arv), children(), parent(nullptr) { _set_parent(); }
                                 explicit TestCaseNode(TaggedScalar const& k, TaggedScalar const& v, AnchorRef const& arv) : type((arv.type|KEYVAL|KEYTAG|VALTAG)), key(k.scalar), key_tag(k.tag), key_anchor(), val(v.scalar), val_tag(v.tag), val_anchor(arv), children(), parent(nullptr) { _set_parent(); }
    // keyval, with key anchor/ref
    template<size_t N, size_t M> explicit TestCaseNode(const char (&k)[N]   , AnchorRef const& ark, const char (&v)[M]   ) : type((ark.type|KEYVAL              )), key(k       ), key_tag(     ), key_anchor(ark), val(v       ), val_tag(     ), val_anchor(), children(), parent(nullptr) { _set_parent(); }
    template<size_t N>           explicit TestCaseNode(const char (&k)[N]   , AnchorRef const& ark, TaggedScalar const& v) : type((ark.type|KEYVAL|VALTAG       )), key(k       ), key_tag(     ), key_anchor(ark), val(v.scalar), val_tag(v.tag), val_anchor(), children(), parent(nullptr) { _set_parent(); }
    template<size_t M>           explicit TestCaseNode(TaggedScalar const& k, AnchorRef const& ark, const char (&v)[M]   ) : type((ark.type|KEYVAL|KEYTAG       )), key(k.scalar), key_tag(k.tag), key_anchor(ark), val(v       ), val_tag(     ), val_anchor(), children(), parent(nullptr) { _set_parent(); }
                                 explicit TestCaseNode(TaggedScalar const& k, AnchorRef const& ark, TaggedScalar const& v) : type((ark.type|KEYVAL|KEYTAG|VALTAG)), key(k.scalar), key_tag(k.tag), key_anchor(ark), val(v.scalar), val_tag(v.tag), val_anchor(), children(), parent(nullptr) { _set_parent(); }
    // keyval, with key anchor/ref + val anchor/ref
    template<size_t N, size_t M> explicit TestCaseNode(const char (&k)[N]   , AnchorRef const& ark, const char (&v)[M]   , AnchorRef const& arv) : type((ark.type|arv.type|KEYVAL              )), key(k       ), key_tag(     ), key_anchor(ark), val(v       ), val_tag(     ), val_anchor(arv), children(), parent(nullptr) { _set_parent(); }
    template<size_t N>           explicit TestCaseNode(const char (&k)[N]   , AnchorRef const& ark, TaggedScalar const& v, AnchorRef const& arv) : type((ark.type|arv.type|KEYVAL|VALTAG       )), key(k       ), key_tag(     ), key_anchor(ark), val(v.scalar), val_tag(v.tag), val_anchor(arv), children(), parent(nullptr) { _set_parent(); }
    template<size_t M>           explicit TestCaseNode(TaggedScalar const& k, AnchorRef const& ark, const char (&v)[M]   , AnchorRef const& arv) : type((ark.type|arv.type|KEYVAL|KEYTAG       )), key(k.scalar), key_tag(k.tag), key_anchor(ark), val(v       ), val_tag(     ), val_anchor(arv), children(), parent(nullptr) { _set_parent(); }
                                 explicit TestCaseNode(TaggedScalar const& k, AnchorRef const& ark, TaggedScalar const& v, AnchorRef const& arv) : type((ark.type|arv.type|KEYVAL|KEYTAG|VALTAG)), key(k.scalar), key_tag(k.tag), key_anchor(ark), val(v.scalar), val_tag(v.tag), val_anchor(arv), children(), parent(nullptr) { _set_parent(); }


    // keyval, explicit type
    template<size_t N, size_t M> explicit TestCaseNode(NodeType t, const char (&k)[N]   , const char (&v)[M]   ) : type((KEYVAL|t              )), key(k       ), key_tag(     ), key_anchor(), val(v       ), val_tag(     ), val_anchor(), children(), parent(nullptr) { _set_parent(); }
    template<size_t N>           explicit TestCaseNode(NodeType t, const char (&k)[N]   , std::nullptr_t       ) : type((KEYVAL|t              )), key(k       ), key_tag(     ), key_anchor(), val(        ), val_tag(     ), val_anchor(), children(), parent(nullptr) { _set_parent(); }
    template<size_t M>           explicit TestCaseNode(NodeType t, std::nullptr_t       , const char (&v)[M]   ) : type((KEYVAL|t              )), key(        ), key_tag(     ), key_anchor(), val(v       ), val_tag(     ), val_anchor(), children(), parent(nullptr) { _set_parent(); }
    template<size_t N>           explicit TestCaseNode(NodeType t, const char (&k)[N]   , TaggedScalar const& v) : type((KEYVAL|VALTAG|t       )), key(k       ), key_tag(     ), key_anchor(), val(v.scalar), val_tag(v.tag), val_anchor(), children(), parent(nullptr) { _set_parent(); }
    template<size_t M>           explicit TestCaseNode(NodeType t, TaggedScalar const& k, const char (&v)[M]   ) : type((KEYVAL|KEYTAG|t       )), key(k.scalar), key_tag(k.tag), key_anchor(), val(v       ), val_tag(     ), val_anchor(), children(), parent(nullptr) { _set_parent(); }
                                 explicit TestCaseNode(NodeType t, TaggedScalar const& k, TaggedScalar const& v) : type((KEYVAL|KEYTAG|VALTAG|t)), key(k.scalar), key_tag(k.tag), key_anchor(), val(v.scalar), val_tag(v.tag), val_anchor(), children(), parent(nullptr) { _set_parent(); }
                                 explicit TestCaseNode(NodeType t, TaggedScalar const& k, std::nullptr_t       ) : type((KEYVAL|KEYTAG       |t)), key(k.scalar), key_tag(k.tag), key_anchor(), val(        ), val_tag(     ), val_anchor(), children(), parent(nullptr) { _set_parent(); }
                                 explicit TestCaseNode(NodeType t, std::nullptr_t       , TaggedScalar const& v) : type((KEYVAL       |VALTAG|t)), key(        ), key_tag(     ), key_anchor(), val(v.scalar), val_tag(v.tag), val_anchor(), children(), parent(nullptr) { _set_parent(); }
                                 explicit TestCaseNode(NodeType t, std::nullptr_t       , std::nullptr_t       ) : type((KEYVAL              |t)), key(        ), key_tag(     ), key_anchor(), val(        ), val_tag(     ), val_anchor(), children(), parent(nullptr) { _set_parent(); }
    // keyval, explicit type, with val anchor/ref
    template<size_t N, size_t M> explicit TestCaseNode(NodeType t, const char (&k)[N]   , const char (&v)[M]   , AnchorRef const& arv) : type((arv.type|KEYVAL|t              )), key(k       ), key_tag(     ), key_anchor(), val(v       ), val_tag(     ), val_anchor(arv), children(), parent(nullptr) { _set_parent(); }
    template<size_t N>           explicit TestCaseNode(NodeType t, const char (&k)[N]   , TaggedScalar const& v, AnchorRef const& arv) : type((arv.type|KEYVAL|VALTAG|t       )), key(k       ), key_tag(     ), key_anchor(), val(v.scalar), val_tag(v.tag), val_anchor(arv), children(), parent(nullptr) { _set_parent(); }
    template<size_t M>           explicit TestCaseNode(NodeType t, TaggedScalar const& k, const char (&v)[M]   , AnchorRef const& arv) : type((arv.type|KEYVAL|KEYTAG|t       )), key(k.scalar), key_tag(k.tag), key_anchor(), val(v       ), val_tag(     ), val_anchor(arv), children(), parent(nullptr) { _set_parent(); }
                                 explicit TestCaseNode(NodeType t, TaggedScalar const& k, TaggedScalar const& v, AnchorRef const& arv) : type((arv.type|KEYVAL|KEYTAG|VALTAG|t)), key(k.scalar), key_tag(k.tag), key_anchor(), val(v.scalar), val_tag(v.tag), val_anchor(arv), children(), parent(nullptr) { _set_parent(); }
    // keyval, explicit type, with key anchor/ref
    template<size_t N, size_t M> explicit TestCaseNode(NodeType t, const char (&k)[N]   , AnchorRef const& ark, const char (&v)[M]   ) : type((ark.type|KEYVAL|t              )), key(k       ), key_tag(     ), key_anchor(ark), val(v       ), val_tag(     ), val_anchor(), children(), parent(nullptr) { _set_parent(); }
    template<size_t N>           explicit TestCaseNode(NodeType t, const char (&k)[N]   , AnchorRef const& ark, TaggedScalar const& v) : type((ark.type|KEYVAL|VALTAG|t       )), key(k       ), key_tag(     ), key_anchor(ark), val(v.scalar), val_tag(v.tag), val_anchor(), children(), parent(nullptr) { _set_parent(); }
    template<size_t M>           explicit TestCaseNode(NodeType t, TaggedScalar const& k, AnchorRef const& ark, const char (&v)[M]   ) : type((ark.type|KEYVAL|KEYTAG|t       )), key(k.scalar), key_tag(k.tag), key_anchor(ark), val(v       ), val_tag(     ), val_anchor(), children(), parent(nullptr) { _set_parent(); }
                                 explicit TestCaseNode(NodeType t, TaggedScalar const& k, AnchorRef const& ark, TaggedScalar const& v) : type((ark.type|KEYVAL|KEYTAG|VALTAG|t)), key(k.scalar), key_tag(k.tag), key_anchor(ark), val(v.scalar), val_tag(v.tag), val_anchor(), children(), parent(nullptr) { _set_parent(); }
    // keyval, explicit type, with key anchor/ref + val anchor/ref
    template<size_t N, size_t M> explicit TestCaseNode(NodeType t, const char (&k)[N]   , AnchorRef const& ark, const char (&v)[M]   , AnchorRef const& arv) : type((ark.type|arv.type|KEYVAL|t              )), key(k       ), key_tag(     ), key_anchor(ark), val(v       ), val_tag(     ), val_anchor(arv), children(), parent(nullptr) { _set_parent(); }
    template<size_t N>           explicit TestCaseNode(NodeType t, const char (&k)[N]   , AnchorRef const& ark, TaggedScalar const& v, AnchorRef const& arv) : type((ark.type|arv.type|KEYVAL|VALTAG|t       )), key(k       ), key_tag(     ), key_anchor(ark), val(v.scalar), val_tag(v.tag), val_anchor(arv), children(), parent(nullptr) { _set_parent(); }
    template<size_t M>           explicit TestCaseNode(NodeType t, TaggedScalar const& k, AnchorRef const& ark, const char (&v)[M]   , AnchorRef const& arv) : type((ark.type|arv.type|KEYVAL|KEYTAG|t       )), key(k.scalar), key_tag(k.tag), key_anchor(ark), val(v       ), val_tag(     ), val_anchor(arv), children(), parent(nullptr) { _set_parent(); }
                                 explicit TestCaseNode(NodeType t, TaggedScalar const& k, AnchorRef const& ark, TaggedScalar const& v, AnchorRef const& arv) : type((ark.type|arv.type|KEYVAL|KEYTAG|VALTAG|t)), key(k.scalar), key_tag(k.tag), key_anchor(ark), val(v.scalar), val_tag(v.tag), val_anchor(arv), children(), parent(nullptr) { _set_parent(); }


    // container
    template<size_t N> explicit TestCaseNode(const char (&k)[N]   , iseqmap    s) : type(), key(k       ), key_tag(     ), key_anchor(), val(), val_tag(     ), val_anchor(), children(s      ), parent(nullptr) { _set_parent(); type = _guess(); }
    template<size_t N> explicit TestCaseNode(const char (&k)[N]   , TaggedList s) : type(), key(k       ), key_tag(     ), key_anchor(), val(), val_tag(s.tag), val_anchor(), children(s.ilist), parent(nullptr) { _set_parent(); type = _guess(); }
                       explicit TestCaseNode(TaggedScalar const& k, iseqmap    s) : type(), key(k.scalar), key_tag(k.tag), key_anchor(), val(), val_tag(     ), val_anchor(), children(s      ), parent(nullptr) { _set_parent(); type = _guess(); }
                       explicit TestCaseNode(TaggedScalar const& k, TaggedList s) : type(), key(k.scalar), key_tag(k.tag), key_anchor(), val(), val_tag(s.tag), val_anchor(), children(s.ilist), parent(nullptr) { _set_parent(); type = _guess(); }
                       explicit TestCaseNode(                       iseqmap    m) : TestCaseNode("", m) {}
                       explicit TestCaseNode(                       TaggedList m) : TestCaseNode("", m) {}
    // container, with val anchor/ref
    template<size_t N> explicit TestCaseNode(const char (&k)[N]   , iseqmap    s, AnchorRef const& arv) : type(), key(k       ), key_tag(     ), key_anchor(), val(), val_tag(     ), val_anchor(arv), children(s      ), parent(nullptr) { _set_parent(); type = _guess(); }
    template<size_t N> explicit TestCaseNode(const char (&k)[N]   , TaggedList s, AnchorRef const& arv) : type(), key(k       ), key_tag(     ), key_anchor(), val(), val_tag(s.tag), val_anchor(arv), children(s.ilist), parent(nullptr) { _set_parent(); type = _guess(); }
                       explicit TestCaseNode(TaggedScalar const& k, iseqmap    s, AnchorRef const& arv) : type(), key(k.scalar), key_tag(k.tag), key_anchor(), val(), val_tag(     ), val_anchor(arv), children(s      ), parent(nullptr) { _set_parent(); type = _guess(); }
                       explicit TestCaseNode(TaggedScalar const& k, TaggedList s, AnchorRef const& arv) : type(), key(k.scalar), key_tag(k.tag), key_anchor(), val(), val_tag(s.tag), val_anchor(arv), children(s.ilist), parent(nullptr) { _set_parent(); type = _guess(); }
                       explicit TestCaseNode(                       iseqmap    m, AnchorRef const& arv) : TestCaseNode("", m, arv) {}
                       explicit TestCaseNode(                       TaggedList m, AnchorRef const& arv) : TestCaseNode("", m, arv) {}
    // container, with key anchor/ref
    template<size_t N> explicit TestCaseNode(const char (&k)[N]   , AnchorRef const& ark, iseqmap    s) : type(), key(k       ), key_tag(     ), key_anchor(ark), val(), val_tag(     ), val_anchor(), children(s      ), parent(nullptr) { _set_parent(); type = _guess(); }
    template<size_t N> explicit TestCaseNode(const char (&k)[N]   , AnchorRef const& ark, TaggedList s) : type(), key(k       ), key_tag(     ), key_anchor(ark), val(), val_tag(s.tag), val_anchor(), children(s.ilist), parent(nullptr) { _set_parent(); type = _guess(); }
                       explicit TestCaseNode(TaggedScalar const& k, AnchorRef const& ark, iseqmap    s) : type(), key(k.scalar), key_tag(k.tag), key_anchor(ark), val(), val_tag(     ), val_anchor(), children(s      ), parent(nullptr) { _set_parent(); type = _guess(); }
                       explicit TestCaseNode(TaggedScalar const& k, AnchorRef const& ark, TaggedList s) : type(), key(k.scalar), key_tag(k.tag), key_anchor(ark), val(), val_tag(s.tag), val_anchor(), children(s.ilist), parent(nullptr) { _set_parent(); type = _guess(); }
    // container, with key anchor/ref + val anchor/ref
    template<size_t N> explicit TestCaseNode(const char (&k)[N]   , AnchorRef const& ark, iseqmap    s, AnchorRef const& arv) : type(), key(k       ), key_tag(     ), key_anchor(ark), val(), val_tag(     ), val_anchor(arv), children(s      ), parent(nullptr) { _set_parent(); type = _guess(); }
    template<size_t N> explicit TestCaseNode(const char (&k)[N]   , AnchorRef const& ark, TaggedList s, AnchorRef const& arv) : type(), key(k       ), key_tag(     ), key_anchor(ark), val(), val_tag(s.tag), val_anchor(arv), children(s.ilist), parent(nullptr) { _set_parent(); type = _guess(); }
                       explicit TestCaseNode(TaggedScalar const& k, AnchorRef const& ark, iseqmap    s, AnchorRef const& arv) : type(), key(k.scalar), key_tag(k.tag), key_anchor(ark), val(), val_tag(     ), val_anchor(arv), children(s      ), parent(nullptr) { _set_parent(); type = _guess(); }
                       explicit TestCaseNode(TaggedScalar const& k, AnchorRef const& ark, TaggedList s, AnchorRef const& arv) : type(), key(k.scalar), key_tag(k.tag), key_anchor(ark), val(), val_tag(s.tag), val_anchor(arv), children(s.ilist), parent(nullptr) { _set_parent(); type = _guess(); }


    // container, explicit type
    template<size_t N> explicit TestCaseNode(NodeType t, const char (&k)[N]   , iseqmap    s) : type((t              )), key(k       ), key_tag(     ), key_anchor(), val(), val_tag(     ), val_anchor(), children(s      ), parent(nullptr) { _set_parent(); }
    template<size_t N> explicit TestCaseNode(NodeType t, const char (&k)[N]   , TaggedList s) : type((t       |VALTAG)), key(k       ), key_tag(     ), key_anchor(), val(), val_tag(s.tag), val_anchor(), children(s.ilist), parent(nullptr) { _set_parent(); }
                       explicit TestCaseNode(NodeType t, TaggedScalar const& k, iseqmap    s) : type((t       |KEYTAG)), key(k.scalar), key_tag(k.tag), key_anchor(), val(), val_tag(     ), val_anchor(), children(s      ), parent(nullptr) { _set_parent(); }
                       explicit TestCaseNode(NodeType t,                        iseqmap    s) : type((t              )), key(        ), key_tag(     ), key_anchor(), val(), val_tag(     ), val_anchor(), children(s      ), parent(nullptr) { _set_parent(); }
                       explicit TestCaseNode(NodeType t,                        TaggedList s) : type((t       |VALTAG)), key(        ), key_tag(     ), key_anchor(), val(), val_tag(s.tag), val_anchor(), children(s.ilist), parent(nullptr) { _set_parent(); }
                       explicit TestCaseNode(NodeType t, TaggedScalar const& k, TaggedList s) : type((t|KEYTAG|VALTAG)), key(k.scalar), key_tag(k.tag), key_anchor(), val(), val_tag(s.tag), val_anchor(), children(s.ilist), parent(nullptr) { _set_parent(); }
    // container, explicit type, with val anchor/ref
    template<size_t N> explicit TestCaseNode(NodeType t, const char (&k)[N]   , iseqmap    s, AnchorRef const& arv) : type((t       |VALANCH)), key(k       ), key_tag(     ), key_anchor(), val(), val_tag(     ), val_anchor(arv), children(s      ), parent(nullptr) { _set_parent(); }
    template<size_t N> explicit TestCaseNode(NodeType t, const char (&k)[N]   , TaggedList s, AnchorRef const& arv) : type((t|VALTAG|VALANCH)), key(k       ), key_tag(     ), key_anchor(), val(), val_tag(s.tag), val_anchor(arv), children(s.ilist), parent(nullptr) { _set_parent(); }
                       explicit TestCaseNode(NodeType t, TaggedScalar const& k, iseqmap    s, AnchorRef const& arv) : type((t|KEYTAG|VALANCH)), key(k.scalar), key_tag(k.tag), key_anchor(), val(), val_tag(     ), val_anchor(arv), children(s      ), parent(nullptr) { _set_parent(); }
                       explicit TestCaseNode(NodeType t,                        iseqmap    s, AnchorRef const& arv) : type((t       |VALANCH)), key(        ), key_tag(     ), key_anchor(), val(), val_tag(     ), val_anchor(arv), children(s      ), parent(nullptr) { _set_parent(); }
                       explicit TestCaseNode(NodeType t,                        TaggedList s, AnchorRef const& arv) : type((t|VALTAG|VALANCH)), key(        ), key_tag(     ), key_anchor(), val(), val_tag(s.tag), val_anchor(arv), children(s.ilist), parent(nullptr) { _set_parent(); }
    // container, explicit type, with key anchor/ref
    template<size_t N> explicit TestCaseNode(NodeType t, const char (&k)[N]   , AnchorRef const& ark, iseqmap    s) : type((t       |KEYANCH)), key(k       ), key_tag(     ), key_anchor(ark), val(), val_tag(     ), val_anchor(), children(s      ), parent(nullptr) { _set_parent(); }
    template<size_t N> explicit TestCaseNode(NodeType t, const char (&k)[N]   , AnchorRef const& ark, TaggedList s) : type((t|VALTAG|KEYANCH)), key(k       ), key_tag(     ), key_anchor(ark), val(), val_tag(s.tag), val_anchor(), children(s.ilist), parent(nullptr) { _set_parent(); }
                       explicit TestCaseNode(NodeType t, TaggedScalar const& k, AnchorRef const& ark, iseqmap    s) : type((t|KEYTAG|KEYANCH)), key(k.scalar), key_tag(k.tag), key_anchor(ark), val(), val_tag(     ), val_anchor(), children(s      ), parent(nullptr) { _set_parent(); }
    // container, explicit type, with key anchor/ref + val anchor/ref
    template<size_t N> explicit TestCaseNode(NodeType t, const char (&k)[N]   , AnchorRef const& ark, iseqmap    s, AnchorRef const& arv) : type((t       |KEYANCH|VALANCH)), key(k       ), key_tag(     ), key_anchor(ark), val(), val_tag(     ), val_anchor(arv), children(s      ), parent(nullptr) { _set_parent(); }
    template<size_t N> explicit TestCaseNode(NodeType t, const char (&k)[N]   , AnchorRef const& ark, TaggedList s, AnchorRef const& arv) : type((t|VALTAG|KEYANCH|VALANCH)), key(k       ), key_tag(     ), key_anchor(ark), val(), val_tag(s.tag), val_anchor(arv), children(s.ilist), parent(nullptr) { _set_parent(); }
                       explicit TestCaseNode(NodeType t, TaggedScalar const& k, AnchorRef const& ark, iseqmap    s, AnchorRef const& arv) : type((t|KEYTAG|KEYANCH|VALANCH)), key(k.scalar), key_tag(k.tag), key_anchor(ark), val(), val_tag(     ), val_anchor(arv), children(s      ), parent(nullptr) { _set_parent(); }

public:

    void _move(TestCaseNode&& that)
    {
        type = that.type;
        key = that.key;
        key_tag = that.key_tag;
        key_anchor = that.key_anchor;
        val = that.val;
        val_tag = that.val_tag;
        val_anchor = that.val_anchor;
        children = std::move(that.children);
        parent = nullptr;
        _set_parent();
    }
    void _copy(TestCaseNode const& that)
    {
        type = that.type;
        key = that.key;
        key_tag = that.key_tag;
        key_anchor = that.key_anchor;
        val = that.val;
        val_tag = that.val_tag;
        val_anchor = that.val_anchor;
        children = that.children;
        parent = nullptr;
        _set_parent();
    }

    void _set_parent()
    {
        C4_SUPPRESS_WARNING_GCC_PUSH
        #if defined(__GNUC__) && __GNUC__ > 9
        C4_SUPPRESS_WARNING_GCC("-Wanalyzer-possible-null-dereference")
        #endif
        for(auto &ch : children)
        {
            ch.parent = this;
        }
        C4_SUPPRESS_WARNING_GCC_POP
    }

    NodeType_e _guess() const;

    bool is_root() const { return parent; }
    bool is_doc() const { return type & DOC; }
    bool is_map() const { return type & MAP; }
    bool is_seq() const { return type & SEQ; }
    bool has_val() const { return type & VAL; }
    bool has_key() const { return type & KEY; }
    bool is_container() const { return type & (SEQ|MAP); }
    bool has_key_anchor() const { return type & KEYANCH; }
    bool has_val_anchor() const { return type & VALANCH; }

public:

    TestCaseNode const& operator[] (size_t i) const
    {
        C4_ASSERT(i < children.size());
        return children[i];
    }

    TestCaseNode const& operator[] (csubstr const& name) const
    {
        auto ch = lookup(name);
        C4_ASSERT(ch != nullptr);
        return *ch;
    }

    TestCaseNode const* lookup(csubstr const& name) const
    {
        C4_ASSERT( ! children.empty());
        for(auto const& ch : children)
            if(ch.key == name)
                return &ch;
        return nullptr;
    }

public:

    void compare(yml::ConstNodeRef const& n, bool ignore_quote=false) const;
    void compare_child(yml::ConstNodeRef const& n, id_type pos) const;

    id_type reccount() const
    {
        id_type c = 1;
        for(auto const& ch : children)
            c += ch.reccount();
        return c;
    }

    void recreate(yml::NodeRef *n) const;

};

} // namespace yml
} // namespace c4

#endif // _C4_YML_TEST_TEST_CASE_NODE_HPP_
