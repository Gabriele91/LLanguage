//
//  l_compiler_utilities.cpp
//  LLanguage
//
//  Created by Gabriele on 27/01/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//

#include <l_compiler_utilities.h>

namespace l_language
{
    namespace compiler_utilities
    {
        std::string  compile_string(const std::string& value)
        {
            const char *tmp = value.c_str();
            std::string out = "\"";// add ".....
            while ((*tmp) != '\0')
            {
                switch (*tmp)
                {
                    case '\n': out += "\\n"; break;
                    case '\t': out += "\\t"; break;
                    case '\b': out += "\\b"; break;
                    case '\r': out += "\\r"; break;
                    case '\f': out += "\\f"; break;
                    case '\a': out += "\\a"; break;
                    case '\\': out += "\\"; break;
                    case '\?': out += "\\?"; break;
                    case '\'': out += "\\\'"; break;
                    case '\"': out += "\\\""; break;
                    default: out += (*tmp); break;
                }
                ++tmp;//next char
            }
            out += '\"';// add ...."
            //return..
            return out;
        }
    };
};
