#ifndef C4_YML_EXTRA_EMITTER_INTS_DEF_HPP_
#define C4_YML_EXTRA_EMITTER_INTS_DEF_HPP_

/** @file emitter.def.hpp */

#ifndef C4_YML_EXTRA_EMITTER_INTS_HPP_
#include "c4/yml/extra/emitter_ints.hpp"
#endif
#ifndef C4_YML_SCALAR_STYLE_HPP_
#include "c4/yml/scalar_style.hpp"
#endif
#ifndef C4_YML_SCALAR_CHARCONV_HPP_
#include "c4/yml/scalar_charconv.hpp"
#endif
#ifndef C4_YML_ERROR_HPP_
#include "c4/yml/error.hpp"
#endif


C4_SUPPRESS_WARNING_GCC_CLANG_WITH_PUSH("-Wold-style-cast")
C4_SUPPRESS_WARNING_GCC("-Wuseless-cast")
// NOLINTBEGIN(hicpp-signed-bitwise,*avoid-c-style-cast)

namespace c4 {
namespace yml {
namespace extra {
namespace ievt {

namespace detail {

enum : evt_bits { // NOLINT
    styles_ievt_sclr = ievt::PLAI|ievt::SQUO|ievt::DQUO|ievt::LITL|ievt::FOLD,
    styles_ievt_cont = ievt::BLCK|ievt::FLOW|ievt::FSL_|ievt::FML1|ievt::FMLN,
};

//see also NodeType implementation in scalar_style.cpp
inline NodeType scalar_style_choose_json_ievt(csubstr scalar) noexcept
{
    // do not quote numbers or special scalars
    return scalar_is_plain_number_json(scalar)
        || scalar_is_special_json(scalar) ? ievt::PLAI : ievt::DQUO;
}
//see also NodeType implementation in scalar_style.cpp
inline evt_bits scalar_style_choose_block_ievt(csubstr scalar) noexcept
{
    if(scalar.len)
    {
        if(scalar_style_query_plain_block(scalar))
            return ievt::PLAI;
        RYML_ASSERT_BASIC_(scalar_style_query_squo(scalar)
                           && "if this assertion fires, please submit an issue!");
        return ievt::SQUO;
    }
    (void)scalar_style_choose_json_ievt;
    return scalar.str ? ievt::SQUO : ievt::PLAI;
}
inline evt_bits scalar_style_choose_flow_ievt(csubstr scalar) noexcept
{
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

C4_HOT C4_ALWAYS_INLINE bool hasall(evt_bits mask, evt_bits bits) noexcept
{
    return (mask & bits) == bits;
}
C4_HOT C4_ALWAYS_INLINE bool hasany(evt_bits mask, evt_bits bits) noexcept
{
    return (mask & bits);
}
C4_HOT C4_ALWAYS_INLINE bool hasnone(evt_bits mask, evt_bits bits) noexcept
{
    return !(mask & bits);
}
C4_HOT C4_ALWAYS_INLINE bool seqormap(evt_bits mask) noexcept
{
    return (mask & ievt::BEG_) && (mask & (ievt::SEQ_|ievt::MAP_));
}

inline evt_bits get_all_bits_key(evt_bits const* C4_RESTRICT evts, evt_size evts_size, evt_size pos) noexcept
{
    RYML_ASSERT_BASIC_(evts[pos] & ievt::KEY_);
    evt_bits accum = {};
    for( ; pos < evts_size; pos = ievt::nextpos(evts, pos))
    {
        if(evts[pos] & ievt::VAL_)
            break;
        accum |= evts[pos];
    }
    return accum;
}

inline bool has_next_doc_and_is_expl_(evt_bits const* C4_RESTRICT evts, evt_size evts_size, evt_size pos) noexcept
{
    RYML_ASSERT_BASIC_(evts[pos] & ievt::EDOC);
    while(++pos < evts_size)
    {
        if(hasall(evts[pos], ievt::ESTR))
            return false;
        else if(hasall(evts[pos], ievt::BDOC))
            return (evts[pos] & ievt::EXPL);
    }
    return false;
}
} // namespace detail


//-----------------------------------------------------------------------------

template<class Writer>
void EmitterInts<Writer>::emit_as(EmitType_e type,
                                  evt_bits const* evts,
                                  evt_size evts_size,
                                  csubstr src,
                                  csubstr arena)
{
    RYML_ASSERT_BASIC_(!!evts || !evts_size);
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
        emit_yaml_(0);
    else if(type == EMIT_JSON)
        json_emit_(0);
    else
        RYML_ERR_BASIC_("unknown emit type"); // LCOV_EXCL_LINE
    m_evts = nullptr;
    m_evts_size = 0;
    m_src = {};
    m_arena = {};
}

/** @cond dev */


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
    const evt_bits evt = m_evts[pos];

    // emit leading tokens, such as keys or comments
    const bool has_parent = detail::hasnone(evt, ievt::BSTR);
    const bool emit_key = has_parent && (evt & ievt::KEY_) && m_opts.emit_nonroot_key();
    const bool emit_dash = has_parent && !(evt & ievt::KEY_) && detail::hasnone(evt, ievt::BDOC) && m_opts.emit_nonroot_dash();
    RYML_ASSERT_BASIC_(!(emit_key && emit_dash));

    // emit opening tokens (such as tags, anchors or comments)
    if(emit_key)
    {
        blck_map_open_entry_(pos);
        ++m_ilevel;
    }
    else if(emit_dash)
    {
        blck_seq_open_entry_(pos);
        ++m_ilevel;
    }
    else
    {
        top_open_entry_(pos);
    }

    // emit the payload
    if(detail::hasall(evt, ievt::BSTR))
    {
        RYML_ASSERT_BASIC_(m_ilevel == 0);
        visit_stream_(pos);
    }
    else if(detail::hasall(evt, ievt::BDOC))
    {
        RYML_ASSERT_BASIC_(m_ilevel == 0);
        visit_doc_(pos, evt & ievt::EXPL);
    }
    else if(detail::seqormap(evt))
    {
        visit_blck_container_(pos);
    }
    else if(evt & ievt::SCLR)
    {
        visit_doc_val_(pos);
    }

    // emit closing tokens
    if(emit_key)
    {
        --m_ilevel;
        blck_close_entry_(pos);
    }
    else if(emit_dash)
    {
        --m_ilevel;
        blck_close_entry_(pos);
    }
    else
    {
        top_close_entry_(pos);
    }

