#ifndef _C4_YML_EVENT_HANDLER_YAMLSTD_HPP_
#define _C4_YML_EVENT_HANDLER_YAMLSTD_HPP_

#ifdef RYML_SINGLE_HEADER
#include <ryml_all.hpp>
#else
#ifndef _C4_YML_EVENT_HANDLER_STACK_HPP_
#include "c4/yml/event_handler_stack.hpp"
#endif
#ifndef _C4_YML_DETAIL_PRINT_HPP_
#include "c4/yml/detail/print.hpp"
#endif
#endif

#ifndef _C4_YML_EXTRA_STRING_HPP_
#include "./string.hpp"
#endif

C4_SUPPRESS_WARNING_GCC_CLANG_PUSH
C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")
C4_SUPPRESS_WARNING_GCC("-Wuseless-cast")

namespace c4 {
namespace yml {


/** @addtogroup doc_event_handlers
 * @{ */

void append_escaped(extra::string *s, csubstr val);


/** The stack state needed specifically by @ref EventHandlerYamlStd */
struct EventHandlerYamlStdState : public ParserState
{
    NodeData ev_data;
};

/** The event handler producing standard YAML events as used in the
 * [YAML test suite](https://github.com/yaml/yaml-test-suite).
 * See the documentation for @ref doc_event_handlers, which has
 * important notes about the event model used by rapidyaml.
 *
 * This class is used only in the CI of this project, and in the
 * application used as part of the [standard YAML
 * playground](https://play.yaml.io/main/parser). It is not part of
 * the library and is not installed. *
 */
struct EventHandlerYamlStd : public EventHandlerStack<EventHandlerYamlStd, EventHandlerYamlStdState>
{

    /** @name types
     * @{ */

    // our internal state must inherit from parser state
    using state = EventHandlerYamlStdState;

    using EventSink = extra::string;

    /** @} */

public:

    /** @cond dev */
    EventSink *C4_RESTRICT m_sink;
    extra::string_vector m_val_buffers;
    extra::string m_key_tag_buf;
    extra::string m_val_tag_buf;
    TagDirective m_tag_directives[RYML_MAX_TAG_DIRECTIVES];
    bool m_has_yaml_directive;
    extra::string m_arena;
    bool m_has_docs;

    // undefined at the end
    #define _enable_(bits) _enable__<bits>()
    #define _disable_(bits) _disable__<bits>()
    #define _has_any_(bits) _has_any__<bits>()
    /** @endcond */

public:

    /** @name construction and resetting
     * @{ */

    EventHandlerYamlStd() : EventHandlerStack(), m_sink(), m_val_buffers(), m_key_tag_buf(), m_val_tag_buf(), m_tag_directives(), m_has_yaml_directive(), m_arena(), m_has_docs() {}
    EventHandlerYamlStd(Callbacks const& cb) : EventHandlerStack(cb), m_sink(), m_val_buffers(), m_key_tag_buf(), m_val_tag_buf(), m_tag_directives(), m_has_yaml_directive(), m_arena(), m_has_docs()  {}
    EventHandlerYamlStd(EventSink *sink, Callbacks const& cb) : EventHandlerStack(cb), m_sink(sink), m_val_buffers(), m_key_tag_buf(), m_val_tag_buf(), m_tag_directives(), m_has_yaml_directive(), m_arena(), m_has_docs()
    {
        reset();
    }
    EventHandlerYamlStd(EventSink *sink) : EventHandlerYamlStd(sink, get_callbacks()) {}

    void reset()
    {
        _stack_reset_root();
        m_curr->flags |= RUNK|RTOP;
        m_has_yaml_directive = false;
        for(TagDirective &td : m_tag_directives)
            td = {};
        m_val_buffers.clear();
        m_val_buffers.resize((size_t)m_stack.size());
        m_arena.clear();
        m_arena.reserve(1024);
        m_key_tag_buf.resize(256);
        m_val_tag_buf.resize(256);
        m_has_docs = false;
    }

    /** @} */

public:

    /** @name parse events
     * @{ */

