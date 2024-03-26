#ifndef _C4_YML_EVENT_HANDLER_YAMLSTD_HPP_
#define _C4_YML_EVENT_HANDLER_YAMLSTD_HPP_

#ifndef _C4_YML_DETAIL_STACK_HPP_
#include "c4/yml/detail/stack.hpp"
#endif

#ifndef _C4_YML_DETAIL_PARSER_DBG_HPP_
#include "c4/yml/detail/parser_dbg.hpp"
#endif

#ifndef _C4_YML_PARSER_STATE_HPP_
#include "c4/yml/parser_state.hpp"
#endif

#ifndef _C4_YML_TREE_HPP_
#include "c4/yml/tree.hpp"
#endif


#include <vector>
#include <string>
#include "c4/yml/std/string.hpp"
#include "c4/yml/detail/print.hpp"

namespace c4 {
namespace yml {


struct EventSink
{
    std::string result; // FIXME -don't use std::string
    void reset() noexcept { result.clear(); }
    void append(csubstr s) noexcept { result.append(s.str, s.len); }
    void append(char c) noexcept { result += c; }
    void insert(csubstr s, size_t pos) noexcept { result.insert(pos, s.str, s.len); }
    void insert(char c, size_t pos) noexcept { result.insert(pos, 1, c); }
    csubstr get() const { return csubstr(&result[0], result.size()); }
    substr get() { return substr(&result[0], result.size()); }
    size_t find_last(csubstr s) const { return result.rfind(s.str, std::string::npos, s.len); }
};


inline void append_scalar_escaped(EventSink &sink, csubstr val)
{
    uint8_t const* C4_RESTRICT s = reinterpret_cast<uint8_t const*>(val.str);
#define _c4flush_use_instead(repl, skip)        \
    do {                                        \
        sink.append(val.range(prev, i));        \
        sink.append(repl);                      \
        prev = i + skip;                        \
    }                                           \
    while(0)

    size_t prev = 0;
    for(size_t i = 0; i < val.len; ++i)
    {
        switch(s[i])
        {
        case UINT8_C(0x0a): // \n
            _c4flush_use_instead("\\n", 1); break;
        case UINT8_C(0x5c): // '\\'
            _c4flush_use_instead("\\\\", 1); break;
        case UINT8_C(0x09): // \t
            _c4flush_use_instead("\\t", 1); break;
        case UINT8_C(0x0d): // \r
            _c4flush_use_instead("\\r", 1); break;
        case UINT8_C(0x00): // \0
            _c4flush_use_instead("\\0", 1); break;
        case UINT8_C(0x0c): // \f (form feed)
            _c4flush_use_instead("\\f", 1); break;
        case UINT8_C(0x08): // \b (backspace)
            _c4flush_use_instead("\\b", 1); break;
        case UINT8_C(0x07): // \a (bell)
            _c4flush_use_instead("\\a", 1); break;
        case UINT8_C(0x0b): // \v (vertical tab)
            _c4flush_use_instead("\\v", 1); break;
        case UINT8_C(0x1b): // \e (escape)
            _c4flush_use_instead("\\e", 1); break;
        case UINT8_C(0xc2):
            if(i+1 < val.len)
            {
                const uint8_t np1 = s[i+1];
                if(np1 == UINT8_C(0xa0))
                    _c4flush_use_instead("\\_", 2);
                else if(np1 == UINT8_C(0x85))
                    _c4flush_use_instead("\\N", 2);
            }
            break;
        case UINT8_C(0xe2):
            if(i+2 < val.len)
            {
                if(s[i+1] == UINT8_C(0x80))
                {
                    if(s[i+2] == UINT8_C(0xa8))
                        _c4flush_use_instead("\\L", 3);
                    else if(s[i+2] == UINT8_C(0xa9))
                        _c4flush_use_instead("\\P", 3);
                }
            }
            break;
        }
    }
    // flush the rest
    sink.append(val.sub(prev));
}



struct EventHandlerYamlStd
{
    static constexpr const bool is_events = true;
    static constexpr const bool is_wtree = false;

    // our internal state must inherit from parser state
    struct HandlerState : public ParserState
    {
        size_t tr_id;
        NodeData ev_data;
        bool ev_has_children;
    };

