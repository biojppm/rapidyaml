#ifndef _C4_YML_EXTRA_EVENT_HANDLER_INTS_HPP_
#define _C4_YML_EXTRA_EVENT_HANDLER_INTS_HPP_

/** @file event_handler_ints.hpp An event handler that creates an
 * integer buffer with a very compact representation of the YAML tree
 * in a source buffer. This is not part of the main rapidyaml library.
 *
 * @see c4::yml::extra::ievt::EventFlags
 * @see c4::yml::extra::EventHandlerInts
 * */

#ifndef RYML_SINGLE_HEADER
#ifndef _C4_YML_NODE_TYPE_HPP_
#include <c4/yml/node_type.hpp>
#endif
#ifndef _C4_YML_EVENT_HANDLER_STACK_HPP_
#include "c4/yml/event_handler_stack.hpp"
#endif
#ifndef _C4_YML_TAG_HPP_
#include <c4/yml/tag.hpp>
#endif
#ifndef _C4_YML_DETAIL_DBGPRINT_HPP_
#include <c4/yml/detail/dbgprint.hpp>
#endif
#endif

// NOLINTBEGIN(hicpp-signed-bitwise)

namespace c4 {
namespace yml {
namespace extra {

/** @addtogroup doc_event_handlers
 * @{ */

namespace ievt {

/** data type for integer events. This is set to a 32 bit signed
 * integer to allow compatibility with a wide range of processing
 * languages. */
using DataType = int32_t;

/** enumeration of integer event bits. */
typedef enum : DataType {

    // Event types
    BSTR = (1 <<  0),  ///< +STR begin stream
    ESTR = (1 <<  1),  ///< -STR end stream
    BDOC = (1 <<  2),  ///< +DOC begin doc
    EDOC = (1 <<  3),  ///< -DOC end doc
    BMAP = (1 <<  4),  ///< +MAP begin map
    EMAP = (1 <<  5),  ///< -MAP end map
    BSEQ = (1 <<  6),  ///< +SEQ begin seq
    ESEQ = (1 <<  7),  ///< -SEQ end seq
    SCLR = (1 <<  8),  ///< =VAL scalar
    ANCH = (1 <<  9),  ///< &anchor
    ALIA = (1 << 10),  ///< *ref =ALI alias (reference)
    TAG_ = (1 << 11),  ///< !tag

    // Style flags
    PLAI = (1 << 12),  ///< plain scalar
    SQUO = (1 << 13),  ///< single-quoted scalar (')
    DQUO = (1 << 14),  ///< double-quoted scalar ("")
    LITL = (1 << 15),  ///< block literal scalar (|)
    FOLD = (1 << 16),  ///< block folded scalar (>)
    FLOW = (1 << 17),  ///< flow container: [] for seqs or {} for maps
    BLCK = (1 << 18),  ///< block container

    // Structure flags
    KEY_ = (1 << 19),  ///< as key
    VAL_ = (1 << 20),  ///< as value
    EXPL = (1 << 21),  ///< `---` (with BDOC) or
                       ///< `...` (with EDOC)

    // Directive flags
    YAML = (1 << 22),  ///< `%YAML <version>`
    TAGD = (1 << 23),  ///< tag directive name : `%TAG <name> .......`
    TAGV = (1 << 24),  ///< tag directive value: `%TAG ...... <value>`

    // Buffer flags
    /// IMPORTANT. Marks events whose string was placed in the
    /// arena. This happens when the filtered string is larger than the
    /// original string in the YAML code (eg from tags that resolve to
    /// a larger string, or from "\L" or "\P" in double quotes, which
    /// expand from two to three bytes). Because of this size
    /// expansion, the filtered string cannot be placed in the original
    /// source and needs to be placed in the arena.
    AREN = (1 << 25),
    /// special flag to enable look-back in the event array. it
    /// signifies that the previous event has a string, meaning that
    /// the jump back to that event is 3 positions. without this flag it
    /// would be impossible to jump to the previous event.
    PSTR = (1 << 26),
    /// special flag to mark a scalar as unfiltered (when the parser
    /// is set not to filter).
    UNFILT = (1 << 27),

    // Utility flags/masks
    LAST = UNFILT,              ///< the last flag defined above
    MASK = (LAST << 1) - 1,     ///< a mask of all bits in this enumeration
    /// with string: mask of all the events that encode a string
    /// following the event. in the event has a string. the next two
    /// integers will provide respectively the string's offset and
    /// length. See also @ref PSTR.
    WSTR = SCLR|ALIA|ANCH|TAG_|TAGD|TAGV|YAML,
} EventFlags;

} // namespace ievt

/** @} */

} // namespace extra
} // namespace yml
} // namespace c4


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