    void start_parse(const char* filename, detail::pfn_relocate_arena relocate_arena, void *relocate_arena_data)
    {
        this->_stack_start_parse(filename, relocate_arena, relocate_arena_data);
    }

    void finish_parse()
    {
        if((_num_tag_directives() || m_has_yaml_directive) && !m_has_docs)
            _RYML_CB_ERR_(m_stack.m_callbacks, "directives cannot be used without a document", {});
        this->_stack_finish_parse();
    }

    void cancel_parse()
    {
        while(m_stack.size() > 1)
            _pop();
        _buf_flush_();
    }

    /** @} */

public:

    /** @name YAML stream events */
    /** @{ */

    void begin_stream()
    {
        _send_("+STR\n");
    }

    void end_stream()
    {
        _send_("-STR\n");
        _buf_flush_();
    }

    /** @} */

public:

    /** @name YAML document events */
    /** @{ */

    /** implicit doc start (without ---) */
    void begin_doc()
    {
        _c4dbgp("begin_doc");
        if(_stack_should_push_on_begin_doc())
        {
            _c4dbgp("push!");
            _push();
            _enable_(DOC);
        }
        _send_("+DOC\n");
        m_has_docs = true;
    }
    /** implicit doc end (without ...) */
    void end_doc()
    {
        _c4dbgp("end_doc");
        _send_("-DOC\n");
        if(_stack_should_pop_on_end_doc())
        {
            _c4dbgp("pop!");
            _pop();
        }
    }

    /** explicit doc start, with --- */
    void begin_doc_expl()
    {
        _c4dbgp("begin_doc_expl");
        if(_stack_should_push_on_begin_doc())
        {
            _c4dbgp("push!");
            _push();
        }
        _send_("+DOC ---\n");
        _enable_(DOC);
        m_has_docs = true;
    }
    /** explicit doc end, with ... */
    void end_doc_expl()
    {
        _c4dbgp("end_doc_expl");
        _send_("-DOC ...\n");
        if(_stack_should_pop_on_end_doc())
        {
            _c4dbgp("pop!");
            _pop();
        }
        m_has_yaml_directive = false;
    }

    /** @} */

public:

    /** @name YAML map functions */
    /** @{ */

    void begin_map_key_flow()
    {
        _RYML_CB_CHECK(m_stack.m_callbacks, !_has_any_(VAL));
        _send_("+MAP {}");
        _send_key_props_();
        _send_('\n');
        _mark_parent_with_children_();
        _enable_(MAP|FLOW_SL);
        _push();
    }
    void begin_map_key_block()
    {
        _RYML_CB_CHECK(m_stack.m_callbacks, !_has_any_(VAL));
        _send_("+MAP");
        _send_key_props_();
        _send_('\n');
        _mark_parent_with_children_();
        _enable_(MAP|BLOCK);
        _push();
    }

    void begin_map_val_flow()
    {
        _RYML_CB_CHECK(m_stack.m_callbacks, !_has_any_(VAL));
        _send_("+MAP {}");
        _send_val_props_();
        _send_('\n');
        _mark_parent_with_children_();
        _enable_(MAP|FLOW_SL);
        _push();
    }
    void begin_map_val_block()
    {
        _RYML_CB_CHECK(m_stack.m_callbacks, !_has_any_(VAL));
        _send_("+MAP");
        _send_val_props_();
        _send_('\n');
        _mark_parent_with_children_();
        _enable_(MAP|BLOCK);
        _push();
    }

    void end_map()
    {
        _pop();
        _send_("-MAP\n");
    }

    /** @} */

public:

    /** @name YAML seq events */
    /** @{ */

    void begin_seq_key_flow()
    {
        _RYML_CB_CHECK(m_stack.m_callbacks, !_has_any_(VAL));
        _send_("+SEQ []");
        _send_key_props_();
        _send_('\n');
        _mark_parent_with_children_();
        _enable_(SEQ|FLOW_SL);
        _push();
    }
    void begin_seq_key_block()
    {
        _RYML_CB_CHECK(m_stack.m_callbacks, !_has_any_(VAL));
        _send_("+SEQ");
        _send_key_props_();
        _send_('\n');
        _mark_parent_with_children_();
        _enable_(SEQ|BLOCK);
        _push();
    }

