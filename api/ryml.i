
%module ryml

//-----------------------------------------------------------------------------
// this block will be pasted verbatim in the generated source file

%{
// specifies that the resulting C file should be built as a python
// extension, inserting the module init code
#define SWIG_FILE_WITH_INIT

#include "ryml.hpp"
namespace c4 {
namespace yml {
void swig_test(fdx f);
} // namespace yml
} // namespace c4
%}

//-----------------------------------------------------------------------------
namespace c4 {

template<class C>
struct basic_substring
{
    basic_substring(C *s, size_t sz);
    ~basic_substring();
};
%template(substr) basic_substring<char>;
%template(csubstr) basic_substring<const char>;

} // namespace c4

%pythoncode %{
def as_substr(obj):
    assert not isinstance(obj, str)
    return substr(obj, len(obj))
def as_csubstr(obj):
    return csubstr(obj, len(obj))
def parse_in_situ(buf):
    b = as_substr(buf)
    foo(b)
def parse(buf):
    b = as_csubstr(buf)
    cfoo(b)
%}

//-----------------------------------------------------------------------------
%typemap(in) (const char *str, int len) {
  $1 = PyString_AsString($input);
  $2 = PyString_Size($input);
};

%inline %{
void foo(c4::substr s)
{
    printf("INPLACE: s=%.*s\n", (int)s.len, s.str);
}
void cfoo(c4::csubstr s)
{
    printf("READONLY: s=%.*s\n", (int)s.len, s.str);
}

void call_swig_test(const char *s, int sz)
{
    c4::yml::swig_test(c4::yml::fdx{s, (size_t)sz});
}
%}

namespace c4 {
namespace yml {
extern void swig_test(c4::yml::fdx f);
} // namespace yml
} // namespace c4
