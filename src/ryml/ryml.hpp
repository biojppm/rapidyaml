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

class Parser;

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

enum : size_t { npos = size_t(-1) };

template< class C >
class basic_span
{
public:

    C *str;
    size_t len;

    // convert automatically to span of const C
    operator basic_span< const C > () { basic_span< const C > s(str, len); return s; }
    operator bool () const { return has_str(); }

    using iterator = C*;
    using const_iterator = C const*;

public:

    basic_span() : str(nullptr), len(0) {}
    basic_span(C *s_) : str(s_), len(strlen(s_)) {}
    basic_span(C *s_, size_t len_) : str(s_), len(len_) {}
    template< size_t N > basic_span(C *s_[N]) : str(s_), len(N-1) {}

    void clear() { str = nullptr; len = 0; }

    bool has_str() const { return ! empty() && str[0] != '\0'; }
    bool empty() const { return (len == 0 || str == nullptr); }
    size_t size() const { return len; }

    iterator begin() { return str; }
    iterator end  () { return str + len; }

    const_iterator begin() const { return str; }
    const_iterator end  () const { return str + len; }

    inline C      & operator[] (size_t i)       { C4_ASSERT(i >= 0 && i < len); return str[i]; }
    inline C const& operator[] (size_t i) const { C4_ASSERT(i >= 0 && i < len); return str[i]; }

    bool operator== (basic_span const& that) const
    {
        if(len != that.len) return false;
        return (str == that.str || (strncmp(str, that.str, len) == 0));
    }

    basic_span subspan(size_t first, size_t num = npos) const
    {
        size_t rnum = num != npos ? num : len - first;
        C4_ASSERT((first >= 0 && first + rnum <= len) || num == 0);
        return basic_span(str + first, rnum);
    }

    basic_span right_of(size_t pos, bool include_pos = false) const
    {
        if(pos == npos) return subspan(0, 0);
        if( ! include_pos) ++pos;
        return subspan(pos);
    }
    basic_span left_of(size_t pos, bool include_pos = false) const
    {
        if(pos == npos) return *this;
        if( ! include_pos && pos > 0) --pos;
        return subspan(0, pos);
    }

    /** trim left */
    basic_span triml(const C c) const
    {
        return right_of(first_not_of(c));
    }
    /** trim left any of the characters */
    basic_span triml(basic_span< const C > const& chars) const
    {
        return right_of(first_not_of(chars));
    }

    /** trim right */
    basic_span trimr(const C c) const
    {
        return left_of(last_not_of(c), /*include_pos*/true);
    }
    /** trim right any of the characters */
    basic_span trimr(basic_span< const C > const& chars) const
    {
        return left_of(last_not_of(chars), /*include_pos*/true);
    }

    /** trim left and right */
    basic_span trim(const C c) const
    {
        return triml(c).trimr(c);
    }
    /** trim left and right any of the characters */
    basic_span trim(basic_span< const C > const& chars) const
    {
        return triml(chars).trimr(chars);
    }

    inline size_t find(const C c) const
    {
        return first_of(c);
    }
    inline size_t find(basic_span< const C > const& chars) const
    {
        if(len < chars.len) return npos;
        for(size_t i = 0, e = len - chars.len; i < e; ++i)
        {
            bool gotit = true;
            for(size_t j = 0; j < chars.len; ++j)
            {
                if(str[i] != chars[j])
                {
                    gotit = false;
                }
            }
            if(gotit)
            {
                return i;
            }
        }
        return npos;
    }

    inline bool begins_with(const C c) const
    {
        return len > 0 ? str[0] == c : false;
    }
    inline bool begins_with(basic_span< const C > const& pattern) const
    {
        for(size_t i = 0; i < len && i < pattern.len; ++i)
        {
            if(str[i] != pattern[i]) return false;
        }
        return true;
    }

    inline size_t first_of(const C c) const
    {
        for(size_t i = 0; i < len; ++i)
        {
            if(str[i] == c) return i;
        }
        return npos;
    }
    inline size_t first_of(basic_span< const C > const& chars) const
    {
        for(size_t i = 0; i < len; ++i)
        {
            for(size_t j = 0; j < chars.len; ++j)
            {
                if(str[i] == chars[j]) return i;
            }
        }
        return npos;
    }

    inline size_t first_not_of(const C c) const
    {
        for(size_t i = 0; i < len; ++i)
        {
            if(str[i] != c) return i;
        }
        return npos;
    }
    inline size_t first_not_of(basic_span< const C > const& chars) const
    {
        for(size_t i = 0; i < len; ++i)
        {
            for(size_t j = 0; j < chars.len; ++j)
            {
                if(str[i] != chars[j]) return i;
            }
        }
        return npos;
    }

    inline size_t last_of(const C c) const
    {
        for(size_t i = len-1; i != size_t(-1); --i)
        {
            if(str[i] == c) return i;
        }
        return npos;
    }
    inline size_t last_of(basic_span< const C > const& chars) const
    {
        for(size_t i = len-1; i != size_t(-1); --i)
        {
            for(size_t j = 0; j < chars.len; ++j)
            {
                if(str[i] == chars[j]) return i;
            }
        }
        return npos;
    }

