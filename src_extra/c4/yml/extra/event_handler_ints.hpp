#ifndef C4_YML_EXTRA_EVENT_HANDLER_INTS_HPP_
#define C4_YML_EXTRA_EVENT_HANDLER_INTS_HPP_

/** @file event_handler_ints.hpp */

#ifndef RYML_SINGLE_HEADER
#ifndef C4_YML_NODE_TYPE_HPP_
#include <c4/yml/node_type.hpp>
#endif
#ifndef C4_YML_EVENT_HANDLER_STACK_HPP_
#include "c4/yml/event_handler_stack.hpp"
#endif
#ifndef C4_YML_TAG_HPP_
#include <c4/yml/tag.hpp>
#endif
#ifndef C4_YML_DETAIL_DBGPRINT_HPP_
#include <c4/yml/detail/dbgprint.hpp>
#endif
#ifndef C4_YML_EXTRA_EVENT_INTS_HPP_
#include "c4/yml/extra/event_ints.hpp"
#endif
#endif

// NOLINTBEGIN(hicpp-signed-bitwise,*avoid-c-style-cast)
C4_SUPPRESS_WARNING_GCC_CLANG_PUSH
C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")
C4_SUPPRESS_WARNING_GCC("-Wuseless-cast")


namespace c4 {
namespace yml {
namespace extra {
namespace ievt {


/** @addtogroup doc_event_handlers_ints
 * @{ */


/** @cond dev */
struct EventHandlerIntsState : public c4::yml::ParserState
{
    c4::yml::type_bits evt_type;
    int32_t evt_id;
};
template<bool resize_buffers_>
struct EventHandlerInts;
/** @endcond */



/** A parser event handler that creates a compact representation of
 * the YAML tree in a contiguous buffer of integers. The integers are
 * @ref ievt::EventBits containing masks (to represent events),
 * interleaved with offset+length (to represent strings in the source
 * buffer). For a description of the events, see @ref
 * doc_event_handlers_ints
 *
 * This handler must be initialized with the input source buffer, the
 * output arena, and the output event buffer.
 *
 */
using EventHandlerIntsResize = EventHandlerInts<true>;



/** A parser event handler that creates a compact representation of
 * the YAML tree in a contiguous buffer of integers. The integers are
 * @ref ievt::EventBits containing masks (to represent events),
 * interleaved with offset+length (to represent strings in the source
 * buffer). For a description of the events, see @ref doc_event_handlers_ints
 *
 * This handler must be initialized with the input source buffer, the
 * output arena, and the output event buffer.
 *
 * Unlike @ref EventHandlerIntsResize This handler will not take
 * ownership nor attempt to resize the output buffer. If the size
 * required for the output buffer or arena are larger than their
 * actual size, parsing goes all way to the end, determining the
 * required buffer sizes without writing anything past the end of the
 * respective buffer. After parsing is finished, the user must ensure
 * that the buffer size was enough to accomodate all the data that
 * needs to be written into it, or react accordingly (eg, throw an
 * error, or resize the buffer then retry the parse).
 *
 * A couple of functions will be helpful to do this. After parsing, @ref
 * EventHandlerInts::fits_buffers() must be used to verify that the
 * output buffers were enough to accomodate the results. Then, @ref
 * EventHandlerInts::required_size_events() and @ref
 * EventHandlerInts::required_size_arena() can be used to retrieve to
 * necessary information. To get an estimation of the number of events
 * before parsing, see @ref estimate_events_ints_size().
 *
 * Typical code to parse YAML with this handler will look like this:
 *
 * ```c++
 * csubstr filename = ...;
 * substr src = ...;
 * // estimate the size required for the events buffer,
 * // overpredicting it to be safe.
 * int estimated_size = extra::estimate_events_ints_size(src);
 * extra::ievt::EventHandlerIntsNoResize handler;
 * ParseEngine<extra::ievt::EventHandlerIntsNoResize> parser(&handler);
 * // example with a vector
 * std::vector<int> evts;
 * // ensure we have a fighting chance to acommodate the events
 * evts.resize((size_t)estimated_size);
 * // arena to place scalars/tags that may have been extended after filtering
 * std::vector<char> arena;
 * arena.resize(src.len); // this is generally enough
 * // initialize the handler
 * handler.reset(src, arena, evts.data(), (int)evts.size());
 * // parse the YAML
 * parser.parse_in_place_ev(filename, src);
 * if(handler.fits_buffers()) // were the buffer sizes enough?
 * {
 *      evts.resize((size_t)handler.required_size_events()); // trim the vector
 *      ...
 * }
 * else
 * {
 *      // event size estimation underpredicted, or arena is too small!
 *      // for the first case, open an issue at
 *      // https://github.com/biojppm/rapidyaml/issues
 *      error("buffer could not accomodate all the events");
 *      // NOTE: see below for notes on doing a parse retry.
 * }
 * ```
 *
 * The result of @ref estimate_events_ints_size() (click to see more
 * info) must be an overprediction: it overpredicts for every single
 * case among the many hundreds covered in the unit tests. This is
 * deliberate, and aims at ensuring that a retry parse is not
 * needed. But conceivably, it may underpredict in some instances not
 * found in the out tests. What to do then?
 *
 * First, [open an issue](https://github.com/biojppm/rapidyaml/issues) to
 * allow the estimation to be improved! Second, there are two ways to
 * handle this situation in code:
 *
 *   1) throw an error (as sketched above)
 *
 *   2) grow the buffer to the required size (see @ref
 *      EventHandlerInts::required_size_events()), and then parse
 *      again
 *
 * If your code must be able to handle any case including where the
 * prediction undershoots before the estimate function is fixed (after
 * you open the issue), that is, if you are considering a parse retry,
 * there is something important that needs attention. The YAML source
 * buffer is mutated in-place during the parse, and cannot be used to
 * parse again. So if you want to retry, you need to keep a pristine
 * copy of the source, and use it for the retry:
 *
 * ```c++
 * const std::string src = ...;  // the YAML code to be parsed
 * std::string parsed_src = src; // this is where we will parse (filter during parsing)
 * std::vector<int> evts((size_t)estimated_size); // ensure we have a fighting change to acommodate the events
 * std::vector<char> arena(src.size()); // ensure we have a fighting change to acommodate the events
 * ParseEngine<extra::EventHandlerIntsNoResize> parser(&handler);
 * handler.reset(to_substr(parsed_src), to_substr(arena), evts.data(), (int)evts.size());
 * parser.parse_in_place_ev(filename, to_substr(parsed_src));
 * if(handler.fits_buffers()) // were the buffer sizes enough?
 * {
 *      evts.resize((size_t)handler.required_size()); // trim the vector
 *      ...
 * }
 * else
 * {
 *      evts.resize((size_t)handler.required_size_events()); // buffer size was not enough.
 *      arena.resize(handler.required_size_arena()); // buffer size was not enough.
 *      // copy again
 *      parsed_src = src;
 *      // retry parse
 *      handler.reset(to_substr(parsed_src), to_substr(arena), evts.data(), (int)evts.size());
 *      parser.parse_in_place_ev(filename, to_substr(parsed_src));
 *      assert((size_t)handler.fits_buffers()); // must always be true
 * }
 * ```
 *
 * When bringing this to other programming languages, the semantics
 * will be very similar to this.
 */
using EventHandlerIntsNoResize = EventHandlerInts<false>;


//-----------------------------------------------------------------------------

template<bool resize_buffers_>
struct EventHandlerInts : public c4::yml::EventHandlerStack<EventHandlerInts<resize_buffers_>, EventHandlerIntsState>
{

