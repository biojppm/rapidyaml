#include <stdio.h>
#include <c4/fs/fs.hpp>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *str, size_t len);

int main(int argc, const char *argv[])
{
    if(argc < 2)
        return 1;
    const char *filename = argv[1];
    if(!c4::fs::file_exists(filename))
        return 1;
    std::string file = c4::fs::file_get_contents<std::string>(filename);
    (void)LLVMFuzzerTestOneInput(reinterpret_cast<const uint8_t*>(&file[0]), file.size());
    return 0;
}
