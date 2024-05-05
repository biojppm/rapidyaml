#ifndef _C4_YML_TEST_TEST_GROUP_TEST_GROUP_DEF_HPP_
#define _C4_YML_TEST_TEST_GROUP_TEST_GROUP_DEF_HPP_

namespace c4 {
namespace yml {

int YmlTestCaseDefsWereIncluded() { return 42; }

//-----------------------------------------------------------------------------

TEST_P(YmlTestCase, parse_unix)
{
    SCOPED_TRACE("unix style\n" + c->filelinebuf + ": case");
    _test_parse_using_ryml(&d->unix_style);
}

TEST_P(YmlTestCase, parse_windows)
{
    SCOPED_TRACE("windows style\n" + c->filelinebuf + ": case");
    _test_parse_using_ryml(&d->windows_style);
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, emit_yml_unix_stdout)
{
    SCOPED_TRACE("unix style\n" + c->filelinebuf + ": case");
    _test_emit_yml_stdout(&d->unix_style);
}
TEST_P(YmlTestCase, emit_json_unix_stdout)
{
    SCOPED_TRACE("unix style json\n" + c->filelinebuf + ": case");
    _test_emit_json_stdout(&d->unix_style_json);
}

TEST_P(YmlTestCase, emit_yml_windows_stdout)
{
    SCOPED_TRACE("windows style\n" + c->filelinebuf + ": case");
    _test_emit_yml_stdout(&d->windows_style);
}
TEST_P(YmlTestCase, emit_json_windows_stdout)
{
    SCOPED_TRACE("windows style json\n" + c->filelinebuf + ": case");
    _test_emit_json_stdout(&d->windows_style_json);
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, emit_yml_unix_cout)
{
    SCOPED_TRACE("unix style\n" + c->filelinebuf + ": case");
    _test_emit_yml_cout(&d->unix_style);
}
TEST_P(YmlTestCase, emit_json_unix_cout)
{
    SCOPED_TRACE("unix style json\n" + c->filelinebuf + ": case");
    _test_emit_json_cout(&d->unix_style_json);
}

TEST_P(YmlTestCase, emit_yml_windows_cout)
{
    SCOPED_TRACE("windows style\n" + c->filelinebuf + ": case");
    _test_emit_yml_cout(&d->windows_style);
}
TEST_P(YmlTestCase, emit_json_windows_cout)
{
    SCOPED_TRACE("windows style json\n" + c->filelinebuf + ": case");
    _test_emit_json_cout(&d->windows_style_json);
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, emit_yml_unix_stringstream)
{
    SCOPED_TRACE("unix style\n" + c->filelinebuf + ": case");
    _test_emit_yml_stringstream(&d->unix_style);
}
TEST_P(YmlTestCase, emit_json_unix_stringstream)
{
    SCOPED_TRACE("unix style json\n" + c->filelinebuf + ": case");
    _test_emit_json_stringstream(&d->unix_style_json);
}

TEST_P(YmlTestCase, emit_yml_windows_stringstream)
{
    SCOPED_TRACE("windows style\n" + c->filelinebuf + ": case");
    _test_emit_yml_stringstream(&d->windows_style);
}
TEST_P(YmlTestCase, emit_json_windows_stringstream)
{
    SCOPED_TRACE("windows style json\n" + c->filelinebuf + ": case");
    _test_emit_json_stringstream(&d->windows_style_json);
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, emit_yml_unix_ofstream)
{
    SCOPED_TRACE("unix style\n" + c->filelinebuf + ": case");
    _test_emit_yml_ofstream(&d->unix_style);
}
TEST_P(YmlTestCase, emit_json_unix_ofstream)
{
    SCOPED_TRACE("unix style json\n" + c->filelinebuf + ": case");
    _test_emit_json_ofstream(&d->unix_style_json);
}

TEST_P(YmlTestCase, emit_yml_windows_ofstream)
{
    SCOPED_TRACE("windows style\n" + c->filelinebuf + ": case");
    _test_emit_yml_ofstream(&d->windows_style);
}
TEST_P(YmlTestCase, emit_json_windows_ofstream)
{
    SCOPED_TRACE("windows style json\n" + c->filelinebuf + ": case");
    _test_emit_json_ofstream(&d->windows_style_json);
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, emit_yml_unix_string)
{
    SCOPED_TRACE("unix style\n" + c->filelinebuf + ": case");
    _test_emit_yml_string(&d->unix_style);
}
TEST_P(YmlTestCase, emit_json_unix_string)
{
    SCOPED_TRACE("unix style json\n" + c->filelinebuf + ": case");
    _test_emit_json_string(&d->unix_style_json);
}

TEST_P(YmlTestCase, emit_yml_windows_string)
{
    SCOPED_TRACE("windows style\n" + c->filelinebuf + ": case");
    _test_emit_yml_string(&d->windows_style);
}
TEST_P(YmlTestCase, emit_json_windows_string)
{
    SCOPED_TRACE("windows style json\n" + c->filelinebuf + ": case");
    _test_emit_json_string(&d->windows_style_json);
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, unix_emitrs)
{
    SCOPED_TRACE("unix style\n" + c->filelinebuf + ": case");
    _test_emitrs(&d->unix_style);
}
TEST_P(YmlTestCase, unix_emitrs_json)
{
    SCOPED_TRACE("unix style json\n" + c->filelinebuf + ": case");
    _test_emitrs_json(&d->unix_style_json);
}

TEST_P(YmlTestCase, windows_emitrs)
{
    SCOPED_TRACE("windows style\n" + c->filelinebuf + ": case");
    _test_emitrs(&d->windows_style);
}
TEST_P(YmlTestCase, windows_emitrs_json)
{
    SCOPED_TRACE("windows style json\n" + c->filelinebuf + ": case");
    _test_emitrs_json(&d->windows_style_json);
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, unix_emitrs_cfile)
{
    SCOPED_TRACE("unix style\n" + c->filelinebuf + ": case");
    _test_emitrs_cfile(&d->unix_style);
}
TEST_P(YmlTestCase, unix_emitrs_json_cfile)
{
    SCOPED_TRACE("unix style json\n" + c->filelinebuf + ": case");
    _test_emitrs_json_cfile(&d->unix_style_json);
}

TEST_P(YmlTestCase, windows_emitrs_cfile)
{
    SCOPED_TRACE("windows style\n" + c->filelinebuf + ": case");
    _test_emitrs_cfile(&d->windows_style);
}
TEST_P(YmlTestCase, windows_emitrs_json_cfile)
{
    SCOPED_TRACE("windows style json\n" + c->filelinebuf + ": case");
    _test_emitrs_json_cfile(&d->windows_style_json);
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, complete_unix_round_trip)
{
    SCOPED_TRACE("unix style:\n" + c->filelinebuf + ": case");
    _test_complete_round_trip(&d->unix_style);
}
TEST_P(YmlTestCase, complete_unix_round_trip_json)
{
    SCOPED_TRACE("unix style json\n" + c->filelinebuf + ": case");
    _test_complete_round_trip_json(&d->unix_style_json);
}

TEST_P(YmlTestCase, complete_windows_round_trip)
{
    SCOPED_TRACE("windows style\n" + c->filelinebuf + ": case");
    _test_complete_round_trip(&d->windows_style);
}
TEST_P(YmlTestCase, complete_windows_round_trip_json)
{
    SCOPED_TRACE("windows style json\n" + c->filelinebuf + ": case");
    _test_complete_round_trip_json(&d->windows_style_json);
}

//-----------------------------------------------------------------------------
TEST_P(YmlTestCase, unix_recreate_from_ref)
{
    SCOPED_TRACE("unix style\n" + c->filelinebuf + ": case");
    _test_recreate_from_ref(&d->unix_style);
}

TEST_P(YmlTestCase, windows_recreate_from_ref)
{
    SCOPED_TRACE("windows style\n" + c->filelinebuf + ": case");
    _test_recreate_from_ref(&d->windows_style);
}

} // namespace c4
} // namespace yml

#endif // _C4_YML_TEST_TEST_GROUP_TEST_GROUP_DEF_HPP_
