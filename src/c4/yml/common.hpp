#ifndef _C4_YML_COMMON_HPP_
#define _C4_YML_COMMON_HPP_

/** @file common.hpp Common utilities and infrastructure used by ryml. */

#include <cstddef>
#include <c4/substr.hpp>
#include <c4/charconv.hpp>
#include <c4/yml/export.hpp>


//-----------------------------------------------------------------------------

#ifndef RYML_DEFAULT_TREE_CAPACITY
/// default capacity for the tree when not set explicitly
#define RYML_DEFAULT_TREE_CAPACITY (16)
#endif

#ifndef RYML_DEFAULT_TREE_ARENA_CAPACITY
/// default capacity for the tree's arena when not set explicitly
#define RYML_DEFAULT_TREE_ARENA_CAPACITY (0)
#endif


#ifndef RYML_LOCATIONS_SMALL_THRESHOLD
/// threshold at which a location search will revert from linear to
/// binary search.
#define RYML_LOCATIONS_SMALL_THRESHOLD (30)
#endif


#ifndef RYML_ERRMSG_SIZE
/// size for the error message buffer
#define RYML_ERRMSG_SIZE (1024)
#endif


#ifndef RYML_LOGBUF_SIZE
/// size for the buffer used to format individual values to string
/// while preparing an error message. This is only used for formatting
/// individual values in the message; final messages will be larger
/// than this value (see @ref RYML_ERRMSG_SIZE). This is also used for
/// the detailed debug log messages when RYML_DBG is defined.
#define RYML_LOGBUF_SIZE (256)
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


#ifndef RYML_LOGBUF_SIZE_MAX
/// size for the fallback larger log buffer. When @ref
/// RYML_LOGBUF_SIZE is not large enough to convert a value to string,
/// then temporary stack memory is allocated up to
/// RYML_LOGBUF_SIZE_MAX. This limit is in place to prevent a stack
/// overflow. If the printed value requires more than
/// RYML_LOGBUF_SIZE_MAX, the value is silently skipped.
#define RYML_LOGBUF_SIZE_MAX (1024)
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

/** (Undefined by default) Use shorter error message from
 * checks/asserts: do not show the check condition in the error
 * message. */
#   defined RYML_SHORT_CHECK_MSG

#endif


//-----------------------------------------------------------------------------

/** @cond dev */

#ifndef RYML_USE_ASSERT
#   define RYML_USE_ASSERT C4_USE_ASSERT
#endif

#if RYML_USE_ASSERT
#   define RYML_NOEXCEPT
#else
#   define RYML_NOEXCEPT noexcept
#endif

#define RYML_DEPRECATED(msg) C4_DEPRECATED(msg)

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


typedef enum Encoding_ {
    NOBOM,         //!< No Byte Order Mark was found
    UTF8,          //!< UTF8
    UTF16LE,       //!< UTF16, Little-Endian
    UTF16BE,       //!< UTF16, Big-Endian
    UTF32LE,       //!< UTF32, Little-Endian
    UTF32BE,       //!< UTF32, Big-Endian
} Encoding_e;


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


/** holds a source or yaml file position, for example when an error is
 * detected; See also @ref location_format() and @ref
 * location_format_with_context().
 *
 * @ingroup doc_error_handling */
struct RYML_EXPORT Location
{
    size_t offset; ///< number of bytes from the beginning of the source buffer
    size_t line;   ///< line
    size_t col;    ///< column
    csubstr name;  ///< name of the file

    operator bool () const noexcept { return !name.empty() || line != npos || offset != npos || col != npos; }

    C4_NO_INLINE Location() noexcept : offset(npos), line(npos), col(npos), name() {};
    C4_NO_INLINE Location(                         size_t l          ) noexcept : offset(npos), line(l), col(npos), name() {}
    C4_NO_INLINE Location(                         size_t l, size_t c) noexcept : offset(npos), line(l), col(c   ), name() {}
    C4_NO_INLINE Location(               size_t b, size_t l, size_t c) noexcept : offset(b   ), line(l), col(c   ), name() {}
    C4_NO_INLINE Location(    csubstr n,           size_t l          ) noexcept : offset(npos), line(l), col(npos), name(n) {}
    C4_NO_INLINE Location(    csubstr n,           size_t l, size_t c) noexcept : offset(npos), line(l), col(c   ), name(n) {}
    C4_NO_INLINE Location(    csubstr n, size_t b, size_t l, size_t c) noexcept : offset(b   ), line(l), col(c   ), name(n) {}
    C4_NO_INLINE Location(const char *n,           size_t l          ) noexcept : offset(npos), line(l), col(npos), name(to_csubstr(n)) {}
    C4_NO_INLINE Location(const char *n,           size_t l, size_t c) noexcept : offset(npos), line(l), col(c   ), name(to_csubstr(n)) {}
    C4_NO_INLINE Location(const char *n, size_t b, size_t l, size_t c) noexcept : offset(b   ), line(l), col(c   ), name(to_csubstr(n)) {}
};
static_assert(std::is_standard_layout<Location>::value, "Location not trivial");

/// @cond dev
#define RYML_LOC_HERE() (::c4::yml::Location(__FILE__, static_cast<size_t>(__LINE__)))
/// @endcond


/** Data for a basic error.
 * @ingroup doc_error_handling */
struct RYML_EXPORT ErrorDataBasic
{
    Location location; ///< location where the error was detected (may be from YAML or C++ source code)
    ErrorDataBasic() noexcept = default;
    ErrorDataBasic(Location const& cpploc_) noexcept : location(cpploc_) {}
};

/** Data for a parse error.
 * @ingroup doc_error_handling */
