
%module ryml

%include "stdint.i"

//-----------------------------------------------------------------------------
// this block will be pasted verbatim in the generated C++ source file

%{
// specifies that the resulting C file should be built as a python
// extension, inserting the module init code
#define SWIG_FILE_WITH_INIT

#include <c4/yml/yml.hpp>
#include <c4/yml/error.def.hpp>

namespace c4 {
namespace yml {

using substr = c4::substr;
using csubstr = c4::csubstr;

} /* namespace yml */
} /* namespace c4 */

%}


//-----------------------------------------------------------------------------

%apply (const char *STRING, size_t LENGTH) { (const char *str, size_t len) };
%apply (char *STRING, size_t LENGTH) { (char *str, size_t len) };
%newobject emit_malloc;

%typemap(in) c4::substr {
#if defined(SWIGPYTHON)
  if($input != Py_None)
  {
      if(PyMemoryView_Check($input)) // is it a memoryview?
      {
          Py_buffer const* view = PyMemoryView_GET_BUFFER($input);
          $1 = c4::substr((char*)view->buf, view->len);
      }
      else
      {
          int ok = PyObject_CheckBuffer($input);
          Py_buffer view;
          ok = ok && (0 == PyObject_GetBuffer($input, &view, PyBUF_SIMPLE | PyBUF_WRITABLE));
          if(ok)
          {
              $1 = c4::substr((char*)view.buf, view.len);
              PyBuffer_Release(&view);
          }
          else
          {
              PyErr_SetString(PyExc_TypeError, "substr: could not get mutable memory - have you passed an imutable type such as str or bytes?");
              SWIG_fail;
          }
      }
  }
  else
  {
      $1 = c4::substr(nullptr, size_t(0));
  }
#else
#error no "in" typemap defined for this export language
#endif
};

%typemap(in) c4::csubstr {
#if defined(SWIGPYTHON)
  if($input != Py_None)
  {
      if(PyMemoryView_Check($input)) // is it a memoryview?
      {
          Py_buffer const* view = PyMemoryView_GET_BUFFER($input);
          $1 = c4::csubstr((const char*)view->buf, view->len);
      }
      else
      {
          Py_buffer view;
          view.buf = nullptr;
          int ok = PyObject_CheckBuffer($input);
          ok = ok && (0 == PyObject_GetBuffer($input, &view, PyBUF_SIMPLE));
          if(ok)
          {
              $1 = c4::csubstr((const char*)view.buf, view.len);
              PyBuffer_Release(&view);
          }
          else
          {
              // https://stackoverflow.com/questions/36098984/python-3-3-c-api-and-utf-8-strings
              Py_ssize_t sz = 0;
              const char *buf = PyUnicode_AsUTF8AndSize($input, &sz);
              if(buf || sz == 0)
              {
                  $1 = c4::csubstr(buf, sz);
              }
              else
              {
                  PyErr_SetString(PyExc_TypeError, "csubstr: could not get readonly memory from python object");
                  SWIG_fail;
              }
          }
      }
  }
  else
  {
      $1 = c4::csubstr(nullptr, size_t(0));
  }
#else
#error no "in" typemap defined for this export language
#endif
};
%typemap(in) csubstr = c4::csubstr;
%typemap(in) substr = c4::substr;
%typemap(in) c4::yml::csubstr = c4::csubstr;
%typemap(in) c4::yml::substr = c4::substr;

// Copy the typecheck code for "char *".
%typemap(typecheck) c4::substr = char *;
%typemap(typecheck) c4::csubstr = const char *;


//-----------------------------------------------------------------------------

%typemap(out) c4::csubstr {
#if defined(SWIGPYTHON)
  if($1.str == nullptr) {
    $result = Py_None;
    Py_INCREF($result);
  } else {
    PyObject *obj = PyMemoryView_FromMemory((char*)$1.str, $1.len, PyBUF_READ);
    if( ! obj)
    {
        PyErr_SetString(PyExc_TypeError, "could not get readonly memory from c4::csubstr - have you passed a str?");
        SWIG_fail;
    }
    $result = obj;
  }
#else
#error no "out" typemap defined for this export language
#endif
};


//-----------------------------------------------------------------------------

