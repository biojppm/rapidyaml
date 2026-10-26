#ifndef C4_YML_EXTRA_INTS_REFERENCE_RESOLVER_HPP_
#include "c4/yml/extra/ints_reference_resolver.hpp"
#endif

#define C4_YML_EXTRA_INTS_REFERENCE_RESOLVER_HPP_

#ifndef C4_YML_EXTRA_EVENT_INTS_HPP_
#include "c4/yml/extra/event_ints.hpp"
#endif
#ifndef C4_YML_RESOLVE_OPTIONS_HPP_
#include "c4/yml/resolve_options.hpp"
#endif
#ifndef C4_YML_DETAIL_SMALL_MAP_HPP_
#include "c4/yml/detail/small_map.hpp"
#endif



#define prkv(kv) \
    printf("%.*s unqcount=%d count=%d id=%d fromtarget=%d fromalias=%d conflict=%d", \
           static_cast<int>(kv.val.key.len), kv.val.key.str,            \
           kv.val.unqcount, kv.val.count, kv.val.id,                    \
           kv.val.status & FROM_TARGET, kv.val.status & FROM_ALIAS, kv.val.status & CONFLICT)
#define prv(v) \
    printf("unqcount=%d count=%d id=%d fromtarget=%d fromalias=%d conflict=%d", \
           v.unqcount, v.count, v.id,                    \
           v.status & FROM_TARGET, v.status & FROM_ALIAS, v.status & CONFLICT)


// NOLINTBEGIN(*-signed-bitwise)

