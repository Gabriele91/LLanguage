//
//  l_iterator.h
//  LLanguage
//
//  Created by Gabriele on 04/04/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#pragma once
#include <l_ref.h>
namespace l_language
{
    class l_iterator : public l_ref
    {
    public:
        virtual bool valid() const       = 0;
        virtual l_variable get() const   = 0;
        virtual l_variable next() const  = 0;
        virtual l_variable get_id() const= 0;
        virtual void self_next()         = 0;
    };
};