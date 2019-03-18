import ryml
import unittest


class SimpleHardcoded:

    yaml = "{HELLO: a, foo: b, bar: c, baz: d, seq: [0, 1, 2, 3]}"

    def check(self, ut, t):
        # some convenient shorthands
        eq = ut.assertEqual
        fs = ut.assertFalse
        tr = ut.assertTrue
        #
        eq(t.size(), 10)
        tr(t.is_root(0))
        eq(t.num_children(0), 5)
        eq(t.find_child(0, b"HELLO"), 1)
        eq(t.find_child(0, b"foo"), 2)
        eq(t.find_child(0, b"bar"), 3)
        eq(t.find_child(0, b"baz"), 4)
        eq(t.find_child(0, b"seq"), 5)
        eq(t.parent(0), ryml.NONE)
        eq(t.parent(1), 0)
        eq(t.parent(2), 0)
        eq(t.parent(3), 0)
        eq(t.parent(4), 0)
        eq(t.parent(5), 0)
        fs(t.is_root(1))
        fs(t.is_root(2))
        fs(t.is_root(3))
        fs(t.is_root(4))
        fs(t.is_root(5))
        fs(t.has_child(0, b"foozzie"))
        fs(t.has_child(0, b"bark"))
        fs(t.has_child(0, b"bart"))
        fs(t.has_child(0, b"bazk"))
        eq(t.next_sibling(0), ryml.NONE)
        eq(t.prev_sibling(0), ryml.NONE)
        eq(t.prev_sibling(1), ryml.NONE)
        eq(t.next_sibling(5), ryml.NONE)
        tr(t.has_child(0, b"HELLO"))
        tr(t.has_child(0, b"foo"))
        tr(t.has_child(0, b"bar"))
        tr(t.has_child(0, b"baz"))
        eq(t.key(1), b"HELLO")
        eq(t.key(2), b"foo")
        eq(t.key(3), b"bar")
        eq(t.key(4), b"baz")
        eq(t.key(5), b"seq")
        eq(t.val(1), b"a")
        eq(t.val(2), b"b")
        eq(t.val(3), b"c")
        eq(t.val(4), b"d")
        eq(t.val(6), b"0")
        eq(t.val(7), b"1")
        eq(t.val(8), b"2")
        eq(t.val(9), b"3")
        tr(t.has_sibling(1, b"bar"))
        tr(t.has_sibling(1, b"baz"))
        tr(t.has_sibling(2, b"foo"))
        tr(t.has_sibling(2, b"baz"))
        tr(t.has_sibling(3, b"foo"))
        tr(t.has_sibling(3, b"bar"))
        for i in (1, 2, 3, 4, 5):
            eq(t.find_sibling(i, b"HELLO"), 1)
            eq(t.find_sibling(i, b"foo"), 2)
            eq(t.find_sibling(i, b"bar"), 3)
            eq(t.find_sibling(i, b"baz"), 4)
            eq(t.find_sibling(i, b"seq"), 5)
        #
        num = 0
        for id in ryml.children(t):
            num += 1
            eq(id, num)
        eq(num, t.num_children(t.root_id()))
        eq(num, t.num_siblings(t.first_child(t.root_id())))
        #
        num = 0
        for id in ryml.children(t, 1):
            num += 1
        eq(num, 0)
        #
        num = 0
        for id in ryml.siblings(t, 1):
            num += 1
            eq(id, num)
        eq(num, t.num_children(t.root_id()))
        eq(num, t.num_siblings(t.first_child(t.root_id())))
        #
        num = 0
        for id in ryml.siblings(t, 3):
            num += 1
            eq(id, num)
        eq(num, 5)
        eq(num, t.num_siblings(t.first_child(t.root_id())))
        #
        num = 0
        for id in ryml.siblings(t, 0):
            num += 1
        eq(num, 1)
        #
        num = 0
        for id in ryml.walk(t):
            num += 1
            if t.is_root(id):
                eq(id, 0)
            if t.is_map(id):
                eq(id, 0)
            if t.is_seq(id):
                eq(id, 5)
            if t.is_keyval(id):
                tr(id > 0 and id < 5)
            if t.is_val(id):
                tr(id > 5)
        eq(num, t.size())
        #
        num = 0
        for id in ryml.walk(t, 5):
            num += 1
        eq(num, 5)
        #
        num = 0
        for id in ryml.walk(t, 9):
            num += 1
        eq(num, 1)


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
class _TestBase(unittest.TestCase):

    def _setUp(self, case=None):
        self.case = case
        self.src_as_str = str(case.yaml)
        self.src_as_bytes = bytes(case.yaml, "utf8")
        self.src_as_bytearray = bytearray(case.yaml, "utf8")

    # ----------------------------------------------------------
    def _test11_str__ro(self):  # cannot read string buffers (or can we?)
        with self.assertRaises(TypeError) as context:
            ryml.parse(self.src_as_str)
        self.assertTrue(type(context.exception), TypeError)

    def _test12_str__ro__reuse_tree(self):  # cannot read string buffers (or can we?)
        with self.assertRaises(TypeError) as context:
            t = ryml.Tree()
            ryml.parse(self.src_as_str, tree=t)
        self.assertTrue(type(context.exception), TypeError)

    def _test13_str__rw(self):  # cannot mutate string buffers (or can we?)
        with self.assertRaises(TypeError) as context:
            ryml.parse_in_situ(self.src_as_str)
        self.assertTrue(type(context.exception), TypeError)

    # ----------------------------------------------------------
    def _test21_bytes__ro(self):
        tree = ryml.parse(self.src_as_bytes)
        self.case.check(self, tree)

    def _test22_bytes__ro__reuse_tree(self):
        t = ryml.Tree()
        r = ryml.parse(self.src_as_bytes, tree=t)
        self.assertTrue(r is t)
        self.case.check(self, t)

    def _test23_bytes__rw(self):  # cannot mutate bytes buffers
        with self.assertRaises(TypeError) as context:
            ryml.parse_in_situ(self.src_as_bytes)
        self.assertTrue(type(context.exception), TypeError)

    # ----------------------------------------------------------
    def _test31_bytearray__ro(self):
        tree = ryml.parse(self.src_as_bytearray)
        self.case.check(self, tree)

    def _test32_bytearray__ro__reuse_tree(self):
        t = ryml.Tree()
        r = ryml.parse(self.src_as_bytearray, tree=t)
        self.assertTrue(r is t)
        self.case.check(self, t)

    def _test33_bytearray__rw(self):  # bytearray buffers are mutable
        tree = ryml.parse_in_situ(self.src_as_bytearray)
        self.case.check(self, tree)

    def _test34_bytearray__rw__reuse_tree(self):  # bytearray buffers are mutable
        t = ryml.Tree()
        r = ryml.parse_in_situ(self.src_as_bytearray, tree=t)
        self.assertTrue(r is t)
        self.case.check(self, t)


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
class TestSimpleHardCoded(_TestBase):

    def setUp(self):
        _TestBase._setUp(self, SimpleHardcoded())

    # ----------------------------------------------------------
    def test11_str__ro(self):
        super()._test11_str__ro()

    def test12_str__ro__reuse_tree(self):
        self._test12_str__ro__reuse_tree()

    def test13_str__rw(self):
        self._test13_str__rw()

    # ----------------------------------------------------------
    def test21_bytes__ro(self):
        self._test21_bytes__ro()

    def test22_bytes__ro__reuse_tree(self):
        self._test22_bytes__ro__reuse_tree()

    def test23_bytes__rw(self):
        self._test23_bytes__rw()

    # ----------------------------------------------------------
    def test31_bytearray__ro(self):
        self._test31_bytearray__ro()

    def test32_bytearray__ro__reuse_tree(self):
        self._test32_bytearray__ro__reuse_tree()

    def test33_bytearray__rw(self):
        self._test33_bytearray__rw()

    def test34_bytearray__rw__reuse_tree(self):
        self._test34_bytearray__rw__reuse_tree()


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
if __name__ == "__main__":
    unittest.main()
