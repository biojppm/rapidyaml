#ifndef TEST_EMITS_COMMON_HPP_
#define TEST_EMITS_COMMON_HPP_

#ifndef C4_ERROR_HPP_
#include "c4/error.hpp"
#endif
#if defined(__GNUC__) && __GNUC__ >= 6
C4_SUPPRESS_WARNING_GCC("-Wnull-dereference")
#endif
#include "../test_lib/tree_and_ints.hpp"

namespace c4 {
namespace yml {

using extra::ievt::evt_bits;
using extra::ievt::evt_size;
using strref = std::string const&;
namespace xievt = extra::ievt;

static const EmitOptions without_dash = {};
static const EmitOptions with_dash = EmitOptions{}.emit_nonroot_dash(true);
static const EmitOptions with_key = {};
static const EmitOptions without_key = EmitOptions{}.emit_nonroot_key(false);
static const EmitOptions noindent = EmitOptions{}.indent_flow_ml(false);
static const EmitOptions with_flow_spc = EmitOptions{}.force_flow_spc(true);

void test_emits_tree(Tree const* tree, id_type id, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts={});
void test_emits_tree(ConstNodeRef node, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts={});

void test_emits_ints(IntBufsCR ints, evt_size pos, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts={}, bool with_json=true);
void test_emits_ints_nojson(IntBufsCR ints, evt_size pos, std::string const& expected_yaml, EmitOptions const& opts={});
void test_emits_ints(evt_size pos, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts={});

void test_emits(Tree const& t, id_type id, evt_size pos, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts={});
void test_emits(TreeAndInts const& ti, id_type id, evt_size pos, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts={});
void test_emits(Tree const& t, id_type id, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts={});
void test_emits(TreeAndInts const& ti, id_type id, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts={});

void test_emits(Tree const& t, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts={});
void test_emits(TreeAndInts const& ti, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts={});

void test_emits(ConstNodeRef node, evt_size pos, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts={});
void test_emits(TreeAndInts const& ti, ConstNodeRef node, evt_size pos, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts={});
void test_emits(ConstNodeRef node, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts={});
void test_emits(TreeAndInts const& ti, ConstNodeRef node, std::string const& expected_yaml, std::string const& expected_json, EmitOptions const& opts={});

#define test_emits_(...) { SCOPED_TRACE("test_emits"); test_emits(__VA_ARGS__); }
#define test_emits_tree_(...) { SCOPED_TRACE("test_emits"); test_emits_tree(__VA_ARGS__); }
#define test_emits_ints_(...) { SCOPED_TRACE("test_emits_ints"); test_emits_ints(__VA_ARGS__); }
#define test_emits_ints_nojson_(...) { SCOPED_TRACE("test_emits_ints_nojson"); test_emits_ints_nojson(__VA_ARGS__); }


//-----------------------------------------------------------------------------

struct TmpStyle // NOLINT
{
    Tree *tree;
    id_type id;
    NodeType prev;
    IntBufs *ints;
    evt_size evt_pos;
    evt_bits evt_prev;
    bool with_key;
    evt_size evt_pos_key;
    evt_bits evt_prev_key;
    extra::ievt::evt_bits prev_ints;
    TmpStyle(TreeAndInts &ti, NodeRef const& n, NodeType tmp, evt_size evt_pos_, evt_bits evt,
             bool with_key_=false, evt_size evt_pos_key_={})
        : TmpStyle(&ti.tree, &ti.ints, n.id(), tmp, evt_pos_, evt, with_key_, evt_pos_key_) {}
    TmpStyle(Tree *tree_, IntBufs *ints_, id_type id_, NodeType tmp, evt_size evt_pos_, evt_bits evt,
             bool with_key_=false, evt_size evt_pos_key_={});
    ~TmpStyle();

