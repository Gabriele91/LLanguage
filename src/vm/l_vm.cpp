//
//  vm.cpp
//  LLanguage
//
//  Created by Gabriele on 16/01/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//

#include <stdio.h>
#include <l_vm.h>
#include <l_gc.h>
#include <l_vector.h>
#include <l_table.h>
#include <iostream>

namespace l_language
{
    
    
    void l_call_context::init(unsigned int fun_id,l_thread *thread)
    {
        //save pointer
        m_fun_id = fun_id;
        //save thread
        m_thread = thread;
    }
    
    l_thread::l_thread(l_vm* vm ,
                       unsigned int main_id,
                       size_t stack)
    {
        //init vm
        m_vm = vm;
        //set stack
        stack_size(stack);
        //set main id
        m_main_fun_id = main_id;
        //main context
        m_contexts.resize(1);
        //...
        if(m_vm)
        {
            //main alloc
            m_contexts[0].init(m_main_fun_id,this);
        }
    }
    
    //get vm
    l_vm* l_thread::get_vm()
    {
        return m_vm;
    }
    //get gc
    l_gc* l_thread::get_gc()
    {
        return m_vm ? &(m_vm->get_gc()) : nullptr;
    }
    
    l_variable& l_thread::global(int i)
    {
        l_call_context& main = m_contexts[0];
        l_variable& id = m_vm->m_functions[main.m_fun_id].m_costants[i];
        return main.variable(id);
    }
    
    //alloc vm
    l_vm::l_vm()
    {
        m_gc = new l_gc(*this);
    }
    //dealloc vm
    l_vm::~l_vm()
    {
        delete m_gc;
    }
    
    
    //get gc
    l_gc& l_vm::get_gc()
    {
        return *m_gc;
    }
    const l_gc& l_vm::get_gc() const
    {
        return *m_gc;
    }
    //
    
    void l_vm::execute(unsigned int id_thread)
    {
        execute(&m_threads[id_thread]);
    }
    
