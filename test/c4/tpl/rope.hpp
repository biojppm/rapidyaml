#ifndef _C4_TPL_ROPE_HPP_
#define _C4_TPL_ROPE_HPP_

#include "./common.hpp"

#ifdef __GNUC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wtype-limits"
#endif

namespace c4 {
namespace tpl {

/** A memory-efficient rope class (a sequence of strings). It's array- and
 * index-based and not node_pointer-based; this saves work on resizes and is
 * more cache-efficient. It uses non-owning strings, and thus performs no
 * string allocations, copies or deallocations. The array is in fact an
 * index-based linked list implementation. */
class Rope
{
public:

    struct rope_entry
    {
        cspan s;
        size_t m_prev;
        size_t m_next;
    };

    //! an indexer into a rope
    struct rope_pos
    {
        size_t entry;
        size_t i;

        rope_pos() : entry(0), i(0) {}
        rope_pos(size_t e, size_t p) : entry(e), i(p) {}

        bool valid() const { return entry != NONE && i != npos; }

        bool operator== (rope_pos const& that) const { return entry == that.entry && i == that.i; }
        bool operator!= (rope_pos const& that) const { return entry != that.entry || i != that.i; }
    };

public:

    rope_entry * m_buf;
    size_t       m_cap;

    size_t       m_size;
    size_t       m_head;
    size_t       m_tail;
    size_t       m_free_head;
    size_t       m_free_tail;

    size_t       m_str_size;

    Allocator    m_alloc;

public:

    Rope(Allocator const& a={}) { memset(this, 0, sizeof(*this)); m_head = m_tail = m_free_head = m_free_tail = NONE; m_alloc = a; }
    Rope(size_t cap, Allocator const& a={}) : Rope(a) { reserve(cap); }

    ~Rope() { _free(); }

    Rope(Rope const& that) : Rope() { _copy(that); }
    Rope(Rope     && that) : Rope() { _move(&that); }

    Rope& operator= (Rope const& that) { _free(); _copy(that); return *this; }
    Rope& operator= (Rope     && that) { _free(); _move(&that); return *this; }

private:

    void _free()
    {
        if(m_buf)
        {
            m_alloc.free(m_buf, sizeof(rope_entry) * m_cap);
            m_buf = nullptr;
        }
    }

    void _copy(Rope const& that)
    {
        C4_ASSERT(m_buf == nullptr);
        memcpy(this, &that, sizeof(*this));
        m_buf = (rope_entry*) m_alloc.allocate(m_cap * sizeof(rope_entry), /*hint*/that.m_buf);
        memcpy(m_buf, that.m_buf, m_cap * sizeof(rope_entry));
    }

    void _move(Rope *that)
    {
        memcpy(this, &that, sizeof(*this));
        m_buf = that->m_buf;
        that->m_buf = nullptr;
    }

public:

    rope_entry      * get(size_t i)       { C4_ASSERT(i != NONE && i < m_cap); return m_buf + i; }
    rope_entry const* get(size_t i) const { C4_ASSERT(i != NONE && i < m_cap); return m_buf + i; }

    size_t prev(size_t i) const { C4_ASSERT(i >= 0 && i < m_cap); return (m_buf + i)->m_prev; }
    size_t next(size_t i) const { C4_ASSERT(i >= 0 && i < m_cap); return (m_buf + i)->m_next; }

    size_t head() const { return m_head; }
    size_t tail() const { return m_tail; }

public:

    bool empty() const { return m_str_size == 0 && m_size == 0; }

    size_t str_size() const { return m_str_size; }

    size_t num_entries() const { return m_size; }

    void reserve(size_t cap)
    {
        if(cap <= m_cap) return;
        if(m_free_head == NONE)
        {
            C4_ASSERT(m_free_tail == m_free_head);
            m_free_head = m_cap;
            m_free_tail = cap;
        }
        else
        {
            C4_ASSERT(m_buf != nullptr);
            C4_ASSERT(m_free_tail != NONE);
            m_buf[m_free_tail].m_next = m_cap;
        }
        rope_entry *buf = (rope_entry*) m_alloc.allocate(cap * sizeof(rope_entry), /*hint*/m_buf);
        if(m_buf)
        {
            memcpy(buf, m_buf, m_cap * sizeof(rope_entry));
            m_alloc.free(m_buf, m_cap * sizeof(rope_entry));
        }
        size_t first = m_cap, del = cap - m_cap;
        m_cap = cap;
        m_buf = buf;
        _clear_range(first, del);
    }

    void clear()
    {
        _clear_range(0, m_cap);
        m_size = 0;
        m_head = NONE;
        m_tail = NONE;
        m_free_head = 0;
        m_free_tail = m_cap;
    }

private:

    void _clear(size_t i)
    {
        _p(i).s.clear();
    }

    rope_entry      & _p(size_t i)       { C4_ASSERT(i != NONE && i < m_cap); return m_buf[i]; }
    rope_entry const& _p(size_t i) const { C4_ASSERT(i != NONE && i < m_cap); return m_buf[i]; }

    void _clear_range(size_t first, size_t num)
    {
        if(num == 0) return; // prevent overflow when subtracting
        C4_ASSERT(first >= 0 && first + num <= m_cap);
        memset(m_buf + first, 0, num * sizeof(rope_entry));
        for(size_t i = first, e = first + num; i < e; ++i)
        {
            _clear(i);
            auto *n = m_buf + i;
            n->m_prev = i - 1;
            n->m_next = i + 1;
        }
        m_buf[first + num - 1].m_next = NONE;
    }

    size_t _claim()
    {
        if(m_free_head == NONE || m_buf == nullptr)
        {
            size_t sz = 2 * m_cap;
            sz = sz ? sz : 16;
            reserve(sz);
            C4_ASSERT(m_free_head != NONE);
        }

        C4_ASSERT(m_size < m_cap);
        C4_ASSERT(m_free_head >= 0 && m_free_head < m_cap);

        size_t ichild = m_free_head;
        rope_entry *child = m_buf + ichild;

        ++m_size;
        m_free_head = child->m_next;
        if(m_free_head == NONE)
        {
            m_free_tail = NONE;
            C4_ASSERT(m_size == m_cap);
        }

        _clear(ichild);

        return ichild;
    }

    void _release(size_t i)
    {
        C4_ASSERT(i >= 0 && i < m_cap);
        auto & w = m_buf[i];
        m_str_size -= w.s.len;

        // add to the front of the free list
        w.m_next = m_free_head;
        w.m_prev = NONE;
        if(m_free_head != NONE)
        {
            m_buf[m_free_head].m_prev = i;
        }
        m_free_head = i;
        if(m_free_tail == NONE)
        {
            m_free_tail = m_free_head;
        }

        _clear(i);

        --m_size;
    }

public:

    size_t insert_before(size_t next, cspan s)
    {
        C4_ASSERT(next != NONE);
        size_t i = insert_after(_p(next).m_prev);
        _p(i).s = s;
        m_str_size += s.len;
        return i;
    }

    size_t insert_before(size_t next)
    {
        C4_ASSERT(next != NONE);
        size_t i = insert_after(_p(next).m_prev);
        return i;
    }

    size_t insert_after(size_t prev, cspan s)
    {
        size_t i = insert_after(prev);
        _p(i).s = s;
        m_str_size += s.len;
        return i;
    }

    size_t insert_after(size_t prev)
    {
        size_t i = _claim();
        auto &e = _p(i);
        e.s.clear();
        e.m_prev = prev;
        e.m_next = NONE;
        if(prev == NONE)
        {
            e.m_next = m_head;
            m_head = i;
        }
        else if(prev != NONE)
        {
            auto & p = _p(prev);
            e.m_next = p.m_next;
            p.m_next = i;
        }
        if(e.m_next == NONE)
        {
            m_tail = i;
        }
        else
        {
            _p(e.m_next).m_prev = i;
        }
        return i;
    }

    size_t prepend(cspan s) { return insert_after(NONE, s); }
    size_t prepend() { return insert_after(NONE); }

    size_t append(cspan s) { return insert_after(m_tail, s); }
    size_t append() { return insert_after(m_tail); }

public:

    size_t split(size_t entry, size_t pos)
    {
        return _do_erase(entry, pos, 0);
    }

    /** replace a portion of entry, starting at pos, num characters long */
    size_t replace(size_t entry, size_t pos, size_t num, cspan s)
    {
        C4_ASSERT(pos     >= 0 && pos     <= _p(entry).s.len);
        C4_ASSERT(pos+num >= 0 && pos+num <= _p(entry).s.len);
        // replace a subspan: split the span once or twice as needed
        if(pos > 0 || num < _p(entry).s.len)
        {
            size_t n = _do_erase(entry, pos, num);
            n = insert_after(n, s);
            return n;
        }
        // replace the whole span
        C4_ASSERT(pos == 0 && num == _p(entry).s.len);
        _p(entry).s = s;
        return entry;
    }

    /** fully replace an entry */
    size_t replace(size_t entry, cspan s)
    {
        C4_ASSERT(entry != NONE && entry < m_cap);
        auto &e = _p(entry);
        m_str_size -= e.s.len;
        m_str_size += s.len;
        e.s = s;
        return entry;
    }

    void erase(size_t entry, size_t pos, size_t num)
    {
        if(num == 0) return;
        _do_erase(entry, pos, num);
    }

