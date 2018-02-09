#ifndef _LIBYAML_HPP_
#define _LIBYAML_HPP_

#include <yaml.h>
#include <c4/yml/yml.hpp>

#include <stdexcept>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

namespace c4 {
namespace yml {


namespace detail {
class Event;
} // detail

class detail::Event
{
public:
    yaml_event_t m_event;
public:
    Event() {}
    ~Event()
    {
        yaml_event_delete(&m_event);
    }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class LibyamlParser
{
public:

    yaml_parser_t m_parser;

    const char *  m_input;
    size_t        m_length;

    NodeData *    m_load_root;

private:

    cspan get_scalar_val(detail::Event const &ev) const
    {
        // the memory in data.scalar is allocated anew, so don't do this
        //auto const& scalar = e.m_event.data.scalar;
        //cspan val((const char*)scalar.value, scalar.length);
        //return val;
        // ... but the event tells us where in the string the value is
        auto const& e = ev.m_event;
        size_t len = e.end_mark.index - e.start_mark.index;
        cspan val(m_input + e.start_mark.index, len);
        return val;
    }

public:

    LibyamlParser()
    {
        memset(this, 0, sizeof(*this));
        yaml_parser_initialize(&m_parser);
    }

    ~LibyamlParser()
    {
        yaml_parser_delete(&m_parser);
        memset(&m_parser, 0, sizeof(decltype(m_parser)));
    }

    template< size_t N >
    void parse(/*Tree *s, */const char (&input)[N])
    {
        parse(/*s, */&input[0], N-1);
    }

    void parse(/*Tree *s, */const cspan sp)
    {
        parse(/*s, */sp.str, sp.len);
    }
    void parse(/*Tree *s, */const char* input, size_t length)
    {
        m_input = input;
        m_length = length;
        yaml_parser_set_input_string(&m_parser, (const unsigned char*)input, length);
        _do_parse(/*s*/);
    }

    void _do_parse(/*Tree *s*/)
    {
        bool done = false;
        //bool doc_had_scalars = false;
        //cspan prev_scalar;
        while( ! done)
        {
            detail::Event ev;
            if( ! yaml_parser_parse(&m_parser, &ev.m_event))
            {
                _handle_error();
                break;
            }

#define _c4_handle_case(_ev)                            \
case YAML_ ## _ev ## _EVENT:                            \
    printf(#_ev " val=%.*s\n",                          \
           /*(int)prev_scalar.len, prev_scalar.str,*/   \
           (int)val.len, val.str);

            cspan val = get_scalar_val(ev);
            switch(ev.m_event.type)
            {

            _c4_handle_case(MAPPING_START)
                /*if(( ! s->stack_top_is_type(DOC) || doc_had_scalars)
                   &&
                   ( ! m_load_root))
                {
                    C4_ASSERT( ! prev_scalar.empty());
                    s->begin_map(prev_scalar, s->top_last());
                    prev_scalar.clear();
                }
                */
                break;
            _c4_handle_case(MAPPING_END)
                /*if( ! s->stack_top_is_type(DOC) && ! m_load_root)
                {
                    s->end_map();
                }*/
                break;

            _c4_handle_case(SEQUENCE_START)
                //s->begin_seq(prev_scalar, s->top_last());
                break;
            _c4_handle_case(SEQUENCE_END)
                //s->end_seq();
                break;

            _c4_handle_case(SCALAR)
                /*if(s->stack_top_is_type(SEQ))
                {
                    s->add_val({}, val, s->top_last());
                    prev_scalar.clear();
                }
                else
                {
                    if( ! prev_scalar.empty())
                    {
                        s->add_val(prev_scalar, val, s->top_last());
                        prev_scalar.clear();
                    }
                    else
                    {
                        prev_scalar = val;
                    }
                }
                doc_had_scalars = true;
                */
                break;

            _c4_handle_case(DOCUMENT_START)
                /*if( ! m_load_root)
                {
                    s->begin_doc(s->top_last());
                    doc_had_scalars = false;
                }*/
                break;
            _c4_handle_case(DOCUMENT_END)
                /*if( ! m_load_root)
                {
                    s->end_doc();
                }*/
                break;

            _c4_handle_case(STREAM_START)
                //s->begin_stream();
                break;
            _c4_handle_case(STREAM_END)
                //s->end_stream();
                done = true;
                break;

            _c4_handle_case(ALIAS)
                //C4_ASSERT(false && "YAML_ALIAS_EVENT not implemented");
                break;
#undef _c4_handle_case
            default:
                break;
            };
        }
    }

    void _handle_error()
    {
        Location problem_loc, context_loc;
        if(m_parser.problem)
        {
            auto const& m = m_parser.problem_mark;
            problem_loc = Location(m_parser.problem, m.index, m.line+1, m.column+1);
        }
        if(m_parser.context)
        {
            auto const& m = m_parser.context_mark;
            context_loc = Location(m_parser.context, m.index, m.line+1, m.column+1);
        }

        switch(m_parser.error)
        {

        case YAML_MEMORY_ERROR:
            error("Memory error: Not enough memory for parsing");
            break;

        case YAML_READER_ERROR:
            if (m_parser.problem_value != -1)
            {
                char buf[32];
                int ret = snprintf(buf, sizeof(buf), "Reader error: #%X", m_parser.problem_value);
                error(buf, ret, &problem_loc);
            }
            else
            {
                error("Reader error", &problem_loc);
            }
            break;

        case YAML_SCANNER_ERROR:
            error("Scanner error", &context_loc, &problem_loc);
            break;

        case YAML_PARSER_ERROR:
            error("Parser error", &context_loc, &problem_loc);
            break;

        default:
            /* Couldn't happen. */
            error("Internal error");
            break;
        };
    }

    static void error(const char* msg, size_t length, Location *loc1 = nullptr, Location *loc2 = nullptr)
    {
        fprintf(stderr, "%.*s\n", (int)length, msg);
        if(loc1 && *loc1)
        {
            fprintf(stderr, "    : %s at %zd:%zd (offset %zd)\n", loc1->name, loc1->line, loc1->col, loc1->offset);
        }
        if(loc2 && *loc2)
        {
            fprintf(stderr, "    : %s at %zd:%zd (offset %zd)\n", loc2->name, loc1->line, loc2->col, loc2->offset);
        }
        throw std::runtime_error({msg, length});
    }
    template< size_t N >
    static void error(char const (&msg)[N], Location *loc1 = nullptr, Location *loc2 = nullptr)
    {
        error(&msg[0], N-1, loc1, loc2);
    }

};

} // namespace yml
} // namespace c4

#endif // _LIBYAML_HPP_
