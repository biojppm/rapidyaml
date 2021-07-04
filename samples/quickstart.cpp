// This file does a quick tour of ryml. It has multiple self-contained
// and commented samples that illustrate how to use ryml, and how it
// works. Although this is not a unit test, the samples are written as
// a sequence of instructions and predicate checks to better convey
// what is the expected result at any stage. And to ensure the code
// here is correct and up to date, it's also run as part of the CI
// tests.
//
// The directories that exist side-by-side with the file contain
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

// a quick'n'dirty assertion to verify a predicate
#define CHECK(predicate)                                                \
    do                                                                  \
    {                                                                   \
        const char *result##__LINE__ = nullptr;                         \
        printf("%s:%d: %s " #predicate "\n", __FILE__, __LINE__,        \
               (predicate) ? (result##__LINE__="OK!") : "ERR:");        \
        fflush(stdout);                                                 \
        if(!result##__LINE__)                                           \
            ++num_failed_checks;                                        \
    } while(0)
static int num_failed_checks = 0;


//-----------------------------------------------------------------------------

void sample_quick_overview();
void sample_substr();
void sample_parse_read_only();
void sample_parse_in_situ();
void sample_parse_reuse_tree();
void sample_parse_reuse_parser();
void sample_parse_reuse_tree_and_parser();
void sample_iterate_trees();
void sample_create_trees();
void sample_tree_arena();
void sample_fundamental_types();
void sample_formatting();
void sample_base64();
void sample_user_scalar_types();
void sample_user_container_types();
void sample_std_types();
void sample_emit_to_container();
void sample_emit_to_stream();
void sample_emit_to_file();
void sample_emit_nested_node();
void sample_anchors_and_aliases();
void sample_tags();
void sample_docs();


int main()
{
    sample_quick_overview();
    sample_substr();
    sample_parse_read_only();
    sample_parse_in_situ();
    sample_parse_reuse_tree();
    sample_parse_reuse_parser();
    sample_parse_reuse_tree_and_parser();
    sample_iterate_trees();
    sample_create_trees();
    sample_tree_arena();
    sample_fundamental_types();
    sample_formatting();
    sample_base64();
    sample_user_scalar_types();
    sample_user_container_types();
    sample_std_types();
    sample_emit_to_container();
    sample_emit_to_stream();
    sample_emit_to_file();
    sample_emit_nested_node();
    sample_anchors_and_aliases();
    sample_tags();
    sample_docs();
    return num_failed_checks;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void sample_quick_overview()
{
    // Parse YAML code in place, potentially mutating the buffer.
    // It is also possible to:
    //   - parse a read-only buffer
    //   - reuse an existing tree
    //   - reuse an existing parser.
    char yml_buf[] = "{foo: 1, bar: [2, 3], john: doe}";
    ryml::Tree tree = ryml::parse(ryml::substr(yml_buf));


    //------------------------------------------------------------------
    // To read the parsed tree.
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
    ryml::NodeRef bar = tree["bar"];
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


    //------------------------------------------------------------------
    // Hierarchy:
    ryml::NodeRef root = tree.rootref();
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
        // iterate children:
        {
            size_t count = 0;
            for(ryml::NodeRef child : root.children())
                CHECK(child.key() == expected_keys[count++]);
        }
        // iterate siblings:
        {
            size_t count = 0;
            for(ryml::NodeRef child : root["foo"].siblings())
                CHECK(child.key() == expected_keys[count++]);
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
        root["john"] >> john; // requires from_chars(std::string). see samples below.
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
    //     std::string crash("dangling");
    //     root["john"] == ryml::to_csubstr(crash);
    // }
    // CHECK(root["john"] == "dangling"); // CRASH! the string was deallocated

    // operator<< first serializes the input to the tree's arena, then
    // assigns the serialized string to the receiving node. This avoids
    // constraints with the lifetime, since the arena lives with the tree.
    CHECK(tree.arena().empty());
    root["foo"] << "says who";
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
        // root["john"] == ryml::to_csubstr(ok); // don't, will dangle
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
 * These types are imported from the
 * c4core library into the ryml namespace
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

    // substr is mutable:
    {
        char buf[] = "foobar";
        ryml::substr foobar = buf;
        CHECK(foobar == "foobar");
        foobar[0] = 'F';
        CHECK(foobar == "Foobar");
        foobar.back() = 'R';
        CHECK(foobar == "FoobaR");
        foobar.reverse();
        CHECK(foobar == "RabooF");
        foobar.reverse();
        CHECK(foobar == "FoobaR");
        foobar.replace('o', '0');
        CHECK(foobar == "F00baR");
        foobar.replace('a', '_');
        CHECK(foobar == "F00b_R");
        foobar.replace("_0b", 'a');
        CHECK(foobar == "FaaaaR");
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

    // begins_with()/ends_with()
    {
        ryml::csubstr s = "foobar123";
        // char* overloads
        CHECK(s.begins_with("foobar"));
        CHECK(s.ends_with("123"));
        // char overloads
        CHECK(s.begins_with('f'));
        CHECK(s.ends_with('3'));
    }

    // ...

    // many more facilities:
    // see https://c4core.docsforge.com/master/api/c4/basic_substring/
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

/** demonstrate in-place parsing of a YAML source buffer */
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
    ryml::parse("{foo: 1, bar: [2, 3]}", &tree);
    auto root = tree.rootref();

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
        for(auto const& n : root.children())
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
        // etc.
        CHECK(root[5].has_key());
        CHECK(!root[5].has_val()); // it is a map so not a val
        CHECK(root[5].key() == "calling-birds");
        //CHECK(root[5].val() == ""); // RUNTIME ERROR! node does not have a val.
        CHECK(keys[5] == "calling-birds");
        CHECK(vals[5] == "");
    }

    // iterate siblings
    {
    }
}


//-----------------------------------------------------------------------------

/** shows how to programatically create trees */
void sample_create_trees()
{
    // TODO
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
    CHECK(tree.to_arena(uint16_t(1)) == "1");       CHECK(tree.arena() == "abcde0101");
    CHECK(tree.to_arena(uint32_t(2)) == "2");       CHECK(tree.arena() == "abcde01012");
    CHECK(tree.to_arena(uint64_t(3)) == "3");       CHECK(tree.arena() == "abcde010123");
    CHECK(tree.to_arena(int8_t(4)) == "4");         CHECK(tree.arena() == "abcde0101234");
    CHECK(tree.to_arena(int8_t(-4)) == "-4");       CHECK(tree.arena() == "abcde0101234-4");
    CHECK(tree.to_arena(int16_t(5)) == "5");        CHECK(tree.arena() == "abcde0101234-45");
    CHECK(tree.to_arena(int16_t(-5)) == "-5");      CHECK(tree.arena() == "abcde0101234-45-5");
    CHECK(tree.to_arena(int32_t(6)) == "6");        CHECK(tree.arena() == "abcde0101234-45-56");
    CHECK(tree.to_arena(int32_t(-6)) == "-6");      CHECK(tree.arena() == "abcde0101234-45-56-6");
    CHECK(tree.to_arena(int8_t(7)) == "7");         CHECK(tree.arena() == "abcde0101234-45-56-67");
    CHECK(tree.to_arena(int8_t(-7)) == "-7");       CHECK(tree.arena() == "abcde0101234-45-56-67-7");
    CHECK(tree.to_arena((void*)1) == "0x1");        CHECK(tree.arena() == "abcde0101234-45-56-67-70x1");
    CHECK(tree.to_arena(float(0.124)) == "0.124");  CHECK(tree.arena() == "abcde0101234-45-56-67-70x10.124");
    CHECK(tree.to_arena(double(0.248)) == "0.248"); CHECK(tree.arena() == "abcde0101234-45-56-67-70x10.1240.248");
    CHECK(tree.to_arena(bool(true)) == "1");        CHECK(tree.arena() == "abcde0101234-45-56-67-70x10.1240.2481");
    CHECK(tree.to_arena(bool(false)) == "0");       CHECK(tree.arena() == "abcde0101234-45-56-67-70x10.1240.24810");
    CHECK(tree.to_arena(ryml::fmt::boolalpha(bool(true))) == "true");   CHECK(tree.arena() == "abcde0101234-45-56-67-70x10.1240.24810true");
    CHECK(tree.to_arena(ryml::fmt::boolalpha(bool(false))) == "false"); CHECK(tree.arena() == "abcde0101234-45-56-67-70x10.1240.24810truefalse");
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
        char buf_[256] = {};
        ryml::substr buf = buf_;
        using namespace ryml;  // all the symbols below are in the ryml namespace.
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
            const uint32_t payload[] = {10, 20, 30, 40, UINT32_C(0xdeadbeef)};
            // (package payload as a substr, for comparison only)
            csubstr expected = csubstr((const char *)payload, sizeof(payload));
            csubstr actual = cat_sub(buf, fmt::raw(payload));
            CHECK(!actual.overlaps(expected));
            CHECK(0 == memcmp(expected.str, actual.str, expected.len));
            // also possible with variables:
            for(const uint32_t pl : payload)
            {
                // (package payload as a substr, for comparison only)
                expected = csubstr((const char *)&pl, sizeof(pl));
                actual = cat_sub(buf, fmt::raw(pl));
                CHECK(actual.size() == sizeof(uint32_t));
                CHECK(!actual.overlaps(expected));
                CHECK(0 == memcmp(expected.str, actual.str, expected.len));
                // read back:
                uint32_t result = pl + 10000; // anything different.
                auto reader = fmt::raw(result); // keeps a reference to result
                uncat(actual, reader);
                CHECK(result == pl); // roundtrip completed successfully
            }
        }
        // with non-const data, fmt::craw() may be needed for disambiguation:
        {
            uint32_t payload[] = {10, 20, 30, 40, UINT32_C(0xdeadbeef)};
            // (package payload as a substr, for comparison only)
            csubstr expected = csubstr((const char *)payload, sizeof(payload));
            csubstr actual = cat_sub(buf, fmt::craw(payload));
            CHECK(!actual.overlaps(expected));
            CHECK(0 == memcmp(expected.str, actual.str, expected.len));
            // also possible with variables:
            for(uint32_t pl : payload)
            {
                // (package payload as a substr, for comparison only)
                expected = csubstr((const char *)&pl, sizeof(pl));
                actual = cat_sub(buf, fmt::craw(pl));
                CHECK(actual.size() == sizeof(uint32_t));
                CHECK(!actual.overlaps(expected));
                CHECK(0 == memcmp(expected.str, actual.str, expected.len));
                // read back:
                uint32_t result = pl + 10000; // anything different.
                auto reader = fmt::raw(result); // keeps a reference to result
                uncat(actual, reader);
                CHECK(result == pl); // roundtrip completed successfully
            }
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
    for(auto c : cases)
    {
        tree[c.text] << ryml::fmt::base64(c.text);
        CHECK(tree[c.text].val() == c.base64);
    }
    // to encode base64 and write the result to key:
    for(auto c : cases)
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
    for(auto c : cases)
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
    define the functions above for those types */
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

/** just like you can parse into a nested node, you can also emit from a nested node. */
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

/** demonstrates usage with anchors and alias references */
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
*valref: key
val is from: *keyref
)";

    std::string resolved = R"(base:
  name: Everyone has same name
foo:
  name: Everyone has same name
  age: 10
bar:
  name: Everyone has same name
  age: 20
bill_to:
  street: |-
    123 Tornado Alley
    Suite 16
  city: East Centerville
  state: KS
ship_to:
  street: |-
    123 Tornado Alley
    Suite 16
  city: East Centerville
  state: KS
key: val
val: key
val is from: val
)";

    ryml::Tree tree = ryml::parse(ryml::to_csubstr(unresolved));
    // by default, references are not resolved when parsing:
    CHECK(ryml::emitrs<std::string>(tree) == unresolved);
    CHECK( ! tree["base"].has_key_anchor());
    CHECK(   tree["base"].has_val_anchor());
    CHECK(   tree["base"].val_anchor() == "base");
    CHECK(   tree["key"].key_anchor() == "keyref");
    CHECK(   tree["key"].val_anchor() == "valref");
    CHECK(   tree["*valref"].key_ref() == "valref");
    CHECK( ! tree["*valref"].is_val_ref());
    CHECK( ! tree["val is from"].is_key_ref());
    CHECK(   tree["val is from"].is_val_ref());

    // to resolve references, simply call tree.resolve(),
    // which will perform the reference instantiations:
    tree.resolve();
    CHECK(ryml::emitrs<std::string>(tree) == resolved);
    // all the anchors and references are substistuted and then removed:
    CHECK( ! tree["base"].has_key_anchor());
    CHECK( ! tree["base"].has_val_anchor());
    CHECK( ! tree["base"].has_val_anchor());
    CHECK( ! tree["key"].has_key_anchor());
    CHECK( ! tree["key"].has_val_anchor());
    CHECK( ! tree["val"].is_key_ref()); // notice *valref is now turned to val
    CHECK( ! tree["val"].is_val_ref()); // notice *valref is now turned to val
    CHECK( ! tree["val is from"].is_key_ref());
    CHECK( ! tree["val is from"].is_val_ref());
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
        // equivalent: using the lower level index API
        const size_t stream_id = tree.root_id();
        CHECK(tree.is_root(stream_id));
        CHECK(tree.is_stream(stream_id));
        CHECK(!tree.is_doc(stream_id));
        CHECK(tree.num_children(stream_id) == 3);
        for(size_t doc_id = tree.first_child(stream_id); doc_id != ryml::NONE; doc_id = tree.next_sibling(stream_id))
            CHECK(tree.is_doc(doc_id));

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

    // emitting streams as json is not possible, but...
    {
        //CHECK(ryml::emitrs_json<std::string>(tree) == yml); // RUNTIME ERROR!
        // ... since json does not have streams, you cannot emit the above
        // tree as json when you start from the root.

        // but you can iterate through individual documents and emit
        // them separately:
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
