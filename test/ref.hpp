#ifndef _REF_HPP_
#define _REF_HPP_


#include <vector>
#include <map>

#include <ryml/ryml.hpp>

namespace c4 {
namespace yml {

/** */
class RefNode
{
public:

    using seq_type = std::vector< RefNode >;
    using seq_value_type = seq_type::value_type;
    using seq_init_type = std::initializer_list< seq_value_type >;

    using map_type = std::map< cspan, RefNode >;
    using map_value_type = map_type::value_type;
    using map_init_type = std::initializer_list< map_value_type >;

public:

    RefNode(RefNode     &&) = default;
    RefNode(RefNode const&) = default;

    RefNode& operator= (RefNode     &&) = default;
    RefNode& operator= (RefNode const&) = default;

    RefNode() : type(TYPE_NONE), key(), value(), map(), seq() {}

    template< size_t N >
    RefNode(const char (&v)[N]) : type(TYPE_VAL), key(), value(v), map(), seq() {}
    RefNode(cspan const& v) : type(TYPE_VAL), key(), value(v), map(), seq() {}

    template< size_t N, size_t M >
    RefNode(const char (&k)[N], const char (&v)[M]) : type(TYPE_VAL), key(k), value(v), map(), seq() {}
    RefNode(cspan const& k, cspan const& v) : type(TYPE_VAL), key(k), value(v), map(), seq() {}

    template< size_t N >
    RefNode(const char (&k)[N], map_init_type m) : RefNode(TYPE_MAP, k, m) {}
    RefNode(cspan const& k, map_init_type m) : RefNode(TYPE_MAP, k, m) {}

    template< size_t N >
    RefNode(NodeType_e t,  const char (&k)[N], map_init_type m) : type(t), key(k), value(), map(m), seq() {}
    RefNode(NodeType_e t, cspan const& k, map_init_type m) : type(t), key(k), value(), map(m), seq() {}

    RefNode(map_init_type m) : RefNode(TYPE_MAP, {}, m) {}
    RefNode(NodeType_e t, map_init_type m) : RefNode(t, {}, m) {}

    template< size_t N >
    RefNode( const char (&k)[N], seq_init_type m) : RefNode(TYPE_SEQ, k, m) {}
    RefNode(cspan const& k, seq_init_type m) : RefNode(TYPE_SEQ, k, m) {}

    template< size_t N >
    RefNode(NodeType_e t,  const char (&k)[N], seq_init_type s) : type(t), key(k), value(), map(), seq(s) {}
    RefNode(NodeType_e t, cspan const& k, seq_init_type s) : type(t), key(k), value(), map(), seq(s) {}

    RefNode(seq_init_type m) : RefNode(TYPE_SEQ, {}, m) {}
    RefNode(NodeType_e t, seq_init_type m) : RefNode(t, {}, m) {}

public:

    NodeType_e type;
    cspan key;
    cspan value;
    map_type map;
    seq_type seq;

    bool is_val() const { return type == TYPE_VAL && key.empty(); }
    bool is_key_val() const { return type == TYPE_VAL && ! key.empty(); }
    bool is_map() const { return type == TYPE_MAP || (type == TYPE_DOC && ! map.empty()); }
    bool is_seq() const { return type == TYPE_SEQ || (type == TYPE_SEQ && ! map.empty()) || type == TYPE_ROOT; }

    bool test() const
    {
        return true;
    }

    /*
    void print(int level = 0) const
    {
        if(is_val())
        {
            printf("%.*s", (int)key.len, key.str);
            return;
        }
        else if(is_key_val())
        {
            printf("%.*s", (int)key.len, key.str);
            return;
        }
        ++level;
        if(is_seq())
        {
            printf("\n");
            for(auto const& c : seq)
            {
                printf("%*s", level, "  ");
                printf("%.*s", (int)key.len, key.str);
                c.print(level);
            }
        }
        else if(is_map())
        {
            ++level;
            for(auto const& c : map)
            {
                c.print(level);
            }
        }
    }
*/

};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct Ref
{
    cspan file;
    RefNode root;

    template< size_t N >
    Ref(const char (&f)[N], RefNode const& r) : file(f), root(r)
    {
    }

    bool test() const
    {
        return root.test();
    }
};


template< size_t N, class... Args >
Ref mkref(const char (&txt)[N], Args&&... a)
{
    return Ref(txt, RefNode(TYPE_ROOT, std::forward< Args >(a)...));
}


} // namespace yml
} // namespace c4

#endif /* _REF_HPP_ */
