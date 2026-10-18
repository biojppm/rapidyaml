#ifndef C4_YML_TEST_TEST_GROUP_TEST_GROUP_DEF_HPP_
#define C4_YML_TEST_TEST_GROUP_TEST_GROUP_DEF_HPP_

#ifndef C4_RYML_TEST_GROUP_HPP_
#include "./test_lib/test_group.hpp"
#endif

namespace c4 {
namespace yml {

inline int YmlTestCaseDefsWereIncluded() { return 42; }


//-----------------------------------------------------------------------------

TEST_P(YmlTestCase, recreate_from_ref)
{
    SCOPED_TRACE("\n" + c->filelinebuf + ": case");
    d->ensure_reftree(c);
}


//-----------------------------------------------------------------------------

TEST_P(YmlTestCase, parse_yaml_tree_unix)
{
    SCOPED_TRACE("unix style\n" + c->filelinebuf + ": case");
    _test_parse_yaml_to_tree(&d->unix_style);
    {
        SCOPED_TRACE("redo parse to existing tree");
        SrcTree &tree = d->unix_style.tree;
        tree.val.result.clear();
        tree.src.result = tree.orig.result;
        tree.reset();
        _test_parse_yaml_to_tree(&d->unix_style);
    }
}

TEST_P(YmlTestCase, parse_yaml_tree_windows)
{
    SCOPED_TRACE("windows style\n" + c->filelinebuf + ": case");
    _test_parse_yaml_to_tree(&d->windows_style);
}


//-----------------------------------------------------------------------------

TEST_P(YmlTestCase, parse_yaml_ints_resize_unix)
{
    SCOPED_TRACE("unix style\n" + c->filelinebuf + ": case");
    _test_parse_yaml_to_ints_resize(&d->unix_style);
}

TEST_P(YmlTestCase, parse_yaml_ints_resize_windows)
{
    SCOPED_TRACE("windows style\n" + c->filelinebuf + ": case");
    _test_parse_yaml_to_ints_noresize(&d->windows_style);
}


TEST_P(YmlTestCase, parse_yaml_ints_noresize_unix)
{
    SCOPED_TRACE("unix style\n" + c->filelinebuf + ": case");
    _test_parse_yaml_to_ints_noresize(&d->unix_style);
}

TEST_P(YmlTestCase, parse_yaml_ints_noresize_windows)
{
    SCOPED_TRACE("windows style\n" + c->filelinebuf + ": case");
    _test_parse_yaml_to_ints_noresize(&d->windows_style);
}


//-----------------------------------------------------------------------------

TEST_P(YmlTestCase, roundtrip_yaml_tree_unix)
{
    SCOPED_TRACE("unix style\n" + c->filelinebuf + ": case");
    _test_roundtrip_yaml_tree(&d->unix_style);
}

TEST_P(YmlTestCase, roundtrip_yaml_tree_windows)
{
    SCOPED_TRACE("windows style\n" + c->filelinebuf + ": case");
    _test_roundtrip_yaml_tree(&d->windows_style);
}


//-----------------------------------------------------------------------------

TEST_P(YmlTestCase, roundtrip_yaml_ints_resize_unix)
{
    SCOPED_TRACE("unix style\n" + c->filelinebuf + ": case");
    _test_roundtrip_yaml_ints_resize(&d->unix_style);
}

TEST_P(YmlTestCase, roundtrip_yaml_ints_resize_windows)
{
    SCOPED_TRACE("windows style\n" + c->filelinebuf + ": case");
    _test_roundtrip_yaml_ints_resize(&d->windows_style);
}


TEST_P(YmlTestCase, roundtrip_yaml_ints_noresize_unix)
{
    SCOPED_TRACE("unix style\n" + c->filelinebuf + ": case");
    _test_roundtrip_yaml_ints_noresize(&d->unix_style);
}

TEST_P(YmlTestCase, roundtrip_yaml_ints_noresize_windows)
{
    SCOPED_TRACE("windows style\n" + c->filelinebuf + ": case");
    _test_roundtrip_yaml_ints_noresize(&d->windows_style);
}


//-----------------------------------------------------------------------------

TEST_P(YmlTestCase, roundtrip_json_tree_unix)
{
    SCOPED_TRACE("unix style\n" + c->filelinebuf + ": case");
    _test_roundtrip_json_tree(&d->unix_style);
}

TEST_P(YmlTestCase, roundtrip_json_tree_windows)
{
    SCOPED_TRACE("windows style\n" + c->filelinebuf + ": case");
    _test_roundtrip_json_tree(&d->windows_style);
}


//-----------------------------------------------------------------------------

TEST_P(YmlTestCase, roundtrip_json_ints_resize_unix)
{
    SCOPED_TRACE("unix style\n" + c->filelinebuf + ": case");
    _test_roundtrip_json_ints_resize(&d->unix_style);
}

TEST_P(YmlTestCase, roundtrip_json_ints_resize_windows)
{
    SCOPED_TRACE("windows style\n" + c->filelinebuf + ": case");
    _test_roundtrip_json_ints_resize(&d->windows_style);
}


TEST_P(YmlTestCase, roundtrip_json_ints_noresize_unix)
{
    SCOPED_TRACE("unix style\n" + c->filelinebuf + ": case");
    _test_roundtrip_json_ints_noresize(&d->unix_style);
}

TEST_P(YmlTestCase, roundtrip_json_ints_noresize_windows)
{
    SCOPED_TRACE("windows style\n" + c->filelinebuf + ": case");
    _test_roundtrip_json_ints_noresize(&d->windows_style);
}


} // namespace c4
} // namespace yml

#endif // C4_YML_TEST_TEST_GROUP_TEST_GROUP_DEF_HPP_
