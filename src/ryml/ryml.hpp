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
        return (str == that.str || (strncmp(str, that.str, len) == 0));
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

struct LineCol
{
    size_t offset, line, col;
};

struct Location : public LineCol
{
    const char *name;
    operator bool () const { return name != nullptr || line != 0 || offset != 0; }

    Location() { memset(this, 0, sizeof(*this)); }
    Location(const char *n, size_t b, size_t l, size_t c) : LineCol{b, l, c}, name(n) {}
    Location(const char *n, yaml_mark_t m) : LineCol{m.index, m.line+1, m.column+1}, name(n) {}
};

struct Region
{
   const char *name;
   LineCol begin;
   LineCol end;
};

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

    static void error(const char* msg, size_t length, Location *loc1 = nullptr, Location *loc2 = nullptr)
    {
        fprintf(stderr, "%.*s\n", (int)length, msg);
        if(loc1 && *loc1)
        {
            fprintf(stderr, "    : %s at %zd:%zd (offset %zd)\n", loc1->name, loc1->line, loc1->col, loc1->offset);
        }
        if(loc2 && *loc2)
        {
            fprintf(stderr, "    : %s at %zd:%zd (offset %zd)\n", loc2->name, loc1->line, loc2->col, loc2->offset);
        }
        abort();
    }
    template< size_t N >
    static void error(char const (&msg)[N], Location *loc1 = nullptr, Location *loc2 = nullptr)
    {
        error(&msg[0], N-1, loc1, loc2);
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

    size_t id() const;

    NodeType_e type() const { return m_type; }
    cspan const& name() const { return m_name; }
    cspan const& val() const { C4_ASSERT(m_type == TYPE_VAL); return m_val; }

    Node * parent() const;

    size_t num_children() const;
    Node * child(size_t i) const;
    Node * first_child() const;
    Node * last_child() const;
    Node * find_child(cspan const& name) const;

    Node & operator[] (size_t i) const
    {
        Node *c = child(i);
        if( ! c) { RymlCallbacks::error("could not find node");}
        return *c;
    }

    Node & operator[] (cspan const& name) const
    {
        Node *c = find_child(name);
        if( ! c) { RymlCallbacks::error("could not find node");}
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

    size_t id(Node const* n)
    {
        if(!n) return NONE;
        C4_ASSERT(n >= m_buf && n < m_buf + m_num);
        return n - m_buf;
    }
    size_t id(Node const* n) const
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

    void set_parent(Node *parent, Node *child, Node *prev_sibling, Node *next_sibling);

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
                    Node *p = _stack_top(s);
                    s->set_parent(p, n, p->last_child(), nullptr);
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
                    Node *p = _stack_top(s);
                    s->set_parent(p, n, p->last_child(), nullptr);
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
                    Node *p = _stack_top(s);
                    s->set_parent(p, n, p->last_child(), nullptr);
                    prev_scalar.clear();
                }
                else
                {
                    if(prev_scalar)
                    {
                        Node *n = s->add_val(prev_scalar, val);
                        Node *p = _stack_top(s);
                        s->set_parent(p, n, p->last_child(), nullptr);
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
                    Node *p = _stack_top(s);
                    s->set_parent(p, n, p->last_child(), nullptr);
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


} // namespace yml
} // namespace c4

#include <ryml/ryml.def.hpp>

#endif // _C4_RYML_HPP_
