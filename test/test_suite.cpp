#include <gtest/gtest.h>

#include <c4/substr.hpp>
#include <c4/std/string.hpp>
#include <c4/fs/fs.hpp>
#include <c4/log/log.hpp>

#include <c4/yml/tree.hpp>
#include <c4/yml/parse.hpp>
#include <c4/yml/emit.hpp>
#include <c4/yml/detail/print.hpp>
#include <c4/yml/detail/checks.hpp>

#include "test_case.hpp"

#define RYML_NFO (1 || RYML_DBG)


C4_SUPPRESS_WARNING_MSVC_PUSH
C4_SUPPRESS_WARNING_MSVC(4702) // unreachable code

namespace c4 {
namespace yml {

/* Each case from the test suite contains:
 *
 *  - (awkward) input yaml (in_yaml)
 *  - (somewhat standard) output equivalent (out_yaml)
 *  - (when meaningful/possible) json equivalent (in_json)
 *  - yaml parsing events (events)
 *
 * Running a test consists of parsing the contents above into a data
 * structure, and then repeatedly parsing and emitting yaml in a sort
 * of pipe. Ie, (eg for in_yaml) parse in_yaml, emit corresponding
 * yaml, then parse this emitted yaml, and so on. Each parse/emit pair
 * is named a processing level in this test. */


// this is the number of processing levels
#define NLEVELS 4


typedef enum {
    CPART_NONE = 0,
    CPART_IN_YAML = 1 << 0,
    CPART_IN_JSON = 1 << 1,
    CPART_OUT_YAML = 1 << 2,
    CPART_EMIT_YAML = 1 << 3,
    CPART_EVENTS = 1 << 4,
    CPART_ALL = CPART_IN_YAML|CPART_IN_JSON|CPART_OUT_YAML|CPART_EMIT_YAML|CPART_EVENTS,
    CPART_ANY = CPART_ALL,
} CasePart_e;
constexpr CasePart_e operator| (CasePart_e lhs, CasePart_e rhs) noexcept { return (CasePart_e)((int)lhs|(int)rhs); }


csubstr to_csubstr(CasePart_e cp) noexcept
{
    if(cp == CPART_NONE) return {"NONE"};
    else if(cp == CPART_IN_YAML) return {"IN_YAML"};
    else if(cp == CPART_IN_JSON) return {"IN_JSON"};
    else if(cp == CPART_OUT_YAML) return {"OUT_YAML"};
    else if(cp == CPART_EVENTS) return {"EVENTS"};
    else if(cp == CPART_ALL) return {"ALL"};
    return {"<unknown>"};
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


struct AllowedFailure
{
    csubstr test_code;
    CasePart_e contexts;
    csubstr reason;
    constexpr AllowedFailure() : test_code(), contexts(), reason() {}
    constexpr AllowedFailure(csubstr tc, CasePart_e ctx, csubstr r) : test_code(tc), contexts(ctx), reason(r) {}
    bool skip() const { return skip(CPART_ALL); }
    bool skip(CasePart_e case_part) const
    {
        if((case_part != CPART_ALL && (contexts & case_part))
           ||
           (case_part == CPART_ALL && (contexts == CPART_ALL)))
        {
            c4::log("skipping test {} in {}: matches {}. Reason: {}",
                    test_code, to_csubstr(case_part), to_csubstr(contexts), reason);
            return true;
        }
        return false;
    }
};


// To see the test case contents, refer to this URL:
// https://github.com/yaml/yaml-test-suite/tree/master/test/
constexpr const AllowedFailure g_allowed_failures[] = {
    // we do not accept container keys
    {"4FJ6", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs}"},
    {"6BFJ", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs}"},
    {"6PBE", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs}"},
    {"KK5P", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs}"},
    {"KZN9", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs}"},
    {"LX3P", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs}"},
    {"M5DY", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs}"},
    {"Q9WF", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs}"},
    {"SBG9", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs}"},
    {"X38W", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs}"},
    {"XW4D", CPART_ALL, "only string keys allowed (keys cannot be maps or seqs}"},
    // we do not accept anchors with :
    {"2SXE", CPART_IN_YAML|CPART_OUT_YAML|CPART_EVENTS, "weird characters in anchors, anchors must not end with :"},
    {"W5VH", CPART_IN_YAML, "TODO[next]: weird characters in anchors"},

    // malformed json
    {"35KP", CPART_IN_JSON|CPART_EVENTS, "malformed JSON from multiple documents,scalar 'd e' is wrongly parsed with trailing newline"},
    {"6XDY", CPART_IN_JSON|CPART_EVENTS, "malformed JSON from multiple documents|empty doc does not get an empty value added to it"},
    {"6ZKB", CPART_IN_JSON|CPART_IN_YAML|CPART_EVENTS, "malformed JSON from multiple documents|TODO[next]: document handling"},
    {"7Z25", CPART_IN_JSON, "malformed JSON from multiple documents"},
    {"9DXL", CPART_IN_JSON|CPART_IN_YAML|CPART_EMIT_YAML, "malformed JSON from multiple documents|TODO[next]: document handling"},
    {"9KAX", CPART_IN_JSON, "malformed JSON from multiple documents"},
    {"JHB9", CPART_IN_JSON, "malformed JSON from multiple documents"},
    {"KSS4", CPART_IN_JSON|CPART_EVENTS, "malformed JSON from multiple documents"},
    {"M7A3", CPART_IN_JSON|CPART_IN_YAML|CPART_EVENTS, "malformed JSON from multiple documents|TODO[next]: plain scalar parsing, same indentation on next line is problematic"},
    {"RZT7", CPART_IN_JSON|CPART_EVENTS, "malformed JSON from multiple documents"},
    {"U9NS", CPART_IN_JSON|CPART_EVENTS, "malformed JSON from multiple documents"},
    {"W4TN", CPART_IN_JSON|CPART_EVENTS, "malformed JSON from multiple documents"},

    // TODO
    {"S4T7", CPART_EVENTS, "(false positive) difference in behavior when parsing events"},
    {"U9NS", CPART_EVENTS, "(false positive) difference in behavior when parsing events"},
    {"36F6", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"4Q9F", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"4ZYM", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"565N", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"5BVJ", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"5GBF", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"6SLA", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"6WPF", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"77H8", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"7A4E", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"93WF", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"96L6", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"9TFX", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"A6F9", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"B3HG", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"DWX9", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"F6MC", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"F8F9", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"G4RS", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"G992", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"H2RW", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"HMK4", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"HS5T", CPART_EVENTS|CPART_IN_YAML, "need to unescape the newline in the scalar from the events source"},
    {"J3BT", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"M29M", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"M9B4", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"MJS9", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"MZX3", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"NB6Z", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"NP9H", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"PRH3", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"Q8AD", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"R4YG", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"RZT7", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"T26H", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"TL85", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"W42U", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"W4TN", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"XV9V", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"Z67P", CPART_EVENTS, "need to unescape the newline in the scalar from the events source"},
    {"RZP5", CPART_IN_YAML|CPART_OUT_YAML, "plain scalar block parsing, anchors"},
    {"735Y", CPART_IN_YAML|CPART_EVENTS, "plain scalar parsing|need to unescape the newline in the scalar from the events source"},
    {"7T8X", CPART_IN_YAML|CPART_OUT_YAML, "scalar block parsing"},
    {"82AN", CPART_IN_YAML, "plain scalar parsing, same indentation on next line is problematic"},
    {"9YRD", CPART_IN_YAML|CPART_EVENTS, "plain scalar parsing, same indentation on next line is problematic|need to unescape the newline in the scalar from the events source"},
    {"EXG3", CPART_IN_YAML|CPART_EVENTS, "plain scalar parsing, same indentation on next line is problematic"},
    {"EX5H", CPART_IN_YAML|CPART_EMIT_YAML|CPART_EVENTS, "plain scalar parsing, same indentation on next line is problematic|need to unescape the newline in the scalar from the events source"},
    {"HS5T", CPART_IN_YAML|CPART_EVENTS, "plain scalar parsing, same indentation on next line is problematic"},
    {"3MYT", CPART_EVENTS, "emitting block scalars is not idempotent"},
    {"4CQQ", CPART_EVENTS, "emitting block scalars is not idempotent"},
    {"4QFQ", CPART_EVENTS, "emitting block scalars is not idempotent"},
    {"5WE3", CPART_EVENTS, "emitting block scalars is not idempotent"},
    {"6HB6", CPART_EVENTS, "emitting block scalars is not idempotent"},
    {"6JQW", CPART_EVENTS, "emitting block scalars is not idempotent"},
    {"6VJK", CPART_EVENTS, "emitting block scalars is not idempotent"},
    {"7TMG", CPART_EVENTS, "multiline scalar is parsed wrong"},
    {"7W2P", CPART_EVENTS, "multiline scalar is parsed wrong"},
    {"A984", CPART_EVENTS, "emitting block scalars is not idempotent"},
    {"AB8U", CPART_EVENTS, "emitting block scalars is not idempotent"},
    {"D83L", CPART_EVENTS, "block scalars: multiline problems"},
    {"DK3J", CPART_EVENTS, "emitting block scalars is not idempotent"},
    {"FP8R", CPART_EVENTS, "emitting block scalars is not idempotent"},
    {"JDH8", CPART_EVENTS, "emitting block scalars is not idempotent"},
    {"K527", CPART_EVENTS, "emitting block scalars is not idempotent"},
    {"KSS4", CPART_EVENTS, "emitting block scalars is not idempotent"},
    {"M5C3", CPART_EVENTS, "emitting block scalars is not idempotent"},
    {"M7A3", CPART_EVENTS, "emitting block scalars is not idempotent"},
    {"K858", CPART_OUT_YAML|CPART_IN_JSON|CPART_EVENTS, "emitting block scalars is not idempotent"},
    {"NAT4", CPART_IN_YAML|CPART_EMIT_YAML|CPART_IN_JSON, "emitting block scalars is not idempotent"},
    {"NJ66", CPART_EVENTS, "emitting block scalars is not idempotent"},
    {"P2AD", CPART_EVENTS, "emitting block scalars is not idempotent"},
    {"T4YY", CPART_EVENTS, "emitting block scalars is not idempotent"},
    {"T5N4", CPART_EVENTS, "emitting block scalars is not idempotent"},
    {"TS54", CPART_EVENTS, "emitting block scalars is not idempotent"},
    {"UT92", CPART_EVENTS, "emitting block scalars is not idempotent"},
    {"XLQ9", CPART_EVENTS, "emitting block scalars is not idempotent"},
    {"52DL", CPART_EVENTS, "single ! is parsed as a tag"},
    {"5TYM", CPART_EVENTS, "wrong parse result from tags"},
    {"6CK3", CPART_EVENTS, "wrong parse result from tags"},
    {"6WLZ", CPART_EVENTS, "single ! is parsed as a tag"},
    {"7FWL", CPART_EVENTS, "tags are parsed wrong"},
    {"8MK2", CPART_EVENTS, "tags are parsed wrong"},
    {"9WXW", CPART_EVENTS, "tags are inconsistently treated with !"},
    {"C4HZ", CPART_EVENTS, "tags are inconsistently treated with !"},
    {"CC74", CPART_EVENTS, "tags are inconsistently treated with !"},
    {"CUP7", CPART_EVENTS, "tags are inconsistently treated with !"},
    {"EHF6", CPART_EVENTS, "emission of tags is not idempotent"},
    {"P76L", CPART_EVENTS, "wrong parse result from tags"},
    {"S4JQ", CPART_EVENTS, "wrong parse result from tags"},
    {"U3C3", CPART_EVENTS, "wrong parse result from tags"},
    {"UGM3", CPART_EVENTS, "wrong parse result from tags"},
    {"Z9M4", CPART_EVENTS, "wrong parse result from tags"},
    {"DFF7", CPART_EVENTS, "problem with implicit key"},
    {"FH7J", CPART_IN_YAML|CPART_OUT_YAML, "implicit keys"},
    {"FRK4", CPART_EVENTS, "implicit key is wrongly parsed"},
    {"V9D5", CPART_EVENTS, "null key is wrongly parsed"},
    {"X8DW", CPART_EVENTS, "null key is wrongly parsed"},
    {"ZWK4", CPART_EVENTS, "null key is wrongly parsed"},
    {"3UYS", CPART_IN_YAML, "no need to escape the slash in \"a\\/b\""},
    {"4ABK", CPART_EVENTS, "key is wrongly serialized: 'omitted value:'"},
    {"PW8X", CPART_IN_YAML|CPART_OUT_YAML, "anchors with implicit key"},
    {"6BCT", CPART_IN_YAML, "allow tabs after - or :"},
    {"K54U", CPART_EVENTS, "problem with tab after ---"},
    {"DC7X", CPART_IN_YAML, "improve handling of tab characters"},
    {"6FWR", CPART_EMIT_YAML|CPART_EVENTS, "fail to parse"},
    {"9MMW", CPART_IN_YAML, "re the json/yaml incompatibility where a space is required after :"},
    {"A2M4", CPART_EVENTS, "fails to parse the value sequence, parses as scalar"},
    {"CN3R", CPART_IN_YAML|CPART_OUT_YAML, "anchors + maps nested in seqs"},
    {"G5U8", CPART_ALL, "sequences with -"},
    {"H3Z8", CPART_EVENTS, "need to unescape the utf8 characters"},
    {"PUW8", CPART_EVENTS, "empty doc must have an empty val"},
    {"RTP8", CPART_EVENTS, "empty doc wrongly added at end"},
    {"Q5MG", CPART_EVENTS, "'   {}' is wrongly parsed as a val"},
};


AllowedFailure is_failure_expected(csubstr filename)
{
    RYML_CHECK(filename.ends_with(".tml"));
    csubstr test_code = filename.basename();
    test_code = test_code.offs(0, 4);
    for(AllowedFailure af : g_allowed_failures)
        if(af.test_code == test_code)
            return af;
    return {};
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#if RYML_NFO
#define _nfo_print_tree(title, tree) do { c4::log("{}:{}: " title ":", __FILE__, __LINE__); print_tree(tree); c4::yml::emit(tree, stdout); fflush(stdout); } while(0)
#define _nfo_logf(fmt, ...)          do { c4::log("{}:{}: " fmt      , __FILE__, __LINE__, __VA_ARGS__);                                   fflush(stdout); } while(0)
#define _nfo_log(fmt)                do { c4::log("{}:{}: " fmt      , __FILE__, __LINE__);                                                fflush(stdout); } while(0)
#define _nfo_printf(...) printf(__VA_ARGS__)
#else
#define _nfo_print_tree(title, tree)
#define _nfo_logf(fmt, ...)
#define _nfo_log(fmt)
#define _nfo_printf(...)
#endif
#define _nfo_llogf(fmt, ...) _nfo_logf("line[{}]: '{}': " fmt, linenum, line, __VA_ARGS__)
#define _nfo_llog(fmt)       _nfo_logf("line[{}]: '{}': " fmt, linenum, line)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct OptionalScalar
{
    csubstr val = {};
    bool was_set = false;
    inline operator csubstr() const { return get(); }
    inline operator bool() const { return was_set; }
    void operator= (csubstr v) { val = v; was_set = true; }
    csubstr get() const { RYML_ASSERT(was_set); return val; }
};

size_t to_chars(c4::substr buf, OptionalScalar const& s)
{
    if(!s)
        return 0u;
    if(s.val.len <= buf.len)
        memcpy(buf.str, s.val.str, s.val.len);
    return s.val.len;
}


struct Scalar
{
    OptionalScalar scalar = {};
    OptionalScalar anchor = {};
    OptionalScalar ref    = {};
    OptionalScalar tag    = {};
    bool           quoted = false;
    inline operator bool() const { if(anchor || tag) { RYML_ASSERT(scalar); } return scalar.was_set; }
    void add_key_props(Tree *tree, size_t node) const
    {
        if(ref)
        {
            _nfo_logf("node[{}]: set key ref: '{}'", node, ref);
            tree->set_key_ref(node, ref);
        }
        if(anchor)
        {
            _nfo_logf("node[{}]: set key anchor: '{}'", node, anchor);
            tree->set_key_anchor(node, anchor);
        }
        if(tag)
        {
            csubstr ntag = normalize_tag(tag);
            _nfo_logf("node[{}]: set key tag: '{}' -> '{}'", node, tag, ntag);
            tree->set_key_tag(node, ntag);
        }
        if(quoted)
        {
            _nfo_logf("node[{}]: set key as quoted", node);
            tree->_add_flags(node, KEYQUO);
        }
    }
    void add_val_props(Tree *tree, size_t node) const
    {
        if(ref)
        {
            _nfo_logf("node[{}]: set val ref: '{}'", node, ref);
            tree->set_val_ref(node, ref);
        }
        if(anchor)
        {
            _nfo_logf("node[{}]: set val anchor: '{}'", node, anchor);
            tree->set_val_anchor(node, anchor);
        }
        if(tag)
        {
            csubstr ntag = normalize_tag(tag);
            _nfo_logf("node[{}]: set val tag: '{}' -> '{}'", node, tag, ntag);
            tree->set_val_tag(node, ntag);
        }
        if(quoted)
        {
            _nfo_logf("node[{}]: set val as quoted", node);
            tree->_add_flags(node, VALQUO);
        }
    }
};

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
        _nfo_logf("anchor: {}", *anchor);
    }
    if(tokens.begins_with('<'))
    {
        size_t pos = tokens.find('>');
        RYML_ASSERT(pos != (size_t)csubstr::npos);
        *tag = tokens.first(pos + 1);
        tokens = tokens.right_of(pos).triml(' ');
        _nfo_logf("tag: {}", *tag);
    }
    return tokens;
}


struct EventsParser
{
    void parse(csubstr src, Tree *C4_RESTRICT tree_)
    {
        struct ParseLevel { size_t tree_node; };
        detail::stack<ParseLevel> m_stack = {};
        Tree &C4_RESTRICT tree = *tree_;
        size_t linenum = 0;
        Scalar key = {};
        _nfo_logf("parsing events! src:\n{}", src);
        for(csubstr line : src.split('\n'))
        {
            line = line.trimr('\r');
            _nfo_printf("\n\n-----------------------\n");
            {
                size_t curr = m_stack.empty() ? tree.root_id() : m_stack.top().tree_node;
                _nfo_llog("");
                _nfo_logf("line[{}]: top={} type={}", linenum, curr, NodeType::type_str(tree.type(curr)));
            }
            if(line.begins_with("=VAL "))
            {
                line = line.stripl("=VAL ");
                ASSERT_GE(m_stack.size(), 0u);
                Scalar curr = {};
                line = parse_anchor_and_tag(line, &curr.anchor, &curr.tag);
                if(line.begins_with('"') || line.begins_with('\''))
                {
                    _nfo_llog("quoted scalar!");
                    curr.scalar = line.sub(1);
                    curr.quoted = true;
                }
                else if(line.begins_with('|') || line.begins_with('>'))
                {
                    curr.scalar = line.sub(1);
                    _nfo_llog("block scalar!");
                }
                else
                {
                    ASSERT_EQ(line.begins_with(':'), true);
                    curr.scalar = line.sub(1);
                }
                _nfo_logf("parsed scalar: '{}'", curr.scalar);
                if(m_stack.empty())
                {
                    _nfo_log("stack was empty, pushing root as DOC...");
                    //tree._p(tree.root_id())->m_type.add(DOC);
                    m_stack.push({tree.root_id()});
                }
                ParseLevel &top = m_stack.top();
                if(tree.is_seq(top.tree_node))
                {
                    _nfo_logf("is seq! seq_id={}", top.tree_node);
                    ASSERT_EQ(key, false);
                    ASSERT_EQ(curr, true);
                    _nfo_logf("seq[{}]: adding child", top.tree_node);
                    size_t node = tree.append_child(top.tree_node);
                    _nfo_logf("seq[{}]: child={} val='{}'", top.tree_node, node, curr.scalar);
                    tree.to_val(node, curr.scalar);
                    curr.add_val_props(&tree, node);
                }
                else if(tree.is_map(top.tree_node))
                {
                    _nfo_logf("is map! map_id={}", top.tree_node);
                    if(!key)
                    {
                        _nfo_logf("store key='{}' anchor='{}' tag='{}' quoted={}", curr.scalar, curr.anchor, curr.tag, curr.quoted);
                        key = curr;
                    }
                    else
                    {
                        _nfo_logf("map[{}]: adding child", top.tree_node);
                        size_t node = tree.append_child(top.tree_node);
                        _nfo_logf("map[{}]: child={} key='{}' val='{}'", top.tree_node, node, key.scalar, curr.scalar);
                        tree.to_keyval(node, key.scalar, curr.scalar);
                        key.add_key_props(&tree, node);
                        curr.add_val_props(&tree, node);
                        _nfo_logf("clear key='{}'", key.scalar);
                        key = {};
                    }
                }
                else
                {
                    _nfo_logf("setting tree_node={} to DOCVAL...", top.tree_node);
                    tree.to_val(top.tree_node, curr.scalar, DOC);
                    curr.add_val_props(&tree, top.tree_node);
                }
            }
            else if(line.begins_with("=ALI "))
            {
                csubstr alias = line.stripl("=ALI ");
                _nfo_logf("REF token: {}", alias);
                ParseLevel top = m_stack.top();
                if(tree.is_seq(top.tree_node))
                {
                    _nfo_logf("node[{}] is seq: set {} as val ref", top.tree_node, alias);
                    ASSERT_EQ(key, false);
                    size_t node = tree.append_child(top.tree_node);
                    tree.to_val(node, alias);
                    tree.set_val_ref(node, alias);
                }
                else if(tree.is_map(top.tree_node))
                {
                    if(key)
                    {
                        _nfo_logf("node[{}] is map and key '{}' is pending: set {} as val ref", top.tree_node, key.scalar, alias);
                        size_t node = tree.append_child(top.tree_node);
                        tree.to_keyval(node, key.scalar, alias);
                        key.add_key_props(&tree, node);
                        tree.set_val_ref(node, alias);
                        _nfo_logf("clear key='{}'", key);
                        key = {};
                    }
                    else
                    {
                        _nfo_logf("node[{}] is map and no key is pending: save {} as key ref", top.tree_node, alias);
                        key.scalar = alias;
                        key.ref = alias;
                    }
                }
                else
                {
                    C4_ERROR("ALI event requires map or seq");
                }
            }
            else if(line.begins_with("+SEQ"))
            {
                _nfo_log("pushing SEQ");
                OptionalScalar anchor = {};
                OptionalScalar tag = {};
                parse_anchor_and_tag(line.stripl("+SEQ").triml(' '), &anchor, &tag);
                size_t node = tree.root_id();
                if(m_stack.empty())
                {
                    _nfo_log("stack was empty, set root to SEQ");
                    tree._add_flags(node, SEQ);
                    m_stack.push({node});
                    ASSERT_EQ(key, false); // for the key to exist, the parent must exist and be a map
                }
                else
                {
                    size_t parent = m_stack.top().tree_node;
                    _nfo_logf("stack was not empty. parent={}", parent);
                    ASSERT_NE(parent, (size_t)NONE);
                    NodeType more_flags = NOTYPE;
                    if(tree.is_doc(parent) && !(tree.is_seq(parent) || tree.is_map(parent)))
                    {
                        _nfo_logf("set node to parent={}, add DOC", parent);
                        node = parent;
                        more_flags.add(DOC);
                    }
                    else
                    {
                        _nfo_logf("add child to parent={}", parent);
                        node = tree.append_child(parent);
                        m_stack.push({node});
                        _nfo_logf("add child to parent={}: child={}", parent, node);
                    }
                    if(key)
                    {
                        _nfo_logf("has key, set to keyseq: parent={} child={} key='{}'", parent, node, key);
                        ASSERT_EQ(tree.is_map(parent) || node == parent, true);
                        tree.to_seq(node, key.scalar, more_flags);
                        key.add_key_props(&tree, node);
                        _nfo_logf("clear key='{}'", key.scalar);
                        key = {};
                    }
                    else
                    {
                        if(tree.is_map(parent))
                        {
                            _nfo_logf("has null key, set to keyseq: parent={} child={}", parent, node);
                            ASSERT_EQ(tree.is_map(parent) || node == parent, true);
                            tree.to_seq(node, csubstr{}, more_flags);
                        }
                        else
                        {
                            _nfo_logf("no key, set to seq: parent={} child={}", parent, node);
                            tree.to_seq(node, more_flags);
                        }
                    }
                }
                if(tag)
                    tree.set_val_tag(node, normalize_tag(tag));
                if(anchor)
                    tree.set_val_anchor(node, anchor);
            }
            else if(line.begins_with("+MAP"))
            {
                _nfo_log("pushing MAP");
                OptionalScalar anchor = {};
                OptionalScalar tag = {};
                parse_anchor_and_tag(line.stripl("+MAP").triml(' '), &anchor, &tag);
                size_t node = tree.root_id();
                if(m_stack.empty())
                {
                    _nfo_log("stack was empty, set root to MAP");
                    tree._add_flags(node, MAP);
                    m_stack.push({node});
                    ASSERT_EQ(key, false); // for the key to exist, the parent must exist and be a map
                }
                else
                {
                    size_t parent = m_stack.top().tree_node;
                    _nfo_logf("stack was not empty. parent={}", parent);
                    ASSERT_NE(parent, (size_t)NONE);
                    NodeType more_flags = NOTYPE;
                    if(tree.is_doc(parent) && !(tree.is_seq(parent) || tree.is_map(parent)))
                    {
                        _nfo_logf("set node to parent={}, add DOC", parent);
                        node = parent;
                        more_flags.add(DOC);
                    }
                    else
                    {
                        _nfo_logf("add child to parent={}", parent);
                        node = tree.append_child(parent);
                        m_stack.push({node});
                        _nfo_logf("add child to parent={}: child={}", parent, node);
                    }
                    if(key)
                    {
                        _nfo_logf("has key, set to keymap: parent={} child={} key='{}'", parent, node, key);
                        ASSERT_EQ(tree.is_map(parent) || node == parent, true);
                        tree.to_map(node, key.scalar, more_flags);
                        key.add_key_props(&tree, node);
                        _nfo_logf("clear key='{}'", key.scalar);
                        key = {};
                    }
                    else
                    {
                        if(tree.is_map(parent))
                        {
                            _nfo_logf("has null key, set to keymap: parent={} child={}", parent, node);
                            ASSERT_EQ(tree.is_map(parent) || node == parent, true);
                            tree.to_map(node, csubstr{}, more_flags);
                        }
                        else
                        {
                            _nfo_logf("no key, set to map: parent={} child={}", parent, node);
                            tree.to_map(node, more_flags);
                        }
                    }
                }
                if(tag)
                    tree.set_val_tag(node, normalize_tag(tag));
                if(anchor)
                    tree.set_val_anchor(node, anchor);
            }
            else if(line.begins_with("-SEQ"))
            {
                _nfo_logf("popping SEQ, empty={}", m_stack.empty());
                size_t node;
                if(m_stack.empty())
                    node = tree.root_id();
                else
                    node = m_stack.pop().tree_node;
                ASSERT_EQ(tree.is_seq(node), true) << "node=" << node;
            }
            else if(line.begins_with("-MAP"))
            {
                _nfo_logf("popping MAP, empty={}", m_stack.empty());
                size_t node;
                if(m_stack.empty())
                    node = tree.root_id();
                else
                    node = m_stack.pop().tree_node;
                ASSERT_EQ(tree.is_map(node), true) << "node=" << node;
            }
            else if(line.begins_with("+DOC"))
            {
                csubstr rem = line.stripl("+DOC").triml(' ');
                _nfo_logf("pushing DOC: {}", rem);
                size_t node = tree.root_id();
                auto is_sep = rem.first_of_any("---\n", "--- ", "---\r") || rem.ends_with("---");
                ASSERT_EQ(key, false); // for the key to exist, the parent must exist and be a map
                if(m_stack.empty())
                {
                    _nfo_log("stack was empty");
                    ASSERT_EQ(node, tree.root_id());
                    if(tree.is_stream(node))
                    {
                        _nfo_log("there is already a stream, append a DOC");
                        node = tree.append_child(node);
                        tree.to_doc(node);
                        m_stack.push({node});
                    }
                    else if(is_sep)
                    {
                        _nfo_logf("separator was specified: {}", rem);
                        if(!tree.is_container(node))
                        {
                            tree._add_flags(node, STREAM);
                            node = tree.append_child(node);
                            _nfo_logf("create STREAM at {} and add DOC child={}", tree.root_id(), node);
                            tree.to_doc(node);
                            m_stack.push({node});
                        }
                        else
                        {
                            _nfo_log("rearrange root as STREAM");
                            tree.set_root_as_stream();
                            node = tree.append_child(tree.root_id());
                            _nfo_logf("added doc as STREAM child: {}", node);
                            tree.to_doc(node);
                            m_stack.push({node});
                        }
                    }
                    else
                    {
                        if(tree.is_doc(node))
                        {
                            _nfo_log("rearrange root as STREAM");
                            tree.set_root_as_stream();
                            m_stack.push({node});
                        }
                    }
                }
                else
                {
                    size_t parent = m_stack.top().tree_node;
                    _nfo_logf("add DOC to parent={}", parent);
                    ASSERT_NE(parent, (size_t)NONE);
                    node = tree.append_child(parent);
                    _nfo_logf("child DOC={}", node);
                    tree.to_doc(node);
                    m_stack.push({node});
                }
                if(!rem.empty())
                {
                    if(rem.begins_with('<'))
                    {
GTEST_FAIL();
                        tree.set_val_tag(node, normalize_tag(rem));
                    }
                }
            }
            else if(line.begins_with("-DOC"))
            {
                _nfo_log("popping DOC");
                csubstr rem = line.stripl("+DOC").triml(' ');
                bool is_sep = rem.find("...") != csubstr::npos;
                size_t node = m_stack.empty() ? tree.root_id() : m_stack.pop().tree_node;
                if(is_sep && !tree.is_doc(node))
                {
                    _nfo_logf("got a sep '{}' and node[{}] is not a doc - adding DOC", rem, node);
                    tree._add_flags(node, DOC);
                }
            }
            else if(line.begins_with("+STR"))
            {
                ASSERT_EQ(m_stack.size(), 0u);
            }
            else if(line.begins_with("-STR"))
            {
                ASSERT_LE(m_stack.size(), 1u);
                if(!m_stack.empty())
                    m_stack.pop();
            }
            else if(line.empty())
            {
                // nothing to do
            }
            else
            {
                C4_ERROR("unknown event: '%.*s'", (int)line.len, line.str);
            }
            linenum++;
        }
    }
};



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

struct Events
{
    csubstr filename;
    std::string src;
    Tree    tree;
    mutable Tree adjusted_tree;
    bool    was_parsed = false;

