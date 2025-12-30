import ryml
import pytest
try:
    from ryml.ryml import _same_ptr, _same_mem
except:
    from ryml import _same_ptr, _same_mem


def _(id, *args):
    return pytest.param(*args, id=id)


scalar_test_cases_fundamental = [
    _("int0"       , 1, "1"),
    _("int1"       , 1000, "1000"),
    _("int2"       , 1000000, "1000000"),
    _("float0"     , 2.4, "2.4"),
    _("float1"     , 2.42, "2.42"),
    _("float1"     , 2.424, "2.424"),
]

scalar_test_cases_buffer = [
    _("str0"       , "", ""),
    _("str1"       , "scalar", "scalar"),
    _("str2"       , "thisisalongerstr", "thisisalongerstr"),
    _("bytes0"     , b"", ""),
    _("bytes1"     , b"scalar2", "scalar2"),
    _("bytes2"     , b"thisisalongerstr2", "thisisalongerstr2"),
    _("bytearray0" , bytearray("", "utf8"), ""),
    _("bytearray1" , bytearray("scalar3", "utf8"), "scalar3"),
    _("bytearray2" , bytearray("longerscalar3", "utf8"), "longerscalar3"),
    _("memoryview0", memoryview(bytearray("", "utf8")), ""),
    _("memoryview1", memoryview(bytearray("scalar5", "utf8")), "scalar5"),
    _("memoryview2", memoryview(bytearray("longerscalar5", "utf8")), "longerscalar5"),
    _("bytearray", bytearray(b"bytearray"), "bytearray"),
    _("memoryview(str)", memoryview("str".encode("utf-8")), "str"),
    _("memoryview(bytes)", memoryview(b"bytes"), "bytes"),
    _("memoryview(bytearray)", memoryview(bytearray(b"memoryview")), "memoryview"),
]

scalar_test_cases = scalar_test_cases_fundamental + scalar_test_cases_buffer


# ------------------------------------------------
# csubstr

@pytest.mark.parametrize("s,expected", scalar_test_cases_buffer)
def test_csubstr(s, expected):
    m = ryml.as_csubstr(s)
    assert isinstance(m, memoryview)
    assert _same_ptr(s, m)
    assert _same_mem(s, m)
    assert expected == str(m, "utf8")
    #
    m = ryml.as_csubstr(m)
    assert isinstance(m, memoryview)
    assert _same_ptr(s, m)
    assert _same_mem(s, m)
    assert expected == str(m, "utf8")


# ------------------------------------------------
# substr

@pytest.mark.parametrize("s,expected", scalar_test_cases_buffer)
def test_substr(s, expected):
    if isinstance(s, (str, bytes)):
        with pytest.raises(TypeError):
            m = ryml.as_substr(s)
    else:
        m = ryml.as_substr(s)
        assert isinstance(m, memoryview)
        assert _same_ptr(s, m)
        assert _same_mem(s, m)
        assert expected == str(m, "utf8")
        #
        m = ryml.as_substr(m)
        assert isinstance(m, memoryview)
        assert _same_ptr(s, m)
        assert _same_mem(s, m)
        assert expected == str(m, "utf8")


# ------------------------------------------------
# arena

def test_arena_none():
    val = None
    expected = None
    tree = ryml.Tree()
    assert tree.arena_size() == 0
    in_arena = tree.to_arena(val)
    assert tree.arena_size() == 0

@pytest.mark.parametrize("val,expected", scalar_test_cases)
def test_to_arena(val, expected):
    tree = ryml.Tree()
    assert tree.arena_size() == 0
    in_arena = tree.to_arena(val)
    assert isinstance(in_arena, memoryview)
    actual = str(in_arena, "utf8")
    assert actual == expected
    actual = bytearray(in_arena).decode("utf-8")
    assert actual == expected
    assert tree.arena_size() == len(expected)



# ------------------------------------------------