    /** @name types
     * @{ */

    using value_type = ievt::evt_bits;
    using base_type = c4::yml::EventHandlerStack<EventHandlerInts<resize_buffers_>, EventHandlerIntsState>;
    using state = EventHandlerIntsState; // our internal state must inherit from parser state
    enum { requires_strings_on_buffers = true, resize_buffers = resize_buffers_ }; // NOLINT

    /** @} */

public:

    /** @cond dev */
    evtbuf m_evt;
    evt_size m_evt_prev;
    substr m_arena;
    size_t m_arena_pos;
    id_type m_curr_doc;
    TagDirectives m_tag_directives;
    TagCache m_tag_cache;

    // undefined at the end
    #define ryml_enable_(bits) enable_<bits>()
    #define ryml_disable_(bits) disable_<bits>()
    #define ryml_has_any_(bits) has_any_<bits>()
    /** @endcond */

public:

    /** @name construction and resetting
     * @{ */

    EventHandlerInts(c4::yml::Callbacks const& cb) noexcept
        : base_type(cb)
    {
        reset(substr{}, substr{}, nullptr, 0);
    }
    EventHandlerInts() noexcept
        : EventHandlerInts(c4::yml::get_callbacks())
    {
    }
    ~EventHandlerInts() noexcept
    {
        if C4_IF_CONSTEXPR (resize_buffers)
        {
            if(m_evt.ptr)
                base_type::m_stack.m_callbacks.m_free(m_evt.ptr, static_cast<size_t>(m_evt.cap) * sizeof(m_evt.ptr[0]), base_type::m_stack.m_callbacks.m_user_data);
            if(base_type::m_src.str)
                base_type::m_stack.m_callbacks.m_free(base_type::m_src.str, base_type::m_src.len * sizeof(base_type::m_src.str[0]), base_type::m_stack.m_callbacks.m_user_data);
            if(m_arena.str)
                base_type::m_stack.m_callbacks.m_free(m_arena.str, m_arena.len * sizeof(m_arena.str[0]), base_type::m_stack.m_callbacks.m_user_data);
        }
    }

public:

    void reset(substr str, substr arena, evtbuf evt)
    {
        base_type::_stack_reset_root();
        base_type::m_curr->flags |= c4::yml::RUNK|c4::yml::RTOP;
        base_type::m_curr->evt_type = {};
        base_type::m_curr->evt_id = 0;
        m_arena = arena;
        m_arena_pos = 0;
        base_type::m_src = str;
        m_evt = evt;
        m_evt.len = 0;
        m_evt_prev = 0;
        m_curr_doc = 0;
        m_tag_directives.clear();
        m_tag_cache.clear();
    }
    void reset(substr str, substr arena, evt_bits *dst, evt_size dst_cap)
    {
        evtbuf evt;
        evt.ptr = dst;
        evt.len = 0;
        evt.cap = dst_cap;
        reset(str, arena, evt);
    }
    void reset(ievt::Buffers const& buf)
    {
        reset(buf.src, buf.arena, buf.evts);
    }

public:

    ievt::Buffers get_buffers() const noexcept
    {
        RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, m_arena_pos <= m_arena.len);
        return get_buffers(false);
    }

    ievt::Buffers get_buffers(bool transfer_ownership) noexcept
    {
        RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, m_arena_pos <= m_arena.len);
        ievt::Buffers buf;
        get_buffers(&buf.src, &buf.arena, &buf.evts);
        buf.callbacks = base_type::m_stack.m_callbacks;
        buf.owned = false;
        (void)transfer_ownership;
        if C4_IF_CONSTEXPR (resize_buffers)
        {
            if(transfer_ownership)
            {
                base_type::m_src = {};
                m_arena = {};
                m_evt = {};
                buf.owned = true;
            }
        }
        return buf;
    }

    /** */
    void get_buffers(substr *str, substr *arena, evtbuf *buf) const noexcept
    {
        if C4_IF_CONSTEXPR (resize_buffers)
        {
            RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, m_arena_pos <= m_arena.len);
            RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, m_evt.len <= m_evt.cap);
        }
        *str = base_type::m_src;
        *arena = m_arena;
        *buf = m_evt;
    }

public:

    /** get the size needed for the event buffer from the previous parse
     * @warning this is valid only until the next parse */
    evt_size required_size_events() const
    {
        return m_evt.len;
    }

    /** get the size needed for the arena from the previous parse
     * @warning this is valid only until the next parse */
    size_t required_size_arena() const
    {
        return m_arena_pos;
    }

    /** Test if the event and arena buffers successfully
     * accomodated all the parse events.
     *
     * @warning this is valid only until the next parse */
    bool fits_buffers() const
    {
        return m_evt.len <= m_evt.cap && m_arena_pos <= m_arena.len;
    }

