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
#include <l_variable.h>
#include <l_syntactic_tree.h>

namespace l_language
{
    #define TABLE_GET_ERROR 0xFFFFF
    class l_vm;
    class l_gc;
    class l_function;
    //..
    class l_variables_context
    {
        
    protected:
        
        //main context function
        l_function* m_main {  nullptr  };
        //gc
        l_gc*       m_gc {  nullptr  };
        //vm
        l_vm*       m_vm {  nullptr  };
        //const struct
        struct const_info
        {
            //type
            enum type_context
            {
                T_CTX_CAN_TO_BE_UPPER,
                T_CTX_CAN_NOT_TO_BE_UPPER,
                T_CTX_MUST_TO_BE_UPPER,
                T_CTX_MUST_TO_BE_GLOBAL
            };
            type_context m_context_type{ T_CTX_CAN_TO_BE_UPPER };
            //info
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
                        case l_syntactic_tree::constant_node::CNULL:
                            m_variable=l_variable();
                        break;
                        case l_syntactic_tree::constant_node::CBOOL:
                            m_variable=l_variable(m_constant_node->m_value.m_bool);
                        break;
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
                m_variable = variable;
            }
        };        //function context, value map
        struct function_table
        {
            //type
            using table = std::map< std::string, const_info > ;
            //field
            table m_table;
            //id generators
            unsigned int m_var_gen_id { 0 };
            //utility methos
            unsigned int get_new_var_id()
            {
                return m_var_gen_id++;
            }
            //operators
            const_info& operator[](const std::string& str)
            {
                return m_table[str];
            }
            
            size_t size() const
            {
                return m_table.size();
            }
            //iterators
            table::iterator begin()
            {
                return m_table.begin();
            }
            table::const_iterator begin() const
            {
                return m_table.begin();
            }
            table::iterator end()
            {
                return m_table.end();
            }
            table::const_iterator end() const
            {
                return m_table.end();
            }
            //find
            table::iterator find(const std::string& str)
            {
                return m_table.find(str);
            }
            table::const_iterator find(const std::string& str) const
            {
                return m_table.find(str);
            }
        };
        //variable map
        using functions_var_table = std::map< l_function*, function_table >;
        //map
        functions_var_table m_funs_table;
        //id generators
        unsigned int m_gen_id { 0 };
        //new id
        unsigned int get_new_id(l_function* fun)
        {
            return m_funs_table[fun].get_new_var_id();
        }
        
        //const to index
        std::string const_index(l_syntactic_tree::constant_node* const_value)
        {
            //name node
            std::string c_name;
            //name
            switch (const_value->m_const_type)
            {
                case l_syntactic_tree::constant_node::CNULL:  c_name = "null:null"; break;
                case l_syntactic_tree::constant_node::CBOOL:  c_name = "bool:"+std::string(const_value->m_value.m_bool ? "true" : "false"); break;
                case l_syntactic_tree::constant_node::FLOAT:  c_name = "float:"+std::to_string(const_value->m_value.m_float); break;
                case l_syntactic_tree::constant_node::INT:    c_name = "int:"+std::to_string(const_value->m_value.m_int); break;
                case l_syntactic_tree::constant_node::STRING: c_name = "string:"+const_value->m_value.m_string; break;
                default: assert(0); break;
            }
            
            return c_name;
        }
        
        std::string function_index(const std::string& name)
        {
            return "function:"+name;
        }
        
        std::string function_index(l_syntactic_tree::function_def_node* fdef_node)
        {
            return "function:"+fdef_node->m_variable->m_name;
        }

		std::string method_index(l_syntactic_tree::class_node* name,
								 l_syntactic_tree::function_def_node* fdef_node)
		{
			return "method:" + name->get_class_name() +":"+ fdef_node->m_variable->m_name;
		}

		std::string class_index(l_syntactic_tree::class_node* fclass_node)
		{
			return "class:" + fclass_node->m_class_name->m_name;
		}
        
        std::string variable_index(const std::string& name)
        {
            return "string:"+name;
        }
        
        std::string variable_index(l_syntactic_tree::variable_node* var_node)
        {
            return "string:"+var_node->m_name;
        }
        
        
        void visit(l_function* fun, l_syntactic_tree::node* node);

		//function def
		void visit(l_function* fun, l_syntactic_tree::function_def_node* node);

		//method def
		void visit(l_function* fun, l_syntactic_tree::class_node*		 c_node,
									l_syntactic_tree::function_def_node* m_node,
                                    bool is_operator_def=false);
        
        //assignable
        void visit(l_function* fun,l_syntactic_tree::variable_node* node);
        
        void visit(l_function* fun,l_syntactic_tree::assignable_node* node);
        
        //call
        void visit(l_function*  fun, l_syntactic_tree::call_node* call_node);
        
        //exp
        void visit(l_function* fun, l_syntactic_tree::exp_node* exp_node);
        //is dec?
        bool is_declared(l_function* fun, l_syntactic_tree::variable_node* variable_node);
        //isn't an up value
        void is_not_upper_value(l_function* fun, l_syntactic_tree::variable_node* variable_node);
        void must_to_be_upper_value(l_function* fun, l_syntactic_tree::variable_node* variable_node);
        void must_to_be_global_value(l_function* fun, l_syntactic_tree::variable_node* variable_node);
        
        //op
        void visit(l_function* fun, l_syntactic_tree::op_node* op_node);
        
        //type context
        void visit(l_function*  fun, l_syntactic_tree::context_type_node* ctx_node);
        
        //class
        void visit(l_function*  fun, l_syntactic_tree::class_node* class_node);
        
        //for
        void visit(l_function* fun, l_syntactic_tree::for_node* for_node);
        
        //while
        void visit(l_function* fun, l_syntactic_tree::while_node* while_node);
        
        //return
        void visit(l_function* fun, l_syntactic_tree::return_node* return_node);
        
        //if
        void visit(l_function* fun, l_syntactic_tree::if_node* if_node);
        
        //root
        void visit(l_function* fun, l_syntactic_tree::root_node* root);
        
        
    public:
        
        void build_variable_context_from_tree(l_vm* vm,
                                              const l_syntactic_tree* tree);
        void build_variable_context_from_tree(l_vm* vm,
                                              l_function* function,
                                              const l_syntactic_tree* tree);
        
        void set_vm(l_vm* vm);
        
        void set_main_function(l_function* main);
        
        void set_gc_from_vm(l_vm* vm);
        
        //push variable
        void add_variable_into_table(l_function* f_context, l_syntactic_tree::variable_node* var_value);
        
        //push constant
        void add_const_into_table(l_function* f_context, l_syntactic_tree::constant_node* const_value);
        
        //push constant
        void add_into_table(l_function* f_context, const l_variable& variable, const std::string& const_name);
        
        //get
        int get_var_id(l_function* f_context,l_syntactic_tree::variable_node* node);
        
        int get_const_id(l_function* f_context,l_syntactic_tree::constant_node* c_node);

		int get_function_id(l_function* f_context, l_syntactic_tree::function_def_node* c_node);

		int get_method_id(l_function* f_context, l_syntactic_tree::class_node* c_node, l_syntactic_tree::function_def_node* m_node);

        int get_class_id(l_function* f_context,l_syntactic_tree::class_node* c_node);
        
        bool is_upper_value(l_function* f_context,l_syntactic_tree::variable_node* node);
        bool is_global_value(l_function* f_context,l_syntactic_tree::variable_node* node);
        int get_table_id(l_function* f_context,const std::string& name);
        
    };
};