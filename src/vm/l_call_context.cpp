//
//  l_call_context.cpp
//  LLanguage
//
//  Created by Gabriele on 07/02/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#include <l_object.h>
#include <l_call_context.h>
#include <l_vm.h>

namespace l_language
{
    
    
    void l_call_context::init(unsigned int fun_id,l_thread *thread)
    {
        //save pointer
        m_fun_id = fun_id;
        //save thread
        m_thread = thread;
    }
    
    void l_call_context::init(unsigned int fun_id,l_thread *thread,const l_variable& next)
    {
        //save pointer
        m_fun_id = fun_id;
        //save thread
        m_thread = thread;
        //save next
        m_next = next;
    }
    
    //unmark event
    void l_call_context::mark()
    {
        //..
        if(is_marked()) return;
        //mark
        l_obj::mark();
        //unmark
        m_local_variables.mark();
        //mark next
        if(next()) next()->mark();
    }
    
    //unmark event
    void l_call_context::unmark()
    {
        //..
        if(is_unmarked()) return;
        //unmark
        l_obj::unmark();
        //unmark
        m_local_variables.unmark();
        //unmark next
        if(next()) next()->unmark();
    }
    
    l_variable l_call_context::gc_new(l_gc* gc)
    {
        return  gc->new_obj< l_call_context >();
    }
    
    l_variable l_call_context::gc_new(l_gc& gc)
    {
        return  gc.new_obj< l_call_context >();
    }
    
    l_variable l_call_context::gc_new(l_vm* vm)
    {
        return  gc_new(vm->get_gc());
    }
    
    l_variable l_call_context::gc_new(l_vm& vm)
    {
        return  gc_new(vm.get_gc());
    }
    
    l_variable l_call_context::const_new(l_gc* gc)
    {
        l_call_context*  l_call_ctx  = new l_call_context();
        l_call_ctx->set_gc(gc);
        
        l_variable variable = (l_obj*)l_call_ctx;
        variable.m_const = true;
        
        return  variable;
    }
    
    l_variable l_call_context::const_new(l_gc& gc)
    {
        l_call_context*  l_call_ctx  = new l_call_context();
        l_call_ctx->set_gc(&gc);
        
        l_variable variable = (l_obj*)l_call_ctx;
        variable.m_const = true;
        
        return  variable;
    }
    
    l_variable l_call_context::const_new(l_vm* vm)
    {
        l_call_context*  l_call_ctx  = new l_call_context();
        l_call_ctx->set_gc(&vm->get_gc());
        
        l_variable variable = (l_obj*)l_call_ctx;
        variable.m_const = true;
        
        return  variable;
    }
    
    l_variable l_call_context::const_new(l_vm& vm)
    {
        l_call_context*  l_call_ctx  = new l_call_context();
        l_call_ctx->set_gc(&vm.get_gc());
        
        l_variable variable = (l_obj*)l_call_ctx;
        variable.m_const = true;
        
        return  variable;
    }}