namespace c4 {
namespace yml {
namespace extra {
namespace ievt {


namespace {
enum : evt_size { nosibling = -1 };
using KnownKey_ = ievt::ReferenceResolver::KnownKey_;
using KnownKeyRange_ = ievt::ReferenceResolver::KnownKeyRange_;

KnownKey_ const* krng_select_latest_(KnownKeyRange_ r) noexcept
{
    evt_size maxpos = -1;
    KnownKey_ const* which = nullptr;
    for(auto const& kv : r)
    {
        if(kv.val.count > maxpos)
        {
            maxpos = kv.val.count;
            which = &kv.val;
        }
    }
    return which;
}

KnownKey_ const* krng_select_earliest_(KnownKeyRange_ r) noexcept
{
    static_assert(std::is_same<evt_size,decltype(INT_MAX)>::value, "evt_size vs INT_MAX");
    evt_size minpos = INT_MAX;
    KnownKey_ const* which = nullptr;
    for(auto const& kv : r)
    {
        if(kv.val.count < minpos)
        {
            minpos = kv.val.count;
            which = &kv.val;
        }
    }
    return which;
}

KnownKey_ const* krng_select_earliest_original_latest_rest_(KnownKeyRange_ r) noexcept
{
    static_assert(std::is_same<evt_size,decltype(INT_MAX)>::value, "evt_size vs INT_MAX");
    evt_size maxpos = -1;
    evt_size minpos_orig = INT_MAX;
    KnownKey_ const* which = nullptr;
    KnownKey_ const* which_orig = nullptr;
    for(auto const& kv : r)
    {
        if(kv.val.status & ievt::ReferenceResolver::FROM_TARGET)
        {
            if(kv.val.count < minpos_orig)
            {
                minpos_orig = kv.val.count;
                which_orig = &kv.val;
            }
        }
        if(kv.val.count > maxpos)
        {
            maxpos = kv.val.count;
            which = &kv.val;
        }
    }
    return which_orig ? which_orig : which;
}

KnownKey_ const* krng_select_latest_original_earliest_rest_(KnownKeyRange_ r) noexcept
{
    static_assert(std::is_same<evt_size,decltype(INT_MAX)>::value, "evt_size vs INT_MAX");
    evt_size minpos = -1;
    evt_size maxpos_orig = minpos;
    KnownKey_ const* which = nullptr;
    KnownKey_ const* which_orig = nullptr;
    for(auto const& kv : r)
    {
        if(kv.val.status & ievt::ReferenceResolver::FROM_TARGET)
        {
            if(kv.val.count > maxpos_orig)
            {
                maxpos_orig = kv.val.count;
                which_orig = &kv.val;
            }
        }
        if(kv.val.count < minpos)
        {
            minpos = kv.val.count;
            which = &kv.val;
        }
    }
    return which_orig ? which_orig : which;
}

KnownKey_ const* krng_select_(KnownKeyRange_ r, ResolveOptions opts) noexcept
{
    KnownKey_ const* which;
    if(opts.overwrite_original() && opts.overwrite_mergedin())
        which = krng_select_latest_(r);
    else if(opts.overwrite_original())
        which = krng_select_latest_original_earliest_rest_(r);
    else if(opts.overwrite_mergedin())
        which = krng_select_earliest_original_latest_rest_(r);
    else
        which = krng_select_earliest_(r); // this is the standard
    RYML_ASSERT_BASIC_(which);
    RYML_ASSERT_BASIC_(which >= &r.lo->val);
    RYML_ASSERT_BASIC_(which < &r.hi->val);
    return which;
}
} // namespace


#ifdef RYML_USE_ASSERT
bool ReferenceResolver::krng_check_(KnownKeyRange_ r)
{
    #define docheck(cond) RYML_ASSERT_BASIC_(cond); if(!(cond)) return false
    docheck(r.hi);
    docheck(r.lo);
    evt_size num = static_cast<evt_size>(r.hi - r.lo);
    docheck(num);
    evt_size count = 0;
    bool several = num > 1;
    auto const* prev = r.lo;
    for(auto const& kv : r)
    {
        docheck(unique_keys.lookup(kv.val.unqcount));
        docheck(kv.val.key == *unique_keys.lookup(kv.val.unqcount));
        docheck(!!(kv.val.status & ievt::ReferenceResolver::CONFLICT) == several);
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
#endif


ReferenceResolver::ReferenceResolver(Callbacks const& cb)
    : Buffers()
    , wpos()
    , newsz()
    , sz_limit()
    , opts()
    , anchor_map(cb)
    , known_keys(cb)
    , parents()
    , siblings()
    , can_resize()
{
}

void ReferenceResolver::resolve_resize(Buffers *evts_, ResolveOptions const& opts_)
{
    can_resize = true;
    resolve_(evts_, opts_);
}

C4_NODISCARD evt_size ReferenceResolver::resolve_noresize(Buffers evts_, ResolveOptions const& opts_)
{
    can_resize = false;
    resolve_(&evts_, opts_);
    return wpos;
}

void ReferenceResolver::resolve_(Buffers *evts_, ResolveOptions const& opts_)
{
    if(!evts_->evts.len)
        return;
    reset_(*evts_, can_resize, opts_);
    evt_size len0 = evts.len;
    // gather all the anchors
    anchor_map.clear();
    for(evt_size pos = 0; evts.ptr[pos] != ievt::ESTR; pos = nextpos(evts.ptr, pos))
        if(evts.ptr[pos] & ievt::ANCH)
            add_anchor_(pos);
    anchor_map.sort_if_needed();
    // now go to all refs, and resolve them
    for(evt_size pos = 0; evts.ptr[pos] != ievt::ESTR; pos = nextpos(evts.ptr, pos))
    {
        if(detail::seqormap(evts.ptr[pos]))
        {
            if(descend_(pos))
            {
                ;
            }
        }
        else if(evts.ptr[pos] & ievt::ALIA)
        {
            AnchorTargetRange_ rng = lookup_anchor_(pos);
            set_rref_(pos, rng.beg, rng.end);
        }
    }
    // finally:
    if(opts.clear_anchors())
        for(auto const& entry : anchor_map.m_arr)
            clear_str_(entry.val.pos);
    // adjust for the sentinel
    if(wpos > len0)
        ++wpos;
    if(can_resize)
    {
        evts.len = wpos;
        evts_->operator=(*this);
    }
}

void ReferenceResolver::reset_(Buffers evts_, bool can_resize_, ResolveOptions opts_)
{
    Buffers::operator=(evts_);
    wpos = evts.len;
    newsz = evts.len;
    sz_limit = static_cast<evt_size>(opts_.max_size(static_cast<size_t>(evts.len)));
    opts = opts_;
    can_resize = can_resize_;
    parents.clear();
    siblings.resize(static_cast<size_t>(evts.len));
    for(size_t i = 0; i < siblings.size(); ++i)
        siblings[i] = nosibling;
}

C4_NODISCARD evt_size ReferenceResolver::next_sibling_memoized_(evt_size pos)
{
    size_t spos = static_cast<size_t>(pos);
    evt_size sib = siblings[spos];
    if(sib != nosibling)
        return sib;
    siblings[spos] = sib = detail::find_next_sibling(evts.ptr, evts.len, pos);
    return sib;
}

C4_NODISCARD evt_size ReferenceResolver::matching_close_memoized_(evt_size pos)
{
    RYML_ASSERT_BASIC_(detail::seqormap(evts.ptr[pos]));
    return ievt::prevpos(evts.ptr, next_sibling_memoized_(pos));
}

C4_NODISCARD bool ReferenceResolver::is_cyclic_ref_(evt_size anchor) const RYML_NOEXCEPT
{
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[anchor], ievt::ANCH));
    evt_size elm = detail::find_next_entry_(evts.ptr, evts.len, anchor, ievt::KEY_|ievt::VAL_);
    for(size_t i = 0; i < parents.size(); ++i)
        if(elm == parents.top(i))
            return true;
    return false;
}

C4_NODISCARD auto ReferenceResolver::elm_range_(evt_size elm) -> Range_
{
    RYML_ASSERT_BASIC_(detail::isentry(evts.ptr[elm]));
    Range_ rng;
    rng.beg = elm;
    if(evts.ptr[rng.beg] & (ievt::SCLR|ievt::ALIA)) // scalars
    {
        rng.end = rng.beg + 3;
    }
    else // containers
    {
        RYML_ASSERT_BASIC_(detail::seqormap(evts.ptr[rng.beg]));
        rng.end = matching_close_memoized_(rng.beg) + 1/*include the end*/;
    }
    return rng;
}

C4_NODISCARD auto ReferenceResolver::keyval_range_(evt_size beg) -> Range_
{
    RYML_ASSERT_BASIC_(evts.ptr[beg] & ievt::KEY_);
    RYML_ASSERT_BASIC_(detail::isentry(evts.ptr[beg]));
    Range_ r = elm_range_(detail::find_next_entry_(evts.ptr, evts.len, beg, ievt::VAL_));
    r.beg = beg;
    return r;
}

C4_NODISCARD auto ReferenceResolver::keyval_range_val_(evt_size beg) -> Range_
{
    RYML_ASSERT_BASIC_(evts.ptr[beg] & ievt::KEY_);
    RYML_ASSERT_BASIC_(detail::isentry(evts.ptr[beg]));
    return elm_range_(detail::find_next_entry_(evts.ptr, evts.len, beg, ievt::VAL_));
}

C4_NODISCARD bool ReferenceResolver::descend_(evt_size &pos)
{
    bool ret = false;
    parents.emplace_top() = pos;
    if(detail::hasall(evts.ptr[pos], ievt::BSEQ))
    {
        pos = resolve_children_seq_(pos);
        ret = true;
    }
    else if(detail::hasall(evts.ptr[pos], ievt::BMAP))
    {
        pos = resolve_children_map_(pos);
        ret = true;
    }
    parents.pop();
    return ret;
}

C4_NODISCARD evt_size ReferenceResolver::resolve_children_seq_(evt_size pos)
{
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[pos], ievt::BSEQ));
    ++pos;
    for( ; evts.ptr[pos] != ievt::ESTR; pos = nextpos(evts.ptr, pos))
    {
        if(evts.ptr[pos] & ievt::ALIA)
        {
            AnchorTargetRange_ rng = lookup_anchor_(pos);
            set_rref_(pos, rng.beg, rng.end);
        }
        else if(descend_(pos))
        {
            ;
        }
        else if(detail::hasall(evts.ptr[pos], ievt::ESEQ))
        {
            break;
        }
    }
    return pos;
}

C4_NODISCARD bool ReferenceResolver::requires_merge_(evt_size pos, bool *key_conflict)
{
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[pos], ievt::BMAP));
    known_keys.clear();
    unique_keys.clear();
    bool has_merge_elm = false;
    // https://yamlvalidate.com/blog/yaml-anchors-aliases-merge-keys/
printf("populate(%d): enter\n", pos);
    for(evt_size ch = pos + 1; true; ch = next_sibling_memoized_(ch))
    {
printf("wtf 0 ch=%d\n", ch);
        if(detail::hasall(evts.ptr[ch], ievt::EMAP))
            break;
        else if(!detail::hasall(evts.ptr[ch], ievt::KEY_|ievt::SCLR))
            continue;
        csubstr key = getstr(ch);
printf("wtf 0.0.1 ch=%d, key=%.*s\n", ch, static_cast<int>(key.len), key.str);
        if( ! is_merge_elm_(ch, key))  // is it an element like `<<: *ref` or `<<: [*ref1,*ref2]`?
        {
printf("wtf 0.1 ch=%d\n", ch);
            add_known_key_(ch, FROM_TARGET, key_conflict);
        }
        else
        {
            has_merge_elm = true;
printf("wtf 1 ch=%d\n", ch);
            Range_ rng = keyval_range_val_(ch);
printf("wtf 2 ch=%d rng=(%d,%d)\n", ch, rng.beg, rng.end);
            if(detail::hasall(evts.ptr[rng.beg], ievt::VAL_|ievt::ALIA))
            {
printf("wtf 3 ch=%d\n", ch);
                add_to_known_keys_(rng.beg, FROM_ALIAS, key_conflict);
            }
            else
            {
printf("wtf 4 ch=%d\n", ch);
                RYML_ASSERT_BASIC_(rng.beg == ch + 3);
                RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[ch + 3], ievt::BSEQ));
                ++rng.beg;
printf("wtf 4.1 ch=%d rng.beg=%d\n", ch, rng.beg);
                while(true)
                {
                    RYML_ASSERT_BASIC_(evts.ptr[rng.beg] & ievt::ALIA);
printf("wtf 4.2 ch=%d rng.beg=%d\n", ch, rng.beg);
                    add_to_known_keys_(rng.beg, FROM_ALIAS, key_conflict);
printf("wtf 4.3 ch=%d rng.beg=%d\n", ch, rng.beg);
                    rng.beg = nextpos(evts.ptr, rng.beg);
printf("wtf 4.4 ch=%d rng.beg=%d\n", ch, rng.beg);
                    if(detail::hasall(evts.ptr[rng.beg], ievt::ESEQ))
                        break;
                }
            }
printf("wtf 5 ch=%d rng.beg=%d\n", ch, rng.beg);
        }
    }
    if(*key_conflict)
    {
        known_keys.sort();
        unique_keys.sort();
    }
