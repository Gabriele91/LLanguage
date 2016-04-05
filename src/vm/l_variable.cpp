//
//  l_variable.cpp
//  LLanguage
//
//  Created by Gabriele on 19/03/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//

#include <stdio.h>
#include <l_parser.h>
#include <l_variable.h>

#include <l_vm.h>
#include <l_table.h>
#include <l_array.h>
#include <l_iterator.h>

namespace l_language
{
    static const char* str_null  = l_parser::get_keyword(l_parser::K_NULL );
    static const char* str_true  = l_parser::get_keyword(l_parser::K_TRUE );
    static const char* str_false = l_parser::get_keyword(l_parser::K_FALSE);
    #define B2CSTR(var) (var ? str_true : str_false)
    
    
    std::string l_variable::to_string() const
    {
             if(is_float()) return std::to_string(m_value.m_f);
        else if(is_int())   return std::to_string(m_value.m_i);
        else if(is_bool())  return m_value.m_b ? str_true : str_false;
        else if(is_null())  return str_null;
        else if(is_string())return string()->str();
        //bad cast
        return "";
    }
    
    l_table* l_variable::table()
    {
        return static_cast< l_table* >( m_value.m_pobj );
    }
    
    l_array* l_variable::array()
    {
        return static_cast< l_array* >( m_value.m_pobj );
    }
    
    l_iterator* l_variable::iterator()
    {
        return static_cast< l_iterator* >( m_value.m_pobj );
    }
    
    l_string* l_variable::string()
    {
        return static_cast< l_string* >( m_value.m_pobj );
    }
    
    const l_table* l_variable::table() const
    {
        return static_cast< l_table* >( m_value.m_pobj );
    }
    
    const l_array* l_variable::array() const
    {
        return static_cast< l_array* >( m_value.m_pobj );
    }
    
    const l_iterator* l_variable::iterator() const
    {
        return static_cast< l_iterator* >( m_value.m_pobj );
    }
    
    const l_string* l_variable::string() const
    {
        return static_cast< l_string* >( m_value.m_pobj );
    }
    