    void begin_seq_val_flow()
    {
        _RYML_CB_CHECK(m_stack.m_callbacks, !_has_any_(VAL));
        _send_("+SEQ []");
        _send_val_props_();
        _send_('\n');
        _mark_parent_with_children_();
        _enable_(SEQ|FLOW_SL);
        _push();
    }
    void begin_seq_val_block()
    {
        _RYML_CB_CHECK(m_stack.m_callbacks, !_has_any_(VAL));
        _send_("+SEQ");
        _send_val_props_();
        _send_('\n');
        _mark_parent_with_children_();
        _enable_(SEQ|BLOCK);
        _push();
    }

    void end_seq()
    {
        _pop();
        _send_("-SEQ\n"); // before popping
    }

    /** @} */

public:

    /** @name YAML structure events */
    /** @{ */

    void add_sibling()
    {
        _RYML_CB_ASSERT(m_stack.m_callbacks, m_parent);
        _buf_flush_to_(m_curr->level, m_parent->level);
        m_curr->ev_data = {};
    }

    /** set the previous val as the first key of a new map, with flow style.
     *
     * See the documentation for @ref doc_event_handlers, which has
     * important notes about this event.
     */
    void actually_val_is_first_key_of_new_map_flow()
    {
        // ensure we have a temporary buffer to save the current val
        const id_type tmp = m_curr->level + id_type(2);
        _buf_ensure_(tmp + id_type(2));
        // save the current val to the temporary buffer
        _buf_flush_to_(m_curr->level, tmp);
        _disable_(_VALMASK|VAL_STYLE);
        // create the map.
        // this will push a new level, and tmp is one further
        begin_map_val_flow();
        _RYML_CB_ASSERT(m_stack.m_callbacks, tmp != m_curr->level);
        // now move the saved val as the first key
        _buf_flush_to_(tmp, m_curr->level);
    }

    /** like its flow counterpart, but this function can only be
     * called after the end of a flow-val at root or doc level.
     *
     * See the documentation for @ref doc_event_handlers, which has
     * important notes about this event.
     */
    void actually_val_is_first_key_of_new_map_block()
    {
        EventSink &sink = _buf_();
        substr full = sink;(void)full;
        // interpolate +MAP\n after the last +DOC\n
        _RYML_CB_ASSERT(m_stack.m_callbacks, full.len);
        _RYML_CB_ASSERT(m_stack.m_callbacks, !full.count('\r'));
        size_t docpos = sink.find_last("+DOC\n");
        if(docpos != npos)
        {
            _RYML_CB_ASSERT(m_stack.m_callbacks, (m_stack.size() == 1u) ? (docpos >= 5u) : (docpos == 0u));
            _RYML_CB_ASSERT(m_stack.m_callbacks, docpos + 5u < full.len);
            sink.insert("+MAP\n", docpos + 5u);
        }
        else
        {
            // ... or interpolate +MAP\n after the last +DOC ---\n
            docpos = sink.find_last("+DOC ---\n");
            _RYML_CB_ASSERT(m_stack.m_callbacks, docpos != npos);
            _RYML_CB_ASSERT(m_stack.m_callbacks, (m_stack.size() == 1u) ? (docpos >= 5u) : (docpos == 0u));
            _RYML_CB_ASSERT(m_stack.m_callbacks, docpos + 9u < full.len);
            sink.insert("+MAP\n", docpos + 9u);
        }
        _push();
    }

    /** @} */

public:

    /** @name YAML scalar events */
    /** @{ */


