//
//  l_class.cpp
//  LLanguage
//
//  Created by Gabriele on 19/04/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//

#include <stdio.h>
#include <l_vm.h>
#include <l_class.h>


namespace l_language
{
    
    //op compare
    l_class::l_type_operator l_class::get_op_id(const std::string& name)
    {
        if(name=="+") return l_class::OP_ADD;
        if(name=="-") return l_class::OP_SUB;
        if(name=="*") return l_class::OP_MUL;
        if(name=="/") return l_class::OP_DIV;
        if(name=="%") return l_class::OP_MOD;
        
        if(name=="one:-")   return l_class::OP_UNM;
        
        if(name=="right:+") return l_class::OP_RIGHT_ADD;
        if(name=="right:-") return l_class::OP_RIGHT_SUB;
        if(name=="right:*") return l_class::OP_RIGHT_MUL;
        if(name=="right:/") return l_class::OP_RIGHT_DIV;
        if(name=="right:%") return l_class::OP_RIGHT_MOD;
        
        if(name=="iterator")return l_class::OP_GET_IT;
        if(name=="next")    return l_class::OP_GET_NEXT;
        if(name=="value")   return l_class::OP_GET_VALUE;
        if(name=="key")     return l_class::OP_GET_KEY;
        if(name=="valid")   return l_class::OP_GET_VALID;
        
        return l_class::OP_MAX;
    }
    //mark event
    void l_class::mark()
    {
        //..
        if(is_marked()) return;
        //mark
        m_class_name.mark();
        //mark parents
        m_parents.mark();
        //mark ops
        m_operators.mark();
        //mark
        l_ref::mark();
        //mark childs
        for(auto& map:m_maps)
        {
            for(auto it:map)
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
    }
    
    //unmark event
    void l_class::unmark()
    {
        //..
        if(is_unmarked()) return;
        //unmark
        m_class_name.unmark();
        //unmark parents
        m_parents.unmark();
        //mark ops
        m_operators.unmark();
        //unmark
        l_ref::unmark();
        //mark childs
        for(auto& map:m_maps)
        {
            for(auto it:map)
            {
                l_variable& var = it.second.m_variable;
                // Unmark attrbute
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
    }
    
    //exists def?
    bool l_class::exists_def(const l_variable& key, l_map_attribute_const_it& it) const
    {
        return (it=m_maps[M_DEFS].find(key))!=m_maps[M_DEFS].end();
    }
    
    void l_class::set_class_name(const l_variable& c_name)
    {
        m_class_name = c_name;
    }
    
    l_variable l_class::get_class_name() const
    {
        return m_class_name ;
    }
    
    void l_class::set_class_function(l_function* function)
    {
        m_function = function;
    }
    
    l_function* l_class::get_class_function() const
    {
        return m_function;
    }
    
    l_variable l_class::get_constructor()
    {
        return get_def(m_class_name);
    }
        
    void l_class::add_variable (const l_variable& key,const l_variable&  value, l_attribute_access access)
    {
        m_maps[M_ATTRS][key] = l_attribute(access, this, value);
    }
    
    void l_class::add_def      (const l_variable& key,const l_variable&  value, l_attribute_access access)
    {
        m_maps[M_DEFS][key] = l_attribute(access, this, value);
    }
    
    void l_class::add_operator (l_type_operator type,const l_variable&  value)
    {
        m_operators[type] = value;
    }
    
    void l_class::add_parent(const l_variable& value)
    {
        //append
        m_parents.push(value);
        //get class
        const l_class* p_class = value.clazz();
        //get attributes
        const l_map_attribute& p_attrs = p_class->m_maps[M_ATTRS];
        //add attributes
        for (auto it : p_attrs)
        {
            m_maps[M_ATTRS][it.first] = it.second;
        }		
    }
    
    l_variable l_class::get_value(const l_variable&  key)
    {
        for(l_map_attribute& map : m_maps)
        {
            //first
			l_map_attribute_it val_it = map.find(key);
            //find?
            if(val_it != map.end())
            {
                //return
                return val_it->second.m_variable;
            }
        }
        
        return l_variable();
    }
    
    l_variable l_class::get_def(const l_variable&  key)
    {
        //first
		l_map_attribute_it val_it = m_maps[M_DEFS].find(key);
        //find?
        if(val_it != m_maps[M_DEFS].end()) return val_it->second.m_variable;
        //search from parens
        for (auto 
			 rit = m_parents.m_pool.rbegin();
             rit != m_parents.m_pool.rend();
             ++rit)
        {
            //..
            l_variable& it_class = *rit;
            //pointer
            const l_variable& def = it_class.clazz()->get_def(key);
            //find?
            if (!def.is_null()) return def;
        }
        
        return l_variable();
    }

	l_variable l_class::get_operator(l_type_operator  type)
	{        
		//get operator
		const l_variable& a_operator = m_operators[type];
		//return
		if (!a_operator.is_null()) return a_operator;
		//search from parens
		for (auto
			rit = m_parents.m_pool.rbegin();
			rit != m_parents.m_pool.rend();
			++rit)
		{
			//..
			l_variable& it_class = *rit;
			//pointer
			const l_variable& a_operator = it_class.clazz()->get_operator(type);
			//find?
			if (!a_operator.is_null()) return a_operator;
		}
		return l_variable();
	}

	bool l_class::can_access_from_context(const l_variable&  key, const l_call_context& context, l_attribute& attribute)
	{
		//variables
		bool success = false;
		//get attribute
		attribute = get_attribute(key);
		//test exits
		if (!attribute.is_valid()) return false;
		//type of access
		switch (attribute.m_access)
		{
			default:
			case l_attribute_access::PUBLIC: success = true;
			break;
			case l_attribute_access::PROTECTED:
			{
				l_variable closer_class = context.get_class();
				if (closer_class.is_class())
				{
					success = 
						closer_class.clazz() == attribute.m_class
					 || closer_class.clazz()->is_derivate_of(attribute.m_class);
				}
			}
			break;
			case l_attribute_access::PRIVATE:
			{
				l_variable closer_class = context.get_class();
				if (closer_class.is_class())
				{
					success = closer_class.clazz() == attribute.m_class;
				}
			}
			break;
		}
		return success;
	}
	
	l_attribute l_class::get_attribute(const l_variable&  key)
	{
		for (l_map_attribute& map : m_maps)
		{
			//first
			l_map_attribute_it val_it = map.find(key);
			//find?
			if (val_it != map.end())
			{
				return  val_it->second;
			}
		}
		//parent
		for (auto 
			 rit = m_parents.m_pool.rbegin();
			 rit != m_parents.m_pool.rend();
			 ++rit)
		{
			//..
			l_variable& it_class = *rit;
			//get attribute
			l_attribute attribute = it_class.clazz()->get_attribute(key);
			//find?
			if (attribute.is_valid()) return attribute;
		}
		return l_attribute();
	}

    bool l_class::is_derivate_of(const l_class* clazz) const
    {
        for(size_t p = 0; p!=m_parents.size(); ++p)
        {
            if(m_parents[p].clazz() == clazz) return true;
        }
        for(size_t p = 0; p!=m_parents.size(); ++p)
        {
            if(m_parents[p].clazz()->is_derivate_of(clazz)) return true;
        }
        return false;
    }
    
    
    l_variable l_class::new_object(l_thread* th)
    {
        //alloc
        l_variable var = th->get_gc()->new_obj<l_object>( l_variable(this) );
        //..
        l_object* object = var.to<l_object>();
        //copy attributes
        object->m_attributes = m_maps[M_ATTRS];
        //call gc
        return var;
    }
    
    l_variable l_class::gc_new(l_gc* gc)
    {
        return gc->new_obj<l_class>();
    }
    
    l_variable l_class::gc_new(l_gc& gc)
    {
        return gc.new_obj<l_class>();
    }
    
    l_variable l_class::gc_new(l_vm* vm)
    {
        return vm->get_gc().new_obj<l_class>();
    }
    
    l_variable l_class::gc_new(l_vm& vm)
    {
        return vm.get_gc().new_obj<l_class>();
    }

}
