#ifndef C4_YML_DETAIL_KNOWN_KEYS_HPP_
#define C4_YML_DETAIL_KNOWN_KEYS_HPP_

#ifndef C4_YML_RESOLVE_OPTIONS_HPP_
#include "c4/yml/resolve_options.hpp"
#endif
#ifndef C4_YML_DETAIL_SMALL_MAP_HPP_
#include "c4/yml/detail/small_map.hpp"
#endif

#ifndef RYML_KNOWN_KEYS_SSO_SIZE
#define RYML_KNOWN_KEYS_SSO_SIZE (32)
#endif

/** @cond dev */

namespace c4 {
namespace yml {
namespace detail {

struct KnownKeys
{
    using count_type = int32_t;
    using status_bits = uint32_t;

    typedef enum : status_bits { // NOLINT
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
    };

    template<class K, class V>
    using map = yml::detail::small_map<K, V, RYML_KNOWN_KEYS_SSO_SIZE>;

public:

    map<csubstr, Entry> known_keys;
    map<count_type, csubstr> unique_keys;
    status_bits all_entries;
    ResolveOptions opts;

    using KnownKeyRange_ = decltype(known_keys)::range_type;

public:

    KnownKeys(Callbacks const& cb)
        : known_keys(cb)
        , unique_keys(cb)
        , all_entries()
        , opts()
    {}

    void reset(ResolveOptions opts_) noexcept { opts = opts_; }

public:

    void start_adding() noexcept
    {
        known_keys.clear();
        unique_keys.clear();
        all_entries = {};
    }

    void add(csubstr key, id_type id, status_bits status) noexcept;

    // return the first conflicting key triggering an error according to opts
    Entry const* adding_is_complete();

public:

    bool has_key_conflict() const noexcept
    {
        return (all_entries & CONFLICT);
    }

    bool requires_merge() const noexcept
    {
        return (all_entries & MERGEDIN) ||
            ((all_entries & CONFLICT) && !opts.keep_repeated_keys());
    }

    id_type num_unique_keys() const noexcept
    {
        return unique_keys.m_arr.size();
    }

    Entry const* select(id_type unique_key, KnownKeyRange_ *rng=nullptr) const RYML_NOEXCEPT;
    Entry const* select(csubstr key, KnownKeyRange_ *rng=nullptr) const RYML_NOEXCEPT;
};

} // namespace detail
} // namespace yml
} // namespace c4

/** @endcond */

#endif /* C4_YML_DETAIL_KNOWN_KEYS_HPP_ */
