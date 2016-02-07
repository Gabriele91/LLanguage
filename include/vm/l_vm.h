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

namespace l_language
{
    //context list
    struct l_call_context
    {
    
        //auto alloc
        void alloc(const l_list_function& list)
        {
            m_vars.resize(list[m_fun_id].m_values_size);
            m_up_values.resize(list[m_fun_id].m_up_val_size);
        }
        void alloc(unsigned int fun_id,const l_list_function& list)
        {
            m_fun_id = fun_id;
            m_vars.resize(list[m_fun_id].m_values_size);
            m_up_values.resize(list[m_fun_id].m_up_val_size);
        }
        //get/set
        l_variable& up_value(int i)
        {
            return m_up_values[i];
        }
        
        l_variable& variable(int i)
        {
            return m_vars[i];
        }
        
        unsigned int get_fun_id()
        {
            return m_fun_id;
        }
        
        size_t get_size_up_values() const
        {
            return m_up_values.size();
        }
        
        size_t get_size_vars() const
        {
            return m_vars.size();
        }
        
    protected:
        //call fun id
        unsigned int m_fun_id { 0 };
        //upvalue
        l_list_variables m_up_values;
        //context
        l_list_variables m_vars;
    };
    
    //list contexts
    using l_list_call_function = std::vector < l_call_context >;
    
    //thread
    struct l_thread
    {
        //main function id
        unsigned int m_main_fun_id { 0 };
        //program counter
        unsigned int m_pc { 0 };
        //current index
        long m_top{ -1 };
        //temp variable
        l_variable m_register[3];
        //save vm
        l_vm* m_vm;
        //contexts
        l_list_call_function m_contexts;
        //init (thread)
        l_thread(l_vm* vm = nullptr,
                 unsigned int main_id = 0,
                 size_t stack = 512);
        //access
        l_variable& global(int i);
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
        void push_return(const l_variable& var,size_t nargs)
        {
            if(nargs == 0) push(var);
            else value(nargs-1) = var;
        }
        //get top
        l_variable& value(size_t x)
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
        
    };
    
    //list threads
    using l_list_threads = std::vector < l_thread >;
    
    class l_vm
    {
        //gc
        l_gc* m_gc;
        
    public:
        //init vm
        l_vm();
        virtual ~l_vm();
        //global modules
        l_list_variables m_globals;
        //thread list
        l_list_threads   m_threads;
        //functions
        l_list_function  m_functions;
        //get gc
        l_gc& get_gc()
        {
            return *m_gc;
        }
        const l_gc& get_gc() const
        {
            return *m_gc;
        }
        //execute
        void execute(unsigned int id_thread);
        void execute(l_thread* this_thread);
        //
    };
}
