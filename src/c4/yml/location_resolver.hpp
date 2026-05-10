#ifndef C4_YML_LOCATION_RESOLVER_HPP_
#define C4_YML_LOCATION_RESOLVER_HPP_

#ifndef C4_YML_COMMON_HPP__
#include "c4/yml/common.hpp"
#endif

namespace c4 {
namespace yml {

/** @addtogroup doc_parse
 * @{ */

struct LocationResolver
{
public:

    /** @name construction and assignment */
    /** @{ */

    LocationResolver() noexcept;
    LocationResolver(Callbacks const& cb) noexcept;
    ~LocationResolver() noexcept;

    LocationResolver(LocationResolver &&) noexcept;
    LocationResolver(LocationResolver const&);
    LocationResolver& operator=(LocationResolver &&) noexcept;
    LocationResolver& operator=(LocationResolver const&);

    /** @} */

public:

    /** @name modifiers */
    /** @{ */

    /** Prepare subsequent queries with a new source buffer. Does one
     * scan through the source buffer. */
    void reset(csubstr filename, csubstr src);

    size_t size() const { return m_size; }
    size_t capacity() const { return m_capacity; }

    /** Reserve a certain capacity for the array used to track node
     * locations in the source buffer. */
    void reserve(size_t num_source_lines);

    /** reallocates, keeping current data */
    void set_callbacks(Callbacks const& callbacks);

    /** @} */

public:

    /** @name location queries */
    /** @{ */

    /** Given a pointer to a buffer position, get the location.
     * @param[in] val must be pointing to somewhere in the source
     * buffer that was last parsed by this object. */
    Location val_location(const char *val, size_t linear_threshold=RYML_LOCATIONS_SMALL_THRESHOLD) const;

    /** Get the string starting at a particular location, to the end
     * of the parsed source buffer. */
    csubstr location_contents(Location const& loc) const;

    /** @} */

private:

    void _free();
    void _clr();
    void _resize(size_t sz, bool keep);

public:

    csubstr   m_filename;
    csubstr   m_src;
    size_t   *m_offsets;  ///< offset of newline characters in the source bubber
    size_t    m_size;     ///< size of the offsets buffer
    size_t    m_capacity; ///< capacity of the offsets buffer
    Callbacks m_callbacks;

};


/** @} */

} // namespace yml
} // namespace c4

// NOLINTEND(hicpp-signed-bitwise)

#if defined(_MSC_VER)
#   pragma warning(pop)
#endif

#endif /* C4_YML_LOCATION_RESOLVER_HPP_ */
