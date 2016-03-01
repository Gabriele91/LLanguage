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
#include <l_vm.h>
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
        
        
        
        bool compile_variable_set(l_function* fun,l_syntactic_tree::variable_node* node)
        {
            if(fun == m_main)
            {
                fun->push({ L_SET_GLOBAL, get_var_id(fun, node), node->m_line });
            }
            else
            {
                if(is_upper_value(fun,node))
                    fun->push({ L_SET_UP_VALUE, get_var_id(fun, node), node->m_line });
                else if(is_global_value(fun,node))
                    fun->push({ L_SET_GLOBAL, get_var_id(fun, node), node->m_line });
                else
                    fun->push({ L_SET_LOCAL, get_var_id(fun, node), node->m_line });
                //assert(0);
            }
            return true;
        }
        
        bool compile_variable_get(l_function* fun,l_syntactic_tree::variable_node* node)
        {
            if(fun == m_main)
            {
                fun->push({ L_GET_GLOBAL, get_var_id(fun, node), node->m_line });
            }
            else
            {
                if(is_upper_value(fun,node))
                    fun->push({ L_GET_UP_VALUE, get_var_id(fun, node), node->m_line });
                else if(is_global_value(fun,node))
                    fun->push({ L_GET_GLOBAL, get_var_id(fun, node), node->m_line });
                else
                    fun->push({ L_GET_LOCAL, get_var_id(fun, node), node->m_line });
                //assert(0);
            }
            return true;
        }
        
        bool compile_assignable(l_function* fun,l_syntactic_tree::assignable_node* node,int is_not_first = true)
        {
            //return variable
            if (node->m_type == l_syntactic_tree::VARIABLE_NODE)
            {
                return compile_variable_get(fun,node->to_variable_node());
            }
            else if (node->m_type == l_syntactic_tree::CALL_NODE)
            {
                return compile_call(fun,(l_syntactic_tree::call_node*)node);
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
        
        bool compile_assignable_get(l_function* fun,l_syntactic_tree::assignable_node* node)
        {
            //return
            return compile_assignable(fun,node,true);
        }
        
        bool compile_assignable_set(l_function* fun,l_syntactic_tree::assignable_node* node)
        {
            //return
            return compile_assignable(fun,node,false);
        }
        
        
        bool compile_exp_aux(l_function* fun,l_syntactic_tree::exp_node* node)
        {
            if (node->m_type == l_syntactic_tree::EXP_NODE)
            {
                if (node->is_link())
                {
                    return compile_exp_aux(fun,node->m_left);
                }
                if (node->is_one())
                {
                    compile_exp_aux(fun,node->m_left);
                    
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
                    //flag
                    bool is_logic_op = true;
                    //left
                    compile_exp_aux(fun,node->m_left);
                    //logic op
                         if(node->m_name == "||") fun->push({ L_IF_OR_POP,   0, node->m_line });
                    else if(node->m_name == "&&") fun->push({ L_IF0_OR_POP,  0, node->m_line });
                    else is_logic_op = false;
                    //right
                    compile_exp_aux(fun,node->m_right);
                    //math op
                         if(is_logic_op)          return true;
                    else if(node->m_name == "+")  fun->push({ L_ADD, 0, node->m_line });
                    else if(node->m_name == "-")  fun->push({ L_SUB, 0, node->m_line });
                    else if(node->m_name == "*")  fun->push({ L_MUL, 0, node->m_line });
                    else if(node->m_name == "/")  fun->push({ L_DIV, 0, node->m_line });
                    else if(node->m_name == "%")  fun->push({ L_MOD, 0, node->m_line });
                    else if(node->m_name == "==") fun->push({ L_EQ,  0, node->m_line });
                    else if(node->m_name == "!=") fun->push({ L_NEQ,  0, node->m_line });
                    else if(node->m_name == ">")  fun->push({ L_RT,  0, node->m_line });
                    else if(node->m_name == "<")  fun->push({ L_LT,  0, node->m_line });
                    else if(node->m_name == ">=") fun->push({ L_RE,  0, node->m_line });
                    else if(node->m_name == "<=") fun->push({ L_LE,  0, node->m_line });
                    else return false;
                }
                
                return true;
                
            }
            else if (node->m_type == l_syntactic_tree::CONSTANT_NODE)
            {
                //cast
                l_syntactic_tree::constant_node* c_node = node->to<l_syntactic_tree::constant_node>();
                //push command
                fun->push({ L_PUSHK,  get_const_id(fun, c_node), node->m_line });
                //success
                return true;
            }
            else if (node->m_type == l_syntactic_tree::FIELD_NODE ||
                     node->m_type == l_syntactic_tree::VARIABLE_NODE)
            {
                auto*  assignable_node = node->to<l_syntactic_tree::assignable_node>();
                return compile_assignable_get(fun,assignable_node);
            }
            else if(node->m_type == l_syntactic_tree::CALL_NODE)
            {
                //return true...
                return compile_call(fun, (l_syntactic_tree::call_node*)node);
            }
            else if(node->m_type == l_syntactic_tree::ARRAY_NODE)
            {
                //cast to array
                auto* array_node = node->to<l_syntactic_tree::array_node>();
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
            else if(node->m_type == l_syntactic_tree::TABLE_NODE)
            {
                //cast to array
                auto* table_node = node->to<l_syntactic_tree::table_node>();
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
        
        //exp
        bool compile_exp(l_function* fun,l_syntactic_tree::exp_node* node)
        {
            //get op
            size_t op0 = fun->size_commands();
            //reconstruct jmp
            bool status = compile_exp_aux(fun, node);
            //invalid?
            if(!status) return false;
            //get last op
            size_t opN = fun->size_commands();
            //recostruct
            for(size_t op = op0; op != opN; ++op)
            {
                switch (fun->command(op).m_op_code)
                {
                    case L_IF_OR_POP:
                    case L_IF0_OR_POP:
                        fun->command(op).m_arg = (unsigned int)opN;
                    break;
                    default:break;
                }
            }
            return true;
        }
        
        //compile op
        bool compile_op(l_function* fun,l_syntactic_tree::op_node* node)
        {
            
            //no op
            l_op_code opcode = L_NO_OP;
            //cases
            if(node->m_name == "+=") opcode = L_ADD;
            else if(node->m_name == "-=") opcode = L_SUB;
            else if(node->m_name == "*=") opcode = L_MUL;
            else if(node->m_name == "/=") opcode = L_DIV;
            //
            if(node->m_assignable->is_variable())
            {
                //is (+|-|*|/)=
                if(opcode != L_NO_OP)
                {
                    //get <var>
                    compile_variable_get(fun,node->m_assignable->to<l_syntactic_tree::variable_node>());
                }
                //<exp>
                if(!compile_exp(fun,node->m_exp)) return false;
                //op
                //+|-|*|/
                if(opcode != L_NO_OP)
                {
                    fun->push({ opcode, 0, node->m_line });
                }
                //set var
                compile_variable_set(fun,node->m_assignable->to<l_syntactic_tree::variable_node>());
            }
            else
            {
                if(!compile_assignable_set(fun,node->m_assignable)) return false;
                //is (+|-|*|/)=
                if(opcode != L_NO_OP)
                {
                    //get <var>
                    if(!compile_assignable_get(fun,node->m_assignable)) return false;
                }
                //exp
                if(!compile_exp(fun,node->m_exp)) return false;
                //op
                //+|-|*|/
                if(opcode != L_NO_OP)
                {
                    fun->push({ opcode, 0, node->m_assignable->m_line });
                }
                //pusn into array
                fun->push({ L_SET_AT_VAL, 0, node->m_assignable->m_line });
            }
            
            return true;
        }
        
        bool compile_while(l_function* fun,l_syntactic_tree::while_node* node)
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
        
        bool compile_if_else(l_function* fun,l_syntactic_tree::if_node* ifs_node)
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
        
        bool compile_for(l_function* fun,l_syntactic_tree::for_node* for_node)
        {
            
            l_op_code  for_type = for_node->m_type_for == l_syntactic_tree::for_node::FOR_OF ?
                                  L_FOR_OF :
                                  L_FOR_IN;
            
            switch (for_node->m_type_for)
            {
                case l_syntactic_tree::for_node::FOR_OF:
                case l_syntactic_tree::for_node::FOR_IN:
                {
                    compile_exp(fun,for_node->m_variable_right);
                    //get it
                    fun->push({ L_IT, 0, for_node->m_line });
                    //get line
                    int jmp_cmd_line = (int)fun->m_commands.size();
                    //for cmd
                    size_t for_it = fun->push({ for_type, 0, for_node->m_line });
                    //put
                    //compile_assignable_set(fun,for_node->m_variable_left);
                    compile_variable_set(fun,for_node->m_variable_left->to<l_syntactic_tree::variable_node>());
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
        bool compile_call(l_function* fun,l_syntactic_tree::call_node* call_node)
        {
            int n_args = (int)call_node->m_args.size();
            //push args
            for(auto
                it  = call_node->m_args.rbegin();
                it != call_node->m_args.rend();
                ++it)
            {
                compile_exp(fun, *it);
            }
            //push call
            compile_assignable_get(fun,call_node->m_assignable);
            //push call
            fun->push({ L_CALL, n_args, call_node->m_line });
            //success
            return true;
        }
        //compile fun
        bool compile_function_def(l_function* fun,l_syntactic_tree::function_def_node* function_def_node)
        {
            //get function id
            size_t id_function = ((size_t)function_def_node->m_data);
            //compile staments
            compile_statements(&m_vm->function((unsigned int)id_function),function_def_node->m_staments);
            //get fun id
            fun->push({ L_CLOSER, get_function_id(fun, function_def_node), function_def_node->m_line });
            //push call
            compile_variable_set(fun,function_def_node->m_variable);
            //success
            return true;
        }
        //compile return
        bool compile_return(l_function* fun,l_syntactic_tree::return_node* return_node)
        {
            //compile exp
            if(return_node->m_exp) compile_exp(fun, return_node->m_exp);
            //get fun id
            fun->push({ L_RETURN, return_node->m_exp ? 1:0, return_node->m_line });
            //success
            return true;
        }
        //compile a statements
        bool compile_statements(l_function* fun,l_syntactic_tree::list_nodes& nodes)
        {
            for (l_syntactic_tree::node* node : nodes)
            {
                switch (node->m_type)
                {
                    case l_syntactic_tree::OP_NODE:
                        if(! compile_op(fun,node->to<l_syntactic_tree::op_node>()) ) return false;
                    break;
                    case l_syntactic_tree::WHILE_NODE: 
                        if(! compile_while(fun,node->to<l_syntactic_tree::while_node>()) ) return false;
                    break;
                    case l_syntactic_tree::IF_NODE:
                        if(! compile_if_else(fun,node->to<l_syntactic_tree::if_node>()) ) return false;
                    break;
                    case l_syntactic_tree::FOR_NODE:
                        if(! compile_for(fun, node->to<l_syntactic_tree::for_node>()) ) return false;
                        break;
                    case l_syntactic_tree::CALL_NODE:
                        if(! compile_call(fun,node->to<l_syntactic_tree::call_node>()) ) return false;
                        break;
                    case l_syntactic_tree::FUNCTION_DEF_NODE:
                        if(! compile_function_def(fun,node->to<l_syntactic_tree::function_def_node>()) ) return false;
                    break;
                    case l_syntactic_tree::RETURN_NODE:
                        if(! compile_return(fun,node->to<l_syntactic_tree::return_node>()) ) return false;
                    break;
                    default:  break;
                }
            }
            return true;
        }
        

        //reference vm
        l_vm* m_vm { nullptr };
        
    public:
        
        l_compiler_tree_to_vm(l_thread* thread)
        {
            set_thread(thread);
        }
        
        void set_thread(l_thread* thread)
        {
            //ptr to vm
            m_vm = thread->get_vm();
            //main function
            thread->m_main_fun_id = m_vm->get_new_function_id();
            //set main
            set_main_function(&m_vm->function(thread->m_main_fun_id));
            //set gc
            set_vm(thread->get_vm());
        }
        
        void add_c_function(l_thread& thread,const l_cfunction function,const std::string& cfun_name)
        {
            l_variable variable = l_string::const_new(thread.get_gc(),cfun_name);
            //add variable
            thread.main_context()->variable(variable,function);
            //name into table
            add_into_table(&m_vm->function(thread.main_context()->get_fun_id()), variable, variable_index(cfun_name));
        }

		void add_global_variable(l_thread& thread,  const l_variable& g_variable, const std::string& var_name)
		{
			l_variable variable = l_string::const_new(thread.get_gc(), var_name);
			//add variable
			thread.main_context()->variable(variable, g_variable);
			//name into table
			add_into_table(&m_vm->function(thread.main_context()->get_fun_id()), variable, variable_index(var_name));
		}
        
        l_thread* compile(const l_syntactic_tree* tree)
        {
            //build tablet
            build_variable_context_from_tree(m_vm, tree);
            //push consts
            for(size_t i=0; i!=m_vm->m_functions.size(); ++i)
            {
                l_function* fun = &m_vm->function((unsigned int)i);
                //search in table
                auto it_fun_table = m_funs_table.find(fun);
                //if find
                if(it_fun_table != m_funs_table.end())
                {
                    //ptr to fun table
                    auto& fun_table = it_fun_table->second;
                    //alloc values
                    fun->m_costants.resize(fun_table.size());
                    //puth const
                    for(auto& it : fun_table)
                    {
                        //save const
                        fun->m_costants[it.second.m_id] = it.second.m_variable;
                    }
                }
            }
            //
            if(!compile_statements(m_main,tree->m_root->m_staments)) return nullptr;
            //compile statemens
            return m_vm->m_threads.back().get();
        }
    };
}
#endif