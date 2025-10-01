#ifndef C4_YML_BM_COMMON_HPP_
#define C4_YML_BM_COMMON_HPP_
#include <ryml.hpp>
#include <ryml_std.hpp>
#include <c4/fs/fs.hpp>
#include "c4/yml/parse.hpp"
#include "c4/yml/extra/event_handler_ints.hpp"

#include <vector>
#include <iostream>

#include <benchmark/benchmark.h>

// warning suppressions for thirdparty code
#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable : 4100) // sajson.h(1313,41): warning C4100: 'input_document_size_in_bytes': unreferenced formal parameter
#   pragma warning(disable : 4127) // conditional expression is constant
#   pragma warning(disable : 4200) // sajson.h(209,28): warning C4200: nonstandard extension used: zero-sized array in struct/union
#   pragma warning(disable : 4242) // sajson.h(2295,1): warning C4242: '=': conversion from 'unsigned int' to 'char', possible loss of data
#   pragma warning(disable : 4244) // sajson.h(2295,26): warning C4244: '=': conversion from 'unsigned int' to 'char', possible loss of data
#   pragma warning(disable : 4389) // '==': signed/unsigned mismatch
#   pragma warning(disable : 4996) // warning C4996: 'Json::Reader': Use CharReader and CharReaderBuilder instead.
#   pragma warning(disable : 5054) // rapidjson: operator '|': deprecated between enumerations of different types
#elif defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wdeprecated-declarations"
#   pragma clang diagnostic ignored "-Wunused-parameter"
#   pragma clang diagnostic ignored "-Wc99-extensions"
#   pragma clang diagnostic ignored "-Wfloat-equal"
#   pragma clang diagnostic ignored "-Wshadow"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wconversion"
#   pragma clang diagnostic ignored "-Wold-style-cast"
#   if __clang_major__ >= 8
#       pragma clang diagnostic ignored "-Wimplicit-int-conversion"
#   endif
#elif defined(__GNUC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#   pragma GCC diagnostic ignored "-Wshadow"
#   pragma GCC diagnostic ignored "-Wunused-parameter"
#   pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#   pragma GCC diagnostic ignored "-Wfloat-equal"
#   pragma GCC diagnostic ignored "-Wpedantic"
#   pragma GCC diagnostic ignored "-Wuseless-cast"
#   pragma GCC diagnostic ignored "-Wconversion"
#   pragma GCC diagnostic ignored "-Wsign-conversion"
#   pragma GCC diagnostic ignored "-Wold-style-cast"
#   if __GNUC__ >= 8
#       pragma GCC diagnostic ignored "-Wclass-memaccess" // rapidjson/document.h:1952:24
#   endif
#endif
#include "./libyaml.hpp"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <sajson.h>
#include <json/json.h>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#ifdef RYML_HAVE_LIBFYAML
#include <libfyaml.h>
#endif
#if defined(_MSC_VER)
#   pragma warning(pop)
#elif defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__GNUC__)
#   pragma GCC diagnostic pop
#endif


namespace bm = benchmark;


// now for our code
#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable : 4996) // warning C4996: 'Json::Reader': Use CharReader and CharReaderBuilder instead.
#elif defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wdeprecated-declarations"
#   pragma clang diagnostic ignored "-Wunused-variable"
#   pragma clang diagnostic ignored "-Wsign-conversion"
#   pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#   pragma GCC diagnostic ignored "-Wunused-variable"
#   pragma GCC diagnostic ignored "-Wsign-conversion"
#   pragma GCC diagnostic ignored "-Wold-style-cast"
#endif


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

