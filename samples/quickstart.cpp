// ryml: quickstart
//
// This file does a quick tour of ryml. It has multiple self-contained
// and commented samples that illustrate how to use ryml, and how it
// works. Although this is not a unit test, the samples are written as
// a sequence of actions and predicate checks to better convey what is
// the expected result at any stage. And to ensure the code here is
// correct and up to date, it's also run as part of the CI tests.
//
// The directories that exist side-by-side with this file contain
// several examples on how to build this with cmake, such that you can
// hit the ground running. I suggest starting first with the
// add_subdirectory example, treating it just like any other
// self-contained cmake project.
//
// If something is unclear, please open an issue or send a pull
// request at https://github.com/biojppm/rapidyaml . If you have an
// issue while using ryml, it is also encouraged to try to reproduce
// the issue here, or look first through the relevant section.
//
// Happy ryml'ing!


//-----------------------------------------------------------------------------

// <ryml_std.hpp> is only needed if interop with std types is desired.
// ryml itself does not use any STL container.
#include <ryml_std.hpp> // optional header. BUT when used, needs to be included BEFORE ryml.hpp
#include <ryml.hpp>

// tbese are only needed for the examples below
#include <c4/format.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include <array>
#include <map>
#include <c4/yml/preprocess.hpp> // needed only for the json sample


//-----------------------------------------------------------------------------

// CONTENTS:
//
// (Each function addresses a topic and is fully self-contained. Jump
// to the function to find out about its topic.)
namespace sample {
void sample_quick_overview();       ///< briefly skim over most of the features
void sample_substr();               ///< about ryml's string views (from c4core)
void sample_parse_file();           ///< ready-to-go example of parsing a file from disk
void sample_parse_read_only();      ///< parse a read-only YAML source buffer
void sample_parse_in_situ();        ///< parse an immutable YAML source buffer
void sample_parse_reuse_tree();     ///< parse into an existing tree, maybe into a node
void sample_parse_reuse_parser();   ///< reuse an existing parser
void sample_parse_reuse_tree_and_parser(); ///< how to reuse existing trees and parsers
void sample_iterate_trees();        ///< visit individual nodes and iterate through trees
void sample_create_trees();         ///< programatically create trees
void sample_tree_arena();           ///< interact with the tree's serialization arena
void sample_fundamental_types();    ///< serialize/deserialize fundamental types
void sample_formatting();           ///< control formatting when serializing/deserializing
void sample_base64();               ///< encode/decode base64
void sample_user_scalar_types();    ///< serialize/deserialize scalar (leaf/string) types
void sample_user_container_types(); ///< serialize/deserialize container (map or seq) types
void sample_std_types();            ///< serialize/deserialize STL containers
void sample_emit_to_container();    ///< emit to memory, eg a string or vector-like container
void sample_emit_to_stream();       ///< emit to a stream, eg std::ostream
void sample_emit_to_file();         ///< emit to a FILE*
void sample_emit_nested_node();     ///< pick a nested node as the root when emitting
void sample_json();                 ///< JSON parsing and emitting
void sample_anchors_and_aliases();  ///< deal with YAML anchors and aliases
void sample_tags();                 ///< deal with YAML type tags
void sample_docs();                 ///< deal with YAML docs
void sample_error_handler();        ///< set a custom error handler
void sample_global_allocator();     ///< set a global allocator for ryml
void sample_per_tree_allocator();   ///< set per-tree allocators
int  report_checks();
} /* namespace sample */

