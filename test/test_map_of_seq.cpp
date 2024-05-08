#include "./test_lib/test_group.hpp"
#include "./test_lib/test_group.def.hpp"

namespace c4 {
namespace yml {


CASE_GROUP(MAP_OF_SEQ)
{

ADD_CASE_TO_GROUP("map of empty seqs",
R"({foo: [], bar: [], baz: []})",
N(MFS, L{
         N(KP|SFS, "foo", L()),
         N(KP|SFS, "bar", L()),
         N(KP|SFS, "baz", L()),
     })
);

ADD_CASE_TO_GROUP("map of seqs, one line",
R"({men: [John Smith, Bill Jones], women: [Mary Smith, Susan Williams]})",
N(MFS, L{
         N(KP|SFS, "men", L{N{VP, "John Smith"}, N{VP, "Bill Jones"}}),
         N(KP|SFS, "women", L{N{VP, "Mary Smith"}, N{VP, "Susan Williams"}})
     })
);

ADD_CASE_TO_GROUP("map of seqs",
R"(
men:
  - John Smith
  - Bill Jones
women:
  - Mary Smith
  - Susan Williams
)",
N(MB, L{
         N(KP|SB, "men", L{N{VP, "John Smith"}, N{VP, "Bill Jones"}}),
         N(KP|SB, "women", L{N{VP, "Mary Smith"}, N{VP, "Susan Williams"}})
     })
);

ADD_CASE_TO_GROUP("map of seqs, not indented",
R"(
men:
- John Smith
- Bill Jones
women:
- Mary Smith
- Susan Williams
)",
N(MB, L{
         N(KP|SB, "men", L{N{VP, "John Smith"}, N{VP, "Bill Jones"}}),
         N(KP|SB, "women", L{N{VP, "Mary Smith"}, N{VP, "Susan Williams"}})
     })
);

ADD_CASE_TO_GROUP("map of seqs, not indented, more",
R"(
product:
- sku: BL4438H
  quantity: 1
  description: Super Hoop
  price: 2392.00  # jumping one level here would be wrong.
tax: 1234.5       # we must jump two levels
product2:
  subproduct1:
  - sku: BL4438H
    quantity: 1
    description: Super Hoop
    price: 2392.00  # jumping one level here would be wrong.
  subproduct2:
  - sku: BL4438H
    quantity: 1
    description: Super Hoop
    price: 2392.00  # jumping one level here would be wrong.
  tax2: 789.10      # we must jump two levels
tax3: 1234.5
product3:
  subproduct1:
  - sku: BL4438H
    quantity: 1
    description: Super Hoop
    price: 2392.00  # jumping one level here would be wrong.
  subproduct2:
  - sku: BL4438H
    quantity: 1
    description: Super Hoop
    price: 2392.00  # jumping one level here would be wrong.
  # a comment here, will it ruin parsing?
  tax2: 789.10      # we must jump two levels
tax4: 1234.5
product4:
  subproduct1:
  - sku: BL4438H
    quantity: 1
    description: Super Hoop
    price: 2392.00  # jumping one level here would be wrong.
  subproduct2:
  - sku: BL4438H
    quantity: 1
    description: Super Hoop
    price: 2392.00  # jumping one level here would be wrong.
 # what about here?
  tax2: 789.10      # we must jump two levels
tax5: 1234.5
)",
N(MB, L{
    N(KP|SB, "product", L{
       N(MB, L{N(KP|VP, "sku", "BL4438H"), N(KP|VP, "quantity", "1"), N(KP|VP, "description", "Super Hoop"), N(KP|VP, "price", "2392.00")}),
    }),
    N(KP|VP, "tax", "1234.5"),
    N(KP|MB, "product2", L{
      N(KP|SB, "subproduct1", L{
        N(MB, L{N(KP|VP, "sku", "BL4438H"), N(KP|VP, "quantity", "1"), N(KP|VP, "description", "Super Hoop"), N(KP|VP, "price", "2392.00")}),
      }),
      N(KP|SB, "subproduct2", L{
        N(MB, L{N(KP|VP, "sku", "BL4438H"), N(KP|VP, "quantity", "1"), N(KP|VP, "description", "Super Hoop"), N(KP|VP, "price", "2392.00")}),
      }),
      N(KP|VP, "tax2", "789.10"),
    }),
    N(KP|VP, "tax3", "1234.5"),
    N(KP|MB, "product3", L{
      N(KP|SB, "subproduct1", L{
        N(MB, L{N(KP|VP, "sku", "BL4438H"), N(KP|VP, "quantity", "1"), N(KP|VP, "description", "Super Hoop"), N(KP|VP, "price", "2392.00")}),
      }),
      N(KP|SB, "subproduct2", L{
        N(MB, L{N(KP|VP, "sku", "BL4438H"), N(KP|VP, "quantity", "1"), N(KP|VP, "description", "Super Hoop"), N(KP|VP, "price", "2392.00")}),
      }),
      N(KP|VP, "tax2", "789.10"),
    }),
    N(KP|VP, "tax4", "1234.5"),
    N(KP|MB, "product4", L{
      N(KP|SB, "subproduct1", L{
        N(MB, L{N(KP|VP, "sku", "BL4438H"), N(KP|VP, "quantity", "1"), N(KP|VP, "description", "Super Hoop"), N(KP|VP, "price", "2392.00")}),
      }),
      N(KP|SB, "subproduct2", L{
        N(MB, L{N(KP|VP, "sku", "BL4438H"), N(KP|VP, "quantity", "1"), N(KP|VP, "description", "Super Hoop"), N(KP|VP, "price", "2392.00")}),
      }),
      N(KP|VP, "tax2", "789.10"),
    }),
    N(KP|VP, "tax5", "1234.5"),
})
);

ADD_CASE_TO_GROUP("map of seqs, next line",
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
N(MB, L{
        N(KP|SB, "men", L{N{VP, "John Smith"}, N{VP, "Bill Jones"}}),
        N(KP|SB, "women", L{N{VP, "Mary Smith"}, N{VP, "Susan Williams"}})
     })
);

ADD_CASE_TO_GROUP("map of seqs, next line without space",
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
N(MB, L{
        N(KP|SB, "men", L{N{VP, "John Smith"}, N{VP, "Bill Jones"}}),
        N(KP|SB, "women", L{N{VP, "Mary Smith"}, N{VP, "Susan Williams"}})
     })
);

ADD_CASE_TO_GROUP("map of seqs, deal with unk",
R"(
skip_commits:
  files:
    - a
    - b
    - c
    - d
    - e
)",
N(MB, L{
  N(KP|MB, "skip_commits", L{
    N(KP|SB, "files", L{
      N(VP, "a"), N(VP, "b"), N(VP, "c"), N(VP, "d"), N(VP, "e")
    })
  })
})
);
}

} // namespace yml
} // namespace c4