def test_instantiate_tree():
    tree = ryml.Tree()
    assert not tree.empty()
    root = tree.root_id()
    tree.to_seq(root)
    tree = ryml.Tree(0)
    assert tree.empty()
    assert tree.size() == 0
    assert tree.capacity() == 0
    assert tree.slack() == 0
    assert tree.arena_size() == 0
    assert tree.arena_capacity() == 0
    assert tree.arena_slack() == 0
    tree = ryml.Tree(0, 10)
    assert tree.empty()
    assert tree.size() == 0
    assert tree.capacity() == 0
    assert tree.slack() == 0
    assert tree.arena_size() == 0
    assert tree.arena_capacity() == 10
    assert tree.arena_slack() == 10
    tree = ryml.Tree(10, 0)
    assert not tree.empty()
    assert tree.size() == 1 # from the root
    assert tree.capacity() == 10
    assert tree.slack() == 9
    assert tree.arena_size() == 0
    assert tree.arena_capacity() == 0
    assert tree.arena_slack() == 0
    tree = ryml.Tree(10, 10)
    assert not tree.empty()
    assert tree.size() == 1 # from the root
    assert tree.capacity() == 10
    assert tree.slack() == 9
    assert tree.arena_size() == 0
    assert tree.arena_capacity() == 10
    assert tree.arena_slack() == 10


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

def _copy(s):
    " force a copy of the src string and return as buftype "
    if isinstance(s, str):
        cp = (s + ".")[:-1]  # https://stackoverflow.com/questions/24804453/how-can-i-copy-a-python-string
    else:
        cp = (s + b".")[:-1]  # https://stackoverflow.com/questions/24804453/how-can-i-copy-a-python-string
    assert id(cp) != id(s)
    return cp


def _copy_as(src, buftype):
    cp = _copy(src)
    if isinstance(cp, bytes) and  (buftype is str):
        buf = buftype(cp, "utf8")
    else:
        buf = buftype(cp)
    assert id(buf) != id(src)
    assert isinstance(buf, buftype)
    return cp, buf


def _addmap(t, node, k=None, flags=0):
    m = t.append_child(node)
    if k is None:
        t.to_map(m, flags)
    else:
        t.to_map(m, k, flags)
    return m


def _addseq(t, node, k=None, flags=0):
    m = t.append_child(node)
    if k is None:
        t.to_seq(m, flags)
    else:
        t.to_seq(m, k, flags)
    return m


def _addval(t, node, k, v=None, flags=0):
    ch = t.append_child(node)
    if v is None:
        t.to_val(ch, k, flags)
    else:
        t.to_keyval(ch, k, v, flags)
    return ch


class TransformScalar:
    def __init__(self, tree, in_arena):
        self.tree = tree
        self.in_arena = in_arena
    def __call__(self, s):
        if self.in_arena or not isinstance(s, str):
            s = self.tree.to_arena(s)
        return s


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

class ScalarCase:

    yaml = b"this is a scalar"

    def mktree(self, *args, **kwargs):
        tree = ryml.Tree()
        _s = TransformScalar(tree, *args, **kwargs)
        s = _s(self.yaml)
        tree.to_val(tree.root_id(), s, ryml.VAL_PLAIN)
        return tree

    def check(self, tree: ryml.Tree):
        assert tree.size() == 1
        assert tree.root_id() == 0
        assert tree.is_val(0)
        assert tree.is_val_plain(0)
        assert tree.val(0) == self.yaml

    def check_json(self, tree: ryml.Tree):
        assert tree.size() == 1
        assert tree.root_id() == 0
        assert tree.is_val(0)
        assert tree.is_val_dquo(0)
        assert tree.val(0) == self.yaml


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

