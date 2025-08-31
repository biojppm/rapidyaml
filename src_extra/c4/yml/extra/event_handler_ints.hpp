#pragma once
#ifndef _C4_YML_EXTRA_EVENT_HANDLER_INTS_HPP_
#define _C4_YML_EXTRA_EVENT_HANDLER_INTS_HPP_

#ifdef RYML_SINGLE_HEADER
#include <ryml_all.hpp>
#else
#ifndef _C4_YML_NODE_TYPE_HPP_
#include <c4/yml/node_type.hpp>
#endif
#ifndef _C4_YML_EVENT_HANDLER_STACK_HPP_
#include "c4/yml/event_handler_stack.hpp"
#endif
#ifndef _C4_YML_TAG_HPP_
#include <c4/yml/tag.hpp>
#endif
#ifndef _C4_YML_DETAIL_PARSER_DBG_HPP_
#include <c4/yml/detail/parser_dbg.hpp>
#endif
#endif

#ifndef _C4_YML_EXTRA_STRING_HPP_
#include "c4/yml/extra/string.hpp"
#endif


C4_SUPPRESS_WARNING_GCC_CLANG_PUSH
C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")
C4_SUPPRESS_WARNING_GCC("-Wuseless-cast")
// NOLINTBEGIN(hicpp-signed-bitwise)


namespace c4 {
namespace yml {
namespace extra {
namespace ievt {
using DataType = int32_t;
typedef enum : DataType {
    // Event types
    BSTR = (1 <<  0),  ///< +STR
    ESTR = (1 <<  1),  ///< -STR
    BDOC = (1 <<  2),  ///< +DOC
    EDOC = (1 <<  3),  ///< -DOC
    BMAP = (1 <<  4),  ///< +MAP
    EMAP = (1 <<  5),  ///< -MAP
    BSEQ = (1 <<  6),  ///< +SEQ
    ESEQ = (1 <<  7),  ///< -SEQ
    SCLR = (1 <<  8),  ///< =VAL
    ALIA = (1 <<  9),  ///< =ALI
    // Style flags
    PLAI = (1 << 10),  ///< : (plain scalar)
    SQUO = (1 << 11),  ///< ' (single-quoted scalar)
    DQUO = (1 << 12),  ///< " (double-quoted scalar)
    LITL = (1 << 13),  ///< | (block literal scalar)
    FOLD = (1 << 14),  ///< > (block folded scalar)
    FLOW = (1 << 15),  ///< flow container: [] for seqs or {} for maps
    BLCK = (1 << 16),  ///< block container
    // Modifiers
    ANCH = (1 << 17),  ///< anchor
    TAG_ = (1 << 18),  ///< tag
    // Structure flags
    KEY_ = (1 << 19),  ///< as key
    VAL_ = (1 << 20),  ///< as value
    EXPL = (1 << 21),  ///< --- (with BDOC) or
                       ///< ... (with EDOC)
    /// special flag to enable look back in the event array. it
    /// signifies that the previous event has a string, meaning that
    /// the jump back to it event is 3 positions. without this flag it
    /// would be impossible to jump to the previous event
    PSTR = (1 << 22),
    // Utility flags
    LAST = PSTR,
    MASK = (LAST << 1) - 1,
    /// with string: the event has a string. the next two integers
    /// will provide respectively the string's offset and length
    WSTR = SCLR|ALIA|ANCH|TAG_,
} EventFlags;

} // namespace ievt
} // namespace extra
} // namespace yml
} // namespace c4


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

namespace c4 {
namespace yml {
namespace extra {

RYML_EXPORT int32_t estimate_num_events_ints(csubstr src);

} // namespace extra
} // namespace yml
} // namespace c4


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

namespace c4 {
namespace yml {
namespace extra {

struct EventHandlerIntsState : public c4::yml::ParserState
{
    c4::yml::type_bits evt_type;
    int32_t evt_id;
};


struct EventHandlerInts : public c4::yml::EventHandlerStack<EventHandlerInts, EventHandlerIntsState>
{

    /** @name types
     * @{ */

    using value_type = ievt::DataType;
    using state = EventHandlerIntsState; // our internal state must inherit from parser state

    /** @} */

public:

    /** @cond dev */
    csubstr m_str;
    ievt::DataType * m_evt;
    int32_t m_evt_curr;
    int32_t m_evt_prev;
    int32_t m_evt_size;
    extra::string m_arena;
    TagDirective m_tag_directives[RYML_MAX_TAG_DIRECTIVES];
    bool m_has_yaml_directive;
    bool m_has_docs;

