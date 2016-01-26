//
//  object.h
//  LLanguage
//
//  Created by Gabriele on 16/01/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#pragma once

namespace l_language
{
    class l_ref;
    class l_variable;
    class l_gc;
    class l_obj;
    
    class l_obj
    {
        
    public:
        
        bool is_marked() const;
        
        size_t ref_count() const;
        
    protected:
        
        virtual ~l_obj();
        
        void mark() const;
        
        void unmark() const;
        
        void inc_ref_count() const;
        
        void dec_ref_count() const;
        
        virtual void mark_childs() const = 0;
        
        virtual void unmark_childs() const = 0;
        
        virtual bool has_childs() const = 0;
        
        mutable l_gc*  m_gc          { nullptr };
        mutable bool   m_mark        { false   };
        mutable bool   m_self_delete { true    };
        mutable size_t m_count       { 0	   };
        
        friend class   l_gc;
        friend class   l_variable;
        
    };
};
