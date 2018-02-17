#include "../rope.hpp"

namespace c4 {
namespace tpl {


TEST(rope, raw_replace_empty_at_beginning)
{
    cspan ss("some selected words");
    Rope rp;
    rp.append(ss);
    EXPECT_EQ(rp.num_entries(), 1);
    EXPECT_EQ(rp.get(0)->s, ss);
    rp.replace(0, 0, 0, "write down ");
    EXPECT_EQ(rp.num_entries(), 2);
    size_t c = rp.head();
    EXPECT_EQ(rp.get(c)->s, "write down "); c = rp.next(c); EXPECT_NE(c, NONE);
    EXPECT_EQ(rp.get(c)->s, "some selected words"); c = rp.next(c); EXPECT_EQ(c, NONE);
}

TEST(rope, raw_replace_nonempty_at_beginning)
{
    cspan ss("some selected words");
    Rope rp;
    rp.append(ss);
    EXPECT_EQ(rp.num_entries(), 1);
    EXPECT_EQ(rp.get(0)->s, ss);
    rp.replace(0, 0, 4, "write down these carefully");
    EXPECT_EQ(rp.num_entries(), 2);
    size_t c = rp.head();
    EXPECT_EQ(rp.get(c)->s, "write down these carefully"); c = rp.next(c); EXPECT_NE(c, NONE);
    EXPECT_EQ(rp.get(c)->s, " selected words"); c = rp.next(c); EXPECT_EQ(c, NONE);
}

TEST(rope, raw_replace_empty_at_middle)
{
    cspan ss("some selected words");
    Rope rp;
    rp.append(ss);
    EXPECT_EQ(rp.num_entries(), 1);
    EXPECT_EQ(rp.get(0)->s, ss);
    rp.replace(0, 5, 0, "carefully ");
    EXPECT_EQ(rp.num_entries(), 3);
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
    EXPECT_EQ(rp.num_entries(), 1);
    EXPECT_EQ(rp.get(0)->s, ss);
    rp.replace(0, 5, 8, "carefully crafted");
    EXPECT_EQ(rp.num_entries(), 3);
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
    EXPECT_EQ(rp.num_entries(), 1);
    EXPECT_EQ(rp.get(0)->s, ss);
    rp.replace(0, ss.len, 0, " before committing");
    EXPECT_EQ(rp.num_entries(), 2);
    size_t c = rp.head();
    EXPECT_EQ(rp.get(c)->s, "some selected words"); c = rp.next(c); EXPECT_NE(c, NONE);
    EXPECT_EQ(rp.get(c)->s, " before committing"); c = rp.next(c); EXPECT_EQ(c, NONE);
}

TEST(rope, raw_replace_nonempty_at_end)
{
    cspan ss("some selected words");
    Rope rp;
    rp.append(ss);
    EXPECT_EQ(rp.num_entries(), 1);
    EXPECT_EQ(rp.get(0)->s, ss);
    rp.replace(0, 5, ss.len-5, "more commits before pushing");
    EXPECT_EQ(rp.num_entries(), 2);
    size_t c = rp.head();
    EXPECT_EQ(rp.get(c)->s, "some "); c = rp.next(c); EXPECT_NE(c, NONE);
    EXPECT_EQ(rp.get(c)->s, "more commits before pushing"); c = rp.next(c); EXPECT_EQ(c, NONE);
}

TEST(rope, basic)
{
    cspan s(R"(
this is a long text {% t %}
with some tokens in it {% t %}
{% t %}
and yet some more
{% t %}
but not this one {% u %}
)");
    Rope r;
    r.append(s);

    r.append("\n\nand more lines still {% t %}\n\n{% t %}\n\n{% u %}\n\n{% t %}\n");
    std::cout << "\n\nBEFORE:\n" << r << "\n";

    r.replace_all("{% t %}", "______");
    r.replace_all("{% u %}", "tututu");
    r.replace_all("yet", "asdkjasdkjhaskdjasd");

    std::cout << "\n\nAFTER:\n" << r << "\n";

    r.insert_after("is a", " REALLY");
    std::cout << "\n\nAFTER:\n" << r << "\n";

    r.insert_before("REALLY", "really ");

    std::cout << "\n\nAFTER:\n" << r << "\n";

    r.insert_before_all("______", "^^^^");
    std::cout << "\n\nAFTER:\n" << r << "\n";

    r.insert_after_all("______", "^^^^");
    std::cout << "\n\nAFTER:\n" << r << "\n";
}


} // namespace tpl
} // namespace c4
