//
//  l_compiler_tree_to_vm.cpp
//  LLanguage
//
//  Created by Gabriele on 22/03/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//

#include <l_compiler_tree_to_vm.h>
#include <l_vm.h>
#include <l_class.h>

namespace l_language
{
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    bool l_compiler_tree_to_vm::compile_variable_set(l_function* fun,l_syntactic_tree::variable_node* node)
    {
        if(fun == m_main)
        {
            fun->push({ L_SET_GLOBAL, get_var_id(fun, node), node->m_line });
        }
        else
        {
            if(node->m_name == "this")
                fun->push({ L_SET_THIS, 0, node->m_line });
            else if(is_upper_value(fun,node))
                fun->push({ L_SET_UP_VALUE, get_var_id(fun, node), node->m_line });
            else if(is_global_value(fun,node))
                fun->push({ L_SET_GLOBAL, get_var_id(fun, node), node->m_line });
            else
                fun->push({ L_SET_LOCAL, get_var_id(fun, node), node->m_line });
            //assert(0);
        }
        return true;
    }
    
    bool l_compiler_tree_to_vm::compile_variable_get(l_function* fun,l_syntactic_tree::variable_node* node)
    {
        if(fun == m_main)
        {
            fun->push({ L_GET_GLOBAL, get_var_id(fun, node), node->m_line });
        }
        else
        {
            if(node->m_name == "this")
                fun->push({ L_GET_THIS, 0, node->m_line });
            else if(is_upper_value(fun,node))
                fun->push({ L_GET_UP_VALUE, get_var_id(fun, node), node->m_line });
            else if(is_global_value(fun,node))
                fun->push({ L_GET_GLOBAL, get_var_id(fun, node), node->m_line });
            else
                fun->push({ L_GET_LOCAL, get_var_id(fun, node), node->m_line });
            //assert(0);
        }
        return true;
    }
    
