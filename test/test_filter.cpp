#ifdef RYML_SINGLE_HEADER
#include "ryml_all.hpp"
#else
#include "c4/yml/filter_processor.hpp"
#include "c4/yml/std/string.hpp"
#endif
#include <gtest/gtest.h>


namespace c4 {
namespace yml {

struct InplaceTester
{
    std::string buf;
    substr subject;
    FilterProcessorInplace proc;
    C4_NO_COPY_OR_MOVE(InplaceTester);
    InplaceTester(const char *str)
        : buf(str)
        , subject(to_substr(buf))
        , proc(subject, buf.capacity())
    {
    }
    void set_to_end(size_t slack)
    {
        ASSERT_GT(proc.wcap, 0);
        ASSERT_LE(slack, proc.wcap);
        proc.wpos = proc.wcap - slack;
    }
    void trim_capacity() { proc.wcap = buf.size(); }
};

struct SrcDstTester
{
    std::string src_;
    std::string dst_;
    csubstr src;
    substr dst;
    FilterProcessorSrcDst proc;
    C4_NO_COPY_OR_MOVE(SrcDstTester);
    SrcDstTester(const char *str)
        : src_(str)
        , dst_(str)
        , src(to_csubstr(src_))
        , dst(to_substr(dst_))
        , proc(src, dst)
    {
    }
    void set_to_end(size_t slack)
    {
        ASSERT_GT(dst.len, 0);
        ASSERT_LE(slack, dst.len);
        proc.wpos = dst.len - slack;
    }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(FilterProcessorInplace, set)
{
    InplaceTester t("subject");
    EXPECT_EQ(t.proc.rpos, 0);
    EXPECT_EQ(t.proc.wpos, 0);
    EXPECT_FALSE(t.proc.unfiltered_chars);
    EXPECT_EQ(t.proc.sofar(), "");
    t.proc.skip();
    EXPECT_EQ(t.proc.rpos, 1);
    EXPECT_EQ(t.proc.wpos, 0);
    EXPECT_EQ(t.proc.sofar(), "");
    EXPECT_EQ(t.proc.result(), "");
    t.proc.set('.');
    EXPECT_EQ(t.proc.rpos, 1);
    EXPECT_EQ(t.proc.wpos, 1);
    EXPECT_EQ(t.proc.sofar(), ".");
    EXPECT_EQ(t.proc.result(), ".");
    EXPECT_EQ(t.subject, ".ubject");
    EXPECT_FALSE(t.proc.unfiltered_chars);
    t.proc.skip(2);
    EXPECT_EQ(t.proc.rpos, 3);
    EXPECT_EQ(t.proc.wpos, 1);
    t.proc.set('.', 2);
    EXPECT_EQ(t.proc.rpos, 3);
    EXPECT_EQ(t.proc.wpos, 3);
    EXPECT_EQ(t.subject, "...ject");
    EXPECT_EQ(t.proc.sofar(), "...");
    EXPECT_EQ(t.proc.result(), "...");
    EXPECT_FALSE(t.proc.unfiltered_chars);
    t.proc.skip(3);
    t.proc.set('x', 3);
    EXPECT_EQ(t.proc.wpos, 6);
    EXPECT_EQ(t.proc.rpos, 6);
    EXPECT_EQ(t.subject, "...xxxt");
    EXPECT_EQ(t.proc.sofar(), "...xxx");
    EXPECT_EQ(t.proc.result(), "...xxx");
    EXPECT_FALSE(t.proc.unfiltered_chars);
    t.proc.skip(1);
    t.proc.set('.');
    EXPECT_EQ(t.proc.wpos, 7);
    EXPECT_EQ(t.proc.rpos, 7);
    EXPECT_EQ(t.subject, "...xxx.");
    EXPECT_EQ(t.proc.sofar(), "...xxx.");
    EXPECT_EQ(t.proc.result(), "...xxx.");
    EXPECT_FALSE(t.proc.unfiltered_chars);
}

TEST(FilterProcessorSrcDst, set)
{
    SrcDstTester t("subject");
    EXPECT_EQ(t.proc.wpos, 0);
    EXPECT_EQ(t.proc.rpos, 0);
    EXPECT_EQ(t.proc.sofar(), "");
    EXPECT_EQ(t.proc.result(), "");
    t.proc.skip();
    EXPECT_EQ(t.proc.wpos, 0);
    EXPECT_EQ(t.proc.rpos, 1);
    t.proc.set('.');
    EXPECT_EQ(t.proc.wpos, 1);
    EXPECT_EQ(t.proc.rpos, 1);
    EXPECT_EQ(t.dst, ".ubject");
    EXPECT_EQ(t.proc.sofar(), ".");
    EXPECT_EQ(t.proc.result(), ".");
    t.proc.skip(2);
    EXPECT_EQ(t.proc.rpos, 3);
    EXPECT_EQ(t.proc.wpos, 1);
    t.proc.set('.', 2);
    EXPECT_EQ(t.proc.wpos, 3);
    EXPECT_EQ(t.proc.rpos, 3);
    EXPECT_EQ(t.dst, "...ject");
    EXPECT_EQ(t.proc.sofar(), "...");
    EXPECT_EQ(t.proc.result(), "...");
    t.proc.skip(3);
    t.proc.set('x', 3);
    EXPECT_EQ(t.proc.wpos, 6);
    EXPECT_EQ(t.proc.rpos, 6);
    EXPECT_EQ(t.dst, "...xxxt");
    EXPECT_EQ(t.proc.sofar(), "...xxx");
    EXPECT_EQ(t.proc.result(), "...xxx");
}

TEST(FilterProcessorInplace, set_single_does_not_unfilter)
{
    // skip -> set
    {
        InplaceTester t("0");
        t.trim_capacity();
        EXPECT_EQ(t.proc.wcap, 1);
        EXPECT_EQ(t.proc.rpos, 0);
        EXPECT_EQ(t.proc.wpos, 0);
        EXPECT_FALSE(t.proc.unfiltered_chars);
        EXPECT_EQ(t.proc.sofar(), "");
        t.proc.skip();
        EXPECT_EQ(t.proc.rpos, 1);
        EXPECT_EQ(t.proc.wpos, 0);
        t.proc.set('a');
        EXPECT_EQ(t.proc.rpos, 1);
        EXPECT_EQ(t.proc.wpos, 1);
        EXPECT_EQ(t.proc.sofar(), "a");
        EXPECT_EQ(t.proc.result(), "a");
        EXPECT_FALSE(t.proc.unfiltered_chars);
    }
    // set -> skip
    {
        InplaceTester t("0");
        t.trim_capacity();
        EXPECT_EQ(t.proc.wcap, 1);
        EXPECT_EQ(t.proc.rpos, 0);
        EXPECT_EQ(t.proc.wpos, 0);
        EXPECT_FALSE(t.proc.unfiltered_chars);
        EXPECT_EQ(t.proc.sofar(), "");
        t.proc.set('a');
        EXPECT_EQ(t.proc.rpos, 0);
        EXPECT_EQ(t.proc.wpos, 1);
        t.proc.skip();
        EXPECT_EQ(t.proc.rpos, 1);
        EXPECT_EQ(t.proc.wpos, 1);
        EXPECT_EQ(t.proc.sofar(), "a");
        EXPECT_EQ(t.proc.result(), "a");
        EXPECT_FALSE(t.proc.unfiltered_chars);
    }
}

TEST(FilterProcessorInplace, set_bulk_does_not_unfilter)
{
    // skip -> set
    {
        InplaceTester t("0123");
        t.trim_capacity();
        EXPECT_EQ(t.proc.wcap, 4);
        EXPECT_EQ(t.proc.rpos, 0);
        EXPECT_EQ(t.proc.wpos, 0);
        EXPECT_FALSE(t.proc.unfiltered_chars);
        EXPECT_EQ(t.proc.sofar(), "");
        t.proc.skip(4);
        t.proc.set('a', 4);
        EXPECT_EQ(t.proc.rpos, 4);
        EXPECT_EQ(t.proc.wpos, 4);
        EXPECT_EQ(t.proc.sofar(), "aaaa");
        EXPECT_EQ(t.proc.result(), "aaaa");
        EXPECT_FALSE(t.proc.unfiltered_chars);
    }
    // set -> skip
    {
        InplaceTester t("0123");
        t.trim_capacity();
        EXPECT_EQ(t.proc.wcap, 4);
        EXPECT_EQ(t.proc.rpos, 0);
        EXPECT_EQ(t.proc.wpos, 0);
        EXPECT_FALSE(t.proc.unfiltered_chars);
        EXPECT_EQ(t.proc.sofar(), "");
        t.proc.set('a', 4);
        t.proc.skip(4);
        EXPECT_EQ(t.proc.rpos, 4);
        EXPECT_EQ(t.proc.wpos, 4);
        EXPECT_EQ(t.proc.sofar(), "aaaa");
        EXPECT_EQ(t.proc.result(), "aaaa");
        EXPECT_FALSE(t.proc.unfiltered_chars);
    }
}


//-----------------------------------------------------------------------------

TEST(FilterProcessorInplace, copy)
{
    {
        InplaceTester t("subject");
        EXPECT_EQ(t.proc.rpos, 0);
        EXPECT_EQ(t.proc.wpos, 0);
        EXPECT_FALSE(t.proc.unfiltered_chars);
        EXPECT_EQ(t.proc.sofar(), "");
        EXPECT_EQ(t.proc.result(), "");
        t.proc.copy();
        EXPECT_EQ(t.proc.rpos, 1);
        EXPECT_EQ(t.proc.wpos, 1);
        EXPECT_EQ(t.proc.sofar(), "s");
        EXPECT_EQ(t.proc.result(), "s");
        EXPECT_EQ(t.subject, "subject");
        EXPECT_FALSE(t.proc.unfiltered_chars);
        t.proc.skip();
        EXPECT_EQ(t.proc.rpos, 2);
        EXPECT_EQ(t.proc.wpos, 1);
        EXPECT_EQ(t.proc.sofar(), "s");
        EXPECT_EQ(t.proc.result(), "s");
        EXPECT_EQ(t.subject, "subject");
        EXPECT_FALSE(t.proc.unfiltered_chars);
        t.proc.copy();
        EXPECT_EQ(t.proc.rpos, 3);
        EXPECT_EQ(t.proc.wpos, 2);
        EXPECT_EQ(t.proc.sofar(), "sb");
        EXPECT_EQ(t.proc.result(), "sb");
        EXPECT_EQ(t.subject, "sbbject");
        EXPECT_FALSE(t.proc.unfiltered_chars);
        t.proc.copy(4);
        EXPECT_EQ(t.proc.rpos, 7);
        EXPECT_EQ(t.proc.wpos, 6);
        EXPECT_EQ(t.proc.sofar(), "sbject");
        EXPECT_EQ(t.proc.result(), "sbject");
        EXPECT_EQ(t.subject, "sbjectt");
        EXPECT_FALSE(t.proc.unfiltered_chars);
    }
    {
        InplaceTester t("s");
        t.trim_capacity();
        EXPECT_EQ(t.proc.wcap, 1);
        t.proc.copy();
        EXPECT_EQ(t.proc.rpos, 1);
        EXPECT_EQ(t.proc.wpos, 1);
        EXPECT_EQ(t.proc.sofar(), "s");
        EXPECT_EQ(t.proc.result(), "s");
        EXPECT_EQ(t.subject, "s");
        EXPECT_FALSE(t.proc.unfiltered_chars);
    }
    {
        InplaceTester t("0");
        t.trim_capacity();
        EXPECT_EQ(t.proc.wcap, 1);
        EXPECT_EQ(t.proc.rpos, 0);
        EXPECT_EQ(t.proc.wpos, 0);
        EXPECT_EQ(t.proc.sofar(), "");
        t.proc.copy();
        EXPECT_EQ(t.proc.rpos, 1);
        EXPECT_EQ(t.proc.wpos, 1);
        EXPECT_EQ(t.proc.sofar(), "0");
        ASSERT_FALSE(t.proc.unfiltered_chars);
        EXPECT_EQ(t.proc.result(), "0");
        EXPECT_EQ(t.subject, "0");
    }
    {
        InplaceTester t("012345");
        t.trim_capacity();
        EXPECT_EQ(t.proc.wcap, 6);
        EXPECT_EQ(t.proc.rpos, 0);
        EXPECT_EQ(t.proc.wpos, 0);
        EXPECT_EQ(t.proc.sofar(), "");
        ASSERT_FALSE(t.proc.unfiltered_chars);
        EXPECT_EQ(t.proc.result(), "");
        EXPECT_EQ(t.subject, "012345");
        t.proc.copy(6);
        EXPECT_EQ(t.proc.rpos, 6);
        EXPECT_EQ(t.proc.wpos, 6);
        EXPECT_EQ(t.proc.sofar(), "012345");
        ASSERT_FALSE(t.proc.unfiltered_chars);
        EXPECT_EQ(t.proc.result(), "012345");
        EXPECT_EQ(t.subject, "012345");
    }
}

TEST(FilterProcessorSrcDst, copy)
{
    SrcDstTester t("subject");
    EXPECT_EQ(t.proc.rpos, 0);
    EXPECT_EQ(t.proc.wpos, 0);
    EXPECT_EQ(t.proc.sofar(), "");
    EXPECT_EQ(t.proc.result(), "");
    t.proc.copy();
    EXPECT_EQ(t.proc.rpos, 1);
    EXPECT_EQ(t.proc.wpos, 1);
    EXPECT_EQ(t.proc.sofar(), "s");
    EXPECT_EQ(t.proc.result(), "s");
    EXPECT_EQ(t.dst, "subject");
    t.proc.skip();
    EXPECT_EQ(t.proc.rpos, 2);
    EXPECT_EQ(t.proc.wpos, 1);
    EXPECT_EQ(t.proc.sofar(), "s");
    EXPECT_EQ(t.proc.result(), "s");
    EXPECT_EQ(t.dst, "subject");
    t.proc.copy();
    EXPECT_EQ(t.proc.rpos, 3);
    EXPECT_EQ(t.proc.wpos, 2);
    EXPECT_EQ(t.proc.sofar(), "sb");
    EXPECT_EQ(t.proc.result(), "sb");
    EXPECT_EQ(t.dst, "sbbject");
    t.proc.copy(4);
    EXPECT_EQ(t.proc.rpos, 7);
    EXPECT_EQ(t.proc.wpos, 6);
    EXPECT_EQ(t.proc.sofar(), "sbject");
    EXPECT_EQ(t.proc.result(), "sbject");
    EXPECT_EQ(t.dst, "sbjectt");
}

TEST(FilterProcessorInplace, copy_single_does_not_unfilter)
{
    InplaceTester t("0");
    t.trim_capacity();
    EXPECT_EQ(t.proc.wcap, 1);
    EXPECT_EQ(t.proc.rpos, 0);
    EXPECT_EQ(t.proc.wpos, 0);
    EXPECT_FALSE(t.proc.unfiltered_chars);
    EXPECT_EQ(t.proc.sofar(), "");
    t.proc.copy();
    EXPECT_EQ(t.proc.rpos, 1);
    EXPECT_EQ(t.proc.wpos, 1);
    EXPECT_EQ(t.proc.sofar(), "0");
    EXPECT_EQ(t.proc.result(), "0");
    EXPECT_FALSE(t.proc.unfiltered_chars);
}

TEST(FilterProcessorInplace, copy_bulk_does_not_unfilter)
{
    InplaceTester t("0123");
    t.trim_capacity();
    EXPECT_EQ(t.proc.wcap, 4);
    EXPECT_EQ(t.proc.rpos, 0);
    EXPECT_EQ(t.proc.wpos, 0);
    EXPECT_FALSE(t.proc.unfiltered_chars);
    EXPECT_EQ(t.proc.sofar(), "");
    t.proc.copy(4);
    EXPECT_EQ(t.proc.rpos, 4);
    EXPECT_EQ(t.proc.wpos, 4);
    EXPECT_EQ(t.proc.sofar(), "0123");
    EXPECT_EQ(t.proc.result(), "0123");
    EXPECT_FALSE(t.proc.unfiltered_chars);
}


//-----------------------------------------------------------------------------

TEST(FilterProcessorInplace, translate_esc_single)
{
    InplaceTester t("\\t\\b\\n\\r\\t");
    t.trim_capacity();
    EXPECT_EQ(t.proc.wcap, 10);
    EXPECT_EQ(t.proc.rpos, 0);
    EXPECT_EQ(t.proc.wpos, 0);
    EXPECT_FALSE(t.proc.unfiltered_chars);
    EXPECT_EQ(t.proc.sofar(), "");
    t.proc.translate_esc('\t');
    EXPECT_EQ(t.proc.rpos, 2);
    EXPECT_EQ(t.proc.wpos, 1);
    EXPECT_EQ(t.proc.sofar(), "\t");
    EXPECT_EQ(t.proc.result(), "\t");
    EXPECT_EQ(t.subject, "\tt\\b\\n\\r\\t");
    EXPECT_FALSE(t.proc.unfiltered_chars);
    t.proc.translate_esc('\b');
    EXPECT_EQ(t.proc.rpos, 4);
    EXPECT_EQ(t.proc.wpos, 2);
    EXPECT_EQ(t.proc.sofar(), "\t\b");
    EXPECT_EQ(t.proc.result(), "\t\b");
    EXPECT_EQ(t.subject, "\t\b\\b\\n\\r\\t");
    EXPECT_FALSE(t.proc.unfiltered_chars);
    t.proc.translate_esc('\n');
    EXPECT_EQ(t.proc.rpos, 6);
    EXPECT_EQ(t.proc.wpos, 3);
    EXPECT_EQ(t.proc.sofar(), "\t\b\n");
    EXPECT_EQ(t.proc.result(), "\t\b\n");
    EXPECT_EQ(t.subject, "\t\b\nb\\n\\r\\t");
    EXPECT_FALSE(t.proc.unfiltered_chars);
    t.proc.translate_esc('\r');
    EXPECT_EQ(t.proc.rpos, 8);
    EXPECT_EQ(t.proc.wpos, 4);
    EXPECT_EQ(t.proc.sofar(), "\t\b\n\r");
    EXPECT_EQ(t.proc.result(), "\t\b\n\r");
    EXPECT_EQ(t.subject, "\t\b\n\r\\n\\r\\t");
    EXPECT_FALSE(t.proc.unfiltered_chars);
    t.proc.translate_esc('\t');
    EXPECT_EQ(t.proc.rpos, 10);
    EXPECT_EQ(t.proc.wpos, 5);
    EXPECT_EQ(t.proc.sofar(), "\t\b\n\r\t");
    EXPECT_EQ(t.proc.result(), "\t\b\n\r\t");
    EXPECT_EQ(t.subject, "\t\b\n\r\tn\\r\\t");
    EXPECT_FALSE(t.proc.unfiltered_chars);
    t.set_to_end(/*slack*/1);
    EXPECT_EQ(t.proc.rpos, 10);
    EXPECT_EQ(t.proc.wpos, 9);
    EXPECT_EQ(t.subject, "\t\b\n\r\tn\\r\\t");
    EXPECT_FALSE(t.proc.unfiltered_chars);
    // can write this one
    t.proc.translate_esc('+');
    EXPECT_EQ(t.proc.rpos, 12); // this is fine, no read is done
    EXPECT_EQ(t.proc.wpos, 10);
    EXPECT_EQ(t.subject, "\t\b\n\r\tn\\r\\+");
    EXPECT_FALSE(t.proc.unfiltered_chars);
    // but this one will set to unfiltered
    t.proc.translate_esc('x');
    EXPECT_EQ(t.proc.rpos, 14); // this is fine, no read is done
    EXPECT_EQ(t.proc.wpos, 11);
    EXPECT_EQ(t.subject, "\t\b\n\r\tn\\r\\+");
    EXPECT_TRUE(t.proc.unfiltered_chars);
}

TEST(FilterProcessorSrcDst, translate_esc_single)
{
    SrcDstTester t("\\t\\b\\n\\r\\t");
    EXPECT_EQ(t.proc.rpos, 0);
    EXPECT_EQ(t.proc.wpos, 0);
    EXPECT_EQ(t.proc.sofar(), "");
    t.proc.translate_esc('\t');
    EXPECT_EQ(t.proc.rpos, 2);
    EXPECT_EQ(t.proc.wpos, 1);
    EXPECT_EQ(t.proc.sofar(), "\t");
    EXPECT_EQ(t.proc.result(), "\t");
    t.proc.translate_esc('\b');
    EXPECT_EQ(t.proc.rpos, 4);
    EXPECT_EQ(t.proc.wpos, 2);
    EXPECT_EQ(t.proc.sofar(), "\t\b");
    EXPECT_EQ(t.proc.result(), "\t\b");
    t.proc.translate_esc('\n');
    EXPECT_EQ(t.proc.rpos, 6);
    EXPECT_EQ(t.proc.wpos, 3);
    EXPECT_EQ(t.proc.sofar(), "\t\b\n");
    EXPECT_EQ(t.proc.result(), "\t\b\n");
    t.proc.translate_esc('\r');
    EXPECT_EQ(t.proc.rpos, 8);
    EXPECT_EQ(t.proc.wpos, 4);
    EXPECT_EQ(t.proc.sofar(), "\t\b\n\r");
    EXPECT_EQ(t.proc.result(), "\t\b\n\r");
    t.proc.translate_esc('\t');
    EXPECT_EQ(t.proc.rpos, 10);
    EXPECT_EQ(t.proc.wpos, 5);
    EXPECT_EQ(t.proc.sofar(), "\t\b\n\r\t");
    EXPECT_EQ(t.proc.result(), "\t\b\n\r\t");
    t.set_to_end(/*slack*/1);
    EXPECT_EQ(t.proc.rpos, 10);
    EXPECT_EQ(t.proc.wpos, 9);
    // can write this one
    t.proc.translate_esc('+');
    EXPECT_EQ(t.proc.rpos, 12); // this is fine, no read is done
    EXPECT_EQ(t.proc.wpos, 10);
    // but this one will set to unfiltered
    t.proc.translate_esc('x');
    EXPECT_EQ(t.proc.rpos, 14); // this is fine, no read is done
    EXPECT_EQ(t.proc.wpos, 11);
}

} // namespace yml
} // namespace c4


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// this is needed to use the test case library

namespace c4 {
namespace yml {
struct Case;
Case const* get_case(csubstr /*name*/)
{
    return nullptr;
}
} // namespace yml
} // namespace c4