    void init(csubstr filename_, csubstr src_)
    {
        filename = filename_;
        src.assign(src_.begin(), src_.end());
        tree.clear();
        tree.clear_arena();
        was_parsed = false;
    }

    void compare_trees(csubstr actual_src, Tree const& actual_tree) const
    {
        if(actual_src.empty())
            GTEST_SKIP();
        _nfo_logf("SRC:\n{}", actual_src);
        _nfo_print_tree("EXPECTED", tree);
        _nfo_print_tree("ACTUAL", actual_tree);
        test_compare(tree, actual_tree);
    }

    EventsParser parser;
    void parse_events(csubstr actual_src)
    {
        if(was_parsed)
            return;
        if(actual_src.empty())
            GTEST_SKIP();
        parser.parse(c4::to_csubstr(src), &tree);
        _nfo_print_tree("EXPECTED", tree);
        was_parsed = true;
    }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/** a processing level */
struct ProcLevel
{
    csubstr         filename;
    std::string     src;
    c4::yml::Parser parser;
    c4::yml::Tree   tree;
    std::string     emitted;

    AllowedFailure  allowed_failure;
    CasePart_e      case_part;
    bool            immutable = false;
    bool            reuse = false;
    bool            was_parsed = false;
    bool            was_emitted = false;

    void init(csubstr filename_, csubstr src_, bool immutable_, bool reuse_, CasePart_e case_part_, AllowedFailure af)
    {
        filename = filename_;
        src.assign(src_.begin(), src_.end());
        allowed_failure = af;
        case_part = case_part_;
        immutable = immutable_;
        reuse = reuse_;
        was_parsed = false;
        was_emitted = false;
    }