    bool l_variable::add(l_variable& var,l_variable& output)
    {
        
        switch (m_type)
        {
            case LNULL:
            switch (var.m_type)
            {
                case LNULL:
                output = (int)0;
                break;
                
                case LBOOL:
                output = (int)var.m_value.m_b;
                break;
                
                case INT:
                output = var.m_value.m_i;
                break;
                
                case FLOAT:
                output = var.m_value.m_f;
                break;
                
                case STRING:
                output = var.string()->gc_merge_left(str_null);
                break;
                
                default:
                return false;
                break;
            }
            return true;
            
            case LBOOL:
            switch (var.m_type)
            {
                case LNULL:
                output = (int) (m_value.m_b);
                break;
                
                case LBOOL:
                output = (int) (m_value.m_b + var.m_value.m_b);
                break;
                
                case INT:
                output = m_value.m_b + var.m_value.m_i;
                break;
                
                case FLOAT:
                output = m_value.m_b + var.m_value.m_f;
                break;
                
                case STRING:
                output = var.string()->gc_merge_left(B2CSTR(m_value.m_b));
                break;
                
                default:
                return false;
                break;
            }
            return true;
            
            case INT:
            switch (var.m_type)
            {
                case LNULL:
                output = m_value.m_i;
                break;
                
                case LBOOL:
                output = m_value.m_i + var.m_value.m_b;
                break;
                
                case INT:
                output = m_value.m_i + var.m_value.m_i;
                break;
                
                case FLOAT:
                output = m_value.m_i + var.m_value.m_f;
                break;
                
                case STRING:
                output = var.string()->gc_merge_left(std::to_string(m_value.m_i));
                break;
                
                default:
                return false;
                break;
            }
            return true;
            
            case FLOAT:
            switch (var.m_type)
            {
                case LNULL:
                output = m_value.m_f;
                break;
                
                case LBOOL:
                output = m_value.m_f + (float)var.m_value.m_b;
                break;
                
                case INT:
                output = m_value.m_f + (float)var.m_value.m_i;
                break;
                
                case FLOAT:
                output = m_value.m_f + var.m_value.m_f;
                break;
                
                case STRING:
                output = var.string()->gc_merge_left(std::to_string(m_value.m_f));
                break;
                
                default:
                return false;
                break;
            }
            return true;
            
            
            case STRING:
            switch (var.m_type)
            {
                case LNULL:
                output = var.string()->gc_merge(str_null);
                break;
                
                case LBOOL:
                output = var.string()->gc_merge(B2CSTR(var.m_value.m_b));
                break;
                
                case INT:
                output = var.string()->gc_merge(std::to_string(m_value.m_i));
                break;
                
                case FLOAT:
                output = var.string()->gc_merge(std::to_string(m_value.m_f));
                break;
                
                case STRING:
                output = var.string()->gc_merge(var.string());
                break;
                
                default:
                return false;
                break;
            }
            return true;
            
            default:
            return false;
            break;
        }
    }
    
#define math_op(OP)\
    switch (m_type)\
    {\
        case LBOOL:\
        switch (var.m_type)\
        {\
            case LNULL: output =         m_value.m_b  OP false; break;\
            case LBOOL: output =         m_value.m_b  OP var.m_value.m_b; break;\
            case INT:   output = ((int)  m_value.m_b) OP var.m_value.m_i; break;\
            case FLOAT: output = ((float)m_value.m_b) OP var.m_value.m_f; break;\
            default: return false; break; \
        }\
        return true;\
        case INT:\
        switch (var.m_type)\
        {\
            case LNULL: output = m_value.m_i          OP 0;                      break;\
            case LBOOL: output = m_value.m_i          OP ((int)var.m_value.m_b); break;\
            case INT: output   = m_value.m_i          OP var.m_value.m_i;        break;\
            case FLOAT: output = ((float)m_value.m_i) OP var.m_value.m_f;        break;\
            default: return false;  break;\
        }\
        return true;\
        case FLOAT:\
        switch (var.m_type)\
        {\
            case LNULL: output = m_value.m_f OP 0.0f; break;\
            case LBOOL: output = m_value.m_f OP ((float)var.m_value.m_b); break;\
            case INT:   output = m_value.m_f OP var.m_value.m_i; break;\
            case FLOAT: output = m_value.m_f OP var.m_value.m_f;  break;\
            default:   return false;  break;\
        }\
        return true;\
        default: break;\
    }


    bool l_variable::sub(l_variable& var,l_variable& output)
    {
        if(m_type == LNULL)
        {
            switch (var.m_type)
            {
                case LNULL: output = (int)0; break;
                case LBOOL: output = -((int)var.m_value.m_b); break;
                case INT:   output = -var.m_value.m_i; break;
                case FLOAT: output = -var.m_value.m_f; break;
                default: return false;
            }
            return true;
        }
        math_op( - )
        return false;
    }
    
    bool l_variable::mul(l_variable& var,l_variable& output)
    {
        if(m_type == LNULL)
        {
            switch (var.m_type)
            {
                case LNULL:
                case LBOOL:
                case INT:
                case FLOAT:
                    output = (int)0;
                    return true;
                default:
                    return false;
            }
        }
        math_op( * )
        return false;
    }
    
    bool l_variable::div(l_variable& var,l_variable& output)
    {
        if(m_type == LNULL)
        {
            switch (var.m_type)
            {
                case LNULL:
                case LBOOL:
                case INT:
                case FLOAT:
                    output = (int)0;
                    return true;
                default:
                    return false;
            }
        }
        math_op( / )
        return false;
    }
    
    bool l_variable::equal(l_variable& var,l_variable& output)
    {
        if(m_type == LNULL)
        {
            output = var.m_type == LNULL;
            return true;
        }
        else if (m_type == STRING && var.m_type == STRING)
        {
            output = l_variable(string()->str() == var.string()->str());
            return true;
        }
        else math_op( == )
            
        output = false;
        return false;
    }
    
