import ryml


# ryml cannot accept strings because it does not take ownership of the
# source buffer; only bytes or bytearrays are accepted.
src = b"{HELLO: a, foo: b, bar: c, baz: d, seq: [0, 1, 2, 3]}"


# verify that the given tree is as expected from the source above
def check(tree: ryml.Tree):
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
    assert tree.is_seq(5)
    # use bytes objects for queries
    assert tree.find_child(0, b"HELLO") == 1
    assert tree.find_child(0, b"foo") == 2
    assert tree.find_child(0, b"seq") == 5
    assert tree.key(1) == b"HELLO"
    assert tree.val(1) == b"a"
    assert tree.key(2) == b"foo"
    assert tree.val(2) == b"b"
    assert tree.find_child(0, b"seq") == 5
    # hierarchy:
    assert tree.first_child(0) == 1
    assert tree.last_child(0) == 5
    assert tree.next_sibling(1) == 2
    assert tree.first_sibling(5) == 1
    assert tree.last_sibling(1) == 5
    assert tree.first_child(5) == 6
    assert tree.last_child(5) == 9
    # to loop over children:
    for i, ch in enumerate(ryml.children(tree, 5)):
        assert tree.val(ch) == [b"0", b"1", b"2", b"3"][i]
    # to loop over siblings:
    for i, sib in enumerate(ryml.siblings(tree, 5)):
        assert tree.key(sib) == [b"HELLO", b"foo", b"bar", b"baz", b"seq"][i]
    # to walk over all elements
    visited = [False] * tree.size()
    for node_id, indentation_level in ryml.walk(tree):
        visited[node_id] = True
    assert False not in visited
    # NOTE about encoding!
    k = tree.key(5)
    print(k)  # '<memory at 0x7f80d5b93f48>'
    assert k == b"seq"               # ok, as expected
    assert k != "seq"                # not ok - NOTE THIS!
    assert str(k) != "seq"           # not ok
    assert str(k, "utf8") == "seq"   # ok again


def test_immutable_buffer():
    # copy the source buffer to the tree arena and parse the copy
    tree = ryml.parse_in_arena(src)
    check(tree) # OK


def test_mutable_buffer():
    # parse a mutable buffer in place
    # requires bytearrays or objects offering writeable memory
    mutable_buffer = bytearray(src)
    # WATCHOUT: the tree is pointing into mutable_buffer!
    tree = ryml.parse_in_place(mutable_buffer)
    check(tree) # OK
