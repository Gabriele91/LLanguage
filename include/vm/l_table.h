//
//  l_table.h
//  LLanguage
//
//  Created by Gabriele on 28/01/16.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#pragma once
#include <map>
#include <memory>
#include <cstdlib>
#include <assert.h>
#include <unordered_map>
#include <l_object.h>
#include <l_variable.h>
#include <l_iterator.h>

namespace l_language
{
    //table
    class l_table;
    //table iterator
    class l_table_it;
    class l_call_context;
    class l_vm;
    class l_thread;
    
    
    class l_table : public l_obj
    {
        //friend class
        friend class l_table_it;
        friend class l_call_context;
        friend class l_vm;
        friend class l_thread;
        
        //pair
        struct l_pair
        {
            l_variable m_key;
            l_variable m_value;
        };
        
        //bucket
        struct l_bucket
        {
            l_pair     m_pair;
            l_bucket  *m_next { nullptr };
            bool       m_use  { false   };
        };
        
        //attributes
        size_t                   m_table_size;
        l_bucket*                m_table;
        size_t                   m_block_size;
        size_t                   m_block_pos;
        size_t                   m_num_keys;
        std::vector< l_bucket* > m_blocks;
        
        
        inline size_t str_hash(const char* key)
        {
            //input
            const size_t* cptr = (const size_t*)key;
            //return
            return (*cptr);
        };
        
        class l_table_raw_it
        {
            //table
            l_table* m_table{ nullptr };
            //id size
            size_t m_it{0};
            
        public:
            
            l_table_raw_it()
            {
                
            }
            
            l_table_raw_it(l_table& table,size_t it)
            :m_table(&table)
            ,m_it(it)
            {
                
            }
            
            l_table_raw_it(l_table& table)
            :m_table(&table)
            {
                
            }
            
            l_table_raw_it& operator=(const l_table_raw_it& left)
            {
                m_table = left.m_table;
                m_it    = left.m_it;
                return *this;
            }
            
            bool operator==(const l_table_raw_it& left) const
            {
                return  m_it == left.m_it && m_table == left.m_table;
            }
            
            bool operator!=(const l_table_raw_it& left) const
            {
                return  m_it != left.m_it || m_table != left.m_table;
            }
            
            l_table_raw_it& operator++()
            {
                while(m_it < m_table->m_blocks.size())
                {
                    if(m_table->m_blocks[m_it]->m_use)
                        return *this;
                    else
                        ++m_it;
                }
                return *this;
            }
            
            class
            
            l_pair& operator*() const
            {
                return m_table->m_blocks[m_it]->m_pair;
            }
            
            l_pair& operator->() const
            {
                return m_table->m_blocks[m_it]->m_pair;
            }
            
        };

        
        virtual void mark()
        {
            //..
            if(is_marked()) return;
            //mark
            l_obj::mark();
            //mark childs
            for(auto& it : *this)
            {
                //ref var
                l_variable& var = it.m_value;
                //mark
                if(var.is_object())
                if(var.is_unmarked())
                {
                    var.mark();
                }
                
                //key is static
                const l_variable& key = it.m_key;
                //is a object?
                if(key.is_object())
                if(key.is_unmarked())
                {
                    //object
                    l_obj* key_object = key.m_value.m_pobj;
                    //mark
                    key_object->mark();
                }
            }
        }
        
        virtual void unmark()
        {
            //..
            if(is_unmarked()) return;
            //mark
            l_obj::unmark();
            //mark childs
            for(auto it : *this)
            {
                //get value
                l_variable& var = it.m_value;
                //unmark
                if(var.is_object())
                if(var.is_marked())
                {
                    var.unmark();
                }
                //key is static
                const l_variable& key = it.m_key;
                //is a object?
                if(key.is_object())
                if(key.is_marked())
                {
                    //object
                    l_obj* key_object = key.m_value.m_pobj;
                    //mark
                    key_object->unmark();
                }
            }
        }
        
        
    public:
        
        l_table(size_t table_size = 1024, size_t block_size = 16)
        {
            assert((table_size & (table_size - 1)) == 0);	// Must be a power of two.
            m_table      = new l_bucket[table_size];
            m_table_size = table_size;
            m_block_pos  = m_block_size = block_size;
            m_num_keys   = 0;
        }
        
