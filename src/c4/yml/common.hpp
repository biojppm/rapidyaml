#ifndef _C4_YML_COMMON_HPP_
#define _C4_YML_COMMON_HPP_

/** @file common.hpp Common utilities and infrastructure used by ryml. */

#include <cstddef>
#include <c4/substr.hpp>
#include <c4/charconv.hpp>
#include <c4/yml/export.hpp>

/// @cond dev
#if (defined(C4_EXCEPTIONS) && (!defined(RYML_NO_DEFAULT_CALLBACKS) && defined(RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS))) || defined(__DOXYGEN__)
#define _RYML_WITH_EXCEPTIONS
#endif
/// @endcond

#ifdef _RYML_WITH_EXCEPTIONS
#include <stdexcept>
#endif


//-----------------------------------------------------------------------------

#ifndef RYML_ERRMSG_SIZE
/// size for the error message buffer
#define RYML_ERRMSG_SIZE (1024)
#endif


#ifndef RYML_LOGBUF_SIZE
/// size for the buffer used to format individual values to string
/// while preparing an error message. This is only used for formatting
/// individual values in the message; final messages will be larger
/// than this value (see @ref RYML_ERRMSG_SIZE). This size is also
/// used for the detailed debug log messages when RYML_DBG is defined.
#define RYML_LOGBUF_SIZE (256)
#endif
static_assert(RYML_LOGBUF_SIZE < RYML_ERRMSG_SIZE, "invalid size");


#ifndef RYML_LOCATIONS_SMALL_THRESHOLD
/// threshold at which a location search will revert from linear to
/// binary search.
#define RYML_LOCATIONS_SMALL_THRESHOLD (30)
#endif


//-----------------------------------------------------------------------------
// Specify groups to have a predefined topic order in doxygen:

/** @defgroup doc_quickstart Quickstart
 *
 * Example code for every feature.
 */

/** @defgroup doc_parse Parse utilities
 * @see sample::sample_parse_in_place
 * @see sample::sample_parse_in_arena
 * @see sample::sample_parse_file
 * @see sample::sample_parse_reuse_tree
 * @see sample::sample_parse_reuse_parser
 * @see sample::sample_parse_reuse_tree_and_parser
 * @see sample::sample_location_tracking
 */

/** @defgroup doc_emit Emit utilities
 *
 * Utilities to emit YAML and JSON, either to a memory buffer or to a
 * file or ostream-like class.
 *
 * @see sample::sample_emit_to_container
 * @see sample::sample_emit_to_stream
 * @see sample::sample_emit_to_file
 * @see sample::sample_emit_nested_node
 * @see sample::sample_emit_style
 */

/** @defgroup doc_node_type Node types
 */

/** @defgroup doc_tree Tree utilities
 * @see sample::sample_quick_overview
 * @see sample::sample_iterate_trees
 * @see sample::sample_create_trees
 * @see sample::sample_tree_arena
 *
 * @see sample::sample_static_trees
 * @see sample::sample_location_tracking
 *
 * @see sample::sample_docs
 * @see sample::sample_anchors_and_aliases
 * @see sample::sample_tags
 */

/** @defgroup doc_node_classes Node classes
 *
 * High-level node classes.
 *
 * @see sample::sample_quick_overview
 * @see sample::sample_iterate_trees
 * @see sample::sample_create_trees
 * @see sample::sample_tree_arena
 */

/** @defgroup doc_error_handling Error handling
 *
 * Utilities to report handle errors, and to build and report error
 * messages.
 *
 * @see sample::sample_error_handler
 */

/** @defgroup doc_callbacks Callbacks for errors and allocation
 *
 * Functions called by ryml to allocate/free memory and to report
 * errors.
 *
 * @see sample::sample_error_handler
 * @see sample::sample_global_allocator
 * @see sample::sample_per_tree_allocator
 */

/** @defgroup doc_serialization Serialization/deserialization
 *
 * Contains information on how to serialize and deserialize
 * fundamental types, user scalar types, user container types and
 * interop with std scalar/container types.
 *
 */

/** @defgroup doc_ref_utils Anchor/Reference utilities
 *
 * @see sample::sample_anchors_and_aliases
 * */

/** @defgroup doc_tag_utils Tag utilities
 * @see sample::sample_tags
 */

/** @defgroup doc_preprocessors Preprocessors
 *
 * Functions for preprocessing YAML prior to parsing.
 */


//-----------------------------------------------------------------------------

// document macros for doxygen
#ifdef __DOXYGEN__ // defined in Doxyfile::PREDEFINED

/** define this macro with a boolean value to enable/disable
 * assertions to check preconditions and assumptions throughout the
 * codebase; this causes a slowdown of the code, and larger code
 * size. By default, this macro is defined unless NDEBUG is defined
 * (see C4_USE_ASSERT); as a result, by default this macro is truthy
 * only in debug builds. */
#   define RYML_USE_ASSERT

/** (Undefined by default) Define this macro to disable ryml's default
 * implementation of the callback functions. See @ref doc_callbacks.  */
#   define RYML_NO_DEFAULT_CALLBACKS

/** (Undefined by default) When this macro is defined (and
 * @ref RYML_NO_DEFAULT_CALLBACKS is not defined), the default error
 * handler will throw exceptions. See @ref doc_error_handling. */
#   define RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS

/** Conditionally expands to `noexcept` when @ref RYML_USE_ASSERT is 0 and
 * is empty otherwise. The user is unable to override this macro. */
#   define RYML_NOEXCEPT

#endif


//-----------------------------------------------------------------------------

/** @cond dev*/

#ifndef RYML_USE_ASSERT
#   define RYML_USE_ASSERT C4_USE_ASSERT
#endif

#if RYML_USE_ASSERT
#   define RYML_NOEXCEPT
#else
#   define RYML_NOEXCEPT noexcept
#endif

#define RYML_DEPRECATED(msg) C4_DEPRECATED(msg)

#if defined(RYML_DBG) && !defined(NDEBUG) && !defined(C4_NO_DEBUG_BREAK)
#   define RYML_DEBUG_BREAK()                               \
    do {                                                    \
        if(c4::get_error_flags() & c4::ON_ERROR_DEBUGBREAK) \
        {                                                   \
            C4_DEBUG_BREAK();                               \
        }                                                   \
    } while(false)