class CompactCase:

    yaml = b"{HELLO: a, foo: b, bar: c, baz: d, seq: [0, 1, 2, 3]}"

    def mktree(self, *args, **kwargs):
        tree = ryml.Tree()
        _s = TransformScalar(tree, *args, **kwargs)
        root = tree.root_id()
        tree.to_map(root, ryml.FLOW_SL)
        _addval(tree, root, _s("HELLO"), _s("a"))
        _addval(tree, root, _s("foo"), _s("b"))
        _addval(tree, root, _s("bar"), _s("c"))
        _addval(tree, root, _s("baz"), _s("d"))
        seq = _addseq(tree, root, _s("seq"), ryml.FLOW_SL)
        _addval(tree, seq, _s("0"))
        _addval(tree, seq, _s("1"))
        _addval(tree, seq, _s("2"))
        _addval(tree, seq, _s("3"))
        return tree

    # verify that the given tree is as expected from the source above
    def check(self, tree: ryml.Tree):
        # For now, only the index-based low-level API is implemented.
        # Here's the node structure for a tree parsed from the source
        # above:
        #
        # [node 0] root, map
        #   ` [node 1] "HELLO": "a"
        #   ` [node 2] "foo": "b"
        #   ` [node 3] "bar": "c"
        #   ` [node 4] "baz": "d"
        #   ` [node 5] "seq":
        #       ` [node 6] "0"
        #       ` [node 7] "1"
        #       ` [node 8] "2"
        #       ` [node 9] "3"
        #
        # let's now do some assertions:
        assert tree.size() == 10
        assert tree.root_id() == 0
        assert tree.is_root(0)
        assert tree.is_map(0)
        assert tree.is_keyval(1)
        assert tree.is_seq(5)
        assert tree.is_val(6)
        # use bytes objects for queries
        assert tree.find_child(0, b"HELLO") == 1
        assert tree.find_child(0, "HELLO") == 1
        assert tree.find_child(0, b"foo") == 2
        assert tree.find_child(0, "foo") == 2
        assert tree.find_child(0, b"seq") == 5
        assert tree.find_child(0, "seq") == 5
        assert tree.key(1) == b"HELLO"
        assert tree.val(1) == b"a"
        assert tree.key(2) == b"foo"
        assert tree.val(2) == b"b"
        assert tree.find_child(0, b"seq") == 5
        assert tree.find_child(0, "seq") == 5
        # hierarchy:
        assert tree.first_child(0) == 1
        assert tree.last_child(0) == 5
        assert tree.next_sibling(1) == 2
        assert tree.first_sibling(5) == 1
        assert tree.last_sibling(1) == 5
        assert tree.first_child(5) == 6
        assert tree.last_child(5) == 9
        # to loop over children:
        expected = [b"0", b"1", b"2", b"3"]
        for i, ch in enumerate(ryml.children(tree, 5)):
            assert tree.val(ch) == expected[i]
        # to loop over siblings:
        expected = [b"HELLO", b"foo", b"bar", b"baz", b"seq"]
        for i, sib in enumerate(ryml.siblings(tree, 5)):
            assert tree.key(sib) == expected[i]
        # to walk over all elements
        visited = [False] * tree.size()
        for node_id, indentation_level in ryml.walk(tree):
            visited[node_id] = True
        assert False not in visited
        # NOTE about encoding!
        k = tree.key(5)
        assert isinstance(k, memoryview)
        #print(k)  # '<memory at 0x7f80d5b93f48>'
        assert k == b"seq"               # ok, as expected
        assert k != "seq"                # not ok - NOTE THIS!
        assert str(k) != "seq"           # not ok
        assert str(k, "utf8") == "seq"   # ok again

    def check_json(self, tree: ryml.Tree):
        self.check(tree)


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

