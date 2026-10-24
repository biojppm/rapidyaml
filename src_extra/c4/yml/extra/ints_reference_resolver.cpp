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

// NOLINTBEGIN(*-signed-bitwise)

namespace c4 {
namespace yml {
namespace extra {
namespace ievt {


namespace {
enum : evt_size { nosibling = -1 };
} // namespace


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

C4_NODISCARD auto ReferenceResolver::val_range_(evt_size beg) -> Range_
{
    RYML_ASSERT_BASIC_(evts.ptr[beg] & ievt::KEY_);
    RYML_ASSERT_BASIC_(detail::isentry(evts.ptr[beg]));
    Range_ rng;
    rng.beg = detail::find_next_entry_(evts.ptr, evts.len, beg, ievt::VAL_);
    RYML_ASSERT_BASIC_(evts.ptr[rng.beg] & ievt::VAL_);
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

C4_NODISCARD bool ReferenceResolver::map_has_merge_elms_(evt_size pos)
{
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[pos], ievt::BMAP));
    for(evt_size ch = pos + 1; evts.ptr[ch] != ievt::ESTR; ch = next_sibling_memoized_(ch))
        if(detail::hasall(evts.ptr[ch], ievt::KEY_|ievt::SCLR))
            if(is_merge_elm_(ch, getstr(ch)))
                return true;
    return false;
}

C4_NODISCARD bool ReferenceResolver::populate_known_keys_(evt_size pos)
{
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[pos], ievt::BMAP));
    bool key_conflict = false;
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
            add_known_key_(ch, FROM_TARGET, &key_conflict);
        }
        else
        {
printf("wtf 1 ch=%d\n", ch);
            Range_ rng = val_range_(ch);
printf("wtf 2 ch=%d rng=(%d,%d)\n", ch, rng.beg, rng.end);
            if(detail::hasall(evts.ptr[rng.beg], ievt::VAL_|ievt::ALIA))
            {
printf("wtf 3 ch=%d\n", ch);
                add_to_known_keys_(rng.beg, FROM_ALIAS, &key_conflict);
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
                    add_to_known_keys_(rng.beg, FROM_ALIAS, &key_conflict);
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
    if(key_conflict)
        known_keys.sort();
printf("populate(%d): exit\n", pos);
int kcount = 0;
for(auto const& kv : known_keys.m_arr)
{
    printf("populate(%d) key[%d]: %.*s unqcount=%d count=%d id=%d fromtarget=%d fromalias=%d conflict=%d\n",
           pos, kcount++,
           static_cast<int>(kv.key.len), kv.key.str,
           kv.val.unqcount, kv.val.count,
           kv.val.id, kv.val.status & FROM_TARGET, kv.val.status & FROM_ALIAS, kv.val.status & CONFLICT);
}
printf("populate(%d) key_conflict: %d\n", pos, key_conflict);
    return key_conflict;
}

C4_NODISCARD evt_size ReferenceResolver::resolve_children_map_(evt_size parent)
{
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[parent], ievt::BMAP));
    num_known_keys = 0;
    num_unique_keys = 0;
    known_keys.clear();
    bool key_conflict = false;
    if(map_has_merge_elms_(parent))
        key_conflict = populate_known_keys_(parent);
    for(evt_size child = parent + 1; evts.ptr[child] != ievt::ESTR; child = next_sibling_memoized_(child))
    {
        if(evts.ptr[child] & ievt::ALIA)
        {
            AnchorTargetRange_ rng = lookup_anchor_(child);
            set_rref_(child, rng.beg, rng.end);
        }
        else if(detail::hasall(evts.ptr[child], ievt::KEY_|ievt::SCLR))
        {
            csubstr key = getstr(child);
            if(is_merge_elm_(child, key))
                handle_merge_elm_(child, key_conflict);
        }
        else if(detail::hasall(evts.ptr[child], ievt::EMAP))
        {
            break;
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
        evts.ptr[pos] = ievt::RREF;
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
        merge_map_single_(child, key_conflict);
    else if(detail::hasall(evts.ptr[child + 3], ievt::VAL_|ievt::BSEQ))
        merge_map_multiple_(child, key_conflict);
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
    AnchorTargetRange_ rng = lookup_anchor_(pos);
    if C4_LIKELY (detail::hasall(evts.ptr[rng.beg], ievt::BMAP))
    {
        RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[rng.end - 1], ievt::EMAP));
        ++rng.beg; // skip BMAP
        --rng.end; // skip EMAP
        if(rng.beg == rng.end) // src map is empty?
        {
            set_noref_(pos);
        }
        else if(!key_conflict || !has_key_conflict_(rng.beg, rng.end))
        {
            set_rref_(pos, rng.beg, rng.end);
        }
        else
        {
            RYML_ASSERT_BASIC_(rng.beg != rng.end);
            Range_ newrng = add_merge_elms_(rng.beg, rng.end);
            if(newrng.end > newrng.beg)
                set_rref_(pos, newrng.beg, newrng.end);
            else
                set_noref_(pos);
        }
    }
    else
    {
        RYML_ERR_BASIC_("invalid merge reference at pos={}", pos);
    }
}