    void reserve_arena(size_t size)
    {
        if C4_IF_CONSTEXPR (resize_buffers)
            if(size > m_arena.len)
                _grow_arena(size - m_arena.len);
    }

    void reserve_evts(evt_size size)
    {
        if C4_IF_CONSTEXPR (resize_buffers)
            if((size_t)size > m_arena.len)
                _grow_evts(size);
    }

    C4_ALWAYS_INLINE TagDirectives &tag_directives() { return m_tag_directives; }
    C4_ALWAYS_INLINE TagCache &tag_cache() { return m_tag_cache; }

    /** @} */

public:

    /** @name parse events
     * @{ */

    void start_parse(const char* filename, substr src)
    {
        RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, src.str == base_type::m_src.str);
        RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, src.len == base_type::m_src.len);
        this->_stack_start_parse(filename, src);
    }

    void finish_parse()
    {
        this->_stack_finish_parse();
    }

    void cancel_parse()
    {
        while(base_type::m_stack.size() > 1)
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
        _c4dbgpf("{}/{}: begin_doc", m_evt.len, m_evt.cap);
        _send_flag_only_(ievt::BDOC);
        if(base_type::_stack_should_push_on_begin_doc())
        {
            _c4dbgp("push!");
            _push();
            ryml_enable_(DOC);
        }
    }
    /** implicit doc end (without ...) */
    void end_doc()
    {
        _c4dbgpf("{}/{}: end_doc", m_evt.len, m_evt.cap);
        _send_flag_only_(ievt::EDOC);
        if(base_type::_stack_should_pop_on_end_doc())
        {
            _c4dbgp("pop!");
            _pop();
        }
        ++m_curr_doc;
    }

    /** explicit doc start, with --- */
    void begin_doc_expl()
    {
        _c4dbgpf("{}/{}: begin_doc_expl", m_evt.len, m_evt.cap);
        _send_flag_only_(ievt::BDOC|ievt::EXPL);
        _c4dbgp("push!");
        _push();
        ryml_enable_(DOC);
    }
    /** explicit doc end, with ... */
    void end_doc_expl()
    {
        _c4dbgpf("{}/{}: end_doc_expl", m_evt.len, m_evt.cap);
        _send_flag_only_(ievt::EDOC|ievt::EXPL);
        if(base_type::_stack_should_pop_on_end_doc())
        {
            _c4dbgp("pop!");
            _pop();
        }
        ++m_curr_doc;
    }

    /** @} */

public:

    /** @name YAML map functions */
    /** @{ */

    void begin_map_key_flow()
    {
        _c4dbgpf("{}/{}: bmap key flow", m_evt.len, m_evt.cap);
        _send_flag_only_(ievt::KEY_|ievt::BMAP|ievt::FLOW);
        _mark_parent_with_children_();
        ryml_enable_(c4::yml::KEY|c4::yml::MAP|c4::yml::FLOW_SL);
        _push();
    }
    void begin_map_key_block()
    {
        _c4dbgpf("{}/{}: bmap key block", m_evt.len, m_evt.cap);
        _send_flag_only_(ievt::KEY_|ievt::BMAP|ievt::BLCK);
        _mark_parent_with_children_();
        ryml_enable_(c4::yml::KEY|c4::yml::MAP|c4::yml::BLOCK);
        _push();
    }

    void begin_map_val_flow()
    {
        _c4dbgpf("{}/{}: bmap flow", m_evt.len, m_evt.cap);
        _send_flag_only_(ievt::VAL_|ievt::BMAP|ievt::FLOW);
        _mark_parent_with_children_();
        ryml_enable_(c4::yml::MAP|c4::yml::FLOW_SL);
        _push();
    }
    void begin_map_val_block()
    {
        _c4dbgpf("{}/{}: bmap block", m_evt.len, m_evt.cap);
        _send_flag_only_(ievt::VAL_|ievt::BMAP|ievt::BLCK);
        _mark_parent_with_children_();
        ryml_enable_(c4::yml::MAP|c4::yml::BLOCK);
        _push();
    }

    void end_map_block()
    {
        _pop();
        _send_flag_only_(ievt::EMAP);
    }

    void end_map_flow(bool multiline, type_bits multiline_style=FLOW_ML1)
    {
        _pop();
        _c4dbgpf("{}/{}: emap flow multiline={} start={}", m_evt.len, m_evt.cap, multiline, base_type::m_curr->evt_id);
        if C4_IF_CONSTEXPR(resize_buffers)
        {
            RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, base_type::m_curr->evt_id < m_evt.cap);
            RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, (m_evt.ptr[base_type::m_curr->evt_id] & ievt::BMAP) == ievt::BMAP);
            m_evt.ptr[base_type::m_curr->evt_id] |= multiline ? translate_flowml_(multiline_style) : ievt::FSL_;
        }
        else
        {
            if(base_type::m_curr->evt_id < m_evt.cap)
            {
                RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, (m_evt.ptr[base_type::m_curr->evt_id] & ievt::BMAP) == ievt::BMAP);
                m_evt.ptr[base_type::m_curr->evt_id] |= multiline ? translate_flowml_(multiline_style) : ievt::FSL_;
            }
        }
        _send_flag_only_(ievt::EMAP);
    }

    /** @} */

