#ifndef _C4_BLOB_HPP_
#define _C4_BLOB_HPP_

#include "c4/error.hpp"

/** @file blob.hpp Mutable and immutable binary data blobs.
*/

namespace c4 {

template<class T>
struct blob_;

namespace detail {
template<class T> struct is_blob_type : std::integral_constant<bool, false> {};
template<class T> struct is_blob_type<blob_<T>> : std::integral_constant<bool, true> {};
template<class T> struct is_blob_value_type : std::integral_constant<bool, (std::is_fundamental<T>::value || std::is_trivially_copyable<T>::value)> {};
} // namespace

// NOLINTBEGIN(cppcoreguidelines-special-member-functions,hicpp-special-member-functions)

template<class T>
struct blob_
{
    static_assert(std::is_same<T, char>::value || std::is_same<T, const char>::value, "must be either char or const char");
    static_assert(sizeof(T) == 1u, "must be either char or const char");

public:

    T *    buf;
    size_t len;

public:

    C4_ALWAYS_INLINE blob_() noexcept = default;
    C4_ALWAYS_INLINE blob_(blob_ const& that) noexcept = default;
    C4_ALWAYS_INLINE blob_(blob_     && that) noexcept = default;
    C4_ALWAYS_INLINE blob_& operator=(blob_     && that) noexcept = default;
    C4_ALWAYS_INLINE blob_& operator=(blob_ const& that) noexcept = default;

    template<class U, class=typename std::enable_if<std::is_const<T>::value && std::is_same<typename std::add_const<U>::type, T>::value, U>::type> C4_ALWAYS_INLINE blob_(blob_<U> const& that) noexcept : buf(that.buf), len(that.len) {} // NOLINT
    template<class U, class=typename std::enable_if<std::is_const<T>::value && std::is_same<typename std::add_const<U>::type, T>::value, U>::type> C4_ALWAYS_INLINE blob_(blob_<U>     && that) noexcept : buf(that.buf), len(that.len) {} // NOLINT
    template<class U, class=typename std::enable_if<std::is_const<T>::value && std::is_same<typename std::add_const<U>::type, T>::value, U>::type> C4_ALWAYS_INLINE blob_& operator=(blob_<U>     && that) noexcept { buf = that.buf; len = that.len; } // NOLINT
    template<class U, class=typename std::enable_if<std::is_const<T>::value && std::is_same<typename std::add_const<U>::type, T>::value, U>::type> C4_ALWAYS_INLINE blob_& operator=(blob_<U> const& that) noexcept { buf = that.buf; len = that.len; } // NOLINT

    C4_ALWAYS_INLINE blob_(void       *ptr, size_t n) noexcept : buf(reinterpret_cast<T*>(ptr)), len(n) {} // NOLINT
    C4_ALWAYS_INLINE blob_(void const *ptr, size_t n) noexcept : buf(reinterpret_cast<T*>(ptr)), len(n) {} // NOLINT

    #define _C4_REQUIRE_BLOBTYPE(ty) class=typename std::enable_if<((!detail::is_blob_type<ty>::value) && (detail::is_blob_value_type<ty>::value)), T>::type
    template<class U, _C4_REQUIRE_BLOBTYPE(U)> C4_ALWAYS_INLINE blob_(U &var) noexcept : buf(reinterpret_cast<T*>(&var)), len(sizeof(U)) {} // NOLINT
    template<class U, _C4_REQUIRE_BLOBTYPE(U)> C4_ALWAYS_INLINE blob_(U *ptr, size_t n) noexcept : buf(reinterpret_cast<T*>(ptr)), len(sizeof(U) * n) { C4_ASSERT(is_aligned(ptr)); } // NOLINT
    template<class U, _C4_REQUIRE_BLOBTYPE(U)> C4_ALWAYS_INLINE blob_& operator= (U &var) noexcept { buf = reinterpret_cast<T*>(&var); len = sizeof(U); return *this; } // NOLINT
    template<class U, size_t N, _C4_REQUIRE_BLOBTYPE(U)> C4_ALWAYS_INLINE blob_(U (&arr)[N]) noexcept : buf(reinterpret_cast<T*>(arr)), len(sizeof(U) * N) {} // NOLINT
    template<class U, size_t N, _C4_REQUIRE_BLOBTYPE(U)> C4_ALWAYS_INLINE blob_& operator= (U (&arr)[N]) noexcept { buf = reinterpret_cast<T*>(arr); len = sizeof(U) * N; return *this; } // NOLINT
    #undef _C4_REQUIRE_BLOBTYPE
};

// NOLINTEND(cppcoreguidelines-special-member-functions,hicpp-special-member-functions)

/** an immutable binary blob */
using cblob = blob_<const char>;
/** a mutable binary blob */
using  blob = blob_<char>;

static_assert(std::is_trivially_copyable<blob>::value, "must be trivially copyable");
static_assert(std::is_trivially_copyable<cblob>::value, "must be trivially copyable");

} // namespace c4

#endif // _C4_BLOB_HPP_
