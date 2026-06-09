#include "./bm_common.hpp"

#include <fstream>

/** this is used by the benchmarks.
 *
 * @note We cannot declare the case as value-static as there is no guarantee
 * that the allocator's lifetime starts before and ends after the case's
 * lifetime. So use a pointer to control the lifetime. */
static BmCase * C4_RESTRICT s_bm_case = nullptr;


int main(int argc, char** argv)
{
    BmCase fixture;
    s_bm_case = &fixture;
    s_bm_case->run("EMIT", argc, argv);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#define ONLY_FOR_JSON \
    if(!s_bm_case->is_json) { st.SkipWithError("not a json file"); return; }

namespace {

void reset(std::ostringstream &s)
{
    s.str("");
    s.clear();
}
void reset(std::ofstream &s)
{
    s.clear();
    s.seekp(0, std::ios::beg); // back to the start!
}
void reset(FILE *f)
{
    (void)fseek(f, 0, SEEK_SET);
}


void bm_rapidjson(bm::State& st)
{
    const char *src = s_bm_case->src.data();
    rapidjson::Document doc;
    rapidjson::StringBuffer buffer;
    if(s_bm_case->is_json)
        doc.Parse(src);
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        doc.Accept(writer);
    }
    s_bm_case->report(st);
}

void bm_jsoncpp(bm::State& st)
{
    const char *b = &s_bm_case->src.front(), *e = &s_bm_case->src.back();
    Json::Value root;
    Json::Reader reader;
    if(s_bm_case->is_json)
        reader.parse(b, e, root);
    std::string str;
    std::ostringstream os;
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        os << root;
        str = os.str();
        reset(os);
    }
    s_bm_case->report(st);
}

void bm_nlohmann(bm::State& st)
{
    const char* src = s_bm_case->src.data();
    nlohmann::json root;
    if(s_bm_case->is_json)
        root = nlohmann::json::parse(src);
    std::string str;
    std::ostringstream os;
    for(auto _ : st)
    {
        ONLY_FOR_JSON;
        os << root;
        str = os.str();
        reset(os);
    }
    s_bm_case->report(st);
}

#ifdef RYML_HAVE_LIBFYAML
C4_SUPPRESS_WARNING_GCC_CLANG_WITH_PUSH("-Wold-style-cast")
int fyaml_dumper_ostream(struct fy_emitter *emit, fy_emitter_write_type type, const char *str, int len, void *userdata)
{
    C4_UNUSED(emit);
    C4_UNUSED(type);
    auto *os = (std::ostringstream*)userdata;
    os->write(str, len);
    return 0;
}

int fyaml_dumper_str(struct fy_emitter *emit, fy_emitter_write_type type, const char *str, int len, void *userdata)
{
    C4_UNUSED(emit);
    C4_UNUSED(type);
    auto *s = (std::string*)userdata;
    s->append(str, len);
    return 0;
}
C4_SUPPRESS_WARNING_GCC_CLANG_POP

void bm_fyaml_ostream(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(s_bm_case->src);
    std::ostringstream os;
    std::string str;
    fy_emitter_cfg cfg = {};
    cfg.flags = FYECF_DEFAULT;
    cfg.output = &fyaml_dumper_ostream;
    cfg.userdata = &os;
    cfg.diag = nullptr;
    fy_emitter *emitter = fy_emitter_create(&cfg);
    fy_document *doc = fy_document_build_from_string(nullptr, src.str, src.len);
    for(auto _ : st)
    {
        fy_emit_document(emitter, doc);
        str = os.str();
        reset(os);
    }
    s_bm_case->report(st);
    fy_document_destroy(doc);
    fy_emitter_destroy(emitter);
}

void bm_fyaml_str(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(s_bm_case->src);
    std::string str;
    fy_emitter_cfg cfg = {};
    cfg.flags = FYECF_DEFAULT;
    cfg.output = &fyaml_dumper_str;
    cfg.userdata = &str;
    cfg.diag = nullptr;
    fy_emitter *emitter = fy_emitter_create(&cfg);
    fy_document *doc = fy_document_build_from_string(nullptr, src.str, src.len);
    for(auto _ : st)
    {
        fy_emit_document(emitter, doc);
        str.clear();
    }
    s_bm_case->report(st);
    fy_document_destroy(doc);
    fy_emitter_destroy(emitter);
}

void bm_fyaml_str_reserve(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(s_bm_case->src);
    std::string str;
    str.resize(2 * src.size());
    fy_emitter_cfg cfg = {};
    cfg.flags = FYECF_DEFAULT;
    cfg.output = &fyaml_dumper_str;
    cfg.userdata = &str;
    cfg.diag = nullptr;
    fy_emitter *emitter = fy_emitter_create(&cfg);
    fy_document *doc = fy_document_build_from_string(nullptr, src.str, src.len);
    for(auto _ : st)
    {
        fy_emit_document(emitter, doc);
        str.clear();
    }
    s_bm_case->report(st);
    fy_document_destroy(doc);
    fy_emitter_destroy(emitter);
}
#endif

void bm_yamlcpp(bm::State& st)
{
    const char* src = s_bm_case->src.data();
    YAML::Node node = YAML::Load(src);
    std::string str;
    std::ostringstream os;
    for(auto _ : st)
    {
        os << node;
        str = os.str();
        reset(os);
    }
    s_bm_case->report(st);
}

void bm_ryml_yaml_ostream(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(s_bm_case->src).trimr('\0');
    ryml::Tree tree = ryml::parse_in_arena(s_bm_case->filename, src);
    std::string str;
    std::ostringstream os;
    for(auto _ : st)
    {
        os << tree;
        str = os.str();
        reset(os);
    }
    s_bm_case->report(st);
}

