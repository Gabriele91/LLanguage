//
//  l_exception.h
//  LLanguage
//
//  Created by Gabriele on 18/04/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#pragma once
#include <exception>
namespace l_language
{
    
    class l_exception : public std::exception
    {
    public:
        
        l_exception(l_language::l_thread* thread,
                    const std::string& who,
                    const std::string& what)
        {
            m_thread = thread;
            m_who    = who;
            m_what   = what;
        }
        
        virtual const char* who() const noexcept
        {
            return m_who.c_str();
        }
        
        virtual const char* what() const noexcept
        {
            return m_what.c_str();
        }
        
    private:
        
        l_language::l_thread* m_thread;
        std::string           m_who;
        std::string           m_what;
    };
    
    
    class l_exception_number_of_arguments : public l_exception
    {
    public:
        l_exception_number_of_arguments(l_language::l_thread* thread,const std::string& what) : l_exception(thread,"number_of_arguments",what){}
    };
    
    class l_exception_to_int : public l_exception
    {
    public:
        l_exception_to_int(l_language::l_thread* thread,const std::string& what) : l_exception(thread,"to_int",what){}
    };
    
    class l_exception_to_float : public l_exception
    {
    public:
        l_exception_to_float(l_language::l_thread* thread,const std::string& what) : l_exception(thread,"to_float",what) {}
    };
    
    class l_exception_to_string : public l_exception
    {
    public:
        l_exception_to_string(l_language::l_thread* thread,const std::string& what) : l_exception(thread,"to_string",what) {}
    };
    
    class l_exception_not_valid_this : public l_exception
    {
    public:
        l_exception_not_valid_this(l_language::l_thread* thread,const std::string& what) : l_exception(thread,"this",what) {}
    };
    
};