//
//  l_base_lib_xrange.h
//  LLanguage
//
//  Created by Gabriele on 04/04/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#pragma once
#include <l_base_lib.h>
#include <l_object.h>
#include <l_iterator.h>
#include <cmath>
namespace l_language
{
    class l_xrange;
    class l_xrange_it;
    
    //iterators
    class l_xrange_it : public l_iterator
    {
        //attributes
        int m_current_index { 0 };
        int m_end_index     { 0 };
        int m_step_index    { 0 };
        //friend class
        friend class l_xrange;
        friend class l_gc;
        //mark event
        virtual void mark()
        {
            //..
            if(is_marked()) return;
            //mark
            l_obj::mark();
        }
        
        //unmark event
        virtual void unmark()
        {
            //..
            if(is_unmarked()) return;
            //mark
            l_obj::unmark();
        }
        
        static l_variable gc_new(l_gc* gc,int init,int end,int step = 1)
        {
            return  gc->new_obj< l_xrange_it >(init,end,step);
        }
        
        static l_variable gc_new(l_gc& gc,int init,int end,int step = 1)
        {
            return  gc.new_obj< l_xrange_it >(init,end,step);
        }
        
        static l_variable gc_new(l_vm* vm,int init,int end,int step = 1)
        {
            return  vm->get_gc().new_obj< l_xrange_it >(init,end,step);
        }
        
        static l_variable gc_new(l_vm& vm,int init,int end,int step = 1)
        {
            return  vm.get_gc().new_obj< l_xrange_it >(init,end,step);
        }
        
        l_xrange_it(int init,int end,int step)
        {
            m_current_index = init;
            m_end_index     = end;
            m_step_index    = step;
        }
        
    public:
        
        virtual l_variable get() const
        {
            return m_current_index;
        }
        
        virtual l_variable get_id() const
        {
            return m_current_index;
        }
        
        virtual bool valid() const
        {
            return m_current_index < m_end_index;
        }
        
        l_variable next() const
        {
            //next
            return l_xrange_it::gc_new(get_gc(),m_current_index+1,m_end_index,m_step_index);
        }
        
        virtual void self_next()
        {
            m_current_index += m_step_index;
        }
        
    };
    
    //it generator
    class l_xrange : public l_obj
    {
        int m_init_index   { 0 };
        int m_end_index    { 0 };
        int m_step_index   { 1 };
        
        //mark event
        virtual void mark()
        {
            //..
            if(is_marked()) return;
            //mark
            l_obj::mark();
        }
        
        //unmark event
        virtual void unmark()
        {
            //..
            if(is_unmarked()) return;
            //mark
            l_obj::unmark();
        }
        
    public:
        
        l_xrange(l_variable init,l_variable end,l_variable step)
        {
            m_init_index   = init.to_int();
            m_end_index    = end.to_int();
            m_step_index   = step.to_int();
        }
        
        l_xrange(int init,int end,int step)
        {
            m_init_index   = init;
            m_end_index    = end;
            m_step_index   = step;
        }
        
        static l_variable gc_new(l_gc* gc,l_variable init,l_variable end,l_variable step = 1)
        {
            return  gc->new_obj< l_xrange >(init,end,step);
        }
        
        static l_variable gc_new(l_gc* gc,int init,int end,int step = 1)
        {
            return  gc->new_obj< l_xrange >(init,end,step);
        }
        
        static l_variable gc_new(l_gc& gc,l_variable init,l_variable end,l_variable step = 1)
        {
            return  gc.new_obj< l_xrange >(init,end,step);
        }
        
        static l_variable gc_new(l_gc& gc,int init,int end,int step = 1)
        {
            return  gc.new_obj< l_xrange >(init,end,step);
        }
        
        static l_variable gc_new(l_vm* vm,l_variable init,l_variable end,l_variable step = 1)
        {
            return  gc_new(vm->get_gc(),init,end,step);
        }
        
        static l_variable gc_new(l_vm* vm,int init,int end,int step = 1)
        {
            return  gc_new(vm->get_gc(),init,end,step);
        }
        
        static l_variable gc_new(l_vm& vm,l_variable init,l_variable end,l_variable step = 1)
        {
            return  gc_new(vm.get_gc(),init,end,step);
        }
        
        static l_variable gc_new(l_vm& vm,int init,int end,int step = 1)
        {
            return  gc_new(vm.get_gc(),init,end,step);
        }
        
        l_variable get_it()
        {
            //next
            return l_xrange_it::gc_new(get_gc(),m_init_index,m_end_index,m_step_index);
        }
    };
}