    void erase(size_t entry)
    {
        _release(entry);
    }

    size_t _do_erase(size_t entry, size_t pos, size_t num)
    {
        C4_ASSERT(pos >= 0 && pos <= get(entry)->s.len);
        C4_ASSERT(pos+num >= 0 && pos+num <= get(entry)->s.len);
        size_t i = entry;
        if(pos == 0)
        {
            auto &w = _p(entry);
            w.s = w.s.sub(pos + num);
            i = w.m_prev;
        }
        else if(pos > 0 && pos+num < get(entry)->s.len)
        {
            i = insert_after(entry); // may cause a relocation
            auto &e = _p(i), &w = _p(entry);
            e.s = w.s;
            w.s = e.s.sub(0, pos);
            e.s = e.s.sub(pos + num);
            i = entry;
        }
        else if(pos > 0 && pos+num == get(entry)->s.len)
        {
            auto &w = _p(entry);
            w.s = w.s.sub(0, pos);
            i = entry;
        }
        else if(pos == get(entry)->s.len)
        {
            C4_ASSERT(num == 0); // @todo extend to next node
            i = entry;
        }
        else
        {
            C4_ERROR("never reach");
        }
        C4_ASSERT(m_str_size >= num);
        m_str_size -= num;
        return i;
    }

public:

    cspan sub(size_t entry, size_t pos=0) const
    {
        return _p(entry).s.sub(pos);
    }

    cspan sub(rope_pos pos) const
    {
        return _p(pos.entry).s.sub(pos.i);
    }

    /** this won't match splitted tokens */
    rope_pos lookup_token(cspan token) const { return lookup_token(token, {m_head, 0}); }
    rope_pos lookup_token(cspan token, rope_pos pos) const
    {
        while(pos.entry < m_cap && pos.valid())
        {
            auto p = sub(pos).find(token);
            if(p != npos)
            {
                rope_pos ret(pos.entry, p);
                return ret;
            }
            pos.entry = next(pos.entry);
            pos.i = 0;
        }
        return {NONE, npos};
    }

    rope_pos split_before(cspan token) { return split_before(token, {m_head, 0}); }
    rope_pos split_before(cspan token, rope_pos pos)
    {
        pos = lookup_token(token, pos);
        if( ! pos.valid()) return {NONE, npos};
        size_t entry = split(pos.entry, pos.i);
        return {entry, 0};
    }

    rope_pos split_after(cspan token) { return split_after(token, {m_head, 0}); }
    rope_pos split_after(cspan token, rope_pos pos)
    {
        pos = lookup_token(token, pos);
        if( ! pos.valid()) return {NONE, npos};
        C4_ASSERT(sub(pos).len >= token.len);
        size_t entry = split(pos.entry, pos.i + token.len);
        return {entry, 0};
    }

    rope_pos insert_before(cspan token, cspan val) { return insert_before(token, val, {m_head, 0}); }
    rope_pos insert_before(cspan token, cspan val, rope_pos pos)
    {
        pos = split_before(token, pos);
        if( ! pos.valid()) return {NONE, npos};
        size_t entry = insert_after(pos.entry, val);
        return {entry, 0};
    }

    rope_pos insert_after(cspan token, cspan val) { return insert_after(token, val, {m_head, 0}); }
    rope_pos insert_after(cspan token, cspan val, rope_pos pos)
    {
        pos = split_after(token, pos);
        if( ! pos.valid()) return {NONE, npos};
        size_t entry = insert_after(pos.entry, val);
        return {entry, 0};
    }

public:

    void insert_before_all(cspan token, cspan val)
    {
        rope_pos pos = {m_head, 0};
        while(pos.valid())
        {
            pos = insert_before(token, val, pos);
            if( ! pos.valid()) break;
            C4_ASSERT(_p(pos.entry).s.str == val.str && _p(pos.entry).s.len == val.len);
            C4_ASSERT(next(pos.entry) != NONE);
            C4_ASSERT(sub({next(pos.entry), 0}).sub(0, token.len) == token.str);
            pos = {next(pos.entry), token.len};
        }
    }

    void insert_after_all(cspan token, cspan val)
    {
        rope_pos pos = {m_head, 0};
        while(pos.valid())
        {
            pos = insert_after(token, val, pos);
            if( ! pos.valid()) break;
            C4_ASSERT(_p(pos.entry).s.str == val.str && _p(pos.entry).s.len == val.len);
            pos = {next(pos.entry), 0};
        }
    }

    void replace_all(cspan token, cspan repl)
    {
        for(auto const& tk : tokens(token))
        {
            C4_ASSERT(sub(tk).begins_with(token));
            replace(tk.entry, tk.i, token.len, repl);
        }
    }

private:

    template< class It >
    struct container_impl
    {
        It b, e;

