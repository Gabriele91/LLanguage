//
//  compiler_tree_to_vm.h
//  LLanguage
//
//  Created by Gabriele on 19/01/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#pragma once
#include <string>
#include <map>
#include <l_vm.h>
#include <syntactic_tree.h>
#include <variables_context.h>
#if 1
namespace l_language
{
    class compiler_tree_to_vm : public variables_context
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
        
        
        
        bool compile_variable_set(l_function* fun,syntactic_tree::variable_node* node)
        {
            if(fun == m_main)
            {
                fun->push({ L_SET_GLOBAL, get_var_id(fun, node) });
            }
            else
            {
                assert(0);
            }
            return true;
        }
        
        bool compile_variable_get(l_function* fun,syntactic_tree::variable_node* node)
        {
            if(fun == m_main)
            {
                fun->push({ L_GET_GLOBAL, get_var_id(fun, node), node->m_line });
            }
            else
            {
                assert(0);
            }
            return true;
        }
        
        bool compile_assignable(l_function* fun,syntactic_tree::assignable_node* node,int is_not_first = true)
        {
            //return variable
            if (node->m_type == syntactic_tree::VARIABLE_NODE)
            {
                return compile_variable_get(fun,node->to_variable_node());
            }
            //rec call
            compile_assignable(fun,node->to_field_node()->m_assignable);
            //exp
            compile_exp(fun,node->to_field_node()->m_exp);
            //push
            if(is_not_first) fun->push({ L_GET_AT_VAL, 0, node->m_line });
            //return
            return true;
        }
        
        bool compile_assignable_get(l_function* fun,syntactic_tree::assignable_node* node)
        {
            //return
            return compile_assignable(fun,node,true);
        }
        
        bool compile_assignable_set(l_function* fun,syntactic_tree::assignable_node* node)
        {
            //return
            return compile_assignable(fun,node,false);
        }
        