    void receive_src(ProcLevel & prev)
    {
        if(!prev.was_emitted)
            prev.emit();
        if(src != prev.emitted)
        {
            was_parsed = false;
            was_emitted = false;
            src = prev.emitted;
        }
    }

    template<class T>
    void log(const char* context, T const& v)
    {
        C4_UNUSED(context);
        C4_UNUSED(v);
        #if RYML_NFO
        constexpr const char sep[] = "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+\n";
        c4::log("{}:\n{}{}{}", context, sep, v, sep);
        #endif
    }

    void parse()
    {
        if(allowed_failure.skip(case_part))
            GTEST_SKIP();
        if(was_parsed)
            return;
        RYML_ASSERT(case_part != CPART_EVENTS);
        log("parsing source", src);
        if(reuse)
        {
            tree.clear();
            if(immutable)
                parser.parse(filename, c4::to_csubstr(src), &tree);
            else
                parser.parse(filename, c4::to_substr(src), &tree);
        }
        else
        {
            if(immutable)
                tree = c4::yml::parse(filename, c4::to_csubstr(src));
            else
                tree = c4::yml::parse(filename, c4::to_substr(src));
        }
        was_parsed = true;
        #if RYML_NFO
        c4::yml::print_tree(tree);
        #endif
        //_resolve_if_needed();
    }

