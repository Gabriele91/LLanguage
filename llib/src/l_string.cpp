//
//  l_string.cpp
//  LLanguage
//
//  Created by Gabriele on 26/01/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//

#include <stdio.h>
#include <l_vm.h>
#include <l_variable.h>
#include <l_string.h>

namespace l_language
{
    l_variable l_string::gc_merge(l_string* string)
    {
        return  get_gc()->new_obj< l_string >(m_cpp_str+string->m_cpp_str);
    }
    
    l_variable l_string::gc_merge(const std::string& str)
    {
        return  get_gc()->new_obj< l_string >(m_cpp_str+str);
    }
    
    l_variable l_string::gc_merge_left(l_string* string)
    {
        return  get_gc()->new_obj< l_string >(string->m_cpp_str+m_cpp_str);
    }
    
    l_variable l_string::gc_merge_left(const std::string& str)
    {
        return  get_gc()->new_obj< l_string >(str+m_cpp_str);
    }
    
    l_variable l_string::gc_new(l_gc* gc,const std::string& string)
    {
        return  gc->new_obj< l_string >(string);
    }
    
    l_variable l_string::gc_new(l_gc& gc,const std::string& string)
    {
        return  gc.new_obj< l_string >(string);
    }
    
    l_variable l_string::gc_new(l_vm* vm,const std::string& string)
    {
        return  vm->get_gc().new_obj< l_string >(string);
    }
    
    l_variable l_string::gc_new(l_vm& vm,const std::string& string)
    {
        return  vm.get_gc().new_obj< l_string >(string);
    }
    
    l_variable l_string::const_new(l_gc* gc,const std::string& string)
    {
        l_string*  var_str  = new l_string(string);
        var_str->set_gc(gc);
        
        l_variable variable = var_str;
        variable.m_const = true;
        
        return  variable;
    }
    
    l_variable l_string::const_new(l_gc& gc,const std::string& string)
    {
        l_string*  var_str  = new l_string(string);
        var_str->set_gc(&gc);
        
        l_variable variable = var_str;
        variable.m_const = true;
        
        return  variable;
    }
    
    l_variable l_string::const_new(l_vm* vm,const std::string& string)
    {
        l_string*  var_str  = new l_string(string);
        var_str->set_gc(&vm->get_gc());
        
        l_variable variable = var_str;
        variable.m_const = true;
        
        return  variable;
    }
    
    l_variable l_string::const_new(l_vm& vm,const std::string& string)
    {
        l_string*  var_str  = new l_string(string);
        var_str->set_gc(&vm.get_gc());
        
        l_variable variable = var_str;
        variable.m_const = true;
        
        return  variable;
    }
}