namespace c4 {
namespace yml {
namespace extra {

/** @addtogroup doc_event_handlers
 * @{ */

/** Read YAML source and, without undergoing a full parse, estimate
 * the size of the integer buffer required for @ref
 * EventHandlerInts. This estimation is meant to exceed the actual
 * number of required events.
 *
 * @note This function must overpredict. It does so for every case in
 * the hundreds/thousands of extensive tests of rapidyaml -- both for
 * the YAML test suite and the internal cases. If you find a case
 * where that does not hold, it is a bug. Please report it at
 * https://github.com/biojppm/rapidyaml/issues! */
RYML_EXPORT int32_t estimate_events_ints_size(csubstr src);

/** @} */

} // namespace extra
} // namespace yml
} // namespace c4


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

C4_SUPPRESS_WARNING_GCC_CLANG_PUSH
C4_SUPPRESS_WARNING_GCC_CLANG("-Wold-style-cast")
C4_SUPPRESS_WARNING_GCC("-Wuseless-cast")

namespace c4 {
namespace yml {
namespace extra {


/** @addtogroup doc_event_handlers
 * @{ */

/** @cond dev */
struct EventHandlerIntsState : public c4::yml::ParserState
{
    c4::yml::type_bits evt_type;
    int32_t evt_id;
};
/** @endcond */


/** A parser event handler that creates a compact representation of
 * the YAML tree in a buffer of integers (see @ref ievt::EventFlags)
 * containing masks (to represent events) and offset+length (to
 * represent strings in the source buffer).
 *
 * This is meant for use by other programming languages, and supports
 * container keys (unlike the ryml tree). It parses faster than the ryml
 * tree parser, because the resulting data structure is much simpler.
 *
 * The resulting integer buffer is a linear array of integers containing
 * events (as a mask of @ref ievt::EventFlags), which in some cases (see
 * @ref ievt::WSTR) are followed by an encoded string (encoded as an
 * offset and length to the parsed source buffer).
 *
 * For example, parsing `[a, bb, ccc]` results in the following event
 * buffer:
 *
 * ```c++
 * using namespace c4::yml::extra::ievt;
 * const DataType arr[] = {       // result of parsing: [a, bb, ccc]
 *   BSTR,                        // begin stream
 *   BDOC,                        // begin doc
 *   VAL_|BSEQ|FLOW,              // begin seq as val, flow
 *   VAL_|SCLR|PLAI,      1, 1,   // val scalar, plain style: "a"   starts at offset 1 and has length 1
 *   VAL_|SCLR|PLAI|PSTR, 4, 2,   // val scalar, plain style: "bb"  starts at offset 4 and has length 2
 *   VAL_|SCLR|PLAI|PSTR, 8, 3,   // val scalar, plain style: "ccc" starts at offset 8 and has length 3
 *   ESEQ|PSTR,                   // end seq
 *   EDOC,                        // end doc
 *   ESTR,                        // end stream
 * };
 * ```
 *
 * Here is a sketch clarifying the meaning of this event sequence:
 *
@code
source  : [a, bb, ccc]
                                                               string offset "a"              string offset "bb"             string offset "ccc"
                                                               |  string length "a"           |  string length "bb"          |  string length "ccc"
                                                               |  |                           |  |                           |  |
           event    event   event [            event "a".......|..|      event "bb"...........|..|      event "ccc"..........|..|      event ]       event    event
           |        |       |                  |               |  |      |                    |  |      |                    |  |      |             |        |
           +--------+-------+------------------+---------------+--+------+--------------------+--+------+--------------------+--+------+-------------+--------+-----|
value    : BSTR     BDOC    VAL_|BSEQ|FLOW     VAL_|SCLR|PLAI..1..1      VAL_|SCLR|PLAI|PSTR..4..2      VAL_|SCLR|PLAI|PSTR..8..3      ESEQ|PSTR     EDOC     ESTR  (array)
event #  : 0        1       2                  3               .  .      4              |     .  .      5              |     .  .      6             7        8     (event #)
index/pos: 0        1       2                  3               4  5      6              |     7  8      9              |     10 11     12            13       14    (index/pos)
                                                \              |  |       \             |     |  |       \             |     |  |
                                                 has a string--+--+        has a string-+-----+--+        has a string-+-----+--+
                                                                                        |                              |
                                                                                        prev event has string          prev event has string
                                                                                        (jump back 3 to get to it)     (jump back 3 to get to it)
@endcode
 *
 *
 * Note that the buffer contains both events and strings encoded as
 * integer pairs. That is, events that have an associated string are
 * immediately followed by two integers providing the offset and length
 * of that string in the source buffer. (In the example above, this
 * happens in the events for the strings `a`, `bb`, and `ccc` at
 * positions 3, 6 and 9, respectively).
 *
 * The flag @ref ievt::PSTR and the mask @ref ievt::WSTR are provided to
 * enable easier iteration over the array: you can use them to test for
 * presence of a string when iterating over the array.
 *
 * The flag @ref ievt::PSTR announces that an event is *preceded* by a
 * string. That is, the previous event has a string, so that when this
 * flag is found while iterating right-to-left, a jump of -3 should be
 * used to get at the bitmask of the previous event. (In the example
 * above, this flag is present for the events for `bb` and `ccc`, but not
 * `a` because it is not preceded by a string).
 *
 * Likewise, to signify that the current event is *followed* by a string,
 * there is the mask @ref ievt::WSTR, which is a mask of all the flags of
 * events that have a string: @ref ievt::SCLR, @ref ievt::ALIA, @ref
 * ievt::ANCH and @ref ievt::TAG_. While iterating left-to-right in the
 * array, presence of any of the bits in the mask @ref ievt::WSTR means
 * that a jump of +3 should be employed to get at the bitmask of the next
 * event.
 *
 * Typical code to iterate left-to-right over the array will look like
 * this:
 *
 * ```c++
 * // source buffer, modified in place during parsing (IMPORTANT!)
 * substr src = ...;
 * substr arena = ...; // arena used for scalars/tags that are extended during filtering
 * // events resulting from parsing
 * const int events[] = {...};
 * int events_size = ...;
 * for(int i = 0; i < events_size; ++i)
 * {
 *     if(events[i] & ievt::WSTR) // this event has a string following it
 *     {
 *         size_t offset = (size_t)events[i+1];
 *         size_t length = (size_t)events[i+2];
 *         csubstr region = (events[i] & ievt::AREN) ? arena : src; // is the string in the arena?
 *         csubstr str = region.sub(offset, length); // get the string
 *         ...
 *         i += 2; // skip the two ints of the string
 *                 // (the jump is three places; the loop adds the other place)
 *     }
 *     else  // this is a single-int event
 *     {
 *         ...
 *     }
 * }
 * ```
 *
 * This handler must be initialized with the input source buffer, the
 * output arena, and the output event buffer. This handler will not take
 * ownership nor attempt to resize the output buffer. If the size
 * required for the output buffer or arena are larger than their actual
 * size, parsing goes all way to the end, determining the required buffer
 * sizes without writing anything past the end of the respective
 * buffer. After parsing is finished, the user must ensure that the
 * buffer size was enough to accomodate all the data that needs to be
 * written into it, or react accordingly (eg, throw an error, or resize
 * the buffer then retry the parse).
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
 * extra::EventHandlerInts handler;
 * ParseEngine<extra::EventHandlerInts> parser(&handler);
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
 * info) must be an overprediction: it overpredicts for every single case
 * among the many hundreds of cases covered in the unit tests. This is
 * deliberate, and aims at ensuring that a retry parse is not needed. But
 * conceivably, it may underpredict in some instances not found in the out
 * tests. What to do then?
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
 * parse again. So if you want to retry, you need to keep a pristine copy
 * of the source, and use it for the retry:
 *
 * ```c++
 * const std::string src = ...;  // the YAML code to be parsed
 * std::string parsed_src = src; // this is where we will parse (filter during parsring)
 * std::vector<int> evts((size_t)estimated_size); // ensure we have a fighting change to acommodate the events
 * std::vector<char> arena(src.size()); // ensure we have a fighting change to acommodate the events
 * ParseEngine<extra::EventHandlerInts> parser(&handler);
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
    int32_t m_evt_pos;
    int32_t m_evt_prev;
    int32_t m_evt_size;
    substr m_arena;
    size_t m_arena_pos;
    TagDirective m_tag_directives[RYML_MAX_TAG_DIRECTIVES];
    bool m_has_yaml_directive;
    bool m_has_docs;

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
        reset(csubstr{}, substr{}, nullptr, 0);
    }
    EventHandlerInts()
        : EventHandlerInts(c4::yml::get_callbacks())
    {
    }

    void reset(csubstr str, substr arena, ievt::DataType *dst, int32_t dst_size)
    {
        _stack_reset_root();
        m_curr->flags |= c4::yml::RUNK|c4::yml::RTOP;
        m_curr->evt_type = {};
        m_curr->evt_id = 0;
        m_arena = arena;
        m_arena_pos = 0;
        m_str = str;
        m_evt = dst;
        m_evt_size = dst_size;
        m_evt_pos = 0;
        m_evt_prev = 0;
        m_has_docs = false;
        m_has_yaml_directive = false;
        for(TagDirective &td : m_tag_directives)
            td = {};
    }

    /** get the size needed for the event buffer from the previous parse
     * @warning this is valid only until the next parse */
    int required_size_events() const
    {
        return m_evt_pos;
    }

