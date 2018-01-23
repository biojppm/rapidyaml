#ifndef _C4_RYML_HPP_
#define _C4_RYML_HPP_

#define RYML_DBG // remove this
#define RYML_ERRMSG_SIZE 1024

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
    NOTYPE  = 0,       ///< no type is set
    VAL     = (1<<0),  ///< a leaf node, has a (possibly empty) value
    KEY     = (1<<1),  ///< is member of a map, must have non-empty key
    MAP     = (1<<2),  ///< a map: a parent of keyvals
    SEQ     = (1<<3),  ///< a seq: a parent of vals
    DOC     = (1<<4),  ///< a document
    STREAM  = (1<<5)|SEQ,  ///< a stream: a seq of docs
    KEYVAL  = KEY|VAL,
    KEYSEQ  = KEY|SEQ,
    KEYMAP  = KEY|MAP,
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
private:

    Node() = default;

    Node(Node const&) = delete;
    Node(Node     &&) = delete;
    Node& operator= (Node const&) = delete;
    Node& operator= (Node     &&) = delete;

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

    mutable Tree * m_s;
    NodeType_e     m_type;
    cspan          m_key;
    cspan          m_val;

    size_t         m_parent;
    children       m_children;

    listseq        m_list;

public:

    Tree *tree() const { return m_s; }
    size_t id() const;

    NodeType_e type() const { return m_type; }
    const char* type_str() const { return type_str(m_type); }
    static const char* type_str(NodeType_e ty);

    cspan const& key() const { C4_ASSERT(m_type & KEY); return m_key; }
    cspan const& val() const { C4_ASSERT(m_type & VAL); return m_val; }

    bool operator== (cspan const& cmp) const
    {
        C4_ASSERT(is_val());
        return m_val == cmp;
    }

    bool   is_root() const { return m_parent == NONE; }
    bool   is_stream() const { return (m_type & STREAM) == STREAM; }
    bool   is_doc() const { return m_type & DOC; }
    bool   is_container() const { return m_type & (MAP|SEQ|STREAM|DOC); }
    bool   is_map() const { return m_type & MAP; }
    bool   is_seq() const { return m_type & SEQ; }
    bool   has_val() const { return m_type & VAL; }
    bool   has_key() const { return m_type & KEY; }
    bool   is_val() const { return (m_type & KEYVAL) == VAL; }
    bool   is_keyval() const { return (m_type & KEYVAL) == KEYVAL; }

    bool   parent_is_seq() const { C4_ASSERT(parent()); return parent()->is_seq(); }
    bool   parent_is_map() const { C4_ASSERT(parent()); return parent()->is_map(); }

    /** true when name and value are empty, and has no children */
    bool   empty() const { return ! has_children() && m_key.empty() && (( ! (m_type & VAL)) || m_val.empty()); }
    bool   has_children() const { return m_children.first != NONE; }
    /** counts with this */
    bool   has_siblings() const;
    /** does not count with this */
    bool   has_other_siblings() const;

    bool   has_parent() const { return m_parent != NONE; }
    bool   has_sibling(Node const* n) const;
    bool   has_child(Node const* ch) const;

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
    size_t num_other_siblings() const;
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

    void  remove_sibling(cspan const& name); ///< remove a sibling by name
    void  remove_sibling(size_t i); ///< remove a sibling by index
    void  remove_sibling(Node *n);  ///< remove a sibling

    void  remove_child  (cspan const& name); ///< remove a child by name
    void  remove_child  (size_t i); ///< remove a child by index
    void  remove_child  (Node *n);  ///< remove a child

public:

    void to_val(cspan const& val, int more_flags = 0);
    void to_keyval(cspan const& key, cspan const& val, int more_flags = 0);
    void to_map(int more_flags = 0);
    void to_map(cspan const& key, int more_flags = 0);
    void to_seq(int more_flags = 0);
    void to_seq(cspan const& key, int more_flags = 0);
    void to_doc(int more_flags = 0);
    void to_stream(int more_flags = 0);

