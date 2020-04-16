#ifndef _C4_YML_EMIT_HPP_
#define _C4_YML_EMIT_HPP_


#ifndef _C4_YML_COMMON_HPP_
#include "./common.hpp"
#endif
#include <c4/substr.hpp>


namespace c4 {
namespace yml {



//-----------------------------------------------------------------------------

/** convert flow-type relaxed maps into strict YAML flow map
 */
size_t preproc_relaxed_map(csubstr s, substr buf);

template<class CharContainer>
substr preproc_relaxed_map(csubstr s, CharContainer *out)
{
    size_t sz = preproc_relaxed_map(s, to_substr(*out));
    if(sz > out->size())
    {
        out->resize(sz);
        sz = preproc_relaxed_map(s, to_substr(*out));
    }
    return to_substr(*out).first(sz);
}

template<class CharContainer>
CharContainer preproc_relaxed_map(csubstr s)
{
    CharContainer c;
    preproc_relaxed_map(s, &c);
    return c;
}


} // namespace yml
} // namespace c4

#endif /* _C4_YML_COMMON_HPP_ */
