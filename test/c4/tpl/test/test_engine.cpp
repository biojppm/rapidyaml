#include "../engine.hpp"
#include "c4/yml/std/vector.hpp"

namespace c4 {
namespace tpl {

struct tpl_results { cspan props_yml, result; };
using tpl_cases = std::initializer_list< tpl_results >;

void do_engine_test(cspan tpl, cspan parsed_tpl, tpl_cases cases)
{
    std::vector< char > parsed_tpl_buf;
    std::vector< char > parsed_yml_buf;
    std::vector< char > result_buf;

    c4::tpl::Engine eng;
    eng.parse(tpl);
    cspan ret = eng.rope().chain_all_resize(&parsed_tpl_buf);
    (void)parsed_tpl;//EXPECT_EQ(ret, parsed_tpl);

    c4::yml::Tree tree;

    for(auto const& c : cases)
    {
        parsed_yml_buf.assign(c.props_yml.begin(), c.props_yml.end());
        tree = c4::yml::parse(to_span(parsed_yml_buf));
        print_tree(tree);
        eng.render(tree);
        ret = eng.rope().chain_all_resize(&result_buf);
        EXPECT_EQ(ret, c.result);
    }
}

//-----------------------------------------------------------------------------

TEST(engine, expr)
{
    do_engine_test("foo is {{foo}}",
                   "foo is <<<expr>>>",
                   tpl_cases{
                       {"{foo: 1}", "foo is 1"},
                       {"{foo: 2}", "foo is 2"},
                       {"{foo: 10}", "foo is 10"},
                   });
}

TEST(engine, expr2)
{
    do_engine_test("foo is {{foo}}, bar is {{bar}}",
                   "foo is <<<expr>>>, bar is <<<expr>>>",
                   tpl_cases{
                       {"{foo: 1, bar: 10}", "foo is 1, bar is 10"},
                       {"{foo: 2, bar: 20}", "foo is 2, bar is 20"},
                       {"{foo: 3, bar: 30}", "foo is 3, bar is 30"},
                   });

}

//-----------------------------------------------------------------------------
TEST(engine, if_simple)
{
    do_engine_test("{% if foo %}bar{% endif %}",
                   "<<<if>>>",
                   tpl_cases{
                       {"{}", ""},
                       {"{foo: 2}", "bar"},
                   });
}

TEST(engine, if_simple_empty)
{
    do_engine_test("{% if foo %}{% endif %}",
                   "<<<if>>>",
                   tpl_cases{
                       {"{}", ""},
                       {"{foo: 2}", ""},
                   });
}

TEST(engine, if_else_simple)
{
    do_engine_test("{% if foo %}foo{% else %}bar{% endif %}",
                   "<<<if>>>",
                   tpl_cases{
                       {"{}", "bar"},
                       {"{foo: 2}", "foo"},
                   });
}

TEST(engine, if_elif_simple)
{
    do_engine_test("{% if foo %}foo{% elif bar %}bar{% endif %}",
                   "<<<if>>>",
                   tpl_cases{
                       {"{}", ""},
                       {"{foo: 2}", "foo"},
                       {"{bar: 2}", "bar"},
                       {"{foo: 2, bar: 2}", "foo"},
                   });
}

TEST(engine, if_elif_else_simple)
{
    do_engine_test("{% if foo %}foo{% elif bar %}bar{% else %}baz{% endif %}",
                   "<<<if>>>",
                   tpl_cases{
                       {"{}", "baz"},
                       {"{foo: 2}", "foo"},
                       {"{bar: 2}", "bar"},
                       {"{baz: 2}", "baz"},
                       {"{foo: 2, bar: 2}", "foo"},
                       {"{bar: 2, baz: 2}", "bar"},
                   });
}


//-----------------------------------------------------------------------------
TEST(engine, basic)
{
    do_engine_test(R"(
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
)",
                   R"(
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
)",
                   tpl_cases{
                       {
                           R"({
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
}})",
                            R"(
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

)"

                               },

                   }
        );
}

} // namespace tpl
} // namespace c4
