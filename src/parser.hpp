#pragma once

#include <deque>
#include <set>
#include <memory>
#include <map>
#include <iostream>
#include <vector>
#include <string>
#include "token.hpp"
#include "symbol_table.hpp"

// Each sentence in the language is assumed to fit in one line. Thus,
// for simplicity, parser will parse the source code line by line.

/**
  * When parsing a single line, parser DOES CHANGE the order of tokens in the
  * given token stream (vector). Currently the only change occurs when
  * transforming the given expressions to postfix notation.

  * Later the expressions in postfix notation can be decoded by other modules
  * with the help of a stack. This way, operator precedence information is
  * preserved across modules.

  * Parsing is done with the help of an index. Each parse function parses
  * the token_vec starting from the index given to it and returns an index
  * where it's parsing is done. This way, by calling parse functions one after
  * another, we advance the index until the end is reached.
  */

class Parser {
public:
	//get a pointer to the symbol table to update. Parser does NOT own the
	//symbol table and thus, does NOT call delete on it.
	Parser(SymbolTable* sym_table_ptr);
	~Parser() {};
	Parser(const Parser&) = default;
	Parser& operator=(const Parser&) = default;
	//Parses the given token vector and returns the statement type.
	//If there are expressions in the token vector, replaces the infix 
	//expressions with their corresponding postfix versions.
	//Return value is a vector of terminals in the same order they were in the
	//source code.
	TokenCategory parse(std::vector<Token>& token_vec);
private:
	//initializes the production rules
	void initialize_rules();
	void sym_table_scalar(const std::vector<Token>&) const;
	void sym_table_vector(const std::vector<Token>&, TokenCategory&) const;
	void sym_table_matrix(const std::vector<Token>&, TokenCategory&) const;
	inline bool is_terminal(const TokenCategory& t) const
	{
		return this->terminal_categories.find(t)
			   !=
			   this->terminal_categories.end();
	};
	inline bool is_nonterminal(const TokenCategory& t) const
	{
		return this->nonterminal_categories.find(t)
			   !=
			   this->nonterminal_categories.end();
	};
	/**
	  * derivation and parse functions take the starting index that they will
	  * start parsing and return the index just after the index their parsing
	  * ends.
	  */
	//Try giving meaning to the token vector according to the specified
	//statement type given by category.
	std::pair<size_t, TokenCategory> derivation(std::vector<Token>& token_vec, size_t index);
	//Parses a single expression and returns the index of the element just past
	//the parsed expression
	size_t parse_expression(std::vector<Token>& token_vec, size_t index);
	//Parses a single term and returns the index of the element just past
	//the parsed term
	size_t parse_term(std::vector<Token>& token_vec, size_t index);
	//Parses a single factor and returns the index of the element just past
	//the parsed factor
	size_t parse_factor		(std::vector<Token>& token_vec, size_t index);
	size_t parse_subscript	(std::vector<Token>& token_vec, size_t index);
	size_t parse_tr_func	(std::vector<Token>& token_vec, size_t index);
	size_t parse_sqrt_func	(std::vector<Token>& token_vec, size_t index);
	size_t parse_choose_func(std::vector<Token>& token_vec, size_t index);
	//Parses a single initializer_list and returns the index of the element
	//just past the parsed factor
	size_t parse_init_list(std::vector<Token>& token_vec, size_t index);
	//replaces the expression between the given indices [begin, end) with the
	//postfix expression stored in the expr_deque.
	//Since the infix and postfix expressions may differ in size, this method
	//takes the current index and returns the updated index
	size_t replace_infix_with_postfix(std::vector<Token>& token_vec,
									  size_t orig_expr_begin_index, //including
									  size_t orig_expr_end_index, //not including
									  size_t current_index);
private:
	const std::set<TokenCategory> terminal_categories {
		TokenCategory::ScalarType,
		TokenCategory::VectorType,
		TokenCategory::MatrixType,
		TokenCategory::ForKeyword,
		TokenCategory::InKeyword,
		TokenCategory::PrintFunction,
		TokenCategory::PrintSepFunction,
		TokenCategory::TrFunction,
		TokenCategory::SqrtFunction,
		TokenCategory::ChooseFunction,
		TokenCategory::Identifier,
		TokenCategory::OpenSquareBrackets,
		TokenCategory::CloseSquareBrackets,
		TokenCategory::Integer,
		TokenCategory::Real,
		TokenCategory::AssignmentOperator,
		TokenCategory::OpenCurlyBraces,
		TokenCategory::CloseCurlyBraces,
		TokenCategory::OpenParenthesis,
		TokenCategory::CloseParenthesis,
		TokenCategory::AdditionOperator,
		TokenCategory::SubtractionOperator,
		TokenCategory::MultiplicationOperator,
		TokenCategory::Comma,
		TokenCategory::DoubleColon,
		TokenCategory::Dot
	};
	const std::set<TokenCategory> nonterminal_categories {
		TokenCategory::ScalarDeclaration,
		TokenCategory::VectorDeclaration,
		TokenCategory::MatrixDeclaration,
		TokenCategory::SingleForStatement,
		TokenCategory::DoubleForStatement,
		TokenCategory::EndForStatement,
		TokenCategory::PrintStatement,
		TokenCategory::PrintSepStatement,
		TokenCategory::ExprAssignment,
		TokenCategory::SingleSubscriptExprAssignment,
		TokenCategory::DoubleSubscriptExprAssignment,
		TokenCategory::ListAssignment,
		TokenCategory::Expression,
		TokenCategory::Term,
		TokenCategory::Factor,
		TokenCategory::InitializerList,
		TokenCategory::ExpressionBegin,
		TokenCategory::ExpressionEnd
	};
	//Statement category and the production rule associated with it.
	std::map<TokenCategory,std::vector<TokenCategory>> prod_rules;
	//const pointer. Pointer shouldn't change who it points to at any point
	SymbolTable* const sym_table;
	//deque used in transforming the expressions to postfix notation
	std::deque<Token> expr_deque;
};
