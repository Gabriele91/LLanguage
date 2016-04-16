//
//  l_class.h
//  LLanguage
//
//  Created by Gabriele on 14/04/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#pragma once
#pragma once
#include <map>
#include <memory>
#include <assert.h>
#include <unordered_map>
#include <l_object.h>
#include <l_thread.h>
#include <l_table.h>
#include <l_gc.h>

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
    class l_class_object;
    
    class l_class_object : public l_obj
    {
        //firend
        friend class l_class;
        //attributes
        l_variable   m_class;
        l_map_object m_attributes;
        //mark event
        virtual void mark()
        {
            //..
            if(is_marked()) return;
            //mark
            l_obj::mark();
            //mark class
            m_class.mark();
            //mark fields
            for(auto it:m_attributes)
            {
                l_variable& var = it.second;
                
                if(var.is_ref_obj())
                if(var.is_unmarked())
                {
                    var.mark();
                }
                
                //key is static
                const l_variable& key = it.first;
                //is a object?
                if(key.is_ref_obj())
                if(key.is_unmarked())
                {
                    //object
                    l_obj* key_object = key.m_value.m_pobj;
                    //mark
                    key_object->mark();
                }
            }

        }
        //mark event
        virtual void unmark()
        {
            //..
            if(is_unmarked()) return;
            //mark
            l_obj::unmark();
            //unmark class
            m_class.unmark();
            //unmark fields
            for(auto it:m_attributes)
            {
                l_variable& var = it.second;
                //...
                if(var.is_ref_obj())
                if(var.is_marked())
                {
                    var.unmark();
                }
                //key is static
                const l_variable& key = it.first;
                //is a object?
                if(key.is_ref_obj())
                if(key.is_marked())
                {
                    //object
                    l_obj* key_object = key.m_value.m_pobj;
                    //mark
                    key_object->unmark();
                }
            }
        }
        
    public:
        
        l_class_object(l_variable class_obj)
        {
            m_class = class_obj;
        }
        
        l_variable get_value(const l_variable&  key);
        bool       set_value(const l_variable&  key,const l_variable& value);
    };
    
    //implementation
    class l_class : public l_obj
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
        virtual void mark()
        {
            //..
            if(is_marked()) return;
            //mark
            m_class_name.mark();
			//mark parents
			m_parents.mark();
            //mark
            l_obj::mark();
            //mark childs
            for(auto& map:m_maps)
            for(auto it:map)
            {
                l_variable& var = it.second;
                
                if(var.is_ref_obj())
                if(var.is_unmarked())
                {
                    var.mark();
                }
                
                //key is static
                const l_variable& key = it.first;
                //is a object?
                if(key.is_ref_obj())
                if(key.is_unmarked())
                {
                    //object
                    l_obj* key_object = key.m_value.m_pobj;
                    //mark
                    key_object->mark();
                }
            }
        }

        //unmark event
        virtual void unmark()
        {
            //..
            if(is_unmarked()) return;
            //unmark
            m_class_name.unmark();
			//unmark parents
			m_parents.unmark();
            //unmark
            l_obj::unmark();
            //mark childs
            for(auto& map:m_maps)
            for(auto it:map)
            {
                l_variable& var = it.second;
                
                if(var.is_ref_obj())
                if(var.is_marked())
                {
                    var.unmark();
                }
                //key is static
                const l_variable& key = it.first;
                //is a object?
                if(key.is_ref_obj())
                if(key.is_marked())
                {
                    //object
                    l_obj* key_object = key.m_value.m_pobj;
                    //mark
                    key_object->unmark();
                }
            }
        }

		//exists def?
        bool exists_def(const l_variable& key,l_map_object_const_it& it) const
        {
            return (it=m_maps[M_DEFS].find(key))!=m_maps[M_DEFS].end();
        }
        
        
    public:
        
        l_class()
        {
        }
        
        void set_class_name(const l_variable& c_name)
        {
            m_class_name = c_name;
        }
        
        l_variable get_constructor()
        {
            return get_def(m_class_name);
        }
        
        void add_variable (const l_variable& key,const l_variable&  value)
        {
            m_maps[M_ATTRS][key] = value;
        }
        void add_def      (const l_variable& key,const l_variable&  value)
        {
            m_maps[M_DEFS][key] = value;
        }
        void add_operator (const l_variable& key,const l_variable&  value)
        {
            m_maps[M_OPERS][key] = value;
        }
		void add_parent(const l_variable& value)
		{
			//append
			m_parents.push(value);
			//get class
			const l_class* p_class = value.clazz();
			//get attributes
			const l_map_object& p_attrs = p_class->m_maps[M_ATTRS];
			//add attributes
			for (auto it : p_attrs)
			{
				m_maps[M_ATTRS][it.first] = it.second;
			}
		}
        
        l_variable get_value(const l_variable&  key)
        {
            for(l_map_object& map : m_maps)
            {
                //first
                l_map_object_it val_it = map.find(key);
                //find?
                if(val_it != map.end())
                {
                    //return
                    return val_it->second;
                }
            }
            
            return l_variable();
        }

        l_variable get_def(const l_variable&  key)
        {
            //first
            l_map_object_it val_it = m_maps[M_DEFS].find(key);
            //find?
            if(val_it != m_maps[M_DEFS].end()) return val_it->second;
			//search from parens
			for (auto rit = m_parents.m_pool.rbegin();
					  rit != m_parents.m_pool.rend(); 
				    ++rit)
			{
				//..
				l_variable& it_class = *rit;
				//pointer 
				l_variable& def = it_class.clazz()->get_def(key);
				//find?
				if (!def.is_null()) return def;
			}
            
            return l_variable();
        }
        
        l_variable new_object(l_thread* th)
        {
            //alloc
            l_variable var = th->get_gc()->new_obj<l_class_object>( l_variable(this) );
            //..
            l_class_object* object = var.to<l_class_object>();
            //copy attributes
            object->m_attributes = m_maps[M_ATTRS];
            //call gc
            return var;
        }
        
    };
    
    inline l_variable l_class_object::get_value(const l_variable&  key)
    {
        //first
        l_map_object_it val_it = m_attributes.find(key);
        //find?
        if(val_it != m_attributes.end())
        {
            //return
            return val_it->second;
        }
        //from class
        return m_class.clazz()->get_def(key);
    }
    
    inline bool l_class_object::set_value(const l_variable&  key,const l_variable& value)
    {
        //first
        l_map_object_it val_it = m_attributes.find(key);
        //find?
        if(val_it != m_attributes.end())
        {
            //set
            val_it->second=value;
            //return
            return true;
        }
        return false;
    }
}