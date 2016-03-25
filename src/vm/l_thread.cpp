//
//  l_thread.cpp
//  LLanguage
//
//  Created by Gabriele on 22/03/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//

#include <stdio.h>
#include <l_thread.h>
#include <l_vm.h>
#include <l_gc.h>
#include <l_array.h>
#include <l_table.h>
#include <iostream>

namespace l_language
{
    
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
        //...
        if(m_vm)
        {
            //main alloc
            m_main_ctx = l_call_context::gc_new(vm);
            //init
            main_context()->l_closer::init(m_main_fun_id,this);
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
        l_call_context& main = *main_context();
        l_variable& id = m_vm->function(main.m_fun_id).m_costants[i];
        return main.variable(id);
    }
    l_variable& l_thread::global(l_call_context& fun,int i)
    {
        l_call_context& main = *main_context();
        l_variable& id = m_vm->function(fun.m_fun_id).m_costants[i];
        return main.variable(id);
    }
    l_function* l_thread::main_function()
    {
        return &m_vm->function(m_main_fun_id);
    }
    //access
    l_variable& l_thread::local(l_call_context& fun,int i)
    {
        l_variable& id = m_vm->function(fun.m_fun_id).m_costants[i];
        //is in this table?
        if(fun.exists(id))
        {
            return fun.variable(id);
        }
        //search before...
        else
        {
            //get next
            l_call_context* rec_fun = fun.next();
            //search...
            while(rec_fun)
            {
                if(rec_fun->exists(id)) return rec_fun->variable(id);
                rec_fun = rec_fun->next();
            }
        }
        //else alloc
        return fun.variable(id);
    }
    //access
    l_variable& l_thread::strick_local(l_call_context& fun,int i)
    {
        l_variable& id = m_vm->function(fun.m_fun_id).m_costants[i];
        return fun.variable(id);
    }
    
