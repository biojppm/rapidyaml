#ifndef _C4_YML_STD_STRING_HPP_
#define _C4_YML_STD_STRING_HPP_

#include "../node.hpp"
#include <string>

namespace std {

//-----------------------------------------------------------------------------
// converting std::strings to yml::span / yml::cspan.
// This is not required, but is handy.

c4::yml::span to_span(std::string &s)
{
    return c4::yml::span(&s[0], s.size());
}

c4::yml::cspan to_cspan(std::string const& s)
{
    return c4::yml::cspan(&s[0], s.size());
}


//-----------------------------------------------------------------------------

// std::string is a value-like type, and it will be a leaf node
// in the data tree hierarchy (a VAL node in ryml parlance).
// So it should be serialized via to_str()/from_str().

inline size_t to_str(c4::yml::span buf, std::string const& s)
{
    return to_str(buf, to_cspan(s));
}

inline size_t from_str(c4::yml::cspan buf, std::string * s)
{
    s->resize(buf.len);
    c4::yml::span v = to_span(*s);
    return from_str(buf, &v);
}

} // namespace std

#endif // _C4_YML_STD_STRING_HPP_