struct RYML_EXPORT ErrorDataParse
{
    Location cpploc; ///< location in the C++ source file where the error was detected.
    Location ymlloc; ///< location in the YAML source buffer where the error was detected.
    ErrorDataParse() noexcept = default;
    ErrorDataParse(Location const& cpploc_, Location const& ymlloc_) noexcept : cpploc(cpploc_), ymlloc(ymlloc_) {}
};

/** Data for a visit error.
 * @ingroup doc_error_handling */
struct RYML_EXPORT ErrorDataVisit
{
    Location cpploc;  ///< location in the C++ source file where the error was detected.
    Tree const* tree; ///< tree where the error was detected
    id_type node;     ///< node where the error was detected
    ErrorDataVisit() noexcept = default;
    ErrorDataVisit(Location const& cpploc_, Tree const *tree_ , id_type node_) noexcept : cpploc(cpploc_), tree(tree_), node(node_) {}
};



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/** Options to give to the parser to control its behavior. */
struct RYML_EXPORT ParserOptions
{
private:

    typedef enum : uint32_t {
        SCALAR_FILTERING = (1u << 0u),
        LOCATIONS = (1u << 1u),
        DETECT_FLOW_ML = (1u << 2u),
        DEFAULTS = SCALAR_FILTERING|DETECT_FLOW_ML,
    } Flags_e;

    uint32_t flags = DEFAULTS;

public:

    ParserOptions() = default;

public:

    /** @name source location tracking */
    /** @{ */

    /** enable/disable source location tracking */
    ParserOptions& locations(bool enabled) noexcept
    {
        if(enabled)
            flags |= LOCATIONS;
        else
            flags &= ~LOCATIONS;
        return *this;
    }
    /** query source location tracking status */
    C4_ALWAYS_INLINE bool locations() const noexcept { return (flags & LOCATIONS); }

    /** @} */

public:

    /** @name detection of @ref FLOW_ML container style */
    /** @{ */

    /** enable/disable detection of @ref FLOW_ML container style. When
     * enabled, the parser will set @ref FLOW_ML as the style of flow
     * containers which have the terminating bracket on a line
     * different from that of the opening bracket. */
    ParserOptions& detect_flow_ml(bool enabled) noexcept
    {
        if(enabled)
            flags |= DETECT_FLOW_ML;
        else
            flags &= ~DETECT_FLOW_ML;
        return *this;
    }
    /** query status of detection of @ref FLOW_ML container style. */
    C4_ALWAYS_INLINE bool detect_flow_ml() const noexcept { return (flags & DETECT_FLOW_ML); }

    /** @} */

public:

    /** @name scalar filtering status (experimental; disable at your discretion) */
    /** @{ */

    /** enable/disable scalar filtering while parsing */
    ParserOptions& scalar_filtering(bool enabled) noexcept
    {
        if(enabled)
            flags |= SCALAR_FILTERING;
        else
            flags &= ~SCALAR_FILTERING;
        return *this;
    }
    /** query scalar filtering status */
    C4_ALWAYS_INLINE bool scalar_filtering() const noexcept { return (flags & SCALAR_FILTERING); }

    /** @} */
};


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
 * @warning This function is NOT thread-safe, because it reads global static data
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
using pfn_error_basic = void (*) (csubstr msg, ErrorDataBasic const& errdata, void *user_data);
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
using pfn_error_parse = void (*) (csubstr msg, ErrorDataParse const& errdata, void *user_data);
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
using pfn_error_visit = void (*) (csubstr msg, ErrorDataVisit const& errdata, void *user_data);

/// @cond dev
using pfn_error RYML_DEPRECATED("use a more specific error type: `basic`, `parse` or `visit`") = void (*) (const char* msg, size_t msg_len, Location const& cpploc, void *user_data);
/// @endcond


/** A c-style callbacks class to customize behavior on errors or
 * allocation. Can be used globally by the library and/or locally by
 * @ref Tree and @ref Parser objects. */
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
     * defined. */
    Callbacks& set_allocate(pfn_allocate allocate=nullptr);

    /** Set or reset the free callback. When the parameter is null,
     * m_free will fall back to ryml's default free implementation,
     * unless @ref RYML_NO_DEFAULT_CALLBACKS is defined. */
    Callbacks& set_free(pfn_free free=nullptr);

    /** Set or reset the error_basic callback. When the parameter is null,
     * m_error_basic will fall back to ryml's default error_basic implementation,
     * unless @ref RYML_NO_DEFAULT_CALLBACKS is defined. */
    Callbacks& set_error_basic(pfn_error_basic error_basic=nullptr);

    /** Set or reset the error_parse callback. When the parameter is null,
     * m_error_parse will fall back to ryml's default error_parse implementation,
     * unless @ref RYML_NO_DEFAULT_CALLBACKS is defined. */
    Callbacks& set_error_parse(pfn_error_parse error_parse=nullptr);

    /** Set or reset the error_visit callback. When the parameter is null,
     * m_error_visit will fall back to ryml's default error_visit implementation,
     * unless @ref RYML_NO_DEFAULT_CALLBACKS is defined. */
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

#define _RYML_CB_ALLOC_HINT(cb, T, num, hint) (T*) (cb).m_allocate((num) * sizeof(T), (hint), (cb).m_user_data)
#define _RYML_CB_ALLOC(cb, T, num) _RYML_CB_ALLOC_HINT((cb), T, (num), nullptr)
#define _RYML_CB_FREE(cb, buf, T, num)                              \
    do {                                                            \
        (cb).m_free((buf), (num) * sizeof(T), (cb).m_user_data);    \
        (buf) = nullptr;                                            \
    } while(false)

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
