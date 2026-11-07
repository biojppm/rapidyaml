#include "./bm_common.hpp"
#if defined(C4_WIN)
#include <c4/windows.hpp>
#elif defined(C4_POSIX) || defined(C4_MACOS) || defined(C4_IOS) || defined(C4_ANDROID)
#include <unistd.h>
#endif


/** this is used by the benchmarks.
 *
 * @note We cannot declare the case as value-static as there is no guarantee
 * that the allocator's lifetime starts before and ends after the case's
 * lifetime. So use a pointer to control the lifetime. */
static BmCase * C4_RESTRICT s_bm_case = nullptr;


int main(int argc, char** argv)
{
    bm::MaybeReenterWithoutASLR(argc, argv);
    BmCase fixture;
    s_bm_case = &fixture;
    s_bm_case->run("PARSE", argc, argv);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#define ONLY_FOR_JSON \
    if(!s_bm_case->is_json) { st.SkipWithError("not a json file"); return; }


ryml::id_type estimate_capacity(ryml::csubstr src)
{
    return (3 * ryml::estimate_tree_capacity(src)) >> 1;
}


/** the _reserve benchmarks below reserve with this callback, so that
 * the reserved pages are resident before the timed parse starts: one
 * byte is written in each page. */
void make_resident_one_byte_per_page(void *mem, size_t len, void * /*user_data*/)
{
    size_t page = 0;
    #if defined(C4_WIN)
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    page = static_cast<size_t>(info.dwPageSize);
    #elif defined(C4_POSIX) || defined(C4_MACOS) || defined(C4_IOS) || defined(C4_ANDROID)
    const long sz = sysconf(_SC_PAGESIZE);
    page = sz > 0 ? static_cast<size_t>(sz) : 0u;
    #endif
    if(!page) // no page size: do nothing
        return;
    char *bytes = static_cast<char*>(mem);
    for(size_t i = 0; i < len; i += page)
        bytes[i] = 0;
}

static const ryml::Callbacks s_resident_callbacks = ryml::Callbacks{}.set_make_resident(&make_resident_one_byte_per_page);


void bm_rapidjson_arena(bm::State& st)
{
    const char *src = s_bm_case->src.data();
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        rapidjson::Document doc;
        doc.Parse(src);
    }
    s_bm_case->report(st);
}

void bm_rapidjson_inplace(bm::State& st)
{
    char *src = s_bm_case->in_place.data();
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        s_bm_case->prepare(st, kResetInPlace);
        rapidjson::Document doc;
        doc.ParseInsitu(src);
    }
    s_bm_case->report(st);
}

void bm_sajson_arena(bm::State& st)
{
    sajson::string src = {s_bm_case->src.data(), s_bm_case->src.size()};
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        sajson::document document = sajson::parse(sajson::dynamic_allocation(), src);
    }
    s_bm_case->report(st);
}

void bm_sajson_inplace(bm::State& st)
{
    sajson::mutable_string_view src = {s_bm_case->in_place.size(), s_bm_case->in_place.data()};
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        s_bm_case->prepare(st, kResetInPlace);
        sajson::document document = sajson::parse(sajson::dynamic_allocation(), src);
    }
    s_bm_case->report(st);
}

void bm_jsoncpp_arena(bm::State& st)
{
    const char *b = &s_bm_case->src.front(), *e = &s_bm_case->src.back();
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        Json::Value root;
        Json::Reader reader;
        reader.parse(b, e, root);
    }
    s_bm_case->report(st);
}

void bm_nlohmann_arena(bm::State& st)
{
    const char* src = s_bm_case->src.data();
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        auto root = nlohmann::json::parse(src);
    }
    s_bm_case->report(st);
}

void bm_yamlcpp_arena(bm::State& st)
{
    const char* src = s_bm_case->src.data();
    for(auto _ : st)
    {
        YAML::Node node = YAML::Load(src);
    }
    s_bm_case->report(st);
}

