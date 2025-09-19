#ifdef RYML_SINGLE_HEADER
#include "ryml_all.hpp"
#else
#include "c4/yml/filter_processor.hpp"
#include "c4/yml/event_handler_tree.hpp"
#include "c4/yml/parse_engine.hpp"
#include "c4/yml/parse_engine.def.hpp"
#include "c4/yml/parse.hpp"
#include "c4/yml/tree.hpp"
#include "c4/yml/node.hpp"
#include "c4/yml/std/string.hpp"
#endif
#include <gtest/gtest.h>


namespace c4 {
namespace yml {

template<class Processor>
struct TesterInplace_
{
    std::string buf;
    substr subject;
    Processor proc;
    C4_NO_COPY_OR_MOVE(TesterInplace_);
    TesterInplace_(const char *str)
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
    void set_capacity(size_t cap)
    {
        buf.reserve(cap);
        subject = to_substr(buf);
        proc = FilterProcessorInplaceMidExtending(subject, cap);
    }
};
using TesterInplaceMid = TesterInplace_<FilterProcessorInplaceMidExtending>;
using TesterInplaceEnd = TesterInplace_<FilterProcessorInplaceEndExtending>;


struct TesterSrcDst
{
    std::string src_;
    std::string dst_;
    csubstr src;
    substr dst;
    FilterProcessorSrcDst proc;
    C4_NO_COPY_OR_MOVE(TesterSrcDst);
    TesterSrcDst(const char *str)
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
    void set_dst_size(size_t cap)
    {
        dst_.resize(cap);
        dst = to_substr(dst_);
        proc = FilterProcessorSrcDst(src, dst);
    }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(FilterProcessorInplaceMid, set)
{
    TesterInplaceMid t("subject");
    EXPECT_EQ(t.proc.rpos, 0);
    EXPECT_EQ(t.proc.wpos, 0);
    EXPECT_FALSE(t.proc.unfiltered_chars);
    EXPECT_EQ(t.proc.sofar(), "");
    t.proc.skip();
    EXPECT_EQ(t.proc.rpos, 1);
    EXPECT_EQ(t.proc.wpos, 0);
    EXPECT_EQ(t.proc.sofar(), "");
    EXPECT_EQ(t.proc.result().get(), "");
    t.proc.set('.');
    EXPECT_EQ(t.proc.rpos, 1);
    EXPECT_EQ(t.proc.wpos, 1);
    EXPECT_EQ(t.proc.sofar(), ".");
    EXPECT_EQ(t.proc.result().get(), ".");
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
    ASSERT_TRUE(t.proc.result().valid());
    EXPECT_EQ(t.proc.result().get(), "...");
    EXPECT_FALSE(t.proc.unfiltered_chars);
    t.proc.skip(3);
    t.proc.set('x', 3);
    EXPECT_EQ(t.proc.wpos, 6);
    EXPECT_EQ(t.proc.rpos, 6);
    EXPECT_EQ(t.subject, "...xxxt");
    EXPECT_EQ(t.proc.sofar(), "...xxx");
    ASSERT_TRUE(t.proc.result().valid());
    EXPECT_EQ(t.proc.result().get(), "...xxx");
    EXPECT_FALSE(t.proc.unfiltered_chars);
    t.proc.skip(1);
    t.proc.set('.');
    EXPECT_EQ(t.proc.wpos, 7);
    EXPECT_EQ(t.proc.rpos, 7);
    EXPECT_EQ(t.subject, "...xxx.");
    EXPECT_EQ(t.proc.sofar(), "...xxx.");
    ASSERT_TRUE(t.proc.result().valid());
    EXPECT_EQ(t.proc.result().get(), "...xxx.");
    EXPECT_FALSE(t.proc.unfiltered_chars);
}

TEST(FilterProcessorInplaceEnd, set)
{
    TesterInplaceEnd t("subject");
    EXPECT_EQ(t.proc.rpos, 0);
    EXPECT_EQ(t.proc.wpos, 0);
    EXPECT_EQ(t.proc.sofar(), "");
    t.proc.skip();
    EXPECT_EQ(t.proc.rpos, 1);
    EXPECT_EQ(t.proc.wpos, 0);
    EXPECT_EQ(t.proc.sofar(), "");
    EXPECT_EQ(t.proc.result().get(), "");
    EXPECT_EQ(t.subject, "subject");
    t.proc.set('.');
    EXPECT_EQ(t.proc.rpos, 1);
    EXPECT_EQ(t.proc.wpos, 1);
    EXPECT_EQ(t.proc.sofar(), ".");
    EXPECT_EQ(t.proc.result().get(), ".");
    EXPECT_EQ(t.subject, ".ubject");
    t.proc.skip(2);
    EXPECT_EQ(t.proc.rpos, 3);
    EXPECT_EQ(t.proc.wpos, 1);
    t.proc.set('.', 2);
    EXPECT_EQ(t.proc.rpos, 3);
    EXPECT_EQ(t.proc.wpos, 3);
    EXPECT_EQ(t.subject, "...ject");
    EXPECT_EQ(t.proc.sofar(), "...");
    ASSERT_TRUE(t.proc.result().valid());
    EXPECT_EQ(t.proc.result().get(), "...");
    t.proc.skip(3);
    t.proc.set('x', 3);
    EXPECT_EQ(t.proc.wpos, 6);
    EXPECT_EQ(t.proc.rpos, 6);
    EXPECT_EQ(t.subject, "...xxxt");
    EXPECT_EQ(t.proc.sofar(), "...xxx");
    ASSERT_TRUE(t.proc.result().valid());
    EXPECT_EQ(t.proc.result().get(), "...xxx");
    t.proc.skip(1);
    t.proc.set('.');
    EXPECT_EQ(t.proc.wpos, 7);
    EXPECT_EQ(t.proc.rpos, 7);
    EXPECT_EQ(t.subject, "...xxx.");
    EXPECT_EQ(t.proc.sofar(), "...xxx.");
    ASSERT_TRUE(t.proc.result().valid());
    EXPECT_EQ(t.proc.result().get(), "...xxx.");
}

TEST(FilterProcessorSrcDst, set)
{
    TesterSrcDst t("subject");
    EXPECT_EQ(t.proc.wpos, 0);
    EXPECT_EQ(t.proc.rpos, 0);
    EXPECT_EQ(t.proc.sofar(), "");
    EXPECT_EQ(t.proc.result().get(), "");
    t.proc.skip();
    EXPECT_EQ(t.proc.wpos, 0);
    EXPECT_EQ(t.proc.rpos, 1);
    t.proc.set('.');
    EXPECT_EQ(t.proc.wpos, 1);
    EXPECT_EQ(t.proc.rpos, 1);
    EXPECT_EQ(t.dst, ".ubject");
    EXPECT_EQ(t.proc.sofar(), ".");
    EXPECT_EQ(t.proc.result().get(), ".");
    t.proc.skip(2);
    EXPECT_EQ(t.proc.rpos, 3);
    EXPECT_EQ(t.proc.wpos, 1);
    t.proc.set('.', 2);
    EXPECT_EQ(t.proc.wpos, 3);
    EXPECT_EQ(t.proc.rpos, 3);
    EXPECT_EQ(t.dst, "...ject");
    EXPECT_EQ(t.proc.sofar(), "...");
    EXPECT_EQ(t.proc.result().get(), "...");
    t.proc.skip(3);
    t.proc.set('x', 3);
    EXPECT_EQ(t.proc.wpos, 6);
    EXPECT_EQ(t.proc.rpos, 6);
    EXPECT_EQ(t.dst, "...xxxt");
    EXPECT_EQ(t.proc.sofar(), "...xxx");
    EXPECT_EQ(t.proc.result().get(), "...xxx");
}

TEST(FilterProcessorInplaceMid, set_single_does_not_unfilter)
{
    // skip -> set
    {
        TesterInplaceMid t("0");
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
        EXPECT_EQ(t.proc.result().get(), "a");
        EXPECT_FALSE(t.proc.unfiltered_chars);
    }
    // set -> skip
    {
        TesterInplaceMid t("0");
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
        EXPECT_EQ(t.proc.result().get(), "a");
        EXPECT_FALSE(t.proc.unfiltered_chars);
    }
}

TEST(FilterProcessorInplaceEnd, set_single_does_not_unfilter)
{
    // skip -> set
    {
        TesterInplaceEnd t("0");
        t.trim_capacity();
        EXPECT_EQ(t.proc.wcap, 1);
        EXPECT_EQ(t.proc.rpos, 0);
        EXPECT_EQ(t.proc.wpos, 0);
        EXPECT_EQ(t.proc.sofar(), "");
        t.proc.skip();
        EXPECT_EQ(t.proc.rpos, 1);
        EXPECT_EQ(t.proc.wpos, 0);
        t.proc.set('a');
        EXPECT_EQ(t.proc.rpos, 1);
        EXPECT_EQ(t.proc.wpos, 1);
        EXPECT_EQ(t.proc.sofar(), "a");
        EXPECT_EQ(t.proc.result().get(), "a");
    }
    // set -> skip
    {
        TesterInplaceEnd t("0");
        t.trim_capacity();
        EXPECT_EQ(t.proc.wcap, 1);
        EXPECT_EQ(t.proc.rpos, 0);
        EXPECT_EQ(t.proc.wpos, 0);
        EXPECT_EQ(t.proc.sofar(), "");
        t.proc.set('a');
        EXPECT_EQ(t.proc.rpos, 0);
        EXPECT_EQ(t.proc.wpos, 1);
        t.proc.skip();
        EXPECT_EQ(t.proc.rpos, 1);
        EXPECT_EQ(t.proc.wpos, 1);
        EXPECT_EQ(t.proc.sofar(), "a");
        EXPECT_EQ(t.proc.result().get(), "a");
    }
}

TEST(FilterProcessorInplaceMid, set_bulk_does_not_unfilter)
{
    // skip -> set
    {
        TesterInplaceMid t("0123");
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
        EXPECT_EQ(t.proc.result().get(), "aaaa");
        EXPECT_FALSE(t.proc.unfiltered_chars);
    }
    // set -> skip
    {
        TesterInplaceMid t("0123");
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
        EXPECT_EQ(t.proc.result().get(), "aaaa");
        EXPECT_FALSE(t.proc.unfiltered_chars);
    }
}

TEST(FilterProcessorInplaceEnd, set_bulk_does_not_unfilter)
{
    // skip -> set
    {
        TesterInplaceEnd t("0123");
        t.trim_capacity();
        EXPECT_EQ(t.proc.wcap, 4);
        EXPECT_EQ(t.proc.rpos, 0);
        EXPECT_EQ(t.proc.wpos, 0);
        EXPECT_EQ(t.proc.sofar(), "");
        t.proc.skip(4);
        t.proc.set('a', 4);
        EXPECT_EQ(t.proc.rpos, 4);
        EXPECT_EQ(t.proc.wpos, 4);
        EXPECT_EQ(t.proc.sofar(), "aaaa");
        EXPECT_EQ(t.proc.result().get(), "aaaa");
    }
    // set -> skip
    {
        TesterInplaceEnd t("0123");
        t.trim_capacity();
        EXPECT_EQ(t.proc.wcap, 4);
        EXPECT_EQ(t.proc.rpos, 0);
        EXPECT_EQ(t.proc.wpos, 0);
        EXPECT_EQ(t.proc.sofar(), "");
        t.proc.set('a', 4);
        t.proc.skip(4);
        EXPECT_EQ(t.proc.rpos, 4);
        EXPECT_EQ(t.proc.wpos, 4);
        EXPECT_EQ(t.proc.sofar(), "aaaa");
        EXPECT_EQ(t.proc.result().get(), "aaaa");
    }
}


//-----------------------------------------------------------------------------

TEST(FilterProcessorInplaceMid, copy)
{
    {
        TesterInplaceMid t("subject");
        EXPECT_EQ(t.proc.rpos, 0);
        EXPECT_EQ(t.proc.wpos, 0);
        EXPECT_FALSE(t.proc.unfiltered_chars);
        EXPECT_EQ(t.proc.sofar(), "");
        EXPECT_EQ(t.proc.result().get(), "");
        t.proc.copy();
        EXPECT_EQ(t.proc.rpos, 1);
        EXPECT_EQ(t.proc.wpos, 1);
        EXPECT_EQ(t.proc.sofar(), "s");
        EXPECT_EQ(t.proc.result().get(), "s");
        EXPECT_EQ(t.subject, "subject");
        EXPECT_FALSE(t.proc.unfiltered_chars);
        t.proc.skip();
        EXPECT_EQ(t.proc.rpos, 2);
        EXPECT_EQ(t.proc.wpos, 1);
        EXPECT_EQ(t.proc.sofar(), "s");
        EXPECT_EQ(t.proc.result().get(), "s");
        EXPECT_EQ(t.subject, "subject");
        EXPECT_FALSE(t.proc.unfiltered_chars);
        t.proc.copy();
        EXPECT_EQ(t.proc.rpos, 3);
        EXPECT_EQ(t.proc.wpos, 2);
        EXPECT_EQ(t.proc.sofar(), "sb");
        EXPECT_EQ(t.proc.result().get(), "sb");
        EXPECT_EQ(t.subject, "sbbject");
        EXPECT_FALSE(t.proc.unfiltered_chars);
        t.proc.copy(4);
        EXPECT_EQ(t.proc.rpos, 7);
        EXPECT_EQ(t.proc.wpos, 6);
        EXPECT_EQ(t.proc.sofar(), "sbject");
        EXPECT_EQ(t.proc.result().get(), "sbject");
        EXPECT_EQ(t.subject, "sbjectt");
        EXPECT_FALSE(t.proc.unfiltered_chars);
    }
    {
        TesterInplaceMid t("s");
        t.trim_capacity();
        EXPECT_EQ(t.proc.wcap, 1);
        t.proc.copy();
        EXPECT_EQ(t.proc.rpos, 1);
        EXPECT_EQ(t.proc.wpos, 1);
        EXPECT_EQ(t.proc.sofar(), "s");
        EXPECT_EQ(t.proc.result().get(), "s");
        EXPECT_EQ(t.subject, "s");
        EXPECT_FALSE(t.proc.unfiltered_chars);
    }
    {
        TesterInplaceMid t("0");
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
        EXPECT_EQ(t.proc.result().get(), "0");
        EXPECT_EQ(t.subject, "0");
    }
    {
        TesterInplaceMid t("012345");
        t.trim_capacity();
        EXPECT_EQ(t.proc.wcap, 6);
        EXPECT_EQ(t.proc.rpos, 0);
        EXPECT_EQ(t.proc.wpos, 0);
        EXPECT_EQ(t.proc.sofar(), "");
        ASSERT_FALSE(t.proc.unfiltered_chars);
        EXPECT_EQ(t.proc.result().get(), "");
        EXPECT_EQ(t.subject, "012345");
        t.proc.copy(6);
        EXPECT_EQ(t.proc.rpos, 6);
        EXPECT_EQ(t.proc.wpos, 6);
        EXPECT_EQ(t.proc.sofar(), "012345");
        ASSERT_FALSE(t.proc.unfiltered_chars);
        EXPECT_EQ(t.proc.result().get(), "012345");
        EXPECT_EQ(t.subject, "012345");
    }
}

TEST(FilterProcessorInplaceEnd, copy)
{
    {
        TesterInplaceEnd t("subject");
        EXPECT_EQ(t.proc.rpos, 0);
        EXPECT_EQ(t.proc.wpos, 0);
        EXPECT_EQ(t.proc.sofar(), "");
        EXPECT_EQ(t.proc.result().get(), "");
        t.proc.copy();
        EXPECT_EQ(t.proc.rpos, 1);
        EXPECT_EQ(t.proc.wpos, 1);
        EXPECT_EQ(t.proc.sofar(), "s");
        EXPECT_EQ(t.proc.result().get(), "s");
        EXPECT_EQ(t.subject, "subject");
        t.proc.skip();
        EXPECT_EQ(t.proc.rpos, 2);
        EXPECT_EQ(t.proc.wpos, 1);
        EXPECT_EQ(t.proc.sofar(), "s");
        EXPECT_EQ(t.proc.result().get(), "s");
        EXPECT_EQ(t.subject, "subject");
        t.proc.copy();
        EXPECT_EQ(t.proc.rpos, 3);
        EXPECT_EQ(t.proc.wpos, 2);
        EXPECT_EQ(t.proc.sofar(), "sb");
        EXPECT_EQ(t.proc.result().get(), "sb");
        EXPECT_EQ(t.subject, "sbbject");
        t.proc.copy(4);
        EXPECT_EQ(t.proc.rpos, 7);
        EXPECT_EQ(t.proc.wpos, 6);
        EXPECT_EQ(t.proc.sofar(), "sbject");
        EXPECT_EQ(t.proc.result().get(), "sbject");
        EXPECT_EQ(t.subject, "sbjectt");
    }
    {
        TesterInplaceEnd t("s");
        t.trim_capacity();
        EXPECT_EQ(t.proc.wcap, 1);
        t.proc.copy();
        EXPECT_EQ(t.proc.rpos, 1);
        EXPECT_EQ(t.proc.wpos, 1);
        EXPECT_EQ(t.proc.sofar(), "s");
        EXPECT_EQ(t.proc.result().get(), "s");
        EXPECT_EQ(t.subject, "s");
    }
    {
        TesterInplaceEnd t("0");
        t.trim_capacity();
        EXPECT_EQ(t.proc.wcap, 1);
        EXPECT_EQ(t.proc.rpos, 0);
        EXPECT_EQ(t.proc.wpos, 0);
        EXPECT_EQ(t.proc.sofar(), "");
        t.proc.copy();
        EXPECT_EQ(t.proc.rpos, 1);
        EXPECT_EQ(t.proc.wpos, 1);
        EXPECT_EQ(t.proc.sofar(), "0");
        EXPECT_EQ(t.proc.result().get(), "0");
        EXPECT_EQ(t.subject, "0");
    }
    {
        TesterInplaceEnd t("012345");
        t.trim_capacity();
        EXPECT_EQ(t.proc.wcap, 6);
        EXPECT_EQ(t.proc.rpos, 0);
        EXPECT_EQ(t.proc.wpos, 0);
        EXPECT_EQ(t.proc.sofar(), "");
        EXPECT_EQ(t.proc.result().get(), "");
        EXPECT_EQ(t.subject, "012345");
        t.proc.copy(6);
        EXPECT_EQ(t.proc.rpos, 6);
        EXPECT_EQ(t.proc.wpos, 6);
        EXPECT_EQ(t.proc.sofar(), "012345");
        EXPECT_EQ(t.proc.result().get(), "012345");
        EXPECT_EQ(t.subject, "012345");
    }
}

TEST(FilterProcessorSrcDst, copy)
{
    TesterSrcDst t("subject");
    EXPECT_EQ(t.proc.rpos, 0);
    EXPECT_EQ(t.proc.wpos, 0);
    EXPECT_EQ(t.proc.sofar(), "");
    EXPECT_EQ(t.proc.result().get(), "");
    t.proc.copy();
    EXPECT_EQ(t.proc.rpos, 1);
    EXPECT_EQ(t.proc.wpos, 1);
    EXPECT_EQ(t.proc.sofar(), "s");
    EXPECT_EQ(t.proc.result().get(), "s");
    EXPECT_EQ(t.dst, "subject");
    t.proc.skip();
    EXPECT_EQ(t.proc.rpos, 2);
    EXPECT_EQ(t.proc.wpos, 1);
    EXPECT_EQ(t.proc.sofar(), "s");
    EXPECT_EQ(t.proc.result().get(), "s");
    EXPECT_EQ(t.dst, "subject");
    t.proc.copy();
    EXPECT_EQ(t.proc.rpos, 3);
    EXPECT_EQ(t.proc.wpos, 2);
    EXPECT_EQ(t.proc.sofar(), "sb");
    EXPECT_EQ(t.proc.result().get(), "sb");
    EXPECT_EQ(t.dst, "sbbject");
    t.proc.copy(4);
    EXPECT_EQ(t.proc.rpos, 7);
    EXPECT_EQ(t.proc.wpos, 6);
    EXPECT_EQ(t.proc.sofar(), "sbject");
    EXPECT_EQ(t.proc.result().get(), "sbject");
    EXPECT_EQ(t.dst, "sbjectt");
}

TEST(FilterProcessorInplaceMid, copy_single_does_not_unfilter)
{
    TesterInplaceMid t("0");
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
    EXPECT_EQ(t.proc.result().get(), "0");
    EXPECT_FALSE(t.proc.unfiltered_chars);
}

TEST(FilterProcessorInplaceEnd, copy_single_does_not_unfilter)
{
    TesterInplaceEnd t("0");
    t.trim_capacity();
    EXPECT_EQ(t.proc.wcap, 1);
    EXPECT_EQ(t.proc.rpos, 0);
    EXPECT_EQ(t.proc.wpos, 0);
    EXPECT_EQ(t.proc.sofar(), "");
    t.proc.copy();
    EXPECT_EQ(t.proc.rpos, 1);
    EXPECT_EQ(t.proc.wpos, 1);
    EXPECT_EQ(t.proc.sofar(), "0");
    EXPECT_EQ(t.proc.result().get(), "0");
}

TEST(FilterProcessorInplaceMid, copy_bulk_does_not_unfilter)
{
    TesterInplaceMid t("0123");
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
    EXPECT_EQ(t.proc.result().get(), "0123");
    EXPECT_FALSE(t.proc.unfiltered_chars);
}

TEST(FilterProcessorInplaceEnd, copy_bulk_does_not_unfilter)
{
    TesterInplaceEnd t("0123");
    t.trim_capacity();
    EXPECT_EQ(t.proc.wcap, 4);
    EXPECT_EQ(t.proc.rpos, 0);
    EXPECT_EQ(t.proc.wpos, 0);
    EXPECT_EQ(t.proc.sofar(), "");
    t.proc.copy(4);
    EXPECT_EQ(t.proc.rpos, 4);
    EXPECT_EQ(t.proc.wpos, 4);
    EXPECT_EQ(t.proc.sofar(), "0123");
    EXPECT_EQ(t.proc.result().get(), "0123");
}


//-----------------------------------------------------------------------------

TEST(FilterProcessorInplaceMid, translate_esc_single)
{
    TesterInplaceMid t("\\t\\b\\n\\r\\t");
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
    EXPECT_EQ(t.proc.result().get(), "\t");
    EXPECT_EQ(t.subject, "\tt\\b\\n\\r\\t");
    EXPECT_FALSE(t.proc.unfiltered_chars);
    t.proc.translate_esc('\b');
    EXPECT_EQ(t.proc.rpos, 4);
    EXPECT_EQ(t.proc.wpos, 2);
    EXPECT_EQ(t.proc.sofar(), "\t\b");
    EXPECT_EQ(t.proc.result().get(), "\t\b");
    EXPECT_EQ(t.subject, "\t\b\\b\\n\\r\\t");
    EXPECT_FALSE(t.proc.unfiltered_chars);
    t.proc.translate_esc('\n');
    EXPECT_EQ(t.proc.rpos, 6);
    EXPECT_EQ(t.proc.wpos, 3);
    EXPECT_EQ(t.proc.sofar(), "\t\b\n");
    EXPECT_EQ(t.proc.result().get(), "\t\b\n");
    EXPECT_EQ(t.subject, "\t\b\nb\\n\\r\\t");
    EXPECT_FALSE(t.proc.unfiltered_chars);
    t.proc.translate_esc('\r');
    EXPECT_EQ(t.proc.rpos, 8);
    EXPECT_EQ(t.proc.wpos, 4);
    EXPECT_EQ(t.proc.sofar(), "\t\b\n\r");
    EXPECT_EQ(t.proc.result().get(), "\t\b\n\r");
    EXPECT_EQ(t.subject, "\t\b\n\r\\n\\r\\t");
    EXPECT_FALSE(t.proc.unfiltered_chars);
    t.proc.translate_esc('\t');
    EXPECT_EQ(t.proc.rpos, 10);
    EXPECT_EQ(t.proc.wpos, 5);
    EXPECT_EQ(t.proc.sofar(), "\t\b\n\r\t");
    EXPECT_EQ(t.proc.result().get(), "\t\b\n\r\t");
    EXPECT_EQ(t.subject, "\t\b\n\r\tn\\r\\t");
    EXPECT_FALSE(t.proc.unfiltered_chars);
    t.set_to_end(/*slack*/1);
    EXPECT_EQ(t.proc.rpos, 10);
    EXPECT_EQ(t.proc.wpos, 9);
    EXPECT_EQ(t.subject, "\t\b\n\r\tn\\r\\t");
    EXPECT_FALSE(t.proc.unfiltered_chars);
    // can write this one
    t.proc.set('+');
    EXPECT_EQ(t.proc.rpos, 10); // this is fine, no read is done
    EXPECT_EQ(t.proc.wpos, 10);
    EXPECT_EQ(t.subject, "\t\b\n\r\tn\\r\\+");
    EXPECT_FALSE(t.proc.unfiltered_chars);
    // but this one will set to unfiltered
    t.proc.set('x');
    EXPECT_EQ(t.proc.rpos, 10); // this is fine, no read is done
    EXPECT_EQ(t.proc.wpos, 11);
    EXPECT_EQ(t.subject, "\t\b\n\r\tn\\r\\+");
    EXPECT_TRUE(t.proc.unfiltered_chars);
}

TEST(FilterProcessorInplaceEnd, translate_esc_single)
{
    TesterInplaceEnd t("\\t\\b\\n\\r\\t");
    t.trim_capacity();
    EXPECT_EQ(t.proc.wcap, 10);
    EXPECT_EQ(t.proc.rpos, 0);
    EXPECT_EQ(t.proc.wpos, 0);
    EXPECT_EQ(t.proc.sofar(), "");
    t.proc.translate_esc('\t');
    EXPECT_EQ(t.proc.rpos, 2);
    EXPECT_EQ(t.proc.wpos, 1);
    EXPECT_EQ(t.proc.sofar(), "\t");
    EXPECT_EQ(t.proc.result().get(), "\t");
    EXPECT_EQ(t.subject, "\tt\\b\\n\\r\\t");
    t.proc.translate_esc('\b');
    EXPECT_EQ(t.proc.rpos, 4);
    EXPECT_EQ(t.proc.wpos, 2);
    EXPECT_EQ(t.proc.sofar(), "\t\b");
    EXPECT_EQ(t.proc.result().get(), "\t\b");
    EXPECT_EQ(t.subject, "\t\b\\b\\n\\r\\t");
    t.proc.translate_esc('\n');
    EXPECT_EQ(t.proc.rpos, 6);
    EXPECT_EQ(t.proc.wpos, 3);
    EXPECT_EQ(t.proc.sofar(), "\t\b\n");
    EXPECT_EQ(t.proc.result().get(), "\t\b\n");
    EXPECT_EQ(t.subject, "\t\b\nb\\n\\r\\t");
    t.proc.translate_esc('\r');
    EXPECT_EQ(t.proc.rpos, 8);
    EXPECT_EQ(t.proc.wpos, 4);
    EXPECT_EQ(t.proc.sofar(), "\t\b\n\r");
    EXPECT_EQ(t.proc.result().get(), "\t\b\n\r");
    EXPECT_EQ(t.subject, "\t\b\n\r\\n\\r\\t");
    t.proc.translate_esc('\t');
    EXPECT_EQ(t.proc.rpos, 10);
    EXPECT_EQ(t.proc.wpos, 5);
    EXPECT_EQ(t.proc.sofar(), "\t\b\n\r\t");
    EXPECT_EQ(t.proc.result().get(), "\t\b\n\r\t");
    EXPECT_EQ(t.subject, "\t\b\n\r\tn\\r\\t");
    t.set_to_end(/*slack*/1);
    EXPECT_EQ(t.proc.rpos, 10);
    EXPECT_EQ(t.proc.wpos, 9);
    EXPECT_EQ(t.subject, "\t\b\n\r\tn\\r\\t");
    // can write this one
    t.proc.set('+');
    EXPECT_EQ(t.proc.rpos, 10); // this is fine, no read is done
    EXPECT_EQ(t.proc.wpos, 10);
    EXPECT_EQ(t.subject, "\t\b\n\r\tn\\r\\+");
    // but this one will set to unfiltered
    t.proc.set('x');
    EXPECT_EQ(t.proc.rpos, 10); // this is fine, no read is done
    EXPECT_EQ(t.proc.wpos, 11);
    EXPECT_EQ(t.subject, "\t\b\n\r\tn\\r\\+");
}

TEST(FilterProcessorSrcDst, translate_esc_single)
{
    TesterSrcDst t("\\t\\b\\n\\r\\t");
    EXPECT_EQ(t.proc.rpos, 0);
    EXPECT_EQ(t.proc.wpos, 0);
    EXPECT_EQ(t.proc.sofar(), "");
    t.proc.translate_esc('\t');
    EXPECT_EQ(t.proc.rpos, 2);
    EXPECT_EQ(t.proc.wpos, 1);
    EXPECT_EQ(t.proc.sofar(), "\t");
    EXPECT_EQ(t.proc.result().get(), "\t");
    t.proc.translate_esc('\b');
    EXPECT_EQ(t.proc.rpos, 4);
    EXPECT_EQ(t.proc.wpos, 2);
    EXPECT_EQ(t.proc.sofar(), "\t\b");
    EXPECT_EQ(t.proc.result().get(), "\t\b");
    t.proc.translate_esc('\n');
    EXPECT_EQ(t.proc.rpos, 6);
    EXPECT_EQ(t.proc.wpos, 3);
    EXPECT_EQ(t.proc.sofar(), "\t\b\n");
    EXPECT_EQ(t.proc.result().get(), "\t\b\n");
    t.proc.translate_esc('\r');
    EXPECT_EQ(t.proc.rpos, 8);
    EXPECT_EQ(t.proc.wpos, 4);
    EXPECT_EQ(t.proc.sofar(), "\t\b\n\r");
    EXPECT_EQ(t.proc.result().get(), "\t\b\n\r");
    t.proc.translate_esc('\t');
    EXPECT_EQ(t.proc.rpos, 10);
    EXPECT_EQ(t.proc.wpos, 5);
    EXPECT_EQ(t.proc.sofar(), "\t\b\n\r\t");
    EXPECT_EQ(t.proc.result().get(), "\t\b\n\r\t");
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


//-----------------------------------------------------------------------------

TEST(FilterProcessorInplaceMid, translate_esc_bulk)
{
    TesterInplaceMid t("0011223344");
    t.trim_capacity();
    EXPECT_EQ(t.proc.wcap, 10);
    EXPECT_EQ(t.proc.rpos, 0);
    EXPECT_EQ(t.proc.wpos, 0);
    EXPECT_FALSE(t.proc.unfiltered_chars);
    EXPECT_EQ(t.proc.sofar(), "");
    t.proc.translate_esc_bulk("aa", /*nw*/2, /*nr*/1);
    EXPECT_EQ(t.proc.rpos, 2);
    EXPECT_EQ(t.proc.wpos, 2);
    EXPECT_EQ(t.proc.sofar(), "aa");
    EXPECT_EQ(t.proc.result().get(), "aa");
    EXPECT_EQ(t.subject, "aa11223344");
    EXPECT_FALSE(t.proc.unfiltered_chars);
    t.proc.translate_esc_bulk("bb", /*nw*/2, /*nr*/1);
    EXPECT_EQ(t.proc.rpos, 4);
    EXPECT_EQ(t.proc.wpos, 4);
    EXPECT_EQ(t.proc.sofar(), "aabb");
    EXPECT_EQ(t.proc.result().get(), "aabb");
    EXPECT_EQ(t.subject, "aabb223344");
    EXPECT_FALSE(t.proc.unfiltered_chars);
    t.proc.translate_esc_bulk("cc", /*nw*/2, /*nr*/1);
    EXPECT_EQ(t.proc.rpos, 6);
    EXPECT_EQ(t.proc.wpos, 6);
    EXPECT_EQ(t.proc.sofar(), "aabbcc");
    EXPECT_EQ(t.proc.result().get(), "aabbcc");
    EXPECT_EQ(t.subject, "aabbcc3344");
    EXPECT_FALSE(t.proc.unfiltered_chars);
    t.proc.translate_esc_bulk("dd", /*nw*/2, /*nr*/1);
    EXPECT_EQ(t.proc.rpos, 8);
    EXPECT_EQ(t.proc.wpos, 8);
    EXPECT_EQ(t.proc.sofar(), "aabbccdd");
    EXPECT_EQ(t.proc.result().get(), "aabbccdd");
    EXPECT_EQ(t.subject, "aabbccdd44");
    EXPECT_FALSE(t.proc.unfiltered_chars);
    t.proc.translate_esc_bulk("ee", /*nw*/2, /*nr*/1);
    EXPECT_EQ(t.proc.rpos, 10);
    EXPECT_EQ(t.proc.wpos, 10);
    EXPECT_EQ(t.proc.sofar(), "aabbccddee");
    EXPECT_EQ(t.proc.result().get(), "aabbccddee");
    EXPECT_EQ(t.subject, "aabbccddee");
    EXPECT_FALSE(t.proc.unfiltered_chars);
}

TEST(FilterProcessorInplaceEnd, translate_esc_bulk)
{
    TesterInplaceEnd t("0011223344");
    t.trim_capacity();
    EXPECT_EQ(t.proc.wcap, 10);
    EXPECT_EQ(t.proc.rpos, 0);
    EXPECT_EQ(t.proc.wpos, 0);
    EXPECT_EQ(t.proc.sofar(), "");
    t.proc.translate_esc_bulk("aa", /*nw*/2, /*nr*/1);
    EXPECT_EQ(t.proc.rpos, 2);
    EXPECT_EQ(t.proc.wpos, 2);
    EXPECT_EQ(t.proc.sofar(), "aa");
    EXPECT_EQ(t.proc.result().get(), "aa");
    EXPECT_EQ(t.subject, "aa11223344");
    t.proc.translate_esc_bulk("bb", /*nw*/2, /*nr*/1);
    EXPECT_EQ(t.proc.rpos, 4);
    EXPECT_EQ(t.proc.wpos, 4);
    EXPECT_EQ(t.proc.sofar(), "aabb");
    EXPECT_EQ(t.proc.result().get(), "aabb");
    EXPECT_EQ(t.subject, "aabb223344");
    t.proc.translate_esc_bulk("cc", /*nw*/2, /*nr*/1);
    EXPECT_EQ(t.proc.rpos, 6);
    EXPECT_EQ(t.proc.wpos, 6);
    EXPECT_EQ(t.proc.sofar(), "aabbcc");
    EXPECT_EQ(t.proc.result().get(), "aabbcc");
    EXPECT_EQ(t.subject, "aabbcc3344");
    t.proc.translate_esc_bulk("dd", /*nw*/2, /*nr*/1);
    EXPECT_EQ(t.proc.rpos, 8);
    EXPECT_EQ(t.proc.wpos, 8);
    EXPECT_EQ(t.proc.sofar(), "aabbccdd");
    EXPECT_EQ(t.proc.result().get(), "aabbccdd");
    EXPECT_EQ(t.subject, "aabbccdd44");
    t.proc.translate_esc_bulk("ee", /*nw*/2, /*nr*/1);
    EXPECT_EQ(t.proc.rpos, 10);
    EXPECT_EQ(t.proc.wpos, 10);
    EXPECT_EQ(t.proc.sofar(), "aabbccddee");
    EXPECT_EQ(t.proc.result().get(), "aabbccddee");
    EXPECT_EQ(t.subject, "aabbccddee");
}

TEST(FilterProcessorSrcDst, translate_esc_bulk)
{
    TesterSrcDst t("0011223344");
    EXPECT_EQ(t.proc.rpos, 0);
    EXPECT_EQ(t.proc.wpos, 0);
    EXPECT_EQ(t.proc.sofar(), "");
    t.proc.translate_esc_bulk("aa", /*nw*/2, /*nr*/1);
    EXPECT_EQ(t.proc.rpos, 2);
    EXPECT_EQ(t.proc.wpos, 2);
    EXPECT_EQ(t.proc.sofar(), "aa");
    EXPECT_EQ(t.proc.result().get(), "aa");
    t.proc.translate_esc_bulk("bb", /*nw*/2, /*nr*/1);
    EXPECT_EQ(t.proc.rpos, 4);
    EXPECT_EQ(t.proc.wpos, 4);
    EXPECT_EQ(t.proc.sofar(), "aabb");
    EXPECT_EQ(t.proc.result().get(), "aabb");
    t.proc.translate_esc_bulk("cc", /*nw*/2, /*nr*/1);
    EXPECT_EQ(t.proc.rpos, 6);
    EXPECT_EQ(t.proc.wpos, 6);
    EXPECT_EQ(t.proc.sofar(), "aabbcc");
    EXPECT_EQ(t.proc.result().get(), "aabbcc");
    t.proc.translate_esc_bulk("dd", /*nw*/2, /*nr*/1);
    EXPECT_EQ(t.proc.rpos, 8);
    EXPECT_EQ(t.proc.wpos, 8);
    EXPECT_EQ(t.proc.sofar(), "aabbccdd");
    EXPECT_EQ(t.proc.result().get(), "aabbccdd");
    t.proc.translate_esc_bulk("ee", /*nw*/2, /*nr*/1);
    EXPECT_EQ(t.proc.rpos, 10);
    EXPECT_EQ(t.proc.wpos, 10);
    EXPECT_EQ(t.proc.sofar(), "aabbccddee");
    EXPECT_EQ(t.proc.result().get(), "aabbccddee");
}


//-----------------------------------------------------------------------------

TEST(FilterProcessorInplaceMid, translate_esc_extending_bulk_excess__trimmed_capacity)
{
    TesterInplaceMid t("0011223344");
    t.trim_capacity();
    EXPECT_EQ(t.proc.wcap, 10);
    EXPECT_EQ(t.proc.rpos, 0);
    EXPECT_EQ(t.proc.wpos, 0);
    EXPECT_EQ(t.proc.src.len, 10);
    EXPECT_FALSE(t.proc.unfiltered_chars);
    EXPECT_EQ(t.proc.sofar(), "");
    t.proc.translate_esc_extending("aaa", /*nw*/3, /*nr*/1);
    EXPECT_EQ(t.proc.rpos, 2);
    EXPECT_EQ(t.proc.wpos, 3);
    EXPECT_EQ(t.proc.src.len, 10);
    EXPECT_EQ(t.proc.sofar(), "001");
    EXPECT_EQ(t.proc.result().str, nullptr);
    EXPECT_EQ(t.subject, "0011223344");
    EXPECT_TRUE(t.proc.unfiltered_chars);
    t.proc.translate_esc_extending("bbb", /*nw*/3, /*nr*/1);
    EXPECT_EQ(t.proc.rpos, 4);
    EXPECT_EQ(t.proc.wpos, 6);
    EXPECT_EQ(t.proc.src.len, 10);
    EXPECT_EQ(t.proc.sofar(), "001122");
    EXPECT_EQ(t.proc.result().str, nullptr);
    EXPECT_EQ(t.subject, "0011223344");
    EXPECT_TRUE(t.proc.unfiltered_chars);
    t.proc.translate_esc_extending("ccc", /*nw*/3, /*nr*/1);
    EXPECT_EQ(t.proc.rpos, 6);
    EXPECT_EQ(t.proc.wpos, 9);
    EXPECT_EQ(t.proc.src.len, 10);
    EXPECT_EQ(t.proc.sofar(), "001122334");
    EXPECT_EQ(t.proc.result().str, nullptr);
    EXPECT_EQ(t.subject, "0011223344");
    EXPECT_TRUE(t.proc.unfiltered_chars);
    t.proc.translate_esc_extending("ddd", /*nw*/3, /*nr*/1);
    EXPECT_EQ(t.proc.rpos, 8);
    EXPECT_EQ(t.proc.wpos, 12);
    EXPECT_EQ(t.proc.src.len, 10);
    EXPECT_EQ(t.proc.sofar(), "0011223344");
    EXPECT_EQ(t.proc.result().str, nullptr);
    EXPECT_EQ(t.subject, "0011223344");
    EXPECT_TRUE(t.proc.unfiltered_chars);
    // write 4 characters
    t.proc.translate_esc_extending("cccc", /*nw*/4, /*nr*/1);
    EXPECT_EQ(t.proc.rpos, 10);
    EXPECT_EQ(t.proc.wpos, 16);
    EXPECT_EQ(t.proc.src.len, 10);
    EXPECT_EQ(t.proc.sofar(), "0011223344");
    EXPECT_EQ(t.proc.result().str, nullptr);
    EXPECT_EQ(t.subject, "0011223344");
    EXPECT_TRUE(t.proc.unfiltered_chars);
}

TEST(FilterProcessorInplaceMid, translate_esc_extending_bulk_excess__spare_capacity)
{
    TesterInplaceMid t("0011223344");
    const size_t needed_capacity = 16u;
    const size_t smaller_capacity = 14u;
    ASSERT_LT(smaller_capacity, needed_capacity);
    t.set_capacity(smaller_capacity);
    ASSERT_GE(t.buf.capacity(), smaller_capacity);
    const substr full_subject = {&t.buf[0], smaller_capacity};
    full_subject.sub(t.buf.size()).fill('^');
    EXPECT_EQ(t.proc.wcap, smaller_capacity);
    EXPECT_EQ(t.proc.rpos, 0);
    EXPECT_EQ(t.proc.wpos, 0);
    EXPECT_EQ(t.proc.src.len, 10);
    EXPECT_EQ(full_subject, "0011223344^^^^");
    EXPECT_FALSE(t.proc.unfiltered_chars);
    EXPECT_EQ(t.proc.sofar(), "");
    t.proc.translate_esc_extending("aaa", /*nw*/3, /*nr*/1);
    EXPECT_EQ(t.proc.rpos, 3);
    EXPECT_EQ(t.proc.wpos, 3);
    EXPECT_EQ(t.proc.src.len, 11);
    EXPECT_EQ(t.proc.sofar(), "aaa");
    EXPECT_EQ(t.proc.result().get(), "aaa");
    EXPECT_EQ(full_subject, "aaa11223344^^^");
    EXPECT_FALSE(t.proc.unfiltered_chars);
    t.proc.translate_esc_extending("bbb", /*nw*/3, /*nr*/1);
    EXPECT_EQ(t.proc.rpos, 6);
    EXPECT_EQ(t.proc.wpos, 6);
    EXPECT_EQ(t.proc.src.len, 12);
    EXPECT_EQ(t.proc.sofar(), "aaabbb");
    EXPECT_EQ(t.proc.result().get(), "aaabbb");
    EXPECT_EQ(full_subject, "aaabbb223344^^");
    EXPECT_FALSE(t.proc.unfiltered_chars);
    t.proc.translate_esc_extending("ccc", /*nw*/3, /*nr*/1);
    EXPECT_EQ(t.proc.rpos, 9);
    EXPECT_EQ(t.proc.wpos, 9);
    EXPECT_EQ(t.proc.src.len, 13);
    EXPECT_EQ(t.proc.sofar(), "aaabbbccc");
    EXPECT_EQ(t.proc.result().get(), "aaabbbccc");
    EXPECT_EQ(full_subject, "aaabbbccc3344^");
    EXPECT_FALSE(t.proc.unfiltered_chars);
    t.proc.translate_esc_extending("ddd", /*nw*/3, /*nr*/1);
    EXPECT_EQ(t.proc.rpos, 12);
    EXPECT_EQ(t.proc.wpos, 12);
    EXPECT_EQ(t.proc.src.len, 14);
    EXPECT_EQ(t.proc.sofar(), "aaabbbcccddd");
    EXPECT_EQ(t.proc.result().get(), "aaabbbcccddd");
    EXPECT_EQ(full_subject, "aaabbbcccddd44");
    EXPECT_FALSE(t.proc.unfiltered_chars);
    t.proc.translate_esc_extending("eeee", /*nw*/4, /*nr*/1);
    EXPECT_EQ(t.proc.rpos, 14);
    EXPECT_EQ(t.proc.wpos, 16);
    EXPECT_EQ(t.proc.src.len, smaller_capacity); // not 16! limited
    EXPECT_EQ(t.proc.sofar(), "aaabbbcccddd44");
    EXPECT_EQ(t.proc.result().str, nullptr);
    EXPECT_EQ(full_subject, "aaabbbcccddd44");
    EXPECT_TRUE(t.proc.unfiltered_chars);
}

TEST(FilterProcessorInplaceMid, copy_after_translate_esc_extending_bulk_excess__trimmed_capacity)
{
    {
        TesterInplaceMid t("0011223344");
        t.trim_capacity();
        EXPECT_EQ(t.proc.wcap, 10);
        EXPECT_EQ(t.proc.rpos, 0);
        EXPECT_EQ(t.proc.wpos, 0);
        EXPECT_EQ(t.proc.src.len, 10);
        EXPECT_FALSE(t.proc.unfiltered_chars);
        EXPECT_EQ(t.proc.sofar(), "");
        t.proc.translate_esc_extending("aaaa", /*nw*/4, /*nr*/1);
        EXPECT_EQ(t.proc.rpos, 2);
        EXPECT_EQ(t.proc.wpos, 4);
        EXPECT_EQ(t.proc.src.len, 10);
        EXPECT_EQ(t.proc.sofar(), "0011");
        EXPECT_EQ(t.proc.result().str, nullptr);
        EXPECT_EQ(t.subject, "0011223344");
        EXPECT_TRUE(t.proc.unfiltered_chars);
        t.proc.copy(); // do not write!
        EXPECT_EQ(t.proc.rpos, 3);
        EXPECT_EQ(t.proc.wpos, 5);
        EXPECT_EQ(t.proc.src.len, 10);
        EXPECT_EQ(t.proc.sofar(), "00112"); // must not copy 1.
        EXPECT_EQ(t.proc.result().str, nullptr);
        EXPECT_EQ(t.subject, "0011223344");
        EXPECT_TRUE(t.proc.unfiltered_chars);
    }
    {
        TesterInplaceMid t("0011223344");
        t.trim_capacity();
        EXPECT_EQ(t.proc.wcap, 10);
        EXPECT_EQ(t.proc.rpos, 0);
        EXPECT_EQ(t.proc.wpos, 0);
        EXPECT_EQ(t.proc.src.len, 10);
        EXPECT_FALSE(t.proc.unfiltered_chars);
        EXPECT_EQ(t.proc.sofar(), "");
        t.proc.translate_esc_extending("aaaa", /*nw*/4, /*nr*/1);
        EXPECT_EQ(t.proc.rpos, 2);
        EXPECT_EQ(t.proc.wpos, 4);
        EXPECT_EQ(t.proc.src.len, 10);
        EXPECT_EQ(t.proc.sofar(), "0011");
        EXPECT_EQ(t.proc.result().str, nullptr);
        EXPECT_EQ(t.subject, "0011223344");
        EXPECT_TRUE(t.proc.unfiltered_chars);
        t.proc.copy(2);
        EXPECT_EQ(t.proc.rpos, 4);
        EXPECT_EQ(t.proc.wpos, 6);
        EXPECT_EQ(t.proc.src.len, 10);
        EXPECT_EQ(t.proc.sofar(), "001122"); // must not copy 11.
        EXPECT_EQ(t.proc.result().str, nullptr);
        EXPECT_EQ(t.subject, "0011223344");
        EXPECT_TRUE(t.proc.unfiltered_chars);
    }
}

TEST(FilterProcessorInplaceMid, set_after_translate_esc_extending_bulk_excess__trimmed_capacity)
{
    {
        TesterInplaceMid t("0011223344");
        t.trim_capacity();
        EXPECT_EQ(t.proc.wcap, 10);
        EXPECT_EQ(t.proc.rpos, 0);
        EXPECT_EQ(t.proc.wpos, 0);
        EXPECT_EQ(t.proc.src.len, 10);
        EXPECT_FALSE(t.proc.unfiltered_chars);
        EXPECT_EQ(t.proc.sofar(), "");
        t.proc.translate_esc_extending("aaaa", /*nw*/4, /*nr*/1);
        EXPECT_EQ(t.proc.rpos, 2);
        EXPECT_EQ(t.proc.wpos, 4);
        EXPECT_EQ(t.proc.src.len, 10);
        EXPECT_EQ(t.proc.sofar(), "0011");
        EXPECT_EQ(t.proc.result().str, nullptr);
        EXPECT_EQ(t.subject, "0011223344");
        EXPECT_TRUE(t.proc.unfiltered_chars);
        t.proc.set('!'); // do not write!
        EXPECT_EQ(t.proc.rpos, 2);
        EXPECT_EQ(t.proc.wpos, 5);
        EXPECT_EQ(t.proc.src.len, 10);
        EXPECT_EQ(t.proc.sofar(), "00112"); // must not set '!'
        EXPECT_EQ(t.proc.result().str, nullptr);
        EXPECT_EQ(t.subject, "0011223344");
        EXPECT_TRUE(t.proc.unfiltered_chars);
    }
    {
        TesterInplaceMid t("0011223344");
        t.trim_capacity();
        EXPECT_EQ(t.proc.wcap, 10);
        EXPECT_EQ(t.proc.rpos, 0);
        EXPECT_EQ(t.proc.wpos, 0);
        EXPECT_EQ(t.proc.src.len, 10);
        EXPECT_FALSE(t.proc.unfiltered_chars);
        EXPECT_EQ(t.proc.sofar(), "");
        t.proc.translate_esc_extending("aaaa", /*nw*/4, /*nr*/1);
        EXPECT_EQ(t.proc.rpos, 2);
        EXPECT_EQ(t.proc.wpos, 4);
        EXPECT_EQ(t.proc.src.len, 10);
        EXPECT_EQ(t.proc.sofar(), "0011");
        EXPECT_EQ(t.proc.result().str, nullptr);
        EXPECT_EQ(t.subject, "0011223344");
        EXPECT_TRUE(t.proc.unfiltered_chars);
        t.proc.set('!', 2);
        EXPECT_EQ(t.proc.rpos, 2);
        EXPECT_EQ(t.proc.wpos, 6);
        EXPECT_EQ(t.proc.src.len, 10);
        EXPECT_EQ(t.proc.sofar(), "001122"); // must not set '!'
        EXPECT_EQ(t.proc.result().str, nullptr);
        EXPECT_EQ(t.subject, "0011223344");
        EXPECT_TRUE(t.proc.unfiltered_chars);
    }
}

TEST(FilterProcessorInplaceMid, translate_esc_extending_with_temporary_excess_requirement__trimmed_capacity)
{
    TesterInplaceMid t("00112233445566");
    t.trim_capacity();
    EXPECT_EQ(t.proc.wcap, 14);
    EXPECT_EQ(t.proc.rpos, 0);
    EXPECT_EQ(t.proc.wpos, 0);
    EXPECT_EQ(t.proc.src.len, 14);
    EXPECT_FALSE(t.proc.unfiltered_chars);
    EXPECT_EQ(t.proc.sofar(), "");
    EXPECT_EQ(t.proc.maxcap, 14);
    // 00112233445566
    // ^ (rpos)
    // ^ (wpos)
    t.proc.translate_esc_extending("aaaa", /*nw*/4, /*nr*/1);
    EXPECT_EQ(t.proc.rpos, 2);
    EXPECT_EQ(t.proc.wpos, 4);
    EXPECT_EQ(t.proc.src.len, 14);
    EXPECT_EQ(t.proc.sofar(), "0011");
    EXPECT_EQ(t.proc.result().str.str, nullptr);
    EXPECT_EQ(t.proc.result().str.len, 4);
    EXPECT_EQ(t.proc.result().required_len(), 16);
    EXPECT_EQ(t.subject, "00112233445566");
    EXPECT_EQ(t.proc.maxcap, 16);   // increased!
    EXPECT_TRUE(t.proc.unfiltered_chars);
    // 00112233445566
    //   ^ (rpos)
    //     ^ (wpos)
    t.proc.translate_esc('b'); // do not write!
    EXPECT_EQ(t.proc.rpos, 4);
    EXPECT_EQ(t.proc.wpos, 5);
    EXPECT_EQ(t.proc.src.len, 14);
    EXPECT_EQ(t.proc.sofar(), "00112"); // must not set 'b'
    EXPECT_EQ(t.proc.result().str.str, nullptr);
    EXPECT_EQ(t.proc.result().str.len, 5);
    EXPECT_EQ(t.proc.result().required_len(), 16);
    EXPECT_EQ(t.subject, "00112233445566");
    EXPECT_EQ(t.proc.maxcap, 16);
    EXPECT_TRUE(t.proc.unfiltered_chars);
    // 00112233445566
    //     ^ (rpos)
    //      ^ (wpos)
    t.proc.translate_esc('c'); // do not write!
    EXPECT_EQ(t.proc.rpos, 6);
    EXPECT_EQ(t.proc.wpos, 6);
    EXPECT_EQ(t.proc.src.len, 14);
    EXPECT_EQ(t.proc.sofar(), "001122"); // must not set 'c'
    EXPECT_EQ(t.proc.result().str.str, nullptr);
    EXPECT_EQ(t.proc.result().str.len, 6);
    EXPECT_EQ(t.proc.result().required_len(), 16);
    EXPECT_EQ(t.subject, "00112233445566");
    EXPECT_EQ(t.proc.maxcap, 16);
    EXPECT_TRUE(t.proc.unfiltered_chars);
    // 00112233445566
    //       ^ (rpos)
    //       ^ (wpos)
    t.proc.translate_esc('d'); // can write
    EXPECT_EQ(t.proc.rpos, 8);
    EXPECT_EQ(t.proc.wpos, 7);
    EXPECT_EQ(t.proc.src.len, 14);
    EXPECT_EQ(t.proc.sofar(), "0011223");
    EXPECT_EQ(t.proc.result().str.str, nullptr);
    EXPECT_EQ(t.proc.result().str.len, 7);
    EXPECT_EQ(t.proc.result().required_len(), 16);
    EXPECT_EQ(t.subject, "00112233445566");
    EXPECT_EQ(t.proc.maxcap, 16);
    EXPECT_TRUE(t.proc.unfiltered_chars);
    // 00112233445566
    //         ^ (rpos)
    //        ^ (wpos)
    t.proc.translate_esc('e'); // can write
    EXPECT_EQ(t.proc.rpos, 10);
    EXPECT_EQ(t.proc.wpos, 8);
    EXPECT_EQ(t.proc.src.len, 14);
    EXPECT_EQ(t.proc.sofar(), "00112233"); // can set because now wpos < rpos
    EXPECT_EQ(t.proc.result().str.str, nullptr);
    EXPECT_EQ(t.proc.result().str.len, 8);
    EXPECT_EQ(t.proc.result().required_len(), 16);
    EXPECT_EQ(t.subject, "00112233445566");
    EXPECT_EQ(t.proc.maxcap, 16);
    EXPECT_TRUE(t.proc.unfiltered_chars);
    // 00112233445566
    //           ^ (rpos)
    //         ^ (wpos)
    t.proc.translate_esc('f'); // can write
    EXPECT_EQ(t.proc.rpos, 12);
    EXPECT_EQ(t.proc.wpos, 9);
    EXPECT_EQ(t.proc.src.len, 14);
    EXPECT_EQ(t.proc.sofar(), "001122334"); // can set because now wpos < rpos
    EXPECT_EQ(t.proc.result().str.str, nullptr);
    EXPECT_EQ(t.proc.result().str.len, 9);
    EXPECT_EQ(t.proc.result().required_len(), 16);
    EXPECT_EQ(t.subject, "00112233445566");
    EXPECT_EQ(t.proc.maxcap, 16);
    EXPECT_TRUE(t.proc.unfiltered_chars);
    // 00112233445566
    //             ^ (rpos)
    //          ^ (wpos)
    t.proc.translate_esc('g'); // can write
    EXPECT_EQ(t.proc.rpos, 14);
    EXPECT_EQ(t.proc.wpos, 10);
    EXPECT_EQ(t.proc.src.len, 14);
    EXPECT_EQ(t.proc.sofar(), "0011223344"); // can set because now wpos < rpos
    EXPECT_EQ(t.proc.result().str.str, nullptr);
    EXPECT_EQ(t.proc.result().str.len, 10);
    EXPECT_EQ(t.proc.result().required_len(), 16);
    EXPECT_EQ(t.subject, "00112233445566");
    EXPECT_EQ(t.proc.maxcap, 16);
    EXPECT_TRUE(t.proc.unfiltered_chars);
    // 00112233445566
    //               ^ (rpos)
    //           ^ (wpos)
    t.proc.set('h'); // can write
    EXPECT_EQ(t.proc.rpos, 14);
    EXPECT_EQ(t.proc.wpos, 11);
    EXPECT_EQ(t.proc.src.len, 14);
    EXPECT_EQ(t.proc.sofar(), "00112233445");
    EXPECT_EQ(t.proc.result().str.str, nullptr);
    EXPECT_EQ(t.proc.result().str.len, 11);
    EXPECT_EQ(t.proc.result().required_len(), 16);
    EXPECT_EQ(t.subject, "00112233445566");
    EXPECT_EQ(t.proc.maxcap, 16);
    EXPECT_TRUE(t.proc.unfiltered_chars);
    // 00112233445566
    //               ^ (rpos)
    //            ^ (wpos)
    t.proc.set('i'); // can write
    EXPECT_EQ(t.proc.rpos, 14);
    EXPECT_EQ(t.proc.wpos, 12);
    EXPECT_EQ(t.proc.src.len, 14);
    EXPECT_EQ(t.proc.sofar(), "001122334455");
    EXPECT_EQ(t.proc.result().str.str, nullptr);
    EXPECT_EQ(t.proc.result().str.len, 12);
    EXPECT_EQ(t.proc.result().required_len(), 16);
    EXPECT_EQ(t.subject, "00112233445566");
    EXPECT_EQ(t.proc.maxcap, 16);
    EXPECT_TRUE(t.proc.unfiltered_chars);
    // 00112233445566
    //               ^ (rpos)
    //             ^ (wpos)
    t.proc.set('j'); // can write
    EXPECT_EQ(t.proc.rpos, 14);
    EXPECT_EQ(t.proc.wpos, 13);
    EXPECT_EQ(t.proc.src.len, 14);
    EXPECT_EQ(t.proc.sofar(), "0011223344556");
    EXPECT_EQ(t.proc.result().str.str, nullptr);
    EXPECT_EQ(t.proc.result().str.len, 13);
    EXPECT_EQ(t.proc.result().required_len(), 16);
    EXPECT_EQ(t.subject, "00112233445566");
    EXPECT_EQ(t.proc.maxcap, 16);
    EXPECT_TRUE(t.proc.unfiltered_chars);
    // 00112233445566
    //               ^ (rpos)
    //              ^ (wpos)
    t.proc.set('k'); // can write
    EXPECT_EQ(t.proc.rpos, 14);
    EXPECT_EQ(t.proc.wpos, 14);
    EXPECT_EQ(t.proc.src.len, 14);
    EXPECT_EQ(t.proc.sofar(), "00112233445566");
    EXPECT_EQ(t.proc.result().str.str, nullptr);
    EXPECT_EQ(t.proc.result().str.len, 14);
    EXPECT_EQ(t.proc.result().required_len(), 16);
    EXPECT_EQ(t.subject, "00112233445566");
    EXPECT_EQ(t.proc.maxcap, 16);
    EXPECT_TRUE(t.proc.unfiltered_chars);
    // 00112233445566
    //               ^ (rpos)
    //               ^ (wpos)
    t.proc.set('!'); // cannot write
    EXPECT_EQ(t.proc.rpos, 14);
    EXPECT_EQ(t.proc.wpos, 15);
    EXPECT_EQ(t.proc.src.len, 14);
    EXPECT_EQ(t.proc.sofar(), "00112233445566");
    EXPECT_EQ(t.proc.result().str.str, nullptr);
    EXPECT_EQ(t.proc.result().str.len, 15);
    EXPECT_EQ(t.proc.result().required_len(), 16);
    EXPECT_EQ(t.subject, "00112233445566");
    EXPECT_EQ(t.proc.maxcap, 16);
    EXPECT_TRUE(t.proc.unfiltered_chars);
    // 00112233445566
    //               ^ (rpos)
    //                ^ (wpos)
    t.proc.set('!'); // cannot write
    EXPECT_EQ(t.proc.rpos, 14);
    EXPECT_EQ(t.proc.wpos, 16);
    EXPECT_EQ(t.proc.src.len, 14);
    EXPECT_EQ(t.proc.sofar(), "00112233445566");
    EXPECT_EQ(t.proc.result().str.str, nullptr);
    EXPECT_EQ(t.proc.result().str.len, 16);
    EXPECT_EQ(t.proc.result().required_len(), 16);
    EXPECT_EQ(t.subject, "00112233445566");
    EXPECT_EQ(t.proc.maxcap, 16);
    EXPECT_TRUE(t.proc.unfiltered_chars);
    // 00112233445566
    //               ^ (rpos)
    //                 ^ (wpos)
    t.proc.set('!'); // cannot write
    EXPECT_EQ(t.proc.rpos, 14);
    EXPECT_EQ(t.proc.wpos, 17);
    EXPECT_EQ(t.proc.src.len, 14);
    EXPECT_EQ(t.proc.sofar(), "00112233445566");
    EXPECT_EQ(t.proc.result().str.str, nullptr);
    EXPECT_EQ(t.proc.result().str.len, 17);
    EXPECT_EQ(t.proc.result().required_len(), 17); // increased!
    EXPECT_EQ(t.subject, "00112233445566");
    EXPECT_EQ(t.proc.maxcap, 17);
    EXPECT_TRUE(t.proc.unfiltered_chars);
}

TEST(FilterProcessorInplaceMid, translate_esc_extending_after_translate_esc_extending_bulk_excess__trimmed_capacity)
{
    {
        TesterInplaceMid t("0011223344");
        t.trim_capacity();
        EXPECT_EQ(t.proc.wcap, 10);
        EXPECT_EQ(t.proc.rpos, 0);
        EXPECT_EQ(t.proc.wpos, 0);
        EXPECT_EQ(t.proc.src.len, 10);
        EXPECT_FALSE(t.proc.unfiltered_chars);
        EXPECT_EQ(t.proc.sofar(), "");
        t.proc.translate_esc_extending("aaaa", /*nw*/4, /*nr*/1);
        EXPECT_EQ(t.proc.rpos, 2);
        EXPECT_EQ(t.proc.wpos, 4);
        EXPECT_EQ(t.proc.src.len, 10);
        EXPECT_EQ(t.proc.sofar(), "0011");
        EXPECT_EQ(t.proc.result().str, nullptr);
        EXPECT_EQ(t.subject, "0011223344");
        EXPECT_TRUE(t.proc.unfiltered_chars);
        t.proc.translate_esc('!'); // do not write!
        EXPECT_EQ(t.proc.rpos, 4);
        EXPECT_EQ(t.proc.wpos, 5);
        EXPECT_EQ(t.proc.src.len, 10);
        EXPECT_EQ(t.proc.sofar(), "00112"); // must not set '!'
        EXPECT_EQ(t.proc.result().str, nullptr);
        EXPECT_EQ(t.subject, "0011223344");
        EXPECT_TRUE(t.proc.unfiltered_chars);
    }
    {
        TesterInplaceMid t("0011223344");
        t.trim_capacity();
        EXPECT_EQ(t.proc.wcap, 10);
        EXPECT_EQ(t.proc.rpos, 0);
        EXPECT_EQ(t.proc.wpos, 0);
        EXPECT_EQ(t.proc.src.len, 10);
        EXPECT_FALSE(t.proc.unfiltered_chars);
        EXPECT_EQ(t.proc.sofar(), "");
        t.proc.translate_esc_extending("aaaa", /*nw*/4, /*nr*/1);
        EXPECT_EQ(t.proc.rpos, 2);
        EXPECT_EQ(t.proc.wpos, 4);
        EXPECT_EQ(t.proc.src.len, 10);
        EXPECT_EQ(t.proc.sofar(), "0011");
        EXPECT_EQ(t.proc.result().str, nullptr);
        EXPECT_EQ(t.subject, "0011223344");
        EXPECT_TRUE(t.proc.unfiltered_chars);
        t.proc.translate_esc_bulk("!!", /*nw*/2, /*nr*/1);
        EXPECT_EQ(t.proc.rpos, 4);
        EXPECT_EQ(t.proc.wpos, 6);
        EXPECT_EQ(t.proc.src.len, 10);
        EXPECT_EQ(t.proc.sofar(), "001122"); // must not set '!'
        EXPECT_EQ(t.proc.result().str, nullptr);
        EXPECT_EQ(t.subject, "0011223344");
        EXPECT_TRUE(t.proc.unfiltered_chars);
    }
}


//-----------------------------------------------------------------------------

TEST(FilterProcessorSrcDst, translate_esc_extending_bulk_excess)
{
    TesterSrcDst t("0011223344");
    const size_t needed_size = 16u;
    const size_t smaller_size = 14u;
    ASSERT_LT(smaller_size, needed_size);
    t.set_dst_size(smaller_size);
    ASSERT_EQ(t.dst.len, smaller_size);
    to_substr(t.dst).fill('^');
    EXPECT_EQ(t.proc.rpos, 0);
    EXPECT_EQ(t.proc.wpos, 0);
    EXPECT_EQ(t.proc.sofar(), "");
    EXPECT_EQ(t.dst, csubstr("^^^^^^^^^^^^^^"));
    t.proc.translate_esc_extending("aaa", /*nw*/3, /*nr*/1);
    EXPECT_EQ(t.proc.rpos, 2);
    EXPECT_EQ(t.proc.wpos, 3);
    EXPECT_EQ(t.proc.sofar(), "aaa");
    EXPECT_EQ(t.proc.result().get(), "aaa");
    EXPECT_EQ(t.dst, "aaa^^^^^^^^^^^");
    t.proc.translate_esc_extending("bbb", /*nw*/3, /*nr*/1);
    EXPECT_EQ(t.proc.rpos, 4);
    EXPECT_EQ(t.proc.wpos, 6);
    EXPECT_EQ(t.proc.sofar(), "aaabbb");
    EXPECT_EQ(t.proc.result().get(), "aaabbb");
    EXPECT_EQ(t.dst, "aaabbb^^^^^^^^");
    t.proc.translate_esc_extending("ccc", /*nw*/3, /*nr*/1);
    EXPECT_EQ(t.proc.rpos, 6);
    EXPECT_EQ(t.proc.wpos, 9);
    EXPECT_EQ(t.proc.sofar(), "aaabbbccc");
    EXPECT_EQ(t.proc.result().get(), "aaabbbccc");
    EXPECT_EQ(t.dst, "aaabbbccc^^^^^");
    t.proc.translate_esc_extending("ddd", /*nw*/3, /*nr*/1);
    EXPECT_EQ(t.proc.rpos, 8);
    EXPECT_EQ(t.proc.wpos, 12);
    EXPECT_EQ(t.proc.sofar(), "aaabbbcccddd");
    EXPECT_EQ(t.proc.result().get(), "aaabbbcccddd");
    EXPECT_EQ(t.dst, "aaabbbcccddd^^");
    t.proc.translate_esc_extending("eeee", /*nw*/4, /*nr*/1);
    EXPECT_EQ(t.proc.rpos, 10);
    EXPECT_EQ(t.proc.wpos, 16);
    EXPECT_EQ(t.proc.sofar(), "aaabbbcccddd^^");
    EXPECT_EQ(t.proc.result().str, nullptr);
    EXPECT_EQ(t.dst, "aaabbbcccddd^^");
}

TEST(Filter, _find_last_newline_and_larger_indentation)
{
    EXPECT_EQ(_find_last_newline_and_larger_indentation("ab\n\n\n", 0), npos);
    EXPECT_EQ(_find_last_newline_and_larger_indentation("ab\n \n\n", 0), 2u);
    EXPECT_EQ(_find_last_newline_and_larger_indentation("ab\n\n \n", 0), 3u);
    EXPECT_EQ(_find_last_newline_and_larger_indentation("ab\n \n \n", 0), 4u);
    EXPECT_EQ(_find_last_newline_and_larger_indentation("ab\n \n  \n", 1), 4u);
    EXPECT_EQ(_find_last_newline_and_larger_indentation("ab\n  \n \n", 1), 2u);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(Parser, parser_options_filtering)
{
    ParserOptions opt = {};
    EXPECT_TRUE(opt.scalar_filtering());
    opt.scalar_filtering(false);
    EXPECT_FALSE(opt.scalar_filtering());
    opt.scalar_filtering(true);
    EXPECT_TRUE(opt.scalar_filtering());
}

TEST(Parser, parse_without_filtering_where_none_required)
{
    EventHandlerTree evt_handler = {};
    Parser parser(&evt_handler, ParserOptions().scalar_filtering(false));
    csubstr yaml = R"(plain key: plain val
'squo key': 'squo val'
"dquo key": "dquo val"
? |
: |
? >
: >
)";
    const Tree tree = parse_in_arena(&parser, yaml);
    EXPECT_FALSE(tree[0].type() & KEY_UNFILT);
    EXPECT_FALSE(tree[0].type() & VAL_UNFILT);
    EXPECT_FALSE(tree[1].type() & KEY_UNFILT);
    EXPECT_FALSE(tree[1].type() & VAL_UNFILT);
    EXPECT_FALSE(tree[2].type() & KEY_UNFILT);
    EXPECT_FALSE(tree[2].type() & VAL_UNFILT);
    EXPECT_TRUE(tree[3].type() & KEY_UNFILT); // block scalars always require filtering
    EXPECT_TRUE(tree[3].type() & VAL_UNFILT); // block scalars always require filtering
    EXPECT_TRUE(tree[4].type() & KEY_UNFILT); // block scalars always require filtering
    EXPECT_TRUE(tree[4].type() & VAL_UNFILT); // block scalars always require filtering
}

TEST(Parser, parse_without_filtering_where_it_is_required)
{
    EventHandlerTree evt_handler = {};
    Parser parser(&evt_handler, ParserOptions().scalar_filtering(false));
    csubstr yaml = R"(? plain
    key
: plain
    val
? 'squo
    key'
: 'squo
    val'
? "dquo
    key"
: "dquo
    val"
? |
  literal
  key
: |
  literal
  value
? >
  folded
  key
: >
  folded
  value
)";
    const Tree tree = parse_in_arena(&parser, yaml);
    EXPECT_TRUE(tree[0].type() & KEY_UNFILT);
    EXPECT_TRUE(tree[0].type() & VAL_UNFILT);
    EXPECT_TRUE(tree[1].type() & KEY_UNFILT);
    EXPECT_TRUE(tree[1].type() & VAL_UNFILT);
    EXPECT_TRUE(tree[2].type() & KEY_UNFILT);
    EXPECT_TRUE(tree[2].type() & VAL_UNFILT);
    EXPECT_TRUE(tree[3].type() & KEY_UNFILT);
    EXPECT_TRUE(tree[3].type() & VAL_UNFILT);
    EXPECT_TRUE(tree[4].type() & KEY_UNFILT);
    EXPECT_TRUE(tree[4].type() & VAL_UNFILT);
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
