#ifndef _C4_YML_EVENT_HANDLER_TREE_HPP_
#define _C4_YML_EVENT_HANDLER_TREE_HPP_

#ifndef _C4_YML_TREE_HPP_
#include "c4/yml/tree.hpp"
#endif

#ifndef _C4_YML_DETAIL_STACK_HPP_
#include "c4/yml/detail/stack.hpp"
#endif

#ifndef _C4_YML_DETAIL_PARSER_DBG_HPP_
#include "c4/yml/detail/parser_dbg.hpp"
#endif

#ifndef _C4_YML_PARSER_STATE_HPP_
#include "c4/yml/parser_state.hpp"
#endif


namespace c4 {
namespace yml {


struct EventHandlerTree
{
    static constexpr const bool is_events = false; // remove
    static constexpr const bool is_wtree = true;

    // our internal state must inherit from parser state
    struct HandlerState : public ParserState
    {
        size_t tr_id;
        NodeData *tr_data;
    };

    using state = HandlerState;

public:

    detail::stack<state> m_stack;
    state *C4_RESTRICT m_curr;
    state *C4_RESTRICT m_parent;

public: // used only for wtree

    Tree *C4_RESTRICT m_tree;
    size_t m_id;

public:

    EventHandlerTree() : m_stack(), m_curr(), m_parent(), m_tree(), m_id(npos) {}
    EventHandlerTree(Callbacks const& cb) : m_stack(cb), m_curr(), m_parent(), m_tree(), m_id(npos) {}
    EventHandlerTree(Tree *tree, size_t id) : m_stack(tree->callbacks()), m_curr(), m_parent(), m_tree(tree), m_id(id)
    {
        reset(tree, id);
    }

    void reset(Tree *tree, size_t id)
    {
        m_tree = tree;
        m_id = id;
        m_stack.clear();
        auto _reset_parser_state = [&](state* st, size_t node){
            _tr_set_state_(st, node);
            const NodeType type = m_tree->type(node);
            #ifdef RYML_DBG
            char flagbuf[80];
            #endif
            _c4dbgpf("resetting state: initial flags={}", detail::_parser_flags_to_str(flagbuf, st->flags));
            if(type == NOTYPE)
            {
                _c4dbgpf("node[{}] is notype", node);
                if(m_tree->is_root(id))
                {
                    _c4dbgpf("node[{}] is root", node);
                    st->flags |= RUNK|RTOP;
                }
                else
                {
                    _c4dbgpf("node[{}] is not root. setting USTY", node);
                    st->flags |= USTY;
                }
            }
            else if(type.is_map())
            {
                _c4dbgpf("node[{}] is map", node);
                st->flags |= RMAP|USTY;
            }
            else if(type.is_seq())
            {
                _c4dbgpf("node[{}] is map", node);
                st->flags |= RSEQ|USTY;
            }
            else if(type.has_key())
            {
                _c4dbgpf("node[{}] has key. setting USTY", node);
                st->flags |= USTY;
            }
            else
            {
                _RYML_CB_ERR(m_stack.m_callbacks, "cannot append to node");
            }
            if(type.is_doc())
            {
                _c4dbgpf("node[{}] is doc", node);
                st->flags |= RDOC;
            }
            _c4dbgpf("resetting state: final flags={}", detail::_parser_flags_to_str(flagbuf, st->flags));
        };
        if(m_tree->is_root(id))
        {
            m_stack.push({});
            m_parent = nullptr;
            m_curr = &m_stack.top();
            _reset_parser_state(m_curr, m_tree->root_id());
        }
        else
        {
            m_stack.push({}); // parent
            m_stack.push({}); // node
            m_parent = &m_stack.top(1);
            m_curr = &m_stack.top();
            _reset_parser_state(m_parent, m_tree->parent(id));
            _reset_parser_state(m_curr, id);
        }
    }

    void start_parse(const char* filename)
    {
        m_curr->start_parse(filename, m_curr->tr_id);
    }

    void finish_parse()
    {
    }

    void cancel_parse()
    {
    }

public:

    /** push a new parent, add a child to the new parent, and set the
     * child as the current node */
    void _push()
    {
        m_stack.push_top();
        m_parent = &m_stack.top(1); // don't use m_curr. watch out for relocations inside the prev push
        m_curr = &m_stack.top();
        m_curr->reset_after_push();
        {
            NodeData const* prev = m_tree->m_buf; // watch out against relocation of the tree nodes
            m_curr->tr_id = m_tree->_append_child__unprotected(m_parent->tr_id);
            m_curr->tr_data = m_tree->_p(m_curr->tr_id);
            if(prev != m_tree->m_buf)
                _tr_refresh_after_relocation();
            _c4dbgpf("pushed! level={}. top is now node={} (parent={})", m_curr->level, m_curr->tr_id, m_parent ? m_parent->tr_id : npos);
        }
    }

    /** end the current scope */
    void _pop()
    {
        _RYML_CB_ASSERT(m_stack.m_callbacks, m_parent);
        _RYML_CB_ASSERT(m_stack.m_callbacks, m_stack.size() > 1);
        {
            _tr_remove_speculative_with_parent();
        }
        m_parent->reset_before_pop(*m_curr);
        m_stack.pop();
        m_parent = m_stack.size() > 1 ? &m_stack.top(1) : nullptr;
        m_curr = &m_stack.top();
        #ifdef RYML_DBG
        {
            if(m_parent)
                _c4dbgpf("popped! top is now node={} (parent={})", m_curr->tr_id, m_parent->tr_id);
            else
                _c4dbgpf("popped! top is now node={} @ ROOT", m_curr->tr_id);
        }
        #endif
    }

    template<type_bits bits> C4_ALWAYS_INLINE void _enable__() noexcept
    {
        m_curr->tr_data->m_type.type = static_cast<NodeType_e>(m_curr->tr_data->m_type.type | bits);
    }
    template<type_bits bits> C4_ALWAYS_INLINE void _disable__() noexcept
    {
        m_curr->tr_data->m_type.type = static_cast<NodeType_e>(m_curr->tr_data->m_type.type & (~bits));
    }
    template<type_bits bits> C4_ALWAYS_INLINE bool _has_any__() const noexcept
    {
        return (m_curr->tr_data->m_type.type & bits) != 0;
    }

    #if RYML_DBG
    #define _enable_(bits) _enable__<bits>(); _c4dbgpf("node[{}]: enable {}", m_curr->tr_id, #bits)
    #define _disable_(bits) _disable__<bits>(); _c4dbgpf("node[{}]: disable {}", m_curr->tr_id, #bits)
    #else
    #define _enable_(bits) _enable__<bits>()
    #define _disable_(bits) _disable__<bits>()
    #endif
    #define _has_any_(bits) _has_any__<bits>()

public:

    C4_ALWAYS_INLINE void _tr_set_state_(state *C4_RESTRICT s, size_t id) noexcept
    {
        s->tr_id = id;
        s->tr_data = m_tree->_p(id);
    }
    void _tr_refresh_after_relocation()
    {
        _c4dbgp("tree: refreshing stack data after tree data relocation");
        for(auto &st : m_stack)
            st.tr_data = m_tree->_p(st.tr_id);
    }

    void _tr_set_root_as_stream()
    {
        _c4dbgp("set root as stream");
        _RYML_CB_ASSERT(m_stack.m_callbacks, m_tree->root_id() == 0u);
        _RYML_CB_ASSERT(m_stack.m_callbacks, m_curr->tr_id == 0u);
        const bool hack = !m_tree->has_children(m_curr->tr_id) && !m_tree->is_val(m_curr->tr_id);
        if(hack)
            m_tree->_p(m_tree->root_id())->m_type.add(VAL);
        m_tree->set_root_as_stream();
        _RYML_CB_ASSERT(m_stack.m_callbacks, m_tree->is_stream(m_tree->root_id()));
        _RYML_CB_ASSERT(m_stack.m_callbacks, m_tree->has_children(m_tree->root_id()));
        _RYML_CB_ASSERT(m_stack.m_callbacks, m_tree->is_doc(m_tree->first_child(m_tree->root_id())));
        if(hack)
            m_tree->_p(m_tree->first_child(m_tree->root_id()))->m_type.rem(VAL);
        _tr_set_state_(m_curr, m_tree->root_id());
    }

    static NodeData _tr_val2key_(NodeData const& C4_RESTRICT d) noexcept
    {
        NodeData r = d;
        r.m_key = d.m_val;
        r.m_val = {};
        r.m_type = d.m_type;
        static_assert((_VALMASK >> 1u) == _KEYMASK, "required for this function to work");
        static_assert((_WIP_VAL_STYLE >> 1u) == _WIP_KEY_STYLE, "required for this function to work");
        r.m_type.type = ((d.m_type.type & (_VALMASK|_WIP_VAL_STYLE)) >> 1u);
        r.m_type.type = (r.m_type.type & ~(_VALMASK|_WIP_VAL_STYLE));
        r.m_type.type = (r.m_type.type | KEY);
        return r;
    }