printf("populate(%d): exit\n", pos);
int kcount = 0;
for(auto const& kv : known_keys.m_arr)
{
    printf("populate(%d) key[%d]: ", pos, kcount++);
    prkv(kv);
    printf("\n");
}
printf("populate(%d) has_merge_elm=%d key_conflict: %d\n", pos, has_merge_elm, *key_conflict);
    return has_merge_elm || (*key_conflict && opts.merge_repeated_keys());
}

void ReferenceResolver::add_known_key_(evt_size id, uint32_t status, bool *key_conflict)
{
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[id], ievt::KEY_|ievt::SCLR));
    evt_size count = static_cast<evt_size>(known_keys.m_arr.size());
    evt_size unq = static_cast<evt_size>(unique_keys.m_arr.size());
    csubstr key = getstr(id);
    KnownKey_ *existing = known_keys.lookup_modify(key);
    if( ! existing)
    {
        unique_keys.insert(unq) = key;
    }
    else
    {
        status |= CONFLICT;
        existing->status |= CONFLICT;
        *key_conflict = true;
        unq = existing->unqcount;
    }
    KnownKey_ &entry = known_keys.insert(key);
    entry.id = id;
    entry.count = count;
    entry.unqcount = unq;
    entry.status = status;
    entry.key = key;
}

