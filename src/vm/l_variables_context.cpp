//
//  l_variables_context.cpp
//  LLanguage
//
//  Created by Gabriele on 22/03/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//

#include <stdio.h>
#include <l_vm.h>
#include <l_variables_context.h>

namespace l_language
{
    
    
    
    void l_variables_context::visit(l_function* fun, l_syntactic_tree::node* node)
    {
        
        switch (node->m_type)
        {
            case l_syntactic_tree::OP_NODE:              visit(fun,node->to<l_syntactic_tree::op_node>());              break;
            case l_syntactic_tree::CALL_NODE:            visit(fun,node->to<l_syntactic_tree::call_node>());            break;
            case l_syntactic_tree::IF_NODE:              visit(fun,node->to<l_syntactic_tree::if_node>());              break;
            case l_syntactic_tree::WHILE_NODE:           visit(fun,node->to<l_syntactic_tree::while_node>());           break;
            case l_syntactic_tree::FOR_NODE:             visit(fun,node->to<l_syntactic_tree::for_node>());             break;
            case l_syntactic_tree::FUNCTION_DEF_NODE:    visit(fun,node->to<l_syntactic_tree::function_def_node>());    break;
            case l_syntactic_tree::RETURN_NODE:          visit(fun,node->to<l_syntactic_tree::return_node>());          break;
            case l_syntactic_tree::CONTEXT_TYPE_NODE:    visit(fun,node->to<l_syntactic_tree::context_type_node>());    break;
            case l_syntactic_tree::CLASS_NODE:         /*visit(fun,node->to<l_syntactic_tree::context_type_node>());*/  break;
            default: assert(0); break;
        }
    }
    
    //function def
    void l_variables_context::visit(l_function* fun,l_syntactic_tree::function_def_node* node)
    {
        //alloc new function
        l_function* new_fun = &m_vm->get_new_function();
        //add data
        node->m_data = (void*)(m_vm->get_count_of_functions()-1);
        //add variable node
        if(!node->m_variable)
        {
            static int anonim_function_id = 0;
            node->m_variable =  l_syntactic_tree::variable
            ("$"
             "_anonymous"
             "_function["
             +std::to_string(++anonim_function_id)
             +"]"
             );
        }
        //...
        visit(fun, node->m_variable);
        //alloc variable
        add_into_table(fun, l_variable((l_function_id)(m_vm->get_count_of_functions()-1)), function_index(node));
        //n args
        new_fun->m_args_size = (unsigned int)  node->m_args.size();
        //args...
        for(l_syntactic_tree::variable_node* variable : node->m_args)
        {
            add_variable_into_table(new_fun, variable);
            is_not_upper_value(new_fun, variable);
        }
        //args list
        if(node->m_name_args_list)
        {
            //add last variable
            add_variable_into_table(new_fun, node->m_name_args_list);
            is_not_upper_value(new_fun, node->m_name_args_list);
            //have args list
            new_fun->m_have_args_list = true;
        }
        //staments
        for(l_syntactic_tree::node* st : node->m_staments)
        {
            visit(new_fun,st);
        }
    }
    
    //assignable
    void l_variables_context::visit(l_function* fun,l_syntactic_tree::variable_node* node)
    {
        add_variable_into_table(fun, node);
    }
    
    void l_variables_context::visit(l_function* fun,l_syntactic_tree::assignable_node* node)
    {
        //return variable
        if (node->m_type == l_syntactic_tree::VARIABLE_NODE)
        {
            visit(fun,node->to_variable_node());
        }
        //is a call
        else if(node->m_type == l_syntactic_tree::CALL_NODE )
        {
            auto* call_node = (l_syntactic_tree::call_node*)node;
            //visit call
            visit(fun,call_node);
        }
        else
        {
            //else is a field
            visit(fun,node->to_field_node()->m_exp);
            //rec call
            visit(fun,node->to_field_node()->m_assignable);
        }
    }
    
    //call
    void l_variables_context::visit(l_function*  fun, l_syntactic_tree::call_node* call_node)
    {
        //args
        for (size_t i = 0; i != call_node->m_args.size(); ++i)
        {
            visit(fun,call_node->m_args[i]);
        }
        //visit call name
        visit(fun,call_node->m_exp_to_call);
    }
    
