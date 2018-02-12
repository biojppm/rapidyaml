#ifndef _C4_TPL_ENGINE_HPP_
#define _C4_TPL_ENGINE_HPP_

#include "./token.hpp"

namespace c4 {
namespace tpl {

class NewEngine
{
public:

    cspan m_src;
    Rope m_rope;
    TokenContainer m_tokens;

public:

    NewEngine() : m_src(), m_rope(), m_tokens() {}

    Rope const& rope() { return m_rope; }

    void parse(cspan src)
    {
        m_src = src;

        TplLocation pos{&m_rope, {m_rope.append(src), 0}};
        cspan rem = m_src;
        while( ! rem.empty())
        {
            auto *tk = m_tokens.parse_next(&rem, &pos);
            if( ! tk) break; // we're done
            tk->parse(&rem, &pos);
        }
    }

    void render(Tree const& t, Rope *r)
    {
        auto root = t.rootref();
        for(auto const& token : m_tokens)
        {
            cspan val = token.resolve(root);
            r->replace(token.rope_entry(), val);
        }
    }

    Rope const& render(Tree const& tree)
    {
        render(tree, &m_rope);
        return m_rope;
    }

    span chain_all(span buf)
    {
        return m_rope.chain_all(buf);
    }

};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef enum
{
    VAL,
    IF,
    FOR,
    COMMENT
} TokenType_e;

struct ValData
{
    cspan name;
};

struct ForData
{
    cspan var;
    cspan value;
    cspan body;
};

struct IfData
{
    cspan cond;
    cspan if_branch;
    cspan else_branch;
};

struct CommentData
{
};

struct TokenData
{
    TokenType_e type;
    size_t rope_entry;
    cspan  text;
    size_t parent;
    TokenData(){}
    TokenData(TokenData const&) = default;
    TokenData(TokenData     &&) = default;
    union {
        ValData dval;
        ForData dfor;
        IfData  dif;
        CommentData dcom;
    };
};


class Engine
{
public:

    struct State
    {
        size_t rope_entry;
        void clear()
        {
            rope_entry = NONE;
        }
    };

    cspan m_src;
    Rope m_rope;
    std::vector< TokenData > m_tokens;
    State m_state;

public:

    Engine() : m_src(), m_rope(), m_tokens(), m_state() {}

    Rope const& rope() { return m_rope; }

    void parse(cspan src)
    {
        m_src = src;
        cspan rem = src;
        m_state.clear();
        m_state.rope_entry = m_rope.append(src);
        while( ! rem.empty())
        {
            auto result = rem.first_of_any("{{", "{% if", "{% for", "{# ");
            switch(result.which)
            {
            case 0: // "{{"
            {
                auto const& d = _parse_val(rem, result.pos);
                rem = rem.subspan(d.text.end() - rem.begin());
                break;
            }
            case 1: // "{% if"
                C4_ERROR("not implemented");
                break;
            case 2: // "{% for"
                C4_ERROR("not implemented");
                break;
            default:
                // we're done
                rem = rem.subspan(rem.len);
            }
        }
    }

    void render(Tree const& t, Rope *r)
    {
        auto root = t.rootref();
        for(auto const& token : m_tokens)
        {
            switch(token.type)
            {
            case VAL:
            {
                cspan val = _resolve_val(root, token.dval.name);
                r->replace(token.rope_entry, val);
                break;
            }
            default:
                C4_ERROR("not implemented");
            }
        }
    }

    Rope const& render(Tree const& tree)
    {
        render(tree, &m_rope);
        return m_rope;
    }

    TokenData const& _parse_val(cspan rem, size_t b)
    {
        C4_ASSERT(rem.subspan(b).begins_with("{{"));
        m_tokens.emplace_back();
        size_t e = rem.find("}}");
        C4_ASSERT(e != npos && e >= b);
        C4_ASSERT(b+2 <= e);
        C4_ASSERT(b+4 <= rem.len);
        TokenData &d = m_tokens.back();
        d.type = VAL;
        d.text = rem.subspan(b, e+2 -b);
        d.rope_entry = m_rope.replace(m_state.rope_entry, b, e+2 - b, "<<<val>>>");
        d.dval.name = rem.subspan(b+2, e-b-2).trim(" ");
        m_state.rope_entry = m_rope.next(d.rope_entry);
        return d;
    }

    span chain_all(span buf)
    {
        return m_rope.chain_all(buf);
    }

    cspan _resolve_val(NodeRef const& root, cspan key) const
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
                        subkey = _resolve_val(n, subkey);
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

TEST(engine, basic)
{
    std::vector< char > output_buf(1024);
    span outbuf(output_buf.data(), output_buf.size());

    c4::tpl::NewEngine eng;
    eng.parse(R"(
foo={{foo}}
bar={{bar}}
baz={{baz}}
seq[0]={{seq[0]}}
seq[1]={{seq[1]}}
seq[2]={{seq[2]}}
nested.foo={{nested.foo}}
nested.very.bar={{nested.very.bar}}
nested.very.deeply.baz={{nested.very.deeply.baz}}
foo={{ foo }}
bar={{ bar }}
baz={{ baz }}
seq[0]={{ seq[0] }}
seq[1]={{ seq[1] }}
seq[2]={{ seq[2] }}
nested.foo={{ nested.foo }}
nested.very.bar={{ nested.very.bar }}
nested.very.deeply.baz={{ nested.very.deeply.baz }}
# an if here
{% if foo %}
foo is active!
{% endif %}
# a for here
{% for s in seq %}
   - s
{% endfor %}
)");

    span ret = eng.chain_all(outbuf);
    EXPECT_EQ(ret, R"(
foo=<<<expr>>>
bar=<<<expr>>>
baz=<<<expr>>>
seq[0]=<<<expr>>>
seq[1]=<<<expr>>>
seq[2]=<<<expr>>>
nested.foo=<<<expr>>>
nested.very.bar=<<<expr>>>
nested.very.deeply.baz=<<<expr>>>
foo=<<<expr>>>
bar=<<<expr>>>
baz=<<<expr>>>
seq[0]=<<<expr>>>
seq[1]=<<<expr>>>
seq[2]=<<<expr>>>
nested.foo=<<<expr>>>
nested.very.bar=<<<expr>>>
nested.very.deeply.baz=<<<expr>>>
# an if here
<<<if>>>
# a for here
<<<for>>>
)");

    char ysrc[] = R"({
foo: 0,
bar: 1,
seq: [c0, c1, c2],
nested: {
  foo: 10,
  very: {
    bar: 100,
    deeply: {
      baz: 1000
    }
  }
}})";
    Tree tree = c4::yml::parse(ysrc);
    print_tree(tree);

    eng.render(tree);
    ret = eng.chain_all(outbuf);
    std::cout << ret;
    EXPECT_EQ(ret, R"(
foo=0
bar=1
baz=
seq[0]=c0
seq[1]=c1
seq[2]=c2
nested.foo=10
nested.very.bar=100
nested.very.deeply.baz=1000
foo=0
bar=1
baz=
seq[0]=c0
seq[1]=c1
seq[2]=c2
nested.foo=10
nested.very.bar=100
nested.very.deeply.baz=1000
# an if here

# a for here

)");
}

} // namespace tpl
} // namespace c4

#endif /* _C4_TPL_ENGINE_HPP_ */
