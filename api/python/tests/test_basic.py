import pytest
import ryml
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

@pytest.mark.parametrize("val,expected", scalar_test_cases)
def test_arena(val, expected):
    tree = ryml.Tree()
    assert tree.arena_size() == 0
    in_arena = tree.to_arena(val)
    assert isinstance(in_arena, memoryview)
    actual = str(in_arena, "utf8")
    assert actual == expected
    assert tree.arena_size() == len(expected)


def test_arena_none():
    val = None
    expected = None
    tree = ryml.Tree()
    assert tree.arena_size() == 0
    in_arena = tree.to_arena(val)
    assert tree.arena_size() == 0


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


# ------------------------------------------------

if __name__ == "__main__":
    import sys
    sys.exit(pytest.main(["-s", "-vvv"]))