    //execute context
    l_thread::type_return l_thread::execute(l_call_context&  context)
    {
        //lock context
        context.lock();
        //save context
        register3(R_CONTEXT)      = l_variable( &context );
        //pc...
        unsigned int     pc       = 0;
        l_function&      function = m_vm->function(context.get_fun_id());
        l_list_command&  commands = function.m_commands;
        //macro
        #define raise(str)\
        {\
            push_error(str, pc, (unsigned int)commands[pc].m_line);\
            return T_RETURN_ERROR;\
        }
        #define vconst(c)  function.m_costants[c]
        #define top_size   (m_top+1)
        //for all commands
        for (pc = 0; pc < commands.size(); ++pc)
        {
            //current command
            l_command& cmp = commands[pc];
            //opcodes
            switch (cmp.m_op_code)
            {
                case L_JMP: pc = cmp.m_arg - 1; break;
                case L_RETURN:
                    //push return
                    if(cmp.m_arg)
                    {
                        //get value
                        register3(2) = pop();
                        //return...
                        return T_RETURN_VALUE;
                    }
                    else
                    {
                        //came back
                        return T_RETURN_VOID;
                    }
                    //..
                break;
                    
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
                    
                case L_IF_OR_POP:
                    if(top().is_true())
                    {
                        pc = cmp.m_arg - 1;
                    }
                    else
                    {
                        pop();
                    }
                break;
                    
                case L_IF0_OR_POP:
                    if(top().is_false())
                    {
                        pc = cmp.m_arg - 1;
                    }
                    else
                    {
                        pop();
                    }
                break;
                    
                case L_PUSH:          push( stack(cmp.m_arg) );  break;
                case L_PUSH_NULL:     push( l_variable() );      break;
                case L_PUSH_TRUE:     push( l_variable(true) );  break;
                case L_PUSH_FALSE:    push( l_variable(false) ); break;
                case L_PUSHK:         push( vconst(cmp.m_arg) ); break;
                case L_CLOSER:
                {
                    //get value
                    l_variable& call_fun = vconst(cmp.m_arg);
                    //...
                    if(call_fun.is_function())
                    {
                        
                        //new context
                        register3(0) = l_closer::gc_new(get_gc());
                        //init context
                        register3(0).to<l_closer>()->init(call_fun.m_value.m_fid, this, l_variable(&context));
                        //push context
                        push(register3(0));
                    }
                }
                break;
                    ////////////////////////////////////////////////////////////
                case L_ADD:
                    if(!stack(1).add(stack(0),stack(1))) raise("not valid operation");
                    pop();
                break;
                    
                case L_MUL:
                    if(!stack(1).mul(stack(0),stack(1))) raise("not valid operation");
                    pop();
                break;
                    
                case L_SUB:
                    if(!stack(1).sub(stack(0),stack(1))) raise("not valid operation");
                    pop();
                break;
                    
                case L_DIV:
                    if(!stack(1).div(stack(0),stack(1))) raise("not valid operation");
                    pop();
                break;
                    
                case L_MOD:
                    if(!stack(1).mod(stack(0),stack(1))) raise("not valid operation");
                    pop();
                break;
                    
                case L_UNM:
                    if(!stack(0).unm(stack(0))) raise("not valid operation");
                break;
                    ////////////////////////////////////////////////////////////
                case L_EQ:
                    if(!stack(1).equal(stack(0),stack(1))) raise("not valid operation");
                    pop();
                break;
                    
                case L_NEQ:
                    if(!stack(1).not_equal(stack(0),stack(1))) raise("not valid operation");
                    pop();
                break;
                    
                case L_RT:
                    if(!stack(1).rt(stack(0),stack(1))) raise("not valid operation");
                    pop();
                break;
                    
                case L_RE:
                    if(!stack(1).re(stack(0),stack(1))) raise("not valid operation");
                    pop();
                break;
                    
                case L_LT:
                    if(!stack(1).lt(stack(0),stack(1))) raise("not valid operation");
                    pop();
                break;
                    
                case L_LE:
                    if(!stack(1).le(stack(0),stack(1))) raise("not valid operation");
                    pop();
                break;
                    ////////////////////////////////////////////////////////////
                case L_NOT:
                    if(!stack(0).not_value(stack(0))) raise("not valid operation");
                break;
                    
                case L_OR:
                    if(!stack(1).or_value(stack(0),stack(1))) raise("not valid operation");
                    pop();
                break;
                    
                case L_AND:
                    if(!stack(1).and_value(stack(0),stack(1))) raise("not valid operation");
                    pop();
                break;
                    ////////////////////////////////////////////////////////////
                case L_GET_GLOBAL:
                    push( global(context,cmp.m_arg) );
                break;
                    
                case L_SET_GLOBAL:
                    global(context,cmp.m_arg) = pop();
                break;
                    ////////////////////////////////////////////////////////////
                case L_GET_LOCAL:
                    push( strick_local(context,cmp.m_arg) );
                break;
                    
                case L_SET_LOCAL:
                    strick_local(context,cmp.m_arg) = pop();
                break;
                    ////////////////////////////////////////////////////////////
                case L_SET_UP_VALUE:
                    local(context,cmp.m_arg) = pop();
                break;
                    
                case L_GET_UP_VALUE:
                    push( local(context,cmp.m_arg) );
                break;
                    ////////////////////////////////////////////////////////////
                case L_NEW_ARRAY:
                {
                    register3(0) = l_array::gc_new(get_gc());
                    //init ?
                    if( cmp.m_arg > 0 )
                    {
                        //get object
                        l_obj* this_obj = (l_obj*)register3(0).m_value.m_pobj;
                        //types
                        l_array* vector = dynamic_cast< l_array* > ( this_obj );
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
                    if ( r_b.is_object() )
                    {
                        //get object
                        l_obj* this_obj = (l_obj*)r_b.m_value.m_pobj;
                        //is a vector
                        if(r_b.is_array())
                        {
                            //types
                            l_array* vector = dynamic_cast< l_array* > ( this_obj );
                            //to size int
                            size_t index = 0;
                            //cast
                            if( r_c.is_int() )  index= (size_t)r_c.m_value.m_i;
                            else if( r_c.is_float() )index= (size_t)r_c.m_value.m_f;
                            else raise( "value isn't a valid key" );
                            //get
                            stack(1) = vector->operator[](index) ;
                        }
                        else if(r_b.is_table())
                        {
                            //types
                            l_table* table = dynamic_cast< l_table* > ( this_obj );
                            //is a string?
                            if(!r_c.is_string()) raise( "value isn't a valid key" );
                            //get and pop value
                            stack(1) = table->operator[](r_c);
                        }
                        else
                        {
                            raise( "value isn't a vector/table" );
                        }
                    }
                    else
                    {
                        raise( "value isn't a vector/table/object" );
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
                    if ( r_a.is_object() )
                    {
                        //get object
                        l_obj* this_obj = (l_obj*)r_a.m_value.m_pobj;
                        //is a vector
                        if(r_a.is_array())
                        {
                            //types
                            l_array* vector = dynamic_cast< l_array* > ( this_obj );
                            //to size int
                            size_t index = 0;
                            //cast
                            if( r_b.is_int()   ) index= (size_t)r_b.m_value.m_i;
                            else if( r_b.is_float() ) index= (size_t)r_b.m_value.m_f;
                            else raise( "value isn't a valid key" );
                            //get and pop value
                            vector->operator[](index) = pop();
                        }
                        else if(r_a.is_table())
                        {
                            //types
                            l_table* table = dynamic_cast< l_table* > ( this_obj );
                            //is a string?
                            if(!r_b.is_string()) raise( "value isn't a valid key" );
                            //get and pop value
                            table->operator[](r_b) = pop();
                        }
                        else
                        {
                            raise( "value isn't a vector/table" );
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
                    if ( r_a.is_object() )
                    {
                        //get object
                        l_obj* this_obj = (l_obj*)r_a.m_value.m_pobj;
                        //is a vector
                        if(r_a.is_array())
                        {
                            //types
                            l_array* vector = dynamic_cast< l_array* > ( this_obj );
                            //pop value
                            pop();
                            //push it
                            push( vector->get_it() );
                        }
                        else if (r_a.is_table())
                        {
                            //types
                            l_table* table = dynamic_cast< l_table* > ( this_obj );
                            //pop value
                            pop();
                            //push it
                            push( table->get_it() );
                        }
                        else
                        {
                            raise( "this value not have a iterator" );
                        }
                    }
                    else
                    {
                        //pop value
                        pop();
                        //..
                        raise( "value isn't a table/array/object" );
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
                    if ( r_it.is_object() )
                    {
                        //get object
                        l_obj* this_obj = (l_obj*)r_it.m_value.m_pobj;
                        //types
                        l_array_it* a_it = dynamic_cast< l_array_it* > ( this_obj );
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
                            raise( "value isn't a valid iterator" );
                        }
                    }
                    else
                    {
                        //pop it
                        pop();
                        //and jump
                        pc = cmp.m_arg - 1;
                        //...
                        raise( "value isn't an iterator" );
                    }
                }
                break;
                    
                case L_CALL:
                {
                    //get index
                    register3(0) = pop();
                    //get args
                    if( register3(0).is_cfunction() )
                    {
                        //return size
                        int n_return = register3(0).m_value.m_pcfun(this,cmp.m_arg);
                        //assert (1 return)
                        assert(n_return <= 1);
                        //error
                        if(n_return<0)
                        {
                            raise( "native call exception" );
                        }
                        else if(n_return>1)
                        {
                            raise( "native call can't return more than a value" );
                        }
                        //pop args
                        for(int i=0; i < cmp.m_arg; ++i)
                        {
                            pop();
                        }
                        //if return
                        if(n_return) push(register3(2));
                    }
                    else if( register3(0).is_closer() )
                    {
                        //get context
                        l_closer* closer = register3(0).to<l_closer>();
                        //else assert
                        if(!closer){ assert(0); };
                        //new function
                        l_function& call_fun    = m_vm->function(closer->get_fun_id());
                        //save last context
                        l_variable last_ctx     = register3(R_CONTEXT);
                        //new context
                        register3(1)            = l_call_context::gc_new(get_gc());
                        l_call_context* new_ctx = register3(1).to<l_call_context>();
                        //init
                        new_ctx->init(*closer);
                        //lock context
                        new_ctx->lock();
                        //put arguments
                        for(unsigned int
                            arg  = 0;
                            arg != cmp.m_arg;
                          ++arg)
                        {
                            if (arg < call_fun.m_args_size)
                                new_ctx->variable( call_fun.constant(arg) ) = pop();
                            else
                                pop();
                        }
                        //save stack
                        long stack_top_bf_call = m_top;
                        //execute call
                        type_return n_return = execute(*new_ctx);
                        //error?
                        if(n_return==T_RETURN_ERROR)
                        {
                            raise( "call exception" );
                        }
                        //unlock context
                        new_ctx->unlock();
                        //reset last context
                        register3(R_CONTEXT) = last_ctx;
                        //restore stack
                        m_top = stack_top_bf_call;
                        //return?
                        if(n_return == T_RETURN_VALUE)
                        {
                            push(register3(2));
                        }
                    }
                    else
                    {
                        raise( "value isn't an function" );
                    }
                }
                break;
                    
                default:  break;
            }
        }
        
        return T_RETURN_VOID;
    }
    
}