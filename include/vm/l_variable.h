//
//  l_variable.h
//  LLanguage
//
//  Created by Gabriele on 16/01/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#pragma once
#include <cmath>
#include <vector>
#include <assert.h>
#include <l_ref.h>
#include <l_string.h>


namespace l_language
{
    //types
    class l_vm;
    class l_thread;
    class l_array;
    class l_closer;
    class l_iterator;
    class l_gc;
    class l_table;
    class l_class;
    class l_object;
    typedef int (*l_cfunction) (l_thread*,int args);
    typedef unsigned int l_function_id;
    //variable
    class l_variable
    {
    public:
        
        //types
        enum type
        {
            LNULL,
            LBOOL,
            INT,
            FLOAT,
            CLOSER,
            FUNCTION,
            CFUNCTION,
            STRING,
            ARRAY,
            TABLE,
            ITERATOR,
            CLASS,
            OBJECT,
            COBJECT,
            REF
        };
        
        union value
        {
            bool          m_b;
            int   	      m_i;
            float  	 	  m_f;
            l_ref*        m_pobj;
            l_cfunction   m_pcfun;
            l_function_id m_fid;
        };
        
        //attributes
        type   m_type  { LNULL };
        value  m_value { 0 };
        bool   m_const { false };
        
        l_variable()
        {
            //none
        }
        
