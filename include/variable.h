//
//  variable.h
//  LLanguage
//
//  Created by Gabriele on 16/01/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#pragma once

#include <gc.h>
#include <assert.h>
#include <string>

namespace l_language
{
    //types
    class l_vm;
    class l_thread;
    typedef int (*l_cfunction) (l_thread*,int args);
    //variable
    class l_variable
    {
    public:
        
        //types
        enum type
        {
            INT,
            FLOAT,
            STRING,
            FUNCTION,
            CFUNCTION,
            OBJECT
        };
        
        union value
        {
            int   	      m_i;
            float  	 	  m_f;
            l_obj*        m_pobj;
            std::string*  m_pstr;
            l_cfunction   m_pcfun;
        };
        
        //attributes
        type   m_type{ INT };
        value  m_value{ 0 };
        
        l_variable() : l_variable(0) {}
        
        
        l_variable(bool value)
        {
            m_type      = INT;
            m_value.m_i = value != 0;
        }
        
        l_variable(int i)
        {
            m_type      = INT;
            m_value.m_i = i;
        }
        
        l_variable(float f)
        {
            m_type      = FLOAT;
            m_value.m_f = f;
        }
        
        l_variable(const char* cstr)
        {
            m_type         = STRING;
            m_value.m_pstr = new std::string(cstr);
        }
        
        l_variable(const std::string& str)
        {
            m_type         = STRING;
            m_value.m_pstr = new std::string(str);
        }
        
        l_variable(l_obj* obj)
        {
            m_type         = OBJECT;
            m_value.m_pobj = obj;
        }
        
        l_variable(const l_cfunction cfun)
        {
            m_type          = CFUNCTION;
            m_value.m_pcfun = cfun;
        }
        
        l_variable(const l_variable& value)
        {
            m_type = value.m_type;
            
            if (m_type == STRING)
            {
                m_value.m_pstr = new std::string(*value.m_value.m_pstr);
            }
            else if (m_type == OBJECT)
            {
                m_value = value.m_value;
            }
            else
            {
                m_value = value.m_value;
            }
        }
        
        l_variable& operator = (const l_variable& value)
        {
            //delete event..
            if (m_type == STRING) delete m_value.m_pstr;
            //copy type
            m_type = value.m_type;
            //copy value
            if (m_type == STRING)
            {
                m_value.m_pstr = new std::string(*value.m_value.m_pstr);
            }
            else if (m_type == OBJECT)
            {
                m_value = value.m_value;
            }
            else
            {
                m_value = value.m_value;
            }
            return *this;
        }
        
        virtual ~l_variable()
        {
            if (m_type == STRING)
            {
                delete m_value.m_pstr;
            }
        }
        
        bool is_false()
        {
            if (m_type == INT || m_type == FUNCTION)
            {
                return m_value.m_i == 0;
            }
            if(m_type == FLOAT)
            {
                return m_value.m_f == 0.0;
            }
            if(m_type == CFUNCTION)
            {
                return m_value.m_pcfun == nullptr;
            }
            if(m_type == OBJECT)
            {
                return m_value.m_pobj == nullptr;
            }
            if(m_type == STRING)
            {
                return m_value.m_pstr == nullptr;
            }
            return false;
        }
        
        bool is_true()
        {
            return !is_false();
        }
        
        l_variable operator + (l_variable var)
        {
            if (m_type == INT && var.m_type == INT)
            {
                return m_value.m_i + var.m_value.m_i;
            }
            
            if (m_type == FLOAT && var.m_type == INT)
            {
                return m_value.m_f + var.m_value.m_i;
            }
            
            if (m_type == INT && var.m_type == FLOAT)
            {
                return m_value.m_i + var.m_value.m_f;
            }
            
            if (m_type == FLOAT && var.m_type == FLOAT)
            {
                return m_value.m_f + var.m_value.m_f;
            }
            
            if (m_type == STRING && var.m_type == STRING)
            {
                return *m_value.m_pstr + *var.m_value.m_pstr;
            }
            
            if (m_type == STRING && var.m_type == FLOAT)
            {
                return *m_value.m_pstr + std::to_string(var.m_value.m_f);
            }
            
            if (m_type == STRING && var.m_type == INT)
            {
                return *m_value.m_pstr + std::to_string(var.m_value.m_i);
            }
            
            if (m_type == FLOAT && var.m_type == STRING)
            {
                return std::to_string(m_value.m_f) + *var.m_value.m_pstr;
            }
            
            if (m_type == INT && var.m_type == STRING)
            {
                return std::to_string(m_value.m_i) + *var.m_value.m_pstr;
            }
            
            assert(0);
            return l_variable();
        }
        