    #ifdef RYML_DBG
    mutable char m_evtstrbuf0[100];
    mutable char m_evtstrbuf1[100];
    #endif

    // undefined at the end
    #define _enable_(bits) _enable__<bits>()
    #define _disable_(bits) _disable__<bits>()
    #define _has_any_(bits) _has_any__<bits>()
    /** @endcond */

public:

    /** @name construction and resetting
     * @{ */

    EventHandlerInts(c4::yml::Callbacks const& cb)
        : EventHandlerStack(cb)
    {
        reset({}, nullptr, 0);
    }
    EventHandlerInts()
        : EventHandlerInts(c4::yml::get_callbacks())
    {
    }

    void reset(csubstr str, ievt::DataType *dst, int32_t dst_size)
    {
        _stack_reset_root();
        m_curr->flags |= c4::yml::RUNK|c4::yml::RTOP;
        m_curr->evt_type = {};
        m_curr->evt_id = 0;
        m_arena.clear();
        m_str = str;
        m_evt = dst;
        m_evt_size = dst_size;
        m_evt_curr = 0;
        m_evt_prev = 0;
        m_has_docs = 0;
        m_has_yaml_directive = false;
        for(TagDirective &td : m_tag_directives)
            td = {};
    }

    void reserve(int arena_size)
    {
        m_arena.reserve((size_t)arena_size);
    }

    /** @} */

public:

    /** @name parse events
     * @{ */

    void start_parse(const char* filename, c4::yml::detail::pfn_relocate_arena relocate_arena, void *relocate_arena_data)
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
    }

    /** @} */

public:

    /** @name YAML stream events */
    /** @{ */

    void begin_stream()
    {
        _send_flag_only_(ievt::BSTR);
    }

    void end_stream()
    {
        _send_flag_only_(ievt::ESTR);
    }

    /** @} */

public:

    /** @name YAML document events */
    /** @{ */

    /** implicit doc start (without ---) */
    void begin_doc()
    {
        _c4dbgpf("{}/{}: begin_doc", m_evt_curr, m_evt_size);
        _send_flag_only_(ievt::BDOC);
        if(_stack_should_push_on_begin_doc())
        {
            _c4dbgp("push!");
            _push();
        }
        m_has_docs = true;
    }
    /** implicit doc end (without ...) */
    void end_doc()
    {
        _c4dbgpf("{}/{}: end_doc", m_evt_curr, m_evt_size);
        _send_flag_only_(ievt::EDOC);
        if(_stack_should_pop_on_end_doc())
        {
            _c4dbgp("pop!");
            _pop();
        }
    }

    /** explicit doc start, with --- */
    void begin_doc_expl()
    {
        _c4dbgpf("{}/{}: begin_doc_expl", m_evt_curr, m_evt_size);
        _send_flag_only_(ievt::BDOC|ievt::EXPL);
        if(_stack_should_push_on_begin_doc())
        {
            _c4dbgp("push!");
            _push();
        }
        m_has_docs = true;
    }
    /** explicit doc end, with ... */
    void end_doc_expl()
    {
        _c4dbgpf("{}/{}: end_doc_expl", m_evt_curr, m_evt_size);
        _send_flag_only_(ievt::EDOC|ievt::EXPL);
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
        _c4dbgpf("{}/{}: bmap key flow", m_evt_curr, m_evt_size);
        _send_flag_only_(ievt::KEY_|ievt::BMAP|ievt::FLOW);
        _mark_parent_with_children_();
        _enable_(c4::yml::KEY|c4::yml::MAP|c4::yml::FLOW_SL);
        _push();
    }
    void begin_map_key_block()
    {
        _c4dbgpf("{}/{}: bmap key block", m_evt_curr, m_evt_size);
        _send_flag_only_(ievt::KEY_|ievt::BMAP|ievt::BLCK);
        _mark_parent_with_children_();
        _enable_(c4::yml::KEY|c4::yml::MAP|c4::yml::BLOCK);
        _push();
    }

