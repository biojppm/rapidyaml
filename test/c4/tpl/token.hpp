#ifndef _C4_TPL_TOKEN_HPP_
#define _C4_TPL_TOKEN_HPP_

#include <vector>

namespace c4 {
namespace tpl {

// try to do it like this, it's really well done:
// http://jinja.pocoo.org/docs/2.10/templates/

using Tree = c4::yml::Tree;
using NodeRef = c4::yml::NodeRef;

class TokenBase;
class TokenExpression;
class TokenIf;
class TokenFor;
class TokenComment;

struct TplLocation
{
    Rope *         m_rope;
    Rope::rope_pos m_rope_pos;
    //! @todo:
    //size_t         m_offset;
    //size_t         m_line;
    //size_t         m_column;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/** allow user-registered tokens */
#define C4_DECLARE_TOKEN(cls)                           \
    static TokenType _get_type()                        \
    {                                                   \
        return {_get_name(),                            \
                sizeof(cls),                            \
                &_create_inplace,                       \
                &_destroy_inplace,                      \
                &_get_ptr_inplace};                     \
    }                                                   \
    static const char* _get_name()                      \
    {                                                   \
        return #cls;                                    \
    }                                                   \
    static TokenBase* _create_inplace(span mem)         \
    {                                                   \
        C4_ASSERT(sizeof(cls) <= mem.len);              \
        cls *ptr = new ((void*)mem.str) cls();          \
        return ptr;                                     \
    }                                                   \
    static void _destroy_inplace(span mem)              \
    {                                                   \
        C4_ASSERT(sizeof(cls) <= mem.len);              \
        cls *ptr = reinterpret_cast< cls* >(mem.str);   \
        ptr->~cls();                                    \
    }                                                   \
    static TokenBase* _get_ptr_inplace(span mem)        \
    {                                                   \
        C4_ASSERT(sizeof(cls) <= mem.len);              \
        cls *ptr = reinterpret_cast< cls* >(mem.str);   \
        return ptr;                                     \
    }

#define C4_REGISTER_TOKEN(cls) TokenRegistry::register_token< cls >()


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

struct TokenType
{
    using inplace_create  = TokenBase* (*)(span mem);
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

/** */
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
        }
        m_max_size = TokenRegistry::get_max_size();
        m_entry_size = sizeof(size_t) + m_max_size;
        m_tokens.reserve(cap * m_entry_size);
    }

    span get_token_span(size_t i)
    {
        C4_ASSERT(i >= 0 && i < m_num_tokens);
        return {&m_tokens[i * m_entry_size], m_entry_size};
    }

    cspan get_token_span(size_t i) const
    {
        C4_ASSERT(i >= 0 && i < m_num_tokens);
        return {&m_tokens[i * m_entry_size], m_entry_size};
    }

    TokenType const& get_type(size_t i) const
    {
        cspan mem = get_token_span(i);
        size_t type_pos;
        memcpy(&type_pos, mem.str, sizeof(size_t));
        auto const& type = TokenRegistry::get_type(type_pos);
        return type;
    }

    TokenBase *get(size_t i)
    {
        span mem = get_token_span(i);
        size_t type_pos;
        memcpy(&type_pos, mem.str, sizeof(size_t));
        auto const& type = TokenRegistry::get_type(type_pos);
        mem = mem.subspan(sizeof(size_t));
        TokenBase *obj = type.get_ptr(mem);
        return obj;
    }

    TokenBase *parse_next(cspan *rem, TplLocation *loc)
    {
        C4_ASSERT(TokenRegistry::get_max_size() == m_max_size);
        auto result = TokenRegistry::match_any(*rem);
        if( ! result) return nullptr;
        size_t pos = m_num_tokens++;
        m_tokens.resize(m_tokens.size() + m_entry_size);
        span entry = get_token_span(pos);
        memcpy(entry.str, &result.which, sizeof(size_t));
        span mem = entry.subspan(sizeof(size_t));
        auto const& type = TokenRegistry::get_type(result.which);
        TokenBase *obj = type.create(mem);
        *rem = rem->subspan(result.pos);
        loc->m_rope_pos.i = result.pos;
        return obj;
    }

public:

    struct iterator
    {
        TokenContainer *this_;
        size_t i;

        using value_type = TokenBase*;

        iterator(TokenContainer* t, size_t i_) : this_(t), i(i_) {}

        iterator operator++ () { ++i; return *this; }
        iterator operator-- () { ++i; return *this; }

        TokenBase* operator-> () { return  this_->get(i); }
        TokenBase& operator*  () { return *this_->get(i); }

        bool operator!= (iterator that) const { C4_ASSERT(this_ == that.this_); return i != that.i; }
        bool operator== (iterator that) const { C4_ASSERT(this_ == that.this_); return i == that.i; }
    };

    iterator begin() { return iterator(this, 0); }
    iterator end() { return iterator(this, m_num_tokens); }

public:

