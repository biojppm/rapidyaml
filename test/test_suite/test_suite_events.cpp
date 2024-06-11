#include "test_suite_events.hpp"
#include "test_suite_event_handler.hpp"
#include "test_suite_common.hpp"
#ifndef RYML_SINGLE_HEADER
#include <c4/yml/detail/stack.hpp>
#endif

namespace c4 {
namespace yml {


std::string emit_events_from_source(substr src)
{
    EventHandlerYamlStd::EventSink sink;
    EventHandlerYamlStd handler(&sink);
    ParseEngine<EventHandlerYamlStd> parser(&handler);
    parser.parse_in_place_ev("(testyaml)", src);
    csubstr result = sink;
    return std::string(result.str, result.len);
}


namespace /*anon*/ {

csubstr filtered_scalar(csubstr str, Tree *tree)
{
    csubstr tokens[] = {R"(\n)", R"(\t)", R"(\\)"};
    if(!str.first_of_any_iter(std::begin(tokens), std::end(tokens)))
        return str;
    substr buf = tree->alloc_arena(str.len); // we are going to always replace with less characters
    size_t strpos = 0;
    size_t bufpos = 0;
    auto append_str = [&](size_t pos){
        csubstr rng = str.range(strpos, pos);
        memcpy(buf.str + bufpos, rng.str, rng.len);
        bufpos += rng.len;
        strpos = pos;
    };
    size_t i;
    auto append_chars = [&](csubstr s, size_t skipstr){
        memcpy(buf.str + bufpos, s.str, s.len);
        bufpos += s.len;
        i += skipstr - 1; // incremented at the loop
        strpos += skipstr;
    };
    for(i = 0; i < str.len; ++i)
    {
        char curr = str[i];
        char next1 = i+1 < str.len ? str[i+1] : '\0';
        if(curr == '\\')
        {
            if(next1 == '\\')
            {
                char next2 = i+2 < str.len ? str[i+2] : '\0';
                if(next2 == 'n')
                {
                    append_str(i);
                    append_chars(R"(\n)", 3u); // '\\n' -> '\n'
                }
                else if(next2 == 't')
                {
                    append_str(i);
                    append_chars(R"(\t)", 3u); // '\\t' -> '\t'
                }
                else
                {
                    append_str(i);
                    append_chars(R"(\)", 2u); // '\\' -> '\'
                }
            }
            else if(next1 == 'n')
            {
                append_str(i);
                append_chars("\n", 2u);
            }
            else if(next1 == 't')
            {
                append_str(i);
                append_chars("\t", 2u);
            }
            else if(next1 == 'b')
            {
                append_str(i);
                append_chars("\b", 2u);
            }
            else if(next1 == 'r')
            {
                append_str(i);
                append_chars("\r", 2u);
            }
        }
    }
    append_str(str.len);
    buf = buf.first(bufpos);
    _nfo_logf("filtering: result=~~~{}~~~", buf);
    return buf;
}

struct Scalar
{
    OptionalScalar scalar = {};
    OptionalScalar anchor = {};
    OptionalScalar ref    = {};
    OptionalScalar tag    = {};
    NodeType       flags = {};
    inline operator bool() const { if(anchor || tag) { RYML_ASSERT(scalar); } return scalar.was_set; }
    void add_key_props(Tree *tree, id_type node) const
    {
        if(ref)
        {
            _nfo_logf("node[{}]: set key ref: '{}'", node, ref.get());
            tree->set_key_ref(node, ref);
        }
        if(anchor)
        {
            _nfo_logf("node[{}]: set key anchor: '{}'", node, anchor.get());
            tree->set_key_anchor(node, anchor);
        }
        if(tag)
        {
            csubstr ntag = normalize_tag_long(tag);
            _nfo_logf("node[{}]: set key tag: '{}' -> '{}'", node, tag.get(), ntag);
            tree->set_key_tag(node, ntag);
        }
        if(flags != NOTYPE)
        {
            #ifdef RYML_DBG
            char buf1[128];
            char buf2[128];
            char buf3[128];
            #endif
            _nfo_logf("node[{}]: set key flags: {}: {}->{}", node, flags.type_str(buf1), flags.type_str(buf2), flags.type_str(buf3));
            tree->_add_flags(node, flags & KEY_STYLE);
        }
    }
    void add_val_props(Tree *tree, id_type node) const
    {
        if(ref)
        {
            _nfo_logf("node[{}]: set val ref: '{}'", node, ref.get());
            tree->set_val_ref(node, ref);
        }
        if(anchor)
        {
            _nfo_logf("node[{}]: set val anchor: '{}'", node, anchor.get());
            tree->set_val_anchor(node, anchor);
        }
        if(tag)
        {
            csubstr ntag = normalize_tag_long(tag);
            _nfo_logf("node[{}]: set val tag: '{}' -> '{}'", node, tag.get(), ntag);
            tree->set_val_tag(node, ntag);
        }
        if(flags != NOTYPE)
        {
            #ifdef RYML_DBG
            char buf1[128];
            char buf2[128];
            char buf3[128];
            #endif
            _nfo_logf("node[{}]: set val flags: {}: {}->{}", node, flags.type_str(buf1), flags.type_str(buf2), flags.type_str(buf3));
            tree->_add_flags(node, flags & VAL_STYLE);
        }
    }
    csubstr filtered_scalar(Tree *tree) const
    {
        return ::c4::yml::filtered_scalar(scalar, tree);
    }
};

} // namespace /*anon*/

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
        _nfo_logf("anchor: {}", anchor->get());
    }
    if(tokens.begins_with('<'))
    {
        size_t pos = tokens.find('>');
        RYML_ASSERT(pos != (size_t)csubstr::npos);
        *tag = tokens.first(pos + 1);
        tokens = tokens.right_of(pos).triml(' ');
        _nfo_logf("tag: {}", tag->maybe_get());
    }
    return tokens;
}

