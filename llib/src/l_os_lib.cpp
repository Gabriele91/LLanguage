//
//  l_os_lib.cpp
//  LLanguage
//
//  Created by Gabriele on 23/03/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#include <ctime>
#include <chrono>
#include <l_os_lib.h>
#include <l_string.h>
#include <l_variable.h>
#include <l_function_wrapper.h>
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

int l_clock(l_language::l_thread* th, int args)
{
    clock_t clock = std::clock();
    th->push_return({( (float)clock /  (float)CLOCKS_PER_SEC) });
    return 1;
}

int l_time(l_language::l_thread* th, int args)
{
    th->push_return({ (int)(std::time(nullptr)) });
    return 1;
}

int l_mstime(l_language::l_thread* th, int args)
{
    int mstime = int(std::chrono::duration_cast< std::chrono::milliseconds >(
        std::chrono::system_clock::now().time()
    ).count());
    th->push_return({ mstime });
    return 1;
}

namespace l_language
{
    const l_language::l_vm::l_extern_libary& l_os_lib(l_vm* vm)
    {
        static l_language::l_vm::l_extern_libary l_lib
        {
            { "system" , l_system              },
            { "get_env", l_get_env             },
            { "clock",   l_clock               },
            { "time",    l_time                },
            { "mstime",  l_mstime              }
        };
        return l_lib;
    }
}