// Language independent exception handler.
// KEEP THIS BEFORE THE FOLLOWING FUNCTIONS!
// see https://stackoverflow.com/a/61621747
%include <std_string.i>
%include <std_except.i>
%include <exception.i>
%include <pyabc.i>
%pythonabc(c4__yml__ExceptionParse, "Exception")
%pythonabc(c4__yml__ExceptionVisit, "Exception")
%pythonabc(c4__yml__ExceptionBasic, "Exception")
%exceptionclass c4::yml::ExceptionBasic;
%exceptionclass c4::yml::ExceptionParse;
%exceptionclass c4::yml::ExceptionVisit;
%exception {
    try {
        $action
    } catch (c4::yml::ExceptionParse const& exc) {
        SWIG_Python_Raise(SWIG_NewPointerObj(
                              (new c4::yml::ExceptionParse(exc)),
                              SWIGTYPE_p_c4__yml__ExceptionParse,
                              SWIG_POINTER_OWN),
                          "ExceptionParse",
                          SWIGTYPE_p_c4__yml__ExceptionParse);
        //std::string err = c4::yml::format_exc<std::string>(exc);
        //PyErr_SetString(SWIG_Python_ExceptionType(SWIGTYPE_p_c4__yml__ExceptionParse), c4::csubstr(err.data(), err.size()), exc.errdata_parse);
        SWIG_fail;
    } catch (c4::yml::ExceptionVisit const& exc) {
        SWIG_Python_Raise(SWIG_NewPointerObj(
                              (new c4::yml::ExceptionVisit(exc)),
                              SWIGTYPE_p_c4__yml__ExceptionVisit,
                              SWIG_POINTER_OWN),
                          "ExceptionVisit",
                          SWIGTYPE_p_c4__yml__ExceptionVisit);
        //std::string err = c4::yml::format_exc<std::string>(exc);
        //PyErr_SetString(SWIG_Python_ExceptionType(SWIGTYPE_p_c4__yml__ExceptionVisit), c4::csubstr(err.data(), err.size()), exc.errdata_visit);
        SWIG_fail;
    } catch (c4::yml::ExceptionBasic const& exc) {
        //SWIG_exception(SWIGTYPE_p_c4__yml__ExceptionBasic, c4::csubstr(exc.str, strlen(exc.str)), exc.errdata_basic);
        SWIG_Python_Raise(SWIG_NewPointerObj(
                              (new c4::yml::ExceptionBasic(exc)),
                              SWIGTYPE_p_c4__yml__ExceptionBasic,
                              SWIG_POINTER_OWN),
                          "ExceptionBasic",
                          SWIGTYPE_p_c4__yml__ExceptionBasic);
        //std::string err = c4::yml::format_exc<std::string>(exc);
        //PyErr_SetString(SWIG_Python_ExceptionType(SWIGTYPE_p_c4__yml__ExceptionBasic), c4::csubstr(err.data(), err.size()), exc.errdata_basic);
        SWIG_fail;
    } catch(std::exception const& e) {
        SWIG_exception(SWIG_RuntimeError, e.what());
    } catch(...) {
        SWIG_exception(SWIG_UnknownError, "unknown error");
    }
}


//-----------------------------------------------------------------------------

%inline %{

void parse_csubstr(c4::csubstr s, c4::yml::Tree *t)
{
    c4::yml::parse_in_arena(s, t);
}

void parse_substr(c4::substr s, c4::yml::Tree *t)
{
    c4::yml::parse_in_place(s, t);
}

char * emit_yaml_malloc(c4::yml::Tree const& t, size_t id)
{
    c4::substr buf;
    c4::substr ret = c4::yml::emit_yaml(t, id, buf, /*error_on_excess*/false);
    if(ret.str == nullptr && ret.len > 0)
    {
        // Use new[]+delete[] in SWIG.
        char * alloc = new char[ret.len + 1]; // we'll return a c-string and not a csubstr
        c4::substr alloced_buf(alloc, ret.len);
        ret = c4::yml::emit_yaml(t, id, alloced_buf, /*error_on_excess*/true);
        ret.str[ret.len] = 0;
    }
    return ret.str;
}

char * emit_json_malloc(c4::yml::Tree const& t, size_t id)
{
    c4::substr buf;
    c4::substr ret = c4::yml::emit_json(t, id, buf, /*error_on_excess*/false);
    if(ret.str == nullptr && ret.len > 0)
    {
        // Use new[]+delete[] in SWIG.
        char * alloc = new char[ret.len + 1]; // we'll return a c-string and not a csubstr
        c4::substr alloced_buf(alloc, ret.len);
        ret = c4::yml::emit_json(t, id, alloced_buf, /*error_on_excess*/true);
        ret.str[ret.len] = 0;
    }
    return ret.str;
}

size_t _compute_yaml_length(const c4::yml::Tree &t, size_t id)
{
    c4::substr buf = {};
    c4::substr ret = c4::yml::emit_yaml(t, id, buf, /*error_on_excess*/false);
    return ret.len;
}

size_t _compute_json_length(const c4::yml::Tree &t, size_t id)
{
    c4::substr buf = {};
    c4::substr ret = c4::yml::emit_json(t, id, buf, /*error_on_excess*/false);
    return ret.len;
}

bool emit_yaml_to_substr(const c4::yml::Tree &t, size_t id, c4::substr s, size_t *OUTPUT)
{
    c4::substr result = c4::yml::emit_yaml(t, id, s, /*error_on_excess*/false);
    *OUTPUT = result.len;
    return result.str == nullptr;
}

bool emit_json_to_substr(const c4::yml::Tree &t, size_t id, c4::substr s, size_t *OUTPUT)
{
    c4::substr result = c4::yml::emit_json(t, id, s, /*error_on_excess*/false);
    *OUTPUT = result.len;
    return result.str == nullptr;
}


// force a roundtrip to C++, which triggers a conversion to csubstr and returns it as a memoryview
c4::csubstr _get_as_csubstr(c4::csubstr s)
{
    //printf("_get_as_csubstr: %p[%zu]'%.*s'\n", s.str, s.len, (int)s.len, s.str);
    return s;
}

c4::csubstr  _get_as_substr(c4::substr s)
{
    //printf("_get_as_substr: %p[%zu]'%.*s'\n", s.str, s.len, (int)s.len, s.str);
    return s;
}


// helper for calling to_arena()
c4::csubstr _to_arena_buf_cpp(c4::yml::Tree &t, c4::csubstr s)
{
    return t.to_arena(s);
}


// utilities for testing
bool _same_ptr(c4::csubstr l, c4::csubstr r)
{
    return l.str == r.str;
}

bool _same_mem(c4::csubstr l, c4::csubstr r)
{
    return l.str == r.str && l.len == r.len;
}
%}


