R"SOURCE_CODE(
#include <string>
#include <vector>
#include <iostream>
#include <list>
#include <assert.h>
#include <cmath>

class it_obj;
class it_ref;
class it_variable;
class it_vector;
class it_gc;

class it_obj
{

public:
	
	bool is_marked() const
	{
		return m_mark;
	}

		size_t ref_count() const
	{
		return m_count;
	}

protected:

	virtual ~it_obj();

	void mark() const
	{
		if (is_marked()) return;
		m_mark = true;
		if (has_childs()) mark_childs();
	}

	void unmark() const
	{
		if (!is_marked()) return;
		m_mark = false;
		if (has_childs()) unmark_childs();
	}

	void inc_ref_count() const
	{
		//inc count of allocations
		++m_count;
	}

	void dec_ref_count() const
	{
		//dec counter
		--m_count;
		//auto dealloc
		if (!m_count && m_self_delete) delete this;
	}
	
	virtual void mark_childs() const = 0;

	virtual void unmark_childs() const = 0;

	virtual bool has_childs() const = 0;
	
	mutable it_gc* m_gc          { nullptr };
	mutable bool   m_mark        { false   };
    mutable bool   m_self_delete { true    };
	mutable size_t m_count       { 0	   };

	friend class   it_gc;
	friend class   it_variable;

};

class it_ref
{
public:

	//reference variable
	it_ref(it_variable& variable,bool is_in_stack);
	virtual ~it_ref();

	it_variable& variable() const
	{
		return m_variable;
	}

private:

	it_variable& m_variable;
	bool         m_is_in_stack{ false };

};

class it_variable
{
public:

	//types
	enum type
	{
		INT,
		FLOAT,
		STRING,
		OBJECT
	};

	union value
	{
		int   	      m_i;
		float  	 	  m_f;
		const it_obj* m_pobj;
		std::string*  m_pstr;
	};

	//attributes
	it_ref m_ref;
	type   m_type{ INT };
	value  m_value{ 0 };

	it_variable() : it_variable(0) {}

	it_variable(bool is_in_stack)
	:m_ref(*this, is_in_stack)
	{
		m_type = INT;
		m_value.m_i = 0;
	}

	it_variable(int i)
	:m_ref(*this, true)
	{
		m_type = INT;
		m_value.m_i = i;
	}

	it_variable(float f)
	:m_ref(*this, true)
	{
		m_type = FLOAT;
		m_value.m_f = f;
	}

	it_variable(const char* cstr)
	:m_ref(*this, true)
	{
		m_type = STRING;
		m_value.m_pstr = new std::string(cstr);
	}

	it_variable(const std::string& str)
	:m_ref(*this, true)
	{
		m_type = STRING;
		m_value.m_pstr = new std::string(str);
	}

	it_variable(const it_obj* obj,bool is_in_stack=true)
	:m_ref(*this, is_in_stack)
	{
		m_type = OBJECT;
		obj->inc_ref_count();
		m_value.m_pobj = obj;
	}
	
	it_variable(const it_variable& value, bool is_in_stack = true)
	:m_ref(*this, is_in_stack && m_type == OBJECT)
	{
		m_type = value.m_type;

		if (m_type == STRING)
		{
			m_value.m_pstr = new std::string(*value.m_value.m_pstr);
		}
		else if (m_type == OBJECT)
		{
			m_value = value.m_value;
			m_value.m_pobj->inc_ref_count();
		}
		else
		{
			m_value = value.m_value;
		}
	}

	it_variable& operator = (const it_variable& value)
	{
		//delete event..
			 if (m_type == STRING) delete m_value.m_pstr;
		else if (m_type == OBJECT) m_value.m_pobj->dec_ref_count();
		//copy type
		m_type = value.m_type;
		//copy value
		if (m_type == STRING)
		{
			m_value.m_pstr = new std::string(*value.m_value.m_pstr);
		}
		else if (m_type == OBJECT)
		{
			m_value = value.m_value;
			m_value.m_pobj->inc_ref_count();
		}
		else
		{
			m_value = value.m_value;
		}
		return *this;
	}

	virtual ~it_variable()
	{
			 if (m_type == STRING) delete m_value.m_pstr;
		else if (m_type == OBJECT) m_value.m_pobj->dec_ref_count();
	}

