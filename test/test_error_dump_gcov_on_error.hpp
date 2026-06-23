#pragma once
#ifndef C4_YML_TEST_ERROR_DUMP_GCOV_ON_ERROR_HPP_
#define C4_YML_TEST_ERROR_DUMP_GCOV_ON_ERROR_HPP_

#ifdef RYML_SINGLE_HEADER
#include <ryml_all.hpp>
#else
#include "c4/yml/parse.hpp"
#include "c4/yml/tree.hpp"
#endif


#if !(defined(RYML_COVERAGE) && defined(C4_UNIX))
#define RYML_DUMP_GCOV_ON_ERROR_()
#else
#define RYML_DUMP_GCOV_ON_ERROR_() std::signal(SIGABRT, dump_gcov_on_abort)
#include <csignal>
#include <cstdlib>
extern "C" void __gcov_dump(); // prior to g++11, was named __gcov_flush()
void dump_gcov_on_abort(int signal)
{
    if(signal == SIGABRT)
        __gcov_dump();
    std::exit(1);
}
#endif

#ifdef _RYML_WITH_EXCEPTIONS
#define RYML_DUMP_GCOV_ON_ERROR(...)            \
    do {                                        \
        try { __VA_ARGS__ }                     \
        catch(...) { __gcov_dump(); }           \
    } while(0)
#else
#define RYML_DUMP_GCOV_ON_ERROR(...)            \
    do {                                        \
        RYML_DUMP_GCOV_ON_ERROR_();             \
        __VA_ARGS__                             \
    } while(0)
#endif

#endif // C4_YML_TEST_ERROR_DUMP_GCOV_ON_ERROR_HPP_