    void _resolve_if_needed()
    {
        const NodeRef root = tree.rootref();
        bool has_anchors_or_refs = root.visit([](NodeRef const* node, size_t /*level*/){
            return (node->is_anchor() || node->is_ref());
        });
        if(has_anchors_or_refs)
        {
            tree.resolve();
            #if RYML_NFO
            c4::yml::print_tree(tree);
            #endif
        }
    }

    void emit()
    {
        if(allowed_failure.skip(case_part))
            GTEST_SKIP();
        if(was_emitted)
            return;
        if(!was_parsed)
            parse();
        emitrs(tree, &emitted);
        auto ss = c4::to_csubstr(emitted);
        if(ss.ends_with("\n...\n"))
            emitted.resize(emitted.size() - 4);
        log("emitted YAML", emitted);
        was_emitted = true;
        #if RYML_NFO
        c4::log("EMITTED:\n{}", emitted);
        #endif
    }

    void compare_trees(ProcLevel const& prev)
    {
        if(allowed_failure.skip(case_part))
            GTEST_SKIP();
        if(!was_parsed)
            parse();
        #if RYML_NFO
        c4::print("PREV:"); print_tree(prev.tree);
        c4::print("CURR:"); print_tree(tree);
        #endif
        test_compare(tree, prev.tree);
    }

