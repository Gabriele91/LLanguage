//
//  program_language.h
//  LLanguage
//
//  Created by Gabriele Di Bari on 24/12/15.
//  Copyright © 2015 Gabriele Di Bari. All rights reserved.
//
#pragma once

#include <l_config.h>
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
        //virtual machine
        l_vm         m_vm;
        l_thread&    m_thread;
        //compilers
        vm_compiler  m_vm_comp;
        
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
            ERRORS  = 0b0001,
            EXECUTE = 0b0010,
            DUMP    = 0b0100,
        };
        
        struct compiler_ouput
        {
            int         m_type{ NONE };
            std::string m_out_js;
            std::string m_out_cpp;
            std::string m_errors;
        };
        
        compiler_ouput compile(const std::string& source_code,int flags = EXECUTE)
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
            if(flags & (EXECUTE|DUMP))
            {
                m_vm_comp.compile(&it_tree);
                //main bytecode
                if(flags & DUMP)
                {
                    for(size_t i=0;i!=m_vm.m_functions.size();++i)
                    {
                        std::cout<< "------------\n"
                                    "Function["<<i<<"]:\n\n";
                        m_vm.function((unsigned int)i).dump_all_function();
                        std::cout<< "\n";
                    }
                }
                //start
                if(flags & EXECUTE) m_vm.execute(&m_thread);
            }
            //return...
            return output;
        }
        
        void add_lib(const l_extern_libary& libs)
        {
            for(const lib_field& field : libs)
            {
                m_vm_comp.add_c_function( m_thread, field.m_cfunction, field.m_name );
            }
        }

		void add_lib(const std::string& name,const l_extern_libary& libs)
		{
			l_variable table = l_table::gc_new(m_vm.get_gc());
			for (const lib_field& field : libs)
			{
				table.to<l_table>()->push(l_string::gc_new(m_vm.get_gc(), field.m_name), field.m_cfunction);
			}				
			m_vm_comp.add_global_variable(m_thread, table, name);
		}

        //default
        l_program_language()
        :m_thread(m_vm.get_new_thread())
        ,m_vm_comp(&m_thread)
        {
        }
        
        //default
        virtual ~l_program_language()
        {
            //dealloc
        }
    };
};