    void begin_map_val_flow()
    {
        _c4dbgpf("{}/{}: bmap flow", m_evt_curr, m_evt_size);
        _send_flag_only_(ievt::VAL_|ievt::BMAP|ievt::FLOW);
        _mark_parent_with_children_();
        _enable_(c4::yml::MAP|c4::yml::FLOW_SL);
        _push();
    }
    void begin_map_val_block()
    {
        _c4dbgpf("{}/{}: bmap block", m_evt_curr, m_evt_size);
        _send_flag_only_(ievt::VAL_|ievt::BMAP|ievt::BLCK);
        _mark_parent_with_children_();
        _enable_(c4::yml::MAP|c4::yml::BLOCK);
        _push();
    }

    void end_map()
    {
        _pop();
        _send_flag_only_(ievt::EMAP);
    }

    /** @} */

public:

    /** @name YAML seq events */
    /** @{ */

    void begin_seq_key_flow()
    {
        _c4dbgpf("{}/{}: bseq key flow", m_evt_curr, m_evt_size);
        _send_flag_only_(ievt::KEY_|ievt::BSEQ|ievt::FLOW);
        _mark_parent_with_children_();
        _enable_(c4::yml::KEY|c4::yml::SEQ|c4::yml::FLOW_SL);
        _push();
    }
    void begin_seq_key_block()
    {
        _c4dbgpf("{}/{}: bseq key block", m_evt_curr, m_evt_size);
        _send_flag_only_(ievt::KEY_|ievt::BSEQ|ievt::BLCK);
        _mark_parent_with_children_();
        _enable_(c4::yml::KEY|c4::yml::SEQ|c4::yml::BLOCK);
        _push();
    }

    void begin_seq_val_flow()
    {
        _c4dbgpf("{}/{}: bseq flow", m_evt_curr, m_evt_size);
        _send_flag_only_(ievt::VAL_|ievt::BSEQ|ievt::FLOW);
        _mark_parent_with_children_();
        _enable_(c4::yml::SEQ|c4::yml::FLOW_SL);
        _push();
    }
    void begin_seq_val_block()
    {
        _c4dbgpf("{}/{}: bseq block", m_evt_curr, m_evt_size);
        _send_flag_only_(ievt::VAL_|ievt::BSEQ|ievt::BLCK);
        _mark_parent_with_children_();
        _enable_(c4::yml::SEQ|c4::yml::BLOCK);
        _push();
    }

    void end_seq()
    {
        _pop();
        _send_flag_only_(ievt::ESEQ);
    }

    /** @} */

public:

    /** @name YAML structure events */
    /** @{ */

    void add_sibling()
    {
        _RYML_CB_ASSERT(m_stack.m_callbacks, m_parent);
        m_curr->evt_type = {};
    }