    using state = HandlerState;

public:

    detail::stack<state> m_stack;
    state *C4_RESTRICT m_curr;
    state *C4_RESTRICT m_parent;

public: // used only for events

    EventSink *C4_RESTRICT m_ev_sink;
    std::vector<EventSink> m_ev_val_buffers; // FIXME: don't use std::vector
    // TODO: use this for both tree and events (ie remove the tree directives)
    char m_ev_key_tag_buf[256];
    char m_ev_val_tag_buf[256];
    TagDirective m_ev_tag_directives[RYML_MAX_TAG_DIRECTIVES];
    std::string m_ev_arena;

public:

    EventHandlerYamlStd() : m_stack(), m_curr(), m_parent(), m_ev_sink(), m_ev_val_buffers() {}
    EventHandlerYamlStd(Callbacks const& cb) : m_stack(cb), m_curr(), m_parent(), m_ev_sink(), m_ev_val_buffers() {}
    EventHandlerYamlStd(EventSink *sink, Callbacks const& cb) : m_stack(cb), m_curr(), m_parent(), m_ev_sink(sink), m_ev_val_buffers()
    {
        reset();
    }
    EventHandlerYamlStd(EventSink *sink) : EventHandlerYamlStd(sink, get_callbacks()) {}

    void reset()
    {
        m_stack.clear();
        m_stack.push({});
        for(auto &td : m_ev_tag_directives)
            td = {};
        m_parent = nullptr;
        m_curr = &m_stack.top();
        m_curr->flags |= RUNK|RTOP;
        m_ev_val_buffers.resize(m_stack.size());
        m_ev_arena.clear();
    }

    void start_parse(const char* filename)
    {
        m_curr->start_parse(filename, m_curr->tr_id);
    }

    void finish_parse()
    {
    }

    void cancel_parse()
    {
        {
            while(m_stack.size() > 1)
                _pop();
            _ev_buf_flush_();
        }
    }

public:

    /** push a new parent, add a child to the new parent, and set the
     * child as the current node */
    void _push()
    {
        m_stack.push_top();
        m_parent = &m_stack.top(1); // don't use m_curr. watch out for relocations inside the prev push
        m_curr = &m_stack.top();
        m_curr->reset_after_push();
        {
            _ev_buf_ensure_(m_stack.size() + 1u);
            _ev_buf_().reset();
            m_curr->ev_data = {};
            _c4dbgpf("pushed! level={}", m_curr->level);
        }
    }