void bm_libyaml_arena(bm::State& st)
{
    if(s_bm_case->skip_libyaml_if_needed(st))
        return;
    c4::csubstr src = c4::to_csubstr(s_bm_case->src.data());
    for(auto _ : st)
    {
        c4::yml::LibyamlParser p;
        c4::yml::Tree t;
        p.parse(&t, src);
    }
    s_bm_case->report(st);
}

void bm_libyaml_arena_reuse(bm::State& st)
{
    if(s_bm_case->skip_libyaml_if_needed(st))
        return;
    c4::csubstr src = c4::to_csubstr(s_bm_case->src.data());
    for(auto _ : st)
    {
        c4::yml::LibyamlParser libyaml_parser;
        s_bm_case->prepare(st, kClearTree|kClearTreeArena);
        libyaml_parser.parse(&s_bm_case->libyaml_tree, src);
    }
    s_bm_case->report(st);
}

#ifdef RYML_HAVE_LIBFYAML
void bm_libfyaml_arena(bm::State& st)
{
    if(s_bm_case->skip_libfyaml_if_needed(st))
        return;
    c4::csubstr src = c4::to_csubstr(s_bm_case->src.data());
    for(auto _ : st)
    {
        struct fy_document *fyd = fy_document_build_from_string(nullptr, src.str, src.len);
        fy_document_destroy(fyd);
    }
    s_bm_case->report(st);
}
#endif


//-----------------------------------------------------------------------------

void bm_ryml_yaml_arena(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(s_bm_case->src).trimr('\0');
    for(auto _ : st)
    {
        ryml::Tree tree = ryml::parse_in_arena(s_bm_case->filename, src);
    }
    s_bm_case->report(st);
}

void bm_ryml_yaml_arena_reserve(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(s_bm_case->src).trimr('\0');
    ryml::id_type cap = estimate_capacity(src);
    for(auto _ : st)
    {
        st.PauseTiming();
        ryml::Tree tree(s_resident_callbacks);
        tree.reserve(cap);
        st.ResumeTiming();
        ryml::parse_in_arena(s_bm_case->filename, src, &tree);
    }
    s_bm_case->report(st);
}

void bm_ryml_yaml_arena_nofilter(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(s_bm_case->src).trimr('\0');
    for(auto _ : st)
    {
        st.PauseTiming();
        ryml::EventHandlerTree evth;
        ryml::Parser parser(&evth, ryml::ParserOptions().scalar_filtering(false));
        st.ResumeTiming();
        ryml::Tree tree = ryml::parse_in_arena(&parser, s_bm_case->filename, src);
    }
    s_bm_case->report(st);
}

void bm_ryml_yaml_arena_nofilter_reserve(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(s_bm_case->src).trimr('\0');
    ryml::id_type cap = estimate_capacity(src);
    for(auto _ : st)
    {
        st.PauseTiming();
        ryml::EventHandlerTree evth;
        ryml::Parser parser(&evth, ryml::ParserOptions().scalar_filtering(false));
        ryml::Tree tree(s_resident_callbacks);
        tree.reserve(cap);
        st.ResumeTiming();
        ryml::parse_in_arena(&parser, s_bm_case->filename, src, &tree);
    }
    s_bm_case->report(st);
}


//-----------------------------------------------------------------------------

void bm_ryml_json_arena(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(s_bm_case->src).trimr('\0');
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        ryml::Tree tree = ryml::parse_json_in_arena(s_bm_case->filename, src);
    }
    s_bm_case->report(st);
}

void bm_ryml_json_arena_reserve(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(s_bm_case->src).trimr('\0');
    ryml::id_type cap = estimate_capacity(src);
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        st.PauseTiming();
        ryml::Tree tree(s_resident_callbacks);
        tree.reserve(cap);
        st.ResumeTiming();
        ryml::parse_json_in_arena(s_bm_case->filename, src, &tree);
    }
    s_bm_case->report(st);
}

void bm_ryml_json_arena_nofilter(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(s_bm_case->src).trimr('\0');
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        st.PauseTiming();
        ryml::EventHandlerTree evth;
        ryml::Parser parser(&evth, ryml::ParserOptions().scalar_filtering(false));
        st.ResumeTiming();
        ryml::Tree tree = ryml::parse_json_in_arena(&parser, s_bm_case->filename, src);
    }
    s_bm_case->report(st);
}