    void compare_emitted(ProcLevel const& prev)
    {
        if(allowed_failure.skip(case_part))
            GTEST_SKIP();
        if(!was_emitted)
            emit();
        #if RYML_NFO
        c4::log("PREV:\n{}", prev.emitted);
        c4::log("CURR:\n{}", emitted);
        #endif
        EXPECT_EQ(emitted, prev.emitted);
    }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/** holds data for one particular test suite approach. */
struct Approach
{
    ProcLevel levels[NLEVELS];
    bool enabled = false;

    void init(csubstr filename, csubstr src, bool immutable_, bool reuse_, CasePart_e case_part, AllowedFailure af)
    {
        enabled = true;
        for(auto &l : levels)
        {
            l.init(filename, src, immutable_, reuse_, case_part, af);
            RYML_ASSERT(case_part != CPART_EVENTS);
        }
    }

    void parse(size_t num, bool emit)
    {
        for(size_t i = 0; i < num; ++i)
        {
            levels[i].parse();
            if(emit)
                levels[i].emit();
            if(i + 1 < num)
                levels[i+1].receive_src(levels[i]);
        }
    }

    void compare_trees(size_t num)
    {
        for(size_t i = 1; i < num; ++i)
            levels[i].compare_trees(levels[i-1]);
    }
    void compare_trees(size_t num, Approach const& other)
    {
        for(size_t i = 0; i < num; ++i)
            levels[i].compare_trees(other.levels[i]);
    }