    void _tr_remove_speculative()
    {
        _c4dbgp("remove speculative node");
        _RYML_CB_ASSERT(m_stack.m_callbacks, m_tree->size() > 0);
        const size_t last_added = m_tree->size() - 1u;
        if(m_tree->has_parent(last_added))
            if(m_tree->_p(last_added)->m_type == NOTYPE)
                m_tree->remove(last_added);
    }

    void _tr_remove_speculative_with_parent()
    {
        _RYML_CB_ASSERT(m_stack.m_callbacks, m_tree->size() > 0);
        const size_t last_added = m_tree->size() - 1u;
        _RYML_CB_ASSERT(m_stack.m_callbacks, m_tree->has_parent(last_added));
        if(m_tree->_p(last_added)->m_type == NOTYPE)
        {
            _c4dbgpf("remove speculative node with parent. parent={} node={} parent(node)={}", m_parent->tr_id, last_added, m_tree->parent(last_added));
            m_tree->remove(last_added);
        }
    }

    C4_ALWAYS_INLINE void _tr_save_loc()
    {
        m_tree->_p(m_curr->tr_id)->m_val.scalar.str = m_curr->line_contents.rem.str;
    }

    bool _should_push_on_begin_doc() const
    {
        const bool is_root = (m_stack.size() == 1u);
        {
            _RYML_CB_ASSERT(m_stack.m_callbacks, is_root == m_tree->is_root(m_curr->tr_id));
            return is_root && (_has_any_(DOC) || m_tree->has_children(m_curr->tr_id) || m_tree->is_val(m_curr->tr_id));
        }
    }

    bool _should_pop_on_end_doc() const
    {
        const bool is_root = (m_stack.size() == 1u);
        return !is_root && _has_any_(DOC);
    }

public:

    /** @name YAML stream functions */
    /** @{ */

    void begin_stream()
    {
    }

    void end_stream()
    {
    }

    /** @} */

public:

    /** @name YAML document functions */
    /** @{ */

    /** called at the beginning of parsing a non-empty stream */
    void begin_doc()
    {
        _c4dbgp("begin_doc");
        {
            if(_should_push_on_begin_doc())
            {
                _c4dbgp("push!");
                _tr_set_root_as_stream();
                _push();
                _enable_(DOC);
            }
        }
    }
    /** called at the end of parsing, before @ref end_stream() */
    void end_doc()
    {
        _c4dbgp("end_doc");
        if(_should_pop_on_end_doc())
        {
            {
                _tr_remove_speculative();
            }
            _c4dbgp("pop!");
            _pop();
        }
    }

    /** explicit begin doc: called on `---` tokens */
    void begin_doc_expl()
    {
        _c4dbgp("begin_doc_expl");
        {
            _RYML_CB_ASSERT(m_stack.m_callbacks, m_tree->root_id() == m_curr->tr_id);
            if(!m_tree->is_stream(m_tree->root_id())) //if(_should_push_on_begin_doc())
            {
                _c4dbgp("ensure stream");
                _tr_set_root_as_stream();
                size_t first = m_tree->first_child(m_tree->root_id());
                _RYML_CB_ASSERT(m_stack.m_callbacks, m_tree->is_stream(m_tree->root_id()));
                _RYML_CB_ASSERT(m_stack.m_callbacks, m_tree->num_children(m_tree->root_id()) == 1u);
                if(m_tree->has_children(first) || m_tree->is_val(first))
                {
                    _c4dbgp("push!");
                    _push();
                }
                else
                {
                    _c4dbgp("tweak");
                    _push();
                    _tr_remove_speculative();
                    m_curr->tr_id = m_tree->last_child(m_tree->root_id());
                    m_curr->tr_data = m_tree->_p(m_curr->tr_id);
                }
            }
            else
            {
                _c4dbgp("push!");
                _push();
            }
        }
        _enable_(DOC);
    }
    /** explicit end doc: called on `...` tokens */
    void end_doc_expl()
    {
        _c4dbgp("end_doc_expl");
        {
            _tr_remove_speculative();
        }
        if(_should_pop_on_end_doc())
        {
            _c4dbgp("pop!");
            _pop();
        }
    }

