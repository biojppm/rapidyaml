#ifndef _C4_YML_PARSE_ENGINE_HPP_
#define _C4_YML_PARSE_ENGINE_HPP_

#ifndef _C4_YML_DETAIL_PARSER_DBG_HPP_
#include "c4/yml/detail/parser_dbg.hpp"
#endif

#ifndef _C4_YML_PARSER_STATE_HPP_
#include "c4/yml/parser_state.hpp"
#endif


#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable: 4251/*needs to have dll-interface to be used by clients of struct*/)
#endif

namespace c4 {
namespace yml {

class Tree;
class NodeRef;
class ConstNodeRef;

struct RYML_EXPORT ParserOptions
{
private:

    typedef enum : uint32_t {
        SCALAR_FILTERING = (1u << 0),
        LOCATIONS = (1u << 1),
        DEFAULTS = SCALAR_FILTERING,
    } Flags_e;

    uint32_t flags = DEFAULTS;

public:

    ParserOptions() = default;

public:

    /** @name source location tracking */
    /** @{ */

    /** enable/disable source location tracking */
    ParserOptions& locations(bool enabled) noexcept
    {
        if(enabled)
            flags |= LOCATIONS;
        else
            flags &= ~LOCATIONS;
        return *this;
    }
    /** query source location tracking status */
    C4_ALWAYS_INLINE bool locations() const noexcept { return (flags & LOCATIONS); }

    /** @} */

public:

    /** @name scalar filtering status (experimental; disable at your discretion) */
    /** @{ */

    /** enable/disable scalar filtering while parsing */
    ParserOptions& scalar_filtering(bool enabled) noexcept
    {
        if(enabled)
            flags |= SCALAR_FILTERING;
        else
            flags &= ~SCALAR_FILTERING;
        return *this;
    }
    /** query scalar filtering status */
    C4_ALWAYS_INLINE bool scalar_filtering() const noexcept { return (flags & SCALAR_FILTERING); }

    /** @} */
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

template<class EventHandler>
class ParseEngine
{
public:

    using handler_type = EventHandler;

public:

    /** @name construction and assignment */
    /** @{ */

    ParseEngine(EventHandler *evt_handler, ParserOptions opts={});
    ~ParseEngine();

    ParseEngine(ParseEngine &&);
    ParseEngine(ParseEngine const&);
    ParseEngine& operator=(ParseEngine &&);
    ParseEngine& operator=(ParseEngine const&);

    /** @} */

public:

    /** @name modifiers */
    /** @{ */

    /** Reserve a certain capacity for the parsing stack.
     * This should be larger than the expected depth of the parsed
     * YAML tree.
     *
     * The parsing stack is the only (potential) heap memory used
     * directly by the parser.
     *
     * If the requested capacity is below the default
     * stack size of 16, the memory is used directly in the parser
     * object; otherwise it will be allocated from the heap.
     *
     * @note this reserves memory only for the parser itself; all the
     * allocations for the parsed tree will go through the tree's
     * allocator (when different).
     *
     * @note for maximum efficiency, the tree and the arena can (and
     * should) also be reserved. */
    void reserve_stack(size_t capacity)
    {
        m_evt_handler->m_stack.reserve(capacity);
    }

    /** Reserve a certain capacity for the array used to track node
     * locations in the source buffer. */
    void reserve_locations(size_t num_source_lines)
    {
        _resize_locations(num_source_lines);
    }

    RYML_DEPRECATED("filter arena no longer needed")
    void reserve_filter_arena(size_t) {}

    /** @} */

public:

    /** @name getters and modifiers */
    /** @{ */

    /** Get the options used to build this parser object. */
    ParserOptions const& options() const { return m_options; }

    /** Get the current callbacks in the parser. */
    Callbacks const& callbacks() const { return m_evt_handler->m_stack.m_callbacks; }

    /** Get the name of the latest file parsed by this object. */
    csubstr filename() const { return m_file; }

    /** Get the latest YAML buffer parsed by this object. */
    csubstr source() const { return m_buf; }

