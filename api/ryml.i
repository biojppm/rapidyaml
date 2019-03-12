
%module ryml
%{
#define SWIG_FILE_WITH_INIT
#include "ryml.hpp"
void swig_test();
%}

extern void swig_test();
