//
//  compile_tree_to_cpp.h
//  LLanguage
//
//  Created by Gabriele Di Bari on 12/12/15.
//  Copyright © 2015 Gabriele Di Bari. All rights reserved.
//
#pragma once

#include <string>
#include <list>
#include <assert.h>
#include <syntactic_tree.h>
#include <compiler_utilities.h>

namespace l_language
{
	const char* code_cpp_header = 
	#include <backend_cpp_header.h>
	;

	class compile_tree_to_cpp
	{
	public:

		std::list < std::string > m_errors;
		std::string m_cpp_code;

		const std::string& compile_variable(syntactic_tree::variable_node* node)
		{
			return node->m_name;
		}

		std::string compile_assignable(syntactic_tree::assignable_node* node)
		{
			//return variable
			if (node->m_type == syntactic_tree::VARIABLE_NODE) return compile_variable(node->to_variable_node());
			//else
			//return compile_assignable(node->to_field_node()->m_assignable) + "[" + compile_exp(node->to_field_node()->m_exp) + "]";
			return 
				"get_field_vector("
				+compile_assignable(node->to_field_node()->m_assignable) 
				+","
				+compile_exp(node->to_field_node()->m_exp) 
				+")";
		}

		std::string compile_exp(syntactic_tree::exp_node* node)
		{
			if (node->m_type == syntactic_tree::EXP_NODE)
            {
                if (node->is_link())
                {
                    return "(" + compile_exp(node->m_left) + ")";
                }
                if (node->is_one())
                {
                    return node->m_name + "(" + compile_exp(node->m_left) + ")";
                }
                else
                {
                    return "(" + compile_exp(node->m_left) + ")" + node->m_name + "(" + compile_exp(node->m_right) + ")";
                }
			}
			if (node->m_type == syntactic_tree::CONSTANT_NODE)
			{
				syntactic_tree::constant_node* c_node = node->to<syntactic_tree::constant_node>();
				switch (c_node->m_const_type)
				{
				case syntactic_tree::constant_node::FLOAT:  return "it_variable(" + std::to_string(c_node->m_value.m_float) + "f)";
				case syntactic_tree::constant_node::INT:    return "it_variable(" + std::to_string(c_node->m_value.m_int) + ")";
				case syntactic_tree::constant_node::STRING: return "it_variable(" + compiler_utilities::compile_string(c_node->m_value.m_string) + ")";
				default: assert(0); break;
				}
			}
			if (node->m_type == syntactic_tree::CALL_NODE)
			{
				return compile_call((syntactic_tree::call_node*)node,false);
			}
			if (node->m_type == syntactic_tree::FIELD_NODE ||
				node->m_type == syntactic_tree::VARIABLE_NODE)
			{
				auto*  assignable_node = node->to<syntactic_tree::assignable_node>();
				return compile_assignable(assignable_node);
            }
            if(node->m_type == syntactic_tree::ARRAY_NODE)
            {
                auto* array_node = node->to<syntactic_tree::array_node>();
                //create array declaretion
                std::string output = "new_array(";
                for(auto& node_exp:array_node->m_exps) output += compile_exp(node_exp)+",";
                output += ")";
                //return
                return output;
            }
			m_errors.push_back(std::to_string(node->m_line) + ":" + std::to_string(node->m_char) + ":invalid expression");
            //return void string
            return "";
		}

		std::string compile_op(syntactic_tree::op_node* node)
		{
			return compile_assignable(node->m_assignable) + " = " + compile_exp(node->m_exp) + ";\n";
		}

		std::string compile_call(syntactic_tree::call_node* node,bool end_line = true)
		{
			//args to string..
			std::string ouput_args;
			for (size_t i = 0; i != node->m_args.size(); ++i)
			{
				ouput_args += compile_exp(node->m_args[i]) +( i < node->m_args.size()-1 ?",":"" );
			}
			//call
            return compile_assignable(node->m_assignable) + "(" + ouput_args + ")" + (end_line ? "; \n" : "");
		}
        
        std::string compile_while(syntactic_tree::while_node* node)
        {
            return "while("+compile_exp(node->m_exp)+")\n"
            "{\n" + compile_statements(node->m_staments) + "}\n";
        }
        
        std::string compile_for(syntactic_tree::for_node* node)
        {
            switch (node->m_type_for)
            {
                    
                case syntactic_tree::for_node::FOR_IN:
                    return "for("+compile_assignable(node->m_variable_left)+" in "+compile_assignable(node->m_variable_right)+")\n"
                    "{\n" + compile_statements(node->m_staments) + "}\n";
                    
                case syntactic_tree::for_node::FOR_OF:
                    return "for("+compile_assignable(node->m_variable_left)+" of "+compile_assignable(node->m_variable_right)+")\n"
                    "{\n" + compile_statements(node->m_staments) + "}\n";
                    
                default: return "";
            }
            
        }
        
        std::string compile_if_else(syntactic_tree::if_node* node)
        {
            //output
            std::string output =  "if (" + compile_exp(node->get_if_exp()) + ")\n"
            "{\n" + compile_statements(node->get_if_subnode_list()) + "}\n";
            //else if
            if(node->have_else_if())
                for(int i=1; i != node->m_ifs.size(); ++i )
                {
                    output+= "else if (" + compile_exp(node->get_else_if_exp(i)) + ")\n"
                    "{\n" + compile_statements(node->get_else_if_subnode_list(i)) + "}\n";
                }
            
            //else
            if(node->have_else())
                output += "else\n{\n"+ compile_statements(node->m_else_staments) +"}\n";
            
            //output...
            return output;
        }

		std::string compile_statements(syntactic_tree::list_nodes& nodes)
		{
			std::string output;
			for (syntactic_tree::node* node : nodes)
			{
				switch (node->m_type)
                {
                    case syntactic_tree::OP_NODE:     output += compile_op(node->to<syntactic_tree::op_node>()); break;
                    case syntactic_tree::CALL_NODE:   output += compile_call(node->to<syntactic_tree::call_node>()); break;
                    case syntactic_tree::IF_NODE:     output += compile_if_else(node->to<syntactic_tree::if_node>()); break;
                    case syntactic_tree::WHILE_NODE:  output += compile_while(node->to<syntactic_tree::while_node>()); break;
                    case syntactic_tree::FOR_NODE:    output += compile_for(node->to<syntactic_tree::for_node>()); break;
				default:
					m_errors.push_back(std::to_string(node->m_line) + ":" + std::to_string(node->m_char) + ":invalid statement");
					break;
				}
			}
			return output;
		}

		compile_tree_to_cpp& compile(const syntactic_tree* tree)
		{
			m_cpp_code = std::string(code_cpp_header)+"\nint main()\n{\n" + compile_statements(tree->m_root->m_staments) + "return 0;\n}\n";
			return *this;
		}

	};
};