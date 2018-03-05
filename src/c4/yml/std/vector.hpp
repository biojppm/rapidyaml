#ifndef _C4_YML_STD_VECTOR_HPP_
#define _C4_YML_STD_VECTOR_HPP_

#include "../node.hpp"
#include <vector>

namespace std {

// std::string is a sequence-like type, and it requires child nodes
// in the data tree hierarchy (a SEQ node in ryml parlance).
// So it should be serialized via write()/read().

template< class V, class Alloc >
void write(c4::yml::NodeRef *n, std::vector< V, Alloc > const& vec)
{
    *n |= c4::yml::SEQ;
    for(auto const& v : vec)
    {
        n->append_child() << v;
    }
}

template< class V, class Alloc >
bool read(c4::yml::NodeRef const& n, std::vector< V, Alloc > *vec)
{
    vec->resize(n.num_children());
    size_t pos = 0;
    for(auto const& ch : n)
    {
        ch >> (*vec)[pos++];
    }
    return true;
}


//-----------------------------------------------------------------------------
template< class Alloc >
c4::yml::span to_span(std::vector< char, Alloc > &vec)
{
    char *data = vec.empty() ? nullptr : vec.data(); // data() may or may not return a null pointer.
    return c4::yml::span(data, vec.size());
}

template< class Alloc >
c4::yml::cspan to_span(std::vector< char, Alloc > const& vec)
{
    const char *data = vec.empty() ? nullptr : vec.data(); // data() may or may not return a null pointer.
    return c4::yml::cspan(data, vec.size());
}

template< class Alloc >
c4::yml::cspan to_span(std::vector< const char, Alloc > const& vec)
{
    const char *data = vec.empty() ? nullptr : vec.data(); // data() may or may not return a null pointer.
    return c4::yml::cspan(data, vec.size());
}

template< class Alloc >
c4::yml::cspan to_cspan(std::vector< char, Alloc > const& vec)
{
    const char *data = vec.empty() ? nullptr : vec.data(); // data() may or may not return a null pointer.
    return c4::yml::cspan(data, vec.size());
}

template< class Alloc >
c4::yml::cspan to_cspan(std::vector< const char, Alloc > const& vec)
{
    const char *data = vec.empty() ? nullptr : vec.data(); // data() may or may not return a null pointer.
    return c4::yml::cspan(data, vec.size());
}

} // namespace std

#endif // _C4_YML_STD_VECTOR_HPP_
