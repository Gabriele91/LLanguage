//
//  l_base_lib.cpp
//  LLanguage
//
//  Created by Gabriele on 27/01/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#include <l_base_lib.h>
#include <l_array.h>
#include <l_object.h>
#include <l_class.h>
#include <l_xrange.h>
#include <cmath>


int l_to_int(l_language::l_thread* th,int args)
{
    //get arg
    l_language::l_variable& var = th->value(0);
    //cast
     l_language::l_int i = 0;
    //cast
    switch (var.m_type)
    {
        case l_language::l_variable::LNULL: i = 0; break;
        case l_language::l_variable::LBOOL: i = var.m_value.m_b ? 1 : 0; break;
        case l_language::l_variable::INT: i = var.m_value.m_i; break;
        case l_language::l_variable::FLOAT: i = (l_language::l_int)var.m_value.m_f; break;
        case l_language::l_variable::STRING: i = std::atoi(var.string()->c_str()); break;
        default: return -1; break;
    }
    //push string
    th->push_return(l_language::l_variable(i));
    //return
    return 1;
}

int l_to_float(l_language::l_thread* th,int args)
{
    //get arg
    l_language::l_variable& var = th->value(0);
    //cast
    float f = 0;
    //cast
    switch (var.m_type)
    {
        case l_language::l_variable::LNULL: f = 0.0f; break;
        case l_language::l_variable::LBOOL: f = var.m_value.m_b ? 1.0f : 0.0f; break;
        case l_language::l_variable::INT: f = (l_language::l_float)var.m_value.m_i; break;
        case l_language::l_variable::FLOAT: f = var.m_value.m_f; break;
        case l_language::l_variable::STRING: f = (l_language::l_float)std::atof(var.string()->c_str()); break;
        default: return -1; break;
    }
    //push string
    th->push_return(l_language::l_variable(f));
    //return
    return 1;
}

int l_range(l_language::l_thread* th,int args)
{
    int start = 0;
    int stop  = 0;
    int step  = 1;
    //..
    switch (args)
    {
        case 1: stop = th->value(0).to_int(); break;
            
        case 2: start = th->value(0).to_int();
                stop  = th->value(1).to_int(); break;
            
        case 3: start = th->value(0).to_int();
                stop  = th->value(1).to_int();
                step  = th->value(2).to_int(); break;
            
        default: return -1; break;
    }
    //alloc
    l_language::l_variable array = l_language::l_array::gc_new(th->get_gc());
    //add values
    for(l_language::l_int value  = start;
                          value != stop;
                          value += step)
    {
        array.to<l_language::l_array>()->push({ value });
    }
    //return
    th->push_return(array);
    //return
    return 1;
}

int l_xrange_gen(l_language::l_thread* th,int args)
{
    int start = 0;
    int stop  = 0;
    int step  = 1;
    //..
    switch (args)
    {
        case 1: stop = th->value(0).to_int(); break;
        
        case 2: start = th->value(0).to_int();
        stop  = th->value(1).to_int(); break;
        
        case 3: start = th->value(0).to_int();
                stop  = th->value(1).to_int();
                step  = th->value(2).to_int(); break;
        
        default: return -1; break;
    }
    //alloc
    l_language::l_variable xrange = l_language::l_xrange::gc_new(th->get_gc(),start,stop,step);
    //return
    th->push_return(xrange);
    //return
    return 1;
}

int l_to_string(l_language::l_thread* th,int args)
{
    //get arg
    l_language::l_variable& var = th->value(0);
    //cast
    std::string  str;
    //cast
    switch (var.m_type)
    {
        case l_language::l_variable::LNULL: str = "null"; break;
        case l_language::l_variable::LBOOL: str = var.m_value.m_b ? "true" : "false"; break;
        case l_language::l_variable::INT: str = std::to_string(var.m_value.m_i); break;
        case l_language::l_variable::FLOAT: str = std::to_string(var.m_value.m_f); break;
        case l_language::l_variable::STRING: str = var.string()->c_str(); break;
        default: return -1; break;
    }
    //push string
    th->push_return(l_language::l_string::gc_new(th->m_vm->get_gc(), str));
    //return
    return 1;
}

