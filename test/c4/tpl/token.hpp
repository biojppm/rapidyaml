#ifndef _C4_TPL_TOKEN_HPP_
#define _C4_TPL_TOKEN_HPP_

#include <c4/yml/tree.hpp>
#include <c4/yml/node.hpp>
#include "./token_container.hpp"

namespace c4 {
namespace tpl {

// try to do it like this, it's really well done:
// http://jinja.pocoo.org/docs/2.10/templates/

using Tree = c4::yml::Tree;
using NodeRef = c4::yml::NodeRef;

struct TemplateBlock;

class TokenBase;

class TokenExpression;
class TokenIf;
class TokenFor;
class TokenComment;


inline void register_known_tokens()
{
    C4TPL_REGISTER_TOKEN(TokenExpression);
    C4TPL_REGISTER_TOKEN(TokenIf);
    C4TPL_REGISTER_TOKEN(TokenFor);
    C4TPL_REGISTER_TOKEN(TokenComment);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class TokenBase
{
public:

    virtual ~TokenBase() = default;

    virtual cspan const& stoken() const = 0;
    virtual cspan const& etoken() const = 0;
    virtual cspan const& marker() const = 0;

public:

    bool m_root_level{true};
    size_t m_id;

    TplLocation m_start;
    TplLocation m_end;
    size_t m_rope_entry;

    cspan m_full_text;
    cspan m_interior_text;

public:

    Rope * rope() const { return m_start.m_rope; }
    size_t rope_entry() const { return m_rope_entry; }

    virtual void parse(cspan *rem, TplLocation *curr_pos);

    virtual void parse_body(TokenContainer * /*cont*/) const {}

    virtual bool resolve(NodeRef const& /*n*/, cspan *value) const
    {
        *value = {};
        return false;
    }

    virtual size_t render(NodeRef & root, Rope *rope) const
    {
        cspan val = {};
        resolve(root, &val);
        rope->replace(m_rope_entry, val);
        return m_rope_entry;
    }

    virtual size_t duplicate(NodeRef & /*root*/, Rope * /*rope*/, size_t /*start_entry*/) const
    {
        // empty by default
        return m_rope_entry;
    }

    static NodeRef get_property(NodeRef const& root, cspan name);

    bool eval(NodeRef const& root, cspan key, cspan *result) const;

    void mark();

    cspan subspan() const { return m_start.m_rope->subspan(m_rope_entry, 0); }

    cspan skip_nested(cspan s) const;

    virtual TemplateBlock* get_block(size_t bid) = 0;

protected:

};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class TokenExpression : public TokenBase
{
public:

    C4TPL_DECLARE_TOKEN(TokenExpression, "{{", "}}", "<<<expr>>>");

public:

    cspan  m_expr;
    size_t m_expr_offs;

    void parse(cspan *rem, TplLocation *curr_pos) override
    {
        cspan orig = *rem; (void)orig;
        base_type::parse(rem, curr_pos);
        m_expr = m_interior_text.trim(" ");
        C4_ASSERT(orig.contains(m_expr));
        m_expr_offs = m_expr.begin() - orig.begin();
    }

    void parse_body(TokenContainer * /*cont*/) const override
    {
        C4_ASSERT(m_expr.find('|') == npos && "filters not implemented");
    }

    bool resolve(NodeRef const& root, cspan *value) const override
    {
        return this->eval(root, m_expr, value);
    }

    size_t duplicate(NodeRef & root, Rope *rope, size_t start_entry) const override
    {
        cspan val = {};
        resolve(root, &val);
        size_t insert_entry = rope->insert_after(start_entry, val);
        return insert_entry;
    }

    TemplateBlock* get_block(size_t /*bid*/) override { C4_ERROR("never call"); return nullptr; }

};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
class TokenFilter : public TokenBase
{
    C4_DECLARE_TOKEN(TokenFilter, "|", " ", "<<<filter>>>");

    TemplateBlock* get_block(size_t bid) override { C4_ERROR("never call"); return nullptr; }

};
*/


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class TokenComment : public TokenBase
{
public:

    C4TPL_DECLARE_TOKEN(TokenComment, "{#", "#}", "<<<cmt>>>");

    TemplateBlock* get_block(size_t /*bid*/) override { C4_ERROR("never call"); return nullptr; }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

struct TemplateBlock
{
    struct subpart
    {
        size_t entry{NONE};
        cspan body{};
        size_t token{NONE};
    };

    cspan body;
    TplLocation start;
    std::vector< subpart > parts;
    TokenContainer *tokens; // to get the tokens from their ids
    size_t owner_id, block_id; // major smell - rewrite TokenContainer to avoid relocations

    void set_body(cspan b)
    {
        body = b;
        start.m_rope->replace(start.m_rope_pos.entry, b);
    }

    void parse(TokenContainer *cont);

    size_t render(NodeRef & root, Rope *r) const;

    size_t duplicate(NodeRef & root, Rope *r, size_t start_entry) const;

    void clear(Rope *r) const;

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
        ELSE,            // else- blocks, always true
    } Type_e;

    cspan  m_str;
    cspan  m_arg;
    cspan  m_argval;
    cspan  m_cmp;
    cspan  m_cmpval;
    Type_e m_ctype;

    void init_as_else()
    {
        m_str.clear();
        m_ctype = ELSE;
    }

    void init(cspan str)
    {
        C4_ASSERT( ! str.begins_with("{% if"));
        m_str = str;
        parse();
    }

    bool resolve(TokenIf const* tk, NodeRef & root);

    void parse();
 
private:

    void _eval(TokenIf const* tk, NodeRef & root);
   };


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class TokenIf : public TokenBase
{
public:

    C4TPL_DECLARE_TOKEN(TokenIf, "{% if ", "{% endif %}", "<<<if>>>");

    void parse(cspan *rem, TplLocation *curr_pos) override;

    void parse_body(TokenContainer *cont) const override;

    static cspan _scan_condition(cspan token, cspan *s);

    bool resolve(NodeRef const& root, cspan *value) const override;

    size_t render(NodeRef & root, Rope *rope) const override;

    size_t duplicate(NodeRef & root, Rope *rope, size_t start_entry) const override;

    TemplateBlock* get_block(size_t bid) override { C4_ASSERT(bid < m_blocks.size()); return &m_blocks[bid]; }

public:

    struct condblock : public TemplateBlock
    {
        mutable IfCondition condition;
    };

    mutable std::vector< condblock > m_blocks;

    condblock* _add_block(cspan cond, cspan s, bool as_else=false);
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class TokenFor : public TokenBase
{
public:

    C4TPL_DECLARE_TOKEN(TokenFor, "{% for ", "{% endfor %}", "<<<for>>>");

    void parse(cspan *rem, TplLocation *curr_pos) override;

    void parse_body(TokenContainer *cont) const override;

    bool resolve(NodeRef const& root, cspan *value) const override;

    size_t render(NodeRef & root, Rope *rope) const override;

    size_t duplicate(NodeRef & root, Rope *rope, size_t start_entry) const override;

    TemplateBlock* get_block(size_t bid) override { C4_ASSERT(bid == 0); return &m_block; }

public:

    void _set_loop_properties(NodeRef &root, NodeRef const& var, size_t i, size_t num) const;
    void _clear_loop_properties(NodeRef &root) const;

    size_t _do_render(NodeRef& root, Rope *rope, size_t start_entry, bool duplicating) const;

public:

    mutable TemplateBlock m_block;
    cspan m_var;
    cspan m_val;
};

} // namespace tpl
} // namespace c4

#endif /* _C4_TPL_TOKEN_HPP_ */
