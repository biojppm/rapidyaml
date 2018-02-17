#ifndef _C4_TPL_ENGINE_HPP_
#define _C4_TPL_ENGINE_HPP_

#include "./token.hpp"

namespace c4 {
namespace tpl {

class Engine
{
public:

    cspan m_src;
    Rope m_rope;
    TokenContainer m_tokens;

public:

    Engine() : m_src(), m_rope(), m_tokens() {}

    Rope const& rope() const { return m_rope; }

    void parse(cspan src)
    {
        m_src = src;
        if(m_src.empty()) return;
        TplLocation pos{&m_rope, {m_rope.append(src), 0}};
        cspan rem = m_src;
        while( ! rem.empty())
        {
            auto *tk = m_tokens.next_token(&rem, &pos);
            if( ! tk) break; // we're done
            tk->parse(&rem, &pos);
            tk->parse_body(&m_tokens);
        }
        //for(auto &tk : m_tokens)
        {
            //tk.mark();
        }
    }

    void render(c4::yml::NodeRef const& root, Rope *r) const
    {
        for(auto const& token : m_tokens)
        {
            cspan val = {};
            token.resolve(root, &val);
            r->replace(token.rope_entry(), val);
        }
    }

    void render(Tree const& t, Rope *r) const
    {
        render(t.rootref(), r);
    }

    Rope const& render(c4::yml::NodeRef const& root)
    {
        render(root, &m_rope);
        return m_rope;
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

TEST(engine, basic)
{
    std::vector< char > output_buf(1024);
    span outbuf(output_buf.data(), output_buf.size());

    c4::tpl::Engine eng;
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
foo is active! val={{foo}}
{% endif %}
{% if nothing %}
nothing is active!
{% elif bar %}
bar is active! val={{bar}}
{% endif %}
{% if zzz %}
zzz is active!
{% elif zzz %}
zzz is active!
{% elif seq %}
seq is not empty!
{% endif %}
{% if zzz %}
zzz is active!
{% elif c0 in seq %}
c0 is in seq!
{% endif %}
# a for here
{% for s in seq %}
   - {{s}}
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
<<<if>>>
<<<if>>>
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
foo is active! val=0
bar is active! val=1
seq is not empty!
c0 is in seq!
# a for here

)");
}

} // namespace tpl
} // namespace c4

#endif /* _C4_TPL_ENGINE_HPP_ */
