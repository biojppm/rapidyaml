#ifndef C4_YML_EXTRA_INTS_REFERENCE_RESOLVER_HPP_
#define C4_YML_EXTRA_INTS_REFERENCE_RESOLVER_HPP_

#ifndef C4_YML_EXTRA_EVENT_INTS_HPP_
#include "c4/yml/extra/event_ints.hpp"
#endif
#ifndef C4_YML_DETAIL_KNOWN_KEYS_HPP_
#include "c4/yml/detail/known_keys.hpp"
#endif

#ifndef RYML_IEVT_RESOLVER_SSO_SIZE
#define RYML_IEVT_RESOLVER_SSO_SIZE (32)
#endif

C4_SUPPRESS_WARNING_MSVC_WITH_PUSH(4251) // needs to have dll-interface to be used by clients


namespace c4 {
namespace yml {
namespace extra {
namespace ievt {

/** @addtogroup doc_event_handlers_ints
 * @{ */

struct RYML_EXPORT ReferenceResolver : private Buffers
{
public:

    ReferenceResolver() noexcept : ReferenceResolver(get_callbacks()) {}
    explicit ReferenceResolver(Callbacks const& cb) noexcept;
    ReferenceResolver(ReferenceResolver&&) = delete;
    ReferenceResolver(ReferenceResolver const&) = delete;
    ReferenceResolver& operator= (ReferenceResolver&&) = delete;
    ReferenceResolver& operator= (ReferenceResolver const&) = delete;
    ~ReferenceResolver() noexcept = default;

public:

    void resolve_resize(Buffers *evts_, ResolveOptions const& opts_={});

private:

    struct Range_
    {
        evt_size beg; /// beginning pos of the range
        evt_size end; /// end pos of the range (past 1)
    };
    struct AnchorData
    {
        id_type  visit_count; /// the pos of the anchor
        Range_   rng;         /// range of the anchored data
        evt_size removed;     /// mark as removed (to avoid clearing when at the end)
    };

    using KnownKeys = yml::detail::KnownKeys;
    using AnchorMap = yml::detail::small_map<csubstr, AnchorData, RYML_IEVT_RESOLVER_SSO_SIZE>;
    template<class T> using vec = yml::detail::stack<T, RYML_IEVT_RESOLVER_SSO_SIZE>;

private:

    AnchorMap m_anchor_map;        // anchor lookup helper
    KnownKeys m_known_keys;        // key map, used to merge keys within a map
    vec<evt_size> m_parents;       // parents in a call stack
    vec<evt_size> m_siblings;      // memoization of siblings
    vec<evt_size> m_mapw_scratch;  // map-writer scratch size
    ResolveOptions m_opts;
    evt_size m_wpos;
    evt_size m_newsz;
    evt_size m_sz_limit;
    evt_size m_mapw_firstslot;
    evt_size m_mapw_lastslot;
    evt_size m_mapw_wslot; // current write slot
    evt_size m_mapw_posref;

private: // mapw: map-writing helpers to merge keys

    void mapw_reset_(evt_size map);
    void mapw_cpkeyval_original_(evt_size id, bool is_last_entry);
    void mapw_cpkeyval_mergedin_(evt_size id, bool is_last_entry);
    evt_size mapw_cpdata_(evt_bits const* C4_RESTRICT data, evt_size beg, evt_size end, bool is_last_entry);

private:

    void resolve_(Buffers *evts_, ResolveOptions const& opts_);
    void reset_(Buffers evts_, ResolveOptions opts_);
    void reset_siblings_range_(evt_size beg, evt_size end);
    bool gather_anchors_();
    void iter_elms_();

    C4_NODISCARD evt_size next_sibling_memoized_(evt_size pos);
    C4_NODISCARD evt_size matching_close_memoized_(evt_size pos);
    C4_NODISCARD bool is_cyclic_ref_(evt_size anchor) const RYML_NOEXCEPT;
    C4_NODISCARD Range_ elm_range_(evt_size keyval);
    C4_NODISCARD Range_ elm_range_scratch_(evt_size keyval);
    C4_NODISCARD Range_ keyval_range_(evt_size keyval);
    C4_NODISCARD Range_ keyval_range_scratch_(evt_size keyval);
    C4_NODISCARD Range_ keyval_range_val_(evt_size keyval);

    C4_NODISCARD bool descend_(evt_size &pos);
    C4_NODISCARD evt_size resolve_children_seq_(evt_size pos);
    C4_NODISCARD evt_size resolve_children_map_(evt_size parent);
    evt_size resolve_map_merge_(evt_size parent, bool key_conflict);
    C4_NODISCARD evt_size resolve_map_repeat_(evt_size parent);
    C4_NODISCARD bool requires_merge_(evt_size map);
    C4_NODISCARD bool has_merge_elms_(evt_size map);
    C4_NODISCARD evt_size merge_map_keys_(evt_size map);
    void add_anchor_(evt_size pos);
    AnchorData const* lookup_anchor_(evt_size pos) const;
    C4_NODISCARD evt_size count_extra(evt_size beg, evt_size end) const noexcept;
    void set_noref_(evt_size pos);
    void replace_with_ref_(evt_size pos, evt_size beg, evt_size end);
    void set_rref_(evt_size pos, evt_size beg, evt_size end);
    void clear_str_(evt_size pos);
    void clear_str2_(evt_size pos);
    void clear_range_(evt_size beg, evt_size end) noexcept;
    void clear_keyval_(evt_size src) noexcept;
    bool maybe_clear_anchtag_(evt_size pos);
    void rem_anchor_from_map_(evt_size pos);
    bool is_merge_elm_(evt_size pos) const;
    bool is_merge_elm_(evt_size pos, csubstr key) const;
    bool is_any_merge_elm_alia_missing_(evt_size pos) const;
    void handle_merge_elm_(evt_size child);
    void merge_map_single_(evt_size pos);
    void merge_map_multiple_(evt_size pos);
    void apply_merge_ref_(evt_size pos);
    void add_to_known_keys_(evt_size beg, evt_size end, uint32_t status);
    void add_to_known_keys_(evt_size alia, uint32_t status);
    void copy_range_to_end_(evt_bits const* mem, evt_size beg, evt_size end);
    bool grow_evts_to_add_(evt_size extra);

};

/** @} */

} // namespace ievt
} // namespace extra
} // namespace yml
} // namespace c4

C4_SUPPRESS_WARNING_MSVC_POP

#endif /* C4_YML_EXTRA_INTS_REFERENCE_RESOLVER_HPP_ */
