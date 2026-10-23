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
private:

    typedef enum : uint32_t {
        CLEAR_ANCHORS = 1u << 0u,
        OVERWRITE_EXISTING = 1u << 1u,
        OVERWRITE_ORIGINAL = 1u << 2u,
        DEFAULT_FLAGS = CLEAR_ANCHORS,
    } Flags_e;

private:

    ResolveOptions& set_flag_(bool enabled, uint32_t f)
    {
        if(enabled)
            m_flags |= f;
        else
            m_flags &= ~f;
        return *this;
    }

public:

    /** @name clear anchors
     *
     * Controls whether existing anchors should be cleared after
     * resolving. Defaults to true.
     *
     * @{
     *
     */

    ResolveOptions& clear_anchors(bool clear) noexcept { set_flag_(clear, CLEAR_ANCHORS); return *this; }
    C4_ALWAYS_INLINE bool clear_anchors() const noexcept { return (m_flags & CLEAR_ANCHORS) != 0; }

    /** @} */

public:

    /** @name merge_overwrites
     *
     * Controls overwrite behavior when merging nodes.
     *
     * See https://yamlvalidate.com/blog/yaml-anchors-aliases-merge-keys/
     *
     * @{
     *
     */

    // docs to the right >>>>

    C4_ALWAYS_INLINE bool overwrite_original() const noexcept { return (m_flags & OVERWRITE_ORIGINAL) != 0; } /// enable overwriting of nodes originally in the target map. default is false.
    ResolveOptions& overwrite_original(bool enabled) noexcept { set_flag_(enabled, OVERWRITE_ORIGINAL); return *this; } /// enable overwriting of nodes originally in the target map. default is false.

    C4_ALWAYS_INLINE bool overwrite_existing() const noexcept { return (m_flags & OVERWRITE_EXISTING) != 0; } ///< enable overwriting of nodes merged earlier, ie non-original nodes. default is false.
    ResolveOptions& overwrite_existing(bool enabled) noexcept { set_flag_(enabled, OVERWRITE_EXISTING); return *this; } ///< enable overwriting of nodes merged earlier, ie non-original nodes. default is false.

    ResolveOptions& set_overwrite_standard() noexcept { set_flag_(false, OVERWRITE_EXISTING|OVERWRITE_ORIGINAL); return *this; } /// set the standard behaviour when merging.
    ResolveOptions& set_overwrite_latest() noexcept { set_flag_(true, OVERWRITE_EXISTING|OVERWRITE_ORIGINAL); return *this; } /// set to the non-standard behaviour of choosing latest, ie overwriting both existing and original nodes while merging.

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
    C4_ALWAYS_INLINE id_type max_size(id_type size_orig) const noexcept { return m_max_size_multiplier > 1 ? m_max_size_multiplier * size_orig : 0u; }

    /** @} */

private:

    id_type m_max_size_multiplier = 50;
    uint32_t m_flags = DEFAULT_FLAGS;

};


} // namespace yml
} // namespace c4

#endif /* C4_YML_RESOLVE_OPTIONS_HPP_ */