private:

    void _add_flags(NodeType_e f) { m_type = (NodeType_e)((int)f|(int)m_type); }
    void _add_flags(int        f) { m_type = (NodeType_e)(     f|(int)m_type); }

    void _rem_flags(NodeType_e f) { m_type = (NodeType_e)((~(int)f)&(int)m_type); }
    void _rem_flags(int        f) { m_type = (NodeType_e)((~     f)&(int)m_type); }

    void _set_flags(NodeType_e f) { m_type = f; }
    void _set_flags(int        f) { m_type = (NodeType_e)f; }

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
    if(has_children())
    {
        fn.push(this, indentation_level);
        for(Node *ch = first_child(); ch; ch = ch->next_sibling())
        {
            // no need to forward skip_root as it won't be root
            if(ch->visit(fn, indentation_level + increment))
            {
                fn.pop(this, indentation_level);
                return true;
            }
        }
        fn.pop(this, indentation_level);
    }
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

public:

    Tree();
    Tree(size_t sz);
    ~Tree();

    size_t size () const { return m_size; }
    bool   empty() const { return m_size == 0; }

    size_t capacity() const { return m_cap; }
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

    //! create and insert a new sibling of n. insert after "after"
    Node * insert_sibling(Node *n, Node *after)
    {
        C4_ASSERT(n->parent() != nullptr);
        return insert_child(get(n->m_parent), after);
    }
    Node * prepend_sibling(Node *n) { return insert_sibling(n, nullptr); }
    Node *  append_sibling(Node *n) { return insert_sibling(n, n->last_child()); }

    //! create and insert a new sibling of n. insert after "after"
    size_t insert_sibling(size_t n, size_t after)
    {
        C4_ASSERT(get(n)->parent() != nullptr);
        C4_ASSERT( ! get(after) || (get(n)->has_sibling(get(after)) && get(after)->has_sibling(get(n))));
        return insert_child(get(n)->m_parent, after);
    }
    size_t prepend_sibling(size_t n) { return insert_sibling(n, NONE); }
    size_t  append_sibling(size_t n) { return insert_sibling(n, id(get(n)->last_child())); }

public:

    //! create and insert a new child of "n". insert after "after"
    Node * insert_child(Node *n, Node *after)
    {
        size_t cap = m_cap;
        size_t ich = insert_child(id(n), id(after));
        if(m_cap != cap)
        {
            C4_ERROR("pointers were invalidated");
        }
        Node *ch = get(ich);
        return ch;
    }
    Node * prepend_child(Node *n) { return insert_child(n, nullptr); }
    Node *  append_child(Node *n) { return insert_child(n, n->last_child()); }

    //! create and insert a new child of "parent". insert after "after"
    size_t insert_child(size_t parent, size_t after)
    {
        C4_ASSERT(get(parent)->is_container());
        C4_ASSERT(get(parent)->has_child(get(after)) || after == NONE);
        size_t child = claim(after);
        set_parent(parent, child, after);
        return child;
    }
    size_t prepend_child(size_t n) { return insert_child(n, NONE); }
    size_t  append_child(size_t n) { return insert_child(n, id(get(n)->last_child())); }

public:

    //! remove a child by name, pointer version
    void remove_child(Node * parent, Node * child)
    {
        remove_child(id(parent), id(child));
    }
    //! remove a child by name, index version
    void remove_child(size_t parent, size_t child)
    {
        C4_ASSERT(get(parent)->is_container());
        C4_ASSERT(get(parent)->has_children());
        release(child);
        C4_ERROR("not implemented");
    }

    //! remove an entire branch at once
    void remove_branch(Node *first)
    {
        remove_branch(id(first));
    }
    //! remove an entire branch at once
    void remove_branch(size_t first)
    {
        C4_ERROR("not implemented");
    }

