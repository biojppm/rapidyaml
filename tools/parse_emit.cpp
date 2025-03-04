#ifdef RYML_SINGLE_HEADER
#include <ryml_all.hpp>
#else
#include <c4/yml/std/std.hpp>
#include <c4/yml/parse.hpp>
#include <c4/yml/emit.hpp>
#include <c4/yml/common.hpp>
#endif
#include <c4/fs/fs.hpp>

#include <cstdio>
#include <chrono>

#ifdef C4_EXCEPTIONS
#include <stdexcept>
#else
#include <csetjmp>
std::jmp_buf jmp_env = {};
c4::csubstr jmp_msg = {};
#endif


using namespace c4;


C4_SUPPRESS_WARNING_GCC_CLANG_WITH_PUSH("-Wold-style-cast")
C4_SUPPRESS_WARNING_GCC("-Wuseless-cast")


bool quiet = false;
bool emit_as_json = false;
bool timed_sections = false;
bool emit_to_string = false;


//-----------------------------------------------------------------------------

struct timed_section
{
    using myclock = std::chrono::steady_clock;
    using msecs = std::chrono::duration<double, std::milli>;

    csubstr name;
    myclock::time_point start;

    msecs since() const { return myclock::now() - start; }
    timed_section(csubstr n) : name(n), start(myclock::now()) {}
    ~timed_section()
    {
        if(timed_sections)
        {
            fprintf(stderr, "%.6fms: %.*s\n", since().count(), (int)name.len, name.str);
            fflush(stderr);
        }
    }
};

#define TS(name) timed_section C4_XCAT(__, C4_XCAT(name, __LINE__))(#name)

// LCOV_EXCL_START

constexpr const char *usage = R"(usage:

   %s <options> <path/to/file.yaml>

Parse yaml from file (or stdin when file is `-`) and emit to stdout.

Options:

  -q,--quiet    do not emit (default: emit yaml)
  -d,--debug    print rapidyaml tree
  -j,--json     emit json instead of yaml (default: emit yaml)
  -s,--string   emit to string before dumping to stdout.
                otherwise, emit directly to stdout
  -t,--timed    time sections (print timings to stderr)

)";

csubstr parse_args(int argc, const char *argv[])
{
    if(argc < 2)
    {
        printf(usage, argv[0]);
        yml::err_basic(RYML_LOC_HERE(), "unknown argument");
    }
    csubstr file = to_csubstr(argv[argc - 1]);
    for(int i = 1; i+1 < argc; ++i)
    {
        csubstr arg = to_csubstr(argv[i]);
        if(arg == "-q" || arg == "--quiet")
            quiet = true;
        else if(arg == "-t" || arg == "--timed")
            timed_sections = true;
        else if(arg == "-s" || arg == "--string")
            emit_to_string = true;
        else if(arg == "-j" || arg == "--json")
            emit_as_json = true;
        else
        {
            printf(usage, argv[0]);
            yml::err_basic(RYML_LOC_HERE(), "unknown argument");
        }
    }
    return file;
}

void load_file(csubstr filename, std::string *buf)
{
    buf->clear();
    if(filename == "-") // read from stdin
    {
        for(int c = std::getchar(); c != EOF; c = std::getchar())
        {
            buf->push_back((char)c);
            if(buf->size() == buf->capacity())
                buf->reserve(2u * (buf->capacity() >= 128u ? buf->capacity() : 128u));
        }
    }
    else
    {
        if(!fs::path_exists(filename.str))
        {
            std::fprintf(stderr, "cannot find file: %s (cwd=%s)\n", filename.str, fs::cwd<std::string>().c_str());
            yml::err_basic(RYML_LOC_HERE(), "file not found");
        }
        fs::file_get_contents<std::string>(filename.str, buf);
    }
}

void emit_json_docs(yml::Tree const& tree, std::string *dst=nullptr)
{
    auto emitnode = [&](yml::ConstNodeRef node){
        if(dst)
        {
            emitrs_json(node, dst, /*append*/true);
            *dst += '\n';
        }
        else
        {
            emit_json(node, stdout);
        }
    };
    yml::ConstNodeRef root = tree.rootref();
    if(!root.is_stream())
        emitnode(root);
    else
        for(yml::ConstNodeRef doc : root.children())
            emitnode(doc);
}

void report_error(const char* msg, size_t length, FILE *f)
{
    fwrite(msg, 1, length, f);
    fflush(f);
}

[[noreturn]] void throwerr(const char *msg, size_t len)
{
    C4_IF_EXCEPTIONS(
        throw std::runtime_error({msg, len});
        ,
        jmp_msg.assign(msg, len);
        std::longjmp(jmp_env, 1);
        );
    C4_UNREACHABLE_AFTER_ERR();
}

yml::Callbacks create_custom_callbacks()
{
    return yml::Callbacks{}
        .set_error_basic([](const char *msg, size_t msg_len, yml::Location const& cpploc, void *){
            yml::err_basic_print(msg, msg_len, cpploc, stderr);
            throwerr(msg, msg_len);
        })
        .set_error_parse([](const char *msg, size_t msg_len, yml::Location const& cpploc, yml::Location const& ymlloc, void *){
            yml::err_parse_print(msg, msg_len, cpploc, ymlloc, stderr);
            throwerr(msg, msg_len);
        })
        .set_error_visit([](const char *msg, size_t msg_len, yml::Location const& cpploc, yml::Tree const* tree, yml::id_type id, void *){
            yml::err_visit_print(msg, msg_len, cpploc, tree, id, stderr);
            throwerr(msg, msg_len);
        });
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void process_file(csubstr file)
{
    TS(objects);
    std::string contents;
    yml::Tree tree(yml::get_callbacks());
    {
        TS(read_file);
        load_file(file, &contents);
    }
    {
        TS(tree_reserve);
        yml::id_type cap;
        {
            TS(estimate_capacity);
            cap = yml::estimate_tree_capacity(to_csubstr(contents));
        }
        fprintf(stderr, "reserving capacity=%zu\n", (size_t)cap);
        tree.reserve(cap);
    }
    {
        TS(parse_yml);
        yml::parse_in_place(file, to_substr(contents), &tree);
    }
    if(emit_as_json)
    {
        TS(resolve_refs);
        tree.resolve();
    }
    if(emit_to_string)
    {
        std::string output;
        {
            TS(emit_to_buffer);
            output.resize(contents.size()); // resize, not just reserve
            if(!emit_as_json)
                yml::emitrs_yaml(tree, &output);
            else
                emit_json_docs(tree, &output);
        }
        if(!quiet)
        {
            TS(print_stdout);
            fwrite(output.data(), 1, output.size(), stdout);
        }
    }
    else if(!quiet)
    {
        TS(emit_to_stdout);
        if(!emit_as_json)
            yml::emit_yaml(tree);
        else
            emit_json_docs(tree);
    }
}
// LCOV_EXCL_STOP

int main(int argc, const char *argv[])
{
    TS(TOTAL);
    set_callbacks(create_custom_callbacks());
    C4_IF_EXCEPTIONS_(try, if(setjmp(jmp_env) == 0))
    {
        csubstr file = parse_args(argc, argv);
        process_file(file);
    }
    C4_IF_EXCEPTIONS_(catch(std::exception const&), else)
    {
        return 1;
    }
    return 0;
}

C4_SUPPRESS_WARNING_GCC_CLANG_POP
