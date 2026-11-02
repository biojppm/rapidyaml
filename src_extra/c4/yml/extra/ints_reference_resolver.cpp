#ifndef C4_YML_EXTRA_INTS_REFERENCE_RESOLVER_HPP_
#include "c4/yml/extra/ints_reference_resolver.hpp"
#endif

#ifndef C4_YML_EXTRA_EVENT_INTS_HPP_
#include "c4/yml/extra/event_ints.hpp"
#endif
#ifndef C4_YML_DETAIL_LOOKUP_ANCHOR_HPP_
#include "c4/yml/detail/lookup_anchor.hpp"
#endif

#define c4dbgrri_(...)                          \
    do {                                        \
        printf("%s:%d: ", __FILE__, __LINE__);  \
        printf(__VA_ARGS__);                    \
        printf("\n");                           \
    } while(0)
#define c4dbgrri__(...)                         \
    do {                                        \
        printf("%s:%d: ", __FILE__, __LINE__);  \
        printf(__VA_ARGS__);                    \
    } while(0)
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



ReferenceResolver::ReferenceResolver(Callbacks const& cb) noexcept
    : Buffers()
    , wpos()
    , newsz()
    , sz_limit()
    , opts()
    , anchor_map(cb)
    , known_keys(cb)
    , parents(cb)
    , siblings(cb)
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
       opts.keep_repeated_keys() &&
       !opts.merge_always() &&
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
            if(!entry.val.removed)
            {
                clear_str_(static_cast<evt_size>(entry.val.visit_count));
print____("after clear anchor at %zu", entry.val.visit_count);
            }
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
    RYML_ASSERT_BASIC_(spos < siblings.size());
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
    RYML_ASSERT_BASIC_(detail::isentry(mapw_scratch[selm]));
    Range_ rng;
    rng.beg = elm;
    if(mapw_scratch[selm] & (ievt::SCLR|ievt::ALIA)) // scalars
    {
        rng.end = rng.beg + 3;
    }
    else // containers. don't use the memoized matching close!
    {
        RYML_ASSERT_BASIC_(detail::seqormap(mapw_scratch[selm]));
        rng.end = detail::find_matching_close_(&mapw_scratch[0], static_cast<evt_size>(mapw_scratch.size()), rng.beg)
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
    RYML_ASSERT_BASIC_(beg >= 0 && sbeg < mapw_scratch.size());
    RYML_ASSERT_BASIC_(mapw_scratch[sbeg] & ievt::KEY_);
    RYML_ASSERT_BASIC_(detail::isentry(mapw_scratch[sbeg]));
    Range_ r = elm_range_scratch_(detail::find_next_entry_(&mapw_scratch[0], static_cast<evt_size>(mapw_scratch.size()), beg, ievt::VAL_));
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
c4dbgrri_("after descend. pos=%d", pos);
    parents.pop();
    return ret;
}

C4_NODISCARD evt_size ReferenceResolver::resolve_children_seq_(evt_size pos)
{
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[pos], ievt::BSEQ));
    for(++pos; evts.ptr[pos] != ievt::ESTR; pos = nextpos(evts.ptr, pos))
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

C4_NODISCARD evt_size ReferenceResolver::resolve_children_map_(evt_size parent)
{
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[parent], ievt::BMAP));
c4dbgrri_("-------------------------\n");
c4dbgrri_("merge id=%d: enter", parent);
    if(opts.keep_repeated_keys())
    {
        return resolve_map_repeat_(parent);
    }
    bool do_merge = requires_merge_(parent);
    bool key_conflict = known_keys.has_key_conflict();
    evt_size pos = resolve_map_merge_(parent, key_conflict);
    if(do_merge && key_conflict)
    {
c4dbgrri_("merge id=%d: do_merge=%d key_conflict=%d", parent, do_merge, key_conflict);
        return merge_map_keys_(parent);
    }
    return pos;
}

