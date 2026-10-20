#ifndef C4_YML_EXTRA_EVENT_INTS_HPP_
#include "c4/yml/extra/event_ints.hpp"
#endif
#ifndef C4_YML_EMIT_OPTIONS_HPP_
#include "c4/yml/emit_options.hpp"
#endif
#ifndef C4_YML_SCALAR_CHARCONV_HPP_
#include "c4/yml/scalar_charconv.hpp"
#endif
#ifndef C4_YML_SCALAR_STYLE_HPP_
#include "c4/yml/scalar_style.hpp"
#endif
#ifndef C4_YML_RESOLVE_OPTIONS_HPP_
#include "c4/yml/resolve_options.hpp"
#endif
#ifndef C4_YML_DETAIL_SMALL_MAP_HPP_
#include "c4/yml/detail/small_map.hpp"
#endif

// NOLINTBEGIN(hicpp-signed-bitwise)

namespace c4 {
namespace yml {
namespace extra {
namespace ievt {

RYML_EXPORT evt_size estimate_events_size(csubstr src)
{
    evt_size count = 7; // BSTR + BDOC + =VAL + EDOC + ESTR
    for(size_t i = 0; i < src.len; ++i)
    {
        switch(src.str[i])
        {
        case ':': // this has strings preceding/following it
        case ',': // overestimate, assume map
        case '%': // assume TAGD->string + TAGV->string
            count += 6;
            break;
        // these have (or are likely to have) a string following it
        case '-':
        case '&':
        case '*':
        case '<':
        case '!':
        case '\'':
        case '"':
        case '|':
        case '>':
        case '\n':
            count += 3;
            break;
        case '[':
        case ']':
            count += 4;
            break;
        case '{':
        case '}':
            count += 7;
            break;
        case '?':
            count += 5;
            break;
        }
    }
    return count;
}


namespace detail {

bool has_next_doc_and_is_expl_(evt_bits const* C4_RESTRICT evts, evt_size evts_size, evt_size pos) RYML_NOEXCEPT
{
    RYML_ASSERT_BASIC_(pos < evts_size);
    RYML_ASSERT_BASIC_(evts[pos] & ievt::EDOC);
    while(pos < evts_size)
    {
        evt_bits evt = evts[pos];
        if(detail::hasall(evt, ievt::BDOC))
            return (evt & ievt::FLOW);
        else if(detail::hasall(evt, ievt::ESTR))
            break;
        pos += ievt::nextstep(evt);
    }
    return false;
}


evt_bits get_all_bits_key_or_val_(evt_bits const* C4_RESTRICT evts, evt_size evts_size, evt_size pos) RYML_NOEXCEPT
{
    evt_bits accum = {};
    const evt_bits stop = (evts[pos] & ievt::KEY_) ? ievt::VAL_ : ievt::KEY_;
    for( ; pos < evts_size; pos = ievt::nextpos(evts, pos))
    {
        if(evts[pos] & stop)
            break;
        accum |= evts[pos];
    }
    return accum;
}


evt_size find_matching_open_(evt_bits const* C4_RESTRICT evts, evt_size pos)
{
    evt_bits evt = evts[pos];
    RYML_ASSERT_BASIC_((evt & ievt::END_) && (evt & (mask_open_close & ~ievt::END_)));
    RYML_ASSERT_BASIC_((evt & mask_begend) != mask_begend);
    const evt_bits close = evt & mask_open_close;
    const evt_bits open = (close & ~ievt::END_) | ievt::BEG_;
    pos -= ievt::prevstep(evt); // don't count the starting close token
    uint32_t count = 0;
    while(pos >= 0)
    {
        evt = evts[pos];
        if((evt & close) == close)
        {
            ++count;
        }
        else if((evt & open) == open)
        {
            if(!(count--))
                return pos;
        }
        pos -= ievt::prevstep(evt);
    }
    RYML_ERR_BASIC_("evt error"); // LCOV_EXCL_LINE
}


evt_size find_matching_close_(evt_bits const* C4_RESTRICT evts, evt_size sz, evt_size pos)
{
    evt_bits evt = evts[pos];
    RYML_ASSERT_BASIC_((evt & ievt::BEG_) && (evt & (mask_open_close & ~ievt::BEG_)));
    RYML_ASSERT_BASIC_((evt & mask_begend) != mask_begend);
    const evt_bits open = evt & mask_open_close;
    const evt_bits close = (open & ~ievt::BEG_) | ievt::END_;
    pos += ievt::nextstep(evt); // don't count the starting close token
    uint32_t count = 0;
    while(pos < sz)
    {
        evt = evts[pos];
        if((evt & open) == open)
        {
            ++count;
        }
        else if((evt & close) == close)
        {
            if(!(count--))
                return pos;
        }
        pos += ievt::nextstep(evt);
    }
    RYML_ERR_BASIC_("evt error"); // LCOV_EXCL_LINE
}


evt_size find_next_entry_(evt_bits const* C4_RESTRICT evts, evt_size sz, evt_size pos, evt_bits key_or_val)
{
    evt_bits evt = evts[pos];
    if(detail::seqormap(evt & ~ievt::STRM))
    {
        if(evt & key_or_val)
            return pos;
        pos = find_matching_close_(evts, sz, pos);
    }
    while(pos < sz)
    {
        evt = evts[pos];
        if((evt & key_or_val) && detail::isentry(evt))
            return pos;
        pos += ievt::nextstep(evt);
    }
    RYML_ERR_BASIC_("evt error"); // LCOV_EXCL_LINE
}


MaybeParent find_parent_(evt_bits const* C4_RESTRICT evts, evt_size pos) noexcept
{
    MaybeParent p{0};
    C4_STATIC_ASSERT(std::is_signed<evt_size>::value);
    pos -= ievt::prevstep(evts[pos]);
    uint32_t count = 0;
    while(pos > 0)
    {
        const evt_bits evt = evts[pos];
        if(evt & ievt::END_)
        {
            ++count;
        }
        else if(evt & ievt::BEG_)
        {
            if(evt & (detail::mask_seqmap))
            {
                if(!(count--))
                {
                    p.pos = pos;
                    break;
                }
            }
        }
        pos -= ievt::prevstep(evt);
    }
    return p;
}


bool is_cyclic_ref_(evt_bits const* C4_RESTRICT evts, evt_size sz, evt_size ref_pos, evt_size anchor_pos) noexcept
{
    RYML_ASSERT_BASIC_(evts[ref_pos] & ievt::ALIA);
    RYML_ASSERT_BASIC_(evts[anchor_pos] & ievt::ANCH);
    evt_size anchor_element_pos = find_next_entry_(evts, sz, anchor_pos, evts[anchor_pos] & (ievt::KEY_|ievt::VAL_));
    MaybeParent par = find_parent_(evts, ref_pos);
    while(par)
    {
        if(par.pos == anchor_element_pos)
            return true;
        par = find_parent_(evts, par.pos);
    }
    return false;
}

EmitKickoff kickoff_emit(evt_bits const* evts, evt_size sz, evt_size pos, EmitOptions const& m_opts)
{
    EmitKickoff ek = {};
    ek.parent = detail::find_parent_(evts, pos);
    RYML_ASSERT_BASIC_(!ek.parent || detail::seqormap(evts[ek.parent.pos]));
    ek.emit_key = m_opts.emit_nonroot_key() && ek.parent && detail::hasall(evts[ek.parent.pos], ievt::BMAP) && (evts[pos] & ievt::KEY_);
    ek.emit_dash = m_opts.emit_nonroot_dash() && ek.parent && detail::hasall(evts[ek.parent.pos], ievt::BSEQ);
    RYML_ASSERT_BASIC_(!(ek.emit_key && ek.emit_dash));
    const evt_bits evt = evts[pos];
    if(ek.emit_key)
    {
        RYML_ASSERT_BASIC_(evt & KEY_);
        ek.keypos = detail::find_next_entry_(evts, sz, pos, ievt::KEY_);
        ek.valpos = detail::find_next_entry_(evts, sz, ek.keypos, ievt::VAL_);
        RYML_ASSERT_BASIC_(ek.keypos < sz);
        RYML_ASSERT_BASIC_(ek.keypos < ek.valpos);
    }
    else
    {
        ek.valpos = pos;
        if(!(detail::isentry(evt) ||
             detail::hasall(evt, ievt::BSTR) ||
             detail::hasall(evt, ievt::BDOC)))
           ek.valpos = detail::find_next_entry_(evts, sz, pos,
                                                m_opts.emit_nonroot_key() ?
                                                ievt::VAL_ : ievt::KEY_|ievt::VAL_);
    }
    RYML_ASSERT_BASIC_(ek.valpos < sz);
    return ek;
}

evt_bits scalar_style_choose_json_ievt(csubstr scalar) noexcept
{
    // see also NodeType implementation in scalar_style.cpp
    // do not quote numbers or special scalars
    return scalar_is_plain_number_json(scalar)
        || scalar_is_special_json(scalar) ? ievt::PLAI : ievt::DQUO;
}
evt_bits scalar_style_choose_block_ievt(csubstr scalar) noexcept
{
    // see also NodeType implementation in scalar_style.cpp
    if(scalar.len)
    {
        if(scalar_style_query_plain_block(scalar))
            return ievt::PLAI;
        else if(scalar_style_query_squo(scalar))
            return ievt::SQUO;
        return ievt::DQUO;
    }
    return scalar.str ? ievt::SQUO : ievt::PLAI;
}
evt_bits scalar_style_choose_flow_ievt(csubstr scalar) noexcept
{
    // see also NodeType implementation in scalar_style.cpp
    if(scalar.len)
    {
        if(scalar_style_query_plain_flow(scalar))
            return ievt::PLAI;
        else if(scalar_style_query_squo(scalar))
            return ievt::SQUO;
        return ievt::DQUO;
    }
    return scalar.str ? ievt::SQUO : ievt::PLAI;
}


//-----------------------------------------------------------------------------

namespace {

struct ReferenceResolver
{
private:
    struct AnchorTargetRange_
    {
        evt_size beg; /// beginning pos of the range
        evt_size end; /// end pos of the range (past 1)
        evt_size pos; /// anchor pos
    };
    template<class K, class V>
    using map = yml::detail::small_map<K,V>;

private:

    csubstr src;
    csubstr arena;
    evtbuf  evts;
    evt_size wpos;
    evt_size newsz;
    evt_size sz_limit;
    ResolveOptions opts;
    map<csubstr, AnchorTargetRange_> anchor_map;
    map<csubstr, evt_size> merge_map;

public:

    ReferenceResolver(evtbuf evts_, csubstr src_, csubstr arena_, ResolveOptions opts_={}) noexcept
        : src(src_)
        , arena(arena_)
        , evts(evts_)
        , wpos(evts_.len)
        , newsz(evts_.len)
        , sz_limit(evts_.len * static_cast<evt_size>(opts_.max_size_multiplier()))
        , opts(opts_)
        , anchor_map()
        , merge_map()
    {
    }

    void resolve()
    {
        if(!evts.len)
            return;
        // gather all the anchors
        anchor_map.clear();
        for(evt_size pos = 0; evts.ptr[pos] != ievt::ESTR; pos = nextpos(evts.ptr, pos))
            if(evts.ptr[pos] & ievt::ANCH)
                add_anchor_(pos);
        anchor_map.sort();
        // now go to all refs, and resolve them
        for(evt_size pos = 0; evts.ptr[pos] != ievt::ESTR; pos = nextpos(evts.ptr, pos))
        {
            if(descend_(pos))
            {
                ;
            }
        }
        // finally:
        if(opts.clear_anchors())
            for(auto const& entry : anchor_map.m_arr)
                clear_str_(entry.val.pos);
    }


private:

