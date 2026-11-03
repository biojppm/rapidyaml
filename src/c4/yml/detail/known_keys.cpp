#ifndef C4_YML_DETAIL_KNOWN_KEYS_HPP_
#include "c4/yml/detail/known_keys.hpp"
#endif

namespace c4 {
namespace yml {
namespace detail {

namespace {
#ifdef RYML_USE_ASSERT
bool check_(KnownKeys const& this_, KnownKeys::KnownKeyRange_ r)
{
    #define docheck(cond) RYML_ASSERT_BASIC_(cond); if(!(cond)) return false
    docheck(r.hi);
    docheck(r.lo);
    KnownKeys::count_type num = static_cast<KnownKeys::count_type>(r.hi - r.lo);
    docheck(num);
    KnownKeys::count_type count = 0;
    bool several = num > 1;
    auto const* prev = r.lo;
    for(auto const& kv : r)
    {
        docheck(this_.unique_keys.lookup(kv.val.unqcount));
        docheck(kv.val.key == *this_.unique_keys.lookup(kv.val.unqcount));
        docheck(!!(kv.val.status & KnownKeys::CONFLICT) == several);
        if(count++)
        {
            docheck(kv.key == prev->key);
            docheck(kv.val.unqcount == prev->val.unqcount);
            ++prev;
        }
    }
    return true;
    #undef docheck
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
} // namespace


void KnownKeys::add(csubstr key, id_type id, status_bits status) noexcept
{
    count_type count = static_cast<count_type>(known_keys.m_arr.size());
    count_type unq = static_cast<count_type>(unique_keys.m_arr.size());
    Entry *existing = known_keys.lookup_modify(key);
    if( ! existing)
    {
        unique_keys.insert(unq) = key;
    }
    else
    {
        status |= CONFLICT;
        existing->status |= CONFLICT;
        unq = existing->unqcount;
    }
    Entry &entry = known_keys.insert(key);
    entry.id = id;
    entry.count = count;
    entry.unqcount = unq;
    entry.status = status;
    entry.key = key;
    all_entries |= status;
}

auto KnownKeys::select(csubstr key, KnownKeyRange_ *out) const RYML_NOEXCEPT
    -> Entry const*
{
    KnownKeyRange_ rng = known_keys.equal_range(key);
    RYML_ASSERT_BASIC_(check_(*this, rng));
    Entry const* selected = select_(rng, opts);
    if(out)
        *out = rng;
    return selected;
}

auto KnownKeys::select(id_type unique_key, KnownKeyRange_ *rng) const RYML_NOEXCEPT
    -> Entry const*
{
    RYML_ASSERT_BASIC_(unique_key < unique_keys.m_arr.size());
    csubstr key = unique_keys.m_arr[unique_key].val;
    return select(key, rng);
}

auto KnownKeys::adding_is_complete()
    -> Entry const*
{
    if(!requires_merge())
        return nullptr;
    unique_keys.sort();
    known_keys.sort();
    Entry const* first_key_causing_err = nullptr;
    for(auto const& uk : unique_keys.m_arr)
    {
        count_type noriginal = 0;
        count_type nmergedin = 0;
        for(auto const& kk : known_keys.equal_range(uk.val))
        {
            if(kk.val.status & CONFLICT)
            {
                noriginal += !!(kk.val.status & ORIGINAL);
                nmergedin += !!(kk.val.status & MERGEDIN);
                if(((noriginal > 1) && opts.err_on_repeated_keys_original()) ||
                   ((nmergedin > 1 || (nmergedin && noriginal)) && opts.err_on_repeated_keys_mergedin()))
                {
                    first_key_causing_err = &kk.val;
                    break;
                }
            }
        }
    }
    return first_key_causing_err;
}

} // namespace detail
} // namespace yml
} // namespace c4
