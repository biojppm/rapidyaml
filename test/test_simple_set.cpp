#include "./test_group.hpp"

namespace c4 {
namespace yml {


TEST(simple_set, emit)
{
    const char yaml[] = R"(!!set
? aa
? bb
? cc
)";
    std::string expected = R"(!!set
aa: 
bb: 
cc: 
)";
    Tree t = parse_in_arena(yaml);
    auto s = emitrs_yaml<std::string>(t);
    EXPECT_EQ(expected, s);
}


TEST(simple_set, emit_doc)
{
    const char yaml[] = R"(--- !!set
? aa
? bb
? cc
)";
    std::string expected = R"(--- !!set
aa: 
bb: 
cc: 
)";
    Tree t = parse_in_arena(yaml);
    auto s = emitrs_yaml<std::string>(t);
    EXPECT_EQ(expected, s);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

CASE_GROUP(SIMPLE_SET)
{

ADD_CASE_TO_GROUP("doc as set, missing value",
R"(!!set
? a
? b
?
)",
N(TL("!!set", L{N(KEYVAL, "a", {}), N(KEYVAL, "b", {}), N(KEYVAL, {}, "")}))
);

ADD_CASE_TO_GROUP("doc as set, implicit",
R"(!!set
? a
? b
)",
N(TL("!!set", L{N(KEYVAL, "a", {}), N(KEYVAL, "b", {})}))
);

ADD_CASE_TO_GROUP("doc as set",
R"(--- !!set
? aa
? bb
? cc
)",
N(STREAM, L{N(DOCMAP, TL("!!set", L{
  N(KEYVAL, "aa", /*"~"*/{}),
  N(KEYVAL, "bb", /*"~"*/{}),
  N(KEYVAL, "cc", /*"~"*/{})})
)}));

ADD_CASE_TO_GROUP("sets 2XXW",
R"(
--- !!set
? Mark McGwire
? Sammy Sosa
? Ken Griff
)",
N(STREAM, L{N(DOCMAP, TL("!!set", L{
  N(KEYVAL, "Mark McGwire", /*"~"*/{}),
  N(KEYVAL, "Sammy Sosa", /*"~"*/{}),
  N(KEYVAL, "Ken Griff", /*"~"*/{}),})
)}));

ADD_CASE_TO_GROUP("sets 2XXW, indented",
R"(
    --- !!set
    ? Mark McGwire
    ? Sammy Sosa
    ? Ken Griff
)",
N(STREAM, L{N(DOCMAP, TL("!!set", L{
  N(KEYVAL, "Mark McGwire", /*"~"*/{}),
  N(KEYVAL, "Sammy Sosa", /*"~"*/{}),
  N(KEYVAL, "Ken Griff", /*"~"*/{}),})
)}));

ADD_CASE_TO_GROUP("sets 2XXW, no set",
R"(
---
? Mark McGwire
? Sammy Sosa
? Ken Griff
)",
N(STREAM, L{N(DOCMAP, L{
  N(KEYVAL, "Mark McGwire", /*"~"*/{}),
  N(KEYVAL, "Sammy Sosa", /*"~"*/{}),
  N(KEYVAL, "Ken Griff", /*"~"*/{}),}
)}));

ADD_CASE_TO_GROUP("sets 2XXW, no doc",
R"(!!set
? Mark McGwire
? Sammy Sosa
? Ken Griff
)",
TL("!!set", L{
  N(KEYVAL, "Mark McGwire", /*"~"*/{}),
  N(KEYVAL, "Sammy Sosa", /*"~"*/{}),
  N(KEYVAL, "Ken Griff", /*"~"*/{}),
}));

ADD_CASE_TO_GROUP("sets 2XXW, no doc, no set",
R"(
? Mark McGwire
? Sammy Sosa
? Ken Griff
)",
L{
  N(KEYVAL, "Mark McGwire", /*"~"*/{}),
  N(KEYVAL, "Sammy Sosa", /*"~"*/{}),
  N(KEYVAL, "Ken Griff", /*"~"*/{}),
});
}

} // namespace yml
} // namespace c4