public:

    /** @name YAML seq events */
    /** @{ */

    void begin_seq_key_flow()
    {
        _c4dbgpf("{}/{}: bseq key flow", m_evt.len, m_evt.cap);
        _send_flag_only_(ievt::KEY_|ievt::BSEQ|ievt::FLOW);
        _mark_parent_with_children_();
        ryml_enable_(c4::yml::KEY|c4::yml::SEQ|c4::yml::FLOW_SL);
        _push();
    }
    void begin_seq_key_block()
    {
        _c4dbgpf("{}/{}: bseq key block", m_evt.len, m_evt.cap);
        _send_flag_only_(ievt::KEY_|ievt::BSEQ|ievt::BLCK);
        _mark_parent_with_children_();
        ryml_enable_(c4::yml::KEY|c4::yml::SEQ|c4::yml::BLOCK);
        _push();
    }

    void begin_seq_val_flow()
    {
        _c4dbgpf("{}/{}: bseq flow", m_evt.len, m_evt.cap);
        _send_flag_only_(ievt::VAL_|ievt::BSEQ|ievt::FLOW);
        _mark_parent_with_children_();
        ryml_enable_(c4::yml::SEQ|c4::yml::FLOW_SL);
        _push();
    }
    void begin_seq_val_block()
    {
        _c4dbgpf("{}/{}: bseq block", m_evt.len, m_evt.cap);
        _send_flag_only_(ievt::VAL_|ievt::BSEQ|ievt::BLCK);
        _mark_parent_with_children_();
        ryml_enable_(c4::yml::SEQ|c4::yml::BLOCK);
        _push();
    }

    void end_seq_block()
    {
        _pop();
        _send_flag_only_(ievt::ESEQ);
    }

    void end_seq_flow(bool multiline, type_bits multiline_style=FLOW_ML1)
    {
        _pop();
        _c4dbgpf("{}/{}: eseq flow multiline={} start={}", m_evt.len, m_evt.cap, multiline, base_type::m_curr->evt_id);
        if C4_IF_CONSTEXPR(resize_buffers)
        {
            RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, base_type::m_curr->evt_id < m_evt.cap);
            RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, (m_evt.ptr[base_type::m_curr->evt_id] & ievt::BSEQ) == ievt::BSEQ);
            m_evt.ptr[base_type::m_curr->evt_id] |= multiline ? translate_flowml_(multiline_style) : ievt::FSL_;
        }
        else
        {
            if(base_type::m_curr->evt_id < m_evt.cap)
            {
                RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, (m_evt.ptr[base_type::m_curr->evt_id] & ievt::BSEQ) == ievt::BSEQ);
                m_evt.ptr[base_type::m_curr->evt_id] |= multiline ? translate_flowml_(multiline_style) : ievt::FSL_;
            }
        }
        _send_flag_only_(ievt::ESEQ);
    }

    /** @} */

public:

    /** @name YAML structure events */
    /** @{ */

    void add_sibling()
    {
        RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, base_type::m_parent);
        base_type::m_curr->evt_type = {};
    }

    /** @} */

public:

    /** @name YAML scalar events */
    /** @{ */


    C4_ALWAYS_INLINE void set_key_scalar_plain_empty()
    {
        _c4dbgpf("{}/{}: set_key_scalar_plain_empty", m_evt.len, m_evt.cap);
        _send_str_(_get_latest_empty_scalar(), ievt::KEY_|ievt::SCLR|ievt::PLAI);
        ryml_enable_(c4::yml::KEY|c4::yml::KEY_PLAIN|c4::yml::KEYNIL);
    }
    C4_ALWAYS_INLINE void set_val_scalar_plain_empty()
    {
        _c4dbgpf("{}/{}: set_val_scalar_plain_empty", m_evt.len, m_evt.cap);
        _send_str_(_get_latest_empty_scalar(), ievt::VAL_|ievt::SCLR|ievt::PLAI);
        ryml_enable_(c4::yml::VAL|c4::yml::VAL_PLAIN|c4::yml::VALNIL);
    }


    C4_ALWAYS_INLINE void set_key_scalar_plain(csubstr scalar)
    {
        _c4dbgpf("{}/{}: set_key_scalar_plain: @{} [{}]~~~{}~~~", m_evt.len, m_evt.cap, scalar.str-base_type::m_src.str, scalar.len, scalar);
        _send_str_(scalar, ievt::KEY_|ievt::SCLR|ievt::PLAI);
        ryml_enable_(c4::yml::KEY|c4::yml::KEY_PLAIN);
    }
    C4_ALWAYS_INLINE void set_val_scalar_plain(csubstr scalar)
    {
        _c4dbgpf("{}/{}: set_val_scalar_plain: @{} [{}]~~~{}~~~", m_evt.len, m_evt.cap, scalar.str-base_type::m_src.str, scalar.len, scalar);
        _send_str_(scalar, ievt::VAL_|ievt::SCLR|ievt::PLAI);
        ryml_enable_(c4::yml::VAL|c4::yml::VAL_PLAIN);
    }


    C4_ALWAYS_INLINE void set_key_scalar_dquoted(csubstr scalar)
    {
        _c4dbgpf("{}/{}: set_key_scalar_dquo: @{} [{}]~~~{}~~~", m_evt.len, m_evt.cap, scalar.str?size_t(scalar.str-base_type::m_src.str):base_type::m_src.len, scalar.len, scalar.str?scalar:csubstr{});
        _send_str_(scalar, ievt::KEY_|ievt::SCLR|ievt::DQUO);
        ryml_enable_(c4::yml::KEY|c4::yml::KEY_DQUO);
    }
    C4_ALWAYS_INLINE void set_val_scalar_dquoted(csubstr scalar)
    {
        _c4dbgpf("{}/{}: set_val_scalar_dquo: @{} [{}]~~~{}~~~", m_evt.len, m_evt.cap, scalar.str?size_t(scalar.str-base_type::m_src.str):base_type::m_src.len, scalar.len, scalar.str?scalar:csubstr{});
        _send_str_(scalar, ievt::VAL_|ievt::SCLR|ievt::DQUO);
        ryml_enable_(c4::yml::VAL|c4::yml::VAL_DQUO);
    }


    C4_ALWAYS_INLINE void set_key_scalar_squoted(csubstr scalar)
    {
        _c4dbgpf("{}/{}: set_key_scalar_squo: @{} [{}]~~~{}~~~", m_evt.len, m_evt.cap, scalar.str-base_type::m_src.str, scalar.len, scalar);
        _send_str_(scalar, ievt::KEY_|ievt::SCLR|ievt::SQUO);
        ryml_enable_(c4::yml::KEY|c4::yml::KEY_SQUO);
    }
    C4_ALWAYS_INLINE void set_val_scalar_squoted(csubstr scalar)
    {
        _c4dbgpf("{}/{}: set_val_scalar_squo: @{} [{}]~~~{}~~~", m_evt.len, m_evt.cap, scalar.str-base_type::m_src.str, scalar.len, scalar);
        _send_str_(scalar, ievt::VAL_|ievt::SCLR|ievt::SQUO);
        ryml_enable_(c4::yml::VAL|c4::yml::VAL_SQUO);
    }


    C4_ALWAYS_INLINE void set_key_scalar_literal(csubstr scalar)
    {
        _c4dbgpf("{}/{}: set_key_scalar_literal: @{} [{}]~~~{}~~~", m_evt.len, m_evt.cap, scalar.str?size_t(scalar.str-base_type::m_src.str):base_type::m_src.len, scalar.len, scalar.str?scalar:csubstr{});
        _send_str_(scalar, ievt::KEY_|ievt::SCLR|ievt::LITL);
        ryml_enable_(c4::yml::KEY|c4::yml::KEY_LITERAL);
    }
    C4_ALWAYS_INLINE void set_val_scalar_literal(csubstr scalar)
    {
        _c4dbgpf("{}/{}: set_val_scalar_literal: @{} [{}]~~~{}~~~", m_evt.len, m_evt.cap, scalar.str?size_t(scalar.str-base_type::m_src.str):base_type::m_src.len, scalar.len, scalar.str?scalar:csubstr{});
        _send_str_(scalar, ievt::VAL_|ievt::SCLR|ievt::LITL);
        ryml_enable_(c4::yml::VAL|c4::yml::VAL_LITERAL);
    }


    C4_ALWAYS_INLINE void set_key_scalar_folded(csubstr scalar)
    {
        _c4dbgpf("{}/{}: set_key_scalar_folded: @{} [{}]~~~{}~~~", m_evt.len, m_evt.cap, scalar.str?size_t(scalar.str-base_type::m_src.str):base_type::m_src.len, scalar.len, scalar.str?scalar:csubstr{});
        _send_str_(scalar, ievt::KEY_|ievt::SCLR|ievt::FOLD);
        ryml_enable_(c4::yml::KEY|c4::yml::KEY_FOLDED);
    }
    C4_ALWAYS_INLINE void set_val_scalar_folded(csubstr scalar)
    {
        _c4dbgpf("{}/{}: set_val_scalar_folded: @{} [{}]~~~{}~~~", m_evt.len, m_evt.cap, scalar.str?size_t(scalar.str-base_type::m_src.str):base_type::m_src.len, scalar.len, scalar.str?scalar:csubstr{});
        _send_str_(scalar, ievt::VAL_|ievt::SCLR|ievt::FOLD);
        ryml_enable_(c4::yml::VAL|c4::yml::VAL_FOLDED);
    }


    C4_ALWAYS_INLINE void mark_key_scalar_unfiltered() // NOLINT
    {
        _c4dbgpf("{}/{}: mark_key_scalar_unfiltered", m_evt.len, m_evt.cap);
        if C4_IF_CONSTEXPR (resize_buffers)
        {
            if(m_evt.len >= m_evt.cap)
                _grow_evts();
            m_evt.ptr[m_evt.len] |= ievt::UNFILT;
        }
        else
        {
            if(m_evt.len < m_evt.cap)
                m_evt.ptr[m_evt.len] |= ievt::UNFILT;
        }
    }
    C4_ALWAYS_INLINE void mark_val_scalar_unfiltered() // NOLINT
    {
        _c4dbgpf("{}/{}: mark_val_scalar_unfiltered", m_evt.len, m_evt.cap);
        if C4_IF_CONSTEXPR (resize_buffers)
        {
            if(m_evt.len >= m_evt.cap)
                _grow_evts();
            m_evt.ptr[m_evt.len] |= ievt::UNFILT;
        }
        else
        {
            if(m_evt.len < m_evt.cap)
                m_evt.ptr[m_evt.len] |= ievt::UNFILT;
        }
    }

    /** @} */