bool compare_events(csubstr ref_evts, csubstr emt_evts, bool ignore_container_style=false, bool ignore_scalar_style=false)
{
    auto diff_line_with_optional_ending = [](csubstr ref, csubstr emt, csubstr optional_ending){
        RYML_ASSERT(ref != emt);
        ref = ref.stripr(optional_ending).trimr(' ');
        emt = emt.stripr(optional_ending).trimr(' ');
        bool diff = ref != emt;
        return diff;
    };
    auto diff_val_with_scalar_wildcard = [](csubstr ref, csubstr emt){
        RYML_ASSERT(ref.begins_with("=VAL "));
        RYML_ASSERT(emt.begins_with("=VAL "));
        ref = ref.sub(5);
        emt = emt.sub(5);
        OptionalScalar reftag = {}, refanchor = {};
        OptionalScalar emttag = {}, emtanchor = {};
        if((bool(reftag) != bool(emttag)) || (reftag && (reftag.get() != emttag.get())))
            return true;
        if((bool(refanchor) != bool(emtanchor)) || (refanchor && (refanchor.get() != emtanchor.get())))
            return true;
        ref = parse_anchor_and_tag(ref, &refanchor, &reftag).triml(' ');
        emt = parse_anchor_and_tag(emt, &emtanchor, &emttag).triml(' ');
        RYML_ASSERT(ref.len > 0);
        RYML_ASSERT(emt.len > 0);
        RYML_ASSERT(ref[0] == ':' || ref[0] == '\'' || ref[0] == '"' || ref[0] == '|' || ref[0] == '>');
        RYML_ASSERT(emt[0] == ':' || emt[0] == '\'' || emt[0] == '"' || emt[0] == '|' || emt[0] == '>');
        ref = ref.sub(1);
        emt = emt.sub(1);
        if(ref != emt)
            return true;
        return false;
    };
    if(bool(ref_evts.len) != bool(emt_evts.len))
        return true;
    size_t posref = 0;
    size_t posemt = 0;
    bool fail = false;
    while(posref < ref_evts.len && posemt < emt_evts.len)
    {
        const size_t endref = ref_evts.find('\n', posref);
        const size_t endemt = emt_evts.find('\n', posemt);
        if((endref == npos || endemt == npos) && (endref != endemt))
        {
            fail = true;
            break;
        }
        csubstr ref = ref_evts.range(posref, endref);
        csubstr emt = emt_evts.range(posemt, endemt);
        if(ref != emt)
        {
            if(ref.begins_with("+DOC"))
            {
                if(diff_line_with_optional_ending(ref, emt, "---"))
                {
                    fail = true;
                    break;
                }
            }
            else if(ref.begins_with("-DOC"))
            {
                if(diff_line_with_optional_ending(ref, emt, "..."))
                {
                    fail = true;
                    break;
                }
            }
            else if(ignore_container_style && ref.begins_with("+MAP"))
            {
                if(diff_line_with_optional_ending(ref, emt, "{}"))
                {
                    fail = true;
                    break;
                }
            }
            else if(ignore_container_style && ref.begins_with("+SEQ"))
            {
                if(diff_line_with_optional_ending(ref, emt, "[]"))
                {
                    fail = true;
                    break;
                }
            }
            else if(ignore_scalar_style && ref.begins_with("=VAL"))
            {
                if(diff_val_with_scalar_wildcard(ref, emt))
                {
                    fail = true;
                    break;
                }
            }
            else
            {
                fail = true;
                break;
            }
        }
        posref = endref + 1u;
        posemt = endemt + 1u;
    }
    return fail;
}

