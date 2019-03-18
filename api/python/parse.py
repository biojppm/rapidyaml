import ryml

print(type(b"fdx").__name__)
print(type(bytes("fdx", "utf8")).__name__)

ryml.fdx_(bytearray("haha", "utf8"))
#ryml.fdxw_(bytearray("haha", "utf8"))
ryml.fdx_(bytes("hehe", "utf8"))
#ryml.fdxw_(bytes("hehe", "utf8"))
ryml.fdx_(b"hihi")
#ryml.fdxw_(b"hihi")
ryml.fdx_(memoryview(b"hoho"))


#ryml.fdxw_("hoho")


def fn(obj, chk, tree):
    def log(*args, **kwargs):
        print("trying {} [{}]:".format(type(obj).__name__, obj),
              *args, **kwargs)
    for f in (ryml.parse, ryml.parse_in_situ):
        log("-------- with", f.__name__)
        try:
            r = f(obj)
            chk(r)
            tree.clear()
            tree.clear_arena()
            r = f(obj, tree=tree)
            assert r is tree
            chk(r)
        except TypeError as e:
            print(e)


def check(t):
    assert t.is_root(0)
    assert t.size() == 5
    assert t.num_children(0) == 4
    assert not t.has_child(0, b"foozzie")
    assert not t.has_child(0, b"bark")
    assert not t.has_child(0, b"bart")
    assert not t.has_child(0, b"bazk")
    assert t.has_child(0, b"HELLO")
    assert t.has_child(0, b"foo")
    assert t.has_child(0, b"bar")
    assert t.has_child(0, b"baz")
    assert t.has_sibling(1, b"bar")
    assert t.has_sibling(1, b"baz")
    assert t.has_sibling(2, b"foo")
    assert t.has_sibling(2, b"baz")
    assert t.has_sibling(3, b"foo")
    assert t.has_sibling(3, b"bar")
    assert t.find_child(0, b"HELLO") == 1
    assert t.find_child(0, b"foo") == 2
    assert t.find_child(0, b"bar") == 3
    assert t.find_child(0, b"baz") == 4
    assert t.parent(1) == 0
    assert t.parent(2) == 0
    assert t.parent(3) == 0
    assert t.parent(4) == 0
    for i in (1, 2, 3, 4):
        assert t.find_sibling(i, b"HELLO") == 1
        assert t.find_sibling(i, b"foo") == 2
        assert t.find_sibling(i, b"bar") == 3
        assert t.find_sibling(i, b"baz") == 4


t = ryml.Tree()
yaml = "{HELLO: a, foo: b, bar: c, baz: d}"
fn(yaml, check, t)
fn(bytearray(yaml, "utf8"), check, t)
fn(yaml.encode("utf8"), check, t)