private:

    /** @cond dev*/
    #define _add_scalar_(i, scalar)                                     \
    _c4dbgpf("{}/{}: scalar!", i, m_evt.cap);                           \
    RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, _is_sub_(scalar)); \
    RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, m_evt.ptr[i] & ievt::WSTR); \
    RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, ((i) + 3) < m_evt.cap); \
    if C4_LIKELY((scalar).is_sub(base_type::m_src))                     \
    {                                                                   \
        m_evt.ptr[(i) + 1] = (evt_bits)((scalar).str - base_type::m_src.str); \
    }                                                                   \
    else                                                                \
    {                                                                   \
        m_evt.ptr[i] |= ievt::AREN;                                     \
        m_evt.ptr[(i) + 1] = (evt_bits)((scalar).str - m_arena.str);    \
        _c4dbgpf("{}/{}: arena! ->{}", i, m_evt.cap, m_evt.ptr[(i)+1]); \
    }                                                                   \
    m_evt.ptr[(i) + 2] = (evt_bits)(scalar).len;                        \
    m_evt.ptr[(i) + 3] = ievt::PSTR
    /** @endcond */

public:

    /** @name YAML anchor/reference events */
    /** @{ */

    void set_key_anchor(csubstr anchor)
    {
        _c4dbgpf("{}/{}: set_key_anchor: {}", m_evt.len, m_evt.cap, anchor);
        RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, !ryml_has_any_(KEYREF));
        ryml_enable_(c4::yml::KEYANCH);
        if(m_evt.len + 3 < m_evt.cap)
        {
            m_evt.ptr[m_evt.len] |= ievt::KEY_|ievt::ANCH;
            _add_scalar_(m_evt.len, anchor);
        }
        m_evt_prev = m_evt.len;
        m_evt.len += 3;
    }
    void set_val_anchor(csubstr anchor)
    {
        _c4dbgpf("{}/{}: set_val_anchor: {}", m_evt.len, m_evt.cap, anchor);
        RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, !ryml_has_any_(VALREF));
        ryml_enable_(c4::yml::VALANCH);
        if(m_evt.len + 3 < m_evt.cap)
        {
            m_evt.ptr[m_evt.len] |= ievt::VAL_|ievt::ANCH;
            _add_scalar_(m_evt.len, anchor);
        }
        m_evt_prev = m_evt.len;
        m_evt.len += 3;
    }

    void set_key_ref(csubstr ref)
    {
        _c4dbgpf("{}/{}: set_key_ref: {}", m_evt.len, m_evt.cap, ref);
        RYML_ASSERT_PARSE_CB_(base_type::m_stack.m_callbacks, ref.begins_with('*'), base_type::m_curr->pos);
        RYML_ASSERT_PARSE_CB_(base_type::m_stack.m_callbacks, !ryml_has_any_(KEYANCH), base_type::m_curr->pos);
        ryml_enable_(c4::yml::KEY|c4::yml::KEYREF);
        _send_str_(ref.sub(1), ievt::KEY_|ievt::ALIA); // skip the leading *
    }
    void set_val_ref(csubstr ref)
    {
        _c4dbgpf("{}/{}: set_val_ref: {}", m_evt.len, m_evt.cap, ref);
        RYML_ASSERT_PARSE_CB_(base_type::m_stack.m_callbacks, ref.begins_with('*'), base_type::m_curr->pos);
        RYML_ASSERT_PARSE_CB_(base_type::m_stack.m_callbacks, !ryml_has_any_(VALANCH), base_type::m_curr->pos);
        ryml_enable_(c4::yml::VAL|c4::yml::VALREF);
        _send_str_(ref.sub(1), ievt::VAL_|ievt::ALIA); // skip the leading *
    }

    /** @} */

