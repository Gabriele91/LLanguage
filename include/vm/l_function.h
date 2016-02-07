//
//  l_function.h
//  LLanguage
//
//  Created by Gabriele on 07/02/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#pragma once
#include <vector>
#include <l_opcode.h>
#include <l_variable.h>
#include <l_commands.h>

namespace l_language
{
    //function
    struct l_function
    {
        //allocs
        unsigned int m_up_val_size { 0 };
        unsigned int m_values_size { 0 };
        unsigned int m_args_size   { 0 };
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
        //function dump
        void dump_asm_function();
        void dump_consts_function();
        void dump_all_function();
    };
    //list functions
    using l_list_function = std::vector < l_function >;
};