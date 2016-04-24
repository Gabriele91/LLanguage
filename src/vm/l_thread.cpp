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
        get_last_context()        = l_variable( &context );
        //pc...
        unsigned int     pc       = 0;
        l_function&      function = m_vm->function(context.get_fun_id());
        l_list_command&  commands = function.m_commands;
        //macro
        #define raise(str)\
        {\
            push_error(str, pc, (unsigned int)cmp.m_line);\
            return T_RETURN_ERROR;\
        }
        #define vconst(c)  function.m_costants[c]
        #define top_size   (m_top+1)
        //for all commands
        for (pc = 0; pc < commands.size(); ++pc)
        {
            //current command
            l_command& cmp = commands[pc];
            //save info
            m_last_code_asm  = pc;
            m_last_code_line = (unsigned int)commands[pc].m_line;
            //opcodes
            switch (cmp.m_op_code)
            {
                case L_JMP: pc = cmp.m_arg - 1; break;
                case L_RETURN:
                    //push return
                    if(cmp.m_arg)
                    {
                        //get value
                        get_return() = pop();
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
                        get_temp1() = l_closer::gc_new(get_gc());
                        //init context
                        get_temp1().to<l_closer>()->init(call_fun.m_value.m_fid, this, l_variable(&context));
                        //push context
                        push(get_temp1());
                    }
                }
                break;
                ////////////////////////////////////////////////////////////
                #define operation(op,OP)\
                if(!stack(1).op(stack(0),stack(1)))\
                {\
                    if(stack(0).is_object())\
                    {\
                        l_class* cobj = stack(0).object()->get_class().clazz();\
                        l_variable op = cobj->get_operator(l_class::OP_ ## OP);\
                        if(op.is_null()) raise("not valid operation");\
                        get_this() = stack(0);\
                        pop();\
                        push(op);\
                        execute_call(pc, {cmp.m_op_code , 1});\
                    }\
                    else\
                        raise("not valid operation");\
                }\
                pop();
                ////////////////////////////////////////////////////////////
                case L_ADD:
                    operation(add,ADD);
                break;
                    
                case L_MUL:
                    operation(mul,MUL);
                break;
                    
                case L_SUB:
                    operation(sub,SUB);
                    pop();
                break;
                    
                case L_DIV:
                    if(stack(0).is_false()) raise("division by zero not permitted");
                    operation(div,DIV);
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
                    get_temp1() = l_array::gc_new(get_gc());
                    //init ?
                    if( cmp.m_arg > 0 )
                    {
                        //types
                        l_array* vector = get_temp1().array();
                        //put stack into vector
                        for(int i = cmp.m_arg-1; i >= 0; --i)
                        {
                            vector->operator[](i) = pop();
                        }
                    }
                    //push array (n.b. gc run...)
                    push( get_temp1() );
                }
                break;
                //alloc tablet
                case L_NEW_TABLE:
                {
                    get_temp1() = l_table::gc_new(get_gc());
                    //init ?
                    if( cmp.m_arg > 0 )
                    {
                        //types
                        l_table* table = get_temp1().table();
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
                    push( get_temp1() );
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
                        else if(r_b.is_object())
                        {
                            //types
                            l_object* object =  r_b.object();
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
                        else if(r_a.is_object())
                        {
                            //types
                            l_object* object =  r_a.object();
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
                            l_xrange* xrange = static_cast< l_xrange* > ( r_a.m_value.m_pobj );
                            //pop value
                            pop();
                            //push it
                            push( xrange->get_it() );
                        }
                        else if (r_a.is_object())
                        {
                            //ref class
                            l_class* ref_class = r_a.object()->get_class().clazz();
                            //get call
                            l_variable op_get_it = ref_class->get_operator(l_class::OP_GET_IT);
                            //set this
                            get_this() = r_a;
                            //pop value r_a from stack
                            pop();
                            //push this call
                            push(op_get_it);
                            //execute call
                            if(execute_call(pc,cmp)!=T_RETURN_VOID) return T_RETURN_ERROR;
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
                    else if ( r_it.is_object() )
                    {
                        //ref class
                        l_class* ref_class = r_it.object()->get_class().clazz();
                        //..
                        l_variable op_get_valid  = ref_class->get_operator(l_class::OP_GET_VALID);
                        l_variable op_get_v_o_k = (cmp.m_op_code == L_FOR_OF) ?
                                                    ref_class->get_operator(l_class::OP_GET_VALUE):
                                                    ref_class->get_operator(l_class::OP_GET_KEY);
                        l_variable op_get_next  = ref_class->get_operator(l_class::OP_GET_NEXT);
                        //is a op
                        if(
                           (
                            op_get_valid.is_closer() ||
                            op_get_valid.is_cfunction()
                           )
                           &&
                           (
                            op_get_v_o_k.is_closer() ||
                            op_get_v_o_k.is_cfunction()
                           )
                           &&
                           (
                            op_get_next.is_closer() ||
                            op_get_next.is_cfunction()
                           )
                        )
                        {
                            //set this
                            get_this() = r_it;
                            //get iterator
                            push(op_get_valid);
                            //execute call
                            if(execute_call(pc,{ cmp.m_op_code, 0 })!=T_RETURN_VOID) return T_RETURN_ERROR;
                            //is valid?
                            if(value(0).is_true())
                            {
                                //pop bool
                                pop();
                                //get value
                                push(op_get_v_o_k);
                                //execute call
                                if(execute_call(pc,{ cmp.m_op_code, 0 })!=T_RETURN_VOID) return T_RETURN_ERROR;
                                //get next it
                                push(op_get_next);
                                //execute call
                                if(execute_call(pc,{ cmp.m_op_code, 0 })!=T_RETURN_VOID) return T_RETURN_ERROR;
                            }
                            else
                            {
                                //pop bool
                                pop();
                                //pop iterator
                                pop();
                                //and jump
                                pc = cmp.m_arg - 1;
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
                    //Execute call
                    if(execute_call(pc,cmp)!=T_RETURN_VOID) return T_RETURN_ERROR;
                    //...
                break;
                case L_START_CLASS_DEC:
                    //alloc
                    get_class_temp() = l_class::gc_new(get_gc());
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
    
    l_thread::type_return l_thread::execute_call(int pc,l_command cmp)
    {
        //get index
        get_temp1() = pop();
        //return this?
        bool b_ret_this = false;
        //class call
        if(get_temp1().is_class())
        {
            l_class* this_class = get_temp1().clazz();
            //this
            get_this() = this_class->new_object(this);
            //is a this call
            cmp.m_op_code = L_THIS_CALL;
            //return this
            b_ret_this = true;
            //get costructor
            get_temp1() = this_class->get_constructor();
            //is null?
            if(get_temp1().is_null())
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
                return T_RETURN_VOID;
            }
        }
        //get args
        if( get_temp1().is_cfunction() )
        {
            //return size
            int n_return = get_temp1().m_value.m_pcfun(this,cmp.m_arg);
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
            //constructor
            if(b_ret_this)
            {
                push(get_this());
            }
            //if return
            else if(n_return)
            {
                push(get_return());
            }
        }
        else if( get_temp1().is_closer() )
        {
            //get context
            l_closer* closer = get_temp1().to<l_closer>();
            //else assert
            if(!closer){ assert(0); };
            //new function
            l_function& call_fun    = m_vm->function(closer->get_fun_id());
            //save last context
            l_variable last_ctx     = get_last_context();
            //new context
            get_temp2()            = l_call_context::gc_new(get_gc());
            l_call_context* new_ctx = get_temp2().to<l_call_context>();
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
                get_temp3() = l_array::gc_new(get_gc());
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
                    get_temp3().array()->push(pop());
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
                new_ctx->variable( call_fun.constant(n_fun_args) ) = get_temp3();
                //to null
                get_temp3() = l_variable();
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
                get_return() = new_ctx->this_field();
            }
            //unlock context
            new_ctx->unlock();
            //reset last context
            get_last_context() = last_ctx;
            //restore stack
            m_top = stack_top_bf_call;
            //return?
            if(n_return == T_RETURN_VALUE || b_ret_this)
            {
                push(get_return());
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
        //no errors
        return T_RETURN_VOID;
    }

    
}