    /** Check that the current tokens are not trailing on the previous
     * doc */
    void check_trailing_doc_token()
    {
        const bool isndoc = (m_curr->flags & NDOC) != 0;
        {
            const size_t id = m_curr->tr_id;
            const bool suspicious = (m_tree->is_container(id) || m_tree->is_val(id));
            _c4dbgpf("target={} isroot={} suspicious={} ndoc={}", id, m_tree->is_root(id), suspicious, isndoc);
            if((m_tree->is_root(id) || m_tree->is_doc(id)) && suspicious && !isndoc)
                _RYML_CB_ERR_(m_stack.m_callbacks, "parse error", m_curr->pos);
        }
    }

    /** @} */

public:

    /** @name map functions */
    /** @{ */

    void begin_map_key_flow()
    {
        _RYML_CB_ERR_(m_stack.m_callbacks, "ryml trees cannot handle containers as keys", m_curr->pos);
    }
    void begin_map_key_block()
    {
        _RYML_CB_ERR_(m_stack.m_callbacks, "ryml trees cannot handle containers as keys", m_curr->pos);
    }

    void begin_map_val_flow()
    {
        _enable_(MAP|_WIP_STYLE_FLOW_SL);
        _tr_save_loc();
        _push();
    }
    void begin_map_val_block()
    {
        _enable_(MAP|_WIP_STYLE_BLOCK);
        _tr_save_loc();
        _push();
    }

    void end_map()
    {
        _pop();
    }

    /** @} */

public:

    /** @name seq functions */
    /** @{ */

    void begin_seq_key_flow()
    {
        _RYML_CB_ERR_(m_stack.m_callbacks, "ryml trees cannot handle containers as keys", m_curr->pos);
    }
    void begin_seq_key_block()
    {
        _RYML_CB_ERR_(m_stack.m_callbacks, "ryml trees cannot handle containers as keys", m_curr->pos);
    }

    void begin_seq_val_flow()
    {
        _enable_(SEQ|_WIP_STYLE_FLOW_SL);
        _tr_save_loc();
        _push();
    }
    void begin_seq_val_block()
    {
        _enable_(SEQ|_WIP_STYLE_BLOCK);
        _tr_save_loc();
        _push();
    }

    void end_seq()
    {
        _pop();
    }