//-----------------------------------------------------------------------------

%pythoncode %{

from deprecation import deprecated


def as_csubstr(s):
    """return as a ryml::csubstr"""
    if isinstance(s, memoryview):
        return s
    return _get_as_csubstr(s)

def as_substr(s):
    """return as a ryml::ssubstr"""
    if isinstance(s, memoryview):
        return s
    return _get_as_substr(s)

def u(memview):
    """return memview as a utf8"""
    return str(memview, "utf8")


def children(tree, node=None):
    """walk (non-recursively) the children of a node"""
    assert tree is not None
    if node is None:
        node = tree.root_id()
    ch = tree.first_child(node)
    while ch != NONE:
        yield ch
        ch = tree.next_sibling(ch)


def siblings(tree, node):
    """walk (non-recursively) the siblings of a node"""
    assert tree is not None
    if node is None:
        return
    ch = tree.first_sibling(node)
    while ch != NONE:
        yield ch
        ch = tree.next_sibling(ch)


def walk(tree, node=None, depth=0):
    """walk recursively starting at a node, returning a tuple of node and depth"""
    assert tree is not None
    if node is None:
        node = tree.root_id()
    yield node, depth
    ch = tree.first_child(node)
    while ch != NONE:
       for gc, il in walk(tree, ch, depth + 1):
           yield gc, il
       ch = tree.next_sibling(ch)


@deprecated(deprecated_in="0.5.0", details="Use parse_in_arena() instead")
def parse(buf, **kwargs):
    return parse_in_arena(tree, id)


def parse_in_arena(buf, tree=None):
    """
    parse immutable YAML in the trees arena. Copy the YAML into a buffer
    in the C++ tree's arena, then parse the YAML from the trees arena.

    :param buf:
       the YAML buffer to be parsed
    :type buf: ``str``, ``bytes``, ``bytearray`` or ``memoryview``

    :param tree:
       a tree to be reused. When no tree is given, a new tree is created,
       and returned at the end.
    :type buf: ``ryml.Tree``
    """
    if tree is None:
        tree = Tree()
    parse_csubstr(buf, tree)
    return tree


def parse_in_place(buf, tree=None):
    """
    parse in place a mutable buffer containing YAML. The resulting tree
    will point into the given buffer.

    :param buf:
       the YAML buffer to be parsed
    :type buf:
       ``bytearray`` or ``memoryview`` (NOT ``str`` or ``bytes``, which are not writeable)

    :param tree:
       a tree to be reused. When no tree is given, a new tree is created,
       and returned at the end.
    :type buf: ``ryml.Tree``
    """
    if tree is None:
        tree = Tree()
    parse_substr(buf, tree)
    return tree


@deprecated(deprecated_in="0.5.0", details="Use emit_yaml() instead")
def emit(tree, id=None):
    return emit_yaml(tree, id)

def emit_yaml(tree, id=None):
    """emit the given tree as YAML, creating a new output buffer"""
    if id is None:
        id = tree.root_id()
    return emit_yaml_malloc(tree, id)

def emit_json(tree, id=None):
    """emit the given tree as JSON, creating a new output buffer"""
    if id is None:
        id = tree.root_id()
    return emit_json_malloc(tree, id)


def emit_yaml_in_place(tree, buf, id=None):
    return _emit_fn_in_place(tree, buf, id, emit_yaml_to_substr)

def emit_json_in_place(tree, buf, id=None):
    return _emit_fn_in_place(tree, buf, id, emit_json_to_substr)

@deprecated(deprecated_in="0.5.0", details="Use emit_yaml_in_place() instead")
def emit_in_place(tree, buf, id=None):
    return emit_yaml_in_place(tree, buf, id)

def _emit_fn_in_place(tree, buf, id, fn):
    if id is None:
        id = tree.root_id()
    (failed, expected_size) = fn(tree, id, buf)
    if failed:
        raise IndexError("Output buffer has {} bytes, but emit requires {} bytes".format(
            len(buf), expected_size))
    return memoryview(buf)[:expected_size]


def compute_yaml_length(tree, id=None):
    if id is None:
        id = tree.root_id()
    return _compute_yaml_length(tree, id)

def compute_json_length(tree, id=None):
    if id is None:
        id = tree.root_id()
    return _compute_json_length(tree, id)

@deprecated(deprecated_in="0.5.0", details="Use compute_yaml_length() instead")
def compute_emit_length(tree, id=None):
    return compute_yaml_length(tree, id)

@deprecated(deprecated_in="0.11.0", details="Use compute_yaml_length() instead")
def compute_emit_yaml_length(tree, id=None):
    return compute_yaml_length(tree, id)

@deprecated(deprecated_in="0.11.0", details="Use compute_json_length() instead")
def compute_emit_json_length(tree, id=None):
    return compute_json_length(tree, id)


%}


