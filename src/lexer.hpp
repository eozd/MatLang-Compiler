#pragma once

#include <string>
#include <vector>
#include "token.hpp"

/** Given a string, tokenizes it according to the rules defined in this class
  *
  */
class Lexer {
public:
	Lexer();
	//tokenizes the given line and returns the list of tokens
	std::vector<Token> tokenize(const std::string& line);

private:
	//categories that makes sense to lexer
	//anything that cannot be counted as one of these produces an error
	const std::vector<TokenCategory> token_categories {
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
	const TokenRepresentation tok_rep;
};