    size_t stack_capacity() const { return m_evt_handler->m_stack.capacity(); }
    size_t locations_capacity() const { return m_newline_offsets_capacity; }

    RYML_DEPRECATED("filter arena no longer needed")
    size_t filter_arena_capacity() const { return 0u; }

    /** @} */

public:

    /** @name parse methods */
    /** @{ */

    void parse_in_place_ev(csubstr filename, substr src);
    void parse_json_in_place_ev(csubstr filename, substr src);

    /** @} */

public:

    /** @name deprecated parse_methods
     * @{ */

    template<class U=EventHandler> RYML_DEPRECATED("deliberately undefined. use the freestanding function in parse.hpp.") typename std::enable_if<U::is_wtree, void>::type parse_in_place(csubstr filename, substr yaml, Tree *t, size_t node_id);
    template<class U=EventHandler> RYML_DEPRECATED("deliberately undefined. use the freestanding function in parse.hpp.") typename std::enable_if<U::is_wtree, void>::type parse_in_place(                  substr yaml, Tree *t, size_t node_id);
    template<class U=EventHandler> RYML_DEPRECATED("deliberately undefined. use the freestanding function in parse.hpp.") typename std::enable_if<U::is_wtree, void>::type parse_in_place(csubstr filename, substr yaml, Tree *t                );
    template<class U=EventHandler> RYML_DEPRECATED("deliberately undefined. use the freestanding function in parse.hpp.") typename std::enable_if<U::is_wtree, void>::type parse_in_place(                  substr yaml, Tree *t                );
    template<class U=EventHandler> RYML_DEPRECATED("deliberately undefined. use the freestanding function in parse.hpp.") typename std::enable_if<U::is_wtree, void>::type parse_in_place(csubstr filename, substr yaml, NodeRef node           );
    template<class U=EventHandler> RYML_DEPRECATED("deliberately undefined. use the freestanding function in parse.hpp.") typename std::enable_if<U::is_wtree, void>::type parse_in_place(                  substr yaml, NodeRef node           );
    template<class U=EventHandler> RYML_DEPRECATED("deliberately undefined. use the freestanding function in parse.hpp.") typename std::enable_if<U::is_wtree, Tree>::type parse_in_place(csubstr filename, substr yaml                         );
    template<class U=EventHandler> RYML_DEPRECATED("deliberately undefined. use the freestanding function in parse.hpp.") typename std::enable_if<U::is_wtree, Tree>::type parse_in_place(                  substr yaml                         );
    template<class U=EventHandler> RYML_DEPRECATED("deliberately undefined. use the freestanding function in parse.hpp.") typename std::enable_if<U::is_wtree, void>::type parse_in_arena(csubstr filename, csubstr yaml, Tree *t, size_t node_id);
    template<class U=EventHandler> RYML_DEPRECATED("deliberately undefined. use the freestanding function in parse.hpp.") typename std::enable_if<U::is_wtree, void>::type parse_in_arena(                  csubstr yaml, Tree *t, size_t node_id);
    template<class U=EventHandler> RYML_DEPRECATED("deliberately undefined. use the freestanding function in parse.hpp.") typename std::enable_if<U::is_wtree, void>::type parse_in_arena(csubstr filename, csubstr yaml, Tree *t                );
    template<class U=EventHandler> RYML_DEPRECATED("deliberately undefined. use the freestanding function in parse.hpp.") typename std::enable_if<U::is_wtree, void>::type parse_in_arena(                  csubstr yaml, Tree *t                );
    template<class U=EventHandler> RYML_DEPRECATED("deliberately undefined. use the freestanding function in parse.hpp.") typename std::enable_if<U::is_wtree, void>::type parse_in_arena(csubstr filename, csubstr yaml, NodeRef node           );
    template<class U=EventHandler> RYML_DEPRECATED("deliberately undefined. use the freestanding function in parse.hpp.") typename std::enable_if<U::is_wtree, void>::type parse_in_arena(                  csubstr yaml, NodeRef node           );
    template<class U=EventHandler> RYML_DEPRECATED("deliberately undefined. use the freestanding function in parse.hpp.") typename std::enable_if<U::is_wtree, Tree>::type parse_in_arena(csubstr filename, csubstr yaml                         );
    template<class U=EventHandler> RYML_DEPRECATED("deliberately undefined. use the freestanding function in parse.hpp.") typename std::enable_if<U::is_wtree, Tree>::type parse_in_arena(                  csubstr yaml                         );
    template<class U=EventHandler> RYML_DEPRECATED("deliberately undefined. use the freestanding csubstr version in parse.hpp.") typename std::enable_if<U::is_wtree, void>::type parse_in_arena(csubstr filename, substr yaml, Tree *t, size_t node_id);
    template<class U=EventHandler> RYML_DEPRECATED("deliberately undefined. use the freestanding csubstr version in parse.hpp.") typename std::enable_if<U::is_wtree, void>::type parse_in_arena(                  substr yaml, Tree *t, size_t node_id);
    template<class U=EventHandler> RYML_DEPRECATED("deliberately undefined. use the freestanding csubstr version in parse.hpp.") typename std::enable_if<U::is_wtree, void>::type parse_in_arena(csubstr filename, substr yaml, Tree *t                );
    template<class U=EventHandler> RYML_DEPRECATED("deliberately undefined. use the freestanding csubstr version in parse.hpp.") typename std::enable_if<U::is_wtree, void>::type parse_in_arena(                  substr yaml, Tree *t                );
    template<class U=EventHandler> RYML_DEPRECATED("deliberately undefined. use the freestanding csubstr version in parse.hpp.") typename std::enable_if<U::is_wtree, void>::type parse_in_arena(csubstr filename, substr yaml, NodeRef node           );
    template<class U=EventHandler> RYML_DEPRECATED("deliberately undefined. use the freestanding csubstr version in parse.hpp.") typename std::enable_if<U::is_wtree, void>::type parse_in_arena(                  substr yaml, NodeRef node           );
    template<class U=EventHandler> RYML_DEPRECATED("deliberately undefined. use the freestanding csubstr version in parse.hpp.") typename std::enable_if<U::is_wtree, Tree>::type parse_in_arena(csubstr filename, substr yaml                         );
    template<class U=EventHandler> RYML_DEPRECATED("deliberately undefined. use the freestanding csubstr version in parse.hpp.") typename std::enable_if<U::is_wtree, Tree>::type parse_in_arena(                  substr yaml                         );