void ReferenceResolver::add_to_known_keys_(evt_size beg, evt_size end, uint32_t status, bool *has_key_conflict)
{
    RYML_ASSERT_BASIC_(evts.ptr[beg] & ievt::KEY_);
    RYML_ASSERT_BASIC_(!(status & CONFLICT));
    while(beg < end)
    {
        if(detail::hasall(evts.ptr[beg], ievt::KEY_|ievt::SCLR))
            add_known_key_(beg, status, has_key_conflict);
        beg = next_sibling_memoized_(beg);
    }
}

void ReferenceResolver::add_to_known_keys_(evt_size alia, uint32_t status, bool *has_key_conflict)
{
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[alia], ievt::VAL_|ievt::ALIA));
    AnchorTargetRange_ rng = lookup_anchor_(alia);
    ++rng.beg;
    --rng.end;
    add_to_known_keys_(rng.beg, rng.end, status, has_key_conflict);
}

C4_NODISCARD evt_size ReferenceResolver::resolve_children_map_(evt_size parent)
{
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[parent], ievt::BMAP));
    enum : evt_size { noposref = -1 };
    evt_size posref = noposref;
    bool key_conflict = false;
printf("-------------------------\n");
printf("merge id=%d: enter\n", parent);
    bool do_merge = requires_merge_(parent, &key_conflict);
