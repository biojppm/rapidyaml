
%module ryml

//-----------------------------------------------------------------------------
// this block will be pasted verbatim in the generated C++ source file

%{
// specifies that the resulting C file should be built as a python
// extension, inserting the module init code
#define SWIG_FILE_WITH_INIT

#include <c4/yml/yml.hpp>

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
  Py_buffer view;
  int ok = PyObject_CheckBuffer($input);
  if(ok)
  {
      ok = (0 == PyObject_GetBuffer($input, &view, PyBUF_SIMPLE|PyBUF_WRITABLE));
  }
  if(ok)
  {
      $1 = c4::substr((char*)view.buf, view.len);
      PyBuffer_Release(&view);
  }
  else
  {
      PyErr_SetString(PyExc_TypeError, "could not get mutable memory for c4::csubstr - have you passed a str?");
      SWIG_fail;
  }
#else
#error no "in" typemap defined for this export language
#endif
};

%typemap(in) c4::csubstr {
#if defined(SWIGPYTHON)
  Py_buffer view;
  view.buf = nullptr;
  int ok = PyObject_CheckBuffer($input);
  if(ok)
  {
      ok = (0 == PyObject_GetBuffer($input, &view, PyBUF_CONTIG_RO));
  }
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
          PyErr_SetString(PyExc_TypeError, "c4::csubstr: could not get readonly memory from python object");
          SWIG_fail;
      }
  }
#else
#error no "in" typemap defined for this export language
#endif
};
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
%include <exception.i>
%exception {
    try {
        $action
    } catch(std::exception &e) {
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
        // Use new[] to parse with delete[] in SWIG.
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
        // Use new[] to parse with delete[] in SWIG.
        char * alloc = new char[ret.len + 1]; // we'll return a c-string and not a csubstr
        c4::substr alloced_buf(alloc, ret.len);
        ret = c4::yml::emit_json(t, id, alloced_buf, /*error_on_excess*/true);
        ret.str[ret.len] = 0;
    }
    return ret.str;
}

size_t emit_yaml_length(const c4::yml::Tree &t, size_t id)
{
    c4::substr buf;
    c4::substr ret = c4::yml::emit_yaml(t, id, buf, /*error_on_excess*/false);
    return ret.len;
}

