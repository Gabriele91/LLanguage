//
//  l_opcode.cpp
//  LLanguage
//
//  Created by Gabriele on 07/02/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#include <l_opcode.h>

namespace l_language
{
    //op code strs
    const char* l_op_code_str[]=
    {
        "NO_OP",
        
        "IF",        //IF .. GOTO B
        "IF0",       //IF NOT(..) GOTO B
        "IF_OR_POP", //IF .. GOTO B OR POP FROM STACK
        "IF0_OR_POP",//IF NOT(..) GOTO B OR POP FROM STACK
        "JMP",     //JMP TO A
        
        //Logic op
        "EQ",  //==
        "NEQ", //!=
        "LT",  //<
        "RT",  //>
        "LE",  //>=
        "RE",  //<=
        "AND", //&&
        "OR",  //||
        "NOT", // !
        
        //Math ops
        "ADD",     // +
        "MUL",     // *
        "SUB",     // -
        "DIV",     // /
        "MOD",     // %
        "UNM",     // -
        
        //Loads
        "PUSH_NULL",
        "PUSH_TRUE",
        "PUSH_FALSE",
        "PUSHK",
        
        //Allocs
        "NEW_ARRAY",//new array
        "NEW_TABLE",//new table
        
        //Get
        "GET_UP_VALUE",
        "GET_LOCAL",
        "GET_GLOBAL",
        "GET_AT_VAL",
        "GET_THIS",
        //Set
        "SET_UP_VALUE",
        "SET_LOCAL",
        "SET_GLOBAL",
        "SET_AT_VAL",
        "SET_THIS",
        "SET_THIS_NPOP",
        
        //for
        "IT",
        "FOR_IN",
        "FOR_OF",    //for R(A) of R(B) { commands len C }  //R(B) must to be a table or array
        
        //Move
        "PUSH",
        
        //Function
        "CLOSER",    //make function/closer
        "CALL",
        "THIS_CALL",
        "RETURN"
    };

};