void bm_ryml_json_arena_nofilter_reserve(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(s_bm_case->src).trimr('\0');
    ryml::id_type cap = estimate_capacity(src);
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        st.PauseTiming();
        ryml::EventHandlerTree evth;
        ryml::Parser parser(&evth, ryml::ParserOptions().scalar_filtering(false));
        ryml::Tree tree(s_resident_callbacks);
        tree.reserve(cap);
        st.ResumeTiming();
        ryml::parse_json_in_arena(&parser, s_bm_case->filename, src, &tree);
    }
    s_bm_case->report(st);
}


//-----------------------------------------------------------------------------

void bm_rymlints_estimate(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(s_bm_case->in_place).trimr('\0');
    for(auto _ : st)
    {
        auto sz = c4::yml::extra::estimate_events_ints_size(src);
        bm::DoNotOptimize(sz);
    }
    s_bm_case->report(st);
}


void bm_rymlints_yaml_inplace(bm::State& st)
{
    c4::substr src = c4::to_substr(s_bm_case->in_place).trimr('\0');
    for(auto _ : st)
    {
        IntObjects obj;
        s_bm_case->prepare(st, kResetInPlace);
        parse_yaml_inplace(s_bm_case->filename, src, obj.parser, &obj.data);
        if(obj.again())
        {
            s_bm_case->prepare(st, kResetInPlace);
            parse_yaml_inplace(s_bm_case->filename, src, obj.parser, &obj.data);
        }
        bm::DoNotOptimize(obj);
    }
    s_bm_case->report(st);
}

void bm_rymlints_yaml_inplace_reserve(bm::State& st)
{
    c4::substr src = c4::to_substr(s_bm_case->in_place).trimr('\0');
    for(auto _ : st)
    {
        st.PauseTiming();
        s_bm_case->prepare(kResetInPlace);
        IntObjects obj;
        obj.data.resize(src);
        st.ResumeTiming();
        parse_yaml_inplace(s_bm_case->filename, src, obj.parser, &obj.data);
        bm::DoNotOptimize(obj);
    }
    s_bm_case->report(st);
}

void bm_rymlints_yaml_inplace_nofilter(bm::State& st)
{
    c4::substr src = c4::to_substr(s_bm_case->in_place).trimr('\0');
    for(auto _ : st)
    {
        IntObjects obj(ryml::ParserOptions().scalar_filtering(false));
        s_bm_case->prepare(st, kResetInPlace);
        parse_yaml_inplace(s_bm_case->filename, src, obj.parser, &obj.data);
        if(obj.again())
        {
            s_bm_case->prepare(st, kResetInPlace);
            parse_yaml_inplace(s_bm_case->filename, src, obj.parser, &obj.data);
        }
        bm::DoNotOptimize(obj);
    }
    s_bm_case->report(st);
}

void bm_rymlints_yaml_inplace_nofilter_reserve(bm::State& st)
{
    c4::substr src = c4::to_substr(s_bm_case->in_place).trimr('\0');
    for(auto _ : st)
    {
        st.PauseTiming();
        s_bm_case->prepare(kResetInPlace);
        IntObjects obj(ryml::ParserOptions().scalar_filtering(false));
        obj.data.resize(src);
        st.ResumeTiming();
        parse_yaml_inplace(s_bm_case->filename, src, obj.parser, &obj.data);
        bm::DoNotOptimize(obj);
    }
    s_bm_case->report(st);
}


//-----------------------------------------------------------------------------

void bm_rymlints_json_inplace(bm::State& st)
{
    c4::substr src = c4::to_substr(s_bm_case->in_place).trimr('\0');
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        IntObjects obj;
        s_bm_case->prepare(st, kResetInPlace);
        parse_json_inplace(s_bm_case->filename, src, obj.parser, &obj.data);
        if(obj.again())
        {
            s_bm_case->prepare(st, kResetInPlace);
            parse_json_inplace(s_bm_case->filename, src, obj.parser, &obj.data);
        }
        bm::DoNotOptimize(obj);
    }
    s_bm_case->report(st);
}

