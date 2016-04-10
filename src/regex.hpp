#pragma once

#include <sys/types.h>
#include <vector>
#include <string>
#include <regex.h>
#include <memory>

/** A wrapper class for POSIX regex library
  *
  */
class Regex {
public:
	//construct a regex obj
	Regex();
	//no destructor needed. unique_ptr handles the resource by itself
	explicit Regex(const std::string& regex, int cflags = REG_EXTENDED);
	Regex(const Regex&);
	Regex(Regex&&);
	Regex& operator=(const Regex&);
	Regex& operator=(Regex&&);
	//assigns a new regex to the object, while effectively deleting the previous
	void assign(const std::string& regex, int cfags);
	//search for the regex in the given string. To match the whole string,
	//regex pattern should contain ^ and $ anchors.
	bool search(const std::string& str) const;
	//search the character
	bool search(const char) const;
	//returns the match results of last search or match operation

private:
	bool search(const char* const) const;

private:
	//specify unique ptr deleter
	std::unique_ptr<regex_t, void(*)(regex_t*)> compiled;
	std::string regex_pattern;
	int regex_flags;
};