    inline size_t last_not_of(const C c) const
    {
        for(size_t i = len-1; i != size_t(-1); --i)
        {
            if(str[i] != c) return i;
        }
        return npos;
    }
    inline size_t last_not_of(basic_span< const C > const& chars) const
    {
        for(size_t i = len-1; i != size_t(-1); --i)
        {
            bool gotit = false;
            for(size_t j = 0; j < chars.len; ++j)
            {
                if(str[i] == chars[j])
                {
                    gotit = true;
                    break;
                }
            }
            if(gotit)
            {
                return i;
            }
        }
        return npos;
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
#define _c4prsp(sp) ((int)(sp).len), (sp).str



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

    stack(stack const& that) : stack()
    {
        reserve(that.m_pos);
        memcpy(m_stack, that.m_stack, sizeof(T) * that.m_pos);
    }
    stack& operator= (stack const& that)
    {
        if(m_stack)
        {
            RymlCallbacks::free(m_stack, m_size * sizeof(T));
        }
        reserve(that.m_pos);
        memcpy(m_stack, that.m_stack, sizeof(T) * that.m_pos);
        return *this;
    }

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

    void reserve(size_t sz)
    {
        if(sz == 0) sz = 8;
        C4_ASSERT(m_pos < sz);
        T *buf = (T*) RymlCallbacks::allocate(sz * sizeof(T), m_stack);
        if(m_stack)
        {
            memcpy(buf, m_stack, m_pos * sizeof(T));
            RymlCallbacks::free(m_stack, m_size * sizeof(T));
        }
        m_stack = buf;
        m_size = sz;
    }
    void push(T n)
    {
        if(m_pos >= m_size)
        {
            reserve(2 * m_size);
        }
        m_stack[m_pos] = n;
        m_pos++;
        //printf("stack_push[%zd]: %zd\n", m_pos, n);
    }

    T pop()
    {
        //printf("stack_pop[%zd]: %zd\n", m_pos, m_stack[m_pos - 1]);
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
    bool   is_map() const { return m_type == TYPE_DOC || m_type == TYPE_MAP; }
    bool   is_seq() const { return m_type == TYPE_ROOT || m_type == TYPE_SEQ; }
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

    //! create and insert a new sibling as a key-value node in a seq entry. insert after "after".
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

    //! create and insert a new child as a key-value node in a seq entry. insert after "after".
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

    void set_load_root(Node *r) { m_load_root_id = r ? r->id() : NONE; }

    void add_root()
    {
        begin_stream();
        end_stream();
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

    /** place the new node after "after" */
    Node *begin_map(cspan const& name, Node *after = nullptr)
    {
        return _stack_push(name, TYPE_MAP, after);
    }
    Node *end_map()
    {
        return _stack_pop();
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
       //STREAM_START, //(encoding)          ///< The stream start.
       //STREAM_END,                         ///< The stream end.
       VERSION_DIRECTIVE, //(major,minor)  ///< The '%YAML' directive.
       TAG_DIRECTIVE, //(handle,prefix)    ///< The '%TAG' directive.
       DOCUMENT_START,                     ///< '---'
       DOCUMENT_END,                       ///< '...'
       BLOCK_SEQUENCE_START,               ///< Indentation increase denoting a block
       BLOCK_MAPPING_START,                ///< sequence or a block mapping.
       BLOCK_END,                          ///< Indentation decrease.
       SEQ_START,                          ///< '['
       SEQ_END,                            ///< ']'
       MAP_START,                          ///< '{'
       MAP_END,                            ///< '}'
       BLOCK_ENTRY,                        ///< '-'
       FLOW_ENTRY,                         ///< ','
       KEY,                                ///< '?' or nothing (simple keys).
       VALUE,                              ///< ':'
       ALIAS, //(anchor)                   ///< '*anchor'
       ANCHOR, //(anchor)                  ///< '&anchor'
       TAG, //(handle,suffix)              ///< '!handle!suffix'
       SCALAR, //(value,style)             ///< A scalar.
       COMMENT, //(text)                   ///< A comment
    } Token_e;

    struct Token : public Region
    {
        Token_e type;
        cspan str;
        struct version_s { int major, minor; };
        struct tag_directive_s { cspan handle, prefix; };
        union {
            version_s version;
            tag_directive_s tag_directive;
        };
    };

    typedef enum {
        RTOP = 0x01 <<  0, // reading at top level
        RUNK = 0x01 <<  1, // reading an unknown: must determine whether scalar, map or seq
        RMAP = 0x01 <<  2, // reading a map
        RSEQ = 0x01 <<  3, // reading a seq
        EXPL = 0x01 <<  4, // reading is inside explicit flow chars: [] or {}
        RKEY = 0x01 <<  5, // reading a scalar as key
        RVAL = 0x01 <<  6, // reading a scalar as val
        CPLX = 0x01 <<  7, // reading a complex key
        BLCK = 0x01 <<  8, // reading a block scalar (|)
        FOLD = 0x01 <<  9, // reading a block scalar (>), folded (remove interior newlines)
        KEEP = 0x01 << 10, // reading a block scalar (+): keep last newline
        STRP = 0x01 << 11, // reading a block scalar (-): strip last newline
        SSCL = 0x01 << 12, // there's a scalar stored
    } State_e;

    struct State
    {

        struct LineContents
        {
            cspan  full;        ///< the full line, excluding newlines on the right
            cspan  ltrim;       ///< the line, starting at the first non-space character
            size_t indentation; ///< the number of columns with space on the left

            void reset(cspan const& full_)
            {
                full = full_;

                // find the first column where the character is not a space
                indentation = full.first_not_of(' ');
                ltrim = full.subspan(indentation);
            }
        };

        Location pos;
        LineContents     line_contents;
        size_t   flags;
        size_t   level;
        Node *   node;
        cspan    scalar;

        void reset(const char *file, Node *n)
        {
            flags = RUNK|RTOP;
            level = 0;
            pos.name = file;
            pos.offset = 0;
            pos.line = 1;
            pos.col = 1;
            node = n;
        }

        void line_scanned(cspan const& sp)
        {
            line_contents.reset(sp);
            printf("%3zd: '%.*s'\n", pos.line-1, _c4prsp(line_contents.full));
        }

        void line_progressed(size_t first_remaining)
        {
            line_contents.ltrim = line_contents.ltrim.subspan(first_remaining);
        }

        void line_ended(size_t len)
        {
            pos.offset += len;
            ++pos.line;
            pos.col = 1;
        }

        bool has_all(size_t f) const { return (flags & f) == f; }
        bool has_any(size_t f) const { return (flags & f) != 0; }
        bool has_none(size_t f) const { return (flags & f) == 0; }
    };

    void _push_state(size_t next_state, Node *n)
    {
        m_stack.push(m_state);
        m_state.flags = next_state;
        m_state.node = n;
        ++m_state.level;
    }

    void _pop_state()
    {
        C4_ASSERT( ! m_stack.empty());
        m_state = m_stack.pop();
        if(m_state.line_contents.indentation == 0)
        {
            C4_ASSERT(m_state.has_none(RTOP));
            m_state.flags |= RTOP;
        }
    }

    bool _indentation_pushed() const
    {
        if(m_stack.empty()) return false;
        return m_state.line_contents.indentation > m_stack.peek().line_contents.indentation;
    }

    bool _indentation_popped() const
    {
        if(m_stack.empty()) return false;
        return m_state.line_contents.indentation < m_stack.peek().line_contents.indentation;
    }

    void _store_scalar(cspan const& s)
    {
        C4_ASSERT(!(m_state.flags & SSCL));
        m_state.flags |= SSCL;
        m_state.scalar = s;
    }

public:

    cspan  m_file;
    cspan  m_buf;

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

    cspan _scan_line();

    void _handle_line(cspan ltrim);

    cspan _read_scalar(cspan const& line_remainder, bool known_quoted = false);


    void _push_map(cspan ltrim, size_t next_state, bool create_node)
    {
        printf("push_map\n");
        C4_ASSERT(next_state & RMAP);
        Node* node = m_state.node;
        if(create_node)
        {
            node = node->append_child({}, TYPE_MAP);
        }
        else
        {
            C4_ASSERT(node->is_map() || node->empty());
            node->m_type = TYPE_MAP;
        }
        _push_state(next_state, node);
    }
    void _pop_map(cspan ltrim)
    {
        printf("pop_map\n");
        _pop_state();
    }

    void _push_seq(cspan ltrim, size_t next_state, bool create_node)
    {
        printf("push_seq\n");
        C4_ASSERT(next_state & RSEQ);
        Node* node = m_state.node;
        if(create_node)
        {
            node = node->append_child({}, TYPE_SEQ);
        }
        else
        {
            C4_ASSERT(node->is_seq() || node->empty());
            node->m_type = TYPE_SEQ;
        }
        _push_state(next_state, node);
    }
    void _pop_seq(cspan ltrim)
    {
        printf("pop_seq\n");
        _pop_state();
    }

    void _append_val(cspan const& val)
    {
        printf("append val: %.*s\n", _c4prsp(val));
    }

    void _append_key_val(cspan const& key, cspan const& val)
    {
        printf("append key-val: %.*s %.*s\n", _c4prsp(key), _c4prsp(val));
    }

private:

    static inline bool _matches(const char c, const char (&chars)[1])
    {
        RymlCallbacks::error("WTF???");
        return false;
    }
    static inline bool _matches(const char c, const char (&chars)[2])
    {
        return (c == chars[0]);
    }
    static inline bool _matches(const char c, const char (&chars)[3])
    {
        return (c == chars[0]) || (c == chars[1]);
    }
    static inline bool _matches(const char c, const char (&chars)[4])
    {
        return (c == chars[0]) || (c == chars[1]) || (c == chars[2]);
    }
    static inline bool _matches(const char c, const char (&chars)[5])
    {
        return (c == chars[0]) || (c == chars[1]) || (c == chars[2]) || (c == chars[3]);
    }
    template< size_t N >
    static inline bool _matches(const char c, const char (&chars)[N])
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