//-----------------------------------------------------------------------------


namespace c4 {
namespace yml {


constexpr const size_t NONE = (size_t)-1;

using type_bits = uint32_t;


struct Location
{
    size_t offset; ///< number of bytes from the beginning of the source buffer
    size_t line;
    size_t col;
    c4::yml::csubstr name;
};

struct ErrorDataBasic
{
    c4::yml::Location location; ///< location where the error was detected (may be from YAML or C++ source code)
    ErrorDataBasic() noexcept = default;
    ErrorDataBasic(c4::yml::Location const& cpploc_) noexcept;
};
struct ErrorDataParse
{
    c4::yml::Location cpploc; ///< location in the C++ source file where the error was detected.
    c4::yml::Location ymlloc; ///< location in the YAML source buffer where the error was detected.
    ErrorDataParse() noexcept = default;
    ErrorDataParse(c4::yml::Location const& cpploc_, c4::yml::Location const& ymlloc_) noexcept;
};
struct ErrorDataVisit
{
    c4::yml::Location cpploc;  ///< location in the C++ source file where the error was detected.
    c4::yml::Tree const* tree; ///< tree where the error was detected
    c4::yml::id_type node;     ///< node where the error was detected
    ErrorDataVisit() noexcept = default;
    ErrorDataVisit(c4::yml::Location const& cpploc_, c4::yml::Tree const *tree_ , c4::yml::id_type node_) noexcept;
};

struct ExceptionBasic : public std::exception
{
    ExceptionBasic(c4::yml::csubstr msg, c4::yml::ErrorDataBasic const& errdata_) noexcept;
    const char* what() const noexcept override { return msg; }
    c4::yml::ErrorDataBasic errdata_basic; ///< error data
    char msg[RYML_ERRMSG_SIZE];   ///< the reported error message, without location indication.
};
struct ExceptionParse : public ExceptionBasic
{
    ExceptionParse(c4::yml::csubstr msg, c4::yml::ErrorDataParse const& errdata_) noexcept;
    c4::yml::ErrorDataParse errdata_parse;
};
struct ExceptionVisit : public ExceptionBasic
{
    ExceptionVisit(c4::yml::csubstr msg, c4::yml::ErrorDataVisit const& errdata_) noexcept;
    c4::yml::ErrorDataVisit errdata_visit;
};

typedef enum : c4::yml::type_bits {
    NOTYPE  = 0,
    KEY     = (1u << 0),
    VAL     = (1u << 1),
    MAP     = (1u << 2),
    SEQ     = (1u << 3),
    DOC     = (1u << 4),
    STREAM  = (1u << 5)|SEQ,
    KEYREF  = (1u << 6),
    VALREF  = (1u << 7),
    KEYANCH = (1u << 8),
    VALANCH = (1u << 9),
    KEYTAG  = (1u << 10),
    VALTAG  = (1u << 11),
    KEYNIL  = (1u << 12),
    VALNIL  = (1u << 13),
    _TYMASK = (1u << 14)-1u,
    //
    // unfiltered flags:
    //
    KEY_UNFILT  = (1u << 14),
    VAL_UNFILT  = (1u << 15),
    //
    // style flags:
    //
    FLOW_SL     = (1u << 16),
    FLOW_ML1    = (1u << 17),
    FLOW_MLN    = (1u << 18),
    FLOW_SPC    = (1u << 19),
    BLOCK       = (1u << 20),
    KEY_LITERAL = (1u << 21),
    VAL_LITERAL = (1u << 22),
    KEY_FOLDED  = (1u << 23),
    VAL_FOLDED  = (1u << 24),
    KEY_SQUO    = (1u << 25),
    VAL_SQUO    = (1u << 26),
    KEY_DQUO    = (1u << 27),
    VAL_DQUO    = (1u << 28),
    KEY_PLAIN   = (1u << 29),
    VAL_PLAIN   = (1u << 30),
    //
    // type combination masks:
    //
    KEYVAL  = KEY|VAL,
    KEYSEQ  = KEY|SEQ,
    KEYMAP  = KEY|MAP,
    DOCMAP  = DOC|MAP,
    DOCSEQ  = DOC|SEQ,
    DOCVAL  = DOC|VAL,
    //
    // style combination masks:
    //
    SCALAR_LITERAL = KEY_LITERAL|VAL_LITERAL,
    SCALAR_FOLDED  = KEY_FOLDED|VAL_FOLDED,
    SCALAR_SQUO    = KEY_SQUO|VAL_SQUO,
    SCALAR_DQUO    = KEY_DQUO|VAL_DQUO,
    SCALAR_PLAIN   = KEY_PLAIN|VAL_PLAIN,
    KEYQUO         = KEY_SQUO|KEY_DQUO|KEY_FOLDED|KEY_LITERAL, ///< key style is one of ', ", > or |
    VALQUO         = VAL_SQUO|VAL_DQUO|VAL_FOLDED|VAL_LITERAL, ///< val style is one of ', ", > or |
    KEY_STYLE      = KEY_LITERAL|KEY_FOLDED|KEY_SQUO|KEY_DQUO|KEY_PLAIN, ///< mask of all the scalar styles for key (not container styles!)
    VAL_STYLE      = VAL_LITERAL|VAL_FOLDED|VAL_SQUO|VAL_DQUO|VAL_PLAIN, ///< mask of all the scalar styles for val (not container styles!)
    SCALAR_STYLE   = KEY_STYLE|VAL_STYLE,
    CONTAINER_STYLE_FLOW  = FLOW_SL|FLOW_ML,
    CONTAINER_STYLE_BLOCK = BLOCK,
    CONTAINER_STYLE       = FLOW_SL|FLOW_ML|BLOCK,
    STYLE          = SCALAR_STYLE | CONTAINER_STYLE,
    //
    // mixed masks
    KEYMASK_ = KEY | KEYQUO | KEYANCH | KEYREF | KEYTAG,
    VALMASK_ = VAL | VALQUO | VALANCH | VALREF | VALTAG,
} NodeType_e;


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

struct NodeType
{
public:

    type_bits m_bits;

public:

    NodeType() noexcept;
    NodeType(type_bits t) noexcept;

    bool has_any(type_bits t) const noexcept;
    bool has_all(type_bits t) const noexcept;
    bool has_none(type_bits t) const noexcept;

    void set(type_bits t) noexcept;
    void add(type_bits t) noexcept;
    void rem(type_bits t) noexcept;
    void addrem(type_bits bits_to_add, type_bits bits_to_remove) noexcept;

    void clear() noexcept;

public:

    /** @name node type queries
     * @{ */

    /** return a preset string based on the node type */
    const char *type_str() const noexcept;
    /** return a preset string based on the node type */
    static const char* type_str(type_bits t) noexcept;

    /** fill a string with the node type flags. */
    size_t type_str(c4::substr buf) const noexcept;
    /** fill a string with the node type flags. */
    static size_t type_str(c4::substr buf, type_bits t) noexcept;

    /** fill a string with the node type flags. If the string is small, returns {null, len} */
    c4::csubstr type_str_sub(c4::substr buf) const noexcept;
    /** fill a string with the node type flags. If the string is small, returns {null, len}  */
    static c4::csubstr type_str_sub(c4::substr buf, type_bits t) noexcept;

public:

    /** @name node type queries
     * @{ */

    bool is_notype()         const noexcept;
    bool is_stream()         const noexcept;
    bool is_doc()            const noexcept;
    bool is_container()      const noexcept;
    bool is_map()            const noexcept;
    bool is_seq()            const noexcept;
    bool has_key()           const noexcept;
    bool has_val()           const noexcept;
    bool is_val()            const noexcept;
    bool is_keyval()         const noexcept;
    bool key_is_null()       const noexcept;
    bool val_is_null()       const noexcept;
    bool has_key_tag()       const noexcept;
    bool has_val_tag()       const noexcept;
    bool has_key_anchor()    const noexcept;
    bool has_val_anchor()    const noexcept;
    bool has_anchor()        const noexcept;
    bool is_key_ref()        const noexcept;
    bool is_val_ref()        const noexcept;
    bool is_ref()            const noexcept;

    bool is_key_unfiltered() const noexcept;
    bool is_val_unfiltered() const noexcept;

    /** @} */

public:

    /** @name container+scalar style queries
     * @{ */