void bm_ryml_yaml_ofstream(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(s_bm_case->src).trimr('\0');
    ryml::Tree tree = ryml::parse_in_arena(s_bm_case->filename, src);
    std::ofstream os(c4::fs::tmpnam<std::string>());
    for(auto _ : st)
    {
        os << tree;
        reset(os);
    }
    s_bm_case->report(st);
}

void bm_ryml_yaml_file(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(s_bm_case->src).trimr('\0');
    ryml::Tree tree = ryml::parse_in_arena(s_bm_case->filename, src);
    std::string name = c4::fs::tmpnam<std::string>();
    FILE *f = fopen(name.c_str(), "wb");
    C4_CHECK(f);
    for(auto _ : st)
    {
        emit_yaml(tree, f);
        reset(f);
    }
    (void)fclose(f);
    s_bm_case->report(st);
}

void bm_ryml_yaml_str_file(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(s_bm_case->src).trimr('\0');
    ryml::Tree tree = ryml::parse_in_arena(s_bm_case->filename, src);
    std::string name = c4::fs::tmpnam<std::string>();
    FILE *f = fopen(name.c_str(), "wb");
    C4_CHECK(f);
    std::string str;
    for(auto _ : st)
    {
        emitrs_yaml(tree, &str);
        ryml::file_put_contents(str, f, name.c_str());
        reset(f);
    }
    (void)fclose(f);
    s_bm_case->report(st);
}

void bm_ryml_json_ostream(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(s_bm_case->src).trimr('\0');
    ryml::Tree tree = ryml::parse_in_arena(s_bm_case->filename, src);
    std::string str;
    std::ostringstream os;
    for(auto _ : st)
    {
        os << ryml::as_json(tree);
        str = os.str();
        reset(os);
    }
    s_bm_case->report(st);
}

void bm_ryml_json_ofstream(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(s_bm_case->src).trimr('\0');
    ryml::Tree tree = ryml::parse_in_arena(s_bm_case->filename, src);
    std::ofstream os(c4::fs::tmpnam<std::string>());
    for(auto _ : st)
    {
        os << ryml::as_json(tree);
        reset(os);
    }
    s_bm_case->report(st);
}

void bm_ryml_json_file(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(s_bm_case->src).trimr('\0');
    ryml::Tree tree = ryml::parse_in_arena(s_bm_case->filename, src);
    std::string name = c4::fs::tmpnam<std::string>();
    FILE *f = fopen(name.c_str(), "wb");
    C4_CHECK(f);
    for(auto _ : st)
    {
        emit_json(tree, f);
        reset(f);
    }
    (void)fclose(f);
    s_bm_case->report(st);
}

void bm_ryml_json_str_file(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(s_bm_case->src).trimr('\0');
    ryml::Tree tree = ryml::parse_in_arena(s_bm_case->filename, src);
    std::string name = c4::fs::tmpnam<std::string>();
    FILE *f = fopen(name.c_str(), "wb");
    C4_CHECK(f);
    std::string str;
    for(auto _ : st)
    {
        emitrs_json(tree, &str);
        ryml::file_put_contents(str, f, name.c_str());
        reset(f);
    }
    (void)fclose(f);
    s_bm_case->report(st);
}

void bm_ryml_yaml_str(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(s_bm_case->src).trimr('\0');
    ryml::Tree tree = ryml::parse_in_arena(s_bm_case->filename, src);
    std::string str;
    for(auto _ : st)
    {
        emitrs_yaml(tree, &str);
    }
    s_bm_case->report(st);
}

void bm_ryml_json_str(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(s_bm_case->src).trimr('\0');
    ryml::Tree tree = ryml::parse_in_arena(s_bm_case->filename, src);
    std::string str;
    for(auto _ : st)
    {
        emitrs_json(tree, &str);
    }
    s_bm_case->report(st);
}

void bm_ryml_yaml_str_reserve(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(s_bm_case->src).trimr('\0');
    std::string str;
    str.resize(2 * src.size());
    ryml::Tree tree = ryml::parse_in_arena(s_bm_case->filename, src);
    for(auto _ : st)
    {
        emitrs_yaml(tree, &str);
    }
    s_bm_case->report(st);
}

void bm_ryml_json_str_reserve(bm::State& st)
{
    c4::csubstr src = c4::to_csubstr(s_bm_case->src).trimr('\0');
    std::string str;
    str.resize(2 * src.size());
    ryml::Tree tree = ryml::parse_in_arena(s_bm_case->filename, src);
    for(auto _ : st)
    {
        emitrs_json(tree, &str);
    }
    s_bm_case->report(st);
}

BENCHMARK(bm_ryml_yaml_str_reserve);
BENCHMARK(bm_ryml_json_str_reserve);
BENCHMARK(bm_ryml_yaml_str);
BENCHMARK(bm_ryml_json_str);
BENCHMARK(bm_ryml_yaml_ostream);
BENCHMARK(bm_ryml_json_ostream);
BENCHMARK(bm_ryml_yaml_ofstream);
BENCHMARK(bm_ryml_json_ofstream);
BENCHMARK(bm_ryml_yaml_file);
BENCHMARK(bm_ryml_json_file);
BENCHMARK(bm_ryml_yaml_str_file);
BENCHMARK(bm_ryml_json_str_file);
#ifdef RYML_HAVE_LIBFYAML
BENCHMARK(bm_fyaml_str_reserve);
BENCHMARK(bm_fyaml_str);
BENCHMARK(bm_fyaml_ostream);
#endif
BENCHMARK(bm_yamlcpp);
BENCHMARK(bm_rapidjson);
BENCHMARK(bm_jsoncpp);
BENCHMARK(bm_nlohmann);

} // namespace anon