    static void register_known_tokens()
    {
        C4_REGISTER_TOKEN(TokenExpression);
        C4_REGISTER_TOKEN(TokenIf);
        C4_REGISTER_TOKEN(TokenFor);
        C4_REGISTER_TOKEN(TokenComment);
    }

};



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class TokenBase
{
public:

    virtual ~TokenBase() {}
    virtual cspan const& stoken() const = 0;
    virtual cspan const& etoken() const = 0;
    virtual cspan const& marker() const = 0;

    TplLocation m_start;
    TplLocation m_end;
    size_t m_rope_entry;

    cspan m_full_text;
    cspan m_interior_text;

    size_t rope_entry() const { return m_rope_entry; }

    virtual void parse(cspan *rem, TplLocation *curr_pos)
    {
        // todo deal with nested tokens
        auto const &s = stoken(), &e = etoken();
        C4_ASSERT(rem->begins_with(s));
        m_start = *curr_pos;
        auto pos = rem->find(e);
        C4_ASSERT(pos != npos);
        C4_ASSERT(pos + e.len <= rem->len);
        m_full_text = rem->subspan(0, pos + e.len);
        m_interior_text = m_full_text.subspan(e.len, m_full_text.len - e.len - s.len);

        auto &rp = curr_pos->m_rope_pos;
        C4_ASSERT(curr_pos->m_rope->get(rp.entry)->s.len >= m_full_text.len);
        rp.entry = curr_pos->m_rope->replace(rp.entry, rp.i, m_full_text.len, this->marker());
        m_rope_entry = rp.entry;
        rp.entry = curr_pos->m_rope->next(rp.entry);
        rp.i = 0;
        m_end = *curr_pos;

        *rem = rem->subspan(m_full_text.len);
    }
    virtual cspan resolve(NodeRef const& /*n*/) const
    {
        return {};
    }
};



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class TokenExpression : public TokenBase
{
public:

    C4_DECLARE_TOKEN(TokenExpression);

    using base_type = TokenBase;

public:

    virtual cspan const& stoken() const override { static const cspan s("{{", 2); return s; }
    virtual cspan const& etoken() const override { static const cspan s("}}", 2); return s; }
    virtual cspan const& marker() const override { static const cspan s("<<<expr>>>", 10); return s; }

    cspan m_expr;

    void parse(cspan *rem, TplLocation *curr_pos) override
    {
        base_type::parse(rem, curr_pos);
        m_expr = m_interior_text.trim(" ");
    }

    virtual cspan resolve(NodeRef const& root) const override
    {
        return _resolve(root, m_expr);
    }

private:

    cspan _resolve(NodeRef const& root, cspan key) const
    {
        C4_ASSERT(root.valid());
        NodeRef n = root;
        do {
            auto pos = key.find('.');
            if(pos != npos)
            {
                cspan left = key.left_of(pos);
                n = n.find_child(left);
                key = key.right_of(pos);
            }
            else
            {
                pos = key.find('[');
                if(pos != npos)
                {
                    cspan left = key.left_of(pos);
                    if( ! left.empty())
                    {
                        n = n.find_child(left);
                        if( ! n.valid())
                        {
                            break;
                        }
                        key = key.right_of(pos);
                        pos = key.find(']');
                        C4_ASSERT(pos != npos);
                        cspan subkey = key.left_of(pos);
                        key = key.right_of(pos);
                        subkey = _resolve(n, subkey);
                        if(subkey.empty())
                        {
                            break;
                        }
                        else
                        {
                            return subkey;
                        }
                    }
                }
                else
                {
                    if(key.begins_with_any("0123456789"))
                    {
                        size_t num;
                        bool ret = from_str(key, &num);
                        if(ret)
                        {
                            if(n.num_children() >= num)
                            {
                                n = n[num];
                                key.clear();
                            }
                        }
                    }
                    else
                    {
                        n = n.find_child(key);
                        key.clear();
                    }
                }
            }
        } while( ! key.empty() && n.valid());
        return n.valid() ? n.val() : cspan();
    }
};

/*
class TokenFilter : public TokenBase
{
    C4_DECLARE_TOKEN(TokenFilter);
    virtual cspan const& stoken() const override { static const cspan s("|", 1); return s; }
    virtual cspan const& etoken() const override { static const cspan s(" ", 3); return s; }
    virtual cspan const& marker() const override { static const cspan s("<<<filter>>>", 12); return s; }
};
*/

class TokenComment : public TokenBase
{
public:
    C4_DECLARE_TOKEN(TokenComment);
    virtual cspan const& stoken() const override { static const cspan s("{# ", 3); return s; }
    virtual cspan const& etoken() const override { static const cspan s(" #}", 3); return s; }
    virtual cspan const& marker() const override { static const cspan s("<<<cmt>>>", 9); return s; }
};

class TokenIf : public TokenBase
{
public:
    C4_DECLARE_TOKEN(TokenIf);
    virtual cspan const& stoken() const override { static const cspan s("{% if ", 6); return s; }
    virtual cspan const& etoken() const override { static const cspan s("{% endif %}", 11); return s; }
    virtual cspan const& marker() const override { static const cspan s("<<<if>>>", 8); return s; }
};

class TokenFor : public TokenBase
{
public:
    C4_DECLARE_TOKEN(TokenFor);
    virtual cspan const& stoken() const override { static const cspan s("{% for ", 7); return s; }
    virtual cspan const& etoken() const override { static const cspan s("{% endfor %}", 12); return s; }
    virtual cspan const& marker() const override { static const cspan s("<<<for>>>", 9); return s; }
};

} // namespace tpl
} // namespace c4

#endif /* _C4_TPL_TOKEN_HPP_ */