    bool l_variable::not_equal(l_variable& var,l_variable& output)
    {
        
        if(m_type == LNULL)
        {
            output = var.m_type != LNULL;
            return true;
        }
        else if (m_type == STRING && var.m_type == STRING)
        {
            output = l_variable(string()->str() != var.string()->str());
            return true;
        }
        else math_op( != )
        
        output = false;
        return false;
    }
    
    bool l_variable::unm(l_variable& output)
    {
        if (m_type == LNULL)
        {
            output = l_variable(1);
            return true;
        }
        
        if (m_type == LBOOL)
        {
            output = l_variable(m_value.m_b ? false : true);
            return true;
        }
        
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
    
    bool l_variable::not_value(l_variable& output)
    {
        output = l_variable(is_false());
        return true;
    }
    
    bool l_variable::lt (l_variable& var, l_variable& output)
    {
        if(m_type == LNULL)
        {
            switch (var.m_type)
            {
                case LNULL: output = false; break;
                case LBOOL: output = false < var.m_value.m_b; break;
                case INT:   output = 0     < var.m_value.m_i; break;
                case FLOAT: output = 0.0f  < var.m_value.m_f; break;
                default:
                    output = false;
                    return false;
            }
            return true;
        }
        else math_op( < )
            
        output = false;
        return false;
    }
    
    
    bool l_variable::le (l_variable& var, l_variable& output)
    {
        if(m_type == LNULL)
        {
            switch (var.m_type)
            {
                case LNULL: output = true; break;
                case LBOOL: output = true; break;
                case INT:   output = 0     <= var.m_value.m_i; break;
                case FLOAT: output = 0.0f  <= var.m_value.m_f; break;
                default:
                    output = false;
                    return false;
            }
            return true;
        }
        else math_op( <= )
            
        output = false;
        return false;
    }
    
    bool l_variable::rt (l_variable& var, l_variable& output)
    {
        if(m_type == LNULL)
        {
            switch (var.m_type)
            {
                case LNULL: output = false; break;
                case LBOOL: output = false; break;
                case INT:   output = 0     > var.m_value.m_i; break;
                case FLOAT: output = 0.0f  > var.m_value.m_f; break;
                default:
                    output = false;
                    return false;
            }
            return true;
        }
        else math_op( > )
            
        output = false;
        return false;
    }
    
    
    bool l_variable::re (l_variable& var, l_variable& output)
    {
        if(m_type == LNULL)
        {
            switch (var.m_type)
            {
                case LNULL: output = true; break;
                case LBOOL: output = true; break;
                case INT:   output = 0     >= var.m_value.m_i; break;
                case FLOAT: output = 0.0f  >= var.m_value.m_f; break;
                default:
                    output = false;
                    return false;
            }
            return true;
        }
        else math_op( >= )
            
        output = false;
        return false;
    }
    
    
    bool l_variable::and_value (l_variable& var, l_variable& output)
    {
        if(m_type == LNULL)
        {
            switch (var.m_type)
            {
                case LNULL:
                case LBOOL:
                case INT:
                case FLOAT:
                    output = false;
                    return true;
                default:
                    output = false;
                    return false;
            }
        }
        else math_op( && )
            
        output = false;
        return false;
    }
    
    
    bool l_variable::or_value (l_variable& var, l_variable& output)
    {
        if(m_type == LNULL)
        {
            switch (var.m_type)
            {
                case LNULL: output = false; break;
                case LBOOL: output = var.m_value.m_b; break;
                case INT:   output = var.m_value.m_i != 0; break;
                case FLOAT: output = var.m_value.m_f != 0.0; break;
                default:
                    output = false;
                    return false;
            }
            return true;
        }
        else math_op( || )
        
        output = false;
        return false;
    }
    
    bool l_variable::mod (l_variable& var, l_variable& output)
    {
        if((is_int()  && var.is_int())   ||
           (is_bool() && var.is_bool())  ||
           (is_null() && var.is_null()))
        {
            output = (l_language::l_variable(to_int() % var.to_int()));
            return true;
        }
        else if(is_float() && (var.is_float() || var.is_int() || var.is_bool() || var.is_null()))
        {
            output = (l_language::l_variable(std::fmod(to_float(),var.to_float())));
            return true;
        }
        
        output = l_variable(false);
        return false;
    }
}