    /** @} */

public:

    /** @name locations */
    /** @{ */

    /** Get the location of a node of the last tree to be parsed by this parser. */
    Location location(Tree const& tree, size_t node_id) const;
    /** Get the location of a node of the last tree to be parsed by this parser. */
    Location location(ConstNodeRef node) const;
    /** Get the string starting at a particular location, to the end
     * of the parsed source buffer. */
    csubstr location_contents(Location const& loc) const;
    /** Given a pointer to a buffer position, get the location.
     * @param[in] val must be pointing to somewhere in the source
     * buffer that was last parsed by this object. */
    Location val_location(const char *val) const;

    /** @} */

public:

    /** @name scalar filtering */
    /** @{*/

    /** filter a plain scalar */
    FilterResult filter_scalar_plain(csubstr scalar, substr dst, size_t indentation) noexcept;
    /** filter a plain scalar in place */
    FilterResult filter_scalar_plain_in_place(substr scalar, size_t cap, size_t indentation) noexcept;

    /** filter a single-quoted scalar */
    FilterResult filter_scalar_squoted(csubstr scalar, substr dst) noexcept;
    /** filter a single-quoted scalar in place */
    FilterResult filter_scalar_squoted_in_place(substr scalar, size_t cap) noexcept;

    /** filter a double-quoted scalar */
    FilterResult filter_scalar_dquoted(csubstr scalar, substr dst);
    /** filter a double-quoted scalar in place */
    FilterResultExtending filter_scalar_dquoted_in_place(substr scalar, size_t cap);

