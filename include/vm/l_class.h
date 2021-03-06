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
#include <l_array.h>
#include <l_table.h>
#include <l_attribute.h>
#include <l_object.h>
#include <l_thread.h>

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
        
    public:
        
        enum l_type_operator
        {
            OP_ADD,
            OP_MUL,
            OP_SUB,
            OP_DIV,
            OP_MOD,
            //...
            OP_RIGHT_ADD,
            OP_RIGHT_MUL,
            OP_RIGHT_SUB,
            OP_RIGHT_DIV,
            OP_RIGHT_MOD,
            //
            OP_UNM,
            //GET ITEM
            OP_GET_IT,
            OP_GET_VALUE,
            OP_GET_KEY,
            OP_GET_NEXT,
            OP_GET_VALID,
            //EXTRA
            OP_IT_CLASS,
            //
            OP_MAX
        };
        static l_type_operator get_op_id(const std::string& name);
        
        l_class()
        {
            //alloc
            m_operators[OP_MAX] = l_variable();
        }
        
        void set_class_name(const l_variable& c_name);
        
        l_variable get_class_name() const;
        
        void set_class_function(l_function* function);
        
        l_function* get_class_function() const;
        
        l_variable get_constructor();
        
        void add_variable (const l_variable& key,const l_variable&  value, l_attribute_access access = l_attribute_access::PUBLIC);
        
        void add_def      (const l_variable& key,const l_variable&  value, l_attribute_access access = l_attribute_access::PUBLIC);
        
        void add_operator (l_type_operator type,const l_variable&  value);
        
        void add_parent(const l_variable& value);
        
		l_attribute get_attribute(const l_variable&  key);

		l_variable get_value(const l_variable&  key);

		l_variable get_def(const l_variable&  key);
		
		bool can_access_from_context(const l_variable&  key, const l_call_context& context, l_attribute& attribute);
        
        l_variable get_operator(l_type_operator  type);
        
        l_variable new_object(l_thread* th);
        
        static l_variable gc_new(l_gc* gc);
        static l_variable gc_new(l_gc& gc);
        static l_variable gc_new(l_vm* vm);
        static l_variable gc_new(l_vm& vm);
        
    private:
        //attributes
        enum l_map_type
        {
            M_ATTRS,
            M_DEFS,
            M_NMAP
        };
        //maps
        l_function*     m_function   { nullptr };
        l_variable      m_class_name;
        l_array		    m_parents;
        l_array		    m_operators;
        l_map_attribute m_maps[M_NMAP];
        
        //mark event
        virtual void mark();
        
        //unmark event
        virtual void unmark();

		//used in can_access_from_context
		bool is_derivate_of(const l_class* clazz) const;

        //exists def?
        bool exists_def(const l_variable& key,l_map_attribute_const_it& it) const;
    };

}
