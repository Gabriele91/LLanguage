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
            :m_variable(false)
            {
            }
            
            l_pool_object(const l_variable& variable)
            :m_variable(false)
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
        
        virtual void mark_childs() const
        {
            for (auto& obj : m_pool)
                if (obj.variable().m_type == l_variable::OBJECT)
                {
                    obj.variable().mark();
                }
        }
        
        virtual void unmark_childs() const
        {
            for (auto& obj : m_pool)
                if (obj.variable().m_type == l_variable::OBJECT)
                {
                    obj.variable().unmark();
                }
        }
        
        virtual bool has_childs() const
        {
            return true;
        }
        
    public:
        
        size_t size() const
        {
            return m_pool.size();
        }
        
        l_variable& operator[](size_t i)
        {
            if (size() <= i) m_pool.resize(i+1);
            return m_pool[i];
        }
        
        void push(l_variable variable)
        {
            m_pool.push_back(variable);
        }
        
        static l_variable gc_new()
        {
            return   l_variable::stack((l_obj*)l_gc::s_global_gc.new_obj< l_vector >());
        }
        
        l_variable get_it();
        
    };
    
    class l_vector_it : public l_obj
    {
        const l_vector*                       m_vector;
        l_vector::l_pool_object_list_const_it m_iterator;
        
        virtual void mark_childs() const
        {
            if(m_vector)
            {
                m_vector->mark();
                
                if(m_vector->m_pool.end() != m_iterator)
                {
                    m_iterator->m_variable.mark();
                }
            }
        }
        
        virtual void unmark_childs() const
        {
            if(m_vector)
            {
                m_vector->unmark();
                
                if(m_vector->m_pool.end() != m_iterator)
                {
                    m_iterator->m_variable.unmark();
                }
            }
        }
        
        virtual bool has_childs() const
        {
            return valid();
        }
        
    public:
        
        l_vector_it(l_variable vector)
        {
            if( vector.m_type == l_variable::OBJECT )
            {
                //get vector
                m_vector = dynamic_cast< const l_vector* >(vector.m_value.m_pobj);
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
        
        l_vector_it(l_vector::l_pool_object_list_const_it c_it)
        {
            //get iterator
            m_iterator = ++c_it;
            assert(0);
        }
        
        template < typename ... A >
        static l_variable gc_new(A... args)
        {
            return   l_variable::stack((l_obj*)l_gc::s_global_gc.new_obj< l_vector_it >(args...));
        }
        
        l_variable get() const
        {
            return m_iterator->m_variable;
        }
        
        l_variable get_id() const
        {
            auto difference = std::distance(m_vector->m_pool.begin(),m_iterator);
            return l_variable::stack((int)difference);
        }
        
        bool valid() const
        {
            return m_vector && m_vector->m_pool.end() != m_iterator;
        }
        
        l_variable next() const
        {
            return   l_variable::heap((l_obj*)l_gc::s_global_gc.new_obj< l_vector_it >(m_iterator + 1));
        }
        
        void self_next()
        {
            ++m_iterator;
        }
        
    };
    
    //get iterator
    inline l_variable l_vector::get_it()
    {
        return l_vector_it::gc_new(this);
    }
};
