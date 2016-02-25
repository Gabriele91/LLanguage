//
//  l_variable.h
//  LLanguage
//
//  Created by Gabriele on 16/01/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#pragma once

#include <vector>
#include <assert.h>
#include <l_object.h>
#include <l_string.h>


namespace l_language
{
    //types
    class l_vm;
    class l_thread;
    class l_vector;
    class l_gc;
    class l_table;
    typedef int (*l_cfunction) (l_thread*,int args);
    typedef unsigned int l_function_id;
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
            l_function_id m_fid;
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
        
        l_variable(l_function_id f_id)
        {
            m_type        = FUNCTION;
            m_value.m_fid = f_id;
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
                 if(is_int())   return      m_value.m_i;
            else if(is_float()) return (int)m_value.m_f;
            assert(0);
			return 0;
        }
        
        float to_float()
        {
                 if(is_float()) return        m_value.m_f;
            else if(is_int())   return (float)m_value.m_i;
            assert(0);
			return 0;
        }
        
        std::string to_string()
        {
                 if(is_float()) return std::to_string(m_value.m_f);
            else if(is_int())   return std::to_string(m_value.m_i);
            else if(is_string())return string()->str();
            assert(0);
        }
        
        template< class T >
        T* to()
        {
            return dynamic_cast< T* >( m_value.m_pobj );
        }
        