        l_variable(bool value)
        {
            m_type      = LBOOL;
            m_value.m_b = value;
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
        
        l_variable(l_string* obj)
        {
            m_type         = STRING;
            m_value.m_pobj = (l_ref*)obj;
        }
        
        l_variable(l_array* obj)
        {
            m_type         = ARRAY;
            m_value.m_pobj = (l_ref*)obj;
        }
        
        l_variable(l_table* obj)
        {
            m_type         = TABLE;
            m_value.m_pobj = (l_ref*)obj;
        }
        
        l_variable(l_iterator* obj)
        {
            m_type         = ITERATOR;
            m_value.m_pobj = (l_ref*)obj;
        }
        
        l_variable(l_ref* obj)
        {
            m_type         = REF;
            m_value.m_pobj = obj;
        }
        
        l_variable(const l_cfunction cfun)
        {
            m_type          = CFUNCTION;
            m_value.m_pcfun = cfun;
        }
        
        l_variable(l_function_id f_id)
        {
            m_type        = FUNCTION;
            m_value.m_fid = f_id;
        }
        
        l_variable(l_closer* obj)
        {
            m_type         = CLOSER;
            m_value.m_pobj = (l_ref*)obj;
        }
        
        l_variable(l_class* obj)
        {
            m_type         = CLASS;
            m_value.m_pobj = (l_ref*)obj;
        }
        
        l_variable(l_object* obj)
        {
            m_type         = OBJECT;
            m_value.m_pobj = (l_ref*)obj;
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
        
        bool is_null() const
        {
            return m_type == LNULL;
        }
        
        bool is_bool() const
        {
            return m_type == LBOOL;
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
        
        bool is_array() const
        {
            return m_type == ARRAY;
        }
        
        bool is_table() const
        {
            return m_type == TABLE;
        }
        
        bool is_iterator() const
        {
            return m_type == ITERATOR;
        }
        
        bool is_ref_obj() const
        {
            return m_type == REF         ||
                   m_type == STRING      ||
                   m_type == TABLE       ||
                   m_type == ARRAY       ||
                   m_type == ITERATOR    ||
                   m_type == CLASS       ||
                   m_type == OBJECT      ||
                   m_type == COBJECT;
        }
        
        bool is_object() const
        {
            return m_type == OBJECT;
        }
        
        bool is_cobject() const
        {
            return m_type == COBJECT;
        }
        
        bool is_class() const
        {
            return m_type == CLASS;
        }
        
        bool is_function() const
        {
            return m_type == FUNCTION;
        }
        
        bool is_closer() const
        {
            return m_type == CLOSER;
        }
        
        bool is_cfunction() const
        {
            return m_type == CFUNCTION;
        }
        
        int to_int() const
        {
                  if(is_int())   return m_value.m_i;
            else if(is_float())  return (int)m_value.m_f;
            else if(is_bool())   return (int)m_value.m_b;
            else if(is_null())   return 0;
            //bad cast
			return m_value.m_i;
        }
        
        float to_float() const
        {
                 if(is_float()) return        m_value.m_f;
            else if(is_int())   return (float)m_value.m_i;
            else if(is_bool())  return (float)m_value.m_b;
            else if(is_null())  return 0.0f;
            //bad cast
            return m_value.m_f;
        }
        
        bool can_to_int() const
        {
            return
               is_float()
            || is_int()
            || is_bool()
            || is_null();
        }
        
        bool can_to_float() const
        {
            return
               is_float()
            || is_int()
            || is_bool()
            || is_null();
        }
        
        std::string to_string() const;
        
        template< class T >
        T* to()
        {
            return dynamic_cast< T* >( m_value.m_pobj );
        }
        
        template< class T >
        T* static_to()
        {
            return static_cast< T* >( m_value.m_pobj );
        }
        
        template< class T >
        const T* to() const
        {
            return dynamic_cast< const T* >( m_value.m_pobj );
        }
        
        template< class T >
        const T* static_to() const
        {
            return static_cast< const T* >( m_value.m_pobj );
        }
        
        l_table* table();
        
        l_array* array();
        
        l_iterator* iterator();
        
        l_string* string();
        
        l_class* clazz();
        
        l_object* object();
        
        const l_table* table() const;
        
        const l_array* array() const;
        
        const l_iterator* iterator() const;
        
        const l_string* string() const;
        
        const l_class* clazz() const;
        
        const l_object* object() const;
        
        bool is_false()
        {
            if(m_type == LBOOL)
            {
                return !m_value.m_b;
            }
            if(m_type == LNULL)
            {
                return true;
            }
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
            if(m_type == ARRAY)
            {
                return m_value.m_pobj == nullptr;
            }
            if(m_type == TABLE)
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
        
        bool add(l_variable& var,l_variable& output);
        
        bool sub(l_variable& var,l_variable& output);
        
        bool mul(l_variable& var,l_variable& output);
        
        bool div(l_variable& var,l_variable& output);
        
        bool equal(l_variable& var,l_variable& output);
        
        bool not_equal(l_variable& var,l_variable& output);
        
        bool unm(l_variable& output);
        
        bool not_value(l_variable& output);
        
        bool lt (l_variable& var, l_variable& output);
        
        bool le (l_variable& var, l_variable& output);
        
        bool rt (l_variable& var, l_variable& output);
        
        bool re (l_variable& var, l_variable& output);
        
        bool and_value (l_variable& var, l_variable& output);
        
        bool or_value (l_variable& var, l_variable& output);
        
        bool mod (l_variable& var, l_variable& output);
        
        
        l_variable operator + (l_variable& var)
        {
            l_variable output;
            add(var,output);
            return output;
        }
        
        l_variable operator - (l_variable& var)
        {
            l_variable output;
            sub(var,output);
            return output;
        }
        
        l_variable operator * (l_variable& var)
        {
            l_variable output;
            mul(var,output);
            return output;
        }
        
        l_variable operator / (l_variable& var)
        {
            l_variable output;
            div(var,output);
            return output;
        }
        
        l_variable operator == (l_variable& var)
        {
            l_variable output;
            equal(var,output);
            return output;
        }
        
        l_variable operator != (l_variable& var)
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
        
        l_variable operator < (l_variable& var)
        {
            l_variable output;
            lt(var,output);
            return output;
        }
        
        l_variable operator <= (l_variable& var)
        {
            l_variable output;
            le(var,output);
            return output;
        }

        l_variable operator > (l_variable& var)
        {
            l_variable output;
            rt(var,output);
            return output;
        }
        
        l_variable operator >= (l_variable& var)
        {
            l_variable output;
            re(var,output);
            return output;
        }
        
        l_variable operator && (l_variable& var)
        {
            l_variable output;
            and_value(var,output);
            return output;
        }
        
        l_variable operator || (l_variable& var)
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
        
        l_variable operator % (l_variable& var)
        {
            l_variable output;
            mod(var,output);
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