class ModifyingCase:

    yaml = b"{'HELLO': a, foo: \"b\", bar: c, baz: d, seq: [0, 1, 2, 3]}"

    def mktree(self, *args, **kwargs):
        tree = ryml.Tree()
        _s = TransformScalar(tree, *args, **kwargs)
        root = tree.root_id()
        tree.to_map(root, ryml.FLOW_SL)
        _addval(tree, root, _s("HELLO"), _s("a"), ryml.KEY_SQUO)
        _addval(tree, root, _s("foo"), _s("b"), ryml.VAL_DQUO)
        _addval(tree, root, _s("bar"), _s("c"))
        _addval(tree, root, _s("baz"), _s("d"))
        seq = _addseq(tree, root, _s("seq"), ryml.FLOW_SL)
        _addval(tree, seq, _s("0"))
        _addval(tree, seq, _s("1"))
        _addval(tree, seq, _s("2"))
        _addval(tree, seq, _s("3"))
        return tree

    def check(self, t):
        self._check(t, is_json=False)

    def check_json(self, t):
        self._check(t, is_json=True)

    def _check(self, t, is_json):
        assert t.size() == 10
        assert t.is_root(0)
        assert t.num_children(0) == 5
        assert t.find_child(0, b"HELLO") == 1
        assert t.find_child(0, "HELLO") == 1
        assert t.find_child(0, b"foo") == 2
        assert t.find_child(0, "foo") == 2
        assert t.find_child(0, b"bar") == 3
        assert t.find_child(0, "bar") == 3
        assert t.find_child(0, b"baz") == 4
        assert t.find_child(0, "baz") == 4
        assert t.find_child(0, b"seq") == 5
        assert t.find_child(0, "seq") == 5
        assert t.parent(0) == ryml.NONE
        assert t.parent(1) == 0
        assert t.parent(2) == 0
        assert t.parent(3) == 0
        assert t.parent(4) == 0
        assert t.parent(5) == 0
        assert not t.is_root(1)
        assert not t.is_root(2)
        assert not t.is_root(3)
        assert not t.is_root(4)
        assert not t.is_root(5)
        assert not t.has_child(0, b"foozzie")
        assert not t.has_child(0, b"bark")
        assert not t.has_child(0, b"bart")
        assert not t.has_child(0, b"bazk")
        assert t.next_sibling(0) == ryml.NONE
        assert t.prev_sibling(0) == ryml.NONE
        assert t.prev_sibling(1) == ryml.NONE
        assert t.next_sibling(5) == ryml.NONE
        assert t.has_child(0, b"HELLO")
        assert t.has_child(0, b"foo")
        assert t.has_child(0, b"bar")
        assert t.has_child(0, b"baz")
        assert t.key(1) == b"HELLO"
        assert t.key(2) == b"foo"
        assert t.key(3) == b"bar"
        assert t.key(4) == b"baz"
        assert t.key(5) == b"seq"
        assert t.val(1) == b"a"
        assert t.val(2) == b"b"
        assert t.val(3) == b"c"
        assert t.val(4) == b"d"
        assert t.val(6) == b"0"
        assert t.val(7) == b"1"
        assert t.val(8) == b"2"
        assert t.val(9) == b"3"
        if not is_json:
            assert t.is_key_squo(1)
            assert not t.is_key_squo(2)
            assert not t.is_key_squo(3)
            assert not t.is_key_squo(4)
            assert not t.is_key_squo(5)
            assert t.is_key_quoted(1)
            assert not t.is_key_quoted(2)
            assert not t.is_key_quoted(3)
            assert not t.is_key_quoted(4)
            assert not t.is_key_quoted(5)
        else:
            assert t.is_key_dquo(1)
            assert t.is_key_dquo(2)
            assert t.is_key_dquo(3)
            assert t.is_key_dquo(4)
            assert t.is_key_dquo(5)
            assert t.is_key_quoted(1)
            assert t.is_key_quoted(2)
            assert t.is_key_quoted(3)
            assert t.is_key_quoted(4)
            assert t.is_key_quoted(5)
        if not is_json:
            assert not t.is_val_quoted(1)
            assert t.is_val_quoted(2)
            assert not t.is_val_quoted(3)
            assert not t.is_val_quoted(4)
            assert not t.is_val_quoted(5)
            assert not t.is_val_quoted(6)
            assert not t.is_val_quoted(7)
            assert not t.is_val_quoted(8)
            assert not t.is_val_quoted(9)
        else:
            assert t.is_val_quoted(1)
            assert t.is_val_quoted(2)
            assert t.is_val_quoted(3)
            assert t.is_val_quoted(4)
            assert not t.is_val_quoted(5)
            assert not t.is_val_quoted(6)
            assert not t.is_val_quoted(7)
            assert not t.is_val_quoted(8)
            assert not t.is_val_quoted(9)
        if not is_json:
            assert t.is_quoted(1)
            assert t.is_quoted(2)
            assert not t.is_quoted(3)
            assert not t.is_quoted(4)
            assert not t.is_quoted(5)
            assert not t.is_quoted(6)
            assert not t.is_quoted(7)
            assert not t.is_quoted(8)
            assert not t.is_quoted(9)
        else:
            assert t.is_quoted(1)
            assert t.is_quoted(2)
            assert t.is_quoted(3)
            assert t.is_quoted(4)
            assert t.is_quoted(5)
            assert not t.is_quoted(6)
            assert not t.is_quoted(7)
            assert not t.is_quoted(8)
            assert not t.is_quoted(9)
        assert t.has_sibling(1, b"bar")
        assert t.has_sibling(1, b"baz")
        assert t.has_sibling(2, b"foo")
        assert t.has_sibling(2, b"baz")
        assert t.has_sibling(3, b"foo")
        assert t.has_sibling(3, b"bar")
        for i in (1, 2, 3, 4, 5):
            assert t.find_sibling(i, b"HELLO") == 1
            assert t.find_sibling(i, b"foo") == 2
            assert t.find_sibling(i, b"bar") == 3
            assert t.find_sibling(i, b"baz") == 4
            assert t.find_sibling(i, b"seq") == 5
        #
        num = 0
        for id in ryml.children(t):
            num += 1
            assert id == num
        assert num == t.num_children(t.root_id())
        assert num == t.num_siblings(t.first_child(t.root_id()))
        #
        num = 0
        for id in ryml.children(t, 1):
            num += 1
        assert num == 0
        #
        num = 0
        for id in ryml.siblings(t, 1):
            num += 1
            assert id == num
        assert num == t.num_children(t.root_id())
        assert num == t.num_siblings(t.first_child(t.root_id()))
        #
        num = 0
        for id in ryml.siblings(t, 3):
            num += 1
            assert id == num
        assert num == 5
        assert num == t.num_siblings(t.first_child(t.root_id()))
        #
        for i, ch in enumerate(ryml.children(t, 5)):
            assert t.val(ch), [b"0", b"1", b"2" == b"3"][i]
        sibs = [b"HELLO", b"foo", b"bar", b"baz", b"seq"]
        sibs_s = ["HELLO", "foo", "bar", "baz", "seq"]
        for i, sib in enumerate(ryml.siblings(t, 5)):
            k = t.key(sib)
            k_s = str(k, "utf8")
            assert k == sibs[i]
            assert k_s == sibs_s[i]
            assert k != sibs_s[i]
            assert k_s != sibs[i]
            k_s = str(k)
            assert k_s != sibs_s[i]
            assert k_s != sibs[i]
        num = 0
        for id in ryml.siblings(t, 0):
            num += 1
        assert num == 1
        #
        num = 0
        for id, level in ryml.walk(t):
            num += 1
            if t.is_root(id):
                assert id == 0
                assert level == 0
            if t.is_map(id):
                assert id == 0
                assert level == 0
            if t.is_seq(id):
                assert id == 5
                assert level == 1
            if t.is_keyval(id):
                assert id > 0 and id < 5
            if t.is_val(id):
                assert id > 5
                assert level == 2
        assert num == t.size()
        #
        num = 0
        for id in ryml.walk(t, 5):
            num += 1
        assert num == 5
        #
        num = 0
        for id in ryml.walk(t, 9):
            num += 1
        assert num == 1
        _check_tree_mod(t)


