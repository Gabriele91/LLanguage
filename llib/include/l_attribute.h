//
//  l_attribute.h
//  LLanguage
//
//  Created by Gabriele on 05/07/17.
//  Copyright � 2016 Gabriele Di Bari. All rights reserved.
//
#pragma once
#include <map>
#include <unordered_map>
#include <l_variable.h>
#include <l_table.h>

namespace l_language
{

	enum class l_attribute_access
	{
		PUBLIC,
		PROTECTED,
		PRIVATE
	};

	class l_attribute
	{
	public:

		l_attribute_access m_access{ l_attribute_access::PUBLIC };
		const l_class*	   m_class{ nullptr };
		l_variable		   m_variable;

		l_attribute();
		
		l_attribute(l_attribute_access access, const l_class* clazz, const l_variable& variable);
		
		explicit operator l_variable& ();

		explicit operator const l_variable& ();

		bool is_valid() const;

		std::string to_string() const;
	};

	//type
	using l_map_attribute		   = std::unordered_map< l_variable, l_attribute, l_value_compare, l_value_compare >;
	using l_map_attribute_it	   = l_map_attribute::iterator;
	using l_map_attribute_const_it = l_map_attribute::const_iterator;


}