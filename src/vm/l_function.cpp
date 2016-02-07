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

namespace l_language
{
    void l_function::dump_asm_function()
    {
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
            for(int i=0; i< (20-len); ++i) std::cout << " ";
            
            std::cout
            <<  cmd.m_arg
            <<  "\n";
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
            << value
            << "\n";
        }
        std::cout<< " \n";
        
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
            for(int i=0; i< (20-len); ++i) std::cout << " ";
            
            std::cout
            <<  cmd.m_arg
            <<  "\n";
        }
    }
}