    C4_NODISCARD bool descend_(evt_size &pos)
    {
        bool ret = false;
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
        return ret;
    }

    C4_NODISCARD evt_size resolve_children_seq_(evt_size pos)
    {
        RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[pos], ievt::BSEQ));
        ++pos;
        for( ; evts.ptr[pos] != ievt::ESTR; pos = nextpos(evts.ptr, pos))
        {
            if(evts.ptr[pos] & ievt::ALIA)
            {
                AnchorTargetRange_ rng = lookup_anchor_(pos);
                set_rref_to_anchor_(pos, rng);
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

    C4_NODISCARD evt_size resolve_children_map_(evt_size parent)
    {
        RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[parent], ievt::BMAP));
        merge_map.clear();
        for(evt_size child = parent + 1; evts.ptr[child] != ievt::ESTR; child = detail::find_next_sibling(evts.ptr, evts.len, child))
        {
            if(evts.ptr[child] & ievt::ALIA)
            {
                AnchorTargetRange_ rng = lookup_anchor_(child);
                set_rref_to_anchor_(child, rng);
            }
            else if(detail::hasall(evts.ptr[child], ievt::KEY_|ievt::SCLR))
            {
                csubstr key = getstr_(child);
                // is it a merge key such as `<<: *ref` or `<<: [*ref1,*ref2]`
                if(is_merge_(child, key))
                    merge_elements_(child);
                else
                    merge_map.insert(key) = child;
            }
            else if(detail::hasall(evts.ptr[child], ievt::EMAP))
            {
                break;
            }
        }
        evt_size pos = parent + 1;
        for( ; evts.ptr[pos] != ievt::ESTR; pos = detail::find_next_sibling(evts.ptr, evts.len, pos))
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

    C4_ALWAYS_INLINE csubstr getstr_(evt_size pos) const RYML_NOEXCEPT
    {
        csubstr str = !(evts.ptr[pos] & ievt::AREN) ? src : arena;
        RYML_ASSERT_BASIC_(evts.ptr[pos] & ievt::WSTR);
        RYML_ASSERT_BASIC_(static_cast<size_t>(evts.ptr[pos + 1]) <= str.len);
        RYML_ASSERT_BASIC_(static_cast<size_t>(evts.ptr[pos + 1] + evts.ptr[pos + 2]) <= str.len);
        str.str = str.str + evts.ptr[pos + 1];
        str.len = static_cast<size_t>(evts.ptr[pos + 2]);
        return str;
    }

    void add_anchor_(evt_size pos)
    {
        RYML_ASSERT_BASIC_(evts.ptr[pos] & ievt::ANCH);
        AnchorTargetRange_& r = anchor_map.insert(getstr_(pos));
        r.beg = pos + (!(evts.ptr[pos + 3] & ievt::TAG_) ? 3 : 6);
        if(evts.ptr[r.beg] & ievt::SCLR)
        {
            r.end = r.beg + 3;
        }
        else
        {
            RYML_ASSERT_BASIC_(evts.ptr[r.beg] & ievt::BEG_);
            r.end = detail::find_matching_close_(evts.ptr, evts.len, r.beg);
            r.end = nextpos(evts.ptr, r.end); // END_ must be part of the range, so extend beyond it
        }
        r.pos = pos;
    }
    AnchorTargetRange_ lookup_anchor_(evt_size pos) const
    {
        RYML_ASSERT_BASIC_(evts.ptr[pos] & ievt::ALIA);
        csubstr ref = getstr_(pos);
        AnchorTargetRange_ const* r = anchor_map.lookup(ref);
        if C4_LIKELY (!!r)
        {
            RYML_ASSERT_BASIC_(getstr_(r->pos) == ref);
            if C4_LIKELY (!detail::is_cyclic_ref_(evts.ptr, evts.len, pos, r->pos))
                 return *r;
            RYML_ERR_BASIC_("cyclic reference: '{}' at ALIA[{}]: ANCH at {}", ref, r->pos, pos);
        }
        RYML_ERR_BASIC_("anchor not found: reference at ALIA[{}]: {}", pos, ref);
    }
    evt_size count_extra(evt_size beg, evt_size end) const noexcept
    {
        evt_size count = end - beg;
        for(evt_size pos = beg; pos < end; pos = nextpos(evts.ptr, pos))
            if(detail::hasall(evts.ptr[pos], ievt::RREF))
                count += count_extra(evts.ptr[pos+1], evts.ptr[pos+2]);
        return count;
    }
    void set_rref_to_anchor_(evt_size pos, AnchorTargetRange_ rng)
    {
        RYML_ASSERT_BASIC_(evts.ptr[pos] & ievt::ALIA);
        evt_size extra = sz_limit ? count_extra(rng.beg, rng.end) : 0;
        newsz += extra;
        if C4_LIKELY (!sz_limit || newsz < sz_limit)
        {
            evts.ptr[pos] &= ~ALIA;
            evts.ptr[pos] |= RREF;
            evts.ptr[pos + 1] = rng.beg;
            evts.ptr[pos + 2] = rng.end;
            evts.ptr[pos + 3] &= ~PEX2;
            evts.ptr[pos + 3] |= PEX2;
        }
        else
        {
            RYML_ERR_BASIC_("aliasing would exceed max size limit");
        }
    }
    void clear_str_(evt_size pos) // NOLINT
    {
        evts.ptr[pos] &= ievt::KEY_|ievt::VAL_|ievt::PEX2; // keep only these flags (no unary ~ operator) // NOLINT
        evts.ptr[pos + 2] = evts.ptr[pos + 1] = ((ievt::KEY_|ievt::VAL_) & evts.ptr[pos]);
        evts.ptr[pos + 3] &= ~ievt::PEX2;
    }

    bool is_merge_(evt_size pos, csubstr key) const
    {
        RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[pos], ievt::KEY_|ievt::SCLR));
        if(!(evts.ptr[pos] & ievt::PLAI) || key != "<<")
            return false;
        pos += 3;
        if(detail::hasall(evts.ptr[pos], ievt::VAL_|ievt::ALIA))
            return true;
        if(detail::hasall(evts.ptr[pos], ievt::VAL_|ievt::BSEQ))
        {
            bool isref = false;
            ++pos;
            while(true)
            {
                if(evts.ptr[pos] & ALIA)
                {
                    isref = true;
                    pos += 3;
                }
                else if(evts.ptr[pos] & ESEQ)
                    break;
                else if C4_UNLIKELY (isref)
                    RYML_ERR_BASIC_("invalid reference at pos={}", pos);
            }
            return isref;
        }
        return false;
    }

