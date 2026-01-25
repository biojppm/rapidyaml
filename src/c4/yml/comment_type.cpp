#ifdef RYML_WITH_COMMENTS

#include "c4/yml/comment_type.hpp"

namespace c4 {
namespace yml {

const char *comment_type_str(CommentType_e type)
{
    switch(type)
    {
    #define _c4comm(comm_symbol, bit) case COMM_##comm_symbol: return #comm_symbol;
    _RYML_DEFINE_COMMENTS(_c4comm)
    default: break;
    #undef _c4comm
    }
    return "";
}

} // namespace yml
} // namespace c4

#endif // RYML_WITH_COMMENTS