    void compare_emitted(size_t num)
    {
        for(size_t i = 1; i < num; ++i)
            levels[i].compare_emitted(levels[i-1]);
    }
    void compare_emitted(size_t num, Approach const& other)
    {
        for(size_t i = 0; i < num; ++i)
            levels[i].compare_emitted(other.levels[i]);
    }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/** Each approach can be read from mutable/immutable yaml source and
 * with/without reuse. */
struct Subject
{
    Approach unix_ro;
    Approach unix_ro_reuse;
    Approach unix_rw;
    Approach unix_rw_reuse;

    Approach windows_ro;
    Approach windows_ro_reuse;
    Approach windows_rw;
    Approach windows_rw_reuse;

    std::string unix_src;
    std::string windows_src;

    void init(csubstr filename, csubstr src, CasePart_e case_part, AllowedFailure af)
    {
        src = replace_all("\r", "", src, &unix_src);

        unix_ro      .init(filename, src, /*immutable*/true , /*reuse*/false, case_part, af);
        unix_ro_reuse.init(filename, src, /*immutable*/true , /*reuse*/true , case_part, af);
        unix_rw      .init(filename, src, /*immutable*/false, /*reuse*/false, case_part, af);
        unix_rw_reuse.init(filename, src, /*immutable*/false, /*reuse*/true , case_part, af);

        src = replace_all("\n", "\r\n", src, &windows_src);

        windows_ro      .init(filename, src, /*immutable*/true , /*reuse*/false, case_part, af);
        windows_ro_reuse.init(filename, src, /*immutable*/true , /*reuse*/true , case_part, af);
        windows_rw      .init(filename, src, /*immutable*/false, /*reuse*/false, case_part, af);
        windows_rw_reuse.init(filename, src, /*immutable*/false, /*reuse*/true , case_part, af);
    }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// some utility functions, used below

size_t find_first_after(size_t pos, std::initializer_list<size_t> candidates)
{
    size_t ret = npos;
    for(size_t s : candidates)
        if(s > pos && s < ret)
            ret = s;
    return ret;
}


csubstr filter_out_indentation(csubstr src, std::string *dst)
{
    if( ! src.begins_with("    "))
    {
        dst->assign(src.begin(), src.end());
        return c4::to_csubstr(*dst);
    }
    return replace_all("\n    ", "\n", src.sub(4), dst);
}

csubstr filter_out_double_backslash(csubstr src, std::string *dst)
{
    return replace_all(R"(\\)", R"(\)", src, dst);
}


/** now finally all the ways that a test case can be processed are
 * available through this class. Tests are defined below and use only
 * one of these. */
struct SuiteCase
{
    csubstr     filename;
    std::string file_contents;

    csubstr desc;
    csubstr from;
    csubstr tags;

    Subject in_yaml;
    Subject in_json;
    Subject out_yaml;
    Subject emit_yaml;

    Events  events;

    static csubstr src(Subject const& s)
    {
        return c4::to_csubstr(s.unix_ro.levels[0].src);
    }