def _check_tree_mod(t):
    # some convenient shorthands
    def _addval_and_check(node, k, v=None):
        ch = _addval(t, node, k, v)
        pos = t.child_pos(node, ch)
        assert t.child(node, pos) == ch
        if v is not None:
            assert t.find_child(node, k) == ch
            assert t.child(node, pos), t.find_child(node == k)
        return ch
    def _addseq_and_check(node, k):
        ch = _addseq(t, node, k)
        assert t.find_child(node, k) == ch
        return ch
    def _addmap_and_check(node, k):
        ch = _addmap(t, node, k)
        assert t.find_child(node, k) == ch
        return ch
    m = _addmap_and_check(t.root_id(), "check_tree_mod_map")
    _addval_and_check(m, "k1", "v1")
    _addval_and_check(m, "k2", "v2")
    _addval_and_check(m, "k3", "v3")
    assert t.num_children(m) == 3
    assert t.num_siblings(t.first_child(m)) == 3
    s = _addseq_and_check(t.root_id(), "check_tree_mod_seq")
    _addval_and_check(s, "v1")
    _addval_and_check(s, "v2")
    _addval_and_check(s, "v3")
    assert t.num_children(s) == 3
    assert t.num_siblings(t.first_child(m)) == 3


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------


testclasses = [ScalarCase, CompactCase, ModifyingCase]
arena_types = [str, bytes, bytearray, memoryview]
in_place_types = [bytearray, memoryview]
in_place_types_error = [str, bytes]


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