#else
#   define RYML_DEBUG_BREAK()
#endif

/** @endcond */


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

namespace c4 {
namespace yml {

C4_SUPPRESS_WARNING_GCC_CLANG_WITH_PUSH("-Wold-style-cast")

class Tree;

#ifndef RYML_ID_TYPE
/** The type of a node id in the YAML tree. In the future, the default
 * will likely change to int32_t, which was observed to be faster.
 * @see id_type */
#define RYML_ID_TYPE size_t
#endif


/** The type of a node id in the YAML tree; to override the default
 * type, define the macro @ref RYML_ID_TYPE to a suitable integer
 * type. */
using id_type = RYML_ID_TYPE;
static_assert(std::is_integral<id_type>::value, "id_type must be an integer type");


C4_SUPPRESS_WARNING_GCC_WITH_PUSH("-Wuseless-cast")
enum : id_type {
    /** an index to none */
    NONE = id_type(-1),
};
C4_SUPPRESS_WARNING_GCC_CLANG_POP


enum : size_t {
    /** a null string position */
    npos = size_t(-1)
};


typedef enum BlockStyle_ {
    BLOCK_LITERAL, //!< keep newlines (|)
    BLOCK_FOLD     //!< replace newline with single space (>)
} BlockStyle_e;


typedef enum BlockChomp_ {
    CHOMP_CLIP,    //!< single newline at end (default)
    CHOMP_STRIP,   //!< no newline at end     (-)
    CHOMP_KEEP     //!< all newlines from end (+)
} BlockChomp_e;


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/** holds a position into a source buffer
 * @ingroup doc_error_handling */
struct RYML_EXPORT LineCol
{
    //! number of bytes from the beginning of the source buffer
    size_t offset;
    //! line
    size_t line;
    //! column
    size_t col;

    LineCol() noexcept = default;
    //! construct from line
    LineCol(size_t l) noexcept : offset(0), line(l), col(0) {}
    //! construct from line and column
    LineCol(size_t l, size_t c) noexcept : offset(0), line(l), col(c) {}
    //! construct from offset, line and column
    LineCol(size_t o, size_t l, size_t c) noexcept : offset(o), line(l), col(c) {}
};
static_assert(std::is_trivial<LineCol>::value, "LineCol not trivial");
static_assert(std::is_standard_layout<LineCol>::value, "Location not trivial");


/** holds a source or yaml file position.
 * @ingroup doc_error_handling */
struct RYML_EXPORT Location
{
    //! number of bytes from the beginning of the source buffer
    size_t offset;
    //! line
    size_t line;
    //! column
    size_t col;
    //! file name
    csubstr name;

    operator bool () const noexcept { return !name.empty() || line != 0 || offset != 0 || col != 0; }
    operator LineCol const& () const noexcept { return reinterpret_cast<LineCol const&>(*this); } // NOLINT

    C4_NO_INLINE Location() noexcept = default;
    C4_NO_INLINE Location(                         size_t l          ) noexcept : offset( ), line(l), col( ), name( ) {}
    C4_NO_INLINE Location(                         size_t l, size_t c) noexcept : offset( ), line(l), col(c), name( ) {}
    C4_NO_INLINE Location(               size_t b, size_t l, size_t c) noexcept : offset(b), line(l), col(c), name( ) {}
    C4_NO_INLINE Location(    csubstr n,           size_t l          ) noexcept : offset( ), line(l), col( ), name(n) {}
    C4_NO_INLINE Location(    csubstr n,           size_t l, size_t c) noexcept : offset( ), line(l), col(c), name(n) {}
    C4_NO_INLINE Location(    csubstr n, size_t b, size_t l, size_t c) noexcept : offset(b), line(l), col(c), name(n) {}
    C4_NO_INLINE Location(const char *n,           size_t l          ) noexcept : offset( ), line(l), col( ), name(to_csubstr(n)) {}
    C4_NO_INLINE Location(const char *n,           size_t l, size_t c) noexcept : offset( ), line(l), col(c), name(to_csubstr(n)) {}
    C4_NO_INLINE Location(const char *n, size_t b, size_t l, size_t c) noexcept : offset(b), line(l), col(c), name(to_csubstr(n)) {}
};
static_assert(std::is_standard_layout<Location>::value, "Location not trivial");
static_assert(offsetof(Location, offset) == offsetof(LineCol, offset), "inconsistent offset for offset");
static_assert(offsetof(Location, line) == offsetof(LineCol, line), "inconsistent offset for line");
static_assert(offsetof(Location, col) == offsetof(LineCol, col), "inconsistent offset for col");

/// @cond dev
#define RYML_LOC_HERE() (::c4::yml::Location(__FILE__, static_cast<size_t>(__LINE__)))
/// @endcond


//-----------------------------------------------------------------------------

/** @addtogroup doc_callbacks
 *
 * @{ */

struct Callbacks;


/** set the global callbacks for the library; after a call to this
 * function, these callbacks will be used by newly created objects
 * (unless they are copying older objects with different
 * callbacks). If @ref RYML_NO_DEFAULT_CALLBACKS is defined, it is
 * mandatory to call this function prior to using any other library
 * facility.
 *
 * @warning This function is NOT thread-safe, because it sets global static data
 * */
RYML_EXPORT void set_callbacks(Callbacks const& c);

/** get the global callbacks
 *
 * @warning This function is NOT thread-safe vis-a-vis concurrent calls to @ref set_callbacks()
 * */
RYML_EXPORT Callbacks const& get_callbacks();

/** set the global callbacks back to their defaults.
 *
 * @warning This function is NOT thread-safe, because it sets global static data
 * */
RYML_EXPORT void reset_callbacks();


/** the type of the function used to allocate memory; ryml will only
 * allocate memory through this callback. */
using pfn_allocate = void* (*)(size_t len, void* hint, void *user_data);


/** the type of the function used to free memory; ryml will only free
 * memory through this callback. */
using pfn_free = void (*)(void* mem, size_t size, void *user_data);


/** the type of the function used to report basic errors.
 *
 * @warning Must not return. When implemented by the user, this
 * function MUST interrupt execution (and ideally be marked with
 * `[[noreturn]]`). If the function returned, the caller could enter
 * into an infinite loop, or the program may crash. It is up to the
 * user to choose the interruption mechanism; typically by either
 * throwing an exception, or using `std::longjmp()` ([see
 * documentation](https://en.cppreference.com/w/cpp/utility/program/setjmp))
 * or ultimately by calling `std::abort()`. */
using pfn_error_basic = void (*) (const char* msg, size_t msg_len, Location const& cpp_location, void *user_data);
/** the type of the function used to report parse errors.
 *
 * @warning Must not return. When implemented by the user, this
 * function MUST interrupt execution (and ideally be marked with
 * `[[noreturn]]`). If the function returned, the caller could enter
 * into an infinite loop, or the program may crash. It is up to the
 * user to choose the interruption mechanism; typically by either
 * throwing an exception, or using `std::longjmp()` ([see
 * documentation](https://en.cppreference.com/w/cpp/utility/program/setjmp))
 * or ultimately by calling `std::abort()`. */
using pfn_error_parse = void (*) (const char* msg, size_t msg_len, Location const& cpp_location, Location const& yml_location, void *user_data);
/** the type of the function used to report visit errors.
 *
 * @warning Must not return. When implemented by the user, this
 * function MUST interrupt execution (and ideally be marked with
 * `[[noreturn]]`). If the function returned, the caller could enter
 * into an infinite loop, or the program may crash. It is up to the
 * user to choose the interruption mechanism; typically by either
 * throwing an exception, or using `std::longjmp()` ([see
 * documentation](https://en.cppreference.com/w/cpp/utility/program/setjmp))
 * or ultimately by calling `std::abort()`. */
using pfn_error_visit = void (*) (const char* msg, size_t msg_len, Location const& cpp_location, Tree const* tree, id_type node, void *user_data);

/// @cond dev
using pfn_error RYML_DEPRECATED("use a more specific error type: `basic`, `parse` or `visit`") = pfn_error_parse;
/// @endcond


/** a c-style callbacks class. Can be used globally by the library
 * and/or locally by @ref Tree and @ref Parser objects. */
struct RYML_EXPORT Callbacks
{
    void *          m_user_data;   ///< data to be forwarded in every call to a callback
    pfn_allocate    m_allocate;    ///< a pointer to an allocate handler function
    pfn_free        m_free;        ///< a pointer to a free handler function
    pfn_error_basic m_error_basic; ///< a pointer to a basic error handler function
    pfn_error_parse m_error_parse; ///< a pointer to a parse error handler function
    pfn_error_visit m_error_visit; ///< a pointer to a visit error handler function

public:

