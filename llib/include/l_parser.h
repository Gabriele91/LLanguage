//
//  l_parser.h
//  LLanguage
//
//  Created by Gabriele Di Bari on 12/12/15.
//  Copyright � 2015 Gabriele Di Bari. All rights reserved.
//
#pragma once

#include <list>
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <sstream>
#include <l_syntactic_tree.h>
//utf 8
#define UTF_8_CODE

namespace l_language
{
	/*
	// ebnf3

	// consts
	variable   := {[a-Z]}
	escaped    := lit('"') | '\\' | '/' | 'b' | 'f' | 'n' | 'r' | 't'
    chars      := any_chars - '"' - '\\' | '\\' - escaped
	string     := '"' {[chars]} '"'
	constant   := double | integer | string
	space      := {[(' '|'\t'|'\r'|'\n')]}
    end_comand := '.'
	end_partial:= ';'
	end_line   := ','

	//operators
	logic_op   := ('and'|'&&'|'or'|'||')
	logic_comp := ('<'|'>'|'=='|'<='|'>=')
	logic_one  := ('!')

	exp		   := logic
	logic      := compare {[ logic_op compare ]}
	compare    := summinus [ logic_comp summinus ]
	summinus   := timediv {[ ('+'|'-') timediv ]}
	timediv    := oneop {[ ('*'|'/'|'%') oneop ]}
	oneop      := ('-' value) | (logic_one value) | value
     
	value      := '(' exp ')' 
                  | constant 
                  | call 
                  | assignable
                  | array_dec
                  | table_dec
     
	assignable :=   variable
                  | variable '[' exp ']'
                  | variable '.' variable
     
     array_dec   := '[' [exp] {[ ',' exp ]} ']'
     table_dec   := '{' [table_field] {[ ',' table_field ]} '}'
     table_field := (constant | variable) ':' exp
     
     class_dec   := 'class' variable [ ':' variable {[',' variable]}] class_body
     class_field := ( 'var' variable [assignament exp] {[',' variable [assignament exp]]} | def | def_op )
     class_body  := '{'  {[  ('public'|'private'|'protected') | class_field ]} '}'
     
	//statments
	llanguage    := staments
	staments     := { stament }
	stamentsblock:= '{' staments '}' | stament
    stament		 := if | call | cicle | operation | def | class_dec
    assignment   := '=' | '<-' | "+=" | "-=" | "*=" | "/="
    operation    := assignable assignament exp | call
    if			 := 'if' exp stamentsblock {[ else_if ]} [ else ]
    else_if      := else if exp stamentsblock
    else         := else stamentsblock
    call         := assignable '(' [exp] {[ ',' exp ]} ')'
	cicle        := while | for
	while        := 'while' exp stamentsblock
    for_c_args   := operation {[','] operation} ';' exp ';' operation {[','] operation}
    for_each_args:= variable ('in'|'of') exp
    for          := 'for' ( (for_each_args | for_c_args) [';'] | '(' (for_each_args | for_c_args ) ')') stamentsblock
    def          := ('def'|'function') [variable] '(' ([ variable {[',' variable]} [',' '...' variable] ] | ['...'variable]) ')' stamentsblock
    def_op       := 'operator' ['right'] ('+'|'-'|'*'|'/'|'%'|logic_comp|logic_op|logic_one)
                   '(' ([ variable {[',' variable]} [',' '...' variable] ] | ['...'variable]) ')'
                    stamentsblock
    return       := 'return' [exp]
    type context := ('global' | 'super') [ variable [assignament exp] {[ ',' variable [assignament exp] ]} ]
	*/
	class l_parser
	{
	public:
		//context
        size_t  m_line { 0 };
        bool    m_count_lines { true };
        bool    m_push_errors { true };
        //errors
        struct error_info
        {
            size_t m_line { 0 };
            std::string m_error;
            
            error_info(size_t line,const std::string& error)
            {
                m_line = line;
                m_error= error;
            }
            
        };
		std::list < error_info >  m_errors;
        //variables / functions
        enum variable_type
        {
            IS_VARIABLE,
            IS_KEYWORK
        };
        struct variable_row
        {
            
            variable_type  m_type { IS_VARIABLE };
            std::string    m_name;
            
            variable_row(const std::string& name)
            :m_type(IS_VARIABLE)
            ,m_name(name)
            {
            }
            
            variable_row(variable_type type,const std::string& name)
            :m_type(type)
            ,m_name(name)
            {
            }
        };
		std::vector< variable_row > m_variables;
		//put variable
        void push_variable(const std::string& str);
        bool exists_variable(const std::string& str) const;
        bool is_variable(const std::string& str);
		//push error
        void push_error(const std::string& error);
		///////////////////////////////////////////////////////////////////////////////////////////////
		//compare string
        static bool strcmp(const char* left, const char* right, size_t len);
        static bool strcmp(const char* left, const char* right);
        static bool strcmp_skip(const char*& left, const char* right, size_t len);
        static bool strcmp_skip(const char*& left, const char* right);
		///////////////////////////////////////////////////////////////////////////////////////////////
		//defines
        #define CSTRCMP(x,y) l_language::l_parser::strcmp(x,y,sizeof(y)-1)
        #define CSTRCMP_SKIP(x,y) l_language::l_parser::strcmp_skip(x,y,sizeof(y)-1)
        #define KEYWORDCMP(x,key) l_language::l_parser::strcmp(x,get_keyword(K_ ## key))
        #define KEYWORDCMP_SKIP(x,key) l_language::l_parser::strcmp_skip(x,get_keyword(K_ ## key))
        ///////////////////////////////////////////////////////////////////////////////////////////////
        enum keyword
        {
            K_IF,
            K_ELSE,
            K_DEF,
            K_FUNCTION,
            K_OPERATOR,
            K_RIGHT,
            
            K_WHILE,
            K_FOR,
            
            K_SUPER,
            K_GLOBAL,
            
            K_CLASS,
            K_VAR,
            K_PUBLIC,
            K_PRIVATE,
            K_PROTECTED,
            
            K_OF,
            K_IN,
            
            K_RETURN,
            
            K_OR,
            K_AND,
            
