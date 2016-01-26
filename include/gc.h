//
//  gc.h
//  LLanguage
//
//  Created by Gabriele on 16/01/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#pragma once
#include <list>
#include <object.h>

namespace l_language
{
    class l_ref;
    class l_variable;
    class l_gc;
    class l_obj;
        
    class l_ref
    {
    public:
        
        //reference variable
        l_ref(l_variable& variable,bool is_in_stack);
        virtual ~l_ref();
        
        l_variable& variable() const
        {
            return m_variable;
        }
        
    private:
        
        l_variable&  m_variable;
        bool         m_is_in_stack{ false };
        
    };
    
    class l_gc
    {
        //type object ref
        struct l_ref_obj
        {
            l_obj* m_obj{ nullptr };
            size_t  m_size{ 0 };
            
            l_ref_obj(l_obj* obj, size_t  size)
            {
                m_obj  = obj;
                m_size = size;
            }
        };
        //type of pool
        using l_pool = std::list< l_ref_obj >;
        using l_refs = std::list< l_ref* >;
        
    public:
        
        static         l_gc s_global_gc;
        static const size_t kbyte = 1024;
        static const size_t mbyte = 1024*1024;
        
        template < typename T, typename... A >
        l_obj* new_obj(A... args)
        {
            //update size
            if (m_max_size >= m_size_allocs) delete_garbage();
            //alloc
            l_obj* optr =(l_obj*)new T(args...);
            //put gc ref
            optr->m_gc = this;
            //push obj to pool
            push(optr,sizeof(T));
            //return ptr
            return optr;
        }
        
        void set_max_alloc_size(size_t max_alloc);
        
        //execute garbage collector
        void delete_garbage();
        
        
    protected:
        
        
        //gc actions
        void push(l_ref* ref);
        
        void remove(l_refs::iterator it);
        
        void remove(l_ref* ref);
        
        void push(l_obj* obj,size_t size);
        
        void remove(l_pool::iterator it);
        
        void remove(l_obj* obj);
        
        void free(l_obj* obj);
        
        void free(l_pool::iterator it);
        
        void mark_pool();
        
        void unmark_pool();
        
        //pool of the objects
        l_pool m_pool;
        //list of the refs
        l_refs m_refs;
        
        //allocations size
        size_t m_max_size   { kbyte*16 };
        size_t m_size_allocs{ 0        };
        
        //Object self delete
        friend class l_obj;
        friend class l_ref;
    };


};
