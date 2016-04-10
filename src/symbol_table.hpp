#pragma once
#include <iostream>
#include <map>
#include "token.hpp"

/**
  * Dimensions represent the dimension of a matrix
  */
struct Dimensions {
	int rows;
	int cols;
	Dimensions(int t_rows, int t_cols)
		: rows(t_rows)
		, cols(t_cols)
	{};
};

bool operator==(const Dimensions& d1, const Dimensions& d2);
bool operator!=(const Dimensions& d1, const Dimensions& d2);
std::ostream& operator<<(std::ostream&, const Dimensions&);

/**
  * Types of variables in the system
  */
enum class VariableType {
	Scalar,
	Matrix
};

std::ostream& operator<<(std::ostream&, const VariableType&);

/**
  * Variable class. Represents both scalar and matrix variables
  */
class Variable {
public:
	Variable(const std::string& t_name,
			 const VariableType& t_type,
			 const Dimensions& t_dim)
		: m_name(t_name)
		, m_var_type(t_type)
		, m_dim(t_dim)
	{};
	VariableType type() const { return m_var_type; };
	Dimensions dim() const { return m_dim; };
	std::string name() const { return m_name; };
private:
	std::string m_name;
	VariableType m_var_type;
	Dimensions m_dim;
};

std::ostream& operator<<(std::ostream&, const Variable&);

/** Symbol table is defined as a map from the names of variables to variables
  */
class SymbolTable {
public:
	SymbolTable()
		:variables()
	{};
	//Having variables of different types with the same name is NOT ALLOWED
	void insert(const Variable&);

	//lookup checks if the requested variable exists in the map according to 
	//its type. If so, returns. If not,
	//throws a runtime_error with a specific message.
	Variable lookup(const std::string& t_name) const;

	void print() const; //for testing purposes

private:
	std::map<std::string, Variable> variables;
};