C4_NODISCARD evt_size ReferenceResolver::resolve_map_repeat_(evt_size pos)
{
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[pos], ievt::BMAP));
    for(++pos; true; pos = next_sibling_memoized_(pos))
    {
        if(evts.ptr[pos] & ievt::ALIA)
        {
            AnchorData const* anch = lookup_anchor_(pos);
            if(anch)
                set_rref_(pos, anch->rng.beg, anch->rng.end);
        }
        else if(detail::hasall(evts.ptr[pos], ievt::KEY_|ievt::SCLR))
        {
            if(is_merge_elm_(pos))
                handle_merge_elm_(pos, /*key_conflict*/false);
        }
        else if(descend_(pos))
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

C4_NODISCARD evt_size ReferenceResolver::resolve_map_merge_(evt_size pos, bool key_conflict)
{
    for(++pos; true; pos = next_sibling_memoized_(pos))
    {
        if(evts.ptr[pos] & ievt::ALIA)
        {
c4dbgrri_("merge id=%d: resolve %d: ALIA", pos, pos);(void)fflush(nullptr);
            AnchorData const* anch = lookup_anchor_(pos);
            if(anch)
{
                set_rref_(pos, anch->rng.beg, anch->rng.end);
print____("merge id=%d: after resolve %d: ALIA", pos, pos);
}
        }
        else if(detail::hasall(evts.ptr[pos], ievt::KEY_|ievt::SCLR))
        {
c4dbgrri_("merge id=%d: resolve %d: key", pos, pos);
            if(is_merge_elm_(pos))
            {
c4dbgrri_("merge id=%d: resolve %d: merge_elm", pos, pos);
                if( ! key_conflict)
                {
c4dbgrri_("merge id=%d: resolve %d: noconflict", pos, pos);
                    handle_merge_elm_(pos, key_conflict);
                }
                else
                {
c4dbgrri_("merge id=%d: resolve %d: conflict", pos, pos);
                    maybe_clear_anchtag_(pos);
print____("merge id=%d: resolve %d: conflict", pos, pos);
                    clear_keyval_(pos);
print____("merge id=%d: resolve %d: conflict", pos, pos);
                }
            }
        }
        else if(descend_(pos))
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

C4_NODISCARD bool ReferenceResolver::requires_merge_(evt_size pos)
{
    // https://yamlvalidate.com/blog/yaml-anchors-aliases-merge-keys/
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[pos], ievt::BMAP));
    {
        bool has_merge_elms = false;
        for(evt_size ch = pos + 1; true; ch = next_sibling_memoized_(ch))
        {
            if(detail::hasall(evts.ptr[ch], ievt::EMAP))
            {
                break;
            }
            else if(detail::hasall(evts.ptr[ch], ievt::KEY_|ievt::SCLR))
            {
                if(is_merge_elm_(ch))  // is it an element like `<<: *ref` or `<<: [*ref1,*ref2]`?
                {
                    has_merge_elms = true;
                    break;
                }
            }
        }
        if(!has_merge_elms && !opts.merge_always())
            return false;
    }
c4dbgrri_("populate(%d): enter", pos);
    known_keys.start_adding();
    for(evt_size ch = pos + 1; true; ch = next_sibling_memoized_(ch))
    {
c4dbgrri_("wtf 0 ch=%d", ch);
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
c4dbgrri_("wtf 0.0.1 ch=%d, key=%.*s", ch, static_cast<int>(key.len), key.str);
        if( ! is_merge_elm_(ch, key))  // is it an element like `<<: *ref` or `<<: [*ref1,*ref2]`?
        {
c4dbgrri_("wtf 0.1 ch=%d", ch);
            known_keys.add(key, static_cast<id_type>(ch), KnownKeys::ORIGINAL);
        }
        else
        {
c4dbgrri_("wtf 1 ch=%d", ch);
            Range_ rng = keyval_range_val_(ch);
c4dbgrri_("wtf 2 ch=%d rng=(%d,%d)", ch, rng.beg, rng.end);
            if(detail::hasall(evts.ptr[rng.beg], ievt::VAL_|ievt::ALIA))
            {
c4dbgrri_("wtf 3 ch=%d", ch);
                add_to_known_keys_(rng.beg, KnownKeys::MERGEDIN);
            }
            else
            {
c4dbgrri_("wtf 4 ch=%d", ch);
                RYML_ASSERT_BASIC_(rng.beg == ch + 3);
                RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[ch + 3], ievt::BSEQ));
                ++rng.beg;
c4dbgrri_("wtf 4.1 ch=%d rng.beg=%d", ch, rng.beg);
                while(true)
                {
                    RYML_ASSERT_BASIC_(evts.ptr[rng.beg] & ievt::ALIA);
c4dbgrri_("wtf 4.2 ch=%d rng.beg=%d", ch, rng.beg);
                    add_to_known_keys_(rng.beg, KnownKeys::MERGEDIN);
c4dbgrri_("wtf 4.3 ch=%d rng.beg=%d", ch, rng.beg);
                    rng.beg = nextpos(evts.ptr, rng.beg);
c4dbgrri_("wtf 4.4 ch=%d rng.beg=%d", ch, rng.beg);
                    if(detail::hasall(evts.ptr[rng.beg], ievt::ESEQ))
                        break;
                }
            }
c4dbgrri_("wtf 5 ch=%d rng.beg=%d", ch, rng.beg);
        }
    }
    bool req = known_keys.requires_merge();
    if(req)
        known_keys.adding_is_complete();
