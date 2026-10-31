#ifndef C4_YML_EXTRA_INTS_REFERENCE_RESOLVER_HPP_
#include "c4/yml/extra/ints_reference_resolver.hpp"
#endif

#ifndef C4_YML_EXTRA_EVENT_INTS_HPP_
#include "c4/yml/extra/event_ints.hpp"
#endif
#ifndef C4_YML_DETAIL_LOOKUP_ANCHOR_HPP_
#include "c4/yml/detail/lookup_anchor.hpp"
#endif

#include "c4/yml/extra/ints_utils.hpp"
#define print____(...)                                  \
    do {                                                \
        printf("-----------------------------------\n");\
        printf("%s:%d: ", __FILE__, __LINE__);          \
        printf(__VA_ARGS__);                            \
        printf("\n");                                   \
        /*events_ints_print(src, arena, evts.ptr, wpos);*/  \
    } while(0)

// NOLINTBEGIN(*-signed-bitwise)

namespace c4 {
namespace yml {
namespace extra {
namespace ievt {


namespace {
enum : evt_size { nosibling = -1 }; // NOLINT
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

void ReferenceResolver::reset_(Buffers evts_, bool can_resize_, ResolveOptions opts_)
{
    Buffers::operator=(evts_);
    wpos = evts.len;
    newsz = evts.len;
    sz_limit = static_cast<evt_size>(opts_.max_size(static_cast<id_type>(evts.len)));
    opts = opts_;
    known_keys.reset(opts_);
    can_resize = can_resize_;
    parents.clear();
    siblings.resize(static_cast<id_type>(evts.cap));
    reset_siblings_range_(0, evts.cap);
}

void ReferenceResolver::reset_siblings_range_(evt_size beg, evt_size end)
{
    id_type sbeg = static_cast<id_type>(beg);
    id_type send = static_cast<id_type>(end);
    RYML_ASSERT_BASIC_(sbeg <= siblings.size());
    RYML_ASSERT_BASIC_(send <= siblings.size());
    for( ; sbeg < send; ++sbeg)
        siblings[sbeg] = nosibling;
}

void ReferenceResolver::resolve_(Buffers *evts_, ResolveOptions const& opts_)
{
    if(!evts_->evts.len)
        return;
    reset_(*evts_, can_resize, opts_);
    evt_size startlen = evts.len;
    // gather all the anchors
    bool has_refs = false;
    anchor_map.clear();
    for(evt_size pos = 0; evts.ptr[pos] != ievt::ESTR; pos = nextpos(evts.ptr, pos))
    {
        if(evts.ptr[pos] & ievt::ANCH)
            add_anchor_(pos);
        else if(evts.ptr[pos] & ievt::ALIA)
            has_refs = true;
    }
    // exit early if there's nothing to do
    if(!has_refs &&
       !opts.merge_repeated_keys() &&
       (!opts.clear_anchors() || anchor_map.m_arr.empty()))
        return;
    anchor_map.sort();
print____("before resolve");
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
            AnchorData const* anch = lookup_anchor_(pos);
            if(anch)
                set_rref_(pos, anch->rng.beg, anch->rng.end);
        }
    }
    // finally:
    if(opts.clear_anchors())
    {
print____("before clear anchors");
        for(auto const& entry : anchor_map.m_arr)
        {
            clear_str_(static_cast<evt_size>(entry.val.visit_count));
print____("after clear anchor at %zu", entry.val.visit_count);
        }
    }
    // adjust for the sentinel
    wpos += (wpos > startlen);
    if(can_resize)
    {
        evts.len = wpos;
        evts_->operator=(*this);
    }
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
        RYML_ASSERT_BASIC_(detail::seqormap(evts.ptr[elm]));
        rng.end = matching_close_memoized_(elm)
            + 1/*include the end*/;
    }
    return rng;
}

C4_NODISCARD auto ReferenceResolver::elm_range_scratch_(evt_size elm) -> Range_
{
    id_type selm = static_cast<id_type>(elm);
    RYML_ASSERT_BASIC_(detail::isentry(scratch[selm]));
    Range_ rng;
    rng.beg = elm;
    if(scratch[selm] & (ievt::SCLR|ievt::ALIA)) // scalars
    {
        rng.end = rng.beg + 3;
    }
    else // containers. don't use the memoized matching close!
    {
        RYML_ASSERT_BASIC_(detail::seqormap(scratch[selm]));
        rng.end = detail::find_matching_close_(&scratch[0], static_cast<evt_size>(scratch.size()), rng.beg)
            + 1/*include the end*/;
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

C4_NODISCARD auto ReferenceResolver::keyval_range_scratch_(evt_size beg) -> Range_
{
    id_type sbeg = static_cast<id_type>(beg);
    RYML_ASSERT_BASIC_(beg >= 0 && sbeg < scratch.size());
    RYML_ASSERT_BASIC_(scratch[sbeg] & ievt::KEY_);
    RYML_ASSERT_BASIC_(detail::isentry(scratch[sbeg]));
    Range_ r = elm_range_scratch_(detail::find_next_entry_(&scratch[0], static_cast<evt_size>(scratch.size()), beg, ievt::VAL_));
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
printf("after descend. pos=%d\n", pos);
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
            AnchorData const* anch = lookup_anchor_(pos);
            if(anch)
                set_rref_(pos, anch->rng.beg, anch->rng.end);
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

C4_NODISCARD bool ReferenceResolver::requires_merge_(evt_size pos)
{
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[pos], ievt::BMAP));
    known_keys.start_adding();
    // https://yamlvalidate.com/blog/yaml-anchors-aliases-merge-keys/
printf("populate(%d): enter\n", pos);
    for(evt_size ch = pos + 1; true; ch = next_sibling_memoized_(ch))
    {
printf("wtf 0 ch=%d\n", ch);
        if(detail::hasall(evts.ptr[ch], ievt::EMAP))
        {
            break;
        }
        else if(detail::hasall(evts.ptr[ch], ievt::KEY_|ievt::ALIA))
        {
            known_keys.add(getstr(ch), static_cast<id_type>(ch), KnownKeys::ORIGINAL);
            continue;
        }
        else if(!detail::hasall(evts.ptr[ch], ievt::KEY_|ievt::SCLR))
        {
            continue;
        }
        csubstr key = getstr(ch);
printf("wtf 0.0.1 ch=%d, key=%.*s\n", ch, static_cast<int>(key.len), key.str);
        if( ! is_merge_elm_(ch, key))  // is it an element like `<<: *ref` or `<<: [*ref1,*ref2]`?
        {
printf("wtf 0.1 ch=%d\n", ch);
            known_keys.add(key, static_cast<id_type>(ch), KnownKeys::ORIGINAL);
        }
        else
        {
printf("wtf 1 ch=%d\n", ch);
            Range_ rng = keyval_range_val_(ch);
printf("wtf 2 ch=%d rng=(%d,%d)\n", ch, rng.beg, rng.end);
            if(detail::hasall(evts.ptr[rng.beg], ievt::VAL_|ievt::ALIA))
            {
printf("wtf 3 ch=%d\n", ch);
                add_to_known_keys_(rng.beg, KnownKeys::MERGEDIN);
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
                    add_to_known_keys_(rng.beg, KnownKeys::MERGEDIN);
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
    bool req = known_keys.requires_merge();
    if(req)
        known_keys.adding_is_complete();
    return req;
}

void ReferenceResolver::add_to_known_keys_(evt_size beg, evt_size end, uint32_t status)
{
    RYML_ASSERT_BASIC_(evts.ptr[beg] & ievt::KEY_);
    RYML_ASSERT_BASIC_(!(status & KnownKeys::CONFLICT));
    while(beg < end)
    {
        if(detail::hasall(evts.ptr[beg], ievt::KEY_|ievt::SCLR))
            known_keys.add(getstr(beg), static_cast<id_type>(beg), status);
        beg = next_sibling_memoized_(beg);
    }
}

void ReferenceResolver::add_to_known_keys_(evt_size alia, uint32_t status)
{
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[alia], ievt::VAL_|ievt::ALIA));
    AnchorData const* anch = lookup_anchor_(alia);
    if(anch)
        add_to_known_keys_(anch->rng.beg + 1, anch->rng.end - 1, status);
}

C4_NODISCARD evt_size ReferenceResolver::resolve_children_map_(evt_size parent)
{
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[parent], ievt::BMAP));
printf("-------------------------\n");
printf("merge id=%d: enter\n", parent);
    bool do_merge = requires_merge_(parent);
    bool key_conflict = known_keys.has_key_conflict();
printf("merge id=%d: do_merge=%d key_conflict=%d\n", parent, do_merge, key_conflict);
    for(evt_size child = parent + 1; true; child = next_sibling_memoized_(child))
    {
        if(evts.ptr[child] & ievt::ALIA)
        {
printf("merge id=%d: resolve %d: ALIA\n", parent, child);(void)fflush(nullptr);
            AnchorData const* anch = lookup_anchor_(child);
            if(anch)
{
                set_rref_(child, anch->rng.beg, anch->rng.end);
print____("merge id=%d: after resolve %d: ALIA", parent, child);
}
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
                }
            }
        }
        else if(detail::hasall(evts.ptr[child], ievt::EMAP))
        {
            break;
        }
    }
    if(do_merge && key_conflict)
    {
        return merge_map_keys_(parent);
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


C4_NODISCARD evt_size ReferenceResolver::merge_map_keys_(evt_size parent)
{
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[parent], ievt::BMAP));
    // this is the number of slots we have to write
    // in-place, ie between BMAP and EMAP:
    const evt_size firstslot = parent + 1;
    const evt_size lastslot = matching_close_memoized_(parent);
    RYML_ASSERT_BASIC_(lastslot - firstslot > 3);
    // we'll copy the map data to a scratch space, and then
    // selectively copy from the scratch to in-place at the
    // map. once (if) the data overflows the available space in
    // the map, new elements are added at the end of the evts
    // buffer, after the ESTR event
    RYML_ASSERT_BASIC_(lastslot - firstslot > 3);
    // copy the data, including the sentinel event because of PEX2
    scratch.resize(static_cast<size_t>((lastslot+1) - firstslot));
    memcpy(&scratch[0], evts.ptr + firstslot, static_cast<size_t>((lastslot+1) - firstslot) * sizeof(evt_bits));
    evt_bits const* C4_RESTRICT tmp = &scratch[0];
    evt_size wslot = 0; // current write slot
    evt_size posref = -1;
    // now define facilities to copy the data
    auto cpdata = [&](evt_bits const* C4_RESTRICT data, evt_size beg, evt_size end, bool is_last_entry){
        evt_size sz = end - beg;
        RYML_ASSERT_BASIC_(sz);
        // leave room for a RREF before spilling to past ESTR
        evt_size extra = is_last_entry ? 0 : 3;
        evt_size dstpos = firstslot + wslot;
printf("copydata:  slot=%d:%d  dstpos=%d  wslot=%d/%d  sz=%d  extra=%d\n", firstslot, lastslot, dstpos, wslot, lastslot-firstslot, sz, extra);
        if(dstpos + sz + extra < lastslot)
        {
            RYML_ASSERT_BASIC_(data != tmp || static_cast<size_t>(beg) < scratch.size());
            // we only need to copy if the data is going to end up
            // in a different position
printf("copyinplace:  beg=%d wslot=%d/%d\n", beg, wslot, lastslot-firstslot);
            if(data == tmp && beg == wslot)
            {
printf("copyinplace:  nothing to do\n");
                return dstpos;
            }
printf("copyinplace:  overwrite  sz=%d  %d:%d  <--  %d:%d  (%d:%d)\n", sz, dstpos, dstpos+sz, beg, end, beg+firstslot, end+firstslot);
            RYML_ASSERT_BASIC_(dstpos > 2);
            RYML_ASSERT_BASIC_(dstpos + sz < evts.len);
            const evt_bits xbeg = evts.ptr[dstpos] & ievt::PEX2;
            const evt_bits xend = data[end] & ievt::PEX2;
            memcpy(evts.ptr + dstpos, data + beg, static_cast<size_t>(sz) * sizeof(evt_bits));
            evts.ptr[dstpos] &= ~ievt::PEX2;
            evts.ptr[dstpos] |= xbeg;
            evts.ptr[dstpos + sz] &= ~ievt::PEX2;
            evts.ptr[dstpos + sz] |= xend;
        }
        else
        {
            if(posref == -1)
            {
printf("copytoend:  markref=%d\n", dstpos);
                posref = dstpos;
            }
            dstpos = wpos;
printf("copytoend:  sz=%d  %d:%d  <--  %d:%d  (%d:%d)\n", sz, wpos, wpos+sz, beg, end, beg+firstslot, end+firstslot);
            copy_range_to_end_(data, beg, end);
        }
        wslot += sz;
        return dstpos;
    };
    auto copy_keyval_original = [&](evt_size id, bool is_last_entry){
        RYML_ASSERT_BASIC_(id >= firstslot && id < lastslot);
        const Range_ r = keyval_range_scratch_(id - firstslot);
        evt_size wpos0 = wpos;
        evt_size copy_pos = cpdata(tmp, r.beg, r.end, is_last_entry);
printf("copyorig:   %d <-- %d\n", copy_pos, id);
        // copy_pos maybe inplace or at the end
        evt_size shift = copy_pos - (firstslot + r.beg);
        // update anchor data
        for(evt_size scratchpos = r.beg; scratchpos != r.end; scratchpos = nextpos(tmp, scratchpos))
        {
            if(tmp[scratchpos] & ievt::ANCH)
            {
                evt_size origpos = firstslot + scratchpos;
                evt_size next = origpos + shift;
printf("copyorig:   anchor! scratchpos=%d origpos=%d next=%d shift=%d\n", scratchpos, origpos, next, shift);
                RYML_ASSERT_BASIC_(!(tmp[scratchpos] & ievt::AREN));
                csubstr anch = src.sub(static_cast<size_t>(tmp[scratchpos + 1]),
                                       static_cast<size_t>(tmp[scratchpos + 2]));
printf("copyorig:   anchor=%.*s\n", static_cast<int>(anch.len), anch.str);
                auto rng = anchor_map.equal_range(anch);
                for(auto &entry : rng)
                {
                    if(entry.val.visit_count == static_cast<id_type>(origpos))
                    {
printf("copyorig:   anchor! origpos=%d shift=%d next=%d wpos=%d\n", origpos, shift, next, wpos);
printf("copyorig:   anchor! copy_pos=%d evts.len=%d\n", copy_pos, evts.len);
                        RYML_ASSERT_BASIC_(copy_pos < wpos0 ?
                                           (next >= firstslot && next < lastslot) :
                                           (next >= wpos0 && next < wpos));
                        RYML_ASSERT_BASIC_(evts.ptr[next] & ievt::ANCH);
                        RYML_ASSERT_BASIC_(getstr(next) == anch);
                        entry.val.visit_count = static_cast<id_type>(next);
                        break;
                    }
                }
            }
        }
    };
    auto copy_keyval_mergedin = [&](evt_size id, bool is_last_entry){
        RYML_ASSERT_BASIC_(id < firstslot || id > lastslot);
        const Range_ r = keyval_range_(id);
        cpdata(evts.ptr, r.beg, r.end, is_last_entry);
    };
    // this is the range of new members added at the end (past the
    // ESTR event), when (if) the data overflows the original
    // space in the map.
    Range_ newrng;
    newrng.beg = wpos;
    // we're ready. iterate by unique key
    for(id_type i = 0; i < known_keys.num_unique_keys(); ++i)
    {
        // iterate all the key's candidates
printf("merge[%d]: unique_key=%zu  wslot=%d -------\n", parent, i, wslot);
        KnownKeys::KnownKeyRange_ keyentries;
        KnownKeys::Entry const* selected = known_keys.select(i, &keyentries);
        RYML_ASSERT_BASIC_(keyentries.lo < keyentries.hi);
evt_size eidx = 0;
        for(auto const& kv : keyentries)
        {
            evt_size id = static_cast<evt_size>(kv.val.id);
            bool is_last = (&kv + 1 == keyentries.hi);
printf("merge[%d]: unique_key=%zu ... rng[%d] id=%d ...", parent, i, eidx++, id);
prkv(kv);
printf("\n");
            if(&kv.val == selected)
            {
                if(kv.val.status & KnownKeys::ORIGINAL)
                {
printf("merge[%d]: unique_key=%zu ...... original, selected! copy! wslot=%d\n", parent, i, wslot);
                    copy_keyval_original(id, is_last);
print____("after cp original");
                }
                else
                {
printf("merge[%d]: unique_key=%zu ...... mergedin, selected. copy! wslot=%d\n", parent, i, wslot);
                    copy_keyval_mergedin(id, is_last);
print____("after cp mergedin");
                }
            }
        }
    }
    newrng.end = wpos;
printf("aki 5.1 newrng=(%d,%d)\n", newrng.beg, newrng.end);
    if(newrng.end > newrng.beg)
    {
printf("aki 5.2 merging to %d: newrng=(%d,%d)\n", posref, newrng.beg, newrng.end);
        RYML_ASSERT_BASIC_(posref != -1);
        RYML_ASSERT_BASIC_(posref + 3 <= lastslot);
        replace_with_ref_(posref, newrng.beg, newrng.end);
        wslot += 3;
        clear_range_(posref + 3, lastslot);
print____("add ref to wpos");
    }
    else if(firstslot + wslot < lastslot)
    {
printf("aki 5.2 clear (%d,%d)\n", firstslot + wslot, lastslot);
        clear_range_(firstslot + wslot, lastslot);
print____("after clear range");
    }
    reset_siblings_range_(firstslot, lastslot);
    return lastslot;
}