public:

    /** @name YAML tag events */
    /** @{ */

    void set_key_tag(csubstr tag)
    {
        _c4dbgpf("{}/{}: set key tag [{}]~~~{}~~~", m_evt.len, m_evt.cap, tag.len, tag.str ? tag : csubstr("(arena full)"));
        RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, _is_sub_(tag));
        ryml_enable_(c4::yml::KEYTAG);
        _send_str_(tag, ievt::KEY_|ievt::TAG_);
    }
    void set_val_tag(csubstr tag)
    {
        _c4dbgpf("{}/{}: set val tag [{}]~~~{}~~~", m_evt.len, m_evt.cap, tag.len, tag.str ? tag : csubstr("(arena full)"));
        RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, _is_sub_(tag));
        ryml_enable_(c4::yml::VALTAG);
        _send_str_(tag, ievt::VAL_|ievt::TAG_);
    }

    /** @} */

public:

    /** @name YAML directive events */
    /** @{ */

    void add_directive_yaml(csubstr yaml_version)
    {
        _c4dbgpf("{}/{}: %YAML directive! version={}", m_evt.len, m_evt.cap, yaml_version);
        _send_str_(yaml_version, ievt::YAML);
    }

    void add_directive_tag(csubstr handle, csubstr prefix)
    {
        _c4dbgpf("{}/{}: %TAG directive! handle={} prefix={} doc_id={}", m_evt.len, m_evt.cap, handle, prefix, m_curr_doc);
        if C4_UNLIKELY(!m_tag_directives.add(handle, prefix, m_curr_doc))
            RYML_ERR_PARSE_CB_(base_type::m_stack.m_callbacks, base_type::m_curr->pos, "too many %TAG directives");
        _send_str_(handle, ievt::TAGH);
        _send_str_(prefix, ievt::TAGP);
    }

    /** @} */

