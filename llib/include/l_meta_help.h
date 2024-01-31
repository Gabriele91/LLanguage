//
//  l_meta_help.h
//  LLanguage
//
//  Created by Gabriele on 18/04/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#pragma once


namespace l_language
{
    namespace  l_wrapper
    {
        //ITERATOR TEMPLATE
        template <int... INDEX> struct t_index_int{};
        
        //CASE N TO 1
        template<int BEGIN, int COUNT, int...REM>
        struct t_make_int_range_type
        {
            typedef typename t_make_int_range_type<BEGIN,COUNT-1,BEGIN+COUNT-1,REM...>::type type;
        };
        //CASE 0
        template<int BEGIN, int...REM>
        struct t_make_int_range_type<BEGIN,0,REM...>
        {
            typedef t_index_int<REM...> type;
        };
        
        //BUILD RANGE TYPE
        template<int Begin, int Count>
        inline typename t_make_int_range_type<Begin,Count>::type t_make_int_range()
        {
            return typename t_make_int_range_type<Begin,Count>::type();
        }
    }
}