void ReferenceResolver::add_anchor_(evt_size pos)
{
    RYML_ASSERT_BASIC_(evts.ptr[pos] & ievt::ANCH);
    AnchorData& r = anchor_map.insert(getstr(pos));
    r.rng.beg = pos + (!(evts.ptr[pos + 3] & ievt::TAG_) ? 3 : 6);
    if(evts.ptr[r.rng.beg] & ievt::SCLR)
    {
        r.rng.end = r.rng.beg + 3;
    }
    else
    {
        RYML_ASSERT_BASIC_(evts.ptr[r.rng.beg] & ievt::BEG_);
        r.rng.end = next_sibling_memoized_(r.rng.beg); // END_ must be part of the range, so extend beyond it
    }
    r.visit_count = static_cast<id_type>(pos);
}

auto ReferenceResolver::lookup_anchor_(evt_size pos) const
    -> AnchorData const*
{
    RYML_ASSERT_BASIC_(evts.ptr[pos] & ievt::ALIA);
    csubstr ref = getstr(pos);
    AnchorData const* r = yml::detail::lookup_anchor(anchor_map, static_cast<size_t>(pos), ref, opts);
    if(!r)
        return nullptr;
    evt_size apos = static_cast<evt_size>(r->visit_count);
    RYML_ASSERT_BASIC_(getstr(apos) == ref);
    if C4_LIKELY (!is_cyclic_ref_(apos))
        return r;
    RYML_ERR_BASIC_("cyclic reference: '{}' at ALIA[{}]: ANCH at {}", ref, apos, pos);
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
        evts.ptr[pos + 3] |= ievt::PEX2;
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
    if((beg + 3 == end) && (evts.ptr[beg] & ievt::SCLR))
    {
        evts.ptr[pos] &= ievt::PEX2|ievt::KEY_|ievt::VAL_; // retain only this
        evts.ptr[pos] |= evts.ptr[beg] & ~(ievt::PEX2|ievt::KEY_|ievt::VAL_); // copy everything but this
        evts.ptr[pos + 1] = evts.ptr[beg + 1];
        evts.ptr[pos + 2] = evts.ptr[beg + 2];
    }
    else
    {
        replace_with_ref_(pos, beg, end);
    }
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
    AnchorData const* anch = lookup_anchor_(pos);
    if(!anch)
        return;
    Range_ rng = anch->rng;
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
            if(known_keys.exists_with_conflict(getstr(beg)))
                return true;
        beg = next_sibling_memoized_(beg);
    }
    return false;
}