    if(!has_parent
       || emit_dash || emit_key
       || !(evt & ievt::VAL_)
       || !(evt & ievt::PLAI))
    {
        write_pws_and_pend_(PWS_NONE_);
    }
}


//-----------------------------------------------------------------------------

template<class Writer>
void EmitterInts<Writer>::visit_stream_(evt_size &pos)
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
                visit_doc_(++pos, expl);
            }
            else if(expl)
            {
                newl_();
                pend_none_();
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
            ++pos;
        }
    }
    --m_depth;
#ifdef OLD
    auto write_tag_directives = [this](const evt_size next_node){
        (void)next_node;
        #ifdef RYML_TO_BE_IMPLEMENTED
        const evt_size doc = m_tree->ancestor_doc(next_node);
        const TagDirectiveRange tagds = m_tree->m_tag_directives.lookup_range(doc);
        if(tagds.e != tagds.b)
        {
            const evt_size parent = m_tree->parent(next_node);
            if(next_node != m_tree->first_child(parent))
            {
                write_pws_and_pend_(PWS_NEWL_);
                write_("...");
            }
        }
        for(TagDirective const& td : tagds)
        {
            write_pws_and_pend_(PWS_NONE_);
            write_("%TAG ");
            write_(td.handle);
            write_(' ');
            write_(td.prefix);
            pend_newl_();
        }
        #endif
    };
    const evt_size first_child = m_tree->first_child(id);
    if(first_child != NONE)
        write_tag_directives(first_child);
    ++m_depth;
    for(evt_size child = first_child; child != NONE; child = m_tree->next_sibling(child))
    {
        NodeType ty = m_tree->type(child);
        m_ilevel = 0;
        write_pws_and_pend_(PWS_NONE_);
        top_open_entry_(child);
        visit_doc_(child);
        top_close_entry_(child);
        if(ty.is_val())
        {
            if(ty.m_bits & VALNIL)
                pend_newl_();
        }
        else if(ty.is_container())
        {
            if(ty.is_flow())
                pend_newl_();
        }
        if(m_tree->next_sibling(child) != NONE)
        {
            write_tag_directives(m_tree->next_sibling(child));
        }
    }
    --m_depth;
#endif
}


//-----------------------------------------------------------------------------

template<class Writer>
void EmitterInts<Writer>::visit_doc_val_(evt_size &pos)
{
    // some plain scalars such as '...' and '---' must not
    // appear at 0-indentation
    evt_bits evt = m_evts[pos];
    RYML_ASSERT_BASIC_(detail::hasall(evt, ievt::VAL_|ievt::SCLR));
    RYML_ASSERT_BASIC_(detail::hasnone(evt, ievt::ALIA));
    const csubstr val = getstr_(pos);
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
#ifdef OLD
    NodeType ty = m_tree->type(id);
    const csubstr val = m_tree->val(id);
    type_bits val_style = ty.m_bits & VAL_STYLE;
    const bool is_ambiguous = ((ty.m_bits & VAL_PLAIN) || !val_style)
        && (val.begins_with("...") || val.begins_with("---"));
    if(is_ambiguous)
    {
        ++m_ilevel;
        if(m_pws != PWS_NONE_)
            pend_newl_();
        else
            indent_(m_ilevel);
    }
    write_pws_and_pend_(PWS_NONE_);
    if(m_tree->is_val_ref(id))
    {
        write_ref_(val);
    }
    else
    {
        if(!val_style)
            val_style = scalar_style_choose_block(val);
        blck_write_scalar_(val, val_style);
    }
    if(is_ambiguous)
    {
        --m_ilevel;
    }
#endif
}


//-----------------------------------------------------------------------------

template<class Writer>
void EmitterInts<Writer>::visit_doc_(evt_size &pos, bool begin_expl)
{
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
                visit_flow_container_(pos);
                if(evt & ievt::FMLX)
                    pend_newl_();
            }
            else
            {
                if(begin_expl || anchor_or_tag)
                    pend_newl_();
                visit_blck_container_(pos);
            }
            anchor_or_tag = false;
        }
        else if(evt & ievt::SCLR)
        {
            RYML_ASSERT_BASIC_(evt & ievt::VAL_);
            csubstr val = getstr_(pos);
            visit_doc_val_(pos);
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
            return;
        }
    }
#ifdef OLD
    const NodeType ty = m_tree->type(id);
    RYML_ASSERT_VISIT_(ty.is_doc(), m_tree, id);
    RYML_ASSERT_VISIT_(!ty.has_key(), m_tree, id);
    if(ty.is_container()) // this is more frequent
    {
        visit_blck_container_(id);
    }
    else if(ty.is_val())
    {
        visit_doc_val_(id);
    }
#endif
}


//-----------------------------------------------------------------------------

// to be called only at top level
template<class Writer>
void EmitterInts<Writer>::top_open_entry_(evt_size &node)
{
    (void)node;
#ifdef OLD
    NodeType ty = m_tree->type(node);
    if(ty.is_doc() && !m_tree->is_root(node))
    {
        write_("---");
        pend_space_();
    }
    if(ty.has_val_anchor())
    {
        write_pws_and_pend_(PWS_SPACE_);
        write_('&');
        write_(m_tree->val_anchor(node));
    }
    if(ty.has_val_tag())
    {
        write_pws_and_pend_(PWS_SPACE_);
        write_tag_(m_tree->val_tag(node));
    }
    if(m_pws == PWS_SPACE_)
    {
        if(ty.has_val())
        {
            if(ty.is_val_plain() && !m_tree->val(node).len)
                pend_none_();
        }
        else
        {
            RYML_ASSERT_VISIT_CB_(m_tree->callbacks(), ty.is_container(), m_tree, node);
            if(!ty.is_flow())
                pend_newl_();
        }
    }
#endif
}


//-----------------------------------------------------------------------------

template<class Writer>
void EmitterInts<Writer>::top_close_entry_(evt_size &node)
{
    (void)node;
#ifdef OLD
    NodeType ty = m_tree->type(node);
    if(ty.is_val() && !(ty.m_bits & VALNIL))
    {
        pend_newl_();
    }
#endif
}


//-----------------------------------------------------------------------------

template<class Writer>
void EmitterInts<Writer>::visit_blck_container_(evt_size &pos)
{
    evt_bits evt = m_evts[pos];
    RYML_ASSERT_BASIC_(detail::seqormap(evt));
    RYML_ASSERT_BASIC_(pos + 1 < m_evts_size);
    if(!(evt & (ievt::FLOW|ievt::BLCK)))
        evt |= (m_evts[pos + 1] & ievt::END_) ? ievt::FSL_ : ievt::BLCK;
    write_pws_and_pend_(PWS_NONE_);
    if(evt & ievt::FSL_)
        visit_flow_sl_(pos);
    else if(evt & ievt::FMLX)
        visit_flow_ml_(pos);
    else
        visit_blck_(pos);
    RYML_ASSERT_BASIC_(!(evt & ievt::END_));
#ifdef OLD
    NodeType ty = m_tree->type(id);
    if(!(ty.m_bits & CONTAINER_STYLE))
        ty.m_bits |= (m_tree->empty(id) ? FLOW_SL : BLOCK);
    write_pws_and_pend_(PWS_NONE_);
    if(ty.is_flow_sl())
        visit_flow_sl_(id);
    else if(ty.is_flow_mlx())
        visit_flow_ml_(id);
    else
        visit_blck_(id);
#endif
}

template<class Writer>
void EmitterInts<Writer>::visit_flow_container_(evt_size &pos)
{
    evt_bits evt = m_evts[pos];
    RYML_ASSERT_BASIC_(detail::seqormap(evt));
    RYML_ASSERT_BASIC_(pos + 1 < m_evts_size);
    if(!(evt & ievt::FLOW))
        evt |= ievt::FLOW|ievt::FSL_;
    write_pws_and_pend_(PWS_NONE_);
    if(evt & ievt::FMLX)
        visit_flow_ml_(pos);
    else
        visit_flow_sl_(pos);
    RYML_ASSERT_BASIC_(!(evt & ievt::END_));
#ifdef OLD
    NodeType ty = m_tree->type(id);
    if(!(ty.m_bits & CONTAINER_STYLE))
        ty.m_bits |= FLOW_SL;
    write_pws_and_pend_(PWS_NONE_);
    if(ty.is_flow_mlx())
        visit_flow_ml_(id);
    else // if(ty.is_flow_sl())
        visit_flow_sl_(id);
#endif
}


