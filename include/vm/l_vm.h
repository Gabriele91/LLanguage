//
//  l_vm.h
//  LLanguage
//
//  Created by Gabriele on 09/01/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#pragma once

#include <vector>
#include <stack>
#include <l_variable.h>
#include <l_gc.h>

namespace l_language
{
    enum l_op_code
    {
        L_NO_OP,
        //IF
        L_IF,      //IF .. GOTO B
        L_IF0,     //IF NOT(..) GOTO B
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
        L_PUSHK,
        //Allocs
        L_NEW_ARRAY,//new array
        L_NEW_TABLE,//new table
        //Get
        L_GET_UP_VAL,
        L_GET_GLOBAL,
        L_GET_AT_VAL,
        //Set
        L_SET_UP_VAL,
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
        L_CALL       //call a clouser         CALL R(A) ARGS(R(A+1....B))
    };
    
    //op code strs
    extern const char* l_op_code_str[];
    
    //list variables
    using l_list_variables = std::vector < l_variable >;
    
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
    
    //context list
    struct l_call_context
    {
    
        //auto alloc
        void alloc(const l_list_function& list)
        {
            m_vars.resize(list[m_fun_id].m_values_size);
            m_up_values.resize(list[m_fun_id].m_up_val_size);
        }
        void alloc(unsigned int fun_id,const l_list_function& list)
        {
            m_fun_id = fun_id;
            m_vars.resize(list[m_fun_id].m_values_size);
            m_up_values.resize(list[m_fun_id].m_up_val_size);
        }
        //get/set
        l_variable& up_value(int i)
        {
            return m_up_values[i];
        }
        
        l_variable& variable(int i)
        {
            return m_vars[i];
        }
        
        unsigned int get_fun_id()
        {
            return m_fun_id;
        }
        
        size_t get_size_up_values() const
        {
            return m_up_values.size();
        }
        
        size_t get_size_vars() const
        {
            return m_vars.size();
        }
        
    protected:
        //call fun id
        unsigned int m_fun_id { 0 };
        //upvalue
        l_list_variables m_up_values;
        //context
        l_list_variables m_vars;
    };
    
    //list contexts
    using l_list_call_function = std::vector < l_call_context >;
    
    //thread
    struct l_thread
    {
        //main function id
        unsigned int m_main_fun_id { 0 };
        //program counter
        unsigned int m_pc { 0 };
        //current index
        long m_top{ -1 };
        //temp variable
        l_variable m_register[3];
        //save vm
        l_vm* m_vm;
        //contexts
        l_list_call_function m_contexts;
        //init (thread)
        l_thread(l_vm* vm = nullptr,
                 unsigned int main_id = 0,
                 size_t stack = 512);
        //access
        l_variable& global(int i);
        //stack
        l_list_variables m_stack;
        //get top
        l_variable& top()
        {
            return m_stack[m_top];
        }
        //get a register
        l_variable& register3(size_t i)
        {
            return m_register[i];
        }
        //push return
        void push_return(const l_variable& var,size_t nargs)
        {
            if(nargs == 0) push(var);
            else value(nargs-1) = var;
        }
        //get top
        l_variable& value(size_t x)
        {
            return m_stack[m_top-x];
        }
        //pop
        l_variable& pop()
        {
            return m_stack[m_top--];
        }
        //push
        void push(const l_variable& value)
        {
            m_stack[++m_top] = value;
        }
        void push()
        {
            ++m_top;
        }
        //size
        size_t size() const
        {
            return m_top + 1;
        }
        //capacity
        size_t capacity() const
        {
            return m_stack.size();
        }
        //real capacity
        size_t real_capacity() const
        {
            return m_stack.capacity();
        }
        //stack size
        void stack_size(size_t size)
        {
            m_stack.resize(size);
        }
        
    };
    
    //list threads
    using l_list_threads = std::vector < l_thread >;
    
    class l_vm
    {
        //gc
        l_gc m_gc;
        
    public:
        //init vm
        l_vm()
        :m_gc(*this)
        {
        
        }
        //global modules
        l_list_variables m_globals;
        //thread list
        l_list_threads  m_threads;
        //functions
        l_list_function m_functions;
        //get gc
        l_gc& get_gc()
        {
            return m_gc;
        }
        const l_gc& get_gc() const
        {
            return m_gc;
        }
        //execute
        void execute(unsigned int id_thread);
        void execute(l_thread* this_thread);
        //
    };
}
