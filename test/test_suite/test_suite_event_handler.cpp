#ifndef RYML_SINGLE_HEADER
#include <c4/yml/node.hpp>
#include <c4/yml/std/string.hpp>
#endif
#include "./test_suite_event_handler.hpp"

// FIXME instantiate the templates
#include "c4/yml/parse_engine.def.hpp"
namespace c4 {
namespace yml {
template class ParseEngine<EventHandlerYamlStd>;
} // namespace yml
} // namespace c4