//-----------------------------------------------------------------------------

template<class Writer>
void EmitterInts<Writer>::flow_seq_open_entry_(evt_size &node)
{
    (void)node;
#ifdef OLD
    NodeType ty = m_tree->type(node);
    write_pws_and_pend_(PWS_NONE_);
    if(ty.m_bits & VALANCH)
    {
        write_pws_and_pend_(PWS_SPACE_);
        write_('&');
        write_(m_tree->val_anchor(node));
    }
    if(ty.m_bits & VALTAG)
    {
        write_pws_and_pend_(PWS_SPACE_);
        write_tag_(m_tree->val_tag(node));
    }
#endif
}


//-----------------------------------------------------------------------------

template<class Writer>
void EmitterInts<Writer>::flow_map_open_entry_(evt_size &node)
{
    (void)node;
#ifdef OLD
    NodeType ty = m_tree->type(node);
    write_pws_and_pend_(PWS_NONE_);
    RYML_ASSERT_VISIT_CB_(m_tree->callbacks(), ty.has_key(), m_tree, node);
    if(ty.m_bits & KEYANCH)
    {
        write_pws_and_pend_(PWS_SPACE_);
        write_("&");
        write_(m_tree->key_anchor(node));
    }
    if(ty.m_bits & KEYTAG)
    {
        write_pws_and_pend_(PWS_SPACE_);
        write_tag_(m_tree->key_tag(node));
    }
    if(ty.m_bits & KEYREF)
    {
        write_pws_and_pend_(PWS_SPACE_);
        write_ref_(m_tree->key(node));
    }
    else
    {
        write_pws_and_pend_(PWS_NONE_);
        csubstr key = m_tree->key(node);
        if(!(ty.m_bits & styles_flow_key_))
            ty.m_bits |= scalar_style_choose_flow(key) & styles_flow_key_;
        flow_write_scalar_(key, ty.m_bits & styles_flow_key_);
    }
    write_pws_and_pend_(PWS_SPACE_);
    write_(':');
    if(ty.m_bits & VALANCH)
    {
        write_pws_and_pend_(PWS_SPACE_);
        write_('&');
        write_(m_tree->val_anchor(node));
    }
    if(ty.m_bits & VALTAG)
    {
        write_pws_and_pend_(PWS_SPACE_);
        write_tag_(m_tree->val_tag(node));
    }
#endif
}


//-----------------------------------------------------------------------------

template<class Writer>
C4_NODISCARD bool EmitterInts<Writer>::maybe_start_flow_pws_ml_(evt_size node) noexcept
{
    RYML_ASSERT_BASIC_(m_evts[node] & ievt::FMLX);
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
void EmitterInts<Writer>::blck_seq_open_entry_(evt_size &pos)
{
    (void)pos;
#ifdef OLD
    NodeType ty = m_tree->type(node);
    write_pws_and_pend_(PWS_SPACE_); // pend the space after the following dash
    write_('-');
    bool has_tag_or_anchor = false;
    if(ty.m_bits & VALANCH)
    {
        has_tag_or_anchor = true;
        write_pws_and_pend_(PWS_SPACE_);
        write_('&');
        write_(m_tree->val_anchor(node));
    }
    if(ty.m_bits & VALTAG)
    {
        has_tag_or_anchor = true;
        write_pws_and_pend_(PWS_SPACE_);
        write_tag_(m_tree->val_tag(node));
    }
    if(has_tag_or_anchor && ty.is_container())
    {
        if(!(ty.m_bits & CONTAINER_STYLE))
            ty |= BLOCK;
        if((ty.m_bits & BLOCK) && m_tree->has_children(node))
            pend_newl_();
    }
#endif
}


//-----------------------------------------------------------------------------

template<class Writer>
void EmitterInts<Writer>::blck_map_open_entry_(evt_size &pos)
{
    (void)pos;
#ifdef OLD
    NodeType ty = m_tree->type(node);
    RYML_ASSERT_BASIC_(ty.has_key());
    csubstr key = m_tree->key(node);
    if(!(ty.m_bits & (KEY_STYLE|KEYREF)))
        ty.m_bits |= (scalar_style_choose_block(key).m_bits & KEY_STYLE);
    write_pws_and_pend_(PWS_NONE_);
    if(ty.m_bits & KEYANCH)
    {
        write_pws_and_pend_(PWS_SPACE_);
        write_('&');
        write_(m_tree->key_anchor(node));
    }
    if(ty.m_bits & KEYTAG)
    {
        write_pws_and_pend_(PWS_SPACE_);
        write_tag_(m_tree->key_tag(node));
    }
    if(ty.m_bits & KEYREF)
    {
        write_pws_and_pend_(PWS_SPACE_);
        write_ref_(key);
    }
    else
    {
        write_pws_and_pend_(PWS_NONE_);
        const type_bits use_qmrk = ty.m_bits & styles_block_key_;
        if(!use_qmrk)
        {
            blck_write_scalar_(key, ty.m_bits & KEY_STYLE);
        }
        else
        {
            write_("? ");
            blck_write_scalar_(key, ty.m_bits & KEY_STYLE);
            pend_newl_();
        }
    }
    write_pws_and_pend_(PWS_SPACE_); // pend the space after the colon
    write_(':');
    if(ty.m_bits & VALANCH)
    {
        write_pws_and_pend_(PWS_SPACE_);
        write_('&');
        write_(m_tree->val_anchor(node));
    }
    if(ty.m_bits & VALTAG)
    {
        write_pws_and_pend_(PWS_SPACE_);
        write_tag_(m_tree->val_tag(node));
    }
    if(ty.is_container() && m_tree->has_children(node))
    {
        if(!(ty.m_bits & CONTAINER_STYLE))
            ty |= BLOCK;
        if(ty.is_block())
            pend_newl_();
    }
#endif
}


//-----------------------------------------------------------------------------

template<class Writer>
void EmitterInts<Writer>::blck_close_entry_(evt_size &pos)
{
    (void)pos;
#ifdef OLD
    (void)node;
    pend_newl_();
#endif
}


//-----------------------------------------------------------------------------

template<class Writer>
void EmitterInts<Writer>::visit_blck_seq_(evt_size &pos)
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
            visit_blck_container_(pos);
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
            ++pos;
        }
        continue;
    nextval:
        newval = true;
        has_tag_or_anchor = false;
    }
#ifdef OLD
    RYML_ASSERT_BASIC_(m_tree->is_seq(node), m_tree, node);
    bool empty = true;
    for(evt_size child = m_tree->first_child(node); child != NONE; child = m_tree->next_sibling(child))
    {
        empty = false;
        NodeType ty = m_tree->type(child);
        RYML_ASSERT_BASIC_(ty.is_val() || ty.is_container() || ty == NOTYPE, m_tree, node);
        blck_seq_open_entry_(child);
        if(ty.is_val())
        {
            write_pws_and_pend_(PWS_NONE_);
            csubstr val = m_tree->val(child);
            if(!ty.is_val_ref())
            {
                if(!(ty.m_bits & VAL_STYLE))
                    ty.m_bits |= (scalar_style_choose_block(val).m_bits & VAL_STYLE);
                blck_write_scalar_(val, ty.m_bits & VAL_STYLE);
            }
            else
            {
                write_ref_(val);
            }
        }
        else if(ty.is_container())
        {
            ++m_depth;
            ++m_ilevel;
            visit_blck_container_(child);
            --m_depth;
            --m_ilevel;
        }
        blck_close_entry_(child);
    }
    if(empty)
    {
        write_pws_and_pend_(PWS_NONE_);
        write_("[]");
    }
