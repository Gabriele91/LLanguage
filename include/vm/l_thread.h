//
//  l_thread.h
//  LLanguage
//
//  Created by Gabriele on 22/03/16.
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
        //type register
        enum l_register_names
        {
            R_TEMP1,
            R_TEMP2,
            R_TEMP3,
            R_RETURN,
            R_CONTEXT,
            R_THIS,
            R_TEMP_CLASS,
            R_N_REGS
        };
        //temp variable
        l_variable m_register[R_N_REGS];
        //save vm
        l_vm* m_vm;
        //contexts
        l_variable m_main_ctx;
        //init (thread)
        l_thread(l_vm* vm = nullptr,
                 unsigned int main_id = 0,
                 size_t stack = 512);
        //access
        l_variable& get_last_context()
        {
            return m_register[R_CONTEXT];
        }
        l_variable& get_return()
        {
            return m_register[R_RETURN];
        }
        l_variable& get_this()
        {
            return m_register[R_THIS];
        }
        l_variable& get_class_temp()
        {
            return m_register[R_TEMP_CLASS];
        }
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
            m_register[R_RETURN] = var;
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
}

