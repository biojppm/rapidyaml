#ifndef _C4_TPL_MGR_HPP_
#define _C4_TPL_MGR_HPP_

#include <stddef.h>

namespace c4 {
namespace tpl {

namespace detail {
template< size_t num_, size_t v_, bool bit1 > struct _lsb;
template< size_t num_, size_t v_ >
struct _lsb< num_, v_, true >
{
    enum : size_t { num = num_ };
};
template< size_t num_, size_t v_ >
struct _lsb< num_, v_, false >
{
    enum : size_t { num = _lsb< num_+1, (v_>>1), ((v_>>1)&1) >::num };
};
} // namespace detail

/** TMP version of lsb(); this needs to be implemented with template
 * meta-programming because C++11 cannot use a constexpr function with
 * local variables
 * @see lsb */
template< class I, size_t v >
struct lsb11
{
    static_assert(v != 0, "v != 0");
    enum : I { value = (I)detail::_lsb< 0, v, (v&1) >::num };
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#define C4_DECLARE_MANAGED(cls, base, idx) \
public:\
\
    using base_type = base;\
\
private:\
\
    static idx _s_type_id;\
    idx m_id;\
\
public:\
\
    inline idx id() const { return m_id; }\
    static inline idx s_type_id() { return _s_type_id; }\
    static inline cspan s_type_name() { return cspan(#cls); }\
\
private:\
\

#define C4_DEFINE_MANAGED(cls, idx) \
idx cls::_s_type_id = (idx)-1


#define C4_REGISTER_MANAGED(mgr, cls) \
(mgr).register_type<cls>()



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template< class I >
struct object_id_tpl
{
    I m_type_pos;
    object_id_tpl() { m_type_pos = (I)-1; }
    object_id_tpl(I type_pos)
    {
        m_type_pos = type_pos;
        C4_ASSERT(type() <= type_mask);
        C4_ASSERT(pos() <= pos_mask);
    }
    object_id_tpl(I type_, I pos_)
    {
        C4_ASSERT(type_ <= type_mask);
        C4_ASSERT(pos_ <= pos_mask);
        m_type_pos = (type << type_shift) | pos_;
    }
    I type() const { return (m_type_pos &  type_mask) >> type_shift; }
    I pos () const { return (m_type_pos & (~type_mask)); }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template< class B, class I >
struct LinearArenaBase
{
    virtual void reserve(size_t cap) = 0;
    virtual void clear() = 0;
    virtual void free() = 0;
    virtual I    claim() = 0;
    virtual B*   create() = 0;
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/** a collection of objects of a single concrete type inheriting
 * from a base type. Also acts as a linear allocation arena. */
template< class T, class I, I PageSize=128 >
struct LinearArena : public LinearArenaBase< typename T::base_type, I >
{
public:

    struct Page
    {
        T * mem;
        I   numpg; ///< number of pages allocated in this block
                   ///< (following numpg pages are allocated together with this)
    };

    cspan     m_type_name;
    I         m_type_id;

    Page *    m_pages;
    I         m_size;
    I         m_num_pages;
    Allocator m_alloc;

private:

    enum : I
    {
        /** id mask: all the bits up to PageSize. Use to extract the position
         * of an index within a page. */
        id_mask = I(PageSize) - I(1),
        /** page lsb: the number of bits complementary to PageSize. Use to
         * extract the page of an index. */
        page_lsb = lsb11< I, PageSize >::value,
    };

    static inline I _page(I id) { return i >> m_page_lsb; }
    static inline I _pos (I id) { return i &  m_id_mask; }

public:

    LinearArena(Allocator const& a={})
        :
        m_type_name(T::s_type_name()),
        m_type_id(T::s_type_id()),
        m_pages(nullptr),
        m_size(0),
        m_num_pages(0),
        m_allocator(a)
    {
    }
    ~LinearArena()
    {
        clear();
        free();
    }

    LinearArena(LinearArena const& that) = delete;
    LinearArena(LinearArena     && that) = delete;

    LinearArena& operator= (LinearArena const& that) = delete;
    LinearArena& operator= (LinearArena     && that) = delete;

public:

    I next_num_pages(size_t cap)
    {
        I rem = (cap % PageSize);
        cap += rem ? PageSize - rem : 0;
        return cap / PageSize;
    }

    void reserve(size_t cap) override
    {
        if(cap <= capacity()) return;
        I np = next_num_pages(cap);
        C4_ASSERT(np > m_num_pages);

        // allocate pages arr
        auto * pgs = (Page*) m_alloc.allocate(np * sizeof(Page), m_pages);
        memcpy(pgs, m_pages, m_num_pages * sizeof(Page));
        m_alloc.free(m_pages, np * sizeof(Page));
        m_pages = pgs;

        // allocate page mem
        I more_pages = np - m_num_pages;
        auto* last = m_num_pages > 0 ? m_pages[i-1]->mem : nullptr;
        auto* mem = (T*) m_alloc.allocate(more_pages * PageSize * sizeof(T), last);
        // the first page owns the mem (by setting numpg to the number of pages in this mem block)
        m_pages[m_num_pages]->mem = mem;
        m_pages[m_num_pages]->numpg = more_pages;
        // remaining pages only have their pointers set (and numpg is set to 0)
        for(I i = m_num_pages+1; i < np; ++i)
        {
            m_pages[i]->mem = mem + i * PageSize;
            m_pages[i]->numpg = 0;
        }
        m_num_pages = np;
    }

    void free() override final
    {
        if(m_num_pages == 0) return;
        for(I i = 0; i < m_num_pages; ++i)
        {
            Page *p = m_pages + i;
            if(p->numpg == 0) continue;
            m_alloc.free(p->mem, p->numpg * PageSize * sizeof(T));
            i += numpg;
            C4_ASSERT(i <= m_num_pages);
        }
        m_alloc.free(m_pages, m_num_pages * sizeof(Page));
        m_num_pages = 0;
    }

    void clear() override final
    {
        for(I i = 0; i < m_size; ++i)
        {
            T *ptr = get(i);
            ptr->~T();
        }
        m_size = 0;
    }

    I claim() override final
    {
        if(m_size == m_capacity)
        {
            reserve(m_size + 1); // adds a single page
        }
        return m_size++;
    }

    T* create() override final
    {
        I pos = claim();
        T *ptr = get(pos);
        object_id_tpl<I> id(m_type_id, pos);
        new (ptr) T();
        ptr->m_id = id.m_type_pos;
        return ptr;
    }

public:

    inline T* get(I id) const
    {
        I pg = _page(id), pos = _pos(id);
        char *mem = (char*) m_pages[pg].mem;
        mem += pos * m_type_size;
        return (T*) mem;
    }

    inline I size() const { return m_size; }
    inline I capacity() const { return m_num_pages * PageSize; }

};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/** A manager of objects with a common base type. */
template< class B, typename I=uint32_t, uint8_t Tbits=4 >
struct ObjMgr
{
public:

    enum : I {
        type_shift = 8 * sizeof(I) - Tbits,
        type_mask  = ((1 << Tbits) - 1) << type_shift,
        pos_mask   = (~(1 & 0)) & (~type_mask)
    };

public:

    template< class T >
    using ColT = LinearArena< T, I >;
    using ColB = LinearArenaBase< B, I >;
    using object_id = object_id_tpl< I >;

    union Col
    {
        Col(){}
        char mem[sizeof(ColT<B>)];
        ColT<B> dum;
    };

    std::vector< Col > m_collections;
    Allocator          m_allocator;

public:

    template< class T >
    I register_type()
    {
        static_assert(sizeof(ColT<T>) == sizeof(Col));
        static_assert(sizeof(ColT<B>) == sizeof(Col));
        T::_s_type_id = m_collections.size();
        m_collections.emplace_back();
        auto *col = get_col<T>();
        new ((void*)col) ColT<T>(m_allocator);
        return T::_s_type_id;
    }

    ColT<B>* get_col(I type_id)
    {
        auto * col = reinterpret_cast<ColT<B>*>(m_collections[type_id]);
        C4_ASSERT(col->m_type_id == type_id);
        return col;
    }

    template< class T >
    ColT<T>* get_col()
    {
        auto * col = reinterpret_cast<ColT<T>*>(m_collections[T::s_type_id()]);
        C4_ASSERT(col->m_type_id == T::s_type_id());
        return col;
    }

    /** linear search through the types */
    ColT<B>* get_col(cspan type_name)
    {
        for(auto &c : m_collections)
        {
            if(c.dum.m_type_name == type_name)
            {
                return reinterpret_cast<ColT<B>*>(c);
            }
        }
        return nullptr;
    }

public:

    ObjMgr()
    {
    }
    ~ObjMgr()
    {
        clear();
        free();
    }

    ObjMgr(ObjMgr const&) = delete;
    ObjMgr(ObjMgr     &&) = delete;

    ObjMgr& operator= (ObjMgr const&) = delete;
    ObjMgr& operator= (ObjMgr     &&) = delete;

public:

    void clear()
    {
        for(auto &c : m_collections)
        {
            c.dum.clear();
        }
    }

    void free()
    {
        for(auto &c : m_collections)
        {
            c.dum.free();
        }
    }

public:

    B *get(object_id i) const
    {
        auto *col = get_col(i.type());
        C4_ASSERT(col->m_type_id == T::s_type_id());
        B *ptr = col->get(i.pos());
        return ptr;
    }

    template< class T >
    T *get_as(object_id i) const
    {
        auto *col = get_col<T>(i.type());
        C4_ASSERT(col->m_type_id == T::s_type_id());
        T *ptr = col->get(i.pos());
        return ptr;
    }

public:

    template< class T, class... CtorArgs >
    T* create_as(CtorArgs&& ...args)
    {
        auto *col = get_col<T>();
        I pos = col->claim();
        T *ptr = col->get(pos);
        object_id id(col->m_type_id, pos);
        new ((void*)ptr) T(id.m_type_pos, std::forward< CtorArgs >(args)...);
        return ptr;
    }

    B* create(I type_id)
    {
        auto *col = get_col(type_id);
        I pos = col->claim();
        B *ptr = col->get(pos);
        object_id id(col->m_type_id, pos);
        new ((void*)ptr) T(id.m_type_pos);
        return ptr;
    }

    B *create(cspan type_name)
    {
        auto *col = get_col(type_name);
    }
};

} // namespace tpl
} // namespace c4

#endif /* _C4_TPL_MGR_HPP_ */
