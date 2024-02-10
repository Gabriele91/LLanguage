
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

void print_return(const l_language::l_variable& var)
{
    switch (var.m_type)
    {
      //case l_language::l_variable::LNULL: printf("null\n"); break;
        case l_language::l_variable::LBOOL: printf("%s\n",var.m_value.m_b?"true":"false"); break;
        case l_language::l_variable::INT: printf("%d\n",var.m_value.m_i); break;
        case l_language::l_variable::FLOAT: printf("%g\n",var.m_value.m_f); break;
        case l_language::l_variable::STRING:
        {
            printf("%s\n",var.string()->c_str());
        }
        break;
        case l_language::l_variable::FUNCTION:
        {
            printf("function: %d\n",var.m_value.m_i);
        }
        case l_language::l_variable::CLOSER:
        {
            printf("closer: %p\n",(void*)var.m_value.m_pobj);
        }
        break;
        case l_language::l_variable::CFUNCTION:
        {
            printf("cfuncrion: %p\n",(void*)var.m_value.m_pcfun);
        }
        break;
        case l_language::l_variable::OBJECT:
        {
            printf("object: %p\n",(void*)var.m_value.m_pobj);
        }
        break;
        default: break;
    }
}

void interactive_mode()
{
    //fast access
    using l_vmachine       = l_language::l_vm;
    using l_variable       = l_language::l_variable;
    using l_function       = l_language::l_function;
    using compiler_flags   = l_language::l_vm::compiler_flags;
    using compiler_ouput   = l_language::l_vm::compiler_ouput;
    //compiler object
    l_vmachine l_vmruntime;
    //add libs
    l_vmruntime.add_lib(l_language::l_base_lib);
    l_vmruntime.add_lib("io",  l_language::l_io_lib);
    l_vmruntime.add_lib("os",  l_language::l_os_lib);
    l_vmruntime.add_lib("math",l_language::l_math_lib);
    // Read lines
    std::string source_ctx;
    std::string source_buffer;
    std::string source_line;
    std::cout << "> ";
    // Copy context
    int main_function_id = l_vmruntime.thread(0).main_context()->get_fun_id();
    l_function main_function = l_vmruntime.function(main_function_id);
    // Get lines
    while (std::getline(std::cin, source_line)) 
    {
        // Append 
        source_buffer += source_line + '\n';
        // Compile
        compiler_ouput output = l_vmruntime.compile(source_buffer, 0);
        // Test error
        if(!!(output.m_type & l_language::l_vm::INCOMPLETE_ERROR))
        {  
            std::cout << ". ";
            continue;
        }
        else if(output.m_type & l_language::l_vm::ERRORS)
        {
            std::cout << "Errors:" << std::endl;
            std::cout << output.m_errors;
        }
        else 
        {
            // execute
            output = l_vmruntime.compile(source_buffer, l_language::l_vm::EXECUTE);
            // Output errors 
            if(output.m_type & l_language::l_vm::RUNTIME_ERROR)
            {
                std::cout << "Runtime errors:" << std::endl;
                std::cout << output.m_errors;
            }
            else if (l_vmruntime.thread(0).size())
            {
                print_return(l_vmruntime.thread(0).top());
            }
        }
        // Free source context
        source_buffer.clear();
        // Reset main function
        l_vmruntime.function(main_function_id) = main_function;
        // Clean GC, todo: fix class closer!
        l_vmruntime.get_gc().delete_garbage();
        std::cout << "> ";
    }
}

int main(int argc,const char* args[])
{
    //fast access
    using l_vmachine       = l_language::l_vm;
    using compiler_flags   = l_language::l_vm::compiler_flags;
    using compiler_ouput   = l_language::l_vm::compiler_ouput;
    //bad case
    if(argc < 1)
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
    for(size_t i=1; i < size_t(argc); ++i) v_args[i-1]=args[i];
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
    if(i_source.size())
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
    else
    {
        interactive_mode();
    }
    
    return 0;
}
