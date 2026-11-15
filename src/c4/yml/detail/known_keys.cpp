#ifndef C4_YML_DETAIL_KNOWN_KEYS_HPP_
#include "c4/yml/detail/known_keys.hpp"
#endif

namespace c4 {
namespace yml {
namespace detail {

namespace {
#if RYML_USE_ASSERT
bool check_rng_(KnownKeys const& this_, KnownKeys::KnownKeyRange_ r)
{
    C4_SUPPRESS_WARNING_GCC_WITH_PUSH("-Wuseless-cast")
    #define ryml_docheck_(cond) RYML_ASSERT_BASIC_(cond); if(!(cond)) return false
    ryml_docheck_(r.hi);
    ryml_docheck_(r.lo);
    KnownKeys::count_type num = static_cast<KnownKeys::count_type>(r.hi - r.lo);
    ryml_docheck_(num);
    KnownKeys::count_type count = 0;
    bool several = num > 1;
    auto const* prev = r.lo;
    for(auto const& kv : r)
    {
        ryml_docheck_(static_cast<id_type>(kv.val.unqcount) < this_.m_unique_keys.size());
        ryml_docheck_(kv.key == this_.m_unique_keys[static_cast<id_type>(kv.val.unqcount)]);
        ryml_docheck_(!!(kv.val.status & KnownKeys::CONFLICT) == several);
        if(count++)
        {
            ryml_docheck_(kv.key == prev->key);
            ryml_docheck_(kv.val.unqcount == prev->val.unqcount);
            ++prev;
        }
    }
    return true;
    #undef ryml_docheck_
    C4_SUPPRESS_WARNING_GCC_POP
}
#endif // RYML_USE_ASSERT

KnownKeys::Entry const* select_prio_(
    KnownKeys::Entry const* orig,
    KnownKeys::Entry const* rest,
    ResolveOptions opts) RYML_NOEXCEPT
{
    RYML_ASSERT_BASIC_(orig || rest);
    bool prio_orig = opts.priority_original();
    bool prio_rest = opts.priority_mergedin();
    if(prio_orig != prio_rest)
    {
        if(prio_orig)
            return orig ? orig : rest;
        else
            return rest ? rest : orig;
    }
    else if(rest && orig) // 2 candidates, same prio
    {
        RYML_ASSERT_BASIC_(orig->count != rest->count);
        if(opts.pick_last())
            return orig->count > rest->count ? orig : rest;
        else
            return orig->count < rest->count ? orig : rest;
    }
    return orig ? orig : rest;
}

KnownKeys::Entry const* select_earliest_(KnownKeys::KnownKeyRange_ r, ResolveOptions opts) RYML_NOEXCEPT
{
    static_assert(std::is_same<KnownKeys::count_type,decltype(INT_MAX)>::value, "count_type vs INT_MAX");
    KnownKeys::count_type minpos_rest = INT_MAX;
    KnownKeys::count_type minpos_orig = INT_MAX;
    KnownKeys::Entry const* which_rest = nullptr;
    KnownKeys::Entry const* which_orig = nullptr;
    for(auto const& kv : r)
    {
        if(kv.val.status & KnownKeys::ORIGINAL)
        {
            if(kv.val.count < minpos_orig)
            {
                minpos_orig = kv.val.count;
                which_orig = &kv.val;
            }
        }
        else
        {
            if(kv.val.count < minpos_rest)
            {
                minpos_rest = kv.val.count;
                which_rest = &kv.val;
            }
        }
    }
    RYML_ASSERT_BASIC_(which_orig || which_rest);
    RYML_ASSERT_BASIC_(!which_orig || which_orig >= &r.lo->val);
    RYML_ASSERT_BASIC_(!which_orig || which_orig < &r.hi->val);
    RYML_ASSERT_BASIC_(!which_rest || which_rest >= &r.lo->val);
    RYML_ASSERT_BASIC_(!which_rest || which_rest < &r.hi->val);
    return select_prio_(which_orig, which_rest, opts);
}

KnownKeys::Entry const* select_latest_(KnownKeys::KnownKeyRange_ r, ResolveOptions opts) RYML_NOEXCEPT
{
    KnownKeys::count_type maxpos_rest = -1;
    KnownKeys::count_type maxpos_orig = -1;
    KnownKeys::Entry const* which_rest = nullptr;
    KnownKeys::Entry const* which_orig = nullptr;
    for(auto const& kv : r)
    {
        if(kv.val.status & KnownKeys::ORIGINAL)
        {
            if(kv.val.count > maxpos_orig)
            {
                maxpos_orig = kv.val.count;
                which_orig = &kv.val;
            }
        }
        else
        {
            if(kv.val.count > maxpos_rest)
            {
                maxpos_rest = kv.val.count;
                which_rest = &kv.val;
            }
        }
    }
    RYML_ASSERT_BASIC_(which_orig || which_rest);
    RYML_ASSERT_BASIC_(!which_orig || which_orig >= &r.lo->val);
    RYML_ASSERT_BASIC_(!which_orig || which_orig < &r.hi->val);
    RYML_ASSERT_BASIC_(!which_rest || which_rest >= &r.lo->val);
    RYML_ASSERT_BASIC_(!which_rest || which_rest < &r.hi->val);
    return select_prio_(which_orig, which_rest, opts);
}

KnownKeys::Entry const* select_(KnownKeys::KnownKeyRange_ r, ResolveOptions opts) RYML_NOEXCEPT
{
    return opts.pick_last() ? select_latest_(r, opts) : select_earliest_(r, opts);
}

enum : KnownKeys::count_type { NOCOUNT = -1 }; // NOLINT
} // namespace


//-----------------------------------------------------------------------------

void KnownKeys::add(csubstr key, id_type id, status_bits status) noexcept
{
    count_type count = static_cast<count_type>(m_known_keys.m_arr.size());
    Entry &entry = m_known_keys.insert(key);
    entry.id = id;
    entry.count = count;
    entry.unqcount = NOCOUNT;
    entry.status = status;
    m_unique_keys.emplace_top() = key;
    m_all_entries |= status;
}


auto KnownKeys::select(csubstr key, KnownKeyRange_ *out) const RYML_NOEXCEPT
    -> Entry const*
{
    KnownKeyRange_ rng = m_known_keys.equal_range(key);
    RYML_ASSERT_BASIC_(check_rng_(*this, rng));
    Entry const* selected = select_(rng, m_opts);
    if(out)
        *out = rng;
    return selected;
}


auto KnownKeys::select(id_type unique_key, KnownKeyRange_ *rng) const RYML_NOEXCEPT
    -> Entry const*
{
    RYML_ASSERT_BASIC_(unique_key < m_unique_keys.size());
    csubstr key = m_unique_keys[unique_key];
    return select(key, rng);
}


void KnownKeys::adding_is_complete()
{
    if(m_known_keys.m_arr.empty())
        return;
    RYML_ASSERT_BASIC_(!m_unique_keys.empty());
    m_known_keys.sort();
    // finalize:
    //   - remove repeats from unique_keys
    //   - mark CONFLICT in repeated entries
    //   - set the unique count in all entries
    count_type unqcount = 0;
    for(auto const& uk : m_unique_keys)
    {
        auto const rng = m_known_keys.equal_range_mut(uk);
        RYML_ASSERT_BASIC_(rng.hi - rng.lo > 0);
        const status_bits status = (rng.hi - rng.lo == 1) ? status_bits{} : CONFLICT;
        count_type isnew = false;
        for(auto & knownkey : rng)
        {
            // have we visited this range before?
            if(knownkey.val.unqcount != NOCOUNT)
                break;
            RYML_ASSERT_BASIC_(uk == knownkey.key);
            // it's the first time we see the range,
            // so update all the entries in it
            knownkey.val.unqcount = unqcount;
            knownkey.val.status |= status;
            // write once the unqcount value
            if(!isnew)
            {
                m_unique_keys[static_cast<id_type>(unqcount)] = uk;
                m_all_entries |= status;
                isnew = true;
            }
        }
        unqcount += isnew;
    }
    m_unique_keys.resize(static_cast<id_type>(unqcount));
}


auto KnownKeys::first_bad_key() const noexcept
    -> Entry const*
{
    for(csubstr const& uk : m_unique_keys)
    {
        count_type noriginal = 0;
        count_type nmergedin = 0;
        for(auto const& kk : m_known_keys.equal_range(uk))
        {
            if(kk.val.status & CONFLICT)
            {
                noriginal += !!(kk.val.status & ORIGINAL);
                nmergedin += !!(kk.val.status & MERGEDIN);
                if(((noriginal > 1) && m_opts.err_on_repeated_keys_original()) ||
                   ((nmergedin > 1 || (nmergedin && noriginal)) && m_opts.err_on_repeated_keys_mergedin()))
                {
                    return &kk.val;
                }
            }
        }
    }
    return nullptr;
}

} // namespace detail
} // namespace yml
} // namespace c4
