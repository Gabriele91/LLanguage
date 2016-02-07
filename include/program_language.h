//
//  program_language.h
//  LLanguage
//
//  Created by Gabriele Di Bari on 24/12/15.
//  Copyright © 2015 Gabriele Di Bari. All rights reserved.
//
#pragma once

#include <config.h>
#include <l_compiler_tree_to_vm.h>
#include <l_syntactic_tree.h>
#include <l_parser.h>

namespace l_language
{
    class l_program_language
    {
        
        using tree         = l_language::l_syntactic_tree;
        using parser       = l_language::l_parser;
        using vm_compiler  = l_language::l_compiler_tree_to_vm;
        //parser...
        parser       m_parser;
        //compilers
        vm_compiler  m_vm_comp;
        //virtual machine
        l_vm         m_vm;
        l_thread*    m_thread;
        
    public:
        
        //lib fields
        class lib_field
        {
        public:
            
            std::string m_name;
            l_cfunction m_cfunction;
            
            lib_field(const std::string& name,
                      l_cfunction  cfunction)
            {
                m_name = name;
                m_cfunction = cfunction;
            }
        };
        //libary vector
        using l_extern_libary = std::vector< lib_field >;
        
        //flags
        enum compiler_flags
        {
            NONE    = 0x0000,
            TO_JS   = 0x0001,
            TO_CPP  = 0b0010,
            TO_VM   = 0b0100,
            ERRORS  = 0b1000
        };
        
        struct compiler_ouput
        {
            int         m_type{ NONE };
            std::string m_out_js;
            std::string m_out_cpp;
            std::string m_errors;
        };
        
        compiler_ouput compile(const std::string& source_code,int flags = NONE)
        {
            //targets
            tree         it_tree;
            //output
            compiler_ouput output;
            //
            if (!m_parser.italanguage(source_code,it_tree))
            //ouput errors
            {
                for (auto& error : m_parser.m_errors)
                {
                    output.m_errors += std::to_string( error.m_line ) + ":" + error.m_error + "\n";
                }
                //fail
                output.m_type = ERRORS;
                //return
                return output;
            }
            //else compile to...
            //VM COMPILER
            if(flags & TO_VM)
            {
                m_thread= m_vm_comp.compile(&it_tree);
                //main bytecode
                m_vm.m_functions[0].dump_all_function();
                //start
                m_vm.execute(m_thread);
            }
            //return...
            return output;
        }
        
        void add_lib(const l_extern_libary& libs)
        {
            for(const lib_field& field : libs)
            {
                m_vm_comp.add_c_function( field.m_cfunction, field.m_name );
            }
        }
        
        //default
        l_program_language()
        {
            //init vm
            m_vm_comp.set_vm(&m_vm);
        }
        
        //default
        virtual ~l_program_language()
        {
            //dealloc
        }
    };
};