    /** set the previous val as the first key of a new map, with flow style.
     *
     * See the documentation for @ref doc_event_handlers, which has
     * important notes about this event.
     */
    void actually_val_is_first_key_of_new_map_flow()
    {
        _c4dbgpf("{}/{}: prev={} actually_val_is_first_key_of_new_map_flow", m_evt_curr, m_evt_size, m_evt_prev);
        _RYML_CB_ASSERT(m_stack.m_callbacks, m_evt_curr > 2);
        _RYML_CB_ASSERT(m_stack.m_callbacks, m_evt_prev > 0);
        // BEFORE
        // ... flag start len (free)
        //     |              |
        //     prev           curr
        // AFTER
        // ... BMAP flag start len (free)
        //          |              |
        //          prev           curr
        if(m_evt_prev + 1 < m_evt_size)
        {
            if(m_evt[m_evt_prev] & ievt::WSTR)
            {
                _c4dbgpf("{}/{}: WSTR", m_evt_curr, m_evt_size);
                _RYML_CB_ASSERT(m_stack.m_callbacks, m_evt_prev > 0);
                int32_t pos = _extend_left_to_include_tag_and_or_anchor(m_evt_prev);
                if(m_evt_curr + 1 < m_evt_size)
                {
                    for(int32_t i = pos; i <= m_evt_prev; i = _next(i))
                    {
                        m_evt[i] |= ievt::KEY_;
                        m_evt[i] &= ~ievt::VAL_;
                    }
                    int32_t num_move = m_evt_curr + 1 - pos;
                    _RYML_CB_ASSERT(m_stack.m_callbacks, num_move > 0);
                    memmove(m_evt + pos + 1, m_evt + pos, (size_t)num_move * sizeof(ievt::DataType));
                }
                m_evt[pos] = ievt::BMAP|ievt::FLOW|ievt::VAL_;
                // move PSTR to prev
                if(m_evt[pos + 1] & ievt::PSTR)
                {
                    m_evt[pos    ] |= ievt::PSTR;
                    m_evt[pos + 1] &= ~ievt::PSTR;
                }
            }
            else
            {
                _c4dbgpf("{}/{}: container key", m_evt_curr, m_evt_size);
                _RYML_CB_ASSERT(m_stack.m_callbacks, (m_evt[m_evt_prev] & (ievt::EMAP|ievt::ESEQ)));
                int32_t pos;
                _c4dbgpf("{}/{}: find matching open for {}", m_evt_curr, m_evt_size, m_evt_prev);
                if(m_evt[m_evt_prev] & ievt::EMAP)
                {
                    pos = _find_matching_open(ievt::BMAP, ievt::EMAP, m_evt_prev);
                }
                else
                {
                    _RYML_CB_ASSERT(m_stack.m_callbacks, (m_evt[m_evt_prev] & ievt::ESEQ));
                    pos = _find_matching_open(ievt::BSEQ, ievt::ESEQ, m_evt_prev);
                }
                _c4dbgpf("{}/{}: matching open for {}={}", m_evt_curr, m_evt_size, m_evt_prev, pos);
                _RYML_CB_CHECK(m_stack.m_callbacks, pos >= 0); // internal error
                _RYML_CB_CHECK(m_stack.m_callbacks, pos < m_evt_prev); // internal error
                _RYML_CB_ASSERT(m_stack.m_callbacks, (m_evt[pos] & ievt::ESEQ) == (m_evt[m_evt_prev] & ievt::BSEQ));
                _RYML_CB_ASSERT(m_stack.m_callbacks, (m_evt[pos] & ievt::EMAP) == (m_evt[m_evt_prev] & ievt::BMAP));
                // shift the array one position to the right, starting at pos
                int32_t posp1 = pos + 1;
                if(m_evt_curr + 1 < m_evt_size)
                {
                    int32_t num_move = m_evt_curr + 1 - pos;
                    _RYML_CB_ASSERT(m_stack.m_callbacks, num_move > 0);
                    memmove(m_evt + posp1, m_evt + pos, (size_t)num_move * sizeof(ievt::DataType));
                }
                _RYML_CB_ASSERT(m_stack.m_callbacks, posp1 < m_evt_curr);
                // start the map
                m_evt[pos] = ievt::BMAP|ievt::FLOW|ievt::VAL_;
                // set next as key, not val
                m_evt[posp1] |= ievt::KEY_;
                m_evt[posp1] &= ~ievt::VAL_;
                // move PSTR to pos
                if(m_evt[posp1] & ievt::PSTR)
                {
                    m_evt[pos] |= ievt::PSTR;
                    m_evt[posp1] &= ~ievt::PSTR;
                }
            }
        }
        m_curr->evt_id = m_evt_curr - 2;
        ++m_evt_prev;
        ++m_evt_curr;
        _enable_(c4::yml::MAP|c4::yml::FLOW_SL);
        _push();
    }
    int32_t _find_matching_open(ievt::DataType open, ievt::DataType close, int32_t pos) const
    {
        _c4dbgpf("find_matching: start at {}", pos);
        _RYML_CB_ASSERT(m_stack.m_callbacks, (m_evt[pos] & close));
        _RYML_CB_ASSERT(m_stack.m_callbacks, !(m_evt[pos] & open));
        pos = _prev(pos); // don't count the starting close token
        uint32_t count = 0;
        while(pos >= 0)
        {
            ievt::DataType e = m_evt[pos];
            _c4dbgpf("find_matching: pos={} count={} e={}", pos, count, m_evt[pos]);
            if(e & close)
            {
                _c4dbgpf(".............: pos={} close! count={} e={}", pos, count, m_evt[pos]);
                ++count;
            }
            else if(e & open)
            {
                _c4dbgpf(".............: pos={} open! count={} e={}", pos, count, m_evt[pos]);
                if(!count)
                    return pos;
                else
                    --count;
            }
            pos = _prev(pos);
        }
        _c4dbgpf("find_matching: not found!", 0);
        return -1;
    }
    int32_t _extend_left_to_include_tag_and_or_anchor(int32_t pos) const
    {
        int32_t prev = _prev(pos);
        while((prev > 0) && (m_evt[prev] & (ievt::TAG_|ievt::ANCH)))
        {
            _c4dbgpf("{}/{}: {} is anchor/tag. extend to {}", m_evt_curr, m_evt_size, prev, prev);
            pos = prev;
            prev = _prev(prev);
        }
        return pos;
    }
    C4_ALWAYS_INLINE int32_t _next(int32_t pos) const noexcept
    {
        _RYML_CB_ASSERT(m_stack.m_callbacks, pos < m_evt_size);
        return pos + ((m_evt[pos] & ievt::WSTR) ? 3 : 1);
    }
    C4_ALWAYS_INLINE int32_t _prev(int32_t pos) const noexcept
    {
        _RYML_CB_ASSERT(m_stack.m_callbacks, pos < m_evt_size);
        return pos - ((m_evt[pos] & ievt::PSTR) ? 3 : 1);
    }

