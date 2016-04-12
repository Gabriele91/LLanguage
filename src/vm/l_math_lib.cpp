//
//  l_math_lib.cpp
//  LLanguage
//
//  Created by Gabriele on 12/04/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#include <l_math_lib.h>
#include <l_array.h>
#include <cmath>

template < float (*T)(float) >
int l_float_fun_float(l_language::l_thread* th, int args)
{
    //have an arg?
    if (args != 1) return -1;
    //is float?
    if (!th->value(0).can_to_float()) return -1;
    //else execute
    float sret =  T (th->value(0).to_float());
    //set return
    th->push_return(l_language::l_variable(sret));
    //number of ret values
    return 1;
}

template < float (*T)(float,float) >
int l_float_fun_float_float(l_language::l_thread* th, int args)
{
    //have an arg?
    if (args != 2) return -1;
    //is float?
    if (!th->value(0).can_to_float()) return -1;
    if (!th->value(1).can_to_float()) return -1;
    //else execute
    float sret =  T (th->value(0).to_float(),th->value(1).to_float());
    //set return
    th->push_return(l_language::l_variable(sret));
    //number of ret values
    return 1;
}


namespace l_language
{
    l_language::l_vm::l_extern_libary l_math_lib=
    {
        //trig.
        { "sin",   l_float_fun_float< std::sin  >  },
        { "cos",   l_float_fun_float< std::cos  >  },
        { "tan",   l_float_fun_float< std::tan  >  },
        { "asin",  l_float_fun_float< std::asin >  },
        { "acos",  l_float_fun_float< std::acos >  },
        { "atan",  l_float_fun_float< std::atan >  },
        { "sin",   l_float_fun_float< std::sin  >  },
        //trig. iperb.
        { "sinh",   l_float_fun_float< std::sinh  > },
        { "cosh",   l_float_fun_float< std::cosh  > },
        { "tanh",   l_float_fun_float< std::tanh  > },
        { "asinh",  l_float_fun_float< std::asinh > },
        { "acosh",  l_float_fun_float< std::acosh > },
        { "atanh",  l_float_fun_float< std::atanh > },
        //value op.
        { "abs",         l_float_fun_float< std::abs   >      },
        { "sqrt",        l_float_fun_float< std::sqrt  >      },
        { "exp",         l_float_fun_float< std::exp   >      },
        { "exp2",        l_float_fun_float< std::exp2  >      },
        { "log",         l_float_fun_float< std::log   >      },
        { "log2",        l_float_fun_float< std::log2  >      },
        { "log10",       l_float_fun_float< std::log10 >      },
        { "ceil",        l_float_fun_float< std::ceil  >      },
        { "floor",       l_float_fun_float< std::floor >      },
        { "trunc",       l_float_fun_float< std::trunc >      },
        { "round",       l_float_fun_float< std::round >      },
        { "near_by_int", l_float_fun_float< std::nearbyintf > },
        //double value op.
        { "pow",         l_float_fun_float_float< std::pow >     },
        { "hypot",       l_float_fun_float_float< std::hypot >   },
        { "copy_sign",   l_float_fun_float_float< std::copysign >},
    };
}