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
#include <l_class.h>
#include <l_xrange.h>
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
                case L_GET_THIS:
                    push(context.this_field());
                break;
                case L_SET_THIS:
                    context.this_field() = pop();
                break;
                case L_SET_THIS_NPOP:
                    context.this_field() = stack(cmp.m_arg);
                break;
                ////////////////////////////////////////////////////////////
                case L_NEW_ARRAY:
                {
                    register3(0) = l_array::gc_new(get_gc());
                    //init ?
                    if( cmp.m_arg > 0 )
                    {
                        //types
                        l_array* vector = register3(0).array();
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
                        //types
                        l_table* table = register3(0).table();
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
                          l_variable& r_b = stack(1);
                    const l_variable& r_c = stack(0);
                    //try
                    if ( r_b.is_ref_obj() )
                    {
                        //is a vector
                        if(r_b.is_array())
                        {
                            //types
                            l_array* vector = r_b.array();
                            //to size int
                            size_t index = 0;
                            //cast
                            if( r_c.is_int() )  index= (size_t)r_c.m_value.m_i;
                            else if( r_c.is_float() )index= (size_t)r_c.m_value.m_f;
                            else raise( "value isn't a valid key" );
                            //save last
                            get_this() = stack(1);
                            //get
                            stack(1)   = vector->operator[](index) ;
                        }
                        else if(r_b.is_table())
                        {
                            //types
                            l_table* table =  r_b.table();
                            //is a string?
                            if(!r_c.is_string()) raise( "value isn't a valid key" );
                            //save last
                            get_this() = stack(1);
                            //get and pop value
                            stack(1) = table->operator[](r_c);
                        }
                        else if(r_b.is_class_object())
                        {
                            //types
                            l_class_object* object =  r_b.class_object();
                            //is a string?
                            if(!r_c.is_string()) raise( "value isn't a valid key" );
                            //save last
                            get_this() = stack(1);
                            //get and pop value
                            stack(1) = object->get_value(r_c);
                        }
                        else
                        {
                            raise( "value isn't a vector/table/object, field not available" );
                        }
                    }
                    else
                    {
                        raise( "value isn't a vector/table/object, field not avaliable" );
                    }
                    //pop index
                    pop();
                }
                break;
                case L_SET_AT_VAL:
                {
                    //get table/array
                          l_variable& r_a = stack(2);
                    //get index
                    const l_variable& r_b = stack(1);
                    //try
                    if ( r_a.is_ref_obj() )
                    {
                        //is a vector
                        if(r_a.is_array())
                        {
                            //types
                            l_array* vector = r_a.array();
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
                            l_table* table =  r_a.table();
                            //is a string?
                            if(!r_b.is_string()) raise( "value isn't a valid key" );
                            //get and pop value
                            table->operator[](r_b) = pop();
                        }
                        else if(r_a.is_class_object())
                        {
                            //types
                            l_class_object* object =  r_a.class_object();
                            //is a string?
                            if(!r_b.is_string()) raise( "value isn't a valid key" );
                            //get and pop value
                            if(!object->set_value(r_b, pop())) raise( "value is't an attribute" );
                        }
                        else
                        {
                            raise( "value isn't a vector/table/object, field not available" );
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
                    if ( r_a.is_ref_obj() )
                    {
                        //get object
                        l_obj* this_obj = (l_obj*)r_a.m_value.m_pobj;
                        //is a vector
                        if(r_a.is_array())
                        {
                            //types
                            l_array* vector = r_a.array();
                            //pop value
                            pop();
                            //push it
                            push( vector->get_it() );
                        }
                        else if (r_a.is_table())
                        {
                            //types
                            l_table* table = r_a.table();
                            //pop value
                            pop();
                            //push it
                            push( table->get_it() );
                        }
                        else if (r_a.to<l_xrange>())
                        {
                            //types
                            l_xrange* xrange = static_cast< l_xrange* > ( this_obj );
                            //pop value
                            pop();
                            //push it
                            push( xrange->get_it() );
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
                        raise( "value isn't a table/array/object, iterator not supported" );
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
                    if ( r_it.is_iterator() )
                    {
                        //types
                        l_iterator* l_it  = r_it.iterator();
                        //is array it
                        if(l_it)
                        {
                            //else assert
                            assert(l_it);
                            //push it
                            if(l_it->valid())
                            {
                                if(cmp.m_op_code == L_FOR_OF)
                                {
                                    //get next
                                    push( l_it->get() );
                                }
                                else //L_FOR_IN
                                {
                                    //get next
                                    push( l_it->get_id() );
                                }
                                //next
                                l_it->self_next();
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
                case L_THIS_CALL:
                case L_CALL:
                {
                    //get index
                    register3(0) = pop();
                    //return this?
                    bool b_ret_this = false;
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
                        if(n_return) push(get_return());
                    }
                    else if( register3(0).is_closer() || register3(0).is_class() )
                    {
                        if(register3(0).is_class())
                        {
                            l_class* this_class = register3(0).clazz();
                            //this
                            get_this() = this_class->new_object(this);
                            //is a this call
                            cmp.m_op_code = L_THIS_CALL;
                            //return this
                            b_ret_this = true;
                            //get costructor
                            register3(0) = this_class->get_constructor();
                            //is null?
                            if(register3(0).is_null())
                            {
                                //pop args
                                for(unsigned int
                                    arg  = 0;
                                    arg != cmp.m_arg;
                                    ++arg)
                                {
                                    pop();
                                }
                                //push this
                                push(get_this());
                                //remove this ref
                                get_this() = l_variable();
                                //..
                                break;
                            }
                        }
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
                        //this?
                        if(cmp.m_op_code == L_THIS_CALL)
                        {
                            new_ctx->this_field() = get_this();
                        }
                        //init
                        new_ctx->init(*closer);
                        //lock context
                        new_ctx->lock();
                        //n args
                        unsigned int n_fun_args = call_fun.m_args_size;
                        //alloc array args..?
                        if (call_fun.m_have_args_list)
                        {
                            //alloc array
                            register3(3) = l_array::gc_new(get_gc());
                        }
                        //put arguments
                        for(unsigned int
                            arg  = 0;
                            arg != cmp.m_arg;
                          ++arg)
                        {
                            if (arg < n_fun_args)
                            {
                                new_ctx->variable( call_fun.constant(arg) ) = pop();
                            }
                            else if (call_fun.m_have_args_list)
                            {
                                register3(3).array()->push(pop());
                            }
                            else
                            {
                                pop();
                            }
                        }
                        //add var list
                        if (call_fun.m_have_args_list)
                        {
                            //push array
                            new_ctx->variable( call_fun.constant(n_fun_args) ) = register3(3);
                            //to null
                            register3(3) = l_variable();
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
						else if (b_ret_this)
						{
							register3(2) = new_ctx->this_field();
						}
                        //unlock context
                        new_ctx->unlock();
                        //reset last context
                        register3(R_CONTEXT) = last_ctx;
                        //restore stack
                        m_top = stack_top_bf_call;
                        //return?
                        if(n_return == T_RETURN_VALUE || b_ret_this)
                        {
                            push(register3(2));
                        }
                    }
                    else
                    {
                        raise( "value isn't an function" );
                    }
                    //dealloc
                    if(cmp.m_op_code == L_THIS_CALL)
                    {
                        get_this() = l_variable();
                    }
                }
                    break;
                case L_START_CLASS_DEC:
                    //alloc
                    get_class_temp() = l_variable(get_gc()->new_obj<l_class>());
                    //end
                    break;
                case L_CLASS_ATTR:
				{
					//values 
					const l_variable& key   = stack(0);
					const l_variable& value = stack(1);
					//add
					get_class_temp().clazz()->add_variable(key, value);
					//pop
					pop();
					pop();
				}
                //end
                break;
                case L_CLASS_METHOD:
				{
					//values 
					const l_variable& key   = stack(0);
					const l_variable& value = stack(1);
					//add
					get_class_temp().clazz()->add_def(key, value);
					//pop
					pop();
					pop();
					//end
				}
                break;
                case L_CLASS_PARENT:
                    //push class
					for (unsigned int i = 0; i != cmp.m_arg; ++i)
					{
						get_class_temp().clazz()->add_parent(pop());
					}
                    //end
                    break;
                case L_END_CLASS_DEC:
                    //set class name
                    get_class_temp().clazz()->set_class_name( vconst(cmp.m_arg) );
                    //push
                    push(get_class_temp());
                    //end
                    break;
                default:  break;
            }
        }
        
        return T_RETURN_VOID;
    }
    
}