c4dbgrri_("populate(%d): exit", pos);
    return req;
}

void ReferenceResolver::add_to_known_keys_(evt_size beg, evt_size end, uint32_t status)
{
    RYML_ASSERT_BASIC_((evts.ptr[beg] & ievt::KEY_) || beg == end);
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

C4_NODISCARD evt_size ReferenceResolver::merge_map_keys_(evt_size parent)
{
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[parent], ievt::BMAP));
    // prepare to overwrite the map:
    mapw_reset_(parent);
    // this is the range of new members added at the end (past the
    // ESTR event), when (if) the data overflows the original
    // space in the map.
    Range_ newrng;
    newrng.beg = wpos;
    // we're ready. iterate by unique key
    for(id_type i = 0; i < known_keys.num_unique_keys(); ++i)
    {
c4dbgrri_("merge[%d]: unique_key=%zu  wslot=%d -------", parent, i, mapw_wslot);
        KnownKeys::KnownKeyRange_ keyentries;
        KnownKeys::Entry const* selected = known_keys.select(i, &keyentries);
        RYML_ASSERT_BASIC_(keyentries.lo < keyentries.hi);
evt_size eidx = 0;
        // iterate all the key's candidates
        for(auto const& kv : keyentries)
        {
            evt_size id = static_cast<evt_size>(kv.val.id);
            bool is_last = (&kv + 1 == keyentries.hi);
c4dbgrri__("merge[%d]: unique_key=%zu ... rng[%d] id=%d ...", parent, i, eidx++, id);
prkv(kv);
printf("\n");
            if(&kv.val == selected)
            {
                if(kv.val.status & KnownKeys::ORIGINAL)
                {
c4dbgrri_("merge[%d]: unique_key=%zu ...... original, selected! copy! wslot=%d", parent, i, mapw_wslot);
                    mapw_cpkeyval_original_(id, is_last);
print____("after cp original");
                }
                else
                {
c4dbgrri_("merge[%d]: unique_key=%zu ...... mergedin, selected. copy! wslot=%d", parent, i, mapw_wslot);
                    mapw_cpkeyval_mergedin_(id, is_last);
print____("after cp mergedin");
                }
            }
            else if(opts.keep_repeated_keys())
            {
                if(kv.val.status & KnownKeys::ORIGINAL)
                {
                    mapw_cpkeyval_original_(id, is_last);
                }
            }
        }
    }
    newrng.end = wpos;
