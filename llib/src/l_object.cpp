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
        if(m_raw_pointer.is_ref_obj() && m_raw_pointer.is_unmarked())
            m_raw_pointer.mark();
        //mark fields
        for(auto& it : m_attributes)
        {
            l_variable& var = it.second.m_variable;
            // Mark attrbute
            if(var.is_ref_obj() && var.is_unmarked())
            {
                var.mark();
            } 
            //key is static
            const l_variable& key = it.first;
            //is a object?
            if(key.is_ref_obj() && key.is_unmarked())
            {
                //object
                l_variable* key_object = (l_variable*)&key;
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
        if(m_raw_pointer.is_ref_obj() && m_raw_pointer.is_marked())
            m_raw_pointer.unmark();
        //unmark fields
        for(auto it:m_attributes)
        {
            l_variable& var = it.second.m_variable;
            //...
            if(var.is_ref_obj() && var.is_marked())
            {
                var.unmark();
            }
            //key is static
            const l_variable& key = it.first;
            //is a object?
            if(key.is_ref_obj() && key.is_marked())
            {
                //object
                l_variable* key_object = (l_variable*)&key;
                //mark
                key_object->unmark();
            }
        }
    }
    
    l_variable l_object::get_value(const l_variable&  key)
    {
        //first
        l_map_attribute_it val_it = m_attributes.find(key);
        //find?
        if(val_it != m_attributes.end())
        {
            //return
            return val_it->second.m_variable;
        }
        //from class
        return m_class.clazz()->get_def(key);
    }

    bool l_object::set_value(const l_variable&  key,const l_variable& value)
    {
        //first
		l_map_attribute_it val_it = m_attributes.find(key);
        //find?
        if(val_it != m_attributes.end())
        {
            //set
            val_it->second.m_variable=value;
            //return
            return true;
        }
        return false;
    }

}