public:

    void move_branch(Node * branch, Node * new_parent, Node * after)
    {
        move_branch(id(branch), id(new_parent), id(after));
    }
    void move_branch(size_t branch, size_t new_parent, size_t after)
    {
        C4_ERROR("not implemented");
    }

private:

    void set_parent(size_t parent, size_t child, size_t prev_sibling, size_t next_sibling=NONE);
    void clear_range(size_t first, size_t num);

    size_t claim(size_t after);
    size_t claim(size_t prev, size_t next);

    void release(size_t i);

    void _clear(size_t i)
    {
        Node *n = get(i);
        n->m_s = nullptr;
        n->m_type = NOTYPE;
        n->m_key.clear();
        n->m_val.clear();
        n->m_parent = NONE;
        n->m_children.first = NONE;
        n->m_children.last = NONE;
    }

    Node      * head()       { return m_buf + m_head; }
    Node const* head() const { return m_buf + m_head; }

    Node      * tail()       { return m_buf + m_tail; }
    Node const* tail() const { return m_buf + m_tail; }

public:

    Node      * root()       { C4_ASSERT(m_cap > 0 && m_size > 0); Node *n = m_buf; C4_ASSERT(n->parent() == nullptr); return n; }
    Node const* root() const { C4_ASSERT(m_cap > 0 && m_size > 0); Node *n = m_buf; C4_ASSERT(n->parent() == nullptr); return n; }

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

    struct RepC
    {
        char c;
        size_t num_times;
    };

    size_t _visit(Node const* n, size_t ilevel = 0)
    {
        m_pos = 0;
        _do_visit(n, ilevel);
        return m_pos;
    }

    void _do_visit(Node const* n, size_t ilevel = 0, bool no_ind = false)
    {
        RepC ind{' ', 2 * size_t(!no_ind) * ilevel};

        if(n->is_stream())
        {
            ;
        }
        else if(n->is_doc())
        {
            _write("---\n");
        }
        else if(n->is_keyval())
        {
            C4_ASSERT(n->has_parent());
            _write(ind, n->key(), ": ", n->val(), '\n');
        }
        else if(n->is_val())
        {
            C4_ASSERT(n->has_parent());
            _write(ind, "- ", n->val(), '\n');
        }
        else if(n->is_container() && ! n->is_root())
        {
            C4_ASSERT(n->parent_is_map() || n->parent_is_seq());
            C4_ASSERT(n->is_map() || n->is_seq());

            if(n->parent_is_seq())
            {
                C4_ASSERT( ! n->has_key());
                _write(ind, "- ");
            }
            else if(n->parent_is_map())
            {
                C4_ASSERT(n->has_key());
                _write(ind, n->key(), ':');
            }

            if(n->has_children())
            {
                if(n->is_seq())
                {
                    if(n->parent_is_map())
                    {
                        _write('\n');
                    }
                    else
                    {
                        // do not indent the first child, as it will be written on the same line
                        no_ind = true;
                    }
                }
                else if(n->is_map())
                {
                    if(n->parent_is_seq())
                    {
                        // do not indent the first child, as it will be written on the same line
                        no_ind = true;
                    }
                    else
                    {
                        _write('\n');
                    }
                }
                else
                {
                    C4_ERROR("invalid node");
                }
            }
            else
            {
                if(n->is_seq())
                {
                    _write("[]\n");
                }
                else if(n->is_map())
                {
                    _write("{}\n");
                }
            }
        }

        size_t next_level = ilevel + 1;
        if(n->is_stream() || n->is_doc() || n->is_root())
        {
            next_level = ilevel; // do not indent at top level
        }

        for(Node const* ch = n->first_child(); ch; ch = ch->next_sibling())
        {
            _do_visit(ch, next_level, no_ind);
            no_ind = false;
        }

        if(n->is_stream())
        {
            _write("...\n");
        }

    }