    // legacy:
    TmpStyle(NodeRef n, NodeType tmp) : TmpStyle(n.tree(), nullptr, n.id(), tmp, {}, {}) {}
    TmpStyle(Tree &t_, id_type id_, NodeType tmp) : TmpStyle(&t_, nullptr, id_, tmp, {}, {}) {}
    TmpStyle(TreeAndInts &ti, evt_size evt_pos_, evt_bits evt) : TmpStyle(nullptr, &ti.ints, {}, {}, evt_pos_, evt) {}
    TmpStyle(TreeAndInts &ti, NodeType tmp, evt_bits evt) : TmpStyle(&ti.tree, &ti.ints, ti.tree.root_id(), tmp, 0, evt) {}
};

inline TmpStyle mknosty(TreeAndInts &ti, NodeRef n, evt_size pos) { return {ti, n, NodeType{},  pos, evt_bits{}}; }
inline TmpStyle mkplai(TreeAndInts &ti, NodeRef n, evt_size pos) { return {ti, n, VAL_PLAIN,  pos, xievt::PLAI}; }
inline TmpStyle mksquo(TreeAndInts &ti, NodeRef n, evt_size pos) { return {ti, n, VAL_SQUO,  pos, xievt::SQUO}; }
inline TmpStyle mkdquo(TreeAndInts &ti, NodeRef n, evt_size pos) { return {ti, n, VAL_DQUO,  pos, xievt::DQUO}; }
inline TmpStyle mkfold(TreeAndInts &ti, NodeRef n, evt_size pos) { return {ti, n, VAL_FOLDED,  pos, xievt::FOLD}; }
inline TmpStyle mklitl(TreeAndInts &ti, NodeRef n, evt_size pos) { return {ti, n, VAL_LITERAL,  pos, xievt::LITL}; }

inline TmpStyle mknostykv(TreeAndInts &ti, NodeRef n, evt_size kpos, evt_size vpos) { return {ti, n, NOTYPE,  vpos, evt_bits{}, true,  kpos}; }
inline TmpStyle mkplaikv(TreeAndInts &ti, NodeRef n, evt_size kpos, evt_size vpos) { return {ti, n, KEY_PLAIN|VAL_PLAIN,  vpos, xievt::PLAI, true,  kpos}; }
inline TmpStyle mksquokv(TreeAndInts &ti, NodeRef n, evt_size kpos, evt_size vpos) { return {ti, n, KEY_SQUO|VAL_SQUO,  vpos, xievt::SQUO, true,  kpos}; }
inline TmpStyle mkdquokv(TreeAndInts &ti, NodeRef n, evt_size kpos, evt_size vpos) { return {ti, n, KEY_DQUO|VAL_DQUO,  vpos, xievt::DQUO, true,  kpos}; }
inline TmpStyle mkfoldkv(TreeAndInts &ti, NodeRef n, evt_size kpos, evt_size vpos) { return {ti, n, KEY_FOLDED|VAL_FOLDED,  vpos, xievt::FOLD, true,  kpos}; }
inline TmpStyle mklitlkv(TreeAndInts &ti, NodeRef n, evt_size kpos, evt_size vpos) { return {ti, n, KEY_LITERAL|VAL_LITERAL,  vpos, xievt::LITL, true,  kpos}; }

inline TmpStyle mkblk(TreeAndInts &ti, NodeRef n, evt_size pos) { return {ti, n, BLOCK,  pos, 0}; }
inline TmpStyle mkflowsl(TreeAndInts &ti, NodeRef n, evt_size pos, evt_bits spc=0) { return {ti, n, FLOW_SL,  pos, xievt::FLOW|xievt::FSL_|spc}; }
inline TmpStyle mkflowml1(TreeAndInts &ti, NodeRef n, evt_size pos, evt_bits spc=0) { return {ti, n, FLOW_ML1, pos, xievt::FLOW|xievt::FML1|spc}; }
inline TmpStyle mkflowmln(TreeAndInts &ti, NodeRef n, evt_size pos, evt_bits spc=0) { return {ti, n, FLOW_MLN, pos, xievt::FLOW|xievt::FMLN|spc}; }

inline TmpStyle mknosty(IntBufs &ti, evt_size pos) { return {nullptr, &ti, 0, NOTYPE, pos, evt_bits{}}; }
inline TmpStyle mkblk(IntBufs &ti, evt_size pos) { return {nullptr, &ti, 0, NOTYPE, pos, 0}; }
inline TmpStyle mkflowsl(IntBufs &ti, evt_size pos, evt_bits spc=0) { return {nullptr, &ti, 0, NOTYPE, pos, xievt::FLOW|xievt::FSL_|spc}; }
inline TmpStyle mkflowml1(IntBufs &ti, evt_size pos, evt_bits spc=0) { return {nullptr, &ti, 0, NOTYPE, pos, xievt::FLOW|xievt::FML1|spc}; }
inline TmpStyle mkflowmln(IntBufs &ti, evt_size pos, evt_bits spc=0) { return {nullptr, &ti, 0, NOTYPE, pos, xievt::FLOW|xievt::FMLN|spc}; }
// use the extra arg to disambiguate overloads above ^
inline TmpStyle mknosty(IntBufs &ti, IntBufs &, evt_size kpos, evt_size vpos) { return {nullptr, &ti, 0, NOTYPE, vpos, evt_bits{}, true, kpos}; }
inline TmpStyle mkblk(IntBufs &ti, IntBufs &, evt_size kpos, evt_size vpos) { return {nullptr, &ti, 0, NOTYPE, vpos, 0, true, kpos}; }
inline TmpStyle mkflowsl(IntBufs &ti, IntBufs &, evt_size kpos, evt_size vpos, evt_bits spc=0) { return {nullptr, &ti, 0, NOTYPE, vpos, xievt::FLOW|xievt::FSL_|spc, true, kpos}; }
inline TmpStyle mkflowml1(IntBufs &ti, IntBufs &, evt_size kpos, evt_size vpos, evt_bits spc=0) { return {nullptr, &ti, 0, NOTYPE, vpos, xievt::FLOW|xievt::FML1|spc, true, kpos}; }
inline TmpStyle mkflowmln(IntBufs &ti, IntBufs &, evt_size kpos, evt_size vpos, evt_bits spc=0) { return {nullptr, &ti, 0, NOTYPE, vpos, xievt::FLOW|xievt::FMLN|spc, true, kpos}; }

#define TMPSTY(sty, ...) TmpStyle C4_XCAT(tmpsty_, __LINE__) = mk##sty(__VA_ARGS__)

} // namespace yml
} // namespace c4


#endif /* TEST_EMITS_COMMON_HPP_ */