    C4_ALWAYS_INLINE void set_key_scalar_plain(csubstr scalar)
    {
        _c4dbgpf("node[{}]: set key scalar plain: [{}]~~~{}~~~ ({})", m_curr->node_id, scalar.len, scalar, reinterpret_cast<void const*>(scalar.str));
        _send_key_scalar_(scalar, ':');
        _enable_(KEY|KEY_PLAIN);
    }
    C4_ALWAYS_INLINE void set_val_scalar_plain(csubstr scalar)
    {
        _c4dbgpf("node[{}]: set val scalar plain: [{}]~~~{}~~~ ({})", m_curr->node_id, scalar.len, scalar, reinterpret_cast<void const*>(scalar.str));
        _send_val_scalar_(scalar, ':');
        _enable_(VAL|VAL_PLAIN);
    }


    C4_ALWAYS_INLINE void set_key_scalar_dquoted(csubstr scalar)
    {
        _c4dbgpf("node[{}]: set key scalar dquot: [{}]~~~{}~~~ ({})", m_curr->node_id, scalar.len, scalar, reinterpret_cast<void const*>(scalar.str));
        _send_key_scalar_(scalar, '"');
        _enable_(KEY|KEY_DQUO);
    }
    C4_ALWAYS_INLINE void set_val_scalar_dquoted(csubstr scalar)
    {
        _c4dbgpf("node[{}]: set val scalar dquot: [{}]~~~{}~~~ ({})", m_curr->node_id, scalar.len, scalar, reinterpret_cast<void const*>(scalar.str));
        _send_val_scalar_(scalar, '"');
        _enable_(VAL|VAL_DQUO);
    }


    C4_ALWAYS_INLINE void set_key_scalar_squoted(csubstr scalar)
    {
        _c4dbgpf("node[{}]: set key scalar squot: [{}]~~~{}~~~ ({})", m_curr->node_id, scalar.len, scalar, reinterpret_cast<void const*>(scalar.str));
        _send_key_scalar_(scalar, '\'');
        _enable_(KEY|KEY_SQUO);
    }
    C4_ALWAYS_INLINE void set_val_scalar_squoted(csubstr scalar)
    {
        _c4dbgpf("node[{}]: set val scalar squot: [{}]~~~{}~~~ ({})", m_curr->node_id, scalar.len, scalar, reinterpret_cast<void const*>(scalar.str));
        _send_val_scalar_(scalar, '\'');
        _enable_(VAL|VAL_SQUO);
    }


    C4_ALWAYS_INLINE void set_key_scalar_literal(csubstr scalar)
    {
        _c4dbgpf("node[{}]: set key scalar literal: [{}]~~~{}~~~ ({})", m_curr->node_id, scalar.len, scalar, reinterpret_cast<void const*>(scalar.str));
        _send_key_scalar_(scalar, '|');
        _enable_(KEY|KEY_LITERAL);
    }
    C4_ALWAYS_INLINE void set_val_scalar_literal(csubstr scalar)
    {
        _c4dbgpf("node[{}]: set val scalar literal: [{}]~~~{}~~~ ({})", m_curr->node_id, scalar.len, scalar, reinterpret_cast<void const*>(scalar.str));
        _send_val_scalar_(scalar, '|');
        _enable_(VAL|VAL_LITERAL);
    }


    C4_ALWAYS_INLINE void set_key_scalar_folded(csubstr scalar)
    {
        _c4dbgpf("node[{}]: set key scalar folded: [{}]~~~{}~~~ ({})", m_curr->node_id, scalar.len, scalar, reinterpret_cast<void const*>(scalar.str));
        _send_key_scalar_(scalar, '>');
        _enable_(KEY|KEY_FOLDED);
    }
    C4_ALWAYS_INLINE void set_val_scalar_folded(csubstr scalar)
    {
        _c4dbgpf("node[{}]: set val scalar folded: [{}]~~~{}~~~ ({})", m_curr->node_id, scalar.len, scalar, reinterpret_cast<void const*>(scalar.str));
        _send_val_scalar_(scalar, '>');
        _enable_(VAL|VAL_FOLDED);
    }


    C4_ALWAYS_INLINE void mark_key_scalar_unfiltered()
    {
        C4_NOT_IMPLEMENTED();
    }
    C4_ALWAYS_INLINE void mark_val_scalar_unfiltered()
    {
        C4_NOT_IMPLEMENTED();
    }