int main()
{
    sample::sample_quick_overview();
    sample::sample_substr();
    sample::sample_parse_file();
    sample::sample_parse_read_only();
    sample::sample_parse_in_situ();
    sample::sample_parse_reuse_tree();
    sample::sample_parse_reuse_parser();
    sample::sample_parse_reuse_tree_and_parser();
    sample::sample_iterate_trees();
    sample::sample_create_trees();
    sample::sample_tree_arena();
    sample::sample_fundamental_types();
    sample::sample_formatting();
    sample::sample_base64();
    sample::sample_user_scalar_types();
    sample::sample_user_container_types();
    sample::sample_std_types();
    sample::sample_emit_to_container();
    sample::sample_emit_to_stream();
    sample::sample_emit_to_file();
    sample::sample_emit_nested_node();
    sample::sample_json();
    sample::sample_anchors_and_aliases();
    sample::sample_tags();
    sample::sample_docs();
    sample::sample_error_handler();
    sample::sample_global_allocator();
    sample::sample_per_tree_allocator();
    return sample::report_checks();
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

namespace sample {

/// a quick'n'dirty assertion to verify a predicate
#define CHECK(predicate) do { if(!report_check(__FILE__, __LINE__, #predicate, (predicate))) { C4_DEBUG_BREAK(); } } while(0)

bool report_check(const char *file, int line, const char *predicate, bool result);


//-----------------------------------------------------------------------------

/** a brief tour over most features */
void sample_quick_overview()
{
    // Parse YAML code in place, potentially mutating the buffer.
    // It is also possible to:
    //   - parse a read-only buffer
    //   - reuse an existing tree (advised)
    //   - reuse an existing parser (advised)
    char yml_buf[] = "{foo: 1, bar: [2, 3], john: doe}";
    ryml::Tree tree = ryml::parse(ryml::substr(yml_buf));

    // Note: it will always be significantly faster to use mutable
    // buffers and reuse tree+parser.
    //
    // Below you will find samples that show how to achieve reuse; but
    // please note that for brevity and clarity, many of the examples
    // here are parsing immutable buffers, and not reusing tree or
    // parser.


    //------------------------------------------------------------------
    // API overview

    // ryml has a two-level API:
    //
    // The lower level index API is based on the indices of nodes,
    // where the node's id is the node's position in the tree's data
    // array. This API is very efficient, but somewhat difficult to use:
    size_t root_id = tree.root_id();
    size_t bar_id = tree.find_child(root_id, "bar"); // need to get the index right
    CHECK(tree.is_map(root_id)); // all of the index methods are in the tree
    CHECK(tree.is_seq(bar_id));  // ... and receive the subject index

    // The node API is a lightweight abstraction sitting on top of the
    // index API, but offering a much more convenient interaction:
    ryml::NodeRef root = tree.rootref();
    ryml::NodeRef bar = tree["bar"];
    CHECK(root.is_map());
    CHECK(bar.is_seq());
    // NodeRef is a lightweight handle to the tree and associated id:
    CHECK(root.tree() == &tree); // NodeRef points at its tree, WITHOUT refcount
    CHECK(root.id() == root_id); // NodeRef's id is the index of the node
    CHECK(bar.id() == bar_id);   // NodeRef's id is the index of the node

    // The node API translates very cleanly to the index API, so most
    // of the code examples below are using the node API.

    // One significant point of the node API is that it holds a raw
    // pointer to the tree. Care must be taken to ensure the lifetimes
    // match, so that a node will never access the tree after the tree
    // went out of scope.


    //------------------------------------------------------------------
    // To read the parsed tree

    // Node::operator[] does a lookup, is O(num_children[node]).
    // maps use string keys, seqs use integral keys.
    CHECK(tree["foo"].is_keyval());
    CHECK(tree["foo"].key() == "foo");
    CHECK(tree["foo"].val() == "1");
    CHECK(tree["bar"].is_seq());
    CHECK(tree["bar"].has_key());
    CHECK(tree["bar"].key() == "bar");
    CHECK(tree["bar"][0].val() == "2");
    CHECK(tree["bar"][1].val() == "3");
    CHECK(tree["john"].val() == "doe");
    // An integral key is the position of the child within its parent,
    // so even maps can also use int keys, if the key position is
    // known.
    CHECK(tree[0].id() == tree["foo"].id());
    CHECK(tree[1].id() == tree["bar"].id());
    CHECK(tree[2].id() == tree["john"].id());
    // Tree::operator[](int) searches a root child by its position.
    CHECK(tree[0].id() == tree["foo"].id());  // 0: first child of root
    CHECK(tree[1].id() == tree["bar"].id());  // 1: first child of root
    CHECK(tree[2].id() == tree["john"].id()); // 2: first child of root
    // NodeRef::operator[](int) searches a node child by its position
    // on __the node__'s children list:
    CHECK(bar[0].val() == "2"); // 0 means first child of bar
    CHECK(bar[1].val() == "3"); // 1 means second child of bar
    // NodeRef::operator[](string):
    // A string key is the key of the node: lookup is by name. So it
    // is only available for maps, and it is NOT available for seqs,
    // since seq members do not have keys.
    CHECK(tree["foo"].key() == "foo");
    CHECK(tree["bar"].key() == "bar");
    CHECK(tree["john"].key() == "john");
    CHECK(bar.is_seq());
    // CHECK(bar["BOOM!"].is_seed()); // error, seqs do not have key lookup

    // Note that maps can also use index keys as well as string keys:
    CHECK(root["foo"].id() == root[0].id());
    CHECK(root["bar"].id() == root[1].id());
    CHECK(root["john"].id() == root[2].id());

    // Please note that since a ryml tree uses indexed linked lists for storing
    // children, the complexity of `Tree::operator[csubstr]` and
    // `Tree::operator[size_t]` is linear on the number of root children. If you use
    // it with a large tree where the root has many children, you may get a
    // performance hit. To avoid this hit, you can create your own accelerator
    // structure. For example, before doing a lookup, do a single traverse at the
    // root level to fill an `std::map<csubstr,size_t>` mapping key names to node
    // indices; with a node index, a lookup (via `Tree::get()`) is O(1), so this way
    // you can get O(log n) lookup from a key.
    //
    // As for `NodeRef`, the difference from `NodeRef::operator[]`
    // to `Tree::operator[]` is that the latter refers to the root node, whereas
    // the former can be invoked on any node. But the lookup process is the same for
    // both and their algorithmic complexity is the same: they are both linear in
    // the number of direct children; but depending on the data, that number may
    // be very different from one to another.

    //------------------------------------------------------------------
    // Hierarchy:

    {
        ryml::NodeRef foo = root.first_child();
        ryml::NodeRef john = root.last_child();
        CHECK(tree.size() == 6); // O(1) number of nodes in the tree
        CHECK(root.num_children() == 3); // O(num_children[root])
        CHECK(foo.num_siblings() == 3); // O(num_children[parent(foo)])
        CHECK(foo.parent().id() == root.id()); // parent() is O(1)
        CHECK(root.first_child().id() == root["foo"].id()); // first_child() is O(1)
        CHECK(root.last_child().id() == root["john"].id()); // last_child() is O(1)
        CHECK(john.first_sibling().id() == foo.id());
        CHECK(foo.last_sibling().id() == john.id());
        // prev_sibling(), next_sibling(): (both are O(1))
        CHECK(foo.num_siblings() == root.num_children());
        CHECK(foo.prev_sibling().id() == ryml::NONE); // foo is the first_child()
        CHECK(foo.next_sibling().key() == "bar");
        CHECK(foo.next_sibling().next_sibling().key() == "john");
        CHECK(foo.next_sibling().next_sibling().next_sibling().id() == ryml::NONE); // john is the last_child()
    }


    //------------------------------------------------------------------
    // Iterating:
    {
        ryml::csubstr expected_keys[] = {"foo", "bar", "john"};
        // iterate children using the high-level node API:
        {
            size_t count = 0;
            for(ryml::NodeRef const& child : root.children())
                CHECK(child.key() == expected_keys[count++]);
        }
        // iterate siblings using the high-level node API:
        {
            size_t count = 0;
            for(ryml::NodeRef const& child : root["foo"].siblings())
                CHECK(child.key() == expected_keys[count++]);
        }
        // iterate children using the lower-level tree index API:
        {
            size_t count = 0;
            for(size_t child_id = tree.first_child(root_id); child_id != ryml::NONE; child_id = tree.next_sibling(child_id))
                CHECK(tree.key(child_id) == expected_keys[count++]);
        }
        // iterate siblings using the lower-level tree index API:
        // (notice the only difference from above is in the loop
        // preamble, which calls tree.first_sibling(bar_id) instead of
        // tree.first_child(root_id))
        {
            size_t count = 0;
            for(size_t child_id = tree.first_sibling(bar_id); child_id != ryml::NONE; child_id = tree.next_sibling(child_id))
                CHECK(tree.key(child_id) == expected_keys[count++]);
        }
    }


    //------------------------------------------------------------------
    // Gotchas:
    CHECK(!tree["bar"].has_val());          // seq is a container, so no val
    CHECK(!tree["bar"][0].has_key());       // belongs to a seq, so no key
    CHECK(!tree["bar"][1].has_key());       // belongs to a seq, so no key
    //CHECK(tree["bar"].val() == BOOM!);    // ... so attempting to get a val is undefined behavior
    //CHECK(tree["bar"][0].key() == BOOM!); // ... so attempting to get a key is undefined behavior
    //CHECK(tree["bar"][1].key() == BOOM!); // ... so attempting to get a key is undefined behavior


    //------------------------------------------------------------------
    // Deserializing: use operator>>
    {
        int foo = 0, bar0 = 0, bar1 = 0;
        std::string john;
        root["foo"] >> foo;
        root["bar"][0] >> bar0;
        root["bar"][1] >> bar1;
        root["john"] >> john; // requires from_chars(std::string). see serialization samples below.
        CHECK(foo == 1);
        CHECK(bar0 == 2);
        CHECK(bar1 == 3);
        CHECK(john == "doe");
    }


    //------------------------------------------------------------------
    // Modifying existing nodes: operator<< vs operator=

    // operator= assigns an existing string to the receiving node.
    // This pointer will be in effect until the tree goes out of scope
    // so beware to only assign from strings outliving the tree.
    root["foo"] = "says you";
    root["bar"][0] = "-2";
    root["bar"][1] = "-3";
    root["john"] = "ron";
    // Now the tree is _pointing_ at the memory of the strings above.
    // That is OK because those are static strings and will outlive
    // the tree.
    CHECK(root["foo"].val() == "says you");
    CHECK(root["bar"][0].val() == "-2");
    CHECK(root["bar"][1].val() == "-3");
    CHECK(root["john"].val() == "ron");
    // WATCHOUT: do not assign from temporary objects:
    // {
    //     std::string crash("will dangle");
    //     root["john"] = ryml::to_csubstr(crash);
    // }
    // CHECK(root["john"] == "dangling"); // CRASH! the string was deallocated

    // operator<< first serializes the input to the tree's arena, then
    // assigns the serialized string to the receiving node. This avoids
    // constraints with the lifetime, since the arena lives with the tree.
    CHECK(tree.arena().empty());
    root["foo"] << "says who";  // requires to_chars(). see serialization samples below.
    root["bar"][0] << 20;
    root["bar"][1] << 30;
    root["john"] << "deere";
    CHECK(root["foo"].val() == "says who");
    CHECK(root["bar"][0].val() == "20");
    CHECK(root["bar"][1].val() == "30");
    CHECK(root["john"].val() == "deere");
    CHECK(tree.arena() == "says who2030deere"); // the result of serializations to the tree arena
    // using operator<< instead of operator=, the crash above is avoided:
    {
        std::string ok("in_scope");
        // root["john"] = ryml::to_csubstr(ok); // don't, will dangle
        root["john"] << ryml::to_csubstr(ok); // OK, copy to the tree's arena
    }
    CHECK(root["john"] == "in_scope"); // OK!
    CHECK(tree.arena() == "says who2030deerein_scope"); // the result of serializations to the tree arena


    //------------------------------------------------------------------
    // Adding new nodes:

    // adding a keyval node to a map:
    CHECK(root.num_children() == 3);
    root["newkeyval"] = "shiny and new"; // using these strings
    root.append_child() << ryml::key("newkeyval (serialized)") << "shiny and new (serialized)"; // serializes and assigns the serialization
    CHECK(root.num_children() == 5);
    CHECK(root["newkeyval"].key() == "newkeyval");
    CHECK(root["newkeyval"].val() == "shiny and new");
    CHECK(root["newkeyval (serialized)"].key() == "newkeyval (serialized)");
    CHECK(root["newkeyval (serialized)"].val() == "shiny and new (serialized)");
    CHECK( ! root["newkeyval"].key().is_sub(tree.arena())); // it's using directly the static string above
    CHECK( ! root["newkeyval"].val().is_sub(tree.arena())); // it's using directly the static string above
    CHECK(   root["newkeyval (serialized)"].key().is_sub(tree.arena())); // it's using a serialization of the string above
    CHECK(   root["newkeyval (serialized)"].val().is_sub(tree.arena())); // it's using a serialization of the string above
    // adding a val node to a seq:
    CHECK(root["bar"].num_children() == 2);
    root["bar"][2] = "oh so nice";
    root["bar"][3] << "oh so nice (serialized)";
    CHECK(root["bar"].num_children() == 4);
    CHECK(root["bar"][2].val() == "oh so nice");
    CHECK(root["bar"][3].val() == "oh so nice (serialized)");
    // adding a seq node:
    CHECK(root.num_children() == 5);
    root["newseq"] |= ryml::SEQ;
    root.append_child() << ryml::key("newseq (serialized)") |= ryml::SEQ;
    CHECK(root.num_children() == 7);
    CHECK(root["newseq"].num_children() == 0);
    CHECK(root["newseq (serialized)"].num_children() == 0);
    // adding a map node:
    CHECK(root.num_children() == 7);
    root["newmap"] |= ryml::MAP;
    root.append_child() << ryml::key("newmap (serialized)") |= ryml::SEQ;
    CHECK(root.num_children() == 9);
    CHECK(root["newmap"].num_children() == 0);
    CHECK(root["newmap (serialized)"].num_children() == 0);
    // operator[] does not mutate the tree until the returned node is
    // written to.
    //
    // Until such time, the NodeRef object keeps in itself the required
    // information to write to the proper place in the tree. This is
    // called being in a "seed" state.
    //
    // This means that passing a key/index which does not exist will
    // not mutate the tree, but will instead store (in the node) the
    // proper place of the tree to do so if and when it is required.
    //
    // This is a significant difference from eg, the behavior of
    // std::map, which mutates the map immediately within the call to
    // operator[].
    CHECK(!root.has_child("I am nobody"));
    ryml::NodeRef nobody = root["I am nobody"];
    CHECK(nobody.valid());   // points at the tree, and a specific place in the tree
    CHECK(nobody.is_seed()); // ... but nothing is there yet.
    CHECK(!root.has_child("I am nobody")); // same as above
    ryml::NodeRef somebody = root["I am somebody"];
    CHECK(!root.has_child("I am somebody")); // same as above
    CHECK(somebody.valid());
    CHECK(somebody.is_seed()); // same as above
    somebody = "indeed";  // this will commit to the tree, mutating at the proper place
    CHECK(somebody.valid());
    CHECK(!somebody.is_seed()); // now the tree has this node, and it is no longer a seed
    CHECK(root.has_child("I am somebody"));
    CHECK(root["I am somebody"].val() == "indeed");


    //------------------------------------------------------------------
    // Emitting:

    // emit to a FILE*
    ryml::emit(tree, stdout);
    // emit to a stream
    std::stringstream ss;
    ss << tree;
    std::string stream_result = ss.str();
    // emit to a buffer:
    std::string str_result = ryml::emitrs<std::string>(tree);
    // can emit to any given buffer:
    char buf[1024];
    ryml::csubstr buf_result = ryml::emit(tree, buf);
    // now check
    ryml::csubstr expected_result = R"(foo: says who
bar:
  - 20
  - 30
  - oh so nice
  - oh so nice (serialized)
john: in_scope
newkeyval: shiny and new
newkeyval (serialized): shiny and new (serialized)
newseq: []
newseq (serialized): []
newmap: {}
newmap (serialized): []
I am somebody: indeed
)";
    CHECK(buf_result == expected_result);
    CHECK(str_result == expected_result);
    CHECK(stream_result == expected_result);
    // There are many possibilities to emit to buffer;
    // please look at the emit sample functions below.
}


//-----------------------------------------------------------------------------

/** demonstrate usage of ryml::substr/ryml::csubstr
 *
 * These types are imported from the c4core library into the ryml
 * namespace You may have noticed above the use of a `csubstr`
 * class. This class is defined in another library,
 * [c4core](https://github.com/biojppm/c4core), which is imported by
 * ryml. This is a library I use with my projects consisting of
 * multiplatform low-level utilities. One of these is `c4::csubstr`
 * (the name comes from "constant substring") which is a non-owning
 * read-only string view, with many methods that make it practical to
 * use (I would certainly argue more practical than `std::string`). In
 * fact, `c4::csubstr` and its writeable counterpart `c4::substr` are
 * the workhorses of the ryml parsing and serialization code.
 *
 * @see https://c4core.docsforge.com/master/api/c4/basic_substring/
 * @see https://c4core.docsforge.com/master/api/c4/#substr
 * @see https://c4core.docsforge.com/master/api/c4/#csubstr
 */
void sample_substr()
{
    // substr is a mutable view: pointer and length to a string in memory.
    // csubstr is a const-substr (immutable).

    // construct from explicit args
    {
        const char foobar_str[] = "foobar";
        auto s = ryml::csubstr(foobar_str, strlen(foobar_str));
        CHECK(s == "foobar");
        CHECK(s.size() == 6);
        CHECK(s.data() == foobar_str);
        CHECK(s.size() == s.len);
        CHECK(s.data() == s.str);
    }

    // construct from a string array
    {
        const char foobar_str[] = "foobar";
        ryml::csubstr s = foobar_str;
        CHECK(s == "foobar");
        CHECK(s != "foobar0");
        CHECK(s.size() == 6);
        CHECK(s.data() == foobar_str);
        CHECK(s.size() == s.len);
        CHECK(s.data() == s.str);
    }
    // you can also declare directly in-place from an array:
    {
        ryml::csubstr s = "foobar";
        CHECK(s == "foobar");
        CHECK(s != "foobar0");
        CHECK(s.size() == 6);
        CHECK(s.size() == s.len);
        CHECK(s.data() == s.str);
    }

    // construct from a C-string:
    //
    // Since the input is only a pointer, the string length can only
    // be found with a call to strlen(). To make this cost evident, we
    // require a call to to_csubstr():
    {
        const char *foobar_str = "foobar";
        ryml::csubstr s = ryml::to_csubstr(foobar_str);
        CHECK(s == "foobar");
        CHECK(s != "foobar0");
        CHECK(s.size() == 6);
        CHECK(s.size() == s.len);
        CHECK(s.data() == s.str);
    }

    // construct from a std::string: same approach as above.
    // requires inclusion of the <ryml/std/string.hpp> header
    // or of the umbrella header <ryml_std.hpp>.
    // this was a conscious design choice to avoid requiring
    // the heavy std:: allocation machinery
    {
        std::string foobar_str = "foobar";
        ryml::csubstr s = ryml::to_csubstr(foobar_str); // defined in <ryml/std/string.hpp>
        CHECK(s == "foobar");
        CHECK(s != "foobar0");
        CHECK(s.size() == 6);
        CHECK(s.size() == s.len);
        CHECK(s.data() == s.str);
    }

    // convert substr -> csubstr
    {
        char buf[] = "foo";
        ryml::substr foo = buf;
        CHECK(foo.len == 3);
        CHECK(foo.data() == buf);
        ryml::csubstr cfoo = foo;
        CHECK(cfoo.data() == buf);
    }
    // cannot convert csubstr -> substr:
    {
        // ryml::substr foo2 = cfoo; // compile error: cannot write to csubstr
    }

    // construct from char[]/const char[]: mutable vs immutable memory
    {
        char const foobar_str_ro[] = "foobar"; // ro := read-only
        char       foobar_str_rw[] = "foobar"; // rw := read-write
        static_assert(std::is_array<decltype(foobar_str_ro)>::value, "this is an array");
        static_assert(std::is_array<decltype(foobar_str_rw)>::value, "this is an array");
        // csubstr <- read-only memory
        {
            ryml::csubstr foobar = foobar_str_ro;
            CHECK(foobar.data() == foobar_str_ro);
            CHECK(foobar.size() == strlen(foobar_str_ro));
            CHECK(foobar == "foobar"); // AKA strcmp
        }
        // csubstr <- read-write memory: you can create an immutable csubstr from mutable memory
        {
            ryml::csubstr foobar = foobar_str_rw;
            CHECK(foobar.data() == foobar_str_rw);
            CHECK(foobar.size() == strlen(foobar_str_rw));
            CHECK(foobar == "foobar"); // AKA strcmp
        }
        // substr <- read-write memory.
        {
            ryml::substr foobar = foobar_str_rw;
            CHECK(foobar.data() == foobar_str_rw);
            CHECK(foobar.size() == strlen(foobar_str_rw));
            CHECK(foobar == "foobar"); // AKA strcmp
        }
        // substr <- ro is impossible.
        {
            //ryml::substr foobar = foobar_str_ro; // compile error!
        }
    }

    // construct from char*/const char*: mutable vs immutable memory.
    // use to_substr()/to_csubstr()
    {
        char const* foobar_str_ro = "foobar";       // ro := read-only
        char        foobar_str_rw_[] = "foobar";    // rw := read-write
        char      * foobar_str_rw = foobar_str_rw_; // rw := read-write
        static_assert(!std::is_array<decltype(foobar_str_ro)>::value, "this is a decayed pointer");
        static_assert(!std::is_array<decltype(foobar_str_rw)>::value, "this is a decayed pointer");
        // csubstr <- read-only memory
        {
            //ryml::csubstr foobar = foobar_str_ro; // compile error: length is not known
            ryml::csubstr foobar = ryml::to_csubstr(foobar_str_ro);
            CHECK(foobar.data() == foobar_str_ro);
            CHECK(foobar.size() == strlen(foobar_str_ro));
            CHECK(foobar == "foobar"); // AKA strcmp
        }
        // csubstr <- read-write memory: you can create an immutable csubstr from mutable memory
        {
            ryml::csubstr foobar = ryml::to_csubstr(foobar_str_rw);
            CHECK(foobar.data() == foobar_str_rw);
            CHECK(foobar.size() == strlen(foobar_str_rw));
            CHECK(foobar == "foobar"); // AKA strcmp
        }
        // substr <- read-write memory.
        {
            ryml::substr foobar = ryml::to_substr(foobar_str_rw);
            CHECK(foobar.data() == foobar_str_rw);
            CHECK(foobar.size() == strlen(foobar_str_rw));
            CHECK(foobar == "foobar"); // AKA strcmp
        }
        // substr <- read-only is impossible.
        {
            //ryml::substr foobar = ryml::to_substr(foobar_str_ro); // compile error!
        }
    }

    // substr is mutable, without changing the size:
    {
        char buf[] = "foobar";
        ryml::substr foobar = buf;
        CHECK(foobar == "foobar");
        foobar[0] = 'F';            CHECK(foobar == "Foobar");
        foobar.back() = 'R';        CHECK(foobar == "FoobaR");
        foobar.reverse();           CHECK(foobar == "RabooF");
        foobar.reverse();           CHECK(foobar == "FoobaR");
        foobar.reverse_sub(1, 4);   CHECK(foobar == "FabooR");
        foobar.reverse_sub(1, 4);   CHECK(foobar == "FoobaR");
        foobar.reverse_range(2, 5); CHECK(foobar == "FoaboR");
        foobar.reverse_range(2, 5); CHECK(foobar == "FoobaR");
        foobar.replace('o', '0');   CHECK(foobar == "F00baR");
        foobar.replace('a', '_');   CHECK(foobar == "F00b_R");
        foobar.replace("_0b", 'a'); CHECK(foobar == "FaaaaR");
        foobar.toupper();           CHECK(foobar == "FAAAAR");
        foobar.tolower();           CHECK(foobar == "faaaar");
        foobar.fill('.');           CHECK(foobar == "......");
        // see also:
        // - erase()
        // - replace_all()
    }

    // sub-views
    {
        ryml::csubstr s = "fooFOObarBAR";
        CHECK(s.len == 12u);
        // sub(): <- first,[num]
        CHECK(s.sub(0)     == "fooFOObarBAR");
        CHECK(s.sub(0, 12) == "fooFOObarBAR");
        CHECK(s.sub(0,  3) == "foo"         );
        CHECK(s.sub(3)     ==    "FOObarBAR");
        CHECK(s.sub(3,  3) ==    "FOO"      );
        CHECK(s.sub(6)     ==       "barBAR");
        CHECK(s.sub(6,  3) ==       "bar"   );
        CHECK(s.sub(9)     ==          "BAR");
        CHECK(s.sub(9,  3) ==          "BAR");
        // first(): <- length
        CHECK(s.first(0) == ""   );
        CHECK(s.first(1) == "f"  );
        CHECK(s.first(2) != "f"  );
        CHECK(s.first(2) == "fo" );
        CHECK(s.first(3) == "foo");
        // last(): <- length
        CHECK(s.last(0) ==    "");
        CHECK(s.last(1) ==   "R");
        CHECK(s.last(2) ==  "AR");
        CHECK(s.last(3) == "BAR");
        // range(): <- first, last
        CHECK(s.range(0, 12) == "fooFOObarBAR");
        CHECK(s.range(1, 12) ==  "ooFOObarBAR");
        CHECK(s.range(1, 11) ==  "ooFOObarBA" );
        CHECK(s.range(2, 10) ==   "oFOObarB"  );
        CHECK(s.range(3,  9) ==    "FOObar"   );
        // offs(): offset from beginning, end
        CHECK(s.offs(0, 0) == "fooFOObarBAR");
        CHECK(s.offs(1, 0) ==  "ooFOObarBAR");
        CHECK(s.offs(1, 1) ==  "ooFOObarBA" );
        CHECK(s.offs(2, 1) ==   "oFOObarBA" );
        CHECK(s.offs(2, 2) ==   "oFOObarB"  );
        CHECK(s.offs(3, 3) ==    "FOObar"  );
        // right_of(): <- pos, include_pos
        CHECK(s.right_of(0,  true) == "fooFOObarBAR");
        CHECK(s.right_of(0, false) ==  "ooFOObarBAR");
        CHECK(s.right_of(1,  true) ==  "ooFOObarBAR");
        CHECK(s.right_of(1, false) ==   "oFOObarBAR");
        CHECK(s.right_of(2,  true) ==   "oFOObarBAR");
        CHECK(s.right_of(2, false) ==    "FOObarBAR");
        CHECK(s.right_of(3,  true) ==    "FOObarBAR");
        CHECK(s.right_of(3, false) ==     "OObarBAR");
        // left_of() <- pos, include_pos
        CHECK(s.left_of(12, false) == "fooFOObarBAR");
        CHECK(s.left_of(11,  true) == "fooFOObarBAR");
        CHECK(s.left_of(11, false) == "fooFOObarBA" );
        CHECK(s.left_of(10,  true) == "fooFOObarBA" );
        CHECK(s.left_of(10, false) == "fooFOObarB"  );
        CHECK(s.left_of( 9,  true) == "fooFOObarB"  );
        CHECK(s.left_of( 9, false) == "fooFOObar"   );
        // left_of(),right_of() <- substr
        ryml::csubstr FOO = s.sub(3, 3);
        CHECK(s.is_super(FOO)); // required for the following
        CHECK(s.left_of(FOO) == "foo");
        CHECK(s.right_of(FOO) == "barBAR");
    }

    // is_sub(),is_super()
    {
        ryml::csubstr foobar = "foobar";
        ryml::csubstr foo = foobar.first(3);
        CHECK(foo.is_sub(foobar));
        CHECK(foo.is_sub(foo));
        CHECK(!foo.is_super(foobar));
        CHECK(!foobar.is_sub(foo));
        // identity comparison is true:
        CHECK(foo.is_super(foo));
        CHECK(foo.is_sub(foo));
        CHECK(foobar.is_sub(foobar));
        CHECK(foobar.is_super(foobar));
    }

    // overlaps()
    {
        ryml::csubstr foobar = "foobar";
        ryml::csubstr foo = foobar.first(3);
        ryml::csubstr oba = foobar.offs(2, 1);
        ryml::csubstr abc = "abc";
        CHECK(foobar.overlaps(foo));
        CHECK(foobar.overlaps(oba));
        CHECK(foo.overlaps(foobar));
        CHECK(foo.overlaps(oba));
        CHECK(!foo.overlaps(abc));
        CHECK(!abc.overlaps(foo));
    }

    // triml(): trim characters from the left
    // trimr(): trim characters from the right
    // trim(): trim characters from left AND right
    {
        CHECK(ryml::csubstr(" \t\n\rcontents without whitespace\t \n\r").trim("\t \n\r") == "contents without whitespace");
        ryml::csubstr aaabbb = "aaabbb";
        ryml::csubstr aaa___bbb = "aaa___bbb";
        // trim a character:
        CHECK(aaabbb.triml('a') == aaabbb.last(3)); // bbb
        CHECK(aaabbb.trimr('a') == aaabbb);
        CHECK(aaabbb.trim ('a') == aaabbb.last(3)); // bbb
        CHECK(aaabbb.triml('b') == aaabbb);
        CHECK(aaabbb.trimr('b') == aaabbb.first(3)); // aaa
        CHECK(aaabbb.trim ('b') == aaabbb.first(3)); // aaa
        CHECK(aaabbb.triml('c') == aaabbb);
        CHECK(aaabbb.trimr('c') == aaabbb);
        CHECK(aaabbb.trim ('c') == aaabbb);
        CHECK(aaa___bbb.triml('a') == aaa___bbb.last(6)); // ___bbb
        CHECK(aaa___bbb.trimr('a') == aaa___bbb);
        CHECK(aaa___bbb.trim ('a') == aaa___bbb.last(6)); // ___bbb
        CHECK(aaa___bbb.triml('b') == aaa___bbb);
        CHECK(aaa___bbb.trimr('b') == aaa___bbb.first(6)); // aaa___
        CHECK(aaa___bbb.trim ('b') == aaa___bbb.first(6)); // aaa___
        CHECK(aaa___bbb.triml('c') == aaa___bbb);
        CHECK(aaa___bbb.trimr('c') == aaa___bbb);
        CHECK(aaa___bbb.trim ('c') == aaa___bbb);
        // trim ANY of the characters:
        CHECK(aaabbb.triml("ab") == "");
        CHECK(aaabbb.trimr("ab") == "");
        CHECK(aaabbb.trim ("ab") == "");
        CHECK(aaabbb.triml("ba") == "");
        CHECK(aaabbb.trimr("ba") == "");
        CHECK(aaabbb.trim ("ba") == "");
        CHECK(aaabbb.triml("cd") == aaabbb);
        CHECK(aaabbb.trimr("cd") == aaabbb);
        CHECK(aaabbb.trim ("cd") == aaabbb);
        CHECK(aaa___bbb.triml("ab") == aaa___bbb.last(6)); // ___bbb
        CHECK(aaa___bbb.triml("ba") == aaa___bbb.last(6)); // ___bbb
        CHECK(aaa___bbb.triml("cd") == aaa___bbb);
        CHECK(aaa___bbb.trimr("ab") == aaa___bbb.first(6)); // aaa___
        CHECK(aaa___bbb.trimr("ba") == aaa___bbb.first(6)); // aaa___
        CHECK(aaa___bbb.trimr("cd") == aaa___bbb);
        CHECK(aaa___bbb.trim ("ab") == aaa___bbb.range(3, 6)); // ___
        CHECK(aaa___bbb.trim ("ba") == aaa___bbb.range(3, 6)); // ___
        CHECK(aaa___bbb.trim ("cd") == aaa___bbb);
    }

    // unquoted():
    {
        CHECK(ryml::csubstr(R"('this is is single quoted')").unquoted() == "this is is single quoted");
        CHECK(ryml::csubstr(R"("this is is double quoted")").unquoted() == "this is is double quoted");
    }

    // stripl(): remove pattern from the left
    // stripr(): remove pattern from the right
    {
        ryml::csubstr abc___cba = "abc___cba";
        ryml::csubstr abc___abc = "abc___abc";
        CHECK(abc___cba.stripl("abc") == abc___cba.last(6)); // ___cba
        CHECK(abc___cba.stripr("abc") == abc___cba);
        CHECK(abc___cba.stripl("ab") == abc___cba.last(7)); // c___cba
        CHECK(abc___cba.stripr("ab") == abc___cba);
        CHECK(abc___cba.stripl("a") == abc___cba.last(8)); // bc___cba, same as triml('a')
        CHECK(abc___cba.stripr("a") == abc___cba.first(8));
        CHECK(abc___abc.stripl("abc") == abc___abc.last(6)); // ___abc
        CHECK(abc___abc.stripr("abc") == abc___abc.first(6)); // abc___
        CHECK(abc___abc.stripl("ab") == abc___abc.last(7)); // c___cba
        CHECK(abc___abc.stripr("ab") == abc___abc);
        CHECK(abc___abc.stripl("a") == abc___abc.last(8)); // bc___cba, same as triml('a')
        CHECK(abc___abc.stripr("a") == abc___abc);
    }

    // begins_with()/ends_with()
    // begins_with_any()/ends_with_any()
    {
        ryml::csubstr s = "foobar123";
        // char overloads
        CHECK(s.begins_with('f'));
        CHECK(s.ends_with('3'));
        CHECK(!s.ends_with('2'));
        CHECK(!s.ends_with('o'));
        // char[] overloads
        CHECK(s.begins_with("foobar"));
        CHECK(s.begins_with("foo"));
        CHECK(s.begins_with_any("foo"));
        CHECK(!s.begins_with("oof"));
        CHECK(s.begins_with_any("oof"));
        CHECK(s.ends_with("23"));
        CHECK(s.ends_with("123"));
        CHECK(s.ends_with_any("123"));
        CHECK(!s.ends_with("321"));
        CHECK(s.ends_with_any("231"));
    }

    // select()
    {
        ryml::csubstr s = "0123456789";
        CHECK(s.select('0') == s.sub(0, 1));
        CHECK(s.select('1') == s.sub(1, 1));
        CHECK(s.select('2') == s.sub(2, 1));
        CHECK(s.select('8') == s.sub(8, 1));
        CHECK(s.select('9') == s.sub(9, 1));
        CHECK(s.select("0123") == s.range(0, 4));
        CHECK(s.select("012" ) == s.range(0, 3));
        CHECK(s.select("01"  ) == s.range(0, 2));
        CHECK(s.select("0"   ) == s.range(0, 1));
        CHECK(s.select( "123") == s.range(1, 4));
        CHECK(s.select(  "23") == s.range(2, 4));
        CHECK(s.select(   "3") == s.range(3, 4));
    }

    // find()
    {
        ryml::csubstr s012345 = "012345";
        // find single characters:
        CHECK(s012345.find('a') == ryml::npos);
        CHECK(s012345.find('0'    ) == 0u);
        CHECK(s012345.find('0', 1u) == ryml::npos);
        CHECK(s012345.find('1'    ) == 1u);
        CHECK(s012345.find('1', 2u) == ryml::npos);
        CHECK(s012345.find('2'    ) == 2u);
        CHECK(s012345.find('2', 3u) == ryml::npos);
        CHECK(s012345.find('3'    ) == 3u);
        CHECK(s012345.find('3', 4u) == ryml::npos);
        // find patterns
        CHECK(s012345.find("ab"    ) == ryml::npos);
        CHECK(s012345.find("01"    ) == 0u);
        CHECK(s012345.find("01", 1u) == ryml::npos);
        CHECK(s012345.find("12"    ) == 1u);
        CHECK(s012345.find("12", 2u) == ryml::npos);
        CHECK(s012345.find("23"    ) == 2u);
        CHECK(s012345.find("23", 3u) == ryml::npos);
    }

    // count(): count the number of occurrences of a character
    {
        ryml::csubstr buf = "00110022003300440055";
        CHECK(buf.count('1'     ) ==  2u);
        CHECK(buf.count('1',  0u) ==  2u);
        CHECK(buf.count('1',  1u) ==  2u);
        CHECK(buf.count('1',  2u) ==  2u);
        CHECK(buf.count('1',  3u) ==  1u);
        CHECK(buf.count('1',  4u) ==  0u);
        CHECK(buf.count('1',  5u) ==  0u);
        CHECK(buf.count('0'     ) == 10u);
        CHECK(buf.count('0',  0u) == 10u);
        CHECK(buf.count('0',  1u) ==  9u);
        CHECK(buf.count('0',  2u) ==  8u);
        CHECK(buf.count('0',  3u) ==  8u);
        CHECK(buf.count('0',  4u) ==  8u);
        CHECK(buf.count('0',  5u) ==  7u);
        CHECK(buf.count('0',  6u) ==  6u);
        CHECK(buf.count('0',  7u) ==  6u);
        CHECK(buf.count('0',  8u) ==  6u);
        CHECK(buf.count('0',  9u) ==  5u);
        CHECK(buf.count('0', 10u) ==  4u);
        CHECK(buf.count('0', 11u) ==  4u);
        CHECK(buf.count('0', 12u) ==  4u);
        CHECK(buf.count('0', 13u) ==  3u);
        CHECK(buf.count('0', 14u) ==  2u);
        CHECK(buf.count('0', 15u) ==  2u);
        CHECK(buf.count('0', 16u) ==  2u);
        CHECK(buf.count('0', 17u) ==  1u);
        CHECK(buf.count('0', 18u) ==  0u);
        CHECK(buf.count('0', 19u) ==  0u);
        CHECK(buf.count('0', 20u) ==  0u);
    }

    // first_of(),last_of()
    {
        ryml::csubstr s012345 = "012345";
        CHECK(s012345.first_of('a') == ryml::npos);
        CHECK(s012345.first_of("ab") == ryml::npos);
        CHECK(s012345.first_of('0') == 0u);
        CHECK(s012345.first_of("0") == 0u);
        CHECK(s012345.first_of("01") == 0u);
        CHECK(s012345.first_of("10") == 0u);
        CHECK(s012345.first_of("012") == 0u);
        CHECK(s012345.first_of("210") == 0u);
        CHECK(s012345.first_of("0123") == 0u);
        CHECK(s012345.first_of("3210") == 0u);
        CHECK(s012345.first_of("01234") == 0u);
        CHECK(s012345.first_of("43210") == 0u);
        CHECK(s012345.first_of("012345") == 0u);
        CHECK(s012345.first_of("543210") == 0u);
        CHECK(s012345.first_of('5') == 5u);
        CHECK(s012345.first_of("5") == 5u);
        CHECK(s012345.first_of("45") == 4u);
        CHECK(s012345.first_of("54") == 4u);
        CHECK(s012345.first_of("345") == 3u);
        CHECK(s012345.first_of("543") == 3u);
        CHECK(s012345.first_of("2345") == 2u);
        CHECK(s012345.first_of("5432") == 2u);
        CHECK(s012345.first_of("12345") == 1u);
        CHECK(s012345.first_of("54321") == 1u);
        CHECK(s012345.first_of("012345") == 0u);
        CHECK(s012345.first_of("543210") == 0u);
        CHECK(s012345.first_of('0', 6u) == ryml::npos);
        CHECK(s012345.first_of('5', 6u) == ryml::npos);
        CHECK(s012345.first_of("012345", 6u) == ryml::npos);
        //
        CHECK(s012345.last_of('a') == ryml::npos);
        CHECK(s012345.last_of("ab") == ryml::npos);
        CHECK(s012345.last_of('0') == 0u);
        CHECK(s012345.last_of("0") == 0u);
        CHECK(s012345.last_of("01") == 1u);
        CHECK(s012345.last_of("10") == 1u);
        CHECK(s012345.last_of("012") == 2u);
        CHECK(s012345.last_of("210") == 2u);
        CHECK(s012345.last_of("0123") == 3u);
        CHECK(s012345.last_of("3210") == 3u);
        CHECK(s012345.last_of("01234") == 4u);
        CHECK(s012345.last_of("43210") == 4u);
        CHECK(s012345.last_of("012345") == 5u);
        CHECK(s012345.last_of("543210") == 5u);
        CHECK(s012345.last_of('5') == 5u);
        CHECK(s012345.last_of("5") == 5u);
        CHECK(s012345.last_of("45") == 5u);
        CHECK(s012345.last_of("54") == 5u);
        CHECK(s012345.last_of("345") == 5u);
        CHECK(s012345.last_of("543") == 5u);
        CHECK(s012345.last_of("2345") == 5u);
        CHECK(s012345.last_of("5432") == 5u);
        CHECK(s012345.last_of("12345") == 5u);
        CHECK(s012345.last_of("54321") == 5u);
        CHECK(s012345.last_of("012345") == 5u);
        CHECK(s012345.last_of("543210") == 5u);
        CHECK(s012345.last_of('0', 6u) == 0u);
        CHECK(s012345.last_of('5', 6u) == 5u);
        CHECK(s012345.last_of("012345", 6u) == 5u);
    }

    // first_not_of(), last_not_of()
    {
        ryml::csubstr s012345 = "012345";
        CHECK(s012345.first_not_of('a') == 0u);
        CHECK(s012345.first_not_of("ab") == 0u);
        CHECK(s012345.first_not_of('0') == 1u);
        CHECK(s012345.first_not_of("0") == 1u);
        CHECK(s012345.first_not_of("01") == 2u);
        CHECK(s012345.first_not_of("10") == 2u);
        CHECK(s012345.first_not_of("012") == 3u);
        CHECK(s012345.first_not_of("210") == 3u);
        CHECK(s012345.first_not_of("0123") == 4u);
        CHECK(s012345.first_not_of("3210") == 4u);
        CHECK(s012345.first_not_of("01234") == 5u);
        CHECK(s012345.first_not_of("43210") == 5u);
        CHECK(s012345.first_not_of("012345") == ryml::npos);
        CHECK(s012345.first_not_of("543210") == ryml::npos);
        CHECK(s012345.first_not_of('5') == 0u);
        CHECK(s012345.first_not_of("5") == 0u);
        CHECK(s012345.first_not_of("45") == 0u);
        CHECK(s012345.first_not_of("54") == 0u);
        CHECK(s012345.first_not_of("345") == 0u);
        CHECK(s012345.first_not_of("543") == 0u);
        CHECK(s012345.first_not_of("2345") == 0u);
        CHECK(s012345.first_not_of("5432") == 0u);
        CHECK(s012345.first_not_of("12345") == 0u);
        CHECK(s012345.first_not_of("54321") == 0u);
        CHECK(s012345.first_not_of("012345") == ryml::npos);
        CHECK(s012345.first_not_of("543210") == ryml::npos);
        CHECK(s012345.last_not_of('a') == 5u);
        CHECK(s012345.last_not_of("ab") == 5u);
        CHECK(s012345.last_not_of('5') == 4u);
        CHECK(s012345.last_not_of("5") == 4u);
        CHECK(s012345.last_not_of("45") == 3u);
        CHECK(s012345.last_not_of("54") == 3u);
        CHECK(s012345.last_not_of("345") == 2u);
        CHECK(s012345.last_not_of("543") == 2u);
        CHECK(s012345.last_not_of("2345") == 1u);
        CHECK(s012345.last_not_of("5432") == 1u);
        CHECK(s012345.last_not_of("12345") == 0u);
        CHECK(s012345.last_not_of("54321") == 0u);
        CHECK(s012345.last_not_of("012345") == ryml::npos);
        CHECK(s012345.last_not_of("543210") == ryml::npos);
        CHECK(s012345.last_not_of('0') == 5u);
        CHECK(s012345.last_not_of("0") == 5u);
        CHECK(s012345.last_not_of("01") == 5u);
        CHECK(s012345.last_not_of("10") == 5u);
        CHECK(s012345.last_not_of("012") == 5u);
        CHECK(s012345.last_not_of("210") == 5u);
        CHECK(s012345.last_not_of("0123") == 5u);
        CHECK(s012345.last_not_of("3210") == 5u);
        CHECK(s012345.last_not_of("01234") == 5u);
        CHECK(s012345.last_not_of("43210") == 5u);
        CHECK(s012345.last_not_of("012345") == ryml::npos);
        CHECK(s012345.last_not_of("543210") == ryml::npos);
    }

    // first_non_empty_span()
    {
        CHECK(ryml::csubstr("foo bar").first_non_empty_span() == "foo");
        CHECK(ryml::csubstr("       foo bar").first_non_empty_span() == "foo");
        CHECK(ryml::csubstr("\n   \r  \t  foo bar").first_non_empty_span() == "foo");
        CHECK(ryml::csubstr("\n   \r  \t  foo\n\r\t bar").first_non_empty_span() == "foo");
        CHECK(ryml::csubstr("\n   \r  \t  foo\n\r\t bar").first_non_empty_span() == "foo");
        CHECK(ryml::csubstr(",\n   \r  \t  foo\n\r\t bar").first_non_empty_span() == ",");
    }
    // first_uint_span()
    {
        CHECK(ryml::csubstr("1234 asdkjh").first_uint_span() == "1234");
        CHECK(ryml::csubstr("1234\rasdkjh").first_uint_span() == "1234");
        CHECK(ryml::csubstr("1234\tasdkjh").first_uint_span() == "1234");
        CHECK(ryml::csubstr("1234\nasdkjh").first_uint_span() == "1234");
        CHECK(ryml::csubstr("1234]asdkjh").first_uint_span() == "1234");
        CHECK(ryml::csubstr("1234)asdkjh").first_uint_span() == "1234");
        CHECK(ryml::csubstr("1234gasdkjh").first_uint_span() == "");
    }
    // first_int_span()
    {
        CHECK(ryml::csubstr("-1234 asdkjh").first_int_span() == "-1234");
        CHECK(ryml::csubstr("-1234\rasdkjh").first_int_span() == "-1234");
        CHECK(ryml::csubstr("-1234\tasdkjh").first_int_span() == "-1234");
        CHECK(ryml::csubstr("-1234\nasdkjh").first_int_span() == "-1234");
        CHECK(ryml::csubstr("-1234]asdkjh").first_int_span() == "-1234");
        CHECK(ryml::csubstr("-1234)asdkjh").first_int_span() == "-1234");
        CHECK(ryml::csubstr("-1234gasdkjh").first_int_span() == "");
    }
    // first_real_span()
    {
        CHECK(ryml::csubstr("-1234 asdkjh").first_real_span() == "-1234");
        CHECK(ryml::csubstr("-1234\rasdkjh").first_real_span() == "-1234");
        CHECK(ryml::csubstr("-1234\tasdkjh").first_real_span() == "-1234");
        CHECK(ryml::csubstr("-1234\nasdkjh").first_real_span() == "-1234");
        CHECK(ryml::csubstr("-1234]asdkjh").first_real_span() == "-1234");
        CHECK(ryml::csubstr("-1234)asdkjh").first_real_span() == "-1234");
        CHECK(ryml::csubstr("-1234gasdkjh").first_real_span() == "");
        CHECK(ryml::csubstr("1.234 asdkjh").first_real_span() == "1.234");
        CHECK(ryml::csubstr("1.234e5 asdkjh").first_real_span() == "1.234e5");
        CHECK(ryml::csubstr("1.234e+5 asdkjh").first_real_span() == "1.234e+5");
        CHECK(ryml::csubstr("1.234e-5 asdkjh").first_real_span() == "1.234e-5");
        CHECK(ryml::csubstr("1.234 asdkjh").first_real_span() == "1.234");
        CHECK(ryml::csubstr("1.234e5 asdkjh").first_real_span() == "1.234e5");
        CHECK(ryml::csubstr("1.234e+5 asdkjh").first_real_span() == "1.234e+5");
        CHECK(ryml::csubstr("1.234e-5 asdkjh").first_real_span() == "1.234e-5");
        CHECK(ryml::csubstr("-1.234 asdkjh").first_real_span() == "-1.234");
        CHECK(ryml::csubstr("-1.234e5 asdkjh").first_real_span() == "-1.234e5");
        CHECK(ryml::csubstr("-1.234e+5 asdkjh").first_real_span() == "-1.234e+5");
        CHECK(ryml::csubstr("-1.234e-5 asdkjh").first_real_span() == "-1.234e-5");
        CHECK(ryml::csubstr("0x1.e8480p+19 asdkjh").first_real_span() == "0x1.e8480p+19");
        CHECK(ryml::csubstr("0x1.e8480p-19 asdkjh").first_real_span() == "0x1.e8480p-19");
        CHECK(ryml::csubstr("-0x1.e8480p+19 asdkjh").first_real_span() == "-0x1.e8480p+19");
        CHECK(ryml::csubstr("-0x1.e8480p-19 asdkjh").first_real_span() == "-0x1.e8480p-19");
        CHECK(ryml::csubstr("+0x1.e8480p+19 asdkjh").first_real_span() == "+0x1.e8480p+19");
        CHECK(ryml::csubstr("+0x1.e8480p-19 asdkjh").first_real_span() == "+0x1.e8480p-19");
    }
    // see also is_number()

    // basename(), dirname(), extshort(), extlong()
    {
        CHECK(ryml::csubstr("/path/to/file.tar.gz").basename() == "file.tar.gz");
        CHECK(ryml::csubstr("/path/to/file.tar.gz").dirname() == "/path/to/");
        CHECK(ryml::csubstr("C:\\path\\to\\file.tar.gz").basename('\\') == "file.tar.gz");
        CHECK(ryml::csubstr("C:\\path\\to\\file.tar.gz").dirname('\\') == "C:\\path\\to\\");
        CHECK(ryml::csubstr("/path/to/file.tar.gz").extshort() == "gz");
        CHECK(ryml::csubstr("/path/to/file.tar.gz").extlong() == "tar.gz");
        CHECK(ryml::csubstr("/path/to/file.tar.gz").name_wo_extshort() == "/path/to/file.tar");
        CHECK(ryml::csubstr("/path/to/file.tar.gz").name_wo_extlong() == "/path/to/file");
    }

    // split()
    {
        using namespace ryml;
        csubstr parts[] = {"aa", "bb", "cc", "dd", "ee", "ff"};
        {
            size_t count = 0;
            for(csubstr part : csubstr("aa/bb/cc/dd/ee/ff").split('/'))
                CHECK(part == parts[count++]);
        }
        {
            size_t count = 0;
            for(csubstr part : csubstr("aa.bb.cc.dd.ee.ff").split('.'))
                CHECK(part == parts[count++]);
        }
        {
            size_t count = 0;
            for(csubstr part : csubstr("aa-bb-cc-dd-ee-ff").split('-'))
                CHECK(part == parts[count++]);
        }
        // see also next_split()
    }

    //  pop_left(),  pop_right() --- non-greedy version
    // gpop_left(), gpop_right() --- greedy version
    {
        const bool skip_empty = true;
        // pop_left(): pop the last element from the left
        CHECK(ryml::csubstr(  "0/1/2"   ). pop_left('/'            ) ==   "0"    );
        CHECK(ryml::csubstr( "/0/1/2"   ). pop_left('/'            ) == ""       );
        CHECK(ryml::csubstr("//0/1/2"   ). pop_left('/'            ) == ""       );
        CHECK(ryml::csubstr(  "0/1/2"   ). pop_left('/', skip_empty) ==   "0"    );
        CHECK(ryml::csubstr( "/0/1/2"   ). pop_left('/', skip_empty) ==  "/0"    );
        CHECK(ryml::csubstr("//0/1/2"   ). pop_left('/', skip_empty) == "//0"    );
        // gpop_left(): pop all but the first element (greedy pop)
        CHECK(ryml::csubstr(  "0/1/2"   ).gpop_left('/'            ) ==   "0/1"  );
        CHECK(ryml::csubstr( "/0/1/2"   ).gpop_left('/'            ) ==  "/0/1"  );
        CHECK(ryml::csubstr("//0/1/2"   ).gpop_left('/'            ) == "//0/1"  );
        CHECK(ryml::csubstr(  "0/1/2/"  ).gpop_left('/'            ) ==   "0/1/2");
        CHECK(ryml::csubstr( "/0/1/2/"  ).gpop_left('/'            ) ==  "/0/1/2");
        CHECK(ryml::csubstr("//0/1/2/"  ).gpop_left('/'            ) == "//0/1/2");
        CHECK(ryml::csubstr(  "0/1/2//" ).gpop_left('/'            ) ==   "0/1/2/");
        CHECK(ryml::csubstr( "/0/1/2//" ).gpop_left('/'            ) ==  "/0/1/2/");
        CHECK(ryml::csubstr("//0/1/2//" ).gpop_left('/'            ) == "//0/1/2/");
        CHECK(ryml::csubstr(  "0/1/2"   ).gpop_left('/', skip_empty) ==   "0/1"  );
        CHECK(ryml::csubstr( "/0/1/2"   ).gpop_left('/', skip_empty) ==  "/0/1"  );
        CHECK(ryml::csubstr("//0/1/2"   ).gpop_left('/', skip_empty) == "//0/1"  );
        CHECK(ryml::csubstr(  "0/1/2/"  ).gpop_left('/', skip_empty) ==   "0/1"  );
        CHECK(ryml::csubstr( "/0/1/2/"  ).gpop_left('/', skip_empty) ==  "/0/1"  );
        CHECK(ryml::csubstr("//0/1/2/"  ).gpop_left('/', skip_empty) == "//0/1"  );
        CHECK(ryml::csubstr(  "0/1/2//" ).gpop_left('/', skip_empty) ==   "0/1"  );
        CHECK(ryml::csubstr( "/0/1/2//" ).gpop_left('/', skip_empty) ==  "/0/1"  );
        CHECK(ryml::csubstr("//0/1/2//" ).gpop_left('/', skip_empty) == "//0/1"  );
        // pop_right(): pop the last element from the right
        CHECK(ryml::csubstr(  "0/1/2"   ). pop_right('/'            ) ==   "2"    );
        CHECK(ryml::csubstr(  "0/1/2/"  ). pop_right('/'            ) == ""       );
        CHECK(ryml::csubstr(  "0/1/2//" ). pop_right('/'            ) == ""       );
        CHECK(ryml::csubstr(  "0/1/2"   ). pop_right('/', skip_empty) ==   "2"    );
        CHECK(ryml::csubstr(  "0/1/2/"  ). pop_right('/', skip_empty) ==   "2/"   );
        CHECK(ryml::csubstr(  "0/1/2//" ). pop_right('/', skip_empty) ==   "2//"  );
        // gpop_right(): pop all but the first element (greedy pop)
        CHECK(ryml::csubstr(  "0/1/2"   ).gpop_right('/'            ) ==     "1/2");
        CHECK(ryml::csubstr(  "0/1/2/"  ).gpop_right('/'            ) ==     "1/2/"  );
        CHECK(ryml::csubstr(  "0/1/2//" ).gpop_right('/'            ) ==     "1/2//"  );
        CHECK(ryml::csubstr( "/0/1/2"   ).gpop_right('/'            ) ==   "0/1/2");
        CHECK(ryml::csubstr( "/0/1/2/"  ).gpop_right('/'            ) ==   "0/1/2/"  );
        CHECK(ryml::csubstr( "/0/1/2//" ).gpop_right('/'            ) ==   "0/1/2//"  );
        CHECK(ryml::csubstr("//0/1/2"   ).gpop_right('/'            ) ==  "/0/1/2");
        CHECK(ryml::csubstr("//0/1/2/"  ).gpop_right('/'            ) ==  "/0/1/2/"  );
        CHECK(ryml::csubstr("//0/1/2//" ).gpop_right('/'            ) ==  "/0/1/2//"  );
        CHECK(ryml::csubstr(  "0/1/2"   ).gpop_right('/', skip_empty) ==     "1/2");
        CHECK(ryml::csubstr(  "0/1/2/"  ).gpop_right('/', skip_empty) ==     "1/2/"  );
        CHECK(ryml::csubstr(  "0/1/2//" ).gpop_right('/', skip_empty) ==     "1/2//"  );
        CHECK(ryml::csubstr( "/0/1/2"   ).gpop_right('/', skip_empty) ==     "1/2");
        CHECK(ryml::csubstr( "/0/1/2/"  ).gpop_right('/', skip_empty) ==     "1/2/"  );
        CHECK(ryml::csubstr( "/0/1/2//" ).gpop_right('/', skip_empty) ==     "1/2//"  );
        CHECK(ryml::csubstr("//0/1/2"   ).gpop_right('/', skip_empty) ==     "1/2");
        CHECK(ryml::csubstr("//0/1/2/"  ).gpop_right('/', skip_empty) ==     "1/2/"  );
        CHECK(ryml::csubstr("//0/1/2//" ).gpop_right('/', skip_empty) ==     "1/2//"  );
    }

    // see the docs:
    // https://c4core.docsforge.com/master/api/c4/basic_substring/
}


//-----------------------------------------------------------------------------

// helper functions for sample_parse_file()
template<class CharContainer> CharContainer file_get_contents(const char *filename);
template<class CharContainer> size_t        file_get_contents(const char *filename, CharContainer *v);
template<class CharContainer> void          file_put_contents(const char *filename, CharContainer const& v, const char* access="wb");
void                                        file_put_contents(const char *filename, const char *buf, size_t sz, const char* access);


/** demonstrate how to load a YAML file from disk to parse with ryml.
 *
 *  ryml offers no overload to directly parse files from disk; it only
 *  parses source buffers (which may be mutable or immutable). It is
 *  up to the caller to load the file contents into a buffer before
 *  parsing with ryml.
 *
 *  But that does not mean that loading a file is unimportant. There
 *  are many ways to achieve this in C++, but for convenience and to
 *  enable you to quickly get up to speed, here is an example
 *  implementation loading a file from disk and then parsing the
 *  resulting buffer with ryml. */
void sample_parse_file()
{
    const char filename[] = "ryml_example.yml";

    // because this is a minimal sample, it assumes nothing on the
    // environment/OS (other than that it can read/write files). So we
    // create the file on the fly:
    file_put_contents(filename, ryml::csubstr("foo: 1\nbar:\n  - 2\n  - 3\n"));

    // now we can load it into a std::string (for example):
    {
        std::string contents = file_get_contents<std::string>(filename);
        ryml::Tree tree = ryml::parse(ryml::to_csubstr(contents)); // immutable (csubstr) overload
        CHECK(tree["foo"].val() == "1");
        CHECK(tree["bar"][0].val() == "2");
        CHECK(tree["bar"][1].val() == "3");
    }

    // or we can use a vector<char> instead:
    {
        std::vector<char> contents = file_get_contents<std::vector<char>>(filename);
        ryml::Tree tree = ryml::parse(ryml::to_substr(contents)); // mutable (csubstr) overload
        CHECK(tree["foo"].val() == "1");
        CHECK(tree["bar"][0].val() == "2");
        CHECK(tree["bar"][1].val() == "3");
    }

    // generally, any contiguous char container can be used with ryml,
    // provided that the ryml::csubstr view can be created out of it.
    // ryml provides the overloads above for these two containers,
    // but if you have a different container it should be very easy
    // (only requires pointer and length).
}


//-----------------------------------------------------------------------------

/** demonstrate parsing of a read-only YAML source buffer */
void sample_parse_read_only()
{
    // to parse read-only memory, ryml will copy first to the tree's arena.
    // but usage is similar to in-situ
    ryml::Tree tree = ryml::parse("{foo: 1, bar: [2, 3]}");
    ryml::NodeRef root = tree.rootref(); // get a reference to the root

    CHECK(root.is_map());
    CHECK(root["foo"].is_keyval());
    CHECK(root["foo"].key() == "foo");
    CHECK(root["foo"].val() == "1");
    CHECK(root["bar"].is_seq());
    CHECK(root["bar"].has_key());
    CHECK(root["bar"].key() == "bar");
    CHECK(root["bar"][0].val() == "2");
    CHECK(root["bar"][1].val() == "3");

    // deserializing:
    int foo = 0, bar0 = 0, bar1 = 0;
    root["foo"] >> foo;
    root["bar"][0] >> bar0;
    root["bar"][1] >> bar1;
    CHECK(foo == 1);
    CHECK(bar0 == 2);
    CHECK(bar1 == 3);
}


//-----------------------------------------------------------------------------

/** demonstrate in-place parsing of a mutable YAML source buffer */
void sample_parse_in_situ()
{
    char src[] = "{foo: 1, bar: [2, 3]}"; // ryml can parse in situ
    ryml::substr srcview = src; // a mutable view to the source buffer
    ryml::Tree tree = ryml::parse(srcview); // you can also reuse the tree and/or parser
    ryml::NodeRef root = tree.rootref(); // get a reference to the root

    CHECK(root.is_map());
    CHECK(root["foo"].is_keyval());
    CHECK(root["foo"].key() == "foo");
    CHECK(root["foo"].val() == "1");
    CHECK(root["bar"].is_seq());
    CHECK(root["bar"].has_key());
    CHECK(root["bar"].key() == "bar");
    CHECK(root["bar"][0].val() == "2");
    CHECK(root["bar"][1].val() == "3");

    // deserializing:
    int foo = 0, bar0 = 0, bar1 = 0;
    root["foo"] >> foo;
    root["bar"][0] >> bar0;
    root["bar"][1] >> bar1;
    CHECK(foo == 1);
    CHECK(bar0 == 2);
    CHECK(bar1 == 3);

    // after parsing, the tree holds views to the source buffer:
    CHECK(root["foo"].val().data()     == src + strlen("{foo: "));
    CHECK(root["foo"].val().begin()    == src + strlen("{foo: "));
    CHECK(root["foo"].val().end()      == src + strlen("{foo: 1"));
    CHECK(root["foo"].val().is_sub(srcview)); // equivalent to the previous three assertions
    CHECK(root["bar"][0].val().data()  == src + strlen("{foo: 1, bar: ["));
    CHECK(root["bar"][0].val().begin() == src + strlen("{foo: 1, bar: ["));
    CHECK(root["bar"][0].val().end()   == src + strlen("{foo: 1, bar: [2"));
    CHECK(root["bar"][0].val().is_sub(srcview)); // equivalent to the previous three assertions
    CHECK(root["bar"][1].val().data()  == src + strlen("{foo: 1, bar: [2, "));
    CHECK(root["bar"][1].val().begin() == src + strlen("{foo: 1, bar: [2, "));
    CHECK(root["bar"][1].val().end()   == src + strlen("{foo: 1, bar: [2, 3"));
    CHECK(root["bar"][1].val().is_sub(srcview)); // equivalent to the previous three assertions
}


//-----------------------------------------------------------------------------

/** demonstrate reuse/modification of tree when parsing */
void sample_parse_reuse_tree()
{
    ryml::Tree tree;

    // it will always be faster if the tree's size is conveniently reserved:
    tree.reserve(30); // reserve 30 nodes (good enough for this sample)
    // if you are using the tree's arena to serialize data,
    // then reserve also the arena's size:
    tree.reserve(256); // reserve 256 characters (good enough for this sample)

    // now parse into the tree:
    ryml::parse("{foo: 1, bar: [2, 3]}", &tree);

    ryml::NodeRef root = tree.rootref();
    CHECK(root.num_children() == 2);
    CHECK(root.is_map());
    CHECK(root["foo"].is_keyval());
    CHECK(root["foo"].key() == "foo");
    CHECK(root["foo"].val() == "1");
    CHECK(root["bar"].is_seq());
    CHECK(root["bar"].has_key());
    CHECK(root["bar"].key() == "bar");
    CHECK(root["bar"][0].val() == "2");
    CHECK(root["bar"][1].val() == "3");
    CHECK(ryml::emitrs<std::string>(tree) == R"(foo: 1
bar:
  - 2
  - 3
)");

    // WATCHOUT: parsing into an existing tree will APPEND to it:
    ryml::parse("{foo2: 12, bar2: [22, 32]}", &tree);
    CHECK(ryml::emitrs<std::string>(tree) == R"(foo: 1
bar:
  - 2
  - 3
foo2: 12
bar2:
  - 22
  - 32
)");
    CHECK(root.num_children() == 4);
    CHECK(root["foo2"].is_keyval());
    CHECK(root["foo2"].key() == "foo2");
    CHECK(root["foo2"].val() == "12");
    CHECK(root["bar2"].is_seq());
    CHECK(root["bar2"].has_key());
    CHECK(root["bar2"].key() == "bar2");
    CHECK(root["bar2"][0].val() == "22");
    CHECK(root["bar2"][1].val() == "32");

    // clear first before parsing into an existing tree.
    tree.clear();
    ryml::parse("[a, b, {x0: 1, x1: 2}]", &tree);
    CHECK(ryml::emitrs<std::string>(tree) == R"(- a
- b
- x0: 1
  x1: 2
)");
    CHECK(root.is_seq());
    CHECK(root[0].val() == "a");
    CHECK(root[1].val() == "b");
    CHECK(root[2].is_map());
    CHECK(root[2]["x0"].val() == "1");
    CHECK(root[2]["x1"].val() == "2");

    // we can parse directly into a node nested deep in an existing tree:
    ryml::parse("{champagne: Dom Perignon, coffee: Arabica}", root.append_child());
    CHECK(ryml::emitrs<std::string>(tree) == R"(- a
- b
- x0: 1
  x1: 2
- champagne: Dom Perignon
  coffee: Arabica
)");
    CHECK(root.is_seq());
    CHECK(root[0].val() == "a");
    CHECK(root[1].val() == "b");
    CHECK(root[2].is_map());
    CHECK(root[2]["x0"].val() == "1");
    CHECK(root[2]["x1"].val() == "2");
    CHECK(root[3].is_map());
    CHECK(root[3]["champagne"].val() == "Dom Perignon");
    CHECK(root[3]["coffee"].val() == "Arabica");

    // watchout: to add to an existing node within a map, the node's key must first be set:
    ryml::NodeRef more = root[3].append_child({ryml::KEYMAP, "more"});
    ryml::NodeRef beer = root[3].append_child({ryml::KEYSEQ, "beer"});
    ryml::parse("{vinho verde: Soalheiro, vinho tinto: Redoma 2017}", more);
    ryml::parse("[Rochefort 10, Busch, Leffe Rituel]", beer);
    CHECK(ryml::emitrs<std::string>(tree) == R"(- a
- b
- x0: 1
  x1: 2
- champagne: Dom Perignon
  coffee: Arabica
  more:
    vinho verde: Soalheiro
    vinho tinto: Redoma 2017
  beer:
    - Rochefort 10
    - Busch
    - Leffe Rituel
)");

    ryml::parse("[foo, bar, baz, bat]", root);
    CHECK(ryml::emitrs<std::string>(tree) == R"(- a
- b
- x0: 1
  x1: 2
- champagne: Dom Perignon
  coffee: Arabica
  more:
    vinho verde: Soalheiro
    vinho tinto: Redoma 2017
  beer:
    - Rochefort 10
    - Busch
    - Leffe Rituel
- foo
- bar
- baz
- bat
)");

    ryml::parse("[Kasteel Donker]", beer);
    CHECK(ryml::emitrs<std::string>(tree) == R"(- a
- b
- x0: 1
  x1: 2
- champagne: Dom Perignon
  coffee: Arabica
  more:
    vinho verde: Soalheiro
    vinho tinto: Redoma 2017
  beer:
    - Rochefort 10
    - Busch
    - Leffe Rituel
    - Kasteel Donker
- foo
- bar
- baz
- bat
)");
}


//-----------------------------------------------------------------------------

/** Demonstrates reuse of an existing parser. Doing this is
    recommended when multiple files are parsed. */
void sample_parse_reuse_parser()
{
    ryml::Parser parser;

    // it is also advised to reserve the parser depth
    // to the expected depth of the data tree:
    parser.reserve_stack(10); // uses small storage optimization defaulting to 16 depth,
                              // so this instruction is a no-op, and the stack will located
                              // in the parser object.
    parser.reserve_stack(20); // But this will cause an allocation because it is above 16.

    auto champagnes = parser.parse("champagnes.yml", "[Dom Perignon, Gosset Grande Reserve, Ruinart Blanc de Blancs, Jacquesson 742]");
    CHECK(ryml::emitrs<std::string>(champagnes) == R"(- Dom Perignon
- Gosset Grande Reserve
- Ruinart Blanc de Blancs
- Jacquesson 742
)");

    auto beers = parser.parse("beers.yml", "[Rochefort 10, Busch, Leffe Rituel, Kasteel Donker]");
    CHECK(ryml::emitrs<std::string>(beers) == R"(- Rochefort 10
- Busch
- Leffe Rituel
- Kasteel Donker
)");

}


//-----------------------------------------------------------------------------

/** for ultimate speed when parsing multiple times, reuse both the
    tree and parser */
void sample_parse_reuse_tree_and_parser()
{
    ryml::Tree tree;
    ryml::Parser parser;

    // it will always be faster if the tree's size is conveniently reserved:
    tree.reserve(30); // reserve 30 nodes (good enough for this sample)
    // if you are using the tree's arena to serialize data,
    // then reserve also the arena's size:
    tree.reserve(256); // reserve 256 characters (good enough for this sample)
    // it is also advised to reserve the parser depth
    // to the expected depth of the data tree:
    parser.reserve_stack(10); // uses small storage optimization defaulting to 16 depth,
                              // so this instruction is a no-op, and the stack will located
                              // in the parser object.
    parser.reserve_stack(20); // But this will cause an allocation because it is above 16.

    ryml::csubstr champagnes = "[Dom Perignon, Gosset Grande Reserve, Ruinart Blanc de Blancs, Jacquesson 742]";
    ryml::csubstr beers = "[Rochefort 10, Busch, Leffe Rituel, Kasteel Donker]";
    ryml::csubstr wines = "[Soalheiro, Niepoort Redoma 2017, Vina Esmeralda]";

    parser.parse("champagnes.yml", champagnes, &tree);
    CHECK(ryml::emitrs<std::string>(tree) == R"(- Dom Perignon
- Gosset Grande Reserve
- Ruinart Blanc de Blancs
- Jacquesson 742
)");

    // watchout: this will APPEND to the given tree:
    parser.parse("beers.yml", beers, &tree);
    CHECK(ryml::emitrs<std::string>(tree) == R"(- Dom Perignon
- Gosset Grande Reserve
- Ruinart Blanc de Blancs
- Jacquesson 742
- Rochefort 10
- Busch
- Leffe Rituel
- Kasteel Donker
)");

    // if you don't wish to append, clear the tree first:
    tree.clear();
    parser.parse("wines.yml", wines, &tree);
    CHECK(ryml::emitrs<std::string>(tree) == R"(- Soalheiro
- Niepoort Redoma 2017
- Vina Esmeralda
)");
}


//-----------------------------------------------------------------------------

/** shows how to programatically iterate through trees */
void sample_iterate_trees()
{
    ryml::Tree tree = ryml::parse(R"(doe: "a deer, a female deer"
ray: "a drop of golden sun"
pi: 3.14159
xmas: true
french-hens: 3
calling-birds:
   - huey
   - dewey
   - louie
   - fred
xmas-fifth-day:
  calling-birds: four
  french-hens: 3
  golden-rings: 5
  partridges:
    count: 1
    location: a pear tree
  turtle-doves: two
cars: GTO
)");
    ryml::NodeRef root = tree.rootref();

    // iterate children
    {
        std::vector<ryml::csubstr> keys, vals; // to store all the root-level keys, vals
        for(ryml::NodeRef n : root.children())
        {
            keys.emplace_back(n.key());
            vals.emplace_back(n.has_val() ? n.val() : ryml::csubstr{});
        }
        CHECK(keys[0] == "doe");
        CHECK(vals[0] == "a deer, a female deer");
        CHECK(keys[1] == "ray");
        CHECK(vals[1] == "a drop of golden sun");
        CHECK(keys[2] == "pi");
        CHECK(vals[2] == "3.14159");
        CHECK(keys[3] == "xmas");
        CHECK(vals[3] == "true");
        CHECK(root[5].has_key());
        CHECK(root[5].is_seq());
        CHECK(root[5].key() == "calling-birds");
        CHECK(!root[5].has_val()); // it is a map, so not a val
        //CHECK(root[5].val() == ""); // ERROR! node does not have a val.
        CHECK(keys[5] == "calling-birds");
        CHECK(vals[5] == "");
    }

    // iterate siblings
    {
        size_t count = 0;
        ryml::csubstr calling_birds[] = {"huey", "dewey", "louie", "fred"};
        for(ryml::NodeRef n : root["calling-birds"][2].siblings())
            CHECK(n.val() == calling_birds[count++]);
    }
}


//-----------------------------------------------------------------------------

/** shows how to programatically create trees */
void sample_create_trees()
{
    ryml::NodeRef doe;
    CHECK(!doe.valid()); // it's pointing at nowhere

    ryml::Tree tree;
    ryml::NodeRef root = tree.rootref();
    root |= ryml::MAP; // mark root as a map
    doe = root["doe"];
    CHECK(doe.valid()); // it's now pointing at the tree
    CHECK(doe.is_seed()); // but the tree has nothing there, so this is only a seed

    // set the value of the node
    const char a_deer[] = "a deer, a female deer";
    doe = a_deer;
    // now the node really exists in the tree, and this ref is no
    // longer a seed:
    CHECK(!doe.is_seed());
    // WATCHOUT for lifetimes:
    CHECK(doe.val().str == a_deer); // it is pointing at the initial string
    // If you need to avoid lifetime dependency, serialize the data:
    {
        std::string a_drop = "a drop of golden sun";
        // this will copy the string to the tree's arena:
        // (see the serialization samples below)
        root["ray"] << "a drop of golden sun";
        // and now you can modify the original string without changing
        // the tree:
        a_drop[0] = 'Z';
        a_drop[1] = 'Z';
    }
    CHECK(root["ray"].val() == "a drop of golden sun");

    // etc.
    root["pi"] << ryml::fmt::real(3.141592654, 5);
    root["xmas"] << ryml::fmt::boolalpha(true);
    root["french-hens"] << 3;
    ryml::NodeRef calling_birds = root["calling-birds"];
    calling_birds |= ryml::SEQ;
    calling_birds.append_child() = "huey";
    calling_birds.append_child() = "dewey";
    calling_birds.append_child() = "louie";
    calling_birds.append_child() = "fred";
    ryml::NodeRef xmas5 = root["xmas-fifth-day"];
    xmas5 |= ryml::MAP;
    xmas5["calling-birds"] = "four";
    xmas5["french-hens"] << 3;
    xmas5["golden-rings"] << 5;
    xmas5["partridges"] |= ryml::MAP;
    xmas5["partridges"]["count"] << 1;
    xmas5["partridges"]["location"] = "a pear tree";
    xmas5["turtle-doves"] = "two";
    root["cars"] = "GTO";

    std::cout << tree;
    CHECK(ryml::emitrs<std::string>(tree) == R"(doe: 'a deer, a female deer'
ray: a drop of golden sun
pi: 3.14159
xmas: true
'french-hens': 3
'calling-birds':
  - huey
  - dewey
  - louie
  - fred
'xmas-fifth-day':
  'calling-birds': four
  'french-hens': 3
  'golden-rings': 5
  partridges:
    count: 1
    location: a pear tree
  'turtle-doves': two
cars: GTO
)");
}


//-----------------------------------------------------------------------------

/** demonstrates explicit and implicit interaction with the tree's string arena.
 * Notice that ryml only holds strings in the tree's nodes.
    */
void sample_tree_arena()
{
    // mutable buffers are parsed in situ:
    {
        char buf[] = "[a, b, c, d]";
        ryml::substr yml = buf;
        ryml::Tree tree = ryml::parse(yml);
        // notice the arena is empty:
        CHECK(tree.arena().empty());
        // and the tree is pointing at the original buffer:
        ryml::NodeRef root = tree.rootref();
        CHECK(root[0].val().is_sub(yml));
        CHECK(root[1].val().is_sub(yml));
        CHECK(root[2].val().is_sub(yml));
        CHECK(root[3].val().is_sub(yml));
        CHECK(yml.is_super(root[0].val()));
        CHECK(yml.is_super(root[1].val()));
        CHECK(yml.is_super(root[2].val()));
        CHECK(yml.is_super(root[3].val()));
    }

    // when parsing immutable buffers, the buffer is first copied to the
    // tree's arena; the copy in the arena is then the buffer which is
    // actually parsed
    {
        ryml::csubstr yml = "[a, b, c, d]";
        ryml::Tree tree = ryml::parse(yml);
        // notice the buffer was copied to the arena:
        CHECK(tree.arena().data() != yml.data());
        CHECK(tree.arena() == yml);
        // and the tree is pointing at the arena instead of to the
        // original buffer:
        ryml::NodeRef root = tree.rootref();
        ryml::csubstr arena = tree.arena();
        CHECK(root[0].val().is_sub(arena));
        CHECK(root[1].val().is_sub(arena));
        CHECK(root[2].val().is_sub(arena));
        CHECK(root[3].val().is_sub(arena));
        CHECK(arena.is_super(root[0].val()));
        CHECK(arena.is_super(root[1].val()));
        CHECK(arena.is_super(root[2].val()));
        CHECK(arena.is_super(root[3].val()));
    }

    // the arena is also used when the data is serialized to string
    // with NodeRef::operator<<(): mutable buffer
    {
        char buf[] = "[a, b, c, d]"; // mutable
        ryml::substr yml = buf;
        ryml::Tree tree = ryml::parse(yml);
        // notice the arena is empty:
        CHECK(tree.arena().empty());
        ryml::NodeRef root = tree.rootref();

        // serialize an integer, and mutate the tree
        CHECK(root[2].val() == "c");
        CHECK(root[2].val().is_sub(yml)); // val is first pointing at the buffer
        root[2] << 12345;
        CHECK(root[2].val() == "12345");
        CHECK(root[2].val().is_sub(tree.arena())); // now val is pointing at the arena
        // notice the serialized string was appended to the tree's arena:
        CHECK(tree.arena() == "12345");

        // serialize an integer, and mutate the tree
        CHECK(root[3].val() == "d");
        CHECK(root[3].val().is_sub(yml)); // val is first pointing at the buffer
        root[3] << 67890;
        CHECK(root[3].val() == "67890");
        CHECK(root[3].val().is_sub(tree.arena())); // now val is pointing at the arena
        // notice the serialized string was appended to the tree's arena:
        CHECK(tree.arena() == "1234567890");
    }
    // the arena is also used when the data is serialized to string
    // with NodeRef::operator<<(): immutable buffer
    {
        ryml::csubstr yml = "[a, b, c, d]"; // immutable
        ryml::Tree tree = ryml::parse(yml);
        // notice the buffer was copied to the arena:
        CHECK(tree.arena().data() != yml.data());
        CHECK(tree.arena() == yml);
        ryml::NodeRef root = tree.rootref();

        // serialize an integer, and mutate the tree
        CHECK(root[2].val() == "c");
        root[2] << 12345; // serialize an integer
        CHECK(root[2].val() == "12345");
        // notice the serialized string was appended to the tree's arena:
        // notice also the previous values remain there.
        // RYML DOES NOT KEEP TRACK OF REFERENCES TO THE ARENA.
        CHECK(tree.arena() == "[a, b, c, d]12345");
        // old values:  --------------^

        // serialize an integer, and mutate the tree
        root[3] << 67890;
        CHECK(root[3].val() == "67890");
        // notice the serialized string was appended to the tree's arena:
        // notice also the previous values remain there.
        // RYML DOES NOT KEEP TRACK OF REFERENCES TO THE ARENA.
        CHECK(tree.arena() == "[a, b, c, d]1234567890");
        // old values:  --------------^ ---^^^^^
    }

    // to_arena(): directly serialize values to the arena:
    {
        ryml::Tree tree = ryml::parse("{a: b}");
        ryml::csubstr c10 = tree.to_arena(10101010);
        CHECK(c10 == "10101010");
        CHECK(c10.is_sub(tree.arena()));
        CHECK(tree.arena() == "{a: b}10101010");
        CHECK(tree.key(1) == "a");
        CHECK(tree.val(1) == "b");
        tree.set_val(1, c10);
        CHECK(tree.val(1) == c10);
        // and you can also do it through a node:
        ryml::NodeRef root = tree.rootref();
        root["a"].set_val_serialized(2222);
        CHECK(root["a"].val() == "2222");
        CHECK(tree.arena() == "{a: b}101010102222");
    }

    // copy_to_arena(): manually copy a string to the arena:
    {
        ryml::Tree tree = ryml::parse("{a: b}");
        ryml::csubstr mystr = "Gosset Grande Reserve";
        ryml::csubstr copied = tree.copy_to_arena(mystr);
        CHECK(!copied.overlaps(mystr));
        CHECK(copied == mystr);
        CHECK(tree.arena() == "{a: b}Gosset Grande Reserve");
    }

    // alloc_arena(): allocate a buffer from the arena:
    {
        ryml::Tree tree = ryml::parse("{a: b}");
        ryml::csubstr mystr = "Gosset Grande Reserve";
        ryml::substr copied = tree.alloc_arena(mystr.size());
        CHECK(!copied.overlaps(mystr));
        memcpy(copied.str, mystr.str, mystr.len);
        CHECK(copied == mystr);
        CHECK(tree.arena() == "{a: b}Gosset Grande Reserve");
    }

    // reserve_arena(): ensure the arena has a certain size to avoid reallocations
    {
        ryml::Tree tree = ryml::parse("{a: b}");
        CHECK(tree.arena().size() == strlen("{a: b}"));
        tree.reserve_arena(100);
        CHECK(tree.arena_capacity() >= 100);
        CHECK(tree.arena().size() == strlen("{a: b}"));
        tree.to_arena(123456);
        CHECK(tree.arena().first(12) == "{a: b}123456");
    }
}


//-----------------------------------------------------------------------------

/** ryml provides facilities for serializing the C++ fundamental
    types. This is achieved through to_chars()/from_chars().
    See an example below for user scalar types. */
void sample_fundamental_types()
{
    ryml::Tree tree;
    CHECK(tree.arena().empty());
    CHECK(tree.to_arena('a') == "a");               CHECK(tree.arena() == "a");
    CHECK(tree.to_arena("bcde") == "bcde");         CHECK(tree.arena() == "abcde");
    CHECK(tree.to_arena(unsigned(0)) == "0");       CHECK(tree.arena() == "abcde0");
    CHECK(tree.to_arena(int(1)) == "1");            CHECK(tree.arena() == "abcde01");
    CHECK(tree.to_arena(uint8_t(0)) == "0");        CHECK(tree.arena() == "abcde010");
    CHECK(tree.to_arena(uint16_t(1)) ==  "1");      CHECK(tree.arena() == "abcde0101");
    CHECK(tree.to_arena(uint32_t(2)) ==  "2");      CHECK(tree.arena() == "abcde01012");
    CHECK(tree.to_arena(uint64_t(3)) ==  "3");      CHECK(tree.arena() == "abcde010123");
    CHECK(tree.to_arena(int8_t( 4))  ==  "4");      CHECK(tree.arena() == "abcde0101234");
    CHECK(tree.to_arena(int8_t(-4))  == "-4");      CHECK(tree.arena() == "abcde0101234-4");
    CHECK(tree.to_arena(int16_t( 5)) ==  "5");      CHECK(tree.arena() == "abcde0101234-45");
    CHECK(tree.to_arena(int16_t(-5)) == "-5");      CHECK(tree.arena() == "abcde0101234-45-5");
    CHECK(tree.to_arena(int32_t( 6)) ==  "6");      CHECK(tree.arena() == "abcde0101234-45-56");
    CHECK(tree.to_arena(int32_t(-6)) == "-6");      CHECK(tree.arena() == "abcde0101234-45-56-6");
    CHECK(tree.to_arena(int64_t( 7)) ==  "7");      CHECK(tree.arena() == "abcde0101234-45-56-67");
    CHECK(tree.to_arena(int64_t(-7)) == "-7");      CHECK(tree.arena() == "abcde0101234-45-56-67-7");
    CHECK(tree.to_arena((void*)1) == "0x1");        CHECK(tree.arena() == "abcde0101234-45-56-67-70x1");
    CHECK(tree.to_arena(float(0.124)) == "0.124");  CHECK(tree.arena() == "abcde0101234-45-56-67-70x10.124");
    CHECK(tree.to_arena(double(0.234)) == "0.234"); CHECK(tree.arena() == "abcde0101234-45-56-67-70x10.1240.234");
    CHECK(tree.to_arena(bool(true)) == "1");        CHECK(tree.arena() == "abcde0101234-45-56-67-70x10.1240.2341");
    CHECK(tree.to_arena(bool(false)) == "0");       CHECK(tree.arena() == "abcde0101234-45-56-67-70x10.1240.23410");

    // write special float values
    const float  fnan = std::numeric_limits<float >::quiet_NaN();
    const double dnan = std::numeric_limits<double>::quiet_NaN();
    const float  finf = std::numeric_limits<float >::infinity();
    const double dinf = std::numeric_limits<double>::infinity();
    CHECK(tree.to_arena( finf) ==  ".inf"); CHECK(tree.arena() == "abcde0101234-45-56-67-70x10.1240.23410.inf");
    CHECK(tree.to_arena( dinf) ==  ".inf"); CHECK(tree.arena() == "abcde0101234-45-56-67-70x10.1240.23410.inf.inf");
    CHECK(tree.to_arena(-finf) == "-.inf"); CHECK(tree.arena() == "abcde0101234-45-56-67-70x10.1240.23410.inf.inf-.inf");
    CHECK(tree.to_arena(-dinf) == "-.inf"); CHECK(tree.arena() == "abcde0101234-45-56-67-70x10.1240.23410.inf.inf-.inf-.inf");
    CHECK(tree.to_arena( fnan) ==  ".nan"); CHECK(tree.arena() == "abcde0101234-45-56-67-70x10.1240.23410.inf.inf-.inf-.inf.nan");
    CHECK(tree.to_arena( dnan) ==  ".nan"); CHECK(tree.arena() == "abcde0101234-45-56-67-70x10.1240.23410.inf.inf-.inf-.inf.nan.nan");
    // read special float values
    tree = ryml::parse(R"({ninf: -.inf, pinf: .inf, nan: .nan})");
    C4_SUPPRESS_WARNING_GCC_CLANG_WITH_PUSH("-Wfloat-equal");
    float f = 0.f;
    double d = 0.;
    CHECK(f == 0.f);
    CHECK(d == 0.);
    tree["ninf"] >> f; CHECK(f == -finf);
    tree["ninf"] >> d; CHECK(d == -dinf);
    tree["pinf"] >> f; CHECK(f ==  finf);
    tree["pinf"] >> d; CHECK(d ==  dinf);
    tree["nan" ] >> f; CHECK(std::isnan(f));
    tree["nan" ] >> d; CHECK(std::isnan(d));
    C4_SUPPRESS_WARNING_GCC_CLANG_POP
}


//-----------------------------------------------------------------------------

/** ryml provides facilities for formatting (imported from c4core into
 * the ryml namespace)
 *
 * @see https://c4core.docsforge.com/master/formatting-arguments/
 * @see https://c4core.docsforge.com/master/formatting-strings/
 */
void sample_formatting()
{
    // format(), format_sub(), formatrs(): format arguments
    {
        char buf_[256] = {};
        ryml::substr buf = buf_;
        size_t size = ryml::format(buf, "a={} foo {} {} bar {}", 0.1, 10, 11, 12);
        CHECK(size == strlen("a=0.1 foo 10 11 bar 12"));
        CHECK(buf.first(size) == "a=0.1 foo 10 11 bar 12");
        // it is safe to call on an empty buffer:
        // returns the size needed for the result, and no overflow occurs:
        size = ryml::format({} , "a={} foo {} {} bar {}", "this_is_a", 10, 11, 12);
        CHECK(size == ryml::format(buf, "a={} foo {} {} bar {}", "this_is_a", 10, 11, 12));
        CHECK(size == strlen("a=this_is_a foo 10 11 bar 12"));
        // it is also safe to call on an insufficient buffer:
        char smallbuf[8] = {};
        size = ryml::format(smallbuf, "{} is too large {}", "this", "for the buffer");
        CHECK(size == strlen("this is too large for the buffer"));
        // ... and the result is truncated at the buffer size:
        CHECK(ryml::substr(smallbuf, sizeof(smallbuf)) == "this is\0");

        // format_sub() directly returns the written string:
        ryml::csubstr result = ryml::format_sub(buf, "b={}, damn it.", 1);
        CHECK(result == "b=1, damn it.");
        CHECK(result.is_sub(buf));

        // formatrs() means FORMAT & ReSize:
        //
        // Instead of a substr, it receives any owning linear char container
        // for which to_substr() is defined (using ADL).
        // <ryml_std.hpp> has to_substr() definitions for std::string and
        // std::vector<char>.
        //
        // formatrs() starts by calling format(), and if needed, resizes the container
        // and calls format() again.
        //
        // Note that unless the container is previously sized, this
        // may cause an allocation, which will make your code slower.
        // Make sure to call .reserve() on the container for real
        // production code.
        std::string sbuf;
        ryml::formatrs(&sbuf, "and c={} seems about right", 2);
        CHECK(sbuf == "and c=2 seems about right");
        std::vector<char> vbuf; // works with any linear char container
        ryml::formatrs(&vbuf, "and c={} seems about right", 2);
        CHECK(sbuf == "and c=2 seems about right");
        // with formatrs() it is also possible to append:
        ryml::formatrs(ryml::append, &sbuf, ", and finally d={} - done", 3);
        CHECK(sbuf == "and c=2 seems about right, and finally d=3 - done");
    }

    // unformat(): read arguments - opposite of format()
    {
        char buf_[256];

        int a = 0, b = 1, c = 2;
        ryml::csubstr result = ryml::format_sub(buf_, "{} and {} and {}", a, b, c);
        CHECK(result == "0 and 1 and 2");
        int aa = -1, bb = -2, cc = -3;
        size_t num_characters = ryml::unformat(result, "{} and {} and {}", aa, bb, cc);
        CHECK(num_characters != ryml::csubstr::npos); // if a conversion fails, returns ryml::csubstr::npos
        CHECK(num_characters == result.size());
        CHECK(aa == a);
        CHECK(bb == b);
        CHECK(cc == c);

        result = ryml::format_sub(buf_, "{} and {} and {}", 10, 20, 30);
        CHECK(result == "10 and 20 and 30");
        num_characters = ryml::unformat(result, "{} and {} and {}", aa, bb, cc);
        CHECK(num_characters != ryml::csubstr::npos); // if a conversion fails, returns ryml::csubstr::npos
        CHECK(num_characters == result.size());
        CHECK(aa == 10);
        CHECK(bb == 20);
        CHECK(cc == 30);
    }

    // cat(), cat_sub(), catrs(): concatenate arguments
    {
        char buf_[256] = {};
        ryml::substr buf = buf_;
        size_t size = ryml::cat(buf, "a=", 0.1, "foo", 10, 11, "bar", 12);
        CHECK(size == strlen("a=0.1foo1011bar12"));
        CHECK(buf.first(size) == "a=0.1foo1011bar12");
        // it is safe to call on an empty buffer:
        // returns the size needed for the result, and no overflow occurs:
        CHECK(ryml::cat({}, "a=", 0) == 3);
        // it is also safe to call on an insufficient buffer:
        char smallbuf[8] = {};
        size = ryml::cat(smallbuf, "this", " is too large ", "for the buffer");
        CHECK(size == strlen("this is too large for the buffer"));
        // ... and the result is truncated at the buffer size:
        CHECK(ryml::substr(smallbuf, sizeof(smallbuf)) == "this is\0");

        // cat_sub() directly returns the written string:
        ryml::csubstr result = ryml::cat_sub(buf, "b=", 1, ", damn it.");
        CHECK(result == "b=1, damn it.");
        CHECK(result.is_sub(buf));

        // catrs() means CAT & ReSize:
        //
        // Instead of a substr, it receives any owning linear char container
        // for which to_substr() is defined (using ADL).
        // <ryml_std.hpp> has to_substr() definitions for std::string and
        // std::vector<char>.
        //
        // catrs() starts by calling cat(), and if needed, resizes the container
        // and calls cat() again.
        //
        // Note that unless the container is previously sized, this
        // may cause an allocation, which will make your code slower.
        // Make sure to call .reserve() on the container for real
        // production code.
        std::string sbuf;
        ryml::catrs(&sbuf, "and c=", 2, " seems about right");
        CHECK(sbuf == "and c=2 seems about right");
        std::vector<char> vbuf; // works with any linear char container
        ryml::catrs(&vbuf, "and c=", 2, " seems about right");
        CHECK(sbuf == "and c=2 seems about right");
        // with catrs() it is also possible to append:
        ryml::catrs(ryml::append, &sbuf, ", and finally d=", 3, " - done");
        CHECK(sbuf == "and c=2 seems about right, and finally d=3 - done");
    }

    // uncat(): read arguments - opposite of cat()
    {
        char buf_[256];

        int a = 0, b = 1, c = 2;
        ryml::csubstr result = ryml::cat_sub(buf_, a, ' ', b, ' ', c);
        CHECK(result == "0 1 2");
        int aa = -1, bb = -2, cc = -3;
        char sep1 = 'a', sep2 = 'b';
        size_t num_characters = ryml::uncat(result, aa, sep1, bb, sep2, cc);
        CHECK(num_characters == result.size());
        CHECK(aa == a);
        CHECK(bb == b);
        CHECK(cc == c);
        CHECK(sep1 == ' ');
        CHECK(sep2 == ' ');

        result = ryml::cat_sub(buf_, 10, ' ', 20, ' ', 30);
        CHECK(result == "10 20 30");
        num_characters = ryml::uncat(result, aa, sep1, bb, sep2, cc);
        CHECK(num_characters == result.size());
        CHECK(aa == 10);
        CHECK(bb == 20);
        CHECK(cc == 30);
        CHECK(sep1 == ' ');
        CHECK(sep2 == ' ');
    }

    // catsep(), catsep_sub(), catseprs(): concatenate arguments, with a separator
    {
        char buf_[256] = {};
        ryml::substr buf = buf_;
        // use ' ' as a separator
        size_t size = ryml::catsep(buf, ' ', "a=", 0, "b=", 1, "c=", 2, 45, 67);
        CHECK(buf.first(size) == "a= 0 b= 1 c= 2 45 67");
        // any separator may be used
        // use " and " as a separator
        size = ryml::catsep(buf, " and ", "a=0", "b=1", "c=2", 45, 67);
        CHECK(buf.first(size) == "a=0 and b=1 and c=2 and 45 and 67");
        // use " ... " as a separator
        size = ryml::catsep(buf, " ... ", "a=0", "b=1", "c=2", 45, 67);
        CHECK(buf.first(size) == "a=0 ... b=1 ... c=2 ... 45 ... 67");
        // use '/' as a separator
        size = ryml::catsep(buf, '/', "a=", 0, "b=", 1, "c=", 2, 45, 67);
        CHECK(buf.first(size) == "a=/0/b=/1/c=/2/45/67");
        // use 888 as a separator
        size = ryml::catsep(buf, 888, "a=0", "b=1", "c=2", 45, 67);
        CHECK(buf.first(size) == "a=0888b=1888c=28884588867");

        // it is safe to call on an empty buffer:
        // returns the size needed for the result, and no overflow occurs:
        CHECK(size == ryml::catsep({}, 888, "a=0", "b=1", "c=2", 45, 67));
        // it is also safe to call on an insufficient buffer:
        char smallbuf[8] = {};
        CHECK(size == ryml::catsep(smallbuf, 888, "a=0", "b=1", "c=2", 45, 67));
        CHECK(size == strlen("a=0888b=1888c=28884588867"));
        // ... and the result is truncated:
        CHECK(ryml::substr(smallbuf, sizeof(smallbuf)) == "a=0888b\0");

        // catsep_sub() directly returns the written substr:
        ryml::csubstr result = ryml::catsep_sub(buf, " and ", "a=0", "b=1", "c=2", 45, 67);
        CHECK(result == "a=0 and b=1 and c=2 and 45 and 67");
        CHECK(result.is_sub(buf));

        // catseprs() means CATSEP & ReSize:
        //
        // Instead of a substr, it receives any owning linear char container
        // for which to_substr() is defined (using ADL).
        // <ryml_std.hpp> has to_substr() definitions for std::string and
        // std::vector<char>.
        //
        // catseprs() starts by calling catsep(), and if needed, resizes the container
        // and calls catsep() again.
        //
        // Note that unless the container is previously sized, this
        // may cause an allocation, which will make your code slower.
        // Make sure to call .reserve() on the container for real
        // production code.
        std::string sbuf;
        ryml::catseprs(&sbuf, " and ", "a=0", "b=1", "c=2", 45, 67);
        CHECK(sbuf == "a=0 and b=1 and c=2 and 45 and 67");
        std::vector<char> vbuf; // works with any linear char container
        ryml::catseprs(&vbuf, " and ", "a=0", "b=1", "c=2", 45, 67);
        CHECK(ryml::to_csubstr(vbuf) == "a=0 and b=1 and c=2 and 45 and 67");

        // with catseprs() it is also possible to append:
        ryml::catseprs(ryml::append, &sbuf, " well ", " --- a=0", "b=11", "c=12", 145, 167);
        CHECK(sbuf == "a=0 and b=1 and c=2 and 45 and 67 --- a=0 well b=11 well c=12 well 145 well 167");
    }

    // uncatsep(): read arguments with a separator - opposite of catsep()
    {
        char buf_[256] = {};

        int a = 0, b = 1, c = 2;
        ryml::csubstr result = ryml::catsep_sub(buf_, ' ', a, b, c);
        CHECK(result == "0 1 2");
        int aa = -1, bb = -2, cc = -3;
        char sep = 'b';
        size_t num_characters = ryml::uncatsep(result, sep, aa, bb, cc);
        CHECK(num_characters == result.size());
        CHECK(aa == a);
        CHECK(bb == b);
        CHECK(cc == c);
        CHECK(sep == ' ');

        sep = '_';
        result = ryml::catsep_sub(buf_, ' ', 10, 20, 30);
        CHECK(result == "10 20 30");
        num_characters = ryml::uncatsep(result, sep, aa, bb, cc);
        CHECK(num_characters == result.size());
        CHECK(aa == 10);
        CHECK(bb == 20);
        CHECK(cc == 30);
        CHECK(sep == ' ');
    }

    // formatting individual arguments
    {
        using namespace ryml;  // all the symbols below are in the ryml namespace.
        char buf_[256] = {};   // all the results below are written in this buffer
        substr buf = buf_;
        // --------------------------------------
        // fmt::boolalpha(): format as true/false
        // --------------------------------------
        // just as with std streams, printing a bool will output the integer value:
        CHECK("0" == cat_sub(buf, false));
        CHECK("1" == cat_sub(buf, true));
        // to force a "true"/"false", use fmt::boolalpha:
        CHECK("false" == cat_sub(buf, fmt::boolalpha(false)));
        CHECK("true" == cat_sub(buf, fmt::boolalpha(true)));

        // ---------------------------------
        // fmt::hex(): format as hexadecimal
        // ---------------------------------
        CHECK("0xff"  == cat_sub(buf, fmt::hex(255)));
        CHECK("0x100" == cat_sub(buf, fmt::hex(256)));
        CHECK("-0xff"  == cat_sub(buf, fmt::hex(-255)));
        CHECK("-0x100" == cat_sub(buf, fmt::hex(-256)));
        CHECK("3735928559" == cat_sub(buf,          UINT32_C(0xdeadbeef)));
        CHECK("0xdeadbeef" == cat_sub(buf, fmt::hex(UINT32_C(0xdeadbeef))));
        // ----------------------------
        // fmt::bin(): format as binary
        // ----------------------------
        CHECK("0b1000" == cat_sub(buf, fmt::bin(8)));
        CHECK("0b1001" == cat_sub(buf, fmt::bin(9)));
        CHECK("0b10001" == cat_sub(buf, fmt::bin(17)));
        CHECK("0b11001" == cat_sub(buf, fmt::bin(25)));
        CHECK("-0b1000" == cat_sub(buf, fmt::bin(-8)));
        CHECK("-0b1001" == cat_sub(buf, fmt::bin(-9)));
        CHECK("-0b10001" == cat_sub(buf, fmt::bin(-17)));
        CHECK("-0b11001" == cat_sub(buf, fmt::bin(-25)));
        // ---------------------------
        // fmt::bin(): format as octal
        // ---------------------------
        CHECK("0o77"    == cat_sub(buf, fmt::oct(63)));
        CHECK("0o100"   == cat_sub(buf, fmt::oct(64)));
        CHECK("0o377"   == cat_sub(buf, fmt::oct(255)));
        CHECK("0o400"   == cat_sub(buf, fmt::oct(256)));
        CHECK("0o1000"  == cat_sub(buf, fmt::oct(512)));
        CHECK("-0o77"   == cat_sub(buf, fmt::oct(-63)));
        CHECK("-0o100"  == cat_sub(buf, fmt::oct(-64)));
        CHECK("-0o377"  == cat_sub(buf, fmt::oct(-255)));
        CHECK("-0o400"  == cat_sub(buf, fmt::oct(-256)));
        CHECK("-0o1000" == cat_sub(buf, fmt::oct(-512)));
        // ---------------------------
        // fmt::zpad(): pad with zeros
        // ---------------------------
        CHECK("000063" == cat_sub(buf, fmt::zpad(63, 6)));
        CHECK( "00063" == cat_sub(buf, fmt::zpad(63, 5)));
        CHECK(  "0063" == cat_sub(buf, fmt::zpad(63, 4)));
        CHECK(   "063" == cat_sub(buf, fmt::zpad(63, 3)));
        CHECK(    "63" == cat_sub(buf, fmt::zpad(63, 2)));
        CHECK(    "63" == cat_sub(buf, fmt::zpad(63, 1))); // will never trim the result
        CHECK(    "63" == cat_sub(buf, fmt::zpad(63, 0))); // will never trim the result
        CHECK("0x00003f" == cat_sub(buf, fmt::zpad(fmt::hex(63), 6)));
        CHECK("0o000077" == cat_sub(buf, fmt::zpad(fmt::oct(63), 6)));
        CHECK("0b00011001" == cat_sub(buf, fmt::zpad(fmt::bin(25), 8)));
        // ------------------------------------------------
        // fmt::left(): align left with a given field width
        // ------------------------------------------------
        CHECK("63    " == cat_sub(buf, fmt::left(63, 6)));
        CHECK("63   "  == cat_sub(buf, fmt::left(63, 5)));
        CHECK("63  "   == cat_sub(buf, fmt::left(63, 4)));
        CHECK("63 "    == cat_sub(buf, fmt::left(63, 3)));
        CHECK("63"     == cat_sub(buf, fmt::left(63, 2)));
        CHECK("63"     == cat_sub(buf, fmt::left(63, 1))); // will never trim the result
        CHECK("63"     == cat_sub(buf, fmt::left(63, 0))); // will never trim the result
        // the fill character can be specified (defaults to ' '):
        CHECK("63----" == cat_sub(buf, fmt::left(63, 6, '-')));
        CHECK("63++++" == cat_sub(buf, fmt::left(63, 6, '+')));
        CHECK("63////" == cat_sub(buf, fmt::left(63, 6, '/')));
        CHECK("630000" == cat_sub(buf, fmt::left(63, 6, '0')));
        CHECK("63@@@@" == cat_sub(buf, fmt::left(63, 6, '@')));
        CHECK("0x003f    " == cat_sub(buf, fmt::left(fmt::zpad(fmt::hex(63), 4), 10)));
        // --------------------------------------------------
        // fmt::right(): align right with a given field width
        // --------------------------------------------------
        CHECK("    63" == cat_sub(buf, fmt::right(63, 6)));
        CHECK("   63"  == cat_sub(buf, fmt::right(63, 5)));
        CHECK("  63"   == cat_sub(buf, fmt::right(63, 4)));
        CHECK(" 63"    == cat_sub(buf, fmt::right(63, 3)));
        CHECK("63"     == cat_sub(buf, fmt::right(63, 2)));
        CHECK("63"     == cat_sub(buf, fmt::right(63, 1))); // will never trim the result
        CHECK("63"     == cat_sub(buf, fmt::right(63, 0))); // will never trim the result
        // the fill character can be specified (defaults to ' '):
        CHECK("----63" == cat_sub(buf, fmt::right(63, 6, '-')));
        CHECK("++++63" == cat_sub(buf, fmt::right(63, 6, '+')));
        CHECK("////63" == cat_sub(buf, fmt::right(63, 6, '/')));
        CHECK("000063" == cat_sub(buf, fmt::right(63, 6, '0')));
        CHECK("@@@@63" == cat_sub(buf, fmt::right(63, 6, '@')));
        CHECK("    0x003f" == cat_sub(buf, fmt::right(fmt::zpad(fmt::hex(63), 4), 10)));

        // ------------------------------------------
        // fmt::real(): format floating point numbers
        // ------------------------------------------
        CHECK("0"       == cat_sub(buf, fmt::real(0.01f, 0)));
        CHECK("0.0"     == cat_sub(buf, fmt::real(0.01f, 1)));
        CHECK("0.01"    == cat_sub(buf, fmt::real(0.01f, 2)));
        CHECK("0.010"   == cat_sub(buf, fmt::real(0.01f, 3)));
        CHECK("0.0100"  == cat_sub(buf, fmt::real(0.01f, 4)));
        CHECK("0.01000" == cat_sub(buf, fmt::real(0.01f, 5)));
        CHECK("1"       == cat_sub(buf, fmt::real(1.01f, 0)));
        CHECK("1.0"     == cat_sub(buf, fmt::real(1.01f, 1)));
        CHECK("1.01"    == cat_sub(buf, fmt::real(1.01f, 2)));
        CHECK("1.010"   == cat_sub(buf, fmt::real(1.01f, 3)));
        CHECK("1.0100"  == cat_sub(buf, fmt::real(1.01f, 4)));
        CHECK("1.01000" == cat_sub(buf, fmt::real(1.01f, 5)));
        CHECK("1"       == cat_sub(buf, fmt::real(1.234234234, 0)));
        CHECK("1.2"     == cat_sub(buf, fmt::real(1.234234234, 1)));
        CHECK("1.23"    == cat_sub(buf, fmt::real(1.234234234, 2)));
        CHECK("1.234"   == cat_sub(buf, fmt::real(1.234234234, 3)));
        CHECK("1.2342"  == cat_sub(buf, fmt::real(1.234234234, 4)));
        CHECK("1.23423" == cat_sub(buf, fmt::real(1.234234234, 5)));
        CHECK("1000000.00000" == cat_sub(buf, fmt::real(1000000.000000000, 5)));
        CHECK("1234234.23423" == cat_sub(buf, fmt::real(1234234.234234234, 5)));
        // AKA %f
        CHECK("1000000.00000" == cat_sub(buf, fmt::real(1000000.000000000, 5, FTOA_FLOAT)));  // AKA %f, same as above
        CHECK("1234234.23423" == cat_sub(buf, fmt::real(1234234.234234234, 5, FTOA_FLOAT)));  // AKA %f
        CHECK("1234234.2342"  == cat_sub(buf, fmt::real(1234234.234234234, 4, FTOA_FLOAT)));  // AKA %f
        CHECK("1234234.234"   == cat_sub(buf, fmt::real(1234234.234234234, 3, FTOA_FLOAT)));  // AKA %f
        CHECK("1234234.23"    == cat_sub(buf, fmt::real(1234234.234234234, 2, FTOA_FLOAT)));  // AKA %f
        // AKA %e
        CHECK("1.00000e+06"   == cat_sub(buf, fmt::real(1000000.000000000, 5, FTOA_SCIENT))); // AKA %e
        CHECK("1.23423e+06"   == cat_sub(buf, fmt::real(1234234.234234234, 5, FTOA_SCIENT))); // AKA %e
        CHECK("1.2342e+06"    == cat_sub(buf, fmt::real(1234234.234234234, 4, FTOA_SCIENT))); // AKA %e
        CHECK("1.234e+06"     == cat_sub(buf, fmt::real(1234234.234234234, 3, FTOA_SCIENT))); // AKA %e
        CHECK("1.23e+06"      == cat_sub(buf, fmt::real(1234234.234234234, 2, FTOA_SCIENT))); // AKA %e
        // AKA %g
        CHECK("1e+06"         == cat_sub(buf, fmt::real(1000000.000000000, 5, FTOA_FLEX)));   // AKA %g
        CHECK("1.2342e+06"    == cat_sub(buf, fmt::real(1234234.234234234, 5, FTOA_FLEX)));   // AKA %g
        CHECK("1.234e+06"     == cat_sub(buf, fmt::real(1234234.234234234, 4, FTOA_FLEX)));   // AKA %g
        CHECK("1.23e+06"      == cat_sub(buf, fmt::real(1234234.234234234, 3, FTOA_FLEX)));   // AKA %g
        CHECK("1.2e+06"       == cat_sub(buf, fmt::real(1234234.234234234, 2, FTOA_FLEX)));   // AKA %g
        // AKA %a (hexadecimal formatting of floats)
        CHECK("0x1.e8480p+19" == cat_sub(buf, fmt::real(1000000.000000000, 5, FTOA_HEXA)));   // AKA %a
        CHECK("0x1.2d53ap+20" == cat_sub(buf, fmt::real(1234234.234234234, 5, FTOA_HEXA)));   // AKA %a
        CHECK("0x1.2d54p+20"  == cat_sub(buf, fmt::real(1234234.234234234, 4, FTOA_HEXA)));   // AKA %a
        CHECK("0x1.2d5p+20"   == cat_sub(buf, fmt::real(1234234.234234234, 3, FTOA_HEXA)));   // AKA %a
        CHECK("0x1.2dp+20"    == cat_sub(buf, fmt::real(1234234.234234234, 2, FTOA_HEXA)));   // AKA %a

        // --------------------------------------------------------------
        // fmt::raw(): dump data in machine format (respecting alignment)
        // --------------------------------------------------------------
        {
            C4_SUPPRESS_WARNING_CLANG_WITH_PUSH("-Wcast-align")  // we're casting the values directly, so alignment is strictly respected.
            const uint32_t payload[] = {10, 20, 30, 40, UINT32_C(0xdeadbeef)};
            // (package payload as a substr, for comparison only)
            csubstr expected = csubstr((const char *)payload, sizeof(payload));
            csubstr actual = cat_sub(buf, fmt::raw(payload));
            CHECK(!actual.overlaps(expected));
            CHECK(0 == memcmp(expected.str, actual.str, expected.len));
            // also possible with variables:
            for(const uint32_t value : payload)
            {
                // (package payload as a substr, for comparison only)
                expected = csubstr((const char *)&value, sizeof(value));
                actual = cat_sub(buf, fmt::raw(value));
                CHECK(actual.size() == sizeof(uint32_t));
                CHECK(!actual.overlaps(expected));
                CHECK(0 == memcmp(expected.str, actual.str, expected.len));
                // with non-const data, fmt::craw() may be needed for disambiguation:
                actual = cat_sub(buf, fmt::craw(value));
                CHECK(actual.size() == sizeof(uint32_t));
                CHECK(!actual.overlaps(expected));
                CHECK(0 == memcmp(expected.str, actual.str, expected.len));
                //
                // read back:
                uint32_t result;
                auto reader = fmt::raw(result); // keeps a reference to result
                CHECK(&result == (uint32_t*)reader.buf);
                CHECK(reader.len == sizeof(uint32_t));
                uncat(actual, reader);
                // and compare:
                // (vs2017/release/32bit does not reload result from cache, so force it)
                result = *(uint32_t*)reader.buf;
                CHECK(result == value); // roundtrip completed successfully
            }
            C4_SUPPRESS_WARNING_CLANG_POP
        }

        // -------------------------
        // fmt::base64(): see below!
        // -------------------------
    }
}


//-----------------------------------------------------------------------------

/** demonstrates how to read and write base64-encoded blobs.
 @see https://c4core.docsforge.com/master/base64/
 */
void sample_base64()
{
    ryml::Tree tree;
    tree.rootref() |= ryml::MAP;
    struct text_and_base64 { ryml::csubstr text, base64; };
    text_and_base64 cases[] = {
        {{"Love all, trust a few, do wrong to none."}, {"TG92ZSBhbGwsIHRydXN0IGEgZmV3LCBkbyB3cm9uZyB0byBub25lLg=="}},
        {{"The fool doth think he is wise, but the wise man knows himself to be a fool."}, {"VGhlIGZvb2wgZG90aCB0aGluayBoZSBpcyB3aXNlLCBidXQgdGhlIHdpc2UgbWFuIGtub3dzIGhpbXNlbGYgdG8gYmUgYSBmb29sLg=="}},
        {{"Brevity is the soul of wit."}, {"QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu"}},
        {{"All that glitters is not gold."}, {"QWxsIHRoYXQgZ2xpdHRlcnMgaXMgbm90IGdvbGQu"}},
        {{"These violent delights have violent ends..."}, {"VGhlc2UgdmlvbGVudCBkZWxpZ2h0cyBoYXZlIHZpb2xlbnQgZW5kcy4uLg=="}},
        {{"How now, my love?"}, {"SG93IG5vdywgbXkgbG92ZT8="}},
        {{"Why is your cheek so pale?"}, {"V2h5IGlzIHlvdXIgY2hlZWsgc28gcGFsZT8="}},
        {{"How chance the roses there do fade so fast?"}, {"SG93IGNoYW5jZSB0aGUgcm9zZXMgdGhlcmUgZG8gZmFkZSBzbyBmYXN0Pw=="}},
        {{"Belike for want of rain, which I could well beteem them from the tempest of my eyes."}, {"QmVsaWtlIGZvciB3YW50IG9mIHJhaW4sIHdoaWNoIEkgY291bGQgd2VsbCBiZXRlZW0gdGhlbSBmcm9tIHRoZSB0ZW1wZXN0IG9mIG15IGV5ZXMu"}},
    };
    // to encode base64 and write the result to val:
    for(text_and_base64 c : cases)
    {
        tree[c.text] << ryml::fmt::base64(c.text);
        CHECK(tree[c.text].val() == c.base64);
    }
    // to encode base64 and write the result to key:
    for(text_and_base64 c : cases)
    {
        tree.rootref().append_child() << ryml::key(ryml::fmt::base64(c.text)) << c.text;
        CHECK(tree[c.base64].val() == c.text);
    }
    C4_CHECK(ryml::emitrs<std::string>(tree) == R"('Love all, trust a few, do wrong to none.': TG92ZSBhbGwsIHRydXN0IGEgZmV3LCBkbyB3cm9uZyB0byBub25lLg==
'The fool doth think he is wise, but the wise man knows himself to be a fool.': VGhlIGZvb2wgZG90aCB0aGluayBoZSBpcyB3aXNlLCBidXQgdGhlIHdpc2UgbWFuIGtub3dzIGhpbXNlbGYgdG8gYmUgYSBmb29sLg==
Brevity is the soul of wit.: QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu
All that glitters is not gold.: QWxsIHRoYXQgZ2xpdHRlcnMgaXMgbm90IGdvbGQu
These violent delights have violent ends...: VGhlc2UgdmlvbGVudCBkZWxpZ2h0cyBoYXZlIHZpb2xlbnQgZW5kcy4uLg==
'How now, my love?': SG93IG5vdywgbXkgbG92ZT8=
'Why is your cheek so pale?': V2h5IGlzIHlvdXIgY2hlZWsgc28gcGFsZT8=
'How chance the roses there do fade so fast?': SG93IGNoYW5jZSB0aGUgcm9zZXMgdGhlcmUgZG8gZmFkZSBzbyBmYXN0Pw==
'Belike for want of rain, which I could well beteem them from the tempest of my eyes.': QmVsaWtlIGZvciB3YW50IG9mIHJhaW4sIHdoaWNoIEkgY291bGQgd2VsbCBiZXRlZW0gdGhlbSBmcm9tIHRoZSB0ZW1wZXN0IG9mIG15IGV5ZXMu
TG92ZSBhbGwsIHRydXN0IGEgZmV3LCBkbyB3cm9uZyB0byBub25lLg==: 'Love all, trust a few, do wrong to none.'
VGhlIGZvb2wgZG90aCB0aGluayBoZSBpcyB3aXNlLCBidXQgdGhlIHdpc2UgbWFuIGtub3dzIGhpbXNlbGYgdG8gYmUgYSBmb29sLg==: 'The fool doth think he is wise, but the wise man knows himself to be a fool.'
QnJldml0eSBpcyB0aGUgc291bCBvZiB3aXQu: Brevity is the soul of wit.
QWxsIHRoYXQgZ2xpdHRlcnMgaXMgbm90IGdvbGQu: All that glitters is not gold.
VGhlc2UgdmlvbGVudCBkZWxpZ2h0cyBoYXZlIHZpb2xlbnQgZW5kcy4uLg==: These violent delights have violent ends...
SG93IG5vdywgbXkgbG92ZT8=: 'How now, my love?'
V2h5IGlzIHlvdXIgY2hlZWsgc28gcGFsZT8=: 'Why is your cheek so pale?'
SG93IGNoYW5jZSB0aGUgcm9zZXMgdGhlcmUgZG8gZmFkZSBzbyBmYXN0Pw==: 'How chance the roses there do fade so fast?'
QmVsaWtlIGZvciB3YW50IG9mIHJhaW4sIHdoaWNoIEkgY291bGQgd2VsbCBiZXRlZW0gdGhlbSBmcm9tIHRoZSB0ZW1wZXN0IG9mIG15IGV5ZXMu: 'Belike for want of rain, which I could well beteem them from the tempest of my eyes.'
)");
    // to decode the val base64 and write the result to buf:
    char buf1_[128], buf2_[128];
    ryml::substr buf1 = buf1_;  // this is where we will write the result
    ryml::substr buf2 = buf2_;  // this is where we will write the result
    for(auto c : cases)
    {
        // write the decoded result into the given buffer
        tree[c.text] >> ryml::fmt::base64(buf1); // cannot know the needed size
        size_t len = tree[c.text].deserialize_val(ryml::fmt::base64(buf2)); // returns the needed size
        CHECK(len <= buf1.len);
        CHECK(len <= buf2.len);
        CHECK(c.text.len == len);
        CHECK(buf1.first(len) == c.text);
        CHECK(buf2.first(len) == c.text);
    }
    // to decode the val base64 and write the result to buf:
    for(text_and_base64 c : cases)
    {
        // write the decoded result into the given buffer
        tree[c.base64] >> ryml::key(ryml::fmt::base64(buf1)); // cannot know the needed size
        size_t len = tree[c.base64].deserialize_key(ryml::fmt::base64(buf2)); // returns the needed size
        CHECK(len <= buf1.len);
        CHECK(len <= buf2.len);
        CHECK(c.text.len == len);
        CHECK(buf1.first(len) == c.text);
        CHECK(buf2.first(len) == c.text);
    }
    // directly encode variables
    {
        const uint64_t valin = UINT64_C(0xdeadbeef);
        uint64_t valout = 0;
        tree["deadbeef"] << c4::fmt::base64(valin); // sometimes cbase64() is needed to avoid ambiguity
        size_t len = tree["deadbeef"].deserialize_val(ryml::fmt::base64(valout));
        CHECK(len <= sizeof(valout));
        CHECK(valout == UINT64_C(0xdeadbeef)); // base64 roundtrip is bit-accurate
    }
    // directly encode memory ranges
    {
        const uint32_t data_in[11] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xdeadbeef};
        uint32_t data_out[11] = {};
        CHECK(memcmp(data_in, data_out, sizeof(data_in)) != 0); // before the roundtrip
        tree["int_data"] << c4::fmt::base64(data_in);
        size_t len = tree["int_data"].deserialize_val(ryml::fmt::base64(data_out));
        CHECK(len <= sizeof(data_out));
        CHECK(memcmp(data_in, data_out, sizeof(data_in)) == 0); // after the roundtrip
    }
}


