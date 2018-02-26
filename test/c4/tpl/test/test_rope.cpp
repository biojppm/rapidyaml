
#include <gtest/gtest.h>
#include "../rope.hpp"

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable: 4127/*conditional expression is constant*/)
#   pragma warning(disable: 4389/*'==': signed/unsigned mismatch*/)
#endif

namespace c4 {
namespace tpl {


TEST(rope, raw_replace_empty_at_beginning)
{
    cspan ss("some selected words");
    Rope rp;
    rp.append(ss);
    EXPECT_EQ(rp.num_entries(), 1u);
    EXPECT_EQ(rp.get(0)->s, ss);
    rp.replace(0, 0, 0, "write down ");
    EXPECT_EQ(rp.num_entries(), 2u);
    size_t c = rp.head();
    EXPECT_EQ(rp.get(c)->s, "write down "); c = rp.next(c); EXPECT_NE(c, NONE);
    EXPECT_EQ(rp.get(c)->s, "some selected words"); c = rp.next(c); EXPECT_EQ(c, NONE);
}

TEST(rope, raw_replace_nonempty_at_beginning)
{
    cspan ss("some selected words");
    Rope rp;
    rp.append(ss);
    EXPECT_EQ(rp.num_entries(), 1u);
    EXPECT_EQ(rp.get(0)->s, ss);
    rp.replace(0, 0, 4, "write down these carefully");
    EXPECT_EQ(rp.num_entries(), 2u);
    size_t c = rp.head();
    EXPECT_EQ(rp.get(c)->s, "write down these carefully"); c = rp.next(c); EXPECT_NE(c, NONE);
    EXPECT_EQ(rp.get(c)->s, " selected words"); c = rp.next(c); EXPECT_EQ(c, NONE);
}

TEST(rope, raw_replace_empty_at_middle)
{
    cspan ss("some selected words");
    Rope rp;
    rp.append(ss);
    EXPECT_EQ(rp.num_entries(), 1u);
    EXPECT_EQ(rp.get(0)->s, ss);
    rp.replace(0, 5, 0, "carefully ");
    EXPECT_EQ(rp.num_entries(), 3u);
    size_t c = rp.head();
    EXPECT_EQ(rp.get(c)->s, "some "); c = rp.next(c); EXPECT_NE(c, NONE);
    EXPECT_EQ(rp.get(c)->s, "carefully "); c = rp.next(c); EXPECT_NE(c, NONE);
    EXPECT_EQ(rp.get(c)->s, "selected words"); c = rp.next(c); EXPECT_EQ(c, NONE);
}

TEST(rope, raw_replace_nonempty_at_middle)
{
    cspan ss("some selected words");
    Rope rp;
    rp.append(ss);
    EXPECT_EQ(rp.num_entries(), 1u);
    EXPECT_EQ(rp.get(0)->s, ss);
    rp.replace(0, 5, 8, "carefully crafted");
    EXPECT_EQ(rp.num_entries(), 3u);
    size_t c = rp.head();
    EXPECT_EQ(rp.get(c)->s, "some "); c = rp.next(c); EXPECT_NE(c, NONE);
    EXPECT_EQ(rp.get(c)->s, "carefully crafted"); c = rp.next(c); EXPECT_NE(c, NONE);
    EXPECT_EQ(rp.get(c)->s, " words"); c = rp.next(c); EXPECT_EQ(c, NONE);
}

TEST(rope, raw_replace_empty_at_end)
{
    cspan ss("some selected words");
    Rope rp;
    rp.append(ss);
    EXPECT_EQ(rp.num_entries(), 1u);
    EXPECT_EQ(rp.get(0)->s, ss);
    rp.replace(0, ss.len, 0, " before committing");
    EXPECT_EQ(rp.num_entries(), 2u);
    size_t c = rp.head();
    EXPECT_EQ(rp.get(c)->s, "some selected words"); c = rp.next(c); EXPECT_NE(c, NONE);
    EXPECT_EQ(rp.get(c)->s, " before committing"); c = rp.next(c); EXPECT_EQ(c, NONE);
}

TEST(rope, raw_replace_nonempty_at_end)
{
    cspan ss("some selected words");
    Rope rp;
    rp.append(ss);
    EXPECT_EQ(rp.num_entries(), 1u);
    EXPECT_EQ(rp.get(0)->s, ss);
    rp.replace(0, 5, ss.len-5, "more commits before pushing");
    EXPECT_EQ(rp.num_entries(), 2u);
    size_t c = rp.head();
    EXPECT_EQ(rp.get(c)->s, "some "); c = rp.next(c); EXPECT_NE(c, NONE);
    EXPECT_EQ(rp.get(c)->s, "more commits before pushing"); c = rp.next(c); EXPECT_EQ(c, NONE);
}

TEST(rope, basic)
{
    std::vector< char > result;

    cspan s(R"(
this is a long text {% t %}
with some tokens in it {% t %}
{% t %}
and yet some more
{% t %}
but not this one {% u %}
)");
    Rope r;

    EXPECT_TRUE(r.empty());
    EXPECT_EQ(r.str_size(), 0u);
    EXPECT_EQ(r.num_entries(), 0u);

    r.append(s);

    EXPECT_FALSE(r.empty());
    EXPECT_EQ(r.str_size(), s.len);
    EXPECT_EQ(r.num_entries(), 1u);

    cspan ret = r.chain_all_resize(&result);
    EXPECT_EQ(ret, s);

    cspan p("\n\nand more lines still {% t %}\n\n{% t %}\n\n{% u %}\n\n{% t %}\n");
    r.append(p);

    EXPECT_EQ(r.str_size(), s.len + p.len);
    EXPECT_EQ(r.num_entries(), 2u);
    EXPECT_EQ(r.get(0)->s, s);
    EXPECT_EQ(r.get(1)->s, p);

    ret = r.chain_all_resize(&result);
    EXPECT_EQ(ret, R"(
this is a long text {% t %}
with some tokens in it {% t %}
{% t %}
and yet some more
{% t %}
but not this one {% u %}


and more lines still {% t %}

{% t %}

{% u %}

{% t %}
)");

    r.replace_all("{% t %}", "______");
    ret = r.chain_all_resize(&result);
    EXPECT_EQ(ret, R"(
this is a long text ______
with some tokens in it ______
______
and yet some more
______
but not this one {% u %}


and more lines still ______

______

{% u %}

______
)");

    r.replace_all("{% u %}", "tututu");
    ret = r.chain_all_resize(&result);
    EXPECT_EQ(ret, R"(
this is a long text ______
with some tokens in it ______
______
and yet some more
______
but not this one tututu


and more lines still ______

______

tututu

______
)");

    r.replace_all("yet", "asdkjasdkjhaskdjasd");
    ret = r.chain_all_resize(&result);
    EXPECT_EQ(ret, R"(
this is a long text ______
with some tokens in it ______
______
and asdkjasdkjhaskdjasd some more
______
but not this one tututu


and more lines still ______

______

tututu

______
)");


    r.insert_after("is a", " REALLY");
    ret = r.chain_all_resize(&result);
    EXPECT_EQ(ret, R"(
this is a REALLY long text ______
with some tokens in it ______
______
and asdkjasdkjhaskdjasd some more
______
but not this one tututu


and more lines still ______

______

tututu

______
)");


    r.insert_before("REALLY", "really ");
    ret = r.chain_all_resize(&result);
    EXPECT_EQ(ret, R"(
this is a really REALLY long text ______
with some tokens in it ______
______
and asdkjasdkjhaskdjasd some more
______
but not this one tututu


and more lines still ______

______

tututu

______
)");

    r.insert_before_all("______", "^^^^");
    ret = r.chain_all_resize(&result);
    EXPECT_EQ(ret, R"(
this is a really REALLY long text ^^^^______
with some tokens in it ^^^^______
^^^^______
and asdkjasdkjhaskdjasd some more
^^^^______
but not this one tututu


and more lines still ^^^^______

^^^^______

tututu

^^^^______
)");

    r.insert_after_all("______", "^^^^");
    ret = r.chain_all_resize(&result);
    EXPECT_EQ(ret, R"(
this is a really REALLY long text ^^^^______^^^^
with some tokens in it ^^^^______^^^^
^^^^______^^^^
and asdkjasdkjhaskdjasd some more
^^^^______^^^^
but not this one tututu


and more lines still ^^^^______^^^^

^^^^______^^^^

tututu

^^^^______^^^^
)");

    r.insert_before_all("this", "z");
    ret = r.chain_all_resize(&result);
    EXPECT_EQ(ret, R"(
zthis is a really REALLY long text ^^^^______^^^^
with some tokens in it ^^^^______^^^^
^^^^______^^^^
and asdkjasdkjhaskdjasd some more
^^^^______^^^^
but not zthis one tututu


and more lines still ^^^^______^^^^

^^^^______^^^^

tututu

^^^^______^^^^
)");

    r.insert_after_all("\n", "\n");
    ret = r.chain_all_resize(&result);
    EXPECT_EQ(ret, R"(

zthis is a really REALLY long text ^^^^______^^^^

with some tokens in it ^^^^______^^^^

^^^^______^^^^

and asdkjasdkjhaskdjasd some more

^^^^______^^^^

but not zthis one tututu





and more lines still ^^^^______^^^^



^^^^______^^^^



tututu



^^^^______^^^^

)");


    r.insert_after_all("\n", "-> ");
    ret = r.chain_all_resize(&result);
    EXPECT_EQ(ret, R"(
-> 
-> zthis is a really REALLY long text ^^^^______^^^^
-> 
-> with some tokens in it ^^^^______^^^^
-> 
-> ^^^^______^^^^
-> 
-> and asdkjasdkjhaskdjasd some more
-> 
-> ^^^^______^^^^
-> 
-> but not zthis one tututu
-> 
-> 
-> 
-> 
-> 
-> and more lines still ^^^^______^^^^
-> 
-> 
-> 
-> ^^^^______^^^^
-> 
-> 
-> 
-> tututu
-> 
-> 
-> 
-> ^^^^______^^^^
-> 
-> )");
}


} // namespace tpl
} // namespace c4

#if defined(_MSC_VER)
#   pragma warning(pop)
#endif
