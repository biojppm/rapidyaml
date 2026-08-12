#ifndef C4_YML_EXTRA_EVENT_INTS_HPP_
#define C4_YML_EXTRA_EVENT_INTS_HPP_

/** @file event_bits_ints.hpp */

#ifndef RYML_SINGLE_HEADER
#ifndef C4_YML_COMMON_HPP_
#include <c4/yml/common.hpp>
#endif
#ifndef C4_YML_ERROR_HPP_
#include <c4/yml/error.hpp>
#endif
#endif
#include <stdint.h>


// NOLINTBEGIN(hicpp-signed-bitwise,*avoid-c-style-cast)

namespace c4 {
namespace yml {
namespace extra {
namespace ievt {

/** @addtogroup doc_event_handlers_ints
 *
 * An event handler used by @ref ParseEngine to create an integer
 * buffer with a very compact representation of the YAML tree in a
 * source buffer. This feature is an extra and is not part of the main
 * rapidyaml library. It is meant for use by other programming
 * languages, and supports container keys (unlike the ryml tree). It
 * parses faster than the ryml tree parser, because the resulting data
 * structure is much simpler.
 *
 * The resulting integer buffer is a linear array of integers
 * containing encoded YAML events (as a mask of @ref ievt::EventBits),
 * which in some cases (eg scalars) are followed by an encoded string
 * (encoded as an offset and length to the parsed source buffer).
 *
 * For example, parsing `[a, bb, ccc]` results in the following event
 * buffer (grouped to highlight the event sequence structure):
 *
 * ```c++
 * using namespace c4::yml::extra::ievt;
 * const evt_bits arr[] = {       // result of parsing: [a, bb, ccc]
 *   BSTR,                        // begin stream
 *   BDOC,                        // begin doc
 *   VAL_|BSEQ|FLOW,              // begin seq as val, flow
 *   VAL_|SCLR|PLAI,      1, 1,   // val scalar, plain style, "a"  : string starts at offset 1 and has length 1
 *   VAL_|SCLR|PLAI|PSTR, 4, 2,   // val scalar, plain style, "bb" : string starts at offset 4 and has length 2; preceded by a string event (PSTR)
 *   VAL_|SCLR|PLAI|PSTR, 8, 3,   // val scalar, plain style, "ccc": string starts at offset 8 and has length 3; preceded by a string event (PSTR)
 *   ESEQ|PSTR,                   // end seq; preceded by a string event (PSTR)
 *   EDOC,                        // end doc
 *   ESTR,                        // end stream
 * };
 * ```
 *
 * Here is a sketch clarifying the meaning of this event sequence:
 *
@code
source  : [a, bb, ccc]
                                               has a string........
                                               |               offset "a"
                                               |               |  length "a"
                                               |               |  |
           event0   event1  event2 [           event3 "a"......|..|
           |        |       |                  |               |  |
(start)    +--------+-------+------------------+---------------+--+-----> (continued)
arr[i] :   BSTR     BDOC    VAL_|BSEQ|FLOW     VAL_|SCLR|PLAI..1..1
i      :   0        1       2                  3               4  5


                has a string.............              has a string.............
                |                    offset "bb"       |                    offset "ccc"
                |                    |  length "bb"    |                    |  length "ccc"
                |                    |  |              |                    |  |
                event4 "bb"..........|..|              event5 "ccc".........|..|
                |                    |  |              |                    |  |
 (cont)--> -----+--------------------+--+--------------+--------------------+--+-----> (continued)
arr[i] :        VAL_|SCLR|PLAI|PSTR..4..2              VAL_|SCLR|PLAI|PSTR..8..3
i      :        6              |     7  8              9              |     10 11
                               |                                      |
                               prev event has string                  prev event has string
                               (to get to prev, jump                  (to get to prev, jump
                               back 3 slots: ie 6->3)                 back 3 slots: ie 9->6)



                event6 ]      event7   event8
                |             |        |
 (cont)--> -----+-------------+--------+-----|    (end)
arr[i] :        ESEQ|PSTR     EDOC     ESTR
i      :        12   |        13       14
                     |
                     prev event has string
                     (to get to it, jump
                     back 3 slots: ie 12->9)
@endcode
 *
 * Note that the buffer contains both events (encoded as bit masks)
 * and strings (encoded as integer pairs). That is, events that have
 * an associated string are immediately followed by two integers
 * providing the offset and length of that string in the source
 * buffer. (In the example above, this happens in the events for the
 * strings `a`, `bb`, and `ccc` at positions 3, 6 and 9,
 * respectively).
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
 * that a jump of +3 should be employed to get to the next
 * event.
 *
 * Here's another example with the result of parsing `a: bb`
 * ```c++
 * const evt_bits arr[] = {       // result of parsing: `a: bb`
 *   BSTR,                        // begin stream
 *   BDOC,                        // begin doc
 *   VAL_|BMAP|BLCK,              // begin map as val, block
 *   KEY_|SCLR|PLAI,      0, 1,   // key scalar, plain style: "a"   starts at offset 0 and has length 1
 *   VAL_|SCLR|PLAI|PSTR, 3, 2,   // val scalar, plain style: "bb"  starts at offset 3 and has length 2
 *   EMAP|PSTR,                   // end map
 *   EDOC,                        // end doc
 *   ESTR,                        // end stream
 * };
 * ```
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
 * @{ */

/** data type for integer events size. This is set to an int32_t integer
 * to allow compatibility with a wide range of processing languages. */
using evt_size = int32_t;

/** data type for integer events bits. This is set to an int32_t integer
 * to allow compatibility with a wide range of processing languages. */
using evt_bits = int32_t;

/** enumeration of integer event bits. */
typedef enum : evt_bits { // NOLINT

    //-------------------------------------------------------------------------
    // YAML flags

    // YAML structure flags
    KEY_ = (1 <<  0),  ///< as key
    VAL_ = (1 <<  1),  ///< as value

    // YAML event scopes
    BEG_ = (1 <<  2),  ///< scope: begin
    END_ = (1 <<  3),  ///< scope: end
    SEQ_ = (1 <<  4),  ///< scope: seq
    MAP_ = (1 <<  5),  ///< scope: map
    DOC_ = (1 <<  6),  ///< scope: doc
    EXPL = (1 <<  7),  ///< `---` (with BDOC) or `...` (with EDOC)
    STRM = (1 <<  8),  ///< scope: stream
    BSEQ = BEG_|SEQ_,  ///< begin seq    (+SEQ in test suite events)
    ESEQ = END_|SEQ_,  ///< end seq      (-SEQ in test suite events)
    BMAP = BEG_|MAP_,  ///< begin map    (+MAP in test suite events)
    EMAP = END_|MAP_,  ///< end map      (-MAP in test suite events)
    BSTR = BEG_|STRM,  ///< begin stream (+STR in test suite events)
    ESTR = END_|STRM,  ///< end stream   (-STR in test suite events)
    BDOC = BEG_|DOC_,  ///< begin doc    (+DOC in test suite events)
    EDOC = END_|DOC_,  ///< end doc      (-DOC in test suite events)

    // YAML string events
    SCLR = (1 <<  9),  ///< scalar (=VAL in test suite events)
    ALIA = (1 << 10),  ///< *ref (reference)
    ANCH = (1 << 11),  ///< &anchor
    TAG_ = (1 << 12),  ///< !tag
    // directives
    YAML = (1 << 13),  ///< yaml directive: `\%YAML <version>`
    TAGH = (1 << 14),  ///< tag directive, handle: `\%TAG <handle> ........`
    TAGP = (1 << 15),  ///< tag directive, prefix: `\%TAG ........ <prefix>`

    // YAML style flags
    PLAI = (1 << 16),  ///< scalar: plain
    SQUO = (1 << 17),  ///< scalar: single-quoted (')
    DQUO = (1 << 18),  ///< scalar: double-quoted ("")
    LITL = (1 << 19),  ///< scalar: block literal (|)
    FOLD = (1 << 20),  ///< scalar: block folded (>)
    FLOW = (1 << 21),  ///< container: flow: [] for seqs or {} for maps
    BLCK = (1 << 22),  ///< container: block

    /// Special flag to mark a scalar as unfiltered (when the parser
    /// is set not to filter).
    UNFILT = (1 << 23),

    //-------------------------------------------------------------------------
    // NON-YAML FLAGS

    /// Special flag to mark events whose string was placed in the
    /// arena. This happens when the filtered string is larger than
    /// the original string in the YAML code (eg from tags that
    /// resolve to a larger string, or from "\L" or "\P" in double
    /// quotes, which expand from two to three bytes). Because of this
    /// size expansion, the filtered string cannot be placed in the
    /// original source and needs to be placed in the arena.
    AREN = (1 << 24),

    /// WithSTRing: mask of all events that encode a string following
    /// the event. For such events, the next two integers will provide
    /// respectively the string's offset and length. See also @ref PSTR
    WSTR = SCLR|ALIA|ANCH|TAG_|TAGH|TAGP|YAML,

    /// Special flag to enable look-back in the event array. It
    /// signifies that the previous event has a string, meaning that
    /// the jump back to that event is 3 positions. without this flag it
    /// would be impossible to jump to the previous event.
    /// see also @ref WSTR
    PSTR = (1 << 25),

    /// unused: reserved for future use (to enable rope-like buffers)
    JUMP = (1 << 26),
    /// unused: reserved for future use (same purpose as @ref PSTR,
    /// but for @ref JUMP)
    PJUMP = (1 << 27),

    // flow style flags
    FSL_ = (1 << 28), ///< same as @ref c4::yml::FLOW_SL
    FML1 = (1 << 29), ///< same as @ref c4::yml::FLOW_ML1
    FMLN = (1 << 30), ///< same as @ref c4::yml::FLOW_MLN
    FMLX = FML1|FMLN, ///< same as @ref c4::yml::FLOW_MLX
    FSPC = (1 << 31), ///< same as @ref c4::yml::FLOW_SPC

    /// the last flag defined above
    LAST = FSPC,

    /// a mask of all bits in this enumeration
    MASK = LAST < (1 << 31) ? (LAST << 1) - 1 : -1,

} EventBits;


C4_ALWAYS_INLINE evt_size nextpos(evt_bits bits) noexcept
{
    return (bits & ievt::WSTR) ? 3 : 1;
}
C4_ALWAYS_INLINE evt_size prevpos(evt_bits bits) noexcept
{
    return (bits & ievt::PSTR) ? 3 : 1;
}


C4_ALWAYS_INLINE evt_size nextpos(evt_bits bits, evt_size pos) noexcept
{
    return pos + ((bits & ievt::WSTR) ? 3 : 1);
}
C4_ALWAYS_INLINE evt_size prevpos(evt_bits bits, evt_size pos) noexcept
{
    return pos - ((bits & ievt::PSTR) ? 3 : 1);
}


C4_ALWAYS_INLINE evt_size nextpos(evt_bits const *C4_RESTRICT arr, evt_size pos) noexcept
{
    return pos + ((arr[pos] & ievt::WSTR) ? 3 : 1);
}
C4_ALWAYS_INLINE evt_size prevpos(evt_bits const *C4_RESTRICT arr, evt_size pos) noexcept
{
    return pos - ((arr[pos] & ievt::PSTR) ? 3 : 1);
}


struct evtbuf
{
    evt_bits *C4_RESTRICT ptr = {};
    evt_size              len = {};
    evt_size              cap = {};
};


struct Buffers
{
    substr    src   = {};
    substr    arena = {};
    evtbuf    evts  = {};
    bool      owned = {};
    Callbacks callbacks = {};
    csubstr getstr(evt_size pos) const RYML_NOEXCEPT
    {
        RYML_ASSERT_BASIC_(pos + 2 < evts.len);
        RYML_ASSERT_BASIC_(evts.ptr[pos] & ievt::WSTR);
        csubstr region = (evts.ptr[pos] & ievt::AREN) ? arena : src;
        RYML_ASSERT_BASIC_(static_cast<size_t>(evts.ptr[pos + 1]) < region.len);
        RYML_ASSERT_BASIC_(static_cast<size_t>(evts.ptr[pos + 1] + evts.ptr[pos + 2]) <= region.len);
        return {region.str + evts.ptr[pos + 1], static_cast<size_t>(evts.ptr[pos + 2])};
    }
    void destroy()
    {
        if(owned)
        {
            if(evts.ptr)
                callbacks.m_free(evts.ptr, static_cast<size_t>(evts.cap) * sizeof(evts.ptr[0]), callbacks.m_user_data);
            if(src.str)
                callbacks.m_free(src.str, src.len * sizeof(src.str[0]), callbacks.m_user_data);
            if(arena.str)
                callbacks.m_free(arena.str, arena.len * sizeof(arena.str[0]), callbacks.m_user_data);
        }
        evts = {};
        src = {};
        arena = {};
        callbacks = {};
        owned = false;
    }
};


/** Read YAML source and, without undergoing a full parse, estimate
 * the size of the integer buffer required for @ref
 * EventHandlerInts. This estimation is meant to exceed the actual
 * number of required events, and is typically used with @ref
 * EventHandlerIntsNoResize.
 *
 * @note This function must overpredict. It does so for every case in
 * the hundreds/thousands of extensive tests of rapidyaml. If you find
 * a case where that does not hold, it is a bug: please report it at
 * https://github.com/biojppm/rapidyaml/issues */
RYML_EXPORT evt_size estimate_events_size(csubstr src);


/** @} */ // doc_event_handlers_ints

} // namespace ievt

/** @cond dev */
// LCOV_EXCL_START
RYML_DEPRECATED("use ievt::estimate_events_size")
inline ievt::evt_size estimate_events_ints_size(csubstr src)
{
    return ievt::estimate_events_size(src);
}
// LCOV_EXCL_STOP
/** @endcond */

} // namespace extra

/** @cond dev */
namespace detail {
extra::ievt::evtbuf resize(extra::ievt::evtbuf buf, extra::ievt::evt_size sz, Callbacks const& cb);
substr resize(substr buf, size_t sz, Callbacks const& cb);
} // namespace detail
/** @endcond */

} // namespace yml
} // namespace c4



// NOLINTEND(hicpp-signed-bitwise,*avoid-c-style-cast)


#endif /* C4_YML_EXTRA_EVENT_INTS_HPP_ */
