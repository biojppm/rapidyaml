#ifndef C4_YML_EMITTER_INTS_HPP_
#define C4_YML_EMITTER_INTS_HPP_

/** @file emitter_ints.hpp */

#ifndef C4_YML_EMIT_OPTIONS_HPP_
#include "c4/yml/emit_options.hpp"
#endif
#ifndef C4_YML_EXTRA_EVENT_INTS_HPP_
#include "c4/yml/extra/event_ints.hpp"
#endif


namespace c4 {
namespace yml {
namespace extra {

/** A YAML/JSON emitter, templated on a writer class such as @ref WriterBuf,
 * @ref WriterFile, or @ref WriterOStream
 * @ingroup doc_emit
 */
template<class Writer>
class EmitterInts : public Writer
{
public:

    /** Construct the emitter and its internal Writer state.
     *
     * @param opts @ref EmitOptions
     * @param args arguments to be forwarded to the constructor of the writer.
     */
    template<class ...WriterArgs>
    EmitterInts(EmitOptions const& opts, WriterArgs &&...args) noexcept
        : Writer(std::forward<WriterArgs>(args)...)
        , m_evts()
        , m_evts_size()
        , m_src()
        , m_arena()
        , m_opts(opts)
        , m_col()
        , m_depth()
        , m_ilevel()
        , m_pws()
        , m_flow_pws()
    {}

public:

    /** emit! */
    void emit_as(EmitType_e type,
                 evt_bits const* evts,
                 evt_size evts_size,
                 csubstr src,
                 csubstr arena);

public:

    /** get the emit options for this object */
    EmitOptions const& options() const noexcept { return m_opts; }

private: // pending whitespace

    /// pending whitespace
    typedef enum : uint32_t { PWS_NONE_ = 0u, PWS_SPACE_ = 1u, PWS_NEWL_ = 2u } Pws_e; // NOLINT

    /// set pending whitespace, ignoring pending
    C4_ALWAYS_INLINE void pend_none_() noexcept // LCOV_EXCL_LINE
    {
        m_pws = PWS_NONE_;
    }
    /// set pending whitespace, ignoring pending
    C4_ALWAYS_INLINE void pend_newl_() noexcept // LCOV_EXCL_LINE
    {
        m_pws = PWS_NEWL_;
    }
    /// set pending whitespace, ignoring pending
    C4_ALWAYS_INLINE void pend_space_() noexcept // LCOV_EXCL_LINE
    {
        m_pws = PWS_SPACE_;
    }
    /// write pending whitespace, and then set the next pending whitespace
    C4_ALWAYS_INLINE void write_pws_and_pend_(Pws_e next=PWS_NONE_) noexcept // LCOV_EXCL_LINE
    {
        if(m_pws == PWS_SPACE_)
        {
            write_(' ');
        }
        else if(m_pws == PWS_NEWL_)
        {
            newl_();
            indent_(m_ilevel);
        }
        m_pws = next;
    }

    /// specs for obtaining pending whitespace in flow mode
    struct flow_pws
    {
        size_t max_cols = 0; // leave this member first to avoid padding
        Pws_e pend_after_comma = PWS_NONE_;
        bool active = false;
        C4_ALWAYS_INLINE Pws_e next_pws(size_t col) const noexcept // LCOV_EXCL_LINE
        {
            return (active && col >= max_cols) ? PWS_NEWL_ : pend_after_comma;
        }
        void start(evt_bits ty, size_t max_cols_) noexcept;
        void stop() noexcept { active = false; }
    };

    C4_NODISCARD bool maybe_start_flow_pws_ml_(evt_size node) noexcept;
    C4_NODISCARD flow_pws setup_flow_pws_sl_(evt_size node) noexcept;

private:

    void emit_yaml_(evt_size id);

    void visit_stream_(evt_size &id);
    void visit_doc_(evt_size &id);
    void visit_doc_val_(evt_size &id);
    void visit_blck_container_(evt_size &id);
    void visit_flow_container_(evt_size &id);

    void visit_flow_sl_(evt_size &id);
    void visit_flow_sl_seq_(evt_size &id);
    void visit_flow_sl_map_(evt_size &id);

    void visit_flow_ml_(evt_size &id);
    void visit_flow_ml_seq_(evt_size &id);
    void visit_flow_ml_map_(evt_size &id);

