#ifndef C4_YML_DETAIL_LOOKUP_ANCHOR_HPP_
#define C4_YML_DETAIL_LOOKUP_ANCHOR_HPP_

#ifndef C4_YML_COMMON_HPP_
#include <c4/yml/common.hpp>
#endif
#ifndef C4_YML_RESOLVE_OPTIONS_HPP_
#include <c4/yml/resolve_options.hpp>
#endif

#include <cstdio>

namespace c4 {
namespace yml {

namespace detail {
/** @cond dev */
// lookup for an anchor in an anchor map. This is extracted to a
// function because the logic is made tricky by all the relevant
// ResolveOptions
template<class AnchorMap>
typename AnchorMap::mapped_type const*
lookup_anchor(AnchorMap const& anchors, id_type visit_count, csubstr ref, ResolveOptions const& opts)
{
    auto r = anchors.equal_range(ref);
    RYML_ASSERT_BASIC_CB_(anchors.m_arr.m_callbacks, r.hi >= r.lo);
    id_type num_anchors = static_cast<id_type>(r.hi - r.lo);
printf("lookup_anchor[%.*s]: visit_count=%zu #anchors=%zu\n", static_cast<int>(ref.len), ref.str, visit_count, num_anchors);
    if C4_LIKELY (!!num_anchors)
    {
        if(num_anchors == 1)
        {
printf("lookup_anchor[%.*s]: 1: visit_count=%zu vs %zu=anchor\n", static_cast<int>(ref.len), ref.str, visit_count, r.lo->val.visit_count);
            if(visit_count >= r.lo->val.visit_count || opts.whole_scope())
                return &r.lo->val;
        }
        // there are multiple candidate anchors
        else if(opts.accept_multiple_anchors())
        {
            auto const* candidate = r.hi - 1;
            // try standard behavior first:
            for( ; candidate != r.lo - 1; --candidate)
            {
                if(candidate->val.visit_count <= visit_count)
                    return &candidate->val;
            }
            if(opts.whole_scope()) // pick the closest
            {
                // get a suitable value to find the minimum delta in
                // visit_count along the range. we're not using
                // std::numeric_limits::max() to avoid the include
                id_type mindelta = visit_count;
                for(auto const* c = r.lo; c != r.hi; ++c)
                    if(c->val.visit_count > mindelta)
                        mindelta = c->val.visit_count;
                mindelta = 10 * (1 + mindelta); // defend against zero
                candidate = nullptr;
                for(auto const* c = r.lo; c != r.hi; ++c)
                {
                    id_type delta = c->val.visit_count <= visit_count
                        ? visit_count - c->val.visit_count
                        : c->val.visit_count - visit_count;
                    if(delta < mindelta)
                    {
                        candidate = c;
                        mindelta = delta;
                    }
                }
                if(candidate)
                    return &candidate->val;
            }
        }
        else
        {
            RYML_ERR_BASIC_CB_(anchors.m_arr.m_callbacks, "multiple anchors: {}", ref);
        }
    }
    if(opts.ignore_missing_anchors())
        return nullptr;
    RYML_ERR_BASIC_CB_(anchors.m_arr.m_callbacks, "anchor not found: {}", ref);
}
/** @endcond */

} // namespace detail
} // namespace yml
} // namespace c4

#endif /* C4_YML_DETAIL_LOOKUP_ANCHOR_HPP_ */