    /** get the size needed for the arena from the previous parse
     * @warning this is valid only until the next parse */
    size_t required_size_arena() const
    {
        return m_arena_pos;
    }

    /** Predicate to test if the event buffer successfully accomodated
     * all the parse events.
     * @warning this is valid only until the next parse */
    bool fits_buffers() const
    {
        return m_evt_pos <= m_evt_size && m_arena_pos <= m_arena.len;
    }

    void reserve_arena(int /*arena_size*/)
    {
        // does not apply here
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
        _c4dbgpf("{}/{}: begin_doc", m_evt_pos, m_evt_size);
        _send_flag_only_(ievt::BDOC);
        if(_stack_should_push_on_begin_doc())
        {
            _c4dbgp("push!");
            _push();
            _enable_(DOC);
        }
        m_has_docs = true;
    }
    /** implicit doc end (without ...) */
    void end_doc()
    {
        _c4dbgpf("{}/{}: end_doc", m_evt_pos, m_evt_size);
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
        _c4dbgpf("{}/{}: begin_doc_expl", m_evt_pos, m_evt_size);
        _send_flag_only_(ievt::BDOC|ievt::EXPL);
        _c4dbgp("push!");
        _push();
        _enable_(DOC);
        m_has_docs = true;
    }
    /** explicit doc end, with ... */
    void end_doc_expl()
    {
        _c4dbgpf("{}/{}: end_doc_expl", m_evt_pos, m_evt_size);
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
        _c4dbgpf("{}/{}: bmap key flow", m_evt_pos, m_evt_size);
        _send_flag_only_(ievt::KEY_|ievt::BMAP|ievt::FLOW);
        _mark_parent_with_children_();
        _enable_(c4::yml::KEY|c4::yml::MAP|c4::yml::FLOW_SL);
        _push();
    }
    void begin_map_key_block()
    {
        _c4dbgpf("{}/{}: bmap key block", m_evt_pos, m_evt_size);
        _send_flag_only_(ievt::KEY_|ievt::BMAP|ievt::BLCK);
        _mark_parent_with_children_();
        _enable_(c4::yml::KEY|c4::yml::MAP|c4::yml::BLOCK);
        _push();
    }

