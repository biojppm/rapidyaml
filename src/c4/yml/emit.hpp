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

template<class OStream>
using EmitterOStream = Emitter<WriterOStream<OStream>>;
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
     * When writing to a file, the returned substr will be null, but its
     * length will be set to the number of bytes written. */
    substr emit(Tree const& t, size_t id, bool error_on_excess);
    /** @overload */
    substr emit(Tree const& t, bool error_on_excess=true) { return emit(t, t.root_id(), error_on_excess); }
    /** @overload */
    substr emit(NodeRef const& n, bool error_on_excess=true) { return emit(*n.tree(), n.id(), error_on_excess); }

    size_t tell() const;
    void   seek(size_t p);

private:

    size_t  _visit(Tree const& t, size_t id, size_t ilevel=0);
    void _do_visit(Tree const& t, size_t id, size_t ilevel=0, bool indent=true);

private:

    template<class T>
    C4_ALWAYS_INLINE void _write(T a)
    {
        this->Writer::_do_write(a);
    }

    template<size_t N>
    C4_ALWAYS_INLINE void _write(const char (&a)[N])
    {
        csubstr s(a);
        this->Writer::_do_write(s);
    }

    void _write(NodeScalar const& sc, NodeType flags);

    void _write_scalar(csubstr s);

    void _indent(size_t ilevel)
    {
        RepC ind{' ', 2 * ilevel};
        this->Writer::_do_write(ind);
    }

    enum {
        _keysc =  (KEY|KEYREF|KEYANCH)  | ~(VAL|VALREF|VALANCH),
        _valsc = ~(KEY|KEYREF|KEYANCH)  |  (VAL|VALREF|VALANCH),
    };

    C4_ALWAYS_INLINE void _writek(Tree const& t, size_t id) { _write(t.keysc(id), t.type(id) & ~(VAL|VALREF|VALANCH)); }
    C4_ALWAYS_INLINE void _writev(Tree const& t, size_t id) { _write(t.valsc(id), t.type(id) & ~(KEY|KEYREF|KEYANCH)); }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/** emit YAML to the given file. A null file defaults to stdout.
 * Return the number of bytes written. */
inline size_t emit(Tree const& t, size_t id, FILE *f)
{
    EmitterFile em(f);
    size_t len = em.emit(t, id, /*error_on_excess*/true).len;
    return len;
}

/** emit YAML to the given file. A null file defaults to stdout.
 * Return the number of bytes written.
 * @overload */
inline size_t emit(Tree const& t, FILE *f=nullptr)
{
    return emit(t, t.root_id(), f);
}

/** emit YAML to the given file. A null file defaults to stdout.
 * Return the number of bytes written.
 * @overload */
inline size_t emit(NodeRef const& r, FILE *f=nullptr)
{
    return emit(*r.tree(), r.id(), f);
}


//-----------------------------------------------------------------------------

/** emit YAML to an STL-like ostream */
template<class OStream>
inline OStream& operator<< (OStream& s, Tree const& t)
{
    EmitterOStream<OStream> em(s);
    em.emit(t.rootref());
    return s;
}

/** emit YAML to an STL-like ostream
 * @overload */
template<class OStream>
inline OStream& operator<< (OStream& s, NodeRef const& n)
{
    EmitterOStream<OStream> em(s);
    em.emit(n);
    return s;
}


//-----------------------------------------------------------------------------

/** emit YAML to the given buffer. Return a substr trimmed to the emitted YAML.
 * Raise an error if the space in the buffer is insufficient.
 * @overload */
inline substr emit(Tree const& t, size_t id, substr buf, bool error_on_excess=true)
{
    EmitterBuf em(buf);
    substr result = em.emit(t, id, error_on_excess);
    return result;
}

/** emit YAML to the given buffer. Return a substr trimmed to the emitted YAML.
 * Raise an error if the space in the buffer is insufficient.
 * @overload */
inline substr emit(Tree const& t, substr buf, bool error_on_excess=true)
{
    return emit(t, t.root_id(), buf, error_on_excess);
}

/** emit YAML to the given buffer. Return a substr trimmed to the emitted YAML.
 * Raise an error if the space in the buffer is insufficient.
 * @overload
 */
inline substr emit(NodeRef const& r, substr buf, bool error_on_excess=true)
{
    return emit(*r.tree(), r.id(), buf, error_on_excess);
}


//-----------------------------------------------------------------------------

/** emit+resize: YAML to the given std::string/std::vector-like container,
 * resizing it as needed to fit the emitted YAML. */
template<class CharOwningContainer>
substr emitrs(Tree const& t, size_t id, CharOwningContainer * cont)
{
    substr buf = to_substr(*cont);
    substr ret = emit(t, id, buf, /*error_on_excess*/false);
    if(ret.str == nullptr && ret.len > 0)
    {
        cont->resize(ret.len);
        buf = to_substr(*cont);
        ret = emit(t, id, buf, /*error_on_excess*/true);
    }
    return ret;
}

/** emit+resize: YAML to the given std::string/std::vector-like container,
 * resizing it as needed to fit the emitted YAML. */
template<class CharOwningContainer>
CharOwningContainer emitrs(Tree const& t, size_t id)
{
    CharOwningContainer c;
    emitrs(t, id, &c);
    return c;
}

/** emit+resize: YAML to the given std::string/std::vector-like container,
 * resizing it as needed to fit the emitted YAML. */
template<class CharOwningContainer>
substr emitrs(Tree const& t, CharOwningContainer * cont)
{
    return emitrs(t, t.root_id(), cont);
}

/** emit+resize: YAML to the given std::string/std::vector-like container,
 * resizing it as needed to fit the emitted YAML. */
template<class CharOwningContainer>
CharOwningContainer emitrs(Tree const& t)
{
    CharOwningContainer c;
    emitrs(t, t.root_id(), &c);
    return c;
}

/** emit+resize: YAML to the given std::string/std::vector-like container,
 * resizing it as needed to fit the emitted YAML. */
template<class CharOwningContainer>
substr emitrs(NodeRef const& n, CharOwningContainer * cont)
{
    return emitrs(*n.tree(), n.id(), cont);
}

/** emit+resize: YAML to the given std::string/std::vector-like container,
 * resizing it as needed to fit the emitted YAML. */
template<class CharOwningContainer>
CharOwningContainer emitrs(NodeRef const& n)
{
    CharOwningContainer c;
    emitrs(*n.tree(), n.id(), &c);
    return c;
}

} // namespace yml
} // namespace c4

#include "./emit.def.hpp"

#endif /* _C4_YML_EMIT_HPP_ */
