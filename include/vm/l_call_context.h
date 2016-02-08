//
//  l_call_context.h
//  LLanguage
//
//  Created by Gabriele on 08/02/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#pragma once
#include <l_config.h>
#include <l_variable.h>

namespace l_language
{
    class l_thread;
    class l_call_context;
    class l_vm;
    //context list
    struct l_call_context
    {
        //init context
        void init(unsigned int fun_id,l_thread* thread);
        //get/set
        l_variable& up_value(const l_variable& key)
        {
            return m_up_values[key];
        }
        
        l_variable& up_value(const l_variable& key,const l_variable& value)
        {
            return (m_up_values[key] = value);
        }
        
        l_variable& variable(const l_variable& key)
        {
            return m_local_variables[key];
        }
        
        l_variable& variable(const l_variable& key,const l_variable& value)
        {
            return (m_local_variables[key] = value);
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
            return m_up_values.size();
        }
        
        l_variable get_up_values() const
        {
            return l_variable((l_obj*)&m_up_values);;
        }
        
        l_variable get_local_variables() const
        {
            return l_variable((l_obj*)&m_local_variables);
        }
        
    protected:
        //ptr to thread
        l_thread*   m_thread { nullptr };
        //call fun id
        unsigned int m_fun_id { 0 };
        //stack size
        unsigned int m_call_top_stack{ 0 };
        //context
        l_table m_local_variables;
        //upvalue
        l_table m_up_values;
        //firends
        friend class l_thread;
        friend class l_vm;
        friend class l_gc;
    };
    
    //list contexts
    using l_list_call_function = std::vector < l_call_context >;
}