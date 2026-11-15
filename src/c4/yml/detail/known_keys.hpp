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
        id_type     id;       /// id of the key in the tree/evts arr /// TODO: use 32bit
        count_type  unqcount; /// count of unique keys when this was added
        count_type  count;    /// count of other keys when this was added
        status_bits status;
    };

    using KnownKeysMap = small_map<csubstr, Entry, RYML_KNOWN_KEYS_SSO_SIZE, true>;
    using UniqueKeysMap = stack<csubstr>;

public:

    KnownKeysMap   m_known_keys;
    UniqueKeysMap  m_unique_keys;
    status_bits    m_all_entries;
    ResolveOptions m_opts;

    using KnownKeyRange_ = KnownKeysMap::const_range_type;

public:

    KnownKeys(Callbacks const& cb)
        : m_known_keys(cb)
        , m_unique_keys(cb)
        , m_all_entries()
        , m_opts()
    {}

    void reset(ResolveOptions opts_) noexcept { m_opts = opts_; }

public:

    void start_adding() noexcept
    {
        m_known_keys.clear();
        m_unique_keys.clear();
        m_all_entries = {};
    }

    void add(csubstr key, id_type id, status_bits status) noexcept;

    void adding_is_complete();

    // return the first conflicting key triggering an error according to opts
    Entry const* first_bad_key() const noexcept;

public:

    bool has_key_conflict() const noexcept
    {
        return (m_all_entries & CONFLICT);
    }

    bool requires_merge() const noexcept
    {
        return (m_all_entries & MERGEDIN) ||
            ((m_all_entries & CONFLICT) && !m_opts.keep_repeated_keys());
    }

    id_type num_unique_keys() const noexcept
    {
        return m_unique_keys.size();
    }

    Entry const* select(id_type unique_key, KnownKeyRange_ *rng=nullptr) const RYML_NOEXCEPT;
    Entry const* select(csubstr key, KnownKeyRange_ *rng=nullptr) const RYML_NOEXCEPT;
};

} // namespace detail
} // namespace yml
} // namespace c4

/** @endcond */

#endif /* C4_YML_DETAIL_KNOWN_KEYS_HPP_ */
