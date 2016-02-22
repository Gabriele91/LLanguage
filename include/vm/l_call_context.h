//
//  l_call_context.h
//  LLanguage
//
//  Created by Gabriele on 08/02/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#pragma once
#include <l_config.h>
#include <l_object.h>
#include <l_variable.h>
#include <l_table.h>

namespace l_language
{
    class l_thread;
    class l_call_context;
    class l_vm;
    //context list
	class l_call_context : public l_obj
    {
	public:
        //init context
        void init(unsigned int fun_id,l_thread* thread);
        void init(unsigned int fun_id,l_thread* thread,const l_variable& this_context);
        //locals
        l_variable& variable(const l_variable& key)
        {
            return m_local_variables[key];
        }
        bool exists(const l_variable& key)
        {
            return m_local_variables.exists(key);
        }
        //add a local
        l_variable& variable(const l_variable& key,const l_variable& value)
        {
            return (m_local_variables[key] = value);
        }
        
        l_function_id get_fun_id() const
        {
            return m_fun_id;
        }
        
        size_t get_size_vars() const
        {
            return m_local_variables.size();
        }
        
        l_variable get_local_variables() const
        {
            return l_variable((l_obj*)&m_local_variables);
        }
        
        l_call_context* next()
        {
            return m_next.to<l_call_context>();
        }
    
        
        static l_variable gc_new(l_gc* gc);
        
        static l_variable gc_new(l_gc& gc);
        
        static l_variable gc_new(l_vm* vm);
        
        static l_variable gc_new(l_vm& vm);
        
        static l_variable const_new(l_gc* gc);
        
        static l_variable const_new(l_gc& gc);
        
        static l_variable const_new(l_vm* gc);
        
        static l_variable const_new(l_vm& gc);
        
    protected:
        
        //unmark event
        virtual void mark();
        
        //unmark event
        virtual void unmark();
        
        //context lock?
        mutable bool m_context_lock { false };
        //lock /unlock
        void lock()
        {
            m_context_lock = true;
        }
        bool is_lock() const
        {
            return m_context_lock ;
        }
        void unlock()
        {
            m_context_lock = false;
        }
        bool is_unlock() const
        {
            return !m_context_lock ;
        }
        //get is mark
        virtual bool is_marked() const
        {
            return l_obj::is_marked() || m_context_lock;
        }
        //get is unmark
        virtual bool is_unmarked() const
        {
            return l_obj::is_unmarked() && !m_context_lock;
        }
        //ptr to thread
        l_thread*   m_thread { nullptr };
        //call fun id
        l_function_id m_fun_id { 0 };
        //context
        l_table m_local_variables;
        //next
        l_variable m_next;
        //firends
        friend class l_thread;
        friend class l_vm;
        friend class l_gc;
    };
    
    //list contexts
    using l_static_list_call_function = std::vector < l_call_context >;
    using l_list_call_function = std::vector < l_variable >;
}