        ~l_table()
        {
            delete[] m_table;
            for (size_t i = 0; i != m_blocks.size(); ++i)
            {
                delete[] m_blocks[i];
            }
        }
        
        bool exists(const l_variable& var_key)
        {
            //key
            const std::string& key = var_key.string()->str();
            //hash
            size_t hash = str_hash(key.c_str()) & (m_table_size - 1);
            //get bucket from table
            l_bucket *first_bucket = m_table + hash;
            //bucket ptr
            l_bucket *bucket       = first_bucket;
            //search
            if (bucket->m_use)
            {
                do
                {
                    if (bucket->m_pair.m_key.string()->str() == key)
                    {
                        // Found
                        return true;
                    }
                    bucket = bucket->m_next;
                }
                while (bucket);
            }
            //not found
            return false;
        }
        
        l_variable& push(const l_variable& key,const l_variable& var)
        {
            return (operator[](key)=var);
        }
        
        l_variable& operator[](const l_variable& var_key)
        {
            //key
            const std::string& key = var_key.string()->str();
            //hash
            size_t hash = str_hash(key.c_str()) & (m_table_size - 1);
            //get bucket from table
            l_bucket *first_bucket = m_table + hash;
            //bucket ptr
            l_bucket *bucket       = first_bucket;
            //search
            if (bucket->m_use)
            {
                do
                {
                    //key
                    const std::string& bucket_key = var_key.string()->str();
                    //try // Found existing bucket
                    if (bucket_key == key)
                    {
                        return bucket->m_pair.m_value;
                    }
                    //next
                    bucket = bucket->m_next;
                }
                while (bucket);
            }
            // Add it
            ++m_num_keys;
            //this bucket is void?
            if (!first_bucket->m_use)
            {
                // Use bucket in table
                first_bucket->m_use = true;
                //add
                first_bucket->m_pair.m_key = var_key;
                //return
                return first_bucket->m_pair.m_value;
            }
            // Allocate a new bucket
            if (m_block_pos >= m_block_size)
            {
                l_bucket *block = new l_bucket[m_block_size];
                m_blocks.push_back(block);
                m_block_pos = 0;
            }
            //get new bucket
            bucket = m_blocks.back() + m_block_pos;
            //inc index
            ++m_block_pos;
            //save key
            bucket->m_pair.m_key = var_key;
            //is used
            bucket->m_use = true;
            //linking
            bucket->m_next       = first_bucket->m_next;
            first_bucket->m_next = bucket;
            //return value
            return bucket->m_pair.m_value;
        }
        
        size_t size() const
        {
            return m_num_keys;
        }
        
        l_table_raw_it begin()
        {
            return l_table_raw_it(*this);
        }
        
        l_table_raw_it end()
        {
            return l_table_raw_it(*this,m_blocks.size());
        }
        
        static l_variable gc_new(l_gc* gc);
        
        static l_variable gc_new(l_gc& gc);
        
        static l_variable gc_new(l_vm* vm);
        
        static l_variable gc_new(l_vm& vm);
        
        static l_variable const_new(l_gc* gc);
        
        static l_variable const_new(l_gc& gc);
        
        static l_variable const_new(l_vm* gc);
        
        static l_variable const_new(l_vm& gc);
        
        l_variable get_it();
    };
    
    class l_table_it : public l_iterator
    {
        l_table*                  m_table;
        l_table::l_table_raw_it   m_iterator;
        
        //mark event
        virtual void mark()
        {
            //..
            if(is_marked()) return;
            //mark
            l_obj::mark();
            m_table->mark();
        }
        
        //unmark event
        virtual void unmark()
        {
            //..
            if(is_unmarked()) return;
            //mark
            l_obj::unmark();
            m_table->unmark();
        }
        
    public:
        
        l_table_it(l_variable table)
        {
            if( table.m_type == l_variable::OBJECT )
            {
                //get vector
                m_table = dynamic_cast< l_table* >(table.m_value.m_pobj);
                //if is a vector
                if( m_table )
                {
                    //get iterator
                    m_iterator = m_table->begin();
                    //success
                    return;
                }
            }
            assert(0);
        }
        
        l_table_it(l_table* table)
        {
            //save table
            m_table = table;
            //get iterator
            m_iterator = m_table->begin();
        }
        
        l_table_it(const l_table::l_table_raw_it& c_it)
        {
            //get iterator
            m_iterator = c_it;
            assert(0);
        }
        