    /** filter a block-literal scalar */
    FilterResult filter_scalar_block_literal(csubstr scalar, substr dst, size_t indentation, BlockChomp_e chomp) noexcept;
    /** filter a block-literal scalar in place */
    FilterResult filter_scalar_block_literal_in_place(substr scalar, size_t cap, size_t indentation, BlockChomp_e chomp) noexcept;

    /** filter a block-folded scalar */
    FilterResult filter_scalar_block_folded(csubstr scalar, substr dst, size_t indentation, BlockChomp_e chomp) noexcept;
    /** filter a block-folded scalar in place */
    FilterResult filter_scalar_block_folded_in_place(substr scalar, size_t cap, size_t indentation, BlockChomp_e chomp) noexcept;

    /** @} */

private:

    static size_t _estimate_capacity(csubstr src) { size_t c = _count_nlines(src); c = c >= 16 ? c : 16; return c; }

    void  _reset();

    bool  _finished_file() const;
    bool  _finished_line() const;

    substr _peek_next_line(size_t pos=npos) const;
    void    _scan_line();

    struct ScannedScalar
    {
        substr scalar;
        bool needs_filter;
    };

    struct ScannedBlock
    {
        substr scalar;
        size_t indentation;
        BlockChomp_e chomp;
    };

    bool    _is_valid_start_scalar_plain_flow(csubstr s);
    bool    _is_doc_begin(csubstr s);
    bool    _is_doc_end(csubstr s);
    bool    _scan2_scalar_plain_blck(ScannedScalar *C4_RESTRICT sc, size_t indentation);
    bool    _scan2_scalar_plain_seq_flow(ScannedScalar *C4_RESTRICT sc);
    bool    _scan2_scalar_plain_seq_blck(ScannedScalar *C4_RESTRICT sc);
    bool    _scan2_scalar_plain_map_flow(ScannedScalar *C4_RESTRICT sc);
    bool    _scan2_scalar_plain_map_blck(ScannedScalar *C4_RESTRICT sc);
    bool    _scan2_scalar_map_json(ScannedScalar *C4_RESTRICT sc);
    bool    _scan2_scalar_seq_json(ScannedScalar *C4_RESTRICT sc);
    bool    _scan2_scalar_plain_unk(ScannedScalar *C4_RESTRICT sc);

    void    _maybe_skip_comment();

    ScannedScalar _scan2_scalar_squot();
    ScannedScalar _scan2_scalar_dquot();

    void    _scan2_block(ScannedBlock *C4_RESTRICT sb, size_t indref);

public: // exposed for testing. to be cleared.

    csubstr _filter_scalar_plain(substr s, size_t indentation);
    csubstr _filter_scalar_squot(substr s);
    csubstr _filter_scalar_dquot(substr s);
    csubstr _filter_scalar_literal(substr s, size_t indentation, BlockChomp_e chomp);
    csubstr _filter_scalar_folded(substr s, size_t indentation, BlockChomp_e chomp);

    csubstr _maybe_filter_key_scalar_plain(ScannedScalar const& sc, size_t indendation);
    csubstr _maybe_filter_val_scalar_plain(ScannedScalar const& sc, size_t indendation);
    csubstr _maybe_filter_key_scalar_squot(ScannedScalar const& sc);
    csubstr _maybe_filter_val_scalar_squot(ScannedScalar const& sc);
    csubstr _maybe_filter_key_scalar_dquot(ScannedScalar const& sc);
    csubstr _maybe_filter_val_scalar_dquot(ScannedScalar const& sc);
    csubstr _maybe_filter_key_scalar_literal(ScannedBlock const& sb);
    csubstr _maybe_filter_val_scalar_literal(ScannedBlock const& sb);
    csubstr _maybe_filter_key_scalar_folded(ScannedBlock const& sb);
    csubstr _maybe_filter_val_scalar_folded(ScannedBlock const& sb);

private:

    void  _handle_finished_file();

    bool  _handle2_map_block();
    bool  _handle2_seq_block();
    bool  _handle2_map_flow();
    bool  _handle2_seq_flow();
    bool  _handle2_seq_imap();
    bool  _handle2_map_json();
    bool  _handle2_seq_json();