printf("merge id=%d: do_merge=%d key_conflict=%d\n", parent, do_merge, key_conflict);
    for(evt_size child = parent + 1; !detail::hasall(evts.ptr[child], ievt::EMAP); child = next_sibling_memoized_(child))
    {
        if(evts.ptr[child] & ievt::ALIA)
        {
printf("merge id=%d: resolve %d: ALIA\n", parent, child);
            AnchorTargetRange_ rng = lookup_anchor_(child);
            set_rref_(child, rng.beg, rng.end);
        }
        else if(detail::hasall(evts.ptr[child], ievt::KEY_|ievt::SCLR))
        {
printf("merge id=%d: resolve %d: key\n", parent, child);
            if(is_merge_elm_(child))
            {
printf("merge id=%d: resolve %d: merge_elm\n", parent, child);
                if( ! key_conflict)
                {
printf("merge id=%d: resolve %d: noconflict\n", parent, child);
                    handle_merge_elm_(child, key_conflict);
                }
                else
                {
printf("merge id=%d: resolve %d: conflict\n", parent, child);
                    maybe_clear_anchtag_(child);
                    clear_keyval_(child);
                    if(posref == noposref)
                        posref = child;
                }
            }
        }
    }
    if(do_merge)
    {
        Range_ newrng;
        newrng.beg = wpos;
        for(auto const& C4_RESTRICT uk : unique_keys.m_arr)
        {
            csubstr key = uk.val;
            KnownKeyRange_ const r = known_keys.equal_range(key);
printf("merge id=%d: key=%.*s numentries=%zd\n", parent, static_cast<int>(key.len), key.str, r.hi - r.lo);
for(auto const& kv : r)
{
printf("... ");
prkv(kv);
printf("\n");
}
            RYML_ASSERT_BASIC_(krng_check_(r));
            // select the key to remain
            KnownKey_ const* selected = krng_select_(r, opts);
printf("merge id=%d: selected: ", parent);
prv((*selected));
printf("\n");
            // clear all other in-target keys in the range. keys from aliases
            // must remain there (in the anchored elements).
            for(auto const& kv : r)
            {
                if((kv.val.status & FROM_TARGET) && (&kv.val != selected))
                {
printf("merge id=%d: clearing: ", parent);
prkv(kv);
printf("\n");
                    clear_keyval_(kv.val.id);
                }
            }
            // enforce the selected key: clear all in-target keys
            if(!(selected->status & FROM_TARGET))
            {
printf("merge id=%d: copying: ", parent);
prv((*selected));
printf("\n");
                copy_keyval_to_end_(selected->id);
            }
        }
        newrng.end = wpos;
printf("aki 5.1 newrng=(%d,%d)\n", newrng.beg, newrng.end);
        if(newrng.end > newrng.beg)
{
printf("aki 5.2 merging to %d: newrng=(%d,%d)\n", posref, newrng.beg, newrng.end);
            RYML_ASSERT_BASIC_(posref != noposref);
            //RYML_ASSERT_BASIC_((evts.ptr[posref] & ~ievt::EXT2) == 0);
            //RYML_ASSERT_BASIC_(evts.ptr[posref + 1] == 0);
            //RYML_ASSERT_BASIC_(evts.ptr[posref + 2] == 0);
            //RYML_ASSERT_BASIC_((evts.ptr[posref + 3] & ievt::EXT2) == 0);
            replace_with_ref_(posref, newrng.beg, newrng.end);
}
    }
    evt_size pos = parent + 1;
    for( ; evts.ptr[pos] != ievt::ESTR; pos = next_sibling_memoized_(pos))
    {
        if(descend_(pos))
        {
            ;
        }
        else if(detail::hasall(evts.ptr[pos], ievt::EMAP))
        {
            break;
        }
    }
    return pos;
}

void ReferenceResolver::add_anchor_(evt_size pos)
{
    RYML_ASSERT_BASIC_(evts.ptr[pos] & ievt::ANCH);
    AnchorTargetRange_& r = anchor_map.insert(getstr(pos));
    r.beg = pos + (!(evts.ptr[pos + 3] & ievt::TAG_) ? 3 : 6);
    if(evts.ptr[r.beg] & ievt::SCLR)
    {
        r.end = r.beg + 3;
    }
    else
    {
        RYML_ASSERT_BASIC_(evts.ptr[r.beg] & ievt::BEG_);
        r.end = next_sibling_memoized_(r.beg); // END_ must be part of the range, so extend beyond it
    }
    r.pos = pos;
}

ReferenceResolver::AnchorTargetRange_ ReferenceResolver::lookup_anchor_(evt_size pos) const
{
    RYML_ASSERT_BASIC_(evts.ptr[pos] & ievt::ALIA);
    csubstr ref = getstr(pos);
    AnchorTargetRange_ const* r = anchor_map.lookup(ref);
    if C4_LIKELY (!!r)
    {
        RYML_ASSERT_BASIC_(getstr(r->pos) == ref);
        if(r->pos < pos) // the anchor must come before the ref point
        {
            if C4_LIKELY (!is_cyclic_ref_(r->pos))
                 return *r;
            RYML_ERR_BASIC_("cyclic reference: '{}' at ALIA[{}]: ANCH at {}", ref, r->pos, pos);
        }
    }
    RYML_ERR_BASIC_("anchor not found: reference at ALIA[{}]: {}", pos, ref);
}

evt_size ReferenceResolver::count_extra(evt_size beg, evt_size end) const noexcept
{
    evt_size count = end - beg;
    for(evt_size pos = beg; pos < end; pos = nextpos(evts.ptr, pos))
        if(detail::hasall(evts.ptr[pos], ievt::RREF))
            count += count_extra(evts.ptr[pos+1], evts.ptr[pos+2]);
    return count;
}

void ReferenceResolver::clear_str_(evt_size pos)
{
    evts.ptr[pos] &= ievt::PEX2; // keep only these flags (no unary ~ operator)
    evts.ptr[pos + 1] = 0;
    evts.ptr[pos + 2] = 0;
    evts.ptr[pos + 3] &= ~ievt::PEX2;
}

