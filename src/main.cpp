
#include <iostream>
#include <fstream>
#include <sstream>
#include <l_vm.h>
#include <l_base_lib.h>
#include <l_io_lib.h>
#include <l_os_lib.h>
#include <l_math_lib.h>
#include <l_function_wrapper.h>
#define STR_VERSION_MAJOR "0"
#define STR_VERSION_MINOR "6"
#define STR_VERSION_MAINTENANCE "0"


int main(int argc,const char* args[])
{
    //fast access
    using l_vmachine       = l_language::l_vm;
    using compiler_flags   = l_language::l_vm::compiler_flags;
    using compiler_ouput   = l_language::l_vm::compiler_ouput;
    //bad case
    if(argc <= 1)
    {
        MESSAGE( "Invalid input" );
        return -1;
    }
    //name app
    std::string name(args[0]);
    //C++ args
    std::vector< std::string > v_args;
    //alloc args
    v_args.resize(argc-1);
    //copy args
    for(int i=1;i<argc;++i) v_args[i-1]=args[i];
    //options
    bool         b_show_version  = false;
    int          f_compier_flags = compiler_flags::NONE;
    std::string  i_source;
    {
        if(v_args.size()==1 && ( v_args[0]=="-h" || v_args[0]=="--help" ))
        {
            MESSAGE( name << " [options]" );
            MESSAGE( "Options:" );
            MESSAGE( "\t--help/-h help" );
            MESSAGE( "\t--version/-v compiler version" );
            MESSAGE( "\t--source/-s <path> source code" );
            MESSAGE( "\t--dump/-d dump bytecode" );
            return 0;
        }
        else if(v_args.size()==1)
        {
            MESSAGE("Invalid input");
            return -1;
        }
        else
        {
            //for all
            for(size_t arg=0; arg!= v_args.size() ; ++arg)
            {
                if (v_args[arg] == "-v" || v_args[arg] == "--version")
                {
                    b_show_version = true;
                }
                if (v_args[arg] == "-d" || v_args[arg] == "--dump")
                {
                    f_compier_flags |= compiler_flags::DUMP;
                }
                else if( (arg+1)!= v_args.size() )
                {
                    if (v_args[arg] == "-s" || v_args[arg] == "--source")
                    {
                        //save path
                        i_source = v_args[arg + 1]; ++arg;
                        //execute
                        f_compier_flags |= compiler_flags::EXECUTE;
                    }
                    else
                    {
                        MESSAGE( "Invalid input" );
                        return -1;
                    }
                }
                else
                {
                    MESSAGE( "Invalid input" );
                    return -1;
                }
            }
        }
    }
    //version info
    if(b_show_version)
    {
        std::cout << "L Language compiler, versione: " << STR_VERSION_MAJOR       << "."
                                                       << STR_VERSION_MINOR       << "."
                                                       << STR_VERSION_MAINTENANCE << "\n";
    }
    //COMPILER
    {
        //compiler object
        l_vmachine it_compiler;
        //add libs
        it_compiler.add_lib(l_language::l_base_lib);
        it_compiler.add_lib("io",  l_language::l_io_lib);
        it_compiler.add_lib("os",  l_language::l_os_lib);
        it_compiler.add_lib("math",l_language::l_math_lib);
        //read code // "source.it"
        std::ifstream source_file(i_source);
        std::string source((std::istreambuf_iterator<char>(source_file)),
                           (std::istreambuf_iterator<char>()));
        //compile
        l_language::l_vm::compiler_ouput compiler_ouput;
        compiler_ouput = it_compiler.compile(source,f_compier_flags);
        //ouput:
        if(compiler_ouput.m_type & l_language::l_vm::ERRORS)
        {
            std::cout << "Runtime errors:" << std::endl;
            std::cout << compiler_ouput.m_errors;
        }
    }
    
    return 0;
}