void bm_rymlints_json_inplace_reserve(bm::State& st)
{
    c4::substr src = c4::to_substr(s_bm_case->in_place).trimr('\0');
    size_t sz = (size_t)ryml::extra::estimate_events_ints_size(src);
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        st.PauseTiming();
        s_bm_case->prepare(kResetInPlace);
        IntObjects obj;
        obj.data.resize(sz, src.len);
        st.ResumeTiming();
        parse_json_inplace(s_bm_case->filename, src, obj.parser, &obj.data);
        bm::DoNotOptimize(obj);
    }
    s_bm_case->report(st);
}

void bm_rymlints_json_inplace_nofilter(bm::State& st)
{
    c4::substr src = c4::to_substr(s_bm_case->in_place).trimr('\0');
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        IntObjects obj(ryml::ParserOptions().scalar_filtering(false));
        s_bm_case->prepare(st, kResetInPlace);
        parse_json_inplace(s_bm_case->filename, src, obj.parser, &obj.data);
        if(obj.again())
        {
            s_bm_case->prepare(st, kResetInPlace);
            parse_json_inplace(s_bm_case->filename, src, obj.parser, &obj.data);
        }
        bm::DoNotOptimize(obj);
    }
    s_bm_case->report(st);
}

void bm_rymlints_json_inplace_nofilter_reserve(bm::State& st)
{
    c4::substr src = c4::to_substr(s_bm_case->in_place).trimr('\0');
    size_t sz = (size_t)ryml::extra::estimate_events_ints_size(src);
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        st.PauseTiming();
        s_bm_case->prepare(kResetInPlace);
        IntObjects obj(ryml::ParserOptions().scalar_filtering(false));
        obj.data.resize(sz, src.len);
        st.ResumeTiming();
        parse_json_inplace(s_bm_case->filename, src, obj.parser, &obj.data);
        bm::DoNotOptimize(obj);
    }
    s_bm_case->report(st);
}


//-----------------------------------------------------------------------------

void bm_ryml_yaml_inplace(bm::State& st)
{
    c4::substr src = c4::to_substr(s_bm_case->in_place).trimr('\0');
    for(auto _ : st)
    {
        s_bm_case->prepare(st, kResetInPlace);
        ryml::Tree tree = ryml::parse_in_place(s_bm_case->filename, src);
    }
    s_bm_case->report(st);
}

void bm_ryml_yaml_inplace_reserve(bm::State& st)
{
    c4::substr src = c4::to_substr(s_bm_case->in_place).trimr('\0');
    ryml::id_type cap = estimate_capacity(src);
    for(auto _ : st)
    {
        st.PauseTiming();
        s_bm_case->prepare(kResetInPlace);
        ryml::Tree tree(s_resident_callbacks);
        tree.reserve(cap);
        st.ResumeTiming();
        ryml::parse_in_place(s_bm_case->filename, src, &tree);
    }
    s_bm_case->report(st);
}

void bm_ryml_yaml_inplace_nofilter(bm::State& st)
{
    c4::substr src = c4::to_substr(s_bm_case->in_place).trimr('\0');
    for(auto _ : st)
    {
        st.PauseTiming();
        s_bm_case->prepare(kResetInPlace);
        ryml::EventHandlerTree evth;
        ryml::Parser parser(&evth, ryml::ParserOptions().scalar_filtering(false));
        st.ResumeTiming();
        ryml::Tree tree = ryml::parse_in_place(&parser, s_bm_case->filename, src);
    }
    s_bm_case->report(st);
}

void bm_ryml_yaml_inplace_nofilter_reserve(bm::State& st)
{
    c4::substr src = c4::to_substr(s_bm_case->in_place).trimr('\0');
    ryml::id_type cap = estimate_capacity(src);
    for(auto _ : st)
    {
        st.PauseTiming();
        s_bm_case->prepare(kResetInPlace);
        ryml::EventHandlerTree evth;
        ryml::Parser parser(&evth, ryml::ParserOptions().scalar_filtering(false));
        ryml::Tree tree(s_resident_callbacks);
        tree.reserve(cap);
        st.ResumeTiming();
        ryml::parse_in_place(&parser, s_bm_case->filename, src, &tree);
    }
    s_bm_case->report(st);
}