void ReferenceResolver::clear_str2_(evt_size pos)
{
    evts.ptr[pos] &= ievt::PEX2; // keep only these flags (no unary ~ operator)
    evts.ptr[pos + 1] = 0;
    evts.ptr[pos + 2] = 0;
    evts.ptr[pos + 3] = 0;
    evts.ptr[pos + 4] = 0;
    evts.ptr[pos + 5] = 0;
    evts.ptr[pos + 6] &= ~ievt::PEX2;
}

bool ReferenceResolver::maybe_clear_anchtag_(evt_size pos)
{
    RYML_ASSERT_BASIC_(detail::isentry(evts.ptr[pos]));
    if(evts.ptr[pos] & ievt::PEX2)
    {
        if(evts.ptr[pos - 3] & (ievt::ANCH|ievt::TAG_))
        {
            if(detail::hasall(evts.ptr[pos - 3], ievt::TAG_|ievt::PEX2)
               &&
               (evts.ptr[pos - 6] & ievt::ANCH))
                clear_str2_(pos - 6);
            else
                clear_str_(pos - 3);
        }
    }
    return false;
}

void ReferenceResolver::rem_entry_(evt_size pos)
{
    RYML_ASSERT_BASIC_(detail::isentry(evts.ptr[pos]));
    evt_bits clr = maybe_clear_anchtag_(pos) ? 0 : ievt::PEX2;
    if(evts.ptr[pos] & ievt::WSTR)
    {
        evts.ptr[pos] = clr; // don't clear before the if
        clear_str_(pos);
    }
    else
    {
        RYML_ASSERT_BASIC_(detail::seqormap(evts.ptr[pos]));
        evt_size sib = next_sibling_memoized_(pos);
        evts.ptr[pos] = clr; // don't clear before the if
        // cannot use memset because there may be RREF elements inside
        for(evt_size i = pos + 1, next = i; i != sib; i = next)
        {
            evt_size evt = evts.ptr[i];
            evts.ptr[i] = 0;
            next = i + 1;
            if(evt & ievt::EXT2)
            {
                evts.ptr[i + 1] = 0;
                evts.ptr[i + 2] = 0;
                next = i + 3;
            }
        }
        evts.ptr[sib] &= ~ievt::PEX2;
    }
}

void ReferenceResolver::rem_keyval_(evt_size pos_key)
{
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[pos_key], ievt::KEY_|ievt::SCLR));
    evt_size pos_val = detail::find_next_entry_(evts.ptr, evts.len, pos_key, ievt::VAL_);
    rem_entry_(pos_key);
    rem_entry_(pos_val);
}

void ReferenceResolver::set_noref_(evt_size pos)
{
    RYML_ASSERT_BASIC_(evts.ptr[pos] & ievt::ALIA);
    RYML_ASSERT_BASIC_(evts.ptr[pos + 3] & ievt::PEX2);
    evts.ptr[pos] &= ievt::PEX2; // keep only these flags (no unary ~ operator)
    evts.ptr[pos + 1] = 0;
    evts.ptr[pos + 2] = 0;
    evts.ptr[pos + 3] &= ~ievt::PEX2;
}

void ReferenceResolver::replace_with_ref_(evt_size pos, evt_size beg, evt_size end)
{
    evt_size extra = sz_limit ? count_extra(beg, end) : 0;
    newsz += extra;
    if C4_LIKELY (!sz_limit || newsz < sz_limit)
    {
        //maybe_clear_anchtag_(pos);
        evts.ptr[pos] = ievt::RREF | (evts.ptr[pos] & ievt::PEX2);
        evts.ptr[pos + 1] = beg;
        evts.ptr[pos + 2] = end;
    }
    else
    {
        RYML_ERR_BASIC_("aliasing would exceed max size limit");
    }
}

void ReferenceResolver::set_rref_(evt_size pos, evt_size beg, evt_size end)
{
    RYML_ASSERT_BASIC_(evts.ptr[pos] & ievt::ALIA);
    RYML_ASSERT_BASIC_(evts.ptr[pos + 3] & ievt::PEX2);
    replace_with_ref_(pos, beg, end);
}

// is it an element like `<<: *ref` or `<<: [*ref1,*ref2]`?
bool ReferenceResolver::is_merge_elm_(evt_size pos) const
{
    return is_merge_elm_(pos, getstr(pos));
}
bool ReferenceResolver::is_merge_elm_(evt_size pos, csubstr key) const
{
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[pos], ievt::KEY_|ievt::SCLR));
    if(!(evts.ptr[pos] & ievt::PLAI) || key != "<<")
        return false;
    pos += 3;
    if(detail::hasall(evts.ptr[pos], ievt::VAL_|ievt::ALIA))
    {
        return true;
    }
    else if(detail::hasall(evts.ptr[pos], ievt::VAL_|ievt::BSEQ))
    {
        bool isref = false;
        ++pos;
        while(true)
        {
            if(evts.ptr[pos] & ievt::ALIA)
            {
                isref = true;
                pos += 3;
            }
            else if(detail::hasall(evts.ptr[pos], ievt::ESEQ))
            {
                break;
            }
            else
            {
                if C4_LIKELY (!isref)
                    pos = nextpos(evts.ptr, pos);
                else
                    RYML_ERR_BASIC_("invalid reference at pos={}", pos);

            }
        }
        return isref;
    }
    return false;
}