        static l_variable gc_new(l_gc* gc,l_table* vector);
        static l_variable gc_new(l_gc& gc,l_table* vector);
        static l_variable gc_new(l_vm* vm,l_table* vector);
        static l_variable gc_new(l_vm& vm,l_table* vector);
        static l_variable gc_new(l_gc* gc,l_table::l_table_raw_it c_it);
        static l_variable gc_new(l_gc& gc,l_table::l_table_raw_it c_it);
        static l_variable gc_new(l_vm* vm,l_table::l_table_raw_it c_it);
        static l_variable gc_new(l_vm& vm,l_table::l_table_raw_it c_it);
        
        virtual l_variable get() const
        {
            return (*m_iterator).m_value;
        }
        
        virtual l_variable get_id() const
        {
            return (*m_iterator).m_key;
        }
        
        virtual bool valid() const
        {
            return m_table && m_table->end() != m_iterator;
        }
        
        virtual l_variable next() const
        {
            //auto copy it
            l_table::l_table_raw_it next_it = m_iterator;
            //next
            ++next_it;
            //next
            return gc_new(l_obj::get_gc(),m_iterator);
        }
        
        virtual void self_next()
        {
            ++m_iterator;
        }
        
    };
    
    //get iterator
    inline l_variable l_table::get_it()
    {
        return l_table_it::gc_new(get_gc(), this);
    }
#if 0
    //implementation
    class l_table : public l_obj
    {
        //friend class
        friend class l_table_it;
        friend class l_call_context;
        friend class l_vm;
        friend class l_thread;
        //value compare
        struct l_value_compare
        {
			size_t operator()(const l_variable& left) const
			{
                #if 0
                    //assert(left.is_string());
                    std::hash<std::string> f_hash;
                    return f_hash(left.string()->str());
                #elif 0
                    //input
                    const char* cptr = left.string()->str().c_str();
                    //output
                    size_t output { 0 };
                    char* const optr = (char* const)&output;
                    //hash
                    for( size_t i = 0; cptr[i] && i < sizeof(size_t); ++i )
                    {
                        optr[i] = cptr[i];
                    }
                    //return
                    return output;
                #elif 1
                    //input
                    const size_t* cptr = (const size_t*)left.string()->str().c_str();
                    //return
                    return *cptr;
                #else
                    //input
                    const auto& cstr = left.string()->str();
                    const char* cptr = cstr.c_str();
                    //output
                    size_t output { 0 };
                    char*  optr = (char*)&output;
                    //mem
                    std::memcpy(optr, cptr, std::min(cstr.size(),sizeof(size_t)));
                    //return
                    return output;
                #endif
			}

            bool operator()(const l_variable& left,const l_variable& right) const
            {
                #if 1
                    //assert(left.is_string());
                    //assert(right.is_string());
                    return left.string()->str() == right.string()->str();
                #else
                    //pointers
                    const auto& lstr = left.string()->str();
                    const auto& rstr = right.string()->str();
                    //compare size
                    if( lstr.size() != rstr.size() ) return false;
                    //c compare
                    const char* lptr = lstr.c_str();
                    const char* rptr = rstr.c_str();
                    //compare
                    for(; *lptr && *rptr ;++lptr, ++rptr)
                        if(*lptr != *rptr)
                            return false;
                    //return...
                    return *lptr == *rptr;
                #endif
            }
        };
        //type
        using l_map_object           = std::unordered_map< l_variable,
														   l_variable,
														   l_value_compare,
													       l_value_compare >;
        using l_map_object_it        = l_map_object::iterator;
        using l_map_object_const_it  = l_map_object::const_iterator;
        //table
        l_map_object m_map;
        //mark event
        virtual void mark()
        {
            //..
            if(is_marked()) return;
            //mark
            l_obj::mark();
            //mark childs
            for(auto it:m_map)
            {
                l_variable& var = it.second;
                
                if(var.is_object())
                if(var.is_unmarked())
                {
                    var.mark();
                }
                
                //key is static
                const l_variable& key = it.first;
                //is a object?
                if(key.is_object())
                if(key.is_unmarked())
                {
                    //object
                    l_obj* key_object = key.m_value.m_pobj;
                    //mark
                    key_object->mark();
                }
            }
        }
        //unmark event
        virtual void unmark()
        {
            //..
            if(is_unmarked()) return;
            //mark
            l_obj::unmark();
            //mark childs
            for(auto it:m_map)
            {
                l_variable& var = it.second;
                
                if(var.is_object())
                if(var.is_marked())
                {
                    var.unmark();
                }
                //key is static
                const l_variable& key = it.first;
                //is a object?
                if(key.is_object())
                if(key.is_marked())
                {
                    //object
                    l_obj* key_object = key.m_value.m_pobj;
                    //mark
                    key_object->unmark();
                }
            }
        }
        //clear map
        void clear()
        {
            m_map.clear();
        }
        