public:

    /** @name YAML structure events */
    /** @{ */

    /** set the previous val as the first key of a new map, with flow style.
     *
     * See the documentation for @ref doc_event_handlers, which
     * has important notes about this event.
     */
    C4_NO_INLINE void actually_val_is_first_key_of_new_map_flow()
    {
        _c4dbgpf("{}/{}: prev={} actually_val_is_first_key_of_new_map_flow", m_evt.len, m_evt.cap, m_evt_prev);
        RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, m_evt.len > 2);
        RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, m_evt_prev > 0);
        RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, m_evt.len < m_evt.cap || !resize_buffers);
        evt_size pos = m_evt_prev;
        if(resize_buffers || m_evt.len < m_evt.cap)
        {
            // BEFORE
            // ... flag offs len (free)
            //     |              |
            //     prev           curr
            // AFTER
            // ... BMAP flag offs len (free)
            //          |              |
            //          prev           curr
            if(m_evt.ptr[m_evt_prev] & ievt::WSTR)
            {
                _c4dbgpf("{}/{}: WSTR", m_evt.len, m_evt.cap);
                RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, m_evt_prev > 0);
                pos = _extend_left_to_include_tag_and_or_anchor(m_evt_prev);
                if C4_IF_CONSTEXPR (resize_buffers)
                    _grow_evts();
                if(resize_buffers || m_evt.len + 1 < m_evt.cap)
                {
                    for(evt_size i = pos; i <= m_evt_prev; i = _next(i))
                    {
                        m_evt.ptr[i] |= ievt::KEY_;
                        m_evt.ptr[i] &= ~ievt::VAL_;
                    }
                    evt_size num_move = m_evt.len + 1 - pos;
                    RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, num_move > 0);
                    memmove(m_evt.ptr + pos + 1, m_evt.ptr + pos, (size_t)num_move * sizeof(evt_bits));
                }
                m_evt.ptr[pos] = ievt::BMAP|ievt::FLOW|ievt::VAL_;
                // move PSTR to prev
                if(m_evt.ptr[pos + 1] & ievt::PSTR)
                {
                    m_evt.ptr[pos    ] |= ievt::PSTR;
                    m_evt.ptr[pos + 1] &= ~ievt::PSTR;
                }
            }
            else
            {
                _c4dbgpf("{}/{}: container key. prev={}", m_evt.len, m_evt.cap, m_evt_prev);
                RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, (m_evt.ptr[m_evt_prev] & (ievt::EMAP|ievt::ESEQ)));
                if((m_evt.ptr[m_evt_prev] & ievt::EMAP) == ievt::EMAP)
                {
                    pos = _find_matching_open(ievt::BMAP, ievt::EMAP, m_evt_prev);
                }
                else
                {
                    RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, (m_evt.ptr[m_evt_prev] & ievt::ESEQ));
                    pos = _find_matching_open(ievt::BSEQ, ievt::ESEQ, m_evt_prev);
                }
                _c4dbgpf("{}/{}: matching open for {}={}", m_evt.len, m_evt.cap, m_evt_prev, pos);
                RYML_CHECK_BASIC_CB_(base_type::m_stack.m_callbacks, pos >= 0); // internal error
                RYML_CHECK_BASIC_CB_(base_type::m_stack.m_callbacks, pos < m_evt_prev); // internal error
                RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, (m_evt.ptr[pos] & ievt::ESEQ) == (m_evt.ptr[m_evt_prev] & ievt::BSEQ));
                RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, (m_evt.ptr[pos] & ievt::EMAP) == (m_evt.ptr[m_evt_prev] & ievt::BMAP));
                // shift the array one position to the right, starting at pos
                evt_size posp1 = pos + 1;
                if C4_IF_CONSTEXPR (resize_buffers)
                    _grow_evts();
                if(resize_buffers || m_evt.len + 1 < m_evt.cap)
                {
                    evt_size num_move = m_evt.len + 1 - pos;
                    RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, num_move > 0);
                    memmove(m_evt.ptr + posp1, m_evt.ptr + pos, (size_t)num_move * sizeof(evt_bits));
                }
                RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, posp1 < m_evt.len);
                // start the map
                m_evt.ptr[pos] = ievt::BMAP|ievt::FLOW|ievt::VAL_;
                // set next as key, not val
                m_evt.ptr[posp1] |= ievt::KEY_;
                m_evt.ptr[posp1] &= ~ievt::VAL_;
                // move PSTR to pos
                if(m_evt.ptr[posp1] & ievt::PSTR)
                {
                    m_evt.ptr[pos] |= ievt::PSTR;
                    m_evt.ptr[posp1] &= ~ievt::PSTR;
                }
            }
        }
        base_type::m_curr->evt_id = m_evt.len - 2;
        ++m_evt_prev;
        ++m_evt.len;
        ryml_enable_(c4::yml::MAP|c4::yml::FLOW_SL);
        _push();
        base_type::m_parent->evt_id = pos;
    }

    /** like its flow counterpart, but this function can only be
     * called after the end of a flow-val at root or doc level.
     *
     * See the documentation for @ref doc_event_handlers, which has
     * important notes about this event.
     */
    C4_NO_INLINE void actually_val_is_first_key_of_new_map_block()
    {
        _c4dbgpf("{}/{}: prev={} actually_val_is_first_key_of_new_map_block", m_evt.len, m_evt.cap, m_evt_prev);
        RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, m_evt.len < m_evt.cap || !resize_buffers);
        if(resize_buffers || m_evt.len < m_evt.cap)
        {
            // interpolate BMAP|VAL|BLCK after the last BDOC
            evt_size pos = _find_last_bdoc(m_evt.len);
            if(pos >= 0)
            {
                RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, pos < m_evt.cap);
                RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, pos < m_evt.len);
                RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, (m_evt.ptr[pos] & ievt::BDOC) == ievt::BDOC);
                RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, m_evt.len < m_evt.cap);
                ++pos; // add 1 to write after BDOC
                evt_size num_move = m_evt.len - pos;
                evt_size posp1 = pos + 1;
                RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, posp1 < m_evt.cap);
                RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, ((m_evt.ptr[pos] & ievt::BSEQ) == ievt::BSEQ) || ((m_evt.ptr[pos] & ievt::BMAP) == ievt::BMAP));
                RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, num_move > 0);
                RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, 0 == (m_evt.ptr[posp1] & ievt::PSTR));
                memmove(m_evt.ptr + posp1, m_evt.ptr + pos, (size_t)num_move * sizeof(evt_bits));
                m_evt.ptr[pos] = ievt::VAL_|ievt::BMAP|ievt::BLCK;
                m_evt.ptr[posp1] &= ~ievt::VAL_;
                m_evt.ptr[posp1] |= ievt::KEY_;
            }
        }
        ++base_type::m_curr->evt_id;
        ++m_evt_prev;
        ++m_evt.len;
        _push();
    }

    /** @} */

public:

    /** @name arena events */
    /** @{ */

    substr arena()
    {
        return m_arena.first(m_arena_pos < m_arena.len ? m_arena_pos : m_arena.len);
    }
    substr arena_rem() // NOLINT
    {
        return C4_EXPECT(m_arena_pos <= m_arena.len, 1) ? m_arena.sub(m_arena_pos) : m_arena.last(0);
    }
    /** this may fail, in which case an empty string is returned */
    substr alloc_arena(size_t len)
    {
        substr s = arena_rem();
        if C4_LIKELY(len <= s.len)
        {
            s.len = len;
        }
        else
        {
            if C4_IF_CONSTEXPR (resize_buffers)
            {
                _grow_arena(len);
                s = arena_rem();
                s.len = len;
            }
            else
            {
                s.str = nullptr;
            }
        }
        m_arena_pos += len;
        return s;
    }

    /** @} */