    /** like its flow counterpart, but this function can only be
     * called after the end of a flow-val at root or doc level.
     *
     * See the documentation for @ref doc_event_handlers, which has
     * important notes about this event.
     */
    void actually_val_is_first_key_of_new_map_block()
    {
        if(m_evt_prev < m_evt_size)
        {
            // interpolate BMAP|VAL|BLCK after the last BDOC
            int32_t pos = _find_last_bdoc(m_evt_curr);
            if(pos >= 0)
            {
                _RYML_CB_ASSERT(m_stack.m_callbacks, pos < m_evt_size);
                _RYML_CB_ASSERT(m_stack.m_callbacks, pos < m_evt_curr);
                _RYML_CB_ASSERT(m_stack.m_callbacks, (m_evt[pos] & ievt::BDOC));
                if(m_evt_curr < m_evt_size)
                {
                    ++pos; // add 1 to write after BDOC
                    int32_t num_move = m_evt_curr - pos;
                    int32_t posp1 = pos + 1;
                    _RYML_CB_ASSERT(m_stack.m_callbacks, (m_evt[pos] & (ievt::BSEQ|ievt::BMAP)));
                    _RYML_CB_ASSERT(m_stack.m_callbacks, num_move > 0);
                    memmove(m_evt + posp1, m_evt + pos, (size_t)num_move * sizeof(ievt::DataType));
                    m_evt[pos] = ievt::VAL_|ievt::BMAP|ievt::BLCK;
                    m_evt[posp1] &= ~ievt::VAL_;
                    m_evt[posp1] |= ievt::KEY_;
                    if(m_evt[posp1] & ievt::PSTR)
                    {
                        m_evt[pos] |= ievt::PSTR;
                        m_evt[posp1] &= ~ievt::PSTR;
                    }
                }
            }
        }
        ++m_curr->evt_id;
        ++m_evt_prev;
        ++m_evt_curr;
        _push();
    }
    int32_t _find_last_bdoc(int32_t pos) const
    {
        _RYML_CB_ASSERT(m_stack.m_callbacks, m_evt_prev < m_evt_size); // it's safe to read from the array
        while(pos >= 0)
        {
            ievt::DataType e = m_evt[pos];
            if(e & ievt::BDOC)
                return pos;
            pos -= (e & ievt::PSTR) ? 3 : 1;
        }
        return -1;
    }

    /** @} */

public:

    /** @name YAML scalar events */
    /** @{ */


    C4_ALWAYS_INLINE void set_key_scalar_plain_empty()
    {
        _c4dbgpf("{}/{}: set_key_scalar_plain_empty", m_evt_curr, m_evt_size);
        _send_key_scalar_(_get_latest_empty_scalar(), ievt::PLAI);
        _enable_(c4::yml::KEY|c4::yml::KEY_PLAIN|c4::yml::KEYNIL);
    }
    C4_ALWAYS_INLINE void set_val_scalar_plain_empty()
    {
        _c4dbgpf("{}/{}: set_val_scalar_plain_empty", m_evt_curr, m_evt_size);
        _send_val_scalar_(_get_latest_empty_scalar(), ievt::PLAI);
        _enable_(c4::yml::VAL|c4::yml::VAL_PLAIN|c4::yml::VALNIL);
    }
    C4_ALWAYS_INLINE csubstr _get_latest_empty_scalar() const
    {
        // ideally we should search back in the latest event that has
        // a scalar, then select a zero-length scalar immediately
        // after that scalar. But this also works for now:
        return m_str.first(0);
    }