	it_variable operator + (it_variable var)
	{
		if (m_type == INT && var.m_type == INT)
		{
			return m_value.m_i + var.m_value.m_i;
		}

		if (m_type == FLOAT && var.m_type == INT)
		{
			return m_value.m_f + var.m_value.m_i;
		}

		if (m_type == INT && var.m_type == FLOAT)
		{
			return m_value.m_i + var.m_value.m_f;
		}

		if (m_type == FLOAT && var.m_type == FLOAT)
		{
			return m_value.m_f + var.m_value.m_f;
		}

		if (m_type == STRING && var.m_type == STRING)
		{
			return *m_value.m_pstr + *var.m_value.m_pstr;
		}

		if (m_type == STRING && var.m_type == FLOAT)
		{
			return *m_value.m_pstr + std::to_string(var.m_value.m_f);
		}

		if (m_type == STRING && var.m_type == INT)
		{
			return *m_value.m_pstr + std::to_string(var.m_value.m_i);
		}

		if (m_type == FLOAT && var.m_type == STRING)
		{
			return std::to_string(m_value.m_f) + *var.m_value.m_pstr;
		}

		if (m_type == INT && var.m_type == STRING)
		{
			return std::to_string(m_value.m_i) + *var.m_value.m_pstr;
		}

		assert(0);
		return it_variable();
	}

	it_variable operator - (it_variable var)
	{
		if (m_type == INT && var.m_type == INT)
		{
			return m_value.m_i - var.m_value.m_i;
		}

		if (m_type == FLOAT && var.m_type == INT)
		{
			return m_value.m_f - var.m_value.m_i;
		}

		if (m_type == INT && var.m_type == FLOAT)
		{
			return m_value.m_i - var.m_value.m_f;
		}

		if (m_type == FLOAT && var.m_type == FLOAT)
		{
			return m_value.m_f - var.m_value.m_f;
		}

		assert(0);
		return it_variable();
	}

	it_variable operator / (it_variable var)
	{
		if (m_type == INT && var.m_type == INT)
		{
			return m_value.m_i / var.m_value.m_i;
		}

		if (m_type == FLOAT && var.m_type == INT)
		{
			return m_value.m_f / var.m_value.m_i;
		}

		if (m_type == INT && var.m_type == FLOAT)
		{
			return m_value.m_i / var.m_value.m_f;
		}

		if (m_type == FLOAT && var.m_type == FLOAT)
		{
			return m_value.m_f / var.m_value.m_f;
		}

		assert(0);
		return it_variable();
	}

	it_variable operator * (it_variable var)
	{
		if (m_type == INT && var.m_type == INT)
		{
			return m_value.m_i * var.m_value.m_i;
		}

		if (m_type == FLOAT && var.m_type == INT)
		{
			return m_value.m_f * var.m_value.m_i;
		}

		if (m_type == INT && var.m_type == FLOAT)
		{
			return m_value.m_i * var.m_value.m_f;
		}

		if (m_type == FLOAT && var.m_type == FLOAT)
		{
			return m_value.m_f * var.m_value.m_f;
		}

		assert(0);
		return it_variable();
	}

	bool operator == (it_variable var)
	{
		if (m_type == INT && var.m_type == INT)
		{
			return m_value.m_i == var.m_value.m_i;
		}

		if (m_type == FLOAT && var.m_type == INT)
		{
			return m_value.m_f == var.m_value.m_i;
		}

		if (m_type == INT && var.m_type == FLOAT)
		{
			return m_value.m_i == var.m_value.m_f;
		}

		if (m_type == FLOAT && var.m_type == FLOAT)
		{
			return m_value.m_f == var.m_value.m_f;
		}

		assert(0);
		return false;
	}

	bool operator > (it_variable var)
	{
		if (m_type == INT && var.m_type == INT)
		{
			return m_value.m_i > var.m_value.m_i;
		}

		if (m_type == FLOAT && var.m_type == INT)
		{
			return m_value.m_f > var.m_value.m_i;
		}

		if (m_type == INT && var.m_type == FLOAT)
		{
			return m_value.m_i > var.m_value.m_f;
		}

		if (m_type == FLOAT && var.m_type == FLOAT)
		{
			return m_value.m_f > var.m_value.m_f;
		}

		assert(0);
		return false;
	}

	bool operator < (it_variable var)
	{
		if (m_type == INT && var.m_type == INT)
		{
			return m_value.m_i < var.m_value.m_i;
		}

		if (m_type == FLOAT && var.m_type == INT)
		{
			return m_value.m_f < var.m_value.m_i;
		}

		if (m_type == INT && var.m_type == FLOAT)
		{
			return m_value.m_i < var.m_value.m_f;
		}

		if (m_type == FLOAT && var.m_type == FLOAT)
		{
			return m_value.m_f < var.m_value.m_f;
		}

		assert(0);
		return false;
	}