c4dbgrri_("aki 5.1 newrng=(%d,%d)", newrng.beg, newrng.end);
    if(newrng.end > newrng.beg)
    {
c4dbgrri_("aki 5.2 merging to %d: newrng=(%d,%d)", mapw_posref, newrng.beg, newrng.end);
        RYML_ASSERT_BASIC_(mapw_posref != -1);
        RYML_ASSERT_BASIC_(mapw_posref + 3 <= mapw_lastslot);
        replace_with_ref_(mapw_posref, newrng.beg, newrng.end);
        mapw_wslot += 3;
        clear_range_(mapw_posref + 3, mapw_lastslot);
print____("add ref to wpos");
    }
    else if(mapw_firstslot + mapw_wslot < mapw_lastslot)
    {
c4dbgrri_("aki 5.2 clear (%d,%d)", mapw_firstslot + mapw_wslot, mapw_lastslot);
        clear_range_(mapw_firstslot + mapw_wslot, mapw_lastslot);
print____("after clear range");
    }
    reset_siblings_range_(mapw_firstslot, mapw_lastslot);
    return mapw_lastslot;
}


void ReferenceResolver::mapw_reset_(evt_size parent)
{
    RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[parent], ievt::BMAP));
    mapw_wslot = 0; // current write slot
    mapw_posref = -1;
    // this is the number of slots we have to write
    // in-place, ie between BMAP and EMAP:
    mapw_firstslot = parent + 1;
    mapw_lastslot = matching_close_memoized_(parent);
    RYML_ASSERT_BASIC_(mapw_lastslot - mapw_firstslot > 3);
    // we'll copy the map data to a scratch space, and then
    // selectively copy from the scratch to in-place at the
    // map. once (if) the data overflows the available space in
    // the map, new elements are added at the end of the evts
    // buffer, after the ESTR event
    RYML_ASSERT_BASIC_(mapw_lastslot - mapw_firstslot > 3);
    // copy the data, including the sentinel event because of PEX2
    mapw_scratch.resize(static_cast<size_t>((mapw_lastslot+1) - mapw_firstslot));
    memcpy(&mapw_scratch[0], evts.ptr + mapw_firstslot, static_cast<size_t>((mapw_lastslot+1) - mapw_firstslot) * sizeof(evt_bits));
}

evt_size ReferenceResolver::mapw_cpdata_(evt_bits const* C4_RESTRICT data, evt_size beg, evt_size end, bool is_last_entry)
{
    evt_bits const* C4_RESTRICT tmp = &mapw_scratch[0];
    evt_size sz = end - beg;
    RYML_ASSERT_BASIC_(sz);
    // leave room for a RREF before spilling to past ESTR
    evt_size extra = is_last_entry ? 0 : 3;
    evt_size dstpos = mapw_firstslot + mapw_wslot;
c4dbgrri_("copydata:  slot=%d:%d  dstpos=%d  wslot=%d/%d  sz=%d  extra=%d", mapw_firstslot, mapw_lastslot, dstpos, mapw_wslot, mapw_lastslot-mapw_firstslot, sz, extra);
    if(dstpos + sz + extra < mapw_lastslot)
    {
        RYML_ASSERT_BASIC_(data != tmp || static_cast<size_t>(beg) < mapw_scratch.size());
        // we only need to copy if the data is going to end up
        // in a different position
c4dbgrri_("copyinplace:  beg=%d wslot=%d/%d", beg, mapw_wslot, mapw_lastslot-mapw_firstslot);
        if(!(data == tmp && beg == mapw_wslot))
        {
c4dbgrri_("copyinplace:  overwrite  sz=%d  %d:%d  <--  %d:%d  (%d:%d)", sz, dstpos, dstpos+sz, beg, end, beg+mapw_firstslot, end+mapw_firstslot);
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
    c4dbgrri_("copyinplace:  nothing to do\n");
}
    }
    else
    {
        if(mapw_posref == -1)
        {
c4dbgrri_("copytoend:  markref=%d", dstpos);
            mapw_posref = dstpos;
        }
        dstpos = wpos;
c4dbgrri_("copytoend:  sz=%d  %d:%d  <--  %d:%d  (%d:%d)", sz, wpos, wpos+sz, beg, end, beg+mapw_firstslot, end+mapw_firstslot);
        copy_range_to_end_(data, beg, end);
    }
    mapw_wslot += sz;
    return dstpos;
}