    /** loads the several types of tests from an input test suite
     * template file (tml)*/
    bool load(const char* filename_, AllowedFailure allowed_failure)
    {
        filename = c4::to_csubstr(filename_);

        // read the file
        c4::fs::file_get_contents(filename_, &file_contents);
        csubstr contents = c4::to_csubstr(file_contents);

        // now parse the file
        csubstr ws = " \t\r\n";
        csubstr txt;
        size_t b, e;

        // desc
        RYML_CHECK(contents.begins_with("=== "));
        e = contents.find("--- from: ", 4);
        RYML_CHECK(e != npos);
        desc = contents.range(4, e).trimr(ws);

        // from
        b = e + 4;
        e = contents.find("--- tags: ", b);
        RYML_CHECK(e != npos);
        from = contents.range(b, e);
        RYML_CHECK(from.begins_with("from: "));
        RYML_CHECK(from.size() >= 6);
        from = from.sub(6).trimr(ws);

        // tags
        b = e + 4;
        e = contents.find("--- in-yaml", b);
        RYML_CHECK(e != npos);
        tags = contents.range(b, e);
        RYML_CHECK(tags.begins_with("tags: "));
        RYML_CHECK(tags.size() >= 6);
        tags = tags.sub(6).trimr(ws);

        if(tags.find("error") != npos)
        {
            c4::log("{}: test case tagged with error: {}:\n{}\n", filename, tags, contents);
            return false; // tagged with error. skip this test.
        }
        bool has_whitespace = tags.find("whitespace");

        size_t end_tags        = e;
        size_t begin_in_yaml   = contents.find("--- in-yaml"   , end_tags);
        size_t begin_in_json   = contents.find("--- in-json"   , end_tags);
        size_t begin_out_yaml  = contents.find("--- out-yaml"  , end_tags);
        size_t begin_emit_yaml = contents.find("--- emit-yaml" , end_tags);
        size_t begin_events    = contents.find("--- test-event", end_tags);
        size_t lex_token       = contents.find("--- lex-token" , end_tags);
        std::initializer_list<size_t> all = {
            begin_in_yaml,
            begin_in_json,
            begin_out_yaml,
            begin_emit_yaml,
            begin_events,
            lex_token,
            contents.size()
        };

        // some of the examples have their code indented,
        // so we need these workspaces for deindenting
        std::string tmpa;
        std::string tmpb;

        // in_yaml
        RYML_CHECK(begin_in_yaml != npos);
        size_t first_after_in_yaml = find_first_after(begin_in_yaml, all);
        begin_in_yaml = 1 + contents.find('\n', begin_in_yaml); // skip this line
        txt = contents.range(begin_in_yaml, first_after_in_yaml);
        RYML_CHECK( ! txt.first_of_any("--- in-yaml", "--- in-json", "--- out-yaml", "--- emit-yaml", "---test-event"));
        txt = filter_out_indentation(txt, &tmpa);
        if(has_whitespace)
        {
            txt = replace_all("<SPC>", " ", txt, &tmpb);
            txt = replace_all("<TAB>", "\t", txt, &tmpa);
        }
        in_yaml.init(filename, txt, CPART_IN_YAML, allowed_failure);

        // in_json
        if(begin_in_json != npos)
        {
            size_t first_after = find_first_after(begin_in_json, all);
            begin_in_json = 1 + contents.find('\n', begin_in_json); // skip this line
            txt = contents.range(begin_in_json, first_after);
            RYML_CHECK( ! txt.first_of_any("--- in-yaml", "--- in-json", "--- out-yaml", "--- emit-yaml", "---test-event"));
            in_json.init(filename, txt, CPART_IN_JSON, allowed_failure);
        }

        // out_yaml
        if(begin_out_yaml != npos)
        {
            size_t first_after = find_first_after(begin_out_yaml, all);
            begin_out_yaml = 1 + contents.find('\n', begin_out_yaml); // skip this line
            txt = contents.range(begin_out_yaml, first_after);
            RYML_CHECK( ! txt.first_of_any("--- in-yaml", "--- in-json", "--- out-yaml", "--- emit-yaml", "---test-event"));
            txt = filter_out_indentation(txt, &tmpa);
            out_yaml.init(filename, txt, CPART_OUT_YAML, allowed_failure);
        }

        // emit_yaml
        if(begin_emit_yaml != npos)
        {
            size_t first_after = find_first_after(begin_emit_yaml, all);
            begin_emit_yaml = 1 + contents.find('\n', begin_emit_yaml); // skip this line
            txt = contents.range(begin_emit_yaml, first_after);
            RYML_CHECK( ! txt.first_of_any("--- in-yaml", "--- in-json", "--- out-yaml", "--- emit-yaml", "---test-event"));
            txt = filter_out_indentation(txt, &tmpa);
            emit_yaml.init(filename, txt, CPART_EMIT_YAML, allowed_failure);
        }

        // events
        {
            RYML_CHECK(begin_events != npos);
            size_t first_after = find_first_after(begin_events, all);
            begin_events = 1 + contents.find('\n', begin_events); // skip this line
            txt = contents.range(begin_events, first_after);
            txt = filter_out_double_backslash(txt, &tmpa);
            RYML_CHECK( ! txt.first_of_any("--- in-yaml", "--- in-json", "--- out-yaml", "--- emit-yaml", "---test-event"));
            events.init(filename, txt);
        }

        // lex-token
        {
            // don't really care
        }

        return true;
    }

    void parse_events(Approach *appr)
    {
        auto const& lev = appr->levels[0];
        csubstr levsrc = c4::to_csubstr(lev.src);
        if(levsrc.empty() || lev.allowed_failure.skip(lev.case_part) || lev.allowed_failure.skip(CPART_EVENTS))
            GTEST_SKIP();
        events.parse_events(levsrc);
    }

    void compare_events(Approach *appr)
    {
//GTEST_SKIP(); // this is a work in progress
        auto const& lev = appr->levels[0];
        csubstr levsrc = c4::to_csubstr(lev.src);
        if(levsrc.empty() || lev.allowed_failure.skip(lev.case_part) || lev.allowed_failure.skip(CPART_EVENTS))
            GTEST_SKIP();
        events.parse_events(levsrc);
        appr->parse(1, false);
        events.compare_trees(levsrc, lev.tree);
    }

