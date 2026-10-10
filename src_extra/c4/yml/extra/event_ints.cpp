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

// NOLINTBEGIN(hicpp-signed-bitwise)
// ensure the ievt flags work despite being signed
static_assert((ievt::MASK & ievt::BSTR) == ievt::BSTR, "overflow?");
static_assert((ievt::MASK & ievt::ESTR) == ievt::ESTR, "overflow?");
static_assert((ievt::MASK & ievt::BDOC) == ievt::BDOC, "overflow?");
static_assert((ievt::MASK & ievt::EDOC) == ievt::EDOC, "overflow?");
static_assert((ievt::MASK & ievt::BMAP) == ievt::BMAP, "overflow?");
static_assert((ievt::MASK & ievt::EMAP) == ievt::EMAP, "overflow?");
static_assert((ievt::MASK & ievt::BSEQ) == ievt::BSEQ, "overflow?");
static_assert((ievt::MASK & ievt::ESEQ) == ievt::ESEQ, "overflow?");
static_assert((ievt::MASK & ievt::ANCH) == ievt::ANCH, "overflow?");
static_assert((ievt::MASK & ievt::ALIA) == ievt::ALIA, "overflow?");
static_assert((ievt::MASK & ievt::TAG_) == ievt::TAG_, "overflow?");
static_assert((ievt::MASK & ievt::PLAI) == ievt::PLAI, "overflow?");
static_assert((ievt::MASK & ievt::SQUO) == ievt::SQUO, "overflow?");
static_assert((ievt::MASK & ievt::DQUO) == ievt::DQUO, "overflow?");
static_assert((ievt::MASK & ievt::LITL) == ievt::LITL, "overflow?");
static_assert((ievt::MASK & ievt::FOLD) == ievt::FOLD, "overflow?");
static_assert((ievt::MASK & ievt::FLOW) == ievt::FLOW, "overflow?");
static_assert((ievt::MASK & ievt::EXPL) == ievt::EXPL, "overflow?");
static_assert((ievt::MASK & ievt::UNFILT) == ievt::UNFILT, "overflow?");
static_assert((ievt::MASK & ievt::KEY_) == ievt::KEY_, "overflow?");
static_assert((ievt::MASK & ievt::VAL_) == ievt::VAL_, "overflow?");
static_assert((ievt::MASK & ievt::YAML) == ievt::YAML, "overflow?");
static_assert((ievt::MASK & ievt::TAGH) == ievt::TAGH, "overflow?");
static_assert((ievt::MASK & ievt::TAGP) == ievt::TAGP, "overflow?");
static_assert((ievt::MASK & ievt::AREN) == ievt::AREN, "overflow?");
static_assert((ievt::MASK & ievt::PSTR) == ievt::PSTR, "overflow?");
static_assert((ievt::MASK & ievt::WSTR) == ievt::WSTR, "overflow?");
static_assert((ievt::MASK & ievt::RREF) == ievt::RREF, "overflow?");
static_assert((ievt::MASK & ievt::PRREF) == ievt::PRREF, "overflow?");
static_assert((ievt::MASK & ievt::JUMP) == ievt::JUMP, "overflow?");
static_assert((ievt::MASK & ievt::PJUMP) == ievt::PJUMP, "overflow?");
static_assert((ievt::MASK & ievt::FSL_) == ievt::FSL_, "overflow?");
static_assert((ievt::MASK & ievt::FML1) == ievt::FML1, "overflow?");
static_assert((ievt::MASK & ievt::FMLN) == ievt::FMLN, "overflow?");
static_assert((ievt::MASK & ievt::FMLX) == ievt::FMLX, "overflow?");
static_assert((ievt::MASK & ievt::LAST) == ievt::LAST, "overflow?");


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


evt_size find_matching_open_(evt_bits const* C4_RESTRICT evts, evt_size pos) RYML_NOEXCEPT
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


evt_size find_matching_close_(evt_bits const* C4_RESTRICT evts, evt_size sz, evt_size pos) RYML_NOEXCEPT
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


