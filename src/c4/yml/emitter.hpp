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
    C4_ALWAYS_INLINE void _pend_none() noexcept
    {
        m_pws = _PWS_NONE;
    }
    /// set pending whitespace, ignoring pending
    C4_ALWAYS_INLINE void _pend_newl() noexcept
    {
        m_pws = _PWS_NEWL;
    }
    /// set pending whitespace, ignoring pending
    C4_ALWAYS_INLINE void _pend_space() noexcept
    {
        m_pws = _PWS_SPACE;
    }
    /// write pending whitespace, and then set the next pending whitespace
    C4_ALWAYS_INLINE void _write_pws_and_pend(Pws_e next=_PWS_NONE) noexcept
    {
        if(m_pws == _PWS_SPACE)
        {
            _write(' ');
        }
        else if(m_pws == _PWS_NEWL)
        {
            _newl();
            _indent(m_ilevel);
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

    C4_NODISCARD bool _maybe_start_flow_pws_ml(id_type node) noexcept;
    C4_NODISCARD flow_pws _setup_flow_pws_sl(id_type node) noexcept;

private:

    void _emit_yaml(id_type id);

    void _visit_stream(id_type id);
    void _visit_doc(id_type id);
    void _visit_doc_val(id_type id);
    void _visit_blck_container(id_type id);
    void _visit_flow_container(id_type id);

    void _visit_flow_sl(id_type id);
    void _visit_flow_sl_seq(id_type id);
    void _visit_flow_sl_map(id_type id);

    void _visit_flow_ml(id_type id);
    void _visit_flow_ml_seq(id_type id);
    void _visit_flow_ml_map(id_type id);

    void _visit_blck(id_type id);
    void _visit_blck_seq(id_type id);
    void _visit_blck_map(id_type id);

    void _top_open_entry(id_type id);
    void _top_close_entry(id_type id);
    void _blck_seq_open_entry(id_type id);
    void _blck_map_open_entry(id_type id);
    void _blck_close_entry(id_type id);
    void _blck_write_scalar(csubstr str, type_bits type);

    void _flow_seq_open_entry(id_type id);
    void _flow_map_open_entry(id_type id);
    void _flow_close_entry_sl(id_type id, id_type last_sibling, Pws_e pend_after);
    void _flow_close_entry_ml(id_type id, id_type last_sibling, Pws_e pend_after);
    void _flow_write_scalar(csubstr str, type_bits type);

private:

    void _json_emit(id_type id);
    void _write_scalar_literal(csubstr s, id_type level);
    void _write_scalar_folded(csubstr s, id_type level);
    void _write_scalar_squo(csubstr s, id_type level);
    void _write_scalar_dquo(csubstr s, id_type level);
    void _write_scalar_plain(csubstr s, id_type level);

    size_t _write_escaped_newlines(csubstr s, size_t i);
    size_t _write_indented_block(csubstr s, size_t i, id_type level);

private:

    void _json_visit_ml(id_type id, NodeType ty, id_type depth);
    void _json_visit_sl(id_type id, NodeType ty, id_type depth);
    bool _json_maybe_write_naninf(csubstr s);
    void _json_writek(id_type id, NodeType ty);
    void _json_writev(id_type id, NodeType ty);
    void _json_write_scalar_dquo(csubstr s);
    void _json_write_number(csubstr s);

private:

    void _write_tag(csubstr tag)
    {
        if(!tag.begins_with('!'))
            _write('!');
        _write(tag);
    }
    void _write_ref(csubstr ref)
    {
        if(ref != "<<")
        {
            if(!ref.begins_with('*'))
                _write('*');
            _write(ref);
        }
    }

private:

    C4_ALWAYS_INLINE void _indent(id_type level)
    {
        C4_SUPPRESS_WARNING_GCC_WITH_PUSH("-Wuseless-cast")
        size_t num = static_cast<size_t>(2u * level);
        this->Writer::_do_write(' ', num);
        m_col += num;
        C4_SUPPRESS_WARNING_GCC_POP
    }

    template<size_t N>
    C4_ALWAYS_INLINE void _write(const char (&a)[N])
    {
        this->Writer::_do_write(std::forward<const char (&)[N]>(a));
        m_col += N-1;
    }
    C4_ALWAYS_INLINE void _write(csubstr s)
    {
        this->Writer::_do_write(s);
        m_col += s.len;
    }
    C4_ALWAYS_INLINE void _write(char c)
    {
        this->Writer::_do_write(c);
        ++m_col;
    }
    C4_ALWAYS_INLINE void _write(char c, size_t num)
    {
        this->Writer::_do_write(c, num);
        m_col += num;
    }

    /// write a newline and reset the column
    C4_ALWAYS_INLINE void _newl()
    {
        this->Writer::_do_write('\n');
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
