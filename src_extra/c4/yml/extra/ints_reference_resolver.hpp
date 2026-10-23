#ifndef C4_YML_EXTRA_INTS_REFERENCE_RESOLVER_HPP_
#define C4_YML_EXTRA_INTS_REFERENCE_RESOLVER_HPP_

#ifndef C4_YML_EXTRA_EVENT_INTS_HPP_
#include "c4/yml/extra/event_ints.hpp"
#endif
#ifndef C4_YML_RESOLVE_OPTIONS_HPP_
#include "c4/yml/resolve_options.hpp"
#endif
#ifndef C4_YML_DETAIL_SMALL_MAP_HPP_
#include "c4/yml/detail/small_map.hpp"
#endif


namespace c4 {
namespace yml {
namespace extra {
namespace ievt {

/** @addtogroup doc_event_handlers_ints
 * @{ */

struct RYML_EXPORT ReferenceResolver : private Buffers
{
public:

    ReferenceResolver(Callbacks const& cb={});

    void resolve_resize(Buffers *evts_, ResolveOptions const& opts_={});
    C4_NODISCARD evt_size resolve_noresize(Buffers evts_, ResolveOptions const& opts_={});

    void clear_data() { anchor_map.clear(); }

private:

    struct Range_
    {
        evt_size beg; /// beginning pos of the range
        evt_size end; /// end pos of the range (past 1)
        evt_size len() const noexcept { return end - beg; }
    };
    struct AnchorTargetRange_ : Range_
    {
        evt_size pos; /// anchor pos
    };
    struct KnownKey_
    {
        evt_size id;    /// pos of the key in the evt array
        evt_size count; /// count of other keys when this was added
        bool original;
        bool copied;
    };
    template<class K, class V> using map = yml::detail::small_map<K,V>;
    template<class T, size_t N> using vec = yml::detail::stack<T, N>;

private:

    evt_size wpos;
    evt_size newsz;
    evt_size sz_limit;
    ResolveOptions opts;
    map<csubstr, AnchorTargetRange_> anchor_map;
    map<csubstr, KnownKey_> known_keys;
    vec<evt_size, 32> parents;
    vec<evt_size, 32> siblings;
    evt_size num_known_keys;
    bool can_resize;

private:

    void resolve_(Buffers *evts_, ResolveOptions const& opts_);
    void reset_(Buffers evts_, bool can_resize_, ResolveOptions opts_);

    C4_NODISCARD evt_size next_sibling_memoized_(evt_size pos);
    C4_NODISCARD evt_size matching_close_memoized_(evt_size pos);
    C4_NODISCARD bool is_cyclic_ref_(evt_size anchor) const RYML_NOEXCEPT;
    C4_NODISCARD Range_ val_range_(evt_size keyval);

    C4_NODISCARD bool descend_(evt_size &pos);
    C4_NODISCARD evt_size resolve_children_seq_(evt_size pos);
    C4_NODISCARD evt_size resolve_children_map_(evt_size parent);
    void add_anchor_(evt_size pos);
    AnchorTargetRange_ lookup_anchor_(evt_size pos) const;
    C4_NODISCARD evt_size count_extra(evt_size beg, evt_size end) const noexcept;
    void set_noref_(evt_size pos);
    void replace_with_ref_(evt_size pos, evt_size beg, evt_size end);
    void set_rref_(evt_size pos, evt_size beg, evt_size end);
    void clear_str_(evt_size pos);
    void clear_str2_(evt_size pos);
    bool maybe_clear_anchtag_(evt_size pos);
    void rem_entry_(evt_size pos);
    void rem_keyval_(evt_size pos_key);
    bool is_merge_elm_(evt_size pos, csubstr key) const;
    void handle_merge_elm_(evt_size child);
    void merge_map_single_(evt_size pos);
    void merge_map_multiple_(evt_size pos);
    void apply_merge_ref_(evt_size pos);
    bool needs_merge_key_by_key_(evt_size beg, evt_size end);
    void add_known_key_(csubstr key, evt_size id, bool original, bool copied);
    void add_to_known_keys_(evt_size beg, evt_size end);
    void add_merge_elms_(evt_size beg, evt_size end);
    void overwrite_keyval_(evt_size src, evt_size dst);
    void copy_keyval_to_end_(evt_size keyval);
    bool grow_evts_to_add_(evt_size extra);

public:

    ~ReferenceResolver() noexcept = default;
    ReferenceResolver(ReferenceResolver&) = delete;
    ReferenceResolver(ReferenceResolver const&) = delete;
    ReferenceResolver& operator= (ReferenceResolver&) = delete;
    ReferenceResolver& operator= (ReferenceResolver const&) = delete;

};

/** @} */

} // namespace ievt
} // namespace extra
} // namespace yml
} // namespace c4

#endif /* C4_YML_EXTRA_INTS_REFERENCE_RESOLVER_HPP_ */
