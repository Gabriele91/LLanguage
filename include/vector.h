//
//  vector.h
//  LLanguage
//
//  Created by Gabriele on 16/01/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#pragma once

#include <gc.h>
#include <variable.h>

namespace l_language
{
    //vector
    class l_vector;
    //vector iterator
    class l_vector_it;
    
    //implement vector
    class l_vector : public l_obj
    {
        
        //friend class
        friend class l_vector_it;
        
        //implement  object
        class l_pool_object
        {
            //friend class
            friend class l_vector_it;
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
            l_obj::mark();
            //mark childs
            for(auto& var : m_pool)
            {
                if(var.variable().is_object())
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
            l_obj::unmark();
            //mark childs
            for(auto& var : m_pool)
            {
                if(var.variable().is_object())
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
        
        void push(l_variable variable)
        {
            m_pool.push_back(l_pool_object(variable));
        }
        
        static l_variable gc_new(l_gc* gc)
        {
            return  gc->new_obj< l_vector >();
        }
        
        static l_variable gc_new(l_gc& gc)
        {
            return  gc.new_obj< l_vector >();
        }
        
        static l_variable gc_new(l_vm* vm)
        {
            return  gc_new(vm->get_gc());
        }
        
        static l_variable gc_new(l_vm& vm)
        {
            return  gc_new(vm.get_gc());
        }
        
        l_variable get_it();
        
    };
    
    class l_vector_it : public l_obj
    {
        l_vector*                       m_vector;
        l_vector::l_pool_object_list_it m_iterator;
        
        //mark event
        virtual void mark()
        {
            //..
            if(is_marked()) return;
            //mark
            l_obj::mark();
            m_vector->mark();
        }
        
        //unmark event
        virtual void unmark()
        {
            //..
            if(is_unmarked()) return;
            //mark
            l_obj::unmark();
            m_vector->unmark();
        }
        
    public:
        
        l_vector_it(l_variable vector)
        {
            if( vector.m_type == l_variable::OBJECT )
            {
                //get vector
                m_vector = dynamic_cast< l_vector* >(vector.m_value.m_pobj);
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
        
        l_vector_it(l_vector* vector)
        {
            //save vector
            m_vector = vector;
            //get iterator
            m_iterator = m_vector->m_pool.begin();
        }
        
        l_vector_it(const l_vector::l_pool_object_list_it& c_it)
        {
            //get iterator
            m_iterator = c_it;
            assert(0);
        }
        
        static l_variable gc_new(l_gc* gc,l_vector* vector)
        {
            return  gc->new_obj< l_vector_it >(vector);
        }
        
        static l_variable gc_new(l_gc& gc,l_vector* vector)
        {
            return  gc.new_obj< l_vector_it >(vector);
        }
        
        static l_variable gc_new(l_vm* vm,l_vector* vector)
        {
            return  vm->get_gc().new_obj< l_vector_it >(vector);
        }
        
        static l_variable gc_new(l_vm& vm,l_vector* vector)
        {
            return  vm.get_gc().new_obj< l_vector_it >(vector);
        }
        
        static l_variable gc_new(l_gc* gc,l_vector::l_pool_object_list_it c_it)
        {
            return  gc->new_obj< l_vector_it >(c_it);
        }
        
        static l_variable gc_new(l_gc& gc,l_vector::l_pool_object_list_it c_it)
        {
            return  gc.new_obj< l_vector_it >(c_it);
        }
        
        static l_variable gc_new(l_vm* vm,l_vector::l_pool_object_list_it c_it)
        {
            return  vm->get_gc().new_obj< l_vector_it >(c_it);
        }
        
        static l_variable gc_new(l_vm& vm,l_vector::l_pool_object_list_it c_it)
        {
            return  vm.get_gc().new_obj< l_vector_it >(c_it);
        }
        
        l_variable get() const
        {
            return m_iterator->m_variable;
        }
        
        l_variable get_id()
        {
            l_vector::l_pool_object_list_it begin = m_vector->m_pool.begin();
            auto difference = std::distance(begin,m_iterator);
            return l_variable((int)difference);
        }
        
        bool valid() const
        {
            return m_vector && m_vector->m_pool.end() != m_iterator;
        }
        
        l_variable next()
        {
            //next
            return l_vector_it::gc_new(get_gc(),m_iterator+1);
        }
        
        void self_next()
        {
            ++m_iterator;
        }
        
    };
    
    //get iterator
    inline l_variable l_vector::get_it()
    {
        return l_vector_it::gc_new(get_gc(), this);
    }
};