    C4_ALWAYS_INLINE void set_key_scalar_plain(csubstr scalar)
    {
        _c4dbgpf("{}/{}: set_key_scalar_plain: @{} [{}]~~~{}~~~", m_evt_curr, m_evt_size, scalar.str-m_str.str, scalar.len, scalar);
        _send_key_scalar_(scalar, ievt::PLAI);
        _enable_(c4::yml::KEY|c4::yml::KEY_PLAIN);
    }
    C4_ALWAYS_INLINE void set_val_scalar_plain(csubstr scalar)
    {
        _c4dbgpf("{}/{}: set_val_scalar_plain: @{} [{}]~~~{}~~~", m_evt_curr, m_evt_size, scalar.str-m_str.str, scalar.len, scalar);
        _send_val_scalar_(scalar, ievt::PLAI);
        _enable_(c4::yml::VAL|c4::yml::VAL_PLAIN);
    }


    C4_ALWAYS_INLINE void set_key_scalar_dquoted(csubstr scalar)
    {
        _c4dbgpf("{}/{}: set_key_scalar_dquo: @{} [{}]~~~{}~~~", m_evt_curr, m_evt_size, scalar.str-m_str.str, scalar.len, scalar);
        _send_key_scalar_(scalar, ievt::DQUO);
        _enable_(c4::yml::KEY|c4::yml::KEY_DQUO);
    }
    C4_ALWAYS_INLINE void set_val_scalar_dquoted(csubstr scalar)
    {
        _c4dbgpf("{}/{}: set_val_scalar_dquo: @{} [{}]~~~{}~~~", m_evt_curr, m_evt_size, scalar.str-m_str.str, scalar.len, scalar);
        _send_val_scalar_(scalar, ievt::DQUO);
        _enable_(c4::yml::VAL|c4::yml::VAL_DQUO);
    }


    C4_ALWAYS_INLINE void set_key_scalar_squoted(csubstr scalar)
    {
        _c4dbgpf("{}/{}: set_key_scalar_squo: @{} [{}]~~~{}~~~", m_evt_curr, m_evt_size, scalar.str-m_str.str, scalar.len, scalar);
        _send_key_scalar_(scalar, ievt::SQUO);
        _enable_(c4::yml::KEY|c4::yml::KEY_SQUO);
    }
    C4_ALWAYS_INLINE void set_val_scalar_squoted(csubstr scalar)
    {
        _c4dbgpf("{}/{}: set_val_scalar_squo: @{} [{}]~~~{}~~~", m_evt_curr, m_evt_size, scalar.str-m_str.str, scalar.len, scalar);
        _send_val_scalar_(scalar, ievt::SQUO);
        _enable_(c4::yml::VAL|c4::yml::VAL_SQUO);
    }


    C4_ALWAYS_INLINE void set_key_scalar_literal(csubstr scalar)
    {
        _c4dbgpf("{}/{}: set_key_scalar_literal: @{} [{}]~~~{}~~~", m_evt_curr, m_evt_size, scalar.str-m_str.str, scalar.len, scalar);
        _send_key_scalar_(scalar, ievt::LITL);
        _enable_(c4::yml::KEY|c4::yml::KEY_LITERAL);
    }
    C4_ALWAYS_INLINE void set_val_scalar_literal(csubstr scalar)
    {
        _c4dbgpf("{}/{}: set_val_scalar_literal: @{} [{}]~~~{}~~~", m_evt_curr, m_evt_size, scalar.str-m_str.str, scalar.len, scalar);
        _send_val_scalar_(scalar, ievt::LITL);
        _enable_(c4::yml::VAL|c4::yml::VAL_LITERAL);
    }


    C4_ALWAYS_INLINE void set_key_scalar_folded(csubstr scalar)
    {
        _c4dbgpf("{}/{}: set_key_scalar_folded: @{} [{}]~~~{}~~~", m_evt_curr, m_evt_size, scalar.str-m_str.str, scalar.len, scalar);
        _send_key_scalar_(scalar, ievt::FOLD);
        _enable_(c4::yml::KEY|c4::yml::KEY_FOLDED);
    }
    C4_ALWAYS_INLINE void set_val_scalar_folded(csubstr scalar)
    {
        _c4dbgpf("{}/{}: set_val_scalar_folded: @{} [{}]~~~{}~~~", m_evt_curr, m_evt_size, scalar.str-m_str.str, scalar.len, scalar);
        _send_val_scalar_(scalar, ievt::FOLD);
        _enable_(c4::yml::VAL|c4::yml::VAL_FOLDED);
    }


