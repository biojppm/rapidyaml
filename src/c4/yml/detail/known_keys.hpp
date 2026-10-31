#ifndef C4_YML_DETAIL_KNOWN_KEYS_HPP_
#define C4_YML_DETAIL_KNOWN_KEYS_HPP_

#ifndef C4_YML_RESOLVE_OPTIONS_HPP_
#include "c4/yml/resolve_options.hpp"
#endif
#ifndef C4_YML_DETAIL_SMALL_MAP_HPP_
#include "c4/yml/detail/small_map.hpp"
#endif


namespace c4 {
namespace yml {
namespace detail {


#define prkv(kv) \
    printf("%.*s unqcount=%d count=%d id=%zu original=%d mergedin=%d conflict=%d", \
           static_cast<int>(kv.val.key.len), kv.val.key.str,            \
           kv.val.unqcount, kv.val.count, kv.val.id,                    \
           kv.val.status & yml::detail::KnownKeys::ORIGINAL, kv.val.status & yml::detail::KnownKeys::MERGEDIN, kv.val.status & yml::detail::KnownKeys::CONFLICT)
#define prv(v) \
    printf("unqcount=%d count=%d id=%zu original=%d mergedin=%d conflict=%d", \
           v.unqcount, v.count, v.id,                    \
           v.status & yml::detail::KnownKeys::ORIGINAL, v.status & yml::detail::KnownKeys::MERGEDIN, v.status & yml::detail::KnownKeys::CONFLICT)


struct KnownKeys
{
    using count_type = int32_t;
    using status_bits = uint32_t;

    typedef enum : status_bits {
        ORIGINAL = 1u, ///< key is from member of the target map
        MERGEDIN = 2u, ///< key is from member of an alias (merged-in) map
        CONFLICT = 4u, ///< there are other similar named keys
    } StatusBits_e;

    struct Entry
    {
        id_type     id;       /// id of the key in the tree/evts arr
        count_type  unqcount; /// count of unique keys when this was added
        count_type  count;    /// count of other keys when this was added
        status_bits status;
        csubstr     key; // FIXME remove
    };

    template<class K, class V> using map = yml::detail::small_map<K, V, 32>;

public:

    map<csubstr, Entry> known_keys;
    map<count_type, csubstr> unique_keys;
    status_bits all_entries;
    ResolveOptions opts;

    using KnownKeyRange_ = decltype(known_keys)::range_type;

public:

    KnownKeys(Callbacks const& cb) noexcept : known_keys(cb), unique_keys(cb), opts() {}

    void reset(ResolveOptions opts_) noexcept { opts = opts_; }

public:

    void start_adding() noexcept
    {
        known_keys.clear();
        unique_keys.clear();
        all_entries = {};
    }

    void add(csubstr key, id_type id, status_bits status) noexcept;

    bool adding_is_complete()
    {
        bool ret = requires_merge();
        if(ret)
        {
            unique_keys.sort();
            known_keys.sort();
        }
int kcount = 0;
for(auto const& kv : known_keys.m_arr)
{
    printf("... key[%d]: ", kcount++);
    prkv(kv);
    printf("\n");
}
printf("... has_merge_elm=%d key_conflict: %d\n", (all_entries & MERGEDIN), (all_entries & CONFLICT));
        return ret;
    }

public:

    bool has_key_conflict() const noexcept
    {
        return (all_entries & CONFLICT);
    }

    bool requires_merge() const noexcept
    {
        return (all_entries & MERGEDIN) || ((all_entries & CONFLICT) && opts.merge_repeated_keys());
    }

    id_type num_unique_keys() const noexcept
    {
        return unique_keys.m_arr.size();
    }

    Entry const* select(id_type unique_key, KnownKeyRange_ *rng=nullptr) const RYML_NOEXCEPT;
    Entry const* select(csubstr key, KnownKeyRange_ *rng=nullptr) const RYML_NOEXCEPT;

    bool exists_with_conflict(csubstr key) const noexcept
    {
        Entry const* existing = known_keys.lookup(key);
        return existing && (existing->status & CONFLICT);
    }

};

} // namespace detail
} // namespace yml
} // namespace c4

#endif /* C4_YML_DETAIL_KNOWN_KEYS_HPP_ */
