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

C4_SUPPRESS_WARNING_PUSH
C4_SUPPRESS_WARNING_GCC("-Wuseless-cast")
// NOLINTBEGIN(*-signed-bitwise)

namespace c4 {
namespace yml {
namespace extra {
namespace ievt {

#define rymlassertb_(...) RYML_ASSERT_BASIC_CB_(m_anchor_map.m_arr.m_callbacks, __VA_ARGS__)

namespace {
enum : evt_size { nosibling = -1 }; // NOLINT
#if !RYML_USE_ASSERT
#define rymlassert_not_recursive_(var)
#else
#define rymlassert_not_recursive_(var)              \
    rymlassertb_(!var);/*NOLINT*/                   \
    recursive_check_ reccheck_(&(var))
struct recursive_check_ // NOLINT
{
    bool *var;
    explicit recursive_check_(bool *var_) : var(var_) { *var = true; }
    ~recursive_check_() noexcept { *var = false; }
};
#endif
} // namespace

#if 1
#define prrng(mem, beg, end)
#define pr38(parent)
#define printall()
#define wtf(...)
#define wtfparents()
#else
#define wtf(...)                                \
    do {                                        \
        printf("%s:%d: ", __FILE__, __LINE__);  \
        printf(__VA_ARGS__);                    \
        printf("\n");                           \
    } while(0)
#define prrng(mem, beg, end)                        \
    do {                                            \
        printf("{\n  ");                            \
        for(evt_size i_ = (beg); i_ < (end); ++i_)  \
        {                                           \
            printf("[%d]0x%x,", (i_), (mem)[i_]);   \
            if((i_-(beg)) && !((i_-(beg)) % 5))     \
                printf("\n  ");                     \
        }                                           \
        printf("\n}\n");                            \
    } while(0)
#define prrngsl(mem, beg, end)                      \
    do {                                            \
        printf("{\n  ");                            \
        for(evt_size i_ = (beg); i_ < (end); ++i_)  \
        {                                           \
            printf("[%d]%d,", (i_), (mem)[i_]);     \
            if((i_-(beg)) && !((i_-(beg)) % 10))    \
                printf("\n  ");                     \
        }                                           \
        printf("}\n");                              \
    } while(0)
#define wtfparents()                                    \
    do {                                                \
        printf("parents: {");                           \
        for(id_type i = 0; i < m_parents.size(); ++i)   \
            printf("%d,", m_parents[i]);                \
        printf("}\n");                                  \
    } while(0)

#define printall()                                          \
    {                                                       \
        extra::ievt::events_ints_print(src,                 \
                                       arena,               \
                                       evts.ptr, evts.len); \
    }
#endif



ReferenceResolver::ReferenceResolver(Callbacks const& cb) noexcept
    : Buffers()
    , m_anchor_map(cb)
    , m_known_keys(cb)
    , m_parents(cb)
    , m_siblings(cb)
    , m_mapw_scratch(cb)
    , m_subject()
    , m_opts()
    , m_sz_limit()
    , m_newsz()
    , m_wpos()
    , m_mapw_beg()
    , m_mapw_end()
    , m_mapw_count()
    , m_mapw_posref()
    RYML_WITH_ASSERT_(
    , m_guard_resolving()
    , m_guard_merging()
    , m_guard_keymap()
    )
{
    #if RYML_USE_ASSERT
    ResolveParams p = {};
    rymlassertb_(m_anchor_map.m_small_limit == static_cast<id_type>(p.anchor_map_linear_threshold));
    rymlassertb_(m_known_keys.m_known_keys.m_small_limit == static_cast<id_type>(p.known_keys_linear_threshold));
    #endif
}

void ReferenceResolver::reset_(Buffers evts_, ResolveOptions const& opts_, ResolveParams const& params)
{
    Buffers::operator=(evts_);
    m_opts = opts_;
    m_sz_limit = opts_.max_size(evts.len);
    m_newsz = evts.len;
    m_wpos = evts.len;
    m_anchor_map.m_small_limit = static_cast<id_type>(params.anchor_map_linear_threshold);
    m_known_keys.reset(opts_);
    m_known_keys.m_known_keys.m_small_limit = static_cast<id_type>(params.known_keys_linear_threshold);
    m_parents.clear();
    m_siblings.resize(static_cast<id_type>(evts.cap));
    reset_siblings_range_(0, evts.cap);
    RYML_WITH_ASSERT_(m_guard_resolving = m_guard_merging = m_guard_keymap = false;)
printall();
}

void ReferenceResolver::reset_siblings_range_(evt_size beg, evt_size end)
{
    id_type sbeg = static_cast<id_type>(beg);
    id_type send = static_cast<id_type>(end);
    rymlassertb_(sbeg <= send);
    rymlassertb_(sbeg <= m_siblings.size());
    rymlassertb_(send <= m_siblings.size());
    for( ; sbeg < send; ++sbeg)
        m_siblings[sbeg] = nosibling;
}

C4_NODISCARD evt_size ReferenceResolver::next_sibling_memoized_(evt_size pos)
{
    rymlassertb_(pos < evts.len);
    evt_size sib = m_siblings[pos];
    return sib != nosibling ? sib : next_sibling_memoized_fill_(pos);
}

C4_NODISCARD evt_size ReferenceResolver::next_sibling_memoized_fill_(evt_size pos)
{
    return (m_siblings[pos] = !(evts.ptr[pos] & ievt::BEG_) ?
            ievt::nextpos(evts.ptr, pos) :
            (1 + matching_close_memoized_fill_(pos)));
}

C4_NODISCARD evt_size ReferenceResolver::matching_close_memoized_(evt_size pos)
{
    rymlassertb_(pos < evts.len);
    rymlassertb_(evts.ptr[pos] & ievt::BEG_);
    evt_size sib = m_siblings[pos];
    // subtract 1 because we have the sibling
    sib = (sib != nosibling) ? (sib-1) : matching_close_memoized_fill_(pos);
    rymlassertb_(evts.ptr[sib] & ievt::END_);
    return sib;
}

C4_NODISCARD evt_size ReferenceResolver::matching_close_memoized_fill_(evt_size pos)
{
    rymlassertb_(evts.ptr[pos] & ievt::BEG_);
    const evt_bits open = evts.ptr[pos] & detail::mask_open_close;
    const evt_bits close = (open & ~ievt::BEG_) | ievt::END_;
    ++pos; // skip the start event
    while(pos < m_wpos)
    {
        if(detail::hasall(evts.ptr[pos], close))
            break;
        evt_size & C4_RESTRICT sib = m_siblings[pos];
        if(sib == nosibling)
        {
            sib = !(evts.ptr[pos] & ievt::BEG_) ?
                ievt::nextpos(evts.ptr, pos) :
                (1 + matching_close_memoized_fill_(pos));
        }
        pos = sib;
    }
    rymlassertb_(detail::hasall(evts.ptr[pos], close));
    return pos;
}

void ReferenceResolver::resolve_resize(Buffers *evts_, ResolveOptions const& opts_, ResolveParams const& params)
{
    m_subject = evts_;
    resolve_(evts_, opts_, params);
    *m_subject = static_cast<Buffers const&>(*this);
    m_subject = nullptr;
}

void ReferenceResolver::resolve_(Buffers *evts_, ResolveOptions const& opts_, ResolveParams const& params)
{
    if(!evts_->evts.len)
        return;
    reset_(*evts_, opts_, params);
    m_anchor_map.clear();
    // exit early if there's nothing to do
    if(!(gather_anchors_and_check_has_refs_() ||
         m_opts.keep_repeated_keys() ||
         m_opts.merge_always() ||
         (m_opts.clear_anchors() && m_anchor_map.m_arr.empty())))
        return;
    // fill sibling data
    // ....
    // we need to resolve
    m_anchor_map.sort();
    // now go to all refs, and resolve them
    for(evt_size pos = 0; pos < evts.len; )
    {
wtf("root: %d", pos);
        if(detail::seqormap(evts.ptr[pos]))
        {
wtf("root: %d descend", pos);
            pos = descend_(pos);
        }
        else
        {
wtf("root: %d next", pos);
            if(evts.ptr[pos] & ievt::ALIA)
            {
                resolve_alia_(pos);
            }
            else if(detail::hasall(evts.ptr[pos], ievt::ESTR))
            {
                break;
            }
            pos = ievt::nextpos(evts.ptr, pos);
        }
    }
    // finally:
    evt_size orig = evts_->evts.len;
    if(m_opts.clear_anchors())
    {
        clear_anchors_(0, orig);
        if(m_wpos > orig)
            clear_anchors_(orig, m_wpos);
        m_anchor_map.clear();
    }
    // adjust for the sentinel
    m_wpos += (m_wpos > orig);
    evts.len = m_wpos;
    evts_->operator=(*this);
}

bool ReferenceResolver::gather_anchors_and_check_has_refs_()
{
    bool has_refs = false;
    for(evt_size pos = 0; pos < evts.len; pos = nextpos(evts.ptr, pos))
    {
        if(evts.ptr[pos] & ievt::ANCH)
            add_anchor_(pos);
        else if(evts.ptr[pos] & ievt::ALIA)
            has_refs = true;
        else if(detail::hasall(evts.ptr[pos], ievt::ESTR))
            break;
    }
    return has_refs;
}

void ReferenceResolver::clear_anchors_(evt_size beg, evt_size end)
{
    // go again through the evts array. previously this was done by
    // going through the anchor map, but it's very tricky to avoid
    // stale anchor data, and so very easy to clobber relevant data.
    for(evt_size pos = beg; pos < end; pos = nextpos(evts.ptr, pos))
        if(evts.ptr[pos] & ievt::ANCH)
            clear_str_(pos);
}

C4_NODISCARD evt_size ReferenceResolver::descend_(evt_size pos)
{
    rymlassertb_(detail::seqormap(evts.ptr[pos]));
    rymlassertb_(m_parents.empty() || m_parents.top() != pos);
    m_parents.emplace_top() = pos;
wtf("descend[%d]: enter -- ", pos); wtfparents();
    if(detail::hasall(evts.ptr[pos], ievt::BSEQ))
    {
        evt_size endpos = resolve_children_seq_recur_(pos);
wtf("descend[%d]: map endpos=%d", pos, endpos);
        pos = endpos;
    }
    if(detail::hasall(evts.ptr[pos], ievt::BMAP))
    {
        evt_size endpos = resolve_children_map_recur_(pos);
wtf("descend[%d]: map endpos=%d", pos, endpos);
        endpos = resolve_children_map_norecur_(pos);
wtf("descend[%d]: map endpos=%d", pos, endpos);
        pos = endpos;
    }
wtf("descend: exit %d at %d -- ", m_parents.top(), pos); wtfparents();
    m_parents.pop();
    return pos;
}

C4_NODISCARD evt_size ReferenceResolver::resolve_children_seq_recur_(evt_size pos)
{
    rymlassertb_(detail::hasall(evts.ptr[pos], ievt::BSEQ));
    for(++pos; pos < evts.len; )
    {
wtf("descend[%d]: seq recur ch %d", m_parents.top(), pos);
        if(detail::seqormap(evts.ptr[pos]))
        {
            pos = descend_(pos);
        }
        else
        {
            if(evts.ptr[pos] & ievt::ALIA)
            {
                resolve_alia_(pos);
            }
            else if(detail::hasall(evts.ptr[pos], ievt::ESEQ))
            {
                ++pos;
                break;
            }
            pos = next_sibling_memoized_(pos);
        }
    }
    return pos;
}

C4_NODISCARD evt_size ReferenceResolver::resolve_children_map_recur_(evt_size pos)
{
    rymlassertb_(detail::hasall(evts.ptr[pos], ievt::BMAP));
    for(++pos; pos < evts.len; )
    {
wtf("descend[%d]: map recur ch %d", m_parents.top(), pos);
        if(detail::seqormap(evts.ptr[pos]))
        {
wtf("descend[%d]: map recur ch %d -> descend", m_parents.top(), pos);
            pos = descend_(pos);
        }
        else if(!is_merge_elm_(pos))
        {
            if(evts.ptr[pos] & ievt::ALIA)
            {
wtf("descend[%d]: map recur ch %d -> alia", m_parents.top(), pos);
                resolve_alia_(pos);
            }
            else if(detail::hasall(evts.ptr[pos], EMAP))
            {
wtf("descend[%d]: map recur ch %d -> emap", m_parents.top(), pos);
                ++pos;
                break;
            }
            pos = next_sibling_memoized_(pos);
        }
        // skip merge elements (to be dealt with later)
        else
        {
wtf("descend[%d]: map recur ch %d -> merge elm", m_parents.top(), pos);
            rymlassertb_(detail::hasall(evts.ptr[pos], ievt::KEY_|ievt::SCLR));
            rymlassertb_(detail::hasall(evts.ptr[pos + 3], ievt::VAL_|ievt::ALIA)
                         ||
                         detail::hasall(evts.ptr[pos + 3], ievt::VAL_|ievt::BSEQ));
            pos = detail::hasall(evts.ptr[pos + 3], ievt::VAL_|ALIA) ?
                (pos + 6) : next_sibling_memoized_(pos + 3);
            continue;
        }
    }
    return pos;
}

C4_NODISCARD evt_size ReferenceResolver::resolve_children_map_norecur_(evt_size pos)
{
    rymlassertb_(detail::hasall(evts.ptr[pos], ievt::BMAP));
    rymlassert_not_recursive_(m_guard_resolving);
    // the call to requires_merge_() is expensive, so try to avoid it
    if(m_opts.keep_repeated_keys() || !requires_merge_(pos))
    {
        pos = resolve_map_repeat_(pos);
    }
    else
    {
        pos = resolve_map_merge_(pos);
    }
    return pos;
}

C4_NODISCARD evt_size ReferenceResolver::resolve_map_repeat_(evt_size pos)
{
    rymlassertb_(detail::hasall(evts.ptr[pos], ievt::BMAP));
    for(++pos; pos < evts.len; pos = next_sibling_memoized_(pos))
    {
        if(evts.ptr[pos] & ievt::ALIA)
        {
            resolve_alia_(pos);
        }
        else if(detail::hasall(evts.ptr[pos], ievt::KEY_|ievt::SCLR))
        {
            if(is_merge_elm_(pos))
            {
                if(is_any_merge_elm_alia_missing_(pos))
                {
                    ;
                }
                else
                {
                    handle_merge_elm_(pos);
                }
            }
        }
        else if(detail::hasall(evts.ptr[pos], ievt::EMAP))
        {
            ++pos;
            break;
        }
    }
    return pos;
}

C4_NODISCARD bool ReferenceResolver::requires_merge_(evt_size pos)
{
    rymlassert_not_recursive_(m_guard_keymap);
    rymlassertb_(detail::hasall(evts.ptr[pos], ievt::BMAP));
    rymlassertb_(pos + 1 < evts.len);
    // try to exit early
    if( ! m_opts.merge_always() && ! has_merge_elms_(pos))
        return false;
    // gather all the keys
    m_known_keys.start_adding();
    for(evt_size ch = pos + 1; ch < evts.len; ch = next_sibling_memoized_(ch))
    {
        if(detail::hasall(evts.ptr[ch], ievt::EMAP))
        {
            break;
        }
        // we're only interested on scalar keys
        else if(!detail::hasall(evts.ptr[ch],ievt::KEY_|ievt::SCLR))
        {
            continue;
        }
        // it's a key, scalar. add it!
        rymlassertb_(detail::hasnone(evts.ptr[ch], ievt::ALIA));
        rymlassertb_(detail::hasall(evts.ptr[ch], ievt::KEY_|ievt::SCLR));
        csubstr key = getstr(ch);
        if( ! is_merge_elm_(ch, key))
        {
            m_known_keys.add(key, static_cast<id_type>(ch), KnownKeys::ORIGINAL);
        }
        else if(is_any_merge_elm_alia_missing_(ch))
        {
            ;
        }
        else
        {
            Range_ rng = keyval_range_val_(ch);
            if(detail::hasall(evts.ptr[rng.beg], ievt::VAL_|ievt::ALIA))
            {
                add_to_known_keys_(rng.beg, KnownKeys::MERGEDIN);
            }
            else
            {
                rymlassertb_(rng.beg == ch + 3);
                rymlassertb_(detail::hasall(evts.ptr[ch + 3], ievt::BSEQ));
                ++rng.beg;
                while(true)
                {
                    rymlassertb_(evts.ptr[rng.beg] & ievt::ALIA);
                    add_to_known_keys_(rng.beg, KnownKeys::MERGEDIN);
                    rng.beg = nextpos(evts.ptr, rng.beg);
                    if(detail::hasall(evts.ptr[rng.beg], ievt::ESEQ))
                        break;
                }
            }
        }
    }
    m_known_keys.adding_is_complete();
    // now we can tell whether a merge is required
    if( ! m_known_keys.requires_merge())
        return false;
    // should we report an error on repeated keys (depending on opts)?
    KnownKeys::Entry const* first_badkey = m_known_keys.first_bad_key();
    if C4_LIKELY (!first_badkey)
        return m_known_keys.has_key_conflict();
    RYML_ERR_BASIC_CB_(m_anchor_map.m_arr.m_callbacks, "map at pos={} has repeated keys. first={}", pos, getstr(static_cast<evt_size>(first_badkey->id)));
}

void ReferenceResolver::add_to_known_keys_(evt_size beg, evt_size end, uint32_t status)
{
    rymlassertb_((evts.ptr[beg] & ievt::KEY_) || beg == end);
    rymlassertb_(!(status & KnownKeys::CONFLICT));
    while(beg < end)
    {
        if(detail::hasall(evts.ptr[beg], ievt::KEY_|ievt::SCLR))
            m_known_keys.add(getstr(beg), static_cast<id_type>(beg), status);
        beg = next_sibling_memoized_(beg);
    }
}

void ReferenceResolver::add_to_known_keys_(evt_size alia, uint32_t status)
{
    rymlassertb_(detail::hasall(evts.ptr[alia], ievt::VAL_|ievt::ALIA));
    AnchorData const* anch = lookup_anchor_(alia);
    if(anch)
    {
        if C4_LIKELY (detail::hasall(evts.ptr[anch->rng.beg], ievt::BMAP))
            add_to_known_keys_(anch->rng.beg + 1, anch->rng.end - 1, status);
        else
            RYML_ERR_BASIC_CB_(m_anchor_map.m_arr.m_callbacks, "merge element alias is not a map: '{}' at pos={}", getstr(alia), alia);
    }
}


C4_NODISCARD evt_size ReferenceResolver::resolve_map_merge_(evt_size parent)
{
wtf("----------------\n");
wtf("wtf: merge %d", parent);
printall();
    rymlassert_not_recursive_(m_guard_merging);
    rymlassertb_(detail::hasall(evts.ptr[parent], ievt::BMAP));
    rymlassertb_(!m_opts.keep_repeated_keys());
    // we'll copy the map data to a scratch space, and then overwrite
    // it by selectively copy from the scratch to in-place at the
    // map. if the merged data overflows the available space in the
    // map, new elements are added at the end of the evts buffer,
    // after the ESTR event.
    //
    // prepare to overwrite the map: copy data to scratch
    mapw_reset_(parent);
    // this is the range of new members added at the end (past the
    // ESTR event), when (if) the data overflows the original
    // space in the map.
    Range_ newrng;
    newrng.beg = m_wpos;
    // we're ready. iterate by unique key
    for(id_type i = 0; i < m_known_keys.m_unique_keys.m_size; ++i)
    {
wtf("[%d]: merging uk[%zu] -- posref=%d", parent, i, m_mapw_posref);
        KnownKeys::KnownKeyRange_ keyentries;
        KnownKeys::Entry const* selected = m_known_keys.select(i, &keyentries);
        rymlassertb_(keyentries.lo < keyentries.hi);
        // iterate all the key's candidates
        for(auto const& kv : keyentries)
        {
            evt_size id = static_cast<evt_size>(kv.val.id);
wtf("map=%d looking at id=%d", parent, id);
            bool is_last = (&kv + 1 == keyentries.hi);
wtf("[%d]: merging uk[%zu] -- posref=%d ... entry=%d", parent, i, m_mapw_posref, id);
            if(&kv.val == selected)
            {
wtf("[%d]: merging uk[%zu] -- posref=%d ... entry=%d selected", parent, i, m_mapw_posref, id);
                if(kv.val.status & KnownKeys::ORIGINAL)
{wtf("... selected, original ! %d", id);
                    mapw_cpkeyval_original_(id, is_last);
}
                else
{wtf("... selected, mergedin ! %d", id);
                    mapw_cpkeyval_mergedin_(id, is_last);
}
            }
        }
    }
    newrng.end = m_wpos;
    // did we spill the map to the end of the array?
    if(newrng.end > newrng.beg)
    {
        // insert a reference BEFORE the map ends
wtf("[%d]: posref=%d vs %d:%d", parent, m_mapw_posref, m_mapw_beg, m_mapw_end);
        rymlassertb_(m_mapw_posref != -1);
        rymlassertb_(m_mapw_posref >= m_mapw_beg);
        rymlassertb_(m_mapw_posref + 3 <= m_mapw_end);
        replace_with_ref_(m_mapw_posref, newrng.beg, newrng.end);
        m_mapw_count += 3;
        clear_range_(m_mapw_posref + 3, m_mapw_end);
    }
    // is there free space? clear it.
    else if(m_mapw_beg + m_mapw_count < m_mapw_end)
    {
wtf("reset[%d]:\n   %d:%d pos=%d/%d  clear  %d:%d,%d:%d(%d) wtf", parent, m_mapw_beg, m_mapw_end, m_mapw_count, m_mapw_beg+m_mapw_count, m_mapw_beg+m_mapw_count, m_mapw_end, m_mapw_count, (m_mapw_end-m_mapw_beg), (m_mapw_end-m_mapw_beg)-m_mapw_count);
printall();
        clear_range_(m_mapw_beg + m_mapw_count, m_mapw_end);
printall();
    }
    reset_siblings_range_(m_mapw_beg, m_mapw_end);
wtf("wtf: finished merge %d", parent);
printall();
    return m_mapw_end + 1;
}


void ReferenceResolver::mapw_reset_(evt_size parent)
{
    rymlassertb_(detail::hasall(evts.ptr[parent], ievt::BMAP));
    m_mapw_count = 0; // current write slot
    m_mapw_posref = -1;
    // this is the number of slots we have to write
    // in-place, ie between BMAP and EMAP:
    m_mapw_beg = parent + 1;
    m_mapw_end = matching_close_memoized_(parent);
wtf("reset[%d]: %d:%d=%d wtf", parent, m_mapw_beg, m_mapw_end, m_mapw_end-m_mapw_beg);
    rymlassertb_(m_mapw_end - m_mapw_beg > 3);
    // copy the data, +1 for including the sentinel event because of PEX2
    id_type sz = static_cast<id_type>((m_mapw_end + 1) - m_mapw_beg);
    m_mapw_scratch.resize(sz);
    memcpy(&m_mapw_scratch[0], evts.ptr + m_mapw_beg, sz * sizeof(evt_bits));
}

void ReferenceResolver::mapw_cpkeyval_original_(evt_size id, bool is_last_entry)
{
    rymlassertb_(id >= m_mapw_beg && id < m_mapw_end);
wtf("... orig id=%d(orig) / %d(scratch)", id, id - m_mapw_beg);
    evt_bits const* C4_RESTRICT tmp = &m_mapw_scratch[0];
    const Range_ r = keyval_range_scratch_(id - m_mapw_beg);
    evt_size copy_pos = mapw_cpdata_(tmp, static_cast<evt_size>(m_mapw_scratch.m_size), r.beg, r.end, is_last_entry);
    (void)copy_pos;
wtf("... orig copy_pos= %d:%d  -->  %d:%d [%d]", r.beg, r.end, copy_pos, copy_pos + r.end-r.beg, r.end-r.beg);
}

void ReferenceResolver::mapw_cpkeyval_mergedin_(evt_size id, bool is_last_entry)
{
    rymlassertb_(id < m_mapw_beg || id > m_mapw_end);
    const Range_ r = keyval_range_(id);
    mapw_cpdata_(evts.ptr, evts.cap, r.beg, r.end, is_last_entry);
}

evt_size ReferenceResolver::mapw_cpdata_(evt_bits const* C4_RESTRICT data, evt_size data_sz, evt_size beg, evt_size end, bool is_last_entry)
{
    rymlassertb_(beg < end);
    rymlassertb_(beg < data_sz || beg == end);
    rymlassertb_(end <= data_sz);(void)data_sz;
    evt_size sz = end - beg;
    rymlassertb_(sz);
    // leave room for a RREF before spilling to past ESTR
    evt_size extra = is_last_entry ? 0 : 3;
    evt_size dstpos = m_mapw_beg + m_mapw_count;
wtf("cp: %d:%d=%d  at  %d=%d, add %d=%d+%d", m_mapw_beg, m_mapw_end, m_mapw_end-m_mapw_beg, m_mapw_count, m_mapw_beg+m_mapw_count, sz+extra, sz, extra);
    if(dstpos + sz + extra < m_mapw_end)
    {
wtf("cp: in-place");
        rymlassertb_(data != &m_mapw_scratch[0] || static_cast<size_t>(beg) < m_mapw_scratch.size());
wtf("... orig cpinplace dst=%d:%d", dstpos, dstpos+sz);
        // we only need to copy if the data is going to end up
        // in a different position
        if(data != &m_mapw_scratch[0] || beg != m_mapw_count)
        {
wtf("... orig cpinplace dst=%d:%d --- doit!", dstpos, dstpos+sz);
            rymlassertb_(dstpos > 2);
            rymlassertb_(dstpos + sz < evts.len);
            const evt_bits xbeg = evts.ptr[dstpos] & ievt::PEX2;
            const evt_bits xend = data[end] & ievt::PEX2;
            memcpy(evts.ptr + dstpos, data + beg, static_cast<size_t>(sz) * sizeof(evt_bits));
            evts.ptr[dstpos] &= ~ievt::PEX2;
            evts.ptr[dstpos] |= xbeg;
            evts.ptr[dstpos + sz] &= ~ievt::PEX2;
            evts.ptr[dstpos + sz] |= xend;
        }
    }
    else
    {
wtf("cp: spill! dstpos=%d posref=%d", dstpos, m_mapw_posref);
//        // if there is no room to write the RREF, we need to shift
//        // elements to the end until we have room.
//        if(dstpos + 3 > m_mapw_end)
//        {
//            evt_size prev = dstpos;
//wtf("cp: shift! prev=%d dstpos=%d", prev, dstpos);
//            while(prev >= m_mapw_beg && prev + 3 >= m_mapw_end)
//                prev = prevpos(evts.ptr, prev);
//            rymlassertb_(prev >= m_mapw_beg);
//            rymlassertb_(prev + 3 < m_mapw_end);
//            copy_range_to_end_(evts.ptr, prev, dstpos);
//wtf("cp: shift! prev=%d dstpos=%d", prev, dstpos);
//            dstpos = prev;
//        }
        // save the ref position
        if(m_mapw_posref == -1)
        {
wtf("cp: posref=%d vs end=%d", dstpos, m_mapw_end);
            rymlassertb_(dstpos + 3 < m_mapw_end);
            m_mapw_posref = dstpos;
        }
        dstpos = m_wpos;
        copy_range_to_end_(data, beg, end);
    }
    m_mapw_count += sz;
    return dstpos;
}


C4_NODISCARD auto ReferenceResolver::elm_range_(evt_size elm) -> Range_
{
    rymlassertb_(detail::isentry(evts.ptr[elm]));
    Range_ rng;
    rng.beg = elm;
    if(evts.ptr[rng.beg] & (ievt::SCLR|ievt::ALIA)) // scalars
    {
        rng.end = rng.beg + 3;
    }
    else // containers
    {
        rymlassertb_(detail::seqormap(evts.ptr[elm]));
        rng.end = matching_close_memoized_(elm) + 1/*include the end*/;
    }
    return rng;
}

C4_NODISCARD auto ReferenceResolver::elm_range_scratch_(evt_size elm) -> Range_
{
    id_type selm = static_cast<id_type>(elm);
    rymlassertb_(detail::isentry(m_mapw_scratch[selm]));
    Range_ rng;
    rng.beg = elm;
    if(m_mapw_scratch[selm] & (ievt::SCLR|ievt::ALIA)) // scalars
    {
        rng.end = rng.beg + 3;
    }
    else // containers. don't use the memoized matching close!
    {
        rymlassertb_(detail::seqormap(m_mapw_scratch[selm]));
        rng.end = detail::find_matching_close_(&m_mapw_scratch[0], static_cast<evt_size>(m_mapw_scratch.size()), rng.beg)
            + 1/*include the end*/;
    }
    return rng;
}

C4_NODISCARD auto ReferenceResolver::keyval_range_(evt_size beg) -> Range_
{
    rymlassertb_(evts.ptr[beg] & ievt::KEY_);
    rymlassertb_(detail::isentry(evts.ptr[beg]));
    Range_ r = elm_range_(detail::find_next_entry_(evts.ptr, evts.len, beg, ievt::VAL_));
    r.beg = beg;
    return r;
}

C4_NODISCARD auto ReferenceResolver::keyval_range_scratch_(evt_size beg) -> Range_
{
wtf("beg=%d", beg);
    rymlassertb_(beg >= 0 && static_cast<id_type>(beg) < m_mapw_scratch.size());
    rymlassertb_(m_mapw_scratch[static_cast<id_type>(beg)] & ievt::KEY_);
    rymlassertb_(detail::isentry(m_mapw_scratch[static_cast<id_type>(beg)]));
    Range_ r = elm_range_scratch_(detail::find_next_entry_(&m_mapw_scratch[0], static_cast<evt_size>(m_mapw_scratch.size()), beg, ievt::VAL_));
    r.beg = beg;
    return r;
}

C4_NODISCARD auto ReferenceResolver::keyval_range_val_(evt_size beg) -> Range_
{
    rymlassertb_(evts.ptr[beg] & ievt::KEY_);
    rymlassertb_(detail::isentry(evts.ptr[beg]));
    return elm_range_(detail::find_next_entry_(evts.ptr, evts.len, beg, ievt::VAL_));
}

void ReferenceResolver::add_anchor_(evt_size pos)
{
    rymlassertb_(evts.ptr[pos] & ievt::ANCH);
    AnchorData& r = m_anchor_map.insert(getstr(pos));
    r.rng.beg = pos + (!(evts.ptr[pos + 3] & ievt::TAG_) ? 3 : 6);
    if(evts.ptr[r.rng.beg] & ievt::SCLR)
    {
        r.rng.end = r.rng.beg + 3;
    }
    else
    {
        rymlassertb_(evts.ptr[r.rng.beg] & ievt::BEG_);
        r.rng.end = next_sibling_memoized_(r.rng.beg); // END_ must be part of the range, so extend beyond it
    }
    r.visit_count = static_cast<id_type>(pos);
    r.removed = false;
}

void ReferenceResolver::rem_anchor_from_map_(evt_size pos)
{
    rymlassertb_(evts.ptr[pos] & ievt::ANCH);
    csubstr anch = getstr(pos);
    auto rng = m_anchor_map.equal_range_mut(anch);
    rymlassertb_(rng.hi > rng.lo);
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
    rymlassertb_(gotit);(void)gotit;
}

C4_NODISCARD bool ReferenceResolver::is_cyclic_ref_(evt_size anchor) const RYML_NOEXCEPT
{
    rymlassertb_(detail::hasall(evts.ptr[anchor], ievt::ANCH));
    const evt_size elm = detail::find_next_entry_(evts.ptr, evts.len, anchor, ievt::KEY_|ievt::VAL_);
    for(id_type i = 0; i < m_parents.size(); ++i)
        if(elm == m_parents.top(i))
            return true;
    return false;
}

void ReferenceResolver::resolve_alia_(evt_size pos)
{
    rymlassertb_(evts.ptr[pos] & ievt::ALIA);
    AnchorData const* anch = lookup_anchor_(pos);
    if(anch)
        set_rref_(pos, anch->rng.beg, anch->rng.end);
}

C4_NODISCARD auto ReferenceResolver::lookup_anchor_(evt_size pos) const
    -> AnchorData const*
{
    rymlassertb_(evts.ptr[pos] & ievt::ALIA);
    AnchorData const* anchor = yml::detail::lookup_anchor(m_anchor_map, static_cast<id_type>(pos), getstr(pos), m_opts);
    // reject cyclic references to prevent a blowout:
    bool cyclic = anchor && is_cyclic_ref_(static_cast<evt_size>(anchor->visit_count));
    // the branches are organized to keep happy cases first,
    // making this code a bit harder to read
    if C4_LIKELY (anchor && !cyclic) // happy path, vanilla situation
    {
        return anchor;
    }
    else if( ! anchor)
    {
        if(m_opts.ignore_missing_anchors()) // no anchor found, but that's ok
            return nullptr;
        else // no anchor found, and that's bad
            RYML_ERR_BASIC_CB_(m_anchor_map.m_arr.m_callbacks, "missing anchor: '{}' at ALIA[{}]", getstr(pos), pos);
    }
    else // anchor found, but it was cyclic
    {
wtf("cyclic reference\n"
    "'%.*s' at ALIA[%zu]: ANCH at %d", static_cast<int>(getstr(pos).len), getstr(pos).str,
    anchor->visit_count, pos);
        RYML_ERR_BASIC_CB_(m_anchor_map.m_arr.m_callbacks, "cyclic reference: '{}' at ALIA[{}]: ANCH at {}", getstr(pos), anchor->visit_count, pos);
    }
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
    rymlassertb_(detail::isentry(evts.ptr[pos]));
    if(evts.ptr[pos] & ievt::PEX2) // NOLINT
    {
        if(evts.ptr[pos - 3] & (ievt::ANCH|ievt::TAG_)) // NOLINT
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
    rymlassertb_(evts.ptr[pos] & ievt::ALIA);
    rymlassertb_(evts.ptr[pos + 3] & ievt::PEX2);
    evts.ptr[pos] &= ievt::PEX2; // keep only these flags (no unary ~ operator)
    evts.ptr[pos + 1] = 0;
    evts.ptr[pos + 2] = 0;
    evts.ptr[pos + 3] &= ~ievt::PEX2;
}

void ReferenceResolver::replace_with_ref_(evt_size pos, evt_size beg, evt_size end)
{
    evt_size extra = m_sz_limit ? count_extra(beg, end) : 0;
    m_newsz += extra;
    if C4_LIKELY (!m_sz_limit || m_newsz < m_sz_limit)
    {
        //maybe_clear_anchtag_(pos);
        evts.ptr[pos] = ievt::RREF | (evts.ptr[pos] & ievt::PEX2);
        evts.ptr[pos + 1] = beg;
        evts.ptr[pos + 2] = end;
        evts.ptr[pos + 3] |= ievt::PEX2;
    }
    else
    {
        RYML_ERR_BASIC_CB_(m_anchor_map.m_arr.m_callbacks, "aliasing would exceed max size limit");
    }
}

void ReferenceResolver::set_rref_(evt_size pos, evt_size beg, evt_size end)
{
    rymlassertb_(evts.ptr[pos] & ievt::ALIA);
    rymlassertb_(evts.ptr[pos + 3] & ievt::PEX2);
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

C4_NODISCARD bool ReferenceResolver::has_merge_elms_(evt_size pos)
{
    rymlassertb_(detail::hasall(evts.ptr[pos], ievt::BMAP));
    rymlassertb_(pos + 1 < evts.len);
    for(evt_size ch = pos + 1; true; ch = next_sibling_memoized_(ch))
    {
        rymlassertb_(ch < evts.len);
        if(detail::hasall(evts.ptr[ch], ievt::EMAP))
        {
            break;
        }
        else if(detail::hasall(evts.ptr[ch], ievt::KEY_|ievt::SCLR)) // NOLINT
        {
            if(is_merge_elm_(ch))  // is it an element like `<<: *ref` or `<<: [*ref1,*ref2]`?
                return true;
        }
    }
    return false;
}

// is it an element like `<<: *ref` or `<<: [*ref1,*ref2]`?
bool ReferenceResolver::is_merge_elm_(evt_size pos) const
{
    return detail::hasall(evts.ptr[pos], ievt::KEY_|ievt::SCLR)
        && is_merge_elm_(pos, getstr(pos));
}
// is it an element like `<<: *ref` or `<<: [*ref1,*ref2]`?
bool ReferenceResolver::is_merge_elm_(evt_size pos, csubstr key) const
{
    rymlassertb_(detail::hasall(evts.ptr[pos], ievt::KEY_|ievt::SCLR));
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
                    RYML_ERR_BASIC_CB_(m_anchor_map.m_arr.m_callbacks, "invalid reference at pos={}", pos);

            }
        }
        return isref;
    }
    return false;
}

bool ReferenceResolver::is_any_merge_elm_alia_missing_(evt_size pos) const
{
    rymlassertb_(is_merge_elm_(pos));
    if(!m_opts.ignore_missing_anchors())
        return false;
    pos += 3;
    if(detail::hasall(evts.ptr[pos], ievt::VAL_|ievt::ALIA))
    {
        if( ! lookup_anchor_(pos))
            return true;
    }
    else if(detail::hasall(evts.ptr[pos], ievt::VAL_|ievt::BSEQ))
    {
        ++pos;
        while(!detail::hasall(evts.ptr[pos], ievt::ESEQ))
        {
            if((evts.ptr[pos] & ievt::ALIA) && ! lookup_anchor_(pos))
                return true;
            pos = nextpos(evts.ptr, pos);
        }
    }
    return false;
}


void ReferenceResolver::handle_merge_elm_(evt_size child)
{
    // clear any anchors or tags immediately preceding the << scalar
    maybe_clear_anchtag_(child);
    rymlassertb_(detail::hasall(evts.ptr[child], ievt::KEY_|ievt::SCLR));
    rymlassertb_("<<" == getstr(child));
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
    rymlassertb_(detail::hasall(evts.ptr[pos], ievt::KEY_|ievt::SCLR));
    rymlassertb_(detail::hasall(evts.ptr[pos + 3], ievt::VAL_|ievt::ALIA));
    rymlassertb_("<<" == getstr(pos));
    clear_str_(pos); // clear the << scalar
    apply_merge_ref_(pos + 3);
}

void ReferenceResolver::merge_map_multiple_(evt_size pos)
{
    rymlassertb_(detail::hasall(evts.ptr[pos], ievt::KEY_|ievt::SCLR));
    rymlassertb_(detail::hasall(evts.ptr[pos + 3], ievt::VAL_|ievt::BSEQ));
    rymlassertb_(detail::hasall(evts.ptr[pos + 4], ievt::ALIA));
    rymlassertb_("<<" == getstr(pos));
    clear_str_(pos); // clear the << scalar
    evts.ptr[pos + 3] = 0; // clear the BSEQ
    pos += 4;
    while(true)
    {
        rymlassertb_(evts.ptr[pos] & ievt::ALIA);
        evt_size next = nextpos(evts.ptr, pos);
        apply_merge_ref_(pos);
        pos = next;
        if(detail::hasall(evts.ptr[next], ievt::ESEQ))
            break;
    }
    rymlassertb_(detail::hasall(evts.ptr[pos], ievt::ESEQ));
    // clear the ESEQ
    evts.ptr[pos] &= ievt::PEX2; // keep only these flags (no unary ~ operator)
}

void ReferenceResolver::apply_merge_ref_(evt_size pos)
{
    C4_SUPPRESS_WARNING_PUSH
    C4_SUPPRESS_WARNING_CLANG("-Wnull-dereference")
    #if defined(__GNUC__) && (__GNUC__ >= 6)
    C4_SUPPRESS_WARNING_GCC("-Wnull-dereference")
    #endif
    AnchorData const* anch = lookup_anchor_(pos);
    rymlassertb_(anch); // we require this has been checked before
    Range_ rng = anch->rng;
    rymlassertb_(detail::hasall(evts.ptr[rng.beg], ievt::BMAP)); // ditto
    rymlassertb_(detail::hasall(evts.ptr[rng.end - 1], ievt::EMAP));
    ++rng.beg; // skip BMAP
    --rng.end; // skip EMAP
    if(rng.beg == rng.end) // src map is empty?
        set_noref_(pos);
    else
        set_rref_(pos, rng.beg, rng.end);
    C4_SUPPRESS_WARNING_POP
}


void ReferenceResolver::clear_range_(evt_size beg, evt_size end) noexcept
{
    rymlassertb_(beg <= end);
    rymlassertb_(beg <= evts.len);
    rymlassertb_(end + 1 < evts.len);
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
    rymlassertb_(detail::hasall(evts.ptr[keyval], ievt::KEY_|ievt::SCLR));
    rymlassertb_(keyval <= evts.len);
    const Range_ r = keyval_range_(keyval);
    clear_range_(r.beg, r.end);
}

void ReferenceResolver::copy_range_to_end_(evt_bits const* mem, evt_size beg, evt_size end)
{
    const evt_size num = end - beg;
wtf("copy_range_to_end: %d | %d:%d <- %d:%d", evts.len, m_wpos, m_wpos+num, beg, end);
prrng(evts.ptr, m_wpos, m_wpos+num);
    rymlassertb_(num);
    evt_bits const* orig = evts.ptr;
    if(grow_evts_to_add_(num))
    {
        if(mem == orig)
            mem = evts.ptr;
        const evt_bits xbeg = evts.ptr[m_wpos] & ievt::PEX2;
        const evt_bits xend = mem[end] & ievt::PEX2;
        memcpy(evts.ptr + m_wpos, mem + beg, static_cast<size_t>(num) * sizeof(evt_size));
        evts.ptr[m_wpos] &= ~ievt::PEX2;
        evts.ptr[m_wpos] |= xbeg;
        evts.len += num;
        rymlassertb_(evts.cap > evts.len); // there must be room for the sentinel
        evts.ptr[evts.len] = xend;
prrng(evts.ptr, m_wpos, m_wpos+num+1);
    }
    m_wpos += num;
}

C4_SUPPRESS_WARNING_PUSH
C4_SUPPRESS_WARNING_MSVC(4702) // unreachable code
bool ReferenceResolver::grow_evts_to_add_(evt_size extra)
{
    evt_size slack = evts.cap - m_wpos - 1;
    if(slack >= extra)
    {
        return true;
    }
    else /*if(can_resize)*/ // NOLINT
    {
        evt_size newcap = m_wpos + extra + 1;
        evt_size nextcap = (3 * evts.cap) / 2; // do not grow too quick
        newcap = newcap > nextcap ? newcap : nextcap;
        m_subject->evts = evts = detail::resize(evts, newcap, m_known_keys.m_known_keys.m_arr.m_callbacks);
        m_siblings.resize(static_cast<id_type>(evts.cap));
        return true;
    }
    return false; // to be implemented later
}
C4_SUPPRESS_WARNING_POP

#undef rymlassertb_
#undef rymlassert_not_recursive_

} // namespace ievt
} // namespace extra
} // namespace yml
} // namespace c4

// NOLINTEND(*-signed-bitwise)
C4_SUPPRESS_WARNING_POP
