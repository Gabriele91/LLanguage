//
//  l_os_lib.cpp
//  LLanguage
//
//  Created by Gabriele on 23/03/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#include <l_os_lib.h>
#include <l_array.h>
#include <cmath>

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
    l_language::l_vm::l_extern_libary l_os_lib=
    {
        { "system",     l_system        }
    };
}