    bool  _handle_unk();
    bool  _handle_unk_json();
    bool  _handle_usty();

    void  _handle2_flow_skip_whitespace();

    csubstr _scan2_tag();
    void  _end2_map_flow()
    {
        _c4dbgp("mapflow: end");
        m_evt_handler->end_map();
    }
    void  _end2_seq_flow()
    {
        _c4dbgp("seqflow: end");
        if(m_evt_handler->has_val_anchor() || m_evt_handler->has_val_tag())
        {
            _c4dbgp("seqflow: set missing val");
            m_evt_handler->set_val_scalar_plain({});
        }
        m_evt_handler->end_seq();
    }

    void  _end2_map_blck()
    {
        _c4dbgp("mapblck: end");
        if(has_any(RKCL|RVAL))
        {
            _c4dbgp("mapblck: set missing val");
            _handle_annotations_before_blck_val_scalar();
            m_evt_handler->set_val_scalar_plain({});
        }
        else if(has_any(QMRK))
        {
            _c4dbgp("mapblck: set missing keyval");
            _handle_annotations_before_blck_key_scalar();
            m_evt_handler->set_key_scalar_plain({});
            _handle_annotations_before_blck_val_scalar();
            m_evt_handler->set_val_scalar_plain({});
        }
        m_evt_handler->end_map();
    }
    void  _end2_seq_blck()
    {
        if(has_any(RVAL))
        {
            _c4dbgp("seqblck: set missing val");
            _handle_annotations_before_blck_val_scalar();
            m_evt_handler->set_val_scalar_plain({});
        }
        m_evt_handler->end_seq();
    }

    void _end2_map()
    {
        _c4dbgp("map: end");
        _RYML_CB_ASSERT(m_evt_handler->m_stack.m_callbacks, has_any(RMAP));
        if(has_any(FLOW))
        {
            _end2_map_flow();
        }
        else if(has_any(BLCK))
        {
            _end2_map_blck();
        }
        else
        {
            _RYML_CB_ASSERT(m_evt_handler->m_stack.m_callbacks, has_any(USTY));
            m_evt_handler->_pop();
        }
    }
    void _end2_seq()
    {
        _c4dbgp("seq: end");
        _RYML_CB_ASSERT(m_evt_handler->m_stack.m_callbacks, has_any(RSEQ));
        if(has_any(FLOW))
        {
            _end2_seq_flow();
        }
        else if(has_any(BLCK))
        {
            _end2_seq_blck();
        }
        else
        {
            _RYML_CB_ASSERT(m_evt_handler->m_stack.m_callbacks, has_any(USTY));
            m_evt_handler->_pop();
        }
    }
    bool m_2doc_empty = true;
    void  _begin2_doc()
    {
        m_2doc_empty = true;
        add_flags(RDOC);
        m_evt_handler->begin_doc();
        m_evt_handler->m_curr->indref = 0; // ?
    }
    void  _begin2_doc_expl()
    {
        m_2doc_empty = true;
        add_flags(RDOC);
        m_evt_handler->begin_doc_expl();
        m_evt_handler->m_curr->indref = 0; // ?
    }
    void  _end2_doc()
    {
        _c4dbgp("doc: end");
        _RYML_CB_ASSERT(m_evt_handler->m_stack.m_callbacks, has_any(RDOC));
        if(m_2doc_empty)
        {
            _c4dbgp("doc was empty; add empty val");
            m_evt_handler->set_val_scalar_plain({});
        }
        m_evt_handler->end_doc();
    }
    void  _end2_doc_expl()
    {
        _c4dbgp("doc: end");
        //_RYML_CB_ASSERT(m_evt_handler->m_stack.m_callbacks, has_any(RDOC));
        if(m_2doc_empty)
        {
            _c4dbgp("doc: no children; add empty val");
            m_evt_handler->set_val_scalar_plain({});
        }
        m_evt_handler->end_doc_expl();
    }