    /** @} */

public:

    /** @name YAML anchor/reference events */
    /** @{ */

    void set_key_anchor(csubstr anchor)
    {
        _c4dbgpf("node[{}]: set key anchor: [{}]~~~{}~~~", m_curr->node_id, anchor.len, anchor);
        if(C4_UNLIKELY(_has_any_(KEYANCH)))
            _RYML_CB_ERR_(m_stack.m_callbacks, "key cannot have both anchor and ref", m_curr->pos);
        _RYML_CB_ASSERT(m_stack.m_callbacks, !anchor.begins_with('&'));
        _enable_(KEYANCH);
        m_curr->ev_data.m_key.anchor = anchor;
    }
    void set_val_anchor(csubstr anchor)
    {
        _c4dbgpf("node[{}]: set val anchor: [{}]~~~{}~~~", m_curr->node_id, anchor.len, anchor);
        if(C4_UNLIKELY(_has_any_(VALREF)))
            _RYML_CB_ERR_(m_stack.m_callbacks, "val cannot have both anchor and ref", m_curr->pos);
        _RYML_CB_ASSERT(m_stack.m_callbacks, !anchor.begins_with('&'));
        _enable_(VALANCH);
        m_curr->ev_data.m_val.anchor = anchor;
    }

    void set_key_ref(csubstr ref)
    {
        _c4dbgpf("node[{}]: set key ref: [{}]~~~{}~~~", m_curr->node_id, ref.len, ref);
        if(C4_UNLIKELY(_has_any_(KEYANCH)))
            _RYML_CB_ERR_(m_stack.m_callbacks, "key cannot have both anchor and ref", m_curr->pos);
        _RYML_CB_ASSERT(m_stack.m_callbacks, ref.begins_with('*'));
        _enable_(KEY|KEYREF);
        _send_("=ALI ");
        _send_(ref);
        _send_('\n');
    }
    void set_val_ref(csubstr ref)
    {
        _c4dbgpf("node[{}]: set val ref: [{}]~~~{}~~~", m_curr->node_id, ref.len, ref);
        if(C4_UNLIKELY(_has_any_(VALANCH)))
            _RYML_CB_ERR_(m_stack.m_callbacks, "val cannot have both anchor and ref", m_curr->pos);
        _RYML_CB_ASSERT(m_stack.m_callbacks, ref.begins_with('*'));
        _enable_(VAL|VALREF);
        _send_("=ALI ");
        _send_(ref);
        _send_('\n');
    }

    /** @} */

public:

    /** @name YAML tag events */
    /** @{ */

    void set_key_tag(csubstr tag)
    {
        _c4dbgpf("node[{}]: set key tag: [{}]~~~{}~~~", m_curr->node_id, tag.len, tag);
        _enable_(KEYTAG);
        m_curr->ev_data.m_key.tag = _transform_directive(tag, &m_key_tag_buf);
    }
    void set_val_tag(csubstr tag)
    {
        _c4dbgpf("node[{}]: set val tag: [{}]~~~{}~~~", m_curr->node_id, tag.len, tag);
        _enable_(VALTAG);
        m_curr->ev_data.m_val.tag = _transform_directive(tag, &m_val_tag_buf);
    }

    /** @} */

public:

    /** @name YAML directive events */
    /** @{ */

    void add_directive(csubstr directive)
    {
        _RYML_CB_ASSERT(m_stack.m_callbacks, directive.begins_with('%'));
        if(directive.begins_with("%TAG"))
        {
            const id_type pos = _num_tag_directives();
            if(C4_UNLIKELY(pos >= RYML_MAX_TAG_DIRECTIVES))
                _RYML_CB_ERR_(m_stack.m_callbacks, "too many directives", m_curr->pos);
            if(C4_UNLIKELY(!m_tag_directives[pos].create_from_str(directive)))
                _RYML_CB_ERR_(m_stack.m_callbacks, "failed to add directive", m_curr->pos);
        }
        else if(directive.begins_with("%YAML"))
        {
            _c4dbgpf("%YAML directive! ignoring...: {}", directive);
            if(C4_UNLIKELY(m_has_yaml_directive))
                _RYML_CB_ERR_(m_stack.m_callbacks, "multiple yaml directives", m_curr->pos);
            m_has_yaml_directive = true;
        }
        else
        {
            _c4dbgpf("unknown directive! ignoring... {}", directive);
        }
    }

