#ifndef _C4_YML_EMITTER_HPP_
#define _C4_YML_EMITTER_HPP_

/** @file emitter.hpp */

#ifndef _C4_YML_EMIT_OPTIONS_HPP_
#include "c4/yml/emit_options.hpp"
#endif
#ifndef _C4_YML_NODE_TYPE_HPP_
#include "c4/yml/node_type.hpp"
#endif


namespace c4 {
namespace yml {

/** @cond dev */
// fwd declarations
class Tree;
/** Specifies the type of content to emit */
typedef enum { // NOLINT
    EMIT_YAML = 0, ///< emit YAML
    EMIT_JSON = 1, ///< emit JSON
} EmitType_e;
/** @endcond */


/** A YAML/JSON emitter, templated on a writer class such as @ref WriterBuf,
 * @ref WriterFile, or @ref WriterOStream
 * @ingroup doc_emit
 */
template<class Writer>
class Emitter : public Writer
{
public:

    /** Construct the emitter and its internal Writer state.
     *
     * @param opts @ref EmitOptions
     * @param args arguments to be forwarded to the constructor of the writer.
     */
    template<class ...WriterArgs>
    Emitter(EmitOptions const& opts, WriterArgs &&...args) noexcept
        : Writer(std::forward<WriterArgs>(args)...)
        , m_tree()
        , m_opts(opts)
        , m_col()
        , m_depth()
        , m_ilevel()
        , m_pws()
        , m_flow_pws()
    {}

    /** Construct the emitter and its internal Writer state, using default emit options.
     * @param args arguments to be forwarded to the constructor of the writer.
     */
    template<class ...WriterArgs>
    Emitter(WriterArgs &&...args) noexcept
        : Writer(std::forward<WriterArgs>(args)...)
        , m_tree()
        , m_opts()
        , m_col()
        , m_depth()
        , m_ilevel()
        , m_pws()
        , m_flow_pws()
    {}

public:

    /** emit!
     *
     * When writing to a buffer, returns a substr of the emitted YAML.
     * If the given buffer has insufficient space, the returned substr
     * will be null and its size will be the needed space. Whatever
     * the size of the buffer, it is guaranteed that no writes are
     * done past its end.
     *
     * When writing to a file, the returned substr will be null, but its
     * length will be set to the number of bytes written.
     *
     * @param type specify what to emit (YAML or JSON)
     * @param tree the tree to emit
     * @param id the id of the node to emit
     */
    void emit_as(EmitType_e type, Tree const* tree, id_type id=NONE);

public:

    /** get the emit options for this object */
    EmitOptions const& options() const noexcept { return m_opts; }
    /** set the emit options for this object */
    void options(EmitOptions opts) noexcept { m_opts = opts; }

    /** set the max depth for emitted trees (to prevent a stack overflow) */
    void max_depth(id_type max_depth) noexcept { m_opts.max_depth(max_depth); }
    /** get the max depth for emitted trees (to prevent a stack overflow) */
    id_type max_depth() const noexcept { return m_opts.max_depth(); }

private: // pending whitespace

    /// pending whitespace
    typedef enum : uint32_t { _PWS_NONE = 0u, _PWS_SPACE = 1u, _PWS_NEWL = 2u } Pws_e; // NOLINT

    /// set pending whitespace, ignoring pending
    C4_ALWAYS_INLINE void pend_none_() noexcept
    {
        m_pws = _PWS_NONE;
    }
    /// set pending whitespace, ignoring pending
    C4_ALWAYS_INLINE void pend_newl_() noexcept
    {
        m_pws = _PWS_NEWL;
    }
    /// set pending whitespace, ignoring pending
    C4_ALWAYS_INLINE void pend_space_() noexcept
    {
        m_pws = _PWS_SPACE;
    }
    /// write pending whitespace, and then set the next pending whitespace
    C4_ALWAYS_INLINE void write_pws_and_pend_(Pws_e next=_PWS_NONE) noexcept
    {
        if(m_pws == _PWS_SPACE)
        {
            write_(' ');
        }
        else if(m_pws == _PWS_NEWL)
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
        Pws_e pend_after_comma = _PWS_NONE;
        bool active = false;
        C4_ALWAYS_INLINE Pws_e next_pws(size_t col) const noexcept
        {
            return (active && col >= max_cols) ? _PWS_NEWL : pend_after_comma;
        }
        void start(NodeType ty, size_t max_cols_) noexcept;
        void stop() noexcept { active = false; }
    };

