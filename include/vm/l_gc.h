//
//  l_gc.h
//  LLanguage
//
//  Created by Gabriele on 16/01/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#pragma once
#include <list>
#include <l_object.h>

namespace l_language
{
    class l_vm;
    class l_thread;
    class l_variable;
    class l_gc;
    class l_obj;
    
#ifdef DEBUG
    #define GC_DEBUG(x) x
#else
    #define GC_DEBUG(x)
#endif
    class l_gc
    {
        //type object ref
        struct l_ref_obj
        {
            l_obj* m_obj{ nullptr };
            size_t  m_size{ 0 };
            
            GC_DEBUG( size_t m_id { 0 }; )
            
            l_ref_obj(l_obj* obj, size_t  size )
            {
                m_obj  = obj;
                m_size = size;
                
                GC_DEBUG(
                static size_t gen_id{ 0 };
                m_id = gen_id++;
                )
            }
            
        };
        //type of pool
        using l_pool = std::list< l_ref_obj >;
        
    public:
        
        static const size_t kbyte = 1024;
        static const size_t mbyte = 1024*1024;
        
        template < typename T, typename... A >
        T* new_obj(A... args)
        {
            //update size
            if (m_max_size < m_size_allocs)
            {
                delete_garbage();
            }
            //alloc
            l_obj* optr =(l_obj*)new T(args...);
            //put gc ref
            optr->m_gc = this;
            //push obj to pool
            push(optr,sizeof(T));
            //return ptr
            return (T*)optr;
        }
        
        //auto execute
        void set_max_alloc_size(size_t max_alloc);
        
        //execute garbage collector
        void delete_garbage();
        
        //init gc
        l_gc(l_vm& vm):m_vm(vm){}
        
        //get vm
        l_vm& get_vm()
        {
            return m_vm;
        }
        const l_vm& get_vm() const
        {
            return m_vm;
        }
        
        virtual ~l_gc();
        
    protected:
        
        void push(l_obj* object,size_t size);
        
        void  remove(l_pool::iterator it);
        
        void  remove(l_obj* obj);
        
        void  free(l_obj* obj);
        
        void  free(l_pool::iterator it);
        
        void mark_pool();
        
        void unmark_pool();
        
        //pool of the objects
        l_pool m_pool;
        
        //vm ptr
        l_vm& m_vm;
        
        //allocations size
        size_t m_max_size   { kbyte };
        size_t m_size_allocs{ 0     };
        
        //Object self delete
        friend class l_obj;
        friend class l_ref;
    };


};