size_t emit_json_length(const c4::yml::Tree &t, size_t id)
{
    c4::substr buf;
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
    return _get_as_csubstr(s)

def as_substr(s):
    """return as a ryml::ssubstr"""
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
    """parse immutable YAML in the trees arena. Copy the YAML into a buffer
    in the C++ tree's arena, then parse the YAML from the trees arena.

    :param buf:
       the YAML buffer to be parsed
    :type buf: ``str``, ``bytes``, ``bytearray`` or ``memoryview``

    :param tree:
       a tree to be reused. When no tree is given, a new tree is created,
       and returned at the end.
    :type buf: ``ryml.Tree``
    """
    return _call_parse(parse_csubstr, buf, tree)

def parse_in_place(buf, tree=None):
    """parse in place a mutable buffer containing YAML. The resulting tree
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
    _check_valid_for_in_place(buf)
    return _call_parse(parse_substr, buf, tree)



def _call_parse(parse_fn, buf, tree):
    if tree is None:
        tree = Tree()
    parse_fn(buf, tree)
    return tree


def _check_valid_for_in_place(obj):
    if type(obj) in (str, bytes):  # is there a better heuristic?
        raise TypeError("cannot parse in place: " + type(obj).__name__)



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


@deprecated(deprecated_in="0.5.0", details="Use compute_emit_yaml_length() instead")
def compute_emit_length(tree, id=None):
    return compute_emit_yaml_length(tree, id)

def compute_emit_yaml_length(tree, id=None):
    if id is None:
        id = tree.root_id()
    return emit_yaml_length(tree, id)

def compute_emit_json_length(tree, id=None):
    if id is None:
        id = tree.root_id()
    return emit_json_length(tree, id)


@deprecated(deprecated_in="0.5.0", details="Use emit_yaml_in_place() instead")
def emit_in_place(tree, buf, id=None):
    return emit_yaml_in_place(tree, buf, id)

def emit_yaml_in_place(tree, buf, id=None):
    return _emit_fn_in_place(tree, buf, id, emit_yaml_to_substr)

def emit_json_in_place(tree, buf, id=None):
    return _emit_fn_in_place(tree, buf, id, emit_json_to_substr)

def _emit_fn_in_place(tree, buf, id, fn):
    if id is None:
        id = tree.root_id()
    (failed, expected_size) = fn(tree, id, buf)
    if failed:
        raise IndexError("Output buffer has {} bytes, but emit requires {} bytes".format(
            len(buf), expected_size))
    return memoryview(buf)[:expected_size]

%}


//-----------------------------------------------------------------------------

namespace c4 {
namespace yml {

constexpr const size_t NONE = (size_t)-1;

using type_bits = uint32_t;

typedef enum {
    NOTYPE  = 0,          ///< no type is set
    VAL     = (1<<0),     ///< a leaf node, has a (possibly empty) value
    KEY     = (1<<1),     ///< is member of a map, must have non-empty key
    MAP     = (1<<2),     ///< a map: a parent of keyvals
    SEQ     = (1<<3),     ///< a seq: a parent of vals
    DOC     = (1<<4),     ///< a document
    STREAM  = (1<<5)|SEQ, ///< a stream: a seq of docs
    KEYREF  = (1<<6),     ///< a *reference: the key references an &anchor
    VALREF  = (1<<7),     ///< a *reference: the val references an &anchor
    KEYANCH = (1<<8),     ///< the key has an &anchor
    VALANCH = (1<<9),     ///< the val has an &anchor
    KEYTAG  = (1<<10),    ///< the key has an explicit tag/type
    VALTAG  = (1<<11),    ///< the val has an explicit tag/type
    KEY_UNFILT  = (1<<12), ///< the key scalar was left unfiltered; the parser was set not to filter. @see ParserOptions
    VAL_UNFILT  = (1<<13), ///< the val scalar was left unfiltered; the parser was set not to filter. @see ParserOptions
    //
    // style flags:
    //
    FLOW_SL     = (1<<14), ///< mark container with single-line flow style (seqs as '[val1,val2], maps as '{key: val,key2: val2}')
    FLOW_ML     = (1<<15), ///< mark container with multi-line flow style (seqs as '[\n  val1,\n  val2\n], maps as '{\n  key: val,\n  key2: val2\n}')
    BLOCK       = (1<<16), ///< mark container with block style (seqs as '- val\n', maps as 'key: val')
    KEY_LITERAL = (1<<17), ///< mark key scalar as multiline, block literal |
    VAL_LITERAL = (1<<18), ///< mark val scalar as multiline, block literal |
    KEY_FOLDED  = (1<<19), ///< mark key scalar as multiline, block folded >
    VAL_FOLDED  = (1<<20), ///< mark val scalar as multiline, block folded >
    KEY_SQUO    = (1<<21), ///< mark key scalar as single quoted '
    VAL_SQUO    = (1<<22), ///< mark val scalar as single quoted '
    KEY_DQUO    = (1<<23), ///< mark key scalar as double quoted "
    VAL_DQUO    = (1<<24), ///< mark val scalar as double quoted "
    KEY_PLAIN   = (1<<25), ///< mark key scalar as plain scalar (unquoted, even when multiline)
    VAL_PLAIN   = (1<<26), ///< mark val scalar as plain scalar (unquoted, even when multiline)
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
} NodeType_e;

constexpr NodeType_e operator|  (NodeType_e lhs, NodeType_e rhs) noexcept;
constexpr NodeType_e operator&  (NodeType_e lhs, NodeType_e rhs) noexcept;
constexpr NodeType_e operator>> (NodeType_e bits, uint32_t n) noexcept;
constexpr NodeType_e operator<< (NodeType_e bits, uint32_t n) noexcept;
constexpr NodeType_e operator~  (NodeType_e bits) noexcept;
NodeType_e& operator&= (NodeType_e &subject, NodeType_e bits) noexcept;
NodeType_e& operator|= (NodeType_e &subject, NodeType_e bits) noexcept;


/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

struct NodeType
{
public:

    NodeType_e type;

public:

    NodeType() noexcept;
    NodeType(NodeType_e t) noexcept;
    NodeType(type_bits t) noexcept;

    bool has_any(NodeType_e t) const noexcept;
    bool has_all(NodeType_e t) const noexcept;
    bool has_none(NodeType_e t) const noexcept;

    void set(NodeType_e t) noexcept;
    void add(NodeType_e t) noexcept;
    void rem(NodeType_e t) noexcept;
    void addrem(NodeType_e bits_to_add, NodeType_e bits_to_remove) noexcept;

    void clear() noexcept;

public:

    /** @name node type queries
     * @{ */

    /** return a preset string based on the node type */
    const char *type_str() const noexcept;
    /** return a preset string based on the node type */
    static const char* type_str(NodeType_e t) noexcept;

    /** fill a string with the node type flags. If the string is small, returns {null, len} */
    c4::csubstr type_str(c4::substr buf) const noexcept;

    /** fill a string with the node type flags. If the string is small, returns {null, len}  */
    static c4::csubstr type_str(c4::substr buf, NodeType_e t) noexcept;

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
    bool is_flow_ml() const noexcept;
    bool is_flow() const noexcept;

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

    void set_container_style(NodeType_e style) noexcept;
    void set_key_style(NodeType_e style) noexcept;
    void set_val_style(NodeType_e style) noexcept;

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
    ~Tree();

    void reserve(id_type node_capacity);
    void reserve_arena(size_t arena_capacity);
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

    NodeType_e  type(id_type node) const;
    const char* type_str(id_type node) const;

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

    bool type_has_any(id_type node, NodeType_e bits) const;
    bool type_has_all(id_type node, NodeType_e bits) const;
    bool type_has_none(id_type node, NodeType_e bits) const;

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
    bool is_flow_ml(id_type node) const;
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

    void set_container_style(id_type node, NodeType_e style);
    void set_key_style(id_type node, NodeType_e style);
    void set_val_style(id_type node, NodeType_e style);

    /** @} */

public:

    void to_keyval(id_type node, c4::csubstr key, c4::csubstr val, int more_flags=0);
    void to_map(id_type node, c4::csubstr key, int more_flags=0);
    void to_seq(id_type node, c4::csubstr key, int more_flags=0);
    void to_val(id_type node, c4::csubstr val, int more_flags=0);
    void to_stream(id_type node, int more_flags=0);
    void to_map(id_type node, int more_flags=0);
    void to_seq(id_type node, int more_flags=0);
    void to_doc(id_type node, int more_flags=0);

    void set_key_tag(id_type node, c4::csubstr tag);
    void set_key_anchor(id_type node, c4::csubstr anchor);
    void set_val_anchor(id_type node, c4::csubstr anchor);
    void set_key_ref   (id_type node, c4::csubstr ref   );
    void set_val_ref   (id_type node, c4::csubstr ref   );

    void _set_key(id_type node, c4::csubstr key, int more_flags=0);
    void _set_val(id_type node, c4::csubstr val, int more_flags=0);

    void set_val_tag(id_type node, c4::csubstr tag);
    void rem_key_anchor(id_type node);
    void rem_val_anchor(id_type node);
    void rem_key_ref   (id_type node);
    void rem_val_ref   (id_type node);
    void rem_anchor_ref(id_type node);

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

};

} // namespace yml
} // namespace c4

//-----------------------------------------------------------------------------