//-----------------------------------------------------------------------------

// user scalar types: implemented in ryml through to_chars() + from_chars()

// To harness [C++'s ADL rules](http://en.cppreference.com/w/cpp/language/adl),
// it is important to overload these functions in the namespace of the type
// you're serializing (or in the c4::yml namespace).
//
// Please take note of the following pitfall when using serialization
// functions: you have to include the header with the serialization
// before any other headers that use functions from it. see the
// include order at the top of this file.
//
// This constraint also applies to the conversion functions for your
// types; just like with the STL's headers, they should be included
// prior to ryml's headers.

template<class T> struct vec2 { T x, y; };
template<class T> struct vec3 { T x, y, z; };
template<class T> struct vec4 { T x, y, z, w; };

template<class T> struct parse_only_vec2 { T x, y; };
template<class T> struct parse_only_vec3 { T x, y, z; };
template<class T> struct parse_only_vec4 { T x, y, z, w; };

template<class T> struct emit_only_vec2 { T x, y; };
template<class T> struct emit_only_vec3 { T x, y, z; };
template<class T> struct emit_only_vec4 { T x, y, z, w; };

// to_chars(): only needed for emitting
//
// format v to the given string view + return the number of
// characters written into it. The view size (buf.len) must
// be strictly respected. Return the number of characters
// that need to be written. So if the return value
// is larger than buf.len, ryml will resize the buffer and
// call this again with a larger buffer of the correct size.

