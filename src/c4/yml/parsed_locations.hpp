#ifndef _C4_YML_PARSED_LOCATION_HPP_
#define _C4_YML_PARSED_LOCATION_HPP_

#ifndef _C4_YML_COMM_HPP__
#include "c4/yml/common.hpp"
#endif

namespace c4 {
namespace yml {

/** @addtogroup doc_parse
 * @{ */

class ParsedLocations
{
public:

    /** @name construction and assignment */
    /** @{ */

    ParsedLocations();
    ~ParsedLocations() noexcept;

    ParsedLocations(ParsedLocations &&) noexcept;
    ParsedLocations(ParsedLocations const&);
    ParsedLocations& operator=(ParsedLocations &&) noexcept;
    ParsedLocations& operator=(ParsedLocations const&);

    /** @} */

public:

    /** @name modifiers */
    /** @{ */

    void reset(csubstr filename, csubstr src);

    /** Reserve a certain capacity for the array used to track node
     * locations in the source buffer. */
    void reserve(size_t num_source_lines)
    {
        _resize(num_source_lines);
    }

    size_t capacity() const { return m_newline_offsets_capacity; }

    /** @} */

public:

    /** @name locations */
    /** @{ */

    /** Get the string starting at a particular location, to the end
     * of the parsed source buffer. */
    csubstr location_contents(Location const& loc) const;

    /** Given a pointer to a buffer position, get the location.
     * @param[in] val must be pointing to somewhere in the source
     * buffer that was last parsed by this object. */
    Location val_location(const char *val, Callbacks const& cb, size_t linear_threshold=RYML_LOCATIONS_SMALL_THRESHOLD) const;
    Location val_location(const char *val, size_t linear_threshold=npos) const
    {
        return val_location(val, get_callbacks(), linear_threshold);
    }

    /** @} */

private:

    void _free();
    void _clr();
    void _resize(size_t sz);

private:

    csubstr m_filename;
    csubstr m_src;
    size_t *m_newline_offsets;
    size_t  m_newline_offsets_size;
    size_t  m_newline_offsets_capacity;

};


/** @} */

} // namespace yml
} // namespace c4

// NOLINTEND(hicpp-signed-bitwise)

#if defined(_MSC_VER)
#   pragma warning(pop)
#endif

#endif /* _C4_YML_PARSE_ENGINE_HPP_ */