	//gc methos

	bool is_marked() const
	{
		return m_value.m_pobj && m_value.m_pobj->is_marked();
	}

	void mark() const
	{
		assert(m_value.m_pobj);
		m_value.m_pobj->mark(); 
	}

	void unmark() const
	{
		assert(m_value.m_pobj);
		m_value.m_pobj->unmark();
	}

	void mark_childs() const
	{
		assert(m_value.m_pobj);
		return m_value.m_pobj->mark_childs();
	}

	void unmark_childs() const
	{
		assert(m_value.m_pobj);
		return m_value.m_pobj->unmark_childs();
	}

	bool has_childs() const
	{
		assert(m_value.m_pobj);
		return m_value.m_pobj->has_childs();
	}

	};

class it_vector : public it_obj
{
	
	class it_pool_object
	{
		it_variable m_variable;
	
	public:

		it_pool_object()
		:m_variable(false)
		{
		}

		it_pool_object(const it_variable& variable)
		:m_variable(false)
		{
			m_variable = variable;
		}

		it_variable& variable()
		{
			return m_variable;
		}

		const it_variable& variable() const
		{
			return m_variable;
		}

		operator it_variable&()
		{
			return m_variable;
		}

		operator const it_variable& () const
		{
			return m_variable;
		}
	};
	std::vector< it_pool_object > m_pool;

	virtual void mark_childs() const
	{
		for (auto& obj : m_pool)
			if (obj.variable().m_type == it_variable::OBJECT)
			{
				obj.variable().mark();
			}
	}

	virtual void unmark_childs() const
	{
		for (auto& obj : m_pool)
			if (obj.variable().m_type == it_variable::OBJECT)
			{
				obj.variable().unmark();
			}
	}

	virtual bool has_childs() const
	{
		return true;
	}

	public:

	size_t size() const
	{
		return m_pool.size();
	}

	it_variable& operator[](size_t i)
	{
		if (size() <= i) m_pool.resize(i+1);
		return m_pool[i];
	}

	void push(it_variable variable)
	{
		m_pool.push_back(variable);
	}
};

class it_gc
{
	//type object ref
	struct it_ref_obj
	{
		it_obj* m_obj{ nullptr };
		size_t  m_size{ 0 };

		it_ref_obj(it_obj* obj, size_t  size)
		{
			m_obj  = obj;
			m_size = size;
		}
	};
	//type of pool
	using it_pool = std::list< it_ref_obj >;
	using it_refs = std::list< it_ref* >;

public:

	static it_gc s_global_gc;
	static const size_t kbyte = 1024;
	static const size_t mbyte = 1024*1024;

	template < typename T >
	it_obj* new_obj()
	{
		//update size
		if (m_max_size >= m_size_allocs) delete_garbage();
		//alloc
		it_obj* optr =(it_obj*)new T();
        //put gc ref
        optr->m_gc = this;
		//push obj to pool
		push(optr,sizeof(T));
		//return ptr
		return optr;
	}

	void set_max_alloc_size(size_t max_alloc)
	{
		m_max_size = max_alloc;
	}

	//execute garbage collector
	void delete_garbage()
	{
		//mark fase
		unmark_pool();
		mark_pool();
		//list of object to delete
		std::vector< it_pool::iterator > to_delete;
		to_delete.reserve(m_pool.size());
		//search fase
		for (it_pool::iterator it  = m_pool.begin(); 
		                       it != m_pool.end(); 
							 ++it)
		{
			if (!it->m_obj->is_marked()) to_delete.push_back(it);
		}
		//disable ref count delete...
		for (it_pool::iterator it : to_delete) it->m_obj->m_self_delete = false;
		//delete
		for (it_pool::iterator it : to_delete) free(it);
	}


protected:


	//gc actions
	void push(it_ref* ref)
	{
		m_refs.push_back(ref);
	}

	void remove(it_refs::iterator it)
	{
		m_refs.erase(it);
	}

	void remove(it_ref* ref)
	{
		it_refs::iterator it;
		for (it  = m_refs.begin();
		     it != m_refs.end();
		   ++it)
		{
			if ((*it) == ref) { remove(it); return; }
		}
	}

	void push(it_obj* obj,size_t size)
	{
		m_size_allocs += size;
		m_pool.push_back(it_ref_obj( obj, size ));
	}