template<class T> size_t to_chars(ryml::substr buf, vec2<T> v) { return ryml::format(buf, "({},{})", v.x, v.y); }
template<class T> size_t to_chars(ryml::substr buf, vec3<T> v) { return ryml::format(buf, "({},{},{})", v.x, v.y, v.z); }
template<class T> size_t to_chars(ryml::substr buf, vec4<T> v) { return ryml::format(buf, "({},{},{},{})", v.x, v.y, v.z, v.w); }

template<class T> size_t to_chars(ryml::substr buf, emit_only_vec2<T> v) { return ryml::format(buf, "({},{})", v.x, v.y); }
template<class T> size_t to_chars(ryml::substr buf, emit_only_vec3<T> v) { return ryml::format(buf, "({},{},{})", v.x, v.y, v.z); }
template<class T> size_t to_chars(ryml::substr buf, emit_only_vec4<T> v) { return ryml::format(buf, "({},{},{},{})", v.x, v.y, v.z, v.w); }


// from_chars(): only needed for parsing

template<class T> bool from_chars(ryml::csubstr buf, vec2<T> *v) { size_t ret = ryml::unformat(buf, "({},{})", v->x, v->y); return ret != ryml::yml::npos; }
template<class T> bool from_chars(ryml::csubstr buf, vec3<T> *v) { size_t ret = ryml::unformat(buf, "({},{},{})", v->x, v->y, v->z); return ret != ryml::yml::npos; }
template<class T> bool from_chars(ryml::csubstr buf, vec4<T> *v) { size_t ret = ryml::unformat(buf, "({},{},{},{})", v->x, v->y, v->z, v->w); return ret != ryml::yml::npos; }