    void merge_elements_(evt_size child)
    {
        // clear any anchors or tags immediately preceding the << scalar
        if(evts.ptr[child] & ievt::PEX2)
        {
            if(evts.ptr[child - 3] & (ievt::ANCH|ievt::TAG_))
            {
                if(detail::hasall(evts.ptr[child - 3], ievt::TAG_|ievt::PEX2))
                    if(evts.ptr[child - 6] & ievt::ANCH)
                        clear_str_(child - 6);
                clear_str_(child - 3);
            }
        }
        if(detail::hasall(evts.ptr[child + 3], ievt::VAL_|ievt::ALIA))
        {
            merge_map_single_(child);
        }
        else if(detail::hasall(evts.ptr[child + 3], ievt::VAL_|ievt::BSEQ))
        {
            merge_map_multiple_(child);
        }
    }
    void merge_map_single_(evt_size pos)
    {
        RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[pos], ievt::KEY_|ievt::SCLR));
        RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[pos + 3], ievt::VAL_|ievt::ALIA));
        RYML_ASSERT_BASIC_("<<" == getstr_(pos));
        clear_str_(pos); // clear the << scalar
        apply_merge_ref_(pos + 3, /*more*/false);
    }
    void merge_map_multiple_(evt_size pos)
    {
        RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[pos], ievt::KEY_|ievt::SCLR));
        RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[pos + 3], ievt::VAL_|ievt::BSEQ));
        RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[pos + 4], ievt::ALIA));
        RYML_ASSERT_BASIC_("<<" == getstr_(pos));
        clear_str_(pos); // clear the << scalar
        evts.ptr[pos + 3] &= ievt::KEY_|ievt::VAL_|ievt::PEX2; // keep only these flags (no unary ~ operator)
        pos += 4;
        while(true)
        {
            RYML_ASSERT_BASIC_(evts.ptr[pos] & ievt::ALIA);
            evt_size next = nextpos(evts.ptr, pos);
            bool is_last = detail::hasall(evts.ptr[next], ievt::ESEQ);
            apply_merge_ref_(pos, /*more*/!is_last);
            pos = next;
            if(is_last)
                break;
        }
        RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[pos], ievt::ESEQ));
        evts.ptr[pos] &= ievt::PEX2; // keep only these flags (no unary ~ operator)
    }
    void apply_merge_ref_(evt_size pos, bool more)
    {
        AnchorTargetRange_ rng = lookup_anchor_(pos);
        if C4_LIKELY (detail::hasall(evts.ptr[rng.beg], ievt::BMAP))
        {
            RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[rng.end - 1], ievt::EMAP));
            ++rng.beg; // skip BMAP
            --rng.end; // skip EMAP
            bool copy_nonexisting = true;
            if(!needs_merge_(rng.beg, rng.end))
            {
                set_rref_to_anchor_(pos, rng);
                copy_nonexisting = false;
            }
            if(copy_nonexisting || more)
            {
                add_merge_elms_(rng.beg, rng.end, copy_nonexisting, more);
            }
        }
        else
        {
            RYML_ERR_BASIC_("invalid inheriting reference at pos={}", pos);
        }
    }
    bool needs_merge_(evt_size beg, evt_size end)
    {
        RYML_ASSERT_BASIC_(evts.ptr[beg] & ievt::KEY_);
        if(!merge_map.m_arr.empty())
        {
            merge_map.sort();
            while(beg < end)
            {
                if(detail::hasall(evts.ptr[beg], ievt::KEY_|ievt::SCLR))
                {
                    if(merge_map.lookup(getstr_(beg)))
                        return true;
                }
                beg = detail::find_next_sibling(evts.ptr, evts.len, beg);
            }
        }
        return false;
    }
    void add_merge_elms_(evt_size beg, evt_size end, bool copy_nonexisting, bool more_to_add)
    {
        RYML_ASSERT_BASIC_(evts.ptr[beg] & ievt::KEY_);
        while(beg < end)
        {
            if(detail::hasall(evts.ptr[beg], ievt::KEY_|ievt::SCLR))
            {
                csubstr key = getstr_(beg);
                if(!merge_map.lookup(key))
                {
                    if(more_to_add)
                        merge_map.insert(key) = beg;
                    if(copy_nonexisting)
                        copy_keyval_to_end_(beg);
                }
            }
            beg = detail::find_next_sibling(evts.ptr, evts.len, beg);
        }
    }
    void copy_keyval_to_end_(evt_size keyval)
    {
        RYML_ASSERT_BASIC_(detail::hasall(evts.ptr[keyval], ievt::KEY_|ievt::SCLR));
        evt_size val = detail::find_next_entry_(evts.ptr, evts.len, keyval, ievt::VAL_);
        RYML_ASSERT_BASIC_(evts.ptr[val] & ievt::VAL_);
        evt_size num;
        if(evts.ptr[val] & ievt::SCLR) // scalars
        {
            num = val + 3 - keyval;
        }
        else // containers
        {
            RYML_ASSERT_BASIC_(detail::seqormap(evts.ptr[val]));
            num = detail::find_matching_close_(evts.ptr, evts.len, val)
                + 1/*include the end*/
                - keyval;
        }
        if(grow_evts_to_add_(num))
        {
            memcpy(evts.ptr + wpos, evts.ptr + keyval, static_cast<size_t>(num) * sizeof(evt_size));
            evts.len += num;
        }
        wpos += num;
    }
    bool can_resize = false;
    bool grow_evts_to_add_(evt_size extra)
    {
        evt_size slack = evts.cap - wpos;
        if(slack > extra)
        {
            return true;
        }
        else if(can_resize)
        {
            evt_size newcap = wpos + extra;
            evt_size nextcap = (3 * evts.cap) / 2; // do not grow too quick
            newcap = newcap > nextcap ? newcap : nextcap;
            evts = yml::detail::resize(evts, newcap, merge_map.m_arr.m_callbacks);
            return true;
        }
        RYML_ERR_BASIC_("not implemented");
        return false;
    }
};
} // namespace anon
} // namespace detail



