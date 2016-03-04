//
//  tests_main.cpp
//  LLanguage
//
//  Created by Gabriele on 03/03/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#include <iostream>
#include <fstream>
#include <sstream>
#include <l_program_language.h>
#include <l_vector.h>
#include <l_lib_base.h>

//errors vector
size_t                     s_count_test    = 0;
size_t                     s_count_success = 0;
std::vector< std::string > s_tests_fails;

#define TEST(name, fun, return_test, ...)\
{\
    ++s_count_test;\
    std::initializer_list<l_language::l_variable> args=\
    {\
        __VA_ARGS__ \
    };\
    l_language::l_variable r_test(return_test);\
    l_language::l_variable result = it_compiler.pcall(#fun, args );\
    if((result==r_test).is_false()){ s_tests_fails.push_back( std::string(name) ); }\
    else{ ++s_count_success; }\
}

#define TEST_TYPE_OF(name, fun, type, ...)\
{\
    ++s_count_test;\
    std::initializer_list<l_language::l_variable> args=\
    {\
    __VA_ARGS__ \
    };\
    l_language::l_variable result = it_compiler.pcall(#fun, args );\
    if(result.m_type != l_language::l_variable::type){ s_tests_fails.push_back( std::string(name) ); }\
    else{ ++s_count_success; }\
}

#define TEST_ARRAY_INT(name, fun, return_test, ...)\
{\
    ++s_count_test;\
    std::initializer_list<l_language::l_variable> args=\
    {\
    __VA_ARGS__ \
    };\
    l_language::l_variable result = it_compiler.pcall(#fun, args );\
    if(!test_array_int(result,return_test)){ s_tests_fails.push_back( std::string(name) ); }\
    else{ ++s_count_success; }\
}


bool test_array_int(l_language::l_variable& var,const std::vector<int>& validetor)
{
    auto* l_array = var.to<l_language::l_vector>();
    //isn't array
    if(!l_array || l_array->size() != validetor.size()) return false;
    //else test values
    for(size_t i = 0; i != validetor.size(); ++i)
    {
        if( l_array->operator[](i).to_int() != validetor[i] )
        {
            return false;
        }
    }
    return true;
}

int gauss(int rand)
{
    int success_values = rand*(rand+1) / 2;
    return success_values;
}

int main()
{
    //fast access
    using program_language = l_language::l_program_language;
    using compiler_flags   = l_language::l_program_language::compiler_flags;
    //source file
    std::string i_source        = "tests/function.ll";
    int         f_compier_flags = compiler_flags::EXECUTE;
    //compiler object
    program_language it_compiler;
    //add libs
    it_compiler.add_lib("std", l_language::l_base_lib);
    //read code // "source.it"
    std::ifstream source_file(i_source);
    std::string source((std::istreambuf_iterator<char>(source_file)),
                       (std::istreambuf_iterator<char>()));
    //compile
    program_language::compiler_ouput compiler_ouput;
    compiler_ouput = it_compiler.compile(source,f_compier_flags);
    //ouput:
    if(compiler_ouput.m_type & program_language::ERRORS)
    {
        std::cout << compiler_ouput.m_errors;
        return -1;
    }
    //tests
    TEST("pow2",  // test name
          pow2,   // function
          36.0f,  // return
          6.0f    // args
         );
    
    TEST("diff",       // test name
          diff,        // function
          1.0f,        // return
          2.0f, 1.0f   // args
         );
    
    TEST("fib",      // test name
          fib,        // function
          89.0f,      // return
          10.0f       // args
         );
    
    TEST("super",      // test name
         super_test,   // function
         5.0f,         // return
         0.0f          // args
         );
    
    TEST("for of",      // test name
         for_of_test,   // function
         15.0f,         // return
         0.0f           // args
         );
    
    
    TEST("for in",      // test name
         for_in_test,   // function
         10.0f,         // return
         0.0f           // args
         );
    
    TEST_TYPE_OF("is int",   // test name
                 int_test,   // function
                 INT,        // return type
                 0           // args
                 );
    
    TEST_TYPE_OF("is float",   // test name
                 float_test,   // function
                 FLOAT,        // return type
                 0             // args
                 );
    TEST_TYPE_OF("is string",  // test name
                 string_test,  // function
                 STRING,       // return type
                 0             // args
                 );
    //1-100
    int range_values = rand() % 100 + 1;
    
    TEST("for range(len) rand",    // test name
         for_range_1_rand,         // function
         gauss(range_values-1),    // return
         range_values              // args
         );
    
    //0-(range_values-1)
    int start_values = rand() % range_values - 1 ;
    
    TEST("for range(start,len) rand",                    // test name
         for_range_2_rand,                               // function
         gauss(range_values-1)-gauss(start_values),      // return
         start_values,range_values                       // args
         );
    
    std::vector<int> array_values = { 1,3 };
    
    TEST_ARRAY_INT("range(start,len,step)",  // test name
         range_3,                            // function
         array_values,                       // return
         1,5,2                               // args
         );
    
    //print success
    std::cout << s_count_success << " of " << s_count_test << " successes" << std::endl;
    //print fails
    if(s_tests_fails.size())
        for(const std::string& test_name : s_tests_fails)
            std::cout << "- failed \"" << test_name << "\" test" << std::endl;
}