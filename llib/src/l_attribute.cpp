//
//  l_attribute.cpp
//  LLanguage
//
//  Created by Gabriele on 05/07/17.
//  Copyright © 2016 Gabriele Di Bari. All rights reserved.
//
#include <sstream>
#include <l_attribute.h>
#include <l_class.h>


namespace l_language
{
	l_attribute::l_attribute()
	{
	}

	l_attribute::l_attribute(l_attribute_access access, const l_class* clazz, const l_variable& variable)
	{
		m_access = access;
		m_class = clazz;
		m_variable = variable;
	}

	l_attribute::operator l_variable& ()
	{
		return m_variable;
	}

	l_attribute::operator const l_variable& ()
	{
		return m_variable;
	}

	bool l_attribute::is_valid() const
	{
		return m_class != nullptr;
	}

	std::string l_attribute::to_string() const
	{
		std::stringstream sstr;
		//valid?
		if (is_valid()) sstr << m_class->get_class_name().string()->str();
		else            sstr << "unknown";
		sstr
			<< " ("
			<< (m_access == l_attribute_access::PUBLIC ?
				"public" :
				m_access == l_attribute_access::PUBLIC ?
				"protected" :
				"private"
				)
			<< ")";
		return sstr.str();
	}
}