@pytest.mark.parametrize("testclass", testclasses)
@pytest.mark.parametrize("buftype", arena_types)
def test_parse_in_arena(testclass, buftype):
    testcase = testclass()
    cp, buf = _copy_as(testcase.yaml, buftype)
    tree = ryml.parse_in_arena(buf)
    testcase.check(tree)


@pytest.mark.parametrize("testclass", testclasses)
@pytest.mark.parametrize("buftype", arena_types)
def test_parse_in_arena_reuse(testclass, buftype):
    testcase = testclass()
    cp, buf = _copy_as(testcase.yaml, buftype)
    tree = ryml.Tree()
    result = ryml.parse_in_arena(buf, tree=tree)
    assert result is tree
    testcase.check(tree)


@pytest.mark.parametrize("testclass", testclasses)
@pytest.mark.parametrize("buftype", in_place_types)
def test_parse_in_place(testclass, buftype):
    testcase = testclass()
    cp, buf = _copy_as(testcase.yaml, buftype)
    tree = ryml.parse_in_place(buf)
    testcase.check(tree)


@pytest.mark.parametrize("testclass", testclasses)
@pytest.mark.parametrize("buftype", in_place_types)
def test_parse_in_place_reuse(testclass, buftype):
    testcase = testclass()
    cp, buf = _copy_as(testcase.yaml, buftype)
    tree = ryml.Tree()
    result = ryml.parse_in_place(buf, tree=tree)
    assert result is tree
    testcase.check(tree)


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

# test type errors

@pytest.mark.parametrize("testclass", testclasses)
@pytest.mark.parametrize("buftype", in_place_types_error)
def test_parse_in_place__type_error(testclass, buftype):
    testcase = testclass()
    cp, buf = _copy_as(testcase.yaml, buftype)
    with pytest.raises(TypeError):
        tree = ryml.parse_in_place(buf)


@pytest.mark.parametrize("testclass", testclasses)
@pytest.mark.parametrize("buftype", in_place_types_error)
def test_parse_in_place_reuse__type_error(testclass, buftype):
    testcase = testclass()
    cp, buf = _copy_as(testcase.yaml, buftype)
    tree = ryml.Tree()
    with pytest.raises(TypeError):
        ryml.parse_in_place(buf, tree=tree)


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

# test parse errors

parse_error_cases = [
    _("error0", b"[HELLO: b}"),
    _("error1", b"{HELLO: b]"),
]

@pytest.mark.parametrize("testcase", parse_error_cases)
@pytest.mark.parametrize("buftype", arena_types)
def test_parse_error_in_arena(testcase, buftype):
    cp, buf = _copy_as(testcase, buftype)
    with pytest.raises(ryml.ExceptionParse):
        tree = ryml.parse_in_arena(buf)


@pytest.mark.parametrize("testcase", parse_error_cases)
@pytest.mark.parametrize("buftype", arena_types)
def test_parse_error_in_arena_reuse(testcase, buftype):
    cp, buf = _copy_as(testcase, buftype)
    tree = ryml.Tree()
    with pytest.raises(ryml.ExceptionParse):
        ryml.parse_in_arena(buf, tree=tree)


@pytest.mark.parametrize("testcase", parse_error_cases)
@pytest.mark.parametrize("buftype", in_place_types)
def test_parse_error_in_place(testcase, buftype):
    cp, buf = _copy_as(testcase, buftype)
    with pytest.raises(ryml.ExceptionParse):
        tree = ryml.parse_in_place(buf)


@pytest.mark.parametrize("testcase", parse_error_cases)
@pytest.mark.parametrize("buftype", in_place_types)
def test_parse_error_in_place_reuse(testcase, buftype):
    cp, buf = _copy_as(testcase, buftype)
    tree = ryml.Tree()
    with pytest.raises(ryml.ExceptionParse):
        ryml.parse_in_place(buf, tree=tree)


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

# test mktree

@pytest.mark.parametrize("in_arena", [
    "static",
    "in_arena"
])
@pytest.mark.parametrize("testclass", testclasses)
def test_mktree(testclass, in_arena):
    testcase = testclass()
    in_arena = (in_arena == "in_arena")
    output_tree = testcase.mktree(in_arena=in_arena)
    yaml = ryml.emit_yaml(output_tree)
    testcase.check(output_tree)



# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

# test emit