template<class T> bool from_chars(ryml::csubstr buf, parse_only_vec2<T> *v) { size_t ret = ryml::unformat(buf, "({},{})", v->x, v->y); return ret != ryml::yml::npos; }
template<class T> bool from_chars(ryml::csubstr buf, parse_only_vec3<T> *v) { size_t ret = ryml::unformat(buf, "({},{},{})", v->x, v->y, v->z); return ret != ryml::yml::npos; }
template<class T> bool from_chars(ryml::csubstr buf, parse_only_vec4<T> *v) { size_t ret = ryml::unformat(buf, "({},{},{},{})", v->x, v->y, v->z, v->w); return ret != ryml::yml::npos; }


/** to add scalar types (ie leaf types converting to/from string),
    define the functions above for those types. */
void sample_user_scalar_types()
{
    ryml::Tree t;

    auto r = t.rootref();
    r |= ryml::MAP;

    vec2<int> v2in{10, 11};
    vec2<int> v2out{1, 2};
    r["v2"] << v2in; // serializes to the tree's arena, and then sets the keyval
    r["v2"] >> v2out;
    CHECK(v2in.x == v2out.x);
    CHECK(v2in.y == v2out.y);
    vec3<int> v3in{100, 101, 102};
    vec3<int> v3out{1, 2, 3};
    r["v3"] << v3in; // serializes to the tree's arena, and then sets the keyval
    r["v3"] >> v3out;
    CHECK(v3in.x == v3out.x);
    CHECK(v3in.y == v3out.y);
    CHECK(v3in.z == v3out.z);
    vec4<int> v4in{1000, 1001, 1002, 1003};
    vec4<int> v4out{1, 2, 3, 4};
    r["v4"] << v4in; // serializes to the tree's arena, and then sets the keyval
    r["v4"] >> v4out;
    CHECK(v4in.x == v4out.x);
    CHECK(v4in.y == v4out.y);
    CHECK(v4in.z == v4out.z);
    CHECK(v4in.w == v4out.w);
    CHECK(ryml::emitrs<std::string>(t) == R"(v2: '(10,11)'
v3: '(100,101,102)'
v4: '(1000,1001,1002,1003)'
)");

    // note that only the used functions are needed:
    //   - if a type is only parsed, then only from_chars() is needed
    //   - if a type is only emitted, then only to_chars() is needed
    emit_only_vec2<int> eov2in{20, 21}; // only has to_chars()
    parse_only_vec2<int> pov2out{1, 2}; // only has from_chars()
    r["v2"] << eov2in; // serializes to the tree's arena, and then sets the keyval
    r["v2"] >> pov2out;
    CHECK(eov2in.x == pov2out.x);
    CHECK(eov2in.y == pov2out.y);
    emit_only_vec3<int> eov3in{30, 31, 32}; // only has to_chars()
    parse_only_vec3<int> pov3out{1, 2, 3}; // only has from_chars()
    r["v3"] << eov3in; // serializes to the tree's arena, and then sets the keyval
    r["v3"] >> pov3out;
    CHECK(eov3in.x == pov3out.x);
    CHECK(eov3in.y == pov3out.y);
    CHECK(eov3in.z == pov3out.z);
    emit_only_vec4<int> eov4in{40, 41, 42, 43}; // only has to_chars()
    parse_only_vec4<int> pov4out{1, 2, 3, 4}; // only has from_chars()
    r["v4"] << eov4in; // serializes to the tree's arena, and then sets the keyval
    r["v4"] >> pov4out;
    CHECK(eov4in.x == pov4out.x);
    CHECK(eov4in.y == pov4out.y);
    CHECK(eov4in.z == pov4out.z);
    CHECK(ryml::emitrs<std::string>(t) == R"(v2: '(20,21)'
v3: '(30,31,32)'
v4: '(40,41,42,43)'
)");
}