    /** Construct an object with the default callbacks. If
     * @ref RYML_NO_DEFAULT_CALLBACKS is defined, the object will be set with null
     * members.*/
    Callbacks() noexcept;

    RYML_DEPRECATED("use the default constructor, followed by the appropriate setters")
    Callbacks(void *user_data, pfn_allocate alloc, pfn_free free, pfn_error_basic error_basic);

public:

    /** Set the user data. */
    Callbacks& set_user_data(void* user_data);

    /** Set or reset the allocate callback. When the parameter is
     * null, m_allocate will fall back to ryml's default allocate
     * implementation, unless @ref RYML_NO_DEFAULT_CALLBACKS is
     * defined.*/
    Callbacks& set_allocate(pfn_allocate allocate=nullptr);

    /** Set or reset the free callback. When the parameter is null,
     * m_free will fall back to ryml's default free implementation,
     * unless @ref RYML_NO_DEFAULT_CALLBACKS is defined.*/
    Callbacks& set_free(pfn_free free=nullptr);

    /** Set or reset the error_basic callback. When the parameter is null,
     * m_error_basic will fall back to ryml's default error_basic implementation,
     * unless @ref RYML_NO_DEFAULT_CALLBACKS is defined.*/
    Callbacks& set_error_basic(pfn_error_basic error_basic=nullptr);

    /** Set or reset the error_parse callback. When the parameter is null,
     * m_error_parse will fall back to ryml's default error_parse implementation,
     * unless @ref RYML_NO_DEFAULT_CALLBACKS is defined.*/
    Callbacks& set_error_parse(pfn_error_parse error_parse=nullptr);

    /** Set or reset the error_visit callback. When the parameter is null,
     * m_error_visit will fall back to ryml's default error_visit implementation,
     * unless @ref RYML_NO_DEFAULT_CALLBACKS is defined.*/
    Callbacks& set_error_visit(pfn_error_visit error_visit=nullptr);

public:

    bool operator!= (Callbacks const& that) const { return !operator==(that); }
    bool operator== (Callbacks const& that) const
    {
        return (m_user_data == that.m_user_data &&
                m_allocate == that.m_allocate &&
                m_free == that.m_free &&
                m_error_basic == that.m_error_basic &&
                m_error_parse == that.m_error_parse &&
                m_error_visit == that.m_error_visit);
    }
};


/** @} */


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/// @cond dev

namespace detail {
struct _SubstrWriter
{
    substr buf;
    size_t pos;
    _SubstrWriter(substr buf_, size_t pos_=0) : buf(buf_), pos(pos_) { C4_ASSERT(buf.str); }
    void append(csubstr s)
    {
        C4_ASSERT(!s.overlaps(buf));
        C4_ASSERT(s.str || !s.len);
        if(s.len && pos + s.len <= buf.len)
        {
            C4_ASSERT(s.str);
            memcpy(buf.str + pos, s.str, s.len);
        }
        pos += s.len;
    }
    void append(char c)
    {
        C4_ASSERT(buf.str);
        if(pos < buf.len)
            buf.str[pos] = c;
        ++pos;
    }
    void append_n(char c, size_t numtimes)
    {
        C4_ASSERT(buf.str);
        if(numtimes && pos + numtimes < buf.len)
            memset(buf.str + pos, c, numtimes);
        pos += numtimes;
    }
    size_t slack() const { return pos <= buf.len ? buf.len - pos : 0; }
    size_t excess() const { return pos > buf.len ? pos - buf.len : 0; }
    //! get the part written so far
    csubstr curr() const { return pos <= buf.len ? buf.first(pos) : buf; }
    //! get the part that is still free to write to (the remainder)
    substr rem() const { return pos < buf.len ? buf.sub(pos) : buf.last(0); }