    void  _maybe_begin_doc()
    {
        if(has_none(RDOC))
        {
            _c4dbgp("doc must be started");
            _begin2_doc();
        }
    }
    void  _maybe_end_doc()
    {
        if(has_any(RDOC))
        {
            _c4dbgp("doc must be finished");
            _end2_doc();
        }
    }

    void  _start_doc_suddenly();
    void  _end_doc_suddenly();
    void  _end_doc_suddenly__pop();
    void  _end_stream();

    void  _set_indentation(size_t behind);
    void  _save_indentation(size_t behind=0);

    void _handle_directive(csubstr directive);

    void _skipchars(char c);
    void _maybe_skipchars(char c);
    void _maybe_skipchars_up_to(char c, size_t max_to_skip);
    template<size_t N>
    void _skipchars(const char (&chars)[N]);
    bool _maybe_scan_following_colon() noexcept;

private:

    static size_t _count_nlines(csubstr src);

public:

    template<class FilterProcessor> auto _filter_plain(FilterProcessor &C4_RESTRICT proc, size_t indentation) noexcept -> decltype(proc.result());
    template<class FilterProcessor> auto _filter_squoted(FilterProcessor &C4_RESTRICT proc) noexcept -> decltype(proc.result());
    template<class FilterProcessor> auto _filter_dquoted(FilterProcessor &C4_RESTRICT proc) -> decltype(proc.result());
    template<class FilterProcessor> auto _filter_block_literal(FilterProcessor &C4_RESTRICT proc, size_t indentation, BlockChomp_e chomp) noexcept -> decltype(proc.result());
    template<class FilterProcessor> auto _filter_block_folded(FilterProcessor &C4_RESTRICT proc, size_t indentation, BlockChomp_e chomp) noexcept -> decltype(proc.result());

public:

    template<class FilterProcessor> void   _filter_nl_plain(FilterProcessor &C4_RESTRICT proc, size_t indentation) noexcept;
    template<class FilterProcessor> void   _filter_nl_squoted(FilterProcessor &C4_RESTRICT proc) noexcept;
    template<class FilterProcessor> void   _filter_nl_dquoted(FilterProcessor &C4_RESTRICT proc) noexcept;

    template<class FilterProcessor> bool   _filter_ws_handle_to_first_non_space(FilterProcessor &C4_RESTRICT proc) noexcept;
    template<class FilterProcessor> void   _filter_ws_copy_trailing(FilterProcessor &C4_RESTRICT proc) noexcept;
    template<class FilterProcessor> void   _filter_ws_skip_trailing(FilterProcessor &C4_RESTRICT proc) noexcept;

    template<class FilterProcessor> void   _filter_dquoted_backslash(FilterProcessor &C4_RESTRICT proc);

    template<class FilterProcessor> void   _filter_chomp(FilterProcessor &C4_RESTRICT proc, BlockChomp_e chomp, size_t indentation) noexcept;
    template<class FilterProcessor> size_t _handle_all_whitespace(FilterProcessor &C4_RESTRICT proc, BlockChomp_e chomp) noexcept;
    template<class FilterProcessor> size_t _extend_to_chomp(FilterProcessor &C4_RESTRICT proc, size_t contents_len) noexcept;
    template<class FilterProcessor> void   _filter_block_indentation(FilterProcessor &C4_RESTRICT proc, size_t indentation) noexcept;
    template<class FilterProcessor> void   _filter_block_folded_newlines(FilterProcessor &C4_RESTRICT proc, size_t indentation, size_t len) noexcept;
    template<class FilterProcessor> void   _filter_block_folded_newlines_leading(FilterProcessor &C4_RESTRICT proc, size_t indentation, size_t len) noexcept;
    template<class FilterProcessor> void   _filter_block_folded_indented_block(FilterProcessor &C4_RESTRICT proc, size_t indentation, size_t len, size_t curr_indentation) noexcept;

private:

public://FIXME scaffold for sink refactor
    using State = typename EventHandler::state;
    EventHandler *C4_RESTRICT m_evt_handler;
    bool m_was_inside_qmrk;
private://FIXME

