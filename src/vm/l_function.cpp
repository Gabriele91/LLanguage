//
//  l_function.cpp
//  LLanguage
//
//  Created by Gabriele on 07/02/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <l_function.h>
#include <l_compiler_utilities.h>
namespace l_language
{
    void l_function::dump_asm_function()
    {
        //constants
        const int spaces = 20;
        //variables
        size_t last = 0;
        size_t line = 0;
        
        std::cout << "function ( args:"
        << m_args_size
        << ", up value: "
        << m_up_val_size
        << ", consts: "
        << m_costants.size()
        << ", bytecode: "
        << m_commands.size()
        << " )\n\n";
        
        for(auto& cmd : m_commands)
        {
            
            if(cmd.m_line > last)
            {
                last = cmd.m_line;
                std::cout<< std::to_string(cmd.m_line) << "\t";
            }
            else
            {
                std::cout<< " \t";
            }
            
            std::cout
            <<  line++
            <<  "\t"
            <<  l_op_code_str[cmd.m_op_code];
            
            //cmd len
            size_t len = std::strlen(l_op_code_str[cmd.m_op_code]);
            for(int i=0; i< (spaces-len); ++i) std::cout << " ";
            
            // cmd value
            std::cout <<  cmd.m_arg;
            
            //print var name
            if(  cmd.m_op_code == L_GET_GLOBAL
               ||cmd.m_op_code == L_GET_LOCAL
            // ||cmd.m_op_code == L_GET_AT_VAL
               ||cmd.m_op_code == L_GET_UP_VALUE
               ||cmd.m_op_code == L_SET_GLOBAL
               ||cmd.m_op_code == L_SET_LOCAL
            // ||cmd.m_op_code == L_SET_AT_VAL
               ||cmd.m_op_code == L_SET_UP_VALUE)
            {
                std::cout << "\t;" << m_costants[cmd.m_arg].to_string();
            }
            else if ( cmd.m_op_code == L_PUSHK )
            {
                std::cout << "\t;" <<  compiler_utilities::compile_string( m_costants[cmd.m_arg].to_string(), false );
            }
           
            //end line
            std::cout << std::endl;
            
        }
    }
    
    void l_function::dump_consts_function()
    {
        size_t line = 0;
        std::string type ("unknow");
        std::string value("none");
        
        
        for(const l_variable& variable : m_costants)
        {
            switch (variable.m_type)
            {
                case l_variable::INT: type = "int"; value = std::to_string(variable.m_value.m_i); break;
                case l_variable::FLOAT: type = "float"; value = std::to_string(variable.m_value.m_f); break;
                case l_variable::STRING: type = "string"; value = variable.string()->str(); break;
                case l_variable::FUNCTION: type = "function"; value = std::to_string(variable.m_value.m_i);  break;
                case l_variable::CFUNCTION: type = "c-function"; value = std::to_string((unsigned long)variable.m_value.m_pcfun);  break;
                case l_variable::OBJECT: type = "object"; value = std::to_string((unsigned long)variable.m_value.m_pobj); break;
                    
                default: break;
            }
            
            
            std::cout
            << "const["
            << line++
            << "]: type: "
            << type
            << ", value: "
            << value
            << "\n";
        }
    }
    void l_function::dump_all_function()
    {
        
        std::cout   << "function ( args:"
        << m_args_size
        << ", up value: "
        << m_up_val_size
        << ", consts: "
        << m_costants.size()
        << ", bytecode: "
        << m_commands.size()
        << " )\n\n";
        
        
        size_t id_consts = 0;
        std::string type ("unknow");
        std::string value("none");
        
        
        for(l_variable& variable : m_costants)
        {
            switch (variable.m_type)
            {
                case l_variable::INT: type = "int"; value = std::to_string(variable.m_value.m_i); break;
                case l_variable::FLOAT: type = "float"; value = std::to_string(variable.m_value.m_f); break;
                case l_variable::STRING: type = "string"; value = variable.string()->str(); break;
                case l_variable::FUNCTION: type = "function"; value = std::to_string(variable.m_value.m_i);  break;
                case l_variable::CFUNCTION: type = "c-function"; value = std::to_string((unsigned long)variable.m_value.m_pcfun);  break;
                case l_variable::OBJECT: type = "object"; value = std::to_string((unsigned long)variable.m_value.m_pobj); break;
                    
                default: break;
            }
            
            
            std::cout
            << "const["
            << id_consts++
            << "]: type: "
            << type
            << ", value: "
            << compiler_utilities::compile_string(value,false)
            << "\n";
        }
        std::cout<< " \n";
        
        //constants
        const int spaces = 20;
        //..
        size_t last = 0;
        size_t line = 0;
        
        for(auto& cmd : m_commands)
        {
            
            if(cmd.m_line > last)
            {
                last = cmd.m_line;
                std::cout<< std::to_string(cmd.m_line) << "\t";
            }
            else
            {
                std::cout<< " \t";
            }
            
            std::cout
            <<  line++
            <<  "\t"
            <<  l_op_code_str[cmd.m_op_code];
            
            //cmd len
            size_t len = std::strlen(l_op_code_str[cmd.m_op_code]);
            for(int i=0; i< (spaces-len); ++i) std::cout << " ";
            
            //..
            std::cout  <<  cmd.m_arg;
            
            //print var name
            if(  cmd.m_op_code == L_GET_GLOBAL
               ||cmd.m_op_code == L_GET_LOCAL
            // ||cmd.m_op_code == L_GET_AT_VAL
               ||cmd.m_op_code == L_GET_UP_VALUE
               ||cmd.m_op_code == L_SET_GLOBAL
               ||cmd.m_op_code == L_SET_LOCAL
            // ||cmd.m_op_code == L_SET_AT_VAL
               ||cmd.m_op_code == L_SET_UP_VALUE)
            {
                std::cout << "\t;" << m_costants[cmd.m_arg].to_string();
            }
            else if ( cmd.m_op_code == L_PUSHK )
            {
                std::cout << "\t;" <<  compiler_utilities::compile_string( m_costants[cmd.m_arg].to_string(), false );
            }
            
            //end line
            std::cout << std::endl;
        }
    }
}