//-----------------------------------------------------------------------------

void bm_ryml_json_inplace(bm::State& st)
{
    c4::substr src = c4::to_substr(s_bm_case->in_place).trimr('\0');
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        st.PauseTiming();
        s_bm_case->prepare(kResetInPlace);
        st.ResumeTiming();
        ryml::Tree tree = ryml::parse_json_in_place(s_bm_case->filename, src);
    }
    s_bm_case->report(st);
}

void bm_ryml_json_inplace_reserve(bm::State& st)
{
    c4::substr src = c4::to_substr(s_bm_case->in_place).trimr('\0');
    ryml::id_type cap = estimate_capacity(src);
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        st.PauseTiming();
        s_bm_case->prepare(kResetInPlace);
        ryml::Tree tree(s_resident_callbacks);
        tree.reserve(cap);
        st.ResumeTiming();
        ryml::parse_json_in_place(s_bm_case->filename, src, &tree);
    }
    s_bm_case->report(st);
}

void bm_ryml_json_inplace_nofilter(bm::State& st)
{
    c4::substr src = c4::to_substr(s_bm_case->in_place).trimr('\0');
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        st.PauseTiming();
        s_bm_case->prepare(kResetInPlace);
        ryml::EventHandlerTree evth;
        ryml::Parser parser(&evth, ryml::ParserOptions().scalar_filtering(false));
        st.ResumeTiming();
        ryml::Tree tree = ryml::parse_json_in_place(&parser, s_bm_case->filename, src);
    }
    s_bm_case->report(st);
}

void bm_ryml_json_inplace_nofilter_reserve(bm::State& st)
{
    c4::substr src = c4::to_substr(s_bm_case->in_place).trimr('\0');
    ryml::id_type cap = estimate_capacity(src);
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        st.PauseTiming();
        s_bm_case->prepare(kResetInPlace);
        ryml::EventHandlerTree evth;
        ryml::Parser parser(&evth, ryml::ParserOptions().scalar_filtering(false));
        ryml::Tree tree(s_resident_callbacks);
        tree.reserve(cap);
        st.ResumeTiming();
        ryml::parse_json_in_place(&parser, s_bm_case->filename, src, &tree);
    }
    s_bm_case->report(st);
}


//-----------------------------------------------------------------------------

void bm_ryml_yaml_arena_reuse(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(s_bm_case->src).trimr('\0');
    for(auto _ : st)
    {
        s_bm_case->prepare(st, kClearTree|kClearTreeArena);
        parse_in_arena(&s_bm_case->ryml_parser, s_bm_case->filename, src, &s_bm_case->ryml_tree);
    }
    s_bm_case->report(st);
}

void bm_ryml_yaml_arena_reuse_reserve(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(s_bm_case->src).trimr('\0');
    ryml::id_type cap = estimate_capacity(src);
    for(auto _ : st)
    {
        s_bm_case->prepare(st, kClearTree|kClearTreeArena|kReserveTree, cap);
        parse_in_arena(&s_bm_case->ryml_parser, s_bm_case->filename, src, &s_bm_case->ryml_tree);
    }
    s_bm_case->report(st);
}

void bm_ryml_yaml_arena_reuse_nofilter(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(s_bm_case->src).trimr('\0');
    for(auto _ : st)
    {
        s_bm_case->prepare(st, kClearTree|kClearTreeArena);
        parse_in_arena(&s_bm_case->ryml_parser_nofilter, s_bm_case->filename, src);
    }
    s_bm_case->report(st);
}

void bm_ryml_yaml_arena_reuse_nofilter_reserve(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(s_bm_case->src).trimr('\0');
    ryml::id_type cap = estimate_capacity(src);
    for(auto _ : st)
    {
        s_bm_case->prepare(st, kClearTree|kClearTreeArena|kReserveTree, cap);
        parse_in_arena(&s_bm_case->ryml_parser_nofilter, s_bm_case->filename, src, &s_bm_case->ryml_tree);
    }
    s_bm_case->report(st);
}


