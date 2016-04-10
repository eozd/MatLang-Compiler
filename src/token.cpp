#include "token.hpp"
#include <regex.h>
#include "regex.hpp"
#include <vector>

TokenRepresentation::TokenRepresentation()
	: Category_to_regex()
{
	this->initialize_regex();
}

const Regex& TokenRepresentation::representation(const TokenCategory& category) const
{
	//find the representation of the given category
	auto iter = Category_to_regex.find(category);
	//key do not exist
	if (iter == Category_to_regex.end()) {
		throw std::runtime_error("Token_Category_Map: Unexpected Token category");
	}
	//iter->second is the regex. iter->first is the key
	return iter->second;
}

/** Initialize the Token representation of each category
  */
void TokenRepresentation::initialize_regex()
{
	//we want exacty matches, not partial matches.
	//For a scalar, we do not want to match scalarr
	//Thus, use ^ and $ anchors to get the desired behaviour
	Category_to_regex[TokenCategory::ScalarType] =
		Regex(R"(^scalar$)");
	Category_to_regex[TokenCategory::VectorType] =
		Regex(R"(^vector$)");
	Category_to_regex[TokenCategory::MatrixType] =
		Regex(R"(^matrix$)");
	Category_to_regex[TokenCategory::ForKeyword] =
		Regex(R"(^for$)");
	Category_to_regex[TokenCategory::InKeyword] =
		Regex(R"(^in$)");
	Category_to_regex[TokenCategory::PrintSepFunction] =
		Regex(R"(^printsep$)");
	Category_to_regex[TokenCategory::PrintFunction] =
		Regex(R"(^print$)");
	Category_to_regex[TokenCategory::TrFunction] =
		Regex(R"(^tr$)");
	Category_to_regex[TokenCategory::SqrtFunction] =
		Regex(R"(^sqrt$)");
	Category_to_regex[TokenCategory::ChooseFunction] =
		Regex(R"(^choose$)");
	Category_to_regex[TokenCategory::Identifier] =
		Regex(R"(^[_[:alpha:]][_[:alnum:]]*$)");
	Category_to_regex[TokenCategory::OpenSquareBrackets] =
		Regex(R"(^\[$)");
	Category_to_regex[TokenCategory::CloseSquareBrackets] =
		Regex(R"(^]$)");
	Category_to_regex[TokenCategory::Integer] =
		Regex(R"(^[-+]?([0-9]|[1-9][0-9]*)$)");
	Category_to_regex[TokenCategory::Real] =
		Regex(R"(^[-+]?[0-9]|[1-9][0-9]*)\.([0-9]|[1-9][0-9]*)$)");
	Category_to_regex[TokenCategory::AssignmentOperator] =
		Regex(R"(^=$)");
	Category_to_regex[TokenCategory::OpenCurlyBraces] =
		Regex(R"(^\{$)");
	Category_to_regex[TokenCategory::CloseCurlyBraces] =
		Regex(R"(^\}$)");
	Category_to_regex[TokenCategory::OpenParenthesis] =
		Regex(R"(^\($)");
	Category_to_regex[TokenCategory::CloseParenthesis] =
		Regex(R"(^\)$)");
	Category_to_regex[TokenCategory::AdditionOperator] =
		Regex(R"(^\+$)");
	Category_to_regex[TokenCategory::SubtractionOperator] =
		Regex(R"(^-$)");
	Category_to_regex[TokenCategory::MultiplicationOperator] =
		Regex(R"(^\*$)");
	Category_to_regex[TokenCategory::Comma] =
		Regex(R"(^,$)");
	Category_to_regex[TokenCategory::DoubleColon] =
		Regex(R"(^:$)");
	Category_to_regex[TokenCategory::Dot] =
		Regex(R"(^\.$)");
}

Token::Token(const std::string& str, const TokenCategory& category_param)
	: m_str(str)
	, m_category(category_param)
{
}

Token::Token(const char str, const TokenCategory& category_param)
	: m_str(1, str) //1 length, lexeme
	, m_category(category_param)
{
}

