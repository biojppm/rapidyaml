#include "c4/yml/node.hpp"
#include "c4/base64.hpp"
#include "c4/format.hpp"

namespace c4 {
namespace yml {

NodeRef& NodeRef::operator<< (c4::fmt::const_base64_wrapper w)
{
    return *this;
}

NodeRef const& NodeRef::operator>> (c4::fmt::base64_wrapper w) const
{
    return *this;
}

} // namespace yml
} // namespace c4
