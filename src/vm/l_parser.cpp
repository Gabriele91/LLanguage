#include <l_parser.h>

namespace l_language
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    const char** l_parser::get_keywords()
    {
        static const char* str_keywords[]=
        {
            "if",
            "else",
            
            "def",
            "function",
            
            "while",
            "for",
            
            "super",
            "global",
            
            "of",
            "in",
            
            "return",
            
            "or",
            "and",
            
            "null",
            "true",
            "false",
            "...",
            NULL,
        };
        return str_keywords;
    }
    
    
    const char* l_parser::get_keyword(keyword key)
    {
        return get_keywords()[key];
    }
    
    bool l_parser::is_a_keyword(const char* ptr)
    {
        for(const char** keyword = get_keywords(); *keyword; ++keyword)
        {
            if(CSTRCMP(ptr,*keyword))
            {
                return true;
            }
        }
        return false;
    }
    bool l_parser::is_a_value_keyword(const char* ptr)
    {
        return KEYWORDCMP(ptr, NULL) ||
               KEYWORDCMP(ptr, TRUE) ||
               KEYWORDCMP(ptr, FALSE)||
               KEYWORDCMP(ptr, ARGS) ;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    //put variable
    void l_parser::push_variable(const std::string& str)
    {
        m_variables.push_back(variable_row(  str ));
    }
    bool l_parser::exists_variable(const std::string& str) const
    {
        //search
        for(const auto& var : m_variables)
            if(var.m_type == IS_VARIABLE)
                if(var.m_name == str)
                {
                    return true;
                }
        //return false
        return false;
    }
    bool l_parser::is_variable(const std::string& str)
    {
        return exists_variable(str);
    }
    //push error
    void l_parser::push_error(const std::string& error)
    {
        if(m_push_errors) m_errors.push_front(error_info( m_line, error ));
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    //compare string
    bool l_parser::strcmp(const char* left, const char* right, size_t len)
    {
        return (std::strncmp(left, right, len) == 0);
    }
    bool l_parser::strcmp(const char* left, const char* right)
    {
        return strcmp(left, right, std::strlen(right));
    }
    bool l_parser::strcmp_skip(const char*& left, const char* right, size_t len)
    {
        if (strcmp(left, right, len))
        {
            left += len;
            return true;
        }
        return false;
    }
    bool l_parser::strcmp_skip(const char*& left, const char* right)
    {
        return strcmp_skip(left, right, std::strlen(right));
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // types
    bool l_parser::is_line_space(char c)
    {
        return 	 c == ' ' || c == '\t';
    }
    bool l_parser::is_space(char c)
    {
        return 	 c == ' ' || c == '\t' || c == '\r' || c == '\n';
    }
    bool l_parser::is_start_name(char c)
    {
        return 	 (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
    }
    bool l_parser::is_char_name(char c)
    {
        return 	 (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_';
    }
    bool l_parser::is_start_string(char c)
    {
        return c == '\"';
    }
    bool l_parser::is_start_uint_number(char c)
    {
        return 	 (c >= '0' && c <= '9');
    }
    bool l_parser::is_start_ufloat_number(char c)
    {
        return 	 (c >= '0' && c <= '9') || c == '.';
    }
    bool l_parser::is_start_int_number(char c)
    {
        return 	 (c >= '0' && c <= '9') || c == '-';
    }
    bool l_parser::is_start_float_number(char c)
    {
        return 	 (c >= '0' && c <= '9') || c == '.' || c == '-';
    }
    bool l_parser::is_line_comment(const char* c)
    {
        return (*c) == '/' && (*(c + 1)) == '/';
    }
    bool l_parser::is_start_multy_line_comment(const char* c)
    {
        return (*c) == '/' && (*(c + 1)) == '*';
    }
    bool l_parser::is_end_multy_line_comment(const char* c)
    {
        return (*c) == '*' && (*(c + 1)) == '/';
    }
    bool l_parser::is_start_arg(char c)
    {
        return (c == '(');
    }
    bool l_parser::is_end_arg(char c)
    {
        return (c == ')');
    }
    bool l_parser::is_start_index(char c)
    {
        return (c == '[');
    }
    bool l_parser::is_end_index(char c)
    {
        return (c == ']');
    }
    bool l_parser::is_point(char c)
    {
        return (c == '.');
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    bool l_parser::is_int_number(const char* c)
    {
        //minus
        if (!is_start_int_number(*c)) return false;
        //this part is equals of uint
        while (is_start_uint_number(*c)) ++c;
        //isn't a point
        return (*c)!='.';
    }
    bool l_parser::is_float_number(const char* c)
    {
        if(is_int_number(c)) return false;
        //minus
        if (!is_start_float_number(*c)) return false;
        //this part is equals of uint
        while (is_start_ufloat_number(*c)) ++c;
        //isn't a point
        return true;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    void l_parser::skip_line_space(const char*& inout)
    {
        while (is_line_space(*inout)) ++(inout);
    }
    void l_parser::skip_space(const char*& inout)
    {
        while (is_space(*inout)) ++(inout);
    }
    void l_parser::skip_line_comment(const char*& inout)
    {
        if (is_line_comment(inout))
        {
            while (*(inout) != EOF &&
                   *(inout) != '\0'&&
                   *(inout) != '\n')
                ++(inout);
        }
    }
    void l_parser::skip_multy_line_comment(const char*& inout,bool count_lines)
    {
        if (is_start_multy_line_comment(inout))
        {
            while (*(inout) != EOF &&
                   *(inout) != '\0'&&
                   !is_end_multy_line_comment(inout))
            {
                m_line += count_lines && m_count_lines ? (*(inout)) == '\n' : 0;
                ++(inout);
            }
            if ((*(inout)) == '*') ++(inout);
        }
    }
    void l_parser::skip_space_end_comment(const char*& inout,bool count_lines)
    {
        while (is_space(*(inout)) ||
               is_line_comment(inout) ||
               is_start_multy_line_comment(inout))
        {
            skip_line_comment(inout);
            skip_multy_line_comment(inout);
            m_line += count_lines && m_count_lines ? (*(inout)) == '\n' : 0;
            ++(inout);
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    //parser of a unsigned int number
    bool l_parser::parse_uint_number(const char*& c, unsigned int& i)
    {
        std::string number;
        
        while (is_start_uint_number(*c)) { number += (*c); ++c; }
        
        if (number.size())
        {
            std::istringstream(number) >> i;
            return true;
        }
        
        return false;
    }
    //parser of a int number
    bool l_parser::parse_int_number(const char*& c, int& i)
    {
        std::string number;
        //minus
        if ((*c) == '-')
        {
            number += (*c);
            ++c;
            if (!is_start_uint_number(*c)) return false;
        }
        //this part is equals of uint
        while (is_start_uint_number(*c)) { number += (*c); ++c; }
        
        if (number.size())
        {
            std::istringstream(number) >> i;
            return true;
        }
        
        return false;
    }
    //parser float
    bool l_parser::parse_ufloat_number(const char*& c, float& i)
    {
        std::string number;
        //before point
        while (is_start_uint_number(*c)) { number += (*c); ++c; }
        //find point?
        if ((*c) == '.' && is_start_uint_number(*(c + 1)))
        {
            number += (*c); ++c;
        }
        //next point
        while (is_start_uint_number(*c)) { number += (*c); ++c; }
        //return float
        if (number.size())
        {
            std::istringstream(number) >> i;
            return true;
        }
        //not valid
        return false;
    }
    bool l_parser::parse_float_number(const char*& c, float& i)
    {
        std::string number;
        //minus
        if ((*c) == '-')
        {
            number += (*c);
            ++c;
            if (!is_start_ufloat_number(*c)) return false;
        }
        //before point
        while (is_start_uint_number(*c)) { number += (*c); ++c; }
        //find point?
        if ((*c) == '.') { number += (*c); ++c; }
        //next point
        while (is_start_uint_number(*c)) { number += (*c); ++c; }
        //return float
        if (number.size())
        {
            std::istringstream(number) >> i;
            return true;
        }
        //not valid
        return false;
    }
    //parser of a string
    bool l_parser::parse_cstring(const char* in, const char** cout, std::string& out,bool count_lines)
    {
        const char *tmp = in; out = "";
        if ((*tmp) == '\"')  //["...."]
        {
            ++tmp;  //[...."]
            while ((*tmp) != '\"' && (*tmp) != '\n')
            {
                if ((*tmp) == '\\') //[\.]
                {
                    ++tmp;  //[.]
                    switch (*tmp)
                    {
                        case 'n':
                            out += '\n';
                            break;
                        case 't':
                            out += '\t';
                            break;
                        case 'b':
                            out += '\b';
                            break;
                        case 'r':
                            out += '\r';
                            break;
                        case 'f':
                            out += '\f';
                            break;
                        case 'a':
                            out += '\a';
                            break;
                        case '\\':
                            out += '\\';
                            break;
                        case '?':
                            out += '\?';
                            break;
                        case '\'':
                            out += '\'';
                            break;
                        case '\"':
                            out += '\"';
                            break;
                        case '\n': /* jump unix */
                            if(count_lines && m_count_lines) ++m_line;
                            break;
                        case '\r': /* jump mac */
                            if(count_lines && m_count_lines) ++m_line;
                            if ((*(tmp + 1)) == '\n') ++tmp; /* jump window (\r\n)*/
                            break;
                        default:
                            return true;
                            break;
                    }
                }
                else
                {
                    if ((*tmp) != '\0') out += (*tmp);
                    else return false;
                }
                ++tmp;//next char
            }
            if ((*tmp) == '\n') return false;
            if (cout) (*cout) = tmp + 1;
            return true;
        }
        //}
        return false;
    }
    //parsing a name (variable/keyword)
    bool l_parser::parse_name(const char* in, const char** cout, std::string& out)
    {
        if (!is_start_name(*in)) return false;
        out += *in;
        ++in;
        while (is_char_name(*in))
        {
            out += *in;
            ++in;
        }
        (*cout) = in;
        return true;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
}