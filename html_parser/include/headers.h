#pragma once

#include <exception>
#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <string>

#include <cstddef>
#include <cctype>
#include <cstring>

#include <boost/regex.hpp>

#define D_FUNCTION_DECL(ClassName) ClassName##Private* d_function() const

#define D_FUNCTION_IMPL(ClassName) ClassName##Private* ClassName::d_function() const \
{ \
	static ClassName##Private d_object; \
	return &d_object; \
}

#define D_FUNCTION(ClassName) ClassName##Private* const d_pointer = d_function()

#ifndef HTML_PARSER_LIBRARY

#define HTML_PARSER_EXPORT __declspec(dllimport)
#define HTML_PARSER_EXPORT_DEPS extern

#else

#define HTML_PARSER_EXPORT __declspec(dllexport)
#define HTML_PARSER_EXPORT_DEPS

#endif