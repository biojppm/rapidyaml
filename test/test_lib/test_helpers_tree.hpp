#ifndef TEST_LIB_TEST_TREE_HELPERS_HPP_
#define TEST_LIB_TEST_TREE_HELPERS_HPP_

#include "test_lib/test_utils.hpp"

#ifdef RYML_SINGLE_HEADER
#include <ryml_all.hpp>
#else
#include "c4/yml/fwd.hpp"
#include "c4/yml/node_type.hpp"
#endif

C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")


/** @todo use a matcher and EXPECT_THAT():
 * see http://google.github.io/googletest/reference/assertions.html#EXPECT_THAT
 * see http://google.github.io/googletest/gmock_cook_book.html#NewMatchers
 */
#define RYML_COMPARE_NODE_TYPE(lhs, rhs, op, testop)                    \
    do                                                                  \
    {                                                                   \
        if(!((lhs) op (rhs)))                                           \
        {                                                               \
            char ltypebuf[256];                                         \
            char rtypebuf[256];                                         \
            csubstr ltype = NodeType::type_str_sub(ltypebuf, (type_bits)lhs); \
            csubstr rtype = NodeType::type_str_sub(rtypebuf, (type_bits)rhs); \
            EXPECT_##testop(lhs, rhs);                                  \
            std::cout << __FILE__  << ":" << __LINE__ << ": ...\n";     \
            if(ltype.str && rtype.str)                                  \
            {                                                           \
                std::cout                                               \
                    << "  " << ltype << " (" << (lhs)  << ")" << "=" << #lhs \
                    << "\n"                                             \
                    << "  " << rtype << " (" << (rhs)  << ")" << "=" << #rhs \
                    << "\n";                                            \
            }                                                           \
            else                                                        \
            {                                                           \
                std::cout                                               \
                    << "(type too large to fit print buffer)"           \
                    << "\n";                                            \
            }                                                           \
        }                                                               \
    } while(0)


namespace c4 {
namespace yml {

inline void PrintTo(NodeType ty, ::std::ostream* os)
{
    *os << ty.type_str();
}
inline void PrintTo(NodeTypeBits ty, ::std::ostream* os)
{
    *os << NodeType::type_str(ty);
}


void test_compare(ConstNodeRef const& actual, ConstNodeRef const& expected,
                  const char *actual_name="actual", const char *expected_name="expected",
                  type_bits cmp_mask=TYMASK_);
void test_compare(Tree const& actual, Tree const& expected,
                  const char *actual_name="actual", const char *expected_name="expected",
                  type_bits cmp_mask=TYMASK_);
void test_compare(Tree const& actual, id_type node_actual,
                  Tree const& expected, id_type node_expected,
                  id_type level=0, const char *actual_name="actual", const char *expected_name="expected",
                  type_bits cmp_mask=TYMASK_);

void test_arena_not_shared(Tree const& a, Tree const& b);

void test_invariants(NodeType ty);
void test_invariants(Tree const& t);
void test_invariants(ConstNodeRef const& n);


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

using fn_test_check = std::function<void(Tree const&)>;
using fn_test_check_with_parser = std::function<void(Tree const&,Parser const&)>;

void test_check_emit_check(Tree const& t, Parser &parser, fn_test_check check_fn);
void test_check_emit_check_with_parser(Tree const& t, Parser &parser, fn_test_check_with_parser check_fn);

void test_check_emit_check(csubstr yaml, ParserOptions opts, fn_test_check check_fn);
void test_check_emit_check_with_parser(csubstr yaml, ParserOptions opts, fn_test_check_with_parser check_fn);

void test_check_emit_check(Tree const& t, fn_test_check check_fn);
void test_check_emit_check_with_parser(Tree const& t, fn_test_check_with_parser check_fn);

void test_check_emit_check(csubstr yaml, fn_test_check check_fn);
void test_check_emit_check_with_parser(csubstr yaml, fn_test_check_with_parser check_fn);

} // namespace yml
} // namespace c4

C4_SUPPRESS_WARNING_POP

#endif /* TEST_LIB_TEST_TREE_HELPERS_HPP_ */