int l_type_of(l_language::l_thread* th,int args)
{
    std::string str;
    //
    auto type = th->value(0).m_type;
    //
    switch (type)
    {
        case l_language::l_variable::LNULL:       str ="null";           break;
        case l_language::l_variable::LBOOL:       str ="bool";           break;
        case l_language::l_variable::INT:         str ="int";            break;
        case l_language::l_variable::FLOAT:       str ="float";          break;
        case l_language::l_variable::STRING:      str ="string";         break;
        case l_language::l_variable::REF:         str ="ref";            break;
        case l_language::l_variable::FUNCTION:    str ="function";       break;
        case l_language::l_variable::CFUNCTION:   str ="cfunction";      break;
        case l_language::l_variable::ARRAY:       str ="array";          break;
        case l_language::l_variable::TABLE:       str ="table";          break;
        case l_language::l_variable::OBJECT:      str ="object";         break;
		case l_language::l_variable::CLASS:       str ="class";          break;
		case l_language::l_variable::COBJECT:     str ="cobject";        break;
		case l_language::l_variable::CLOSER:      str ="closer";         break;
		default: return -1; break;
    }
    //push type
    th->push_return(l_language::l_string::gc_new(th->m_vm->get_gc(), str));
    //number of return
    return 1;
}

int l_get_global(l_language::l_thread* th,int args)
{
    //push type
    th->push_return(th->main_context()->get_local_variables());
    //number of return
    return 1;
}

int l_using(l_language::l_thread* th, int args)
{
    for(int arg =0; arg!=args; ++arg)
    {
        //get variable
        auto& name = th->value(arg);
        //is a string or a table?
        if(!name.is_table() && !name.is_string()) return -1;
        //gettable
        auto& var_table = name.is_table() ? name : th->main_context()->variable(name);
        //is a table?
        if(!var_table.is_table()) return -1;
        //table obj
        auto& table = *var_table.to<l_language::l_table>();
        //for each
        for (auto it : table.raw())
        {
            th->main_context()->variable(it.first) = it.second;
        }
    }
    //number of ret values
	return 0;
}

int l_eval(l_language::l_thread* th, int args)
{
    //get variable
    auto& command = th->value(0);
    //isn't a string?
    if (!command.is_string()) return -1;
    //else execute
    th->push_return(th->get_vm()->eval(command.to_string().c_str()));
    //number of ret values
    return 1;
}

int l_get_this(l_language::l_thread* th, int args)
{
    //return this
    th->push_return(th->get_this());
    //number of ret values
    return 1;
}

int l_length(l_language::l_thread* th, int args)
{
    if(args <= 0) return -1;
    //get variable
    auto& arg = th->value(0);
    //typs
    switch (arg.m_type)
    {
        case l_language::l_variable::STRING: th->push_return({(l_language::l_int)arg.string()->str().length()});  break;
        case l_language::l_variable::ARRAY:  th->push_return({(l_language::l_int)arg.array()->size()});           break;
        case l_language::l_variable::TABLE:  th->push_return({(l_language::l_int)arg.table()->size()});           break;
        default: return -1; break;
    }
    return 1;
}

int l_class_name(l_language::l_thread* th, int args)
{
    if(args <= 0) return -1;
    //get variable
    auto& arg = th->value(0);
    //typs
    switch (arg.m_type)
    {
        case l_language::l_variable::OBJECT:
        {
            auto* object = arg.object();
            auto* clazz  = object->get_class().clazz();
            th->push_return(clazz->get_class_name());
        }
        break;
        default: return -1; break;
    }
    return 1;
}

int l_copy(l_language::l_thread* th, int args)
{
    if(args <= 0) return -1;
    //get variable
    auto& arg = th->value(0);
    //types
    switch (arg.m_type)
    {
        case l_language::l_variable::STRING:
        {
            th->push_return(l_language::l_string::gc_new(th->get_gc(), arg.string()->str()));
        }
        break;
        case l_language::l_variable::ARRAY:
        {
            //get array
            l_language::l_variable n_array = l_language::l_array::gc_new(th->get_gc());
            //copy
            n_array.array()->get_raw_array() = arg.array()->get_raw_array();
            //push
            th->push_return(n_array);
        }
        break;
        case l_language::l_variable::TABLE:
        {
            //get array
            l_language::l_variable n_table = l_language::l_array::gc_new(th->get_gc());
            //copy
            n_table.table()->get_raw_map() = arg.table()->get_raw_map();
            //push
            th->push_return(n_table);
        }
        break;
        default: return -1; break;
    }
    return 1;
}

namespace l_language
{
    l_language::l_vm::l_extern_libary l_base_lib=
    {
        { "length",     l_length        },
        { "class_name", l_class_name    },
        { "get_global", l_get_global    },
        { "to_int",     l_to_int        },
        { "to_float",   l_to_float      },
        { "to_string",  l_to_string     },
        { "type_of",    l_type_of       },
        { "using",      l_using         },
        { "range",      l_range         },
        { "xrange",     l_xrange_gen    },
        { "eval",       l_eval          },
        { "get_this",   l_get_this      },
        { "copy",       l_copy          }
    };
}