    /** set the previous val as the first key of a new map, with flow style.
     *
     * For example, consider an implicit map inside a seq: `[a: b, c:
     * d]` which is parsed as `[{a: b}, {c: d}]`. The standard event
     * sequence for this YAML would be the following:
     *
     * ```c++
     * handler.begin_seq_val_flow();
     * handler.begin_map_val_flow();
     * handler.set_key_scalar_plain("a");
     * handler.set_val_scalar_plain("b");
     * handler.end_map();
     * handler.add_sibling();
     * handler.begin_map_val_flow();
     * handler.set_key_scalar_plain("c");
     * handler.set_val_scalar_plain("d");
     * handler.end_map();
     * handler.end_seq();
     * ```
     *
     * The problem with this event sequence is that it forces the
     * parser to delay setting the val scalar (in this case "a" and
     * "c") until it knows whether the scalar is a key or a val. This
     * would require the parser to store the scalar until this
     * time. For instance, in the example above, the parser should
     * delay setting "a" and "c", because they are in fact keys and
     * not vals. Until then, the parser would have to store "a" and
     * "c" in its internal state. The downside is that this complexity
     * cost would apply even if there is no implicit map -- every val
     * in a seq would have to be delayed until one of the
     * disambiguating subsequent tokens ',-]:` is found.
     *
     * By calling this function, the parser can avoid this complexity,
     * by preemptively setting the scalar as a val. Then a call to
     * this function will create the map and rearrange the scalar as
     * key. Now the cost applies only once: when a seqimap starts. So
     * the following (easier and cheaper) event sequence below has the
     * same effect as the event sequence above:
     *
     * ```c++
     * handler.begin_seq_val_flow();
     * handler.set_val_scalar_plain("notmap");
     * handler.set_val_scalar_plain("a"); // preemptively set "a" as val!
     * handler.actually_as_block_flow();  // create a map, move the "a" val as the key of the first child of the new map
     * handler.set_val_scalar_plain("b"); // now "a" is a key and "b" the val
     * handler.end_map();
     * handler.set_val_scalar_plain("c"); // "c" also as val!
     * handler.actually_as_block_flow();  // likewise
     * handler.set_val_scalar_plain("d"); // now "c" is a key and "b" the val
     * handler.end_map();
     * handler.end_seq();
     * ```
     *
     * This also applies to container keys (although ryml's tree
     * cannot accomodate these): the parser can preemptively set a
     * container as a val, and call this event to turn that container
     * into a key. For example, consider this yaml:
     *
     * ```yaml
     * [aa, bb]: [cc, ddd]
     * ^       ^ ^
     * |       | |
     * (2)   (1) (3)     <- event sequence
     * ```
     *
     * The standard event sequence for this YAML would be the
     * following:
     *
     * ```c++
     * handler.begin_map_val_block();       // (1)
     * handler.begin_seq_key_flow();        // (2)
     * handler.set_val_scalar_plain("aa");
     * handler.add_sibling();
     * handler.set_val_scalar_plain("bb");
     * handler.end_seq();
     * handler.begin_seq_val_flow();        // (3)
     * handler.set_val_scalar_plain("cc");
     * handler.add_sibling();
     * handler.set_val_scalar_plain("dd");
     * handler.end_seq();
     * handler.end_map();
     * ```
     *
     * The problem with the sequence above is that, reading from
     * left-to-right, the parser can only detect the proper calls at
     * (1) and (2) once it reaches (1) in the YAML source. So, the
     * parser would have to buffer the entire event sequence starting
     * from the beginning until it reaches (1). Using this function,
     * the parser can do instead:
     *
     * ```c++
     * handler.begin_seq_val_flow();               // (2) -- preemptively as val!
     * handler.set_val_scalar_plain("aa");
     * handler.add_sibling();
     * handler.set_val_scalar_plain("bb");
     * handler.end_seq();
     * handler.actually_val_is_new_map_key_flow(); // (1) -- adjust when finding that the prev val was actually a key.
     * handler.begin_seq_val_flow();               // (3) -- go on as before
     * handler.set_val_scalar_plain("cc");
     * handler.add_sibling();
     * handler.set_val_scalar_plain("dd");
     * handler.end_seq();
     * handler.end_map();
     * ```
     */
    void actually_val_is_first_key_of_new_map_flow()
    {
        {
            if(C4_UNLIKELY(m_tree->is_container(m_curr->tr_id)))
                _RYML_CB_ERR_(m_stack.m_callbacks, "ryml trees cannot handle containers as keys", m_curr->pos);
            _RYML_CB_ASSERT(m_stack.m_callbacks, m_parent);
            _RYML_CB_ASSERT(m_stack.m_callbacks, m_tree->is_seq(m_parent->tr_id));
            _RYML_CB_ASSERT(m_stack.m_callbacks, !m_tree->is_container(m_curr->tr_id));
            _RYML_CB_ASSERT(m_stack.m_callbacks, !m_tree->has_key(m_curr->tr_id));
            const NodeData tmp = _tr_val2key_(*m_curr->tr_data);
            _disable_(_VALMASK|_WIP_VAL_STYLE);
            m_curr->tr_data->m_val = {};
            begin_map_val_flow();
            m_curr->tr_data->m_type = tmp.m_type;
            m_curr->tr_data->m_key = tmp.m_key;
        }
    }

    /** like its flow counterpart, but this function can only be
     * called after the end of a flow-val at root or doc level. */
    void actually_val_is_first_key_of_new_map_block()
    {
        _RYML_CB_ERR_(m_stack.m_callbacks, "ryml trees cannot handle containers as keys", m_curr->pos);
    }

    /** @} */

public:

    /** @name scalar functions */
    /** @{ */

    void add_sibling()
    {
        _RYML_CB_ASSERT(m_stack.m_callbacks, m_parent);
        {
            _RYML_CB_ASSERT(m_stack.m_callbacks, m_tree->has_children(m_parent->tr_id));
            NodeData const* prev = m_tree->m_buf; // watchout against relocation of the tree nodes
            _tr_set_state_(m_curr, m_tree->_append_child__unprotected(m_parent->tr_id));
            if(prev != m_tree->m_buf)
                _tr_refresh_after_relocation();
            _c4dbgpf("node[{}]: added sibling={} prev={}", m_parent->tr_id, m_curr->tr_id, m_tree->prev_sibling(m_curr->tr_id));
        }
    }