#endif
}


//-----------------------------------------------------------------------------


template<class Writer>
void EmitterInts<Writer>::visit_blck_map_(evt_size &pos)
{
    RYML_ASSERT_BASIC_(pos + 1 < m_evts_size);
    RYML_ASSERT_BASIC_(detail::hasall(m_evts[pos], ievt::BMAP));
    bool statenew = true;
    bool statekey = true;
    bool has_tag_or_anchor = false;
    bool qmark = false;
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
                evt_bits bits = detail::get_all_bits_key(m_evts, m_evts_size, pos);
                qmark = bits & (ievt::SEQ_|ievt::MAP_|ievt::LITL|ievt::FOLD);
                if(!qmark)
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
            (void)has_tag_or_anchor;
            if(!(evt & detail::styles_ievt_cont))
                evt |= ievt::BLCK;
            ++m_depth;
            ++m_ilevel;
            if(evt & ievt::BLCK)
                pend_newl_();
            write_pws_and_pend_(PWS_NONE_);
            visit_blck_container_(pos);
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
            ++pos;
        }
        continue;
    statenext:
        statenew = true;
        statekey = !statekey;
    }
#ifdef OLD
    RYML_ASSERT_BASIC_(m_tree->is_map(node), m_tree, node);
    bool empty = true;
    for(evt_size child = m_tree->first_child(node); child != NONE; child = m_tree->next_sibling(child))
    {
        empty = false;
        NodeType ty = m_tree->type(child);
        RYML_ASSERT_BASIC_(ty.is_keyval() || ty.is_container() || ty == NOTYPE, m_tree, node);
        blck_map_open_entry_(child); // also writes the key
        if(ty.is_keyval())
        {
            write_pws_and_pend_(PWS_NONE_);
            csubstr val = m_tree->val(child);
            if(!ty.is_val_ref())
            {
                if(!(ty.m_bits & VAL_STYLE))
                    ty |= (scalar_style_choose_block(val).m_bits & VAL_STYLE);
                blck_write_scalar_(val, ty.m_bits & VAL_STYLE);
            }
            else
            {
                write_ref_(val);
            }
        }
        else if(ty.is_container())
        {
            ++m_depth;
            ++m_ilevel;
            visit_blck_container_(child);
            --m_depth;
            --m_ilevel;
        }
        blck_close_entry_(child);
    }
    if(empty)
    {
        write_pws_and_pend_(PWS_NONE_);
        write_("{}");
    }
#endif
}


//-----------------------------------------------------------------------------

template<class Writer>
void EmitterInts<Writer>::visit_flow_sl_seq_(evt_size &pos)
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
            visit_flow_container_(pos);
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
            ++pos;
        }
        continue;
    nextval:
        flow_close_entry_sl_(pos, pws.next_pws(m_col));
    }
    write_(']');
#ifdef OLD
    RYML_ASSERT_BASIC_(m_tree->is_seq(node), m_tree, node);
    const flow_pws pws = setup_flow_pws_sl_(node);
    write_('[');
    for(evt_size child = m_tree->first_child(node), last = m_tree->last_child(node); child != NONE; child = m_tree->next_sibling(child))
    {
        NodeType ty = m_tree->type(child);
        RYML_ASSERT_BASIC_((ty & (VAL|SEQ|MAP)) || ty == NOTYPE, m_tree, node);
        flow_seq_open_entry_(child);
        if(ty.m_bits & VAL)
        {
            write_pws_and_pend_(PWS_NONE_);
            csubstr val = m_tree->val(child);
            if(!ty.is_val_ref())
            {
                if(!(ty.m_bits & styles_flow_val_))
                    ty.m_bits |= (scalar_style_choose_flow(val).m_bits & styles_flow_val_);
                flow_write_scalar_(val, ty.m_bits & styles_flow_val_);
            }
            else
            {
                write_ref_(val);
            }
        }
        else if(ty.m_bits & (SEQ|MAP))
        {
            ++m_depth;
            visit_flow_container_(child);
            --m_depth;
        }
        flow_close_entry_sl_(child, last, pws.next_pws(m_col));
    }
    write_(']');
#endif
}


//-----------------------------------------------------------------------------

template<class Writer>
void EmitterInts<Writer>::visit_flow_ml_seq_(evt_size &pos)
{
    RYML_ASSERT_BASIC_(pos + 1 < m_evts_size);
    RYML_ASSERT_BASIC_(detail::hasall(m_evts[pos], ievt::BSEQ));
    write_('[');
    newl_();
    if(m_opts.indent_flow_ml()) ++m_ilevel;
    indent_(m_ilevel);
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
            visit_flow_container_(pos);
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
            ++pos;
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
#ifdef OLD
    RYML_ASSERT_BASIC_(m_tree->is_seq(node));
    write_('[');
    pend_newl_();
    if(m_opts.indent_flow_ml()) ++m_ilevel;
    const bool stop_at_end = maybe_start_flow_pws_ml_(node);
    for(evt_size child = m_tree->first_child(node), last = m_tree->last_child(node); child != NONE; child = m_tree->next_sibling(child))
    {
        NodeType ty = m_tree->type(child);
        RYML_ASSERT_BASIC_(ty.is_val() || ty.is_container() || ty == NOTYPE, m_tree, node);
        flow_seq_open_entry_(child);
        if(ty.is_val())
        {
            write_pws_and_pend_(PWS_NONE_);
            csubstr val = m_tree->val(child);
            if(!ty.is_val_ref())
            {
                if(!(ty.m_bits & styles_flow_val_))
                    ty.m_bits |= (scalar_style_choose_flow(val).m_bits & styles_flow_val_);
                flow_write_scalar_(val, ty.m_bits & styles_flow_val_);
            }
            else
            {
                write_ref_(val);
            }
        }
        else if(ty.is_container())
        {
            ++m_depth;
            visit_flow_container_(child);
            --m_depth;
        }
        flow_close_entry_ml_(child, last, m_flow_pws.next_pws(m_col));
    }
    if(stop_at_end)
        m_flow_pws.stop();
    if(m_opts.indent_flow_ml()) --m_ilevel;
    write_pws_and_pend_(PWS_NONE_);
    write_(']');
#endif
}


//-----------------------------------------------------------------------------

