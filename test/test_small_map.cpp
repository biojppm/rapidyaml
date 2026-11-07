#ifdef RYML_SINGLE_HEADER
#include "ryml_all.hpp"
#else
#include "c4/yml/detail/small_map.hpp"
#endif
#include "test_lib/test_utils.hpp"

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {
namespace detail {

template<class K, class V>
C4_ALWAYS_INLINE bool operator<= (small_map_entry<K, V> const& lhs, small_map_entry<K, V> const& rhs) noexcept
{
    return lhs.key <= rhs.key;
}

template<class K, class V>
void print(small_map<K,V> const& map, size_t num_per_line=10)
{
    for(size_t i = 0; i < map.m_arr.size(); ++i)
    {
        if(!(i % num_per_line))
        {
            if(i)
                std::cout << "\n";
            std::cout << "[" << i << "]  ";
        }
        auto const& entry = map.m_arr[i];
        std::cout << " (" << entry.key << "->" << entry.val << ")";
    }
    std::cout << "\n";
}


template<class K, class V>
void test_sorted(small_map<K,V> const& map)
{
    if(map.m_arr.size() > map.m_small_limit)
    {
        for(size_t i = 1, sz = map.m_arr.size(); i < sz; ++i)
        {
            if(!(map.m_arr[i-1] <= map.m_arr[i]))
            {
                RYML_TRACE_FMT("i={}", i);
                ASSERT_LE(map.m_arr[i-1], map.m_arr[i]);
            }
        }
        ASSERT_EQ(map.m_sorted_size, map.m_arr.size());
    }
}


template<class K>
void test_basic(size_t num, size_t den)
{
    small_map<K, K> map;
    size_t num_elms = map.small_limit_default * num / den;
    for(size_t i = 0; i < num_elms; ++i)
    {
        K val = static_cast<K>(num_elms - i);
        map.insert(val) = val;
    }
    if(num_elms > map.m_small_limit)
        map.sort();
    SCOPED_CALL(test_sorted, (map));
    for(size_t i = 0; i < num_elms; ++i)
    {
        {
            K val = 10 * static_cast<K>(num_elms + i);
            ASSERT_EQ(map.lookup(val), nullptr);
        }
        {
            K val = static_cast<K>(num_elms - i);
            K const* out = map.lookup(val);
            ASSERT_NE(out, nullptr);
            EXPECT_EQ(*out, val);
        }
    }
    if(testing::Test::HasFailure())
    {
        print(map);
    }
}


TEST(small_map, small_mode)
{
    SCOPED_CALL(test_basic<int>, (1, 1));
    SCOPED_CALL(test_basic<int>, (1, 2));
}

TEST(small_map, large_mode)
{
    SCOPED_CALL(test_basic<int>, (2, 1));
}


} // namespace detail
} // namespace yml
} // namespace c4
