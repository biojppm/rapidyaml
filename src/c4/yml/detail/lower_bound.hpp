#ifndef C4_YML_DETAIL_LOWER_BOUND_HPP_
#define C4_YML_DETAIL_LOWER_BOUND_HPP_

#include <cstdint>

namespace c4 {
namespace yml {
namespace detail {

template<class T, class Needle=T>
T* lower_bound(T *first, T *last, Needle const& needle) noexcept
{
    intptr_t count = last - first;
    while(count > 0)
    {
        intptr_t step = count / 2;
        T *it = first + step;
        if(*it < needle)
        {
            first = ++it;
            count -= step + 1;
        }
        else
        {
            count = step;
        }
    }
    return first;
}

} // namespace detail
} // namespace yml
} // namespace c4

#endif /* C4_YML_DETAIL_LOWER_BOUND_HPP_ */