void ReferenceResolver::mapw_cpkeyval_original_(evt_size id, bool is_last_entry)
{
    RYML_ASSERT_BASIC_(id >= mapw_firstslot && id < mapw_lastslot);
    evt_bits const* C4_RESTRICT tmp = &mapw_scratch[0];
    const Range_ r = keyval_range_scratch_(id - mapw_firstslot);
    evt_size wpos0 = wpos;(void)wpos0;
    evt_size copy_pos = mapw_cpdata_(tmp, r.beg, r.end, is_last_entry);
c4dbgrri_("copyorig:   %d <-- %d", copy_pos, id);
    // copy_pos maybe inplace or at the end
    evt_size shift = copy_pos - (mapw_firstslot + r.beg);
    // update cached anchor data
    for(evt_size scratchpos = r.beg; scratchpos != r.end; scratchpos = nextpos(tmp, scratchpos))
    {
        if(tmp[scratchpos] & ievt::ANCH)
        {
            evt_size origpos = mapw_firstslot + scratchpos;
            evt_size next = origpos + shift;
c4dbgrri_("copyorig:   anchor! scratchpos=%d origpos=%d next=%d shift=%d", scratchpos, origpos, next, shift);
            RYML_ASSERT_BASIC_(!(tmp[scratchpos] & ievt::AREN));
            csubstr anch = src.sub(static_cast<size_t>(tmp[scratchpos + 1]),
                                   static_cast<size_t>(tmp[scratchpos + 2]));
c4dbgrri_("copyorig:   anchor=%.*s", static_cast<int>(anch.len), anch.str);
            auto rng = anchor_map.equal_range(anch);
            for(auto &entry : rng)
            {
                if(entry.val.visit_count == static_cast<id_type>(origpos))
                {
c4dbgrri_("copyorig:   anchor! origpos=%d shift=%d next=%d wpos=%d", origpos, shift, next, wpos);
c4dbgrri_("copyorig:   anchor! copy_pos=%d evts.len=%d", copy_pos, evts.len);
                    RYML_ASSERT_BASIC_(copy_pos < wpos0 ?
                                       (next >= mapw_firstslot && next < mapw_lastslot) :
                                       (next >= wpos0 && next < wpos));
                    RYML_ASSERT_BASIC_(evts.ptr[next] & ievt::ANCH);
                    RYML_ASSERT_BASIC_(getstr(next) == anch);
                    entry.val.visit_count = static_cast<id_type>(next);
                    break;
                }
            }
        }
    }
}

void ReferenceResolver::mapw_cpkeyval_mergedin_(evt_size id, bool is_last_entry)
{
    RYML_ASSERT_BASIC_(id < mapw_firstslot || id > mapw_lastslot);
    const Range_ r = keyval_range_(id);
    mapw_cpdata_(evts.ptr, r.beg, r.end, is_last_entry);
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
    r.removed = false;
}

