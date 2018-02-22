#ifndef _C4_TPL_TOKEN_CONTAINER_HPP_
#define _C4_TPL_TOKEN_CONTAINER_HPP_

#include <vector>
#include "./rope.hpp"

#ifdef __GNUC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wtype-limits"
#endif

namespace c4 {
namespace tpl {

class TokenBase;
class Rope;

struct TplLocation
{
    Rope *         m_rope;
    Rope::rope_pos m_rope_pos;
    //! @todo:
    //size_t         m_offset;
    //size_t         m_line;
    //size_t         m_column;
};

void register_known_tokens();

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/** allow user-registered tokens */
#define C4TPL_DECLARE_TOKEN(cls, stok, etok, mrk)                       \
                                                                        \
    using base_type = TokenBase;                                        \
                                                                        \
    virtual cspan const& stoken() const override { static const cspan s(stok); return s; } \
    virtual cspan const& etoken() const override { static const cspan s(etok); return s; } \
    virtual cspan const& marker() const override { static const cspan s(mrk); return s; } \
                                                                        \
    static TokenType _get_type()                                        \
    {                                                                   \
        return {_get_name(),                                            \
                sizeof(cls),                                            \
                &_create_inplace,                                       \
                &_destroy_inplace,                                      \
                &_get_ptr_inplace};                                     \
    }                                                                   \
    static const char* _get_name()                                      \
    {                                                                   \
        return #cls;                                                    \
    }                                                                   \
    static TokenBase* _create_inplace(span mem, size_t id)              \
    {                                                                   \
        C4_ASSERT(sizeof(cls) <= mem.len);                              \
        cls *ptr = new ((void*)mem.str) cls();                          \
        ptr->m_id = id;                                                 \
        return ptr;                                                     \
    }                                                                   \
    static void _destroy_inplace(span mem)                              \
    {                                                                   \
        C4_ASSERT(sizeof(cls) <= mem.len);                              \
        cls *ptr = reinterpret_cast< cls* >(mem.str);                   \
        ptr->~cls();                                                    \
    }                                                                   \
    static TokenBase* _get_ptr_inplace(span mem)                        \
    {                                                                   \
        C4_ASSERT(sizeof(cls) <= mem.len);                              \
        cls *ptr = reinterpret_cast< cls* >(mem.str);                   \
        return ptr;                                                     \
    }

#define C4TPL_REGISTER_TOKEN(cls) TokenRegistry::register_token< cls >()


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

struct TokenType
{
    using inplace_create  = TokenBase* (*)(span mem, size_t id);
    using inplace_destroy = void       (*)(span mem);
    using inplace_get_ptr = TokenBase* (*)(span mem);

    const char*     name;
    size_t          size;
    inplace_create  create;
    inplace_destroy destroy;
    inplace_get_ptr get_ptr;
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/** registers token types and serves type information  */
class TokenRegistry
{
public:


    template< class T >
    static size_t register_token()
    {
        auto t = T::_get_type();
        register_size(t.size);
        get_types().push_back(t);
        T obj;
        get_stokens().push_back(obj.stoken());
        return get_types().size() - 1;
    }

    static std::vector< TokenType >& get_types()
    {
        static std::vector< TokenType > v;
        return v;
    }
    static std::vector< cspan >& get_stokens()
    {
        static std::vector< cspan > v;
        return v;
    }

    static cspan::first_of_any_result match_any(cspan const& rem)
    {
        auto const& t = get_stokens();
        return rem.first_of_any(t.begin(), t.end());
    }

    static TokenType const& get_type(size_t i)
    {
        auto const& t = get_types();
        C4_ASSERT(i >= 0 && i < t.size());
        return t[i];
    }

public:

    static size_t get_max_size() { return _max_size(); }
    static void register_size(size_t sz) { _max_size() = sz > _max_size() ? sz : _max_size(); }

private:

