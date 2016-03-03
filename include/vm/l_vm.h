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

namespace l_language
{
    
    //thread
    class l_thread
    {
	public:
        //main function id
        l_function_id m_main_fun_id { 0 };
        //current index
        long m_top{ -1 };
        //temp variable
        l_variable m_register[3];
        //save vm
        l_vm* m_vm;
        //contexts
        l_variable m_main_ctx;
        //init (thread)
        l_thread(l_vm* vm = nullptr,
                 unsigned int main_id = 0,
                 size_t stack = 512);
        //access
        l_variable& global(int value_id);
        l_variable& global(l_call_context& fun,int value_id);
        //access
        l_variable& local(l_call_context& this_context,int value_id);
        //access
        l_variable& strick_local(l_call_context& this_context,int value_id);
        //access
        l_call_context* main_context()
        {
            return m_main_ctx.to<l_call_context>();
        }
        l_variable ref_main_context()
        {
            return m_main_ctx;
        }
        l_function* main_function();
        //get vm
        l_vm* get_vm();
        //get gc
        l_gc* get_gc();
        //stack
        l_list_variables m_stack;
        //get top
        l_variable& top()
        {
            return m_stack[m_top];
        }
        //get a register
        l_variable& register3(size_t i)
        {
            return m_register[i];
        }
        //push return
        void push_return(const l_variable& var)
        {
            m_register[2] = var;
        }
        //get top
        l_variable& value(size_t x)
        {
            return m_stack[m_top-x];
        }
        l_variable& stack(size_t x)
        {
            return m_stack[m_top-x];
        }
        //pop
        l_variable& pop()
        {
            return m_stack[m_top--];
        }
        //push
        void push(const l_variable& value)
        {
            m_stack[++m_top] = value;
        }
        void push()
        {
            ++m_top;
        }
        //size
        size_t size() const
        {
            return m_top + 1;
        }
        //capacity
        size_t capacity() const
        {
            return m_stack.size();
        }
        //real capacity
        size_t real_capacity() const
        {
            return m_stack.capacity();
        }
        //stack size
        void stack_size(size_t size)
        {
            m_stack.resize(size);
        }
        //errors
        struct error
        {
            std::string  m_error;
            unsigned int m_code;
            unsigned int m_line;
        };
        std::vector < error > m_errors;
        //..
        void push_error(const std::string& str,
                        unsigned int code,
                        unsigned int line)
        {
            m_errors.push_back
            ({
                str,
                code,
                line
            });
        }
        //execution return
        enum type_return
        {
            T_RETURN_VOID,
            T_RETURN_VALUE,
            T_RETURN_ERROR
        };
        //execute context
        type_return execute(l_call_context& id_context);
    };
    
    //list threads
    using l_list_threads     = std::vector < l_thread >;
    //list threads
    using l_list_threads_ref = std::vector < std::unique_ptr<l_thread> >;
    
    class l_vm
    {
        //gc
        l_gc* m_gc;
        
    public:
        //init vm
        l_vm();
        virtual ~l_vm();
        //thread list
        l_list_threads_ref   m_threads;
        //functions
        l_list_functions_ref  m_functions;
        //get thread
        l_thread& get_new_thread()
        {
            //alloc
            auto* alloc = new l_thread(this,  0, 64);
            //push
            m_threads.push_back(std::unique_ptr<l_thread>(alloc));
            //return
            return *alloc;
        }
        l_thread& thread(unsigned int i)
        {
            return *m_threads[i];
        }
        //new function id
        unsigned int get_new_function_id()
        {
            m_functions.push_back(std::unique_ptr<l_function>(new l_function));
            return (unsigned int)m_functions.size()-1;
        }
        l_function& get_new_function()
        {
            unsigned int f_id = get_new_function_id();
            return *m_functions[f_id];
        }
        l_function& function(unsigned int i)
        {
            return *m_functions[i];
        }
        //get gc
        l_gc& get_gc();
        const l_gc& get_gc() const;
        //execute
        bool execute(unsigned int id_thread);
        bool execute(l_thread* this_thread);
        bool execute_call(l_variable& v_return,
                          l_variable& call,
                          std::initializer_list<l_variable> args);
        //
    };
}
