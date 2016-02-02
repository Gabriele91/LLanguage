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

int l_input(l_language::l_thread* th,int args)
{
    //str
    std::string str;
    std::cin >> str;
    //push string
    th->push_return(l_language::l_string::gc_new(th->m_vm->get_gc(), str),args);
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
    th->push_return(l_language::l_variable(i),args);
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
        case l_language::l_variable::STRING: f = std::atof(var.string()->c_str()); break;
        default: assert(0); break;
    }
    //push string
    th->push_return(l_language::l_variable(f),args);
    //return
    return 1;
}

int l_mod(l_language::l_thread* th,int args)
{
    //get arg
    l_language::l_variable& var       = th->value(args-1);
    l_language::l_variable& var_right = th->value(args-2);
    //cast
    float f = 0;
    //cast
    switch (var.m_type)
    {
        case l_language::l_variable::INT: f = (float)var.m_value.m_i; break;
        case l_language::l_variable::FLOAT: f = var.m_value.m_f; break;
        case l_language::l_variable::STRING: f = std::atof(var.string()->c_str()); break;
        default: assert(0); break;
    }
    //cast
    float f_right = 0;
    //cast
    switch (var.m_type)
    {
        case l_language::l_variable::INT: f_right = (float)var_right.m_value.m_i; break;
        case l_language::l_variable::FLOAT: f_right = var_right.m_value.m_f; break;
        case l_language::l_variable::STRING: f_right = std::atof(var_right.string()->c_str()); break;
        default: assert(0); break;
    }
    //push string
    th->push_return(l_language::l_variable(std::fmod(f,f_right)),args);
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
    th->push_return(l_language::l_string::gc_new(th->m_vm->get_gc(), str),args);
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
    th->push_return(l_language::l_string::gc_new(th->m_vm->get_gc(), str),args);
    //number of return
    return 1;
}

namespace l_language
{
    l_language::l_program_language::l_extern_libary l_base_lib=
    {
        { "print",     l_print     },
        { "input",     l_input     },
        { "to_int",    l_to_int    },
        { "to_float",  l_to_float  },
        { "to_string", l_to_string },
        { "type_of",   l_type_of   },
        { "mod",     l_mod       }
    };
}