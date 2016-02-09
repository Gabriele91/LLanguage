//
//  l_lib_base.cpp
//  LLanguage
//
//  Created by Gabriele on 27/01/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#include <l_lib_base.h>
#include <cmath>
int l_print(l_language::l_thread* th,int args)
{
    for(size_t i=0; i!=args; ++i)
    {
        //
        const l_language::l_variable& var = th->value(args-i-1);
        //
        switch (var.m_type)
        {
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
    l_language::l_variable& var = th->value(args-1);
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
    l_language::l_variable& var = th->value(args-1);
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
    l_language::l_variable& var       = th->value(args-1);
    l_language::l_variable& var_right = th->value(args-2);
    //cases
    if(var.is_float() || var.is_float())
    {
        th->push_return(l_language::l_variable(std::fmod(var.to_float(),var_right.to_float())));
    }
    else if(var.is_int() && var.is_int())
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

int l_to_string(l_language::l_thread* th,int args)
{
    //get arg
    l_language::l_variable& var = th->value(args-1);
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
    auto type = th->value(args-1).m_type;
    //
    switch (type)
    {
        case l_language::l_variable::INT: str="INT"; break;
        case l_language::l_variable::FLOAT: str="FLOAT"; break;
        case l_language::l_variable::STRING: str="STRING"; break;
        case l_language::l_variable::FUNCTION: str="FUNCTION"; break;
        case l_language::l_variable::CFUNCTION: str="CFUNCTION"; break;
        case l_language::l_variable::OBJECT: str = "OBJECT"; break;
        default: assert(0); break;
    }
    //push type
    th->push_return(l_language::l_string::gc_new(th->m_vm->get_gc(), str));
    //number of return
    return 1;
}

int l_global(l_language::l_thread* th,int args)
{
    //push type
    th->push_return(th->main_context().get_local_variables());
    //number of return
    return 1;
}

int l_using(l_language::l_thread* th, int args)
{
	//get variable
	auto& name = th->value(args - 1);
	//gettable
	auto& var_table = th->main_context().variable(name);
	//table obj
	auto& table = *var_table.to<l_language::l_table>();
	//for each
	for (auto it : table.raw())
	{
		th->main_context().variable(it.first) = it.second;
	}
	//number of return
	return 0;
}

namespace l_language
{
    l_language::l_program_language::l_extern_libary l_base_lib=
    {
        { "global",    l_global    },
        { "print",     l_print     },
        { "println",   l_println   },
        { "input",     l_input     },
        { "to_int",    l_to_int    },
        { "to_float",  l_to_float  },
        { "to_string", l_to_string },
        { "type_of",   l_type_of   },
        { "mod",       l_mod       },
		{ "using",     l_using     }
    };
}