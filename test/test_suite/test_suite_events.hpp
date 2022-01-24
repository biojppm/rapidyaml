#ifndef C4_YML_TEST_SUITE_EVENTS_HPP_
#define C4_YML_TEST_SUITE_EVENTS_HPP_

#ifdef RYML_SINGLE_HEADER
#include <ryml_all.hpp>
#else
#include <c4/yml/tree.hpp>
#endif

namespace c4 {
namespace yml {

struct EventsParser
{
    void parse(csubstr src, Tree *C4_RESTRICT tree);
};

size_t emit_events(substr buf, Tree const& C4_RESTRICT tree);

template<class CharContainer>
void emit_events(CharContainer *container, Tree const& C4_RESTRICT tree)
{
    size_t ret = emit_events(to_substr(*container), tree);
    if(ret > container->size())
    {
        container->resize(ret);
        ret = emit_events(to_substr(*container), tree);
        C4_CHECK(ret == container->size());
    }
    container->resize(ret);
}

template<class CharContainer>
CharContainer emit_events(Tree const& C4_RESTRICT tree)
{
    CharContainer result;
    emit_events(&result, tree);
    return result;
}

} // namespace yml
} // namespace c4


#endif /* C4_YML_TEST_SUITE_EVENTS_HPP_ */