    //exp
    void l_variables_context::visit(l_function* fun, l_syntactic_tree::exp_node* exp_node)
    {
        if (exp_node->m_type == l_syntactic_tree::EXP_NODE)
        {
            if(exp_node->m_left) visit(fun,exp_node->m_left);
            if(exp_node->m_right) visit(fun,exp_node->m_right);
        }
        else if (exp_node->m_type == l_syntactic_tree::CONSTANT_NODE)
        {
            l_syntactic_tree::constant_node* c_node = exp_node->to<l_syntactic_tree::constant_node>();
            //is push k
            if(c_node->is_push_k())
            {
                add_const_into_table(fun,c_node);
            }
        }
        else if (exp_node->m_type == l_syntactic_tree::FUNCTION_DEF_NODE)
        {
            auto*  def_node = exp_node->to<l_syntactic_tree::function_def_node>();
            visit(fun,def_node);
        }
        else if (exp_node->m_type == l_syntactic_tree::CALL_NODE)
        {
            auto*  call_node = exp_node->to<l_syntactic_tree::call_node>();
            visit(fun,call_node);
        }
        else if (exp_node->m_type == l_syntactic_tree::FIELD_NODE ||
                 exp_node->m_type == l_syntactic_tree::VARIABLE_NODE)
        {
            auto*  assignable_node = exp_node->to<l_syntactic_tree::assignable_node>();
            visit(fun,assignable_node);
        }
        else if(exp_node->m_type == l_syntactic_tree::ARRAY_NODE)
        {
            auto* array_node = exp_node->to<l_syntactic_tree::array_node>();
            //create array declaretion
            for(auto& arr_node_exp:array_node->m_exps) visit(fun,arr_node_exp);
        }
        else if(exp_node->m_type == l_syntactic_tree::TABLE_NODE)
        {
            auto* table_node = exp_node->to<l_syntactic_tree::table_node>();
            //table array declaretion
            for(auto& table_node_exp:table_node->m_exps)
            {
                visit(fun,table_node_exp[0]);
                visit(fun,table_node_exp[1]);
            }
        }
        else if (exp_node->is_link())
        {
            if(exp_node->m_left) visit(fun,exp_node->m_left);
        }
        else if (exp_node->is_one())
        {
            if(exp_node->m_left) visit(fun,exp_node->m_left);
        }
        else
        {
            assert(0);
        }
    }
    
    //is dec?
    bool l_variables_context::is_declared(l_function* fun, l_syntactic_tree::variable_node* variable_node)
    {
        auto& fun_table = m_funs_table[fun];
        auto  it        = fun_table.find(variable_index(variable_node->m_name));
        return it != fun_table.end();
    }
    
    //isn't an up value
    void l_variables_context::is_not_upper_value(l_function* fun, l_syntactic_tree::variable_node* variable_node)
    {
        //not dec?
        if(!is_declared(fun,variable_node)) add_variable_into_table(fun, variable_node);
        //get ref
        auto& context_type = m_funs_table[fun][ variable_index(variable_node->m_name) ].m_context_type;
        //if can change value
        if( context_type == const_info::T_CTX_CAN_TO_BE_UPPER)
        {
            context_type = const_info::T_CTX_CAN_NOT_TO_BE_UPPER;
        }
    }
    void l_variables_context::must_to_be_upper_value(l_function* fun, l_syntactic_tree::variable_node* variable_node)
    {
        //not dec?
        if(!is_declared(fun,variable_node)) add_variable_into_table(fun, variable_node);
        //..
        m_funs_table[fun][ variable_index(variable_node->m_name) ].m_context_type = const_info::T_CTX_MUST_TO_BE_UPPER;
    }
    void l_variables_context::must_to_be_global_value(l_function* fun, l_syntactic_tree::variable_node* variable_node)
    {
        //not dec?
        if(!is_declared(fun,variable_node)) add_variable_into_table(fun, variable_node);
        //..
        m_funs_table[fun][ variable_index(variable_node->m_name) ].m_context_type = const_info::T_CTX_MUST_TO_BE_GLOBAL;
    }
    