    bool is_container_styled() const noexcept;
    bool is_block() const noexcept;
    bool is_flow_sl() const noexcept;
    bool is_flow_ml1() const noexcept;
    bool is_flow_mln() const noexcept;
    bool is_flow_mlx() const noexcept;
    bool is_flow() const noexcept;
    bool has_flow_space() const noexcept;

    bool is_key_styled() const noexcept;
    bool is_val_styled() const noexcept;
    bool is_key_literal() const noexcept;
    bool is_val_literal() const noexcept;
    bool is_key_folded() const noexcept;
    bool is_val_folded() const noexcept;
    bool is_key_squo() const noexcept;
    bool is_val_squo() const noexcept;
    bool is_key_dquo() const noexcept;
    bool is_val_dquo() const noexcept;
    bool is_key_plain() const noexcept;
    bool is_val_plain() const noexcept;
    bool is_key_quoted() const noexcept;
    bool is_val_quoted() const noexcept;
    bool is_quoted() const noexcept;

    NodeType key_style() const noexcept;
    NodeType val_style() const noexcept;

    void set_container_style(type_bits style) noexcept;
    void set_key_style(type_bits style) noexcept;
    void set_val_style(type_bits style) noexcept;
    void clear_style() noexcept;

    /** @} */
};


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

#ifndef RYML_ID_TYPE
/** The type of a node id in the YAML tree. In the future, the default
 * will likely change to int32_t, which was observed to be faster.
 * @see id_type */
#define RYML_ID_TYPE size_t
#endif


/** The type of a node id in the YAML tree; to override the default
 * type, define the macro @ref RYML_ID_TYPE to a suitable integer
 * type. */
using id_type = RYML_ID_TYPE;

struct Tree
{
    Tree();
    Tree(id_type node_type, size_t arena_capacity=RYML_DEFAULT_TREE_ARENA_CAPACITY);
    ~Tree();

    void reserve(id_type node_capacity=RYML_DEFAULT_TREE_CAPACITY);
    void reserve_arena(size_t arena_capacity=RYML_DEFAULT_TREE_ARENA_CAPACITY);
    void clear();
    void clear_arena();

    bool   empty() const;
    id_type size() const;
    id_type capacity() const;
    id_type slack() const;

    size_t arena_size() const;
    size_t arena_capacity() const;
    size_t arena_slack() const;

    void resolve();

public:

    // getters

    NodeType    type(id_type node) const;

    c4::csubstr key       (id_type node) const;
    c4::csubstr key_tag   (id_type node) const;
    c4::csubstr key_ref   (id_type node) const;
    c4::csubstr key_anchor(id_type node) const;
    c4::yml::NodeScalar keysc(id_type node) const;

    c4::csubstr val       (id_type node) const;
    c4::csubstr val_tag   (id_type node) const;
    c4::csubstr val_ref   (id_type node) const;
    c4::csubstr val_anchor(id_type node) const;
    c4::yml::NodeScalar valsc(id_type node) const;

public:

    // node predicates

    bool type_has_any(id_type node, type_bits bits) const;
    bool type_has_all(id_type node, type_bits bits) const;
    bool type_has_none(id_type node, type_bits bits) const;

    bool is_stream(id_type node) const;
    bool is_doc(id_type node) const;
    bool is_container(id_type node) const;
    bool is_map(id_type node) const;
    bool is_seq(id_type node) const;
    bool has_key(id_type node) const;
    bool has_val(id_type node) const;
    bool is_val(id_type node) const;
    bool is_keyval(id_type node) const;
    bool has_key_tag(id_type node) const;
    bool has_val_tag(id_type node) const;
    bool has_key_anchor(id_type node) const;
    bool has_val_anchor(id_type node) const;
    bool has_anchor(id_type node) const;
    bool is_key_ref(id_type node) const;
    bool is_val_ref(id_type node) const;
    bool is_ref(id_type node) const;

    bool parent_is_seq(id_type node) const;
    bool parent_is_map(id_type node) const;

    bool has_anchor(id_type node, c4::csubstr a) const;
    bool key_is_null(id_type node) const;
    bool val_is_null(id_type node) const;

public:

    // hierarchy predicates

    bool is_root(id_type node) const;

    bool has_parent(id_type node) const;
    bool has_child(id_type node, c4::csubstr key) const;
    //bool has_child(id_type node, id_type ch) const;
    bool has_children(id_type node) const;
    bool has_sibling(id_type node, c4::csubstr key) const;
    //bool has_sibling(id_type node, id_type sib) const;
    bool has_other_siblings(id_type node) const;

public:

    // hierarchy getters

    id_type root_id() const;

