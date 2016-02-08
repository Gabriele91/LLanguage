//
//  variables_context.h.
//  LLanguage
//
//  Created by Gabriele on 22/01/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#pragma once
#include <vector>
#include <string>
#include <array>
#include <map>
#include <l_vm.h>
#include <l_variable.h>
#include <l_syntactic_tree.h>

namespace l_language
{
    #define TABLE_GET_ERROR 0xFFFFF
    
    class l_variables_context
    {
        
    protected:
        
        //main context function
        l_function* m_main {  nullptr  };
        //main context function
        l_gc*       m_gc {  nullptr  };
        //const struct
        struct const_info
        {
            unsigned int m_id { 0 };
            l_syntactic_tree::variable_node*   m_variable_node  { nullptr };
            l_syntactic_tree::constant_node*   m_constant_node  { nullptr };
            l_variable m_variable;
            
            const_info()
            {
            }
            
            const_info(l_gc* gc,
                       unsigned int l_id,
                       l_syntactic_tree::constant_node* constant_node)
            {
                m_id = l_id;
                m_constant_node = constant_node;
                //select type
                if(m_constant_node)
                    switch (m_constant_node->m_const_type)
                    {
                        case l_syntactic_tree::constant_node::INT:
                            m_variable=l_variable(m_constant_node->m_value.m_int);
                            break;
                        case l_syntactic_tree::constant_node::FLOAT:
                            m_variable=l_variable(m_constant_node->m_value.m_float);
                        break;
                        case l_syntactic_tree::constant_node::STRING:
                            m_variable=l_string::const_new(gc,m_constant_node->m_value.m_string);
                        break;
                        default: assert(0); break;
                    };
            }
            
            const_info(l_gc* gc,
                       unsigned int l_id,
                       l_syntactic_tree::variable_node* variable_node)
            {
                m_id = l_id;
                m_variable_node = variable_node;
                m_variable=l_string::const_new(gc,m_variable_node->m_name);
            }
            
            const_info(unsigned int l_id,
                       const l_variable& variable)
            {
                m_id = l_id;
                m_constant_node = nullptr;
                m_variable = variable;
            }
        };
        //function context, value map
        using function_table = std::map< std::string, const_info > ;
        //variable map
        using functions_var_table = std::map< l_function*, function_table >;
        //map
        functions_var_table m_funs_table;
        //id generators
        unsigned int m_gen_id { 0 };
        //utility methos
        unsigned int get_new_id()
        {
            return m_gen_id++;
        }
        
        //const to index
        std::string const_index(l_syntactic_tree::constant_node* const_value)
        {
            //name node
            std::string c_name;
            //name
            switch (const_value->m_const_type)
            {
                case l_syntactic_tree::constant_node::FLOAT:  c_name = "float:"+std::to_string(const_value->m_value.m_float); break;
                case l_syntactic_tree::constant_node::INT:    c_name = "int:"+std::to_string(const_value->m_value.m_int); break;
                case l_syntactic_tree::constant_node::STRING: c_name = "string:"+const_value->m_value.m_string; break;
                default: assert(0); break;
            }
            
            return c_name;
        }
        
        std::string variable_index(const std::string& name)
        {
            return "string:"+name;
        }
        
        std::string variable_index(l_syntactic_tree::variable_node* var_node)
        {
            return "string:"+var_node->m_name;
        }
        
        void visit(l_function* fun, l_syntactic_tree::node* node)
        {
                
            switch (node->m_type)
            {
                case l_syntactic_tree::OP_NODE:     visit(fun,node->to<l_syntactic_tree::op_node>());    break;
                case l_syntactic_tree::CALL_NODE:   visit(fun,node->to<l_syntactic_tree::call_node>());  break;
                case l_syntactic_tree::IF_NODE:     visit(fun,node->to<l_syntactic_tree::if_node>());    break;
                case l_syntactic_tree::WHILE_NODE:  visit(fun,node->to<l_syntactic_tree::while_node>()); break;
                case l_syntactic_tree::FOR_NODE:    visit(fun,node->to<l_syntactic_tree::for_node>());   break;
                default: assert(0); break;
            }
        }
        