    C4_ALWAYS_INLINE void mark_key_scalar_unfiltered()
    {
        _RYML_CB_ERR(m_stack.m_callbacks, "all scalars must be filtered");
    }
    C4_ALWAYS_INLINE void mark_val_scalar_unfiltered()
    {
        _RYML_CB_ERR(m_stack.m_callbacks, "all scalars must be filtered");
    }

    /** @} */

public:

#define _add_scalar_(i, scalar)                                     \
    _c4dbgpf("{}/{}: scalar!", i, m_evt_size);                      \
    _RYML_CB_ASSERT(m_stack.m_callbacks, scalar.is_sub(m_str));     \
    _RYML_CB_ASSERT(m_stack.m_callbacks, m_evt[i] & ievt::WSTR);    \
    _RYML_CB_ASSERT(m_stack.m_callbacks, i + 3 < m_evt_size);       \
    m_evt[i + 1] = (ievt::DataType)(scalar.str - m_str.str);        \
    m_evt[i + 2] = (ievt::DataType)scalar.len;                      \
    m_evt[i + 3] = ievt::PSTR

    /** @name YAML anchor/reference events */
    /** @{ */

    void set_key_anchor(csubstr anchor)
    {
        _c4dbgpf("{}/{}: set_key_anchor", m_evt_curr, m_evt_size);
        if(C4_UNLIKELY(_has_any_(KEYANCH)))
            _RYML_CB_ERR_(m_stack.m_callbacks, "key cannot have both anchor and ref", m_curr->pos);
        _enable_(c4::yml::KEYANCH);
        if(m_evt_curr + 3 < m_evt_size)
        {
            m_evt[m_evt_curr] |= ievt::KEY_|ievt::ANCH;
            _add_scalar_(m_evt_curr, anchor);
        }
        m_evt_prev = m_evt_curr;
        m_evt_curr += 3;
    }
    void set_val_anchor(csubstr anchor)
    {
        _c4dbgpf("{}/{}: set_val_anchor", m_evt_curr, m_evt_size);
        if(C4_UNLIKELY(_has_any_(VALREF)))
            _RYML_CB_ERR_(m_stack.m_callbacks, "val cannot have both anchor and ref", m_curr->pos);
        _enable_(c4::yml::VALANCH);
        if(m_evt_curr + 3 < m_evt_size)
        {
            m_evt[m_evt_curr] |= ievt::VAL_|ievt::ANCH;
            _add_scalar_(m_evt_curr, anchor);
        }
        m_evt_prev = m_evt_curr;
        m_evt_curr += 3;
    }

    void set_key_ref(csubstr ref)
    {
        _RYML_CB_ASSERT(m_stack.m_callbacks, ref.begins_with('*'));
        if(C4_UNLIKELY(_has_any_(KEYANCH)))
            _RYML_CB_ERR_(m_stack.m_callbacks, "key cannot have both anchor and ref", m_curr->pos);
        _enable_(c4::yml::KEY|c4::yml::KEYREF);
        _send_str_(ref.sub(1), ievt::KEY_|ievt::ALIA); // skip the leading *
    }
    void set_val_ref(csubstr ref)
    {
        _RYML_CB_ASSERT(m_stack.m_callbacks, ref.begins_with('*'));
        if(C4_UNLIKELY(_has_any_(VALANCH)))
            _RYML_CB_ERR_(m_stack.m_callbacks, "val cannot have both anchor and ref", m_curr->pos);
        _enable_(c4::yml::VAL|c4::yml::VALREF);
        _send_str_(ref.sub(1), ievt::VAL_|ievt::ALIA); // skip the leading *
    }

    /** @} */

public:

    /** @name YAML tag events */
    /** @{ */

    void set_key_tag(csubstr tag)
    {
        _c4dbgpf("{}/{}: set key tag ~~~{}~~~", m_evt_curr, m_evt_size, tag);
        _enable_(c4::yml::KEYTAG);
        _set_tag(tag, ievt::KEY_);
    }
    void set_val_tag(csubstr tag)
    {
        _c4dbgpf("{}/{}: set val tag ~~~{}~~~", m_evt_curr, m_evt_size, tag);
        _enable_(c4::yml::VALTAG);
        _set_tag(tag, ievt::VAL_);
    }
    void _set_tag(csubstr tag, ievt::DataType which)
    {
        csubstr ttag = _transform_directive(tag);
        _RYML_CB_ASSERT(m_stack.m_callbacks, !ttag.empty());
        if(ttag.begins_with('!') && !ttag.begins_with("!!"))
            ttag = ttag.sub(1);
        if(m_evt_curr + 3 < m_evt_size)
        {
            m_evt[m_evt_curr] |= which|ievt::TAG_;
            _add_scalar_(m_evt_curr, ttag);
        }
        m_evt_prev = m_evt_curr;
        m_evt_curr += 3;
    }

