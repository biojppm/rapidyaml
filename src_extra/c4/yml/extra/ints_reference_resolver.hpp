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

    struct AnchorTargetRange_
    {
        evt_size beg; /// beginning pos of the range
        evt_size end; /// end pos of the range (past 1)
        evt_size pos; /// anchor pos
    };
    template<class K, class V> using map = yml::detail::small_map<K,V>;
    template<class T, size_t N> using vec = yml::detail::stack<T, N>;

private:

    evt_size wpos;
    evt_size newsz;
    evt_size sz_limit;
    ResolveOptions opts;
    map<csubstr, AnchorTargetRange_> anchor_map;
    map<csubstr, evt_size> merge_map;
    bool can_resize;

private:

    void resolve_(Buffers *evts_, ResolveOptions const& opts_);
    void reset_(Buffers evts_, bool can_resize_, ResolveOptions opts_);
    C4_NODISCARD bool descend_(evt_size &pos);
    C4_NODISCARD evt_size resolve_children_seq_(evt_size pos);
    C4_NODISCARD evt_size resolve_children_map_(evt_size parent);
    void add_anchor_(evt_size pos);
    AnchorTargetRange_ lookup_anchor_(evt_size pos) const;
    evt_size count_extra(evt_size beg, evt_size end) const noexcept;
    void set_rref_(evt_size pos, evt_size beg, evt_size end);
    void set_rref_to_anchor_(evt_size pos, evt_size beg, evt_size end);
    void clear_str_(evt_size pos);
    bool is_merge_map_(evt_size pos, csubstr key) const;
    void handle_merge_map_(evt_size child);
    void merge_map_single_(evt_size pos);
    void merge_map_multiple_(evt_size pos);
    void apply_merge_ref_(evt_size pos, bool more);
    bool needs_merge_key_by_key_(evt_size beg, evt_size end);
    void add_merge_elms_(evt_size beg, evt_size end, bool copy_nonexisting, bool more_to_add);
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