void parse_events_to_tree(csubstr src, Tree *C4_RESTRICT tree_)
{
    struct ParseLevel { id_type tree_node; };
    detail::stack<ParseLevel> m_stack = {};
    Tree &C4_RESTRICT tree = *tree_;
    size_t linenum = 0; (void)linenum;
    Scalar key = {};
    _nfo_logf("parsing events! src:\n{}", src);
    for(csubstr line : src.split('\n'))
    {
        line = line.trimr('\r');
        line = line.triml(' ');
        _nfo_printf("\n\n-----------------------\n");
        _nfo_llog("");
        _nfo_logf("line[{}]: top={} type={}", linenum, m_stack.empty() ? tree.root_id() : m_stack.top().tree_node, NodeType::type_str(tree.type(m_stack.empty() ? tree.root_id() : m_stack.top().tree_node)));
        if(line.begins_with("=VAL "))
        {
            line = line.stripl("=VAL ");
            ASSERT_GE(m_stack.size(), 0u);
            Scalar curr = {};
            line = parse_anchor_and_tag(line, &curr.anchor, &curr.tag);
            if(line.begins_with('"'))
            {
                _nfo_llog("double-quoted scalar!");
                curr.scalar = line.sub(1);
                curr.flags = SCALAR_DQUO;
            }
            else if(line.begins_with('\''))
            {
                _nfo_llog("single-quoted scalar!");
                curr.scalar = line.sub(1);
                curr.flags = SCALAR_SQUO;
            }
            else if(line.begins_with('|'))
            {
                _nfo_llog("block literal scalar!");
                curr.scalar = line.sub(1);
                curr.flags = SCALAR_LITERAL;
            }
            else if(line.begins_with('>'))
            {
                _nfo_llog("block folded scalar!");
                curr.scalar = line.sub(1);
                curr.flags = SCALAR_FOLDED;
            }
            else
            {
                _nfo_llog("plain scalar");
                ASSERT_TRUE(line.begins_with(':'));
                curr.scalar = line.sub(1);
                curr.flags = SCALAR_PLAIN;
            }
            _nfo_logf("parsed scalar: '{}'", curr.scalar.maybe_get());
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
                id_type node = tree.append_child(top.tree_node);
                NodeType as_doc = tree.is_stream(top.tree_node) ? DOC : NOTYPE;
                _nfo_logf("seq[{}]: child={} val='{}' as_doc=", top.tree_node, node, curr.scalar.maybe_get(), as_doc.type_str());
                tree.to_val(node, curr.filtered_scalar(&tree), as_doc);
                curr.add_val_props(&tree, node);
            }
            else if(tree.is_map(top.tree_node))
            {
                _nfo_logf("is map! map_id={}", top.tree_node);
                if(!key)
                {
                    char buf[128];(void)buf;
                    _nfo_logf("store key='{}' anchor='{}' tag='{}' type={}", curr.scalar.maybe_get(), curr.anchor.maybe_get(), curr.tag.maybe_get(), curr.flags.type_str(buf));
                    key = curr;
                }
                else
                {
                    _nfo_logf("map[{}]: adding child", top.tree_node);
                    id_type node = tree.append_child(top.tree_node);
                    NodeType as_doc = tree.is_stream(top.tree_node) ? DOC : NOTYPE;
                    _nfo_logf("map[{}]: child={} key='{}' val='{}' as_doc={}", top.tree_node, node, key.scalar.maybe_get(), curr.scalar.maybe_get(), as_doc.type_str());
                    tree.to_keyval(node, key.filtered_scalar(&tree), curr.filtered_scalar(&tree), as_doc);
                    key.add_key_props(&tree, node);
                    curr.add_val_props(&tree, node);
                    _nfo_logf("clear key='{}'", key.scalar.maybe_get());
                    key = {};
                }
            }
            else
            {
                _nfo_logf("setting tree_node={} to DOCVAL...", top.tree_node);
                tree.to_val(top.tree_node, curr.filtered_scalar(&tree), tree.type(top.tree_node));
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
                id_type node = tree.append_child(top.tree_node);
                tree.to_val(node, alias);
                tree.set_val_ref(node, alias);
            }
            else if(tree.is_map(top.tree_node))
            {
                if(key)
                {
                    _nfo_logf("node[{}] is map and key '{}' is pending: set {} as val ref", top.tree_node, key.scalar.maybe_get(), alias);
                    id_type node = tree.append_child(top.tree_node);
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
            csubstr more_tokens = line.stripl("+SEQ").triml(' ');
            NodeType more_flags = NOTYPE;
            if(more_tokens.begins_with('['))
            {
                ASSERT_TRUE(more_tokens.begins_with("[]"));
                more_tokens = more_tokens.offs(2, 0).triml(' ');
                more_flags.add(FLOW_SL);
                _nfo_log("seq is flow");
            }
            else
            {
                more_flags.add(BLOCK);
                _nfo_log("seq is block");
            }
            parse_anchor_and_tag(more_tokens, &anchor, &tag);
            id_type node = tree.root_id();
            if(m_stack.empty())
            {
                _nfo_log("stack was empty, set root to SEQ");
                tree._add_flags(node, SEQ|more_flags);
                m_stack.push({node});
                ASSERT_FALSE(key); // for the key to exist, the parent must exist and be a map
            }
            else
            {
                id_type parent = m_stack.top().tree_node;
                _nfo_logf("stack was not empty. parent={}", parent);
                ASSERT_NE(parent, (id_type)NONE);
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
                    _nfo_logf("clear key='{}'", key.scalar.maybe_get());
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
            csubstr more_tokens = line.stripl("+MAP").triml(' ');
            NodeType more_flags = NOTYPE;
            if(more_tokens.begins_with('{'))
            {
                ASSERT_TRUE(more_tokens.begins_with("{}"));
                more_tokens = more_tokens.offs(2, 0).triml(' ');
                more_flags.add(FLOW_SL);
                _nfo_log("map is flow");
            }
            else
            {
                more_flags.add(BLOCK);
                _nfo_log("map is block");
            }
            parse_anchor_and_tag(more_tokens, &anchor, &tag);
            id_type node = tree.root_id();
            if(m_stack.empty())
            {
                _nfo_log("stack was empty, set root to MAP");
                tree._add_flags(node, MAP|more_flags);
                m_stack.push({node});
                ASSERT_FALSE(key); // for the key to exist, the parent must exist and be a map
            }
            else
            {
                id_type parent = m_stack.top().tree_node;
                _nfo_logf("stack was not empty. parent={}", parent);
                ASSERT_NE(parent, (id_type)NONE);
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
                    _nfo_logf("clear key='{}'", key.scalar.maybe_get());
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
            id_type node;
            if(m_stack.empty())
                node = tree.root_id();
            else
                node = m_stack.pop().tree_node;
            ASSERT_TRUE(tree.is_seq(node)) << "node=" << node;
        }
        else if(line.begins_with("-MAP"))
        {
            _nfo_logf("popping MAP, empty={}", m_stack.empty());
            id_type node;
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
            id_type node = tree.root_id();
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
                    if((!tree.is_container(node)) && (!tree.is_val(node)))
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
                    if(tree.is_container(node) || tree.is_val(node))
                    {
                        _nfo_log("rearrange root as STREAM");
                        tree.set_root_as_stream();
                        m_stack.push({node});
                    }
                }
            }
            else
            {
                id_type parent = m_stack.top().tree_node;
                _nfo_logf("add DOC to parent={}", parent);
                ASSERT_NE(parent, (id_type)NONE);
                node = tree.append_child(parent);
                _nfo_logf("child DOC={}", node);
                tree.to_doc(node);
                m_stack.push({node});
            }
        }
        else if(line.begins_with("-DOC"))
        {
            _nfo_log("popping DOC");
            if(!m_stack.empty())
                m_stack.pop();
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


} // namespace yml
} // namespace c4