    /** @} */

public:

    /** @name YAML directive events */
    /** @{ */

    void add_directive(csubstr directive)
    {
        _c4dbgpf("{}/{}: add directive ~~~{}~~~", m_evt_curr, m_evt_size, directive);
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

    /** push a new parent, add a child to the new parent, and set the
     * child as the current node */
    void _push()
    {
        _stack_push();
        m_curr->evt_type = {};
    }

    /** end the current scope */
    void _pop()
    {
        _stack_pop();
    }

    template<c4::yml::type_bits bits> C4_ALWAYS_INLINE void _enable__() noexcept
    {
        m_curr->evt_type |= bits;
    }
    template<c4::yml::type_bits bits> C4_ALWAYS_INLINE void _disable__() noexcept
    {
        m_curr->evt_type &= ~bits;
    }
    template<c4::yml::type_bits bits> C4_ALWAYS_INLINE bool _has_any__() const noexcept
    {
        return (m_curr->evt_type & bits) != c4::yml::type_bits(0);
    }

    void _mark_parent_with_children_()
    {
        if(m_parent)
            m_parent->has_children = true;
    }

    C4_ALWAYS_INLINE void _send_flag_only_(ievt::DataType flags)
    {
        _c4dbgpf("{}/{}: flag only", m_evt_curr, m_evt_size);
        if(m_evt_curr < m_evt_size)
            m_evt[m_evt_curr] |= flags;
        m_curr->evt_id = m_evt_curr;
        m_evt_prev = m_evt_curr;
        ++m_evt_curr;
        if(m_evt_curr < m_evt_size)
            m_evt[m_evt_curr] = {};
    }

    C4_ALWAYS_INLINE void _send_key_scalar_(csubstr scalar, ievt::DataType flags)
    {
        _c4dbgpf("{}/{}: key scalar", m_evt_curr, m_evt_size);
        if(m_evt_curr + 3 < m_evt_size)
        {
            m_evt[m_evt_curr] |= ievt::SCLR|ievt::KEY_|flags;
            _add_scalar_(m_evt_curr, scalar);
        }
        m_curr->evt_id = m_evt_curr;
        m_evt_prev = m_evt_curr;
        m_evt_curr += 3;
    }

    C4_ALWAYS_INLINE void _send_val_scalar_(csubstr scalar, ievt::DataType flags)
    {
        _c4dbgpf("{}/{}: val scalar", m_evt_curr, m_evt_size);
        if(m_evt_curr + 3 < m_evt_size)
        {
            m_evt[m_evt_curr] |= ievt::SCLR|ievt::VAL_|flags;
            _add_scalar_(m_evt_curr, scalar);
        }
        m_curr->evt_id = m_evt_curr;
        m_evt_prev = m_evt_curr;
        m_evt_curr += 3;
    }

    C4_ALWAYS_INLINE void _send_str_(csubstr scalar, ievt::DataType flags)
    {
        _c4dbgpf("{}/{}: send str", m_evt_curr, m_evt_size);
        if(m_evt_curr + 3 < m_evt_size)
        {
            m_evt[m_evt_curr] |= flags;
            _add_scalar_(m_evt_curr, scalar);
        }
        m_curr->evt_id = m_evt_curr;
        m_evt_prev = m_evt_curr;
        m_evt_curr += 3;
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
    csubstr _transform_directive(csubstr tag)
    {
        if(tag.begins_with('!'))
            return tag;
        csubstr result = c4::yml::normalize_tag_long(tag);
        _RYML_CB_ASSERT(m_stack.m_callbacks, result.is_sub(tag));
        _RYML_CB_CHECK(m_stack.m_callbacks, result.len > 0);
        _RYML_CB_CHECK(m_stack.m_callbacks, result.str);
        return result;
    }
#undef _enable_
#undef _disable_
#undef _has_any_
#undef _add_scalar_

};

} // namespace extra
} // namespace yml
} // namespace c4


// NOLINTEND(hicpp-signed-bitwise)
C4_SUPPRESS_WARNING_GCC_CLANG_POP

#endif /* _C4_YML_EXTRA_EVENT_HANDLER_INTS_HPP_ */
