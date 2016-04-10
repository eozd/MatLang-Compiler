#include <sys/types.h>
#include <algorithm>
#include <string>
#include <iterator>
#include <memory>
#include <regex.h>
#include <iostream>
#include <stdexcept>
#include "regex.hpp"

Regex::Regex()
	//regfree is C POSIX regex cleanup function.
	: compiled(nullptr, regfree)
	, regex_pattern()
	, regex_flags(0)
{
	//by default match everything
	this->assign(R"(^$)", REG_EXTENDED);
}

Regex::Regex(const std::string& regex, int cflags)
	: compiled(nullptr, regfree) //give the deleter function regfree
	, regex_pattern(regex)
	, regex_flags(cflags)
{
	this->assign(this->regex_pattern, this->regex_flags);
}

Regex::Regex(const Regex& other)
	: compiled(nullptr, regfree)
	, regex_pattern(other.regex_pattern)
	, regex_flags(other.regex_flags)
{
	//assign will create a new regex from teh given parameters.
	this->assign(this->regex_pattern, this->regex_flags);
}

Regex::Regex(Regex&& other)
	: compiled(nullptr, regfree)
	, regex_pattern(other.regex_pattern)
	, regex_flags(other.regex_flags)
{
	*this = std::move(other);
}

Regex& Regex::operator=(Regex&& other)
{
	if (this != &other) {
		this->regex_pattern = other.regex_pattern;
		this->regex_flags = other.regex_flags;
		this->assign(this->regex_pattern, this->regex_flags);
		other.compiled.reset(nullptr);
	}
	return *this;
}

Regex& Regex::operator=(const Regex& other)
{
	if (this != &other) {
		this->regex_pattern = other.regex_pattern;
		this->regex_flags = other.regex_flags;
		this->assign(other.regex_pattern, other.regex_flags);
	}
	return *this;
}

void Regex::assign(const std::string& regex, int cflags)
{
	compiled.reset(new regex_t);
	int flag = regcomp(compiled.get(), regex.c_str(), cflags);
	if (flag != 0) { //then ERROR
		if (flag == REG_BADRPT)
			std::cout << "REG_BADRPT: repetition error" << std::endl;
		size_t buffer_size = regerror(flag, compiled.get(), (char*)NULL,
									  (size_t)0);
		std::unique_ptr<char> error_str(new char[buffer_size]);
		//get the error to *error_str
		regerror(flag, compiled.get(), error_str.get(), buffer_size);
		//copy the error_str. We don't want to throw dynamic object
		std::string copied_str(error_str.get(), buffer_size);
		throw std::runtime_error("Regex Error: " + copied_str);
	}
}

bool Regex::search(const char c) const
{
	std::string stringified_char(1, c);
	return this->search(stringified_char.c_str());
}

bool Regex::search(const std::string& str) const
{
	return this->search(str.c_str());
}

bool Regex::search(const char* const char_arr) const
{
	int flag = regexec(compiled.get(), char_arr, 0, nullptr, 0);
	if (flag == 0) {
		/* MATCH */
		return true;
	} else if (flag == REG_NOMATCH) {
		return false;
	} else { //if something bad happened
		//get the required buffer size for the error
		size_t buffer_size = regerror(flag, compiled.get(), nullptr, 0);
		std::unique_ptr<char> error_str(new char[buffer_size]);
		//get the error to *error_str
		regerror(flag, compiled.get(), error_str.get(), buffer_size);
		//copy the error_str. We don't want to throw dynamic object
		std::string copied_str(error_str.get(), buffer_size);
		throw std::runtime_error("Regex Error: " + copied_str);
	}
}
