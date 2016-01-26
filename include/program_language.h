//
//  program_language.h
//  LLanguage
//
//  Created by Gabriele Di Bari on 24/12/15.
//  Copyright © 2015 Gabriele Di Bari. All rights reserved.
//
#pragma once

#include <config.h>
#include <compile_tree_to_cpp.h>
#include <compile_tree_to_js.h>
#include <compiler_tree_to_vm.h>
#include <syntactic_tree.h>
#include <parser.h>

namespace l_language
{
    class program_language
    {
        
        using tree         = l_language::syntactic_tree;
        using parser       = l_language::parser;
        using js_compiler  = l_language::compile_tree_to_js;
        using cpp_compiler = l_language::compile_tree_to_cpp;
        using vm_compiler  = l_language::compiler_tree_to_vm;
        //parser...
        parser       it_parser;
        //compilers
        cpp_compiler m_cpp_comp;
        js_compiler  m_js_comp;
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
            std::string m_cpp_function;
            std::string m_js_function;
            
            lib_field(std::string name,
                      l_cfunction  cfunction,
                      std::string cpp_function,
                      std::string js_function)
            {
                m_name = name;
                m_cfunction = cfunction;
                m_cpp_function = cpp_function;
                m_js_function = js_function;
            }
            
            lib_field(std::string name,
                      l_cfunction cfunction)
            {
                m_name = name;
                m_cfunction = cfunction;
            }
        };
        //libary vector
        using extern_libary = std::vector< lib_field >;
        
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
            if (!it_parser.italanguage(source_code,it_tree))
            //ouput errors
            {
                for (auto& error : it_parser.m_errors)
                {
                    output.m_errors += std::to_string( error.m_line ) + ":" + error.m_error + "\n";
                }
                //fail
                output.m_type = ERRORS;
                //return
                return output;
            }
            //else compile to...
            //CPP COMPILER
            if(flags & TO_CPP)
            {
                m_cpp_comp.compile(&it_tree);
                //errors?
                if(m_cpp_comp.m_errors.size())
                {
                    //ouput errors
                    for (auto& error : m_cpp_comp.m_errors)
                    {
                        output.m_errors += error + "\n";
                    }
                    //output errors
                    output.m_type |= ERRORS;
                }
                //output code
                output.m_type |= TO_CPP;
                output.m_out_cpp = m_cpp_comp.m_cpp_code;
            }
            //JS COMPILER
            if(flags & TO_JS)
            {
                m_js_comp.compile(&it_tree);
                //errors?
                if(m_js_comp.m_errors.size())
                {
                    //ouput errors
                    for (auto& error : m_js_comp.m_errors)
                    {
                        output.m_errors += error + "\n";
                    }
                    //output errors
                    output.m_type |= ERRORS;
                }
                //output code
                output.m_type |= TO_JS;
                output.m_out_js = m_js_comp.m_js_code;
            }
            //VM COMPILER
            if(flags & TO_VM)
            {
                m_thread= m_vm_comp.compile(&it_tree);
                //main bytecode
                //m_vm.m_functions[0].dump_all_function();
                //start
                m_vm.execute(m_thread);
            }
            //return...
            return output;
        }
        
        void add_lib(const extern_libary& libs)
        {
            for(const lib_field& field : libs)
            {
                m_vm_comp.add_c_function( field.m_cfunction, field.m_name );
            }
        }
        
        //default
        program_language()
        {
            //init vm
            m_vm_comp.set_vm(&m_vm);
        }
    };
};