    bool l_compiler_tree_to_vm::compile_assignable_exp(l_function* fun,l_syntactic_tree::exp_node* node,int is_not_first)
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
        else if (node->m_type == l_syntactic_tree::FUNCTION_DEF_NODE)
        {
            return compile_function_def(fun,(l_syntactic_tree::function_def_node*)node,true);
        }
        //rec call
        compile_assignable_exp(fun,node->to_field_node()->m_assignable);
        //exp
        compile_exp(fun,node->to_field_node()->m_exp);
        //push
        if(is_not_first) fun->push({ L_GET_AT_VAL, 0, node->m_line });
        //return
        return true;
    }
    
    bool l_compiler_tree_to_vm::compile_assignable_exp_get(l_function* fun,l_syntactic_tree::exp_node* node)
    {
        //return
        return compile_assignable_exp(fun,node,true);
    }
    
    bool l_compiler_tree_to_vm::compile_assignable_exp_set(l_function* fun,l_syntactic_tree::exp_node* node)
    {
        //return
        return compile_assignable_exp(fun,node,false);
    }
    
    bool l_compiler_tree_to_vm::compile_exp_aux(l_function* fun,l_syntactic_tree::exp_node* node)
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
            if(c_node->is_push_null_or_true_or_false())
            {
                if(c_node->m_const_type == l_syntactic_tree::constant_node::CNULL)
                {
                    fun->push({ L_PUSH_NULL,  0, node->m_line });
                }
                else if( c_node->m_const_type == l_syntactic_tree::constant_node::CBOOL )
                {
                    fun->push({ c_node->m_value.m_bool ? L_PUSH_TRUE : L_PUSH_FALSE,  0, node->m_line });
                }
                else
                    assert(0);
            }
            else
                fun->push({ L_PUSHK,  get_const_id(fun, c_node), node->m_line });
            //success
            return true;
        }
        else if (node->m_type == l_syntactic_tree::FIELD_NODE ||
                 node->m_type == l_syntactic_tree::VARIABLE_NODE)
        {
            auto*  assignable_node = node->to<l_syntactic_tree::assignable_node>();
            return compile_assignable_exp_get(fun,assignable_node);
        }
        else if(node->m_type == l_syntactic_tree::FUNCTION_DEF_NODE)
        {
            //return true...
            return compile_function_def(fun, (l_syntactic_tree::function_def_node*)node, true);
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
    bool l_compiler_tree_to_vm::compile_exp(l_function* fun,l_syntactic_tree::exp_node* node)
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
    bool l_compiler_tree_to_vm::compile_op(l_function* fun,l_syntactic_tree::op_node* node)
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
            return compile_variable_set(fun,node->m_assignable->to<l_syntactic_tree::variable_node>());
        }
        else
        {
            if(!compile_assignable_exp_set(fun,node->m_assignable)) return false;
            //is (+|-|*|/)=
            if(opcode != L_NO_OP)
            {
                //get <var>
                if(!compile_assignable_exp_get(fun,node->m_assignable)) return false;
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
    
    bool l_compiler_tree_to_vm::compile_while(l_function* fun,l_syntactic_tree::while_node* node)
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
    
    bool l_compiler_tree_to_vm::compile_if_else(l_function* fun,l_syntactic_tree::if_node* ifs_node)
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
    
    bool l_compiler_tree_to_vm::compile_for(l_function* fun,l_syntactic_tree::for_node* for_node)
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
            case l_syntactic_tree::for_node::FOR_C:
            {
                //compile left values
                for(auto& node : for_node->m_ops_left)
                {
                    compile_op(fun, node);
                }
                //get line
                int jmp_cmd_line = (int)fun->m_commands.size();
                //ptr to for
                size_t for_if = 0;
                //have a exp?
                if(for_node->m_exp)
                {
                    //push exp
                    compile_exp(fun, for_node->m_exp);
                    //jmp to end
                    for_if = fun->push({ L_IF0, 0, for_node->m_line });
                }
                //statement
                compile_statements(fun, for_node->m_staments);
                //compile right values
                for(auto& node : for_node->m_ops_right)
                {
                    compile_op(fun, node);
                }
                //.. jmp
                fun->push({ L_JMP, jmp_cmd_line,  for_node->m_line});
                //set jump at the end
                if(for_node->m_exp)
                {
                    fun->command( for_if ).m_arg = (int)fun->m_commands.size();
                }
            }
                break;
            default: assert(0); break;
        }
        return true;
    }
    //compile call
    bool l_compiler_tree_to_vm::compile_call(l_function* fun,l_syntactic_tree::call_node* call_node)
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
        //push function
        compile_assignable_exp_get(fun,call_node->m_exp_to_call);
        //is this call?
        if(call_node->m_exp_to_call->m_type == l_syntactic_tree::FIELD_NODE)
        {
            //push call this
            fun->push({ L_THIS_CALL, n_args, call_node->m_line });
        }
        else
        {
            //push call
            fun->push({ L_CALL, n_args, call_node->m_line });
        }
        //success
        return true;
    }
    //compile fun
    bool l_compiler_tree_to_vm::compile_function_def(l_function* fun,
                                                     l_syntactic_tree::function_def_node* function_def_node,
                                                     bool is_exp)
    {
        //get function id
        size_t id_function = ((size_t)function_def_node->m_data);
        //compile staments
        if(!compile_statements(&m_vm->function((unsigned int)id_function),function_def_node->m_staments))
        {
            return false;
        }
        //get fun id
        fun->push({ L_CLOSER, get_function_id(fun, function_def_node), function_def_node->m_line });
        //push call
        if(function_def_node->m_variable)
        {
            //push value into variable def
            compile_variable_set(fun,function_def_node->m_variable);
            //get value for the exp
            if(is_exp)
            {
                compile_variable_get(fun,function_def_node->m_variable);
            }
        }
        else if(!is_exp)
        {
            //not assignment
            return false;
        }
        //success
        return true;
    }
    
    
    //compile class
    bool l_compiler_tree_to_vm::compile_class_def(l_function* fun,
                                                  l_syntactic_tree::class_node* class_node,
                                                  bool is_exp)
    {
        //create class
        fun->push({ L_START_CLASS_DEC,  0, class_node->m_line });
        //add super class
        for(auto& parent: class_node->m_parents)
        {
            compile_variable_get(fun, parent);
        }
        if(class_node->m_parents.size())
        {
            fun->push({ L_CLASS_PARENT,  (int)class_node->m_parents.size(), class_node->m_parents[0]->m_line });
        }
        //add variables
        for(auto& attr : class_node->m_attrs)
        {
            //add exp
            if(attr.m_exp) compile_exp(fun, attr.m_exp);
            else           fun->push({ L_PUSH_NULL,  0, attr.m_var->m_line });
            //add var
            fun->push({ L_PUSHK, get_var_id(fun,attr.m_var), attr.m_var->m_line });
            //type
            fun->push({ L_CLASS_ATTR,  (int)attr.m_type, attr.m_var->m_line });
        }
        //add methods
        for(auto& defs : class_node->m_defs)
        {
            //..
            l_syntactic_tree::function_def_node* method = defs.m_method;
            //compile
            //get function id
            size_t id_function = ((size_t)method->m_data);
            //compile staments
            if (!compile_statements(&m_vm->function((unsigned int)id_function), method->m_staments))
            {
                return false;
            }
            //get closer
            fun->push({ L_CLOSER, get_method_id(fun, class_node, method), method->m_line });
            //add var
            fun->push({ L_PUSHK, get_var_id(fun,method->m_variable), method->m_variable->m_line });
            //method
            fun->push({ L_CLASS_METHOD,  defs.m_type, defs.m_method->m_line });
        }
        //add op
        for(auto& defs : class_node->m_ops)
        {
            //..
            l_syntactic_tree::function_def_node* method = defs.m_method;
            //compile
            //get function id
            size_t id_function = ((size_t)method->m_data);
            //compile staments
            if (!compile_statements(&m_vm->function((unsigned int)id_function), method->m_staments))
            {
                return false;
            }
            //get closer
            fun->push({ L_CLOSER, get_method_id(fun, class_node, method), method->m_line });
            //add var
            fun->push({ L_PUSH_INT, (int)l_class::get_op_id(defs.m_method->m_variable->m_name), method->m_variable->m_line });
            //method
            fun->push({ L_CLASS_OP,  defs.m_type, defs.m_method->m_line });
        }
        //create class object
        fun->push({ L_END_CLASS_DEC,   get_var_id(fun,class_node->m_class_name), class_node->m_line });
        //push value into variable def
        compile_variable_set(fun,class_node->m_class_name);
        //..true
        return true;
    }
    //compile return
    bool l_compiler_tree_to_vm::compile_return(l_function* fun,l_syntactic_tree::return_node* return_node)
    {
        //compile exp
        if(return_node->m_exp)
        {
            if(!compile_exp(fun, return_node->m_exp)) return false;
        }
        //get fun id
        fun->push({ L_RETURN, return_node->m_exp ? 1:0, return_node->m_line });
        //success
        return true;
    }
    //compile context type
    bool l_compiler_tree_to_vm::compile_context_type(l_function* fun,l_syntactic_tree::context_type_node* context_type_node)
    {
        //if(context_type_node->is_op())
        for (auto& op_node :  context_type_node->m_ops)
        {
            if(op_node->is(l_syntactic_tree::OP_NODE))
            {
                if(!compile_op(fun, op_node)) return false;
            }
            else return false;
        }
        //else ignore...
        return true;
    }
    
    //compile a statements
    bool l_compiler_tree_to_vm::compile_statements(l_function* fun,l_syntactic_tree::list_nodes& nodes)
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
                case l_syntactic_tree::CONTEXT_TYPE_NODE:
                    if(! compile_context_type(fun,node->to<l_syntactic_tree::context_type_node>()) ) return false;
                    break;
                case l_syntactic_tree::CLASS_NODE:
                    if(! compile_class_def(fun,node->to<l_syntactic_tree::class_node>()) ) return false;
                    break;
                default:  break;
            }
        }
        return true;
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void l_compiler_tree_to_vm::set_thread(l_thread* thread)
    {
        //ptr to vm
        m_vm = thread->get_vm();
        //set thread
        m_thread = thread;
        //set main
        set_main_function(&m_vm->function(thread->m_main_fun_id));
        //set gc
        set_vm(thread->get_vm());
    }
    
    void l_compiler_tree_to_vm::add_global_variable(l_thread& thread,  const l_variable& g_variable, const std::string& var_name)
    {
        l_variable variable = l_string::const_new(thread.get_gc(), var_name);
        //add variable
        thread.main_context()->variable(variable, g_variable);
        //name into table
        add_into_table(&m_vm->function(thread.main_context()->get_fun_id()), variable, variable_index(var_name));
    }
    
    l_thread* l_compiler_tree_to_vm::compile(const l_syntactic_tree* tree)
    {
        //build tablet
        build_variable_context_from_tree(m_vm, tree);
        //push consts
        for(size_t i=0; i!=m_vm->get_count_of_functions(); ++i)
        {
            //get function
            l_function* fun = &m_vm->function((unsigned int)i);
            //olready added?
            if(fun->m_costants.size()) continue;
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
        return m_thread;
    }
    
    l_function_id l_compiler_tree_to_vm::compile_function(const l_syntactic_tree* tree)
    {
        //build new function
        l_function_id new_function_id = m_vm->get_new_function_id();
        l_function&   new_function    = m_vm->function(new_function_id);
        //build tablet
        build_variable_context_from_tree(m_vm, &new_function, tree);
        //push consts
        {
            //search in table
            auto it_fun_table = m_funs_table.find(&new_function);
            //if find
            if(it_fun_table != m_funs_table.end())
            {
                //ptr to fun table
                auto& fun_table = it_fun_table->second;
                //alloc values
                new_function.m_costants.resize(fun_table.size());
                //puth const
                for(auto& it : fun_table)
                {
                    //save const
                    new_function.m_costants[it.second.m_id] = it.second.m_variable;
                }
            }
        }
        //..
        compile_statements(&new_function,tree->m_root->m_staments);
        //compile statemens
        return new_function_id;
    }
}