void ReferenceResolver::handle_merge_elm_(evt_size child, bool key_conflict)
{
    // clear any anchors or tags immediately preceding the << scalar
    maybe_clear_anchtag_(child);
    if(detail::hasall(evts.ptr[child + 3], ievt::VAL_|ievt::ALIA))
    {
        merge_map_single_(child, key_conflict);
    }
    else if(detail::hasall(evts.ptr[child + 3], ievt::VAL_|ievt::BSEQ))
    {
        merge_map_multiple_(child, key_conflict);
    }
}

void ReferenceResolver::merge_map_single_(evt_size pos, bool key_conflict)
{
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[pos], ievt::KEY_|ievt::SCLR));
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[pos + 3], ievt::VAL_|ievt::ALIA));
    RYML_ASSERT_BASIC_("<<" == getstr(pos));
    clear_str_(pos); // clear the << scalar
    apply_merge_ref_(pos + 3, key_conflict);
}

void ReferenceResolver::merge_map_multiple_(evt_size pos, bool key_conflict)
{
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[pos], ievt::KEY_|ievt::SCLR));
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[pos + 3], ievt::VAL_|ievt::BSEQ));
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[pos + 4], ievt::ALIA));
    RYML_ASSERT_BASIC_("<<" == getstr(pos));
    clear_str_(pos); // clear the << scalar
    evts.ptr[pos + 3] = 0; // clear the BSEQ
    pos += 4;
    while(true)
    {
        RYML_ASSERT_BASIC_(evts.ptr[pos] & ievt::ALIA);
        evt_size next = nextpos(evts.ptr, pos);
        apply_merge_ref_(pos, key_conflict);
        pos = next;
        if(detail::hasall(evts.ptr[next], ievt::ESEQ))
            break;
    }
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[pos], ievt::ESEQ));
    evts.ptr[pos] &= ievt::PEX2; // keep only these flags (no unary ~ operator)
}

void ReferenceResolver::apply_merge_ref_(evt_size pos, bool key_conflict)
{
printf("aki 0 merging %d: conflict=%d\n", pos, key_conflict);
    AnchorTargetRange_ rng = lookup_anchor_(pos);
printf("aki 1 merging %d: rng=(%d,%d)\n", pos, rng.beg, rng.end);
    if C4_LIKELY (detail::hasall(evts.ptr[rng.beg], ievt::BMAP))
    {
        RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[rng.end - 1], ievt::EMAP));
        ++rng.beg; // skip BMAP
        --rng.end; // skip EMAP
printf("aki 2 merging %d: rng=(%d,%d)\n", pos, rng.beg, rng.end);
        if(rng.beg == rng.end) // src map is empty?
        {
printf("aki 3 merging %d: rng=(%d,%d)\n", pos, rng.beg, rng.end);
            set_noref_(pos);
        }
        else if(!key_conflict || !has_key_conflict_(rng.beg, rng.end))
        {
printf("aki 4 merging %d: rng=(%d,%d)\n", pos, rng.beg, rng.end);
            set_rref_(pos, rng.beg, rng.end);
        }
        else
        {
            RYML_ERR_BASIC_("do this elsewhere");
        }
printf("aki 6 merging %d: rng=(%d,%d)\n", pos, rng.beg, rng.end);
    }
    else
    {
        RYML_ERR_BASIC_("invalid merge reference at pos={}", pos);
    }
}

bool ReferenceResolver::has_key_conflict_(evt_size beg, evt_size end)
{
    while(beg < end)
    {
        if(detail::hasall(evts.ptr[beg], ievt::KEY_|ievt::SCLR))
        {
            KnownKey_ const* existing = known_keys.lookup(getstr(beg));
            if(existing && (existing->status & CONFLICT))
                return true;
        }
        beg = next_sibling_memoized_(beg);
    }
    return false;
}


void ReferenceResolver::clear_range_(evt_size beg, evt_size end) noexcept
{
    RYML_ASSERT_BASIC_(beg <= end);
    RYML_ASSERT_BASIC_(beg <= evts.len);
    RYML_ASSERT_BASIC_(end + 1 < evts.len);
    size_t num_to_clear = static_cast<size_t>(end - beg);
    if(num_to_clear)
    {
        evt_bits first = evts.ptr[beg] & ievt::PEX2;
        memset(evts.ptr + beg, 0, num_to_clear * sizeof(evt_size));
        evts.ptr[beg] = first;
        evts.ptr[end] &= ~ievt::PEX2;
    }

}

