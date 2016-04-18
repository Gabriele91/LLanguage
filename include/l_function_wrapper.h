//
//  l_function_wrapper.h
//  LLanguage
//
//  Created by Gabriele on 17/04/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#pragma once
#include <l_vm.h>
#include <l_meta_help.h>
#include <l_exception.h>
#include <assert.h>
namespace l_language
{
    namespace  l_wrapper
    {
        ////////////////////////////////////////////////////////////////////////////////////
        template < typename T >
        inline T l_arg(l_language::l_thread* th,int n)
        {
            return (T)(0);
        }
        
        template < >
        inline int l_arg< int >(l_language::l_thread* thread,int n)
        {
            if(!thread->value(n).can_to_int())
            {
                throw l_exception_to_int(thread,"can't cast value to int");
            }
            return thread->value(n).to_int();
        }
        
        template < >
        inline float l_arg< float >(l_language::l_thread* thread,int n)
        {
            if(!thread->value(n).can_to_float())
            {
                throw l_exception_to_float(thread,"can't cast value to float");
            }
            return thread->value(n).to_float();
        }
        
        template < >
        inline std::string l_arg< std::string >(l_language::l_thread* thread,int n)
        {
            if(!thread->value(n).is_string())
            {
                throw l_exception_to_string(thread,"can't cast value to string");
            }
            return thread->value(n).to_string();
        }
        
        template < typename T >
        inline void l_return(l_language::l_thread* thread,const T& value)
        {
            thread->push_return( l_variable(value) );
        }
        
        template < >
        inline void l_return(l_language::l_thread* thread,const std::string& value)
        {
            thread->push_return( l_string::gc_new(thread->get_gc(),value) );
        }
        ///////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////
        
        template<typename SIG, SIG* S> struct l_def;
        
        template< class... ARGS, void (*FUN)(ARGS...)>
        struct l_def< void(ARGS...),FUN>
        {
            static int call(l_language::l_thread* thread, int args)
            {
                if(args < (sizeof...(ARGS)))
                {
                    throw l_exception_number_of_arguments(thread,"The number of arguments isn't valid");
                }
                return do_call(thread,t_make_int_range<0,sizeof...(ARGS)>());
            }
        private:
            template<int... INDICES>
            static int do_call(l_language::l_thread* thread, t_index_int<INDICES...>)
            {
                FUN(l_arg<ARGS>(thread, INDICES)...);
                return 0;
            }
        };

        template< typename R, class... ARGS, R(*FUN)(ARGS...)>
        struct l_def< R(ARGS...),FUN>
        {
            static int call(l_language::l_thread* thread, int args)
            {
                if(args < (sizeof...(ARGS)))
                {
                    throw l_exception_number_of_arguments(thread,"The number of arguments isn't valid");
                }
                return do_call(thread,t_make_int_range<0,sizeof...(ARGS)>());
            }
        private:
            template<int... INDICES>
            static int do_call(l_language::l_thread* thread, t_index_int<INDICES...>)
            {
                l_return(thread, FUN(l_arg<ARGS>(thread, INDICES)...));
                return 1;
            }
        };
        
    };
    #define l_def(x) ::l_language::l_wrapper::l_def< decltype(x), x >::call
};