void ReferenceResolver::add_known_key_(evt_size id, uint32_t status, bool *key_conflict)
{
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[id], ievt::KEY_|ievt::SCLR));
    csubstr key = getstr(id);
    KnownKey_ *existing = known_keys.lookup_modify(key);
    evt_size unq = num_unique_keys;
    if( ! existing)
    {
        ++num_unique_keys;
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
    entry.count = num_known_keys++;
    entry.unqcount = unq;
    entry.status = status;
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

auto ReferenceResolver::add_merge_elms_(evt_size beg, evt_size end)
    -> Range_
{
    RYML_ASSERT_BASIC_(evts.ptr[beg] & ievt::KEY_);
    Range_ rng;
    rng.beg = wpos;
printf("add: %d:%d\n", beg, end);
    while(beg < end)
    {
        if(detail::hasall(evts.ptr[beg], ievt::KEY_|ievt::SCLR))
        {
            csubstr key = getstr(beg);
            KnownKey_* existing = known_keys.lookup_modify(key);
            if(!existing)
            {
                copy_keyval_to_end_(beg);
            }
            else if(opts.overwrite(existing->status & FROM_TARGET))
            {
                RYML_ASSERT_BASIC_(key == getstr(existing->id));
                if(existing->status & FROM_TARGET)
                {
                    existing->status &= ~FROM_TARGET;
                    overwrite_keyval_(/*orig*/beg, /*dst*/existing->id);
                }
                else
                {
                    existing->id = wpos;
                    copy_keyval_to_end_(beg);
                }
            }
        }
        beg = next_sibling_memoized_(beg);
    }
    rng.end = wpos;
    return rng;
}


void ReferenceResolver::clear_range_(evt_size beg, evt_size end) noexcept
{
    RYML_ASSERT_BASIC_(beg <= end);
    RYML_ASSERT_BASIC_(beg <= evts.len);
    RYML_ASSERT_BASIC_(end <= evts.len);
    size_t num_to_clear = static_cast<size_t>(end - beg);
    if(num_to_clear)
        memset(evts.ptr + beg, 0, num_to_clear * sizeof(evt_size));
}

void ReferenceResolver::overwrite_keyval_(evt_size orig, evt_size dst)
{
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[dst], ievt::KEY_|ievt::SCLR));
    RYML_ASSERT_BASIC_(orig != dst);
    RYML_ASSERT_BASIC_(evts.ptr[orig + 3] & ievt::VAL_);
    const Range_ srcrng = val_range_(orig);
    const Range_ dstrng = val_range_(dst);
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
    const Range_ rng = val_range_(keyval);
    RYML_ASSERT_BASIC_(evts.ptr[rng.beg] & ievt::VAL_);
    const evt_size num = rng.end - keyval;
    RYML_ASSERT_BASIC_(num);
    const evt_bits sentinel = (evts.ptr[rng.beg] & ievt::SCLR) ? ievt::PEX2 : 0;
    if(grow_evts_to_add_(num))
    {
        memcpy(evts.ptr + wpos, evts.ptr + keyval, static_cast<size_t>(num) * sizeof(evt_size));
        evts.len += num;
        RYML_ASSERT_BASIC_(evts.cap > evts.len); // there must be room for the sentinel
        evts.ptr[evts.len] = sentinel;
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
