//
//  l_class_wrapper.h
//  LLanguage
//
//  Created by Gabriele on 20/04/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#pragma once
#include <l_function_wrapper.h>

namespace l_language
{
    namespace  l_wrapper
    {
        
        template<typename C, typename SIG, SIG > struct l_method;
        
        template<  typename C, class... ARGS, void(C::*METHOD)(ARGS...)>
        struct l_method< C, void(C::*)(ARGS...), METHOD >
        {
            static int call(l_language::l_thread* thread, int args)
            {
                if(args < (sizeof...(ARGS)))
                {
                    throw l_exception_number_of_arguments(thread,"The number of arguments isn't valid");
                }
                if(!thread->get_this().is_object())
                {
                    throw l_exception_not_valid_this(thread,"Method call without \"this\"");
                }
                return do_call(thread,t_make_int_range<0,sizeof...(ARGS)>());
            }
        private:
            template<int... INDICES>
            static int do_call(l_language::l_thread* thread, t_index_int<INDICES...>)
            {
                //success
                bool success = true;
                //get this
                l_language::l_object*  var_self = thread->get_this().to< l_object >();
                //to C type
                C* ptr_self = var_self->get_value( l_string::gc_new(thread->get_gc(), "$native$this") ).to< C >();
                //call
                (ptr_self->*METHOD)(l_arg<ARGS>(thread, INDICES, success)...);
                //return
                return success ? 0 : -1;
            }
        };
        
        template< typename C, typename R, class... ARGS, R(C::*METHOD)(ARGS...) >
        struct l_method< C, R(C::*)(ARGS...), METHOD >
        {
            static int call(l_language::l_thread* thread, int args)
            {
                if(args < (sizeof...(ARGS)))
                {
                    throw l_exception_number_of_arguments(thread,"The number of arguments isn't valid");
                }
                if(!thread->get_this().is_object())
                {
                    throw l_exception_not_valid_this(thread,"Method call without \"this\"");
                }
                return do_call(thread,t_make_int_range<0,sizeof...(ARGS)>());
            }
        private:
            template<int... INDICES>
            static int do_call(l_language::l_thread* thread, t_index_int<INDICES...>)
            {
                //success
                bool success = true;
                //get this
                l_language::l_object*  var_self = thread->get_this().to< l_object >();
                //to C type
                C* ptr_self = var_self->get_value( l_string::gc_new(thread->get_gc(), "$native$this") ).to< C >();
                //call
                l_return(thread,(ptr_self->*METHOD)(l_arg<ARGS>(thread, INDICES, success)...));
                //return
                return success ? 1 : -1;
            }
        };
        
        template< typename C, typename... ARGS >
        struct l_constructor
        {
            static int call(l_language::l_thread* thread, int args)
            {
                if(args < (sizeof...(ARGS)))
                {
                    throw l_exception_number_of_arguments(thread,"The number of arguments isn't valid");
                }
                if(!thread->get_this().is_object())
                {
                    throw l_exception_not_valid_this(thread,"Method call without \"this\"");
                }
                return do_call(thread,t_make_int_range<0,sizeof...(ARGS)>());
            }
        private:
            template<int... INDICES>
            static int do_call(l_language::l_thread* thread, t_index_int<INDICES...>)
            {
                //success
                bool success = true;
                //get this
                l_language::l_object*  self = thread->get_this().to< l_object >();
                //get gc
                l_language::l_gc* gc =  thread->get_gc();
                //alloc
                l_language::l_variable raw_self = gc->new_obj< C >(l_arg<ARGS>(thread, INDICES, success)...);
                //add
                self->set_value( l_string::gc_new(thread->get_gc(), "$native$this") , raw_self);
                //return this
                thread->push_return(self);
                //return
                return success ? 1 : -1;
            }
        };
        
        //native method field
        struct l_class_field
        {
            std::string            m_name;
            l_language::l_variable m_field;
        };
        //utility class wrapper
        inline
        l_language::l_variable l_create_class(l_language::l_vm* vm,
                                              const std::string class_name,
                                              const std::vector< l_class_field >& methods)
        {
            using l_class       = l_language::l_class;
            using l_string      = l_language::l_string;
            using l_variable    = l_language::l_variable;
            //add class
            l_variable var_class = l_class::gc_new(vm);
            l_class*   this_class = var_class.to< l_class >();
            //add values
            this_class->add_variable( l_string::gc_new(vm, "$native$this"), l_variable() );
            //set name
            this_class->set_class_name(l_string::gc_new(vm, class_name));
            //append
            for(auto& field : methods)
            {
                this_class->add_def( l_string::gc_new(vm, field.m_name), field.m_field );
            }
            //return class object
            return { this_class };
        }


    }
    #define l_constructor(...) ::l_language::l_wrapper::l_constructor< __VA_ARGS__ >::call
    #define l_method(C,x) ::l_language::l_wrapper::l_method< C, decltype(&C::x), &C::x >::call
}