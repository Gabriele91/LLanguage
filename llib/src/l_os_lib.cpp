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
#ifdef WIN32
    #include <windows.h>
#elif _POSIX_C_SOURCE >= 199309L
    #include <time.h>   // for nanosleep
#else
    #include <unistd.h> // for usleep
#endif

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
    th->push_return({( (l_language::l_float)clock /  (l_language::l_float)CLOCKS_PER_SEC) });
    return 1;
}

int l_time(l_language::l_thread* th, int args)
{
    th->push_return({ (l_language::l_int)(std::time(nullptr)) });
    return 1;
}

int l_mstime(l_language::l_thread* th, int args)
{
    auto mstime = (l_language::l_int)(std::chrono::duration_cast< std::chrono::milliseconds >(
        std::chrono::system_clock().now().time_since_epoch()
    ).count());
    th->push_return({ mstime });
    return 1;
}

static void sleep_ms(int milliseconds)
{
#ifdef WIN32
    Sleep(milliseconds);
#elif _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
#else
    if (milliseconds >= 1000)
      sleep(milliseconds / 1000);
    usleep((milliseconds % 1000) * 1000);
#endif
}

int l_sleep(l_language::l_thread* th, int args)
{
    //have an arg?
    if (args <= 0) return -1;
    //get variable
    auto& command = th->value(0);
    //isn't a string?
    if (!command.is_int()) return -1;
    //else execute
    sleep_ms(command.to_int());
    //number of ret values
    return 0;
}

int l_exit(l_language::l_thread* th, int args)
{
    //have an arg?
    if (args <= 0) return -1;
    //get variable
    auto& command = th->value(0);
    //isn't a int?
    if (!command.is_int()) return -1;
    // Exit
    exit(command.to_int());
    // Ok
    return 0;
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
            { "mstime",  l_mstime              },
            { "exit",    l_exit                },
            { "sleep",   l_sleep               }
        };
        return l_lib;
    }
}