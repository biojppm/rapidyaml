#include "test_lib/test_compare_events.hpp"
#include "test_lib/test_case.hpp"

namespace c4 {
namespace yml {

csubstr parse_anchor_and_tag(csubstr tokens, OptionalScalar *anchor, OptionalScalar *tag)
{
    *anchor = OptionalScalar{};
    *tag = OptionalScalar{};
    if(tokens.begins_with('&'))
    {
        size_t pos = tokens.first_of(' ');
        if(pos == (size_t)csubstr::npos)
        {
            *anchor = tokens.sub(1);
            tokens = {};
        }
        else
        {
            *anchor = tokens.first(pos).sub(1);
            tokens = tokens.right_of(pos);
        }
        _c4dbgpf("anchor: {}", anchor->get());
    }
    if(tokens.begins_with('<'))
    {
        size_t pos = tokens.find('>');
        RYML_ASSERT_BASIC_(pos != (size_t)csubstr::npos);
        *tag = tokens.first(pos + 1);
        tokens = tokens.right_of(pos).triml(' ');
        _c4dbgpf("tag: {}", tag->maybe_get());
    }
    return tokens;
}

void test_compare_events(csubstr ref_evts,
                         csubstr emt_evts,
                         bool ignore_doc_style,
                         bool ignore_container_style,
                         bool ignore_scalar_style,
                         bool ignore_tag_normalization)
{
    RYML_TRACE_FMT("actual=~~~\n{}~~~", emt_evts);
    RYML_TRACE_FMT("expected=~~~\n{}~~~", ref_evts);
    auto test_anchor_tag = [&](csubstr &ref, csubstr &emt){
        OptionalScalar reftag = {}, refanchor = {};
        OptionalScalar emttag = {}, emtanchor = {};
        ref = parse_anchor_and_tag(ref, &refanchor, &reftag).triml(' ');
        emt = parse_anchor_and_tag(emt, &emtanchor, &emttag).triml(' ');
        EXPECT_EQ(bool(refanchor), bool(emtanchor));
        if(bool(refanchor) && bool(emtanchor))
        {
            EXPECT_EQ(refanchor.get(), emtanchor.get());
        }
        EXPECT_EQ(bool(reftag), bool(emttag));
        if(reftag && emttag)
        {
            if(!ignore_tag_normalization)
            {
                EXPECT_EQ(reftag.get(), emttag.get());
            }
        }
    };
    auto test_doc = [&](csubstr ref, csubstr emt){
        EXPECT_TRUE(ref.begins_with("+DOC") || ref.begins_with("-DOC"));
        EXPECT_TRUE(emt.begins_with("+DOC") || emt.begins_with("-DOC"));
        EXPECT_EQ(emt.begins_with("+DOC"), ref.begins_with("+DOC"));
        EXPECT_EQ(emt.begins_with("-DOC"), ref.begins_with("-DOC"));
        if(ignore_doc_style)
        {
            if(ref.begins_with("+DOC"))
            {
                ref = ref.stripr("---").trimr(' ');
                emt = emt.stripr("---").trimr(' ');
            }
            else
            {
                ASSERT_TRUE(ref.begins_with("-DOC"));
                ref = ref.stripr("...").trimr(' ');
                emt = emt.stripr("...").trimr(' ');
            }
        }
        EXPECT_EQ(ref, emt);
    };
    auto test_container = [&](csubstr ref, csubstr emt){
        RYML_TRACE_FMT("expected={}", ref);
        RYML_TRACE_FMT("actual={}", emt);
        EXPECT_TRUE(ref.begins_with("+MAP") || ref.begins_with("+SEQ"));
        EXPECT_TRUE(emt.begins_with("+MAP") || emt.begins_with("+SEQ"));
        EXPECT_EQ(emt.begins_with("+MAP"), ref.begins_with("+MAP"));
        EXPECT_EQ(emt.begins_with("+SEQ"), ref.begins_with("+SEQ"));
        csubstr rest_ref = ref.sub(4).triml(' ');
        csubstr rest_emt = emt.sub(4).triml(' ');
        if(rest_ref.begins_with("{}"))
        {
            if(!ignore_container_style)
            {
                EXPECT_TRUE(rest_emt.begins_with("{}"));
            }
        }
        else if(rest_ref.begins_with("[]"))
        {
            if(!ignore_container_style)
            {
                EXPECT_TRUE(rest_emt.begins_with("[]"));
            }
        }
        test_anchor_tag(ref, emt);
    };
    auto test_val_with_scalar_wildcard = [&](csubstr ref, csubstr emt){
        ASSERT_TRUE(ref.begins_with("=VAL "));
        ASSERT_TRUE(emt.begins_with("=VAL "));
        ref = ref.sub(5);
        emt = emt.sub(5);
        test_anchor_tag(ref, emt);
        ASSERT_GE(ref.len, 0);
        ASSERT_GE(emt.len, 0);
        EXPECT_TRUE(ref[0] == ':' || ref[0] == '\'' || ref[0] == '"' || ref[0] == '|' || ref[0] == '>');
        EXPECT_TRUE(emt[0] == ':' || emt[0] == '\'' || emt[0] == '"' || emt[0] == '|' || emt[0] == '>');
        if(!ignore_scalar_style)
        {
            EXPECT_EQ(ref[0], emt[0]);
        }
        ref = ref.sub(1);
        emt = emt.sub(1);
        EXPECT_EQ(ref, emt);
    };
    EXPECT_EQ(bool(ref_evts.len), bool(emt_evts.len));
    size_t posref = 0;
    size_t posemt = 0;
    while(posref < ref_evts.len && posemt < emt_evts.len)
    {
        const size_t endref = ref_evts.find('\n', posref);
        const size_t endemt = emt_evts.find('\n', posemt);
        ASSERT_FALSE((endref == npos || endemt == npos) && (endref != endemt));
        csubstr ref = ref_evts.range(posref, endref);
        csubstr emt = emt_evts.range(posemt, endemt);
        if(ref != emt)
        {
            if(ref.begins_with("+DOC") || emt.begins_with("-DOC"))
            {
                test_doc(ref, emt);
            }
            else if(ref.begins_with("+MAP") || ref.begins_with("+SEQ"))
            {
                test_container(ref, emt);
            }
            else if(ref.begins_with("=VAL"))
            {
                test_val_with_scalar_wildcard(ref, emt);
            }
            else
            {
                ASSERT_EQ(ref, emt);
            }
        }
        posref = endref + 1u;
        posemt = endemt + 1u;
    }
}

} // namespace yml
} // namespace c4
