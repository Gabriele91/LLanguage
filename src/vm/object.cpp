//
//  object.cpp
//  LLanguage
//
//  Created by Gabriele on 16/01/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//

#include <stdio.h>
#include <gc.h>
#include <object.h>

namespace  l_language
{
    
    //object implementation
    bool l_obj::is_marked() const
    {
        return m_mark;
    }

    size_t l_obj::ref_count() const
    {
        return m_count;
    }

    l_obj::~l_obj()
    {
        m_gc->remove(this);
    }

    void l_obj::mark() const
    {
        if (is_marked()) return;
        m_mark = true;
        if (has_childs()) mark_childs();
    }

    void l_obj::unmark() const
    {
        if (!is_marked()) return;
        m_mark = false;
        if (has_childs()) unmark_childs();
    }

    void l_obj::inc_ref_count() const
    {
        //inc count of allocations
        ++m_count;
    }

    void l_obj::dec_ref_count() const
    {
        //dec counter
        --m_count;
        //auto dealloc
        if (!m_count && m_self_delete) delete this;
    }
}