    size_t advance(size_t more) { pos += more; return pos; }
};


// truncate the result to the buffer, adding ellipsis if it
// doesn't fit
inline C4_NO_INLINE csubstr _maybe_add_ellipsis(substr buf, size_t len)
{
    if(C4_UNLIKELY(len > buf.len))
    {
        const size_t numdots = buf.len > 3 ? 3 : buf.len;
        buf.last(numdots).fill('.');
        len = buf.len;
    }
    return buf.first(len);
}


template<class T> struct dump_directly_ : public std::false_type {};
template<> struct dump_directly_<csubstr> : public std::true_type {};
template<> struct dump_directly_< substr> : public std::true_type {};
template<> struct dump_directly_<const char*> : public std::true_type {};
template<> struct dump_directly_<      char*> : public std::true_type {};
template<size_t N> struct dump_directly_<const char (&)[N]> : public std::true_type {};
template<size_t N> struct dump_directly_<      char (&)[N]> : public std::true_type {};
template<size_t N> struct dump_directly_<const char[N]> : public std::true_type {};
template<size_t N> struct dump_directly_<      char[N]> : public std::true_type {};
template<class T> using dump_directly = dump_directly_<typename std::remove_cv<typename std::remove_reference<T>::type>::type>;

template<class T>
C4_NO_INLINE auto _to_chars_limited(substr buf, T &&var)
    -> typename std::enable_if< ! detail::dump_directly<T>::value, csubstr>::type
{
    size_t len = to_chars(buf, std::forward<T>(var));
    return _maybe_add_ellipsis(buf, len);
}
template<class T>
C4_NO_INLINE auto _to_chars_limited(substr, T &&var)
    -> typename std::enable_if<detail::dump_directly<T>::value, csubstr>::type
{
    return to_csubstr(std::forward<T>(var)); // no need to convert to buf
}


// dumpfn is a function abstracting prints to terminal (or to string).
template<class DumpFn>
C4_NO_INLINE void _dump(DumpFn &&dumpfn, substr, csubstr fmt)
{
    std::forward<DumpFn>(dumpfn)(fmt);
}
template<class DumpFn, class Arg, class ...Args>
C4_NO_INLINE void _dump(DumpFn &&dumpfn, substr argbuf, csubstr fmt, Arg const& arg, Args const& ...more)
{
    size_t pos = fmt.find("{}");
    if(pos == csubstr::npos)
        return std::forward<DumpFn>(dumpfn)(fmt);
    std::forward<DumpFn>(dumpfn)(fmt.first(pos));
    std::forward<DumpFn>(dumpfn)(_to_chars_limited(argbuf, arg));
    _dump(std::forward<DumpFn>(dumpfn), argbuf, fmt.sub(pos + 2), more...);
}


template<class ...Args>
C4_NO_INLINE csubstr _mk_err_msg(substr buf, csubstr fmt, Args const& ...args)
{
    detail::_SubstrWriter writer(buf);
    auto dumpfn = [&writer](csubstr s){ writer.append(s); };
    char writebuf[RYML_LOGBUF_SIZE];
    _dump(dumpfn, writebuf, fmt, args...);
    return _maybe_add_ellipsis(buf, writer.pos);
}

} // namespace detail

/// @endcond


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/** @addtogroup doc_error_handling
 *
 * @{ */


/** print a location to a file. To format a location to a message
 * string, or to dump it to different sinks, see @ref
 * location_msg() */
RYML_EXPORT void location_print(Location const& loc, FILE *f=nullptr);
/** format a location to a message, where @p dumpfn is a
 * function taking a csubstr and abstracting a string concatenation
 * operation, such as appending to a std::string or printing to
 * terminal. For example:
 *
 * ```c++
 * /// to output to cerr:
 * location_msg(loc, [&s](csubstr s){
 *     std::cerr.write(s.str, s.len);
 * });
 *
 * /// to build a string:
 * std::string msg;
 * location_msg(loc, [&s](csubstr s){
 *     msg.append(s.str, s.len);
 * });
 * ```
 */
template<class DumpFn>
C4_NO_INLINE void location_msg(Location const& loc, DumpFn &&dumpfn)
{
    if(!loc)
        return;
    if(!loc.name.empty())
    {
        std::forward<DumpFn>(dumpfn)(loc.name);
        std::forward<DumpFn>(dumpfn)(":");
    }
    char buf_[32];
    substr buf(buf_);
    std::forward<DumpFn>(dumpfn)(detail::_to_chars_limited(buf, loc.line));
    std::forward<DumpFn>(dumpfn)(":");
    if(loc.col)
    {
        std::forward<DumpFn>(dumpfn)(detail::_to_chars_limited(buf, loc.col));
        std::forward<DumpFn>(dumpfn)(":");
    }
    if(loc.offset)
    {
        std::forward<DumpFn>(dumpfn)(" (");
        std::forward<DumpFn>(dumpfn)(detail::_to_chars_limited(buf, loc.offset));
        std::forward<DumpFn>(dumpfn)("B)");
    }
}



/** print a formatted output message reporting a basic error to the given output file */
RYML_EXPORT void err_basic_print(const char* msg, size_t length, Location const& cpploc, FILE *f=nullptr);
/** format a message reporting a basic error, where @p dumpfn is a
 * function taking a csubstr and abstracting a string concatenation
 * operation, such as appending to a std::string or printing to
 * terminal. For example:
 *
 * ```c++
 * /// to output to cerr:
 * err_basic_msg(errmsg, errlen, cpploc, [](csubstr s){
 *     std::cerr.write(s.str, s.len);
 * });
 *
 * /// to build a string:
 * std::string msg;
 * err_basic_msg(errmsg, errlen, cpploc, [&msg](csubstr s){
 *     msg.append(s.str, s.len);
 * });
 * ```
 */
template<class DumpFn>
C4_NO_INLINE void err_basic_fmt(const char* msg, size_t length, Location const& cpploc, DumpFn &&dumpfn)
{
    location_msg(cpploc, dumpfn);
    std::forward<DumpFn>(dumpfn)(" ERROR: [basic] ");
    std::forward<DumpFn>(dumpfn)(csubstr{msg, length});
}
/** report a basic error to its respective handler, with a non-formatted error message. */
C4_NORETURN C4_NO_INLINE void err_basic(Callbacks const& callbacks, Location const& cpploc, const char* msg_);
/** report a basic error to its respective handler, with a non-formatted error message. Like (1), but use the current global callbacks. */
C4_NORETURN C4_NO_INLINE void err_basic(Location const& cpploc, const char* msg);
/** report a basic error to its respective handler, with a formatted error message. */
template<class ...Args>
C4_NORETURN C4_NO_INLINE void err_basic_msg(Callbacks const& callbacks, Location const& cpploc, const char *fmt, Args const& ...args)
{
    char errbuf[RYML_ERRMSG_SIZE];
    csubstr msg = detail::_mk_err_msg(errbuf, to_csubstr(fmt), args...);
    callbacks.m_error_basic(msg.str, msg.len, cpploc, callbacks.m_user_data);
    abort(); // the call above should not return, so force it here in case it does // LCOV_EXCL_LINE
    C4_UNREACHABLE_AFTER_ERR();
}
/** report a basic error to its respective handler, with a formatted error message. Like (1), but use the current global callbacks. */
template<class ...Args>
C4_NORETURN C4_NO_INLINE void err_basic_msg(Location const& cpploc, const char *fmt, Args const& ...args)
{
    err_basic_msg(get_callbacks(), cpploc, fmt, args...);
    C4_UNREACHABLE_AFTER_ERR();
}


/** print a formatted output message reporting a parse error to the given output file */
RYML_EXPORT void err_parse_print(const char* msg, size_t length, Location const& cpploc, Location const& yml_location, FILE *f=nullptr);
/** format a message reporting a parse error, where @p dumpfn is a
 * function taking a csubstr and abstracting a string concatenation
 * operation, such as appending to a std::string or printing to
 * terminal. For example:
 *
 * ```c++
 * /// to output to cerr:
 * err_parse_msg(errmsg, errlen, cpploc, ymlloc, [](csubstr s){
 *     std::cerr.write(s.str, s.len);
 * });
 *
 * /// to build a string:
 * std::string msg;
 * err_parse_msg(errmsg, errlen, cpploc, ymlloc, [&msg](csubstr s){
 *     msg.append(s.str, s.len);
 * });
 * ```
 */
template<class DumpFn>
C4_NO_INLINE void err_parse_fmt(const char* msg, size_t length, Location const& cpploc, Location const& ymlloc, DumpFn &&dumpfn)
{
    location_msg(ymlloc, dumpfn);
    std::forward<DumpFn>(dumpfn)(" ERROR: [parse] ");
    std::forward<DumpFn>(dumpfn)(csubstr{msg, length});
    std::forward<DumpFn>(dumpfn)("\n");
    location_msg(cpploc, dumpfn);
    std::forward<DumpFn>(dumpfn)(" (detected here)");
}
/** report a parse error to its respective handler, with a non-formatted error message */
C4_NORETURN C4_NO_INLINE void err_parse(Callbacks const& callbacks, Location const& cpploc, Location const& ymlloc, const char *msg_);
/** report a parse error to its respective handler, with a non-formatted error message. Like (1), but use the current global callbacks. */
C4_NORETURN C4_NO_INLINE void err_parse(Location const& cpploc, Location const& ymlloc, const char *msg);
/** report a parse error to its respective handler, with a formatted error message */
template<class ...Args>
C4_NORETURN C4_NO_INLINE void err_parse_msg(Callbacks const& callbacks, Location const& cpploc, Location const& ymlloc, const char *fmt, Args const& ...args)
{
    char errbuf[RYML_ERRMSG_SIZE];
    csubstr msg = detail::_mk_err_msg(errbuf, to_csubstr(fmt), args...);
    if(callbacks.m_error_parse)
        callbacks.m_error_parse(msg.str, msg.len, cpploc, ymlloc, callbacks.m_user_data);
    // fall to basic error if there is no parse handler set, but use ymlloc instead of cpploc
    else if(callbacks.m_error_basic)
        callbacks.m_error_basic(msg.str, msg.len, ymlloc, callbacks.m_user_data);
    abort(); // the call above should not return, so force it here in case it does // LCOV_EXCL_LINE
    C4_UNREACHABLE_AFTER_ERR();
}
/** report a parse error to its respective handler, with a formatted error message. Like (1), but use the current global callbacks. */
template<class ...Args>
C4_NORETURN C4_NO_INLINE void err_parse_msg(Location const& cpploc, Location const& ymlloc, const char *fmt, Args const& ...args)
{
    err_parse_msg(get_callbacks(), cpploc, ymlloc, fmt, args...);
    C4_UNREACHABLE_AFTER_ERR();
}


/** print a formatted output message reporting a parse error to the given output file */
RYML_EXPORT void err_visit_print(const char* msg, size_t length, Location const& cpploc, Tree const* tree, id_type id, FILE *f=nullptr);
/** format a message reporting a visit error, where @p dumpfn is a
 * function taking a csubstr and abstracting a string concatenation
 * operation, such as appending to a std::string or printing to
 * terminal. For example:
 *
 * ```c++
 * /// to output to cerr:
 * err_visit_msg(errmsg, errlen, cpploc, tree, id, [](csubstr s){
 *     std::cerr.write(s.str, s.len);
 * });
 *
 * /// to build a string:
 * std::string msg;
 * err_visit_msg(errmsg, errlen, cpploc, tree, id, [&msg](csubstr s){
 *     msg.append(s.str, s.len);
 * });
 * ```
 */
template<class DumpFn>
C4_NO_INLINE void err_visit_fmt(const char* msg, size_t length, Location const& cpploc, Tree const* tree, id_type id, DumpFn &&dumpfn)
{
    char buf_[32];
    substr buf(buf_);
    location_msg(cpploc, dumpfn);
    std::forward<DumpFn>(dumpfn)(" ERROR: [visit] ");
    std::forward<DumpFn>(dumpfn)(csubstr{msg, length});
    std::forward<DumpFn>(dumpfn)("\n");
    if(id != NONE && tree != nullptr)
    {
        location_msg(cpploc, dumpfn);
        std::forward<DumpFn>(dumpfn)(" ERROR: (");
        if(id != NONE)
        {
            std::forward<DumpFn>(dumpfn)("node=");
            std::forward<DumpFn>(dumpfn)(detail::_to_chars_limited(buf, id));
            if(tree != nullptr)
                std::forward<DumpFn>(dumpfn)(" ");
        }
        if(tree != nullptr)
        {
            std::forward<DumpFn>(dumpfn)("tree=");
            std::forward<DumpFn>(dumpfn)(detail::_to_chars_limited(buf, static_cast<void const*>(tree)));
        }
        std::forward<DumpFn>(dumpfn)(")");
    }
}
/** report a visit error to its respective handler, with a non-formatted error message */
C4_NORETURN C4_NO_INLINE void err_visit(Callbacks const& callbacks, Location const& cpploc, Tree const* tree, id_type node, const char *msg_);
/** report a visit error to its respective handler, with a non-formatted error message. Like (1), but use the current global callbacks. */
C4_NORETURN C4_NO_INLINE void err_visit(Location const& cpploc, Tree const* tree, id_type node, const char *msg);
/** report a visit error to its respective handler, with a formatted error message */
template<class ...Args>
C4_NORETURN C4_NO_INLINE void err_visit_msg(Callbacks const& callbacks, Location const& cpploc, Tree const* tree, id_type node, const char *fmt, Args const& ...args)
{
    char errbuf[RYML_ERRMSG_SIZE];
    csubstr msg = detail::_mk_err_msg(errbuf, to_csubstr(fmt), args...);
    if(callbacks.m_error_visit)
        callbacks.m_error_visit(msg.str, msg.len, cpploc, tree, node, callbacks.m_user_data);
    // fall to basic error if there is no visit handler set
    else if(callbacks.m_error_basic)
        callbacks.m_error_basic(msg.str, msg.len, cpploc, callbacks.m_user_data);
    abort(); // the call above should not return, so force it here in case it does // LCOV_EXCL_LINE
    C4_UNREACHABLE_AFTER_ERR();
}
/** report a visit error to its respective handler, with a formatted error message. Like (1), but use the current global callbacks. */
template<class ...Args>
C4_NORETURN C4_NO_INLINE void err_visit_msg(Location const& cpploc, Tree const* tree, id_type node, const char *fmt, Args const& ...args)
{
    err_visit_msg(get_callbacks(), cpploc, tree, node, fmt, args...);
    C4_UNREACHABLE_AFTER_ERR();
}


//-----------------------------------------------------------------------------

#if defined(_RYML_WITH_EXCEPTIONS) || defined(__DOXYGEN__)

/** Exception thrown by the default basic error implementation. To
 * obtain the full error message, use @ref err_basic_fmt(), or use
 * @ref err_basic_print() to print it directly without creating an
 * intermediate string.
 *
 * @note Available only if @ref
 * RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS is defined, and @ref
 * RYML_NO_DEFAULT_CALLBACKS is NOT defined. */
struct RYML_EXPORT BasicException : public std::exception
{
    BasicException(const char *msg_, size_t len, Location const& loc) noexcept;
    const char* what() const noexcept override { return msg; }
    Location cpploc; ///< location of C++ source file where the error was detected
    char msg[RYML_ERRMSG_SIZE]; ///< the reported error message, without location indication.
};


/** Exception thrown by the default parse error implementation. To
 * obtain the full error message containing context, use @ref
 * err_parse_fmt(), or use @ref err_parse_print() to print it directly
 * without creating an intermediate string.
 *
 * @note Available only if @ref
 * RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS is defined, and @ref
 * RYML_NO_DEFAULT_CALLBACKS is NOT defined. */
struct RYML_EXPORT ParseException : public BasicException
{
    ParseException(const char *msg, size_t len, Location const& cpploc_, Location const& ymlloc_) noexcept;
    Location ymlloc; ///< location of YAML source where the error was detected
};


/** Exception thrown by the default visit error implementation. To
 * obtain the full error message containing context, use @ref
 * err_visit_fmt(), or use @ref err_visit_print() to print it directly
 * without creating an intermediate string.
 *
 * @note Available only if @ref
 * RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS is defined, and @ref
 * RYML_NO_DEFAULT_CALLBACKS is NOT defined. */
struct RYML_EXPORT VisitException : public BasicException
{
    VisitException(const char *msg, size_t len, Location const& cpploc_, Tree const* tree_, id_type node_) noexcept;
    Tree const* tree; ///< tree where the error was detected
    id_type node;     ///< id of the node where the error was detected
};

#endif // _RYML_WITH_EXCEPTIONS

/** @} */


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/// @cond dev

#define _RYML_CB_ALLOC_HINT(cb, T, num, hint) (T*) (cb).m_allocate((num) * sizeof(T), (hint), (cb).m_user_data)
#define _RYML_CB_ALLOC(cb, T, num) _RYML_CB_ALLOC_HINT((cb), T, (num), nullptr)
#define _RYML_CB_FREE(cb, buf, T, num)                              \
    do {                                                            \
        (cb).m_free((buf), (num) * sizeof(T), (cb).m_user_data);    \
        (buf) = nullptr;                                            \
    } while(false)


#if RYML_USE_ASSERT
#   define RYML_ASSERT_BASIC(cond) RYML_CHECK_BASIC(cond)
#   define RYML_ASSERT_BASIC_MSG(cond, fmt, ...) RYML_CHECK_BASIC_MSG(cond, fmt, __VA_ARGS__)
#   define _RYML_CB_ASSERT_BASIC(cb, cond) _RYML_CB_CHECK_BASIC((cb), cond)
#   define _RYML_CB_ASSERT_BASIC_MSG(cb, cond, fmt, ...) _RYML_CB_CHECK_BASIC_MSG((cb), (cond), fmt, __VA_ARGS__)
#   define RYML_ASSERT_PARSE(cond, ymlloc) RYML_CHECK_PARSE(cond, (ymlloc))
#   define RYML_ASSERT_PARSE_MSG(cond, ymlloc, fmt, ...) RYML_CHECK_PARSE_MSG(cond, (ymlloc), fmt, __VA_ARGS__)
#   define _RYML_CB_ASSERT_PARSE(cb, cond, ymlloc) _RYML_CB_CHECK_PARSE((cb), cond, (ymlloc))
#   define _RYML_CB_ASSERT_PARSE_MSG(cb, cond, ymlloc, fmt, ...) _RYML_CB_CHECK_PARSE_MSG((cb), cond, (ymlloc), fmt, __VA_ARGS__)
#   define RYML_ASSERT_VISIT(cond, tree, id) RYML_CHECK_VISIT(cond, (tree), (id))
#   define RYML_ASSERT_VISIT_MSG(cond, tree, id, fmt, ...) RYML_CHECK_VISIT_MSG(cond, (tree), (id), fmt, __VA_ARGS__)
#   define _RYML_CB_ASSERT_VISIT(cb, cond, tree, id) _RYML_CB_CHECK_VISIT((cb), cond, (tree), (id))
#   define _RYML_CB_ASSERT_VISIT_MSG(cb, cond, tree, id, fmt, ...) _RYML_CB_CHECK_VISIT_MSG((cb), cond, (tree), (id), fmt, __VA_ARGS__)
#else
#   define RYML_ASSERT_BASIC(cond)
#   define RYML_ASSERT_BASIC_MSG(cond, fmt, ...)
#   define _RYML_CB_ASSERT_BASIC(cb, cond)
#   define _RYML_CB_ASSERT_BASIC_MSG(cb, cond, fmt, ...)
#   define RYML_ASSERT_PARSE(cond, ymlloc)
#   define RYML_ASSERT_PARSE_MSG(cond, ymlloc, fmt, ...)
#   define _RYML_CB_ASSERT_PARSE(cb, cond, ymlloc)
#   define _RYML_CB_ASSERT_PARSE_MSG(cb, cond, ymlloc, fmt, ...)
#   define RYML_ASSERT_VISIT(cond, tree, id)
#   define RYML_ASSERT_VISIT_MSG(cond, tree, id, fmt, ...)
#   define _RYML_CB_ASSERT_VISIT(cb, cond, tree, id)
#   define _RYML_CB_ASSERT_VISIT_MSG(cb, cond, tree, id, fmt, ...)
#endif

#define _RYML_ERR_BASIC(msg)                        \
    do                                              \
    {                                               \
        RYML_DEBUG_BREAK();                         \
        ::c4::yml::err_basic(RYML_LOC_HERE(), msg); \
        C4_UNREACHABLE_AFTER_ERR();                 \
    } while(false)
#define _RYML_ERR_BASIC_MSG(fmt, ...)                                   \
    do                                                                  \
    {                                                                   \
        RYML_DEBUG_BREAK();                                             \
        ::c4::yml::err_basic_msg(RYML_LOC_HERE(), fmt, __VA_ARGS__);    \
        C4_UNREACHABLE_AFTER_ERR();                                     \
    } while(false)
#define _RYML_CB_ERR_BASIC(cb, msg)                         \
    do                                                      \
    {                                                       \
        RYML_DEBUG_BREAK();                                 \
        ::c4::yml::err_basic((cb), RYML_LOC_HERE(), msg);   \
        C4_UNREACHABLE_AFTER_ERR();                         \
    } while(false)
#define _RYML_CB_ERR_BASIC_MSG(cb, fmt, ...)                            \
    do                                                                  \
    {                                                                   \
        RYML_DEBUG_BREAK();                                             \
        ::c4::yml::err_basic_msg((cb), RYML_LOC_HERE(), fmt, __VA_ARGS__); \
        C4_UNREACHABLE_AFTER_ERR();                                     \
    } while(false)
#define RYML_CHECK_BASIC(cond)                                          \
    do {                                                                \
        if(C4_UNLIKELY(!(cond)))                                        \
        {                                                               \
            RYML_DEBUG_BREAK();                                         \
            ::c4::yml::err_basic(RYML_LOC_HERE(), "check failed: " #cond); \
            C4_UNREACHABLE_AFTER_ERR();                                 \
        }                                                               \
    } while(false)
#define RYML_CHECK_BASIC_MSG(cond, fmt, ...)                            \
    do {                                                                \
        if(C4_UNLIKELY(!(cond)))                                        \
        {                                                               \
            RYML_DEBUG_BREAK();                                         \
            ::c4::yml::err_basic_msg(RYML_LOC_HERE(), "check failed: " #cond ". " fmt, __VA_ARGS__); \
            C4_UNREACHABLE_AFTER_ERR();                                 \
        }                                                               \
    } while(false)
#define _RYML_CB_CHECK_BASIC(cb, cond)                                  \
    do {                                                                \
        if(C4_UNLIKELY(!(cond)))                                        \
        {                                                               \
            RYML_DEBUG_BREAK();                                         \
            ::c4::yml::err_basic((cb), RYML_LOC_HERE(), "check failed: " #cond); \
            C4_UNREACHABLE_AFTER_ERR();                                 \
        }                                                               \
    } while(false)
#define _RYML_CB_CHECK_BASIC_MSG(cb, cond, fmt, ...)                    \
    do {                                                                \
        if(C4_UNLIKELY(!(cond)))                                        \
        {                                                               \
            RYML_DEBUG_BREAK();                                         \
            ::c4::yml::err_basic_msg((cb), RYML_LOC_HERE(), "check failed: " #cond ". " fmt, __VA_ARGS__); \
            C4_UNREACHABLE_AFTER_ERR();                                 \
        }                                                               \
    } while(false)


#define _RYML_ERR_PARSE(ymlloc, msg)                            \
    do                                                          \
    {                                                           \
        RYML_DEBUG_BREAK();                                     \
        ::c4::yml::err_parse(RYML_LOC_HERE(), (ymlloc), msg);   \
        C4_UNREACHABLE_AFTER_ERR();                             \
    } while(false)
#define _RYML_ERR_PARSE_MSG(ymlloc, fmt, ...)                           \
    do                                                                  \
    {                                                                   \
        RYML_DEBUG_BREAK();                                             \
        ::c4::yml::err_parse_msg(RYML_LOC_HERE(), (ymlloc), fmt, __VA_ARGS__); \
        C4_UNREACHABLE_AFTER_ERR();                                     \
    } while(false)
#define _RYML_CB_ERR_PARSE(cb, ymlloc, msg)                         \
    do                                                              \
    {                                                               \
        RYML_DEBUG_BREAK();                                         \
        ::c4::yml::err_parse((cb), RYML_LOC_HERE(), (ymlloc), msg); \
        C4_UNREACHABLE_AFTER_ERR();                                 \
    } while(false)
#define RYML_CHECK_PARSE(cond, ymlloc)                                  \
    do {                                                                \
        if(C4_UNLIKELY(!(cond)))                                        \
        {                                                               \
            RYML_DEBUG_BREAK();                                         \
            ::c4::yml::err_parse(RYML_LOC_HERE(), (ymlloc), "check failed: " #cond); \
            C4_UNREACHABLE_AFTER_ERR();                                 \
        }                                                               \
    } while(false)
#define RYML_CHECK_PARSE_MSG(cond, ymlloc, fmt, ...)                    \
    do {                                                                \
        if(C4_UNLIKELY(!(cond)))                                        \
        {                                                               \
            RYML_DEBUG_BREAK();                                         \
            ::c4::yml::err_parse_msg(RYML_LOC_HERE(), (ymlloc), "check failed: " #cond ". " fmt, __VA_ARGS__); \
            C4_UNREACHABLE_AFTER_ERR();                                 \
        }                                                               \
    } while(false)
#define _RYML_CB_CHECK_PARSE(cb, cond, ymlloc)                          \
    do {                                                                \
        if(C4_UNLIKELY(!(cond)))                                        \
        {                                                               \
            RYML_DEBUG_BREAK();                                         \
            ::c4::yml::err_parse((cb), RYML_LOC_HERE(), (ymlloc), "check failed: " #cond); \
            C4_UNREACHABLE_AFTER_ERR();                                 \
        }                                                               \
    } while(false)
#define _RYML_CB_CHECK_PARSE_MSG(cb, cond, ymlloc, fmt, ...)            \
    do {                                                                \
        if(C4_UNLIKELY(!(cond)))                                        \
        {                                                               \
            RYML_DEBUG_BREAK();                                         \
            ::c4::yml::err_parse_msg((cb), RYML_LOC_HERE(), (ymlloc), "check failed: " #cond ". " fmt, __VA_ARGS__); \
            C4_UNREACHABLE_AFTER_ERR();                                 \
        }                                                               \
    } while(false)




#define _RYML_ERR_VISIT(tree, node, msg)                            \
    do                                                              \
    {                                                               \
        RYML_DEBUG_BREAK();                                         \
        ::c4::yml::err_visit(RYML_LOC_HERE(), (tree), (node), msg); \
        C4_UNREACHABLE_AFTER_ERR();                                 \
    } while(false)
#define _RYML_ERR_VISIT_MSG(tree, node, fmt, ...)                       \
    do                                                                  \
    {                                                                   \
        RYML_DEBUG_BREAK();                                             \
        ::c4::yml::err_visit_msg(RYML_LOC_HERE(), (tree), (node), fmt, __VA_ARGS__); \
        C4_UNREACHABLE_AFTER_ERR();                                     \
    } while(false)
#define _RYML_CB_ERR_VISIT(cb, tree, node, msg)                         \
    do                                                                  \
    {                                                                   \
        RYML_DEBUG_BREAK();                                             \
        ::c4::yml::err_visit((cb), RYML_LOC_HERE(), (tree), (node), msg); \
        C4_UNREACHABLE_AFTER_ERR();                                     \
    } while(false)
#define _RYML_CB_ERR_VISIT_MSG(cb, tree, node, fmt, ...)                \
    do                                                                  \
    {                                                                   \
        RYML_DEBUG_BREAK();                                             \
        ::c4::yml::err_visit_msg((cb), RYML_LOC_HERE(), (tree), (node), fmt, __VA_ARGS__); \
        C4_UNREACHABLE_AFTER_ERR();                                     \
    } while(false)
#define RYML_CHECK_VISIT(cond, tree, node)                              \
    do {                                                                \
        if(C4_UNLIKELY(!(cond)))                                        \
        {                                                               \
            RYML_DEBUG_BREAK();                                         \
            ::c4::yml::err_visit(RYML_LOC_HERE(), (tree), (node), "check failed: " #cond); \
            C4_UNREACHABLE_AFTER_ERR();                                 \
        }                                                               \
    } while(false)
#define RYML_CHECK_VISIT_MSG(cond, tree, node, fmt, ...)                \
    do {                                                                \
        if(C4_UNLIKELY(!(cond)))                                        \
        {                                                               \
            RYML_DEBUG_BREAK();                                         \
            ::c4::yml::err_visit_msg(RYML_LOC_HERE(), (tree), (node), "check failed: " #cond ". " fmt, __VA_ARGS__); \
            C4_UNREACHABLE_AFTER_ERR();                                 \
        }                                                               \
    } while(false)
#define _RYML_CB_CHECK_VISIT(cb, cond, tree, node)                      \
    do {                                                                \
        if(C4_UNLIKELY(!(cond)))                                        \
        {                                                               \
            RYML_DEBUG_BREAK();                                         \
            ::c4::yml::err_visit((cb), RYML_LOC_HERE(), (tree), (node), "check failed: " #cond); \
            C4_UNREACHABLE_AFTER_ERR();                                 \
        }                                                               \
    } while(false)
#define _RYML_CB_CHECK_VISIT_MSG(cb, cond, tree, node, fmt, ...)        \
    do {                                                                \
        if(C4_UNLIKELY(!(cond)))                                        \
        {                                                               \
            RYML_DEBUG_BREAK();                                         \
            ::c4::yml::err_visit_msg((cb), RYML_LOC_HERE(), (tree), (node), "check failed: " #cond ". " fmt, __VA_ARGS__); \
            C4_UNREACHABLE_AFTER_ERR();                                 \
        }                                                               \
    } while(false)



/// @endcond


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

typedef enum {
    NOBOM,
    UTF8,
    UTF16LE,
    UTF16BE,
    UTF32LE,
    UTF32BE,
} Encoding_e;


/// @cond dev
inline csubstr _c4prc(const char &C4_RESTRICT c) // pass by reference!
{
    switch(c)
    {
    case '\n': return csubstr("\\n");
    case '\t': return csubstr("\\t");
    case '\0': return csubstr("\\0");
    case '\r': return csubstr("\\r");
    case '\f': return csubstr("\\f");
    case '\b': return csubstr("\\b");
    case '\v': return csubstr("\\v");
    case '\a': return csubstr("\\a");
    default: return csubstr(&c, 1);
    }
}
/// @endcond

C4_SUPPRESS_WARNING_GCC_POP

} // namespace yml
} // namespace c4

#endif /* _C4_YML_COMMON_HPP_ */
