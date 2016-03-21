//
//  l_opcode.h
//  LLanguage
//
//  Created by Gabriele on 07/02/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#pragma once

namespace l_language
{
    enum l_op_code
    {
        L_NO_OP,
        //IF
        L_IF,      //IF .. GOTO B
        L_IF0,     //IF NOT(..) GOTO B
        L_IF_OR_POP, //IF .. GOTO B OR POP FROM STACK
        L_IF0_OR_POP,//IF NOT(..) GOTO B OR POP FROM STACK
        //JMP
        L_JMP,     //JMP TO A
        //Logic op
        L_EQ,  //==
        L_NEQ, //!=
        L_LT,  //<
        L_RT,  //>
        L_LE,  //<=
        L_RE,  //>=
        L_AND, //&&
        L_OR,  //||
        L_NOT, // !
        //Math ops
        L_ADD,     // +
        L_MUL,     // *
        L_SUB,     // -
        L_DIV,     // /
        L_MOD,     // %
        L_UNM,     // -
        //Loads
        L_PUSH_NULL,
        L_PUSH_TRUE,
        L_PUSH_FALSE,
        L_PUSHK,
        //Allocs
        L_NEW_ARRAY,//new array
        L_NEW_TABLE,//new table
        //Get
        L_GET_UP_VALUE,
        L_GET_LOCAL,
        L_GET_GLOBAL,
        L_GET_AT_VAL,
        //Set
        L_SET_UP_VALUE,
        L_SET_LOCAL,
        L_SET_GLOBAL,
        L_SET_AT_VAL,
        //for
        L_IT,
        L_FOR_IN,
        L_FOR_OF,
        //Move
        L_PUSH,
        //Function
        L_CLOSER,    //make function/closer
        L_CALL,      //call a clouser         CALL R(A) ARGS(R(A+1....B))
        L_RETURN
    };

    //op code strs
    extern const char* l_op_code_str[];
};