    //op
    void l_variables_context::visit(l_function* fun, l_syntactic_tree::op_node* op_node)
    {
        //..
        visit(fun,op_node->m_assignable);
        //.. is def
        if(op_node->m_assignable->is_variable())
        {
            is_not_upper_value(fun,op_node->m_assignable->to_variable_node());
        }
        //..
        visit(fun,op_node->m_exp);
    }
    
    //type context
    void l_variables_context::visit(l_function*  fun, l_syntactic_tree::context_type_node* ctx_node)
    {
        //operations
        for (auto& op_node : ctx_node->m_ops)
        {
            l_syntactic_tree::exp_node* exp_to_call_node = nullptr;
            //visit op
            visit(fun, op_node);
            exp_to_call_node = op_node->m_assignable;
            //assignable node
            //get variable
            while (exp_to_call_node)
            {
                if(exp_to_call_node->m_type == l_syntactic_tree::VARIABLE_NODE) break;
                //is a call
                if(exp_to_call_node->m_type == l_syntactic_tree::CALL_NODE )
                {
                    exp_to_call_node = exp_to_call_node->to_call_node()->m_exp_to_call;
                }
                //is a field
                else
                {
                    exp_to_call_node = exp_to_call_node->to_field_node()->m_assignable;
                }
            }
            //error?
            if(!exp_to_call_node) assert(0);
            //cases
            if(ctx_node->m_context_type == l_syntactic_tree::context_type_node::T_GLOBAL)
            {
                must_to_be_global_value(fun,exp_to_call_node->to_variable_node());
            }
            else
            if(ctx_node->m_context_type == l_syntactic_tree::context_type_node::T_SUPER)
            {
                must_to_be_upper_value(fun,exp_to_call_node->to_variable_node());
            }
        }
        //vars
        for (auto& variable : ctx_node->m_vars)
        {
            if(ctx_node->m_context_type == l_syntactic_tree::context_type_node::T_GLOBAL)
                must_to_be_global_value(fun,variable);
            else if(ctx_node->m_context_type == l_syntactic_tree::context_type_node::T_SUPER)
                must_to_be_upper_value(fun,variable);
        }
    }
    
    //for
    void l_variables_context::visit(l_function* fun, l_syntactic_tree::for_node* for_node)
    {
        switch (for_node->m_type_for)
        {
                
            case l_syntactic_tree::for_node::FOR_OF:
            case l_syntactic_tree::for_node::FOR_IN:
                if (for_node->m_variable_left)  visit(fun, for_node->m_variable_left);
                if (for_node->m_variable_right) visit(fun, for_node->m_variable_right);
                for(auto& st_node : for_node->m_staments) visit(fun,st_node);
                break;
            case l_syntactic_tree::for_node::FOR_C:
                
                for(auto& op_node : for_node->m_ops_left ) visit(fun,op_node);
                
                if (for_node->m_exp)                       visit(fun, for_node->m_exp);
                
                for(auto& op_node : for_node->m_ops_right) visit(fun,op_node);
                
                for(auto& st_node : for_node->m_staments ) visit(fun,st_node);
                break;
                
            default: assert(0); break;
        }
        
    }
    
    //while
    void l_variables_context::visit(l_function* fun, l_syntactic_tree::while_node* while_node)
    {
        visit(fun,while_node->m_exp);
        for(auto& st_node : while_node->m_staments) visit(fun,st_node);
    }
    
    //return
    void l_variables_context::visit(l_function* fun, l_syntactic_tree::return_node* return_node)
    {
        if(return_node->m_exp) visit(fun,return_node->m_exp);
    }
    
    //if
    void l_variables_context::visit(l_function* fun, l_syntactic_tree::if_node* if_node)
    {
        for(auto& node : if_node->m_ifs)
        {
            //visit exp
            visit(fun, node.m_exp);
            //visit statements
            for(auto& st_node : node.m_staments) visit(fun, st_node);
        }
        //visit else statements
        for(auto& st_node : if_node->m_else_staments) visit(fun, st_node);
    }
    