//-----------------------------------------------------------------------------

void bm_ryml_json_arena_reuse(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(s_bm_case->src).trimr('\0');
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        s_bm_case->prepare(st, kClearTree|kClearTreeArena);
        parse_json_in_arena(&s_bm_case->ryml_parser, s_bm_case->filename, src, &s_bm_case->ryml_tree);
    }
    s_bm_case->report(st);
}

void bm_ryml_json_arena_reuse_reserve(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(s_bm_case->src).trimr('\0');
    ryml::id_type cap = estimate_capacity(src);
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        s_bm_case->prepare(st, kClearTree|kClearTreeArena|kReserveTree, cap);
        parse_json_in_arena(&s_bm_case->ryml_parser, s_bm_case->filename, src, &s_bm_case->ryml_tree);
    }
    s_bm_case->report(st);
}

void bm_ryml_json_arena_reuse_nofilter(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(s_bm_case->src).trimr('\0');
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        s_bm_case->prepare(st, kClearTree|kClearTreeArena);
        parse_json_in_arena(&s_bm_case->ryml_parser_nofilter, s_bm_case->filename, src, &s_bm_case->ryml_tree);
    }
    s_bm_case->report(st);
}

void bm_ryml_json_arena_reuse_nofilter_reserve(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(s_bm_case->src).trimr('\0');
    ryml::id_type cap = estimate_capacity(src);
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        s_bm_case->prepare(st, kClearTree|kClearTreeArena|kReserveTree, cap);
        parse_json_in_arena(&s_bm_case->ryml_parser_nofilter, s_bm_case->filename, src, &s_bm_case->ryml_tree);
    }
    s_bm_case->report(st);
}


//-----------------------------------------------------------------------------

void bm_rymlints_yaml_inplace_reuse(bm::State& st)
{
    c4::substr src = c4::to_substr(s_bm_case->in_place).trimr('\0');
    s_bm_case->int_obj.data.resize(src);
    for(auto _ : st)
    {
        s_bm_case->prepare(st, kResetInPlace);
        parse_yaml_inplace(s_bm_case->filename, src, s_bm_case->int_obj.parser, &s_bm_case->int_obj.data);
        bm::DoNotOptimize(s_bm_case->int_obj);
    }
    s_bm_case->report(st);
}

void bm_rymlints_yaml_inplace_reuse_nofilter(bm::State& st)
{
    c4::substr src = c4::to_substr(s_bm_case->in_place).trimr('\0');
    s_bm_case->int_obj_nofilter.data.resize(src);
    for(auto _ : st)
    {
        s_bm_case->prepare(st, kResetInPlace);
        parse_yaml_inplace(s_bm_case->filename, src, s_bm_case->int_obj_nofilter.parser, &s_bm_case->int_obj_nofilter.data);
        bm::DoNotOptimize(s_bm_case->int_obj_nofilter);
    }
    s_bm_case->report(st);
}


//-----------------------------------------------------------------------------

void bm_rymlints_json_inplace_reuse(bm::State& st)
{
    c4::substr src = c4::to_substr(s_bm_case->in_place).trimr('\0');
    s_bm_case->int_obj.data.resize(src);
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        s_bm_case->prepare(st, kResetInPlace);
        parse_json_inplace(s_bm_case->filename, src, s_bm_case->int_obj.parser, &s_bm_case->int_obj.data);
        bm::DoNotOptimize(s_bm_case->int_obj);
    }
    s_bm_case->report(st);
}

void bm_rymlints_json_inplace_reuse_nofilter(bm::State& st)
{
    c4::substr src = c4::to_substr(s_bm_case->in_place).trimr('\0');
    s_bm_case->int_obj_nofilter.data.resize(src);
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        s_bm_case->prepare(st, kResetInPlace);
        parse_json_inplace(s_bm_case->filename, src, s_bm_case->int_obj_nofilter.parser, &s_bm_case->int_obj_nofilter.data);
        bm::DoNotOptimize(s_bm_case->int_obj_nofilter);
    }
    s_bm_case->report(st);
}


