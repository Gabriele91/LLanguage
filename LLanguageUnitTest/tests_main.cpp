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
#include <l_lib_base.h>

//errors vector
std::vector< std::string > m_tests_fails;

#define TEST(name, fun, return_test, ...)\
{\
    std::initializer_list<l_language::l_variable> args=\
    {\
        __VA_ARGS__ \
    };\
    l_language::l_variable r_test(return_test);\
    l_language::l_variable result = it_compiler.pcall(#fun, args );\
    if((result==r_test).is_false()) m_tests_fails.push_back( std::string(name) );\
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
    TEST("x",        // test name
         x,          // function
         5.0f,       // return
         0.0f        // args
         );
    
    if(m_tests_fails.size())
        for(const std::string& test_name : m_tests_fails)
            std::cout << "fail " << test_name << " test" << std::endl;
    else
        std::cout << "success" << std::endl;
}