    void _line_progressed(size_t ahead);
    void _line_ended();
    void _line_ended_undo();

    void  _handle2_indentation_pop_from_block_seq();
    void  _handle2_indentation_pop_from_block_map();
    void  _handle2_indentation_pop(ParserState const* dst);

    void _prepare_pop()
    {
        RYML_ASSERT(m_evt_handler->m_stack.size() > 1);
        auto const& curr = m_evt_handler->m_stack.top();
        auto      & next = m_evt_handler->m_stack.top(1);
        next.pos = curr.pos;
        next.line_contents = curr.line_contents;
        next.scalar = curr.scalar;
    }

    inline bool _at_line_begin() const
    {
        return m_evt_handler->m_curr->line_contents.rem.begin() == m_evt_handler->m_curr->line_contents.full.begin();
    }
    inline bool _at_line_end() const
    {
        csubstr r = m_evt_handler->m_curr->line_contents.rem;
        return r.empty() || r.begins_with(' ', r.len);
    }

    C4_ALWAYS_INLINE bool has_all(ParserFlag_t f) const noexcept { return (m_evt_handler->m_curr->flags & f) == f; }
    C4_ALWAYS_INLINE bool has_any(ParserFlag_t f) const noexcept { return (m_evt_handler->m_curr->flags & f) != 0; }
    C4_ALWAYS_INLINE bool has_none(ParserFlag_t f) const noexcept { return (m_evt_handler->m_curr->flags & f) == 0; }
    static C4_ALWAYS_INLINE bool has_all(ParserFlag_t f, ParserState const* C4_RESTRICT s) noexcept { return (s->flags & f) == f; }
    static C4_ALWAYS_INLINE bool has_any(ParserFlag_t f, ParserState const* C4_RESTRICT s) noexcept { return (s->flags & f) != 0; }
    static C4_ALWAYS_INLINE bool has_none(ParserFlag_t f, ParserState const* C4_RESTRICT s) noexcept { return (s->flags & f) == 0; }

    #ifdef RYML_DBG
    static void set_flags(ParserFlag_t f, ParserState *C4_RESTRICT s);
    static void add_flags(ParserFlag_t on, ParserState *C4_RESTRICT s);
    static void addrem_flags(ParserFlag_t on, ParserFlag_t off, ParserState *C4_RESTRICT s);
    static void rem_flags(ParserFlag_t off, ParserState *C4_RESTRICT s);
    C4_ALWAYS_INLINE void set_flags(ParserFlag_t f) noexcept { set_flags(f, m_evt_handler->m_curr); }
    C4_ALWAYS_INLINE void add_flags(ParserFlag_t on) noexcept { add_flags(on, m_evt_handler->m_curr); }
    C4_ALWAYS_INLINE void addrem_flags(ParserFlag_t on, ParserFlag_t off) noexcept { addrem_flags(on, off, m_evt_handler->m_curr); }
    C4_ALWAYS_INLINE void rem_flags(ParserFlag_t off) noexcept { rem_flags(off, m_evt_handler->m_curr); }
    #else
    C4_ALWAYS_INLINE static void set_flags(ParserFlag_t f, ParserState *C4_RESTRICT s) noexcept { s->flags = f; }
    C4_ALWAYS_INLINE static void add_flags(ParserFlag_t on, ParserState *C4_RESTRICT s) noexcept { s->flags |= on; }
    C4_ALWAYS_INLINE static void addrem_flags(ParserFlag_t on, ParserFlag_t off, ParserState *C4_RESTRICT s) noexcept { s->flags &= ~off; s->flags |= on; }
    C4_ALWAYS_INLINE static void rem_flags(ParserFlag_t off, ParserState *C4_RESTRICT s) noexcept { s->flags &= ~off; }
    C4_ALWAYS_INLINE void set_flags(ParserFlag_t f) noexcept { m_evt_handler->m_curr->flags = f; }
    C4_ALWAYS_INLINE void add_flags(ParserFlag_t on) noexcept { m_evt_handler->m_curr->flags |= on; }
    C4_ALWAYS_INLINE void addrem_flags(ParserFlag_t on, ParserFlag_t off) noexcept { m_evt_handler->m_curr->flags &= ~off; m_evt_handler->m_curr->flags |= on; }
    C4_ALWAYS_INLINE void rem_flags(ParserFlag_t off) noexcept { m_evt_handler->m_curr->flags &= ~off; }
    #endif

