//
//  l_vm.h
//  LLanguage
//
//  Created by Gabriele on 09/01/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#pragma once

#include <vector>
#include <stack>
#include <l_variable.h>
#include <l_gc.h>
#include <l_opcode.h>
#include <l_commands.h>
#include <l_function.h>
#include <l_table.h>
#include <l_call_context.h>
#include <l_thread.h>
#include <l_parser.h>
#include <l_compiler_tree_to_vm.h>
#include <l_syntactic_tree.h>

namespace l_language
{
    //f. declaretion
    class l_gc;
    class l_thread;
    //..
    class l_vm
    {
        
        //friend class
        friend class l_gc;
        friend class l_thread;
        //thread list
        l_list_threads_ref    m_threads;
        //functions
        l_list_functions_ref  m_functions;
        //gc
        l_gc*                 m_gc{ nullptr };
        //compiler
        l_parser              m_parser;
        l_compiler_tree_to_vm m_compiler;
        //default
        l_thread&             m_default_thread;
        
    public:
        
        //init vm
        l_vm();
        virtual ~l_vm();
        //thread utility
        l_thread& get_new_thread();
        l_thread& thread(unsigned int i);
        void remove(l_thread* thread);
        //function utility
        unsigned int get_new_function_id();
        l_function& get_new_function();
        l_function& function(unsigned int i);
        size_t get_count_of_functions() const;
        //get gc
        l_gc& get_gc();
        const l_gc& get_gc() const;
        ////////////////////////////////////////////////////////////////////////////////////
        //flags
        enum compiler_flags
        {
            NONE           = 0x000000,
            ERRORS         = 0b000001,
            SYNTAX_ERROR   = 0b000011,
            COMPILE_ERROR  = 0b000101,
            RUNTIME_ERROR  = 0b001001,
            EXECUTE        = 0b010000,
            DUMP           = 0b100000,
        };
        
        struct compiler_ouput
        {
            int         m_type{ NONE };
            std::string m_errors;
        };
        compiler_ouput compile(const std::string& source_code,int flags = EXECUTE);
        l_variable eval(std::string source_code);
        l_variable eval(std::string source_code, compiler_ouput& output);
        bool pcall(l_variable& l_return,  const std::string& function, std::initializer_list<l_variable> args);
        bool pcall(const std::string& function, std::initializer_list<l_variable> args);
        //execute
        bool execute(unsigned int id_thread);
        bool execute(l_thread* this_thread);
        bool execute_call(l_variable& v_return,
                          l_variable& call,
                          std::initializer_list<l_variable> args);
        ////////////////////////////////////////////////////////////////////////////////////
        //lib fields
        class lib_field
        {
        public:
            
            std::string m_name;
            l_variable  m_cfunction;
            
            lib_field(const std::string& name,
                      l_cfunction  cfunction)
            {
                m_name = name;
                m_cfunction = cfunction;
            }
            
            lib_field(const std::string& name,
                      const l_variable&  value)
            {
                m_name = name;
                m_cfunction = value;
            }
        };
        //libary vector
        using l_extern_libary = std::vector< lib_field >;
        //lib utilities
        void add_var(const std::string& name, const l_variable& variable);
        void add_fun(const std::string& name, l_cfunction  cfunction);
        void add_lib(const l_extern_libary& libs);
        void add_lib(const l_extern_libary& (*libs) (l_vm*));
        void add_lib(const std::string& name,const l_extern_libary& libs);
        void add_lib(const std::string& name,const l_extern_libary& (*libs) (l_vm*));
        ////////////////////////////////////////////////////////////////////////////////////
    };
}
