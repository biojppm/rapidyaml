#ifndef _C4_RYML_HPP_
#define _C4_RYML_HPP_

#include <yaml.h>

#include "./common.hpp"
#include "./span.hpp"
#include "./stack.hpp"

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
    TYPE_NONE,  ///< no type set yet
    TYPE_ROOT,  ///< root; can be either a seq or a val; cannot have siblings
    TYPE_DOC,   ///< document; can be either a seq or a val; can have siblings
    TYPE_VAL,   ///< value; must have value; has no name of the parent is a seq; must have name if its parent is a map
    TYPE_SEQ,   ///< sequence: the parent of values
    TYPE_MAP    ///< map: the parent of key-values
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
    NodeType_e m_type;
    cspan      m_name;
    size_t     m_parent;
    listseq    m_siblings;
    union {
        cspan      m_val;
        children   m_children;
    };

    listseq    m_list;

public:

    Tree *tree() const { return m_s; }
    size_t id() const;

    NodeType_e type() const { return m_type; }
    cspan const& name() const { return m_name; }
    cspan const& val() const { C4_ASSERT(m_type == TYPE_VAL); return m_val; }

    bool operator== (cspan const& cmp) const
    {
        C4_ASSERT(is_val());
        return m_val == cmp;
    }

    bool   is_container() const { return m_type == TYPE_DOC || m_type == TYPE_MAP || m_type == TYPE_SEQ || m_type == TYPE_ROOT; }
    bool   is_map() const { return m_type == TYPE_MAP || m_type == TYPE_DOC || m_type == TYPE_ROOT; }
    bool   is_seq() const { return m_type == TYPE_SEQ || m_type == TYPE_DOC || m_type == TYPE_ROOT; }
    bool   is_val() const { return m_type == TYPE_VAL; }
    bool   parent_is_seq() const { return parent()->is_seq(); }
    bool   parent_is_map() const { return parent()->is_map(); }

    bool   empty() const { return ! has_children() && m_name.empty() && (m_type != TYPE_VAL || m_val.empty()); }

    Node * parent() const;

    bool   has_children() const { return num_children() != 0; }
    size_t num_children() const;
    Node * child(size_t i) const;
    Node * first_child() const;
    Node * last_child() const;
    Node * find_child(cspan const& name) const;

    bool   is_child(Node const* ch) const;

    bool   has_siblings() const { return num_siblings() != 0; }
    size_t num_siblings() const;
    Node * sibling(size_t i) const;
    Node * first_sibling() const;
    Node * last_sibling() const;
    Node * find_sibling(cspan const& name) const;

    Node * prev_sibling() const;
    Node * next_sibling() const;

    bool   is_sibling(Node const* n) const;


    template< class Visitor >
    void visit(Visitor fn, size_t indentation_level = 0)
    {
        fn(this, indentation_level);
        fn.push(this);
        for(Node *ch = first_child(); ch; ch = ch->next_sibling())
        {
            ch->visit(fn, indentation_level + 1);
        }
        fn.pop(this);
    }

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

    Node & operator[] (size_t i) const
    {
        Node *c = child(i);
        if( ! c) { RymlCallbacks::error("could not find node");}
        return *c;
    }

    Node & operator[] (cspan const& name) const
    {
        C4_ASSERT(m_type == TYPE_DOC || m_type == TYPE_MAP);
        Node *c = find_child(name);
        if( ! c) { RymlCallbacks::error("could not find node"); }
        return *c;
    }

private:

    Node * prev_node() const;
    Node * next_node() const;

};


const char* type_str(Node const& p)
{
    switch(p.type())
    {
#define _c4_case(which) case TYPE_##which: return #which;
    _c4_case(NONE)
    _c4_case(ROOT)
    _c4_case(DOC)
    _c4_case(VAL)
    _c4_case(SEQ)
    _c4_case(MAP)
#undef _c4_case
    default: return "(unknown?)";
    }
}