    id_type parent(id_type node) const;
    id_type prev_sibling(id_type node) const;
    id_type next_sibling(id_type node) const;
    id_type num_children(id_type node) const;
    id_type child_pos(id_type node, id_type ch) const;
    id_type first_child(id_type node) const;
    id_type last_child(id_type node) const;
    id_type child(id_type node, id_type pos) const;
    id_type find_child(id_type node, c4::csubstr key) const;
    id_type num_siblings(id_type node) const;
    id_type num_other_siblings(id_type node) const;
    id_type sibling_pos(id_type node, id_type sib) const;
    id_type first_sibling(id_type node) const;
    id_type last_sibling(id_type node) const;
    id_type sibling(id_type node, id_type pos) const;
    id_type find_sibling(id_type node, c4::csubstr key) const;

public:

    /** @name node style predicates and modifiers. see the corresponding predicate in NodeType */
    /** @{ */

    bool is_container_styled(id_type node) const;
    bool is_block(id_type node) const;
    bool is_flow_sl(id_type node) const;
    bool is_flow_ml1(id_type node) const;
    bool is_flow_mln(id_type node) const;
    bool is_flow_mlx(id_type node) const;
    bool is_flow(id_type node) const;

    bool is_key_styled(id_type node) const;
    bool is_val_styled(id_type node) const;
    bool is_key_literal(id_type node) const;
    bool is_val_literal(id_type node) const;
    bool is_key_folded(id_type node) const;
    bool is_val_folded(id_type node) const;
    bool is_key_squo(id_type node) const;
    bool is_val_squo(id_type node) const;
    bool is_key_dquo(id_type node) const;
    bool is_val_dquo(id_type node) const;
    bool is_key_plain(id_type node) const;
    bool is_val_plain(id_type node) const;
    bool is_key_quoted(id_type node) const;
    bool is_val_quoted(id_type node) const;
    bool is_quoted(id_type node) const;

    void set_container_style(id_type node, type_bits style);
    void set_key_style(id_type node, type_bits style);
    void set_val_style(id_type node, type_bits style);

    /** @} */

public:

    void set_key_tag(id_type node, c4::csubstr tag);
    void set_key_anchor(id_type node, c4::csubstr anchor);
    void set_val_anchor(id_type node, c4::csubstr anchor);
    void set_key_ref   (id_type node, c4::csubstr ref   );
    void set_val_ref   (id_type node, c4::csubstr ref   );

    void set_val_tag(id_type node, c4::csubstr tag);
    void rem_key_anchor(id_type node);
    void rem_val_anchor(id_type node);
    void rem_key_ref   (id_type node);
    void rem_val_ref   (id_type node);
    void rem_anchor_ref(id_type node);

    // see below for set
    void set_map(id_type node, type_bits more_flags=0);
    void set_seq(id_type node, type_bits more_flags=0);
    void set_doc(id_type node);
    void set_stream(id_type node);

    // these functions do not work correctly with the typemape for csubstr:
    //void set_key(id_type node, c4::csubstr s, type_bits more_flags=0);
    //void set_val(id_type node, c4::csubstr s, type_bits more_flags=0);
    //void to_keyval(id_type node, c4::csubstr key, c4::csubstr val, int more_flags=0);
    //void to_map(id_type node, c4::csubstr key, int more_flags=0);
    //void to_seq(id_type node, c4::csubstr key, int more_flags=0);
    //void to_val(id_type node, c4::csubstr val, int more_flags=0);
    //void to_stream(id_type node, int more_flags=0);
    //void to_map(id_type node, int more_flags=0);
    //void to_seq(id_type node, int more_flags=0);
    //void to_doc(id_type node, int more_flags=0);
    //void _set_key(id_type node, c4::csubstr key, int more_flags=0);
    //void _set_val(id_type node, c4::csubstr val, int more_flags=0);
    //
    // so we do the following:
    %extend {
        void __set_key(id_type node, c4::csubstr s, int flags) { $self->Tree::set_key(node, s, flags); }
        void __set_val(id_type node, c4::csubstr s, int flags) { $self->Tree::set_val(node, s, flags); }
        %pythoncode %{
            def set_key(self, node, key, more_flags=0):
                self.__set_key(node, key, more_flags)
            def set_val(self, node, key, more_flags=0):
                self.__set_val(node, key, more_flags)
            # this is needed to ensure the csubstr typemap applies to the overloads
            @deprecated(deprecated_in="0.16.0", details="Use .set_val()")
            def to_val(self, node, val, more_flags=0):
                self.__set_val(node, val, more_flags)
            @deprecated(deprecated_in="0.16.0", details="Use .set_key() + .set_val()")
            def to_keyval(self, node, key, val, more_flags=0):
                self.__set_key(node, key, more_flags)
                self.__set_val(node, val, more_flags)
            @deprecated(deprecated_in="0.16.0", details="Use .set_map() + .set_key()")
            def to_map(self, node, key=None, more_flags=0):
                if isinstance(key, int):
                    self.set_map(node, key | more_flags)
                elif key is None:
                    self.set_map(node, more_flags)
                else:
                    self._set_key(node, key, more_flags)
                    self.set_map(node, more_flags)
            @deprecated(deprecated_in="0.16.0", details="Use .set_seq() + .set_key()")
            def to_seq(self, node, key=None, more_flags=0):
                if isinstance(key, int):
                    self.set_seq(node, key | more_flags)
                elif key is None:
                    self.set_seq(node, more_flags)
                else:
                    self.__set_key(node, key, more_flags)
                    self.set_seq(node, more_flags)
            @deprecated(deprecated_in="0.16.0", details="Use .set_key()")
            def _set_key(self, node, key, more_flags=0):
                self.__set_key(node, key, more_flags)
            @deprecated(deprecated_in="0.16.0", details="Use .set_val()")
            def _set_val(self, node, val, more_flags=0):
                self.__set_val(node, val, more_flags)
            @deprecated(deprecated_in="0.16.0", details="Use .set_doc()")
            def to_doc(self, node, more_flags=0):
                assert not more_flags, "flags no longer supported on .to_doc()"
                self.set_doc(node)
            @deprecated(deprecated_in="0.16.0", details="Use .set_stream()")
            def to_stream(self, node, more_flags=0):
                assert not more_flags, "flags no longer supported on .to_stream()"
                self.set_stream(node)
        %}
    }

public:

