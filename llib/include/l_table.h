//
//  l_table.h
//  LLanguage
//
//  Created by Gabriele on 28/01/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#pragma once
#include <map>
#include <memory>
#include <assert.h>
#include <unordered_map>
#include <l_ref.h>
#include <l_variable.h>
#include <l_iterator.h>

namespace l_language
{
    //table
    class l_table;
    //table iterator
    class l_table_it;
    class l_call_context;
    class l_vm;
    class l_thread;
    
    //value compare
    struct l_value_compare
    {
        size_t operator()(const l_variable& left) const
        {
            //input
            const size_t* cptr = (const size_t*)left.string()->str().c_str();
            //return
            return *cptr;
        }
        
        bool operator()(const l_variable& left,const l_variable& right) const
        {
            return left.string()->str() == right.string()->str();
        }
    };
    //type
    using l_map_object           = std::unordered_map< l_variable,
                                                       l_variable,
                                                       l_value_compare,
                                                       l_value_compare >;
    using l_map_object_it        = l_map_object::iterator;
    using l_map_object_const_it  = l_map_object::const_iterator;
    
    //implementation
    class l_table : public l_ref
    {
        //friend class
        friend class l_table_it;
        friend class l_call_context;
        friend class l_vm;
        friend class l_thread;
        //table
        l_map_object m_map;
        //mark event
        virtual void mark()
        {
            //..
            if(is_marked()) return;
            //mark
            l_ref::mark();
            //mark childs
            for(auto& it:m_map)
            {
                l_variable& var = it.second;
                // Mark attrbute
                if(var.is_ref_obj() && var.is_unmarked())
                {
                    var.mark();
                } 
                //key is static
                const l_variable& key = it.first;
                //is a object?
                if(key.is_ref_obj() && key.is_unmarked())
                {
                    //object
                    l_variable* key_object = (l_variable*)&key;
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
            //mark
            l_ref::unmark();
            //mark childs
            for(auto it:m_map)
            {
                l_variable& var = it.second;
                
                if(var.is_ref_obj() && var.is_marked())
                {
                    var.unmark();
                }
                //key is static
                const l_variable& key = it.first;
                //is a object?
                if(key.is_ref_obj() && key.is_marked())
                {
                    //object
                    l_variable* key_object = (l_variable*)&key;
                    //mark
                    key_object->unmark();
                }
            }
        }
        //clear map
        void clear()
        {
            m_map.clear();
        }
        
    public:
        
		const l_map_object& raw() const
		{
			return m_map;
		}

        size_t size() const
        {
            return m_map.size();
        }
        
        l_variable& operator[](const l_variable& key)
        {
            assert(key.is_string());
            return m_map[key];
        }
        
        void push(const l_variable& key,const l_variable&  value)
        {
            assert(key.is_string());
            m_map[key] = value;
        }
        
        bool exists(const l_variable& key) const
        {
            assert(key.is_string());
            return m_map.find(key)!=m_map.end();
        }
        
        l_map_object& get_raw_map()
        {
            return m_map;
        }
        
        static l_variable gc_new(l_gc* gc);
        
        static l_variable gc_new(l_gc& gc);
        
        static l_variable gc_new(l_vm* vm);
        
        static l_variable gc_new(l_vm& vm);
        
        static l_variable const_new(l_gc* gc);
        
        static l_variable const_new(l_gc& gc);
        
        static l_variable const_new(l_vm* gc);
        
        static l_variable const_new(l_vm& gc);
        
        l_variable get_it();
        
    };
    
    class l_table_it : public l_iterator
    {
        l_table*         m_table;
        l_map_object_it  m_iterator;
        
        //mark event
        virtual void mark()
        {
            //..
            if(is_marked()) return;
            //mark
            l_ref::mark();
            m_table->mark();
        }
        
        //unmark event
        virtual void unmark()
        {
            //..
            if(is_unmarked()) return;
            //mark
            l_ref::unmark();
            m_table->unmark();
        }
        
    public:
        
        l_table_it(l_variable table)
        {
            if( table.m_type == l_variable::OBJECT )
            {
                //get vector
                m_table = dynamic_cast< l_table* >(table.m_value.m_pobj);
                //if is a vector
                if( m_table )
                {
                    //get iterator
                    m_iterator = m_table->m_map.begin();
                    //success
                    return;
                }
            }
            assert(0);
        }
        
        l_table_it(l_table* table)
        {
            //save table
            m_table = table;
            //get iterator
            m_iterator = m_table->m_map.begin();
        }
        
        l_table_it(const l_map_object_it& c_it)
        {
            //get iterator
            m_iterator = c_it;
            assert(0);
        }
        
        static l_variable gc_new(l_gc* gc,l_table* vector);
        static l_variable gc_new(l_gc& gc,l_table* vector);
        static l_variable gc_new(l_vm* vm,l_table* vector);
        static l_variable gc_new(l_vm& vm,l_table* vector);
        static l_variable gc_new(l_gc* gc,l_map_object_it c_it);
        static l_variable gc_new(l_gc& gc,l_map_object_it c_it);
        static l_variable gc_new(l_vm* vm,l_map_object_it c_it);
        static l_variable gc_new(l_vm& vm,l_map_object_it c_it);
        
        virtual l_variable get() const
        {
            return m_iterator->second;
        }
        
        virtual l_variable get_id() const
        {
            return m_iterator->first;
        }
        
        virtual bool valid() const
        {
            return m_table && m_table->m_map.end() != m_iterator;
        }
        
        virtual l_variable next() const
        {
            //auto copy it
            l_map_object_it next_it = m_iterator;
            //next
            ++next_it;
            //next
            return gc_new(l_ref::get_gc(),m_iterator);
        }
        
        virtual void self_next()
        {
            ++m_iterator;
        }
        
    };
    
    //get iterator
    inline l_variable l_table::get_it()
    {
        return l_table_it::gc_new(get_gc(), this);
    }
}