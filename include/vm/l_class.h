//
//  l_class.h
//  LLanguage
//
//  Created by Gabriele on 14/04/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#pragma once
#include <map>
#include <memory>
#include <assert.h>
#include <unordered_map>
#include <l_ref.h>
#include <l_gc.h>
#include <l_object.h>
#include <l_thread.h>
#include <l_array.h>
#include <l_table.h>

namespace l_language
{
    //table
    class l_table;
    //table iterator
    class l_table_it;
    class l_call_context;
    class l_vm;
    class l_thread;
    class l_class;
    class l_object;
    
    
    //implementation
    class l_class : public l_ref
    {
        //attributes
        enum
        {
            M_ATTRS,
            M_DEFS,
            M_OPERS,
            M_NMAP
        };
        //maps
        l_variable   m_class_name;
		l_array		 m_parents;
        l_map_object m_maps[M_NMAP];
        
        //mark event
        virtual void mark();

        //unmark event
        virtual void unmark();

		//exists def?
        bool exists_def(const l_variable& key,l_map_object_const_it& it) const;
        
        
    public:
        
        l_class()
        {
        }
        
        void set_class_name(const l_variable& c_name);
        
        l_variable get_constructor();
        
        void add_variable (const l_variable& key,const l_variable&  value);
        
        void add_def      (const l_variable& key,const l_variable&  value);
        
        void add_operator (const l_variable& key,const l_variable&  value);
        
        void add_parent(const l_variable& value);
        
        l_variable get_value(const l_variable&  key);

        l_variable get_def(const l_variable&  key);
        
        l_variable new_object(l_thread* th);
        
        static l_variable gc_new(l_gc* gc);
        static l_variable gc_new(l_gc& gc);
        static l_variable gc_new(l_vm* vm);
        static l_variable gc_new(l_vm& vm);
    };

}