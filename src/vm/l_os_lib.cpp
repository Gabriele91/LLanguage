//
//  l_os_lib.cpp
//  LLanguage
//
//  Created by Gabriele on 23/03/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#include <l_os_lib.h>
#include <l_string.h>
#include <l_variable.h>
#include <cmath>

int l_system(l_language::l_thread* th, int args)
{
    //have an arg?
    if (args <= 0) return -1;
    //get variable
    auto& command = th->value(0);
    //isn't a string?
    if (!command.is_string()) return -1;
    //else execute
    int sret = system(command.to_string().c_str());
    //set return
    th->push_return({sret == -1 ? false : true});
    //number of ret values
    return 1;
}

int l_get_env(l_language::l_thread* th, int args)
{
    //have an arg?
    if (args <= 0) return -1;
    //get variable
    auto& command = th->value(0);
    //isn't a string?
    if (!command.is_string()) return -1;
    //else execute
    char* value = getenv(command.to_string().c_str());
    //push return?
    if(value) th->push_return(l_language::l_string::gc_new(th->get_gc(), value));
    else      th->push_return(l_language::l_variable());
    //number of ret values
    return 1;
}

namespace l_language
{
    l_language::l_vm::l_extern_libary l_os_lib=
    {
        { "system" ,  l_system   },
        { "get_env",  l_get_env  }
    };
}