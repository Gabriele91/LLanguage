//
//  gc.cpp
//  LLanguage
//
//  Created by Gabriele on 16/01/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//

#include <stdio.h>
#include <gc.h>
#include <variable.h>
#include <vector>


namespace  l_language
{
    
    //init ref
    l_ref::l_ref(l_variable& variable, bool is_in_stack)
    :m_variable(variable)
    ,m_is_in_stack(is_in_stack)
    {
        if (m_is_in_stack)
        {
            l_gc::s_global_gc.push(this);
        }
    }
    l_ref::~l_ref()
    {
        if (m_is_in_stack)
        {
            l_gc::s_global_gc.remove(this);
        }
    }
    
    //init global gc
    l_gc l_gc::s_global_gc;
    
    //gc max heap size
    void  l_gc::set_max_alloc_size(size_t max_alloc)
    {
        m_max_size = max_alloc;
    }

    //execute garbage collector
    void  l_gc::delete_garbage()
    {
        //mark fase
        unmark_pool();
        mark_pool();
        //list of object to delete
        std::vector< l_pool::iterator > to_delete;
        to_delete.reserve(m_pool.size());
        //search fase
        for (l_pool::iterator it  = m_pool.begin();
             it != m_pool.end();
             ++it)
        {
            if (!it->m_obj->is_marked()) to_delete.push_back(it);
        }
        //disable ref count delete...
        for (l_pool::iterator it : to_delete) it->m_obj->m_self_delete = false;
        //delete
        for (l_pool::iterator it : to_delete) free(it);
    }

    //gc actions
    void  l_gc::push(l_ref* ref)
    {
        m_refs.push_back(ref);
    }

    void  l_gc::remove(l_refs::iterator it)
    {
        m_refs.erase(it);
    }

    void  l_gc::remove(l_ref* ref)
    {
        l_refs::iterator it;
        for (it  = m_refs.begin();
             it != m_refs.end();
             ++it)
        {
            if ((*it) == ref) { remove(it); return; }
        }
    }

    void  l_gc::push(l_obj* obj,size_t size)
    {
        m_size_allocs += size;
        m_pool.push_back(l_ref_obj( obj, size ));
    }

    void  l_gc::remove(l_pool::iterator it)
    {
        //dec size
        m_size_allocs -= it->m_size;
        //delete from pool
        m_pool.erase(it);
    }

    void  l_gc::remove(l_obj* obj)
    {
        l_pool::iterator it;
        for (it = m_pool.begin();
             it != m_pool.end();
             ++it)
        {
            if (it->m_obj == obj) { remove(it); return; }
        }
    }

    void  l_gc::free(l_obj* obj)
    {
        remove(obj);
        delete obj;
    }

    void  l_gc::free(l_pool::iterator it)
    {
        l_obj* obj = it->m_obj;
        remove(it);
        delete obj;
    }

    void  l_gc::mark_pool()
    {
        for (l_ref* ref_obj : m_refs)
            if (ref_obj->variable().m_type == l_variable::OBJECT)
            {
                ref_obj->variable().m_value.m_pobj->mark();
            }
    }

    inline void l_gc::unmark_pool()
    {
        for (auto& ref_obj : m_pool)
        {
            ref_obj.m_obj->m_mark = false;
        }
    }
    
};