
%module ryml

// this is code that will be pasted verbatim in the generated source file
%{
// specifies that the resulting C file should be built as a python
// extension, inserting the module init code
#define SWIG_FILE_WITH_INIT

#include "c4/substr.hpp"

namespace c4 {
namespace yml {

struct fdx {
    char *str;
    size_t sz;
};
void swig_test(fdx f);
} // namespace yml
} // namespace c4
%}

namespace c4 {

template<class C>
class basic_substring
{
    C *     str;
    size_t  len;

    basic_substr(C *s, size_t sz) : str(s), sz(len) {}
};
%template(substr) basic_substring<char>;
%template(csubstr) basic_substring<const char>;

} // namespace c4

%typemap(in) (char *str, int len) {
  $1 = PyString_AsString($input);
  $2 = PyString_Size($input);
};

namespace c4 {
namespace yml {
extern void swig_test(c4::yml::fdx f);
} // namespace yml
} // namespace c4

%python