void ReferenceResolver::rem_anchor_from_map_(evt_size pos)
{
    RYML_ASSERT_BASIC_(evts.ptr[pos] & ievt::ANCH);
    csubstr anch = getstr(pos);
    auto rng = anchor_map.equal_range(anch);
    RYML_ASSERT_BASIC_(rng.hi > rng.lo);
    bool gotit = false;
    for(auto &kv : rng)
    {
        if(static_cast<evt_size>(kv.val.visit_count) == pos)
        {
            kv.val.removed = true;
            gotit = true;
            break;
        }
    }
    RYML_ASSERT_BASIC_(gotit);(void)gotit;
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
            {
                rem_anchor_from_map_(pos - 6);
                clear_str2_(pos - 6);
            }
            else
            {
                if(evts.ptr[pos - 3] & ievt::ANCH)
                    rem_anchor_from_map_(pos - 3);
                clear_str_(pos - 3);
            }
        }
    }
    return false;
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
    return detail::hasall(evts.ptr[pos], ievt::KEY_|ievt::SCLR)
        && is_merge_elm_(pos, getstr(pos));
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
c4dbgrri_("aki 0 merging %d: conflict=%d", pos, key_conflict);
    AnchorData const* anch = lookup_anchor_(pos);
    if(!anch)
        return;
    Range_ rng = anch->rng;
c4dbgrri_("aki 1 merging %d: rng=(%d,%d)", pos, rng.beg, rng.end);
    if C4_LIKELY (detail::hasall(evts.ptr[rng.beg], ievt::BMAP))
    {
        RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[rng.end - 1], ievt::EMAP));
        ++rng.beg; // skip BMAP
        --rng.end; // skip EMAP
c4dbgrri_("aki 2 merging %d: rng=(%d,%d)", pos, rng.beg, rng.end);
        if(rng.beg == rng.end) // src map is empty?
        {
c4dbgrri_("aki 3 merging %d: rng=(%d,%d)", pos, rng.beg, rng.end);
            set_noref_(pos);
        }
        else if(!key_conflict || !has_key_conflict_(rng.beg, rng.end))
        {
c4dbgrri_("aki 4 merging %d: rng=(%d,%d)", pos, rng.beg, rng.end);
            set_rref_(pos, rng.beg, rng.end);
        }
        else
        {
            RYML_ERR_BASIC_("do this elsewhere");
        }
c4dbgrri_("aki 6 merging %d: rng=(%d,%d)", pos, rng.beg, rng.end);
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
c4dbgrri_("aki 0: %d->%d", orig, dst);
    if((evts.ptr[srcrng.beg] & ievt::SCLR) &&
       (evts.ptr[dstrng.beg] & ievt::SCLR))
    {
c4dbgrri_("aki 1: %d->%d", orig, dst);
        // if both are scalars, copy the src scalar and its style
        RYML_ASSERT_BASIC_(srcrng.end == srcrng.beg + 3);
        RYML_ASSERT_BASIC_(dstrng.end == dstrng.beg + 3);
        evts.ptr[dstrng.beg    ] = evts.ptr[srcrng.beg];
        evts.ptr[dstrng.beg + 1] = evts.ptr[srcrng.beg + 1];
        evts.ptr[dstrng.beg + 2] = evts.ptr[srcrng.beg + 2];
    }
    else if(dstrng.end >= dstrng.beg + 3)
    {
c4dbgrri_("aki 2: %d->%d", orig, dst);
        // dst length is large enough to accomodate the ref
        replace_with_ref_(dstrng.beg, srcrng.beg, srcrng.end);
        clear_range_(dstrng.beg + 3, dstrng.end); // clear the rest of dst
    }
    else // the ref does not fit in the val. but it fits the keyval!
    {
c4dbgrri_("aki 3: %d->%d", orig, dst);
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
    evt_bits const* orig = evts.ptr;
    if(grow_evts_to_add_(num))
    {
        if(mem == orig)
            mem = evts.ptr;
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
        siblings.resize(static_cast<id_type>(evts.cap));
        return true;
    }
    return false;
}

} // namespace ievt
} // namespace extra
} // namespace yml
} // namespace c4

// NOLINTEND(*-signed-bitwise)
