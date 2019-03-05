#include <benchmark/benchmark.h>
#include <ryml.hpp>
#include <ryml_std.hpp>
#include <c4/fs/fs.hpp>
#include <vector>
#include <iostream>


namespace bm = benchmark;


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

struct Case
{
    c4::csubstr filename;
    std::vector<char> src;
    std::vector<char> in_place;
    bool is_json;
    ryml::Tree ryml_tree;

    void load(const char* file)
    {
        filename = c4::to_csubstr(file);
        is_json = filename.ends_with(".json");
        src = c4::fs::file_get_contents<std::vector<char>>(file);
        in_place = src;
    }

};
Case c;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void ryml_parse_rw(bm::State& st)
{
    for(auto _ : st)
    {
        ryml::Tree tree = ryml::parse(c.filename, to_substr(c.in_place));
    }
}

void ryml_parse_rw_reuse(bm::State& st)
{
    for(auto _ : st)
    {
        parse(c.filename, to_substr(c.in_place), &c.ryml_tree);
    }
}

void ryml_parse_ro(bm::State& st)
{
    for(auto _ : st)
    {
        ryml::Tree tree = ryml::parse(c.filename, to_csubstr(c.src));
    }
}

void ryml_parse_ro_reuse(bm::State& st)
{
    for(auto _ : st)
    {
        parse(c.filename, to_csubstr(c.src), &c.ryml_tree);
    }
}

BENCHMARK(ryml_parse_ro);
BENCHMARK(ryml_parse_ro_reuse);
BENCHMARK(ryml_parse_rw);
BENCHMARK(ryml_parse_rw_reuse);


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void print(const char* ctx, int argc, char** argv)
{
    std::cout << ctx << ": " << "argc=" << argc << "\n";
    for(int i = 0; i < argc; ++i)
        std::cout << ctx << ": " << "argv[" << i << "]=" << argv[i] << "\n";
}

int main(int argc, char** argv)
{
    print("before", argc, argv);
    bm::Initialize(&argc, argv);
    print("after", argc, argv);
    std::cout << "argc==2: " << argc << "  " << (argc==2) << "\n";
    C4_ASSERT(argc == 2);
    c.load(argv[1]);
    bm::RunSpecifiedBenchmarks();
}
