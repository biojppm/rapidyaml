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

// NOLINTBEGIN(*-signed-bitwise)

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

evtbuf resize(evtbuf buf, evt_size cap, Callbacks const& cb)
{
    RYML_ASSERT_BASIC_CB_(cb, cap >= 0);
    RYML_ASSERT_BASIC_CB_(cb, buf.len >= 0);
    RYML_ASSERT_BASIC_CB_(cb, buf.len <= buf.cap);
    if(cap > buf.cap)
    {
        C4_SUPPRESS_WARNING_PUSH
        C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")
        C4_SUPPRESS_WARNING_GCC_CLANG("-Wcast-qual")
        C4_SUPPRESS_WARNING_GCC("-Wsign-conversion")
        buf.ptr = yml::detail::grow_buf(buf.ptr, (size_t)buf.len, (size_t)buf.cap, (size_t)cap, cb); // NOLINT
        buf.cap = cap;
        // FIXME: don't do this:
        memset(buf.ptr + buf.len, 0, sizeof(buf.ptr[0]) * (size_t)(buf.cap - buf.len)); // NOLINT
        C4_SUPPRESS_WARNING_POP
    }
    return buf;
}
evtbuf destroy(evtbuf buf, Callbacks const& cb)
{
    if(buf.ptr)
        cb.m_free(buf.ptr, static_cast<size_t>(buf.cap) * sizeof(evt_size), cb.m_user_data);
    return {};
}

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
    RYML_ASSERT_BASIC_(evts[pos] & (ievt::KEY_|ievt::VAL_));
    evt_bits accum = {};
    const evt_bits start = evts[pos] & (ievt::KEY_|ievt::VAL_);
    for( ; pos < evts_size; pos = ievt::nextpos(evts, pos))
    {
        accum |= evts[pos];
        if((evts[pos] & start) && detail::isentry(evts[pos]))
            break;
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

} // namespace detail
} // namespace ievt
} // namespace extra

// NOLINTEND(*-signed-bitwise)

} // namespace yml
} // namespace c4
