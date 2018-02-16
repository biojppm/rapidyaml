#ifndef _C4_YML_STD_VECTOR_HPP_
#define _C4_YML_STD_VECTOR_HPP_

#include "../node.hpp"
#include <vector>

namespace std {

template< class V, class Alloc >
void write(c4::yml::NodeRef *n, std::vector< V, Alloc > const& vec)
{
    *n |= SEQ;
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

} // namespace std

#endif // _C4_YML_STD_VECTOR_HPP_