template<class Writer>
void EmitterInts<Writer>::visit_flow_sl_map_(evt_size &pos)
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
            if(!statekey)
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
            visit_flow_container_(pos);
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
            ++pos;
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
#ifdef OLD
    RYML_ASSERT_BASIC_(m_tree->is_map(node), m_tree, node);
    flow_pws pws = setup_flow_pws_sl_(node);
    write_('{');
    for(evt_size child = m_tree->first_child(node), last = m_tree->last_child(node); child != NONE; child = m_tree->next_sibling(child))
    {
        NodeType ty = m_tree->type(child);
        RYML_ASSERT_BASIC_(ty.has_key() && (ty.has_val() || ty.is_container() || ty == NOTYPE), m_tree, node);
        flow_map_open_entry_(child);
        if(ty.has_val())
        {
            write_pws_and_pend_(PWS_NONE_);
            csubstr val = m_tree->val(child);
            if(!ty.is_val_ref())
            {
                if(!(ty.m_bits & styles_flow_val_))
                    ty.m_bits |= (scalar_style_choose_flow(val).m_bits & styles_flow_val_);
                flow_write_scalar_(val, ty.m_bits & styles_flow_val_);
            }
            else
            {
                write_ref_(val);
            }
        }
        else if(ty.is_container())
        {
            ++m_depth;
            visit_flow_container_(child);
            --m_depth;
        }
        flow_close_entry_sl_(child, last, pws.next_pws(m_col));
    }
    write_pws_and_pend_(PWS_NONE_);
    write_('}');
#endif
}


//-----------------------------------------------------------------------------

template<class Writer>
void EmitterInts<Writer>::visit_flow_ml_map_(evt_size &pos)
{
    RYML_ASSERT_BASIC_(pos + 1 < m_evts_size);
    RYML_ASSERT_BASIC_(detail::hasall(m_evts[pos], ievt::BMAP));
    write_('{');
    pend_newl_();
    if(m_opts.indent_flow_ml()) ++m_ilevel;
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
            if(!statekey)
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
            visit_blck_container_(pos);
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
            ++pos;
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
#ifdef OLD
    RYML_ASSERT_BASIC_(m_tree->is_map(node), m_tree, node);
    write_('{');
    pend_newl_();
    if(m_opts.indent_flow_ml()) ++m_ilevel;
    const bool stop_at_end = maybe_start_flow_pws_ml_(node);
    for(evt_size child = m_tree->first_child(node), last = m_tree->last_child(node); child != NONE; child = m_tree->next_sibling(child))
    {
        NodeType ty = m_tree->type(child);
        RYML_ASSERT_BASIC_(ty.has_key() && (ty.has_val() || ty.is_container() || ty == NOTYPE), m_tree, node);
        flow_map_open_entry_(child);
        if(ty.has_val())
        {
            write_pws_and_pend_(PWS_NONE_);
            csubstr val = m_tree->val(child);
            if(!ty.is_val_ref())
            {
                if(!(ty.m_bits & styles_flow_val_))
                    ty.m_bits |= (scalar_style_choose_flow(val).m_bits & styles_flow_val_);
                flow_write_scalar_(val, ty.m_bits & styles_flow_val_);
            }
            else
            {
                write_ref_(val);
            }
        }
        else if(ty.is_container())
        {
            ++m_depth;
            visit_flow_container_(child);
            --m_depth;
        }
        flow_close_entry_ml_(child, last, m_flow_pws.next_pws(m_col));
    }
    if(stop_at_end)
        m_flow_pws.stop();
    if(m_opts.indent_flow_ml()) --m_ilevel;
    write_pws_and_pend_(PWS_NONE_);
    write_('}');
#endif
}


//-----------------------------------------------------------------------------

template<class Writer>
void EmitterInts<Writer>::visit_blck_(evt_size &node)
{
    evt_bits evt = m_evts[node];
    RYML_ASSERT_BASIC_(!(evt & ievt::STRM));
    RYML_ASSERT_BASIC_(detail::seqormap(evt) || detail::hasall(evt, ievt::BDOC));
    if C4_UNLIKELY(m_depth > (evt_size)m_opts.max_depth())
        RYML_ERR_BASIC_("max depth exceeded");
    if(detail::hasall(evt, ievt::BSEQ))
    {
        visit_blck_seq_(node);
    }
    else
    {
        RYML_ASSERT_BASIC_(detail::hasall(evt, ievt::BMAP));
        visit_blck_map_(node);
    }
#ifdef OLD
    const NodeType ty = m_tree->type(node);
    RYML_ASSERT_BASIC_(!ty.is_stream(), m_tree, node);
    RYML_ASSERT_BASIC_(ty.is_container() || ty.is_doc(), m_tree, node);
    RYML_ASSERT_BASIC_(m_tree->is_root(node) || (m_tree->parent_is_map(node) || m_tree->parent_is_seq(node)), m_tree, node);
    if C4_UNLIKELY(m_depth > m_opts.max_depth())
        RYML_ERR_BASIC_(m_tree, node, "max depth exceeded");
    if(ty.is_seq())
    {
        visit_blck_seq_(node);
    }
    else
    {
        RYML_ASSERT_BASIC_(ty.is_map(), m_tree, node);
        visit_blck_map_(node);
    }
#endif
}


//-----------------------------------------------------------------------------

template<class Writer>
void EmitterInts<Writer>::visit_flow_sl_(evt_size &pos)
{
    evt_bits evt = m_evts[pos];
    RYML_ASSERT_BASIC_(!(evt & ievt::STRM));
    RYML_ASSERT_BASIC_(detail::seqormap(evt) || detail::hasall(evt, ievt::BDOC));
    if C4_UNLIKELY(m_depth > (evt_size)m_opts.max_depth())
        RYML_ERR_BASIC_("max depth exceeded");
    if(evt & ievt::SEQ_)
    {
        visit_flow_sl_seq_(pos);
    }
    else
    {
        RYML_ASSERT_BASIC_(evt & ievt::MAP_);
        visit_flow_sl_map_(pos);
    }
#ifdef OLD
    const NodeType ty = m_tree->type(node);
    RYML_ASSERT_BASIC_(!ty.is_stream(), m_tree, node);
    RYML_ASSERT_BASIC_(ty.is_container() || ty.is_doc(), m_tree, node);
    RYML_ASSERT_BASIC_(m_tree->is_root(node) || (m_tree->parent_is_map(node) || m_tree->parent_is_seq(node)), m_tree, node);
    if C4_UNLIKELY(m_depth > m_opts.max_depth())
        RYML_ERR_BASIC_(m_tree, node, "max depth exceeded");
    if(ty.m_bits & SEQ)
    {
        visit_flow_sl_seq_(node);
    }
    else
    {
        RYML_ASSERT_BASIC_(ty.is_map(), m_tree, node);
        visit_flow_sl_map_(node);
    }
#endif
}


//-----------------------------------------------------------------------------

template<class Writer>
void EmitterInts<Writer>::visit_flow_ml_(evt_size &pos)
{
    evt_bits evt = m_evts[pos];
    RYML_ASSERT_BASIC_(!(evt & ievt::STRM));
    RYML_ASSERT_BASIC_(detail::seqormap(evt) || detail::hasall(evt, ievt::BDOC));
    if C4_UNLIKELY(m_depth > (evt_size)m_opts.max_depth())
        RYML_ERR_BASIC_("max depth exceeded");
    if(evt & ievt::SEQ_)
    {
        visit_flow_ml_seq_(pos);
    }
    else
    {
        RYML_ASSERT_BASIC_(evt & ievt::MAP_);
        visit_flow_ml_map_(pos);
    }
#ifdef OLD
    const NodeType ty = m_tree->type(node);
    RYML_ASSERT_BASIC_(!ty.is_stream(), m_tree, node);
    RYML_ASSERT_BASIC_(ty.is_container() || ty.is_doc(), m_tree, node);
    RYML_ASSERT_BASIC_(m_tree->is_root(node) || (m_tree->parent_is_map(node) || m_tree->parent_is_seq(node)), m_tree, node);
    if C4_UNLIKELY(m_depth > m_opts.max_depth())
        RYML_ERR_BASIC_(m_tree, node, "max depth exceeded");
    if(ty.m_bits & SEQ)
    {
        visit_flow_ml_seq_(node);
    }
    else
    {
        RYML_ASSERT_BASIC_(ty.is_map(), m_tree, node);
        visit_flow_ml_map_(node);
    }
#endif
}


