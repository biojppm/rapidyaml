#ifndef _C4_RYML_HPP_
#define _C4_RYML_HPP_

#include <yaml.h>

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
#       define C4_ASSERT(expr) _C4_ASSERT(expr, __FILE__, __LINE__)
#       define _C4_ASSERT(expr, file, line)                             \
        if(!(expr))                                                     \
        {                                                               \
            RymlCallbacks::error(file ":" C4_QUOTE(line) ": Assert failed: " #expr "\n"); \
            abort();                                                    \
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
    union {
        cspan      m_val;
        struct
        {
            size_t m_first_child;
            size_t m_last_child;
        };
    };

public:

    cspan const& name() const { return m_name; }

    cspan const& val() const { C4_ASSERT(m_type == TYPE_VAL); return m_name; }

    Node * parent() const;

    Node * first_child() const;
    Node * last_child() const;
    Node * find_child(cspan const& name) const;
    Node * prev_child(Node *child) const { if(!child) return nullptr; return child->prev_sibling(); }
    Node * next_child(Node *child) const { if(!child) return nullptr; return child->next_sibling(); }

    Node * first_sibling() const;
    Node * last_sibling() const;
    Node * find_sibling(cspan const& name) const;

    Node * prev_sibling() const
    {
        Node *n = prev_node();
        if( ! n) return nullptr;
        if(n->m_parent == m_parent) return n;
        return nullptr;
    }

    Node * next_sibling() const
    {
        Node *n = next_node();
        if( ! n) return nullptr;
        if(n->m_parent == m_parent) return n;
        return nullptr;
    }

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

    size_t m_free;

public:

    Tree() : m_buf(nullptr), m_num(0), m_head(NONE), m_tail(NONE), m_free(NONE) {}
    Tree(size_t sz) : m_buf(nullptr), m_num(0), m_head(NONE), m_tail(NONE), m_free(NONE)
    {
        reserve(sz);
    }
    ~Tree()
    {
        if(m_buf)
        {
            RymlCallbacks::free(m_buf, m_num * sizeof(Node));
        }
    }

    void reserve(size_t sz)
    {
        if(sz <= m_num) return;
        Node *buf = (Node*)RymlCallbacks::allocate(sz * sizeof(Node), nullptr);
        if(m_buf)
        {
            memcpy(buf, m_buf, m_num * sizeof(Node));
            RymlCallbacks::free(m_buf, m_num * sizeof(Node));
        }
        if(m_free == NONE)
        {
            m_free = m_num;
        }
        m_num = sz;
        m_buf = buf;
        clear_range(m_num, sz - m_num);
    }

    void clear()
    {
        clear_range(0, m_num);
        m_head = NONE;
        m_tail = NONE;
        m_free = 0;
    }
    void clear_range(size_t first, size_t num)
    {
        if(num == 0) return; // prevent overflow when subtracting
        C4_ASSERT(first >= 0 && first + num <= m_num);
        memset(m_buf + first, 0, num * sizeof(Node));
        for(size_t i = first, e = first + num; i < e; ++i)
        {
            m_buf[i].m_prev = i - 1;
            m_buf[i].m_next = i + 1;
        }
        m_buf[first + num - 1].m_next = NONE;
    }

    Node *claim(Node *after)
    {
        C4_ASSERT(after == nullptr || (m_buf <= after && (after <= m_buf + m_num)));
        size_t last = after ? after - m_buf : m_tail;
        Node *n = claim(last, NONE);
        return n;
    }
    Node *claim(size_t prev, size_t next)
    {
        C4_ASSERT(prev == NONE || prev >= 0 && prev < m_num);
        C4_ASSERT(next == NONE || next >= 0 && next < m_num);
        if(m_free == NONE)
        {
            reserve(2 * m_num);
        }
        size_t f = m_free;
        Node *n = m_buf + f;
        m_free = n->m_next;
        n->m_s = this;
        n->m_prev = prev;
        n->m_next = next;
        if(prev == NONE) m_head = f;
        else
        {
            n->m_parent = (m_buf + prev)->m_parent;
        }
        if(next == NONE) m_tail = f;
        else
        {
            n->m_parent = (m_buf + next)->m_parent;
        }
        if(prev != NONE && next != NONE) C4_ASSERT((m_buf + prev)->m_parent == (m_buf + next)->m_parent);
        return n;
    }
    void free(size_t i)
    {
        C4_ASSERT(i >= 0 && i < m_num);
        m_buf[m_free].m_prev = i;
        m_buf[i].m_next = m_free;
        m_free = i;
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

    Node       *head()       { return m_buf + m_head; }
    Node const *head() const { return m_buf + m_head; }

    Node       *tail()       { return m_buf + m_tail; }
    Node const *tail() const { return m_buf + m_tail; }

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

};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Node * Node::prev_node() const
{
    return m_s->get(m_prev);
}

Node * Node::next_node() const
{
    return m_s->get(m_next);
}

Node * Node::parent() const
{
    return m_s->get(m_parent);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class Parser
{
public:

    yaml_parser_t m_parser;

    const char *  m_input;
    size_t        m_length;

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

    Parser() : m_parser()
    {
        memset(&m_parser, 0, sizeof(decltype(m_parser)));
        yaml_parser_initialize(&m_parser);
    }
    ~Parser()
    {
        yaml_parser_delete(&m_parser);
        memset(&m_parser, 0, sizeof(decltype(m_parser)));
    }

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
        cspan prev_scalar;
        Node *doc = nullptr;
        Node *seq = nullptr;
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
                if(s->tail()->m_type != TYPE_DOC)
                {
                    C4_ASSERT(prev_scalar);
                    s->begin_map(prev_scalar);
                    prev_scalar.clear();
                    doc = nullptr;
                }
                else
                {
                    doc = s->tail();
                }
                break;
            _c4_handle_case(YAML_MAPPING_END_EVENT)
                if( ! doc)
                {
                    s->end_map();
                }
                break;

            _c4_handle_case(YAML_SEQUENCE_START_EVENT)
                C4_ASSERT(prev_scalar);
                seq = s->begin_seq(prev_scalar);
                break;
            _c4_handle_case(YAML_SEQUENCE_END_EVENT)
                s->end_seq();
                seq = nullptr;
                break;

            _c4_handle_case(YAML_SCALAR_EVENT)
                if( ! seq)
                {
                    if(prev_scalar)
                    {
                        s->add_val(prev_scalar, val);
                        prev_scalar.clear();
                    }
                    else
                    {
                        prev_scalar = val;
                    }
                }
                else
                {
                    s->add_val({}, val);
                    prev_scalar.clear();
                }
                break;

            _c4_handle_case(YAML_STREAM_START_EVENT)
                s->clear();
                break;
            _c4_handle_case(YAML_STREAM_END_EVENT)
                done = true;
                break;

            _c4_handle_case(YAML_DOCUMENT_START_EVENT)
                s->begin_doc();
                break;
            _c4_handle_case(YAML_DOCUMENT_END_EVENT)
                s->end_doc();
                break;

            _c4_handle_case(YAML_ALIAS_EVENT)
                C4_ASSERT(false && "YAML_ALIAS_EVENT not implemented");
                break;

            default:
                break;
            };
        }
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

#endif // _C4_RYML_HPP_