//-----------------------------------------------------------------------------

// user container types: implemented in ryml through write() + read()
//
// Please take note of the following pitfall when using serialization
// functions: you have to include the header with the serialization
// before any other headers that use functions from it. see the
// include order at the top of this file.
//
// This constraint also applies to the conversion functions for your
// types; just like with the STL's headers, they should be included
// prior to ryml's headers.

// user container type: seq-like
template<class T> struct my_seq_type { std::vector<T> seq_member; };
template<class T>
void write(ryml::NodeRef *n, my_seq_type<T> const& seq)
{
    *n |= ryml::SEQ;
    for(auto const& v : seq.seq_member)
        n->append_child() << v;
}
template<class T>
bool read(ryml::NodeRef const& n, my_seq_type<T> *seq)
{
    seq->seq_member.resize(n.num_children()); // num_children() is O(N)
    size_t pos = 0;
    for(auto const ch : n.children())
        ch >> seq->seq_member[pos++];
    return true;
}


// user container type: map-like
template<class K, class V> struct my_map_type { std::map<K, V> map_member; };
template<class K, class V>
void write(ryml::NodeRef *n, my_map_type<K, V> const& map)
{
    *n |= ryml::MAP;
    for(auto const& v : map.map_member)
        n->append_child() << ryml::key(v.first) << v.second;
}
template<class K, class V>
bool read(ryml::NodeRef const& n, my_map_type<K, V> *map)
{
    K k{};
    V v;
    for(auto const ch : n)
    {
        ch >> c4::yml::key(k) >> v;
        map->map_member.emplace(std::make_pair(std::move(k), std::move(v)));
    }
    return true;
}


