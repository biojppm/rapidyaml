#ifndef _C4_TPL_ENGINE_HPP_
#define _C4_TPL_ENGINE_HPP_

#include "./token.hpp"

namespace c4 {
namespace tpl {

class Engine
{
public:

    cspan m_src;
    mutable Rope m_rope;
    TokenContainer m_tokens;

public:

    Engine() : m_src(), m_rope(), m_tokens() {}

    Rope const& rope() const { return m_rope; }

    void parse(cspan src)
    {
        m_src = src;
        if(m_src.empty()) return;
        TplLocation pos{&m_rope, {m_rope.append(src), 0}};
        cspan rem = m_src;
        while( ! rem.empty())
        {
            auto tk_pos = m_tokens.next_token(&rem, &pos);
            if(tk_pos == NONE) break; // we're done
            m_tokens.get(tk_pos)->parse(&rem, &pos);
            m_tokens.get(tk_pos)->parse_body(&m_tokens);
        }
    }

    void mark()
    {
        for(auto &tk : m_tokens)
        {
            tk.mark();
        }
    }

    void render(c4::yml::NodeRef const& root, Rope *rope=nullptr) const
    {
        if(rope == nullptr)
        {
            rope = &m_rope;
        }
        if(rope->num_entries() != m_rope.num_entries())
        {
            *rope = m_rope;
        }
        for(auto const& token : m_tokens)
        {
            if( ! token.m_root_level) continue;
            token.render(root, rope);
        }
    }

    void render(Tree const& t, Rope *r=nullptr) const
    {
        render(t.rootref(), r);
    }

};

} // namespace tpl
} // namespace c4

#endif /* _C4_TPL_ENGINE_HPP_ */
