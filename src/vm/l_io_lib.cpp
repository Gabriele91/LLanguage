//
//  l_io_lib.cpp
//  LLanguage
//
//  Created by Gabriele on 23/03/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#include <l_io_lib.h>
#include <l_array.h>
#include <l_class.h>
#include <l_class_wrapper.h>
#include <cmath>

int l_print(l_language::l_thread* th,int args)
{
    for(size_t i=0; i!=args; ++i)
    {
        //
        const l_language::l_variable& var = th->value(i);
        //
        switch (var.m_type)
        {
            case l_language::l_variable::LNULL: printf("null"); break;
            case l_language::l_variable::LBOOL: printf("%s",var.m_value.m_b?"true":"false"); break;
            case l_language::l_variable::INT: printf("%d",var.m_value.m_i); break;
            case l_language::l_variable::FLOAT: printf("%g",var.m_value.m_f); break;
            case l_language::l_variable::STRING:
            {
                printf("%s",var.string()->c_str());
            }
            break;
            case l_language::l_variable::FUNCTION:
            {
                printf("function: %d",var.m_value.m_i);
            }
            case l_language::l_variable::CLOSER:
            {
                printf("closer: %p",(void*)var.m_value.m_pobj);
            }
            break;
            case l_language::l_variable::CFUNCTION:
            {
                printf("cfuncrion: %p",(void*)var.m_value.m_pcfun);
            }
            break;
            case l_language::l_variable::OBJECT:
            {
                printf("object: %p",(void*)var.m_value.m_pobj);
            }
            break;
            default: break;
        }
    }
    //number of return
    return 0;
}

int l_println(l_language::l_thread* th,int args)
{
    int output = l_print(th, args);
    printf("\n");
    return output;
}

int l_input(l_language::l_thread* th,int args)
{
    //str
    std::string str;
    std::cin >> str;
    //push string
    th->push_return(l_language::l_string::gc_new(th->m_vm->get_gc(), str));
    //return
    return 1;
}


class l_file : public l_language::l_ref
{
    FILE* m_file = nullptr;
    
public:
    
    l_file()
    {
    }
    
    l_file( std::string file, std::string mode )
    {
        open(file,mode);
    }
    
    virtual ~l_file()
    {
        if(m_file) close();
    }
    
    bool open( std::string file, std::string mode)
    {
        m_file = std::fopen(file.c_str(),mode.c_str());
        return m_file != nullptr;
    }
    
    bool write(std::string value)
    {
        return std::fwrite(value.c_str(), value.length(), 1, m_file) == value.length();
    }
    
    bool close()
    {
        bool success = std::fclose(m_file);
        m_file = nullptr;
        return success;
    }
    
    bool is_open()
    {
        return m_file != nullptr;
    }
    
    void print()
    {
        char cbuffer[128]= {'\0'};
        while(std::fread(&cbuffer, sizeof(char), 128, m_file))
        {
            std::cout <<  cbuffer;
        }
        close();
    }
    
    std::string read_all()
    {
        //alloc
        std::stringstream sstr;
        char cbuffer[128]= {'\0'};
        //read
        while(std::fread(&cbuffer[0], sizeof(char), 128, m_file))
            sstr << cbuffer;
        //close
        close();
        //return
        return sstr.str();
    }
    
    int tell()
    {
        return (int)std::ftell(m_file);
    }
    
    int size()
    {
        int pos = tell();
        //go to end
        std::fseek(m_file, 0, SEEK_END);
        //size
        int size = tell();
        //return to last pos
        std::fseek(m_file, pos, SEEK_SET);
        //return size
        return size;
    }
    
    void rewind()
    {
        //return to 0
        std::fseek(m_file, 0, SEEK_SET);
    }
    
    bool eof()
    {
        //return to 0
        return (bool)std::feof(m_file);
    }
    
    std::string read_line()
    {
        //alloc
        std::stringstream sstr;
        char cbuffer = '\0';
        //read
        while(std::fread(&cbuffer, sizeof(char), 1, m_file))
        {
            if(cbuffer&&cbuffer!='\n') sstr << cbuffer;
            else break;
        }
        //eof?
        if(eof()) close();
        //return
        return sstr.str();
    }
    
    static l_language::l_variable gc_new(l_language::l_gc* gc)
    {
        return { gc->new_obj<l_file>() };
    }
    
    static l_language::l_variable gc_new(l_language::l_gc& gc)
    {
        return { gc.new_obj<l_file>() };
    }
    
    static l_language::l_variable gc_new(l_language::l_vm* vm)
    {
        return { vm->get_gc().new_obj<l_file>() };
    }
    
    static l_language::l_variable gc_new(l_language::l_vm& vm)
    {
        return { vm.get_gc().new_obj<l_file>() };
    }
};


namespace l_language
{
    const l_vm::l_extern_libary& l_io_lib(l_vm* vm)
    {
        static l_language::l_vm::l_extern_libary l_lib=
        {
            { "print",     l_print              },
            { "println",   l_println            },
            { "input",     l_input              },
            { "file",      l_wrapper::l_create_class(
                           vm,
                           "file",
                           {
                                { "file",       l_constructor(l_file)      },
                                { "open",       l_method(l_file,open)      },
                                { "write",      l_method(l_file,write)     },
                                { "close",      l_method(l_file,close)     },
                                { "print",      l_method(l_file,print)     },
                                { "is_open",    l_method(l_file,is_open)   },
                                { "read_all",   l_method(l_file,read_all)  },
                                { "read_line",  l_method(l_file,read_line) },
                                { "tell",       l_method(l_file,tell)      },
                                { "size",       l_method(l_file,size)      },
                                { "rewind",     l_method(l_file,rewind)    },
                                { "end_of_file",l_method(l_file,eof)       },
                                { "eof",        l_method(l_file,eof)       },
                           })
            }
        };
        //return lib
        return l_lib;
    }
}