//
//  l_call_context.cpp
//  LLanguage
//
//  Created by Gabriele on 07/02/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#include <l_ref.h>
#include <l_call_context.h>
#include <l_vm.h>

namespace l_language
{
    
    
    void l_closer::init(unsigned int fun_id,l_thread *thread)
    {
        //save pointer
        m_fun_id = fun_id;
        //save thread
        m_thread = thread;
    }
    
    void l_closer::init(unsigned int fun_id,l_thread *thread,const l_variable& next)
    {
        //save pointer
        m_fun_id = fun_id;
        //save thread
        m_thread = thread;
        //save next
        m_next = next;
    }
    
    void l_closer::init(unsigned int fun_id, const l_variable& clazz,l_thread *thread)
    {
        //save pointer
        m_fun_id = fun_id;
        m_clazz = clazz;
        //save thread
        m_thread = thread;
    }
    
    void l_closer::init(unsigned int fun_id, const l_variable& clazz,l_thread *thread,const l_variable& next)
    {
        //save pointer
        m_fun_id = fun_id;
        m_clazz = clazz;
        //save thread
        m_thread = thread;
        //save next
        m_next = next;
    }
    //unmark event
    void l_closer::mark()
    {
        //..
        if(is_marked()) return;
        //mark
        l_ref::mark();
        //mark class
        if(m_clazz.is_ref_obj())
            m_clazz.mark();
        //mark next
        if(next()) next()->mark();
    }
    
    //unmark event
    void l_closer::unmark()
    {
        //..
        if(is_unmarked()) return;
        //unmark
        l_ref::unmark();
        //mark class
        if(m_clazz.is_ref_obj())
            m_clazz.unmark();
        //unmark next
        if(next()) next()->unmark();
    }
    
    l_variable l_closer::gc_new(l_gc* gc)
    {
        return  gc->new_obj< l_call_context >();
    }
    
    l_variable l_closer::gc_new(l_gc& gc)
    {
        return  gc.new_obj< l_call_context >();
    }
    
    l_variable l_closer::gc_new(l_vm* vm)
    {
        return  gc_new(vm->get_gc());
    }
    
    l_variable l_closer::gc_new(l_vm& vm)
    {
        return  gc_new(vm.get_gc());
    }
    
    l_variable l_closer::const_new(l_gc* gc)
    {
        l_closer*  l_this_closer  = new l_closer();
        l_this_closer->set_gc(gc);
        
        l_variable variable = (l_ref*)l_this_closer;
        variable.m_const = true;
        
        return  variable;
    }
    
    l_variable l_closer::const_new(l_gc& gc)
    {
        l_closer*  l_this_closer  = new l_closer();
        l_this_closer->set_gc(&gc);
        
        l_variable variable = (l_ref*)l_this_closer;
        variable.m_const = true;
        
        return  variable;
    }
    
    l_variable l_closer::const_new(l_vm* vm)
    {
        l_closer*  l_this_closer  = new l_closer();
        l_this_closer->set_gc(&vm->get_gc());
        
        l_variable variable = (l_ref*)l_this_closer;
        variable.m_const = true;
        
        return  variable;
    }
    
    l_variable l_closer::const_new(l_vm& vm)
    {
        l_closer*  l_this_closer  = new l_closer();
        l_this_closer->set_gc(&vm.get_gc());
        
        l_variable variable = (l_ref*)l_this_closer;
        variable.m_const = true;
        
        return  variable;
    }
    
    void l_call_context::init(const l_closer& closer)
    {
        //save pointer
        m_fun_id = closer.get_fun_id();
        m_clazz = closer.get_class();
        //save thread
        m_thread = closer.thread();
        //save next
        m_next = closer.m_next;
    }
    
    //unmark event
    void l_call_context::mark()
    {
        //mark in any cases
        if(l_ref::is_marked()) return;
        //mark
        l_ref::mark();
        //mark class
        if(m_clazz.is_ref_obj())
            m_clazz.mark();
        //unmark
        m_local_variables.mark();
        //mark this
        if(m_this.is_ref_obj())
        {
            m_this.mark();
        }
        //mark next
        if(next()) next()->mark();
    }
    
    //unmark event
    void l_call_context::unmark()
    {
        //..
        if(is_unmarked()) return;
        //unmark
        l_ref::unmark();
        //unmark class
        if(m_clazz.is_ref_obj())
            m_clazz.unmark();
        //unmark
        m_local_variables.unmark();
        //unmark this
        if(m_this.is_ref_obj())
        {
            m_this.unmark();
        }
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
        
        l_variable variable = (l_ref*)l_call_ctx;
        variable.m_const = true;
        
        return  variable;
    }
    
    l_variable l_call_context::const_new(l_gc& gc)
    {
        l_call_context*  l_call_ctx  = new l_call_context();
        l_call_ctx->set_gc(&gc);
        
        l_variable variable = (l_ref*)l_call_ctx;
        variable.m_const = true;
        
        return  variable;
    }
    
    l_variable l_call_context::const_new(l_vm* vm)
    {
        l_call_context*  l_call_ctx  = new l_call_context();
        l_call_ctx->set_gc(&vm->get_gc());
        
        l_variable variable = (l_ref*)l_call_ctx;
        variable.m_const = true;
        
        return  variable;
    }
    
    l_variable l_call_context::const_new(l_vm& vm)
    {
        l_call_context*  l_call_ctx  = new l_call_context();
        l_call_ctx->set_gc(&vm.get_gc());
        
        l_variable variable = (l_ref*)l_call_ctx;
        variable.m_const = true;
        
        return  variable;
    }
}