    /** @} */

public:

    /** @name YAML arena events */
    /** @{ */

    substr alloc_arena(size_t len)
    {
        const size_t sz = m_arena.size();
        csubstr prev = m_arena;
        m_arena.resize(sz + len);
        substr out = to_substr(m_arena).sub(sz);
        substr curr = to_substr(m_arena);
        if(curr.str != prev.str)
            _stack_relocate_to_new_arena(prev, curr);
        return out;
    }

    substr alloc_arena(size_t len, substr *relocated)
    {
        csubstr prev = m_arena;
        if(!prev.is_super(*relocated))
            return alloc_arena(len);
        substr out = alloc_arena(len);
        substr curr = to_substr(m_arena);
        if(curr.str != prev.str)
            *relocated = _stack_relocate_to_new_arena(*relocated, prev, curr);
        return out;
    }

    /** @} */

public:

    /** @cond dev */

    /** push a new parent, add a child to the new parent, and set the
     * child as the current node */
    void _push()
    {
        _stack_push();
        _buf_ensure_(m_stack.size() + id_type(1));
        _buf_().clear();
        m_curr->ev_data = {};
        _c4dbgpf("pushed! level={}", m_curr->level);
    }

    /** end the current scope */
    void _pop()
    {
        _buf_flush_to_(m_curr->level, m_parent->level);
        _stack_pop();
    }

    template<type_bits bits> C4_ALWAYS_INLINE void _enable__() noexcept
    {
        m_curr->ev_data.m_type.type = static_cast<NodeType_e>(m_curr->ev_data.m_type.type | bits);
    }
    template<type_bits bits> C4_ALWAYS_INLINE void _disable__() noexcept
    {
        m_curr->ev_data.m_type.type = static_cast<NodeType_e>(m_curr->ev_data.m_type.type & (~bits));
    }
    template<type_bits bits> C4_ALWAYS_INLINE bool _has_any__() const noexcept
    {
        return (m_curr->ev_data.m_type.type & bits) != 0;
    }

    void _mark_parent_with_children_()
    {
        if(m_parent)
            m_parent->has_children = true;
    }

    EventSink& _buf_() noexcept
    {
        _RYML_CB_ASSERT(m_stack.m_callbacks, m_curr->level < m_val_buffers.size());
        return m_val_buffers[m_curr->level];
    }

    EventSink& _buf_(id_type level) noexcept
    {
        _RYML_CB_ASSERT(m_stack.m_callbacks, level < m_val_buffers.size());
        return m_val_buffers[level];
    }

    EventSink const& _buf_(id_type level) const noexcept
    {
        _RYML_CB_ASSERT(m_stack.m_callbacks, level < m_val_buffers.size());
        return m_val_buffers[level];
    }

    static void _buf_flush_to_(EventSink &C4_RESTRICT src, EventSink &C4_RESTRICT dst) noexcept
    {
        dst.append(src);
        src.clear();
    }

    void _buf_flush_to_(id_type level_src, id_type level_dst) noexcept
    {
        auto &src = _buf_(level_src);
        auto &dst = _buf_(level_dst);
        _buf_flush_to_(src, dst);
    }

    void _buf_flush_() noexcept
    {
        _buf_flush_to_(_buf_(), *m_sink);
    }

    void _buf_ensure_(id_type size_needed) noexcept
    {
        if(size_needed > m_val_buffers.size())
            m_val_buffers.resize(size_needed);
    }

    C4_ALWAYS_INLINE void _send_(csubstr s) noexcept { _buf_().append(s); }
    C4_ALWAYS_INLINE void _send_(char c) noexcept { _buf_().append(c); }