//-----------------------------------------------------------------------------

template<class Writer>
void EmitterInts<Writer>::flow_write_scalar_(csubstr str, evt_bits evt)
{
    RYML_ASSERT_BASIC_(!(evt & ievt::BLCK));
    if((evt & ievt::PLAI) || !(evt & detail::styles_ievt_sclr))
    {
        write_scalar_plain_(str, m_ilevel);
    }
    else if(evt & ievt::SQUO)
    {
        write_scalar_squo_(str, m_ilevel);
    }
    else // if(evt & ievt::DQUO)
    {
        write_scalar_dquo_(str, m_ilevel);
    }
}

template<class Writer>
void EmitterInts<Writer>::blck_write_scalar_(csubstr str, evt_bits evt)
{
    if((evt & ievt::PLAI) || !(evt & detail::styles_ievt_sclr))
    {
        write_scalar_plain_(str, m_ilevel);
    }
    else if(evt & ievt::SQUO)
    {
        write_scalar_squo_(str, m_ilevel);
    }
    else if(evt & ievt::DQUO)
    {
        write_scalar_dquo_(str, m_ilevel);
    }
    else if(evt & ievt::LITL)
    {
        write_scalar_literal_(str, m_ilevel);
    }
    else // if(ty & ievt::FOLD)
    {
        write_scalar_folded_(str, m_ilevel);
    }
}

template<class Writer>
size_t EmitterInts<Writer>::write_escaped_newlines_(csubstr s, size_t i)
{
    RYML_ASSERT_BASIC_(s.len > i);
    RYML_ASSERT_BASIC_(s.str[i] == '\n');
    //_c4dbgpf("nl@i={} rem=[{}]~~~{}~~~", i, s.sub(i).len, s.sub(i));
    // add an extra newline for each sequence of consecutive
    // newline/whitespace
    newl_();
    do
    {
        newl_(); // write the newline again
        ++i; // increase the outer loop counter!
    } while(i < s.len && s.str[i] == '\n');
    RYML_ASSERT_BASIC_(i > 0);
    --i;
    RYML_ASSERT_BASIC_(s.str[i] == '\n');
    return i;
}


inline bool _is_indented_block(csubstr s, size_t prev, size_t i) noexcept
{
    if(prev == 0 && s.begins_with_any(" \t"))
        return true;
    const size_t pos = s.first_not_of('\n', i);
    return (pos != npos) && (s.str[pos] == ' ' || s.str[pos] == '\t');
}


template<class Writer>
size_t EmitterInts<Writer>::write_indented_block_(csubstr s, size_t i, evt_size ilevel)
{
    //_c4dbgpf("indblock@i={} rem=[{}]~~~\n{}~~~", i, s.sub(i).len, s.sub(i));
    RYML_ASSERT_BASIC_(i > 0);
    RYML_ASSERT_BASIC_(s.str[i-1] == '\n');
    RYML_ASSERT_BASIC_(i < s.len);
    RYML_ASSERT_BASIC_(s.str[i] == ' ' || s.str[i] == '\t' || s.str[i] == '\n');
again:
    size_t pos = s.find("\n ", i);
    if(pos == npos)
        pos = s.find("\n\t", i);
    if(pos != npos)
    {
        ++pos;
        //_c4dbgpf("indblock line@i={} rem=[{}]~~~\n{}~~~", i, s.range(i, pos).len, s.range(i, pos));
        indent_(ilevel + 1);
        write_(s.range(i, pos));
        i = pos;
        goto again; // NOLINT
    }
    // consume the newlines after the indented block
    // to prevent them from being escaped
    pos = s.find('\n', i);
    if(pos != npos)
    {
        const size_t pos2 = s.first_not_of('\n', pos);
        pos = (pos2 != npos) ? pos2 : pos;
        //_c4dbgpf("indblock line@i={} rem=[{}]~~~\n{}~~~", i, s.range(i, pos).len, s.range(i, pos));
        indent_(ilevel + 1);
        write_(s.range(i, pos));
        i = pos;
    }
    return i;
}

template<class Writer>
void EmitterInts<Writer>::write_scalar_literal_(csubstr s, evt_size ilevel)
{
    RYML_ASSERT_BASIC_(s.find("\r") == csubstr::npos);
    csubstr trimmed = s.trimr('\n');
    const size_t numnewlines_at_end = s.len - trimmed.len;
    const bool is_newline_only = (trimmed.len == 0 && (s.len > 0));
    const bool explicit_indentation = s.triml("\n\r").begins_with_any(" \t");
    //
    write_('|');
    if(explicit_indentation)
        write_('2');
    //
    if(numnewlines_at_end > 1 || is_newline_only)
        write_('+');
    else if(numnewlines_at_end == 0)
        write_('-');
    //
    if(trimmed.len)
    {
        newl_();
        size_t pos = 0; // tracks the last character that was already written
        for(size_t i = 0; i < trimmed.len; ++i)
        {
            if(trimmed[i] != '\n')
                continue;
            // write everything up to this point
            csubstr since_pos = trimmed.range(pos, i+1); // include the newline
            indent_(ilevel + 1);
            write_(since_pos);
            pos = i+1; // already written
        }
        if(pos < trimmed.len)
        {
            indent_(ilevel + 1);
            write_(trimmed.sub(pos));
        }
    }
    for(size_t i = !is_newline_only; i < numnewlines_at_end; ++i)
        newl_();
}

template<class Writer>
void EmitterInts<Writer>::write_scalar_folded_(csubstr s, evt_size ilevel)
{
    RYML_ASSERT_BASIC_(s.find("\r") == csubstr::npos);
    csubstr trimmed = s.trimr('\n');
    const size_t numnewlines_at_end = s.len - trimmed.len;
    const bool is_newline_only = (trimmed.len == 0 && (s.len > 0));
    const bool explicit_indentation = s.triml("\n\r").begins_with_any(" \t");
    //
    write_('>');
    if(explicit_indentation)
        write_('2');
    //
    if(numnewlines_at_end == 0)
        write_('-');
    else if(numnewlines_at_end > 1 || is_newline_only)
        write_('+');
    //
    if(trimmed.len)
    {
        newl_();
        size_t pos = 0; // tracks the last character that was already written
        for(size_t i = 0; i < trimmed.len; ++i)
        {
            if(trimmed[i] != '\n')
                continue;
            // escape newline sequences
            if( ! _is_indented_block(s, pos, i))
            {
                if(pos < i)
                {
                    indent_(ilevel + 1);
                    write_(s.range(pos, i));
                    i = write_escaped_newlines_(s, i);
                    pos = i + 1;
                }
                else
                {
                    if(i+1 < s.len)
                    {
                        if(s.str[i+1] == '\n')
                        {
                            ++i;
                            i = write_escaped_newlines_(s, i);
                            pos = i+1;
                        }
                        else
                        {
                            newl_();
                            pos = i+1;
                        }
                    }
                }
            }
            else // do not escape newlines in indented blocks
            {
                ++i;
                indent_(ilevel + 1);
                write_(s.range(pos, i));
                if(pos > 0 || !s.begins_with_any(" \t"))
                    i = write_indented_block_(s, i, ilevel);
                pos = i;
            }
        }
        if(pos < trimmed.len)
        {
            indent_(ilevel + 1);
            write_(trimmed.sub(pos));
        }
    }
    for(size_t i = !is_newline_only; i < numnewlines_at_end; ++i)
        newl_();
}

