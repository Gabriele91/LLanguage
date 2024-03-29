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
    #ifdef L_SINGLE_PRECISION
        typedef int l_int; 
        typedef float l_float; 
        #define l_int_printf_tag "%d"
        #define l_float_printf_tag "%g"
    #else
        typedef long long l_int; 
        typedef double l_float; 
        #define l_int_printf_tag "%lld"
        #define l_float_printf_tag "%g"
    #endif
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
            l_int   	  m_i;
            l_float  	  m_f;
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

        l_variable(l_variable&& var)
        {
            // Move type
            m_type = std::move(var.m_type);
            // Move value
            m_value = std::move(var.m_value);
            // Move const delete
            m_const = std::move(var.m_const);
            // Reset values
            var.m_type = LNULL;
            var.m_value = value { 0 };
            var.m_const = false;
        }

        l_variable(bool value)
        {
            m_type      = LBOOL;
            m_value.m_b = value;
        }
        
        l_variable(l_int i)
        {
            m_type      = INT;
            m_value.m_i = i;
        }
        
        l_variable(l_float f)
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

        l_variable& operator = (l_variable&& var)
        {
            //copy type
            m_type = std::move(var.m_type);
            //copy value
            m_value = std::move(var.m_value); 
            //const delete
            m_const = std::move(var.m_const);
            // reset values
            var.m_type = LNULL;
            var.m_value = value { 0 };
            var.m_const = false;
            //this
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
                   m_type == CLOSER      ||
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
        
        l_int to_int() const
        {
                  if(is_int())   return m_value.m_i;
            else if(is_float())  return (l_int)m_value.m_f;
            else if(is_bool())   return (l_int)m_value.m_b;
            else if(is_null())   return 0;
            //bad cast
			return m_value.m_i;
        }
        
        l_float to_float() const
        {
                 if(is_float()) return          m_value.m_f;
            else if(is_int())   return (l_float)m_value.m_i;
            else if(is_bool())  return (l_float)m_value.m_b;
            else if(is_null())  return (l_float)0.0f;
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

        l_closer* closer();
        
        const l_table* table() const;
        
        const l_array* array() const;
        
        const l_iterator* iterator() const;
        
        const l_string* string() const;
        
        const l_class* clazz() const;
        
        const l_object* object() const;

        const l_closer* closer() const;
        
        bool is_false() const
        {
            switch(m_type)
            {
                case LBOOL:
                    return !m_value.m_b;
                case LNULL:
                    return true;
                case INT:
                    return m_value.m_i == 0;
                case FUNCTION:
                    return m_value.m_fid == 0;
                case FLOAT:
                    return m_value.m_f == 0.0;
                case REF:
                case STRING:
                case TABLE:
                case ARRAY:
                case ITERATOR:
                case CLASS:
                case OBJECT:
                case CLOSER:
                case COBJECT:
                case CFUNCTION:
                    return m_value.m_pobj == nullptr;
                default:
                    return false;
            }
        }
        
        bool is_true() const
        {
            return !is_false();
        }
        
        bool add(l_variable& var,l_variable& output);
        
        bool sub(l_variable& var,l_variable& output) const;
        
        bool mul(l_variable& var,l_variable& output) const;
        
        bool div(l_variable& var,l_variable& output) const;
        
        bool equal(l_variable& var,l_variable& output) const;
        
        bool not_equal(l_variable& var,l_variable& output) const;
        
        bool unm(l_variable& output) const;
        
        bool not_value(l_variable& output) const;
        
        bool lt (l_variable& var, l_variable& output) const;
        
        bool le (l_variable& var, l_variable& output) const;
        
        bool rt (l_variable& var, l_variable& output) const;
        
        bool re (l_variable& var, l_variable& output) const;
        
        bool and_value (l_variable& var, l_variable& output) const;
        
        bool or_value (l_variable& var, l_variable& output) const;
        
        bool mod (l_variable& var, l_variable& output) const;
        
        
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
            if (is_ref_obj())
                return m_value.m_pobj->is_marked();
            return false;
        }
        
        bool is_unmarked() const
        {
            if (is_ref_obj())
                return m_value.m_pobj->is_unmarked();
            return false;
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