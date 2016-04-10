#pragma once
#include <string>
#include <iostream>
#include <stdexcept>
#include <vector>
#include "regex.hpp"
#include <map>

/**
  * Categories of tokens used in this program
  */
enum class TokenCategory {
	/* TERMINALS START */
	ScalarType,
	VectorType,
	MatrixType,
	ForKeyword,
	InKeyword,
	PrintFunction,
	PrintSepFunction,
	TrFunction,
	SqrtFunction,
	ChooseFunction,
	Identifier,
	OpenSquareBrackets,
	CloseSquareBrackets,
	Integer,
	Real,
	AssignmentOperator,
	OpenCurlyBraces,
	CloseCurlyBraces,
	OpenParenthesis,
	CloseParenthesis,
	AdditionOperator,
	SubtractionOperator,
	MultiplicationOperator,
	Comma,
	DoubleColon,
	Dot,
	/* NONTERMINALS START */
	ScalarDeclaration,
	VectorDeclaration,
	MatrixDeclaration,
	SingleForStatement,
	DoubleForStatement,
	EndForStatement,
	PrintStatement,
	PrintSepStatement,
	ExprAssignment,
	SingleSubscriptExprAssignment,
	DoubleSubscriptExprAssignment,
	ListAssignment,
	Expression,
	Term,
	Factor,
	InitializerList,
	ExpressionBegin,
	ExpressionEnd
};

class Token {
	public:
		//construct a token by a value string and a category
		Token(const std::string& str, const TokenCategory& category_param);
		//support char construction as well
		Token(const char str, const TokenCategory& category_param);

		std::string value() const { return this->m_str; };
		void set_value(const std::string& val) { m_str = val; };
		TokenCategory category() const { return this->m_category; };

	private:
		// use map to keep the order of categories intact
		std::string m_str;
		TokenCategory m_category;
};

/** Stores the representation of each Token type that needs representation.
  */
class TokenRepresentation {
public:
	//initializes the representation Regexes
	TokenRepresentation();
	//returns the regular expression representation of the given token category
	//if the given category doesn't contain a representation, throws an exception
	//Returned Regex object may only be used for searching and cannot be changed
	const Regex& representation(const TokenCategory& category) const;
private:
	void initialize_regex();
	std::map<TokenCategory, Regex> Category_to_regex;
};

std::ostream& operator<<(std::ostream& os, const Token& t);
std::ostream& operator<<(std::ostream& os, const TokenCategory& category);

bool operator==(const Token& t1, const Token& t2);
bool operator!=(const Token& t1, const Token& t2);
