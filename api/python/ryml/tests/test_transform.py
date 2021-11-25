import unittest

import ryml

try:
    import mujson as json
except ImportError:
    import json


class TestTransform(unittest.TestCase):
    """Currently only transforms into strings, since IDK the way to get the original type."""

    matrix = {
        "3.1415": 3.1415,
        "1": 1,
        "false": False,
        "true": True,
        '"a"': "a",
        '{"a":"b"}': {"a": "b"},
        '["a", "b"]': ["a", "b"],
        '{"a": ["b", "c"], "e": null, "f": 3.1415, "g": true, "h": false, "i": 42, "j": "null"}': {
            "a": ["b", "c"],
            "e": None,
            "f": 3.1415,
            "g": True,
            "h": False,
            "i": 42,
            "j": "null",
        },
    }

    def testMatrix(self):
        etalon = {}
        for chall, resp in self.__class__.matrix.items():
            with self.subTest(resp=resp):
                actual = ryml.parse_to_python(chall)
                self.assertEqual(json.dumps(actual, indent="\t"), json.dumps(resp, indent="\t"))


if __name__ == "__main__":
    unittest.main()