    //root
    void l_variables_context::visit(l_function* fun, l_syntactic_tree::root_node* root)
    {
        for(auto* node : root->m_staments) visit(fun,node);
    }
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    void l_variables_context::build_variable_context_from_tree(l_vm* vm,
                                                               const l_syntactic_tree* tree)
    {
        //visit
        visit(m_main,tree->m_root);
    }
    void l_variables_context::build_variable_context_from_tree(l_vm* vm,
                                                               l_function* function,
                                                               const l_syntactic_tree* tree)
    {
        //visit
        visit(function,tree->m_root);
    }
    
    void l_variables_context::set_vm(l_vm* vm)
    {
        m_vm = vm;
        set_gc_from_vm(vm);
    }
    
    void l_variables_context::set_main_function(l_function* main)
    {
        m_main = main;
    }
    
    void l_variables_context::set_gc_from_vm(l_vm* vm)
    {
        m_gc = &vm->get_gc();
    }
    //push variable
    void l_variables_context::add_variable_into_table(l_function* f_context, l_syntactic_tree::variable_node* var_value)
    {
        //function map context
        auto& f_table = m_funs_table[f_context];
        //index
        std::string key = variable_index(var_value);
        //find name
        if(f_table.find(key) == f_table.end())
        {
            f_table[key] = const_info(m_gc, get_new_id(f_context), var_value );
        }
    }
    
    //push constant
    void l_variables_context::add_const_into_table(l_function* f_context, l_syntactic_tree::constant_node* const_value)
    {
        //function map context
        auto& f_table = m_funs_table[f_context];
        //index
        std::string key = const_index(const_value);
        //find name
        if(f_table.find(key) == f_table.end())
        {
            f_table[key] = const_info(m_gc, get_new_id(f_context), const_value );
        }
    }
    
    //push constant
    void l_variables_context::add_into_table(l_function* f_context, const l_variable& variable, const std::string& const_name)
    {
        //function map context
        auto& f_table = m_funs_table[f_context];
        //find name
        if(f_table.find(const_name) == f_table.end())
        {
            f_table[const_name] = const_info( get_new_id(f_context), variable );
        }
    }
    //get
    int l_variables_context::get_var_id(l_function* f_context,l_syntactic_tree::variable_node* node)
    {
        return get_table_id(f_context,variable_index(node));
    }
    
    int l_variables_context::get_const_id(l_function* f_context,l_syntactic_tree::constant_node* c_node)
    {
        return get_table_id(f_context,const_index(c_node));
    }
    
    int l_variables_context::get_function_id(l_function* f_context,l_syntactic_tree::function_def_node* c_node)
    {
        return get_table_id(f_context,function_index(c_node));
    }
    
    bool l_variables_context::is_upper_value(l_function* f_context,l_syntactic_tree::variable_node* node)
    {
        //function map context
        auto& f_table  = m_funs_table[f_context];
        //find name
        auto  id_const = f_table.find(variable_index(node));
        //find name
        if( id_const != f_table.end() )
        {
            return id_const->second.m_context_type == const_info::T_CTX_MUST_TO_BE_UPPER ||
            id_const->second.m_context_type == const_info::T_CTX_CAN_TO_BE_UPPER;
        }
        return false;
    }
    bool l_variables_context::is_global_value(l_function* f_context,l_syntactic_tree::variable_node* node)
    {
        //function map context
        auto& f_table  = m_funs_table[f_context];
        //find name
        auto  id_const = f_table.find(variable_index(node));
        //find name
        if( id_const != f_table.end() )
        {
            return id_const->second.m_context_type == const_info::T_CTX_MUST_TO_BE_GLOBAL;
        }
        return false;
    }
    
    int l_variables_context::get_table_id(l_function* f_context,const std::string& name)
    {
        //function map context
        auto& f_table  = m_funs_table[f_context];
        //find name
        auto  id_const = f_table.find(name);
        //find name
        if( id_const != f_table.end() )
        {
            return id_const->second.m_id;
        }
        return TABLE_GET_ERROR;
    }
};