    /** end the current scope */
    void _pop()
    {
        _RYML_CB_ASSERT(m_stack.m_callbacks, m_parent);
        _RYML_CB_ASSERT(m_stack.m_callbacks, m_stack.size() > 1);
        {
            _ev_buf_flush_to_(m_curr->level, m_parent->level);
        }
        m_parent->reset_before_pop(*m_curr);
        m_stack.pop();
        m_parent = m_stack.size() > 1 ? &m_stack.top(1) : nullptr;
        m_curr = &m_stack.top();
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

    #if RYML_DBG
    #define _enable_(bits) _enable__<bits>()
    #define _disable_(bits) _disable__<bits>()
    #else
    #define _enable_(bits) _enable__<bits>()
    #define _disable_(bits) _disable__<bits>()
    #endif
    #define _has_any_(bits) _has_any__<bits>()

public:

    /** @name event sink util functions */
    /** @{ */

    void _ev_mark_parent_with_children_()
    {
        if(m_parent)
            m_parent->ev_has_children = true;
    }

    EventSink& _ev_buf_() noexcept
    {
        _RYML_CB_ASSERT(m_stack.m_callbacks, m_curr->level < m_ev_val_buffers.size());
        return m_ev_val_buffers[m_curr->level];
    }

    EventSink& _ev_buf_(size_t level) noexcept
    {
        _RYML_CB_ASSERT(m_stack.m_callbacks, level < m_ev_val_buffers.size());
        return m_ev_val_buffers[level];
    }

    EventSink const& _ev_buf_(size_t level) const noexcept
    {
        _RYML_CB_ASSERT(m_stack.m_callbacks, level < m_ev_val_buffers.size());
        return m_ev_val_buffers[level];
    }

    static void _ev_buf_flush_to_(EventSink &C4_RESTRICT src, EventSink &C4_RESTRICT dst) noexcept
    {
        dst.append(src.get());
        src.reset();
    }

    void _ev_buf_flush_to_(size_t level_src, size_t level_dst) noexcept
    {
        auto &src = _ev_buf_(level_src);
        auto &dst = _ev_buf_(level_dst);
        _ev_buf_flush_to_(src, dst);
    }

    void _ev_buf_flush_() noexcept
    {
        _ev_buf_flush_to_(_ev_buf_(), *m_ev_sink);
    }

    void _ev_buf_ensure_(size_t size_needed) noexcept
    {
        if(size_needed > m_ev_val_buffers.size())
            m_ev_val_buffers.resize(size_needed);
    }

    C4_ALWAYS_INLINE void _ev_send_(csubstr s) noexcept { _ev_buf_().append(s); }
    C4_ALWAYS_INLINE void _ev_send_(char c) noexcept { _ev_buf_().append(c); }

    void _ev_send_key_scalar_(csubstr scalar, char scalar_type_code)
    {
        _ev_send_("=VAL");
        _ev_send_key_props_();
        _ev_send_(' ');
        _ev_send_(scalar_type_code);
        append_scalar_escaped(_ev_buf_(), scalar);
        _ev_send_('\n');
    }
    void _ev_send_val_scalar_(csubstr scalar, char scalar_type_code)
    {
        _ev_send_("=VAL");
        _ev_send_val_props_();
        _ev_send_(' ');
        _ev_send_(scalar_type_code);
        append_scalar_escaped(_ev_buf_(), scalar);
        _ev_send_('\n');
    }

    void _ev_send_key_props_()
    {
        if(_has_any_(KEYANCH|KEYREF))
        {
            _ev_send_(" &");
            _ev_send_(m_curr->ev_data.m_key.anchor);
        }
        if(_has_any_(KEYTAG))
        {
            _ev_send_tag_(m_curr->ev_data.m_key.tag);
        }
        m_curr->ev_data.m_key = {};
        _disable_(KEYANCH|KEYREF|KEYTAG);
    }
    void _ev_send_val_props_()
    {
        if(_has_any_(VALANCH|VALREF))
        {
            _ev_send_(" &");
            _ev_send_(m_curr->ev_data.m_val.anchor);
        }
        if(m_curr->ev_data.m_type.type & VALTAG)
        {
            _ev_send_tag_(m_curr->ev_data.m_val.tag);
        }
        m_curr->ev_data.m_val = {};
        _disable_(VALANCH|VALREF|VALTAG);
    }
    void _ev_send_tag_(csubstr tag)
    {
        _RYML_CB_ASSERT(m_stack.m_callbacks, !tag.empty());
        if(tag.str[0] == '<')
        {
            _ev_send_(' ');
            _ev_send_(tag);
        }
        else
        {
            _ev_send_(" <");
            _ev_send_(tag);
            _ev_send_('>');
        }
    }

    void _ev_clear_tag_directives_()
    {
        for(TagDirective &td : m_ev_tag_directives)
            td = {};
    }
    size_t _ev_num_tag_directives() const
    {
        // this assumes we have a very small number of tag directives
        for(size_t i = 0; i < RYML_MAX_TAG_DIRECTIVES; ++i)
            if(m_ev_tag_directives[i].handle.empty())
                return i;
        return RYML_MAX_TAG_DIRECTIVES;
    }
    csubstr _ev_transform_directive(csubstr tag, substr output)
    {
        // lookup from the end. We want to find the first directive that
        // matches the tag and has a target node id leq than the given
        // node_id.
        for(size_t i = RYML_MAX_TAG_DIRECTIVES-1; i != npos; --i)
        {
            TagDirective const& td = m_ev_tag_directives[i];
            if(td.handle.empty())
                continue;
            if(tag.begins_with(td.handle))
            {
                size_t len = td.transform(tag, output, m_stack.m_callbacks);
                if(len == 0)
                {
                    if(tag.begins_with("!<"))
                        return tag.sub(1);
                    return tag;
                }
                _RYML_CB_CHECK(m_stack.m_callbacks, len <= output.len);
                return output.first(len);
            }
        }
        if(tag.begins_with('!'))
        {
            if(is_custom_tag(tag))
            {
                _RYML_CB_ERR_(m_stack.m_callbacks, "tag not found", m_curr->pos);
            }
        }
        csubstr result = normalize_tag_long(tag, output);
        _RYML_CB_CHECK(m_stack.m_callbacks, result.len > 0);
        _RYML_CB_CHECK(m_stack.m_callbacks, result.str);
        return result;
    }

    /** @} */

public:

    /** @name YAML stream functions */
    /** @{ */

    void begin_stream()
    {
        {
            _ev_send_("+STR\n");
        }
    }

    void end_stream()
    {
        {
            _ev_send_("-STR\n");
            _ev_buf_flush_();
        }
    }

    /** @} */

public:

    /** @name YAML document functions */
    /** @{ */

    bool _should_push_on_begin_doc() const
    {
        const bool is_root = (m_stack.size() == 1u);
        {
            return is_root && (_has_any_(DOC) || m_curr->ev_has_children);
        }
    }

    bool _should_pop_on_end_doc() const
    {
        const bool is_root = (m_stack.size() == 1u);
        return !is_root && _has_any_(DOC);
    }

    /** called at the beginning of parsing a non-empty stream */
    void begin_doc()
    {
        _c4dbgp("begin_doc");
        {
            if(_should_push_on_begin_doc())
            {
                _c4dbgp("push!");
                _push();
                _enable_(DOC);
            }
            _ev_send_("+DOC\n");
        }
    }
    /** called at the end of parsing, before @ref end_stream() */
    void end_doc()
    {
        _c4dbgp("end_doc");
        {
            _ev_send_("-DOC\n");
        }
        if(_should_pop_on_end_doc())
        {
            _c4dbgp("pop!");
            _pop();
        }
    }

    /** explicit begin doc: called on `---` tokens */
    void begin_doc_expl()
    {
        _c4dbgp("begin_doc_expl");
        {
            if(_should_push_on_begin_doc())
            {
                _c4dbgp("push!");
                _push();
            }
            _ev_send_("+DOC ---\n");
        }
        _enable_(DOC);
    }
    /** explicit end doc: called on `...` tokens */
    void end_doc_expl()
    {
        _c4dbgp("end_doc_expl");
        {
            _ev_send_("-DOC ...\n");
        }
        if(_should_pop_on_end_doc())
        {
            _c4dbgp("pop!");
            _pop();
        }
    }

    /** Check that the current tokens are not trailing on the previous
     * doc */
    void check_trailing_doc_token()
    {
        const bool isndoc = (m_curr->flags & NDOC) != 0;
        {
            csubstr last = _ev_buf_().get().trimr("\r\n");
            bool prev_is_doc = last.begins_with("+DOC");
            _c4dbgpf("has_children={} ndoc={} prev_is_doc={} sofar=~~~\n{}", m_curr->ev_has_children, isndoc, prev_is_doc, _ev_buf_().get());
            if(m_curr->ev_has_children && !isndoc && !prev_is_doc)
                _RYML_CB_ERR_(m_stack.m_callbacks, "parse error", m_curr->pos);
            else if(last.ends_with("-MAP") || last.ends_with("-SEQ"))
                _RYML_CB_ERR_(m_stack.m_callbacks, "parse error", m_curr->pos);
            else
            {
                size_t pos = last.last_of('\n');
                if(pos != npos && pos+1 < last.len)
                {
                    _c4dbgpf("last newl={}", pos);
                    const csubstr lastline = last.sub(pos+1);
                    const bool prev_is_val = lastline.begins_with("=VAL");
                    if(prev_is_val)
                        _RYML_CB_ERR_(m_stack.m_callbacks, "parse error", m_curr->pos);
                }
            }
        }
    }

    /** @} */

public:

    /** @name map functions */
    /** @{ */

    void begin_map_key_flow()
    {
        {
            _ev_send_("+MAP {}");
            _ev_send_key_props_();
            _ev_send_('\n');
            _ev_mark_parent_with_children_();
            _enable_(MAP|_WIP_STYLE_FLOW_SL);
            _push();
        }
    }
    void begin_map_key_block()
    {
        {
            _ev_send_("+MAP");
            _ev_send_key_props_();
            _ev_send_('\n');
            _ev_mark_parent_with_children_();
            _enable_(MAP|_WIP_STYLE_BLOCK);
            _push();
        }
    }

    void begin_map_val_flow()
    {
        {
            _ev_send_("+MAP {}");
            _ev_send_val_props_();
            _ev_send_('\n');
            _ev_mark_parent_with_children_();
        }
        _enable_(MAP|_WIP_STYLE_FLOW_SL);
        _push();
    }
    void begin_map_val_block()
    {
        {
            _ev_send_("+MAP");
            _ev_send_val_props_();
            _ev_send_('\n');
            _ev_mark_parent_with_children_();
        }
        _enable_(MAP|_WIP_STYLE_BLOCK);
        _push();
    }

    void end_map()
    {
        _pop();
        {
            _ev_send_("-MAP\n");
        }
    }

    /** @} */

public:

    /** @name seq functions */
    /** @{ */

    void begin_seq_key_flow()
    {
        {
            _ev_send_("+SEQ []");
            _ev_send_key_props_();
            _ev_send_('\n');
            _ev_mark_parent_with_children_();
            _enable_(SEQ|_WIP_STYLE_FLOW_SL);
            _push();
        }
    }
    void begin_seq_key_block()
    {
        {
            _ev_send_("+SEQ");
            _ev_send_key_props_();
            _ev_send_('\n');
            _ev_mark_parent_with_children_();
            _enable_(SEQ|_WIP_STYLE_BLOCK);
            _push();
        }
    }

    void begin_seq_val_flow()
    {
        {
            _ev_send_("+SEQ []");
            _ev_send_val_props_();
            _ev_send_('\n');
            _ev_mark_parent_with_children_();
        }
        _enable_(SEQ|_WIP_STYLE_FLOW_SL);
        _push();
    }
    void begin_seq_val_block()
    {
        {
            _ev_send_("+SEQ");
            _ev_send_val_props_();
            _ev_send_('\n');
            _ev_mark_parent_with_children_();
        }
        _enable_(SEQ|_WIP_STYLE_BLOCK);
        _push();
    }

    void end_seq()
    {
        _pop();
        {
            _ev_send_("-SEQ\n"); // before popping
        }
    }

    void actually_val_is_first_key_of_new_map_flow()
    {
        {
            // ensure we have a temporary buffer to save the current val
            const size_t tmp = m_curr->level + 2u;
            _ev_buf_ensure_(tmp + 1u);
            // save the current val to the temporary buffer
            _ev_buf_flush_to_(m_curr->level, tmp);
            // create the map.
            // this will push a new level, and tmp is one further
            begin_map_val_flow();
            _RYML_CB_ASSERT(m_stack.m_callbacks, tmp != m_curr->level);
            // now move the saved val as the first key
            _ev_buf_flush_to_(tmp, m_curr->level);
        }
    }

    /** like its flow counterpart, but this function can only be
     * called after the end of a flow-val at root or doc level. */
    void actually_val_is_first_key_of_new_map_block()
    {
        {
            EventSink &sink = _ev_buf_();
            substr full = sink.get();(void)full;
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
    }

    /** @} */

public:

    /** @name scalar functions */
    /** @{ */

    void add_sibling()
    {
        _RYML_CB_ASSERT(m_stack.m_callbacks, m_parent);
        {
            _ev_buf_flush_to_(m_curr->level, m_parent->level);
            m_curr->ev_data = {};
        }
    }

    C4_ALWAYS_INLINE void set_key_scalar_plain(csubstr scalar)
    {
        _c4dbgpf("node[{}]: set key scalar plain: [{}]~~~{}~~~ ({})", m_curr->tr_id, scalar.len, scalar, reinterpret_cast<void const*>(scalar.str));
        {
            _ev_send_key_scalar_(scalar, ':');
        }
        _enable_(KEY|_WIP_KEY_PLAIN);
    }
    C4_ALWAYS_INLINE void set_val_scalar_plain(csubstr scalar)
    {
        _c4dbgpf("node[{}]: set val scalar plain: [{}]~~~{}~~~ ({})", m_curr->tr_id, scalar.len, scalar, reinterpret_cast<void const*>(scalar.str));
        {
            _ev_send_val_scalar_(scalar, ':');
        }
        _enable_(VAL|_WIP_VAL_PLAIN);
    }

    C4_ALWAYS_INLINE void set_key_scalar_dquoted(csubstr scalar)
    {
        _c4dbgpf("node[{}]: set key scalar dquot: [{}]~~~{}~~~ ({})", m_curr->tr_id, scalar.len, scalar, reinterpret_cast<void const*>(scalar.str));
        {
            _ev_send_key_scalar_(scalar, '"');
        }
        _enable_(KEY|_WIP_KEY_DQUO);
    }
    C4_ALWAYS_INLINE void set_val_scalar_dquoted(csubstr scalar)
    {
        _c4dbgpf("node[{}]: set val scalar dquot: [{}]~~~{}~~~ ({})", m_curr->tr_id, scalar.len, scalar, reinterpret_cast<void const*>(scalar.str));
        {
            _ev_send_val_scalar_(scalar, '"');
        }
        _enable_(VAL|_WIP_VAL_DQUO);
    }

    C4_ALWAYS_INLINE void set_key_scalar_squoted(csubstr scalar)
    {
        _c4dbgpf("node[{}]: set key scalar squot: [{}]~~~{}~~~ ({})", m_curr->tr_id, scalar.len, scalar, reinterpret_cast<void const*>(scalar.str));
        {
            _ev_send_key_scalar_(scalar, '\'');
        }
        _enable_(KEY|_WIP_KEY_SQUO);
    }
    C4_ALWAYS_INLINE void set_val_scalar_squoted(csubstr scalar)
    {
        _c4dbgpf("node[{}]: set val scalar squot: [{}]~~~{}~~~ ({})", m_curr->tr_id, scalar.len, scalar, reinterpret_cast<void const*>(scalar.str));
        {
            _ev_send_val_scalar_(scalar, '\'');
        }
        _enable_(VAL|_WIP_VAL_SQUO);
    }

    C4_ALWAYS_INLINE void set_key_scalar_literal(csubstr scalar)
    {
        _c4dbgpf("node[{}]: set key scalar literal: [{}]~~~{}~~~ ({})", m_curr->tr_id, scalar.len, scalar, reinterpret_cast<void const*>(scalar.str));
        {
            _ev_send_key_scalar_(scalar, '|');
        }
        _enable_(KEY|_WIP_KEY_LITERAL);
    }
    C4_ALWAYS_INLINE void set_val_scalar_literal(csubstr scalar)
    {
        _c4dbgpf("node[{}]: set val scalar literal: [{}]~~~{}~~~ ({})", m_curr->tr_id, scalar.len, scalar, reinterpret_cast<void const*>(scalar.str));
        {
            _ev_send_val_scalar_(scalar, '|');
        }
        _enable_(VAL|_WIP_VAL_LITERAL);
    }

    C4_ALWAYS_INLINE void set_key_scalar_folded(csubstr scalar)
    {
        _c4dbgpf("node[{}]: set key scalar folded: [{}]~~~{}~~~ ({})", m_curr->tr_id, scalar.len, scalar, reinterpret_cast<void const*>(scalar.str));
        {
            _ev_send_key_scalar_(scalar, '>');
        }
        _enable_(KEY|_WIP_KEY_FOLDED);
    }
    C4_ALWAYS_INLINE void set_val_scalar_folded(csubstr scalar)
    {
        _c4dbgpf("node[{}]: set val scalar folded: [{}]~~~{}~~~ ({})", m_curr->tr_id, scalar.len, scalar, reinterpret_cast<void const*>(scalar.str));
        {
            _ev_send_val_scalar_(scalar, '>');
        }
        _enable_(VAL|_WIP_VAL_FOLDED);
    }

    C4_ALWAYS_INLINE void mark_key_scalar_unfiltered()
    {
        {
            C4_NOT_IMPLEMENTED();
        }
        _enable_(_WIP_KEY_UNFILT);
    }
    C4_ALWAYS_INLINE void mark_val_scalar_unfiltered()
    {
        {
            C4_NOT_IMPLEMENTED();
        }
        _enable_(_WIP_VAL_UNFILT);
    }

    /** @} */

public:

    /** @name anchor/reference functions */
    /** @{ */

    void set_key_anchor(csubstr anchor)
    {
        _c4dbgpf("node[{}]: set key anchor: [{}]~~~{}~~~", m_curr->tr_id, anchor.len, anchor);
        RYML_ASSERT(!anchor.begins_with('&'));
        _enable_(KEYANCH);
        {
            m_curr->ev_data.m_key.anchor = anchor;
        }
    }
    void set_val_anchor(csubstr anchor)
    {
        _c4dbgpf("node[{}]: set val anchor: [{}]~~~{}~~~", m_curr->tr_id, anchor.len, anchor);
        RYML_ASSERT(!anchor.begins_with('&'));
        _enable_(VALANCH);
        {
            m_curr->ev_data.m_val.anchor = anchor;
        }
    }

    C4_ALWAYS_INLINE bool has_key_anchor() const
    {
        return _has_any_(KEYANCH);
    }

    C4_ALWAYS_INLINE bool has_val_anchor() const
    {
        return _has_any_(VALANCH);
    }

    void set_key_ref(csubstr ref)
    {
        _c4dbgpf("node[{}]: set key ref: [{}]~~~{}~~~", m_curr->tr_id, ref.len, ref);
        RYML_ASSERT(ref.begins_with('*'));
        _enable_(KEY|KEYREF);
        {
            _ev_send_("=ALI ");
            _ev_send_(ref);
            _ev_send_('\n');
        }
    }
    void set_val_ref(csubstr ref)
    {
        _c4dbgpf("node[{}]: set val ref: [{}]~~~{}~~~", m_curr->tr_id, ref.len, ref);
        RYML_ASSERT(ref.begins_with('*'));
        _enable_(VAL|VALREF);
        {
            _ev_send_("=ALI ");
            _ev_send_(ref);
            _ev_send_('\n');
        }
    }

    /** @} */

public:

    /** @name tag functions */
    /** @{ */

    void set_key_tag(csubstr tag)
    {
        _c4dbgpf("node[{}]: set key tag: [{}]~~~{}~~~", m_curr->tr_id, tag.len, tag);
        _enable_(KEYTAG);
        {
            m_curr->ev_data.m_key.tag = _ev_transform_directive(tag, m_ev_key_tag_buf);
        }
    }
    void set_val_tag(csubstr tag)
    {
        _c4dbgpf("node[{}]: set val tag: [{}]~~~{}~~~", m_curr->tr_id, tag.len, tag);
        _enable_(VALTAG);
        {
            m_curr->ev_data.m_val.tag = _ev_transform_directive(tag, m_ev_val_tag_buf);
        }
    }

    C4_ALWAYS_INLINE bool has_key_tag() const
    {
        return _has_any_(KEYTAG);
    }

    C4_ALWAYS_INLINE bool has_val_tag() const
    {
        return _has_any_(VALTAG);
    }

    /** @} */

public:

    /** @name directive functions */
    /** @{ */

    void add_directive(csubstr directive)
    {
        _RYML_CB_ASSERT(m_stack.m_callbacks, directive.begins_with('%'));
        {
            if(directive.begins_with("%TAG"))
            {
                const size_t pos = _ev_num_tag_directives();
                _RYML_CB_CHECK(m_stack.m_callbacks, pos < RYML_MAX_TAG_DIRECTIVES);
                _RYML_CB_CHECK(m_stack.m_callbacks, m_ev_tag_directives[pos].create_from_str(directive));
            }
        }
    }

    /** @} */

public:

    /** @name arena functions */
    /** @{ */

    substr alloc_arena(size_t len)
    {
        {
            const size_t curr = m_ev_arena.size();
            m_ev_arena.resize(curr + len);
            return to_substr(m_ev_arena).sub(curr);
        }
    }

    /** @} */

#undef _enable_
#undef _disable_
#undef _has_any_
};

} // namespace yml
} // namespace c4

#endif /* _C4_YML_EVENT_HANDLER_YAMLSTD_HPP_ */
