//
//  vm.cpp
//  LLanguage
//
//  Created by Gabriele on 16/01/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//

#include <stdio.h>
#include <l_vm.h>
#include <l_gc.h>
#include <l_array.h>
#include <l_table.h>
#include <iostream>

namespace l_language
{

    //alloc vm
    l_vm::l_vm()
    :m_gc(new l_gc(*this))
    ,m_default_thread(get_new_thread())
    {
        m_compiler.set_thread(&m_default_thread);
    }
    //dealloc vm
    l_vm::~l_vm()
    {
        delete m_gc;
    }
    //get gc
    l_gc& l_vm::get_gc()
    {
        return *m_gc;
    }
    const l_gc& l_vm::get_gc() const
    {
        return *m_gc;
    }
    ////////////////////////////////////////////////////////////////////////////////////
    //thread utility
    l_thread& l_vm::get_new_thread()
    {
        //alloc
        auto* alloc = new l_thread(this,  get_new_function_id(), 64);
        //push
        m_threads.push_back(std::unique_ptr<l_thread>(alloc));
        //return
        return *alloc;
    }
    l_thread& l_vm::thread(unsigned int i)
    {
        return *m_threads[i];
    }
    void l_vm::remove(l_thread* thread)
    {
        for(auto it_thread = m_threads.begin();
            it_thread!= m_threads.end();
            ++it_thread)
        {
            if(it_thread->get() == thread)
            {
                m_threads.erase(it_thread);
                return;
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////
    //function utility
    unsigned int l_vm::get_new_function_id()
    {
        m_functions.push_back(std::unique_ptr<l_function>(new l_function));
        return (unsigned int)m_functions.size()-1;
    }
    l_function& l_vm::get_new_function()
    {
        unsigned int f_id = get_new_function_id();
        return *m_functions[f_id];
    }
    l_function& l_vm::function(unsigned int i)
    {
        return *m_functions[i];
    }
    size_t l_vm::get_count_of_functions() const
    {
        return m_functions.size();
    }
    ////////////////////////////////////////////////////////////////////////////////////
    l_vm::compiler_ouput l_vm::compile(const std::string& source_code,int flags)
    {
        //targets
        l_syntactic_tree it_tree;
        //output
        compiler_ouput   output;
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
            //set thread
            m_compiler.set_thread(&m_default_thread);
            //set thread
            if(!m_compiler.compile(&it_tree))
            {
                output.m_errors += "fail to compile tree\n";
                //fail
                output.m_type = ERRORS;
                //return
                return output;
            }
            //main bytecode
            if(flags & DUMP)
            {
                for(size_t i=0;i!=m_functions.size();++i)
                {
                    std::cout<< "------------\n"
                    "Function["<<i<<"]:\n\n";
                    function((unsigned int)i).dump_all_function();
                    std::cout<< "\n";
                }
            }
            //start
            if(flags & EXECUTE)
                if(!execute(&m_default_thread))
                {
                    for(auto& error:m_default_thread.m_errors)
                    {
                        output.m_errors += std::to_string( error.m_line ) + ":" + error.m_error + "\n";
                    }
                    //fail
                    output.m_type = ERRORS;
                }
        }
        //return...
        return output;
    }
    
    l_variable l_vm::eval(std::string source_code)
    {
        //targets
        l_syntactic_tree it_tree;
        //output
        compiler_ouput output;
        //return
        l_variable l_return;
        //parsing
        if (!m_parser.italanguage(source_code,it_tree))
            //ouput errors
        {
            for (auto& error : m_parser.m_errors)
            {
                output.m_errors += std::to_string( error.m_line ) + ":" + error.m_error + "\n";
            }
            //clear errors
            m_parser.m_errors.clear();
            //fail
            output.m_type = ERRORS;
            //return...
            l_return = l_string::gc_new(this, output.m_errors);
        }
        else
        //execute
        {
            //compile
            l_function_id call_fun_id = m_compiler.compile_function(&it_tree);
            //register
            l_variable&   regT1 = m_default_thread.register3(l_thread::R_TEMP1);
            //new context
            regT1 = l_closer::gc_new(get_gc());
            //init context
            regT1.to<l_closer>()->init( call_fun_id,
                                       &m_default_thread,
                                        m_default_thread.get_last_context());
            //start
            if(!execute_call(l_return,regT1,{}))
            {
                for(auto& error:m_default_thread.m_errors)
                {
                    output.m_errors += std::to_string( error.m_line ) + ":" + error.m_error + "\n";
                }
                //clear errors
                output.m_errors.clear();
                //output errors
                l_return = l_string::gc_new(this, output.m_errors);
            }
        }
        //return
        return l_return;
    }
    
    bool l_vm::pcall(l_variable& l_return,
               const std::string& function,
               std::initializer_list<l_variable> args)
    {
        l_variable name = l_string::const_new(*this, function);
        l_variable call = m_default_thread.main_context()->variable(name);
        return execute_call(l_return,call,args);;
    }
    
    bool l_vm::pcall(const std::string& function,
                     std::initializer_list<l_variable> args)
    {
        l_variable name = l_string::const_new(*this, function);
        l_variable call = m_default_thread.main_context()->variable(name);
        l_variable l_return;
        
        return execute_call(l_return,call,args);
    }
    
    
    bool l_vm::execute(unsigned int id_thread)
    {
        return execute(&thread(id_thread));
    }
    
    bool l_vm::execute(l_thread* ptr_thread)
    {
        //get thread
        return ptr_thread->execute(*ptr_thread->main_context()) != l_thread::T_RETURN_ERROR;
    }

    bool l_vm::execute_call(l_variable& v_return,
                            l_variable& call,
                            std::initializer_list<l_variable> args)
    {
        //get context
        l_closer* closer = call.to<l_closer>();
        //else assert
        if(!closer){ return false; };
        //new function
        l_function& call_fun = function(closer->get_fun_id());
        //new context
        l_variable l_new_ctx = l_call_context::gc_new(get_gc());
        l_call_context* new_ctx = l_new_ctx.to<l_call_context>();
        //init
        new_ctx->init(*closer);
        //lock context
        new_ctx->lock();
        //..
        unsigned int id_arg = 0;
        //put arguments
        for(auto arg : args)
        {
            if (id_arg < call_fun.m_args_size)
                new_ctx->variable( call_fun.constant(id_arg++) ) = arg;
            else
                break;
        }
        //..
        l_thread* thread = new l_thread(this);
        //execute call
        l_thread::type_return n_return = thread->execute(*new_ctx);
        //error?
        if(n_return==l_thread::T_RETURN_ERROR) return false;
        //unlock context
        new_ctx->unlock();
        //return?
        if(n_return == l_thread::T_RETURN_VALUE)
        {
            v_return = (thread->register3(2));
        }
        //dealloc
        delete thread;
        //..
        return true;
    }
    ////////////////////////////////////////////////////////////////////////////////////
    void l_vm::add_lib(const l_vm::l_extern_libary& libs)
    {
        for(const lib_field& field : libs)
        {
            m_compiler.add_c_function( m_default_thread, field.m_cfunction, field.m_name );
        }
    }
    void l_vm::add_lib(const std::string& name,const l_extern_libary& libs)
    {
        l_variable table = l_table::gc_new(get_gc());
        for (const lib_field& field : libs)
        {
            table.to<l_table>()->push(l_string::gc_new(get_gc(), field.m_name), field.m_cfunction);
        }
        m_compiler.add_global_variable(m_default_thread, table, name);
    }
    

};