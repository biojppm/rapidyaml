#ifndef _C4_YML_STD_STRING_HPP_
#define _C4_YML_STD_STRING_HPP_

#include "../node.hpp"
#include <string>

namespace std {

inline size_t to_str(c4::yml::span buf, std::string const& s)
{
    return to_str(buf, c4::yml::cspan(s.data(), s.size()));
}

inline size_t from_str(c4::yml::cspan buf, std::string * s)
{
    s->resize(buf.len);
    c4::yml::span v(&(*s)[0], buf.len);
    return from_str(buf, &v);
}

void write(c4::yml::NodeRef *n, std::string const& s)
{
    *n << c4::yml::cspan(s.data(), s.size());
}

bool read(c4::yml::NodeRef const& n, std::string *s)
{
    s->resize(n.val().len);
    c4::yml::span sp(&(*s)[0], s->size());
    n >> sp;
    return true;
}

//-----------------------------------------------------------------------------
c4::yml::span to_span(std::string &s)
{
    return c4::yml::span(&s[0], s.size());
}

c4::yml::cspan to_span(std::string const& s)
{
    return c4::yml::cspan(&s[0], s.size());
}

c4::yml::cspan to_cspan(std::string const& s)
{
    return c4::yml::cspan(&s[0], s.size());
}

} // namespace std

#endif // _C4_YML_STD_STRING_HPP_
