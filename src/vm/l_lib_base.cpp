//
//  l_lib_base.cpp
//  LLanguage
//
//  Created by Gabriele on 27/01/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#include <l_lib_base.h>
#include <l_array.h>
#include <cmath>
int l_print(l_language::l_thread* th,int args)
{
    for(size_t i=0; i!=args; ++i)
    {
        //
        const l_language::l_variable& var = th->value(i);
        //
        switch (var.m_type)
        {
            case l_language::l_variable::LNULL: printf("null"); break;
            case l_language::l_variable::LBOOL: printf("%s",var.m_value.m_b?"true":"false"); break;
            case l_language::l_variable::INT: printf("%d",var.m_value.m_i); break;
            case l_language::l_variable::FLOAT: printf("%g",var.m_value.m_f); break;
            case l_language::l_variable::STRING:
            {
                printf("%s",var.string()->c_str());
            }
            break;
            case l_language::l_variable::FUNCTION:
            {
                printf("function: %d",var.m_value.m_i);
            }
            case l_language::l_variable::CLOSER:
            {
                printf("closer: %p",(void*)var.m_value.m_pobj);
            }
            break;
            case l_language::l_variable::CFUNCTION:
            {
                printf("cfuncrion: %p",(void*)var.m_value.m_pcfun);
            }
            break;
            case l_language::l_variable::OBJECT:
            {
                printf("object: %p",(void*)var.m_value.m_pobj);
            }
            break;
            default: break;
        }
    }
    //number of return
    return 0;
}

int l_println(l_language::l_thread* th,int args)
{
    int output = l_print(th, args);
    printf("\n");
    return output;
}

int l_input(l_language::l_thread* th,int args)
{
    //str
    std::string str;
    std::cin >> str;
    //push string
    th->push_return(l_language::l_string::gc_new(th->m_vm->get_gc(), str));
    //return
    return 1;
}

int l_to_int(l_language::l_thread* th,int args)
{
    //get arg
    l_language::l_variable& var = th->value(0);
    //cast
    int i = 0;
    //cast
    switch (var.m_type)
    {
        case l_language::l_variable::INT: i = var.m_value.m_i; break;
        case l_language::l_variable::FLOAT: i = (int)var.m_value.m_f; break;
        case l_language::l_variable::STRING: i = std::atoi(var.string()->c_str()); break;
        default: assert(0); break;
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
        case l_language::l_variable::INT: f = (float)var.m_value.m_i; break;
        case l_language::l_variable::FLOAT: f = var.m_value.m_f; break;
        case l_language::l_variable::STRING: f = (float)std::atof(var.string()->c_str()); break;
        default: assert(0); break;
    }
    //push string
    th->push_return(l_language::l_variable(f));
    //return
    return 1;
}

int l_mod(l_language::l_thread* th,int args)
{
    //get arg
    l_language::l_variable& var       = th->value(0);
    l_language::l_variable& var_right = th->value(1);
    //cases
    if(var.is_float() || var_right.is_float())
    {
        th->push_return(l_language::l_variable(std::fmod(var.to_float(),var_right.to_float())));
    }
    else if(var.is_int() && var_right.is_int())
    {
        th->push_return(l_language::l_variable(var.to_int() % var_right.to_int()));
    }
    else
    {
        th->push_return(l_language::l_variable(0));
    }
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
    for(int value  = start;
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

int l_to_string(l_language::l_thread* th,int args)
{
    //get arg
    l_language::l_variable& var = th->value(0);
    //cast
    std::string  str;
    //cast
    switch (var.m_type)
    {
        case l_language::l_variable::INT: str = std::to_string(var.m_value.m_i); break;
        case l_language::l_variable::FLOAT: str = std::to_string(var.m_value.m_f); break;
        case l_language::l_variable::STRING: str = var.string()->c_str(); break;
        default: assert(0); break;
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
        case l_language::l_variable::LNULL:     str ="null";     break;
        case l_language::l_variable::LBOOL:     str ="bool";     break;
        case l_language::l_variable::INT:       str ="int";      break;
        case l_language::l_variable::FLOAT:     str ="float";    break;
        case l_language::l_variable::STRING:    str ="string";   break;
        case l_language::l_variable::FUNCTION:  str ="function"; break;
        case l_language::l_variable::CFUNCTION: str ="cfunction";break;
        case l_language::l_variable::ARRAY:     str ="array";    break;
        case l_language::l_variable::TABLE:     str ="table";    break;
        case l_language::l_variable::OBJECT:    str ="object";   break;
        case l_language::l_variable::COBJECT:   str ="cobject";  break;
        default: assert(0); break;
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
	//get variable
	auto& name = th->value(0);
	//gettable
	auto& var_table = th->main_context()->variable(name);
	//table obj
	auto& table = *var_table.to<l_language::l_table>();
	//for each
	for (auto it : table.raw())
	{
		th->main_context()->variable(it.first) = it.second;
	}
	//number of ret values
	return 0;
}

int l_system(l_language::l_thread* th, int args)
{
	//get variable
	auto& command = th->value(0);
	//isn't a string?
	if (!command.is_string()) return -1;
	//else execute
	system(command.to_string().c_str());
	//number of ret values
	return 0;
}

namespace l_language
{
    l_language::l_program_language::l_extern_libary l_base_lib=
    {
        { "get_global", l_get_global    },
        { "print",      l_print         },
        { "println",    l_println       },
        { "input",      l_input         },
        { "to_int",     l_to_int        },
        { "to_float",   l_to_float      },
        { "to_string",  l_to_string     },
        { "type_of",    l_type_of       },
        { "mod",        l_mod           },
        { "using",      l_using         },
        { "range",      l_range         },
		{ "system",     l_system        }
    };
}