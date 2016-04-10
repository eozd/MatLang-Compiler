#include "symbol_table.hpp"
#include <iostream>
#include <stdexcept>
#include <sstream>

//no return specifies function will not return to its caller. It will 
//always throw an exception no matter what
[[ noreturn ]] void throw_already_exist(const std::string& name)
{
	std::ostringstream oss;
	oss << name << " is already declared";
	throw std::runtime_error(oss.str());
}

[[ noreturn ]] void throw_doesnt_exist(const std::string& name)
{
	std::ostringstream oss;
	oss << name << " is not declared";
	throw std::runtime_error(oss.str());
}

void SymbolTable::insert(const Variable& new_value)
{
	//pair<iterator, bool>
	//iterator is the pointer to the new elem
	//bool is if insertion was successful or not
	const auto it_bool = this->variables.emplace(new_value.name(), new_value);
	//if insertion wasn't successful.
	//insertion is unsuccessful when an element with the given key already exist
	if (!it_bool.second) {
		throw_already_exist(new_value.name());
	}
}

Variable SymbolTable::lookup(const std::string& name) const
{
	const auto it = variables.find(name);
	//if doesn't exist, throw.
	if (it == variables.end()) {
		throw_doesnt_exist(name);
	}
	//secnd is the value.
	return it->second;
}

void SymbolTable::print() const
{
	std::cout << "\t\t\tSYMBOL TABLE" << std::endl;
	for (int i = 0; i < 40; ++i)
		std::cout << "=";
	std::cout << std::endl;
	for (const auto name_var : variables) {
		std::cout << name_var.first << ": " << name_var.second << std::endl;
	}
}

bool operator==(const Dimensions& d1, const Dimensions& d2)
{
	return d1.rows == d2.rows && d1.cols == d2.cols;
}

bool operator!=(const Dimensions& d1, const Dimensions& d2)
{
	return !(d1 == d2);
}

std::ostream& operator<<(std::ostream& os, const Dimensions& dims)
{
	os << "[" << dims.rows << ", " << dims.cols << "]";
	return os;
}

std::ostream& operator<<(std::ostream& os, const VariableType& var_type)
{
	switch (var_type) {
		case VariableType::Scalar:
			os << "Scalar";
			return os;
		case VariableType::Matrix:
			os << "Matrix";
			return os;
		default:
			throw std::runtime_error
				("VariableType::operator<<: Unexpected category");
	}
}

std::ostream& operator<<(std::ostream& os, const Variable& variable)
{
	os << variable.type() << " " << variable.dim();
	return os;
}
