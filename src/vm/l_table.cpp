//
//  l_table.cpp
//  LLanguage
//
//  Created by Gabriele on 07/02/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//

#include <l_table.h>
#include <l_gc.h>
#include <l_vm.h>

namespace l_language
{
    l_variable l_table::gc_new(l_gc* gc)
    {
        return  gc->new_obj< l_table >();
    }
    
    l_variable l_table::gc_new(l_gc& gc)
    {
        return  gc.new_obj< l_table >();
    }
    
    l_variable l_table::gc_new(l_vm* vm)
    {
        return  gc_new(vm->get_gc());
    }
    
    l_variable l_table::gc_new(l_vm& vm)
    {
        return  gc_new(vm.get_gc());
    }
    
    l_variable l_table::const_new(l_gc* gc)
    {
        l_table*  var_str  = new l_table();
        var_str->set_gc(gc);
        
        l_variable variable = var_str;
        variable.m_const = true;
        
        return  variable;
    }
    
    l_variable l_table::const_new(l_gc& gc)
    {
        l_table*  var_str  = new l_table();
        var_str->set_gc(&gc);
        
        l_variable variable = var_str;
        variable.m_const = true;
        
        return  variable;
    }
    
    l_variable l_table::const_new(l_vm* vm)
    {
        l_table*  var_str  = new l_table();
        var_str->set_gc(&vm->get_gc());
        
        l_variable variable = var_str;
        variable.m_const = true;
        
        return  variable;
    }
    
    l_variable l_table::const_new(l_vm& vm)
    {
        l_table*  var_str  = new l_table();
        var_str->set_gc(&vm.get_gc());
        
        l_variable variable = var_str;
        variable.m_const = true;
        
        return  variable;
    }

    //table it
    l_variable l_table_it::gc_new(l_gc* gc,l_table* vector)
    {
        return  gc->new_obj< l_table_it >(vector);
    }
    
    l_variable l_table_it::gc_new(l_gc& gc,l_table* vector)
    {
        return  gc.new_obj< l_table_it >(vector);
    }
    
    l_variable l_table_it::gc_new(l_vm* vm,l_table* vector)
    {
        return  vm->get_gc().new_obj< l_table_it >(vector);
    }
    
    l_variable l_table_it::gc_new(l_vm& vm,l_table* vector)
    {
        return  vm.get_gc().new_obj< l_table_it >(vector);
    }
#if 0
    l_variable l_table_it::gc_new(l_gc* gc,l_table::l_map_object_it c_it)
    {
        return  gc->new_obj< l_table_it >(c_it);
    }
    
    l_variable l_table_it::gc_new(l_gc& gc,l_table::l_map_object_it c_it)
    {
        return  gc.new_obj< l_table_it >(c_it);
    }
    
    l_variable l_table_it::gc_new(l_vm* vm,l_table::l_map_object_it c_it)
    {
        return  vm->get_gc().new_obj< l_table_it >(c_it);
    }
    
    l_variable l_table_it::gc_new(l_vm& vm,l_table::l_map_object_it c_it)
    {
        return  vm.get_gc().new_obj< l_table_it >(c_it);
    }
#else
    
    l_variable l_table_it::gc_new(l_gc* gc,l_table::l_table_raw_it c_it)
    {
        return  gc->new_obj< l_table_it >(c_it);
    }
    
    l_variable l_table_it::gc_new(l_gc& gc,l_table::l_table_raw_it c_it)
    {
        return  gc.new_obj< l_table_it >(c_it);
    }
    
    l_variable l_table_it::gc_new(l_vm* vm,l_table::l_table_raw_it c_it)
    {
        return  vm->get_gc().new_obj< l_table_it >(c_it);
    }
    
    l_variable l_table_it::gc_new(l_vm& vm,l_table::l_table_raw_it c_it)
    {
        return  vm.get_gc().new_obj< l_table_it >(c_it);
    }
#endif
}