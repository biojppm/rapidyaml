#ifndef _C4_TPL_TOKEN_HPP_
#define _C4_TPL_TOKEN_HPP_

#include "./token_container.hpp"

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

inline void register_known_tokens()
{
    C4_REGISTER_TOKEN(TokenExpression);
    C4_REGISTER_TOKEN(TokenIf);
    C4_REGISTER_TOKEN(TokenFor);
    C4_REGISTER_TOKEN(TokenComment);
}

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

    virtual void parse(cspan *rem, TplLocation *curr_pos);

    virtual void parse_body(TokenContainer */*cont*/) const {}

    virtual bool resolve(NodeRef const& /*n*/, cspan *value) const
    {
        *value = {};
        return false;
    }


    bool eval(NodeRef const& root, cspan key, cspan *result) const;

    void mark();

    cspan subspan() const { return m_start.m_rope->subspan(m_rope_entry, 0); }

protected:

    void _do_parse_body(cspan body, TplLocation pos, TokenContainer *cont) const;

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

    virtual bool resolve(NodeRef const& root, cspan *value) const override
    {
        return this->eval(root, m_expr, value);
    }

    virtual void parse_body(TokenContainer *cont) const override
    {
        _do_parse_body(m_expr, m_start, cont);
    }

};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
class TokenFilter : public TokenBase
{
    C4_DECLARE_TOKEN(TokenFilter);
    virtual cspan const& stoken() const override { static const cspan s("|", 1); return s; }
    virtual cspan const& etoken() const override { static const cspan s(" ", 3); return s; }
    virtual cspan const& marker() const override { static const cspan s("<<<filter>>>", 12); return s; }
};
*/


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class TokenComment : public TokenBase
{
public:
    C4_DECLARE_TOKEN(TokenComment);
    virtual cspan const& stoken() const override { static const cspan s("{# ", 3); return s; }
    virtual cspan const& etoken() const override { static const cspan s(" #}", 3); return s; }
    virtual cspan const& marker() const override { static const cspan s("<<<cmt>>>", 9); return s; }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

struct IfCondition
{

    typedef enum {
        _INVALID,
        ARG,             // foo: true if arg value is not empty
        ARG_IN_CMP,      // foo in bar
        ARG_NOT_IN_CMP,  // foo not in bar
        ARG_EQ_CMP,      // foo == bar
        ARG_NE_CMP,      // foo != bar
        ARG_GE_CMP,      // foo >= bar
        ARG_GT_CMP,      // foo >  bar
        ARG_LE_CMP,      // foo <= bar
        ARG_LT_CMP,      // foo <  bar
    } ConditionType_e;

    cspan m_str;
    cspan m_arg;
    cspan m_argval;
    cspan m_cmp;
    cspan m_cmpval;
    TokenIf const* m_token;
    ConditionType_e m_ctype;

    void init(TokenIf *tk, cspan str)
    {
        C4_ASSERT( ! str.begins_with("{% if"));
        m_str = str;
        m_token = tk;
        parse();
    }

    void eval(NodeRef const& root);

    bool resolve(NodeRef const& root);

    void parse();

};

//-----------------------------------------------------------------------------
/**
@begincode
{% if kenny.sick %}
    Kenny is sick.
{% elif kenny.dead %}
    You killed Kenny!  You bastard!!!
{% else %}
    Kenny looks okay --- so far
{% endif %}
@endcode
 */
class TokenIf : public TokenBase
{
public:

    C4_DECLARE_TOKEN(TokenIf);

    using base_type = TokenBase;

public:

    virtual cspan const& stoken() const override { static const cspan s("{% if ", 6); return s; }
    virtual cspan const& etoken() const override { static const cspan s("{% endif %}", 11); return s; }
    virtual cspan const& marker() const override { static const cspan s("<<<if>>>", 8); return s; }

public:

    struct condblock
    {
        mutable IfCondition condition;
        cspan block;
        TplLocation start;
    };

    std::vector< condblock > m_cond_blocks;
    cspan                    m_else_block;
    size_t                   m_else_block_offs;

    void parse(cspan *rem, TplLocation *curr_pos) override;

    void parse_body(TokenContainer *cont) const override;

    static cspan _scan_condition(cspan token, cspan *s);

    virtual bool resolve(NodeRef const& root, cspan *value) const override;
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
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
