#include "test_lib/test_case.hpp"
#include <map>


namespace c4 {
namespace yml {


CaseData* get_data(csubstr name)
{
    static std::map<csubstr, CaseData> m;
    auto it = m.find(name);
    if(it != m.end())
        return &it->second;
    CaseData *cd = &m[name];
    Case const* c = get_case(name);
    RYML_CHECK_BASIC_(c->src.find("\n\r") == csubstr::npos);
    txtbuf tmp;
    replace_all("\r", "", c->src, &tmp);
    cd->unix_style.assign(tmp);
    replace_all("\n", "\r\n", cd->unix_style.src, &tmp);
    cd->windows_style.assign(tmp);
    return cd;
}

} // namespace yml
} // namespace c4