//-----------------------------------------------------------------------------

void bm_ryml_yaml_inplace_reuse(bm::State& st)
{
    c4::substr src = c4::to_substr(s_bm_case->in_place).trimr('\0');
    for(auto _ : st)
    {
        s_bm_case->prepare(st, kResetInPlace|kClearTree|kClearTreeArena);
        parse_in_place(&s_bm_case->ryml_parser, s_bm_case->filename, src, &s_bm_case->ryml_tree);
    }
    s_bm_case->report(st);
}

void bm_ryml_yaml_inplace_reuse_reserve(bm::State& st)
{
    c4::substr src = c4::to_substr(s_bm_case->in_place).trimr('\0');
    ryml::id_type cap = estimate_capacity(src);
    for(auto _ : st)
    {
        s_bm_case->prepare(st, kResetInPlace|kClearTree|kClearTreeArena|kReserveTree, cap);
        parse_in_place(&s_bm_case->ryml_parser, s_bm_case->filename, src, &s_bm_case->ryml_tree);
    }
    s_bm_case->report(st);
}

void bm_ryml_yaml_inplace_reuse_nofilter(bm::State& st)
{
    c4::substr src = c4::to_substr(s_bm_case->in_place).trimr('\0');
    for(auto _ : st)
    {
        s_bm_case->prepare(st, kResetInPlace|kClearTree|kClearTreeArena);
        parse_in_place(&s_bm_case->ryml_parser_nofilter, s_bm_case->filename, src, &s_bm_case->ryml_tree);
    }
    s_bm_case->report(st);
}

void bm_ryml_yaml_inplace_reuse_nofilter_reserve(bm::State& st)
{
    c4::substr src = c4::to_substr(s_bm_case->in_place).trimr('\0');
    ryml::id_type cap = estimate_capacity(src);
    for(auto _ : st)
    {
        s_bm_case->prepare(st, kResetInPlace|kClearTree|kClearTreeArena|kReserveTree, cap);
        parse_in_place(&s_bm_case->ryml_parser_nofilter, s_bm_case->filename, src, &s_bm_case->ryml_tree);
    }
    s_bm_case->report(st);
}


//-----------------------------------------------------------------------------

void bm_ryml_json_inplace_reuse(bm::State& st)
{
    c4::substr src = c4::to_substr(s_bm_case->in_place).trimr('\0');
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        s_bm_case->prepare(st, kResetInPlace|kClearTree|kClearTreeArena);
        parse_json_in_place(&s_bm_case->ryml_parser, s_bm_case->filename, src, &s_bm_case->ryml_tree);
    }
    s_bm_case->report(st);
}

void bm_ryml_json_inplace_reuse_reserve(bm::State& st)
{
    c4::substr src = c4::to_substr(s_bm_case->in_place).trimr('\0');
    ryml::id_type cap = estimate_capacity(src);
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        s_bm_case->prepare(st, kResetInPlace|kClearTree|kClearTreeArena|kReserveTree, cap);
        parse_json_in_place(&s_bm_case->ryml_parser, s_bm_case->filename, src, &s_bm_case->ryml_tree);
    }
    s_bm_case->report(st);
}

void bm_ryml_json_inplace_reuse_nofilter(bm::State& st)
{
    c4::substr src = c4::to_substr(s_bm_case->in_place).trimr('\0');
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        s_bm_case->prepare(st, kResetInPlace|kClearTree|kClearTreeArena);
        parse_json_in_place(&s_bm_case->ryml_parser_nofilter, s_bm_case->filename, src, &s_bm_case->ryml_tree);
    }
    s_bm_case->report(st);
}

void bm_ryml_json_inplace_reuse_nofilter_reserve(bm::State& st)
{
    c4::substr src = c4::to_substr(s_bm_case->in_place).trimr('\0');
    ryml::id_type cap = estimate_capacity(src);
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        s_bm_case->prepare(st, kResetInPlace|kClearTree|kClearTreeArena|kReserveTree, cap);
        parse_json_in_place(&s_bm_case->ryml_parser_nofilter, s_bm_case->filename, src, &s_bm_case->ryml_tree);
    }
    s_bm_case->report(st);
}


