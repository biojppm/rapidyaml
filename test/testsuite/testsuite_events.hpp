#ifndef C4_YML_TEST_SUITE_EVENTS_HPP_
#define C4_YML_TEST_SUITE_EVENTS_HPP_

#ifdef RYML_SINGLE_HEADER
#include <ryml_all.hpp>
#else
#include <c4/yml/tree.hpp>
#endif
#include <string>

namespace c4 {
namespace yml {

void parse_events_to_tree(csubstr events, Tree *C4_RESTRICT tree);

size_t emit_events_from_tree(substr buf, Tree const& C4_RESTRICT tree);
std::string emit_events_from_source(substr src);

template<class CharContainer>
void emit_events_from_tree(CharContainer *container, Tree const& C4_RESTRICT tree)
{
    size_t ret = emit_events_from_tree(to_substr(*container), tree);
    if(ret > container->size())
    {
        container->resize(ret);
        ret = emit_events_from_tree(to_substr(*container), tree);
        C4_CHECK(ret == container->size());
    }
    container->resize(ret);
}

template<class CharContainer>
CharContainer emit_events_from_tree(Tree const& C4_RESTRICT tree)
{
    CharContainer result;
    emit_events_from_tree(&result, tree);
    return result;
}

} // namespace yml
} // namespace c4


#endif /* C4_YML_TEST_SUITE_EVENTS_HPP_ */
