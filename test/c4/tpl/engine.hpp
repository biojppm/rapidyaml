#ifndef _C4_TPL_ENGINE_HPP_
#define _C4_TPL_ENGINE_HPP_

#include "./token.hpp"

namespace c4 {
namespace tpl {

class Engine
{
public:

    cspan m_src;
    TokenContainer m_tokens;

public:

    Engine() : m_src(), m_tokens() {}

    void clear()
    {
        m_tokens.clear();
    }

    void parse(cspan src, Rope *rope)
    {
        m_src = src;
        clear();
        if(m_src.empty()) return;
        TplLocation pos{rope, {rope->append(src), 0}};
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

    void render(c4::yml::NodeRef & root, Rope *rope) const
    {
        if(m_tokens.size() > 0)
        {
            Rope const* parsed_rope = m_tokens.get(0)->rope();
            if(rope->num_entries() != parsed_rope->num_entries())
            {
                *rope = *parsed_rope;
            }
        }
        for(auto const& token : m_tokens)
        {
            if( ! token.m_root_level) continue;
            token.render(root, rope);
        }
    }

    void render(Tree & t, Rope *r) const
    {
        auto n = t.rootref();
        render(n, r);
    }

    Rope render(c4::yml::NodeRef & root) const
    {
        Rope cp;
        render(root, &cp);
        return cp;
    }

    Rope render(Tree & t) const
    {
        auto n = t.rootref();
        return render(n);
    }

};

} // namespace tpl
} // namespace c4

#endif /* _C4_TPL_ENGINE_HPP_ */