void ReferenceResolver::clear_keyval_(evt_size keyval) noexcept
{
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[keyval], ievt::KEY_|ievt::SCLR));
    RYML_ASSERT_BASIC_(keyval <= evts.len);
    const Range_ r = keyval_range_(keyval);
    clear_range_(r.beg, r.end);
}

void ReferenceResolver::overwrite_keyval_(evt_size orig, evt_size dst)
{
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[dst], ievt::KEY_|ievt::SCLR));
    RYML_ASSERT_BASIC_(orig != dst);
    RYML_ASSERT_BASIC_(evts.ptr[orig + 3] & ievt::VAL_);
    const Range_ srcrng = keyval_range_val_(orig);
    const Range_ dstrng = keyval_range_val_(dst);
    RYML_ASSERT_BASIC_(detail::isentry(evts.ptr[srcrng.beg]));
    RYML_ASSERT_BASIC_(detail::isentry(evts.ptr[dstrng.beg]));
    RYML_ASSERT_BASIC_(evts.ptr[srcrng.beg] & ievt::VAL_);
    RYML_ASSERT_BASIC_(evts.ptr[dstrng.beg] & ievt::VAL_);
    RYML_ASSERT_BASIC_((srcrng.beg != dstrng.beg) && (srcrng.end != dstrng.end));
printf("aki 0: %d->%d\n", orig, dst);
    if((evts.ptr[srcrng.beg] & ievt::SCLR) &&
       (evts.ptr[dstrng.beg] & ievt::SCLR))
    {
printf("aki 1: %d->%d\n", orig, dst);
        // if both are scalars, copy the src scalar and its style
        RYML_ASSERT_BASIC_(srcrng.end == srcrng.beg + 3);
        RYML_ASSERT_BASIC_(dstrng.end == dstrng.beg + 3);
        evts.ptr[dstrng.beg    ] = evts.ptr[srcrng.beg];
        evts.ptr[dstrng.beg + 1] = evts.ptr[srcrng.beg + 1];
        evts.ptr[dstrng.beg + 2] = evts.ptr[srcrng.beg + 2];
    }
    else if(dstrng.end >= dstrng.beg + 3)
    {
printf("aki 2: %d->%d\n", orig, dst);
        // dst length is large enough to accomodate the ref
        replace_with_ref_(dstrng.beg, srcrng.beg, srcrng.end);
        clear_range_(dstrng.beg + 3, dstrng.end); // clear the rest of dst
    }
    else // the ref does not fit in the val. but it fits the keyval!
    {
printf("aki 3: %d->%d\n", orig, dst);
        // set the dst key as ref to the src keyval
        RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[srcrng.beg - 3], ievt::KEY_|ievt::SCLR));
        RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[dstrng.beg - 3], ievt::KEY_|ievt::SCLR));
        replace_with_ref_(dstrng.beg - 3, srcrng.beg - 3, srcrng.end);
        clear_range_(dstrng.beg, dstrng.end); // clear the rest of dst
    }
}

void ReferenceResolver::copy_keyval_to_end_(evt_size keyval)
{
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[keyval], ievt::KEY_|ievt::SCLR));
    const Range_ rng = keyval_range_val_(keyval);
    RYML_ASSERT_BASIC_(evts.ptr[rng.beg] & ievt::VAL_);
    const evt_size num = rng.end - keyval;
    RYML_ASSERT_BASIC_(num);
    if(grow_evts_to_add_(num))
    {
        const evt_bits xbeg = evts.ptr[wpos] & ievt::PEX2;
        const evt_bits xend = evts.ptr[rng.end] & ievt::PEX2;
        memcpy(evts.ptr + wpos, evts.ptr + keyval, static_cast<size_t>(num) * sizeof(evt_size));
        evts.ptr[wpos] &= ~ievt::PEX2;
        evts.ptr[wpos] |= xbeg;
        evts.len += num;
        RYML_ASSERT_BASIC_(evts.cap > evts.len); // there must be room for the sentinel
        evts.ptr[evts.len] = xend;
    }
    wpos += num;
}

bool ReferenceResolver::grow_evts_to_add_(evt_size extra)
{
    evt_size slack = evts.cap - wpos - 1;
    if(slack >= extra)
    {
        return true;
    }
    else if(can_resize)
    {
        evt_size newcap = wpos + extra + 1;
        evt_size nextcap = (3 * evts.cap) / 2; // do not grow too quick
        newcap = newcap > nextcap ? newcap : nextcap;
        evts = yml::detail::resize(evts, newcap, known_keys.m_arr.m_callbacks);
        return true;
    }
    return false;
}

} // namespace ievt
} // namespace extra
} // namespace yml
} // namespace c4

// NOLINTEND(*-signed-bitwise)