        container_impl(It const& b_, It const& e_) : b(b_), e(e_) {}

        using iterator = It;

        iterator begin() const { return b; }
        iterator end() const { return e; }
    };

    template< class RopeC, class RopeE >
    struct entry_iterator_impl
    {
        RopeC *m_rope;
        size_t m_entry;

        using value_type = const cspan;

        entry_iterator_impl(RopeC * r, size_t id) : m_rope(r), m_entry(id) {}

        entry_iterator_impl& operator++ () { C4_ASSERT(m_entry != NONE); m_entry = m_rope->next(m_entry); return *this; }
        entry_iterator_impl& operator-- () { C4_ASSERT(m_entry != NONE); m_entry = m_rope->prev(m_entry); return *this; }

        value_type& operator*  () { return  m_rope->get(m_entry)->s; }
        value_type* operator-> () { return &m_rope->get(m_entry)->s; }

        bool operator!= (entry_iterator_impl const& that) const { C4_ASSERT(m_rope == that.m_rope); return m_entry != that.m_entry; }
        bool operator== (entry_iterator_impl const& that) const { C4_ASSERT(m_rope == that.m_rope); return m_entry == that.m_entry; }
    };

public:

    using       iterator = entry_iterator_impl<       Rope,       rope_entry >;
    using const_iterator = entry_iterator_impl< const Rope, const rope_entry >;

          iterator begin()       { return       iterator(this, m_head); }
          iterator   end()       { return       iterator(this, NONE); }
    const_iterator begin() const { return const_iterator(this, m_head); }
    const_iterator   end() const { return const_iterator(this, NONE); }

    container_impl<       iterator > entries()       { return container_impl<       iterator >(begin(), end()); }
    container_impl< const_iterator > entries() const { return container_impl< const_iterator >(begin(), end()); }

private:

    template< class RopeC >
    struct token_iterator_impl
    {
        RopeC *   m_rope;
        rope_pos  m_pos;
        cspan     m_token;

        using value_type = rope_pos;

        token_iterator_impl(RopeC * r, rope_pos p, cspan t) : m_rope(r), m_pos(p), m_token(t) {}

        token_iterator_impl& operator++ () { C4_ASSERT(m_pos.entry != NONE); m_pos = m_rope->lookup_token(m_token, m_pos); return *this; }

        rope_pos const& operator*  () const { return  m_pos; }
        rope_pos const* operator-> () const { return &m_pos; }

        bool operator!= (token_iterator_impl const& that) const { C4_ASSERT(m_rope == that.m_rope); return m_pos != that.m_pos; }
        bool operator== (token_iterator_impl const& that) const { C4_ASSERT(m_rope == that.m_rope); return m_pos == that.m_pos; }

    };

public:

    using       token_iterator = token_iterator_impl<       Rope >;
    using const_token_iterator = token_iterator_impl< const Rope >;

    container_impl<       token_iterator > tokens(cspan token)       { return container_impl<       token_iterator >({this, lookup_token(token), token}, {this, {NONE, npos}, token}); }
    container_impl< const_token_iterator > tokens(cspan token) const { return container_impl< const_token_iterator >({this, lookup_token(token), token}, {this, {NONE, npos}, token}); }

public:

    span chain_all(span buf, bool error_on_excess=true) const
    {
        C4_ASSERT(_count_total_len() == m_str_size);
        if(buf.len < m_str_size)
        {
            if(error_on_excess)
            {
                C4_ERROR("insufficient space");
            }
            span ret;
            ret.str = nullptr;
            ret.len = m_str_size;
            return ret;
        }
        span ret = buf;
        ret.len = 0;
        for(auto const& s : *this)
        {
            memcpy(ret.str + ret.len, s.str, s.len);
            ret.len += s.len;
        }
        return ret;
    }

    template< class CharOwningContainer >
    span chain_all_resize(CharOwningContainer * cont) const
    {
        span buf(cont->data(), cont->size());
        span ret = chain_all(buf, /*error_on_excess*/false);
        if(ret.str == nullptr)
        {
            cont->resize(ret.len);
            buf.assign(cont->data(), cont->size());
            ret = chain_all(buf, /*error_on_excess*/true);
        }
        return ret;
    }

    size_t _count_total_len() const
    {
        size_t len = 0;
        for(auto const& s : *this)
        {
            len += s.len;
        }
        return len;
    }
};


template< class OStream >
inline OStream& operator << (OStream &s, Rope const& r)
{
    for(auto const& sp : r)
    {
        s << sp;
    }
    return s;
}


} // namespace tpl
} // namespace c4

#ifdef __GNUC__
#   pragma GCC diagnostic pop
#endif

#endif /* _C4_TPL_ROPE_HPP_ */
