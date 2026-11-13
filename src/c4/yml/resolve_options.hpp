#ifndef C4_YML_RESOLVE_OPTIONS_HPP_
#define C4_YML_RESOLVE_OPTIONS_HPP_

#ifndef C4_YML_COMMON_HPP_
#include <c4/yml/common.hpp>
#endif

namespace c4 {
namespace yml {


/** Options to control map key merging and anchor/reference
 * resolving. See @ref c4::yml::ReferenceResolver (for tree)  */
struct ResolveOptions
{
private:

    typedef enum : uint32_t { // NOLINT
        CLEAR_ANCHORS = 1u << 0u,
        WHOLE_SCOPE = 1u << 1u,
        IGNORE_MISSING_ANCHORS = 1u << 2u,
        ACCEPT_MULTIPLE_ANCHORS = 1u << 3u,
        PICK_LAST = 1u << 4u,
        PRIORITY_MERGEDIN = 1u << 5u,
        PRIORITY_ORIGINAL = 1u << 6u,
        KEEP_REPEATED_KEYS = 1u << 7u,
        MERGE_ALWAYS = 1u << 8u,
        ERR_ON_REPEATED_KEYS_ORIGINAL = 1u << 9u|MERGE_ALWAYS,
        ERR_ON_REPEATED_KEYS_MERGEDIN = 1u << 10u,
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

    ResolveOptions& max_size_multiplier(uint32_t mult) noexcept { m_max_size_multiplier = mult; return *this; }
    C4_ALWAYS_INLINE uint32_t max_size_multiplier() const noexcept { return m_max_size_multiplier; }

    /// get the max size based on a starting size
    template<class T>
    C4_ALWAYS_INLINE T max_size(T size_orig) const noexcept { return size_orig * static_cast<T>(m_max_size_multiplier); }

    /** @cond dev */
    #if (C4_WORDSIZE == 8)
    RYML_DEPRECATED("use the uint32_t overload") ResolveOptions& max_size_multiplier(size_t mult) noexcept { m_max_size_multiplier = static_cast<uint32_t>(mult); return *this; }
    #endif
    /** @endcond */

    /** @} */

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

    /** @name extension: keep repeated keys
     *
     * Controls whether repeated keys should be kept or merged. When
     * set to true, no key merging is done, and all repeated keys are
     * kept in place. When false, keys are merged such that a single
     * value is selected; the selected entry will be placed at the
     * earliest appearance of the repeated key. Defaults to false (the
     * standard behavior).
     *
     * @{
     *
     */

    ResolveOptions& keep_repeated_keys(bool clear) noexcept { set_flag_(clear, KEEP_REPEATED_KEYS); return *this; }
    C4_ALWAYS_INLINE bool keep_repeated_keys() const noexcept { return (m_flags & KEEP_REPEATED_KEYS) != 0; }

    /** @} */

public:

    /** @name extension: error on repeated keys
     *
     * Controls whether an error should be thrown when a map has
     * repeated keys. Defaults to false (the non-standard behavior).
     *
     * @{
     *
     */

    ResolveOptions& err_on_repeated_keys_original(bool clear) noexcept { set_flag_(clear, ERR_ON_REPEATED_KEYS_ORIGINAL); return *this; }
    C4_ALWAYS_INLINE bool err_on_repeated_keys_original() const noexcept { return (m_flags & ERR_ON_REPEATED_KEYS_ORIGINAL) == ERR_ON_REPEATED_KEYS_ORIGINAL; }

    ResolveOptions& err_on_repeated_keys_mergedin(bool clear) noexcept { set_flag_(clear, ERR_ON_REPEATED_KEYS_MERGEDIN); return *this; }
    C4_ALWAYS_INLINE bool err_on_repeated_keys_mergedin() const noexcept { return (m_flags & ERR_ON_REPEATED_KEYS_MERGEDIN) != 0; }

    /** @} */

public:

    /** @name merge always
     *
     * When set to merge repeated keys (see @ref
     * ResolveOptions::keep_repeated_keys), do it also for maps
     * without `<<` merge elements. When true, all maps will be
     * checked for repeated keys (at a slight processing cost). and
     * not just maps containing merge elements. The keys will then be
     * merged according to @ref ResolveOptions::pick_last. This helps
     * eliminate repeated keys, ensuring that maps adhere to the
     * standard, which specifies unique keys. Defaults to false (the
     * non-standard behavior).
     *
     * @{
     *
     */

    ResolveOptions& merge_always(bool clear) noexcept { set_flag_(clear, MERGE_ALWAYS); return *this; }
    C4_ALWAYS_INLINE bool merge_always() const noexcept { return (m_flags & MERGE_ALWAYS) != 0; }

    /** @} */

public:

    /** @name extension: pick_last
     *
     * Selects overwrite mode when merging repeated keys. When true,
     * select the last possible candidate (ie, later entries overwrite
     * earlier entries). The standard behavior is picking the first
     * entry, which prevails over any later entries. Defaults to false
     * (pick first).
     *
     * See https://yamlvalidate.com/blog/yaml-anchors-aliases-merge-keys/
     *
     * @{
     *
     */

    C4_ALWAYS_INLINE bool pick_last() const noexcept { return (m_flags & PICK_LAST) != 0; }
    ResolveOptions& pick_last(bool enabled) noexcept { set_flag_(enabled, PICK_LAST); return *this; }

    /** @} */

public:

    /** @name extension: priority
     *
     * When merging a map, controls how repeated keys are
     * selected. Defaults to priority_original, meaning give priority
     * to children originally in the map (as specified by the YAML
     * standard). Otherwise, there is priority_mergedin, meaning give
     * priority to children added to the map via merge operator
     * `<<`. If neither or both are set, the behavior is no-priority,
     * deferring to position-based @ref ResolveOptions::pick_last().
     *
     * See https://yamlvalidate.com/blog/yaml-anchors-aliases-merge-keys/
     *
     * @{
     *
     */

    C4_ALWAYS_INLINE bool priority_original() const noexcept { return (m_flags & PRIORITY_ORIGINAL) != 0; }
    ResolveOptions& priority_original(bool enabled) noexcept { set_flag_(enabled, PRIORITY_ORIGINAL); return *this; }

    C4_ALWAYS_INLINE bool priority_mergedin() const noexcept { return (m_flags & PRIORITY_MERGEDIN) != 0; }
    ResolveOptions& priority_mergedin(bool enabled) noexcept { set_flag_(enabled, PRIORITY_MERGEDIN); return *this; }

    /** @} */

public:

    /** @name extension: whole_scope
     *
     * Controls whether anchors are visible everywhere in the
     * tree. The standard dictates that the anchor is visible only to
     * nodes after the anchor in the serialization; this setting makes
     * the anchor visible everywhere in the tree, thus making it
     * visible even by nodes before the anchor. Defaults to false (the
     * standard behavior).
     *
     * @{
     *
     */

    ResolveOptions& whole_scope(bool clear) noexcept { set_flag_(clear, WHOLE_SCOPE); return *this; }
    C4_ALWAYS_INLINE bool whole_scope() const noexcept { return (m_flags & WHOLE_SCOPE) != 0; }

    /** @} */

public:

    /** @name extension: ignore_missing_anchors
     *
     * Controls whether missing errors are ignored. When set to true,
     * no error is reported when an anchor is not found. Defaults to
     * false.
     *
     * @{
     *
     */

    ResolveOptions& ignore_missing_anchors(bool clear) noexcept { set_flag_(clear, IGNORE_MISSING_ANCHORS); return *this; }
    C4_ALWAYS_INLINE bool ignore_missing_anchors() const noexcept { return (m_flags & IGNORE_MISSING_ANCHORS) != 0; }

    /** @} */

public:

    /** @name extension: accept_multiple_anchors
     *
     * Controls whether multiple anchors are accepted. When set to
     * true, no error is reported when multiple anchors are found, and
     * ryml will pick the closest anchor. Defaults to false.
     *
     * @{
     *
     */

    ResolveOptions& accept_multiple_anchors(bool clear) noexcept { set_flag_(clear, ACCEPT_MULTIPLE_ANCHORS); return *this; }
    C4_ALWAYS_INLINE bool accept_multiple_anchors() const noexcept { return (m_flags & ACCEPT_MULTIPLE_ANCHORS) != 0; }

    /** @} */

private:

    uint32_t m_max_size_multiplier = 50;
    uint32_t m_flags = DEFAULT_FLAGS;

};

} // namespace yml
} // namespace c4

#endif /* C4_YML_RESOLVE_OPTIONS_HPP_ */
