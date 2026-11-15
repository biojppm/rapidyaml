#ifndef C4_YML_DETAIL_SMALL_MAP_HPP_
#define C4_YML_DETAIL_SMALL_MAP_HPP_

#ifndef C4_YML_DETAIL_STACK_HPP_
#include "c4/yml/detail/stack.hpp"
#endif
#ifndef C4_YML_DETAIL_LOWER_BOUND_HPP_
#include "c4/yml/detail/lower_bound.hpp"
#endif


/** @cond dev */

namespace c4 {
namespace yml {
namespace detail {

// this header provides a feature-limited small_map implementation.
// it purposely avoids using std headers to avoid the heavy machinery.


template<class K, class V>
struct small_map_entry
{
    K key;
    V val;
};
template<class K, class V>
C4_ALWAYS_INLINE bool operator< (small_map_entry<K, V> const& e, K const& key) noexcept
{
    return (e.key) < (key);  // parentheses needed by gcc 4.8
}
template<class K, class V>
C4_ALWAYS_INLINE bool operator< (small_map_entry<K, V> const& lhs,
                                 small_map_entry<K, V> const& rhs) noexcept
{
    return (lhs.key) < (rhs.key);  // parentheses needed by gcc 4.8
}


// We cannot use std::stable_sort, because it allocates and
// deallocates (!). We don't even use std::sort() to avoid the heavy
// <algorithm> include.
//
// So, to achieve in-place stable sort, we use this custom value_type,
// which has a member with the original position to be used as
// secondary sorting criterion. This costs an extra integer. Ideally,
// we should either:
//
//  - implement an in-place stable sort (see, eg
// https://en.wikipedia.org/wiki/Block_sort and
// https://en.wikipedia.org/wiki/Sorting_algorithm )
//
//  - refactor the calling algorithm to do secondary sorts on
//  - equal_ranges.
template<class K, class V, class I=id_type>
struct small_map_entry_stable
{
    K key;
    V val;
    I pos;
};
template<class K, class V>
C4_ALWAYS_INLINE bool operator< (small_map_entry_stable<K, V> const& e, K const& key) noexcept
{
    return (e.key) < (key);  // parentheses needed by gcc 4.8
}
template<class K, class V>
C4_ALWAYS_INLINE bool operator< (small_map_entry_stable<K, V> const& lhs,
                                 small_map_entry_stable<K, V> const& rhs) noexcept
{
    // parentheses needed by gcc 4.8
    return ((lhs.key) < (rhs.key))
        || (((lhs.key) == (rhs.key)) && (lhs.pos < rhs.pos));
}


// we use heapsort to order the map.
template<class T, class I>
void heap_sort(T *C4_RESTRICT arr, I count) noexcept
{
    static_assert(std::is_trivially_copyable<T>::value, "must be trivial");
    #define rymlswap_(i, j) { tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp; }
    // https://en.wikipedia.org/wiki/Heapsort
    T tmp;
    I start = count / 2;
    while(count > 1)
    {
        if(start)  // heap construction
        {
            --start;
        }
        else  // heap extraction
        {
            --count;
            rymlswap_(0, count);
        }
        // sift down
        I root = start;
        I child = (2 * root) + 1; // left child
        while(child < count)
        {
            // bump if there is a right child and it is greater
            child += (((child + 1) < count) && (arr[child] < arr[child + 1]));
            if(arr[root] < arr[child])
            {
                rymlswap_(root, child)
                root = child; // repeat to continue sifting down the child now
            }
            else
            {
                break; // return to outer loop
            }
            child = (2 * root) + 1; // left child
        }
    }
    #undef rymlswap_
}


// a range type to use in small map
template<class T>
struct small_map_range
{
    T lo, hi;
    C4_ALWAYS_INLINE T begin() const noexcept { return lo; }
    C4_ALWAYS_INLINE T end() const noexcept { return hi; }
};


template<class K, class V, id_type SSO_SIZE, bool stable=false>
struct small_map
{
public:

    enum : size_t { // NOLINT
        sso_size = SSO_SIZE,
        small_limit_default = RYML_SMALL_MAP_LINEAR_THRESHOLD_DEFAULT,
    };

    using key_type = K;
    using mapped_type = V;
    using value_type = typename std::conditional<
        stable,
        small_map_entry_stable<K, V, id_type>,
        small_map_entry<K, V>
        >::type;
    using range_type = small_map_range<value_type *>;
    using const_range_type = small_map_range<value_type const*>;

public:

    id_type m_small_limit;
    id_type m_sorted_size;
    yml::detail::stack<value_type, SSO_SIZE> m_arr = {};

public:

    small_map() : small_map(get_callbacks()) {}
    small_map(Callbacks const& cb) // NOLINT
        : m_small_limit(small_limit_default)
        , m_sorted_size()
        , m_arr(cb)
    {}

    void clear()
    {
        m_arr.clear();
        m_sorted_size = 0;
    }

public:

    #if C4_CPP >= 17
    C4_ALWAYS_INLINE V& insert(K const& key)
    {
        if C4_IF_CONSTEXPR (stable)
        {
            id_type pos = m_arr.size();
            value_type &e = m_arr.emplace_top();
            e.key = key;
            e.pos = pos;
            return e.val;
        }
        else
        {
            value_type &e = m_arr.emplace_top();
            e.key = key;
            return e.val;
        }
    }
    #else
    template<bool S=stable>
    C4_ALWAYS_INLINE auto insert(K const& key)
        -> typename std::enable_if<S, V&>::type
    {
        id_type pos = m_arr.size();
        value_type &e = m_arr.emplace_top();
        e.key = key;
        e.pos = pos;
        return e.val;
    }
    template<bool S=stable>
    C4_ALWAYS_INLINE auto insert(K const& key)
        -> typename std::enable_if<!S, V&>::type
    {
        value_type &e = m_arr.emplace_top();
        e.key = key;
        return e.val;
    }
    #endif


    void sort() noexcept
    {
        heap_sort(m_arr.begin(), m_arr.m_size);
        m_sorted_size = m_arr.m_size;
    }

    void sort_if_needed() noexcept
    {
        if((m_arr.size() > m_small_limit) && (m_arr.size() > m_sorted_size))
            sort();
    }

public:

    C4_ALWAYS_INLINE V const* lookup(K const& key) const noexcept
    {
        return (m_arr.size() <= m_small_limit) ? lookup_linear_(key) : lookup_binary_(key);
    }

    const_range_type equal_range(K const& key) const noexcept
    {
        RYML_ASSERT_BASIC_CB_(m_arr.m_callbacks, m_sorted_size == m_arr.size());
        small_map_range<value_type const*> r;
        auto const *end = m_arr.end();
        r.hi = r.lo = detail::lower_bound(m_arr.begin(), end, key);
        if(r.lo != end)
        {
            RYML_ASSERT_BASIC_CB_(m_arr.m_callbacks, r.lo->key == key || !lookup(key));
            while(r.hi != end && r.hi->key == key)
                ++r.hi;
        }
        return r;
    }

    range_type equal_range_mut(K const& key) noexcept
    {
        const_range_type rng = equal_range(key);
        auto *beg = m_arr.begin();
        range_type ret;
        ret.lo = beg + (rng.lo - beg);
        ret.hi = beg + (rng.hi - beg);
        return ret;
    }

private:

    V const* lookup_linear_(K const& key) const noexcept
    {
        for(auto& e : m_arr)
            if(e.key == key)
                return &e.val;
        return nullptr;
    }
    V const* lookup_binary_(K const& key) const noexcept
    {
        RYML_ASSERT_BASIC_CB_(m_arr.m_callbacks, m_arr.size() == m_sorted_size);
        value_type const* bound = detail::lower_bound(m_arr.begin(), m_arr.end(), key);
        return (bound != m_arr.end() && bound->key == key) ? &bound->val : nullptr;
    }
};

} // namespace detail
} // namespace yml
} // namespace c4

/** @endcond */

#endif /* C4_YML_DETAIL_SMALL_MAP_HPP_ */