evt_size find_next_entry_(evt_bits const* C4_RESTRICT evts, evt_size sz, evt_size pos, evt_bits key_or_val) RYML_NOEXCEPT
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
    //see also NodeType implementation in scalar_style.cpp
    // do not quote numbers or special scalars
    return scalar_is_plain_number_json(scalar)
        || scalar_is_special_json(scalar) ? ievt::PLAI : ievt::DQUO;
}
evt_bits scalar_style_choose_block_ievt(csubstr scalar) noexcept
{
    //see also NodeType implementation in scalar_style.cpp
    if(scalar.len)
    {
        if(scalar_style_query_plain_block(scalar))
            return ievt::PLAI;
        RYML_ASSERT_BASIC_(scalar_style_query_squo(scalar)
                           && "if this assertion fires, please submit an issue!");
        return ievt::SQUO;
    }
    return scalar.str ? ievt::SQUO : ievt::PLAI;
}
evt_bits scalar_style_choose_flow_ievt(csubstr scalar) noexcept
{
    //see also NodeType implementation in scalar_style.cpp
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

namespace {
csubstr getstr(evt_bits const* evts, csubstr src, csubstr arena, evt_size pos)
{
    csubstr str = !(evts[pos] & ievt::AREN) ? src : arena;
    RYML_ASSERT_BASIC_(static_cast<size_t>(evts[pos + 1]) <= str.len);
    RYML_ASSERT_BASIC_(static_cast<size_t>(evts[pos + 1] + evts[pos + 2]) <= str.len);
    str.str = str.str + evts[pos + 1];
    str.len = static_cast<size_t>(evts[pos + 2]);
    return str;
}
struct AnchorTargetRange
{
    evt_size beg; /// beginning pos of the range
    evt_size end; /// end pos of the range (past 1)
};
AnchorTargetRange make_range_from_alias(evt_bits const* evts, evt_size sz, evt_size pos)
{
    RYML_ASSERT_BASIC_(evts[pos] & ievt::ANCH);
    AnchorTargetRange r;
    r.beg = pos + 3;
    if(evts[r.beg] & ievt::SCLR)
    {
        r.end = r.beg + 3;
    }
    else
    {
        RYML_ASSERT_BASIC_(evts[r.beg] & ievt::BEG_);
        r.end = detail::find_matching_close_(evts, sz, r.beg);
        r.end = nextpos(evts, r.end); // END_ must be part of the range, so extend beyond it
    }
    return r;
}
AnchorTargetRange lookup_anchor(evt_bits const* evts, evt_size sz, csubstr src, csubstr arena, evt_size pos)
{
    // get the anchor
    RYML_ASSERT_BASIC_(evts[pos] & ievt::ALIA);
    evt_size spos = pos;
    csubstr ref = detail::getstr(evts, src, arena, pos);
    // lookup the anchor
    while(true)
    {
        pos = ievt::prevpos(evts, pos);
        if(evts[pos] & ievt::ANCH)
        {
            csubstr anch = detail::getstr(evts, src, arena, pos);
            if(anch == ref)
                return detail::make_range_from_alias(evts, sz, pos);
        }
        else if(((evts[pos] & BDOC) == BDOC) || ((evts[pos] & BSTR) == BSTR))
        {
            break;
        }
    }
    RYML_ERR_BASIC_("anchor not found at ALIA[{}]: {}", spos, ref);
}
void set_rref(evt_bits * evts, evt_size pos, AnchorTargetRange rng) RYML_NOEXCEPT
{
    RYML_ASSERT_BASIC_(evts[pos] & ievt::ALIA);
    evts[pos] &= ~ALIA;
    evts[pos] |= RREF;
    evts[pos + 1] = rng.beg;
    evts[pos + 2] = rng.end;
    evts[pos + 3] &= ~PSTR;
    evts[pos + 3] |= PRREF;
}
void clear_anchor(evt_bits * evts, evt_size pos)
{
    evts[pos] &= ievt::KEY_|ievt::VAL_|ievt::PEXTRA2; // keep only these flags (no unary ~ operator)
    evts[pos + 1] = 0;
    evts[pos + 2] = 0;
    evts[pos + 3] &= ~ievt::PSTR;
}
void apply_inheriting_ref_entry(evt_bits * evts, evt_size sz, csubstr src, csubstr arena, evt_size pos)
{
    detail::AnchorTargetRange rng = detail::lookup_anchor(evts, sz, src, arena, pos);
    if C4_LIKELY (detail::hasall(evts[rng.beg], ievt::BMAP))
    {
        RYML_ASSERT_BASIC_(detail::hasall(evts[rng.end - 1], ievt::EMAP));
        ++rng.beg; // skip BMAP
        --rng.end; // skip EMAP
        detail::set_rref(evts, pos, rng);
    }
    else
    {
        RYML_ERR_BASIC_("invalid inheriting reference at pos={}", pos);
    }
}
void apply_inheriting_ref(evt_bits * evts, evt_size sz, csubstr src, csubstr arena, evt_size pos)
{
    RYML_ASSERT_BASIC_(detail::hasall(evts[pos], ievt::KEY_|ievt::SCLR));
    RYML_ASSERT_BASIC_(detail::hasall(evts[pos + 3], ievt::VAL_|ievt::ALIA));
    RYML_ASSERT_BASIC_("<<" == detail::getstr(evts, src, arena, pos));
    detail::clear_anchor(evts, pos); // clear the << scalar
    apply_inheriting_ref_entry(evts, sz, src, arena, pos + 3);
}
void apply_inheriting_seq(evt_bits * evts, evt_size sz, csubstr src, csubstr arena, evt_size pos)
{
    RYML_ASSERT_BASIC_(detail::hasall(evts[pos], ievt::KEY_|ievt::SCLR));
    RYML_ASSERT_BASIC_(detail::hasall(evts[pos + 3], ievt::VAL_|ievt::BSEQ));
    RYML_ASSERT_BASIC_("<<" == detail::getstr(evts, src, arena, pos));
    detail::clear_anchor(evts, pos); // clear the << scalar
    pos += 3;
    // clear the seq
    evts[pos] &= ievt::KEY_|ievt::VAL_|ievt::PEXTRA2; // keep only these flags (no unary ~ operator)
    ++pos; // skip BSEQ
    while(true)
    {
        if C4_LIKELY (evts[pos] & ievt::ALIA)
        {
            apply_inheriting_ref_entry(evts, sz, src, arena, pos);
            pos = nextpos(evts, pos);
            if(detail::hasall(evts[pos], ievt::ESEQ))
                break;
        }
        else
        {
            RYML_ERR_BASIC_("invalid reference at pos={}", pos);
        }
    }
    RYML_ASSERT_BASIC_(detail::hasall(evts[pos], ievt::ESEQ));
    evts[pos] &= ievt::PEXTRA2; // keep only these flags (no unary ~ operator)
}
} // namespace anon
} // namespace detail



