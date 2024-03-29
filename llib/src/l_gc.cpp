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
#include <l_array.h>
#include <l_xrange.h>
#include <l_class.h>
#include <l_object.h>
#include <l_call_context.h>


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
             printf("dealloc: %u, id %u",
                    (unsigned)it->m_size,
                    (unsigned)it->m_id);
                 //if is a vector?
                 l_array*  array = dynamic_cast< l_array* >(it->m_obj);
                 l_table*  table  = dynamic_cast< l_table* >(it->m_obj);
                 //if is a vector?
                 l_array_it*  vector_it = dynamic_cast< l_array_it* >(it->m_obj);
                 l_table_it*  table_it  = dynamic_cast< l_table_it* >(it->m_obj);
                 //if is a class?
                 l_class*  clazz = dynamic_cast< l_class* >(it->m_obj);
                 l_object* object = dynamic_cast< l_object* >(it->m_obj);
                 // Closer
                 l_closer*  closer = dynamic_cast< l_closer* >(it->m_obj);
                 // Closer
                 l_xrange*  xrange = dynamic_cast< l_xrange* >(it->m_obj);
                 //print vector
                      if(array)
                        printf("; array[%u]", (unsigned)array->size());
                 else if(table)
                        printf("; table[%u]", (unsigned)table->size());
                 else if(vector_it)
                        printf("; array it");
                 else if(table_it)
                        printf("; table it");
                 else if(clazz)
                        printf("; class");
                 else if(closer)
                        printf("; closer");
                 else if(xrange)
                        printf("; xrange");
                 //new line
                 printf("\n");
            )
            free(it);
        }
    }
    
    //dealloc consts
    l_gc::~l_gc()
    {
        //dealloc consts
        for(auto&  fun : m_vm.m_functions)
        {
            for(l_variable&  var : fun->m_costants)
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
    void  l_gc::push(l_ref* obj,size_t size)
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

    void  l_gc::remove(l_ref* obj)
    {
        l_pool::iterator it;
        for (it = m_pool.begin();
             it != m_pool.end();
             ++it)
        {
            if (it->m_obj == obj) { remove(it); return; }
        }
    }

    void  l_gc::free(l_ref* obj)
    {
        remove(obj);
        delete obj;
    }

    void  l_gc::free(l_pool::iterator it)
    {
        l_ref* obj = it->m_obj;
        remove(it);
        delete obj;
    }

    void  l_gc::mark_pool()
    {
        for(auto& thread  : m_vm.m_threads)
        {
            //temporal register
            for(l_variable& var : thread->m_register)
            {
                if(var.is_ref_obj() && var.is_unmarked())
                {
                    var.mark();
                }
            }
            //main context mark
            thread->m_main_ctx.mark();
            //stack mark
            for(long i=0;i <= thread->m_top /* n.b. top can get -1 */;++i)
            {
                l_variable& var  = thread->value(i);
                
                if(var.is_ref_obj() && var.is_unmarked())
                {
                    var.mark();
                }
            }
        }
        // Consts
        for(auto& function : m_vm.m_functions)
        for(auto& constant : function->m_costants)
        {
            if(constant.is_ref_obj() && constant.is_unmarked())
            {
                constant.mark();
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