    void _send_key_scalar_(csubstr scalar, char scalar_type_code)
    {
        _send_("=VAL");
        _send_key_props_();
        _send_(' ');
        _send_(scalar_type_code);
        append_escaped(&_buf_(), scalar);
        _send_('\n');
    }
    void _send_val_scalar_(csubstr scalar, char scalar_type_code)
    {
        _send_("=VAL");
        _send_val_props_();
        _send_(' ');
        _send_(scalar_type_code);
        append_escaped(&_buf_(), scalar);
        _send_('\n');
    }

    void _send_key_props_()
    {
        if(_has_any_(KEYANCH|KEYREF))
        {
            _send_(" &");
            _send_(m_curr->ev_data.m_key.anchor);
        }
        if(_has_any_(KEYTAG))
        {
            _send_tag_(m_curr->ev_data.m_key.tag);
        }
        m_curr->ev_data.m_key = {};
        _disable_(KEYANCH|KEYREF|KEYTAG);
    }
    void _send_val_props_()
    {
        if(_has_any_(VALANCH|VALREF))
        {
            _send_(" &");
            _send_(m_curr->ev_data.m_val.anchor);
        }
        if(m_curr->ev_data.m_type.type & VALTAG)
        {
            _send_tag_(m_curr->ev_data.m_val.tag);
        }
        m_curr->ev_data.m_val = {};
        _disable_(VALANCH|VALREF|VALTAG);
    }
    void _send_tag_(csubstr tag)
    {
        _RYML_CB_ASSERT(m_stack.m_callbacks, !tag.empty());
        if(tag.str[0] == '<')
        {
            _send_(' ');
            _send_(tag);
        }
        else
        {
            _send_(" <");
            _send_(tag);
            _send_('>');
        }
    }

    void _clear_tag_directives_()
    {
        for(TagDirective &td : m_tag_directives)
            td = {};
    }
    id_type _num_tag_directives() const
    {
        // this assumes we have a very small number of tag directives
        for(id_type i = 0; i < RYML_MAX_TAG_DIRECTIVES; ++i)
            if(m_tag_directives[i].handle.empty())
                return i;
        return RYML_MAX_TAG_DIRECTIVES;
    }
    csubstr _transform_directive(csubstr tag, extra::string *output)
    {
        // lookup from the end. We want to find the first directive that
        // matches the tag and has a target node id leq than the given
        // node_id.
        for(id_type i = RYML_MAX_TAG_DIRECTIVES-1; i != NONE; --i)
        {
            TagDirective const& td = m_tag_directives[i];
            if(td.handle.empty())
                continue;
            if(tag.begins_with(td.handle))
            {
                bool retry = false;
            again1:
                size_t len = td.transform(tag, *output, m_stack.m_callbacks);
                if(len == 0)
                {
                    if(tag.begins_with("!<"))
                        return tag.sub(1);
                    return tag;
                }
                if(len > output->size())
                {
                    _RYML_CB_CHECK(m_stack.m_callbacks, !retry);
                    retry = true;
                    output->resize(len);
                    output->resize(output->capacity());
                    goto again1;
                }
                return csubstr(*output).first(len);
            }
        }
        if(tag.begins_with('!'))
        {
            if(is_custom_tag(tag))
            {
                _RYML_CB_ERR_(m_stack.m_callbacks, "tag not found", m_curr->pos);
            }
        }
        bool retry = false;
    again2:
        csubstr result = normalize_tag_long(tag, *output);
        if(!result.str)
        {
            _RYML_CB_CHECK(m_stack.m_callbacks, !retry);
            retry = true;
            output->resize(result.len);
            output->resize(output->capacity());
            goto again2;
        }
        return result;
    }

    #undef _enable_
    #undef _disable_
    #undef _has_any_

    /** @endcond */
};

/** @} */

} // namespace yml
} // namespace c4

C4_SUPPRESS_WARNING_GCC_POP

#endif /* _C4_YML_EVENT_HANDLER_YAMLSTD_HPP_ */
