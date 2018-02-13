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
        if(m_src.empty()) return;
        TplLocation pos{&m_rope, {m_rope.append(src), 0}};
        cspan rem = m_src;
        while( ! rem.empty())
        {
            auto *tk = m_tokens.next_token(&rem, &pos);
            if( ! tk) break; // we're done
            tk->parse(&rem, &pos);
        }
    }

    void render(Tree const& t, Rope *r)
    {
        auto root = t.rootref();
        for(auto const& token : m_tokens)
        {
            cspan val = {};
            token.resolve(root, &val);
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
foo is active!
# a for here

)");
}

} // namespace tpl
} // namespace c4

#endif /* _C4_TPL_ENGINE_HPP_ */
