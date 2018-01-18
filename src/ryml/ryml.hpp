#ifndef _C4_RYML_HPP_
#define _C4_RYML_HPP_

#include <yaml.h>

#include "./common.hpp"
#include "./span.hpp"
#include "./stack.hpp"

#define RYML_DBG // remove this
#define RYML_ERRMSG_SIZE 1024

namespace c4 {
namespace yml {

class Parser;


typedef enum {
    BLOCK_LITERAL, ///< keep newlines (|)
    BLOCK_FOLD     ///< replace newline with single space (>)
} BlockStyle_e;

typedef enum {
    CHOMP_CLIP,    ///< single newline at end (default)
    CHOMP_STRIP,   ///< no newline at end     (-)
    CHOMP_KEEP     ///< all newlines from end (+)
} BlockChomp_e;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

typedef enum {
    NOTYPE  = 0,       ///< no type is set
    VAL     = (1<<0),  ///< a leaf node, has a (possibly empty) value
    KEY     = (1<<1),  ///< is member of a map, must have non-empty key
    KEYVAL  = KEY|VAL,
    MAP     = (1<<2),  ///< a map: a parent of keyvals
    SEQ     = (1<<3),  ///< a seq: a parent of vals
    DOC     = (1<<4),  ///< a document
    DOCMAP  = DOC|MAP,
    DOCSEQ  = DOC|SEQ,
} NodeType_e;


class Tree;

/** an index to none */
enum : size_t { NONE = size_t(-1) };


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class Node
{
public:

    struct listseq
    {
        size_t prev;
        size_t next;
    };
    struct children
    {
        size_t first;
        size_t last;
    };

public:

    mutable Tree* m_s;
    NodeType_e    m_type;
    cspan         m_name;
    size_t        m_parent;
    listseq       m_siblings;
    union {
        cspan     m_val;
        children  m_children;
    };

    listseq       m_list;

public:

    Tree *tree() const { return m_s; }
    size_t id() const;

    NodeType_e type() const { return m_type; }
    const char* type_str() const { return type_str(m_type); }
    static const char* type_str(NodeType_e ty);

    cspan const& name() const { return m_name; }
    cspan const& val() const { C4_ASSERT(m_type & VAL); return m_val; }

    bool operator== (cspan const& cmp) const
    {
        C4_ASSERT(is_val());
        return m_val == cmp;
    }

    bool   is_root() const { return m_parent == NONE; }
    bool   is_container() const { return m_type & (MAP|SEQ); }
    bool   is_doc() const { return m_type & DOC; }
    bool   is_map() const { return m_type & MAP; }
    bool   is_seq() const { return m_type & SEQ; }
    bool   is_val() const { return m_type & VAL; }
    bool   has_key() const { return m_type & KEY; }
    bool   parent_is_seq() const { return parent()->is_seq(); }
    bool   parent_is_map() const { return parent()->is_map(); }

    /** true when name and value are empty, and has no children */
    bool   empty() const { return ! has_children() && m_name.empty() && ((m_type & VAL) || m_val.empty()); }
    bool   has_children() const { return num_children() != 0; }
    bool   has_siblings() const { return num_siblings() != 0; }

    bool   is_child(Node const* ch) const;
    bool   is_sibling(Node const* n) const;

public:

    Node * parent() const;

    /** O(#num_children) */
    size_t num_children() const;
    Node * child(size_t i) const;
    Node * first_child() const;
    Node * last_child() const;
    Node * find_child(cspan const& name) const;

    /** O(#num_siblings) */
    size_t num_siblings() const;
    Node * sibling(size_t i) const;
    Node * first_sibling() const;
    Node * last_sibling() const;
    Node * find_sibling(cspan const& name) const;

    Node * prev_sibling() const;
    Node * next_sibling() const;

public:

    Node & operator[] (size_t i) const
    {
        Node *c = child(i);
        if( ! c) { RymlCallbacks::error("could not find node");}
        return *c;
    }

    Node & operator[] (cspan const& name) const
    {
        C4_ASSERT(is_map());
        Node *c = find_child(name);
        if( ! c) { RymlCallbacks::error("could not find node"); }
        return *c;
    }

public:

    template< class Visitor >
    bool visit(Visitor fn, size_t indentation_level=0, bool skip_root=true);

    template< class Visitor >
    bool visit(Visitor fn, size_t indentation_level=0, bool skip_root=true) const;

public:

    //! create and insert a new sibling as a key-value node. insert after "after"
    Node *  insert_sibling(cspan const& name, cspan const& val, Node * after);
    Node *  append_sibling(cspan const& name, cspan const& val) { return insert_sibling(name, val, last_sibling()); }
    Node * prepend_sibling(cspan const& name, cspan const& val) { return insert_sibling(name, val, nullptr); }

    //! create and insert a new sibling as a value node in a seq entry. insert after "after".
    Node *  insert_sibling_seq(cspan const& val, Node * after) { return insert_sibling({}, val, after); }
    Node *  append_sibling_seq(cspan const& val) { return insert_sibling({}, val, last_sibling()); }
    Node * prepend_sibling_seq(cspan const& val) { return insert_sibling({}, val, nullptr); }

    //! create and insert a new sibling while specifying its type. insert after "after".
    Node *  insert_sibling(cspan const& name, NodeType_e sibtype, Node * after);
    Node *  append_sibling(cspan const& name, NodeType_e sibtype) { return insert_sibling(name, sibtype, last_sibling()); }
    Node * prepend_sibling(cspan const& name, NodeType_e sibtype) { return insert_sibling(name, sibtype, nullptr); }

    //! create and insert a new sibling as a seq entry while specifying its type. insert after "after".
    Node *  insert_sibling_seq(NodeType_e sibtype, Node * after) { return insert_sibling({}, sibtype, last_sibling()); }
    Node *  append_sibling_seq(NodeType_e sibtype) { return insert_sibling({}, sibtype, last_sibling()); }
    Node * prepend_sibling_seq(NodeType_e sibtype) { return insert_sibling({}, sibtype, nullptr); }

    //! insert a node as sibling. insert after "after".
    Node *  insert_sibling(Node *sib, Node * after);
    Node *  append_sibling(Node *sib) { return insert_sibling(sib, last_sibling()); }
    Node * prepend_sibling(Node *sib) { return insert_sibling(sib, nullptr); }

public:

    //! create and insert a new child as a key-value node. insert after "after"
    Node *  insert_child(cspan const& name, cspan const& val, Node * after = nullptr);
    Node *  append_child(cspan const& name, cspan const& val) { return insert_child(name, val, last_child()); }
    Node * prepend_child(cspan const& name, cspan const& val) { return insert_child(name, val, nullptr); }

    //! create and insert a new child as a value node in a seq entry. insert after "after".
    Node *  insert_child_seq(cspan const& val, Node * after = nullptr) { return insert_child({}, val, last_child()); }
    Node *  append_child_seq(cspan const& val) { return insert_child({}, val, last_child()); }
    Node * prepend_child_seq(cspan const& val) { return insert_child({}, val, nullptr); }

    //! create and insert a new sibling while specifying its type. insert after "after".
    Node *  insert_child(cspan const& name, NodeType_e chtype, Node * after = nullptr);
    Node *  append_child(cspan const& name, NodeType_e chtype) { return insert_child(name, chtype, last_child()); }
    Node * prepend_child(cspan const& name, NodeType_e chtype) { return insert_child(name, chtype, nullptr); }

    //! create and insert a new child as a seq entry while specifying its type. insert after "after".
    Node *  insert_child_seq(NodeType_e chtype, Node * after = nullptr) { return insert_child({}, chtype, last_child()); }
    Node *  append_child_seq(NodeType_e chtype) { return insert_child({}, chtype, last_child()); }
    Node * prepend_child_seq(NodeType_e chtype) { return insert_child({}, chtype, nullptr); }

    //! insert a node as child. insert after "after".
    Node *  insert_child(Node *ch, Node * after);
    Node *  append_child(Node *ch) { return insert_child(ch, last_child()); }
    Node * prepend_child(Node *ch) { return insert_child(ch, nullptr); }

private:

    Node * _insert_by_type(Node *which_parent, cspan const& name, NodeType_e type, Node *after);

public:

    Node * remove_sibling(cspan const& name); ///< remove a sibling by name
    Node * remove_sibling(size_t i); ///< remove a sibling by index
    Node * remove_sibling(Node *n);  ///< remove a sibling

    Node * remove_child(cspan const& name) { C4_ASSERT(has_children()); return first_child()->remove_sibling(name); }; ///< remove a child by name
    Node * remove_child(size_t i) { C4_ASSERT(has_children()); return first_child()->remove_sibling(i); }; ///< remove a child by index
    Node * remove_child(Node *n) { C4_ASSERT(has_children()); return first_child()->remove_sibling(n); };  ///< remove a child

public:

    void _add_flags(NodeType_e f) { m_type = (NodeType_e)((int)f|(int)m_type); }
    void _add_flags(int        f) { m_type = (NodeType_e)(     f|(int)m_type); }

    void _rem_flags(NodeType_e f) { m_type = (NodeType_e)((~(int)f)&(int)m_type); }
    void _rem_flags(int        f) { m_type = (NodeType_e)((~     f)&(int)m_type); }

    void _set_flags(NodeType_e f) { m_type = f; }
    void _set_flags(int        f) { m_type = (NodeType_e)f; }

private:

    Node * prev_node() const;
    Node * next_node() const;

};


template< class Visitor >
bool Node::visit(Visitor fn, size_t indentation_level, bool skip_root)
{
    return const_cast< Node const* >(this)->visit(fn, indentation_level, skip_root);
}

template< class Visitor >
bool Node::visit(Visitor fn, size_t indentation_level, bool skip_root) const
{
    size_t increment = 0;
    if( ! (is_root() && skip_root))
    {
        if(fn(this, indentation_level))
        {
            return true;
        }
        ++increment;
    }
    fn.push(this);
    for(Node *ch = first_child(); ch; ch = ch->next_sibling())
    {
        // no need to forward skip_root as it won't be root
        if(ch->visit(fn, indentation_level + increment))
        {
            fn.pop(this);
            return true;
        }
    }
    fn.pop(this);
    return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class Tree
{
private:

    Node * m_buf;
    size_t m_cap;

    size_t m_size;
    size_t m_head;
    size_t m_tail;

    size_t m_free_head;
    size_t m_free_tail;

    /** @todo this is wrong, and should be moved out of this class */
    detail::stack< size_t > m_stack;

    size_t m_load_root_id;

public:

    Tree();
    Tree(size_t sz);
    ~Tree();

    size_t capacity() const { return m_cap; }
    size_t size    () const { return m_size; }

    void reserve(size_t sz);

    void clear();

public:

    size_t id(Node const* n)
    {
        if( ! n) return NONE;
        C4_ASSERT(n >= m_buf && n < m_buf + m_cap);
        return n - m_buf;
    }
    size_t id(Node const* n) const
    {
        if( ! n) return NONE;
        C4_ASSERT(n >= m_buf && n < m_buf + m_cap);
        return n - m_buf;
    }

    Node *get(size_t i)
    {
        if(i == NONE) return nullptr;
        C4_ASSERT(i >= 0 && i < m_cap);
        return m_buf + i;
    }
    Node const *get(size_t i) const
    {
        if(i == NONE) return nullptr;
        C4_ASSERT(i >= 0 && i < m_cap);
        return m_buf + i;
    }

public:

    void serialize(Node *root, span const* buffer) const;
    Node * load(Node *root, cspan const& yml_str, Parser *p_ = nullptr);

public:

    void set_load_root(Node *r) { m_load_root_id = r ? id(r) : NONE; }

    void add_root()
    {
        begin_stream();
        end_stream();
        m_stack.push(id(root()));
    }

    Node *begin_stream()
    {
        if(m_load_root_id != NONE)
        {
            C4_ASSERT(get(m_load_root_id)->is_map());
            m_stack.push(m_load_root_id);
            //printf("pushing: load root: %p %d\n", (void*)get(m_load_root_id), get(m_load_root_id)->m_type);
            return get(m_load_root_id);
        }

        C4_ASSERT(m_stack.empty());
        Node *n = claim(nullptr);
        //n->_add_flags(ROOT);

        m_stack.push(id(n));
        return n;
    }
    Node *end_stream()
    {
        return _stack_pop();
    }

    Node *begin_doc(Node *after = nullptr)
    {
        if(m_load_root_id != NONE)
        {
            m_stack.push(m_load_root_id);
            return get(m_load_root_id);
        }

        C4_ASSERT(after == nullptr || after->m_parent == NONE);
        C4_ASSERT( ! after || after->is_doc());
        C4_ASSERT( ! after || get(after->m_parent) == _stack_top());
        C4_ASSERT(_stack_top());
        C4_ASSERT(_stack_top()->is_root());
        return _stack_push({}, DOC, after);
    }
    Node *end_doc()
    {
        return _stack_pop();
    }

    Node *add_val_seq(cspan const& val, Node *after = nullptr)
    {
        C4_ASSERT(_stack_top()->is_seq());
        return add_val({}, val, after);
    }
    /** place the new node after "after" */
    Node *add_val(cspan const& name, cspan const& val, Node *after = nullptr)
    {
        if(name.empty())
        {
            C4_ASSERT(_stack_top()->is_seq());
        }
        else
        {
            C4_ASSERT(_stack_top()->is_map());
        }
        size_t ida = id(after);
        Node *n = claim(after);
        after = get(ida);
        n->_set_flags(VAL);
        n->m_name = name;
        n->m_val  = val;
        if(n->m_name) n->_add_flags(KEY);

        set_parent(_stack_top(), n, after);
        return n;
    }
    Node *add_empty(cspan const& name, Node *after = nullptr)
    {
        if(name.empty())
        {
            C4_ASSERT(_stack_top()->is_seq());
        }
        else
        {
            C4_ASSERT(_stack_top()->is_map());
        }
        size_t ida = id(after);
        Node *n = claim(after);
        after = get(ida);
        n->_set_flags(NOTYPE);
        set_parent(_stack_top(), n, after);
        return n;
    }

    /** place the new node after "after" */
    Node *begin_map(cspan const& name, Node *after = nullptr)
    {
        return _stack_push(name, MAP, after);
    }
    Node *end_map()
    {
        return _stack_pop();
    }
    void make_map(Node *node, Node *after = nullptr)
    {
        C4_ASSERT( ! node->has_children());
        node->_set_flags(MAP);
        set_parent(_stack_top(), node, after);
        _stack_push(node);
    }

    /** place the new node after "after" */
    Node *begin_seq(cspan const& name, Node *after = nullptr)
    {
        return _stack_push(name, SEQ, after);
    }
    Node *end_seq()
    {
        return _stack_pop();
    }
    void make_seq(Node *node, Node *after = nullptr)
    {
        C4_ASSERT( ! node->has_children());
        node->_set_flags(SEQ);
        set_parent(_stack_top(), node, after);
        _stack_push(node);
    }

    bool stack_top_is_type(NodeType_e type)
    {
        return _stack_top()->m_type & type;
    }

    Node * _stack_top()
    {
        C4_ASSERT( ! m_stack.empty());
        size_t id = m_stack.peek();
        Node *n = get(id);
        return n;
    }

    Node * top_last()
    {
        return _stack_top()->last_child();
    }

    void set_parent(Node *parent, Node *child, Node *prev_sibling, Node *next_sibling = nullptr);

private:

    friend class Node;
    void _stack_push(Node *n)
    {
        m_stack.push(id(n));
    }
    Node * _stack_push(cspan const& name, NodeType_e type, Node *after)
    {
        size_t ida = id(after);
        Node *n = claim(after);
        after = get(ida);

        n->m_type = type;
        n->m_name = name;

        set_parent(_stack_top(), n, after);

        m_stack.push(id(n));
        return n;
    }

    Node *_stack_pop()
    {
        Node *n = get(m_stack.pop());
        return n;
    }

private:

    void clear_range(size_t first, size_t num);

    Node *claim(Node *after);
    Node *claim(size_t prev, size_t next);

    void free(size_t i);

    Node      * head()       { return m_buf + m_head; }
    Node const* head() const { return m_buf + m_head; }

    Node      * tail()       { return m_buf + m_tail; }
    Node const* tail() const { return m_buf + m_tail; }

public:

    Node      * root()       { C4_ASSERT(m_cap > 0); Node *n = m_buf; C4_ASSERT(n->parent() == nullptr); return n; }
    Node const* root() const { C4_ASSERT(m_cap > 0); Node *n = m_buf; C4_ASSERT(n->parent() == nullptr); return n; }

    Node      * first_doc()         { Node *n = root()->child(0); C4_ASSERT(n && n->is_doc()); return n; }
    Node const* first_doc() const   { Node *n = root()->child(0); C4_ASSERT(n && n->is_doc()); return n; }
    Node      * doc(size_t i)       { Node *n = root()->child(i); C4_ASSERT(n && n->is_doc()); return n; }
    Node const* doc(size_t i) const { Node *n = root()->child(i); C4_ASSERT(n && n->is_doc()); return n; }

};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class Emitter
{
public:

    span m_span;
    size_t m_pos;

    FILE *m_file;

    friend struct Visitor;

public:

    /** create an emitter that outputs to a file. Defaults to stdout. */
    Emitter(FILE *f = nullptr) : m_span(), m_pos(0), m_file(f ? f : stdout) {}
    /** create an emitter that writes to a string span */
    Emitter(span const& sp) : m_span(sp), m_pos(0), m_file(nullptr) {}

    /** emit YAML to the given file or buffer.
     *
     * When writing to a buffer, returns a subspan of the emitted YAML.
     * If the given buffer has insufficient space, the returned span will
     * be null and its size will be the needed space. No writes are done
     * after the end of the buffer.
     *
     * When writing to a file, the returned span will be null, but its
     * length is set the number of bytes written. */
    span emit(Node const* n, bool error_on_excess=true)
    {
        C4_ASSERT( ! m_span.empty() || m_file);
        size_t num = this->_visit(n);
        span result = {};
        if(m_file)
        {
            result.len = num;
            result.str = nullptr;
        }
        else
        {
            result = m_span.subspan(0, num);
            if(num > m_span.len)
            {
                result.str = nullptr;
                if(error_on_excess)
                {
                    RymlCallbacks::error("not enough space in the given span");
                }
            }
        }
        return result;
    }

    size_t tell() const { return m_pos; }
    void   seek(size_t p) { m_pos = p; }

private:

    size_t _visit(Node const* first)
    {
        size_t b = m_pos;
        Visitor v{this};
        first->visit(v);
        size_t e = m_pos - b;
        return e;
    }

    struct Visitor
    {
        Emitter *this_;
        void push(Node const *n) {}
        void pop(Node const *n) {}
        bool operator()(Node const *n, size_t ilevel)
        {
            switch(n->type())
            {
            case VAL:
                this_->_writeind(ilevel);
                if(n->parent_is_seq())
                {
                    this_->_writev(n->val());
                }
                else if(n->parent_is_map())
                {
                    this_->_writekv(n->name(), n->val());
                }
                break;
            case MAP:
            case SEQ:
                this_->_writeind(ilevel);
                this_->_writek(n->name());
                break;
            case DOC:
            case DOCMAP:
            case DOCSEQ:
                break;
            case NOTYPE:
                break;
            default:
                break;
            }
            return false;
        }
    };

    void _writeind(size_t level)
    {
        size_t num = 2 * level; // 2 spaces per indentation level
        if(m_file)
        {
            m_pos += fprintf(m_file, "%*s", (int)num, "");
        }
        else
        {
            if(m_pos + num < m_span.len)
            {
                _write(' ',  num);
            }
            else
            {
                m_pos += num;
            }
        }

    }

    void _writek(cspan const& k)
    {
        if(m_file)
        {
            m_pos += fprintf(m_file, "%.*s:\n", (int)k.len, k.str);
        }
        else
        {
            size_t num = k.len + 1;
            if(m_pos + num < m_span.len)
            {
                _write(k);
                _write('\n');
            }
            else
            {
                m_pos += num;
            }
        }
    }

    void _writev(cspan const& v)
    {
        if(m_file)
        {
            m_pos += fprintf(m_file, "- %.*s\n", (int)v.len, v.str);
        }
        else
        {
            size_t num = 2 + v.len + 1;
            if(m_pos + num < m_span.len)
            {
                _write("- ");
                _write(v);
                _write('\n');
            }
            else
            {
                m_pos += num;
            }
        }
    }

    void _writekv(cspan const& k, cspan const& v)
    {
        if(m_file)
        {
            m_pos += fprintf(m_file, "%.*s: %.*s\n",
                             (int)k.len, k.str,
                             (int)v.len, v.str);
        }
        else
        {
            size_t num = 2 + v.len + 1;
            if(m_pos + num < m_span.len)
            {
                _write(k);
                _write(": ");
                _write(v);
                _write('\n');
            }
            else
            {
                m_pos += num;
            }
        }
    }

private:

    inline void _write(const char c)
    {
        C4_ASSERT(m_pos + 1 < m_span.len);
        m_span[m_pos] = c;
        ++m_pos;
    }

    inline void _write(const char c, size_t num_times)
    {
        for(size_t i = m_pos, e = m_pos + num_times; i < e; ++i)
        {
            m_span[m_pos + i] = c;
        }
        m_pos += num_times;
    }

    template< size_t N >
    inline void _write(const char (&s)[N])
    {
        size_t nm1 = N-1;
        C4_ASSERT(s[nm1] == '\0');
        C4_ASSERT(m_pos + nm1 < m_span.len);
        memcpy(&(m_span[m_pos]), s, nm1);
        m_pos += nm1;
    }

    inline void _write(cspan const& sp)
    {
        C4_ASSERT(m_pos + sp.len < m_span.len);
        memcpy(&(m_span[m_pos]), sp.str, sp.len);
        m_pos += sp.len;
    }
};

//-----------------------------------------------------------------------------
/** emit YAML to the given file. A null file defaults to stdout.
 * Return the number of bytes written. */

inline size_t emit(Node const* n, FILE *f = nullptr)
{
    Emitter em(f);
    size_t num = em.emit(n).len;
    return num;
}
inline size_t emit(Tree const &t, FILE *f = nullptr)
{
    return emit(t.root(), f);
}

//-----------------------------------------------------------------------------
/** emit YAML to the given buffer. Return a subspan of the emitted YAML.
 * Raise an error if the space in the buffer is insufficient. */
inline span emit(Node const* n, span const& sp, bool error_on_excess=true)
{
    Emitter em(sp);
    span result = em.emit(n, error_on_excess);
    return result;
}
inline span emit(Tree const& t, span const& sp, bool error_on_excess=true)
{
    return emit(t.root(), sp, error_on_excess);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class NextParser
{
private:

    typedef enum {
        RTOP = 0x01 <<  0, // reading at top level
        RUNK = 0x01 <<  1, // reading an unknown: must determine whether scalar, map or seq
        RMAP = 0x01 <<  2, // reading a map
        RSEQ = 0x01 <<  3, // reading a seq
        EXPL = 0x01 <<  4, // reading is inside explicit flow chars: [] or {}
        RKEY = 0x01 <<  5, // reading a scalar as key
        RVAL = 0x01 <<  6, // reading a scalar as val
        CPLX = 0x01 <<  7, // reading a complex key
        SSCL = 0x01 <<  8, // there's a scalar stored

        STARTED_ = 0x01 << 16, // mark the parser started
        INDOK = 0x01 << 17, // allow indentation jumps
    } State_e;

    struct LineContents
    {
        cspan  full;        ///< the full line, including newlines on the right
        cspan  stripped;    ///< the stripped line, excluding newlines on the right
        cspan  rem;         ///< the stripped line remainder; initially starts at the first non-space character
        int    indentation; ///< the number of spaces on the beginning of the line

        void reset(cspan const& full_, cspan const& stripped_)
        {
            full = full_;
            stripped = stripped_;
            rem = stripped_;
            // find the first column where the character is not a space
            indentation = (int)full.first_not_of(' ');
        }
    };

    struct State
    {
        size_t       flags;
        size_t       level;
        Node *       node;
        cspan        scalar;

        Location     pos;
        LineContents line_contents;
        int          prev_indentation;
        int          indentation_jump;

        void _prepare_pop(State const& current)
        {
            pos = current.pos;
            line_contents = current.line_contents;
            scalar = current.scalar;
        }

        void reset(const char *file, Node *n)
        {
            flags = RUNK|RTOP;
            level = 0;
            pos.name = file;
            pos.offset = 0;
            pos.line = 1;
            pos.col = 1;
            node = n;
            scalar.clear();
            prev_indentation = 0;
            indentation_jump = 0;
        }

        void line_scanned(cspan const& full, cspan const& stripped)
        {
            prev_indentation = line_contents.indentation;
            line_contents.reset(full, stripped);
            if(flags & STARTED_)
            {
                indentation_jump = line_contents.indentation - prev_indentation;
            }
            flags |= STARTED_;
            printf("%3zd: '%.*s' (jump=%d)\n", pos.line-1, _c4prsp(line_contents.stripped), indentation_jump);
        }

        void line_progressed(size_t ahead)
        {
            pos.offset += ahead;
            pos.col += ahead;
            C4_ASSERT(pos.col <= line_contents.stripped.len+1);
            line_contents.rem = line_contents.rem.subspan(ahead);
        }

        void line_ended()
        {
            C4_ASSERT(pos.col == line_contents.stripped.len+1);
            pos.offset += line_contents.full.len - line_contents.stripped.len;
            ++pos.line;
            pos.col = 1;
        }

        bool has_all(size_t f) const { return (flags & f) == f; }
        bool has_any(size_t f) const { return (flags & f) != 0; }
        bool has_none(size_t f) const { return (flags & f) == 0; }
    };

public:

    cspan  m_file;
    cspan  m_buf;

    Node * m_root;
    Tree * m_tree;

    State  m_state;
    detail::stack< State > m_stack;

public:

    Tree parse(                   cspan const& buf) { return parse({}, buf); }
    Tree parse(cspan const& file, cspan const& buf)
    {
        Tree t;
        t.add_root();
        parse(file, buf, &t);
        return t;
    }

    void parse(                   cspan const& buf, Tree *t) { return parse({}, buf, t); }
    void parse(cspan const& file, cspan const& buf, Tree *t)
    {
        parse(file, buf, t->root());
    }

    void parse(                   cspan const& buf, Node *root) { return parse({}, buf, root); }
    void parse(cspan const& file, cspan const& buf, Node *root);

private:

    void _reset();

    bool  _finished_file() const;
    bool  _finished_line() const;

    void  _scan_line();
    void  _next_line() { m_state.line_ended(); }

    cspan _scan_scalar();
    cspan _scan_comment();
    cspan _scan_quoted_scalar(const char q);
    cspan _scan_block();

    cspan _filter_quoted_scalar(cspan const& s, const char q);
    cspan _filter_raw_block(cspan const& block, BlockStyle_e style, BlockChomp_e chomp, size_t indentation);

    void  _handle_finished_file();
    void  _handle_line();
    int   _handle_indentation();

    bool  _handle_unk(cspan rem);
    bool  _handle_map(cspan rem);
    bool  _handle_seq(cspan rem);
    bool  _handle_scalar(cspan rem);
    bool  _handle_top(cspan rem);
    bool  _handle_anchors_and_refs(cspan rem);
    bool  _handle_types(cspan rem);

    void  _push_level(bool explicit_flow_chars = false);
    void  _pop_level();

    void  _start_unk(bool as_child=true);

    void  _start_map(bool as_child=true);
    void  _stop_map();

    void  _start_seq(bool as_child=true);
    void  _stop_seq();

    void  _start_doc(bool as_child=true);
    void  _stop_doc();

    void  _append_comment(cspan const& cmt);
    void  _append_val(cspan const& val);
    void  _append_key_val(cspan const& val);
    void  _toggle_key_val();

    void  _store_scalar(cspan const& s);
    cspan _consume_scalar();
    void  _move_scalar_from_top();

private:

    static bool _read_decimal(cspan const& str, size_t *decimal);

private:

#ifdef RYML_DBG
    void _dbg(const char *msg, ...) const;
#endif
    void _err(const char *msg, ...) const;
    int  _fmt_msg(char *buf, int buflen, const char *msg, va_list args) const;
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

inline Tree parse(cspan const& buf)
{
    NextParser np;
    return np.parse(buf);
}

inline Tree parse(cspan const& file, cspan const& buf)
{
    NextParser np;
    return np.parse(file, buf);
}

inline void parse(cspan const& buf, Tree *t)
{
    NextParser np;
    np.parse(buf, t);
}

inline void parse(cspan const& file, cspan const& buf, Tree *t)
{
    NextParser np;
    np.parse(file, buf, t);
}

inline void parse(cspan const& buf, Node *root)
{
    NextParser np;
    np.parse(buf, root);
}

inline void parse(cspan const& file, cspan const& buf, Node *root)
{
    NextParser np;
    np.parse(file, buf, root);
}

} // namespace yml
} // namespace c4

#include <ryml/ryml.def.hpp>

#endif // _C4_RYML_HPP_
