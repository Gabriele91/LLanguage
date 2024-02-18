//
//  l_table.cpp
//  LLanguage
//
//  Created by Gabriele on 07/02/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//

#include <l_array.h>
#include <l_variable.h>
#include <l_exception.h>
#include <l_gc.h>
#include <l_vm.h>


namespace l_language
{
    static int l_array_clear(l_thread* thread,int args)
    {
        l_variable& thiz =thread->get_this();
        //get array
        if(!thiz.is_array())
        {
            thread->push_error(l_exception_not_valid_this(thread,"this isn't array"));
            return -1;
        }
        //push size
        thiz.array()->clear();
        //return
        return 0;
    }
    
    static int l_array_pop(l_thread* thread,int args)
    {
        l_variable& thiz =thread->get_this();
        //get array
        if(!thiz.is_array())
        {
            thread->push_error(l_exception_not_valid_this(thread,"this isn't array"));
            return -1;
        }
        //push size
        thread->push_return(thiz.array()->pop());
        //return
        return 1;
    }
    
    static int l_array_len(l_thread* thread,int args)
    {
        l_variable& thiz =thread->get_this();
        //get array
        if(!thiz.is_array())
        {
            thread->push_error(l_exception_not_valid_this(thread,"this isn't array"));
            return -1;
        }
        //push size
        thread->push_return({(l_int)thiz.array()->size()});
        //return
        return 1;
    }
    
    static int l_array_push(l_thread* thread,int args)
    {
        l_variable& thiz =thread->get_this();
        //get array
        if(!thiz.is_array())
        {
            thread->push_error(l_exception_not_valid_this(thread,"this isn't array"));
            return -1;
        }
        //push all
        for (int i = 0; i!=args; ++i)
            thiz.array()->push(thread->value(0));
        //return 0 values
        return 0;
    }
    
    l_variable l_array::operator[](const l_language::l_variable &variable)
    {
        if(variable.can_to_int())
        {
            return m_pool[variable.to_int()].variable();
        }
        else if (variable.is_string())
        {
            if(variable.string()->str() == "size")
            {
                return l_variable(l_array_len);
            }
            else if(variable.string()->str() == "push")
            {
                return l_variable(l_array_push);
            }
            else if(variable.string()->str() == "pop")
            {
                return l_variable(l_array_pop);
            }
            else if(variable.string()->str() == "clear")
            {
                return l_variable(l_array_clear);
            }
        }
        //null
        return l_variable();
    }
    
    l_variable l_array::gc_new(l_gc* gc)
    {
        return  gc->new_obj< l_array >();
    }
    
    l_variable l_array::gc_new(l_gc& gc)
    {
        return  gc.new_obj< l_array >();
    }
    
    l_variable l_array::gc_new(l_vm* vm)
    {
        return  gc_new(vm->get_gc());
    }
    
    l_variable l_array::gc_new(l_vm& vm)
    {
        return  gc_new(vm.get_gc());
    }
    
    l_variable l_array_it::gc_new(l_gc* gc,l_array* vector)
    {
        return  gc->new_obj< l_array_it >(vector);
    }
    
    l_variable l_array_it::gc_new(l_gc& gc,l_array* vector)
    {
        return  gc.new_obj< l_array_it >(vector);
    }
    
    l_variable l_array_it::gc_new(l_vm* vm,l_array* vector)
    {
        return  vm->get_gc().new_obj< l_array_it >(vector);
    }
    
    l_variable l_array_it::gc_new(l_vm& vm,l_array* vector)
    {
        return  vm.get_gc().new_obj< l_array_it >(vector);
    }
    
    l_variable l_array_it::gc_new(l_gc* gc,l_array::l_pool_object_list_it c_it)
    {
        return  gc->new_obj< l_array_it >(c_it);
    }
    
    l_variable l_array_it::gc_new(l_gc& gc,l_array::l_pool_object_list_it c_it)
    {
        return  gc.new_obj< l_array_it >(c_it);
    }
    
    l_variable l_array_it::gc_new(l_vm* vm,l_array::l_pool_object_list_it c_it)
    {
        return  vm->get_gc().new_obj< l_array_it >(c_it);
    }
    
    l_variable l_array_it::gc_new(l_vm& vm,l_array::l_pool_object_list_it c_it)
    {
        return  vm.get_gc().new_obj< l_array_it >(c_it);
    }
}