#ifdef C4_RESTRICT_HPP_ // must match the include guard from restrict.hpp

/** @file unrestrict.hpp cleans up restrict macros  */

#undef   $
#undef  $$
#undef  c$
#undef c$$

#undef C4_RESTRICT_HPP_

#ifdef __clang__
#   pragma clang diagnostic pop
#elif defined(__GNUC__)
#endif

#endif /* C4_RESTRICT_HPP_ */