//-----------------------------------------------------------------------------

BENCHMARK(bm_rymlints_estimate);


BENCHMARK(bm_rymlints_json_inplace_reuse_nofilter);
BENCHMARK(bm_rymlints_json_inplace_reuse);

BENCHMARK(bm_rymlints_json_inplace_nofilter_reserve);
BENCHMARK(bm_rymlints_json_inplace_nofilter);
BENCHMARK(bm_rymlints_json_inplace_reserve);
BENCHMARK(bm_rymlints_json_inplace);


BENCHMARK(bm_rymlints_yaml_inplace_reuse_nofilter);
BENCHMARK(bm_rymlints_yaml_inplace_reuse);

BENCHMARK(bm_rymlints_yaml_inplace_nofilter_reserve);
BENCHMARK(bm_rymlints_yaml_inplace_nofilter);
BENCHMARK(bm_rymlints_yaml_inplace_reserve);
BENCHMARK(bm_rymlints_yaml_inplace);


BENCHMARK(bm_ryml_json_inplace_reuse_nofilter_reserve);
BENCHMARK(bm_ryml_json_inplace_reuse_nofilter);
BENCHMARK(bm_ryml_json_inplace_reuse_reserve);
BENCHMARK(bm_ryml_json_inplace_reuse);

BENCHMARK(bm_ryml_json_arena_reuse_nofilter_reserve);
BENCHMARK(bm_ryml_json_arena_reuse_nofilter);
BENCHMARK(bm_ryml_json_arena_reuse_reserve);
BENCHMARK(bm_ryml_json_arena_reuse);

BENCHMARK(bm_ryml_json_inplace_nofilter_reserve);
BENCHMARK(bm_ryml_json_inplace_nofilter);
BENCHMARK(bm_ryml_json_inplace_reserve);
BENCHMARK(bm_ryml_json_inplace);

BENCHMARK(bm_ryml_json_arena_nofilter_reserve);
BENCHMARK(bm_ryml_json_arena_nofilter);
BENCHMARK(bm_ryml_json_arena_reserve);
BENCHMARK(bm_ryml_json_arena);


BENCHMARK(bm_ryml_yaml_inplace_reuse_nofilter_reserve);
BENCHMARK(bm_ryml_yaml_inplace_reuse_nofilter);
BENCHMARK(bm_ryml_yaml_inplace_reuse_reserve);
BENCHMARK(bm_ryml_yaml_inplace_reuse);

BENCHMARK(bm_ryml_yaml_arena_reuse_nofilter_reserve);
BENCHMARK(bm_ryml_yaml_arena_reuse_nofilter);
BENCHMARK(bm_ryml_yaml_arena_reuse_reserve);
BENCHMARK(bm_ryml_yaml_arena_reuse);

BENCHMARK(bm_ryml_yaml_inplace_nofilter_reserve);
BENCHMARK(bm_ryml_yaml_inplace_nofilter);
BENCHMARK(bm_ryml_yaml_inplace_reserve);
BENCHMARK(bm_ryml_yaml_inplace);

BENCHMARK(bm_ryml_yaml_arena_nofilter_reserve);
BENCHMARK(bm_ryml_yaml_arena_nofilter);
BENCHMARK(bm_ryml_yaml_arena_reserve);
BENCHMARK(bm_ryml_yaml_arena);


BENCHMARK(bm_libyaml_arena);
BENCHMARK(bm_libyaml_arena_reuse);
#ifdef RYML_HAVE_LIBFYAML
BENCHMARK(bm_libfyaml_arena);
#endif
BENCHMARK(bm_yamlcpp_arena);
BENCHMARK(bm_rapidjson_arena);
BENCHMARK(bm_rapidjson_inplace);
BENCHMARK(bm_sajson_arena);
BENCHMARK(bm_sajson_inplace);
BENCHMARK(bm_jsoncpp_arena);
BENCHMARK(bm_nlohmann_arena);