	void remove(it_pool::iterator it)
	{
		//dec size
		m_size_allocs -= it->m_size;
		//delete from pool
		m_pool.erase(it);
	}

	void remove(it_obj* obj)
	{
		it_pool::iterator it;
		for (it = m_pool.begin();
			 it != m_pool.end();
		   ++it)
		{
			if (it->m_obj == obj) { remove(it); return; }
		}
	}

	void free(it_obj* obj)
	{
		remove(obj);
		delete obj;
	}

	void free(it_pool::iterator it)
	{
		it_obj* obj = it->m_obj;
		remove(it);
		delete obj;
	}

	void mark_pool()
	{
		for (it_ref* ref_obj : m_refs)
			if (ref_obj->variable().m_type == it_variable::OBJECT)
			{
				ref_obj->variable().m_value.m_pobj->mark();
			}
	}

	void unmark_pool()
	{
		for (auto& ref_obj : m_pool)
		{
			ref_obj.m_obj->m_mark = false;
		}
	}

	//pool of the objects
	it_pool m_pool;
	//list of the refs
	it_refs m_refs;
	
	//allocations size
	size_t m_max_size   { kbyte*16 };
	size_t m_size_allocs{ 0        };

	//Object self delete
	friend class it_obj;
	friend class it_ref;
};
//global gc
it_gc it_gc::s_global_gc;
//destructor obj
inline it_obj::~it_obj()
{
	m_gc->remove(this);
}
//init ref
it_ref::it_ref(it_variable& variable, bool is_in_stack)
:m_variable(variable)
,m_is_in_stack(is_in_stack)
{
	if (m_is_in_stack)
	{
		it_gc::s_global_gc.push(this);
	}
}
it_ref::~it_ref()
{
	if (m_is_in_stack)
	{
		it_gc::s_global_gc.remove(this);
	}
}
//to int
it_variable to_integer(it_variable var)
{
	switch (var.m_type)
	{
	case it_variable::INT:    return var.m_value.m_i; break;
	case it_variable::FLOAT:  return (int)var.m_value.m_f; break;
	case it_variable::STRING: return std::stoi(*var.m_value.m_pstr, nullptr); break;
	default: assert(0); break;
	}
}
//field
it_variable& get_field_vector(it_variable v_array,it_variable index)
{
	assert(v_array.m_type == it_variable::OBJECT);
	it_variable i = to_integer(index);
	return (*((it_vector*)v_array.m_value.m_pobj))[i.m_value.m_i];
}


it_variable new_array()
{
    return it_variable(it_gc::s_global_gc.new_obj<it_vector>());
}

it_variable new_array(const std::vector< it_variable >& variables)
{
    it_variable l_variable = (it_gc::s_global_gc.new_obj<it_vector>());
    for(auto& variable: variables)
    {
        ((it_vector*)l_variable.m_value.m_pobj)->push(variable);
    }
    return l_variable;
}

it_variable len(it_variable var)
{
	assert(var.m_type == it_variable::OBJECT);
	return it_variable((int)((it_vector*)var.m_value.m_pobj)->size());
}

void push(it_variable v_array, it_variable value)
{
	assert(v_array.m_type == it_variable::OBJECT);
	((it_vector*)v_array.m_value.m_pobj)->push(value);
}

it_variable to_int(it_variable var)
{
	switch (var.m_type)
	{
	case it_variable::INT:    return var.m_value.m_i; break;
	case it_variable::FLOAT:  return (int)var.m_value.m_f; break;
	case it_variable::STRING: return std::stoi(*var.m_value.m_pstr, nullptr); break;
	default: assert(0); break;
	}
}

it_variable to_string(it_variable var)
{
	switch (var.m_type)
	{
	case it_variable::INT:    return std::to_string(var.m_value.m_i); break;
	case it_variable::FLOAT:  return std::to_string(var.m_value.m_f); break;
	case it_variable::STRING: return *var.m_value.m_pstr; break;
	default: assert(0); break;
	}
}

void print(it_variable var)
{
	switch (var.m_type)
	{
	case it_variable::INT: std::cout << var.m_value.m_i; break;
	case it_variable::FLOAT: std::cout << var.m_value.m_f; break;
	case it_variable::STRING: std::cout << *var.m_value.m_pstr; break;
	default: assert(0); break;
	}
}

template<typename ... Args>
void print(it_variable var, Args ... args)
{
	stampa(var);
	stampa(args...);
}

it_variable input()
{
	std::string value;
	std::cin >> value;
	return it_variable(value);
}

)SOURCE_CODE"