    C4_ALWAYS_INLINE void set_key_scalar_plain(csubstr scalar)
    {
        _c4dbgpf("node[{}]: set key scalar plain: [{}]~~~{}~~~ ({})", m_curr->tr_id, scalar.len, scalar, reinterpret_cast<void const*>(scalar.str));
        {
            m_curr->tr_data->m_key.scalar = scalar;
        }
        _enable_(KEY|_WIP_KEY_PLAIN);
    }
    C4_ALWAYS_INLINE void set_val_scalar_plain(csubstr scalar)
    {
        _c4dbgpf("node[{}]: set val scalar plain: [{}]~~~{}~~~ ({})", m_curr->tr_id, scalar.len, scalar, reinterpret_cast<void const*>(scalar.str));
        {
            m_curr->tr_data->m_val.scalar = scalar;
        }
        _enable_(VAL|_WIP_VAL_PLAIN);
    }

    C4_ALWAYS_INLINE void set_key_scalar_dquoted(csubstr scalar)
    {
        _c4dbgpf("node[{}]: set key scalar dquot: [{}]~~~{}~~~ ({})", m_curr->tr_id, scalar.len, scalar, reinterpret_cast<void const*>(scalar.str));
        {
            m_curr->tr_data->m_key.scalar = scalar;
        }
        _enable_(KEY|_WIP_KEY_DQUO);
    }
    C4_ALWAYS_INLINE void set_val_scalar_dquoted(csubstr scalar)
    {
        _c4dbgpf("node[{}]: set val scalar dquot: [{}]~~~{}~~~ ({})", m_curr->tr_id, scalar.len, scalar, reinterpret_cast<void const*>(scalar.str));
        {
            m_curr->tr_data->m_val.scalar = scalar;
        }
        _enable_(VAL|_WIP_VAL_DQUO);
    }

    C4_ALWAYS_INLINE void set_key_scalar_squoted(csubstr scalar)
    {
        _c4dbgpf("node[{}]: set key scalar squot: [{}]~~~{}~~~ ({})", m_curr->tr_id, scalar.len, scalar, reinterpret_cast<void const*>(scalar.str));
        {
            m_curr->tr_data->m_key.scalar = scalar;
        }
        _enable_(KEY|_WIP_KEY_SQUO);
    }
    C4_ALWAYS_INLINE void set_val_scalar_squoted(csubstr scalar)
    {
        _c4dbgpf("node[{}]: set val scalar squot: [{}]~~~{}~~~ ({})", m_curr->tr_id, scalar.len, scalar, reinterpret_cast<void const*>(scalar.str));
        {
            m_curr->tr_data->m_val.scalar = scalar;
        }
        _enable_(VAL|_WIP_VAL_SQUO);
    }

    C4_ALWAYS_INLINE void set_key_scalar_literal(csubstr scalar)
    {
        _c4dbgpf("node[{}]: set key scalar literal: [{}]~~~{}~~~ ({})", m_curr->tr_id, scalar.len, scalar, reinterpret_cast<void const*>(scalar.str));
        {
            m_curr->tr_data->m_key.scalar = scalar;
        }
        _enable_(KEY|_WIP_KEY_LITERAL);
    }
    C4_ALWAYS_INLINE void set_val_scalar_literal(csubstr scalar)
    {
        _c4dbgpf("node[{}]: set val scalar literal: [{}]~~~{}~~~ ({})", m_curr->tr_id, scalar.len, scalar, reinterpret_cast<void const*>(scalar.str));
        {
            m_curr->tr_data->m_val.scalar = scalar;
        }
        _enable_(VAL|_WIP_VAL_LITERAL);
    }

    C4_ALWAYS_INLINE void set_key_scalar_folded(csubstr scalar)
    {
        _c4dbgpf("node[{}]: set key scalar folded: [{}]~~~{}~~~ ({})", m_curr->tr_id, scalar.len, scalar, reinterpret_cast<void const*>(scalar.str));
        {
            m_curr->tr_data->m_key.scalar = scalar;
        }
        _enable_(KEY|_WIP_KEY_FOLDED);
    }
    C4_ALWAYS_INLINE void set_val_scalar_folded(csubstr scalar)
    {
        _c4dbgpf("node[{}]: set val scalar folded: [{}]~~~{}~~~ ({})", m_curr->tr_id, scalar.len, scalar, reinterpret_cast<void const*>(scalar.str));
        {
            m_curr->tr_data->m_val.scalar = scalar;
        }
        _enable_(VAL|_WIP_VAL_FOLDED);
    }

