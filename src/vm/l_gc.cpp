//
//  gc.cpp
//  LLanguage
//
//  Created by Gabriele on 16/01/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//

#include <stdio.h>
#include <vector>
#include <l_gc.h>
#include <l_vm.h>
#include <l_variable.h>
#include <l_vector.h>


namespace  l_language
{
    
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
        //delete
        for (l_pool::iterator it : to_delete)
        {
            GC_DEBUG(
             //
             printf("dealloc: %u, id %u\n",
                    (unsigned)it->m_size,
                    (unsigned)it->m_id);
                 //if is a vector?
                 l_vector*  vector = dynamic_cast< l_vector* >(it->m_obj);
                 //if is a vector?
                 l_vector_it*  vector_it = dynamic_cast< l_vector_it* >(it->m_obj);
                 //print vector
                 if(vector)
                 {
                     printf("vector[%u]\n", (unsigned)vector->size());
                 }
                 if(vector_it)
                 {
                     printf("vector_it\n");
                 }
            )
            free(it);
        }
    }
    
    //dealloc consts
    l_gc::~l_gc()
    {
        //dealloc consts
        for(l_function&  fun : m_vm.m_functions)
        {
            for(l_variable&  var : fun.m_costants)
            {
                if(var.is_string() && var.m_const)
                {
                    delete var.string();
                }
            }
        }
        //dealloc all heap gc
        for(auto& it : m_pool)
        {
            delete it.m_obj;
        }
    }

    //gc actions
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
        
        for(l_thread& thread  : m_vm.m_threads)
        {
            //temporal register
            for(l_variable& var : thread.m_register)
            {
                if(var.is_object() && var.is_unmarked())
                {
                    var.mark();
                }
            }
            //main context mark
            thread.m_main_ctx.mark();
            //stack mark
            for(long i=0;i <= thread.m_top /* n.b. top can get -1 */;++i)
            {
                l_variable& var  = thread.value(i);
                
                if(var.is_object() && var.is_unmarked())
                {
                    var.mark();
                }
            }
        }
    }

    inline void l_gc::unmark_pool()
    {
        for (auto& ref_obj : m_pool)
        {
            ref_obj.m_obj->unmark();
        }
    }
    
};