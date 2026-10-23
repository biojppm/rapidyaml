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
    anchor_map.sort();
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
    Range_ rng;
    RYML_ASSERT_BASIC_(evts.ptr[beg] & ievt::KEY_);
    RYML_ASSERT_BASIC_(detail::isentry(evts.ptr[beg]));
    rng.beg = detail::find_next_entry_(evts.ptr, evts.len, beg, ievt::VAL_);
    RYML_ASSERT_BASIC_(evts.ptr[rng.beg] & ievt::VAL_);
    if(evts.ptr[rng.beg] & ievt::SCLR) // scalars
    {
        rng.end = rng.beg + 3;
    }
    else // containers
    {
        RYML_ASSERT_BASIC_(detail::seqormap(evts.ptr[rng.beg]));
        rng.end = matching_close_memoized_(rng.end) + 1/*include the end*/;
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

C4_NODISCARD evt_size ReferenceResolver::resolve_children_map_(evt_size parent)
{
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[parent], ievt::BMAP));
    // reset known keys
    known_keys.clear();
    num_known_keys = 0;
    // populate known_keys with all the direct children. this is
    // because these take precedence over merged-in children,
    // even if the direct child appears after them.
    // see:
    // https://yamlvalidate.com/blog/yaml-anchors-aliases-merge-keys/
    for(evt_size child = parent + 1; evts.ptr[child] != ievt::ESTR; child = next_sibling_memoized_(child))
    {
        if(detail::hasall(evts.ptr[child], ievt::KEY_|ievt::SCLR))
        {
            csubstr key = getstr(child);
            if( ! is_merge_elm_(child, key))  // is it an element like `<<: *ref` or `<<: [*ref1,*ref2]`?
            {
                KnownKey_ const* existing_key = known_keys.lookup(key);
                if( ! existing_key) // is this a new key?
                {
                    add_known_key_(key, child, /*original*/true, /*copied*/false);
                }
            }
        }
    }
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
                handle_merge_elm_(child);
            else
                RYML_ASSERT_BASIC_(known_keys.lookup(key));
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

void ReferenceResolver::handle_merge_elm_(evt_size child)
{
    // clear any anchors or tags immediately preceding the << scalar
    maybe_clear_anchtag_(child);
    if(detail::hasall(evts.ptr[child + 3], ievt::VAL_|ievt::ALIA))
    {
        merge_map_single_(child);
    }
    else if(detail::hasall(evts.ptr[child + 3], ievt::VAL_|ievt::BSEQ))
    {
        merge_map_multiple_(child);
    }
}

void ReferenceResolver::merge_map_single_(evt_size pos)
{
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[pos], ievt::KEY_|ievt::SCLR));
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[pos + 3], ievt::VAL_|ievt::ALIA));
    RYML_ASSERT_BASIC_("<<" == getstr(pos));
    clear_str_(pos); // clear the << scalar
    apply_merge_ref_(pos + 3);
}

void ReferenceResolver::merge_map_multiple_(evt_size pos)
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
        apply_merge_ref_(pos);
        pos = next;
        if(detail::hasall(evts.ptr[next], ievt::ESEQ))
            break;
    }
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[pos], ievt::ESEQ));
    evts.ptr[pos] &= ievt::PEX2; // keep only these flags (no unary ~ operator)
}

void ReferenceResolver::apply_merge_ref_(evt_size pos)
{
    AnchorTargetRange_ rng = lookup_anchor_(pos);
    if C4_LIKELY (detail::hasall(evts.ptr[rng.beg], ievt::BMAP))
    {
        RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[rng.end - 1], ievt::EMAP));
        ++rng.beg; // skip BMAP
        --rng.end; // skip EMAP
        if(rng.beg == rng.end) // map is empty?
        {
            set_noref_(pos);
        }
        else if(!needs_merge_key_by_key_(rng.beg, rng.end))
        {
            set_rref_(pos, rng.beg, rng.end);
            add_to_known_keys_(rng.beg, rng.end);
        }
        else if(rng.beg != rng.end)
        {
            evt_size wpos0 = wpos;
            add_merge_elms_(rng.beg, rng.end);
            if(wpos > wpos0)
                set_rref_(pos, wpos0, wpos);
            else
                set_noref_(pos);
        }
    }
    else
    {
        RYML_ERR_BASIC_("invalid merge reference at pos={}", pos);
    }
}