    void begin_map_val_flow()
    {
        _c4dbgpf("{}/{}: bmap flow", m_evt_pos, m_evt_size);
        _send_flag_only_(ievt::VAL_|ievt::BMAP|ievt::FLOW);
        _mark_parent_with_children_();
        _enable_(c4::yml::MAP|c4::yml::FLOW_SL);
        _push();
    }
    void begin_map_val_block()
    {
        _c4dbgpf("{}/{}: bmap block", m_evt_pos, m_evt_size);
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
        _c4dbgpf("{}/{}: bseq key flow", m_evt_pos, m_evt_size);
        _send_flag_only_(ievt::KEY_|ievt::BSEQ|ievt::FLOW);
        _mark_parent_with_children_();
        _enable_(c4::yml::KEY|c4::yml::SEQ|c4::yml::FLOW_SL);
        _push();
    }
    void begin_seq_key_block()
    {
        _c4dbgpf("{}/{}: bseq key block", m_evt_pos, m_evt_size);
        _send_flag_only_(ievt::KEY_|ievt::BSEQ|ievt::BLCK);
        _mark_parent_with_children_();
        _enable_(c4::yml::KEY|c4::yml::SEQ|c4::yml::BLOCK);
        _push();
    }

    void begin_seq_val_flow()
    {
        _c4dbgpf("{}/{}: bseq flow", m_evt_pos, m_evt_size);
        _send_flag_only_(ievt::VAL_|ievt::BSEQ|ievt::FLOW);
        _mark_parent_with_children_();
        _enable_(c4::yml::SEQ|c4::yml::FLOW_SL);
        _push();
    }
    void begin_seq_val_block()
    {
        _c4dbgpf("{}/{}: bseq block", m_evt_pos, m_evt_size);
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
        _c4dbgpf("{}/{}: prev={} actually_val_is_first_key_of_new_map_flow", m_evt_pos, m_evt_size, m_evt_prev);
        _RYML_CB_ASSERT(m_stack.m_callbacks, m_evt_pos > 2);
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
                _c4dbgpf("{}/{}: WSTR", m_evt_pos, m_evt_size);
                _RYML_CB_ASSERT(m_stack.m_callbacks, m_evt_prev > 0);
                int32_t pos = _extend_left_to_include_tag_and_or_anchor(m_evt_prev);
                if(m_evt_pos + 1 < m_evt_size)
                {
                    for(int32_t i = pos; i <= m_evt_prev; i = _next(i))
                    {
                        m_evt[i] |= ievt::KEY_;
                        m_evt[i] &= ~ievt::VAL_;
                    }
                    int32_t num_move = m_evt_pos + 1 - pos;
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
                _c4dbgpf("{}/{}: container key", m_evt_pos, m_evt_size);
                _RYML_CB_ASSERT(m_stack.m_callbacks, (m_evt[m_evt_prev] & (ievt::EMAP|ievt::ESEQ)));
                int32_t pos;
                _c4dbgpf("{}/{}: find matching open for {}", m_evt_pos, m_evt_size, m_evt_prev);
                if(m_evt[m_evt_prev] & ievt::EMAP)
                {
                    pos = _find_matching_open(ievt::BMAP, ievt::EMAP, m_evt_prev);
                }
                else
                {
                    _RYML_CB_ASSERT(m_stack.m_callbacks, (m_evt[m_evt_prev] & ievt::ESEQ));
                    pos = _find_matching_open(ievt::BSEQ, ievt::ESEQ, m_evt_prev);
                }
                _c4dbgpf("{}/{}: matching open for {}={}", m_evt_pos, m_evt_size, m_evt_prev, pos);
                _RYML_CB_CHECK(m_stack.m_callbacks, pos >= 0); // internal error
                _RYML_CB_CHECK(m_stack.m_callbacks, pos < m_evt_prev); // internal error
                _RYML_CB_ASSERT(m_stack.m_callbacks, (m_evt[pos] & ievt::ESEQ) == (m_evt[m_evt_prev] & ievt::BSEQ));
                _RYML_CB_ASSERT(m_stack.m_callbacks, (m_evt[pos] & ievt::EMAP) == (m_evt[m_evt_prev] & ievt::BMAP));
                // shift the array one position to the right, starting at pos
                int32_t posp1 = pos + 1;
                if(m_evt_pos + 1 < m_evt_size)
                {
                    int32_t num_move = m_evt_pos + 1 - pos;
                    _RYML_CB_ASSERT(m_stack.m_callbacks, num_move > 0);
                    memmove(m_evt + posp1, m_evt + pos, (size_t)num_move * sizeof(ievt::DataType));
                }
                _RYML_CB_ASSERT(m_stack.m_callbacks, posp1 < m_evt_pos);
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
        m_curr->evt_id = m_evt_pos - 2;
        ++m_evt_prev;
        ++m_evt_pos;
        _enable_(c4::yml::MAP|c4::yml::FLOW_SL);
        _push();
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
            int32_t pos = _find_last_bdoc(m_evt_pos);
            if(pos >= 0)
            {
                _RYML_CB_ASSERT(m_stack.m_callbacks, pos < m_evt_size);
                _RYML_CB_ASSERT(m_stack.m_callbacks, pos < m_evt_pos);
                _RYML_CB_ASSERT(m_stack.m_callbacks, (m_evt[pos] & ievt::BDOC));
                if(m_evt_pos < m_evt_size)
                {
                    ++pos; // add 1 to write after BDOC
                    int32_t num_move = m_evt_pos - pos;
                    int32_t posp1 = pos + 1;
                    _RYML_CB_ASSERT(m_stack.m_callbacks, (m_evt[pos] & (ievt::BSEQ|ievt::BMAP)));
                    _RYML_CB_ASSERT(m_stack.m_callbacks, num_move > 0);
                    _RYML_CB_ASSERT(m_stack.m_callbacks, 0 == (m_evt[posp1] & ievt::PSTR));
                    memmove(m_evt + posp1, m_evt + pos, (size_t)num_move * sizeof(ievt::DataType));
                    m_evt[pos] = ievt::VAL_|ievt::BMAP|ievt::BLCK;
                    m_evt[posp1] &= ~ievt::VAL_;
                    m_evt[posp1] |= ievt::KEY_;
                }
            }
        }
        ++m_curr->evt_id;
        ++m_evt_prev;
        ++m_evt_pos;
        _push();
    }

