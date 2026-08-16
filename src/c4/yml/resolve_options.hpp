#ifndef C4_YML_RESOLVE_OPTIONS_HPP_
#define C4_YML_RESOLVE_OPTIONS_HPP_

#ifndef C4_YML_COMMON_HPP_
#include <c4/yml/common.hpp>
#endif

namespace c4 {
namespace yml {


/** Options to control reference resolving. See @ref ReferenceResolver */
struct ResolveOptions
{
public:

    /** @name clear anchors
     *
     * Controls whether existing anchors should be cleared after
     * resolving. Defaults to true.
     *
     * @{
     *
     */

    ResolveOptions& clear_anchors(bool clear) noexcept { m_clear_anchors = clear; return *this; }
    C4_ALWAYS_INLINE bool clear_anchors() const noexcept { return m_clear_anchors; }

    /** @} */

public:

    /** @name max_multiplier
     *
     * Set a limit to the size of the resolved tree, to avoid
     * exponential expansion attacks such as the [Billion Laughs
     * Attack](https://en.wikipedia.org/wiki/Billion_laughs_attack). This
     * is done via a multiplier, triggering an error if the new number
     * of nodes exceeds the multiplied original. Set the multiplier to
     * 0 to disable the limit. Defaults to 50.
     *
     * @{
     *
     */

    ResolveOptions& max_size_multiplier(id_type mult) noexcept { m_max_size_multiplier = mult; return *this; }
    C4_ALWAYS_INLINE id_type max_size_multiplier() const noexcept { return m_max_size_multiplier; }

    /** @} */

private:

    id_type m_max_size_multiplier = 50;
    bool m_clear_anchors = true;

};


} // namespace yml
} // namespace c4

#endif /* C4_YML_RESOLVE_OPTIONS_HPP_ */
