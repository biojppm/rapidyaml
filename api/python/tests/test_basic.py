import ryml


def test_create_empty_tree():
    tree = ryml.Tree(0)
    assert tree.empty()


def test_create_tree():
    tree = ryml.Tree()
    assert not tree.empty()
    root = tree.root_id()
    tree.to_seq(root)