    static size_t& _max_size() { static size_t sz = 0; return *&sz; }

};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class TokenContainer
{
public:

    std::vector< char > m_tokens;
    size_t m_num_tokens;
    size_t m_max_size;
    size_t m_entry_size;

public:

    TokenContainer(size_t cap=0) : m_tokens(), m_num_tokens(0), m_max_size(0), m_entry_size(0)
    {
        m_max_size = TokenRegistry::get_max_size();
        if( ! m_max_size)
        {
            register_known_tokens();
            m_max_size = TokenRegistry::get_max_size();
        }
        m_entry_size = sizeof(size_t) + m_max_size;
        m_tokens.reserve(cap * m_entry_size);
    }

    ~TokenContainer();

    size_t size() const { return m_num_tokens; }

    span get_token_mem(size_t i)
    {
        C4_ASSERT(i >= 0 && i < m_num_tokens);
        return {&m_tokens[i * m_entry_size], m_entry_size};
    }

    cspan get_token_mem(size_t i) const
    {
        C4_ASSERT(i >= 0 && i < m_num_tokens);
        return {&m_tokens[i * m_entry_size], m_entry_size};
    }

    TokenType const& get_type(size_t i) const
    {
        cspan mem = get_token_mem(i);
        size_t type_pos;
        memcpy(&type_pos, mem.str, sizeof(size_t));
        auto const& type = TokenRegistry::get_type(type_pos);
        return type;
    }

    TokenBase *get(size_t i)
    {
        span mem = get_token_mem(i);
        size_t type_pos;
        memcpy(&type_pos, mem.str, sizeof(size_t));
        auto const& type = TokenRegistry::get_type(type_pos);
        mem = mem.subspan(sizeof(size_t));
        TokenBase *obj = type.get_ptr(mem);
        return obj;
    }

    TokenBase const* get(size_t i) const
    {
        return const_cast< TokenBase const* >(const_cast< TokenContainer* >(this)->get(i));
    }

    size_t get_id(TokenBase const* ptr) const
    {
        char const* address = reinterpret_cast< char const* >(ptr);
        address -= sizeof(size_t);
        return (address - m_tokens.data()) / m_entry_size;
    }

    size_t next_token(cspan *rem, TplLocation *loc)
    {
        C4_ASSERT(TokenRegistry::get_max_size() == m_max_size);
        auto result = TokenRegistry::match_any(*rem);
        if( ! result) return NONE;
        size_t id = m_num_tokens++;
        m_tokens.resize(m_tokens.size() + m_entry_size);
        span entry = get_token_mem(id);
        memcpy(entry.str, &result.which, sizeof(size_t));
        span mem = entry.subspan(sizeof(size_t));
        auto const& type = TokenRegistry::get_type(result.which);
        /*TokenBase *obj = */type.create(mem, id);
        *rem = rem->subspan(result.pos);
        loc->m_rope_pos.i += result.pos;
        return id;
    }

public:

    template< class TkC, class TkB >
    struct token_iterator_impl
    {
        TkC *this_;
        size_t i;

        using value_type = TkB;

        token_iterator_impl(TkC* t, size_t i_) : this_(t), i(i_) {}

        token_iterator_impl operator++ () { ++i; return *this; }
        token_iterator_impl operator-- () { ++i; return *this; }

        value_type* operator-> () { return  this_->get(i); }
        value_type& operator*  () { return *this_->get(i); }

        bool operator!= (token_iterator_impl that) const { C4_ASSERT(this_ == that.this_); return i != that.i; }
        bool operator== (token_iterator_impl that) const { C4_ASSERT(this_ == that.this_); return i == that.i; }
    };

    using iterator = token_iterator_impl< TokenContainer, TokenBase >;
    using const_iterator = token_iterator_impl< const TokenContainer, const TokenBase >;

    iterator begin() { return iterator(this, 0); }
    iterator end  () { return iterator(this, m_num_tokens); }

    const_iterator begin() const { return const_iterator(this, 0); }
    const_iterator end  () const { return const_iterator(this, m_num_tokens); }

};

} // namespace tpl
} // namespace c4

#ifdef __GNUC__
#   pragma GCC diagnostic pop
#endif

#endif /* _C4_TPL_TOKEN_CONTAINER_HPP_ */