    /** @} */

public:

    /** @cond dev */
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
        return -1; // LCOV_EXCL_LINE
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
        _c4dbgpf("find_matching: not found!", 0); // LCOV_EXCL_LINE
        return -1;  // LCOV_EXCL_LINE
    }
    int32_t _extend_left_to_include_tag_and_or_anchor(int32_t pos) const
    {
        int32_t prev = _prev(pos);
        while((prev > 0) && (m_evt[prev] & (ievt::TAG_|ievt::ANCH)))
        {
            _c4dbgpf("{}/{}: {} is anchor/tag. extend to {}", m_evt_pos, m_evt_size, prev, prev);
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
    /** @endcond */

public:

    /** @name YAML scalar events */
    /** @{ */


    C4_ALWAYS_INLINE void set_key_scalar_plain_empty()
    {
        _c4dbgpf("{}/{}: set_key_scalar_plain_empty", m_evt_pos, m_evt_size);
        _send_key_scalar_(_get_latest_empty_scalar(), ievt::PLAI);
        _enable_(c4::yml::KEY|c4::yml::KEY_PLAIN|c4::yml::KEYNIL);
    }
    C4_ALWAYS_INLINE void set_val_scalar_plain_empty()
    {
        _c4dbgpf("{}/{}: set_val_scalar_plain_empty", m_evt_pos, m_evt_size);
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
        _c4dbgpf("{}/{}: set_key_scalar_plain: @{} [{}]~~~{}~~~", m_evt_pos, m_evt_size, scalar.str-m_str.str, scalar.len, scalar);
        _send_key_scalar_(scalar, ievt::PLAI);
        _enable_(c4::yml::KEY|c4::yml::KEY_PLAIN);
    }
    C4_ALWAYS_INLINE void set_val_scalar_plain(csubstr scalar)
    {
        _c4dbgpf("{}/{}: set_val_scalar_plain: @{} [{}]~~~{}~~~", m_evt_pos, m_evt_size, scalar.str-m_str.str, scalar.len, scalar);
        _send_val_scalar_(scalar, ievt::PLAI);
        _enable_(c4::yml::VAL|c4::yml::VAL_PLAIN);
    }


    C4_ALWAYS_INLINE void set_key_scalar_dquoted(csubstr scalar)
    {
        _c4dbgpf("{}/{}: set_key_scalar_dquo: @{} [{}]~~~{}~~~", m_evt_pos, m_evt_size, scalar.str-m_str.str, scalar.len, scalar);
        _send_key_scalar_(scalar, ievt::DQUO);
        _enable_(c4::yml::KEY|c4::yml::KEY_DQUO);
    }
    C4_ALWAYS_INLINE void set_val_scalar_dquoted(csubstr scalar)
    {
        _c4dbgpf("{}/{}: set_val_scalar_dquo: @{} [{}]~~~{}~~~", m_evt_pos, m_evt_size, scalar.str-m_str.str, scalar.len, scalar);
        _send_val_scalar_(scalar, ievt::DQUO);
        _enable_(c4::yml::VAL|c4::yml::VAL_DQUO);
    }


    C4_ALWAYS_INLINE void set_key_scalar_squoted(csubstr scalar)
    {
        _c4dbgpf("{}/{}: set_key_scalar_squo: @{} [{}]~~~{}~~~", m_evt_pos, m_evt_size, scalar.str-m_str.str, scalar.len, scalar);
        _send_key_scalar_(scalar, ievt::SQUO);
        _enable_(c4::yml::KEY|c4::yml::KEY_SQUO);
    }
    C4_ALWAYS_INLINE void set_val_scalar_squoted(csubstr scalar)
    {
        _c4dbgpf("{}/{}: set_val_scalar_squo: @{} [{}]~~~{}~~~", m_evt_pos, m_evt_size, scalar.str-m_str.str, scalar.len, scalar);
        _send_val_scalar_(scalar, ievt::SQUO);
        _enable_(c4::yml::VAL|c4::yml::VAL_SQUO);
    }


    C4_ALWAYS_INLINE void set_key_scalar_literal(csubstr scalar)
    {
        _c4dbgpf("{}/{}: set_key_scalar_literal: @{} [{}]~~~{}~~~", m_evt_pos, m_evt_size, scalar.str-m_str.str, scalar.len, scalar);
        _send_key_scalar_(scalar, ievt::LITL);
        _enable_(c4::yml::KEY|c4::yml::KEY_LITERAL);
    }
    C4_ALWAYS_INLINE void set_val_scalar_literal(csubstr scalar)
    {
        _c4dbgpf("{}/{}: set_val_scalar_literal: @{} [{}]~~~{}~~~", m_evt_pos, m_evt_size, scalar.str-m_str.str, scalar.len, scalar);
        _send_val_scalar_(scalar, ievt::LITL);
        _enable_(c4::yml::VAL|c4::yml::VAL_LITERAL);
    }


    C4_ALWAYS_INLINE void set_key_scalar_folded(csubstr scalar)
    {
        _c4dbgpf("{}/{}: set_key_scalar_folded: @{} [{}]~~~{}~~~", m_evt_pos, m_evt_size, scalar.str-m_str.str, scalar.len, scalar);
        _send_key_scalar_(scalar, ievt::FOLD);
        _enable_(c4::yml::KEY|c4::yml::KEY_FOLDED);
    }
    C4_ALWAYS_INLINE void set_val_scalar_folded(csubstr scalar)
    {
        _c4dbgpf("{}/{}: set_val_scalar_folded: @{} [{}]~~~{}~~~", m_evt_pos, m_evt_size, scalar.str-m_str.str, scalar.len, scalar);
        _send_val_scalar_(scalar, ievt::FOLD);
        _enable_(c4::yml::VAL|c4::yml::VAL_FOLDED);
    }


    C4_ALWAYS_INLINE void mark_key_scalar_unfiltered()
    {
        _c4dbgpf("{}/{}: mark_key_scalar_unfiltered", m_evt_pos, m_evt_size);
        if(m_evt_pos < m_evt_size)
            m_evt[m_evt_pos] |= ievt::UNFILT;
    }
    C4_ALWAYS_INLINE void mark_val_scalar_unfiltered()
    {
        _c4dbgpf("{}/{}: mark_val_scalar_unfiltered", m_evt_pos, m_evt_size);
        if(m_evt_pos < m_evt_size)
            m_evt[m_evt_pos] |= ievt::UNFILT;
    }

    /** @} */

public:

    /** @cond dev*/
#define _add_scalar_(i, scalar)                                     \
    _c4dbgpf("{}/{}: scalar!", i, m_evt_size);                      \
    _RYML_CB_ASSERT(m_stack.m_callbacks, scalar.is_sub(m_str) || scalar.is_sub(m_arena) || (scalar.str == nullptr)); \
    _RYML_CB_ASSERT(m_stack.m_callbacks, m_evt[i] & ievt::WSTR);    \
    _RYML_CB_ASSERT(m_stack.m_callbacks, i + 3 < m_evt_size);       \
    if(C4_LIKELY(scalar.is_sub(m_str)))                             \
    {                                                               \
        m_evt[i + 1] = (ievt::DataType)(scalar.str - m_str.str);    \
    }                                                               \
    else                                                            \
    {                                                               \
        m_evt[i] |= ievt::AREN;                                     \
        m_evt[i + 1] = (ievt::DataType)(scalar.str - m_arena.str);  \
        _c4dbgpf("{}/{}: arena! ->{}", i, m_evt_size, m_evt[i+1]);  \
    }                                                               \
    m_evt[i + 2] = (ievt::DataType)scalar.len;                      \
    m_evt[i + 3] = ievt::PSTR
    /** @endcond */

    /** @name YAML anchor/reference events */
    /** @{ */

    void set_key_anchor(csubstr anchor)
    {
        _c4dbgpf("{}/{}: set_key_anchor", m_evt_pos, m_evt_size);
        _RYML_CB_ASSERT(m_stack.m_callbacks, !_has_any_(KEYREF));
        _enable_(c4::yml::KEYANCH);
        if(m_evt_pos + 3 < m_evt_size)
        {
            m_evt[m_evt_pos] |= ievt::KEY_|ievt::ANCH;
            _add_scalar_(m_evt_pos, anchor);
        }
        m_evt_prev = m_evt_pos;
        m_evt_pos += 3;
    }
    void set_val_anchor(csubstr anchor)
    {
        _c4dbgpf("{}/{}: set_val_anchor", m_evt_pos, m_evt_size);
        _RYML_CB_ASSERT(m_stack.m_callbacks, !_has_any_(VALREF));
        _enable_(c4::yml::VALANCH);
        if(m_evt_pos + 3 < m_evt_size)
        {
            m_evt[m_evt_pos] |= ievt::VAL_|ievt::ANCH;
            _add_scalar_(m_evt_pos, anchor);
        }
        m_evt_prev = m_evt_pos;
        m_evt_pos += 3;
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
        _c4dbgpf("{}/{}: set key tag ~~~{}~~~", m_evt_pos, m_evt_size, tag);
        _enable_(c4::yml::KEYTAG);
        _set_tag(tag, ievt::KEY_);
    }
    void set_val_tag(csubstr tag)
    {
        _c4dbgpf("{}/{}: set val tag [{}]~~~{}~~~", m_evt_pos, m_evt_size, tag.len, tag);
        _enable_(c4::yml::VALTAG);
        _set_tag(tag, ievt::VAL_);
    }
    void _set_tag(csubstr tag, ievt::DataType which)
    {
        csubstr ttag = _transform_directive(tag);
        _c4dbgpf("{}/{}: transformed_tag [{}]~~~{}~~~", m_evt_pos, m_evt_size, ttag.len, ttag);
        if(m_evt_pos + 3 < m_evt_size)
        {
            m_evt[m_evt_pos] |= which|ievt::TAG_;
            _add_scalar_(m_evt_pos, ttag);
        }
        m_evt_prev = m_evt_pos;
        m_evt_pos += 3;
    }

    /** @} */

public:

    /** @name YAML directive events */
    /** @{ */

    void add_directive(csubstr directive)
    {
        _c4dbgpf("{}/{}: add directive ~~~{}~~~", m_evt_pos, m_evt_size, directive);
        _RYML_CB_ASSERT(m_stack.m_callbacks, directive.begins_with('%'));
        if(directive.begins_with("%TAG"))
        {
            const id_type pos = _num_tag_directives();
            if(C4_UNLIKELY(pos >= RYML_MAX_TAG_DIRECTIVES))
                _RYML_CB_ERR_(m_stack.m_callbacks, "too many directives", m_curr->pos);
            TagDirective &td = m_tag_directives[pos];
            if(C4_UNLIKELY(!td.create_from_str(directive)))
                _RYML_CB_ERR_(m_stack.m_callbacks, "failed to add directive", m_curr->pos);
            td.next_node_id = (id_type)m_evt_pos;
            _send_str_(td.handle, ievt::TAGD);
            _send_str_(td.prefix, ievt::TAGV);
        }
        else if(directive.begins_with("%YAML"))
        {
            _c4dbgpf("%YAML directive! ignoring...: {}", directive);
            if(C4_UNLIKELY(m_has_yaml_directive))
                _RYML_CB_ERR_(m_stack.m_callbacks, "multiple yaml directives", m_curr->pos);
            m_has_yaml_directive = true;
            csubstr rest = directive.sub(5).triml(' ');
            _send_str_(rest, ievt::YAML);
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

    substr arena_rem()
    {
        return C4_LIKELY(m_arena_pos <= m_arena.len) ? m_arena.sub(m_arena_pos) : m_arena.last(0);
    }

    /** this may fail, in which case a an empty string is returned */
    substr alloc_arena(size_t len)
    {
        substr s = arena_rem();
        if(C4_LIKELY(len <= s.len))
            s = s.first(len);
        else
            s.str = nullptr;
        m_arena_pos += len;
        return s;
    }

    /** this may fail, in which case an empty string is returned */
    C4_ALWAYS_INLINE substr alloc_arena(size_t len, substr *relocated)
    {
        (void)relocated;
        return alloc_arena(len);
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
        _c4dbgpf("{}/{}: flag only", m_evt_pos, m_evt_size);
        if(m_evt_pos < m_evt_size)
            m_evt[m_evt_pos] |= flags;
        m_curr->evt_id = m_evt_pos;
        m_evt_prev = m_evt_pos;
        ++m_evt_pos;
        if(m_evt_pos < m_evt_size)
            m_evt[m_evt_pos] = {};
    }

    C4_ALWAYS_INLINE void _send_key_scalar_(csubstr scalar, ievt::DataType flags)
    {
        _c4dbgpf("{}/{}: key scalar", m_evt_pos, m_evt_size);
        if(m_evt_pos + 3 < m_evt_size)
        {
            m_evt[m_evt_pos] |= ievt::SCLR|ievt::KEY_|flags;
            _add_scalar_(m_evt_pos, scalar);
        }
        m_curr->evt_id = m_evt_pos;
        m_evt_prev = m_evt_pos;
        m_evt_pos += 3;
    }

    C4_ALWAYS_INLINE void _send_val_scalar_(csubstr scalar, ievt::DataType flags)
    {
        _c4dbgpf("{}/{}: val scalar", m_evt_pos, m_evt_size);
        if(m_evt_pos + 3 < m_evt_size)
        {
            m_evt[m_evt_pos] |= ievt::SCLR|ievt::VAL_|flags;
            _add_scalar_(m_evt_pos, scalar);
        }
        m_curr->evt_id = m_evt_pos;
        m_evt_prev = m_evt_pos;
        m_evt_pos += 3;
    }

    C4_ALWAYS_INLINE void _send_str_(csubstr scalar, ievt::DataType flags)
    {
        _c4dbgpf("{}/{}: send str", m_evt_pos, m_evt_size);
        if(m_evt_pos + 3 < m_evt_size)
        {
            m_evt[m_evt_pos] |= flags;
            _add_scalar_(m_evt_pos, scalar);
        }
        m_curr->evt_id = m_evt_pos;
        m_evt_prev = m_evt_pos;
        m_evt_pos += 3;
    }

    void _clear_tag_directives_()
    {
        for(TagDirective &td : m_tag_directives)
            td = {};
    }
    C4_NODISCARD id_type _num_tag_directives() const
    {
        // this assumes we have a very small number of tag directives
        id_type i = 0;
        for(; i < RYML_MAX_TAG_DIRECTIVES; ++i)
            if(m_tag_directives[i].handle.empty())
                break;
        return i;
    }
    csubstr _transform_directive(csubstr tag)
    {
        // lookup from the end. We want to find the first directive that
        // matches the tag and has a target node id leq than the given
        // node_id.
        for(id_type i = RYML_MAX_TAG_DIRECTIVES-1; i != NONE; --i)
        {
            TagDirective const& td = m_tag_directives[i];
            if(td.handle.empty())
                continue;
            if(tag.begins_with(td.handle) && (td.handle != td.prefix))
            {
                substr rem = arena_rem();
                size_t len = td.transform(tag, rem, m_stack.m_callbacks, /*with_brackets*/false);
                if(len == 0)
                    return tag;
                alloc_arena(len);
                return rem.first(len <= rem.len ? len : 0);
            }
        }
        if(tag.begins_with('!'))
        {
            if(is_custom_tag(tag))
            {
                _RYML_CB_ERR_(m_stack.m_callbacks, "tag not found", m_curr->pos);
            }
        }
        return tag;
    }
#undef _enable_
#undef _disable_
#undef _has_any_
#undef _add_scalar_

};

/** @} */

} // namespace extra
} // namespace yml
} // namespace c4


// NOLINTEND(hicpp-signed-bitwise)
C4_SUPPRESS_WARNING_GCC_CLANG_POP

#endif /* _C4_YML_EXTRA_EVENT_HANDLER_INTS_HPP_ */
