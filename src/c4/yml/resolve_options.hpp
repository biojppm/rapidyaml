#ifndef C4_YML_RESOLVE_OPTIONS_HPP_
#define C4_YML_RESOLVE_OPTIONS_HPP_

#ifndef C4_YML_COMMON_HPP_
#include <c4/yml/common.hpp>
#endif

namespace c4 {
namespace yml {


/** Options to control reference resolving. See @ref ReferenceResolver
 * and @ref IntsReferenceResolver */
struct ResolveOptions
{
private:

    typedef enum : uint32_t {
        CLEAR_ANCHORS = 1u << 0u,
        PICK_LAST = 1u << 1u,
        PRIORITY_MERGEDIN = 1u << 2u,
        PRIORITY_ORIGINAL = 1u << 3u,
        MERGE_REPEATED_KEYS = 1u << 4u,
        DEFAULT_FLAGS = CLEAR_ANCHORS|PRIORITY_ORIGINAL,
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

    /** @name pick_last
     *
     * When picking among same-key children, select the last possible
     * candidate. The standard behavior is picking the first. Defaults
     * to false (pick first).
     *
     * See https://yamlvalidate.com/blog/yaml-anchors-aliases-merge-keys/
     *
     * @{
     *
     */

    /// docs to the right ---->

    C4_ALWAYS_INLINE bool pick_last() const noexcept { return (m_flags & PICK_LAST) != 0; } /// always pick the last same-key child. default is false (the standard behavior: pick the first same-key child).
    ResolveOptions& pick_last(bool enabled) noexcept { set_flag_(enabled, PICK_LAST); return *this; } /// always pick the last same-key child. default is false. (the standard behavior: pick the first same-key child)

    /** @} */

public:

    /** @name priority
     *
     * Controls how children with same-key are selected when merging a
     * map. Defaults to priority_original, meaning give priority to
     * children originally in the map. Otherwise, there is
     * priority_mergedin, meaning priority to children added to the
     * map via merge operator `<<`. If neither or both of
     * priority_original and priority_mergedin are set, the behavior
     * is no-priority.
     *
     * See https://yamlvalidate.com/blog/yaml-anchors-aliases-merge-keys/
     *
     * @{
     *
     */

    /// docs to the right ---->

    C4_ALWAYS_INLINE bool priority_original() const noexcept { return (m_flags & PRIORITY_ORIGINAL) != 0; } ///< with same-key, give priority to original children (ie direct map children explicitly present).
    ResolveOptions& priority_original(bool enabled) noexcept { set_flag_(enabled, PRIORITY_ORIGINAL); return *this; } ///< with same-key, give priority to original children (ie direct map children explicitly present).

    C4_ALWAYS_INLINE bool priority_mergedin() const noexcept { return (m_flags & PRIORITY_MERGEDIN) != 0; } ///< with same-key, give priority to merged-in children (ie inherited-in via <<).
    ResolveOptions& priority_mergedin(bool enabled) noexcept { set_flag_(enabled, PRIORITY_MERGEDIN); return *this; } ///< with same-key, give priority to merged-in children (ie inherited-in via <<).

    /** @} */

public:

    /** @name merged repeated keys
     *
     * Controls whether repeated keys should be merged even if the map
     * has no merge element children (<<). Defaults to false.
     *
     * @{
     *
     */

    ResolveOptions& merge_repeated_keys(bool clear) noexcept { set_flag_(clear, MERGE_REPEATED_KEYS); return *this; }
    C4_ALWAYS_INLINE bool merge_repeated_keys() const noexcept { return (m_flags & MERGE_REPEATED_KEYS) != 0; }

    /** @} */

public:

    /** @name max_multiplier
     *
     * Set a limit to the expansion of a resolved tree, to avoid
     * exponential expansion attacks such as the [Billion Laughs
     * Attack](https://en.wikipedia.org/wiki/Billion_laughs_attack).
     * This is done via a multiplier, causing ryml to trigger an error
     * if the new number of nodes exceeds the multiplied original. Set
     * the multiplier to 0 to disable the limit. Defaults to 50.
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