void resolve_refs(evt_bits * evts, evt_size sz, csubstr src, csubstr arena, bool clear_anchors)
{
    if(!sz) return;
    for(evt_size pos = 0; evts[pos] != ievt::ESTR; pos = nextpos(evts, pos))
    {
        if(evts[pos] & ievt::ALIA)
        {
            detail::AnchorTargetRange rng = detail::lookup_anchor(evts, sz, src, arena, pos);
            detail::set_rref(evts, pos, rng);
        }
        else if(detail::hasall(evts[pos], ievt::KEY_|ievt::SCLR))
        {
            csubstr key = detail::getstr(evts, src, arena, pos);
            if(key == "<<")
            {
                if(detail::hasall(evts[pos + 3], ievt::VAL_|ievt::ALIA))
                    detail::apply_inheriting_ref(evts, sz, src, arena, pos);
                else if(detail::hasall(evts[pos + 3], ievt::VAL_|ievt::BSEQ))
                    detail::apply_inheriting_seq(evts, sz, src, arena, pos);
            }
        }
    }
    if(clear_anchors)
    {
        for(evt_size pos = 0; evts[pos] != ESTR; pos = nextpos(evts, pos))
            if(evts[pos] & ANCH)
                detail::clear_anchor(evts, pos);
    }
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