        //exp
        bool compile_exp(l_function* fun,syntactic_tree::exp_node* node)
        {
            if (node->m_type == syntactic_tree::EXP_NODE)
            {
                if (node->is_link())
                {
                    return compile_exp(fun,node->m_left);
                }
                if (node->is_one())
                {
                    compile_exp(fun,node->m_left);
                    
                    if(node->m_name == "!")
                    {
                        fun->push({ L_NOT, 0, node->m_line });
                    }
                    else if(node->m_name == "-")
                    {
                        fun->push({ L_UNM, 0, node->m_line });
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    compile_exp(fun,node->m_left);
                    compile_exp(fun,node->m_right);
                    
                         if(node->m_name == "+")  fun->push({ L_ADD, 0, node->m_line });
                    else if(node->m_name == "-")  fun->push({ L_SUB, 0, node->m_line });
                    else if(node->m_name == "*")  fun->push({ L_MUL, 0, node->m_line });
                    else if(node->m_name == "/")  fun->push({ L_DIV, 0, node->m_line });
                    else if(node->m_name == "&&") fun->push({ L_AND, 0, node->m_line });
                    else if(node->m_name == "||") fun->push({ L_OR,  0, node->m_line });
                    else if(node->m_name == "==") fun->push({ L_EQ,  0, node->m_line });
                    else if(node->m_name == ">")  fun->push({ L_RT,  0, node->m_line });
                    else if(node->m_name == "<")  fun->push({ L_LT,  0, node->m_line });
                    else if(node->m_name == ">=") fun->push({ L_RE,  0, node->m_line });
                    else if(node->m_name == "<=") fun->push({ L_LE,  0, node->m_line });
                    else return false;
                }
                
                return true;
                
            }
            else if (node->m_type == syntactic_tree::CONSTANT_NODE)
            {
                //cast
                syntactic_tree::constant_node* c_node = node->to<syntactic_tree::constant_node>();
                //push command
                fun->push({ L_PUSHK,  get_const_id(fun, c_node), node->m_line });
                //success
                return true;
            }
            else if (node->m_type == syntactic_tree::FIELD_NODE ||
                     node->m_type == syntactic_tree::VARIABLE_NODE)
            {
                auto*  assignable_node = node->to<syntactic_tree::assignable_node>();
                return compile_assignable_get(fun,assignable_node);
            }
            else if(node->m_type == syntactic_tree::CALL_NODE)
            {
                //return true...
                return compile_call(fun, (syntactic_tree::call_node*)node);
            }
            else if(node->m_type == syntactic_tree::ARRAY_NODE)
            {
                //cast to array
                auto* array_node = node->to<syntactic_tree::array_node>();
                //push all
                for(auto& node_exp:array_node->m_exps)
                {
                    if(!compile_exp(fun,node_exp)) return false;
                }
                //init table
                fun->push({ L_NEW_ARRAY, (int)array_node->m_exps.size(), node->m_line });
                //return true...
                return true;
            }
            else if(node->m_type == syntactic_tree::TABLE_NODE)
            {
                //cast to array
                auto* table_node = node->to<syntactic_tree::table_node>();
                //push all
                for(auto& node_exp:table_node->m_exps)
                {
                    if(!compile_exp(fun,node_exp[0])) return false;
                    if(!compile_exp(fun,node_exp[1])) return false;
                }
                //init table
                fun->push({ L_NEW_TABLE, (int)table_node->m_exps.size()*2, node->m_line });
                //return true...
                return true;
            }
            return false;
        }
        
        //compile op
        bool compile_op(l_function* fun,syntactic_tree::op_node* node)
        {
            if(node->m_assignable->is_variable())
            {
                if(!compile_exp(fun,node->m_exp)) return false;
                compile_variable_set(fun,node->m_assignable->to<syntactic_tree::variable_node>());
            }
            else
            {
                if(!compile_assignable_set(fun,node->m_assignable)) return false;
                if(!compile_exp(fun,node->m_exp)) return false;
                fun->push({ L_SET_AT_VAL, 0, node->m_line });
            }
            
            return true;
        }
        
        bool compile_while(l_function* fun,syntactic_tree::while_node* node)
        {
            //get current line
            int op_line = (int)fun->m_commands.size();
            //while exp
            if(! compile_exp(fun,node->m_exp) ) return false;
            //push while if
            size_t if0_cmd = fun->push({ L_IF0, 0, node->m_line });
            //compile staments
            if(! compile_statements(fun,node->m_staments) ) return false;
            //push jump
            fun->push({ L_JMP, op_line, node->m_line });
            //cmp ref change jump
            fun->command(if0_cmd).m_arg = (int)fun->m_commands.size();
            //end
            return true;
        }
        
        bool compile_if_else(l_function* fun,syntactic_tree::if_node* ifs_node)
        {
            //if lines
            std::vector< size_t > exp_labels;
            std::vector< size_t > if0_labels;
            std::vector< size_t > jmp_labels;
            //build if and else if
            for(auto& if_field: ifs_node->m_ifs)
            {
                //jmp line
                exp_labels.push_back(fun->m_commands.size());
                //if exp
                compile_exp(fun,if_field.m_exp);
                //if ceck
                if0_labels.push_back(fun->push({ L_IF0, 0, ifs_node->m_line }));
                //statements
                compile_statements(fun,if_field.m_staments);
                //jmp
                jmp_labels.push_back(fun->push({ L_JMP, 0, ifs_node->m_line }));
            }
            //last if block
            size_t jmp_end_if = jmp_labels.back() + 1;
            //push else
            if(ifs_node->have_else())
            {
                compile_statements(fun,ifs_node->m_else_staments);
            }
            //last block
            size_t jmp_end = fun->m_commands.size();
            //refs build
            for(size_t i = 0; i!=jmp_labels.size() ;++i)
            {
                //labels
                if( i != if0_labels.size()-1 )
                {
                    fun->command( if0_labels[i] ).m_arg = (int)exp_labels[i+1];
                }
                else
                {
                    fun->command( if0_labels[i] ).m_arg = (int)jmp_end_if;
                }
                //set jump at the end
                fun->command( jmp_labels[i] ).m_arg = (int)jmp_end;
            }
            
            return true;
        }
        
        bool compile_for(l_function* fun,syntactic_tree::for_node* for_node)
        {
            
            l_op_code  for_type = for_node->m_type_for == syntactic_tree::for_node::FOR_OF ?
                                  L_FOR_OF :
                                  L_FOR_IN;
            
            switch (for_node->m_type_for)
            {
                case syntactic_tree::for_node::FOR_OF:
                case syntactic_tree::for_node::FOR_IN:
                {
                    compile_assignable_get(fun,for_node->m_variable_right);
                    //get it
                    fun->push({ L_IT, 0, for_node->m_line });
                    //get line
                    int jmp_cmd_line = (int)fun->m_commands.size();
                    //for cmd
                    size_t for_it = fun->push({ for_type, 0, for_node->m_line });
                    //put
                    //compile_assignable_set(fun,for_node->m_variable_left);
                    compile_variable_set(fun,for_node->m_variable_left->to<syntactic_tree::variable_node>());
                    //statement
                    compile_statements(fun, for_node->m_staments);
                    //.. jmp
                    fun->push({ L_JMP, jmp_cmd_line,  for_node->m_line});
                    //set jump at the end
                    fun->command( for_it ).m_arg = (int)fun->m_commands.size();
                }
                break;
                    
                default: assert(0); break;
            }
            return true;
        }
        //compile call
        bool compile_call(l_function* fun,syntactic_tree::call_node* call_node)
        {
            int n_args = (int)call_node->m_args.size();
            //push call
            auto* call_name = (syntactic_tree::variable_node*)call_node->m_assignable;
            //search in vars
            int var_id = get_var_id(fun, call_name->m_name);
            //push args
            for(syntactic_tree::exp_node* exp_node : call_node->m_args)
            {
                compile_exp(fun, exp_node);
            }
            //push call
            fun->push({ L_GET_GLOBAL, var_id, call_node->m_line });
            //push call
            fun->push({ L_CALL, n_args, call_node->m_line });
            //success
            return true;
        }
        //compile a statements
        bool compile_statements(l_function* fun,syntactic_tree::list_nodes& nodes)
        {
            for (syntactic_tree::node* node : nodes)
            {
                switch (node->m_type)
                {
                    case syntactic_tree::OP_NODE:
                        if(! compile_op(fun,node->to<syntactic_tree::op_node>()) ) return false;
                    break;
                    case syntactic_tree::WHILE_NODE: 
                        if(! compile_while(fun,node->to<syntactic_tree::while_node>()) ) return false;
                    break;
                    case syntactic_tree::IF_NODE:
                        if(! compile_if_else(fun,node->to<syntactic_tree::if_node>()) ) return false;
                    break;
                    case syntactic_tree::FOR_NODE:
                        if(! compile_for(fun, node->to<syntactic_tree::for_node>()) ) return false;
                    break;
                    case syntactic_tree::CALL_NODE:
                        if(! compile_call(fun,node->to<syntactic_tree::call_node>()) ) return false;
                    break;
                    default:  break;
                }
            }
            return true;
        }
        

        //reference vm
        l_vm* m_vm { nullptr };
        
    public:
        
        void set_vm(l_vm* this_vm)
        {
            //ptr to vm
            m_vm = this_vm;
            //main function
            m_vm->m_functions.resize(1);
            //set main
            set_main_function(&m_vm->m_functions[0]);
            //set gc
            set_gc_from_vm(m_vm);
        }
        
        void add_c_function(const l_cfunction function,const std::string& cfun_name)
        {
            //name into table
            add_variable_into_table_and_id(m_main,cfun_name,-(int)(m_vm->m_globals.size()+1));
            m_vm->m_globals.push_back(l_variable(function));
            
        }
        
        l_thread* compile(const syntactic_tree* tree)
        {
            //build tablet
            build_variable_context_from_tree(m_vm, tree);
            //ptr to fun table
            auto& fun_table = m_funs_table[m_main];
            //add info
            m_main->m_args_size = 0;
            m_main->m_up_val_size = 0;
            m_main->m_values_size = (unsigned int)fun_table.m_vars.size();
            //alloc values
            m_main->m_costants.resize(fun_table.m_consts.size());
            //puth const
            for(auto it : fun_table.m_consts)
            {
                //save const
                m_main->m_costants[it.second.m_id] = it.second.m_variable;
            }
            //
            if(!compile_statements(m_main,tree->m_root->m_staments)) return nullptr;
            //alloc thread
            m_vm->m_threads.push_back(std::move(l_thread(m_vm, // VM
                                                         0,    // MAIN
                                                         32    // STACK
                                                         )));
            //compile statemens
            return &(m_vm->m_threads.back());
        }
    };
}
#endif