        template< class T >
        const T* to() const
        {
            return dynamic_cast< const T* >( m_value.m_pobj );
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
            if (m_type == INT)
            {
                return m_value.m_i == 0;
            }
            if (m_type == FUNCTION)
            {
                return m_value.m_fid == 0;
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
        
        bool add(l_variable& var,l_variable& output)
        {
            if (m_type == INT && var.m_type == INT)
            {
                output = m_value.m_i + var.m_value.m_i;
                return true;
            }
            
            if (m_type == FLOAT && var.m_type == INT)
            {
                output = m_value.m_f + var.m_value.m_i;
                return true;
            }
            
            if (m_type == INT && var.m_type == FLOAT)
            {
                output = m_value.m_i + var.m_value.m_f;
                return true;
            }
            
            if (m_type == FLOAT && var.m_type == FLOAT)
            {
                output = m_value.m_f + var.m_value.m_f;
                return true;
            }
            
            if (is_string() && var.is_string())
            {
                output = string()->gc_merge(var.string());
                return true;
            }
            
            if (is_string()&& var.m_type == FLOAT)
            {
                output = string()->gc_merge(std::to_string(var.m_value.m_f));
                return true;
            }
            
            if (is_string() && var.m_type == INT)
            {
                output = string()->gc_merge(std::to_string(var.m_value.m_i));
                return true;
            }
            
            if (m_type == FLOAT && var.is_string())
            {
                output = var.string()->gc_merge_left(std::to_string(var.m_value.m_f));
                return true;
            }
            
            if (m_type == INT && var.is_string())
            {
                output = var.string()->gc_merge_left(std::to_string(m_value.m_i));
                return true;
            }
            
            return false;
        }
        
        bool sub(l_variable& var,l_variable& output)
        {
            if (m_type == INT && var.m_type == INT)
            {
                output = m_value.m_i - var.m_value.m_i;
                return true;
            }
            
            if (m_type == FLOAT && var.m_type == INT)
            {
                output = m_value.m_f - var.m_value.m_i;
                return true;
            }
            
            if (m_type == INT && var.m_type == FLOAT)
            {
                output = m_value.m_i - var.m_value.m_f;
                return true;
            }
            
            if (m_type == FLOAT && var.m_type == FLOAT)
            {
                output = m_value.m_f - var.m_value.m_f;
                return true;
            }
            
            return false;
        }
        
        bool mul(l_variable& var,l_variable& output)
        {
            if (m_type == INT && var.m_type == INT)
            {
                output = m_value.m_i * var.m_value.m_i;
                return true;
            }
            
            if (m_type == FLOAT && var.m_type == INT)
            {
                output = m_value.m_f * var.m_value.m_i;
                return true;
            }
            
            if (m_type == INT && var.m_type == FLOAT)
            {
                output = m_value.m_i * var.m_value.m_f;
                return true;
            }
            
            if (m_type == FLOAT && var.m_type == FLOAT)
            {
                output = m_value.m_f * var.m_value.m_f;
                return true;
            }
            
            return false;
        }
        
        bool div(l_variable& var,l_variable& output)
        {
            if (m_type == INT && var.m_type == INT)
            {
                output = m_value.m_i / var.m_value.m_i;
                return true;
            }
            
            if (m_type == FLOAT && var.m_type == INT)
            {
                output = m_value.m_f / var.m_value.m_i;
                return true;
            }
            
            if (m_type == INT && var.m_type == FLOAT)
            {
                output = m_value.m_i / var.m_value.m_f;
                return true;
            }
            
            if (m_type == FLOAT && var.m_type == FLOAT)
            {
                output = m_value.m_f / var.m_value.m_f;
                return true;
            }
            
            return false;
        }
        
        bool equal(l_variable& var,l_variable& output)
        {
            if (m_type == INT && var.m_type == INT)
            {
                output = l_variable(m_value.m_i == var.m_value.m_i);
                return true;
            }
            
            if (m_type == FLOAT && var.m_type == INT)
            {
                output = l_variable(m_value.m_f == var.m_value.m_i);
                return true;
            }
            
            if (m_type == INT && var.m_type == FLOAT)
            {
                output = l_variable(m_value.m_i == var.m_value.m_f);
                return true;
            }
            
            if (m_type == FLOAT && var.m_type == FLOAT)
            {
                output = l_variable(m_value.m_f == var.m_value.m_f);
                return true;
            }
            
            if (m_type == STRING && var.m_type == STRING)
            {
                output = l_variable(string()->str() == var.string()->str());
                return true;
            }
            
            output = false;
            return false;
        }
        
        bool not_equal(l_variable& var,l_variable& output)
        {
            if (m_type == INT && var.m_type == INT)
            {
                output = l_variable(m_value.m_i != var.m_value.m_i);
                return true;
            }
            
            if (m_type == FLOAT && var.m_type == INT)
            {
                output = l_variable(m_value.m_f != var.m_value.m_i);
                return true;
            }
            
            if (m_type == INT && var.m_type == FLOAT)
            {
                output = l_variable(m_value.m_i != var.m_value.m_f);
                return true;
            }
            
            if (m_type == FLOAT && var.m_type == FLOAT)
            {
                output = l_variable(m_value.m_f != var.m_value.m_f);
                return true;
            }
            
            if (m_type == STRING && var.m_type == STRING)
            {
                output = l_variable(string()->str() != var.string()->str());
                return true;
            }
            
            output = true;
            return false;
        }
        
        bool unm(l_variable& output)
        {
            if (m_type == INT)
            {
                output = l_variable(-m_value.m_i);
                return true;
            }
            
            if (m_type == FLOAT)
            {
                output = l_variable(-m_value.m_f);
                return true;
            }
            
            return false;
        }
        
        bool not_value(l_variable& output)
        {
            output = l_variable(is_false());
            return true;
        }
        
        bool lt (l_variable& var, l_variable& output)
        {
            if (m_type == INT && var.m_type == INT)
            {
                output = l_variable(m_value.m_i < var.m_value.m_i);
                return true;
            }
            
            if (m_type == FLOAT && var.m_type == INT)
            {
                output = l_variable(m_value.m_f < var.m_value.m_i);
                return true;
            }
            
            if (m_type == INT && var.m_type == FLOAT)
            {
                output = l_variable(m_value.m_i < var.m_value.m_f);
                return true;
            }
            
            if (m_type == FLOAT && var.m_type == FLOAT)
            {
                output = l_variable(m_value.m_f < var.m_value.m_f);
                return true;
            }
            
            output = l_variable(false);
            return false;
        }
        
        
        bool le (l_variable& var, l_variable& output)
        {
            if (m_type == INT && var.m_type == INT)
            {
                output = l_variable(m_value.m_i <= var.m_value.m_i);
                return true;
            }
            
            if (m_type == FLOAT && var.m_type == INT)
            {
                output = l_variable(m_value.m_f <= var.m_value.m_i);
                return true;
            }
            
            if (m_type == INT && var.m_type == FLOAT)
            {
                output = l_variable(m_value.m_i <= var.m_value.m_f);
                return true;
            }
            
            if (m_type == FLOAT && var.m_type == FLOAT)
            {
                output = l_variable(m_value.m_f <= var.m_value.m_f);
                return true;
            }
            
            output = l_variable(false);
            return false;
        }
        
        bool rt (l_variable& var, l_variable& output)
        {
            if (m_type == INT && var.m_type == INT)
            {
                output = l_variable(m_value.m_i > var.m_value.m_i);
                return true;
            }
            
            if (m_type == FLOAT && var.m_type == INT)
            {
                output = l_variable(m_value.m_f > var.m_value.m_i);
                return true;
            }
            
            if (m_type == INT && var.m_type == FLOAT)
            {
                output = l_variable(m_value.m_i > var.m_value.m_f);
                return true;
            }
            
            if (m_type == FLOAT && var.m_type == FLOAT)
            {
                output = l_variable(m_value.m_f > var.m_value.m_f);
                return true;
            }
            
            output = l_variable(false);
            return false;
        }
        
        
        bool re (l_variable& var, l_variable& output)
        {
            if (m_type == INT && var.m_type == INT)
            {
                output = l_variable(m_value.m_i >= var.m_value.m_i);
                return true;
            }
            
            if (m_type == FLOAT && var.m_type == INT)
            {
                output = l_variable(m_value.m_f >= var.m_value.m_i);
                return true;
            }
            
            if (m_type == INT && var.m_type == FLOAT)
            {
                output = l_variable(m_value.m_i >= var.m_value.m_f);
                return true;
            }
            
            if (m_type == FLOAT && var.m_type == FLOAT)
            {
                output = l_variable(m_value.m_f >= var.m_value.m_f);
                return true;
            }
            
            output = l_variable(false);
            return false;
        }
        
        
        bool and_value (l_variable& var, l_variable& output)
        {
            if (m_type == INT && var.m_type == INT)
            {
                output = l_variable(m_value.m_i && var.m_value.m_i);
                return true;
            }
            
            if (m_type == FLOAT && var.m_type == INT)
            {
                output = l_variable(m_value.m_f && var.m_value.m_i);
                return true;
            }
            
            if (m_type == INT && var.m_type == FLOAT)
            {
                output = l_variable(m_value.m_i && var.m_value.m_f);
                return true;
            }
            
            if (m_type == FLOAT && var.m_type == FLOAT)
            {
                output = l_variable(m_value.m_f && var.m_value.m_f);
                return true;
            }
            
            output = l_variable(false);
            return false;
        }
        
        
        bool or_value (l_variable& var, l_variable& output)
        {
            if (m_type == INT && var.m_type == INT)
            {
                output = l_variable(m_value.m_i || var.m_value.m_i);
                return true;
            }
            
            if (m_type == FLOAT && var.m_type == INT)
            {
                output = l_variable(m_value.m_f || var.m_value.m_i);
                return true;
            }
            
            if (m_type == INT && var.m_type == FLOAT)
            {
                output = l_variable(m_value.m_i || var.m_value.m_f);
                return true;
            }
            
            if (m_type == FLOAT && var.m_type == FLOAT)
            {
                output = l_variable(m_value.m_f || var.m_value.m_f);
                return true;
            }
            
            output = l_variable(false);
            return false;
        }
        
        
        l_variable operator + (l_variable var)
        {
            l_variable output;
            add(var,output);
            return output;
        }
        
        l_variable operator - (l_variable var)
        {
            l_variable output;
            sub(var,output);
            return output;
        }
        
        l_variable operator * (l_variable var)
        {
            l_variable output;
            mul(var,output);
            return output;
        }
        
        l_variable operator / (l_variable var)
        {
            l_variable output;
            div(var,output);
            return output;
        }
        
        l_variable operator == (l_variable var)
        {
            l_variable output;
            equal(var,output);
            return output;
        }
        
        l_variable operator != (l_variable var)
        {
            l_variable output;
            not_equal(var,output);
            return output;
        }
        
        l_variable operator - (void)
        {
            l_variable output;
            unm(output);
            return output;
        }
        
        l_variable operator < (l_variable var)
        {
            l_variable output;
            lt(var,output);
            return output;
        }
        
        l_variable operator <= (l_variable var)
        {
            l_variable output;
            le(var,output);
            return output;
        }

        l_variable operator > (l_variable var)
        {
            l_variable output;
            rt(var,output);
            return output;
        }
        
        l_variable operator >= (l_variable var)
        {
            l_variable output;
            re(var,output);
            return output;
        }
        
        l_variable operator && (l_variable var)
        {
            l_variable output;
            and_value(var,output);
            return output;
        }
        
        l_variable operator || (l_variable var)
        {
            l_variable output;
            or_value(var,output);
            return output;
        }
        
        l_variable operator ! (void)
        {
            l_variable output;
            not_value(output);
            return output;
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
    
    //list variables
    using l_list_variables = std::vector < l_variable >;

};