void ReferenceResolver::clear_range_(evt_size beg, evt_size end) noexcept
{
    RYML_ASSERT_BASIC_(beg <= end);
    RYML_ASSERT_BASIC_(beg <= evts.len);
    RYML_ASSERT_BASIC_(end + 1 < evts.len);
    size_t num = static_cast<size_t>(end - beg);
    if(num)
    {
        evt_bits first = evts.ptr[beg] & ievt::PEX2;
        memset(evts.ptr + beg, 0, num * sizeof(evt_size));
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
    copy_range_to_end_(evts.ptr, rng.beg, rng.end);
}

void ReferenceResolver::copy_range_to_end_(evt_bits const* mem, evt_size beg, evt_size end)
{
    const evt_size num = end - beg;
    RYML_ASSERT_BASIC_(num);
    if(grow_evts_to_add_(num))
    {
        const evt_bits xbeg = evts.ptr[wpos] & ievt::PEX2;
        const evt_bits xend = mem[end] & ievt::PEX2;
        memcpy(evts.ptr + wpos, mem + beg, static_cast<size_t>(num) * sizeof(evt_size));
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
        evts = yml::detail::resize(evts, newcap, known_keys.known_keys.m_arr.m_callbacks);
        return true;
    }
    return false;
}

} // namespace ievt
} // namespace extra
} // namespace yml
} // namespace c4

// NOLINTEND(*-signed-bitwise)
