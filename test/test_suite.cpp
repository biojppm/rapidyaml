#include <gtest/gtest.h>

#include <c4/yml/std/std.hpp>

#include <c4/fs/fs.hpp>
#include <c4/log/log.hpp>

#include <c4/yml/tree.hpp>
#include <c4/yml/parse.hpp>
#include <c4/yml/emit.hpp>
#include <c4/yml/detail/print.hpp>
#include <c4/yml/detail/checks.hpp>

#include "test_case.hpp"
#include "test_suite_parts.hpp"

#define RYML_NFO (1 || RYML_DBG)


C4_SUPPRESS_WARNING_MSVC_PUSH
C4_SUPPRESS_WARNING_MSVC(4702) // unreachable code

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

namespace c4 {
namespace yml {


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

csubstr filtered_scalar(csubstr orig, Tree *tree)
{
    csubstr str = orig;
    csubstr tokens[] = {"\\n", "\\t", "\\\n", "\\\t"};
    csubstr::first_of_any_result result = str.first_of_any_iter(std::begin(tokens), std::end(tokens));
    if(!result)
        return str;
    substr buf = tree->alloc_arena(str.len);
    size_t bufpos = 0;
    size_t strpos = 0;
    do
    {
        // copy up to result
        _nfo_logf("result.pos={}  strpos={}", result.pos, strpos);
        RYML_CHECK(bufpos + result.pos <= buf.len);
        RYML_CHECK(result.pos >= strpos);
        memcpy(buf.str + bufpos, str.str + strpos, result.pos - strpos);
        bufpos += result.pos - strpos;
        strpos += result.pos - strpos;
        // now transform or copy the token
        switch(result.which)
        {
        case 0u:
            RYML_ASSERT(str.sub(result.pos).begins_with("\\n"));
            memcpy(buf.str + bufpos, "\n", 1u);
            bufpos += 1u;
            strpos += 2u;
            break;
        case 1u:
            RYML_ASSERT(str.sub(result.pos).begins_with("\\t"));
            memcpy(buf.str + bufpos, "\t", 1u);
            bufpos += 1u;
            strpos += 2u;
            break;
        case 2u:
            RYML_ASSERT(str.sub(result.pos).begins_with("\\\n"));
            memcpy(buf.str + bufpos, "\\n", 2u);
            bufpos += 2u;
            strpos += 3u;
            break;
        case 3u:
            RYML_ASSERT(str.sub(result.pos).begins_with("\\\t"));
            memcpy(buf.str + bufpos, "\\t", 2u);
            bufpos += 2u;
            strpos += 3u;
            break;
        default:
            C4_ERROR("never reach this");
            break;
        }
        str = str.sub(strpos);
        strpos = 0u;
        result = str.first_of_any_iter(std::begin(tokens), std::end(tokens));
    } while(result);
    // and finally copy the remaining tail portion
    memcpy(buf.str + bufpos, str.str + strpos, str.len - strpos);
    bufpos += str.len - strpos;
    strpos += str.len - strpos;
    RYML_CHECK(bufpos <= buf.len);
    RYML_CHECK(strpos == str.len);
    _nfo_logf("filtered scalar '{}' ---> '{}'", orig, buf.first(bufpos));
    return buf.first(bufpos);
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
    csubstr filtered_scalar(Tree *tree) const
    {
        return ::c4::yml::filtered_scalar(scalar, tree);
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
                    ASSERT_TRUE(line.begins_with(':'));
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
                    ASSERT_FALSE(key);
                    ASSERT_TRUE(curr);
                    _nfo_logf("seq[{}]: adding child", top.tree_node);
                    size_t node = tree.append_child(top.tree_node);
                    _nfo_logf("seq[{}]: child={} val='{}'", top.tree_node, node, curr.scalar);
                    tree.to_val(node, curr.filtered_scalar(&tree));
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
                        tree.to_keyval(node, key.filtered_scalar(&tree), curr.filtered_scalar(&tree));
                        key.add_key_props(&tree, node);
                        curr.add_val_props(&tree, node);
                        _nfo_logf("clear key='{}'", key.scalar);
                        key = {};
                    }
                }
                else
                {
                    _nfo_logf("setting tree_node={} to DOCVAL...", top.tree_node);
                    tree.to_val(top.tree_node, curr.filtered_scalar(&tree), DOC);
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
                    ASSERT_FALSE(key);
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
                        tree.to_keyval(node, key.filtered_scalar(&tree), alias);
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
                    ASSERT_FALSE(key); // for the key to exist, the parent must exist and be a map
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
                        tree.to_seq(node, key.filtered_scalar(&tree), more_flags);
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
                    ASSERT_FALSE(key); // for the key to exist, the parent must exist and be a map
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
                        tree.to_map(node, key.filtered_scalar(&tree), more_flags);
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
                ASSERT_TRUE(tree.is_seq(node)) << "node=" << node;
            }
            else if(line.begins_with("-MAP"))
            {
                _nfo_logf("popping MAP, empty={}", m_stack.empty());
                size_t node;
                if(m_stack.empty())
                    node = tree.root_id();
                else
                    node = m_stack.pop().tree_node;
                ASSERT_TRUE(tree.is_map(node)) << "node=" << node;
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

    bool            immutable = false;
    bool            reuse = false;
    bool            was_parsed = false;
    bool            was_emitted = false;

    void init(csubstr filename_, csubstr src_, bool immutable_, bool reuse_)
    {
        filename = filename_;
        src.assign(src_.begin(), src_.end());
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
        if(was_parsed)
            return;
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
    ProcLevel levels[NLEVELS] = {};
    CasePart_e case_part = CPART_NONE;
    AllowedFailure allowed_failure = {};
    AllowedFailure allowed_failure_events = {};
    bool enabled = false;

    void init(csubstr filename, csubstr src_, bool immutable_, bool reuse_, CasePart_e case_part_)
    {
        RYML_ASSERT((case_part & CPART_EVENTS) == 0u);
        case_part = case_part_;
        allowed_failure = is_failure_expected(filename, case_part);
        allowed_failure_events = is_failure_expected(filename, events_part(case_part));
        enabled = !allowed_failure.skip(case_part);
        for(ProcLevel &l : levels)
            l.init(filename, src_, immutable_, reuse_);
    }

    csubstr src() const { return c4::to_csubstr(levels[0].src); }
    bool skip_part() const { return src().empty() || allowed_failure.skip(case_part); }
    bool skip_events() const
    {
        // use bit-or to ensure calling both, so that both report a skip
        RYML_ASSERT(events_part(CPART_IN_YAML) == CPART_IN_YAML_EVENTS);
        RYML_ASSERT(events_part(CPART_OUT_YAML) == CPART_OUT_YAML_EVENTS);
        RYML_ASSERT(events_part(CPART_EMIT_YAML) == CPART_EMIT_YAML_EVENTS);
        return skip_part() | allowed_failure_events.skip(events_part(case_part));
    }

    void parse(size_t num, bool emit)
    {
        if(skip_part())
            GTEST_SKIP();
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
        if(skip_part())
            GTEST_SKIP();
        for(size_t i = 1; i < num; ++i)
            levels[i].compare_trees(levels[i-1]);
    }
    void compare_trees(size_t num, Approach const& other)
    {
        if(skip_part())
            GTEST_SKIP();
        for(size_t i = 0; i < num; ++i)
            levels[i].compare_trees(other.levels[i]);
    }

    void compare_emitted(size_t num)
    {
        if(skip_part())
            GTEST_SKIP();
        for(size_t i = 1; i < num; ++i)
            levels[i].compare_emitted(levels[i-1]);
    }
    void compare_emitted(size_t num, Approach const& other)
    {
        if(skip_part())
            GTEST_SKIP();
        for(size_t i = 0; i < num; ++i)
            levels[i].compare_emitted(other.levels[i]);
    }

    void compare_events(Events *events)
    {
        if(skip_events())
            GTEST_SKIP();
        events->parse_events(src());
        parse(1, /*emit*/false);
        events->compare_trees(src(), levels[0].tree);
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

    void init(csubstr filename, csubstr src, CasePart_e case_part)
    {
        src = replace_all("\r", "", src, &unix_src);

        unix_ro      .init(filename, src, /*immutable*/true , /*reuse*/false, case_part);
        unix_ro_reuse.init(filename, src, /*immutable*/true , /*reuse*/true , case_part);
        unix_rw      .init(filename, src, /*immutable*/false, /*reuse*/false, case_part);
        unix_rw_reuse.init(filename, src, /*immutable*/false, /*reuse*/true , case_part);

        src = replace_all("\n", "\r\n", src, &windows_src);

        windows_ro      .init(filename, src, /*immutable*/true , /*reuse*/false, case_part);
        windows_ro_reuse.init(filename, src, /*immutable*/true , /*reuse*/true , case_part);
        windows_rw      .init(filename, src, /*immutable*/false, /*reuse*/false, case_part);
        windows_rw_reuse.init(filename, src, /*immutable*/false, /*reuse*/true , case_part);
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

csubstr filter_out_newlines(csubstr src, std::string *dst)
{
    return replace_all("\\n", "\n", src, dst);
}


/** all the ways that a test case can be processed are
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
    bool load(const char* filename_)
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
        in_yaml.init(filename, txt, CPART_IN_YAML);

        // in_json
        if(begin_in_json != npos)
        {
            size_t first_after = find_first_after(begin_in_json, all);
            begin_in_json = 1 + contents.find('\n', begin_in_json); // skip this line
            txt = contents.range(begin_in_json, first_after);
            RYML_CHECK( ! txt.first_of_any("--- in-yaml", "--- in-json", "--- out-yaml", "--- emit-yaml", "---test-event"));
            in_json.init(filename, txt, CPART_IN_JSON);
        }

        // out_yaml
        if(begin_out_yaml != npos)
        {
            size_t first_after = find_first_after(begin_out_yaml, all);
            begin_out_yaml = 1 + contents.find('\n', begin_out_yaml); // skip this line
            txt = contents.range(begin_out_yaml, first_after);
            RYML_CHECK( ! txt.first_of_any("--- in-yaml", "--- in-json", "--- out-yaml", "--- emit-yaml", "---test-event"));
            txt = filter_out_indentation(txt, &tmpa);
            if(has_whitespace)
            {
                txt = replace_all("<SPC>", " ", txt, &tmpb);
                txt = replace_all("<TAB>", "\t", txt, &tmpa);
            }
            out_yaml.init(filename, txt, CPART_OUT_YAML);
        }

        // emit_yaml
        if(begin_emit_yaml != npos)
        {
            size_t first_after = find_first_after(begin_emit_yaml, all);
            begin_emit_yaml = 1 + contents.find('\n', begin_emit_yaml); // skip this line
            txt = contents.range(begin_emit_yaml, first_after);
            RYML_CHECK( ! txt.first_of_any("--- in-yaml", "--- in-json", "--- out-yaml", "--- emit-yaml", "---test-event"));
            txt = filter_out_indentation(txt, &tmpa);
            emit_yaml.init(filename, txt, CPART_EMIT_YAML);
        }

        // events
        {
            RYML_CHECK(begin_events != npos);
            size_t first_after = find_first_after(begin_events, all);
            begin_events = 1 + contents.find('\n', begin_events); // skip this line
            txt = contents.range(begin_events, first_after);
            RYML_CHECK( ! txt.first_of_any("--- in-yaml", "--- in-json", "--- out-yaml", "--- emit-yaml", "---test-event"));
            txt = filter_out_double_backslash(txt, &tmpa);
            if(has_whitespace)
            {
                txt = replace_all("<SPC>", " ", txt, &tmpb);
                txt = replace_all("<TAB>", "\t", txt, &tmpa);
            }
            events.init(filename, txt);
        }

        // lex-token
        {
            // don't really care
        }

        return true;
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



#define DEFINE_EVENT_TESTS(cls, pfx)                                \
                                                                    \
TEST(cls##_##pfx##_events, compare)                                 \
{                                                                   \
    RYML_CHECK(g_suite_case != nullptr);                            \
    if(!g_suite_case->cls.pfx.enabled)                              \
        return;                                                     \
    g_suite_case->cls.pfx.compare_events(&g_suite_case->events);    \
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

    SuiteCase suite_case;
    if( ! suite_case.load(path.str))
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
