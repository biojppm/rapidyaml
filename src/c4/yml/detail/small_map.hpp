#ifndef C4_YML_DETAIL_SMALL_MAP_HPP_
#define C4_YML_DETAIL_SMALL_MAP_HPP_


#ifndef C4_YML_DETAIL_LOWER_BOUND_HPP_
#include "c4/yml/detail/lower_bound.hpp"
#endif
#ifndef C4_YML_DETAIL_STACK_HPP_
#include "c4/yml/detail/stack.hpp"
#endif


namespace c4 {
namespace yml {
namespace detail {


template<class K, class V>
struct small_map_entry
{
    K key;
    V val;
};
template<class K, class V>
C4_ALWAYS_INLINE bool operator< (small_map_entry<K, V> const& e, K const& key) noexcept
{
    return e.key < key;
}
template<class K, class V>
C4_ALWAYS_INLINE bool operator< (small_map_entry<K, V> const& lhs,
                                 small_map_entry<K, V> const& rhs) noexcept
{
    return lhs.key < rhs.key;
}


template<class T>
struct small_map_range
{
    T lo, hi;
};


template<class K, class V, size_t SSO_SIZE=128>
struct small_map
{
public:

    enum : size_t {
        sso_size = SSO_SIZE,
        small_limit_default = 64,
    };

    using value_type = small_map_entry<K, V>;
    using range_type = small_map_range<value_type const*>;

public:

    yml::detail::stack<value_type, SSO_SIZE> m_arr = {};
    size_t m_small_limit = small_limit_default;
    size_t m_sorted_size = 0;

public:

    small_map(Callbacks const& cb={}) : m_arr(cb) {}

    void clear()
    {
        m_arr.clear();
        m_sorted_size = 0;
    }

    C4_ALWAYS_INLINE V& insert(K const& key)
    {
        value_type &e = m_arr.emplace_top();
        e.key = key;
        return e.val;
    }

    void sort() noexcept
    {
        if((m_arr.size() > m_small_limit) &&
           (m_arr.size() > m_sorted_size))
        {
            std::stable_sort(m_arr.begin(), m_arr.end());
            m_sorted_size = m_arr.size();
        }
    }

public:

    C4_ALWAYS_INLINE V const* lookup(K const& key) const noexcept
    {
        return m_arr.size() <= m_small_limit ? lookup_linear_(key) : lookup_binary_(key);
    }

    C4_ALWAYS_INLINE V * lookup_modify(K const& key) noexcept
    {
        return m_arr.size() <= m_small_limit ? lookup_linear_(key) : lookup_binary_(key);
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
        return bound->key == key ? &bound->val : nullptr;
    }

    V* lookup_linear_(K const& key) noexcept
    {
        return const_cast<V*>(const_cast<small_map const*>(this)->lookup_linear_(key));
    }
    V* lookup_binary_(K const& key) noexcept
    {
        return const_cast<V*>(const_cast<small_map const*>(this)->lookup_binary_(key));
    }
};

} // namespace detail
} // namespace yml
} // namespace c4

#endif /* C4_YML_DETAIL_SMALL_MAP_HPP_ */
