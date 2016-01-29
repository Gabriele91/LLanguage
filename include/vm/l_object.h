//
//  l_object.h
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
        
        virtual bool is_marked() const
        {
            return m_mark;
        }
        
        virtual void mark()
        {
            m_mark = true;
        }
        
        virtual void unmark()
        {
            m_mark = false;
        }
        
        virtual bool is_unmarked() const
        {
            return !is_marked();
        }
        
        l_gc* get_gc()
        {
            return m_gc;
        }
        
        const l_gc* get_gc() const
        {
            return m_gc;
        }
        
    protected:
        
        virtual ~l_obj()
        {
        }
        
        void set_gc(l_gc* gc)
        {
            m_gc = gc;
        }
        
    private:
        
        //gc reference
        l_gc* m_gc { nullptr };
        
        //mark flag
        bool m_mark { false };
        
        //firend class
        friend class   l_gc;
        friend class   l_variable;
        
    };
};