    void visit_blck_(evt_size &id);
    void visit_blck_seq_(evt_size &id);
    void visit_blck_map_(evt_size &id);

    void top_open_entry_(evt_size &id);
    void top_close_entry_(evt_size &id);
    void blck_seq_open_entry_(evt_size &id);
    void blck_map_open_entry_(evt_size &id);
    void blck_close_entry_(evt_size &id);
    void blck_write_scalar_(csubstr str, evt_bits type);

    void flow_seq_open_entry_(evt_size &id);
    void flow_map_open_entry_(evt_size &id);
    void flow_close_entry_sl_(evt_size id, Pws_e pend_after);
    void flow_close_entry_ml_(evt_size id, Pws_e pend_after);
    void flow_write_scalar_(csubstr str, evt_bits type);

private:

    void json_emit_(evt_size id);
    void write_scalar_literal_(csubstr s, evt_size level);
    void write_scalar_folded_(csubstr s, evt_size level);
    void write_scalar_squo_(csubstr s, evt_size level);
    void write_scalar_dquo_(csubstr s, evt_size level);
    void write_scalar_plain_(csubstr s, evt_size level);

    size_t write_escaped_newlines_(csubstr s, size_t i);
    size_t write_indented_block_(csubstr s, size_t i, evt_size level);

private:

    void json_visit_ml_(evt_size &id, evt_bits ty, evt_size depth);
    void json_visit_sl_(evt_size &id, evt_bits ty, evt_size depth);
    bool json_maybe_write_naninf_(csubstr s);
    void json_writek_(evt_size &id, evt_bits ty);
    void json_writev_(evt_size &id, evt_bits ty);
    void json_write_scalar_dquo_(csubstr s);
    void json_write_number_(csubstr s);

private:

    void write_tag_(csubstr tag)
    {
        if(!tag.begins_with('!'))
            write_('!');
        write_(tag);
    }
    void write_ref_(csubstr ref)
    {
        if(ref != "<<")
        {
            if(!ref.begins_with('*'))
                write_('*');
            write_(ref);
        }
    }

private:

    template<size_t N>
    C4_ALWAYS_INLINE void write_(const char (&a)[N]) // LCOV_EXCL_LINE
    {
        this->Writer::append(std::forward<const char (&)[N]>(a));
        m_col += N-1;
    }
    C4_ALWAYS_INLINE void write_(csubstr s) // LCOV_EXCL_LINE
    {
        this->Writer::append(s);
        m_col += s.len;
    }
    C4_ALWAYS_INLINE void write_(char c) // LCOV_EXCL_LINE
    {
        this->Writer::append(c);
        ++m_col;
    }

    C4_ALWAYS_INLINE void indent_(evt_size level) // LCOV_EXCL_LINE
    {
        C4_SUPPRESS_WARNING_GCC_CLANG_WITH_PUSH("-Wsign-conversion")
        size_t num = static_cast<size_t>(2u * level);
        this->Writer::append(' ', num);
        m_col += num;
        C4_SUPPRESS_WARNING_GCC_CLANG_POP
    }

    /// write a newline and reset the column
    C4_ALWAYS_INLINE void newl_() // LCOV_EXCL_LINE
    {
        this->Writer::append('\n');
        m_col = 0;
    }

    C4_ALWAYS_INLINE csubstr getstr_(evt_size id) noexcept
    {
        RYML_ASSERT_BASIC_(id + 2 < m_evts_size);
        RYML_ASSERT_BASIC_(m_evts[id] & ievt::WSTR);
        csubstr region = (m_evts[id] & ievt::AREN) ? m_arena : m_src;
        region.str = region.str + m_evts[id + 1];
        region.len = static_cast<size_t>(m_evts[id + 2]);
        return region;
    }

private:

    evt_bits const* m_evts;
    evt_size    m_evts_size;
    csubstr     m_src;
    csubstr     m_arena;
    EmitOptions m_opts;
    size_t      m_col;
    evt_size    m_depth;
    evt_size    m_ilevel;
    Pws_e       m_pws;
    flow_pws    m_flow_pws;

};

} // namespace extra
} // namespace yml
} // namespace c4

#endif /* C4_YML_EMITTER_INTS_HPP_ */
