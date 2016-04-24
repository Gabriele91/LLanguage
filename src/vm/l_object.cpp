//
//  l_object.cpp
//  LLanguage
//
//  Created by Gabriele on 19/04/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#include <l_vm.h>
#include <l_object.h>
#include <l_class.h>

namespace l_language
{
    //mark event
    void l_object::mark()
    {
        //..
        if(is_marked()) return;
        //mark
        l_ref::mark();
        //mark class
        m_class.mark();
        //mark
        m_raw_pointer.mark();
        //mark fields
        for(auto it:m_attributes)
        {
            l_variable& var = it.second;
            
            if(var.is_ref_obj())
                if(var.is_unmarked())
                {
                    var.mark();
                }
            
            //key is static
            const l_variable& key = it.first;
            //is a object?
            if(key.is_ref_obj())
                if(key.is_unmarked())
                {
                    //object
                    l_ref* key_object = key.m_value.m_pobj;
                    //mark
                    key_object->mark();
                }
        }
        
    }
    //mark event
    void l_object::unmark()
    {
        //..
        if(is_unmarked()) return;
        //mark
        l_ref::unmark();
        //unmark class
        m_class.unmark();
        //unmark
        m_raw_pointer.unmark();
        //unmark fields
        for(auto it:m_attributes)
        {
            l_variable& var = it.second;
            //...
            if(var.is_ref_obj())
                if(var.is_marked())
                {
                    var.unmark();
                }
            //key is static
            const l_variable& key = it.first;
            //is a object?
            if(key.is_ref_obj())
                if(key.is_marked())
                {
                    //object
                    l_ref* key_object = key.m_value.m_pobj;
                    //mark
                    key_object->unmark();
                }
        }
    }
    
    
    l_variable l_object::get_value(const l_variable&  key)
    {
        //first
        l_map_object_it val_it = m_attributes.find(key);
        //find?
        if(val_it != m_attributes.end())
        {
            //return
            return val_it->second;
        }
        //from class
        return m_class.clazz()->get_def(key);
    }
    
    bool l_object::set_value(const l_variable&  key,const l_variable& value)
    {
        //first
        l_map_object_it val_it = m_attributes.find(key);
        //find?
        if(val_it != m_attributes.end())
        {
            //set
            val_it->second=value;
            //return
            return true;
        }
        return false;
    }
}