    csubstr _scan2_anchor()
    {
        csubstr s = m_evt_handler->m_curr->line_contents.rem;
        _RYML_CB_ASSERT(m_evt_handler->m_stack.m_callbacks, s.begins_with('&'));
        csubstr anchor = s.range(1, s.first_of(' '));
        _line_progressed(1u + anchor.len);
        _maybe_skipchars(' ');
        return anchor;
    }
    csubstr _scan2_ref_seq()
    {
        csubstr s = m_evt_handler->m_curr->line_contents.rem;
        _RYML_CB_ASSERT(m_evt_handler->m_stack.m_callbacks, s.begins_with('*'));
        csubstr ref = s.first(s.first_of(",] :"));
        _line_progressed(ref.len);
        return ref;
    }
    csubstr _scan2_ref_map()
    {
        csubstr s = m_evt_handler->m_curr->line_contents.rem;
        _RYML_CB_ASSERT(m_evt_handler->m_stack.m_callbacks, s.begins_with('*'));
        csubstr ref = s.first(s.first_of(",} "));
        _line_progressed(ref.len);
        return ref;
    }

private:

    void _prepare_locations();
    void _resize_locations(size_t sz);
    bool _locations_dirty() const;

    bool _location_from_cont(Tree const& tree, size_t node, Location *C4_RESTRICT loc) const;
    bool _location_from_node(Tree const& tree, size_t node, Location *C4_RESTRICT loc, size_t level) const;

private:

    void _free();
    void _clr();

    #ifdef RYML_DBG
    template<class ...Args> void _dbg(csubstr fmt, Args const& C4_RESTRICT ...args) const;
    #endif
    template<class ...Args> void _err(csubstr fmt, Args const& C4_RESTRICT ...args) const;
    template<class ...Args> void _errloc(csubstr fmt, Location const& loc, Args const& C4_RESTRICT ...args) const;

    template<class DumpFn>  void _fmt_msg(DumpFn &&dumpfn) const;

private:

    ParserOptions m_options;

    csubstr m_file;
     substr m_buf;

private:

    /** store pending tag or anchor/ref annotations */
    struct ValAnnotation
    {
        struct Annotation
        {
            csubstr str;
            size_t indentation;
            size_t line;
        };
        Annotation annotations[2];
        size_t num_entries;
    };

    ValAnnotation m_pending_anchors;
    ValAnnotation m_pending_tags;

    void _add_annotation(ValAnnotation *C4_RESTRICT dst, csubstr str, size_t indentation, size_t line);
    void _clear_annotations(ValAnnotation *C4_RESTRICT dst);
    bool _handle_indentation_from_annotations();
    bool _annotations_require_key_container() const;
    void _handle_annotations_before_blck_key_scalar();
    void _handle_annotations_before_blck_val_scalar();
    void _handle_annotations_before_start_mapblck(size_t current_line);
    void _handle_annotations_before_start_mapblck_as_key();
    void _handle_annotations_and_indentation_after_start_mapblck(size_t key_indentation, size_t key_line);
    size_t _select_indentation_from_annotations(size_t val_indentation, size_t val_line);

private:

    size_t *m_newline_offsets;
    size_t  m_newline_offsets_size;
    size_t  m_newline_offsets_capacity;
    csubstr m_newline_offsets_buf;

};

RYML_EXPORT C4_NO_INLINE size_t _find_last_newline_and_larger_indentation(csubstr s, size_t indentation) noexcept;

} // namespace yml
} // namespace c4

#if defined(_MSC_VER)
#   pragma warning(pop)
#endif

#endif /* _C4_YML_PARSE_ENGINE_HPP_ */
