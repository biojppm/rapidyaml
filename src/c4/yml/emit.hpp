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

template< class Writer > class Emitter;

using EmitterFile = Emitter< WriterFile >;
using EmitterSpan = Emitter< WriterSpan >;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

template< class Writer >
class Emitter : public Writer
{
public:

    using Writer::Writer;

    /** emit YAML.
     *
     * When writing to a buffer, returns a subspan of the emitted YAML.
     * If the given buffer has insufficient space, the returned span will
     * be null and its size will be the needed space. No writes are done
     * after the end of the buffer.
     *
     * When writing to a file, the returned span will be null, but its
     * length is set the number of bytes written. */
    subs emit(NodeRef const& n, bool error_on_excess=true);

    size_t tell() const;
    void   seek(size_t p);

private:

    size_t _visit(NodeRef const& n, size_t ilevel=0);
    void _do_visit(NodeRef const& n, size_t ilevel=0, bool indent=true);

private:

#define _c4this (static_cast< Writer * >(this))

    template< class T, class... Args >
    inline void _write(T a, Args... more)
    {
        _write_one(a);
        _write(more...);
    }
    inline void _write() {}

    template< class T >
    inline void _write_one(T a)
    {
        _c4this->_do_write(a);
    }
    template< size_t N >
    inline void _write_one(const char (&a)[N])
    {
        // a decays into const char*, so explicitly create using the array
        csubs s;
        s.assign<N>(a);
        _c4this->_do_write(s);
    }
    void _write_one(NodeScalar const& sc);

    struct AnchorScalar : public NodeScalar
    {
        csubs anchor;
        AnchorScalar(NodeRef const& n) : NodeScalar(n.valsc()), anchor(n.anchor()) {}
    };

    void _write_one(AnchorScalar const& sc);

    void _write_scalar(csubs const& s);

#undef _c4this

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

/** emit YAML to the given buffer. Return a subspan of the emitted YAML.
 * Raise an error if the space in the buffer is insufficient. */
inline subs emit(NodeRef const& r, subs sp, bool error_on_excess=true)
{
    EmitterSpan em(sp);
    subs result = em.emit(r, error_on_excess);
    return result;
}

/** emit YAML to the given buffer. Return a subspan of the emitted YAML.
 * Raise an error if the space in the buffer is insufficient. */
inline subs emit(Tree const& t, subs sp, bool error_on_excess=true)
{
    if(t.empty()) return subs();
    return emit(t.rootref(), sp, error_on_excess);
}

//-----------------------------------------------------------------------------

/** emit YAML to the given std::vector-like container, resizing it as needed
 * to fit the emitted YAML. */
template< class CharOwningContainer >
inline subs emit_resize(NodeRef const& n, CharOwningContainer * cont)
{
    subs buf = to_subs(*cont);
    subs ret = emit(n, buf, /*error_on_excess*/false);
    if(ret.str == nullptr && ret.len > 0)
    {
        cont->resize(ret.len);
        buf = to_subs(*cont);
        ret = emit(n, buf, /*error_on_excess*/true);
    }
    return ret;
}

/** emit YAML to the given std::vector-like container, resizing it as needed
 * to fit the emitted YAML. */
template< class CharOwningContainer >
inline subs emit_resize(Tree const& t, CharOwningContainer * cont)
{
    if(t.empty()) return subs();
    return emit_resize(t.rootref(), cont);
}

} // namespace yml
} // namespace c4

#include "./emit.def.hpp"

#endif /* _C4_YML_EMIT_HPP_ */