void resolve_refs(evt_bits * evts, evt_size sz, csubstr src, csubstr arena, ResolveOptions const& opts)
{
    evtbuf buf;
    buf.ptr = evts;
    buf.len = sz;
    buf.cap = sz; // FIXME!
    detail::ReferenceResolver r(buf, src, arena, opts);
    r.resolve();
}

} // namespace ievt
} // namespace extra


namespace detail {
C4_SUPPRESS_WARNING_GCC_CLANG_PUSH
C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")
C4_SUPPRESS_WARNING_GCC_CLANG("-Wcast-qual")
C4_SUPPRESS_WARNING_GCC("-Wsign-conversion")
extra::ievt::evtbuf resize(extra::ievt::evtbuf buf, extra::ievt::evt_size cap, Callbacks const& cb)
{
    RYML_ASSERT_BASIC_CB_(cb, buf.len <= buf.cap);
    if(cap > buf.cap)
    {
        buf.ptr = yml::detail::grow_buf(buf.ptr, (size_t)buf.len, (size_t)buf.cap, (size_t)cap, cb);
        buf.cap = cap;
        memset(buf.ptr + buf.len, 0, sizeof(buf.ptr[0]) * (size_t)(buf.cap - buf.len));
    }
    return buf;
}
substr resize(substr buf, size_t sz, Callbacks const& cb)
{
    if(sz > buf.len)
    {
        buf.str = yml::detail::grow_buf(buf.str, buf.len, sz, cb);
        buf.len = sz;
    }
    return buf;
}
C4_SUPPRESS_WARNING_GCC_CLANG_POP
} // namespace detail

// NOLINTEND(hicpp-signed-bitwise)

} // namespace yml
} // namespace c4