template<class Writer>
void EmitterInts<Writer>::write_scalar_squo_(csubstr s, evt_size ilevel)
{
    size_t pos = 0; // tracks the last character that was already written
    write_('\'');
    for(size_t i = 0; i < s.len; ++i)
    {
        if(s[i] == '\n')
        {
            write_(s.range(pos, i));  // write everything up to (excluding) this char
            //_c4dbgpf("newline at {}. writing ~~~{}~~~", i, s.range(pos, i));
            i = write_escaped_newlines_(s, i);
            //_c4dbgpf("newline --> {}", i);
            if(i < s.len)
                indent_(ilevel + 1);
            pos = i+1;
        }
        else if(s[i] == '\'')
        {
            csubstr sub = s.range(pos, i+1);
            //_c4dbgpf("squote at {}. writing ~~~{}~~~", i, sub);
            write_(sub); // write everything up to (including) this squote
            write_('\''); // write the squote again
            pos = i+1;
        }
    }
    // write remaining characters at the end of the string
    if(pos < s.len)
        write_(s.sub(pos));
    write_('\'');
}

template<class Writer>
void EmitterInts<Writer>::write_scalar_dquo_(csubstr s, evt_size ilevel)
{
    size_t pos = 0; // tracks the last character that was already written
    write_('"');
    for(size_t i = 0; i < s.len; ++i)
    {
        const char curr = s.str[i];
        switch(curr) // NOLINT
        {
        case '"':
        case '\\':
        {
            csubstr sub = s.range(pos, i);
            write_(sub);  // write everything up to (excluding) this char
            write_('\\'); // write the escape
            write_(curr); // write the char
            pos = i+1;
            break;
        }
        case '\n':
        {
            csubstr sub = s.range(pos, i);
            write_(sub);   // write everything up to (excluding) this char
            write_("\\n"); // write the escape
            pos = i+1;
            (void)ilevel;
            break;
        }
        case '\r':
        {
            csubstr sub = s.range(pos, i);
            write_(sub);  // write everything up to (excluding) this char
            write_("\\r"); // write the escaped char
            pos = i+1;
            break;
        }
        case '\b':
        {
            csubstr sub = s.range(pos, i);
            write_(sub);  // write everything up to (excluding) this char
            write_("\\b"); // write the escaped char
            pos = i+1;
            break;
        }
        }
    }
    // write remaining characters at the end of the string
    if(pos < s.len)
        write_(s.sub(pos));
    write_('"');
}

template<class Writer>
void EmitterInts<Writer>::write_scalar_plain_(csubstr s, evt_size ilevel)
{
    if C4_UNLIKELY(ilevel == 0 && (s.begins_with("...") || s.begins_with("---")))
    {
        indent_(ilevel + 1); // indent the next line
        ++ilevel;
    }
    size_t pos = 0; // tracks the last character that was already written
    for(size_t i = 0; i < s.len; ++i)
    {
        const char curr = s.str[i];
        if(curr == '\n')
        {
            csubstr sub = s.range(pos, i);
            write_(sub);  // write everything up to (including) this newline
            i = write_escaped_newlines_(s, i);
            pos = i+1;
            if(pos < s.len)
                indent_(ilevel + 1); // indent the next line
        }
    }
    // write remaining characters at the end of the string
    if(pos < s.len)
        write_(s.sub(pos));
}


//-----------------------------------------------------------------------------

namespace detail {
inline type_bits json_type_(type_bits ty)
{
    enum : type_bits { // NOLINT
        ml_bits = (BLOCK|(STREAM & ~SEQ)), // remove SEQ from STREAM to test
        sl_bits = (CONTAINER_STYLE & ~FLOW_SPC),
    };
    if(ty & ml_bits)
    {
        ty &= ~BLOCK;
        ty |= FLOW_ML1;
    }
    else if((ty & (SEQ|MAP)) && !(ty & sl_bits))
    {
        ty |= FLOW_SL;
    }
    return ty;
}
} // namespace detail


template<class Writer>
void EmitterInts<Writer>::json_emit_(evt_size pos)
{
    (void)pos;
#ifdef OLD
    NodeType ty = m_tree->type(id);
    // JSON does not have streams
    if C4_UNLIKELY(ty.is_stream() && m_opts.json_err_on_stream())
        RYML_ERR_BASIC_(m_tree, id, "found stream node");
    static_assert(STREAM & SEQ, "STREAM must be a SEQ");
    ty = json_type_(ty);
    if(ty.is_flow_mlx())
    {
        json_visit_ml_(id, ty, 0);
        newl_();
    }
    else
    {
        json_visit_sl_(id, ty, 0);
    }
#endif
}

template<class Writer>
void EmitterInts<Writer>::json_visit_sl_(evt_size &pos, evt_bits ty, evt_size depth)
{
    if C4_UNLIKELY(depth > m_opts.max_depth())
        RYML_ERR_BASIC_("max depth exceeded");
    (void)pos;
    (void)ty;
    (void)depth;
#ifdef OLD
    if(ty.is_val())
    {
        json_writev_(id, ty);
    }
    else if(ty.is_keyval())
    {
        json_writek_(id, ty);
        write_(": ");
        json_writev_(id, ty);
    }
    else if(ty.is_container())
    {
        ty = json_type_(ty);
        if(ty.has_key())
        {
            json_writek_(id, ty);
            write_(": ");
        }
        if(ty.is_seq())
            write_('[');
        else if(ty.is_map())
            write_('{');

        for(evt_size child = m_tree->first_child(id); child != NONE; child = m_tree->next_sibling(child))
        {
            if(child != m_tree->first_child(id))
            {
                if((ty & FLOW_SPC) || m_opts.force_flow_spc())
                    write_(", ");
                else
                    write_(',');
            }
            json_visit_sl_(child, m_tree->type(child), depth+1);
        }

        if(ty.is_seq())
            write_(']');
        else if(ty.is_map())
            write_('}');
    }  // container
#endif
}

