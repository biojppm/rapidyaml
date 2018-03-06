#ifndef _C4_YML_STD_STRING_HPP_
#define _C4_YML_STD_STRING_HPP_

#include "../node.hpp"
#include <string>

namespace std {

//-----------------------------------------------------------------------------
// converting std::strings to yml::span / yml::cspan.
// This is not required, but is handy.

inline c4::yml::subs to_subs(std::string &s)
{
    char* data = s.empty() ? nullptr : &s[0];
    return c4::yml::subs(data, s.size());
}

inline c4::yml::csubs to_csubs(std::string const& s)
{
    const char* data = s.empty() ? nullptr : &s[0];
    return c4::yml::csubs(data, s.size());
}


//-----------------------------------------------------------------------------

// std::string is a value-like type, and it will be a leaf node
// in the YAML data tree hierarchy (a VAL node in ryml parlance).
// So it should be serialized via to_str()/from_str().

inline size_t to_str(c4::yml::subs buf, std::string const& s)
{
    return to_str(buf, to_csubs(s));
}

inline bool from_str(c4::yml::csubs buf, std::string * s)
{
    s->resize(buf.len);
    c4::yml::subs v = to_subs(*s);
    return from_str(buf, &v);
}

} // namespace std

#endif // _C4_YML_STD_STRING_HPP_