        //assignable
        void visit(l_function* fun,l_syntactic_tree::variable_node* node)
        {
            add_variable_into_table(fun, node);
        }
        
        void visit(l_function* fun,l_syntactic_tree::assignable_node* node)
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
        void visit(l_function*  fun, l_syntactic_tree::call_node* call_node)
        {
            //args
            for (size_t i = 0; i != call_node->m_args.size(); ++i)
            {
                visit(fun,call_node->m_args[i]);
            }
            //visit call name
            visit(fun,call_node->m_assignable);
        }
        
        //exp
        void visit(l_function* fun, l_syntactic_tree::exp_node* exp_node)
        {
            if (exp_node->m_type == l_syntactic_tree::EXP_NODE)
            {
                if(exp_node->m_left) visit(fun,exp_node->m_left);
                if(exp_node->m_right) visit(fun,exp_node->m_right);
            }
            else if (exp_node->m_type == l_syntactic_tree::CONSTANT_NODE)
            {
                l_syntactic_tree::constant_node* c_node = exp_node->to<l_syntactic_tree::constant_node>();
                add_const_into_table(fun,c_node);
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
        
        //op
        void visit(l_function* fun, l_syntactic_tree::op_node* op_node)
        {
            visit(fun,op_node->m_assignable);
            visit(fun,op_node->m_exp);
        }
        
        //for
        void visit(l_function* fun, l_syntactic_tree::for_node* for_node)
        {
            switch (for_node->m_type_for)
            {
                    
                case l_syntactic_tree::for_node::FOR_OF:
                case l_syntactic_tree::for_node::FOR_IN:
                    
                    visit(fun, for_node->m_variable_left);
                    visit(fun, for_node->m_variable_right);
                    for(auto& st_node : for_node->m_staments) visit(fun,st_node);
                    
                break;
                    
                default: assert(0); break;
            }
            
        }
        
        //while
        void visit(l_function* fun, l_syntactic_tree::while_node* while_node)
        {
            visit(fun,while_node->m_exp);
            for(auto& st_node : while_node->m_staments) visit(fun,st_node);
        }
        
        //if
        void visit(l_function* fun, l_syntactic_tree::if_node* if_node)
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
        void visit(l_function* fun, l_syntactic_tree::root_node* root)
        {
            for(auto* node : root->m_staments) visit(fun,node);
        }
        
        
    public:
        
        void build_variable_context_from_tree(l_vm* vm,
                                              const l_syntactic_tree* tree)
        {
            //visit
            visit(m_main,tree->m_root);
        }
        
        void set_main_function(l_function* main)
        {
            m_main = main;
        }
        
        void set_gc_from_vm(l_vm* vm)
        {
            m_gc = &vm->get_gc();
        }
        //push variable
        void add_variable_into_table(l_function* f_context, l_syntactic_tree::variable_node* var_value)
        {
            //function map context
            auto& f_table = m_funs_table[f_context];
            //index
            std::string key = variable_index(var_value);
            //find name
            if(f_table.find(key) == f_table.end())
            {
                f_table[key] = const_info(m_gc, get_new_id(), var_value );
            }
        }
        
        //push constant
        void add_const_into_table(l_function* f_context, l_syntactic_tree::constant_node* const_value)
        {
            //function map context
            auto& f_table = m_funs_table[f_context];
            //index
            std::string key = const_index(const_value);
            //find name
            if(f_table.find(key) == f_table.end())
            {
                f_table[key] = const_info(m_gc, get_new_id(), const_value );
            }
        }
        
        //push constant
        void add_into_table(l_function* f_context, const l_variable& variable, const std::string& const_name)
        {
            //function map context
            auto& f_table = m_funs_table[f_context];
            //find name
            if(f_table.find(const_name) == f_table.end())
            {
                f_table[const_name] = const_info( get_new_id(), variable );
            }
        } 
        //get
        int get_var_id(l_function* f_context,l_syntactic_tree::variable_node* node)
        {
            return get_table_id(f_context,variable_index(node));
        }
        
        int get_const_id(l_function* f_context,l_syntactic_tree::constant_node* c_node)
        {
            return get_table_id(f_context,const_index(c_node));
        }
        
        int get_table_id(l_function* f_context,const std::string& name)
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
};