// user container type: notice all the members are complex types
// defined above
struct my_type
{
    vec2<int> v2;
    vec3<int> v3;
    vec4<int> v4;
    my_seq_type<int> seq;
    my_map_type<int, int> map;
};
void write(ryml::NodeRef *n, my_type const& val)
{
    *n |= ryml::MAP;
    n->append_child() << ryml::key("v2") << val.v2;
    n->append_child() << ryml::key("v3") << val.v3;
    n->append_child() << ryml::key("v4") << val.v4;
    n->append_child() << ryml::key("seq") << val.seq;
    n->append_child() << ryml::key("map") << val.map;
}
bool read(ryml::NodeRef const& n, my_type *val)
{
    n["v2"] >> val->v2;
    n["v3"] >> val->v3;
    n["v4"] >> val->v4;
    n["seq"] >> val->seq;
    n["map"] >> val->map;
    return true;
}


void sample_user_container_types()
{
    my_type mt_in{
        {20, 21},
        {30, 31, 32},
        {40, 41, 42, 43},
        {{101, 102, 103, 104, 105, 106, 107}},
        {{{1001, 2001}, {1002, 2002}, {1003, 2003}}},
    };
    my_type mt_out;

    ryml::Tree t;
    t.rootref() << mt_in;  // read from this
    t.rootref() >> mt_out; // assign here
    CHECK(mt_out.v2.x == mt_in.v2.x);
    CHECK(mt_out.v2.y == mt_in.v2.y);
    CHECK(mt_out.v3.x == mt_in.v3.x);
    CHECK(mt_out.v3.y == mt_in.v3.y);
    CHECK(mt_out.v3.z == mt_in.v3.z);
    CHECK(mt_out.v4.x == mt_in.v4.x);
    CHECK(mt_out.v4.y == mt_in.v4.y);
    CHECK(mt_out.v4.z == mt_in.v4.z);
    CHECK(mt_out.v4.w == mt_in.v4.w);
    CHECK(mt_in.seq.seq_member.size() > 0);
    CHECK(mt_out.seq.seq_member.size() == mt_in.seq.seq_member.size());
    for(size_t i = 0; i < mt_in.seq.seq_member.size(); ++i)
    {
        CHECK(mt_out.seq.seq_member[i] == mt_in.seq.seq_member[i]);
    }
    CHECK(mt_in.map.map_member.size() > 0);
    CHECK(mt_out.map.map_member.size() == mt_in.map.map_member.size());
    for(auto const& kv : mt_in.map.map_member)
    {
        CHECK(mt_out.map.map_member.find(kv.first) != mt_out.map.map_member.end());
        CHECK(mt_out.map.map_member[kv.first] == kv.second);
    }
    CHECK(ryml::emitrs<std::string>(t) == R"(v2: '(20,21)'
v3: '(30,31,32)'
v4: '(40,41,42,43)'
seq:
  - 101
  - 102
  - 103
  - 104
  - 105
  - 106
  - 107
map:
  1001: 2001
  1002: 2002
  1003: 2003
)");
}


//-----------------------------------------------------------------------------
//
// Please take note of the following pitfall when using serialization
// functions: you have to include the header with the serialization
// before any other headers that use functions from it. see the
// include order at the top of this file.
//
// This constraint also applies to the conversion functions for your
// types; just like with the STL's headers, they should be included
// prior to ryml's headers.

/** demonstrates usage with the std implementations provided by ryml in the ryml_std.hpp header */
void sample_std_types()
{
    std::string yml_std_string = R"(- v2: '(20,21)'
  v3: '(30,31,32)'
  v4: '(40,41,42,43)'
  seq:
    - 101
    - 102
    - 103
    - 104
    - 105
    - 106
    - 107
  map:
    1001: 2001
    1002: 2002
    1003: 2003
- v2: '(120,121)'
  v3: '(130,131,132)'
  v4: '(140,141,142,143)'
  seq:
    - 1101
    - 1102
    - 1103
    - 1104
    - 1105
    - 1106
    - 1107
  map:
    11001: 12001
    11002: 12002
    11003: 12003
- v2: '(220,221)'
  v3: '(230,231,232)'
  v4: '(240,241,242,243)'
  seq:
    - 2101
    - 2102
    - 2103
    - 2104
    - 2105
    - 2106
    - 2107
  map:
    21001: 22001
    21002: 22002
    21003: 22003
)";
    // parse in-place using the std::string above
    auto tree = ryml::parse(ryml::to_substr(yml_std_string));
    // my_type is a container-of-containers type. see above its
    // definition implementation for ryml.
    std::vector<my_type> vmt;
    tree.rootref() >> vmt;
    CHECK(vmt.size() == 3);
    ryml::Tree tree_out;
    tree_out.rootref() << vmt;
    CHECK(ryml::emitrs<std::string>(tree_out) == yml_std_string);
}


//-----------------------------------------------------------------------------

/** demonstrates how to emit to a linear container of char */
void sample_emit_to_container()
{
    // it is possible to emit to any linear container of char.

    ryml::csubstr ymla = "- 1\n- 2\n";
    ryml::csubstr ymlb = R"(- a
- b
- x0: 1
  x1: 2
- champagne: Dom Perignon
  coffee: Arabica
  more:
    vinho verde: Soalheiro
    vinho tinto: Redoma 2017
  beer:
    - Rochefort 10
    - Busch
    - Leffe Rituel
- foo
- bar
- baz
- bat
)";
    auto treea = ryml::parse(ymla);
    auto treeb = ryml::parse(ymlb);

    // eg, std::vector<char>
    {
        // do a blank call on an empty buffer to find the required size.
        // no overflow will occur, and returns a substr with the size
        // required to output
        ryml::csubstr output = ryml::emit(treea, treea.root_id(), ryml::substr{}, /*error_on_excess*/false);
        CHECK(output.str == nullptr);
        CHECK(output.len > 0);
        size_t num_needed_chars = output.len;
        std::vector<char> buf(num_needed_chars);
        // now try again with the proper buffer
        output = ryml::emit(treea, treea.root_id(), ryml::to_substr(buf), /*error_on_excess*/true);
        CHECK(output == ymla);

        // it is possible to reuse the buffer and grow it as needed.
        // first do a blank run to find the size:
        output = ryml::emit(treeb, treeb.root_id(), ryml::substr{}, /*error_on_excess*/false);
        CHECK(output.str == nullptr);
        CHECK(output.len > 0);
        CHECK(output.len == ymlb.len);
        num_needed_chars = output.len;
        buf.resize(num_needed_chars);
        // now try again with the proper buffer
        output = ryml::emit(treeb, treeb.root_id(), ryml::to_substr(buf), /*error_on_excess*/true);
        CHECK(output == ymlb);

        // there is a convenience wrapper performing the same as above:
        // provided to_substr() is defined for that container.
        output = ryml::emitrs(treeb, &buf);
        CHECK(output == ymlb);

        // or you can just output a new container:
        // provided to_substr() is defined for that container.
        std::vector<char> another = ryml::emitrs<std::vector<char>>(treeb);
        CHECK(ryml::to_csubstr(another) == ymlb);

        // you can also emit nested nodes:
        another = ryml::emitrs<std::vector<char>>(treeb[3][2]);
        CHECK(ryml::to_csubstr(another) == R"(more:
  vinho verde: Soalheiro
  vinho tinto: Redoma 2017
)");
    }


    // eg, std::string. notice this is the same code as above
    {
        // do a blank call on an empty buffer to find the required size.
        // no overflow will occur, and returns a substr with the size
        // required to output
        ryml::csubstr output = ryml::emit(treea, treea.root_id(), ryml::substr{}, /*error_on_excess*/false);
        CHECK(output.str == nullptr);
        CHECK(output.len > 0);
        size_t num_needed_chars = output.len;
        std::string buf;
        buf.resize(num_needed_chars);
        // now try again with the proper buffer
        output = ryml::emit(treea, treea.root_id(), ryml::to_substr(buf), /*error_on_excess*/true);
        CHECK(output == ymla);

        // it is possible to reuse the buffer and grow it as needed.
        // first do a blank run to find the size:
        output = ryml::emit(treeb, treeb.root_id(), ryml::substr{}, /*error_on_excess*/false);
        CHECK(output.str == nullptr);
        CHECK(output.len > 0);
        CHECK(output.len == ymlb.len);
        num_needed_chars = output.len;
        buf.resize(num_needed_chars);
        // now try again with the proper buffer
        output = ryml::emit(treeb, treeb.root_id(), ryml::to_substr(buf), /*error_on_excess*/true);
        CHECK(output == ymlb);

        // there is a convenience wrapper performing the above instructions:
        // provided to_substr() is defined for that container
        output = ryml::emitrs(treeb, &buf);
        CHECK(output == ymlb);

        // or you can just output a new container:
        // provided to_substr() is defined for that container.
        std::string another = ryml::emitrs<std::string>(treeb);
        CHECK(ryml::to_csubstr(another) == ymlb);

        // you can also emit nested nodes:
        another = ryml::emitrs<std::string>(treeb[3][2]);
        CHECK(ryml::to_csubstr(another) == R"(more:
  vinho verde: Soalheiro
  vinho tinto: Redoma 2017
)");
    }
}


//-----------------------------------------------------------------------------

/** demonstrates how to emit to a stream-like structure */
void sample_emit_to_stream()
{
    ryml::csubstr ymlb = R"(- a
- b
- x0: 1
  x1: 2
- champagne: Dom Perignon
  coffee: Arabica
  more:
    vinho verde: Soalheiro
    vinho tinto: Redoma 2017
  beer:
    - Rochefort 10
    - Busch
    - Leffe Rituel
- foo
- bar
- baz
- bat
)";
    auto tree = ryml::parse(ymlb);

    std::string s;

    // emit a full tree
    {
        std::stringstream ss;
        ss << tree; // works with any stream having .operator<<() and .write()
        s = ss.str();
        CHECK(ryml::to_csubstr(s) == ymlb);
    }

    // emit a full tree as json
    {
        std::stringstream ss;
        ss << ryml::as_json(tree); // works with any stream having .operator<<() and .write()
        s = ss.str();
        CHECK(ryml::to_csubstr(s) == R"(["a","b",{"x0": 1,"x1": 2},{"champagne": "Dom Perignon","coffee": "Arabica","more": {"vinho verde": "Soalheiro","vinho tinto": "Redoma 2017"},"beer": ["Rochefort 10","Busch","Leffe Rituel"]},"foo","bar","baz","bat"])");
    }

    // emit a nested node
    {
        std::stringstream ss;
        ss << tree[3][2]; // works with any stream having .operator<<() and .write()
        s = ss.str();
        CHECK(ryml::to_csubstr(s) == R"(more:
  vinho verde: Soalheiro
  vinho tinto: Redoma 2017
)");
    }

    // emit a nested node as json
    {
        std::stringstream ss;
        ss << ryml::as_json(tree[3][2]); // works with any stream having .operator<<() and .write()
        s = ss.str();
        CHECK(ryml::to_csubstr(s) == R"("more": {"vinho verde": "Soalheiro","vinho tinto": "Redoma 2017"})");
    }
}


//-----------------------------------------------------------------------------

/** demonstrates how to emit to a FILE* */
void sample_emit_to_file()
{
    ryml::csubstr yml = R"(- a
- b
- x0: 1
  x1: 2
- champagne: Dom Perignon
  coffee: Arabica
  more:
    vinho verde: Soalheiro
    vinho tinto: Redoma 2017
  beer:
    - Rochefort 10
    - Busch
    - Leffe Rituel
- foo
- bar
- baz
- bat
)";
    auto tree = ryml::parse(yml);
    // this is emitting to stdout, but of course you can pass in any
    // FILE* obtained from fopen()
    size_t len = ryml::emit(tree, tree.root_id(), stdout);
    // the return value is the number of characters that were written
    // to the file
    CHECK(len == yml.len);
}


//-----------------------------------------------------------------------------

/** just like parsing into a nested node, you can also emit from a nested node. */
void sample_emit_nested_node()
{
    auto tree = ryml::parse(R"(- a
- b
- x0: 1
  x1: 2
- champagne: Dom Perignon
  coffee: Arabica
  more:
    vinho verde: Soalheiro
    vinho tinto: Redoma 2017
  beer:
    - Rochefort 10
    - Busch
    - Leffe Rituel
- foo
- bar
- baz
- bat
)");
    CHECK(ryml::emitrs<std::string>(tree[3]["beer"][0]) == "- Rochefort 10\n");
    CHECK(ryml::emitrs<std::string>(tree[3]["beer"]) == R"(beer:
  - Rochefort 10
  - Busch
  - Leffe Rituel
)");
}


//-----------------------------------------------------------------------------


/** shows how to parse and emit JSON. */
void sample_json()
{
    {
        // Since JSON is a subset of YAML, parsing JSON is just the
        // same as YAML:
        ryml::Tree tree = ryml::parse(R"({
"doe": "a deer, a female deer",
"ray": "a drop of golden sun",
"me": "a name, I call myself",
"far": "a long long way to go"
})");
        // However, emitting still defaults to YAML
        CHECK(ryml::emitrs<std::string>(tree) == R"('doe': 'a deer, a female deer'
'ray': 'a drop of golden sun'
'me': 'a name, I call myself'
'far': 'a long long way to go'
)");
        // to emit JSON, use the proper overload:
        CHECK(ryml::emitrs_json<std::string>(tree) == R"({"doe": "a deer, a female deer","ray": "a drop of golden sun","me": "a name, I call myself","far": "a long long way to go"})");
        // to emit JSON to a stream:
        std::stringstream ss;
        ss << ryml::as_json(tree);  // <- mark it like this
        CHECK(ss.str() == R"({"doe": "a deer, a female deer","ray": "a drop of golden sun","me": "a name, I call myself","far": "a long long way to go"})");
        // Note the following limitations:
        //
        // - streams cannot be emitted as json, and are not
        //   allowed. But you can work around this by emitting the
        //   individual documents separately; see the sample_docs()
        //   below for such an example.
        //
        // - tags cannot be emitted as json, and are not allowed.
        //
        // - anchors and aliases cannot be emitted as json and are not allowed.
    }

    // IMPORTANT NOTE.
    //
    // Although JSON is generally a subset of YAML, [there is an
    // exception that is valid JSON, but not valid
    // YAML](https://stackoverflow.com/questions/42124227/why-does-the-yaml-spec-mandate-a-space-after-the-colon):
    //
    // ryml::Tree tree = ryml::parse(R"({"a":"b"})"); // ERROR: missing space after the semicolon
    //
    // This above is deliberate, and is meant to save added complexity
    // in the parser code. However, you can still parse this with
    // ryml if prior to parsing you preprocess the JSON into valid
    // YAML, adding the missing spaces after the semicolons. ryml
    // provides a freestanding function to do this:
    // `ryml::preprocess_json()`:
    {
        // there are also in-place overloads, which generally should be preferred:
        std::string yaml = ryml::preprocess_json<std::string>(R"({"a":"b"})");
        // now you have a buffer with valid yaml - note the space:
        CHECK(yaml == R"({"a": "b"})");
        // ... which you can parse:
        ryml::Tree tree = ryml::parse(ryml::to_substr(yaml));
        CHECK(tree["a"] == "b");
    }

    // There is also `ryml::preprocess_rxmap()`, a function to convert
    // non-standard relaxed maps (ie, keys with implicit true values)
    // into standard YAML maps.
    {
        // you can also use in-place overloads
        std::string yaml = ryml::preprocess_rxmap<std::string>("{a, b, c, d: [e, f, g]}");
        CHECK(yaml == R"({a: 1, b: 1, c: 1, d: [e, f, g]})");
        ryml::Tree tree = ryml::parse(ryml::to_substr(yaml));
        CHECK(tree["a"] == "1");
    }
}


