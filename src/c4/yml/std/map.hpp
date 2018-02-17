#ifndef _C4_YML_STD_MAP_HPP_
#define _C4_YML_STD_MAP_HPP_

#include "../node.hpp"
#include <map>

namespace std {

template< class K, class V, class Less, class Alloc >
void write(c4::yml::NodeRef *n, std::map< K, V, Less, Alloc > const& m)
{
    *n |= c4::yml::MAP;
    for(auto const& p : m)
    {
        auto ch = n->append_child();
        ch << c4::yml::key(p.first);
        ch << p.second;
    }
}

template< class K, class V, class Less, class Alloc >
bool read(c4::yml::NodeRef const& n, std::map< K, V, Less, Alloc > * m)
{
    K k{};
    V v;
    for(auto const& ch : n)
    {
        ch >> c4::yml::key(k);
        ch >> v;
        m->emplace(make_pair(move(k), move(v)));
    }
    return true;
}

} // namespace std

#endif // _C4_YML_STD_MAP_HPP_
