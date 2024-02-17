//
//  l_function.h
//  LLanguage
//
//  Created by Gabriele on 07/02/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#pragma once
#include <vector>
#include <memory>
#include <cstring>
#include <l_opcode.h>
#include <l_variable.h>
#include <l_commands.h>

namespace l_language
{
    //function
    class l_function
    {
	public:
        //function type
        enum l_type
        {
            IS_FUNCTION,
			IS_METHOD
        };
        //type of function
        l_type       m_type         { IS_FUNCTION };
        //allocs
        unsigned int m_up_val_size { 0 };
        unsigned int m_values_size { 0 };
        unsigned int m_args_size   { 0 };
        //args list?
        bool m_have_args_list{ false };
        //costant list
        l_list_variables m_costants;
        //commands
        l_list_command m_commands;
        //push command
        size_t push(const l_command& command)
        {
            m_commands.push_back(command);
            return m_commands.size()-1;
        }
        //operator
        l_command& command(size_t i)
        {
            return m_commands[i];
        }
        l_command& last_command()
        {
            return m_commands.back();
        }
        //push costant
        size_t push_costant(const l_variable& costants)
        {
            m_costants.push_back(costants);
            return m_costants.size();
        }
        //operator
        l_variable& constant(size_t i)
        {
            return m_costants[i];
        }
        //get size commands
        size_t size_commands() const
        {
            return m_commands.size();
        }
        //get number of costants
        size_t size_costants() const
        {
            return m_costants.size();
        }
        //function dump
        void dump_asm_function();
        void dump_consts_function();
        void dump_all_function();
    };
    //list functions
    using l_list_functions     = std::vector < l_function >;
    using l_list_functions_ref = std::vector < std::unique_ptr< l_function > >;
};