using IntHandler = ryml::extra::EventHandlerInts;
using IntParser = ryml::ParseEngine<IntHandler>;
struct IntData
{
    std::vector<int> events;
    std::vector<char> arena;
    void resize(ryml::extra::EventHandlerInts const& handler)
    {
        resize((size_t)handler.required_size_events(),
               handler.required_size_arena());
    }
    void resize(ryml::substr src)
    {
        resize((size_t)ryml::extra::estimate_events_ints_size(src),
               src.len);
    }
    void resize(size_t evt_sz, size_t arn_sz)
    {
        events.resize(evt_sz);
        events.resize(arn_sz);
    }
};
struct IntObjects
{
    IntObjects(ryml::ParserOptions opts={})
        : handler()
        , parser(&handler, opts)
        , data()
    {
    }
    IntHandler handler;
    IntParser parser;
    IntData data;
    bool again()
    {
        if(!handler.fits_buffers())
        {
            data.resize(handler);
            return true;
        }
        return false;
    }
};
inline void parse_yaml_inplace(ryml::csubstr filename, ryml::substr src, IntParser &parser, IntData *dst)
{
    parser.m_evt_handler->reset(src,
                                ryml::to_substr(dst->arena),
                                dst->events.data(),
                                (int)dst->events.size());
    parser.parse_in_place_ev(filename, src);
}
inline void parse_json_inplace(ryml::csubstr filename, ryml::substr src, IntParser &parser, IntData *dst)
{
    parser.m_evt_handler->reset(src,
                                ryml::to_substr(dst->arena),
                                dst->events.data(),
                                (int)dst->events.size());
    parser.parse_json_in_place_ev(filename, src);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

enum : int {
      kClearTree=1,
      kClearTreeArena=2,
      kResetInPlace=4,
      kReserveTree=8,
      kReserveInts=8,
      kAll=kClearTree|kClearTreeArena|kResetInPlace|kReserveTree,
};

struct BmCase
{
    std::string            bm_name;
    c4::csubstr            filename;
    std::vector<char>      src;
    std::vector<char>      in_place;
    ryml::EventHandlerTree ryml_evt_handler;
    ryml::EventHandlerTree ryml_evt_handler_nofilter;
    ryml::Parser           ryml_parser{&ryml_evt_handler};
    ryml::Parser           ryml_parser_nofilter{&ryml_evt_handler_nofilter, ryml::ParserOptions().scalar_filtering(false)};
    ryml::Tree             ryml_tree;
    bool                   is_json;
    rapidjson::Document    rapidjson_doc;
    ryml::Tree             libyaml_tree;

    IntObjects int_obj;
    IntObjects int_obj_nofilter{ryml::ParserOptions().scalar_filtering(false)};

    void run(std::string name, int argc, char **argv)
    {
        bm_name = name;
        bm::Initialize(&argc, argv);
        if(argc < 2)
        {
            std::cout << R"(
USAGE: bm <case.yml>
)";
            std::exit(1);
        }
        load(argv[1]);
        bm::RunSpecifiedBenchmarks();
    }

    void load(const char* file)
    {
        filename = c4::to_csubstr(file);
        is_json = filename.ends_with(".json");
        std::cout << "-----------------------------------\n";
        std::cout << "running case: " << bm_name << "/" << filename.basename() << "\n";
        std::cout << "file: " << filename << "\n";
        std::cout << "-----------------------------------\n";
        c4::fs::file_get_contents(file, &src);
        if(src.back() != '\0')
        {
            src.push_back('\0');
        }
        in_place = src;
        _RYML_ASSERT_BASIC_MSG(strlen(in_place.data()) == in_place.size()-1,
                               "len=%zu sz=%zu",
                               strlen(in_place.data()), in_place.size());
    }

    void prepare(bm::State &st, int what, ryml::id_type capacity=0)
    {
        st.PauseTiming();
        prepare(what, capacity);
        st.ResumeTiming();
    }

    void prepare(int what, ryml::id_type capacity=0)
    {
        if(what & kClearTree)
        {
            ryml_tree.clear();
        }
        if(what & kClearTreeArena)
        {
            ryml_tree.clear_arena();
        }
        if(what & kReserveTree)
        {
            _RYML_CHECK_BASIC(capacity > 0);
            ryml_tree.reserve(capacity);
        }
        if(what & kReserveTree)
        {
            _RYML_CHECK_BASIC(capacity > 0);
            ryml_tree.reserve(capacity);
        }
        if(what & kResetInPlace)
        {
            _RYML_ASSERT_BASIC(in_place.size() == src.size());
            memcpy(in_place.data(), src.data(), src.size());
        }
    }

    void report(bm::State &st) const
    {
        // number of times the source was parsed
        st.SetItemsProcessed(st.iterations());
        // number of bytes parsed in the source
        st.SetBytesProcessed(st.iterations() * src.size());
    }

    bool skip_libyaml_if_needed(bm::State &st) const
    {
        if(filename.basename() == "trusty.yml")
        {
            st.SkipWithError("trusty.yml has anchors/references, which do not parse successfully in our parser wrapper for libyaml");
            return true;
        }
        return false;
    }

    bool skip_libfyaml_if_needed(bm::State &st) const
    {
        if(filename.basename() == "typeIDs.yaml")
        {
            st.SkipWithError("libfyaml fails to parse this file");
            return true;
        }
        return false;
    }
};

#endif /* C4_YML_BM_COMMON_HPP_ */
