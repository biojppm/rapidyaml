#include <c4/substr.hpp>
#include <c4/std/string.hpp>
#include <c4/fs/fs.hpp>
#include <c4/log/log.hpp>

#include <c4/yml/tree.hpp>
#include <c4/yml/parse.hpp>
#include <c4/yml/emit.hpp>

#include <gtest/gtest.h>


enum : size_t { npos = c4::csubstr::npos };

struct SuiteCase
{
    c4::csubstr filename;
    std::string file_contents;

    c4::csubstr desc;
    c4::csubstr from;
    c4::csubstr tags;

    c4::csubstr in_yaml;
    std::string in_yaml_filtered;

    c4::csubstr in_json;
    c4::csubstr out_yaml;
    c4::csubstr events;

    std::string in_yaml_in_situ;
    c4::yml::Tree in_tree_in_situ;
    c4::yml::Tree in_tree;

    bool load(const char* filename_)
    {
        filename = c4::to_csubstr(filename_);

        // read the file
        c4::fs::file_get_contents(filename_, &file_contents);
        c4::csubstr contents = to_csubstr(file_contents);

        // now parse the file
        c4::csubstr ws = " \t\r\n";
        size_t b, e;

        // desc
        C4_CHECK(contents.begins_with("=== "));
        e = contents.find("--- from: ", 4);
        C4_CHECK(e != npos);
        desc = contents.range(4, e).trimr(ws);

        // from
        b = e + 4;
        e = contents.find("--- tags: ", b);
        C4_CHECK(e != npos);
        from = contents.range(b, e);
        C4_CHECK(from.begins_with("from: "));
        C4_CHECK(from.size() >= 6);
        from = from.sub(6).trimr(ws);

        // tags
        b = e + 4;
        e = contents.find("--- in-yaml", b);
        C4_CHECK(e != npos);
        tags = contents.range(b, e);
        C4_CHECK(tags.begins_with("tags: "));
        C4_CHECK(tags.size() >= 6);
        tags = tags.sub(6).trimr(ws);
        if(tags.find("error") != npos) return false;

        // in_yaml
        b = e + 4;
        b = 1 + contents.find('\n', b);
        e = contents.find("--- in-json");
        in_yaml = contents.range(b, e).trimr(ws);

        // in_json
        b = e + 4;
        b = 1 + contents.find('\n', b);
        e = contents.find("--- test-event");
        in_json = contents.range(b, e).trimr(ws);

        // events
        b = e + 4;
        b = 1 + contents.find('\n', b);
        events = contents.range(b).trimr(ws);

        // filter
        if(tags.find("whitespace") != npos)
        {
            in_yaml_filtered.clear();
            b = 0;
            do {
                e = in_yaml.find("<SPC>", b);
                if(e == npos)
                {
                    in_yaml_filtered.append(&in_yaml[b], in_yaml.end());
                    break;
                }
                in_yaml_filtered.append(&in_yaml[b], &in_yaml[e]);
                in_yaml_filtered.append(" ");
                b = e + 5;
            } while(b != npos);
            in_yaml = to_csubstr(in_yaml_filtered);
        }

        return true;
    }

    void print() const
    {
        c4::printvar("% desc   : " , desc   , " %\n",
                     "% from   : " , from   , " %\n",
                     "% tags   : " , tags   , " %\n",
                     "% in_yaml:\n", in_yaml, " %\n",
                     "% in_json:\n", in_json, " %\n",
                     "% events :\n", events , " %\n");
    }

    void parse()
    {
        c4::yml::parse(filename, in_yaml, &in_tree);
    }

    void parse_in_situ()
    {
        in_yaml_in_situ.assign(in_yaml.begin(), in_yaml.end());
        c4::yml::parse(filename, to_substr(in_yaml_in_situ), &in_tree_in_situ);
    }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

SuiteCase g_suite_case;


int main(int argc, char* argv[])
{
    // check input
    C4_CHECK(argc == 2);
    if(argc < 2) return 1;
    C4_CHECK(c4::fs::path_exists(argv[1]));

    if( ! g_suite_case.load(argv[1]))
    {
        return 0;
    }

    c4::print(g_suite_case.file_contents);
    g_suite_case.print();

    ::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


//-----------------------------------------------------------------------------

TEST(parse_yaml, in_situ)
{
    g_suite_case.parse_in_situ();
}


TEST(parse_yaml, out_of_situ)
{
    g_suite_case.parse();
}
