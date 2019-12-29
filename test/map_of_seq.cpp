#include "./test_group.hpp"

namespace c4 {
namespace yml {


#define MAP_OF_SEQ_CASES \
    "map of empty seqs", \
    "map of seqs, one line", \
    "map of seqs",           \
    "map of seqs, next line",\
    "map of seqs, deal with unk"


CASE_GROUP(MAP_OF_SEQ)
{
    APPEND_CASES(

C("map of empty seqs",
R"({foo: [], bar: [], baz: []})",
     L{
         N(KEYSEQ, "foo", L()),
         N(KEYSEQ, "bar", L()),
         N(KEYSEQ, "baz", L()),
     }
),

C("map of seqs, one line",
R"({men: [John Smith, Bill Jones], women: [Mary Smith, Susan Williams]})",
     L{
         N("men", L{N{"John Smith"}, N{"Bill Jones"}}),
         N("women", L{N{"Mary Smith"}, N{"Susan Williams"}})
     }
),

C("map of seqs",
R"(
men:
  - John Smith
  - Bill Jones
women:
  - Mary Smith
  - Susan Williams
)",
     L{
         N("men", L{N{"John Smith"}, N{"Bill Jones"}}),
         N("women", L{N{"Mary Smith"}, N{"Susan Williams"}})
     }
),

C("map of seqs, next line",
R"(
men:
  - 
    John Smith
  - 
    Bill Jones
women:
  - 
    Mary Smith
  - 
    Susan Williams
)",
     L{
         N("men", L{N{"John Smith"}, N{"Bill Jones"}}),
         N("women", L{N{"Mary Smith"}, N{"Susan Williams"}})
     }
),

C("map of seqs, deal with unk",
R"(
skip_commits:
  files:
    - a
    - b
    - c
    - d
    - e
)",
L{
  N("skip_commits", L{N("files",
    L{N("a"), N("b"), N("c"), N("d"), N("e")}
  )}),
}
),
    )
}

INSTANTIATE_GROUP(MAP_OF_SEQ)

} // namespace yml
} // namespace c4
