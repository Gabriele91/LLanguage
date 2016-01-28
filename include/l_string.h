//
//  l_string.h
//  LLanguage
//
//  Created by Gabriele on 26/01/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//

#ifndef string_h
#define string_h

#include <l_gc.h>
#include <string>

namespace l_language
{
    class l_vm;
    class l_variable;
    class l_string : public l_obj
    {
        
        std::string m_cpp_str;
        
    public:
        
        l_string()
        {
        }
        
        l_string(const std::string& string)
        {
            m_cpp_str = string;
        }
        
        operator std::string& ()
        {
            return m_cpp_str;
        }
        
        operator const std::string& () const
        {
            return m_cpp_str;
        }
        
        const char* c_str() const
        {
            return m_cpp_str.c_str();
        }
        
        const std::string& str() const
        {
            return m_cpp_str;
        }
        
        l_variable gc_merge(l_string* string);
        
        l_variable gc_merge(const std::string& str);
        
        l_variable gc_merge_left(l_string* string);
        
        l_variable gc_merge_left(const std::string& str);
        
        static l_variable gc_new(l_gc* gc,const std::string& string);
        
        static l_variable gc_new(l_gc& gc,const std::string& string);
        
        static l_variable gc_new(l_vm* vm,const std::string& string);
        
        static l_variable gc_new(l_vm& vm,const std::string& string);
        
        static l_variable const_new(l_gc* gc,const std::string& string);
        
        static l_variable const_new(l_gc& gc,const std::string& string);
        
        static l_variable const_new(l_vm* vm,const std::string& string);
        
        static l_variable const_new(l_vm& vm,const std::string& string);
        
        virtual ~l_string()
        {
        }
    };
};

#endif /* string_h */
