//
//  l_io_lib.cpp
//  LLanguage
//
//  Created by Gabriele on 23/03/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#include <l_io_lib.h>
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

namespace l_language
{
    l_language::l_vm::l_extern_libary l_io_lib=
    {
        { "print",      l_print         },
        { "println",    l_println       },
        { "input",      l_input         }
    };
}