    void l_vm::execute(l_thread* ptr_thread)
    {
        //get thread
        l_thread& thread = *ptr_thread;
        //current function
        unsigned int function_id = thread.m_main_fun_id;
        //main function
        l_function& main_fun = m_functions[function_id];
        //pc...
        unsigned int&    pc          =  thread.m_pc;
        l_list_command&  commands    =  main_fun.m_commands;
        //macro
        #define vconst(c)  m_functions[function_id].m_costants[c]
        #define stack      thread.value
        #define push       thread.push
        #define top        thread.top
        #define top_size   (thread.m_top+1)
        #define pop        thread.pop
        #define global     thread.global
        #define register3  thread.register3
        //for all commands
        for (pc = 0; pc < commands.size(); ++pc)
        {
            //current command
            l_command& cmp = commands[pc];
            //opcodes
            switch (cmp.m_op_code)
            {
                case L_JMP: pc = cmp.m_arg - 1; break;
                    
                
                case L_IF:
                    if(top().is_true())
                    {
                        pc = cmp.m_arg - 1;
                    }
                    pop();
                break;
                    
                case L_IF0:
                    if(top().is_false())
                    {
                        pc = cmp.m_arg - 1;
                    }
                    pop();
                break;
                    
                case L_PUSH:     push( stack(cmp.m_arg) ); break;
                case L_PUSHK:    push( vconst(cmp.m_arg) ); break;
                ////////////////////////////////////////////////////////////
                case L_ADD:
                    stack(1) = stack(1) + stack(0);
                    pop();
                break;
                    
                case L_MUL:
                    stack(1) = stack(1) * stack(0);
                    pop();
                break;
                    
                case L_SUB:
                    stack(1) = stack(1) - stack(0);
                    pop();
                break;
                    
                case L_DIV:
                    stack(1) = stack(1) / stack(0);
                    pop();
                break;
                    
                case L_UNM:
                    stack(0) = -stack(0);
                break;
                ////////////////////////////////////////////////////////////
                case L_EQ:
                    stack(1) = stack(1) == stack(0);
                    pop();
                break;
                    
                case L_NEQ:
                    stack(1) = stack(1) != stack(0);
                    pop();
                break;
                
                case L_RT:
                    stack(1) = stack(1) > stack(0);
                    pop();
                break;
                    
                case L_RE:
                    stack(1) = stack(1) >= stack(0);
                    pop();
                break;
                
                case L_LT:
                    stack(1) = stack(1) < stack(0);
                    pop();
                break;
                    
                case L_LE:
                    stack(1) = stack(1) <= stack(0);
                    pop();
                break;
                    
                case L_NOT:
                    stack(0) = !stack(0);
                break;
                ////////////////////////////////////////////////////////////
                case L_OR:
                    stack(1) = stack(1) || stack(0);
                    pop();
                break;
                    
                case L_AND:
                    stack(1) = stack(1) && stack(0);
                    pop();
                break;
                
                ////////////////////////////////////////////////////////////
                case L_GET_GLOBAL:
                    push( global(cmp.m_arg) );
                break;
                
                case L_SET_GLOBAL:
                    global(cmp.m_arg) = pop();
                break;
                ////////////////////////////////////////////////////////////
                case L_NEW_ARRAY:
                {
                    register3(0) = l_vector::gc_new(get_gc());
                    //init ?
                    if( cmp.m_arg > 0 )
                    {
                        //get object
                        l_obj* this_obj = (l_obj*)register3(0).m_value.m_pobj;
                        //types
                        l_vector* vector = dynamic_cast< l_vector* > ( this_obj );
                        //put stack into vector
                        for(int i = cmp.m_arg-1; i >= 0; --i)
                        {
                            vector->operator[](i) = pop();
                        }
                    }
                    //push array (n.b. gc run...)
                    push( register3(0) );
                }
                break;
                //alloc tablet
                case L_NEW_TABLE:
                {
                    register3(0) = l_table::gc_new(get_gc());
                    //init ?
                    if( cmp.m_arg > 0 )
                    {
                        //get object
                        l_obj* this_obj = (l_obj*)register3(0).m_value.m_pobj;
                        //types
                        l_table* table = dynamic_cast< l_table* > ( this_obj );
                        //assert
                        assert(!(cmp.m_arg % 2));
                        //put stack into vector
                        for(int i = (cmp.m_arg)-1; i >= 0; i-=2)
                        {
                            //push key and value
                            table->operator[](stack(1)) = stack(0);
							//pop value
							pop();
                            //pop key
                            pop();
                        }
                    }
                    //push table (n.b. gc run...)
                    push( register3(0) );
                }
                break;
                
                case L_GET_AT_VAL:
                {
                    const l_variable& r_b = stack(1);
                    const l_variable& r_c = stack(0);
                    //try
                    if ( r_b.m_type == l_variable::type::OBJECT )
                    {
                        //get object
                        l_obj* this_obj = (l_obj*)r_b.m_value.m_pobj;
                        //types
                        l_vector* vector = dynamic_cast< l_vector* > ( this_obj );
                        //types
                        l_table* table = dynamic_cast< l_table* > ( this_obj );
                        //is a vector
                        if(vector)
                        {
                            //to size int
                            size_t index = 0;
                            //cast
                                 if( r_c.m_type == l_variable::INT )  index= (size_t)r_c.m_value.m_i;
                            else if( r_c.m_type == l_variable::FLOAT )index= (size_t)r_c.m_value.m_f;
                            else assert(0);
                            //get
                            stack(1) = vector->operator[](index) ;
                        }
                        else if(table)
                        {
                            //get and pop value
                            stack(1) = table->operator[](r_c);
                        }
                        else
                        {
                            assert(0);
                        }
                    }
                    else
                    {
                        stack(1) = l_variable();
                    }
                    //pop index
                    pop();
                }
                break;
                case L_SET_AT_VAL:
                {
                    //get table/array
                    const l_variable& r_a = stack(2);
                    //get index
                    const l_variable& r_b = stack(1);
                    //try
                    if ( r_a.m_type == l_variable::type::OBJECT )
                    {
                        //get object
                        l_obj* this_obj = (l_obj*)r_a.m_value.m_pobj;
                        //types
                        l_vector* vector = dynamic_cast< l_vector* > ( this_obj );
                        //types
                        l_table* table = dynamic_cast< l_table* > ( this_obj );
                        //is a vector
                        if(vector)
                        {
                            //to size int
                            size_t index = 0;
                            //cast
                                 if( r_b.m_type == l_variable::INT )  index= (size_t)r_b.m_value.m_i;
                            else if( r_b.m_type == l_variable::FLOAT )index= (size_t)r_b.m_value.m_f;
                            else assert(0);
                            //get and pop value
                            vector->operator[](index) = pop();
                        }
                        else if(table)
                        {
                            //get and pop value
                            table->operator[](r_b) = pop();
                        }
                        else
                        {
                            assert(0);
                        }
                    }
                    else
                    {
                        //pop value
                        pop();
                    }
                    //pop index
                    pop();
                    //pop array/tablet
                    pop();
                }
                break;
                //for in
                case L_IT:
                {
                    //get index
                    l_variable& r_a = top();
                    //..
                    //try
                    if ( r_a.m_type == l_variable::type::OBJECT )
                    {
                        //get object
                        l_obj* this_obj = (l_obj*)r_a.m_value.m_pobj;
                        //types
                        l_vector* vector = dynamic_cast< l_vector* > ( this_obj );
                        //types
                        l_table* table = dynamic_cast< l_table* > ( this_obj );
                        //is a vector
                        if(vector)
                        {
                            //pop value
                            pop();
                            //push it
                            push( vector->get_it() );
                        }
                        else if (table)
                        {
                            //pop value
                            pop();
                            //push it
                            push( table->get_it() );
                        }
                        else
                        {
                            assert(0);
                        }
                    }
                    else
                    {
                        //pop value
                        pop();
                        //..
                        assert(0);
                    }
                }
                break;
                //for of
                case L_FOR_IN:
                case L_FOR_OF:
                {
                    //get index
                    l_variable& r_it = top();
                    //try
                    if ( r_it.m_type == l_variable::type::OBJECT )
                    {
                        //get object
                        l_obj* this_obj = (l_obj*)r_it.m_value.m_pobj;
                        //types
                        l_vector_it* a_it = dynamic_cast< l_vector_it* > ( this_obj );
                        //types
                        l_table_it* t_it = dynamic_cast< l_table_it* > ( this_obj );
                        //is array it
                        if(a_it)
                        {
                            //else assert
                            assert(a_it);
                            //push it
                            if(a_it->valid())
                            {
                                if(cmp.m_op_code == L_FOR_OF)
                                {
                                    //get next
                                    push( a_it->get() );
                                }
                                else //L_FOR_IN
                                {
                                    //get next
                                    push( a_it->get_id() );
                                }
                                //next
                                a_it->self_next();
                            }
                            else
                            {
                                //pop iterator
                                pop();
                                //and jump
                                pc = cmp.m_arg - 1;
                            }
                        }
                        //is a table it
                        else if(t_it)
                        {
                            //else assert
                            assert(t_it);
                            //push it
                            if(t_it->valid())
                            {
                                if(cmp.m_op_code == L_FOR_OF)
                                {
                                    //get next
                                    push( t_it->get() );
                                }
                                else //L_FOR_IN
                                {
                                    //get next
                                    push( t_it->get_id() );
                                }
                                //next
                                t_it->self_next();
                            }
                            else
                            {
                                //pop iterator
                                pop();
                                //and jump
                                pc = cmp.m_arg - 1;
                            }
                        }
                        else
                        {
                            assert(0);
                        }
                    }
                    else
                    {
                        //pop it
                        pop();
                        //and jump
                        pc = cmp.m_arg - 1;
                        //...
                        assert(0);
                    }
                }
                break;
                
                case L_CALL:
                {
                    //get index
                    register3(0) = pop();
                    //get args
                    if(register3(0).m_type == l_variable::CFUNCTION)
                    {
                        //return size
                        int n_return = register3(0).m_value.m_pcfun(&thread,cmp.m_arg);
                        //assert (1 return)
                        assert(n_return <= 1);
                        //error
                        assert(n_return >= 0);
                        //pop args
                        for(int i=0; i < cmp.m_arg; ++i)
                        {
                            pop();
                        }
                        //if return
                        if(n_return) push(register3(2));
                    }
                    else
                    {
                        assert(0);
                    }
                }
                break;
                    
                default:  break;
            }
        }
    }
};