private:

    template< class T, class... Args >
    inline void _write(T a, Args... more)
    {
        _do_write(a);
        _write(more...);
    }

    template< class T >
    inline void _write(T a)
    {
        _do_write(a);
    }

private:

    inline void _do_write(const char c)
    {
        if(m_file)
        {
            fwrite(&c, sizeof(char), 1, m_file);
        }
        else if(m_pos + 1 < m_span.len)
        {
            m_span[m_pos] = c;
        }
        ++m_pos;
    }

    inline void _do_write(RepC const rc)
    {
        if(m_file)
        {
            //fwrite(&rc.c, sizeof(char), rc.num_times, m_file); // this fails... need to investigate
            for(size_t i = 0; i < rc.num_times; ++i)
            {
                fputc(rc.c, m_file);
            }
        }
        else if(m_pos + rc.num_times < m_span.len)
        {
            for(size_t i = 0; i < rc.num_times; ++i)
            {
                m_span[m_pos + i] = rc.c;
            }
        }
        m_pos += rc.num_times;
    }

    inline void _do_write(cspan const& sp)
    {
        if(sp.empty()) return;
        if(m_file)
        {
            fwrite(sp.str, sp.len, 1, m_file);
        }
        else if(m_pos + sp.len < m_span.len)
        {
            memcpy(&(m_span[m_pos]), sp.str, sp.len);
        }
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
    if(t.empty()) return 0;
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
    if(t.empty()) return span();
    return emit(t.root(), sp, error_on_excess);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class Parser
{
public:

    Parser();

    Tree parse(                       cspan const& src) { return parse({}, src); }
    Tree parse(cspan const& filename, cspan const& src)
    {
        Tree t;
        size_t capacity = _count_nlines(src);
        capacity = capacity >= 8 ? capacity : 8;
        t.reserve(capacity);
        parse(filename, src, &t);
        return t;
    }

    void parse(                       cspan const& src, Tree *t) { return parse({}, src, t); }
    void parse(cspan const& filename, cspan const& src, Tree *t)
    {
        parse(filename, src, t->root());
    }

    void parse(                       cspan const& src, Node *root) { return parse({}, src, root); }
    void parse(cspan const& filename, cspan const& src, Node *root);

private:

    void  _reset();

    bool  _finished_file() const;
    bool  _finished_line() const;

    void  _scan_line();
    void  _next_line() { _line_ended(); }

    cspan _scan_scalar();
    cspan _scan_comment();
    cspan _scan_quoted_scalar(const char q);
    cspan _scan_block();

    cspan _filter_quoted_scalar(cspan const& s, const char q);
    cspan _filter_raw_block(cspan const& block, BlockStyle_e style, BlockChomp_e chomp, size_t indentation);

    void  _handle_finished_file();
    void  _handle_line();
    int   _handle_indentation();

    bool  _handle_unk();
    bool  _handle_map();
    bool  _handle_seq();
    bool  _handle_scalar();
    bool  _handle_top();
    bool  _handle_anchors_and_refs();
    bool  _handle_types();

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

    static bool   _read_decimal(cspan const& str, size_t *decimal);
    static size_t _count_nlines(cspan src);

private:

#ifdef RYML_DBG
    void _dbg(const char *msg, ...) const;
#endif
    void _err(const char *msg, ...) const;
    int  _fmt_msg(char *buf, int buflen, const char *msg, va_list args) const;

private:

    typedef enum {
        RTOP = 0x01 <<  0, // reading at top level
        RUNK = 0x01 <<  1, // reading an unknown: must determine whether scalar, map or seq
        RMAP = 0x01 <<  2, // reading a map
        RSEQ = 0x01 <<  3, // reading a seq
        EXPL = 0x01 <<  4, // reading is inside explicit flow chars: [] or {}
        CPLX = 0x01 <<  5, // reading a complex key
        RKEY = 0x01 <<  6, // reading a scalar as key
        RVAL = 0x01 <<  7, // reading a scalar as val
        RNXT = 0x01 <<  8, // read next val or keyval
        SSCL = 0x01 <<  9, // there's a scalar stored
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
        size_t       node_id; // don't hold a pointer to the node as it will be relocated during tree resizes
        cspan        scalar;

        Location     pos;
        LineContents line_contents;
        size_t       indref;
        size_t       indprev;

        void reset(const char *file, size_t node_id_)
        {
            flags = RUNK|RTOP;
            level = 0;
            pos.name = file;
            pos.offset = 0;
            pos.line = 1;
            pos.col = 1;
            node_id = node_id_;
            scalar.clear();
            indref = 0;
            indprev = 0;
        }
    };

    void _line_progressed(size_t ahead)
    {
        m_state->pos.offset += ahead;
        m_state->pos.col += ahead;
        C4_ASSERT(m_state->pos.col <= m_state->line_contents.stripped.len+1);
        m_state->line_contents.rem = m_state->line_contents.rem.subspan(ahead);
    }

    void _line_ended()
    {
        C4_ASSERT(m_state->pos.col == m_state->line_contents.stripped.len+1);
        m_state->pos.offset += m_state->line_contents.full.len - m_state->line_contents.stripped.len;
        ++m_state->pos.line;
        m_state->pos.col = 1;
    }

    void _prepare_pop()
    {
        C4_ASSERT(m_stack.size() > 1);
        State const& curr = m_stack.top();
        State      & next = m_stack.top(1);
        next.pos = curr.pos;
        next.line_contents = curr.line_contents;
        next.scalar = curr.scalar;
    }

    inline Node * node(State const* s) const { return m_tree->get(s->node_id); }
    inline Node * node(State const& s) const { return m_tree->get(s .node_id); }
    inline Node * node(size_t node_id) const { return m_tree->get(   node_id); }

    inline bool has_all(size_t f) const { return has_all(f, m_state); }
    inline bool has_any(size_t f) const { return has_any(f, m_state); }
    inline bool has_none(size_t f) const { return has_none(f, m_state); }

    inline bool has_all(size_t f, State const* s) const { return (s->flags & f) == f; }
    inline bool has_any(size_t f, State const* s) const { return (s->flags & f) != 0; }
    inline bool has_none(size_t f, State const* s) const { return (s->flags & f) == 0; }

    inline void set_flags(size_t f) { set_flags(f, m_state); }
    inline void add_flags(size_t on) { add_flags(on, m_state); }
    inline void addrem_flags(size_t on, size_t off) { addrem_flags(on, off, m_state); }
    inline void rem_flags(size_t off) { rem_flags(off, m_state); }

    void set_flags(size_t f, State * s);
    void add_flags(size_t on, State * s);
    void addrem_flags(size_t on, size_t off, State * s);
    void rem_flags(size_t off, State * s);

private:

    cspan   m_file;
    cspan   m_buf;

    size_t  m_root_id;
    Tree *  m_tree;

    detail::stack< State > m_stack;
    State * m_state;
    State * m_deindent;

};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

inline Tree parse(cspan const& buf)
{
    Parser np;
    return np.parse(buf);
}

inline Tree parse(cspan const& filename, cspan const& buf)
{
    Parser np;
    return np.parse(filename, buf);
}

inline void parse(cspan const& buf, Tree *t)
{
    Parser np;
    np.parse(buf, t);
}

inline void parse(cspan const& filename, cspan const& buf, Tree *t)
{
    Parser np;
    np.parse(filename, buf, t);
}

inline void parse(cspan const& buf, Node *root)
{
    Parser np;
    np.parse(buf, root);
}

inline void parse(cspan const& filename, cspan const& buf, Node *root)
{
    Parser np;
    np.parse(filename, buf, root);
}

} // namespace yml
} // namespace c4

#include <ryml/ryml.cpp>

#endif // _C4_RYML_HPP_
