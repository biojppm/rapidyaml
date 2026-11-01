#include "test_error_dump_gcov_on_error.hpp"
int main()
{
    RYML_DUMP_GCOV_ON_ERROR(
        c4::yml::Tree tree = c4::yml::parse_in_arena("bar");
        int val = {};
        tree.load(0, &val);
    );
}