std::ostream& operator<<(std::ostream& os, const TokenCategory& category)
{
	switch (category) {
		case TokenCategory::ScalarType:
			os << "ScalarType";
			break;
		case TokenCategory::VectorType:
			os << "VectorType";
			break;
		case TokenCategory::MatrixType:
			os << "MatrixType";
			break;
		case TokenCategory::ForKeyword:
			os << "ForKeyword";
			break;
		case TokenCategory::InKeyword:
			os << "InKeyword";
			break;
		case TokenCategory::PrintFunction:
			os << "PrintFunction";
			break;
		case TokenCategory::PrintSepFunction:
			os << "PrintSepFunction";
			break;
		case TokenCategory::TrFunction:
			os << "TrFunction";
			break;
		case TokenCategory::SqrtFunction:
			os << "SqrtFunction";
			break;
		case TokenCategory::ChooseFunction:
			os << "ChooseFunction";
			break;
		case TokenCategory::Identifier:
			os << "Identifier";
			break;
		case TokenCategory::OpenSquareBrackets:
			os << "OpenSquareBrackets";
			break;
		case TokenCategory::CloseSquareBrackets:
			os << "CloseSquareBrackets";
			break;
		case TokenCategory::Integer:
			os << "Integer";
			break;
		case TokenCategory::Real:
			os << "Real";
			break;
		case TokenCategory::AssignmentOperator:
			os << "AssignmentOperator";
			break;
		case TokenCategory::OpenCurlyBraces:
			os << "OpenCurlyBraces";
			break;
		case TokenCategory::CloseCurlyBraces:
			os << "CloseCurlyBraces";
			break;
		case TokenCategory::OpenParenthesis:
			os << "OpenParenthesis";
			break;
		case TokenCategory::CloseParenthesis:
			os << "CloseParenthesis";
			break;
		case TokenCategory::AdditionOperator:
			os << "AdditionOperator";
			break;
		case TokenCategory::SubtractionOperator:
			os << "SubtractionOperator";
			break;
		case TokenCategory::MultiplicationOperator:
			os << "MultiplicationOperator";
			break;
		case TokenCategory::Comma:
			os << "Comma";
			break;
		case TokenCategory::DoubleColon:
			os << "DoubleColon";
			break;
		case TokenCategory::Dot:
			os << "Dot";
			break;
		case TokenCategory::ScalarDeclaration:
			os << "ScalarDeclaration";
			break;
		case TokenCategory::VectorDeclaration:
			os << "VectorDeclaration";
			break;
		case TokenCategory::MatrixDeclaration:
			os << "MatrixDeclaration";
			break;
		case TokenCategory::SingleForStatement:
			os << "SingleForStatement";
			break;
		case TokenCategory::DoubleForStatement:
			os << "DoubleForStatement";
			break;
		case TokenCategory::EndForStatement:
			os << "EndForStatement";
			break;
		case TokenCategory::PrintStatement:
			os << "PrintStatement";
			break;
		case TokenCategory::PrintSepStatement:
			os << "PrintSepStatement";
			break;
		case TokenCategory::ExprAssignment:
			os << "ExprAssignment";
			break;
		case TokenCategory::SingleSubscriptExprAssignment:
			os << "SingleSubscriptExprAssignment";
			break;
		case TokenCategory::DoubleSubscriptExprAssignment:
			os << "DoubleSubscriptExprAssignment";
			break;
		case TokenCategory::ListAssignment:
			os << "ListAssignment";
			break;
		case TokenCategory::Expression:
			os << "Expression";
			break;
		case TokenCategory::Term:
			os << "Term";
			break;
		case TokenCategory::Factor:
			os << "Factor";
			break;
		case TokenCategory::InitializerList:
			os << "InitializerList";
			break;
		case TokenCategory::ExpressionBegin:
			os << "ExpressionBegin";
			break;
		case TokenCategory::ExpressionEnd:
			os << "ExpressionEnd";
			break;
		default:
			throw std::runtime_error(
					"TokenCategory.operator<<: Unexpected category");
	}
	return os;
}

std::ostream& operator<<(std::ostream& os, const Token& t)
{
	os << "Token: (" << t.value() << ", " << t.category() << ")";
	return os;
}

bool operator==(const Token& t1, const Token& t2)
{
	return t1.value() == t2.value() && t1.category() == t2.category();
}

bool operator!=(const Token& t1, const Token& t2)
{
	return !(t1 == t2);
}