    /** create and insert a new child of "parent". insert after the (to-be)
     * sibling "after", which must be a child of "parent". To insert as the
     * first child, set after to NONE */
    id_type insert_child(id_type parent, id_type after);
    id_type prepend_child(id_type parent);
    id_type  append_child(id_type parent);

public:

    //! create and insert a new sibling of n. insert after "after"
    id_type insert_sibling(id_type node, id_type after);
    id_type prepend_sibling(id_type node);
    id_type  append_sibling(id_type node);

public:

    //! remove an entire branch at once: ie remove the children and the node itself
    void remove(id_type node);

    //! remove all the node's children, but keep the node itself
    void remove_children(id_type node);

public:

    void reorder();

    /** change the node's position in the parent */
    void move(id_type node, id_type after);

    /** change the node's parent and position */
    void move(id_type node, id_type new_parent, id_type after);
    /** change the node's parent and position */
    id_type move(Tree * src, id_type node, id_type new_parent, id_type after);

    /** recursively duplicate the node */
    id_type duplicate(id_type node, id_type new_parent, id_type after);
    /** recursively duplicate a node from a different tree */
    id_type duplicate(Tree const* src, id_type node, id_type new_parent, id_type after);

    /** recursively duplicate the node's children (but not the node) */
    void duplicate_children(id_type node, id_type parent, id_type after);
    /** recursively duplicate the node's children (but not the node), where the node is from a different tree */
    void duplicate_children(Tree const* src, id_type node, id_type parent, id_type after);

    void duplicate_contents(id_type node, id_type where);

    /** duplicate the node's children (but not the node) in a new parent, but
     * omit repetitions where a duplicated node has the same key (in maps) or
     * value (in seqs). If one of the duplicated children has the same key
     * (in maps) or value (in seqs) as one of the parent's children, the one
     * that is placed closest to the end will prevail. */
    void duplicate_children_no_rep(id_type node, id_type parent, id_type after);

public:

    %extend {
        // to_arena() is a template. We use an overload set instead, as it
        // will let SWIG generate the appropriate dispatch code for us.
        c4::csubstr _to_arena_fundamental(uint8_t val) { return $self->to_arena(val); }
        c4::csubstr _to_arena_fundamental(int8_t val) { return $self->to_arena(val); }
        c4::csubstr _to_arena_fundamental(uint16_t val) { return $self->to_arena(val); }
        c4::csubstr _to_arena_fundamental(int16_t val) { return $self->to_arena(val); }
        c4::csubstr _to_arena_fundamental(uint32_t val) { return $self->to_arena(val); }
        c4::csubstr _to_arena_fundamental(int32_t val) { return $self->to_arena(val); }
        c4::csubstr _to_arena_fundamental(uint64_t val) { return $self->to_arena(val); }
        c4::csubstr _to_arena_fundamental(int64_t val) { return $self->to_arena(val); }
        c4::csubstr _to_arena_fundamental(float val) { return $self->to_arena(val); }
        c4::csubstr _to_arena_fundamental(double val) { return $self->to_arena(val); }
        c4::csubstr _to_arena_null() { return $self->to_arena(nullptr); }
        c4::csubstr _to_arena_buffer(c4::csubstr buf) { return $self->to_arena(buf); }
        %pythoncode %{
            def to_arena(self, val):
                if isinstance(val, (str, bytes, bytearray, memoryview)):
                    return _to_arena_buf_cpp(self, as_csubstr(val))
                elif val is None:
                    return self._to_arena_null()
                else:
                    return self._to_arena_fundamental(val)
        %}
    }
};

} // namespace yml
} // namespace c4
