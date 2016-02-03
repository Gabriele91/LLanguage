//
//  l_variable.h
//  LLanguage
//
//  Created by Gabriele on 16/01/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#pragma once

#include <assert.h>
#include <l_gc.h>
#include <l_string.h>


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
            FUNCTION,
            CFUNCTION,
            STRING,
            OBJECT
        };
        
        union value
        {
            int   	      m_i;
            float  	 	  m_f;
            l_obj*        m_pobj;
            l_cfunction   m_pcfun;
        };
        
        //attributes
        type   m_type  { INT };
        value  m_value { 0 };
        bool   m_const { false };
        
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
        
        l_variable(l_obj* obj)
        {
            m_type         = OBJECT;
            m_value.m_pobj = obj;
        }
        
        l_variable(l_string* obj)
        {
            m_type         = STRING;
            m_value.m_pobj = (l_obj*)obj;
        }
        
        l_variable(const l_cfunction cfun)
        {
            m_type          = CFUNCTION;
            m_value.m_pcfun = cfun;
        }
        
        l_variable(const l_variable& value)
        {
            (*this) = value;
        }
        
        l_variable& operator = (const l_variable& value)
        {
            //copy type
            m_type = value.m_type;
            //copy value
            m_value = value.m_value;
            //const delete
            m_const = value.m_const;
            //strings
            return *this;
        }
        
        l_gc* get_gc()
        {
            return  m_value.m_pobj->get_gc();
        }
        
        const l_gc* get_gc() const
        {
            return  m_value.m_pobj->get_gc();
        }
        
        bool is_int() const
        {
            return m_type == INT;
        }
        
        bool is_float() const
        {
            return m_type == FLOAT;
        }
        
        bool is_string() const
        {
            return m_type == STRING;
        }
        
        bool is_object() const
        {
            return m_type == OBJECT || m_type == STRING;
        }
        
        bool is_function() const
        {
            return m_type == FUNCTION;
        }
        
        bool is_cfunction() const
        {
            return m_type == CFUNCTION;
        }
        
        int to_int()
        {
                 if(is_int())   return m_value.m_i;
            else if(is_float()) return m_value.m_f;
            assert(0);
        }
        
        float to_float()
        {
                 if(is_float()) return m_value.m_f;
            else if(is_int())   return m_value.m_i;
            assert(0);
        }
        
        std::string to_string()
        {
                 if(is_float()) return std::to_string(m_value.m_f);
            else if(is_int())   return std::to_string(m_value.m_i);
            else if(is_string())return string()->str();
            assert(0);
        }

        l_string* string()
        {
            return dynamic_cast< l_string* >( m_value.m_pobj );
        }
        
        const l_string* string() const
        {
            return dynamic_cast< const l_string* >( m_value.m_pobj );
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
            if(m_type == STRING)
            {
                return m_value.m_pobj == nullptr;
            }
            if(m_type == OBJECT)
            {
                return m_value.m_pobj == nullptr;
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
            
            if (is_string() && var.is_string())
            {
                return string()->gc_merge(var.string());
            }
            
            if (is_string()&& var.m_type == FLOAT)
            {
                return string()->gc_merge(std::to_string(var.m_value.m_f));
            }
            
            if (is_string() && var.m_type == INT)
            {
                return string()->gc_merge(std::to_string(var.m_value.m_i));
            }
            
            if (m_type == FLOAT && var.is_string())
            {
                return var.string()->gc_merge_left(std::to_string(var.m_value.m_f));
            }
            
            if (m_type == INT && var.is_string())
            {
                return var.string()->gc_merge_left(std::to_string(m_value.m_i));
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
            
            if (m_type == STRING && var.m_type == STRING)
            {
                return l_variable(string()->str() == var.string()->str());
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
            
            if (m_type == STRING && var.m_type == STRING)
            {
                return l_variable(string()->str() != var.string()->str());
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
