//
//  l_compiler_tree_to_vm.h
//  LLanguage
//
//  Created by Gabriele on 19/01/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#pragma once
#include <string>
#include <map>
#include <l_syntactic_tree.h>
#include <l_variables_context.h>
#if 1
namespace l_language
{
    class l_compiler_tree_to_vm : public l_variables_context
    {
        /*
        //fun
         {
          top  0
          args 0
          local 2 { a, b }
          const 2 { 1, 5 }
         }
         code:
         
         a[1] = 5
         b    = a[1]
         
         opcode:
          set_array_table 0 (-1) (-2);
          get_array_table 1 0 (-1);
         
         */
        
        bool compile_variable_set(l_function* fun,l_syntactic_tree::variable_node* node);
        bool compile_variable_get(l_function* fun,l_syntactic_tree::variable_node* node);
        bool compile_assignable_exp(l_function* fun,l_syntactic_tree::exp_node* node,int is_not_first = true);
        bool compile_assignable_exp_get(l_function* fun,l_syntactic_tree::exp_node* node);
        bool compile_assignable_exp_set(l_function* fun,l_syntactic_tree::exp_node* node);
        bool compile_exp_aux(l_function* fun,l_syntactic_tree::exp_node* node);
        
        //exp
        bool compile_exp(l_function* fun,l_syntactic_tree::exp_node* node);
        
        //compile op
        bool compile_op(l_function* fun,l_syntactic_tree::op_node* node);
        bool compile_while(l_function* fun,l_syntactic_tree::while_node* node);
        bool compile_if_else(l_function* fun,l_syntactic_tree::if_node* ifs_node);
        bool compile_for(l_function* fun,l_syntactic_tree::for_node* for_node);
        //compile call
        bool compile_call(l_function* fun,l_syntactic_tree::call_node* call_node);
        //compile fun
        bool compile_function_def(l_function* fun,
                                  l_syntactic_tree::function_def_node* function_def_node,
                                  bool is_exp = false);
        //compile class
        bool compile_class_def(l_function* fun,
                               l_syntactic_tree::class_node* class_node,
                               bool is_exp = false);
        //compile return
        bool compile_return(l_function* fun,l_syntactic_tree::return_node* return_node);
        //compile context type
        bool compile_context_type(l_function* fun,l_syntactic_tree::context_type_node* context_type_node);
        //compile a statements
        bool compile_statements(l_function* fun,l_syntactic_tree::list_nodes& nodes);

        //reference vm
        l_vm*     m_vm { nullptr };
        l_thread* m_thread { nullptr };
        
    public:
        void set_thread(l_thread* thread);
        
        void add_c_function(l_thread& thread,const l_cfunction function,const std::string& cfun_name);

        void add_global_variable(l_thread& thread,  const l_variable& g_variable, const std::string& var_name);
        
        l_thread* compile(const l_syntactic_tree* tree);
        l_function_id compile_function(const l_syntactic_tree* tree);
    };
}
#endif