public:

    /** @name implementation helpers */
    /** @{ */

    /** push a new parent, add a child to the new parent, and set the
     * child as the current node */
    void _push()
    {
        base_type::_stack_push();
        base_type::m_curr->evt_type = {};
    }

    /** end the current scope */
    void _pop()
    {
        base_type::_stack_pop();
    }

    template<c4::yml::type_bits bits> C4_ALWAYS_INLINE void enable_() noexcept
    {
        base_type::m_curr->evt_type |= bits;
    }
    template<c4::yml::type_bits bits> C4_ALWAYS_INLINE void disable_() noexcept
    {
        base_type::m_curr->evt_type &= ~bits;
    }
    template<c4::yml::type_bits bits> C4_ALWAYS_INLINE bool has_any_() const noexcept
    {
        return (base_type::m_curr->evt_type & bits) != c4::yml::type_bits(0);
    }

    C4_ALWAYS_INLINE evt_size _next(evt_size pos) const noexcept
    {
        RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, pos < m_evt.cap);
        return pos + ((m_evt.ptr[pos] & ievt::WSTR) ? 3 : 1);
    }

    C4_ALWAYS_INLINE evt_size _prev(evt_size pos) const noexcept
    {
        RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, pos < m_evt.cap);
        return pos - ((m_evt.ptr[pos] & ievt::PSTR) ? 3 : 1);
    }

    C4_ALWAYS_INLINE bool _is_sub_(csubstr str) const noexcept
    {
        return (!str.str || str.is_sub(base_type::m_src) || str.is_sub(m_arena));
    }

    C4_NO_INLINE void _grow_evts(evt_size next)
    {
        next = next > 256 ? next : 256;
        m_evt = detail::resize(m_evt, next, base_type::m_stack.m_callbacks);
    }

    C4_NO_INLINE void _grow_evts()
    {
        _grow_evts(2 * m_evt.cap);
    }

    C4_NO_INLINE void _grow_arena(size_t more)
    {
        size_t next = m_arena.len + more;
        next = 2 * m_arena.len > next ? 2 * m_arena.len : next;
        next = next > 256 ? next : 256;
        m_arena = detail::resize(m_arena, next, base_type::m_stack.m_callbacks);
    }

    C4_ALWAYS_INLINE void _send_flag_only_(evt_bits flags)
    {
        _c4dbgpf("{}/{}: flag only", m_evt.len, m_evt.cap);
        if C4_IF_CONSTEXPR (resize_buffers)
        {
            if C4_UNLIKELY(m_evt.len >= m_evt.cap)
                _grow_evts();
            m_evt.ptr[m_evt.len] |= flags;
        }
        else
        {
            if(m_evt.len < m_evt.cap)
                m_evt.ptr[m_evt.len] |= flags;
        }
        base_type::m_curr->evt_id = m_evt.len;
        m_evt_prev = m_evt.len;
        ++m_evt.len;
        if(m_evt.len < m_evt.cap)
            m_evt.ptr[m_evt.len] = {};
    }

    C4_ALWAYS_INLINE void _send_str_(csubstr scalar, evt_bits flags)
    {
        _c4dbgpf("{}/{}: send str", m_evt.len, m_evt.cap);
        if C4_IF_CONSTEXPR (resize_buffers)
        {
            if C4_UNLIKELY(m_evt.len + 3 >= m_evt.cap)
                _grow_evts();
            m_evt.ptr[m_evt.len] |= flags;
            _add_scalar_(m_evt.len, scalar);
        }
        else
        {
            if(m_evt.len + 3 < m_evt.cap)
            {
                m_evt.ptr[m_evt.len] |= flags;
                _add_scalar_(m_evt.len, scalar);
            }
        }
        base_type::m_curr->evt_id = m_evt.len;
        m_evt_prev = m_evt.len;
        m_evt.len += 3;
    }

    void _mark_parent_with_children_()
    {
        if(base_type::m_parent)
            base_type::m_parent->has_children = true;
    }

    C4_ALWAYS_INLINE csubstr _get_latest_empty_scalar() const
    {
        // ideally we should search back in the latest event that has
        // a scalar, then select a zero-length scalar immediately
        // after that scalar. But this also works for now:
        return base_type::m_src.first(0);
    }

    evt_size _find_last_bdoc(evt_size pos) const
    {
        RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, pos < m_evt.cap); // it's safe to read from the array
        while(pos >= 0)
        {
            evt_bits e = m_evt.ptr[pos];
            if((e & ievt::BDOC) == ievt::BDOC)
                return pos;
            pos -= (e & ievt::PSTR) ? 3 : 1;
        }
        return -1; // LCOV_EXCL_LINE
    }

    evt_size _find_matching_open(evt_bits open, evt_bits close, evt_size pos) const
    {
        _c4dbgpf("find_matching: start at {}", pos);
        RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, pos < m_evt.cap);
        RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, (m_evt.ptr[pos] & close) == close);
        RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, (m_evt.ptr[pos] & open) == (close & ~ievt::END_));
        pos = _prev(pos); // don't count the starting close token
        uint32_t count = 0;
        while(pos >= 0)
        {
            evt_bits e = m_evt.ptr[pos];
            _c4dbgpf("find_matching: pos={} count={} e={}", pos, count, m_evt.ptr[pos]);
            if((e & close) == close)
            {
                _c4dbgpf(".............: pos={} close! count={} e={}", pos, count, m_evt.ptr[pos]);
                ++count;
            }
            else if((e & open) == open)
            {
                _c4dbgpf(".............: pos={} open! count={} e={}", pos, count, m_evt.ptr[pos]);
                if(!count)
                    return pos;
                else
                    --count;
            }
            pos = _prev(pos);
        }
        _c4dbgpf("find_matching: not found!", 0); // LCOV_EXCL_LINE
        return -1;  // LCOV_EXCL_LINE
    }

    evt_size _extend_left_to_include_tag_and_or_anchor(evt_size pos) const
    {
        RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, pos < m_evt.cap);
        evt_size prev = _prev(pos);
        while((prev > 0) && (m_evt.ptr[prev] & (ievt::TAG_|ievt::ANCH)))
        {
            _c4dbgpf("{}/{}: {} is anchor/tag. extend to {}", m_evt.len, m_evt.cap, prev, prev);
            pos = prev;
            prev = _prev(prev);
        }
        return pos;
    }

    C4_ALWAYS_INLINE evt_bits translate_flowml_(type_bits multiline_style) const noexcept
    {
        static_assert((uint32_t)(FLOW_ML1 << 12) == (uint32_t)ievt::FML1, "");
        static_assert((uint32_t)(FLOW_MLN << 12) == (uint32_t)ievt::FMLN, "");
        static_assert((uint32_t)(FLOW_MLX << 12) == (uint32_t)ievt::FMLX, "");
        RYML_ASSERT_BASIC_CB_(base_type::m_stack.m_callbacks, 0 == (multiline_style & ~FLOW_MLX));
        return (evt_bits)(multiline_style << 12);
    }

    /** @} */

#undef ryml_enable_
#undef ryml_disable_
#undef ryml_has_any_
#undef _add_scalar_

};

/** @} */

} // namespace ievt
} // namespace extra
} // namespace yml
} // namespace c4


// NOLINTEND(hicpp-signed-bitwise,*avoid-c-style-cast)
C4_SUPPRESS_WARNING_GCC_CLANG_POP

#endif /* C4_YML_EXTRA_EVENT_HANDLER_INTS_HPP_ */
