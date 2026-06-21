#include <stdio.h>
#include <c4/fs/fs.hpp>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *str, size_t len);

using cvec = std::vector<char>;


static void test_one_file(const char *filename, cvec *buf)
{
    C4_CHECK(c4::fs::is_file(filename));
    c4::fs::file_get_contents(filename, buf);
    (void)LLVMFuzzerTestOneInput(reinterpret_cast<const uint8_t*>(buf->data()), buf->size());
}


static void test_directory_files(const char *dirname, cvec *buf)
{
    std::vector<char*> names(256);
    cvec arena(names.size() * 256);
    cvec scratch_(1024);
    c4::fs::maybe_buf<char> scratch;
    c4::fs::EntryList entries;
retry:
    entries = {arena.data(), arena.size(), names.data(), names.size()};
    scratch = {scratch_.data(), scratch_.size()};
    if(!list_entries(dirname, &entries, &scratch))
    {
        names.resize(entries.names.required_size);
        arena.resize(entries.arena.required_size);
        scratch_.resize(scratch.required_size);
        goto retry; //NOLINT
    }
    entries.sort();
    for(const char *filename : entries)
    {
        test_one_file(filename, buf);
    }
    printf("success! tested %zu files\n", entries.names.size);
}


int main(int argc, const char *argv[])
{
    if(argc < 2)
        return 1;
    if(!c4::fs::path_exists(argv[1]))
        return 1;
    cvec buf(4096);
    if(c4::fs::is_dir(argv[1]))
        test_directory_files(argv[1], &buf);
    else
        test_one_file(argv[1], &buf);
    return 0;
}
