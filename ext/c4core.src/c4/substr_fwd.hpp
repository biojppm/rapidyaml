#ifndef _C4_SUBSTR_FWD_HPP_
#define _C4_SUBSTR_FWD_HPP_

namespace c4 {

#ifndef DOXYGEN
template<class C> struct basic_substring;
using csubstr = basic_substring<const char>;
using substr = basic_substring<char>;
template<class T> struct is_string;
template<class T> struct is_writeable_string;
#endif // !DOXYGEN

} // namespace c4

#endif /* _C4_SUBSTR_FWD_HPP_ */