    C4_NODISCARD bool maybe_start_flow_pws_ml_(id_type node) noexcept;
    C4_NODISCARD flow_pws setup_flow_pws_sl_(id_type node) noexcept;

private:

    void emit_yaml_(id_type id);

    void visit_stream_(id_type id);
    void visit_doc_(id_type id);
    void visit_doc_val_(id_type id);
    void visit_blck_container_(id_type id);
    void visit_flow_container_(id_type id);

    void visit_flow_sl_(id_type id);
    void visit_flow_sl_seq_(id_type id);
    void visit_flow_sl_map_(id_type id);

    void visit_flow_ml_(id_type id);
    void visit_flow_ml_seq_(id_type id);
    void visit_flow_ml_map_(id_type id);

    void visit_blck_(id_type id);
    void visit_blck_seq_(id_type id);
    void visit_blck_map_(id_type id);

    void top_open_entry_(id_type id);
    void top_close_entry_(id_type id);
    void blck_seq_open_entry_(id_type id);
    void blck_map_open_entry_(id_type id);
    void blck_close_entry_(id_type id);
    void blck_write_scalar_(csubstr str, type_bits type);

    void flow_seq_open_entry_(id_type id);
    void flow_map_open_entry_(id_type id);
    void flow_close_entry_sl_(id_type id, id_type last_sibling, Pws_e pend_after);
    void flow_close_entry_ml_(id_type id, id_type last_sibling, Pws_e pend_after);
    void flow_write_scalar_(csubstr str, type_bits type);

private:

    void json_emit_(id_type id);
    void write_scalar_literal_(csubstr s, id_type level);
    void write_scalar_folded_(csubstr s, id_type level);
    void write_scalar_squo_(csubstr s, id_type level);
    void write_scalar_dquo_(csubstr s, id_type level);
    void write_scalar_plain_(csubstr s, id_type level);

    size_t write_escaped_newlines_(csubstr s, size_t i);
    size_t write_indented_block_(csubstr s, size_t i, id_type level);

private:

    void json_visit_ml_(id_type id, NodeType ty, id_type depth);
    void json_visit_sl_(id_type id, NodeType ty, id_type depth);
    bool json_maybe_write_naninf_(csubstr s);
    void json_writek_(id_type id, NodeType ty);
    void json_writev_(id_type id, NodeType ty);
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

    C4_ALWAYS_INLINE void indent_(id_type level)
    {
        C4_SUPPRESS_WARNING_GCC_WITH_PUSH("-Wuseless-cast")
        size_t num = static_cast<size_t>(2u * level);
        this->Writer::append(' ', num);
        m_col += num;
        C4_SUPPRESS_WARNING_GCC_POP
    }

    template<size_t N>
    C4_ALWAYS_INLINE void write_(const char (&a)[N])
    {
        this->Writer::append(std::forward<const char (&)[N]>(a));
        m_col += N-1;
    }
    C4_ALWAYS_INLINE void write_(csubstr s)
    {
        this->Writer::append(s);
        m_col += s.len;
    }
    C4_ALWAYS_INLINE void write_(char c)
    {
        this->Writer::append(c);
        ++m_col;
    }
    C4_ALWAYS_INLINE void write_(char c, size_t num)
    {
        this->Writer::append(c, num);
        m_col += num;
    }

    /// write a newline and reset the column
    C4_ALWAYS_INLINE void newl_()
    {
        this->Writer::append('\n');
        m_col = 0;
    }

private:

    Tree const* C4_RESTRICT m_tree;
    EmitOptions m_opts;
    size_t      m_col;
    id_type     m_depth;
    id_type     m_ilevel;
    Pws_e       m_pws;
    flow_pws    m_flow_pws;

};

} // namespace yml
} // namespace c4

#endif /* _C4_YML_EMITTTER_HPP_ */