    public:
        
		const l_map_object& raw() const
		{
			return m_map;
		}

        size_t size() const
        {
            return m_map.size();
        }
        
        l_variable& operator[](const l_variable& key)
        {
            #if defined( _DEBUG ) || defined( DEBUG )
            assert(key.is_string());
            #endif
            return m_map[key];
        }
        
        void push(const l_variable& key,const l_variable&  value)
        {
            #if defined( _DEBUG ) || defined( DEBUG )
            assert(key.is_string());
            #endif
            m_map[key] = value;
        }
        
        bool exists(const l_variable& key) const
        {
            assert(key.is_string());
            return m_map.find(key)!=m_map.end();
        }
        
        static l_variable gc_new(l_gc* gc);
        
        static l_variable gc_new(l_gc& gc);
        
        static l_variable gc_new(l_vm* vm);
        
        static l_variable gc_new(l_vm& vm);
        
        static l_variable const_new(l_gc* gc);
        
        static l_variable const_new(l_gc& gc);
        
        static l_variable const_new(l_vm* gc);
        
        static l_variable const_new(l_vm& gc);
        
        l_variable get_it();
        
    };
    class l_table_it : public l_iterator
    {
        l_table*                  m_table;
        l_table::l_map_object_it  m_iterator;
        
        //mark event
        virtual void mark()
        {
            //..
            if(is_marked()) return;
            //mark
            l_obj::mark();
            m_table->mark();
        }
        
        //unmark event
        virtual void unmark()
        {
            //..
            if(is_unmarked()) return;
            //mark
            l_obj::unmark();
            m_table->unmark();
        }
        
    public:
        
        l_table_it(l_variable table)
        {
            if( table.m_type == l_variable::OBJECT )
            {
                //get vector
                m_table = dynamic_cast< l_table* >(table.m_value.m_pobj);
                //if is a vector
                if( m_table )
                {
                    //get iterator
                    m_iterator = m_table->m_map.begin();
                    //success
                    return;
                }
            }
            assert(0);
        }
        
        l_table_it(l_table* table)
        {
            //save table
            m_table = table;
            //get iterator
            m_iterator = m_table->m_map.begin();
        }
        
        l_table_it(const l_table::l_map_object_it& c_it)
        {
            //get iterator
            m_iterator = c_it;
            assert(0);
        }
        
        static l_variable gc_new(l_gc* gc,l_table* vector);
        static l_variable gc_new(l_gc& gc,l_table* vector);
        static l_variable gc_new(l_vm* vm,l_table* vector);
        static l_variable gc_new(l_vm& vm,l_table* vector);
        static l_variable gc_new(l_gc* gc,l_table::l_map_object_it c_it);
        static l_variable gc_new(l_gc& gc,l_table::l_map_object_it c_it);
        static l_variable gc_new(l_vm* vm,l_table::l_map_object_it c_it);
        static l_variable gc_new(l_vm& vm,l_table::l_map_object_it c_it);
        
        virtual l_variable get() const
        {
            return m_iterator->second;
        }
        
        virtual l_variable get_id() const
        {
            return m_iterator->first;
        }
        
        virtual bool valid() const
        {
            return m_table && m_table->m_map.end() != m_iterator;
        }
        
        virtual l_variable next() const
        {
            //auto copy it
            l_table::l_map_object_it next_it = m_iterator;
            //next
            ++next_it;
            //next
            return gc_new(l_obj::get_gc(),m_iterator);
        }
        
        virtual void self_next()
        {
            ++m_iterator;
        }
        
    };
    
    //get iterator
    inline l_variable l_table::get_it()
    {
        return l_table_it::gc_new(get_gc(), this);
    }
#endif
}