        l_variable operator - (l_variable var)
        {
            if (m_type == INT && var.m_type == INT)
            {
                return m_value.m_i - var.m_value.m_i;
            }
            
            if (m_type == FLOAT && var.m_type == INT)
            {
                return m_value.m_f - var.m_value.m_i;
            }
            
            if (m_type == INT && var.m_type == FLOAT)
            {
                return m_value.m_i - var.m_value.m_f;
            }
            
            if (m_type == FLOAT && var.m_type == FLOAT)
            {
                return m_value.m_f - var.m_value.m_f;
            }
            
            assert(0);
            return l_variable();
        }
        
        l_variable operator / (l_variable var)
        {
            if (m_type == INT && var.m_type == INT)
            {
                return m_value.m_i / var.m_value.m_i;
            }
            
            if (m_type == FLOAT && var.m_type == INT)
            {
                return m_value.m_f / var.m_value.m_i;
            }
            
            if (m_type == INT && var.m_type == FLOAT)
            {
                return m_value.m_i / var.m_value.m_f;
            }
            
            if (m_type == FLOAT && var.m_type == FLOAT)
            {
                return m_value.m_f / var.m_value.m_f;
            }
            
            assert(0);
            return l_variable();
        }
        
        l_variable operator * (l_variable var)
        {
            if (m_type == INT && var.m_type == INT)
            {
                return m_value.m_i * var.m_value.m_i;
            }
            
            if (m_type == FLOAT && var.m_type == INT)
            {
                return m_value.m_f * var.m_value.m_i;
            }
            
            if (m_type == INT && var.m_type == FLOAT)
            {
                return m_value.m_i * var.m_value.m_f;
            }
            
            if (m_type == FLOAT && var.m_type == FLOAT)
            {
                return m_value.m_f * var.m_value.m_f;
            }
            
            assert(0);
            return l_variable();
        }
        
        l_variable operator == (l_variable var)
        {
            if (m_type == INT && var.m_type == INT)
            {
                return l_variable(m_value.m_i == var.m_value.m_i);
            }
            
            if (m_type == FLOAT && var.m_type == INT)
            {
                return l_variable(m_value.m_f == var.m_value.m_i);
            }
            
            if (m_type == INT && var.m_type == FLOAT)
            {
                return l_variable(m_value.m_i == var.m_value.m_f);
            }
            
            if (m_type == FLOAT && var.m_type == FLOAT)
            {
                return l_variable(m_value.m_f == var.m_value.m_f);
            }
            
            assert(0);
            return l_variable();
        }
        
        l_variable operator != (l_variable var)
        {
            if (m_type == INT && var.m_type == INT)
            {
                return l_variable(m_value.m_i != var.m_value.m_i);
            }
            
            if (m_type == FLOAT && var.m_type == INT)
            {
                return l_variable(m_value.m_f != var.m_value.m_i);
            }
            
            if (m_type == INT && var.m_type == FLOAT)
            {
                return l_variable(m_value.m_i != var.m_value.m_f);
            }
            
            if (m_type == FLOAT && var.m_type == FLOAT)
            {
                return l_variable(m_value.m_f != var.m_value.m_f);
            }
            
            assert(0);
            return true;
        }
        
        l_variable operator > (l_variable var)
        {
            if (m_type == INT && var.m_type == INT)
            {
                return l_variable(m_value.m_i > var.m_value.m_i);
            }
            
            if (m_type == FLOAT && var.m_type == INT)
            {
                return l_variable(m_value.m_f > var.m_value.m_i);
            }
            
            if (m_type == INT && var.m_type == FLOAT)
            {
                return l_variable(m_value.m_i > var.m_value.m_f);
            }
            
            if (m_type == FLOAT && var.m_type == FLOAT)
            {
                return l_variable(m_value.m_f > var.m_value.m_f);
            }
            
            assert(0);
            return l_variable(false);
        }
        