// checks whether the keyval needs to be merged
bool ReferenceResolver::needs_merge_key_by_key_(evt_size beg, evt_size end)
{
    RYML_ASSERT_BASIC_(evts.ptr[beg] & ievt::KEY_);
    if(!known_keys.m_arr.empty())
    {
        known_keys.sort();
        while(beg < end)
        {
            if(detail::hasall(evts.ptr[beg], ievt::KEY_|ievt::SCLR))
            {
                if(known_keys.lookup(getstr(beg)))
                    return true;
            }
            beg = next_sibling_memoized_(beg);
        }
    }
    return false;
}

void ReferenceResolver::add_known_key_(csubstr key, evt_size id, bool original, bool copied)
{
    KnownKey_ &entry = known_keys.insert(key);
    entry.id = id;
    entry.count = num_known_keys++;
    entry.original = original;
    entry.copied = copied;
}

void ReferenceResolver::add_to_known_keys_(evt_size beg, evt_size end)
{
    RYML_ASSERT_BASIC_(evts.ptr[beg] & ievt::KEY_);
    while(beg < end)
    {
        if(detail::hasall(evts.ptr[beg], ievt::KEY_|ievt::SCLR))
        {
            csubstr key = getstr(beg);
            if(!known_keys.lookup(key))
                add_known_key_(key, beg, /*original*/false, /*copied*/false);
        }
        beg = next_sibling_memoized_(beg);
    }
}

void ReferenceResolver::add_merge_elms_(evt_size beg, evt_size end)
{
    RYML_ASSERT_BASIC_(evts.ptr[beg] & ievt::KEY_);
    while(beg < end)
    {
        if(detail::hasall(evts.ptr[beg], ievt::KEY_|ievt::SCLR))
        {
            csubstr key = getstr(beg);
            KnownKey_ const* existing = known_keys.lookup(key);
            if(!existing)
            {
                copy_keyval_to_end_(beg);
                add_known_key_(key, beg, /*original*/false, /*copied*/true);
            }
            else if((( ! existing->original) && opts.overwrite_existing()) ||
                    (existing->original && opts.overwrite_original()))
            {
                RYML_ASSERT_BASIC_(key == getstr(existing->id));
                overwrite_keyval_(existing->id, beg);
            }
        }
        beg = next_sibling_memoized_(beg);
    }
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
    if((evts.ptr[srcrng.beg] & ievt::SCLR) &&
       (evts.ptr[dstrng.beg] & ievt::SCLR))
    {
        // if both are scalars, copy the src scalar and its style
        RYML_ASSERT_BASIC_(srcrng.end == srcrng.beg + 3);
        RYML_ASSERT_BASIC_(dstrng.end == dstrng.beg + 3);
        enum : evt_bits { scalar_sty = ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD };
        evts.ptr[dstrng.beg] &= ~scalar_sty;
        evts.ptr[dstrng.beg] |= evts.ptr[srcrng.beg] & scalar_sty;
        evts.ptr[dstrng.beg + 1] = evts.ptr[srcrng.beg + 1];
        evts.ptr[dstrng.beg + 2] = evts.ptr[srcrng.beg + 2];
    }
    else if((evts.ptr[dstrng.beg] & ievt::SCLR) || (dstrng.len() > 2))
    {
        // dst val is scalar can be substituted by a ref
        // (has enough room for the three ints)
        RYML_ASSERT_BASIC_(!(evts.ptr[dstrng.beg] & ievt::SCLR));
        replace_with_ref_(dstrng.beg, srcrng.beg, srcrng.end);
    }
    else // the ref does not fit in the val. but it fits the keyval!
    {
        // set the dst key as ref to the src keyval
        RYML_ASSERT_BASIC_(detail::isentry(evts.ptr[srcrng.beg - 3]));
        RYML_ASSERT_BASIC_(detail::isentry(evts.ptr[dstrng.beg - 3]));
        RYML_ASSERT_BASIC_(evts.ptr[srcrng.beg - 3] & ievt::VAL_);
        RYML_ASSERT_BASIC_(evts.ptr[dstrng.beg - 3] & ievt::VAL_);
        replace_with_ref_(dstrng.beg - 3, srcrng.beg - 3, srcrng.end);
    }
}

void ReferenceResolver::copy_keyval_to_end_(evt_size keyval)
{
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[keyval], ievt::KEY_|ievt::SCLR));
    evt_size val = detail::find_next_entry_(evts.ptr, evts.len, keyval, ievt::VAL_);
    RYML_ASSERT_BASIC_(evts.ptr[val] & ievt::VAL_);
    evt_bits sentinel = (evts.ptr[val] & ievt::SCLR) ? ievt::PEX2 : 0;
    Range_ rng = val_range_(val);
    evt_size num = rng.len();
    RYML_ASSERT_BASIC_(num);
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