    void print() const
    {
        c4::dump("~~~ file   : "   , filename      , "~~~\n",
                 "~~~ desc   : "   , desc          , "~~~\n",
                 "~~~ from   : "   , from          , "~~~\n",
                 "~~~ tags   : "   , tags          , "~~~\n",
                 "~~~ in-yaml:\n"  , src(in_yaml  ), "~~~\n",
                 "~~~ in-json:\n"  , src(in_json  ), "~~~\n",
                 "~~~ out-yaml:\n" , src(out_yaml ), "~~~\n",
                 "~~~ emit-yaml:\n", src(emit_yaml), "~~~\n",
                 "~~~ events :\n"  , events.src    , "~~~\n");
    }

};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


// a global holding the test case data
SuiteCase* g_suite_case = nullptr;



#define DEFINE_EVENT_TESTS(cls, pfx)                            \
                                                                \
                                                                \
struct cls##_##pfx##_##events : public ::testing::Test          \
{                                                               \
};                                                              \
                                                                \
TEST_F(cls##_##pfx##_##events, parse_events)                    \
{                                                               \
    RYML_CHECK(g_suite_case != nullptr);                        \
    g_suite_case->parse_events(&g_suite_case->cls.pfx);         \
}                                                               \
                                                                \
TEST_F(cls##_##pfx##_##events, compare_events)                  \
{                                                               \
    RYML_CHECK(g_suite_case != nullptr);                        \
    g_suite_case->compare_events(&g_suite_case->cls.pfx);       \
}



#define DECLARE_TEST_CLASS(cls, pfx)                            \
                                                                \
                                                                \
struct cls##_##pfx : public ::testing::TestWithParam<size_t>    \
{                                                               \
    Approach* get_test_case() const                             \
    {                                                           \
        RYML_CHECK(g_suite_case != nullptr);                    \
        RYML_CHECK(GetParam() < NLEVELS);                       \
        if(g_suite_case->cls.pfx.enabled)                       \
            return &g_suite_case->cls.pfx;                      \
        c4::dump(#cls "." #pfx ": no input for this case\n");   \
        return nullptr;                                         \
    }                                                           \
};                                                              \
                                                                \
                                                                \
TEST_P(cls##_##pfx, parse)                                      \
{                                                               \
    if(!get_test_case()) GTEST_SKIP();                          \
    get_test_case()->parse(1 + GetParam(), false);              \
}                                                               \
                                                                \
                                                                \
TEST_P(cls##_##pfx, compare_trees)                              \
{                                                               \
    if(!get_test_case()) GTEST_SKIP();                          \
    get_test_case()->compare_trees(1 + GetParam());             \
}                                                               \
                                                                \
                                                                \
TEST_P(cls##_##pfx, emit)                                       \
{                                                               \
    if(!get_test_case()) GTEST_SKIP();                          \
    get_test_case()->parse(1 + GetParam(), true);               \
}                                                               \
                                                                \
                                                                \
TEST_P(cls##_##pfx, compare_emitted)                            \
{                                                               \
    if(!get_test_case()) GTEST_SKIP();                          \
    get_test_case()->compare_emitted(1 + GetParam());           \
}                                                               \


#define DECLARE_TESTS(cls)                                              \
                                                                        \
DECLARE_TEST_CLASS(cls, unix_ro)                                        \
DECLARE_TEST_CLASS(cls, unix_rw)                                        \
DECLARE_TEST_CLASS(cls, unix_ro_reuse)                                  \
DECLARE_TEST_CLASS(cls, unix_rw_reuse)                                  \
DECLARE_TEST_CLASS(cls, windows_ro)                                     \
DECLARE_TEST_CLASS(cls, windows_rw)                                     \
DECLARE_TEST_CLASS(cls, windows_ro_reuse)                               \
DECLARE_TEST_CLASS(cls, windows_rw_reuse)                               \
                                                                        \
INSTANTIATE_TEST_SUITE_P(_, cls##_unix_ro         , testing::Range<size_t>(0, NLEVELS)); \
INSTANTIATE_TEST_SUITE_P(_, cls##_unix_rw         , testing::Range<size_t>(0, NLEVELS)); \
INSTANTIATE_TEST_SUITE_P(_, cls##_unix_ro_reuse   , testing::Range<size_t>(0, NLEVELS)); \
INSTANTIATE_TEST_SUITE_P(_, cls##_unix_rw_reuse   , testing::Range<size_t>(0, NLEVELS)); \
INSTANTIATE_TEST_SUITE_P(_, cls##_windows_ro      , testing::Range<size_t>(0, NLEVELS)); \
INSTANTIATE_TEST_SUITE_P(_, cls##_windows_rw      , testing::Range<size_t>(0, NLEVELS)); \
INSTANTIATE_TEST_SUITE_P(_, cls##_windows_ro_reuse, testing::Range<size_t>(0, NLEVELS)); \
INSTANTIATE_TEST_SUITE_P(_, cls##_windows_rw_reuse, testing::Range<size_t>(0, NLEVELS));


DECLARE_TESTS(out_yaml)
DECLARE_TESTS(emit_yaml)
DECLARE_TESTS(in_json)
DECLARE_TESTS(in_yaml) // this is the hardest one



DEFINE_EVENT_TESTS(out_yaml, unix_ro)
DEFINE_EVENT_TESTS(out_yaml, unix_rw)
DEFINE_EVENT_TESTS(out_yaml, unix_ro_reuse)
DEFINE_EVENT_TESTS(out_yaml, unix_rw_reuse)
DEFINE_EVENT_TESTS(out_yaml, windows_ro)
DEFINE_EVENT_TESTS(out_yaml, windows_rw)
DEFINE_EVENT_TESTS(out_yaml, windows_ro_reuse)
DEFINE_EVENT_TESTS(out_yaml, windows_rw_reuse)

DEFINE_EVENT_TESTS(emit_yaml, unix_ro)
DEFINE_EVENT_TESTS(emit_yaml, unix_rw)
DEFINE_EVENT_TESTS(emit_yaml, unix_ro_reuse)
DEFINE_EVENT_TESTS(emit_yaml, unix_rw_reuse)
DEFINE_EVENT_TESTS(emit_yaml, windows_ro)
DEFINE_EVENT_TESTS(emit_yaml, windows_rw)
DEFINE_EVENT_TESTS(emit_yaml, windows_ro_reuse)
DEFINE_EVENT_TESTS(emit_yaml, windows_rw_reuse)

DEFINE_EVENT_TESTS(in_yaml, unix_ro)
DEFINE_EVENT_TESTS(in_yaml, unix_rw)
DEFINE_EVENT_TESTS(in_yaml, unix_ro_reuse)
DEFINE_EVENT_TESTS(in_yaml, unix_rw_reuse)
DEFINE_EVENT_TESTS(in_yaml, windows_ro)
DEFINE_EVENT_TESTS(in_yaml, windows_rw)
DEFINE_EVENT_TESTS(in_yaml, windows_ro_reuse)
DEFINE_EVENT_TESTS(in_yaml, windows_rw_reuse)


//-------------------------------------------
// this is needed to use the test case library
Case const* get_case(csubstr /*name*/)
{
    return nullptr;
}

} // namespace yml
} // namespace c4




//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    using namespace c4::yml;

    // make gtest parse its args
    ::testing::InitGoogleTest(&argc, argv);

    // now we have only our args to consider
    if(argc != 2)
    {
        c4::log("usage:\n{} <test-suite-file>", argv[0]);
        return 1;
    }

    // load the test case from the suite file
    auto path = c4::to_substr(argv[1]);
    path.replace('\\', '/');
    RYML_CHECK(path.len > 0);
    RYML_CHECK(path[0] != '-');
    RYML_CHECK(c4::fs::path_exists(path.str));
    c4::log("testing suite case: {} ({})", path.basename(), path);

    AllowedFailure allowed_to_fail = is_failure_expected(path);
    if(allowed_to_fail.skip())
    {
        c4::log("\n{}: this case is deliberately not implemented in rapidyaml: {}\n",
                allowed_to_fail.test_code, allowed_to_fail.reason);
        return 0;
    }

    SuiteCase suite_case;
    if( ! suite_case.load(path.str, allowed_to_fail))
    {
        // if an error occurs during loading, the test intentionally crashes,
        // so the load() above never returns. This is NOT the same as
        // a return of false. That means the test was tagged as error,
        // ie, that an error is expected while parsing the code in it,
        // and for now we skip those, and return success.
        c4::log("\n{}: this case is tagged as error, skipping\n", path);
        return 0;
    }
    c4::print(suite_case.file_contents);
    suite_case.print();
    g_suite_case = &suite_case;

    // run all tests!
    int status = RUN_ALL_TESTS();

    // a terminating message
    if(suite_case.filename.not_empty())
    {
        c4::log("\n{}: TESTS {}: {}\n",
            suite_case.filename.basename(),
            status == 0 ? "SUCCEEDED" : "FAILED",
            suite_case.filename);
    }

    return status;
}

C4_SUPPRESS_WARNING_MSVC_PUSH
