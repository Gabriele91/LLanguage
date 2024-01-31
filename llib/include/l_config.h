//
//  Config.h
//  LLanguage
//
//  Created by Gabriele Di Bari on 30/12/15.
//  Copyright © 2015 Gabriele Di Bari. All rights reserved.
//
#pragma once
#include <iostream>

#if defined( _DEBUG )
    #define DEBUGCODE(x) x
#else
    #define DEBUGCODE(x)
#endif

#define MESSAGE( x ) std::cout << x << std::endl;
#define LINEMESSAGE( x ) std::cout << x ;
#define WMESSAGE( x ) std::wcout << x << std::endl;
#define LINEWMESSAGE( x ) std::wcout << x ;
#define NOT( x ) (!(x))

#ifdef _WIN32
    #define ASPACKED( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop) )
    #define ALIGNED( size, __Declaration__ ) __declspec(align(size)) __Declaration__
#else
    #define ASPACKED( __Declaration__ ) __Declaration__ __attribute__((__packed__))
    #define ALIGNED( size, __Declaration__ )  __Declaration__ __attribute__ ((aligned(size)))
#endif