void show_children(Node const& p)
{
    printf("--------\n%zd children for %p(%s):\n", p.num_children(), (void*)&p, type_str(p));
    for(Node *n = p.first_child(); n; n = n->next_sibling())
    {
        printf("  %p(%s) %.*s", (void*)n, type_str(*n), (int)n->name().len, n->name().str);
        if(n->type() == TYPE_VAL)
        {
            printf(": %.*s", (int)n->val().len, n->val().str);
        }
        printf("\n");
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class Tree
{
private:

    Node * m_buf;
    size_t m_num;

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

    void reserve(size_t sz);

    void clear();

public:

    size_t id(Node const* n)
    {
        if( ! n) return NONE;
        C4_ASSERT(n >= m_buf && n < m_buf + m_num);
        return n - m_buf;
    }
    size_t id(Node const* n) const
    {
        if( ! n) return NONE;
        C4_ASSERT(n >= m_buf && n < m_buf + m_num);
        return n - m_buf;
    }

    Node *get(size_t i)
    {
        if(i == NONE) return nullptr;
        C4_ASSERT(i >= 0 && i < m_num);
        return m_buf + i;
    }
    Node const *get(size_t i) const
    {
        if(i == NONE) return nullptr;
        C4_ASSERT(i >= 0 && i < m_num);
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
        n->m_type = TYPE_ROOT;

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
        C4_ASSERT( ! after || after->m_type == TYPE_DOC);
        C4_ASSERT( ! after || get(after->m_parent) == _stack_top());
        C4_ASSERT(_stack_top() && _stack_top()->m_type == TYPE_ROOT);
        return _stack_push({}, TYPE_DOC, after);
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
        n->m_type = TYPE_VAL;
        n->m_name = name;
        n->m_val  = val;

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
        n->m_type = TYPE_NONE;
        set_parent(_stack_top(), n, after);
        return n;
    }

    /** place the new node after "after" */
    Node *begin_map(cspan const& name, Node *after = nullptr)
    {
        return _stack_push(name, TYPE_MAP, after);
    }
    Node *end_map()
    {
        return _stack_pop();
    }
    void make_map(Node *node, Node *after = nullptr)
    {
        C4_ASSERT( ! node->has_children());
        node->m_type = TYPE_MAP;
        set_parent(_stack_top(), node, after);
        _stack_push(node);
    }

    /** place the new node after "after" */
    Node *begin_seq(cspan const& name, Node *after = nullptr)
    {
        return _stack_push(name, TYPE_SEQ, after);
    }
    Node *end_seq()
    {
        return _stack_pop();
    }
    void make_seq(Node *node, Node *after = nullptr)
    {
        C4_ASSERT( ! node->has_children());
        node->m_type = TYPE_SEQ;
        set_parent(_stack_top(), node, after);
        _stack_push(node);
    }

    bool stack_top_is_type(NodeType_e type)
    {
        return _stack_top()->m_type == type;
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

    Node      * root()       { C4_ASSERT(m_num > 0); Node *n = m_buf; C4_ASSERT(n->type() == TYPE_ROOT); return n; }
    Node const* root() const { C4_ASSERT(m_num > 0); Node *n = m_buf; C4_ASSERT(n->type() == TYPE_ROOT); return n; }

    Node      * first_doc()         { Node *n = root()->child(0); C4_ASSERT(n && n->type() == TYPE_DOC); return n; }
    Node const* first_doc() const   { Node *n = root()->child(0); C4_ASSERT(n && n->type() == TYPE_DOC); return n; }
    Node      * doc(size_t i)       { Node *n = root()->child(i); C4_ASSERT(n && n->type() == TYPE_DOC); return n; }
    Node const* doc(size_t i) const { Node *n = root()->child(i); C4_ASSERT(n && n->type() == TYPE_DOC); return n; }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

namespace detail {
class Event;
} // detail

class detail::Event
{
public:
    yaml_event_t m_event;
public:
    Event() {}
    ~Event()
    {
        yaml_event_delete(&m_event);
    }
};

class Parser
{
public:

    yaml_parser_t m_parser;

    const char *  m_input;
    size_t        m_length;

    Node *        m_load_root;

private:

    cspan get_scalar_val(detail::Event const &ev) const
    {
        // the memory in data.scalar is allocated anew, so don't do this
        //auto const& scalar = e.m_event.data.scalar;
        //cspan val((const char*)scalar.value, scalar.length);
        //return val;
        // ... but the event tells us where in the string the value is
        auto const& e = ev.m_event;
        size_t len = e.end_mark.index - e.start_mark.index;
        cspan val(m_input + e.start_mark.index, len);
        return val;
    }

public:

    Parser()
    {
        memset(this, 0, sizeof(*this));
        yaml_parser_initialize(&m_parser);
    }

    ~Parser()
    {
        yaml_parser_delete(&m_parser);
        memset(&m_parser, 0, sizeof(decltype(m_parser)));
    }

    Parser(Parser const&) = delete;
    Parser& operator= (Parser const&) = delete;

    Parser(Parser &&that) = default;
    Parser& operator= (Parser &&that) = default;

    void set_load_root(Node *r) { m_load_root = r; }

    template< size_t N >
    void parse(Tree *s, const char (&input)[N])
    {
        parse(s, &input[0], N-1);
    }
    void parse(Tree *s, cspan const& sp)
    {
        parse(s, sp.str, sp.len);
    }
    void parse(Tree *s, const char* input, size_t length)
    {
        m_input = input;
        m_length = length;
        yaml_parser_set_input_string(&m_parser, (const unsigned char*)input, length);
        _do_parse(s);
    }

    void _do_parse(Tree *s)
    {
        bool done = false;
        bool doc_had_scalars = false;
        cspan prev_scalar;
        while( ! done)
        {
            detail::Event ev;
            if( ! yaml_parser_parse(&m_parser, &ev.m_event))
            {
                _handle_error();
                break;
            }

            cspan val = get_scalar_val(ev);
            switch(ev.m_event.type)
            {
#define _c4_handle_case(_ev) \
case _ev:                   \
    printf(#_ev " prev=%.*s val=%.*s\n",    \
           (int)prev_scalar.len, prev_scalar.str,   \
           (int)val.len, val.str);

            _c4_handle_case(YAML_MAPPING_START_EVENT)
                if(( ! s->stack_top_is_type(TYPE_DOC) || doc_had_scalars)
                   &&
                   ( ! m_load_root))
                {
                    C4_ASSERT( ! prev_scalar.empty());
                    s->begin_map(prev_scalar, s->top_last());
                    prev_scalar.clear();
                }
                break;
            _c4_handle_case(YAML_MAPPING_END_EVENT)
                if( ! s->stack_top_is_type(TYPE_DOC) && ! m_load_root)
                {
                    s->end_map();
                }
                break;

            _c4_handle_case(YAML_SEQUENCE_START_EVENT)
                C4_ASSERT( ! prev_scalar.empty());
                s->begin_seq(prev_scalar, s->top_last());
                break;
            _c4_handle_case(YAML_SEQUENCE_END_EVENT)
                s->end_seq();
                break;

            _c4_handle_case(YAML_SCALAR_EVENT)
                if(s->stack_top_is_type(TYPE_SEQ))
                {
                    s->add_val({}, val, s->top_last());
                    prev_scalar.clear();
                }
                else
                {
                    if( ! prev_scalar.empty())
                    {
                        s->add_val(prev_scalar, val, s->top_last());
                        prev_scalar.clear();
                    }
                    else
                    {
                        prev_scalar = val;
                    }
                }
                doc_had_scalars = true;
                break;

            _c4_handle_case(YAML_DOCUMENT_START_EVENT)
                if( ! m_load_root)
                {
                    s->begin_doc(s->top_last());
                    doc_had_scalars = false;
                }
                break;
            _c4_handle_case(YAML_DOCUMENT_END_EVENT)
                if( ! m_load_root)
                {
                    s->end_doc();
                }
                break;

            _c4_handle_case(YAML_STREAM_START_EVENT)
                s->begin_stream();
                break;
            _c4_handle_case(YAML_STREAM_END_EVENT)
                s->end_stream();
                done = true;
                break;

            _c4_handle_case(YAML_ALIAS_EVENT)
                C4_ASSERT(false && "YAML_ALIAS_EVENT not implemented");
                break;
#undef _c4_handle_case
            default:
                break;
            };
        }
    }

    void _handle_error()
    {
        Location problem_loc, context_loc;
        if(m_parser.problem)
        {
            problem_loc = Location(m_parser.problem, m_parser.problem_mark);
        }
        if(m_parser.context)
        {
            context_loc = Location(m_parser.context, m_parser.context_mark);
        }

        switch(m_parser.error)
        {

        case YAML_MEMORY_ERROR:
            RymlCallbacks::error("Memory error: Not enough memory for parsing");
            break;

        case YAML_READER_ERROR:
            if (m_parser.problem_value != -1)
            {
                char buf[32];
                int ret = snprintf(buf, sizeof(buf), "Reader error: #%X", m_parser.problem_value);
                RymlCallbacks::error(buf, ret, &problem_loc);
            }
            else
            {
                RymlCallbacks::error("Reader error", &problem_loc);
            }
            break;

        case YAML_SCANNER_ERROR:
            RymlCallbacks::error("Scanner error", &context_loc, &problem_loc);
            break;

        case YAML_PARSER_ERROR:
            RymlCallbacks::error("Parser error", &context_loc, &problem_loc);
            break;

        default:
            /* Couldn't happen. */
            RymlCallbacks::error("Internal error");
            break;
        };
    }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

namespace detail {
class Emitter;
}  // detail

class detail::Emitter
{
public:

    span m_span;
    size_t m_pos;

    FILE *m_file;

    friend struct Visitor;

public:

    Emitter(FILE *f = nullptr) : m_span(), m_pos(0), m_file(f ? f : stdout) {}
    Emitter(span const& sp) : m_span(sp), m_pos(0), m_file(nullptr) {}

    size_t tell() const { return m_pos; }
    void   seek(size_t p) { m_pos = p; }

    size_t visit(Node *root)
    {
        size_t b = m_pos;
        Visitor v{this};
        root->visit(v);
        return m_pos - b;
    }

    struct Visitor
    {
        Emitter *this_;
        void push(Node const *n) {}
        void pop(Node const *n) {}
        void operator()(Node const *n, size_t ilevel)
        {
            switch(n->type())
            {
            case TYPE_ROOT:
                break;
            case TYPE_DOC:
                break;
            case TYPE_MAP:
            case TYPE_SEQ:
                this_->_writeind(ilevel);
                this_->_writek(n->name());
                break;
            case TYPE_VAL:
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
            case TYPE_NONE:
            default:
                break;
            }
        }
    };

#define _c4sargs ret.str, ret.len
#define _c4kargs (int)k.len, k.str
#define _c4vargs (int)v.len, v.str

    void _writeind(size_t level)
    {
        size_t num = 2 * level; // 2 spaces per indentation level
        if(m_file)
        {
            m_pos += fprintf(m_file, "%*s", (int)num, "");
        }
        else
        {
            auto ret = _has_room(num);
            if( ! ret.empty())
            {
                m_pos += snprintf(_c4sargs, "%*s", (int)num, "");
            }
        }

    }

    void _writek(cspan const& k)
    {
        if(m_file)
        {
            m_pos += fprintf(m_file, "%.*s:\n", _c4kargs);
        }
        else
        {
            auto ret = _has_room(k.len + 2);
            if( ! ret.empty())
            {
                m_pos += snprintf(_c4sargs, "%.*s:\n", _c4kargs);
            }
        }
    }

    void _writev(cspan const& v)
    {
        if(m_file)
        {
            m_pos += fprintf(m_file, "- %.*s\n", _c4vargs);
        }
        else
        {
            auto ret = _has_room(2 + v.len + 1);
            if( ! ret.empty())
            {
                m_pos += snprintf(_c4sargs, "- %.*s\n", _c4vargs);
            }
        }
    }

    void _writekv(cspan const& k, cspan const& v)
    {
        if(m_file)
        {
            m_pos += fprintf(m_file, "%.*s: %.*s\n", _c4kargs, _c4vargs);
        }
        else
        {
            auto ret = _has_room(k.len + 2 + v.len + 1);
            if( ! ret.empty())
            {
                m_pos += snprintf(_c4sargs, "%.*s: %.*s\n", _c4kargs, _c4vargs);
            }
        }
    }

#undef _c4spargs
#undef _c4kargs
#undef _c4vargs

    span _has_room(size_t more)
    {
        bool ok = m_pos + more < m_span.len;
        span ret = ok ? m_span.subspan(m_pos) : span();
        return ret;
    }
};

//-----------------------------------------------------------------------------
/** emit YAML to the given file */
inline size_t emit(Node *n, FILE *f = nullptr)
{
    detail::Emitter em(f);
    size_t num = em.visit(n);
    return num;
}

/** emit YAML to the given buffer. Return a span of the emitted YAML.
 * If the given buffer has insufficient space, the returned span will
 * be null and its size will be the needed space. */
inline span emit_unchecked(Node *n, span const& sp)
{
    detail::Emitter em(sp);
    size_t num = em.visit(n);
    span result = sp.subspan(0, num);
    if(num >= sp.len)
    {
        result.str = nullptr;
    }
    return result;
}

/** emit YAML to the given buffer. Raise an error if the space in the
 * buffer is insufficient. */
inline span emit(Node *root, span const& sp)
{
    span ret = emit_unchecked(root, sp);
    C4_ASSERT(ret.len <= sp.len);
    if(ret.len >= sp.len)
    {
        RymlCallbacks::error("not enough space in the given span");
    }
    return ret;
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
    } State_e;

    struct LineContents
    {
        cspan  full;        ///< the full line, including newlines on the right
        cspan  stripped;    ///< the stripped line, excluding newlines on the right
        cspan  rem;         ///< the stripped line remainder; initially starts at the first non-space character
        size_t indentation; ///< the number of columns with space on the left

        void reset(cspan const& full_, cspan const& stripped_)
        {
            full = full_;
            stripped = stripped_;
            rem = stripped_;
            // find the first column where the character is not a space
            indentation = full.first_not_of(' ');
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
        }

        void line_scanned(cspan const& full, cspan const& stripped)
        {
            line_contents.reset(full, stripped);
            printf("%3zd: '%.*s'\n", pos.line-1, _c4prsp(line_contents.stripped));
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

    Tree parse(const char *file, cspan const& buf)
    {
        Tree t;
        t.add_root();
        parse(file, buf, &t);
        return t;
    }

    void parse(const char *file, cspan const& buf, Tree *t)
    {
        parse(file, buf, t->root());
    }

    void parse(const char *file, cspan const& buf, Node *root);

private:

    void _reset();

    bool  _finished_file() const;

    void  _scan_line();
    void  _next_line() { m_state.line_ended(); }

    cspan _scan_scalar();
    cspan _scan_quoted_scalar(const char q);
    cspan _filter_quoted_scalar(cspan const& s, const char q);
    cspan _scan_block();
    cspan _filter_raw_block(cspan const& block, BlockStyle_e style, BlockChomp_e chomp, size_t indentation);

    void  _handle_line(cspan rem);
    int   _handle_indentation();
    int   _get_indentation_jump();

    bool  _handle_unk(cspan rem);
    bool  _handle_map(cspan rem);
    bool  _handle_seq(cspan rem);
    bool  _handle_scalar(cspan rem);
    bool  _handle_top(cspan rem);
    bool  _handle_anchors_and_refs(cspan rem);
    bool  _handle_types(cspan rem);

    void  _push_level(bool explicit_flow_chars = false);
    void  _pop_level();

    void  _start_map(bool as_child=true);
    void  _stop_map();

    void  _start_seq(bool as_child=true);
    void  _stop_seq();

    void  _append_val(cspan const& val);
    void  _append_key_val(cspan const& val);
    void  _toggle_key_val();

    void  _store_scalar(cspan const& s);
    cspan _consume_scalar();
    void  _move_scalar_from_top();

private:

    static bool _read_decimal(cspan const& str, size_t *decimal);

    static size_t _find_matching(const char sq_or_dq, cspan const& s);

    static inline bool _any_of(const char c, const char (&chars)[1])
    {
        RymlCallbacks::error("WTF???");
        return false;
    }
    static inline bool _any_of(const char c, const char (&chars)[2])
    {
        return (c == chars[0]);
    }
    static inline bool _any_of(const char c, const char (&chars)[3])
    {
        return (c == chars[0]) || (c == chars[1]);
    }
    static inline bool _any_of(const char c, const char (&chars)[4])
    {
        return (c == chars[0]) || (c == chars[1]) || (c == chars[2]);
    }
    static inline bool _any_of(const char c, const char (&chars)[5])
    {
        return (c == chars[0]) || (c == chars[1]) || (c == chars[2]) || (c == chars[3]);
    }
    template< size_t N >
    static inline bool _any_of(const char c, const char (&chars)[N])
    {
        for(size_t i = 0; i < N; ++i)
        {
            if(c == chars[i])
            {
                return true;
            }
        }
        return false;
    }
};

} // namespace yml
} // namespace c4

#include <ryml/ryml.def.hpp>

#endif // _C4_RYML_HPP_
