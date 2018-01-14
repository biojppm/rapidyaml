#ifndef _C4_RYML_HPP_
#include <ryml/ryml.hpp>
#endif

#include <ctype.h>

namespace c4 {
namespace yml {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

size_t Node::id() const
{
    return m_s->id(this);
}

Node * Node::prev_node() const
{
    return m_s->get(m_list.prev);
}

Node * Node::next_node() const
{
    return m_s->get(m_list.next);
}

Node * Node::parent() const
{
    return m_s->get(m_parent);
}

size_t Node::num_children() const
{
    if(is_val()) return 0;
    size_t count = 0;
    for(Node *n = m_s->get(m_children.first); n; n = n->next_sibling())
    {
        ++count;
    }
    return count;
}

Node * Node::child(size_t i) const
{
    if(is_val()) return nullptr;
    size_t count = 0;
    for(Node *n = m_s->get(m_children.first); n; n = n->next_sibling(), ++count)
    {
        if(count == i) return n;
    }
    C4_ASSERT(i < count);
    return nullptr;
}
Node * Node::first_child() const
{
    if(is_val()) return nullptr;
    return m_s->get(m_children.first);
}
Node * Node::last_child() const
{
    if(is_val()) return nullptr;
    return m_s->get(m_children.last);
}

Node * Node::find_child(cspan const& name) const
{
    if(is_val()) return nullptr;
    C4_ASSERT(m_type == TYPE_DOC || m_type == TYPE_MAP);
    C4_ASSERT(bool(name) == true);
    if(m_children.first == NONE)
    {
        C4_ASSERT(m_children.last == NONE);
        return nullptr;
    }
    else
    {
        C4_ASSERT(m_children.last != NONE);
    }
    for(Node *n = m_s->get(m_children.first); n; n = n->next_sibling())
    {
        if(n->m_name == name)
        {
            return n;
        }
    }
    return nullptr;
}

bool Node::is_child(Node const* ch) const
{
    for(Node const* n = first_child(); n; n = n->next_sibling())
    {
        if(n == ch) return true;
    }
    return false;
}

size_t Node::num_siblings() const
{
    return (m_parent != NONE) ? m_s->get(m_parent)->num_children() : 0;
}

Node * Node::sibling(size_t i) const
{
    C4_ASSERT(parent() != nullptr);
    return m_s->get(m_parent)->child(i);
}

Node * Node::first_sibling() const
{
    return m_s->get(m_parent)->first_child();
}

Node * Node::last_sibling() const
{
    return m_s->get(m_parent)->last_child();
}

Node * Node::find_sibling(cspan const& name) const
{
    return m_s->get(m_parent)->find_child(name);
}

Node * Node::prev_sibling() const
{
    if(m_siblings.prev == NONE) return nullptr;
    Node *n = m_s->get(m_siblings.prev);
    C4_ASSERT(!n || n->m_parent == m_parent);
    return n;
}

Node * Node::next_sibling() const
{
    if(m_siblings.next == NONE) return nullptr;
    Node *n = m_s->get(m_siblings.next);
    C4_ASSERT(!n || n->m_parent == m_parent);
    return n;
}

bool Node::is_sibling(Node const* s) const
{
    for(Node *n = first_sibling(); n; n = n->next_sibling())
    {
        if(n == s) return true;
    }
    return false;
}

Node * Node::insert_sibling(cspan const& name, cspan const& val, Node * after)
{
    C4_ASSERT( ! after || (is_sibling(after) && after->is_sibling(this)));
    C4_ASSERT(m_s->get(m_parent)->is_container());
    m_s->_stack_push(m_s->get(m_parent));
    Node *n = m_s->add_val(name, val, after);
    m_s->_stack_pop();
    return n;
}

Node * Node::insert_sibling(cspan const& name, NodeType_e sibtype, Node * after)
{
    C4_ASSERT( ! after || (is_sibling(after) && after->is_sibling(this)));
    C4_ASSERT(m_s->get(m_parent)->is_container());
    Node *n = _insert_by_type(m_s->get(m_parent), name, sibtype, after);
    return n;
}

Node * Node::insert_sibling(Node *sib, Node * after)
{
    C4_ASSERT(sib && ( ! is_sibling(sib) && ! sib->is_sibling(this)));
    C4_ASSERT( ! after || (is_sibling(after) && after->is_sibling(this)));
    C4_ASSERT( ! sib->name().empty() == is_map());
    C4_ASSERT(   sib->name().empty() == is_seq());
    C4_ASSERT( ! sib->val().empty() == (sib->m_type == TYPE_VAL));
    C4_ASSERT(   sib->val().empty() == (sib->is_container()));
    m_s->set_parent(m_s->get(m_parent), sib, after);
    return sib;
}


Node * Node::insert_child(cspan const& name, cspan const& val, Node * after)
{
    C4_ASSERT( ! after || is_child(after));
    C4_ASSERT(is_container());
    m_s->_stack_push(this);
    m_s->add_val(name, val, after);
    m_s->_stack_pop();
    return nullptr;
}

Node * Node::insert_child(cspan const& name, NodeType_e chtype, Node * after)
{
    C4_ASSERT( ! after || is_child(after));
    Node *n = _insert_by_type(this, name, chtype, after);
    return n;
}

Node * Node::insert_child(Node *ch, Node * after)
{
    C4_ASSERT( ! is_child(ch));
    C4_ASSERT( ! after || is_child(after));
    m_s->set_parent(this, ch, after);
    return ch;
}


Node * Node::_insert_by_type(Node *which_parent, cspan const& name, NodeType_e type, Node *after)
{
    m_s->_stack_push(which_parent);
    Node *n = nullptr;
    switch(type)
    {
    case TYPE_VAL:
        n = m_s->add_val(name, {}, after);
        break;
    case TYPE_SEQ:
        n = m_s->begin_seq(name, after);
        m_s->end_seq();
        break;
    case TYPE_MAP:
        n = m_s->begin_map(name, after);
        m_s->end_map();
        break;
    case TYPE_DOC:
        n = m_s->begin_doc(after);
        m_s->end_doc();
        break;
    case TYPE_NONE:
        n = m_s->add_empty(name, after);
        //C4_ERROR("cannot add sibling with type NONE");
        break;
    case TYPE_ROOT:
        C4_ERROR("cannot add sibling with type ROOT");
        break;
    default:
        C4_ERROR("unknown node type to add as sibling");
        break;
    }
    m_s->_stack_pop();
    return n;
}

Node * Node::remove_sibling(cspan const& name)
{
    C4_ASSERT(find_sibling(name));
    Node *n = find_sibling(name);
    return remove_sibling(n);
}

Node * Node::remove_sibling(size_t i)
{
    C4_ASSERT(sibling(i));
    Node *n = sibling(i);
    return remove_sibling(n);
}

Node * Node::remove_sibling(Node *sib)
{
    C4_ASSERT(sib && ( ! is_sibling(sib) && ! sib->is_sibling(this)));
    C4_ERROR("not implemented");
    return sib;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

Tree::Tree()
    :
    m_buf(nullptr),
    m_num(0),
    m_head(NONE),
    m_tail(NONE),
    m_free_head(NONE),
    m_free_tail(NONE),
    m_load_root_id(NONE)
{
}

Tree::Tree(size_t sz) : Tree()
{
    reserve(sz);
}

Tree::~Tree()
{
    if(m_buf)
    {
        RymlCallbacks::free(m_buf, m_num * sizeof(Node));
    }
}

//-----------------------------------------------------------------------------
void Tree::serialize(Node * root, span const* buffer) const
{
    C4_ASSERT(false && "not implemented");
}

Node * Tree::load(Node * root, cspan const& yml_str, Parser *p_)
{
    Parser p, *ptr;
    ptr = p_ ? p_ : &p;

    C4_ASSERT(root->is_map());

    size_t idr = root->id();

    set_load_root(root);
    ptr->set_load_root(root);
    ptr->parse(this, yml_str);
    ptr->set_load_root(nullptr);
    set_load_root(nullptr);

    return get(idr);
}

//-----------------------------------------------------------------------------
void Tree::reserve(size_t sz)
{
    if(sz <= m_num) return;
    Node *buf = (Node*)RymlCallbacks::allocate(sz * sizeof(Node), nullptr);
    if(m_buf)
    {
        memcpy(buf, m_buf, m_num * sizeof(Node));
        RymlCallbacks::free(m_buf, m_num * sizeof(Node));
    }
    if(m_free_head == NONE)
    {
        C4_ASSERT(m_free_tail == m_free_head);
        m_free_head = m_num;
        m_free_tail = sz;
    }
    else
    {
        C4_ASSERT(m_free_tail != NONE);
        m_buf[m_free_tail].m_list.next = m_num;
    }
    size_t first = m_num, del = sz - m_num;
    m_num = sz;
    m_buf = buf;
    clear_range(first, del);
}

//-----------------------------------------------------------------------------
void Tree::clear()
{
    clear_range(0, m_num);
    m_head = NONE;
    m_tail = NONE;
    m_free_head = 0;
    m_free_tail = m_num;
}

//-----------------------------------------------------------------------------
void Tree::clear_range(size_t first, size_t num)
{
    if(num == 0) return; // prevent overflow when subtracting
    C4_ASSERT(first >= 0 && first + num <= m_num);
    memset(m_buf + first, 0, num * sizeof(Node));
    for(size_t i = first, e = first + num; i < e; ++i)
    {
        Node *n = m_buf + i;
        n->m_list.prev = i - 1;
        n->m_list.next = i + 1;
        n->m_parent = NONE;
        n->m_children.first = NONE;
        n->m_children.last = NONE;
        n->m_siblings.prev = NONE;
        n->m_siblings.next = NONE;
    }
    m_buf[first + num - 1].m_list.next = NONE;
}

//-----------------------------------------------------------------------------
Node *Tree::claim(Node *after)
{
    C4_ASSERT(after == nullptr || (m_buf <= after && (after <= m_buf + m_num)));
    size_t last = after ? after - m_buf : m_tail;
    Node *n = claim(last, NONE);
    return n;
}

//-----------------------------------------------------------------------------
Node *Tree::claim(size_t prev, size_t next)
{
    C4_ASSERT(prev == NONE || prev >= 0 && prev < m_num);
    C4_ASSERT(next == NONE || next >= 0 && next < m_num);
    if(m_free_head == NONE || m_buf == nullptr)
    {
        size_t sz = 2 * m_num;
        sz = sz ? sz : 16;
        reserve(sz);
    }
    size_t f = m_free_head;
    Node *n = m_buf + f;
    m_free_head = n->m_list.next;
    if(m_free_head == NONE)
    {
        m_free_tail = NONE;
    }
    n->m_s = this;
    n->m_list.prev = prev;
    n->m_list.next = next;
    n->m_siblings.prev = NONE;
    n->m_siblings.next = NONE;
    n->m_children.first = NONE;
    n->m_children.last = NONE;
    if(prev == NONE) m_head = f;
    else
    {
        Node *p = m_buf + prev;
        p->m_list.next = f;
    }
    if(next == NONE) m_tail = f;
    else
    {
        Node *v = m_buf + next;
        v->m_list.prev = f;
    }
    return n;
}

//-----------------------------------------------------------------------------
void Tree::set_parent(Node *parent, Node *child, Node *prev_sibling, Node *next_sibling)
{
    C4_ASSERT(child != nullptr && child >= m_buf && child < m_buf + m_num);
    C4_ASSERT(parent == nullptr || parent >= m_buf && parent < m_buf + m_num);

    child->m_parent = parent ? parent - m_buf : NONE;

    if( ! prev_sibling)
    {
        next_sibling = parent->first_child();
    }

    if( ! next_sibling)
    {
        if(prev_sibling)
        {
            next_sibling = prev_sibling->next_sibling();
        }
    }

    child->m_siblings.prev = NONE;
    child->m_siblings.next = NONE;
    if(prev_sibling)
    {
        C4_ASSERT(prev_sibling->next_sibling() == next_sibling);
        child->m_siblings.prev = prev_sibling->id();
        prev_sibling->m_siblings.next = child->id();
    }
    if(next_sibling)
    {
        C4_ASSERT(next_sibling->prev_sibling() == prev_sibling);
        child->m_siblings.next = next_sibling->id();
        next_sibling->m_siblings.prev = child->id();
    }

    if( ! parent) return;
    if(parent->m_children.first == NONE)
    {
        C4_ASSERT(parent->m_children.last == NONE);
        parent->m_children.first = child->id();
        parent->m_children.last = child->id();
    }
    else
    {
        if(child->m_siblings.next == parent->m_children.first)
        {
            parent->m_children.first = child->id();
        }
        if(child->m_siblings.prev == parent->m_children.last)
        {
            parent->m_children.last = child->id();
        }
    }
}

//-----------------------------------------------------------------------------
void Tree::free(size_t i)
{
    C4_ASSERT(i >= 0 && i < m_num);
    m_buf[i].m_list.next = m_free_head;
    m_buf[i].m_list.prev = NONE;
    m_buf[m_free_head].m_list.prev = i;
    m_free_head = i;
    if(m_free_tail == NONE)
    {
        m_free_tail = m_free_head;
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

/** a debug utility */
#define _prhl(fmt, ...)                                                 \
    {                                                                   \
        auto _llen = printf("%s:%d: line %zd:%zd(%zd): ",               \
                          __FILE__, __LINE__,                           \
                          m_state.pos.line-1,                           \
                          m_state.line_contents.rem.begin() - m_state.line_contents.stripped.begin(), \
                          m_state.line_contents.rem.len);               \
        if(m_state.line_contents.rem)                                   \
        {                                                               \
            printf("'%.*s'\n", _c4prsp(m_state.line_contents.rem));     \
        }                                                               \
        if(strlen(fmt) > 0)                                             \
        {                                                               \
            printf("%*s", _llen, " ");                                  \
            printf(fmt "\n", ## __VA_ARGS__);                           \
        }                                                               \
    }

#pragma GCC diagnostic pop
#pragma clang diagnostic pop

//-----------------------------------------------------------------------------
void NextParser::_reset()
{
    m_state.reset(m_file.str, m_root);
}

//-----------------------------------------------------------------------------
bool NextParser::_finished_file() const
{
    bool ret = m_state.pos.offset >= m_buf.len;
    if(ret)
    {
        printf("finished file!!!\n");
    }
    return ret;
}

//-----------------------------------------------------------------------------
void NextParser::parse(const char *file, cspan const& buf, Node *root)
{
    m_file = file ? file : "(no file)";
    m_buf = buf;
    m_root = root;
    m_tree = root->tree();
    _reset();

    while( ! _finished_file())
    {
        _scan_line();

        while(m_state.line_contents.rem)
        {
            _handle_line(m_state.line_contents.rem);
        }

        _next_line();
    }

}

//-----------------------------------------------------------------------------
void NextParser::_handle_line(cspan rem)
{
    printf("-----------\n");
    _prhl("");

    C4_ASSERT( ! rem.empty());

    if(int jump = _get_indentation_jump())
    {
        _prhl("indentation jump: %d", jump);
        if(jump < 0)
        {
            _handle_indentation();
        }
    }

    if(_handle_scalar(rem))
    {
        return;
    }

    if(m_state.has_any(RSEQ))
    {
        _prhl("handle_seq");
        if(_handle_seq(rem))
        {
            return;
        }
    }
    else if(m_state.has_any(RMAP))
    {
        _prhl("handle_map");
        if(_handle_map(rem))
        {
            return;
        }
    }
    else if(m_state.flags & RUNK)
    {
        _prhl("handle_unk");
        if(_handle_unk(rem))
        {
            return;
        }
    }

    if(_handle_top(rem))
    {
        return;
    }
}

//-----------------------------------------------------------------------------
bool NextParser::_handle_scalar(cspan rem)
{
    if(m_state.has_none(RKEY|RVAL|RUNK|RTOP)) return false;

    const bool alnum = isalnum(rem[0]);
    const bool dquot = rem.begins_with('"');
    const bool squot = rem.begins_with('\'');
    const bool block = rem.begins_with('|') || rem.begins_with('>');

    if(alnum || dquot || squot || block)
    {
        _prhl("it's a scalar");
        cspan s = _scan_scalar();
        C4_ASSERT( ! s.empty());

        if(m_state.has_all(RSEQ))
        {
            _append_val(s);
        }
        else if(m_state.has_all(RKEY|RMAP))
        {
            C4_ASSERT(m_state.has_none(RVAL));
            _store_scalar(s);
            _toggle_key_val();
        }
        else if(m_state.has_all(RVAL|RMAP))
        {
            C4_ASSERT(m_state.has_none(RKEY));
            _append_key_val(s); // toggles key val
        }
        else if(m_state.has_all(RUNK))
        {
            C4_ASSERT(m_state.has_none(RSEQ|RMAP));
            // we still don't know whether it's a seq or a map
            // so just store the scalar
            _store_scalar(s);
        }
        else
        {
            C4_ERROR("wtf?");
        }
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
bool NextParser::_handle_unk(cspan rem)
{
    const bool start_as_child = (m_state.level != 0) || m_state.node == nullptr;
    if(rem.begins_with(' '))
    {
        _prhl("indentation jump=%d level=%zd #spaces=%zd", _get_indentation_jump(), m_state.level, m_state.line_contents.indentation);
        C4_ASSERT(_get_indentation_jump() > 0);
        m_state.line_progressed(m_state.line_contents.indentation);
        return true;
    }
    else if(rem.begins_with("- "))
    {
        _prhl("it's a seq (as_child=%d)", start_as_child);
        _start_seq(start_as_child);
        m_state.line_progressed(2);
        return true;
    }
    else if(rem.begins_with('['))
    {
        _prhl("it's a seq (as_child=%d)", start_as_child);
        _start_seq(start_as_child);
        m_state.flags |= EXPL;
        m_state.line_progressed(1);
        return true;
    }

    else if(rem.begins_with('{'))
    {
        _prhl("it's a map (as_child=%d)", start_as_child);
        _start_map(start_as_child);
        m_state.flags |= EXPL;
        m_state.line_progressed(1);
        return true;
    }
    else if(rem.begins_with("? "))
    {
        _prhl("it's a map (as_child=%d)", start_as_child);
        _start_map(start_as_child);
        m_state.line_progressed(2);
        return true;
    }

    else if(m_state.has_all(SSCL))
    {
        _prhl("there's a stored scalar\n");
        if(rem.begins_with(", "))
        {
            _prhl("it's a seq (as_child=%d)", start_as_child);
            _start_seq(start_as_child);
            _append_val(_consume_scalar());
            m_state.line_progressed(2);
        }
        else if(rem.begins_with(": "))
        {
            _prhl("it's a map (as_child=%d)", start_as_child);
            _start_map(start_as_child);
            // wait for the val scalar to append the key-val pair
            m_state.line_progressed(2);
        }
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
bool NextParser::_handle_seq(cspan rem)
{
    if(m_state.has_any(RVAL))
    {
        if(rem.begins_with("- "))
        {
            _prhl("expect another val");
            m_state.line_progressed(2);
            return true;
        }
        else if(rem == '-')
        {
            _prhl("start unknown");
            _push_level();
            m_state.line_progressed(1);
            return true;
        }
        else if(rem.begins_with(", "))
        {
            _prhl("expect another val");
            m_state.line_progressed(2);
            return true;
        }
        else if(rem.begins_with(']'))
        {
            _prhl("end the sequence");
            m_state.line_progressed(1);
            _pop_level();
            return true;
        }
        else if(_handle_anchors_and_refs(rem))
        {
            C4_ERROR("not implemented");
            return true;
        }
        else if(_handle_types(rem))
        {
            C4_ERROR("not implemented");
            return true;
        }
        else
        {
            C4_ERROR("internal error");
        }
    }
    else
    {
        C4_ERROR("internal error");
    }
    return false;
}

//-----------------------------------------------------------------------------
bool NextParser::_handle_map(cspan rem)
{
    C4_ASSERT(m_state.has_all(RMAP));
    if(m_state.has_any(RVAL))
    {
        if(rem.begins_with(": "))
        {
            _prhl("wait for val");
            m_state.line_progressed(2);
            return true;
        }
        else if(rem == ':')
        {
            _prhl("start unknown");
            _push_level();
            m_state.line_progressed(1);
            return true;
        }
        else if(rem.begins_with("- "))
        {
            _prhl("start a sequence\n");
            _push_level();
            _start_seq();
            m_state.line_progressed(2);
            return true;
        }
        else if(rem.begins_with('['))
        {
            _prhl("start a sequence\n");
            _push_level(/*explicit flow*/true);
            _start_seq();
            m_state.line_progressed(1);
            return true;
        }
        else if(rem.begins_with('{'))
        {
            _prhl("start a map");
            _push_level(/*explicit flow*/true);
            _start_map();
            m_state.line_progressed(1);
            return true;
        }
        else if(rem.begins_with(", "))
        {
            _prhl("expect another key-val");
            m_state.line_progressed(2);
            return true;
        }
        else if(rem.begins_with('}'))
        {
            _prhl("end the map");
            m_state.line_progressed(1);
            _pop_level();
            return true;
        }
        else if(_handle_anchors_and_refs(rem))
        {
            C4_ERROR("not implemented");
            return true;
        }
        else if(_handle_types(rem))
        {
            C4_ERROR("not implemented");
            return true;
        }
        else
        {
            C4_ERROR("parse error");
        }
    }
    else if(m_state.has_any(RKEY))
    {
        C4_ERROR("never reach this");
    }
    else
    {
        C4_ERROR("wtf?");
    }
    return false;
}

//-----------------------------------------------------------------------------
bool NextParser::_handle_top(cspan rem)
{
    if(rem.begins_with('#'))
    {
        _prhl("a comment line");
        m_state.line_progressed(rem.len);
        return true;
    }
    // the following tokens can appear only at top level
    else if(m_state.flags & RTOP)
    {
        if(rem.begins_with('%'))
        {
            _prhl("%% directive!");
            C4_ERROR("not implemented");
            if(rem.begins_with("%YAML"))
            {
            }
            else if(rem.begins_with("%TAG"))
            {
            }
            else
            {
                C4_ERROR("unknown directive starting with %");
            }
            return true;
        }
        else if((m_state.flags & RTOP) && rem.begins_with("---"))
        {
            C4_ASSERT(m_state.level == 0);
            // end/start a document
            C4_ERROR("not implemented");
            _prhl("end/start a document\n");
            return true;
        }
        else if((m_state.flags & RTOP) && rem.begins_with("..."))
        {
            // end a document
            _prhl("end a document\n");
            C4_ERROR("not implemented");
            return true;
        }
        else
        {
            C4_ERROR("parse error");
        }
    }
    else
    {
        C4_ERROR("internal error");
    }

    return false;
}

//-----------------------------------------------------------------------------
bool NextParser::_handle_anchors_and_refs(cspan rem)
{
    if(rem.begins_with('&'))
    {
        C4_ERROR("not implemented");
        return true;
    }
    else if(rem.begins_with('*'))
    {
        C4_ERROR("not implemented");
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
bool NextParser::_handle_types(cspan rem)
{
    if(rem.begins_with("!!"))
    {
        C4_ERROR("not implemented");
        return true;
    }
    else if(rem.begins_with('!'))
    {
        C4_ERROR("not implemented");
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
cspan NextParser::_scan_scalar()
{
    cspan s = m_state.line_contents.rem;
    if(s.len == 0) return s;

    if(s.begins_with('\''))
    {
        s = _scan_quoted_scalar(/*single*/true);
        return s;
    }
    else if(s.begins_with('"'))
    {
        s = _scan_quoted_scalar(/*single*/false);
        return s;
    }
    else if(s.begins_with('|') || s.begins_with('>'))
    {
        s = _scan_block();
        return s;
    }
    else if(m_state.has_any(RSEQ))
    {
        if(m_state.has_all(RKEY))
        {
            C4_ERROR("internal error");
        }
        else if(m_state.has_all(RVAL))
        {
            s = s.left_of(s.first_of(",]"));
        }
        else
        {
            C4_ERROR("parse error");
        }
    }
    else if(m_state.has_any(RMAP))
    {
        size_t colon_space = s.find(": ");
        if(colon_space == npos)
        {
            colon_space = s.find(":");
            C4_ASSERT(s.len > 0);
            if(colon_space != s.len-1)
            {
                colon_space = npos;
            }
        }

        if(m_state.has_all(RKEY))
        {
            if(m_state.has_any(CPLX))
            {
                C4_ASSERT(m_state.has_any(RMAP));
                s = s.left_of(colon_space);
            }
            else
            {
                s = s.triml(' ');
                s = s.left_of(colon_space);
                s = s.trimr(' ');
            }
        }
        else if(m_state.has_all(RVAL))
        {
            s = s.trim(' ');
        }
        else
        {
            C4_ERROR("parse error");
        }
    }
    else if(m_state.has_all(RUNK))
    {
        s = s.left_of(s.first_of(",: "));
    }
    else
    {
        C4_ERROR("not implemented");
    }

    _prhl("scalar was '%.*s'", _c4prsp(s));

    m_state.line_progressed(s.len);

    return s;
}

//-----------------------------------------------------------------------------
void NextParser::_scan_line()
{
    if(m_state.pos.offset >= m_buf.len) return;
    char const* b = &m_buf[m_state.pos.offset];
    char const* e = b;
    while(*e != '\r' && *e != '\n')
    {
        ++e;
    }
    size_t len = e - b;
    cspan stripped = m_buf.subspan(m_state.pos.offset, len);
    // advance pos, including line ending chars
    while(*e == '\r' || *e == '\n')
    {
        ++e;
        ++len;
    }
    cspan full = m_buf.subspan(m_state.pos.offset, len);
    m_state.line_scanned(full, stripped);
}

//-----------------------------------------------------------------------------

void NextParser::_push_level(bool explicit_flow_chars)
{
    if(m_state.node == nullptr)
    {
        C4_ASSERT( ! explicit_flow_chars);
        return;
    }
    printf("level pushed!\n");
    size_t st = RUNK;
    if(explicit_flow_chars)
    {
        st |= EXPL;
    }
    printf("stacking node %zd\n", m_state.node->id());
    m_stack.push(m_state);
    m_state.flags = st;
    m_state.node = nullptr;
    ++m_state.level;
}

void NextParser::_pop_level()
{
    printf("level popped!\n");
    if(m_state.has_any(RMAP))
    {
        _stop_map();
    }
    else if(m_state.has_any(RSEQ))
    {
        _stop_seq();
    }
    else
    {
        C4_ERROR("internal error");
    }
    printf("popping node %zd top %zd\n", m_state.node->id(), m_stack.peek().node->id());
    C4_ASSERT( ! m_stack.empty());
    m_stack.peek()._prepare_pop(m_state);
    m_state = m_stack.pop();
    if(m_state.has_any(RMAP))
    {
        _toggle_key_val();
    }
    if(m_state.line_contents.indentation == 0)
    {
        C4_ASSERT(m_state.has_none(RTOP));
        m_state.flags |= RTOP;
    }

    //emit(m_state.node->tree()->root());
}

//-----------------------------------------------------------------------------
void NextParser::_start_map(bool as_child)
{
    printf("start_map\n");
    m_state.flags |= RMAP|RVAL;
    m_state.flags &= ~RKEY;
    m_state.flags &= ~RUNK;
    Node* parent = m_stack.empty() ? m_root : m_stack.peek().node;
    C4_ASSERT(parent != nullptr);
    C4_ASSERT(m_state.node == nullptr || m_state.node == m_root);
    if(as_child)
    {
        cspan name = _consume_scalar();
        m_state.node = parent->tree()->begin_map(name);
    }
    else
    {
        C4_ASSERT(parent->is_map() || parent->empty());
        m_state.node = parent;
        m_state.node->m_type = TYPE_MAP;
        _move_scalar_from_top();
    }
    printf("start_map: id=%zd name='%.*s'\n", m_state.node->id(), _c4prsp(m_state.node->name()));
}
void NextParser::_stop_map()
{
    printf("stop_map\n");
    C4_ASSERT(m_state.node->is_map());
    m_state.node->tree()->end_map();
}

//-----------------------------------------------------------------------------
void NextParser::_start_seq(bool as_child)
{
    printf("start_seq\n");
    m_state.flags |= RSEQ|RVAL;
    m_state.flags &= ~RUNK;
    Node* parent = m_stack.empty() ? m_root : m_stack.peek().node;
    C4_ASSERT(parent != nullptr);
    C4_ASSERT(m_state.node == nullptr || m_state.node == m_root);
    if(as_child)
    {
        cspan name = _consume_scalar();
        m_state.node = parent->tree()->begin_seq(name);
    }
    else
    {
        C4_ASSERT(parent->is_seq() || parent->empty());
        m_state.node = parent;
        m_state.node->m_type = TYPE_SEQ;
        _move_scalar_from_top();
    }
    printf("start_seq: id=%zd name='%.*s'\n", m_state.node->id(), _c4prsp(m_state.node->name()));
}

void NextParser::_stop_seq()
{
    printf("stop_seq\n");
    C4_ASSERT(m_state.node->is_seq());
    m_state.node->tree()->end_seq();
}

//-----------------------------------------------------------------------------
void NextParser::_append_val(cspan const& val)
{
    C4_ASSERT(m_state.node != nullptr);
    C4_ASSERT(m_state.node->is_seq());
    printf("append val: '%.*s'\n", _c4prsp(val));
    m_state.node->append_child_seq(val);
    printf("append val: id=%zd name='%.*s' val='%.*s'\n", m_state.node->last_child()->id(), _c4prsp(m_state.node->last_child()->name()), _c4prsp(m_state.node->last_child()->val()));

    //emit(m_state.node->tree()->root());
}

void NextParser::_append_key_val(cspan const& val)
{
    C4_ASSERT(m_state.node->is_map());
    cspan key = _consume_scalar();;
    printf("append key-val: '%.*s' '%.*s'\n", _c4prsp(key), _c4prsp(val));
    m_state.node->append_child(key, val);
    printf("append key-val: id=%zd name='%.*s' val='%.*s'\n", m_state.node->last_child()->id(), _c4prsp(m_state.node->last_child()->name()), _c4prsp(m_state.node->last_child()->val()));
    _toggle_key_val();

    //emit(m_state.node->tree()->root());
}

void NextParser::_toggle_key_val()
{
    if(m_state.flags & RKEY)
    {
        m_state.flags &= ~RKEY;
        m_state.flags |= RVAL;
    }
    else
    {
        m_state.flags |= RKEY;
        m_state.flags &= ~RVAL;
    }
}

//-----------------------------------------------------------------------------
void NextParser::_store_scalar(cspan const& s)
{
    printf("storing scalar: '%.*s'\n", _c4prsp(s));
    C4_ASSERT(m_state.has_none(SSCL));
    m_state.flags |= SSCL;
    m_state.scalar = s;
}

cspan NextParser::_consume_scalar()
{
    printf("consuming scalar: '%.*s' (flag: %d))\n", _c4prsp(m_state.scalar), m_state.scalar & SSCL);
    C4_ASSERT(m_state.flags & SSCL);
    cspan s = m_state.scalar;
    m_state.flags &= ~SSCL;
    m_state.scalar.clear();
    return s;
}

void NextParser::_move_scalar_from_top()
{
    if(m_stack.empty()) return;
    if(m_stack.peek().flags & SSCL)
    {
        State &top = m_stack.peek();
        printf("moving scalar: '%.*s' (overwriting '%.*s')\n", _c4prsp(top.scalar), _c4prsp(m_state.scalar));
        m_state.flags |= (top.flags & SSCL);
        m_state.scalar = top.scalar;
        top.flags &= ~SSCL;
        top.scalar.clear();
    }
}

//-----------------------------------------------------------------------------
int NextParser::_handle_indentation()
{
    int jump = _get_indentation_jump();
    if(jump < 0)
    {
        printf("indentation decreased %zd level%s!\n", jump, jump>1?"s":"");
        for(size_t i = 0; i < jump; ++i)
        {
            _pop_level();
        }
    }
    else if(jump > 0)
    {
        C4_ERROR("never reach this");
        // level must be pushed elsewhere;
        // too complicated to deal with here
    }
    return jump;
}

int NextParser::_get_indentation_jump()
{
    if(m_stack.empty())
    {
        C4_ASSERT(m_state.has_all(RTOP));
        return 0;
    }
    int lprev = (int)m_stack.peek().level;
    int lcurr = (int)m_state.level;
    int iprev = (int)m_stack.peek().line_contents.indentation;
    int icurr = (int)m_state.line_contents.indentation;
    return icurr != iprev ? lcurr - lprev : 0;
}

//-----------------------------------------------------------------------------
cspan NextParser::_scan_quoted_scalar(const char q)
{
    // quoted scalars can spread over multiple lines!
    // nice explanation here: http://yaml-multiline.info/

    cspan s = m_state.line_contents.rem;
    C4_ASSERT(q == '\'' || q == '\"');

    // find the pos of the matching quote
    size_t pos = npos, sum = 0;
    C4_ASSERT(s.begins_with(q));

    // skip the opening quote
    m_state.line_progressed(1);

    bool needs_filter = true;

    while( ! _finished_file())
    {
        _scan_line();
        s = m_state.line_contents.rem;

        for(size_t i = 1; i < s.len; ++i)
        {
            const char prev = s.str[i-1];
            const char curr = s.str[i];
            if(q == '\'') // scalars with single quotes
            {
                // ... are only escaped with two single quotes
                if(curr == '\'')
                {
                    if(prev != '\'')
                    {
                        pos = i;
                        break;
                    }
                    else
                    {
                        // there are two single quotes, so needs filter
                        needs_filter = true;
                    }
                }
            }
            else // scalars with double quotes
            {
                // every \ is an escape
                if(curr == '"')
                {
                    if(prev != '\\')
                    {
                        pos = i;
                        break;
                    }
                    else
                    {
                        needs_filter = true;
                    }
                }
                else if(curr == '\\')
                {
                    needs_filter = true;
                }
            }
        }

        if(pos != npos)
        {
            m_state.line_progressed(pos);
            pos += sum;
        }
        else
        {
            m_state.line_progressed(s.len);
            sum += s.len;
            break;
        }

        _next_line();
    }
    if(pos == npos)
    {
        C4_ERROR("reached end of file while looking for closing quote");
    }
    C4_ASSERT(s[pos] == q);
    s = s.subspan(1, pos - 1);

    if(needs_filter)
    {
        cspan ret = _filter_quoted_scalar(s, q);
        C4_ASSERT(ret.len < s.len);
        return ret;
    }

    return s;
}

//-----------------------------------------------------------------------------
cspan NextParser::_scan_block()
{
    // nice explanation here: http://yaml-multiline.info/
    cspan s = m_state.line_contents.rem;
    C4_ASSERT(s.begins_with('|') || s.begins_with('>'));

    // parse the spec
    BlockStyle_e newline = s.begins_with('>') ? BLOCK_FOLD : BLOCK_LITERAL;
    BlockChomp_e chomp = CHOMP_CLIP; // default to clip unless + or - are used
    size_t indentation = npos; // have to find out if no spec is given
    if(s.len > 1)
    {
        char c = s.str[1];
        if(c == '-')
        {
            chomp = CHOMP_STRIP;
            c = s.len > 2 ? s.str[2] : '\0'; // advance
        }
        else if(c == '+')
        {
            chomp = CHOMP_KEEP;
            c = s.len > 2 ? s.str[2] : '\0'; // advance
        }

        // from here to the end, only digits (or nothing) are allowed
        if(c >= '0' && c <= '9')
        {
            for(size_t i = 1; i < s.len; ++i)
            {
                C4_ASSERT(s.str[i] >= '0' && s.str[i] <= '9');
            }
            cspan number = s.subspan(1);
            if( ! _read_decimal(number, &indentation))
            {
                C4_ERROR("parse error: could not read decimal");
            }
        }
        else if(c == '\0')
        {
            // no digit appears: we're done parsing the spec
        }
        else
        {
            C4_ERROR("parse error: invalid characters in block specification");
        }
    }

    C4_ERROR("not implemented");

    // finish the current line
    _next_line();

    // start with a zero-length block, already pointing at the right place
    cspan raw_block = m_state.line_contents.full.subspan(0, 0);
    C4_ASSERT(raw_block.begin() == m_state.line_contents.full.begin());

    // read every full line into a raw block,
    // from which newlines are to be stripped as needed
    size_t num_lines = 0, first = m_state.pos.line;
    while( ! _finished_file() && m_state.line_contents.indentation == indentation)
    {
        _scan_line();
        raw_block.len += m_state.line_contents.full.len;
        _next_line();
        ++num_lines;
    }
    C4_ASSERT(m_state.pos.line == (first + num_lines));
    (void)num_lines; // prevent warning
    (void)first;

    // ok! now we strip the newlines and spaces according to the specs
    s = _filter_raw_block(raw_block, newline, chomp, indentation);

    return s;
}

//-----------------------------------------------------------------------------
cspan NextParser::_filter_quoted_scalar(cspan const& s, const char q)
{
    C4_ERROR("not implemented");
    return {};
}

//-----------------------------------------------------------------------------
cspan NextParser::_filter_raw_block(cspan const& block, BlockStyle_e style, BlockChomp_e chomp, size_t indentation)
{
    C4_ASSERT(block.ends_with('\n') || block.ends_with('\r'));

    // with 0-indentation literal blocks we don't need
    // to erase characters from the middle of the string,
    // so we can just reuse the block and chomp at the end
    // by getting a subspan of the block
    if(style == BLOCK_LITERAL && indentation == 0)
    {
        switch(chomp)
        {
        case CHOMP_KEEP:
            return block;
        case CHOMP_STRIP: // strip everything
            {
                auto pos = block.last_not_of("\r\n");
                C4_ASSERT(pos != npos);
                auto ret = block.left_of(pos, /*include_pos*/true);
                return ret;
            }
        case CHOMP_CLIP: // keep a single newline
            {
                auto pos = block.last_not_of("\r\n");
                C4_ASSERT(pos != npos && pos+1 < block.len);
                ++pos;
                // deal for \r\n sequences
                if(block[pos] == '\r')
                {
                    C4_ASSERT(pos+1 < block.len);
                    ++pos;
                    C4_ASSERT(block[pos] == '\n');
                }
                auto ret = block.left_of(pos, /*include_pos*/true);
                return ret;
            }
        default:
            C4_ERROR("unknown style");
        }
    }

    return {};
}

//-----------------------------------------------------------------------------
bool NextParser::_read_decimal(cspan const& str, size_t *decimal)
{
    C4_ASSERT(str.len > 1);
    size_t n = 0, c = 0;
    for(size_t i = 0; i < str.len; ++i)
    {
        if(str.str[i] < '0' || str.str[i] > '9') return false;
        if(i > 1) n *= 10;
        n += c;
    }
    *decimal = n;
    return true;
}

} // namespace ryml
} // namespace c4
