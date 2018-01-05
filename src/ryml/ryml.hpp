#ifndef _C4_RYML_HPP_
#define _C4_RYML_HPP_

#include <yaml.h>

#define RYML_INLINE inline

//#define RYML_NO_DEFAULT_CALLBACKS
#ifndef RYML_NO_DEFAULT_CALLBACKS
#   include <stdlib.h>
#   include <stdio.h>
#endif // RYML_NO_DEFAULT_CALLBACKS

#ifndef C4_ASSERT
#   ifdef NDEBUG
#       define C4_ASSERT(expr) (void)(0)
#   else
#       include <assert.h>
#       define C4_QUOTE(x) #x
#       define C4_ASSERT(expr) _C4_ASSERT((expr), __FILE__, __LINE__)
#       define _C4_ASSERT(expr, file, line)                             \
    {                                                                   \
        if( ! (expr))                                                   \
        {                                                               \
            c4::yml::RymlCallbacks::error(file ":" C4_QUOTE(line) ": Assert failed: " #expr "\n"); \
            abort();                                                    \
        }                                                               \
    }
#   endif
#endif

namespace c4 {
namespace yml {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template< class C >
class basic_span
{
public:

    C *str;
    size_t len;

public:

    basic_span() : str(nullptr), len(0) {}
    basic_span(C *s_) : str(s_), len(strlen(s_)) {}
    basic_span(C *s_, size_t len_) : str(s_), len(len_) {}
    template< size_t N > basic_span(C *s_[N]) : str(s_), len(N-1) {}

    void clear() { str = nullptr; len = 0; }

    operator bool () const { return str != nullptr && len > 0; }

    bool operator== (basic_span const& that) const
    {
        if(len != that.len) return false;
        return (str == that.str || strncmp(str, that.str, len));
    }

};

using span = basic_span< char >;
using cspan = basic_span< const char >;

template< class OStream, class C >
OStream& operator<< (OStream& s, basic_span< C > const& sp)
{
    s.write(sp.str, sp.len);
    return s;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
class Location
{
    const char *name;
    size_t line;
    size_t column;
    operator bool () const { return name != nullptr && line != 0; }

    Location(const char *n, size_t l, size_t c) : name(n), line(l), column(c) {}
};
*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef RYML_NO_DEFAULT_CALLBACKS
struct RymlCallbacks
{
    static void* allocate(size_t length, void * /*hint*/)
    {
        void *mem = ::malloc(length);
        if(mem == nullptr)
        {
            const char msg[] = "could not allocate memory";
            error(msg, sizeof(msg));
        }
        return mem;
    }

    static void free(void *mem, size_t /*length*/)
    {
        ::free(mem);
    }

    static void error(const char* msg, size_t length)
    {
        fprintf(stderr, "%.*s", (int)length, msg);
    }
    template< size_t N >
    static void error(char const (&msg)[N])
    {
        error(&msg[0], N-1);
    }
};
#endif // RYML_NO_DEFAULT_CALLBACKS




//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef enum {
    TYPE_NONE,
    TYPE_ROOT,
    TYPE_DOC,
    TYPE_VAL,
    TYPE_SEQ,
    TYPE_MAP
} NodeType_e;


class Tree;

enum : size_t { NONE = size_t(-1) };


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class Node
{
public:
    mutable Tree* m_s;
    NodeType_e m_type;
    cspan      m_name;
    size_t     m_prev;
    size_t     m_next;
    size_t     m_parent;
    struct children
    {
        size_t first;
        size_t last;
    };
    union {
        cspan      m_val;
        children   m_children;
    };

public:

    NodeType_e type() const { return m_type; }
    cspan const& name() const { return m_name; }
    cspan const& val() const { C4_ASSERT(m_type == TYPE_VAL); return m_name; }

    Node * parent() const;

    size_t num_children() const;
    Node * child(size_t i) const;
    Node * first_child() const;
    Node * last_child() const;
    Node * find_child(cspan const& name) const;

    Node * prev_child(Node *child) const { if(!child) return nullptr; return child->prev_sibling(); }
    Node * next_child(Node *child) const { if(!child) return nullptr; return child->next_sibling(); }

    Node & operator[] (size_t i) const
    {
        Node *c = child(i);
        if( ! c)
        {
            RymlCallbacks::error("could not find node");
        }
        return *c;
    }

    Node & operator[] (cspan const& name) const
    {
        Node *c = find_child(name);
        if( ! c)
        {
            RymlCallbacks::error("could not find node");
        }
        return *c;
    }

    size_t num_siblings() const;
    Node * sibling(size_t i) const;
    Node * first_sibling() const;
    Node * last_sibling() const;
    Node * find_sibling(cspan const& name) const;

    Node * prev_sibling() const;
    Node * next_sibling() const;

    Node * prev_node() const;
    Node * next_node() const;

};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class Tree
{
public:

    Node * m_buf;
    size_t m_num;

    size_t m_head;
    size_t m_tail;

    size_t m_free_head;
    size_t m_free_tail;

public:

    Tree();
    Tree(size_t sz);
    ~Tree();

    void reserve(size_t sz);

    void clear();
    void clear_range(size_t first, size_t num);

    Node *claim(Node *after);
    Node *claim(size_t prev, size_t next);

    void free(size_t i);

    size_t id(Node* n)
    {
        if(!n) return NONE;
        C4_ASSERT(n >= m_buf && n < m_buf + m_num);
        return n - m_buf;
    }
    size_t id(Node* n) const
    {
        if(!n) return NONE;
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

    Node      * head()       { return m_buf + m_head; }
    Node const* head() const { return m_buf + m_head; }

    Node      * tail()       { return m_buf + m_tail; }
    Node const* tail() const { return m_buf + m_tail; }

    void set_parent(Node *child, Node *parent);

    Node *begin_stream()
    {
        Node *n = claim(nullptr);
        n->m_type = TYPE_ROOT;
        return n;
    }
    Node *end_stream()
    {
        return nullptr;
    }

    Node *begin_doc(Node *after = nullptr)
    {
        C4_ASSERT(after == nullptr || after->m_parent == NONE);
        Node *n = claim(after);
        n->m_type = TYPE_DOC;
        return n;
    }
    Node *end_doc()
    {
        return nullptr;
    }

    Node *add_val(cspan const& name, cspan const& val, Node *after = nullptr)
    {
        Node *n = claim(after);
        n->m_type = TYPE_VAL;
        n->m_name = name;
        n->m_val  = val;
        return n;
    }

    Node *begin_map(cspan const& name, Node *after = nullptr)
    {
        Node *n = claim(after);
        n->m_type = TYPE_MAP;
        n->m_name = name;
        return n;
    }
    Node *end_map()
    {
        return nullptr;
    }

    Node *begin_seq(cspan const& name, Node *after = nullptr)
    {
        Node *n = claim(after);
        n->m_type = TYPE_SEQ;
        n->m_name = name;
        return n;
    }
    Node *end_seq()
    {
        return nullptr;
    }

    Node      * root()       { C4_ASSERT(m_num > 0); Node *n = m_buf; C4_ASSERT(n->m_type == TYPE_ROOT); return n; }
    Node const* root() const { C4_ASSERT(m_num > 0); Node *n = m_buf; C4_ASSERT(n->m_type == TYPE_ROOT); return n; }

    Node      * first_doc()         { Node *n = root()->child(0); C4_ASSERT(n && n->type() == TYPE_DOC); return n; }
    Node const* first_doc() const   { Node *n = root()->child(0); C4_ASSERT(n && n->type() == TYPE_DOC); return n; }
    Node      * doc(size_t i)       { Node *n = root()->child(i); C4_ASSERT(n && n->type() == TYPE_DOC); return n; }
    Node const* doc(size_t i) const { Node *n = root()->child(i); C4_ASSERT(n && n->type() == TYPE_DOC); return n; }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

namespace detail {
template< class T > class stack;
} // namespace detail

/** A lightweight stack. This avoids a dependency on std. */
template< class T >
class detail::stack
{
private:

    T *    m_stack;
    size_t m_pos;
    size_t m_size;

public:

    stack() : m_stack(nullptr), m_pos(0), m_size(0)
    {
    }
    ~stack()
    {
        if(m_stack)
        {
            RymlCallbacks::free(m_stack, m_size * sizeof(T));
        }
    }

    stack(stack const& ) = delete;
    stack& operator= (stack const& ) = delete;

    stack(stack &&that)
    {
        memcpy(this, &that, sizeof(*this));
        that.m_stack = nullptr;
    }
    stack& operator= (stack &&that)
    {
        memcpy(this, &that, sizeof(*this));
        that.m_stack = nullptr;
        return *this;
    }

    size_t size() const { return m_pos; }
    size_t empty() const { return m_pos == 0; }

    void push(T n)
    {
        if(m_pos >= m_size)
        {
            size_t sz = 2 * m_size;
            if(sz == 0) sz = 8;
            C4_ASSERT(m_pos < sz);
            T *buf = (T*) RymlCallbacks::allocate(sz * sizeof(T), m_stack);
            memcpy(buf, m_stack, m_pos * sizeof(T));
            RymlCallbacks::free(m_stack, m_size * sizeof(T));
            m_stack = buf;
            m_size = sz;
        }
        m_stack[m_pos] = n;
        m_pos++;
        //printf("stack_push: %zd %zd\n", m_pos, n);
    }

    T pop()
    {
        //printf("stack_pop: %zd %zd\n", m_pos, m_stack[m_pos - 1]);
        C4_ASSERT(m_pos > 0);
        m_pos--;
        T n = m_stack[m_pos];
        return n;
    }

    T peek() const
    {
        C4_ASSERT(m_pos > 0);
        T n = m_stack[m_pos - 1];
        return n;
    }

};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class Parser
{
public:

    yaml_parser_t m_parser;

    const char *  m_input;
    size_t        m_length;

    detail::stack< size_t > m_stack;

private:

    class Event
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

    cspan get_scalar_val(Event const &ev) const
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

    template< size_t N >
    void parse(Tree *s, const char (&input)[N])
    {
        parse(s, &input[0], N-1);
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

            Event ev;
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
                if( ! _stack_top_is_type(s, TYPE_DOC) || doc_had_scalars)
                {
                    C4_ASSERT(prev_scalar == true);
                    Node *n = s->begin_map(prev_scalar);
                    s->set_parent(n, _stack_top(s));
                    prev_scalar.clear();
                    m_stack.push(s->id(n));
                }
                break;
            _c4_handle_case(YAML_MAPPING_END_EVENT)
                if( ! _stack_top_is_type(s, TYPE_DOC))
                {
                    s->end_map();
                    m_stack.pop();
                }
                break;

            _c4_handle_case(YAML_SEQUENCE_START_EVENT)
                {
                    C4_ASSERT(prev_scalar == true);
                    Node *n = s->begin_seq(prev_scalar);
                    s->set_parent(n, _stack_top(s));
                    m_stack.push(s->id(n));
                }
                break;
            _c4_handle_case(YAML_SEQUENCE_END_EVENT)
                s->end_seq();
                m_stack.pop();
                break;

            _c4_handle_case(YAML_SCALAR_EVENT)
                if(_stack_top_is_type(s, TYPE_SEQ))
                {
                    Node *n = s->add_val({}, val);
                    s->set_parent(n, _stack_top(s));
                    prev_scalar.clear();
                }
                else
                {
                    if(prev_scalar)
                    {
                        Node *n = s->add_val(prev_scalar, val);
                        s->set_parent(n, _stack_top(s));
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
                {
                    Node *n = s->begin_doc();
                    s->set_parent(n, _stack_top(s));
                    m_stack.push(s->id(n));
                    doc_had_scalars = false;
                }
                break;
            _c4_handle_case(YAML_DOCUMENT_END_EVENT)
                s->end_doc();
                m_stack.pop();
                break;

            _c4_handle_case(YAML_STREAM_START_EVENT)
                {
                    s->clear();
                    Node *n = s->begin_stream();
                    m_stack.push(s->id(n));
                }
                break;
            _c4_handle_case(YAML_STREAM_END_EVENT)
                s->end_stream();
                m_stack.pop();
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

    Node * _stack_top(Tree *s) const
    {
        C4_ASSERT( ! m_stack.empty());
        size_t id = m_stack.peek();
        Node *n = s->get(id);
        return n;
    }

    Node * _stack_top_is_type(Tree *s, NodeType_e type) const
    {
        C4_ASSERT( ! m_stack.empty());
        size_t id = m_stack.peek();
        Node *n = s->get(id);
        return n->m_type == type ? n : nullptr;
    }

    void _handle_error()
    {
        switch(m_parser.error)
        {

        case YAML_MEMORY_ERROR:
            fprintf(stderr, "Memory error: Not enough memory for parsing\n");
            break;

        case YAML_READER_ERROR:
            if (m_parser.problem_value != -1)
            {
                fprintf(stderr, "Reader error: %s: #%X at %d\n", m_parser.problem,
                        m_parser.problem_value, (int)m_parser.problem_offset);
            }
            else
            {
                fprintf(stderr, "Reader error: %s at %d\n", m_parser.problem,
                        (int)m_parser.problem_offset);
            }
            break;

        case YAML_SCANNER_ERROR:
            if (m_parser.context)
            {
                fprintf(stderr, "Scanner error: %s at line %d, column %d\n"
                        "%s at line %d, column %d\n", m_parser.context,
                        (int)m_parser.context_mark.line+1, (int)m_parser.context_mark.column+1,
                        m_parser.problem, (int)m_parser.problem_mark.line+1,
                        (int)m_parser.problem_mark.column+1);
            }
            else
            {
                fprintf(stderr, "Scanner error: %s at line %d, column %d\n",
                        m_parser.problem, (int)m_parser.problem_mark.line+1,
                        (int)m_parser.problem_mark.column+1);
            }
            break;

        case YAML_PARSER_ERROR:
            if (m_parser.context)
            {
                fprintf(stderr, "Parser error: %s at line %d, column %d\n"
                        "%s at line %d, column %d\n", m_parser.context,
                        (int)m_parser.context_mark.line+1, (int)m_parser.context_mark.column+1,
                        m_parser.problem, (int)m_parser.problem_mark.line+1,
                        (int)m_parser.problem_mark.column+1);
            }
            else
            {
                fprintf(stderr, "Parser error: %s at line %d, column %d\n",
                        m_parser.problem, (int)m_parser.problem_mark.line+1,
                        (int)m_parser.problem_mark.column+1);
            }
            break;

        default:
            /* Couldn't happen. */
            fprintf(stderr, "Internal error\n");
            break;
        };
    }
};


} // namespace yml
} // namespace c4

#include <ryml/ryml.def.hpp>

#endif // _C4_RYML_HPP_
