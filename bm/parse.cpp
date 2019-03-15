#include <benchmark/benchmark.h>
#include <ryml.hpp>
#include <ryml_std.hpp>
#include <c4/fs/fs.hpp>
#include "../test/libyaml.hpp"
#include <vector>
#include <iostream>
#include <yaml-cpp/yaml.h>
#include <rapidjson/document.h>
#include <sajson.h>
#include <json/json.h>
#include <nlohmann/json.hpp>

namespace bm = benchmark;


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

enum : int {
      kClearTree=1,
      kClearTreeArena=2,
      kResetInPlace=4,
      kAll=kClearTree|kClearTreeArena|kResetInPlace,
};

struct BmCase
{
    c4::csubstr            filename;
    std::vector<char>      src;
    std::vector<char>      in_place;
    ryml::Parser           ryml_parser;
    ryml::Tree             ryml_tree;
    bool                   is_json;
    rapidjson::Document    rapidjson_doc;
    ryml::Tree             libyaml_tree;

    void run(int argc, char **argv)
    {
        bm::Initialize(&argc, argv);
        C4_CHECK(argc == 2);
        load(argv[1]);
        bm::RunSpecifiedBenchmarks();
    }

    void load(const char* file)
    {
        filename = c4::to_csubstr(file);
        is_json = filename.ends_with(".json");
        std::cout << "-----------------------------------\n";
        std::cout << "running case: " << filename.basename() << "\n";
        std::cout << "file: " << filename << "\n";
        std::cout << "-----------------------------------\n";
        c4::fs::file_get_contents(file, &src);
        if(src.back() != '\0')
        {
            src.push_back('\0');
        }
        in_place = src;
        C4_ASSERT_MSG(strlen(in_place.data()) == in_place.size()-1,
                      "len=%zu sz=%zu",
                      strlen(in_place.data()), in_place.size());
    }

    void prepare(bm::State &st, int what)
    {
        st.PauseTiming();
        prepare(what);
        st.ResumeTiming();
    }

    void prepare(int what)
    {
        if(what & kClearTree)
        {
            ryml_tree.clear();
        }
        if(what & kClearTreeArena)
        {
            ryml_tree.clear_arena();
        }
        if(what & kResetInPlace)
        {
            C4_ASSERT(in_place.size() == src.size());
            memcpy(in_place.data(), src.data(), src.size());
        }
    }
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


/** this is used by the benchmarks.
 *
 * don't declare the case as static as there's no guarantee the allocator's
 * lifetime starts before and ends after the case's lifetime */
static BmCase * C4_RESTRICT c = nullptr;


int main(int argc, char** argv)
{
    BmCase fixture;
    c = &fixture;
    c->run(argc, argv);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#define ONLY_FOR_JSON \
    if(!c->is_json) { st.SkipWithError("not a json file"); return; }

void rapidjson_ro(bm::State& st)
{
    const char *src = c->src.data();
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        rapidjson::Document doc;
        doc.Parse(src);
    }
    st.SetBytesProcessed(st.iterations() * c->src.size());
}

void rapidjson_rw(bm::State& st)
{
    char *src = c->in_place.data();
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        c->prepare(st, kResetInPlace);
        rapidjson::Document doc;
        doc.ParseInsitu(src);
    }
    st.SetBytesProcessed(st.iterations() * c->src.size());
}

void sajson_ro(bm::State& st)
{
    sajson::string src = {c->src.data(), c->src.size()};
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        sajson::document document = sajson::parse(sajson::dynamic_allocation(), src);
    }
    st.SetBytesProcessed(st.iterations() * c->src.size());
}

void sajson_rw(bm::State& st)
{
    sajson::mutable_string_view src = {c->in_place.size(), c->in_place.data()};
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        c->prepare(st, kResetInPlace);
        sajson::document document = sajson::parse(sajson::dynamic_allocation(), src);
    }
    st.SetBytesProcessed(st.iterations() * c->src.size());
}

