#ifndef _C4_RYML_COMMON_HPP_
#define _C4_RYML_COMMON_HPP_

#define RYML_INLINE inline

//#define RYML_NO_DEFAULT_CALLBACKS
#ifndef RYML_NO_DEFAULT_CALLBACKS
#   include <stdlib.h>
#   include <stdio.h>
#endif // RYML_NO_DEFAULT_CALLBACKS

#ifndef C4_QUOTE
#   define C4_QUOTE(x) #x
#endif


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC system_header

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

#ifndef C4_ERROR
#   define C4_ERROR(msg) C4_ERROR_(__FILE__, __LINE__, msg)
#   define C4_ERROR_(file, line, msg) \
    c4::yml::RymlCallbacks::error(file ":" C4_QUOTE(line) ": fatal error: " msg "\n")
#endif

#ifndef C4_ASSERT
#   ifdef NDEBUG
#       define C4_ASSERT(expr) (void)(0)
#   else
#       include <assert.h>
#       define C4_ASSERT(expr) _C4_ASSERT((expr), __FILE__, __LINE__)
#       define _C4_ASSERT(expr, file, line)                             \
    {                                                                   \
        if( ! (expr))                                                   \
        {                                                               \
            c4::yml::RymlCallbacks::error(file ":" C4_QUOTE(line) ": Assert failed: " #expr "\n"); \
        }                                                               \
    }
#   endif
#endif

#pragma clang diagnostic pop
#pragma GCC diagnostic pop

namespace c4 {
namespace yml {

enum : size_t { npos = size_t(-1) };

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

struct LineCol
{
    size_t offset, line, col;
};

struct Location : public LineCol
{
    const char *name;
    operator bool () const { return name != nullptr || line != 0 || offset != 0; }

    Location() { memset(this, 0, sizeof(*this)); }
    Location(const char *n, size_t b, size_t l, size_t c) : LineCol{b, l, c}, name(n) {}
    Location(const char *n, yaml_mark_t m) : LineCol{m.index, m.line+1, m.column+1}, name(n) {}
};

struct Region
{
   const char *name;
   LineCol begin;
   LineCol end;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifndef RYML_NO_DEFAULT_CALLBACKS
struct RymlCallbacks
{
    static void* allocate(size_t length, void * /*hint*/)
    {
        void *mem = ::malloc(length);
        if(mem == nullptr)
        {
            const char msg[] = "could not allocate memory";
            error(msg, sizeof(msg));
        }
        return mem;
    }

    static void free(void *mem, size_t /*length*/)
    {
        ::free(mem);
    }

    static void error(const char* msg, size_t length, Location *loc1 = nullptr, Location *loc2 = nullptr)
    {
        fprintf(stderr, "%.*s\n", (int)length, msg);
        if(loc1 && *loc1)
        {
            fprintf(stderr, "    : %s at %zd:%zd (offset %zd)\n", loc1->name, loc1->line, loc1->col, loc1->offset);
        }
        if(loc2 && *loc2)
        {
            fprintf(stderr, "    : %s at %zd:%zd (offset %zd)\n", loc2->name, loc1->line, loc2->col, loc2->offset);
        }
        abort();
    }
    template< size_t N >
    static void error(char const (&msg)[N], Location *loc1 = nullptr, Location *loc2 = nullptr)
    {
        error(&msg[0], N-1, loc1, loc2);
    }
};
#endif // RYML_NO_DEFAULT_CALLBACKS

} // namespace yml
} // namespace c4

#endif /* _C4_RYML_COMMON_HPP_ */
