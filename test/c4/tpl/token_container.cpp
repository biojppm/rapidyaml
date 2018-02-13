#include "./token_container.hpp"
#include "./token.hpp"

namespace c4 {
namespace tpl {

TokenContainer::~TokenContainer()
{
    for(TokenBase &tk : *this)
    {
        tk.~TokenBase();
    }
}

} // namespace tpl
} // namespace c4
