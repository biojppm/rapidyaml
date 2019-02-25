#ifndef _C4_YML_EMIT_HPP_
#define _C4_YML_EMIT_HPP_

#ifndef _C4_YML_WRITER_HPP_
#include "./writer.hpp"
#endif

#ifndef _C4_YML_TREE_HPP_
#include "./tree.hpp"
#endif

#ifndef _C4_YML_NODE_HPP_
#include "./node.hpp"
#endif

namespace c4 {
namespace yml {

template<class Writer> class Emitter;

using EmitterFile = Emitter<WriterFile>;
using EmitterBuf  = Emitter<WriterBuf>;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

template<class Writer>
class Emitter : public Writer
{
public:

    using Writer::Writer;

    /** emit YAML.
     *
     * When writing to a buffer, returns a substr of the emitted YAML.
     * If the given buffer has insufficient space, the returned span will
     * be null and its size will be the needed space. No writes are done
     * after the end of the buffer.
     *
     * When writing to a file, the returned span will be null, but its
     * length is set the number of bytes written. */
    substr emit(NodeRef const& n, bool error_on_excess=true);

    size_t tell() const;
    void   seek(size_t p);

private:

    size_t _visit(NodeRef const& n, size_t ilevel=0);
    void _do_visit(NodeRef const& n, size_t ilevel=0, bool indent=true);

private:

    template<class T>
    inline void _write(T a)
    {
        this->Writer::_do_write(a);
    }

    template<size_t N>
    inline void _write(const char (&a)[N])
    {
        csubstr s(a);
        (static_cast<Writer *>(this))->_do_write(s);
    }

    void _write(NodeScalar const& sc);

    void _write_scalar(csubstr const& s);

};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/** emit YAML to the given file. A null file defaults to stdout.
 * Return the number of bytes written. */
inline size_t emit(NodeRef const& r, FILE *f=nullptr)
{
    EmitterFile em(f);
    size_t num = em.emit(r).len;
    return num;
}

/** emit YAML to the given file. A null file defaults to stdout.
 * Return the number of bytes written. */
inline size_t emit(Tree const &t, FILE *f=nullptr)
{
    if(t.empty()) return 0;
    const NodeRef n(t.rootref());
    return emit(n, f);
}

//-----------------------------------------------------------------------------

/** emit YAML to the given buffer. Return a substr of the emitted YAML.
 * Raise an error if the space in the buffer is insufficient. */
inline substr emit(NodeRef const& r, substr sp, bool error_on_excess=true)
{
    EmitterBuf em(sp);
    substr result = em.emit(r, error_on_excess);
    return result;
}

/** emit YAML to the given buffer. Return a substr of the emitted YAML.
 * Raise an error if the space in the buffer is insufficient. */
inline substr emit(Tree const& t, substr sp, bool error_on_excess=true)
{
    if(t.empty()) return substr();
    return emit(t.rootref(), sp, error_on_excess);
}

//-----------------------------------------------------------------------------

/** emit YAML to the given std::vector-like container, resizing it as needed
 * to fit the emitted YAML. */
template<class CharOwningContainer>
inline substr emit_resize(NodeRef const& n, CharOwningContainer * cont)
{
    substr buf = to_substr(*cont);
    substr ret = emit(n, buf, /*error_on_excess*/false);
    if(ret.str == nullptr && ret.len > 0)
    {
        cont->resize(ret.len);
        buf = to_substr(*cont);
        ret = emit(n, buf, /*error_on_excess*/true);
    }
    return ret;
}

/** emit YAML to the given std::vector-like container, resizing it as needed
 * to fit the emitted YAML. */
template<class CharOwningContainer>
inline substr emit_resize(Tree const& t, CharOwningContainer * cont)
{
    if(t.empty()) return substr();
    return emit_resize(t.rootref(), cont);
}

} // namespace yml
} // namespace c4

#include "./emit.def.hpp"

#endif /* _C4_YML_EMIT_HPP_ */
