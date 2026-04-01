#ifndef C4_TEST_MAIN_HPP_
#define C4_TEST_MAIN_HPP_

/** we need to ensure callbacks are always enabled in the tests, even
 * when RYML_NO_DEFAULT_CALLBACKS is defined */
#define RYML_DEFINE_TEST_MAIN()           \
int main(int argc, char **argv)           \
{                                         \
    c4::yml::ensure_callbacks();          \
    testing::InitGoogleTest(&argc, argv); \
    return RUN_ALL_TESTS();               \
}


namespace c4 {
namespace yml {
void ensure_callbacks();
} // namespace yml
} // namespace c4


#endif /* C4_TEST_MAIN_HPP_ */
