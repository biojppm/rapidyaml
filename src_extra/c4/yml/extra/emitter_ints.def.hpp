#ifndef C4_YML_EXTRA_EMITTER_INTS_DEF_HPP_
#define C4_YML_EXTRA_EMITTER_INTS_DEF_HPP_

/** @file emitter.def.hpp */

#ifndef C4_YML_EXTRA_EMITTER_INTS_HPP_
#include "c4/yml/extra/emitter_ints.hpp"
#endif
#ifndef C4_YML_ERROR_HPP_
#include "c4/yml/error.hpp"
#endif
#ifndef C4_YML_DETAIL_EMIT_SCALAR_HPP_
#include "c4/yml/detail/emit_scalar.hpp"
#endif


C4_SUPPRESS_WARNING_GCC_CLANG_WITH_PUSH("-Wold-style-cast")
C4_SUPPRESS_WARNING_GCC("-Wuseless-cast")
// NOLINTBEGIN(hicpp-signed-bitwise,*avoid-c-style-cast)

namespace c4 {
namespace yml {
namespace extra {
namespace ievt {

/** @cond dev */

namespace detail {
enum : evt_bits { // NOLINT
    styles_ievt_quot = ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD,
    styles_ievt_sclr = ievt::PLAI|styles_ievt_quot,
    styles_ievt_cont = ievt::BLCK|ievt::FLOW|ievt::FSL_|ievt::FML1|ievt::FMLN,
};
inline bool query_blck_cont_(evt_bits evt) noexcept
{
    return (evt & ievt::BLCK) || (!(evt & detail::styles_ievt_cont));
}
inline bool key_requires_qmark_block(evt_bits const* C4_RESTRICT evts, evt_size evts_size, evt_size pos) RYML_NOEXCEPT
{
    return get_all_bits_key(evts, evts_size, pos) & (detail::mask_seqmap|ievt::LITL|ievt::FOLD); // NOLINT
}
inline bool key_requires_qmark_flow(evt_bits const* C4_RESTRICT evts, evt_size evts_size, evt_size pos) RYML_NOEXCEPT
{
    return get_all_bits_key(evts, evts_size, pos) & (detail::mask_seqmap);
}
} // namespace detail


//-----------------------------------------------------------------------------

template<class Writer>
void EmitterInts<Writer>::emit_as(EmitType_e type,
                                  evt_bits const* evts,
                                  evt_size evts_size,
                                  evt_size pos,
                                  csubstr src,
                                  csubstr arena)
{
    RYML_ASSERT_BASIC_(evts || !evts_size);
    RYML_ASSERT_BASIC_(pos <= evts_size);
    if(!evts || !evts_size)
        return;
    m_evts = evts;
    m_evts_size = evts_size;
    m_src = src;
    m_arena = arena;
    m_col = 0;
    m_depth = 0;
    m_ilevel = 0;
    m_pws = PWS_NONE_;
    m_flow_pws = {};
    if(type == EMIT_YAML)
        emit_yaml_(pos);
    else if(type == EMIT_JSON)
        emit_json_(pos);
    else
        RYML_ERR_BASIC_("unknown emit type"); // LCOV_EXCL_LINE
    m_evts = nullptr;
    m_evts_size = 0;
    m_src = {};
    m_arena = {};
}


//-----------------------------------------------------------------------------

// The startup logic is made complicated from it having to accept
// initial non-root nodes, and having to deal with tricky tokens like
// doc separators, anchors, tags, optional keys or dashes, and
// comments.
//
// This function kickstarts the tree descent by handling all the
// initial and final logic at the top-level scope, thus avoiding
// top-level kickstart branches in the recursive descending code
// (which should be oblivious of such logic). This makes the recursive
// descending code a lot simpler.

template<class Writer>
void EmitterInts<Writer>::emit_yaml_(evt_size pos)
{
    const detail::EmitKickoff ek = detail::kickoff_emit(m_evts, m_evts_size, pos, m_opts);

    evt_bits evt = m_evts[ek.valpos];
    if(ek.emit_dash)
    {
        ++m_ilevel;
        write_("- ");
    }
    else if(ek.emit_key)
    {
        pos = kickoff_key(pos, ek.keypos);
        if(detail::seqormap(evt) && detail::query_blck_cont_(evt))
            pend_newl_();
    }

    bool flushpws = false;

    // emit the payload
    if(detail::hasall(evt, ievt::BSTR))
    {
        RYML_ASSERT_BASIC_(m_ilevel == 0);
        pos = visit_stream_(pos);
    }
    else if(detail::hasall(evt, ievt::BDOC))
    {
        RYML_ASSERT_BASIC_(m_ilevel == 0);
        bool expl = evt & ievt::EXPL;
        if(expl)
        {
            write_("---");
            pend_space_();
        }
        pos = visit_doc_(pos, expl);
    }
    else if(detail::seqormap(evt))
    {
        {
            evt_size nextpos = write_tag_or_anchor(pos, ek.valpos);
            if(nextpos > pos && detail::seqormap(evt) && detail::query_blck_cont_(evt))
                pend_newl_();
            pos = nextpos;
        }
        pos = visit_blck_container_(pos);
        if(evt & ievt::FMLX)
        {
            pend_newl_();
            flushpws = true;
        }
    }
    else if(evt & (ievt::SCLR|ievt::ALIA))
    {
        pos = write_tag_or_anchor(pos, ek.valpos);
        pos = visit_doc_val_(pos);
    }

    // emit closing tokens
    if(ek.emit_dash || ek.emit_key)
    {
        --m_ilevel;
        pend_newl_();
        flushpws = true;
    }

    if(!ek.parent
       || flushpws
       || !(evt & ievt::SCLR)
       || !(evt & ievt::PLAI))
    {
        write_pws_and_pend_(PWS_NONE_);
    }
}

template<class Writer>
evt_size EmitterInts<Writer>::write_tag_or_anchor(evt_size pos, evt_size dst)
{
    RYML_ASSERT_BASIC_(pos < m_evts_size);
    RYML_ASSERT_BASIC_(dst < m_evts_size);
    while(pos < dst)
    {
        const evt_bits evt = m_evts[pos];
        if(evt & ievt::ANCH)
        {
            write_pws_and_pend_(PWS_SPACE_);
            write_('&');
            write_(getstr_(pos));
            pos += 3;
        }
        else if(evt & ievt::TAG_)
        {
            write_pws_and_pend_(PWS_SPACE_);
            write_tag_(getstr_(pos));
            pos += 3;
        }
        else
        {
            pos += ievt::nextstep(evt); // LCOV_EXCL_LINE
        }
    }
    return pos;
}

template<class Writer>
evt_size EmitterInts<Writer>::kickoff_key(evt_size pos, evt_size keypos)
{
    RYML_ASSERT_BASIC_(pos < m_evts_size);
    evt_bits evt = m_evts[keypos];
    if(evt & (ievt::SCLR|ievt::ALIA))
    {
        csubstr key = getstr_(keypos);
        evt_bits keystyle = (evt & detail::styles_ievt_sclr);
        if(!keystyle)
            keystyle = detail::scalar_style_choose_block_ievt(key);
        if(keystyle & (ievt::FOLD|ievt::LITL))
        {
            write_('?');
            pend_space_();
            pos = write_tag_or_anchor(pos, keypos);
            write_pws_and_pend_(PWS_NEWL_);
            blck_write_scalar_(key, keystyle);
        }
        else
        {
            pos = write_tag_or_anchor(pos, keypos);
            write_pws_and_pend_(PWS_NONE_);
            blck_write_scalar_(key, keystyle);
        }
        write_pws_and_pend_(PWS_SPACE_);
        write_(':');
        ++m_ilevel;
    }
    else
    {
        RYML_ASSERT_BASIC_(detail::seqormap(m_evts[keypos]));
        evt_bits keystyle = (m_evts[keypos] & detail::styles_ievt_cont);
        if(!keystyle)
            keystyle = ievt::BLCK;
        write_('?');
        pend_space_();
        pos = write_tag_or_anchor(pos, keypos);
        ++m_ilevel;
        if(keystyle & ievt::BLCK)
            pend_newl_();
        pos = visit_blck_container_(pos);
        --m_ilevel;
        pend_newl_();
        write_pws_and_pend_(PWS_SPACE_);
        write_(':');
        ++m_ilevel;
    }
    return pos;
}


//-----------------------------------------------------------------------------

template<class Writer>
evt_size EmitterInts<Writer>::visit_stream_(evt_size pos)
{
    m_ilevel = 0;
    ++m_depth;
    uint32_t doc_count = 0;
    while(pos < m_evts_size)
    {
        evt_bits evt = m_evts[pos];
        if(detail::hasall(evt, ievt::BDOC))
        {
            bool expl = doc_count || (evt & ievt::EXPL);
            if(expl)
            {
                if(doc_count)
                    pend_newl_();
                write_pws_and_pend_(PWS_SPACE_);
                write_("---");
            }
            ++doc_count;
            RYML_ASSERT_BASIC_(pos + 1 < m_evts_size);
            if(detail::hasnone(m_evts[pos + 1], ievt::EDOC))
            {
                pos = visit_doc_(pos, expl);
            }
        }
        else if(evt & ievt::YAML)
        {
            write_pws_and_pend_(PWS_NEWL_);
            write_("%YAML ");
            write_(getstr_(pos));
            pos += 3;
        }
        else if(evt & ievt::TAGH)
        {
            RYML_ASSERT_BASIC_(m_evts[pos + 3] & ievt::TAGP);
            write_pws_and_pend_(PWS_NEWL_);
            write_("%TAG ");
            write_(getstr_(pos));
            write_(' ');
            pos += 3;
            write_(getstr_(pos));
            pos += 3;
        }
        else
        {
            pos += ievt::nextstep(evt); // LCOV_EXCL_LINE
        }
    }
    --m_depth;
    return pos;
}


//-----------------------------------------------------------------------------

template<class Writer>
evt_size EmitterInts<Writer>::visit_doc_val_(evt_size pos)
{
    // some plain scalars such as '...' and '---' must not
    // appear at 0-indentation
    evt_bits evt = m_evts[pos];
    RYML_ASSERT_BASIC_(evt & (ievt::SCLR|ievt::ALIA));
    RYML_ASSERT_BASIC_(evt & (ievt::VAL_|ievt::KEY_));
    const csubstr val = getstr_(pos);
    if(!(evt & ievt::ALIA))
    {
        evt_bits valstyle = evt & detail::styles_ievt_sclr;
        const bool is_ambiguous = ((evt & ievt::PLAI) || !valstyle)
            && (val.begins_with("...") || val.begins_with("---"));
        if(!valstyle)
            valstyle = detail::scalar_style_choose_block_ievt(val);
        if(is_ambiguous)
        {
            ++m_ilevel;
            if(m_pws != PWS_NONE_)
                pend_newl_();
            else
                indent_(m_ilevel);
        }
        else if(val.empty() && (valstyle & ievt::PLAI))
        {
            pend_none_();
        }
        write_pws_and_pend_(PWS_NONE_);
        blck_write_scalar_(val, valstyle);
        if(is_ambiguous)
            --m_ilevel;
    }
    else
    {
        write_('*');
        write_(val);
    }
    return pos;
}


//-----------------------------------------------------------------------------

template<class Writer>
evt_size EmitterInts<Writer>::visit_doc_(evt_size pos, bool begin_expl)
{
    RYML_ASSERT_BASIC_(detail::hasall(m_evts[pos], ievt::BDOC));
    ++pos;
    bool anchor_or_tag = false;
    bool was_flow_container = false;
    while(pos < m_evts_size)
    {
        evt_bits evt = m_evts[pos];
        if(detail::seqormap(evt))
        {
            if(evt & ievt::FLOW)
            {
                was_flow_container = true;
                pos = visit_flow_container_(pos);
                if(evt & ievt::FMLX)
                    pend_newl_();
            }
            else
            {
                if(begin_expl || anchor_or_tag)
                    pend_newl_();
                pos = visit_blck_container_(pos);
            }
            anchor_or_tag = false;
        }
        else if(evt & ievt::SCLR)
        {
            RYML_ASSERT_BASIC_(evt & ievt::VAL_);
            csubstr val = getstr_(pos);
            pos = visit_doc_val_(pos);
            if(begin_expl || val.len)
                pend_newl_();
            pos += 3;
        }
        else if(evt & ievt::ANCH)
        {
            anchor_or_tag = true;
            write_pws_and_pend_(PWS_SPACE_);
            write_('&');
            write_(getstr_(pos));
            pos += 3;
        }
        else if(evt & ievt::TAG_)
        {
            anchor_or_tag = true;
            write_pws_and_pend_(PWS_SPACE_);
            write_tag_(getstr_(pos));
            pos += 3;
        }
        else if(evt & ievt::ALIA)
        {
            write_pws_and_pend_(PWS_NEWL_);
            write_ref_(getstr_(pos));
            pos += 3;
        }
        else if(detail::hasall(evt, ievt::EDOC))
        {
            if(was_flow_container)
            {
                // is there a better way?
                if(begin_expl || (evt & ievt::EXPL) || detail::has_next_doc_and_is_expl_(m_evts, m_evts_size, pos))
                    pend_newl_();
            }
            if(evt & ievt::EXPL)
            {
                write_pws_and_pend_(PWS_NEWL_);
                write_("...");
            }
            ++pos;
            break;
        }
        else
        {
            pos += ievt::nextstep(evt); // LCOV_EXCL_LINE
        }
    }
    return pos;
}


//-----------------------------------------------------------------------------

template<class Writer>
evt_size EmitterInts<Writer>::visit_blck_container_(evt_size pos)
{
    evt_bits evt = m_evts[pos];
    RYML_ASSERT_BASIC_(detail::seqormap(evt));
    RYML_ASSERT_BASIC_(pos + 1 < m_evts_size);
    if(!(evt & (ievt::FLOW|ievt::BLCK)))
        evt |= (m_evts[pos + 1] & ievt::END_) ? ievt::FSL_ : ievt::BLCK;
    write_pws_and_pend_(PWS_NONE_);
    if(evt & ievt::FSL_)
        pos = visit_flow_sl_(pos);
    else if(evt & ievt::FMLX)
        pos = visit_flow_ml_(pos);
    else
        pos = visit_blck_(pos);
    RYML_ASSERT_BASIC_(!(evt & ievt::END_));
    return pos;
}

template<class Writer>
evt_size EmitterInts<Writer>::visit_flow_container_(evt_size pos)
{
    evt_bits evt = m_evts[pos];
    RYML_ASSERT_BASIC_(detail::seqormap(evt));
    RYML_ASSERT_BASIC_(pos + 1 < m_evts_size);
    if(!(evt & ievt::FLOW))
        evt |= ievt::FLOW|ievt::FSL_;
    write_pws_and_pend_(PWS_NONE_);
    if(evt & ievt::FMLX)
        pos = visit_flow_ml_(pos);
    else
        pos = visit_flow_sl_(pos);
    RYML_ASSERT_BASIC_(!(evt & ievt::END_));
    return pos;
}


//-----------------------------------------------------------------------------

template<class Writer>
evt_size EmitterInts<Writer>::visit_blck_(evt_size pos)
{
    evt_bits evt = m_evts[pos];
    RYML_ASSERT_BASIC_(!(evt & ievt::STRM));
    RYML_ASSERT_BASIC_(detail::seqormap(evt) || detail::hasall(evt, ievt::BDOC));
    if C4_UNLIKELY(m_depth > (evt_size)m_opts.max_depth())
        RYML_ERR_BASIC_("max depth exceeded");
    if(detail::hasall(evt, ievt::BSEQ))
    {
        pos = visit_blck_seq_(pos);
    }
    else
    {
        RYML_ASSERT_BASIC_(detail::hasall(evt, ievt::BMAP));
        pos = visit_blck_map_(pos);
    }
    return pos;
}


//-----------------------------------------------------------------------------

template<class Writer>
evt_size EmitterInts<Writer>::visit_flow_sl_(evt_size pos)
{
    evt_bits evt = m_evts[pos];
    RYML_ASSERT_BASIC_(!(evt & ievt::STRM));
    RYML_ASSERT_BASIC_(detail::seqormap(evt) || detail::hasall(evt, ievt::BDOC));
    if C4_UNLIKELY(m_depth > (evt_size)m_opts.max_depth())
        RYML_ERR_BASIC_("max depth exceeded");
    if(evt & ievt::SEQ_)
    {
        pos = visit_flow_sl_seq_(pos);
    }
    else
    {
        RYML_ASSERT_BASIC_(evt & ievt::MAP_);
        pos = visit_flow_sl_map_(pos);
    }
    return pos;
}


//-----------------------------------------------------------------------------

template<class Writer>
evt_size EmitterInts<Writer>::visit_flow_ml_(evt_size pos)
{
    evt_bits evt = m_evts[pos];
    RYML_ASSERT_BASIC_(!(evt & ievt::STRM));
    RYML_ASSERT_BASIC_(detail::seqormap(evt) || detail::hasall(evt, ievt::BDOC));
    if C4_UNLIKELY(m_depth > (evt_size)m_opts.max_depth())
        RYML_ERR_BASIC_("max depth exceeded");
    if(evt & ievt::SEQ_)
    {
        pos = visit_flow_ml_seq_(pos);
    }
    else
    {
        RYML_ASSERT_BASIC_(evt & ievt::MAP_);
        pos = visit_flow_ml_map_(pos);
    }
    return pos;
}


//-----------------------------------------------------------------------------

template<class Writer>
evt_size EmitterInts<Writer>::visit_blck_seq_(evt_size pos)
{
    RYML_ASSERT_BASIC_(detail::hasall(m_evts[pos], ievt::BSEQ));
    RYML_ASSERT_BASIC_(pos + 1 < m_evts_size);
    ++pos;
    bool newval = true;
    bool has_tag_or_anchor = false;
    evt_bits evt = {};
    while(pos < m_evts_size)
    {
        evt = m_evts[pos];
        if(detail::hasall(evt, ievt::ESEQ))
        {
            ++pos;
            break;
        }
        if(newval)
        {
            write_pws_and_pend_(PWS_SPACE_); // pend the space after the following dash
            write_('-');
            newval = false;
        }
        if(evt & ievt::SCLR)
        {
            write_pws_and_pend_(PWS_NEWL_);
            const csubstr val = getstr_(pos);
            if(!(evt & detail::styles_ievt_sclr))
                evt |= detail::scalar_style_choose_block_ievt(val);
            blck_write_scalar_(val, evt);
            pos += 3;
            goto nextval; // NOLINT
        }
        else if(detail::seqormap(evt))
        {
            if(has_tag_or_anchor)
            {
                if(detail::hasnone(evt, detail::styles_ievt_cont))
                    evt |= ievt::BLCK;
                bool empty = (m_evts[pos + 1] & ievt::END_);
                if(!empty && (evt & ievt::BLCK))
                    pend_newl_();
            }
            ++m_depth;
            ++m_ilevel;
            pos = visit_blck_container_(pos);
            --m_depth;
            --m_ilevel;
            pend_newl_();
            goto nextval; // NOLINT
        }
        else if(evt & ievt::ALIA)
        {
            write_pws_and_pend_(PWS_NEWL_);
            write_ref_(getstr_(pos));
            pos += 3;
            goto nextval; // NOLINT
        }
        else if(evt & ievt::ANCH)
        {
            has_tag_or_anchor = true;
            write_pws_and_pend_(PWS_SPACE_);
            write_('&');
            write_(getstr_(pos));
            pos += 3;
        }
        else if(evt & ievt::TAG_)
        {
            has_tag_or_anchor = true;
            write_pws_and_pend_(PWS_SPACE_);
            write_tag_(getstr_(pos));
            pos += 3;
        }
        else
        {
            pos += ievt::nextstep(evt); // LCOV_EXCL_LINE
        }
        continue;
    nextval:
        newval = true;
        has_tag_or_anchor = false;
    }
    return pos;
}


//-----------------------------------------------------------------------------


template<class Writer>
evt_size EmitterInts<Writer>::visit_blck_map_(evt_size pos)
{
    RYML_ASSERT_BASIC_(pos + 1 < m_evts_size);
    RYML_ASSERT_BASIC_(detail::hasall(m_evts[pos], ievt::BMAP));
    bool statenew = true;
    bool statekey = true;
    ++pos;
    evt_bits evt = {};
    while(pos < m_evts_size)
    {
        evt = m_evts[pos];
        if(detail::hasall(evt, ievt::EMAP))
        {
            ++pos;
            break;
        }
        if(statenew)
        {
            if(statekey)
            {
                if(!detail::key_requires_qmark_block(m_evts, m_evts_size, pos))
                {
                    write_pws_and_pend_(PWS_NONE_);
                }
                else
                {
                    write_pws_and_pend_(PWS_SPACE_);
                    write_('?');
                }
            }
            else
            {
                write_pws_and_pend_(PWS_SPACE_);
                write_(':');
            }
            statenew = false;
        }
        if(evt & ievt::SCLR)
        {
            write_pws_and_pend_(statekey ? PWS_NONE_ : PWS_NEWL_);
            const csubstr val = getstr_(pos);
            if(!(evt & detail::styles_ievt_sclr))
            {
                evt |= detail::scalar_style_choose_block_ievt(val);
                RYML_ASSERT_BASIC_(!(evt & (ievt::LITL|ievt::FOLD))); // litl/fold scalars require qmark
            }
            blck_write_scalar_(val, evt);
            pos += 3;
            if(evt & (ievt::LITL|ievt::FOLD))
                pend_newl_();
            goto statenext; // NOLINT
        }
        else if(detail::seqormap(evt))
        {
            if(!(evt & detail::styles_ievt_cont))
                evt |= ievt::BLCK;
            ++m_depth;
            ++m_ilevel;
            if(evt & ievt::BLCK)
                pend_newl_();
            write_pws_and_pend_(PWS_NONE_);
            pos = visit_blck_container_(pos);
            if(evt & ievt::FLOW)
                pend_newl_();
            --m_depth;
            --m_ilevel;
            goto statenext; // NOLINT
        }
        else if(evt & ievt::ALIA)
        {
            write_pws_and_pend_(statekey ? PWS_SPACE_ : PWS_NEWL_);
            write_ref_(getstr_(pos));
            pos += 3;
            goto statenext; // NOLINT
        }
        else if(evt & ievt::ANCH)
        {
            write_pws_and_pend_(PWS_SPACE_);
            write_('&');
            write_(getstr_(pos));
            pos += 3;
        }
        else if(evt & ievt::TAG_)
        {
            write_pws_and_pend_(PWS_SPACE_);
            write_tag_(getstr_(pos));
            pos += 3;
        }
        else
        {
            pos += ievt::nextstep(evt); // LCOV_EXCL_LINE
        }
        continue;
    statenext:
        statenew = true;
        statekey = !statekey;
    }
    return pos;
}


//-----------------------------------------------------------------------------

template<class Writer>
C4_NODISCARD bool EmitterInts<Writer>::maybe_start_flow_pws_ml_(evt_size node) noexcept
{
    RYML_ASSERT_BASIC_((m_evts[node] & ievt::FMLX) || m_opts.force_flow_spc());
    if(m_flow_pws.active)
        return false;
    evt_bits evt = m_evts[node];
    if(m_opts.force_flow_spc())
        evt |= ievt::FSPC;
    m_flow_pws.start(evt, m_opts.max_cols());
    return true;
}

template<class Writer>
C4_NODISCARD typename EmitterInts<Writer>::flow_pws EmitterInts<Writer>::setup_flow_pws_sl_(evt_size node) noexcept
{
    flow_pws ret = {};
    if(m_flow_pws.active)
    {
        ret = m_flow_pws;
    }
    else
    {
        evt_bits evt = m_evts[node];
        if(m_opts.force_flow_spc())
            evt |= ievt::FSPC;
        ret.start(evt, 0);
    }
    return ret;
}

template<class Writer>
void EmitterInts<Writer>::flow_pws::start(evt_bits evt, size_t max_cols_) noexcept
{
    max_cols = 0;
    pend_after_comma = (evt & ievt::FSPC) ? PWS_SPACE_ : PWS_NONE_;
    if(evt & ievt::FMLN)
    {
        max_cols_ = max_cols_ >= 2 ? max_cols_ : 2;
        // subtract 1 for the comma, and maybe the space from pend_after_comma
        max_cols = max_cols_ - 1 - pend_after_comma;
        active = true;
    }
    else if(evt & ievt::FML1)
    {
        pend_after_comma = PWS_NEWL_;
    }
}

template<class Writer>
void EmitterInts<Writer>::flow_close_entry_sl_(evt_size pos, Pws_e pend_after)
{
    RYML_ASSERT_BASIC_(pos < m_evts_size);
    if(!(m_evts[pos] & ievt::END_))
    {
        write_pws_and_pend_(pend_after);
        write_(',');
    }
}

template<class Writer>
void EmitterInts<Writer>::flow_close_entry_ml_(evt_size pos, Pws_e pend_after)
{
    RYML_ASSERT_BASIC_(pos < m_evts_size);
    if(!(m_evts[pos] & ievt::END_))
    {
        write_pws_and_pend_(pend_after);
        write_(',');
    }
    else
    {
        pend_newl_();
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
evt_size EmitterInts<Writer>::visit_flow_sl_seq_(evt_size pos)
{
    RYML_ASSERT_BASIC_(detail::hasall(m_evts[pos], ievt::BSEQ));
    RYML_ASSERT_BASIC_(pos + 1 < m_evts_size);
    const flow_pws pws = setup_flow_pws_sl_(pos);
    ++pos;
    write_('[');
    while(pos < m_evts_size)
    {
        evt_bits evt = m_evts[pos];
        if(detail::hasall(evt, ievt::ESEQ))
        {
            ++pos;
            break;
        }
        if(evt & ievt::SCLR)
        {
            write_pws_and_pend_(PWS_NONE_);
            const csubstr val = getstr_(pos);
            if(!(evt & detail::styles_ievt_sclr))
                evt |= detail::scalar_style_choose_flow_ievt(val);
            flow_write_scalar_(val, evt);
            pos += 3;
            goto nextval; // NOLINT
        }
        else if(detail::seqormap(evt))
        {
            ++m_depth;
            pos = visit_flow_container_(pos);
            --m_depth;
            goto nextval; // NOLINT
        }
        else if(evt & ievt::ALIA)
        {
            write_pws_and_pend_(PWS_NONE_);
            write_ref_(getstr_(pos));
            pos += 3;
            goto nextval; // NOLINT
        }
        else if(evt & ievt::ANCH)
        {
            write_pws_and_pend_(PWS_SPACE_);
            write_('&');
            write_(getstr_(pos));
            pos += 3;
        }
        else if(evt & ievt::TAG_)
        {
            write_pws_and_pend_(PWS_SPACE_);
            write_tag_(getstr_(pos));
            pos += 3;
        }
        else
        {
            pos += ievt::nextstep(evt); // LCOV_EXCL_LINE
        }
        continue;
    nextval:
        flow_close_entry_sl_(pos, pws.next_pws(m_col));
    }
    write_(']');
    return pos;
}


//-----------------------------------------------------------------------------

template<class Writer>
evt_size EmitterInts<Writer>::visit_flow_ml_seq_(evt_size pos)
{
    RYML_ASSERT_BASIC_(pos + 1 < m_evts_size);
    RYML_ASSERT_BASIC_(detail::hasall(m_evts[pos], ievt::BSEQ));
    if(m_opts.indent_flow_ml()) ++m_ilevel;
    write_('[');
    pend_newl_();
    const bool stop_at_end = maybe_start_flow_pws_ml_(pos);
    ++pos;
    while(pos < m_evts_size)
    {
        evt_bits evt = m_evts[pos];
        if(detail::hasall(evt, ievt::ESEQ))
        {
            ++pos;
            break;
        }
        if(evt & ievt::SCLR)
        {
            write_pws_and_pend_(PWS_NONE_);
            const csubstr val = getstr_(pos);
            if(!(evt & detail::styles_ievt_sclr))
                evt |= detail::scalar_style_choose_flow_ievt(val);
            flow_write_scalar_(val, evt);
            pos += 3;
            goto nextval; // NOLINT
        }
        else if(detail::seqormap(evt))
        {
            write_pws_and_pend_(PWS_NONE_);
            ++m_depth;
            pos = visit_flow_container_(pos);
            --m_depth;
            goto nextval; // NOLINT
        }
        else if(evt & ievt::ALIA)
        {
            write_pws_and_pend_(PWS_NONE_);
            write_ref_(getstr_(pos));
            pos += 3;
            goto nextval; // NOLINT
        }
        else if(evt & ievt::ANCH)
        {
            write_pws_and_pend_(PWS_SPACE_);
            write_('&');
            write_(getstr_(pos));
            pos += 3;
        }
        else if(evt & ievt::TAG_)
        {
            write_pws_and_pend_(PWS_SPACE_);
            write_tag_(getstr_(pos));
            pos += 3;
        }
        else
        {
            pos += ievt::nextstep(evt); // LCOV_EXCL_LINE
        }
        continue;
    nextval:
        // pos is already at the next event
        flow_close_entry_ml_(pos, m_flow_pws.next_pws(m_col));
    }
    if(stop_at_end)
        m_flow_pws.stop();
    if(m_opts.indent_flow_ml()) --m_ilevel;
    write_pws_and_pend_(PWS_NONE_);
    write_(']');
    return pos;
}


//-----------------------------------------------------------------------------

template<class Writer>
evt_size EmitterInts<Writer>::visit_flow_sl_map_(evt_size pos)
{
    RYML_ASSERT_BASIC_(pos + 1 < m_evts_size);
    RYML_ASSERT_BASIC_(detail::hasall(m_evts[pos], ievt::BMAP));
    const flow_pws pws = setup_flow_pws_sl_(pos);
    bool statenew = true;
    bool statekey = true;
    ++pos;
    write_('{');
    while(pos < m_evts_size)
    {
        evt_bits evt = m_evts[pos];
        if(detail::hasall(evt, ievt::EMAP))
        {
            ++pos;
            break;
        }
        if(statenew)
        {
            if(statekey)
            {
                if(detail::key_requires_qmark_flow(m_evts, m_evts_size, pos))
                    write_("? ");
            }
            else
            {
                write_pws_and_pend_(PWS_SPACE_);
                write_(':');
            }
            statenew = false;
        }
        if(evt & ievt::SCLR)
        {
            write_pws_and_pend_(PWS_NONE_);
            const csubstr val = getstr_(pos);
            if(!(evt & detail::styles_ievt_sclr))
                evt |= detail::scalar_style_choose_flow_ievt(val);
            flow_write_scalar_(val, evt);
            pos += 3;
            goto statenext; // NOLINT
        }
        else if(detail::seqormap(evt))
        {
            ++m_depth;
            write_pws_and_pend_(PWS_NONE_);
            pos = visit_flow_container_(pos);
            --m_depth;
            goto statenext; // NOLINT
        }
        else if(evt & ievt::ALIA)
        {
            write_pws_and_pend_(statekey ? PWS_SPACE_ : PWS_NONE_);
            write_ref_(getstr_(pos));
            pos += 3;
            goto statenext; // NOLINT
        }
        else if(evt & ievt::ANCH)
        {
            write_pws_and_pend_(PWS_SPACE_);
            write_('&');
            write_(getstr_(pos));
            pos += 3;
        }
        else if(evt & ievt::TAG_)
        {
            write_pws_and_pend_(PWS_SPACE_);
            write_tag_(getstr_(pos));
            pos += 3;
        }
        else
        {
            pos += ievt::nextstep(evt); // LCOV_EXCL_LINE
        }
        continue;
    statenext:
        if(!statekey)
            // pos is already at the next event
            flow_close_entry_sl_(pos, pws.next_pws(m_col));
        statenew = true;
        statekey = !statekey;
    }
    write_('}');
    return pos;
}


//-----------------------------------------------------------------------------

template<class Writer>
evt_size EmitterInts<Writer>::visit_flow_ml_map_(evt_size pos)
{
    RYML_ASSERT_BASIC_(pos + 1 < m_evts_size);
    RYML_ASSERT_BASIC_(detail::hasall(m_evts[pos], ievt::BMAP));
    if(m_opts.indent_flow_ml()) ++m_ilevel;
    write_('{');
    pend_newl_();
    const bool stop_at_end = maybe_start_flow_pws_ml_(pos);
    bool statenew = true;
    bool statekey = true;
    ++pos;
    while(pos < m_evts_size)
    {
        evt_bits evt = m_evts[pos];
        if(detail::hasall(evt, ievt::EMAP))
        {
            ++pos;
            break;
        }
        if(statenew)
        {
            if(statekey)
            {
                if(detail::key_requires_qmark_flow(m_evts, m_evts_size, pos))
                {
                    write_pws_and_pend_(PWS_NONE_);
                    write_("? ");
                }
            }
            else
            {
                write_pws_and_pend_(PWS_SPACE_);
                write_(':');
            }
            statenew = false;
        }
        if(evt & ievt::SCLR)
        {
            write_pws_and_pend_(PWS_NONE_);
            const csubstr val = getstr_(pos);
            if(!(evt & detail::styles_ievt_sclr))
                evt |= detail::scalar_style_choose_flow_ievt(val);
            flow_write_scalar_(val, evt);
            pos += 3;
            goto statenext; // NOLINT
        }
        else if(detail::seqormap(evt))
        {
            ++m_depth;
            write_pws_and_pend_(PWS_NONE_);
            pos = visit_flow_container_(pos);
            --m_depth;
            goto statenext; // NOLINT
        }
        else if(evt & ievt::ALIA)
        {
            write_pws_and_pend_(statekey ? PWS_SPACE_ : PWS_NONE_);
            write_ref_(getstr_(pos));
            pos += 3;
            goto statenext; // NOLINT
        }
        else if(evt & ievt::ANCH)
        {
            write_pws_and_pend_(PWS_SPACE_);
            write_('&');
            write_(getstr_(pos));
            pos += 3;
        }
        else if(evt & ievt::TAG_)
        {
            write_pws_and_pend_(PWS_SPACE_);
            write_tag_(getstr_(pos));
            pos += 3;
        }
        else
        {
            pos += ievt::nextstep(evt); // LCOV_EXCL_LINE
        }
        continue;
    statenext:
        if(!statekey)
            // pos is already at the next event
            flow_close_entry_ml_(pos, m_flow_pws.next_pws(m_col));
        statenew = true;
        statekey = !statekey;
    }
    if(stop_at_end)
        m_flow_pws.stop();
    if(m_opts.indent_flow_ml()) --m_ilevel;
    write_pws_and_pend_(PWS_NONE_);
    write_('}');
    return pos;
}


//-----------------------------------------------------------------------------

template<class Writer>
void EmitterInts<Writer>::flow_write_scalar_(csubstr str, evt_bits evt)
{
    RYML_ASSERT_BASIC_(!(evt & ievt::BLCK));
    if((evt & ievt::PLAI) || !(evt & detail::styles_ievt_sclr))
    {
        yml::detail::emit_scalar_plain_(this, str, m_ilevel);
    }
    else if(evt & ievt::SQUO)
    {
        yml::detail::emit_scalar_squo_(this, str, m_ilevel);
    }
    else // if(evt & ievt::DQUO)
    {
        yml::detail::emit_scalar_dquo_(this, str);
    }
}

template<class Writer>
void EmitterInts<Writer>::blck_write_scalar_(csubstr str, evt_bits evt)
{
    if((evt & ievt::PLAI) || !(evt & detail::styles_ievt_sclr))
    {
        yml::detail::emit_scalar_plain_(this, str, m_ilevel);
    }
    else if(evt & ievt::SQUO)
    {
        yml::detail::emit_scalar_squo_(this, str, m_ilevel);
    }
    else if(evt & ievt::DQUO)
    {
        yml::detail::emit_scalar_dquo_(this, str);
    }
    else if(evt & ievt::LITL)
    {
        yml::detail::emit_scalar_literal_(this, str, m_ilevel);
    }
    else // if(ty & ievt::FOLD)
    {
        yml::detail::emit_scalar_folded_(this, str, m_ilevel);
    }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

template<class Writer>
void EmitterInts<Writer>::emit_json_(evt_size pos)
{
    if(detail::hasall(m_evts[pos], ievt::BSTR))
        pos = json_visit_stream_(pos);
    else
        pos = json_visit_nested_(pos);
    write_pws_and_pend_(PWS_NONE_);
}

template<class Writer>
evt_size EmitterInts<Writer>::json_visit_stream_(evt_size pos)
{
    evt_size numdocs = 0;
    bool has_expl = false;
    // count numdocs
    for(evt_size p = pos; p < m_evts_size; p += ievt::nextstep(m_evts[p]))
    {
        if(detail::hasall(m_evts[p], ievt::BDOC))
        {
            ++numdocs;
            has_expl = has_expl || ((m_evts[p] & ievt::EXPL) != 0);
        }
        else if(detail::hasall(m_evts[p], ievt::ESTR))
        {
            break;
        }
    }
    const bool mldocs = (has_expl || numdocs > 1);
    if(mldocs)
    {
        if C4_UNLIKELY(m_opts.json_err_on_stream())
            RYML_ERR_BASIC_("multiple docs");
        ++m_depth;
        if(m_opts.indent_flow_ml()) ++m_ilevel;
        write_('[');
        newl_();
    }
    ++pos; // advance past BSTR
    while(pos < m_evts_size)
    {
        if(detail::hasall(m_evts[pos], ievt::BDOC))
        {
            bool isflowml = false;
            if(mldocs)
            {
                indent_(m_ilevel);
            }
            else
            {
                isflowml = pos + 1 < m_evts_size && (m_evts[pos+1] & (ievt::FMLX|ievt::BLCK));
            }
            pos = json_visit_ml_(pos, m_depth);
            if(mldocs)
            {
                if(detail::hasall(m_evts[pos], ievt::BDOC))
                    write_(',');
                newl_();
            }
            else
            {
                if(isflowml)
                    newl_();
            }
        }
        else if(detail::hasall(m_evts[pos], ievt::ESTR))
        {
            break;
        }
        else
        {
            pos += ievt::nextstep(m_evts[pos]); // LCOV_EXCL_LINE
        }
    }
    write_pws_and_pend_(PWS_NONE_);
    if(mldocs)
    {
        if(m_opts.indent_flow_ml()) --m_ilevel;
        --m_depth;
        write_(']');
        newl_();
    }
    return pos;
}


template<class Writer>
evt_size EmitterInts<Writer>::json_visit_nested_(evt_size pos)
{
    // look for the first of BSEQ|BMAP|SCLR|ALIA
    const detail::EmitKickoff ek = detail::kickoff_emit(m_evts, m_evts_size, pos, m_opts);
    if(ek.emit_key)
    {
        if C4_LIKELY(m_evts[ek.keypos] & (ievt::SCLR|ievt::ALIA))
        {
            pos = json_writek_(ek.keypos);
        }
        else
        {
            RYML_ERR_BASIC_("bad key");
        }
        write_(": ");
    }
    while(pos < m_evts_size)
    {
        const evt_bits evt = m_evts[pos];
        if(detail::hasall(evt, ievt::BSEQ))
        {
            write_('[');
            pos = json_visit_container_(pos);
            write_(']');
            if(evt & ievt::FMLX)
                pend_newl_();
            break;
        }
        else if(detail::hasall(evt, ievt::BMAP))
        {
            write_('{');
            pos = json_visit_container_(pos);
            write_('}');
            if(evt & ievt::FMLX)
                pend_newl_();
            break;
        }
        else if(evt & ievt::SCLR)
        {
            pos = json_writev_(pos, evt, /*has_anchor_or_tag*/false);
            if(ek.emit_key)
                pend_newl_();
            break;
        }
        else if(evt & ievt::ALIA)
        {
            if C4_UNLIKELY(m_opts.json_err_on_anchor())
                RYML_ERR_BASIC_("JSON does not have anchors");
            write_("\"*");
            write_(getstr_(pos));
            write_('"');
            if(ek.emit_key)
                pend_newl_();
            break;
        }
        else
        {
            pos += ievt::nextstep(evt); // LCOV_EXCL_LINE
        }
    }
    if(ek.emit_key || (m_evts[ek.valpos] & (ievt::FMLX|ievt::BLCK)))
        pend_newl_();
    return pos;
}


template<class Writer>
evt_size EmitterInts<Writer>::json_visit_container_(evt_size pos)
{
    ++m_depth;
    if(m_evts[pos] & (ievt::BLCK|ievt::FMLX))
    {
        if(m_opts.indent_flow_ml()) ++m_ilevel;
        pend_newl_();
        pos = json_visit_ml_(pos, m_depth);
        if(m_opts.indent_flow_ml()) --m_ilevel;
        write_pws_and_pend_(PWS_NONE_);
    }
    else
    {
        pos = json_visit_sl_(pos, m_depth);
    }
    --m_depth;
    return pos;
}


template<class Writer>
evt_size EmitterInts<Writer>::json_visit_sl_(evt_size pos, evt_size depth)
{
    if C4_UNLIKELY(depth > (evt_size)m_opts.max_depth())
        RYML_ERR_BASIC_("max depth exceeded");
    RYML_ASSERT_BASIC_(m_evts[pos] & ievt::BEG_);
    const evt_bits term = ((m_evts[pos] & ~ievt::BEG_) | ievt::END_) & detail::mask_open_close;
    const bool with_spc = (m_evts[pos] & ievt::FSPC) || m_opts.force_flow_spc();
    bool has_anchor_or_tag = false;
    ++pos;
    while(pos < m_evts_size)
    {
        evt_bits evt = m_evts[pos];
        if(evt & ievt::SCLR)
        {
            if(evt & ievt::KEY_)
            {
                pos = json_writek_(pos);
                write_(": ");
            }
            else
            {
                RYML_ASSERT_BASIC_(evt & ievt::VAL_);
                pos = json_writev_(pos, evt, has_anchor_or_tag);
                goto next_entry; // NOLINT
            }
        }
        else if(detail::hasall(evt, ievt::BSEQ))
        {
            write_('[');
            ++m_depth;
            pos = json_visit_sl_(pos, m_depth);
            --m_depth;
            write_(']');
            goto next_entry; // NOLINT
        }
        else if(detail::hasall(evt, ievt::BMAP))
        {
            write_('{');
            ++m_depth;
            pos = json_visit_sl_(pos, m_depth);
            --m_depth;
            write_('}');
            goto next_entry; // NOLINT
        }
        else if(evt & ievt::ALIA)
        {
            if C4_UNLIKELY(m_opts.json_err_on_anchor())
                RYML_ERR_BASIC_("JSON does not have anchors");
            write_("\"*");
            write_(getstr_(pos));
            write_('"');
            pos += 3;
            goto next_entry; // NOLINT
        }
        else if(evt & (ievt::ANCH|ievt::TAG_))
        {
            if C4_UNLIKELY((evt & ievt::ANCH) && m_opts.json_err_on_anchor())
                RYML_ERR_BASIC_("JSON does not have anchors");
            if C4_UNLIKELY((evt & ievt::TAG_) && m_opts.json_err_on_tag())
                RYML_ERR_BASIC_("JSON does not have tags");
            has_anchor_or_tag = true;
            pos += 3;
        }
        else if(detail::hasall(evt, term))
        {
            ++pos;
            break;
        }
        else
        {
            pos += ievt::nextstep(evt); // LCOV_EXCL_LINE
        }
        continue;
    next_entry:
        RYML_ASSERT_BASIC_(pos < m_evts_size);
        if(!detail::hasall(m_evts[pos], term)) // pos changed - don't use evt here
        {
            if(with_spc)
                write_(", ");
            else
                write_(',');
            has_anchor_or_tag = false;
        }
        else
        {
            ++pos;
            break;
        }
    }
    return pos;
}


template<class Writer>
evt_size EmitterInts<Writer>::json_visit_ml_(evt_size pos, evt_size depth)
{
    if C4_UNLIKELY(depth > (evt_size)m_opts.max_depth())
        RYML_ERR_BASIC_("max depth exceeded");
    RYML_ASSERT_BASIC_(m_evts[pos] & ievt::BEG_);
    const evt_bits open = m_evts[pos];
    const evt_bits close = ((open & ~ievt::BEG_) | ievt::END_) & detail::mask_open_close;
    bool has_anchor_or_tag = false;
    ++pos;
    while(pos < m_evts_size)
    {
        evt_bits evt = m_evts[pos];
        if(evt & ievt::SCLR)
        {
            write_pws_and_pend_(PWS_NONE_);
            if(evt & ievt::KEY_)
            {
                pos = json_writek_(pos);
                write_(": ");
            }
            else
            {
                RYML_ASSERT_BASIC_(evt & ievt::VAL_);
                pos = json_writev_(pos, evt, has_anchor_or_tag);
                goto next_entry; // NOLINT
            }
        }
        else if(detail::hasall(evt, ievt::BSEQ))
        {
            write_pws_and_pend_(PWS_NONE_);
            write_('[');
            pos = json_visit_container_(pos);
            write_(']');
            goto next_entry; // NOLINT
        }
        else if(detail::hasall(evt, ievt::BMAP))
        {
            write_pws_and_pend_(PWS_NONE_);
            write_('{');
            pos = json_visit_container_(pos);
            write_('}');
            goto next_entry; // NOLINT
        }
        else if(evt & ievt::ALIA)
        {
            if C4_UNLIKELY(m_opts.json_err_on_anchor())
                RYML_ERR_BASIC_("JSON does not have anchors");
            write_pws_and_pend_(PWS_NONE_);
            write_("\"*");
            write_(getstr_(pos));
            write_('"');
            pos += 3;
            goto next_entry; // NOLINT
        }
        else if(evt & (ievt::ANCH|ievt::TAG_))
        {
            if C4_UNLIKELY((evt & ievt::ANCH) && m_opts.json_err_on_anchor())
                RYML_ERR_BASIC_("JSON does not have anchors");
            if C4_UNLIKELY((evt & ievt::TAG_) && m_opts.json_err_on_tag())
                RYML_ERR_BASIC_("JSON does not have tags");
            write_pws_and_pend_(PWS_NONE_);
            has_anchor_or_tag = true;
            pos += 3;
        }
        else if(detail::hasall(evt, close))
        {
            ++pos;
            break;
        }
        else
        {
            pos += ievt::nextstep(evt); // LCOV_EXCL_LINE
        }
        continue;
    next_entry:
        RYML_ASSERT_BASIC_(pos < m_evts_size);
        if(!detail::hasall(m_evts[pos], close)) // pos changed -- don't use evt here
        {
            has_anchor_or_tag = false;
            write_(',');
            bool at_end = (m_col+1 >= m_opts.max_cols());
            if((open & ievt::FMLN) && !at_end)
            {
                if((open & ievt::FSPC) || m_opts.force_flow_spc())
                    write_(' ');
            }
            else if((open & (ievt::FML1|ievt::BLCK)) || at_end)
            {
                pend_newl_();
            }
        }
        else
        {
            ++pos; // advance past the close event
            if(open & (ievt::FMLX|ievt::BLCK))
                pend_newl_();
            break;
        }
    }
    return pos;
}


template<class Writer>
evt_size EmitterInts<Writer>::json_writek_(evt_size pos)
{
    csubstr key = getstr_(pos);
    if(key.len)
    {
        if(yml::detail::emit_json_maybe_write_naninf_(this, key))
            ;
        else
            yml::detail::emit_json_write_scalar_dquo_(this, key);
    }
    else
    {
        write_("\"\"");
    }
    return pos + 3;
}


template<class Writer>
evt_size EmitterInts<Writer>::json_writev_(evt_size pos, evt_bits ty, bool has_anchor_or_tag)
{
    csubstr val = getstr_(pos);
    if(val.len)
    {
        // use double quoted style if the style is marked quoted
        bool dquoted = ((ty & detail::styles_ievt_quot)
                        || (detail::scalar_style_choose_json_ievt(val) & ievt::DQUO)); // choose the style
        if(dquoted)
            yml::detail::emit_json_write_scalar_dquo_(this, val);
        else if(yml::detail::emit_json_maybe_write_naninf_(this, val))
            ;
        else if(val.is_number())
            yml::detail::emit_json_write_number_(this, val);
        else
            write_(val);
    }
    else
    {
        if(val.str || (ty & detail::styles_ievt_quot) || has_anchor_or_tag)
            write_("\"\"");
        else
            write_("null");
    }
    return pos + 3;
}


/** @endcond */

} // namespace ievt
} // namespace extra
} // namespace yml
} // namespace c4

// NOLINTEND(hicpp-signed-bitwise,*avoid-c-style-cast)
C4_SUPPRESS_WARNING_GCC_CLANG_POP

#endif /* C4_YML_EXTRA_EMITTER_INTS_DEF_HPP_ */
