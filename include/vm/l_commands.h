//
//  l_commands.h
//  LLanguage
//
//  Created by Gabriele on 07/02/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#pragma once
#include <l_opcode.h>

namespace l_language
{
    //commands
    struct l_command
    {
        //opcode
        l_op_code m_op_code { L_NO_OP };
        //commands
        int m_arg{ 0 };
        //line
        size_t m_line{ 0 };
        //column
        size_t m_column{ 0 };
        //constractor
        l_command(l_op_code code, int arg = 0, size_t line = 0, size_t column = 0)
        {
            m_op_code = code;
            m_arg     = arg;
            m_line    = line;
            m_column  = column;
        }
    };

    //list type
    using l_list_command = std::vector < l_command >;
};