void jsoncpp_ro(bm::State& st)
{
    const char *b = &c->src.front(), *e = &c->src.back();
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        Json::Value root;
        Json::Reader reader;
        reader.parse(b, e, root);
    }
    st.SetBytesProcessed(st.iterations() * c->src.size());
}

void nlohmann_json_ro(bm::State& st)
{
    const char* src = c->src.data();
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        auto root = nlohmann::json::parse(src);
    }
    st.SetBytesProcessed(st.iterations() * c->src.size());
}

void yamlcpp_ro(bm::State& st)
{
    const char* src = c->src.data();
    for(auto _ : st)
    {
        YAML::Node node = YAML::Load(src);
    }
    st.SetBytesProcessed(st.iterations() * c->src.size());
}

void libyaml_ro(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(c->src.data());
    for(auto _ : st)
    {
        c4::yml::LibyamlParser p;
        c4::yml::Tree t;
        p.parse(&t, src);
    }
    st.SetBytesProcessed(st.iterations() * c->src.size());
}

void libyaml_ro_reuse(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(c->src.data());
    for(auto _ : st)
    {
        c4::yml::LibyamlParser libyaml_parser;
        c->prepare(st, kClearTree|kClearTreeArena);
        libyaml_parser.parse(&c->libyaml_tree, src);
    }
    st.SetBytesProcessed(st.iterations() * c->src.size());
}

void ryml_ro(bm::State& st)
{
    size_t sz;
    c4::csubstr src = c4::to_csubstr(c->src);
    for(auto _ : st)
    {
        ryml::Tree tree = ryml::parse(c->filename, src);
        sz = tree.size();
    }
    st.SetItemsProcessed(st.iterations() * sz);
    st.SetBytesProcessed(st.iterations() * c->src.size());
}

void ryml_rw(bm::State& st)
{
    size_t sz;
    c4::substr src = c4::to_substr(c->in_place);
    for(auto _ : st)
    {
        c->prepare(st, kResetInPlace);
        ryml::Tree tree = ryml::parse(c->filename, src);
        sz = tree.size();
    }
    st.SetItemsProcessed(st.iterations() * sz);
    st.SetBytesProcessed(st.iterations() * c->src.size());
}

void ryml_ro_reuse(bm::State& st)
{
    size_t sz;
    c4::csubstr src = c4::to_csubstr(c->src);
    for(auto _ : st)
    {
        c->prepare(st, kClearTree|kClearTreeArena);
        c->ryml_parser.parse(c->filename, src, &c->ryml_tree);
        sz = c->ryml_tree.size();
    }
    st.SetItemsProcessed(st.iterations() * sz);
    st.SetBytesProcessed(st.iterations() * c->src.size());
}

void ryml_rw_reuse(bm::State& st)
{
    size_t sz;
    c4::substr src = c4::to_substr(c->in_place);
    for(auto _ : st)
    {
        c->prepare(st, kResetInPlace|kClearTree|kClearTreeArena);
        c->ryml_parser.parse(c->filename, c4::to_substr(c->in_place), &c->ryml_tree);
        sz = c->ryml_tree.size();
    }
    st.SetItemsProcessed(st.iterations() * sz);
    st.SetBytesProcessed(st.iterations() * c->src.size());
}

BENCHMARK(rapidjson_ro);
BENCHMARK(rapidjson_rw);
BENCHMARK(sajson_rw);
BENCHMARK(sajson_ro);
BENCHMARK(jsoncpp_ro);
BENCHMARK(nlohmann_json_ro);
BENCHMARK(yamlcpp_ro);
BENCHMARK(libyaml_ro);
BENCHMARK(libyaml_ro_reuse);
BENCHMARK(ryml_ro);
BENCHMARK(ryml_rw);
BENCHMARK(ryml_ro_reuse);
BENCHMARK(ryml_rw_reuse);
