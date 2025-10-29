#ifndef _C4_YML_ERROR_HPP_
#define _C4_YML_ERROR_HPP_

/** @file error.hpp Error utilities used by ryml. */

#ifndef _C4_YML_COMMON_HPP_
#include <c4/yml/common.hpp>
#endif

/// @cond dev
#if (defined(C4_EXCEPTIONS) && (!defined(RYML_NO_DEFAULT_CALLBACKS) && defined(RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS))) || defined(__DOXYGEN__)
#define _RYML_WITH_EXCEPTIONS
#endif
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
/// @endcond

#ifdef _RYML_WITH_EXCEPTIONS
#include <exception>
#endif


namespace c4 {
namespace yml {


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
        const size_t numdots = (buf.len > 3) ? 3 : buf.len;
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

RYML_EXPORT csubstr _get_text_region(csubstr text, size_t pos, size_t num_lines_before, size_t num_lines_after);

} // namespace detail

/// @endcond


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/** @addtogroup doc_error_handling
 *
 * @{ */


/** generic formatting of a location
 *
 * @param dumpfn function taking a csubstr and abstracting a string
 * concatenation operation, such as appending to a std::string or
 * printing to terminal.
 * @param loc the location to be formatted
 *
 * For example:
 *
 * ```c++
 * /// to output to std::cerr:
 * location_format([&s](csubstr s){
 *     std::cerr.write(s.str, s.len);
 * }, loc);
 *
 * /// to build a string:
 * std::string msg;
 * location_format([&s](csubstr s){
 *     msg.append(s.str, s.len);
 * }, loc);
 * ```
 */
template<class DumpFn>
C4_NO_INLINE size_t location_format(DumpFn &&dumpfn, Location const& loc);


/** Generic formatting of a location, printing the source code buffer
 * region around the location.
 *
 * @param dumpfn function taking a csubstr and abstracting a string
 * concatenation operation, such as appending to a std::string or
 * printing to terminal.
 * @param location the location
 * @param source_buffer the source buffer
 * @param call a string with a call of attention to print in the
 *        message (see examples below)
 * @param num_lines_before how many source buffer lines to print
 *        before the location line
 * @param num_lines_after how many source buffer lines to print
 *        after the location line
 * @param first_col_highlight the first column to highlight
 *        around the location line
 * @param last_col_highlight the last column to highlight
 *        around the location line
 * @param maxlen the maximum number of columns to show in the error
 *        message; source buffer lines will have at most this number
 *        of columns shown; if the line is longer than this, the line
 *        will be trimmed as needed at the end and/or beginning, and
 *        only the relevant columns *around* the location are shown
 *
 * For example:
 *
 * ```c++
 * std::string out;
 * auto dumpfn = [&out](csubstr s){ out.append(s.str, s.len); };
 * format_location_with_context(dumpfn, location, source, "error");
 * ```
 *
 * will result in this string:
 *
 * ```
 * file.yaml:3: col=3 (11B): error:
 * error:
 * error:     ccc
 * error:        |
 * error:        (here)
 * error:
 * error: see region:
 * error:
 * error:     aaa
 * error:     bbb
 * error:     ccc
 * error:        |
 * error:        (here)
 * ```
 *
 * If an empty string is passed for the call of attention,
 *
 * ```c++
 * format_location_with_context(dumpfn, location, source);
 * ```
 *
 * the returned string becomes:
 *
 * ```
 * file.yaml:3: col=3 (11B): ccc
 *                             |
 *                             (here)
 * file.yaml:3: col=3 (11B): see region:
 * aaa
 * bbb
 * ccc
 *    |
 *    (here)
 * ```
 */
template<class DumpFn>
C4_NO_INLINE void location_format_with_context(DumpFn &&dumpfn,
                                               Location const &location,
                                               csubstr source_buffer,
                                               csubstr call = "",
                                               size_t num_lines_before = 3,
                                               size_t num_lines_after = 0,
                                               size_t first_col_highlight = 0,
                                               size_t last_col_highlight = 0,
                                               size_t maxlen = 80u);


//-----------------------------------------------------------------------------

/** Given an error message and associated basic error data, format it fully as a basic error message.
 *
 * @param dumpfn function taking a csubstr and abstracting a string
 * concatenation operation, such as appending to a std::string or
 * printing to terminal.
 * @param msg the error message
 * @param errdata the error data
 *
 * For example:
 *
 * ```c++
 * /// to output to cerr:
 * err_basic_format([](csubstr s){
 *     std::cerr.write(s.str, s.len);
 * }, errmsg, errdata);
 *
 * /// to build a string:
 * std::string msg;
 * error_basic_format([&msg](csubstr s){
 *     msg.append(s.str, s.len);
 * }, errmsg, errdata);
 * ```
 */
template<class DumpFn>
C4_NO_INLINE void err_basic_format(DumpFn &&dumpfn, csubstr msg, ErrorDataBasic const& errdata);

/** trigger a basic error to its respective handler, with a non-formatted error message. */
C4_NORETURN RYML_EXPORT C4_NO_INLINE void err_basic(Callbacks const& callbacks, ErrorDataBasic const& errdata, const char* msg_);
/** trigger a basic error to its respective handler, with a non-formatted error message. Like (1), but use the current global callbacks. */
C4_NORETURN RYML_EXPORT C4_NO_INLINE void err_basic(ErrorDataBasic const& errdata, const char* msg);
/** trigger a basic error to its respective handler, with a formatted error message. */
template<class ...Args>
C4_NORETURN C4_NO_INLINE void err_basic(Callbacks const& callbacks, ErrorDataBasic const& errdata, const char *fmt, Args const& ...args)
{
    char errbuf[RYML_ERRMSG_SIZE];
    csubstr msg = detail::_mk_err_msg(errbuf, to_csubstr(fmt), args...);
    callbacks.m_error_basic(msg, errdata, callbacks.m_user_data);
    abort(); // the call above should not return, but force it here in case it does // LCOV_EXCL_LINE
    C4_UNREACHABLE_AFTER_ERR();
}
/** trigger a basic error to its respective handler, with a formatted error message. Like (1), but use the current global callbacks. */
template<class ...Args>
C4_NORETURN C4_NO_INLINE void err_basic(ErrorDataBasic const& errdata, const char *fmt, Args const& ...args)
{
    err_basic(get_callbacks(), errdata, fmt, args...);
    C4_UNREACHABLE_AFTER_ERR();
}


/** Given an error message and associated parse error data, format it fully as a parse error message.
 *
 * @param dumpfn function taking a csubstr and abstracting a string
 * concatenation operation, such as appending to a std::string or
 * printing to terminal.
 * @param msg the error message
 * @param errdata the error data
 *
 * For example:
 *
 * ```c++
 * /// to output to cerr:
 * /// this is what err_parse_print() does
 * err_parse_format([](csubstr s){
 *     std::cerr.write(s.str, s.len);
 * }, errmsg, errdata);
 *
 * /// to build a string:
 * std::string msg;
 * err_parse_format([](csubstr s){
 *     s.append(s.str, s.len);
 * }, errmsg, errdata);
 * ```
 *
 * @note if the (preferably original) source buffer is kept, @ref
 * location_format_with_context() can be used to also an additional
 * rich error message showing the YAML source buffer region around
 * that location.
 */
template<class DumpFn>
C4_NO_INLINE void err_parse_format(DumpFn &&dumpfn, csubstr msg, ErrorDataParse const& errdata);

/** trigger a parse error to its respective handler, with a non-formatted error message */
C4_NORETURN RYML_EXPORT C4_NO_INLINE void err_parse(Callbacks const& callbacks, ErrorDataParse const& errdata, const char *msg);
/** trigger a parse error to its respective handler, with a non-formatted error message. Like (1), but use the current global callbacks. */
C4_NORETURN RYML_EXPORT C4_NO_INLINE void err_parse(ErrorDataParse const& errdata, const char *msg);
/** trigger a parse error to its respective handler, with a formatted error message */
template<class ...Args>
C4_NORETURN C4_NO_INLINE void err_parse(Callbacks const& callbacks, ErrorDataParse const& errdata, const char *fmt, Args const& ...args)
{
    char errbuf[RYML_ERRMSG_SIZE];
    csubstr msg = detail::_mk_err_msg(errbuf, to_csubstr(fmt), args...);
    if(callbacks.m_error_parse)
        callbacks.m_error_parse(msg, errdata, callbacks.m_user_data);
    // fall to basic error if there is no parse handler set, but use errdata.ymlloc instead of errdata.cpploc
    else if(callbacks.m_error_basic)
        callbacks.m_error_basic(msg, errdata.ymlloc, callbacks.m_user_data);
    abort(); // the call above should not return, so force it here in case it does // LCOV_EXCL_LINE
    C4_UNREACHABLE_AFTER_ERR();
}
/** trigger a parse error to its respective handler, with a formatted error message. Like (1), but use the current global callbacks. */
template<class ...Args>
C4_NORETURN C4_NO_INLINE void err_parse(ErrorDataParse const& errdata, const char *fmt, Args const& ...args)
{
    err_parse(get_callbacks(), errdata, fmt, args...);
    C4_UNREACHABLE_AFTER_ERR();
}


/** Given an error message and associated visit error data, format it
 * fully as a visit error message.
 *
 * @param dumpfn function taking a csubstr and abstracting a string
 * concatenation operation, such as appending to a std::string or
 * printing to terminal.
 * @param msg the error message
 * @param errdata the error data
 *
 * For example:
 *
 * ```c++
 * /// to output to cerr:
 * err_visit_format([](csubstr s){
 *     std::cerr.write(s.str, s.len);
 * }, errmsg, errdata);
 *
 * /// to build a string:
 * std::string msg;
 * err_visit_format([&msg](csubstr s){
 *     msg.append(s.str, s.len);
 * }, errmsg, errdata);
 *
 * @note under certain conditions, it is possible to obtain an
 * associated location, and subsequently use @ref
 * location_format_with_context() to also create a rich error message
 * showing the YAML source buffer region around that location. This is
 * possible if the (preferably original) source buffer is kept, and
 * the node location can be retrieved from the parser.
 * ```
 */
template<class DumpFn>
C4_NO_INLINE void err_visit_format(DumpFn &&dumpfn, csubstr msg, ErrorDataVisit const& errdata);


/** trigger a visit error to its respective handler, with a non-formatted error message */
C4_NORETURN RYML_EXPORT C4_NO_INLINE void err_visit(Callbacks const& callbacks, ErrorDataVisit const& errdata, const char *msg);
/** trigger a visit error to its respective handler, with a non-formatted error message. Like (1), but uses the current global callbacks. */
C4_NORETURN RYML_EXPORT C4_NO_INLINE void err_visit(ErrorDataVisit const& errdata, const char *msg);
/** trigger a visit error to its respective handler, with a formatted error message */
template<class ...Args>
C4_NORETURN C4_NO_INLINE void err_visit(Callbacks const& callbacks, ErrorDataVisit const& errdata, const char *fmt, Args const& ...args)
{
    char errbuf[RYML_ERRMSG_SIZE];
    csubstr msg = detail::_mk_err_msg(errbuf, to_csubstr(fmt), args...);
    if(callbacks.m_error_visit)
        callbacks.m_error_visit(msg, errdata, callbacks.m_user_data);
    // fall to basic error if there is no visit handler set
    else if(callbacks.m_error_basic)
        callbacks.m_error_basic(msg, errdata.cpploc, callbacks.m_user_data);
    abort(); // the call above should not return, so force it here in case it does // LCOV_EXCL_LINE
    C4_UNREACHABLE_AFTER_ERR();
}
/** trigger a visit error to its respective handler, with a formatted error message. Like (1), but use the current global callbacks. */
template<class ...Args>
C4_NORETURN C4_NO_INLINE void err_visit(ErrorDataVisit const& errdata, const char *fmt, Args const& ...args)
{
    err_visit(get_callbacks(), errdata, fmt, args...);
    C4_UNREACHABLE_AFTER_ERR();
}


//-----------------------------------------------------------------------------

#if defined(_RYML_WITH_EXCEPTIONS) || defined(__DOXYGEN__)

/** Exception thrown by the default basic error implementation. To
 * obtain the full error message, use @ref err_basic_format(), or the
 * helper @ref format_exc().
 *
 * @note Available only if @ref
 * RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS is defined, and @ref
 * RYML_NO_DEFAULT_CALLBACKS is NOT defined. */
struct RYML_EXPORT ExceptionBasic : public std::exception
{
    ExceptionBasic(csubstr msg, ErrorDataBasic const& errdata_) noexcept;
    const char* what() const noexcept override { return msg; }
    ErrorDataBasic errdata_basic; ///< error data
    char msg[RYML_ERRMSG_SIZE];   ///< the reported error message, without location indication.
};


/** Exception thrown by the default parse error implementation. To
 * obtain the full error message containing context, use @ref
 * err_parse_format(), or the helper @ref format_exc().
 *
 * @note This exception derives from @ref ExceptionBasic and can be
 * catched using either type.
 *
 * @note Available only if @ref
 * RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS is defined, and @ref
 * RYML_NO_DEFAULT_CALLBACKS is NOT defined. */
struct RYML_EXPORT ExceptionParse : public ExceptionBasic
{
    ExceptionParse(csubstr msg, ErrorDataParse const& errdata_) noexcept;
    ErrorDataParse errdata_parse;
};


/** Exception thrown by the default visit error implementation. To
 * obtain the full error message containing context, use @ref
 * err_visit_format(), or the helper @ref format_exc().
 *
 * @note This exception derives from @ref ExceptionBasic and can be
 * catched using either type.
 *
 * @note Available only if @ref
 * RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS is defined, and @ref
 * RYML_NO_DEFAULT_CALLBACKS is NOT defined. */
struct RYML_EXPORT ExceptionVisit : public ExceptionBasic
{
    ExceptionVisit(csubstr msg, ErrorDataVisit const& errdata_) noexcept;
    ErrorDataVisit errdata_visit;
};


/** Format a basic exception to an existing char container
 *
 * @note Available only if @ref
 * RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS is defined, and @ref
 * RYML_NO_DEFAULT_CALLBACKS is NOT defined.  */
template<class CharContainer>
void format_exc(CharContainer *out, ExceptionBasic const& exc)
{
    out->clear();
    err_basic_format([out](csubstr s){
        out->append(s.str, s.len);
    }, csubstr{exc.msg, strlen(exc.msg)}, exc.errdata_basic);
}
/** Format a parse exception to an existing char container
 *
 * @note Available only if @ref
 * RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS is defined, and @ref
 * RYML_NO_DEFAULT_CALLBACKS is NOT defined.  */
template<class CharContainer>
void format_exc(CharContainer *out, ExceptionParse const& exc)
{
    out->clear();
    err_parse_format([out](csubstr s){
        out->append(s.str, s.len);
    }, csubstr{exc.msg, strlen(exc.msg)}, exc.errdata_parse);
}
/** Format a visit exception to an existing char container
 *
 * @note Available only if @ref
 * RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS is defined, and @ref
 * RYML_NO_DEFAULT_CALLBACKS is NOT defined.  */
template<class CharContainer>
void format_exc(CharContainer *out, ExceptionVisit const& exc)
{
    out->clear();
    err_visit_format([out](csubstr s){
        out->append(s.str, s.len);
    }, csubstr{exc.msg, strlen(exc.msg)}, exc.errdata_visit);
}
/** Format a parse exception, and return a newly-created char
 * container
 *
 * @note Available only if @ref
 * RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS is defined, and @ref
 * RYML_NO_DEFAULT_CALLBACKS is NOT defined.  */
template<class CharContainer, class ExceptionT>
CharContainer format_exc(ExceptionT const& exc)
{
    CharContainer str;
    format_exc(&str, exc);
    return str;
}

#endif // _RYML_WITH_EXCEPTIONS

/** @} */


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/// @cond dev


#if RYML_USE_ASSERT
#   define _RYML_ASSERT_BASIC(cond) _RYML_CHECK_BASIC(cond)
#   define _RYML_ASSERT_BASIC_(cb, cond) _RYML_CHECK_BASIC_((cb), cond)
#   define _RYML_ASSERT_BASIC_MSG(cond, ...) _RYML_CHECK_BASIC_MSG(cond, __VA_ARGS__)
#   define _RYML_ASSERT_BASIC_MSG_(cb, cond, ...) _RYML_CHECK_BASIC_MSG_((cb), cond, __VA_ARGS__)
#   define _RYML_ASSERT_PARSE(cond, ymlloc) _RYML_CHECK_PARSE(cond, (ymlloc))
#   define _RYML_ASSERT_PARSE_(cb, cond, ymlloc) _RYML_CHECK_PARSE_((cb), cond, (ymlloc))
#   define _RYML_ASSERT_PARSE_MSG(cond, ymlloc, ...) _RYML_CHECK_PARSE_MSG(cond, (ymlloc), __VA_ARGS__)
#   define _RYML_ASSERT_PARSE_MSG_(cb, cond, ymlloc, ...) _RYML_CHECK_PARSE_MSG_((cb), cond, (ymlloc), __VA_ARGS__)
#   define _RYML_ASSERT_VISIT(cond, tree, node) _RYML_CHECK_VISIT(cond, (tree), (node))
#   define _RYML_ASSERT_VISIT_(cb, cond, tree, node) _RYML_CHECK_VISIT_((cb), cond, (tree), (node))
#   define _RYML_ASSERT_VISIT_MSG(cond, tree, node, ...) _RYML_CHECK_VISIT_MSG(cond, (tree), (node), __VA_ARGS__)
#   define _RYML_ASSERT_VISIT_MSG_(cb, cond, tree, node, ...) _RYML_CHECK_VISIT_MSG_((cb), cond, (tree), (node), __VA_ARGS__)
#else
#   define _RYML_ASSERT_BASIC(cond)
#   define _RYML_ASSERT_BASIC_(cb, cond)
#   define _RYML_ASSERT_BASIC_MSG(cond, ...)
#   define _RYML_ASSERT_BASIC_MSG_(cb, cond, ...)
#   define _RYML_ASSERT_PARSE(cond, ymlloc)
#   define _RYML_ASSERT_PARSE_(cb, cond, ymlloc)
#   define _RYML_ASSERT_PARSE_MSG(cond, ymlloc, ...)
#   define _RYML_ASSERT_PARSE_MSG_(cb, cond, ymlloc, ...)
#   define _RYML_ASSERT_VISIT(cont, tree, node)
#   define _RYML_ASSERT_VISIT_(cb, cont, tree, node)
#   define _RYML_ASSERT_VISIT_MSG(cont, tree, node, ...)
#   define _RYML_ASSERT_VISIT_MSG_(cb, cont, tree, node, ...)
#endif

#define _RYML_ERR_BASIC(...)                                            \
    do                                                                  \
    {                                                                   \
        RYML_DEBUG_BREAK();                                             \
        ::c4::yml::err_basic((::c4::yml::ErrorDataBasic{RYML_LOC_HERE()}), __VA_ARGS__); \
        C4_UNREACHABLE_AFTER_ERR();                                     \
    } while(false)
#define _RYML_ERR_PARSE(ymlloc, ...)                                    \
    do                                                                  \
    {                                                                   \
        RYML_DEBUG_BREAK();                                             \
        ::c4::yml::err_parse((::c4::yml::ErrorDataParse{RYML_LOC_HERE(), ymlloc}), __VA_ARGS__); \
        C4_UNREACHABLE_AFTER_ERR();                                     \
    } while(false)
#define _RYML_ERR_VISIT(tree, node, ...)                                \
    do                                                                  \
    {                                                                   \
        RYML_DEBUG_BREAK();                                             \
        ::c4::yml::err_visit((::c4::yml::ErrorDataVisit{RYML_LOC_HERE(), tree, node}), __VA_ARGS__); \
        C4_UNREACHABLE_AFTER_ERR();                                     \
    } while(false)


#define _RYML_ERR_BASIC_(cb, ...)                                       \
    do                                                                  \
    {                                                                   \
        RYML_DEBUG_BREAK();                                             \
        ::c4::yml::err_basic((cb), (::c4::yml::ErrorDataBasic{RYML_LOC_HERE()}), __VA_ARGS__); \
        C4_UNREACHABLE_AFTER_ERR();                                     \
    } while(false)
#define _RYML_ERR_PARSE_(cb, ymlloc, ...)                               \
    do                                                                  \
    {                                                                   \
        RYML_DEBUG_BREAK();                                             \
        ::c4::yml::err_parse((cb), (::c4::yml::ErrorDataParse{RYML_LOC_HERE(), ymlloc}), __VA_ARGS__); \
        C4_UNREACHABLE_AFTER_ERR();                                     \
    } while(false)
#define _RYML_ERR_VISIT_(cb, tree, node, ...)                           \
    do                                                                  \
    {                                                                   \
        RYML_DEBUG_BREAK();                                             \
        ::c4::yml::err_visit((cb), (::c4::yml::ErrorDataVisit{RYML_LOC_HERE(), tree, node}), __VA_ARGS__); \
        C4_UNREACHABLE_AFTER_ERR();                                     \
    } while(false)


#ifndef RYML_SHORT_CHECK_MSG
#define _RYML_MAYBE_MSG(cond) ": " #cond
#define _RYML_MAYBE_MSG_(cond) ": " #cond ": "
#else
#define _RYML_MAYBE_MSG(cond)
#define _RYML_MAYBE_MSG_(cond) ": "
#endif

#define _RYML_CHECK_BASIC(cond)                                         \
    do {                                                                \
        if(C4_UNLIKELY(!(cond)))                                        \
        {                                                               \
            RYML_DEBUG_BREAK();                                         \
            ::c4::yml::err_basic((::c4::yml::ErrorDataBasic{RYML_LOC_HERE()}), "check failed" _RYML_MAYBE_MSG(cond)); \
            C4_UNREACHABLE_AFTER_ERR();                                 \
        }                                                               \
    } while(false)
#define _RYML_CHECK_PARSE(cond, ymlloc)                                 \
    do {                                                                \
        if(C4_UNLIKELY(!(cond)))                                        \
        {                                                               \
            RYML_DEBUG_BREAK();                                         \
            ::c4::yml::err_parse((::c4::yml::ErrorDataParse{RYML_LOC_HERE(), ymlloc}), "check failed" _RYML_MAYBE_MSG(cond)); \
            C4_UNREACHABLE_AFTER_ERR();                                 \
        }                                                               \
    } while(false)
#define _RYML_CHECK_VISIT(cond, tree, node)                             \
    do {                                                                \
        if(C4_UNLIKELY(!(cond)))                                        \
        {                                                               \
            RYML_DEBUG_BREAK();                                         \
            ::c4::yml::err_visit((::c4::yml::ErrorDataVisit{RYML_LOC_HERE(), tree, node}), "check failed" _RYML_MAYBE_MSG(cond)); \
            C4_UNREACHABLE_AFTER_ERR();                                 \
        }                                                               \
    } while(false)


#define _RYML_CHECK_BASIC_(cb, cond)                                    \
    do {                                                                \
        if(C4_UNLIKELY(!(cond)))                                        \
        {                                                               \
            RYML_DEBUG_BREAK();                                         \
            ::c4::yml::err_basic((cb), (::c4::yml::ErrorDataBasic{RYML_LOC_HERE()}), "check failed" _RYML_MAYBE_MSG(cond)); \
            C4_UNREACHABLE_AFTER_ERR();                                 \
        }                                                               \
    } while(false)
#define _RYML_CHECK_PARSE_(cb, cond, ymlloc)                            \
    do {                                                                \
        if(C4_UNLIKELY(!(cond)))                                        \
        {                                                               \
            RYML_DEBUG_BREAK();                                         \
            ::c4::yml::err_parse((cb), (::c4::yml::ErrorDataParse{RYML_LOC_HERE(), ymlloc}), "check failed" _RYML_MAYBE_MSG(cond)); \
            C4_UNREACHABLE_AFTER_ERR();                                 \
        }                                                               \
    } while(false)
#define _RYML_CHECK_VISIT_(cb, cond, tree, node)                        \
    do {                                                                \
        if(C4_UNLIKELY(!(cond)))                                        \
        {                                                               \
            RYML_DEBUG_BREAK();                                         \
            ::c4::yml::err_visit((cb), (::c4::yml::ErrorDataVisit{RYML_LOC_HERE(), tree, node}), "check failed" _RYML_MAYBE_MSG(cond)); \
            C4_UNREACHABLE_AFTER_ERR();                                 \
        }                                                               \
    } while(false)


#define _RYML_CHECK_BASIC_MSG(cond, ...)                                \
    do {                                                                \
        if(C4_UNLIKELY(!(cond)))                                        \
        {                                                               \
            RYML_DEBUG_BREAK();                                         \
            ::c4::yml::err_basic((::c4::yml::ErrorDataBasic{RYML_LOC_HERE()}), "check failed" _RYML_MAYBE_MSG_(cond) __VA_ARGS__); \
            C4_UNREACHABLE_AFTER_ERR();                                 \
        }                                                               \
    } while(false)
#define _RYML_CHECK_PARSE_MSG(cond, ymlloc, ...)                        \
    do {                                                                \
        if(C4_UNLIKELY(!(cond)))                                        \
        {                                                               \
            RYML_DEBUG_BREAK();                                         \
            ::c4::yml::err_parse((::c4::yml::ErrorDataParse{RYML_LOC_HERE(), ymlloc}), "check failed" _RYML_MAYBE_MSG_(cond) __VA_ARGS__); \
            C4_UNREACHABLE_AFTER_ERR();                                 \
        }                                                               \
    } while(false)
#define _RYML_CHECK_VISIT_MSG(cond, tree, node, ...)                    \
    do {                                                                \
        if(C4_UNLIKELY(!(cond)))                                        \
        {                                                               \
            RYML_DEBUG_BREAK();                                         \
            ::c4::yml::err_visit((::c4::yml::ErrorDataVisit{RYML_LOC_HERE(), tree, node}), "check failed" _RYML_MAYBE_MSG_(cond) __VA_ARGS__); \
            C4_UNREACHABLE_AFTER_ERR();                                 \
        }                                                               \
    } while(false)


#define _RYML_CHECK_BASIC_MSG_(cb, cond, ...)                           \
    do {                                                                \
        if(C4_UNLIKELY(!(cond)))                                        \
        {                                                               \
            RYML_DEBUG_BREAK();                                         \
            ::c4::yml::err_basic((cb), (::c4::yml::ErrorDataBasic{RYML_LOC_HERE()}), "check failed" _RYML_MAYBE_MSG_(cond) __VA_ARGS__); \
            C4_UNREACHABLE_AFTER_ERR();                                 \
        }                                                               \
    } while(false)
#define _RYML_CHECK_PARSE_MSG_(cb, cond, ymlloc, ...)                   \
    do {                                                                \
        if(C4_UNLIKELY(!(cond)))                                        \
        {                                                               \
            RYML_DEBUG_BREAK();                                         \
            ::c4::yml::err_parse((cb), (::c4::yml::ErrorDataParse{RYML_LOC_HERE(), ymlloc}), "check failed" _RYML_MAYBE_MSG_(cond) __VA_ARGS__); \
            C4_UNREACHABLE_AFTER_ERR();                                 \
        }                                                               \
    } while(false)
#define _RYML_CHECK_VISIT_MSG_(cb, cond, tree, node, ...)               \
    do {                                                                \
        if(C4_UNLIKELY(!(cond)))                                        \
        {                                                               \
            RYML_DEBUG_BREAK();                                         \
            ::c4::yml::err_visit((cb), (::c4::yml::ErrorDataVisit{RYML_LOC_HERE(), tree, node}), "check failed" _RYML_MAYBE_MSG_(cond) __VA_ARGS__); \
            C4_UNREACHABLE_AFTER_ERR();                                 \
        }                                                               \
    } while(false)

/// @endcond

} // namespace yml
} // namespace c4

#endif /* _C4_YML_ERROR_HPP_ */