            K_NULL,
            K_TRUE,
            K_FALSE,
            K_ARGS,
        };
        static const char** get_keywords();
        static const char* get_keyword(keyword key);
        bool is_a_keyword(const char* ptr);
        bool is_a_value_keyword(const char* ptr);
        bool is_a_keyword_but_not_value(const char* ptr)
        {
            return !is_a_value_keyword(ptr) && is_a_keyword(ptr);
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////
		//static types
        static bool is_line_space(char c);
        static bool is_space(char c);
        static bool is_start_name(char c);
        static bool is_char_name(char c);
        static bool is_start_string(char c);
        static bool is_start_uint_number(char c);
        static bool is_start_ufloat_number(char c);
        static bool is_start_int_number(char c);
        static bool is_start_float_number(char c);
        static bool is_line_comment(const char* c);
        static bool is_start_multy_line_comment(const char* c);
        static bool is_end_multy_line_comment(const char* c);
        static bool is_start_arg(char c);
        static bool is_end_arg(char c);
        static bool is_start_index(char c);
        static bool is_end_index(char c);
        static bool is_point(char c);
        ///////////////////////////////////////////////////////////////////////////////////////////////
        bool is_int_number(const char* c);
        bool is_float_number(const char* c);
		///////////////////////////////////////////////////////////////////////////////////////////////
        void skip_line_space(const char*& inout);
        void skip_space(const char*& inout);
        void skip_line_comment(const char*& inout);
        void skip_multy_line_comment(const char*& inout,bool count_lines=true);
        void skip_space_end_comment(const char*& inout,bool count_lines=true);
		///////////////////////////////////////////////////////////////////////////////////////////////
		//parser
		//parser of a unsigned int number
        static bool parse_uint_number(const char*& c, unsigned int& i);
		//parser of a int number
        static bool parse_int_number(const char*& c, int& i);
		//parser float
        static bool parse_ufloat_number(const char*& c, float& i);
        static bool parse_float_number(const char*& c, float& i);
		//parser of a string
        bool parse_cstring(const char* in, const char** cout, std::string& out,bool count_lines = true);
		//parsing a name (variable/keyword)
        bool parse_name(const char* in, const char** cout, std::string& out);
        ///////////////////////////////////////////////////////////////////////////////////////////////
		//parser exp
        
		//parse value
		bool parse_value(const char*& ptr, l_syntactic_tree::exp_node*& node)
		{
            skip_space_end_comment(ptr);
            //const null
            if(KEYWORDCMP_SKIP(ptr, NULL))
            {
                node = l_syntactic_tree::constant_null(m_line);
            }
            //const true/false
            else if(KEYWORDCMP_SKIP(ptr, TRUE))
            {
                node = l_syntactic_tree::constant_bool(true,m_line);
            }
            else if(KEYWORDCMP_SKIP(ptr, FALSE))
            {
                node = l_syntactic_tree::constant_bool(false,m_line);
            }
            //costant int
            else if (is_int_number(ptr))
            {
                int value = 0;
                if (!parse_int_number(ptr, value))
                {
                    push_error("not valid float costant");
                    return false;
                }
                node = l_syntactic_tree::constant((int)value, m_line);
            }
            //costant float
            else if (is_float_number(ptr))
            {
                float value = 0;
                if (!parse_float_number(ptr, value))
                {
                    push_error("not valid float costant");
                    return false;
                }
                node = l_syntactic_tree::constant(value, m_line);
            }
			//costant string
			else if (is_start_string(*ptr))
			{
				std::string value;
				if (!parse_cstring(ptr, &ptr, value))
				{
					push_error("not valid string costant");
					return false;
				}
				node = l_syntactic_tree::constant(value, m_line);
			}
            //is variable/field request or call or exp def node
            else if (is_an_assignable_attribute(ptr))
            {
                //get assignable attribute
                l_syntactic_tree::node* var_field_node;
                if (!parse_assignable_or_def(ptr, var_field_node)) return false;
                //field node is a exp node
                node = (l_syntactic_tree::exp_node*)var_field_node;
            }
			//is exp
			else if (is_start_arg(*ptr))
			{
				//skip (
				++ptr;
				//parse exp
				if (!parse_exp(ptr, node))
				{
					push_error("not valid expression (value)");
					return false;
				}
				//skip
				skip_space_end_comment(ptr);
				//end )
				if (!is_end_arg(*ptr))
				{
					push_error("not valid expression (value)");
					return false;
				}
				//skip )
                ++ptr;
                //skip
                skip_space_end_comment(ptr);
                //is a call?
                if(is_call_args(ptr) && node)
                {
                    //create call node
                    l_syntactic_tree::node* call_and_field_node;
                    //parse args and fields
                    if(!parse_field_or_def_and_call(ptr,call_and_field_node,node))
                    {
                        //delete call node
                             if (call_and_field_node) delete call_and_field_node;
                        else if (node)                delete node;
                        //to null
                        node = nullptr;
                        //return false
                        return false;
                    }
                    //save
                    node = (l_syntactic_tree::exp_node*)call_and_field_node;
                }
			}		
			//is array dec
			else if(is_start_index(*ptr))
			{
				//skip [
                ++ptr;
                //skip space
                skip_space_end_comment(ptr);
				//alloc node
				l_syntactic_tree::array_node* array_node = l_syntactic_tree::array(m_line);
				l_syntactic_tree::exp_node* exp_node = nullptr;
				//parsing all values
                if(!is_end_index(*ptr))
                {
                    do
                    {
                        //parsing..
                        if (!parse_exp(ptr, exp_node))
                        {
                            push_error("not valid expression (value)");
                            return false;
                        }
                        //parse list exp
                        if(exp_node) array_node->append(exp_node);
                        //skip
                        skip_space_end_comment(ptr);
                    }
                    while(CSTRCMP_SKIP(ptr,","));
                }
				//end ]
				if (!is_end_index(*ptr))
				{
					push_error("not valid array declaretion (value)");
					return false;
				}
				//skip ]
				++ptr;
                //field node is a exp node
                node = (l_syntactic_tree::exp_node*)array_node;
            }
            //is table dec
            else if(CSTRCMP_SKIP(ptr,"{"))
            {
                //skip space
                skip_space_end_comment(ptr);
                //alloc node
                l_syntactic_tree::table_node* table_node = l_syntactic_tree::table(m_line);
                l_syntactic_tree::exp_node* exp_left = nullptr;
                l_syntactic_tree::exp_node* exp_right = nullptr;
                //parsing }
                if(!CSTRCMP(ptr,"}"))
                {
                    //parsing all values
                    do
                    {
                        //skip space
                        skip_space_end_comment(ptr);
                        //parsing..
                        if (!parse_exp(ptr, exp_left))
                        {
                            push_error("not valid expression (value)");
                            return false;
                        }
                        //skip space
                        skip_space_end_comment(ptr);
                        //jump assignament
                        if(is_assignment(ptr))
                        {
                            std::string op_name;
                            parse_assignment(ptr,op_name);
                            if(op_name!="=")
                            {
                                push_error("not found valid assignment statement");
                                return false;
                            }
                        }
                        else if(!CSTRCMP_SKIP(ptr,":"))
                        {
                            push_error("not found assignment statement");
                            return false;
                        }
                        //skip space
                        skip_space_end_comment(ptr);
                        //parsing..
                        if (!parse_exp(ptr, exp_right))
                        {
                            push_error("not valid expression (value)");
                            return false;
                        }
                        //parse list exp
                        if(exp_left && exp_right)
                        {
                            table_node->append(exp_left,exp_right);
                        }
                        //skip
                        skip_space_end_comment(ptr);
                    }
                    while(CSTRCMP_SKIP(ptr,","));
                }
                //end }
                if (!CSTRCMP_SKIP(ptr,"}"))
                {
                    push_error("not valid table declaretion (value)");
                    return false;
                }
                //field node is a exp node
                node = (l_syntactic_tree::exp_node*)table_node;
            }
			//return true...
			return true;
		}
		//one op
		bool parse_oneop(const char*& ptr, l_syntactic_tree::exp_node*& node)
		{
			//value node
			l_syntactic_tree::exp_node *oneop = nullptr;
			l_syntactic_tree::exp_node *value = nullptr;
			//skip
			skip_space_end_comment(ptr);
			//parse op
			if (CSTRCMP_SKIP(ptr, "-"))
			{
				oneop = l_syntactic_tree::exp("-", nullptr, m_line);
			}
			else if (CSTRCMP_SKIP(ptr, "!"))
			{
				oneop = l_syntactic_tree::exp("!", nullptr, m_line);
			}
			//skip
			skip_space_end_comment(ptr);
			//parser value
			if (!parse_value(ptr, value))
			{
				push_error("not valid expression (one op)");
				return false;
			}
			// oneop := ('-' value) | (logicOne value) | | value
			if (oneop)
			{
				oneop->m_left = value;
				node = oneop;
			}
			else
			{
				node = value;
			}
			//success
			return true;
		}
		//parser timediv
		bool parse_timediv(const char*& ptr, l_syntactic_tree::exp_node*& node)
		{
			//value node
			l_syntactic_tree::exp_node *left   = nullptr;
			l_syntactic_tree::exp_node *opnode = nullptr;
			//skip
			skip_space_end_comment(ptr);
			//parse value
			if (!parse_oneop(ptr, opnode))   return false;
			//skip
			skip_space_end_comment(ptr);
			//cicle
			while (*ptr == '*' || *ptr == '/' || *ptr == '%')
			{
				//left node
				left   = opnode;
				//node
				opnode = l_syntactic_tree::exp(std::string() + *ptr, nullptr, m_line);
				//jmp op (*|/|%)
				++ptr;
				//compone node
				opnode->m_left = left;
				//skip
				skip_space_end_comment(ptr);
				//parse value
				if (!parse_oneop(ptr, opnode->m_right))  return false;
				//skip
				skip_space_end_comment(ptr);
			}
			//save node
			node = opnode;
			//success
			return true;
		}
		//parser timediv
		bool parse_summinus(const char*& ptr, l_syntactic_tree::exp_node*& node)
		{
			//value node
			l_syntactic_tree::exp_node *left = nullptr;
			l_syntactic_tree::exp_node *opnode = nullptr;
			//skip
			skip_space_end_comment(ptr);
			//parse value
			if (!parse_timediv(ptr, opnode))   return false;
			//skip
			skip_space_end_comment(ptr);
			//cicle
			while (*ptr == '+' || *ptr == '-')
			{
				//left node
				left = opnode;
				//node
				opnode = l_syntactic_tree::exp(std::string() + *ptr, nullptr, m_line);
				//jmp op (+|-)
				++ptr;
				//compone node
				opnode->m_left = left;
				//skip
				skip_space_end_comment(ptr);
				//parse value
				if (!parse_timediv(ptr, opnode->m_right))  return false;
				//skip
				skip_space_end_comment(ptr);
			}
			//save node
			node = opnode;
			//success
			return true;
		}
		//parser compare
		bool parse_compare(const char*& ptr, l_syntactic_tree::exp_node*& node)
		{
			//skip
			skip_space_end_comment(ptr);
			//value node
			l_syntactic_tree::exp_node *left = nullptr;
			l_syntactic_tree::exp_node *logiccomp = nullptr;
			l_syntactic_tree::exp_node *right = nullptr;
			//skip
			skip_space_end_comment(ptr);
			//parse value
			if (!parse_summinus(ptr, left))   return false;
			//skip
			skip_space_end_comment(ptr);
            // logicComp := (str oprations|'<='|'>=''<'|'>'|'==')
            if ( CSTRCMP_SKIP(ptr, "<=") )
			{
				logiccomp = l_syntactic_tree::exp(std::string("<="), nullptr, m_line);
            }
            else if( CSTRCMP_SKIP(ptr, ">=") )
            {
                logiccomp = l_syntactic_tree::exp(std::string(">="), nullptr, m_line);
            }
            else if( CSTRCMP_SKIP(ptr, "<") )
            {
                logiccomp = l_syntactic_tree::exp(std::string("<"), nullptr, m_line);
            }
            else if( CSTRCMP_SKIP(ptr, ">") )
            {
                logiccomp = l_syntactic_tree::exp(std::string(">"), nullptr, m_line);
            }
            else if( CSTRCMP_SKIP(ptr, "==") )
            {
                logiccomp = l_syntactic_tree::exp(std::string("=="), nullptr, m_line);
            }
            else if( CSTRCMP_SKIP(ptr, "!=") )
            {
                logiccomp = l_syntactic_tree::exp(std::string("!="), nullptr, m_line);
            }
            else
            {
                node = left;
                return true;
            }
			//skip
			skip_space_end_comment(ptr);
			//parse value
			if (!parse_summinus(ptr, right))  return false;
			//create exp
			logiccomp->m_left = left;
			logiccomp->m_right = right;
			//save node
			node = logiccomp;
			//success
			return true;
		}
		//parser logic
		bool parse_logic(const char*& ptr, l_syntactic_tree::exp_node*& node)
		{
			//value node
			l_syntactic_tree::exp_node *left = nullptr;
			l_syntactic_tree::exp_node *opnode = nullptr;
			//skip
			skip_space_end_comment(ptr);
			//parse value
			if (!parse_compare(ptr, opnode))   return false;
			//skip
			skip_space_end_comment(ptr);
			//cicle
			while (KEYWORDCMP(ptr, AND) ||
				   CSTRCMP(ptr, "&&") ||
				   KEYWORDCMP(ptr, OR) ||
				   CSTRCMP(ptr, "||")  )
			{
				//left node
				left = opnode;
				//op
				if ( KEYWORDCMP_SKIP(ptr, AND) || CSTRCMP_SKIP(ptr, "&&") )
				{
					opnode = l_syntactic_tree::exp(std::string("&&"), nullptr, m_line);
				}
				else if( KEYWORDCMP_SKIP(ptr, OR) || CSTRCMP_SKIP(ptr, "||") )
				{
					opnode = l_syntactic_tree::exp(std::string("||"), nullptr, m_line);
				}
				//compone node
				opnode->m_left = left;
				//skip
				skip_space_end_comment(ptr);
				//parse value
				if (!parse_compare(ptr, opnode->m_right))  return false;
				//skip
				skip_space_end_comment(ptr);
			}
			//save node
			node = opnode;
			//success
			return true;
		}
		//parser exp
		bool parse_exp(const char*& ptr, l_syntactic_tree::exp_node*& node)
		{
			/* none */
			return parse_logic(ptr, node);
		}
        ///////////////////////////////////////////////////////////////////////////////////////////////
        //statments

		//parse stament
		bool parse_stament(const char*& ptr, l_syntactic_tree::list_nodes& nodes)
		{
			//////////////////
			//stament
			//////////////////
			//skip
			skip_space_end_comment(ptr);
			//parse
			l_syntactic_tree::node* stament;
			if (!parse_stament(ptr, stament)) return false;
			//push stament
			nodes.push_back(stament);
			//skip
			skip_space_end_comment(ptr);
			//jump commond
			CSTRCMP_SKIP(ptr, ";");
			//skip
			skip_space_end_comment(ptr);
			//////////////////
			return true;
		}
		//parse staments
		bool parse_staments(const char*& ptr, l_syntactic_tree::list_nodes& nodes)
		{
			//skip
			skip_space_end_comment(ptr);
			//staments
			while (*ptr && *ptr != EOF && !CSTRCMP(ptr, "}"))
			{
				//parse
				l_syntactic_tree::node* stament;
				if (!parse_stament(ptr, stament)) return false;
				//push stament
				nodes.push_back(stament);
				//skip
				skip_space_end_comment(ptr);
				//jump commond
				CSTRCMP_SKIP(ptr, ";");
				//skip
				skip_space_end_comment(ptr);
			}
			return true;
		}
		//parse staments/stament block
		bool parse_stament_s_block(const char*& ptr, l_syntactic_tree::list_nodes& nodes)
		{
			//skip
			skip_space_end_comment(ptr);
			//try
			if (CSTRCMP_SKIP(ptr, "{"))
			{
				//try
				if (!parse_staments(ptr, nodes)) return false;
				//then
				if (!CSTRCMP_SKIP(ptr, "}"))
				{
					push_error("not found \'}\' keyword");
					return false;
				}
				//skip spaces
				skip_space_end_comment(ptr);
				//return
				return true;
			}
			else
			{
				return parse_stament(ptr, nodes);
			}
			//skip
			skip_space_end_comment(ptr);
		}
		//parse stament
		bool parse_stament(const char*& ptr, l_syntactic_tree::node*& node)
		{
			//skip
			skip_space_end_comment(ptr);
            //staments
            if (KEYWORDCMP(ptr, IF))					           return parse_if(ptr, node);
            if (KEYWORDCMP(ptr, CLASS))					           return parse_class(ptr, node);
            if (KEYWORDCMP(ptr, DEF) || KEYWORDCMP(ptr, FUNCTION)) return parse_def(ptr, node);
            if (KEYWORDCMP(ptr, FOR) || KEYWORDCMP(ptr, WHILE))    return parse_cicle(ptr, node);
            if (KEYWORDCMP(ptr, GLOBAL)||KEYWORDCMP(ptr, SUPER))   return parse_type(ptr, node);
            if (KEYWORDCMP(ptr, RETURN))                           return parse_return(ptr, node);
			if (is_operation(ptr))						           return parse_operation(ptr, node);
			//error
			push_error("not valid stament");
			return false;
		}
        //is index field request?
        bool is_a_index_field(const char* ptr)
        {
            //skip space
            skip_space_end_comment(ptr, false);
            //is a name?
            if (!is_start_name(*ptr)) return false;
            //parse variable
            std::string variable_name;
            if (!parse_name(ptr, &ptr, variable_name)) return false;
            //skip space
            skip_space_end_comment(ptr, false);
            //index start char?
            return is_start_index(*ptr);
        }
        //is point field request?
        bool is_a_point_field(const char* ptr)
        {
            //skip space
            skip_space_end_comment(ptr, false);
            //is a name?
            if (!is_start_name(*ptr)) return false;
            //parse variable
            std::string variable_name;
            if (!parse_name(ptr, &ptr, variable_name)) return false;
            //skip space
            skip_space_end_comment(ptr, false);
            //index start char?
            return is_point(*ptr);
        }
        //is field request?
        bool is_a_field(const char* ptr)
        {
            //skip space
            skip_space_end_comment(ptr, false);
            //is a name?
            if (!is_start_name(*ptr)) return false;
            //parse variable
            std::string variable_name;
            if (!parse_name(ptr, &ptr, variable_name)) return false;
            //skip space
            skip_space_end_comment(ptr, false);
            //index start char?
            return is_start_index(*ptr) || is_point(*ptr);
        }

		//parse field
        bool parse_field_or_def_and_call(const char*& ptr,
                                         l_syntactic_tree::node*& node,
                                         l_syntactic_tree::node* variable_node = nullptr)
        {
            //flag
            bool m_do_loop = true;
            //loop
            while (m_do_loop)
            {
                //disable loop
                m_do_loop = false;
                //skip
                skip_space_end_comment(ptr);
                //get field
                l_syntactic_tree::node* assignable_node = nullptr;
                //is variable?
                if(!variable_node)
                {
                    if(KEYWORDCMP(ptr, DEF) || KEYWORDCMP(ptr, FUNCTION))
                    {
                        //type of return
                        l_syntactic_tree::node* def_node = nullptr;
                        //
                        if (!parse_def(ptr,def_node,true))
                        {
                            push_error("not valid lambda declaretion (value)");
                            return false;
                        }
                        //cast
                        variable_node = (l_syntactic_tree::node*)def_node;
                    }
                    else if(!parse_variable(ptr, variable_node))
                    {
                        return false;
                    }
                    //skip
                    skip_space_end_comment(ptr);
                }
                //parse field
                if(is_point(*ptr) || is_start_index(*ptr))
                {
                    if(!parse_field_values(ptr,
                                           (l_syntactic_tree::assignable_node*)variable_node,
                                           assignable_node)) return false;
                }
                //is only variable
                else
                {
                    assignable_node = variable_node;
                }
                //call?
                if(is_call_args(ptr))
                {
                    //cast..
                    auto *call_var_node = (l_syntactic_tree::assignable_node*)assignable_node;
                    //build call
                    node = l_syntactic_tree::call(call_var_node, m_line);
                    //parse args
                    if(!parse_call_arguments(ptr,node))
                    {
                        //delete call node
                        if (node) delete node;
                        //return false
                        return false;
                    }
                    //skip
                    skip_space_end_comment(ptr);
                    //do loop?
                    if(is_point(*ptr) || is_start_index(*ptr))
                    {
                        variable_node = node;
                        node      = nullptr;
                        m_do_loop = true;
                    }
                }
                //this field return
                else
                {
                    node = assignable_node;
                }
                //skip
                skip_space_end_comment(ptr);
            }
            return true;
        }
		bool parse_field_values(const char*& ptr,
                                l_syntactic_tree::assignable_node* variable_node,
                                l_syntactic_tree::node*& node)
		{
			l_syntactic_tree::exp_node* exp_node = nullptr;
            //skip
            skip_space_end_comment(ptr);
            //list values
            std::vector< l_syntactic_tree::exp_node* > list_values;
            //parser
            while (true)
            {
                //jmp space
                skip_space_end_comment(ptr);
                // . || [
                if(is_point(*ptr))
                {
                    //jmp point
                    ++ptr;
                    //skip
                    skip_space_end_comment(ptr);
                    //parse variable name
                    std::string field_name;
                    if (!parse_name(ptr, &ptr, field_name))
                    {
                        push_error("not valid field name");
                        return false;
                    }
                    //append node
                    list_values.push_back (l_syntactic_tree::constant(field_name, m_line));
                    //jmp space
                    skip_space_end_comment(ptr);
                }
                else if(is_start_index(*ptr))
                {
                    //jmp index
                    ++ptr;
                    //skip
                    skip_space_end_comment(ptr);
                    //parse exp
                    if (!parse_exp(ptr, exp_node))
                    {
                        push_error("not valid indexing expression (value)");
                        return false;
                    }
                    //skip
                    skip_space_end_comment(ptr);
                    //end ']'
                    if (!is_end_index(*ptr))
                    {
                        push_error("not valid indexing, not found ']'");
                        return false;
                    }
                    //skip ']'
                    ++ptr;
                    //append
                    list_values.push_back (exp_node);
                    //skip
                    skip_space_end_comment(ptr);
                }
                else
                {
                    break;
                }
            }
            //first
            l_syntactic_tree::assignable_node* left = l_syntactic_tree::field(variable_node, &(*list_values.front()));
            //build tree
            if(list_values.size()>=2)
            for(long i=1;  i != list_values.size(); ++i)
            {
                left = l_syntactic_tree::field(left,list_values[i]);
            }
			//save last field node
            node = left;
			//correct
			return true;
		}
		//is variable
		bool is_variable(const char*& ptr)
		{
			//skip space
			skip_space_end_comment(ptr, false);
			//is a name?
			if (!is_start_name(*ptr)) return false;
			//is a variable
			return true;
		}
		//parse variable
		bool parse_variable(const char*& ptr, l_syntactic_tree::node*& node)
		{
			//skip
			skip_space_end_comment(ptr);
			//parse variable
			std::string variable_name;
			if (!parse_name(ptr, &ptr, variable_name))
			{
				push_error("not valid variable name");
				return false;
			}
			if (!exists_variable(variable_name))
			{
                //push variable
                push_variable(variable_name);
			}
            //return node
            node = l_syntactic_tree::variable(variable_name, m_line);
			//return true...
			return true;
		}
		//is assignable attribute
		bool is_an_assignable_attribute(const char* ptr)
		{
			//is field
			if (is_a_field(ptr)) return true;
			//is variable
			return is_variable(ptr);
		}
		//parse assignable
		bool parse_assignable_or_def(const char*& ptr, l_syntactic_tree::node*& node)
		{
            if(KEYWORDCMP(ptr,NULL) ||
               KEYWORDCMP(ptr,TRUE) ||
               KEYWORDCMP(ptr,FALSE) )
            {
                //error
                push_error("the assignable attribute isn't valid, is a keyword");
                return false;
            }
			else
            if (KEYWORDCMP(ptr, DEF)      ||
                KEYWORDCMP(ptr, FUNCTION) ||
                is_a_field(ptr)           ||
                is_variable(ptr))
			{
				if (parse_field_or_def_and_call(ptr, node)) return true;
			}
			//error
			push_error("the assignable attribute isn't valid");
			return false;
		}
        //is assignment?
        bool is_assignment(const char* ptr)
        {
            //skip space
            skip_space_end_comment(ptr,false);
            // = ?
            if (CSTRCMP_SKIP(ptr, "=")) return true;
            // <- ?
            if (CSTRCMP_SKIP(ptr, "<-")) return true;
            // += ?
            if (CSTRCMP_SKIP(ptr, "+=")) return true;
            // -= ?
            if (CSTRCMP_SKIP(ptr, "-=")) return true;
            // *= ?
            if (CSTRCMP_SKIP(ptr, "*=")) return true;
            // /= ?
            if (CSTRCMP_SKIP(ptr, "/=")) return true;
            // it isn't an assignment
            return false;
        }
        //parse assignment
        bool parse_assignment(const char*& ptr,std::string& name)
        {
            //skip
            skip_space_end_comment(ptr);
            // = ?
            if (CSTRCMP_SKIP(ptr, "=")){ name="="; return true; }
            // 'come' ?
            if (CSTRCMP_SKIP(ptr, "<-")){ name="="; return true; }
            // += ?
            if (CSTRCMP_SKIP(ptr, "+=")){ name="+="; return true; }
            // -= ?
            if (CSTRCMP_SKIP(ptr, "-=")){ name="-="; return true; }
            // *= ?
            if (CSTRCMP_SKIP(ptr, "*=")){ name="*="; return true; }
            // /= ?
            if (CSTRCMP_SKIP(ptr, "/=")){ name="/="; return true; }
            //error
            push_error("not find (+|-|*|/)?= or <-");
            return false;
        }
        //is operation?
        bool is_operation(const char* ptr)
        {
            //skip space
            skip_space_end_comment(ptr,false);
            //variable (=|<-) exp?
            if( is_an_assignable_attribute(ptr) ) return true;
            //it isn't an operation
            return false;
        }
        bool is_operation_slow(const char* ptr,bool only_assignament = false)
        {
            l_syntactic_tree::node* node = nullptr;
            //disable line count and push errors
            m_count_lines = false;
            m_push_errors = false;
            //parse
            bool status = parse_operation(ptr,node,only_assignament);
            //delete node
            delete node;
            //enable count line and errors
            m_count_lines = true;
            m_push_errors = true;
            //it isn't an operation
            return status;
        }
		//parse operation
		bool parse_operation(const char*& ptr, l_syntactic_tree::node*& node,bool only_assignament = false)
		{
			//exp
			l_syntactic_tree::exp_node *exp = nullptr;
			//skip
			skip_space_end_comment(ptr);
            // <assignable> <assignment> <exp> | <assignable> '(' args ')'
            {
                // parse variable or field
                l_syntactic_tree::node *assignable_node = nullptr;
				// is a variable o field?
                if (!parse_assignable_or_def(ptr, assignable_node)) return false;
                //skip space
                skip_space_end_comment(ptr);
                //find assignment ?
				if (is_assignment(ptr))
                {
                    //op name
                    std::string op_name;
                    //parse exp
                    if (!parse_assignment(ptr,op_name) || !parse_exp(ptr, exp))
                    {
                        if (assignable_node) delete assignable_node;
                        return false;
                    }
                    //build node
                    node = l_syntactic_tree::assignment((l_syntactic_tree::assignable_node*)assignable_node, op_name, exp, m_line);
                }
                else if(!only_assignament)
                {
                    node = assignable_node;
                }
                else
                {
                    delete assignable_node;
                    push_error("not valid assignament");
                    return false;
                }
            }
			//is parsed
			return true;
        }
        //is a call?
        bool is_call_args(const char* ptr)
        {
            //skip
            skip_space_end_comment(ptr,false);
            //find '(' ? or isn't a call
            return is_start_arg(*ptr);
        }
        //parse call
        bool parse_call_arguments(const char*& ptr, l_syntactic_tree::node*& node)
        {
            //cast to call node
            auto* call_node = (l_syntactic_tree::call_node*)node;
            //skip space
            skip_space_end_comment(ptr);
            // '('
            if ( CSTRCMP_SKIP(ptr, "(") )
            {
                
                //skip space
                skip_space_end_comment(ptr);
                
                if ( !CSTRCMP(ptr, ")") )
                {
                    do
                    {
                        //skip space
                        skip_space_end_comment(ptr);
                        //parse exp
                        l_syntactic_tree::exp_node* exp;
                        //parse exp
                        if (!parse_exp(ptr, exp)) return false;
                        //append exp
                        call_node->append(exp);
                        //skip space
                        skip_space_end_comment(ptr);
                    } while (CSTRCMP_SKIP(ptr, ","));
                }
            }
            //else 
            return CSTRCMP_SKIP(ptr, ")");
        }
		//parse if
		bool parse_if(const char*& ptr, l_syntactic_tree::node*& node)
		{
			//skip
			skip_space_end_comment(ptr);
            //bool
            bool loop  = true;
            bool error = false;
            //states..
            enum if_eslse_state
            {
                IF_STATE,
                ELSE_IF_STATE,
                ELSE_STATE
            };
            if_eslse_state state = IF_STATE;
            //else if id
            int else_if_id = 0;
            // exp
            l_syntactic_tree::exp_node *exp = nullptr;
            // build node
                     node = l_syntactic_tree::if_else(exp, m_line);
            auto* if_node = (l_syntactic_tree::if_node*) (node);
            //staments pointer
            l_syntactic_tree::list_nodes* staments = nullptr;
			//add sub nodes
            while(loop)
            {
                switch (state)
                {
                    case IF_STATE:
                        // "if"
                        if ( !KEYWORDCMP_SKIP(ptr, IF) ||
                             !parse_exp(ptr, if_node->get_if_exp()) )
                        {
                            loop  = false;
                            error = true;
                            break;
                        }
                        staments = &if_node->get_if_subnode_list();
                        break;
                    case ELSE_IF_STATE:
                    {
                        // "else if"
                        bool unparse_else = !KEYWORDCMP_SKIP(ptr, ELSE);
                        //skip
                        skip_space_end_comment(ptr);
                        //try to parse if
                        if ( unparse_else ||
                             !KEYWORDCMP_SKIP(ptr, IF)   ||
                             !parse_exp(ptr, if_node->get_else_if_exp(else_if_id)) )
                        {
                            loop  = false;
                            error = true;
                            break;
                        }
                        staments = &if_node->get_else_if_subnode_list(else_if_id);
                    }
                    break;
                    case ELSE_STATE:
                        // "else"
                        if ( !KEYWORDCMP_SKIP(ptr, ELSE) )
                        {
                            loop  = false;
                            error = true;
                            break;
                        }
                        staments = &if_node->get_else_subnode_list();
                        break;
                    default:
                        break;
                }
				// parse '{' staments '}' | stament
				if (!parse_stament_s_block(ptr, *staments)) return false;
                //change state
                if( state != ELSE_STATE )
                {
                    const char* l_ptr = ptr;
                    bool        b_else= KEYWORDCMP_SKIP(l_ptr,ELSE);
                    skip_space_end_comment(l_ptr,false);
                    bool        b_if  = KEYWORDCMP_SKIP(l_ptr,IF);
                    //else if case
                    if( b_else && b_if )
                    {
                        state = ELSE_IF_STATE;
                        else_if_id = (int)if_node->add_else_if_row();
                    }
                    //else case
                    else if( b_else )   { state = ELSE_STATE; }
                    //not have a else/else if  case
                    else                { loop  = false; }
                }
                //end of else case
                else{ loop  = false; }
                
            }
            //error?
            if(error)
            {
                switch (state)
                {
                    case IF_STATE: push_error("not valid if stament"); break;
                    case ELSE_IF_STATE: push_error("not valid else if stament"); break;
                    case ELSE_STATE: push_error("not valid else stament"); break;
                    default: break;
                }
                //dealloc
                node = nullptr;
                delete if_node;
                //return unsuccess
                return false;
            }
			//else success
			return true;
		}
        //parse while
        bool parse_while(const char*& ptr, l_syntactic_tree::node*& node)
        {
            //skip
            skip_space_end_comment(ptr);
            // 'mentre'
            if (!KEYWORDCMP_SKIP(ptr, WHILE)) return false;
            // exp
            l_syntactic_tree::exp_node *exp = nullptr;
            if (!parse_exp(ptr, exp)) return false;
			//alloc
			            node = l_syntactic_tree::while_do(exp, m_line);
            auto* while_node = (l_syntactic_tree::while_node*) (node);
			// parse '{' staments '}' | stament
			if (!parse_stament_s_block(ptr, while_node->m_staments)) return false;
            //success
            return true;
        }
        //parse for
        bool parse_for(const char*& ptr, l_syntactic_tree::node*& node)
        {
            //parse for each
            l_syntactic_tree::for_node::type_for type_for = l_syntactic_tree::for_node::FOR_IN;
            l_syntactic_tree::node*     node_left = nullptr;
            l_syntactic_tree::exp_node* node_right = nullptr;
            //parse for
            if (!KEYWORDCMP_SKIP(ptr, FOR))
            {
                push_error("not found \"for\" keyword");
                return false;
            }
            //skip
            skip_space_end_comment(ptr);
            //parenthesis?
            bool have_parenthesis = CSTRCMP_SKIP(ptr, "(");
            //skip space?
            if( have_parenthesis ) skip_space_end_comment(ptr);
            //operation?
            if(is_operation_slow(ptr,true) || CSTRCMP_SKIP(ptr, ";"))
            {
                //alloc new node
                node = l_syntactic_tree::for_clike(m_line);
                //parser
                auto* for_c_node = (l_syntactic_tree::for_node*)node;
                //parsing all init
                while (!CSTRCMP_SKIP(ptr, ";"))
                {
                    //parse op
                    l_syntactic_tree::node* op_node = nullptr;
                    //get op
                    if(!parse_operation(ptr, op_node, true))
                    {
                        delete for_c_node;
                        return false;
                    }
                    //add
                    for_c_node->append_left((l_syntactic_tree::op_node*)op_node);
                    //if find ',' skip
                    CSTRCMP_SKIP(ptr, ",");
                    //skip
                    skip_space_end_comment(ptr);
                }
                //skip
                skip_space_end_comment(ptr);
                //parse exp
                if(!CSTRCMP(ptr, ";"))
                if(!parse_exp(ptr, for_c_node->m_exp))
                {
                    delete for_c_node;
                    return false;
                }
                //skip ';'
                if(!CSTRCMP_SKIP(ptr, ";"))
                {
                    push_error("not found \";\" character");
                    delete for_c_node;
                    return false;
                }
                //..
                while (!CSTRCMP(ptr, "{") && !CSTRCMP(ptr, ")") && !CSTRCMP_SKIP(ptr, ";"))
                {
                    //parse op
                    l_syntactic_tree::node* op_node = nullptr;
                    //get op
                    if(!parse_operation(ptr, op_node))
                    {
                        delete for_c_node;
                        return false;
                    }
                    //add
                    for_c_node->append_right((l_syntactic_tree::op_node*)op_node);
                    //if find ',' skip
                    CSTRCMP_SKIP(ptr, ",");
                    //skip
                    skip_space_end_comment(ptr);
                }
                    
            }
            else
            {
                //parse
                if(!parse_assignable_or_def(ptr, node_left)) return false;
                //skip
                skip_space_end_comment(ptr);
                //
                if (!KEYWORDCMP_SKIP(ptr, IN))
                {
                    //change type
                    type_for = l_syntactic_tree::for_node::FOR_OF;
                    //try
                    if (!KEYWORDCMP_SKIP(ptr, OF))
                    {
                        push_error("not found \"in\"/\"of\" keyword");
                        if(node_left) delete node_left;
                        return false;
                    }
                }
                //skip
                skip_space_end_comment(ptr);
                //parse
                if(!parse_exp(ptr, node_right))
                {
                    if(node_left) delete node_left;
                    return false;
                }
                //static ptrs
                auto* assignable_node_left = (l_syntactic_tree::assignable_node*)node_left;
                auto* assignable_node_right = (l_syntactic_tree::assignable_node*)node_right;
                //build node
                if(type_for == l_syntactic_tree::for_node::FOR_IN)
                {
                    node = l_syntactic_tree::for_in(assignable_node_left,assignable_node_right, m_line);
                }
                else
                {
                    node = l_syntactic_tree::for_of(assignable_node_left,assignable_node_right, m_line);
                }
            }
            //end of parenthesis
            if (have_parenthesis)
            if (!CSTRCMP_SKIP(ptr, ")"))
            {
                push_error("not found \")\" character");
                if(node)  delete node;
                return false;
                
            }
			//cast
            auto* for_node = (l_syntactic_tree::for_node*) (node);
			// parse { staments } | stament
			if (!parse_stament_s_block(ptr, for_node->m_staments))
			{
				//free
				if (node)  delete node;
				//to null
				node = nullptr;
				//fail
				return false;
			}
			//success
			return true;
        }
		//parse cicle
		bool parse_cicle(const char*& ptr, l_syntactic_tree::node*& node)
        {
            if (KEYWORDCMP(ptr, FOR))		return parse_for(ptr, node);
            if (KEYWORDCMP(ptr, WHILE))		return parse_while(ptr, node);
			return false;
        }
        //parse type
        bool parse_type(const char*& ptr, l_syntactic_tree::node*& node)
        {
            l_syntactic_tree::context_type_node* l_context_type = nullptr;
            //skip
            skip_space_end_comment(ptr);
            //global or super?
            if(KEYWORDCMP_SKIP(ptr, GLOBAL))
            {
                l_context_type = l_syntactic_tree::context_global(m_line);
            }
            else if(KEYWORDCMP_SKIP(ptr, SUPER))
            {
                l_context_type = l_syntactic_tree::context_super(m_line);
            }
            else
            {
                push_error("not found \"global/super\" keyword");
                return false;
            }
            //skip
            skip_space_end_comment(ptr);
            //parse
            do
            {
                if(is_operation_slow(ptr,true))
                {
                    //op node
                    l_syntactic_tree::node* op_node = nullptr;
                    //parse
                    if(!parse_operation(ptr,op_node,true))
                    {
                        return false;
                    }
                    //save
                    l_context_type->append( (l_syntactic_tree::op_node*)op_node );
                }
                else
                {
                    //variable node
                    l_syntactic_tree::node* l_variable = nullptr;
                    //parse variable
                    if(!parse_variable(ptr, l_variable))
                    {
                        push_error("not found \"variable\" of args of def");
                        //dealloc
                        delete l_context_type;
                        //false
                        return false;
                    }
                    //skip
                    skip_space_end_comment(ptr);
                    //push var
                    l_context_type->append((l_syntactic_tree::variable_node*)l_variable);
                }
                //skip
                skip_space_end_comment(ptr);
            }
            while (CSTRCMP_SKIP(ptr, ","));

            //skip
            skip_space_end_comment(ptr);
            //cast
            node = (l_syntactic_tree::node*)l_context_type;
            //success
            return true;
        }
        //parse args
        bool parse_def_args(const char*& ptr,l_syntactic_tree::function_def_node* l_def_node)
        {
            //skip
            skip_space_end_comment(ptr);
            //parse args
            if (CSTRCMP_SKIP(ptr, "("))
            {
                //skip
                skip_space_end_comment(ptr);
                //args...
                if(!CSTRCMP_SKIP(ptr, ")"))
                {
                    //
                    bool b_not_have_args_list = true;
                    //values...
                    do
                    {
                        //skip
                        skip_space_end_comment(ptr);
                        //variable node
                        l_syntactic_tree::node* l_variable = nullptr;
                        //args list?
                        if(KEYWORDCMP_SKIP(ptr, ARGS))
                        {
                            b_not_have_args_list = false;
                            //skip
                            skip_space_end_comment(ptr);
                        }
                        //parse variable
                        if(!parse_variable(ptr, l_variable))
                        {
                            push_error("not found \"variable\" of args of def");
                            //false
                            return false;
                        }
                        //push var
                        if(b_not_have_args_list)
                        l_def_node->append_arg((l_syntactic_tree::variable_node*)l_variable);
                        else
                        l_def_node->set_name_args_list((l_syntactic_tree::variable_node*)l_variable);
                        //skip
                        skip_space_end_comment(ptr);
                    }
                    while (b_not_have_args_list && CSTRCMP_SKIP(ptr, ","));
                    //end?
                    if(!CSTRCMP_SKIP(ptr, ")"))
                    {
                        push_error("not found \")\" of args of def");
                        //false
                        return false;
                    }
                }
            }
            return true;
        }
        //parse def
        bool parse_def(const char*& ptr, l_syntactic_tree::node*& node, bool is_exp = false)
        {
            l_syntactic_tree::node*              l_variable = nullptr;
            l_syntactic_tree::function_def_node* l_def_node = nullptr;
            //skip
            skip_space_end_comment(ptr);
            //serach def
            if (!KEYWORDCMP_SKIP(ptr, DEF) && !KEYWORDCMP_SKIP(ptr, FUNCTION))
            {
                push_error("not found \"def/function\" keyword");
                return false;
            }
            //skip
            skip_space_end_comment(ptr);
            //parse variable
            if(is_exp && !is_variable(ptr))
            {
                //lambda
                l_variable = nullptr;
            }
            else if(!parse_variable(ptr, l_variable))
            {
                push_error("not found \"variable\" of def");
                return false;
            }
            //cast
            node = l_syntactic_tree::function_def(l_variable, m_line);
            //alloc node
            l_def_node = (l_syntactic_tree::function_def_node*)node;
            //parse args
            if(!parse_def_args(ptr,l_def_node))
            {
                //dealloc
                delete l_def_node;
                //to null
                node = nullptr;
                //return..
                return false;
            }
			// parse '{' staments '}' | stament
			if (!parse_stament_s_block(ptr, l_def_node->m_staments)) return false;
            //success
            return true;
        }
        //parse def operator
        bool parse_def_op(const char*& ptr, l_syntactic_tree::node*& node)
        {
            l_syntactic_tree::variable_node*     l_variable = nullptr;
            l_syntactic_tree::function_def_node* l_def_node = nullptr;
            //skip
            skip_space_end_comment(ptr);
            //serach def
            if (!KEYWORDCMP_SKIP(ptr, OPERATOR))
            {
                push_error("not found \"operator\" keyword");
                return false;
            }
            //variable string
            std::string variable_name;
            //skip
            skip_space_end_comment(ptr);
            //right?
            bool is_right = KEYWORDCMP_SKIP(ptr, RIGHT);
            //append type to name
            if(is_right)
            {
                variable_name = get_keyword(K_RIGHT);
                variable_name+= ":";
                //skip
                skip_space_end_comment(ptr);
            }
            //op type
                 if(CSTRCMP_SKIP(ptr, "+"))                     variable_name+="+";
            else if(CSTRCMP_SKIP(ptr, "-"))                     variable_name+="-";
            else if(CSTRCMP_SKIP(ptr, "*"))                     variable_name+="*";
            else if(CSTRCMP_SKIP(ptr, "/"))                     variable_name+="/";
            else if(CSTRCMP_SKIP(ptr, "%"))                     variable_name+="%";
            else if(!is_right && CSTRCMP_SKIP(ptr, "iterator")) variable_name+="iterator";
            else if(!is_right && CSTRCMP_SKIP(ptr, "next"))     variable_name+="next";
            else if(!is_right && CSTRCMP_SKIP(ptr, "key"))      variable_name+="key";
            else if(!is_right && CSTRCMP_SKIP(ptr, "value"))    variable_name+="value";
            else if(!is_right && CSTRCMP_SKIP(ptr, "valid"))    variable_name+="valid";
            else
            {
                push_error("not found valid operator of def");
                return false;
            }
            //create node variable
            l_variable = l_syntactic_tree::variable(variable_name,m_line);
            //cast
            node = l_syntactic_tree::function_def(l_variable, m_line);
            //alloc node
            l_def_node = (l_syntactic_tree::function_def_node*)node;
            //parse args
            if(!parse_def_args(ptr,l_def_node))
            {
                //dealloc
                delete l_def_node;
                //to null
                node = nullptr;
                //return..
                return false;
            }
            //is - and not have args...
            if(variable_name=="-" && !l_def_node->m_args.size())
            {
                l_variable->m_name = "one:-";
            }
            // parse '{' staments '}' | stament
            if (!parse_stament_s_block(ptr, l_def_node->m_staments)) return false;
            //success
            return true;
        }        
        //parse return
        bool parse_return(const char*& ptr, l_syntactic_tree::node*& node)
        {
            //ptrs
            l_syntactic_tree::return_node* return_node = nullptr;
            //skip
            skip_space_end_comment(ptr);
            //...
            if (!KEYWORDCMP_SKIP(ptr, RETURN))
            {
                push_error("not found \"return\" keyword");
                return false;
            }
            //skip
            skip_space_end_comment(ptr);
            //is void return?
            bool void_return = CSTRCMP(ptr, "}")               ||
                               is_a_keyword_but_not_value(ptr) ||
                               is_operation_slow(ptr,true);
            //alloc and init
            return_node = l_syntactic_tree::return_value(nullptr,m_line);
            //void return
            if(!void_return)
            {
                //skip
                skip_space_end_comment(ptr);
                //parse exp
                if(!parse_exp(ptr,return_node->m_exp))
                {
                    delete return_node;
                    return false;
                }
            }
            //save return
            node = return_node;
            //go back
            return true;
        }
        //parse class
        bool parse_class(const char*& ptr, l_syntactic_tree::node*& node)
        {
            //ptrs
            l_syntactic_tree::class_node* class_node = nullptr;
            //skip
            skip_space_end_comment(ptr);
            //class
            if(!KEYWORDCMP_SKIP(ptr, CLASS))
            {
                push_error("Not found class keyword");
                return false;
            }
            //alloc
            class_node = l_syntactic_tree::clazz();
            //skip
            skip_space_end_comment(ptr);
            //variable
            if(!parse_variable(ptr, *((l_syntactic_tree::node**)&(class_node->m_class_name))))
            {
                delete class_node;
                return false;
            }
            //skip
            skip_space_end_comment(ptr);
            //parse ':'
            if(CSTRCMP_SKIP(ptr, ":"))
            {
                //skip
                skip_space_end_comment(ptr);
                //parse list
                do
                {
                    //temp var node
                    l_syntactic_tree::node*  variable_node = nullptr;
                    //parse..
                    if(!parse_variable(ptr, variable_node))
                    {
                        delete class_node;
                        return false;
                    }
                    //add parent
                    class_node->add_parent((l_syntactic_tree::variable_node*)variable_node);
                    //skip
                    skip_space_end_comment(ptr);
                }
                while (CSTRCMP_SKIP(ptr, ","));
            }
            //skip
            skip_space_end_comment(ptr);
            //parse class body
            if(!parse_class_body(ptr,class_node))
            {
                delete class_node;
                return false;
            }
            //pass class node
            node = class_node;
            //var name
            return true;
        }
        //parse class body
        bool parse_class_body(const char*& ptr,l_syntactic_tree::class_node* class_node)
        {
            //fast access
            using attribute_def_type = l_syntactic_tree::class_node::attribute_def_type;
            //skip
            skip_space_end_comment(ptr);
            //{
            if (!CSTRCMP_SKIP(ptr, "{"))
            {
                push_error("not found \'{\' keyword");
                return false;
            }
            //skip
            skip_space_end_comment(ptr);
            //status
            attribute_def_type l_current_state = attribute_def_type::T_PUBLIC;
            //get class name
            //std::string def_class_name = "$"+class_node->get_class_name();
            //parse attribute and defs
            while(!CSTRCMP(ptr, "}"))
            {
                if(KEYWORDCMP(ptr, DEF) ||
                   KEYWORDCMP(ptr, FUNCTION))
                {
                    //ptr
                    l_syntactic_tree::node* node = nullptr;
                    //parse
                    if(!parse_def(ptr, node)) return false;
                    //cast type
                    auto* def_node = (l_syntactic_tree::function_def_node*)node;
                    //push
                    class_node->add_def(def_node,l_current_state);
                    //..
                }
                else if(KEYWORDCMP(ptr, OPERATOR))
                {
                    //ptr
                    l_syntactic_tree::node* node = nullptr;
                    //parse
                    if(!parse_def_op(ptr, node)) return false;
                    //cast type
                    auto* def_node = (l_syntactic_tree::function_def_node*)node;
                    //push
                    class_node->add_op(def_node,l_current_state);
                    //..
                }
                else if(KEYWORDCMP_SKIP(ptr, VAR))
                {
                    do
                    {
                        skip_space_end_comment(ptr);
                        //get name
                        std::string variable_name;
                        if (!parse_name(ptr, &ptr, variable_name))
                        {
                            push_error("not valid attribute name");
                            return false;
                        }
                        //append
                        l_syntactic_tree::field_node* attr_variable=
                        l_syntactic_tree::field(l_syntactic_tree::variable("this",m_line),
                                                l_syntactic_tree::constant(variable_name,m_line));
                        //skip
                        skip_space_end_comment(ptr);
                        //..
                        if (is_assignment(ptr))
                        {
                            //node exp
                            l_syntactic_tree::exp_node *exp_node = nullptr;
                            //op name
                            std::string op_name;
                            //parse exp
                            if (!parse_assignment(ptr,op_name) || !parse_exp(ptr, exp_node))
                            {
                                if (exp_node) delete exp_node;
                                return false;
                            }
                            //append
                            class_node->add_attr(attr_variable,
                                                 exp_node,
                                                 l_current_state,
                                                 m_line);
                        }
                        else
                        {
                            //append
                            class_node->add_attr(attr_variable,
                                                 l_current_state,
                                                 m_line);
                        }
                        //jmp
                        skip_space_end_comment(ptr);
                    }
                    while (CSTRCMP_SKIP(ptr, ","));
                    
                }
                else if(KEYWORDCMP_SKIP(ptr, PUBLIC))
                {
                    //change state
                    l_current_state = attribute_def_type::T_PUBLIC;
                    //jmp
                    skip_space_end_comment(ptr);
                    //jmp :
                    if (!CSTRCMP_SKIP(ptr, ":"))
                    {
                        push_error("not found \':\' keyword");
                        return false;
                    }
                }
                else if(KEYWORDCMP_SKIP(ptr, PRIVATE))
                {
                    //change state
                    l_current_state = attribute_def_type::T_PRIVATE;
                    //jmp
                    skip_space_end_comment(ptr);
                    //jmp :
                    if (!CSTRCMP_SKIP(ptr, ":"))
                    {
                        push_error("not found \':\' keyword");
                        return false;
                    }
                    
                }
                else if(KEYWORDCMP_SKIP(ptr, PROTECTED))
                {
                    //change state
                    l_current_state = attribute_def_type::T_PROTECTED;
                    //jmp
                    skip_space_end_comment(ptr);
                    //jmp :
                    if (!CSTRCMP_SKIP(ptr, ":"))
                    {
                        push_error("not found \':\' keyword");
                        return false;
                    }
                }
                else
                {
                    push_error("invalid class declaretion");
                    return false;
                }
                //..
                //skip
                skip_space_end_comment(ptr);
            }
            //then
            if (!CSTRCMP_SKIP(ptr, "}"))
            {
                push_error("not found \'}\' keyword");
                return false;
            }
            //skip spaces
            skip_space_end_comment(ptr);
            //return success
            return true;
        }
        //parse itlanguage
		bool parse_llanguage(const char*& ptr, l_syntactic_tree::root_node*& node)
		{
			//skip
			skip_space_end_comment(ptr);
			//staments
			while (*ptr != EOF && *ptr!='\0')
			{
				//parse
				l_syntactic_tree::node* stament;
				if (!parse_stament(ptr, stament)) return false;
				//push stament
				node->append(stament);
				//skip
				skip_space_end_comment(ptr);
                //jump commond
                CSTRCMP_SKIP(ptr, ";");
                //skip
                skip_space_end_comment(ptr);
			}
			return true;
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////
        //pre build list
        using list_functions = std::vector<std::string>;
        using list_variables = std::vector<std::string>;
        //append to context
        void append_into_context(const list_variables& pre_builds_variables)
        {
            //put all into the context
            for(auto& name : pre_builds_variables) push_variable(name);
        }
        //parse language
        bool llanguage(const std::string& source,
                       l_syntactic_tree& tree)
        {
            //start parsing
            return llanguage(source,
                             tree,
                             list_variables());
        }
		//parse language
		bool llanguage(const std::string& source,
                       l_syntactic_tree& tree,
                       const list_variables& pre_builds_variables)
		{
            //stat at line 1
            m_line = 1;
            //append all
            append_into_context(pre_builds_variables);
            //contant string ptr
			const char* source_ptr = source.c_str();
            //start parsing
			return parse_llanguage(source_ptr, tree.m_root);
		}
	};
};