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
        inline T l_arg(l_language::l_thread* thread,int n, bool& success)
        {
            return (T)(0);
        }
        
        template < >
        inline l_variable l_arg(l_language::l_thread* thread,int n, bool& success)
        {
            return thread->value(n);
        }
        
        template < >
        inline int l_arg< int >(l_language::l_thread* thread,int n, bool& success)
        {
            if(!thread->value(n).can_to_int())
            {
                thread->push_error( l_exception_to_int(thread,"can't cast value to int") );
                success = false;
            }
            return thread->value(n).to_int();
        }
        
        template < >
        inline size_t l_arg< size_t >(l_language::l_thread* thread,int n, bool& success)
        {
            if(!thread->value(n).can_to_int())
            {
                thread->push_error( l_exception_to_int(thread,"can't cast value to int") );
                success = false;
            }
            return (size_t)thread->value(n).to_int();
        }
        
        template < >
        inline float l_arg< float >(l_language::l_thread* thread,int n, bool& success)
        {
            if(!thread->value(n).can_to_float())
            {
                thread->push_error( l_exception_to_float(thread,"can't cast value to float") );
                success = false;
            }
            return thread->value(n).to_float();
        }
        
        template < >
        inline double l_arg< double >(l_language::l_thread* thread,int n, bool& success)
        {
            if(!thread->value(n).can_to_float())
            {
                thread->push_error( l_exception_to_float(thread,"can't cast value to float") );
                success = false;
            }
            return (double)thread->value(n).to_float();
        }
        
        template < >
        inline std::string l_arg< std::string >(l_language::l_thread* thread,int n, bool& success)
        {
            if(!thread->value(n).is_string())
            {
                thread->push_error( l_exception_to_string(thread,"can't cast value to string") );
                success = false;
            }
            return thread->value(n).to_string();
        }
        
        template < >
        inline  const std::string& l_arg< const std::string& >(l_language::l_thread* thread,int n, bool& success)
        {
            if(!thread->value(n).is_string())
            {
                thread->push_error( l_exception_to_string(thread,"can't cast value to string") );
                success = false;
            }
            return thread->value(n).string()->str();
        }
        
        template < >
        inline  const char* l_arg< const char* >(l_language::l_thread* thread,int n, bool& success)
        {
            if(!thread->value(n).is_string())
            {
                thread->push_error( l_exception_to_string(thread,"can't cast value to string") );
                success = false;
            }
            return thread->value(n).string()->str().c_str();
        }
        
        template < typename T >
        inline void l_return(l_language::l_thread* thread,const T& value)
        {
            thread->push_return( l_variable(value) );
        }
        
        template < >
        inline void l_return(l_language::l_thread* thread,const bool& value)
        {
            thread->push_return( l_variable(value) );
        }
        
        template < >
        inline void l_return(l_language::l_thread* thread,const double& value)
        {
            thread->push_return( l_variable((float)value) );
        }
        
        template < >
        inline void l_return(l_language::l_thread* thread,const long& value)
        {
            thread->push_return( l_variable((int)value) );
        }
        
        template < >
        inline void l_return(l_language::l_thread* thread,const size_t& value)
        {
            thread->push_return( l_variable((int)value) );
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
                bool success = true;
                FUN(l_arg<ARGS>(thread, INDICES, success)...);
                return success ? 0 : -1;
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
                bool success = true;
                l_return(thread, FUN(l_arg<ARGS>(thread, INDICES,success)...));
                return success ? 1 : -1;
            }
        };
        
    };
    #define l_def_t(type,x) ::l_language::l_wrapper::l_def< type, (type)(x) >::call
    #define l_def(x)        ::l_language::l_wrapper::l_def< decltype(x), x >::call
};