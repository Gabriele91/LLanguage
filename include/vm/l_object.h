//
//  l_object.h
//  LLanguage
//
//  Created by Gabriele on 19/04/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#pragma once

#pragma once
#include <map>
#include <memory>
#include <l_ref.h>
#include <l_variable.h>

namespace l_language
{
    //table
    class l_table;
    //table iterator
    class l_table_it;
    class l_call_context;
    class l_vm;
    class l_thread;
    class l_class;
    class l_object;
    
    class l_object : public l_ref
    {
        //firend
        friend class l_class;
        //attributes
        l_variable   m_class;
        l_variable   m_raw_pointer;
        l_map_object m_attributes;
        //mark event
        virtual void mark();
        //mark event
        virtual void unmark();
        
    public:
        
        l_object(l_variable class_obj)
        {
            m_class = class_obj;
        }
        
        l_variable get_value(const l_variable&  key);
        bool       set_value(const l_variable&  key,const l_variable& value);
        
        l_variable get_class() const
        {
            return m_class;
        }
        
        void  set_raw_pointer(const l_variable& pointer)
        {
            m_raw_pointer = pointer;
        }
        
        l_variable get_raw_pointer() const
        {
            return m_raw_pointer;
        }
    };
    
};