//-----------------------------------------------------------------------------

/** demonstrates usage with anchors and alias references.

Note that dereferencing is opt-in; after parsing, you have to call
`Tree::resolve()` explicitly if you want resolved references in the
tree. This method will resolve all references and substitute the anchored
values in place of the reference.

The `Tree::resolve()` method first does a full traversal of the tree to
gather all anchors and references in a separate collection, then it goes
through that collection to locate the names, which it does by obeying the
YAML standard diktat that

    an alias node refers to the most recent node in
    the serialization having the specified anchor

So, depending on the number of anchor/alias nodes, this is a potentially
expensive operation, with a best-case linear complexity (from the initial
traversal) and a worst-case quadratic complexity (if every node has an
alias/anchor). This potential cost is the reason for requiring an explicit
call to `Tree::resolve()`. */
void sample_anchors_and_aliases()
{
    std::string unresolved = R"(base: &base
  name: Everyone has same name
foo: &foo
  <<: *base
  age: 10
bar: &bar
  <<: *base
  age: 20
bill_to: &id001
  street: |-
    123 Tornado Alley
    Suite 16
  city: East Centerville
  state: KS
ship_to: *id001
&keyref key: &valref val
*valref: *keyref
)";

    ryml::Tree tree = ryml::parse(ryml::to_csubstr(unresolved));
    // by default, references are not resolved when parsing:
    CHECK( ! tree["base"].has_key_anchor());
    CHECK(   tree["base"].has_val_anchor());
    CHECK(   tree["base"].val_anchor() == "base");
    CHECK(   tree["key"].key_anchor() == "keyref");
    CHECK(   tree["key"].val_anchor() == "valref");
    CHECK(   tree["*valref"].is_key_ref());
    CHECK(   tree["*valref"].is_val_ref());
    CHECK(   tree["*valref"].key_ref() == "valref");
    CHECK(   tree["*valref"].val_ref() == "keyref");

    // to resolve references, simply call tree.resolve(),
    // which will perform the reference instantiations:
    tree.resolve();

    // all the anchors and references are substistuted and then removed:
    CHECK( ! tree["base"].has_key_anchor());
    CHECK( ! tree["base"].has_val_anchor());
    CHECK( ! tree["base"].has_val_anchor());
    CHECK( ! tree["key"].has_key_anchor());
    CHECK( ! tree["key"].has_val_anchor());
    CHECK( ! tree["val"].is_key_ref()); // notice *valref is now turned to val
    CHECK( ! tree["val"].is_val_ref()); // notice *valref is now turned to val

    CHECK(tree["ship_to"]["city"] == "East Centerville");
    CHECK(tree["ship_to"]["state"] == "KS");
}


//-----------------------------------------------------------------------------

void sample_tags()
{
    std::string unresolved = R"(--- !!map
a: 0
b: 1
--- !map
? a
: b
--- !!seq
- a
- b
--- !!str
a
 b
...
--- !!str a b
...
--- !!str a b
--- !!str
a: b
--- !!str a: b
---
!!str a: b
---
!!str a
 b
---
!!set
? a
? b
--- !!set
? a
---
[!!int 0, !!str 0]
)";
    std::string resolved = R"(--- !!map
a: 0
b: 1
--- !map
a: b
--- !!seq
- a
- b
--- !!str a b
--- !!str a b
--- !!str a b
--- !!str 'a: b'
--- !!str 'a: b'
---
!!str a: b
--- !!str a b
--- !!set
a: ~
b: ~
--- !!set
a: ~
---
- !!int 0
- !!str 0
)";
    auto tree = ryml::parse(ryml::to_csubstr(unresolved));
    CHECK(ryml::emitrs<std::string>(tree) == resolved);
    auto stream = tree.rootref();
    CHECK(stream.is_stream());
    CHECK(stream.num_children() == 13);
    for(auto node : stream.children())
    {
        CHECK(node.is_doc());
    }
    CHECK(stream[11].val_tag() == "!!set");
}


//-----------------------------------------------------------------------------

void sample_docs()
{
    std::string yml = R"(---
a: 0
b: 1
---
c: 2
d: 3
---
- 4
- 5
- 6
- 7
)";
    ryml::Tree tree = ryml::parse(ryml::to_substr(yml));
    CHECK(ryml::emitrs<std::string>(tree) == yml);

    // iteration through docs
    {
        // using the node API
        const ryml::NodeRef stream = tree.rootref();
        CHECK(stream.is_root());
        CHECK(stream.is_stream());
        CHECK(!stream.is_doc());
        CHECK(stream.num_children() == 3);
        for(const ryml::NodeRef doc : stream.children())
            CHECK(doc.is_doc());
        CHECK(tree.docref(0).id() == stream.child(0).id());
        CHECK(tree.docref(1).id() == stream.child(1).id());
        CHECK(tree.docref(2).id() == stream.child(2).id());
        // equivalent: using the lower level index API
        const size_t stream_id = tree.root_id();
        CHECK(tree.is_root(stream_id));
        CHECK(tree.is_stream(stream_id));
        CHECK(!tree.is_doc(stream_id));
        CHECK(tree.num_children(stream_id) == 3);
        for(size_t doc_id = tree.first_child(stream_id); doc_id != ryml::NONE; doc_id = tree.next_sibling(stream_id))
            CHECK(tree.is_doc(doc_id));
        CHECK(tree.doc(0) == tree.child(stream_id, 0));
        CHECK(tree.doc(1) == tree.child(stream_id, 1));
        CHECK(tree.doc(2) == tree.child(stream_id, 2));

        // using the node API
        CHECK(stream[0].is_doc());
        CHECK(stream[0].is_map());
        CHECK(stream[0]["a"].val() == "0");
        CHECK(stream[0]["b"].val() == "1");
        // equivalent: using the index API
        const size_t doc0_id = tree.first_child(stream_id);
        CHECK(tree.is_doc(doc0_id));
        CHECK(tree.is_map(doc0_id));
        CHECK(tree.val(tree.find_child(doc0_id, "a")) == "0");
        CHECK(tree.val(tree.find_child(doc0_id, "b")) == "1");

        // using the node API
        CHECK(stream[1].is_doc());
        CHECK(stream[1].is_map());
        CHECK(stream[1]["c"].val() == "2");
        CHECK(stream[1]["d"].val() == "3");
        // equivalent: using the index API
        const size_t doc1_id = tree.next_sibling(doc0_id);
        CHECK(tree.is_doc(doc1_id));
        CHECK(tree.is_map(doc1_id));
        CHECK(tree.val(tree.find_child(doc1_id, "c")) == "2");
        CHECK(tree.val(tree.find_child(doc1_id, "d")) == "3");

        // using the node API
        CHECK(stream[2].is_doc());
        CHECK(stream[2].is_seq());
        CHECK(stream[2][0].val() == "4");
        CHECK(stream[2][1].val() == "5");
        CHECK(stream[2][2].val() == "6");
        CHECK(stream[2][3].val() == "7");
        // equivalent: using the index API
        const size_t doc2_id = tree.next_sibling(doc1_id);
        CHECK(tree.is_doc(doc2_id));
        CHECK(tree.is_seq(doc2_id));
        CHECK(tree.val(tree.child(doc2_id, 0)) == "4");
        CHECK(tree.val(tree.child(doc2_id, 1)) == "5");
        CHECK(tree.val(tree.child(doc2_id, 2)) == "6");
        CHECK(tree.val(tree.child(doc2_id, 3)) == "7");
    }

    // Note: since json does not have streams, you cannot emit the above
    // tree as json when you start from the root:
    //CHECK(ryml::emitrs_json<std::string>(tree) == yml); // RUNTIME ERROR!

    // emitting streams as json is not possible, but
    // you can iterate through individual documents and emit
    // them separately:
    {
        const std::string expected_json[] = {
            R"({"a": 0,"b": 1})",
            R"({"c": 2,"d": 3})",
            R"([4,5,6,7])",
        };
        // using the node API
        {
            size_t count = 0;
            const ryml::NodeRef stream = tree.rootref();
            CHECK(stream.num_children() == C4_COUNTOF(expected_json));
            for(const ryml::NodeRef doc : stream.children())
                CHECK(ryml::emitrs_json<std::string>(doc) == expected_json[count++]);
        }
        // equivalent: using the index API
        {
            size_t count = 0;
            const size_t stream_id = tree.root_id();
            CHECK(tree.num_children(stream_id) == C4_COUNTOF(expected_json));
            for(size_t doc_id = tree.first_child(stream_id); doc_id != ryml::NONE; doc_id = tree.next_sibling(doc_id))
                CHECK(ryml::emitrs_json<std::string>(tree, doc_id) == expected_json[count++]);
        }
    }
}


//-----------------------------------------------------------------------------

struct ErrorHandlerExample
{
    // this will be called on error
    void on_error(const char* msg, size_t len, ryml::Location loc)
    {
        throw std::runtime_error(ryml::formatrs<std::string>("{}:{}:{} ({}B): ERROR: {}",
            loc.name, loc.line, loc.col, loc.offset, ryml::csubstr(msg, len)));
    }

    // bridge
    ryml::Callbacks callbacks()
    {
        return ryml::Callbacks(this, nullptr, nullptr, ErrorHandlerExample::s_error);
    }
    static void s_error(const char* msg, size_t len, ryml::Location loc, void *this_)
    {
        return ((ErrorHandlerExample*)this_)->on_error(msg, len, loc);
    }

    // checking
    template<class Fn>
    void check_error_occurs(Fn &&fn) const
    {
        bool expected_error_occurred = false;
        try { fn(); }
        catch(std::runtime_error const&) { expected_error_occurred = true; }
        CHECK(expected_error_occurred);
    }
    void check_effect(bool committed) const
    {
        ryml::Callbacks const& current = ryml::get_callbacks();
        ryml::MemoryResourceCallbacks const& mres = *(ryml::MemoryResourceCallbacks*) ryml::get_memory_resource();
        if(committed)
        {
            CHECK(current.m_error == &s_error);
            CHECK(mres.m_callbacks.m_error == &s_error);
        }
        else
        {
            CHECK(current.m_error != &s_error);
            CHECK(mres.m_callbacks.m_error != &s_error);
        }
        CHECK(current.m_allocate == defaults.m_allocate);
        CHECK(current.m_free == defaults.m_free);
        CHECK(mres.m_callbacks.m_allocate == defaults.m_allocate);
        CHECK(mres.m_callbacks.m_free == defaults.m_free);
    }
    // save the default callbacks for checking
    ErrorHandlerExample() : defaults(ryml::get_callbacks()) {}
    ryml::Callbacks defaults;
};

/** demonstrates how to set a custom error handler for ryml */
void sample_error_handler()
{
    ErrorHandlerExample errh;

    ryml::set_callbacks(errh.callbacks());
    errh.check_effect(/*committed*/true);
    errh.check_error_occurs([]{
        ryml::Tree tree = ryml::parse("errorhandler.yml", "[a: b\n}");
        std::cout << tree;
    });

    ryml::set_callbacks(errh.defaults); // restore defaults.
    errh.check_effect(/*committed*/false);
}


//-----------------------------------------------------------------------------

// Please note the following about the use of custom allocators with
// ryml. Due to [the static initialization order
// fiasco](https://en.cppreference.com/w/cpp/language/siof), if you
// use static ryml trees or parsers, you need to make sure that their
// allocator has the same lifetime. So you can't use ryml's default
// allocator, as it is declared in a ryml file, and the standard
// provides no guarantee on the relative initialization order, such
// that the allocator is constructed before and destroyed after your
// variables (in fact you are pretty much guaranteed to see this
// fail). So please carefully consider your choices, and ponder
// whether you really need to use ryml static trees and parsers. If
// you do need this, then you will need to declare and use an
// allocator from a ryml memory resource that outlives the tree and/or
// parser.

struct GlobalAllocatorExample
{
    std::vector<char> memory_pool = std::vector<char>(10u * 1024u); // 10KB
    size_t num_allocs = 0, alloc_size = 0;
    size_t num_deallocs = 0, dealloc_size = 0;

    void *allocate(size_t len)
    {
        void *ptr = &memory_pool[alloc_size];
        alloc_size += len;
        ++num_allocs;
        if(C4_UNLIKELY(alloc_size > memory_pool.size()))
        {
            std::cerr << "out of memory! requested=" << alloc_size << " vs " << memory_pool.size() << " available" << std::endl;
            std::abort();
        }
        return ptr;
    }

    void free(void *mem, size_t len)
    {
        CHECK((char*)mem     >= &memory_pool.front() && (char*)mem     <  &memory_pool.back());
        CHECK((char*)mem+len >= &memory_pool.front() && (char*)mem+len <= &memory_pool.back());
        dealloc_size += len;
        ++num_deallocs;
        // no need to free here
    }

    // bridge
    ryml::Callbacks callbacks()
    {
        return ryml::Callbacks(this, &GlobalAllocatorExample::s_allocate, &GlobalAllocatorExample::s_free, nullptr);
    }
    static void* s_allocate(size_t len, void* /*hint*/, void *this_)
    {
        return ((GlobalAllocatorExample*)this_)->allocate(len);
    }
    static void s_free(void *mem, size_t len, void *this_)
    {
        return ((GlobalAllocatorExample*)this_)->free(mem, len);
    }

    // checking
    ~GlobalAllocatorExample()
    {
        check_and_reset();
    }
    void check_and_reset()
    {
        std::cout << "size: alloc=" << alloc_size << " dealloc=" << dealloc_size << std::endl;
        std::cout << "count: #allocs=" << num_allocs << " #deallocs=" << num_deallocs << std::endl;
        CHECK(num_allocs == num_deallocs);
        CHECK(alloc_size >= dealloc_size); // failure here means a double free
        CHECK(alloc_size == dealloc_size); // failure here means a leak
        num_allocs = 0;
        num_deallocs = 0;
        alloc_size = 0;
        dealloc_size = 0;
    }
};


/** demonstrates how to set the global allocator for ryml */
void sample_global_allocator()
{
    GlobalAllocatorExample mem;

    // save the existing callbacks for restoring
    ryml::Callbacks defaults = ryml::get_callbacks();

    // set to our callbacks
    ryml::set_callbacks(mem.callbacks());

    // verify that the allocator is in effect
    ryml::Callbacks const& current = ryml::get_callbacks();
    CHECK(current.m_allocate == &mem.s_allocate);
    CHECK(current.m_free == &mem.s_free);

    // so far nothing was allocated
    CHECK(mem.alloc_size == 0);

    // parse one tree and check
    (void)ryml::parse(R"({foo: bar})");
    mem.check_and_reset();

    // parse another tree and check
    (void)ryml::parse(R"([a, b, c, d, {foo: bar, money: pennys}])");
    mem.check_and_reset();

    // verify that by reserving we save allocations
    {
        ryml::Parser parser;      // reuse a parser
        ryml::Tree tree;          // reuse a tree

        tree.reserve(10);         // reserve the number of nodes
        tree.reserve_arena(100);  // reserve the arena size
        parser.reserve_stack(10); // reserve the parser depth.

        // since the parser stack uses Small Storage Optimization,
        // allocations will only happen with capacities higher than 16.
        CHECK(mem.num_allocs == 2); // tree, tree_arena and NOT the parser

        parser.reserve_stack(20); // reserve the parser depth.
        CHECK(mem.num_allocs == 3); // tree, tree_arena and now the parser as well

        // verify that no other allocations occur when parsing
        size_t size_before = mem.alloc_size;
        parser.parse("", R"([a, b, c, d, {foo: bar, money: pennys}])", &tree);
        CHECK(mem.alloc_size == size_before);
        CHECK(mem.num_allocs == 3);
    }
    mem.check_and_reset();

    // restore defaults.
    ryml::set_callbacks(defaults);
}


//-----------------------------------------------------------------------------

struct PerTreeMemoryExample : public ryml::MemoryResource
{
    std::vector<char> memory_pool = std::vector<char>(10u * 1024u); // 10KB
    size_t num_allocs = 0, alloc_size = 0;
    size_t num_deallocs = 0, dealloc_size = 0;

    void *allocate(size_t len, void * /*hint*/) override
    {
        void *ptr = &memory_pool[alloc_size];
        alloc_size += len;
        ++num_allocs;
        if(C4_UNLIKELY(alloc_size > memory_pool.size()))
        {
            std::cerr << "out of memory! requested=" << alloc_size << " vs " << memory_pool.size() << " available" << std::endl;
            std::abort();
        }
        return ptr;
    }

    void free(void *mem, size_t len) override
    {
        CHECK((char*)mem     >= &memory_pool.front() && (char*)mem     <  &memory_pool.back());
        CHECK((char*)mem+len >= &memory_pool.front() && (char*)mem+len <= &memory_pool.back());
        dealloc_size += len;
        ++num_deallocs;
        // no need to free here
    }

    // checking
    ~PerTreeMemoryExample()
    {
        check_and_reset();
    }
    void check_and_reset()
    {
        std::cout << "size: alloc=" << alloc_size << " dealloc=" << dealloc_size << std::endl;
        std::cout << "count: #allocs=" << num_allocs << " #deallocs=" << num_deallocs << std::endl;
        CHECK(num_allocs == num_deallocs);
        CHECK(alloc_size >= dealloc_size); // failure here means a double free
        CHECK(alloc_size == dealloc_size); // failure here means a leak
        num_allocs = 0;
        num_deallocs = 0;
        alloc_size = 0;
        dealloc_size = 0;
    }
};

void sample_per_tree_allocator()
{
    PerTreeMemoryExample mrp;
    PerTreeMemoryExample mr1;
    PerTreeMemoryExample mr2;

    // the trees will use the memory in the resources above,
    // with each tree using a separate resource
    {
        // Watchout: ensure that the lifetime of the memory resource
        // exceeds the lifetime of the tree.
        ryml::Parser parser = {ryml::Allocator(&mrp)};
        ryml::Tree   tree1  = {ryml::Allocator(&mr1)};
        ryml::Tree   tree2  = {ryml::Allocator(&mr2)};

        ryml::csubstr yml1 = "{a: b}";
        ryml::csubstr yml2 = "{c: d, e: f, g: [h, i, 0, 1, 2, 3]}";

        parser.parse("file1.yml", yml1, &tree1);
        parser.parse("file2.yml", yml2, &tree2);
    }

    CHECK(mrp.num_allocs == 0); // YAML depth not large enough to warrant a parser allocation
    CHECK(mr1.alloc_size <= mr2.alloc_size); // because yml2 has more nodes
}


//-----------------------------------------------------------------------------

namespace /*anon*/ {
static int num_checks = 0;
static int num_failed_checks = 0;
} // namespace /*anon*/


bool report_check(const char *file, int line, const char *predicate, bool result)
{
    ++num_checks;
    const char *msg = "OK! ";
    if(!result)
    {
        ++num_failed_checks;
        msg = "ERROR: ";
    }
    std::cout << file << ':' << line << ": " << msg << predicate << std::endl;
    return result;
}


int report_checks()
{
    std::cout << "Completed " << num_checks << " checks." << std::endl;
    if(num_failed_checks)
        std::cout << "ERROR: " << num_failed_checks << '/' << num_checks << " checks failed." << std::endl;
    else
        std::cout << "SUCCESS!" << std::endl;
    return num_failed_checks;
}


// helper functions for sample_parse_file()

C4_SUPPRESS_WARNING_MSVC_WITH_PUSH(4996) // fopen: this function or variable may be unsafe
/** load a file from disk and return a newly created CharContainer */
template<class CharContainer>
size_t file_get_contents(const char *filename, CharContainer *v)
{
    ::FILE *fp = ::fopen(filename, "rb");
    C4_CHECK_MSG(fp != nullptr, "could not open file");
    ::fseek(fp, 0, SEEK_END);
    long sz = ::ftell(fp);
    v->resize(static_cast<typename CharContainer::size_type>(sz));
    if(sz)
    {
        ::rewind(fp);
        size_t ret = ::fread(&(*v)[0], 1, v->size(), fp);
        C4_CHECK(ret == (size_t)sz);
    }
    ::fclose(fp);
    return v->size();
}

/** load a file from disk into an existing CharContainer */
template<class CharContainer>
CharContainer file_get_contents(const char *filename)
{
    CharContainer cc;
    file_get_contents(filename, &cc);
    return cc;
}

/** save a buffer into a file */
template<class CharContainer>
void file_put_contents(const char *filename, CharContainer const& v, const char* access)
{
    file_put_contents(filename, v.empty() ? "" : &v[0], v.size(), access);
}

/** save a buffer into a file */
void file_put_contents(const char *filename, const char *buf, size_t sz, const char* access)
{
    ::FILE *fp = ::fopen(filename, access);
    C4_CHECK_MSG(fp != nullptr, "could not open file");
    ::fwrite(buf, 1, sz, fp);
    ::fclose(fp);
}
C4_SUPPRESS_WARNING_MSVC_POP

} // namespace sample