template<class Writer>
void EmitterInts<Writer>::json_visit_ml_(evt_size &pos, evt_bits ty, evt_size depth)
{
    if C4_UNLIKELY(depth > m_opts.max_depth())
        RYML_ERR_BASIC_("max depth exceeded");
    (void)pos;
    (void)ty;
    (void)depth;
#ifdef OLD
    if(ty.is_val())
    {
        json_writev_(id, ty);
    }
    else if(ty.is_keyval())
    {
        json_writek_(id, ty);
        write_(": ");
        json_writev_(id, ty);
    }
    else if(ty.is_container())
    {
        ty = json_type_(ty);
        if(ty.has_key())
        {
            json_writek_(id, ty);
            write_(": ");
        }
        if(ty.is_seq())
            write_('[');
        else if(ty.is_map())
            write_('{');

        if(m_tree->has_children(id))
        {
            ++depth;
            if(m_opts.indent_flow_ml()) ++m_ilevel;
            newl_();
            indent_(m_ilevel);
            for(evt_size first = m_tree->first_child(id), child = first;
                child != NONE;
                child = m_tree->next_sibling(child))
            {
                if(child != first)
                {
                    write_(',');
                    const size_t maxcols = m_opts.max_cols();
                    if((ty.m_bits & FLOW_MLN) && (m_col+1 < maxcols))
                    {
                        if((ty.m_bits & FLOW_SPC) || m_opts.force_flow_spc())
                            write_(' ');
                    }
                    else if((ty.m_bits & FLOW_ML1) || (m_col+1 >= maxcols))
                    {
                        newl_();
                        indent_(m_ilevel);
                    }
                }
                NodeType chty = m_tree->type(child);
                if(chty.is_flow_sl())
                    json_visit_sl_(child, chty, depth);
                else
                    json_visit_ml_(child, chty, depth);
            }
            if(m_opts.indent_flow_ml()) --m_ilevel;
            --depth;
            newl_();
            indent_(m_ilevel);
        }

        if(ty.is_seq())
            write_(']');
        else if(ty.is_map())
            write_('}');
    }
#endif
}

template<class Writer>
bool EmitterInts<Writer>::json_maybe_write_naninf_(csubstr s)
{
    switch(s.len)
    {
    case 3: case 4: case 5: // inf, nan, .nan, -.inf
    case 8: case 9: // infinity, -infinity
        break;
    default:
        return false;
    }
    const char first = s.str[0];
    csubstr rest = s.sub(1);
    if(s.len == 4 && first == '.')
    {
        if(scalar_is_inf3(rest.str))
            goto write_inf_positive; // NOLINT
        else if(scalar_is_nan3(rest.str))
            goto write_nan; // NOLINT
    }
    else if(first == '-' || first == '+') // begins with sign: must be inf
    {
        // match [-+].inf
        if((rest.len == 4 && rest.str[0] == '.' && scalar_is_inf3(rest.str + 1))
           // match [-+]inf
           || (rest.len == 3 && scalar_is_inf3(rest.str))
           // match [-+]infinity
           || (rest.len == 8 && (0 == memcmp(rest.str, "infinity", 8))))
        {
            if(first == '-')
                goto write_inf_negative; // NOLINT
            else
                goto write_inf_positive; // NOLINT
        }
    }
    else if(s.len == 8 && (0 == memcmp(s.str, "infinity", 8)))
    {
        goto write_inf_positive; // NOLINT
    }
    else if(s.len == 3)
    {
        if(scalar_is_inf3(s.str))
            goto write_inf_positive; // NOLINT
        else if(scalar_is_nan3(s.str))
            goto write_nan; // NOLINT
    }
    return false;
write_inf_positive:
    write_("\".inf\"");
    return true;
write_inf_negative:
    write_("\"-.inf\"");
    return true;
write_nan:
    write_("\".nan\"");
    return true;
}

template<class Writer>
void EmitterInts<Writer>::json_writek_(evt_size &id, evt_bits ty)
{
    if C4_UNLIKELY((ty & ievt::TAG_) && m_opts.json_err_on_tag())
        RYML_ERR_BASIC_("JSON does not have tags");
    if C4_UNLIKELY((ty & ievt::ANCH) && m_opts.json_err_on_anchor())
        RYML_ERR_BASIC_("JSON does not have anchors");
    (void)id;
    (void)ty;
#ifdef OLD
    csubstr key = m_tree->key(id);
    if(key.len)
    {
        if(json_maybe_write_naninf_(key))
            ;
        else
            json_write_scalar_dquo_(key);
    }
    else
    {
        write_("\"\"");
    }
#endif
}

template<class Writer>
void EmitterInts<Writer>::json_writev_(evt_size &id, evt_bits ty)
{
    if C4_UNLIKELY((ty & ievt::TAG_) && m_opts.json_err_on_tag())
        RYML_ERR_BASIC_("JSON does not have tags");
    if C4_UNLIKELY((ty & ievt::ANCH) && m_opts.json_err_on_anchor())
        RYML_ERR_BASIC_("JSON does not have anchors");
    (void)id;
    (void)ty;
#ifdef OLD
    if C4_UNLIKELY(ty.has_val_tag() && m_opts.json_err_on_tag())
        RYML_ERR_BASIC_("JSON does not have tags");
    if C4_UNLIKELY(ty.has_val_anchor() && m_opts.json_err_on_anchor())
        RYML_ERR_BASIC_("JSON does not have anchors");
    csubstr val = m_tree->val(id);
    if(val.len)
    {
        // use double quoted style if the style is marked quoted
        bool dquoted = ((ty.m_bits & VALQUO)
                        || (scalar_style_choose_json(val).m_bits & SCALAR_DQUO)); // choose the style
        if(dquoted)
            json_write_scalar_dquo_(val);
        else if(json_maybe_write_naninf_(val))
            ;
        else if(val.is_number())
            json_write_number_(val);
        else
            write_(val);
    }
    else
    {
        if(val.str || (ty.m_bits & (VALQUO|VALTAG)))
            write_("\"\"");
        else
            write_("null");
    }
#endif
}


template<class Writer>
void EmitterInts<Writer>::json_write_scalar_dquo_(csubstr s)
{
    size_t pos = 0;
    write_('"');
    for(size_t i = 0; i < s.len; ++i)
    {
        switch(s.str[i])
        {
        case '"':
            write_(s.range(pos, i));
            write_("\\\"");
            pos = i + 1;
            break;
        case '\n':
            write_(s.range(pos, i));
            write_("\\n");
            pos = i + 1;
            break;
        case '\t':
            write_(s.range(pos, i));
            write_("\\t");
            pos = i + 1;
            break;
        case '\\':
            write_(s.range(pos, i));
            write_("\\\\");
            pos = i + 1;
            break;
        case '\r':
            write_(s.range(pos, i));
            write_("\\r");
            pos = i + 1;
            break;
        case '\b':
            write_(s.range(pos, i));
            write_("\\b");
            pos = i + 1;
            break;
        case '\f':
            write_(s.range(pos, i));
            write_("\\f");
            pos = i + 1;
            break;
        }
    }
    if(pos < s.len)
    {
        csubstr sub = s.sub(pos);
        write_(sub);
    }
    write_('"');
}

template<class Writer>
void EmitterInts<Writer>::json_write_number_(csubstr s)
{
    if(s.is_integer())
    {
        write_(s);
    }
    else
    {
        if(s.begins_with('-') && s.len > 1)
        {
            csubstr rest = s.sub(1);
            if(rest.begins_with('.'))
            {
                write_("-0");
                write_(rest);
            }
            else if(rest.ends_with('.'))
            {
                write_(s);
                write_('0');
            }
            else
            {
                write_(s);
            }
        }
        else if(s.begins_with('.'))
        {
            write_('0');
            write_(s);
        }
        else if(s.ends_with('.'))
        {
            write_(s);
            write_('0');
        }
        else
        {
            write_(s);
        }
    }
}

/** @endcond */

} // namespace ievt
} // namespace extra
} // namespace yml
} // namespace c4

// NOLINTEND(hicpp-signed-bitwise,*avoid-c-style-cast)
C4_SUPPRESS_WARNING_GCC_CLANG_POP

#endif /* C4_YML_EXTRA_EMITTER_INTS_DEF_HPP_ */
