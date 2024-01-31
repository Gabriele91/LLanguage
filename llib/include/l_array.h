//
//  l_array.h
//  LLanguage
//
//  Created by Gabriele on 16/01/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#pragma once

#include <l_gc.h>
#include <l_variable.h>
#include <l_iterator.h>

namespace l_language
{
	//vector
	class l_array;
	//class
	class l_class;
    //vector iterator
    class l_array_it;
    
    //implement vector
    class l_array : public l_ref
    {
        
        //friend class
		friend class l_class;
        friend class l_array_it;
        
        //implement  object
        class l_pool_object
        {
            //friend class
            friend class l_array_it;
            //attributes
            l_variable m_variable;
            
        public:
            
            l_pool_object()
            {
            }
            
            l_pool_object(const l_variable& variable)
            {
                m_variable = variable;
            }
            
            l_variable& variable()
            {
                return m_variable;
            }
            
            const l_variable& variable() const
            {
                return m_variable;
            }
            
            operator l_variable&()
            {
                return m_variable;
            }
            
            operator const l_variable& () const
            {
                return m_variable;
            }
        };
        using l_pool_object_list           = std::vector< l_pool_object >;
        using l_pool_object_list_it        = l_pool_object_list::iterator;
        using l_pool_object_list_const_it  = l_pool_object_list::const_iterator;
        
        l_pool_object_list m_pool;
        
        //mark event
        virtual void mark()
        {
            //..
            if(is_marked()) return;
            //mark
            l_ref::mark();
            //mark childs
            for(auto& var : m_pool)
            {
                if(var.variable().is_ref_obj())
                if(var.variable().is_unmarked())
                {
                    var.variable().mark();
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
            for(auto& var : m_pool)
            {
                if(var.variable().is_ref_obj())
                if(var.variable().is_marked())
                {
                    var.variable().unmark();
                }
            }
        }
        
    public:
        
        size_t size() const
        {
            return m_pool.size();
        }
        
        l_variable& operator[](size_t i)
        {
            if (size() <= i) m_pool.resize(i+1);
            return m_pool[i].variable();
        }
        
        const l_variable& operator[](size_t i) const
        {
            return m_pool[i].variable();
        }
        
        l_variable operator[](const l_variable& variable);
        
        void push(const l_variable& variable)
        {
            m_pool.push_back(l_pool_object(variable));
        }
        
        void clear()
        {
            m_pool.clear();
        }
        
        l_variable pop()
        {
            if(!size()) return l_variable();
            //gest last
            l_variable last = m_pool[size()-1];
            //pop
            m_pool.pop_back();
            //return
            return last;
        }
        
        l_pool_object_list& get_raw_array()
        {
            return m_pool;
        }
        
        static l_variable gc_new(l_gc* gc);
        
        static l_variable gc_new(l_gc& gc);
        
        static l_variable gc_new(l_vm* vm);
        
        static l_variable gc_new(l_vm& vm);
        
        l_variable get_it();
        
    };
    
    class l_array_it : public l_iterator
    {
        l_array*                       m_vector;
        l_array::l_pool_object_list_it m_iterator;
        
        //mark event
        virtual void mark()
        {
            //..
            if(is_marked()) return;
            //mark
            l_ref::mark();
            m_vector->mark();
        }
        
        //unmark event
        virtual void unmark()
        {
            //..
            if(is_unmarked()) return;
            //mark
            l_ref::unmark();
            m_vector->unmark();
        }
        
    public:
        
        l_array_it(l_variable vector)
        {
            if( vector.m_type == l_variable::OBJECT )
            {
                //get vector
                m_vector = dynamic_cast< l_array* >(vector.m_value.m_pobj);
                //if is a vector
                if( m_vector )
                {
                    //get iterator
                    m_iterator = m_vector->m_pool.begin();
                    //success
                    return;
                }
            }
            assert(0);
        }
        
        l_array_it(l_array* vector)
        {
            //save vector
            m_vector = vector;
            //get iterator
            m_iterator = m_vector->m_pool.begin();
        }
        
        l_array_it(const l_array::l_pool_object_list_it& c_it)
        {
            //get iterator
            m_iterator = c_it;
            assert(0);
        }
        
        static l_variable gc_new(l_gc* gc,l_array* vector);
        
        static l_variable gc_new(l_gc& gc,l_array* vector);
        
        static l_variable gc_new(l_vm* vm,l_array* vector);
        
        static l_variable gc_new(l_vm& vm,l_array* vector);
        
        static l_variable gc_new(l_gc* gc,l_array::l_pool_object_list_it c_it);
        
        static l_variable gc_new(l_gc& gc,l_array::l_pool_object_list_it c_it);
        
        static l_variable gc_new(l_vm* vm,l_array::l_pool_object_list_it c_it);
        
        static l_variable gc_new(l_vm& vm,l_array::l_pool_object_list_it c_it);
        
        virtual l_variable get() const
        {
            return m_iterator->m_variable;
        }
        
        virtual l_variable get_id() const
        {
            l_array::l_pool_object_list_it begin = m_vector->m_pool.begin();
            auto difference = std::distance(begin,m_iterator);
            return l_variable((int)difference);
        }
        
        virtual bool valid() const
        {
            return m_vector && m_vector->m_pool.end() != m_iterator;
        }
        
        virtual l_variable next() const
        {
            //next
            return l_array_it::gc_new(get_gc(),m_iterator+1);
        }
        
        virtual void self_next()
        {
            ++m_iterator;
        }
        
    };
    
    //get iterator
    inline l_variable l_array::get_it()
    {
        return l_array_it::gc_new(get_gc(), this);
    }
};