    C4_ALWAYS_INLINE void mark_key_scalar_unfiltered()
    {
        _enable_(_WIP_KEY_UNFILT);
    }
    C4_ALWAYS_INLINE void mark_val_scalar_unfiltered()
    {
        _enable_(_WIP_VAL_UNFILT);
    }

    /** @} */

public:

    /** @name anchor/reference functions */
    /** @{ */

    void set_key_anchor(csubstr anchor)
    {
        _c4dbgpf("node[{}]: set key anchor: [{}]~~~{}~~~", m_curr->tr_id, anchor.len, anchor);
        RYML_ASSERT(!anchor.begins_with('&'));
        _enable_(KEYANCH);
        {
            m_curr->tr_data->m_key.anchor = anchor;
        }
    }
    void set_val_anchor(csubstr anchor)
    {
        _c4dbgpf("node[{}]: set val anchor: [{}]~~~{}~~~", m_curr->tr_id, anchor.len, anchor);
        RYML_ASSERT(!anchor.begins_with('&'));
        _enable_(VALANCH);
        {
            m_curr->tr_data->m_val.anchor = anchor;
        }
    }

    C4_ALWAYS_INLINE bool has_key_anchor() const
    {
        return _has_any_(KEYANCH);
    }

    C4_ALWAYS_INLINE bool has_val_anchor() const
    {
        return _has_any_(VALANCH);
    }

    void set_key_ref(csubstr ref)
    {
        _c4dbgpf("node[{}]: set key ref: [{}]~~~{}~~~", m_curr->tr_id, ref.len, ref);
        RYML_ASSERT(ref.begins_with('*'));
        _enable_(KEY|KEYREF);
        {
            m_curr->tr_data->m_key.anchor = ref.sub(1);
            m_curr->tr_data->m_key.scalar = ref;
        }
    }
    void set_val_ref(csubstr ref)
    {
        _c4dbgpf("node[{}]: set val ref: [{}]~~~{}~~~", m_curr->tr_id, ref.len, ref);
        RYML_ASSERT(ref.begins_with('*'));
        _enable_(VAL|VALREF);
        {
            m_curr->tr_data->m_val.anchor = ref.sub(1);
            m_curr->tr_data->m_val.scalar = ref;
        }
    }

    /** @} */

public:

    /** @name tag functions */
    /** @{ */

    void set_key_tag(csubstr tag)
    {
        _c4dbgpf("node[{}]: set key tag: [{}]~~~{}~~~", m_curr->tr_id, tag.len, tag);
        _enable_(KEYTAG);
        {
            m_curr->tr_data->m_key.tag = tag;
        }
    }
    void set_val_tag(csubstr tag)
    {
        _c4dbgpf("node[{}]: set val tag: [{}]~~~{}~~~", m_curr->tr_id, tag.len, tag);
        _enable_(VALTAG);
        {
            m_curr->tr_data->m_val.tag = tag;
        }
    }

    C4_ALWAYS_INLINE bool has_key_tag() const
    {
        return _has_any_(KEYTAG);
    }

    C4_ALWAYS_INLINE bool has_val_tag() const
    {
        return _has_any_(VALTAG);
    }

    /** @} */

public:

    /** @name directive functions */
    /** @{ */

    void add_directive(csubstr directive)
    {
        _RYML_CB_ASSERT(m_stack.m_callbacks, directive.begins_with('%'));
        {
            if(directive.begins_with("%TAG"))
            {
                // TODO do not use directives in the tree
                _RYML_CB_CHECK(m_stack.m_callbacks, m_tree->add_tag_directive(directive));
            }
            else if(directive.begins_with("%YAML"))
            {
                _c4dbgpf("%YAML directive! ignoring...: {}", directive);
            }
            else
            {
                _c4dbgpf("% directive unknown! ignoring...: {}", directive);
            }
        }
    }

    /** @} */

public:

    /** @name arena functions */
    /** @{ */

    substr alloc_arena(size_t len)
    {
        {
            return m_tree->alloc_arena(len);
        }
    }

    /** @} */

#undef _enable_
#undef _disable_
#undef _has_any_
};

} // namespace yml
} // namespace c4

#endif /* _C4_YML_PARSER_SINK_HPP_ */