        l_variable operator >= (l_variable var)
        {
            if (m_type == INT && var.m_type == INT)
            {
                return l_variable(m_value.m_i >= var.m_value.m_i);
            }
            
            if (m_type == FLOAT && var.m_type == INT)
            {
                return l_variable(m_value.m_f >= var.m_value.m_i);
            }
            
            if (m_type == INT && var.m_type == FLOAT)
            {
                return l_variable(m_value.m_i >= var.m_value.m_f);
            }
            
            if (m_type == FLOAT && var.m_type == FLOAT)
            {
                return l_variable(m_value.m_f >= var.m_value.m_f);
            }
            
            assert(0);
            return l_variable(false);
        }
        
        l_variable operator < (l_variable var)
        {
            if (m_type == INT && var.m_type == INT)
            {
                return l_variable(m_value.m_i < var.m_value.m_i);
            }
            
            if (m_type == FLOAT && var.m_type == INT)
            {
                return l_variable(m_value.m_f < var.m_value.m_i);
            }
            
            if (m_type == INT && var.m_type == FLOAT)
            {
                return l_variable(m_value.m_i < var.m_value.m_f);
            }
            
            if (m_type == FLOAT && var.m_type == FLOAT)
            {
                return l_variable(m_value.m_f < var.m_value.m_f);
            }
            
            assert(0);
            return l_variable(false);
        }
        
        l_variable operator <= (l_variable var)
        {
            if (m_type == INT && var.m_type == INT)
            {
                return l_variable(m_value.m_i <= var.m_value.m_i);
            }
            
            if (m_type == FLOAT && var.m_type == INT)
            {
                return l_variable(m_value.m_f <= var.m_value.m_i);
            }
            
            if (m_type == INT && var.m_type == FLOAT)
            {
                return l_variable(m_value.m_i <= var.m_value.m_f);
            }
            
            if (m_type == FLOAT && var.m_type == FLOAT)
            {
                return l_variable(m_value.m_f <= var.m_value.m_f);
            }
            
            assert(0);
            return l_variable(false);
        }
        
        l_variable operator && (l_variable var)
        {
            if (m_type == INT && var.m_type == INT)
            {
                return l_variable(m_value.m_i && var.m_value.m_i);
            }
            
            if (m_type == FLOAT && var.m_type == INT)
            {
                return l_variable(m_value.m_f && var.m_value.m_i);
            }
            
            if (m_type == INT && var.m_type == FLOAT)
            {
                return l_variable(m_value.m_i && var.m_value.m_f);
            }
            
            if (m_type == FLOAT && var.m_type == FLOAT)
            {
                return l_variable(m_value.m_f && var.m_value.m_f);
            }
            
            assert(0);
            return l_variable(false);
        }
        
        l_variable operator || (l_variable var)
        {
            if (m_type == INT && var.m_type == INT)
            {
                return l_variable(m_value.m_i || var.m_value.m_i);
            }
            
            if (m_type == FLOAT && var.m_type == INT)
            {
                return l_variable(m_value.m_f || var.m_value.m_i);
            }
            
            if (m_type == INT && var.m_type == FLOAT)
            {
                return l_variable(m_value.m_i || var.m_value.m_f);
            }
            
            if (m_type == FLOAT && var.m_type == FLOAT)
            {
                return l_variable(m_value.m_f || var.m_value.m_f);
            }
            
            assert(0);
            return l_variable(false);
        }
        
        l_variable operator - (void)
        {
            if (m_type == INT)
            {
                return l_variable(-m_value.m_i);
            }
            
            if (m_type == FLOAT)
            {
                return l_variable(-m_value.m_f);
            }
            
            assert(0);
            
            return *this;
        }
        
        l_variable operator ! (void)
        {
            return l_variable(is_false());
        }
        
        //gc methos
        
        bool is_object()
        {
            return m_type == OBJECT;
        }
        
        bool is_marked() const
        {
            return m_value.m_pobj->is_marked();
        }
        
        bool is_unmarked() const
        {
            return m_value.m_pobj->is_unmarked();
        }
        
        void mark()
        {
            assert(m_value.m_pobj);
            m_value.m_pobj->mark(); 
        }
        
        void unmark()
        {
            assert(m_value.m_pobj);
            m_value.m_pobj->unmark();
        }
        
        
    };
};