@pytest.mark.parametrize("testclass", testclasses)
@pytest.mark.parametrize("buftype", arena_types)
def test_emit_yaml(testclass, buftype):
    testcase = testclass()
    cp, buf = _copy_as(testcase.yaml, buftype)
    tree = ryml.parse_in_arena(buf)
    yaml = ryml.emit_yaml(tree)
    output_tree = ryml.parse_in_arena(yaml)
    testcase.check(output_tree)


@pytest.mark.parametrize("testclass", testclasses)
@pytest.mark.parametrize("buftype", arena_types)
def test_emit_json(testclass, buftype):
    testcase = testclass()
    cp, buf = _copy_as(testcase.yaml, buftype)
    tree = ryml.parse_in_arena(buf)
    yaml = ryml.emit_json(tree)
    output_tree = ryml.parse_in_arena(yaml)
    testcase.check_json(output_tree)


# -----------------------------------------------------------------------------

@pytest.mark.parametrize("testclass", testclasses)
@pytest.mark.parametrize("buftype", arena_types)
def test_compute_yaml_length(testclass, buftype):
    testcase = testclass()
    cp, buf = _copy_as(testcase.yaml, buftype)
    tree = ryml.parse_in_arena(buf)
    yaml = ryml.emit_yaml(tree)
    length = ryml.compute_yaml_length(tree)
    assert len(yaml) == length


@pytest.mark.parametrize("testclass", testclasses)
@pytest.mark.parametrize("buftype", arena_types)
def test_compute_json_length(testclass, buftype):
    testcase = testclass()
    cp, buf = _copy_as(testcase.yaml, buftype)
    tree = ryml.parse_in_arena(buf)
    json = ryml.emit_json(tree)
    length = ryml.compute_json_length(tree)
    assert len(json) == length


# -----------------------------------------------------------------------------

@pytest.mark.parametrize("testclass", testclasses)
@pytest.mark.parametrize("buftype", arena_types)
def test_emit_yaml_in_place(testclass, buftype):
    testcase = testclass()
    cp, buf = _copy_as(testcase.yaml, buftype)
    tree = ryml.parse_in_arena(buf)
    yaml = ryml.emit_yaml(tree)
    length = ryml.compute_yaml_length(tree)
    assert len(yaml) == length
    outbuf = bytearray(length)
    out = ryml.emit_yaml_in_place(tree, outbuf)
    assert len(out) == length
    assert out.tobytes().decode('utf8') == yaml


@pytest.mark.parametrize("testclass", testclasses)
@pytest.mark.parametrize("buftype", arena_types)
def test_emit_json_in_place(testclass, buftype):
    testcase = testclass()
    cp, buf = _copy_as(testcase.yaml, buftype)
    tree = ryml.parse_in_arena(buf)
    json = ryml.emit_json(tree)
    length = ryml.compute_json_length(tree)
    assert len(json) == length
    outbuf = bytearray(length)
    out = ryml.emit_json_in_place(tree, outbuf)
    assert len(out) == length
    assert out.tobytes().decode('utf8') == json


# -----------------------------------------------------------------------------

@pytest.mark.parametrize("testclass", testclasses)
@pytest.mark.parametrize("buftype", arena_types)
def test_emit_yaml_in_place_shortbuf(testclass, buftype):
    testcase = testclass()
    cp, buf = _copy_as(testcase.yaml, buftype)
    tree = ryml.parse_in_arena(buf)
    length = ryml.compute_yaml_length(tree)
    outbuf = bytearray(int(length / 2))
    with pytest.raises(IndexError):
        out = ryml.emit_yaml_in_place(tree, outbuf)


@pytest.mark.parametrize("testclass", testclasses)
@pytest.mark.parametrize("buftype", arena_types)
def test_emit_json_in_place_shortbuf(testclass, buftype):
    testcase = testclass()
    cp, buf = _copy_as(testcase.yaml, buftype)
    tree = ryml.parse_in_arena(buf)
    length = ryml.compute_json_length(tree)
    outbuf = bytearray(int(length / 2))
    with pytest.raises(IndexError):
        out = ryml.emit_json_in_place(tree, outbuf)



# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

if __name__ == "__main__":
    import sys
    sys.exit(pytest.main(["-s", "-vvv", __file__]))
