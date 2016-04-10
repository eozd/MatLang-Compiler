#include "parser.hpp"
#include <iterator>
#include <sstream>
#include <algorithm>

Parser::Parser(SymbolTable* const sym_table_ptr)
	: prod_rules()
	, sym_table(sym_table_ptr)
	, expr_deque()
{
	if (sym_table == nullptr) {
		throw std::invalid_argument
			("Parser::Parser: SymbolTable cannot be NULL");
	}
	this->initialize_rules();
}
void Parser::sym_table_scalar(const std::vector<Token>& token_vec) const
{
	const auto name = token_vec.at(1).value();
	Variable sv(name, VariableType::Scalar, Dimensions(1, 1));
	sym_table->insert(sv);
}

void Parser::sym_table_vector(const std::vector<Token>& token_vec,
							  TokenCategory& result) const
{
	const auto name = token_vec.at(1).value();
	int size = std::stoi(token_vec.at(3).value());
	if (size == 0)
		throw std::runtime_error("Vector size cannot be 0");
	//size 1 vectors are scalars
	else if (size == 1) {
		Variable sv(name, VariableType::Scalar, Dimensions(1, 1));
		sym_table->insert(sv);
		result = TokenCategory::ScalarDeclaration;
	} else {
		Variable mv(name, VariableType::Matrix, Dimensions(size, 1));
		sym_table->insert(mv);
	}
}

void Parser::sym_table_matrix(const std::vector<Token>& token_vec,
							  TokenCategory& result) const
{
	const auto name = token_vec.at(1).value();
	int size1 = std::stoi(token_vec.at(3).value());
	int size2 = std::stoi(token_vec.at(5).value());
	if (size1 == 0 || size2 == 0)
		throw std::runtime_error("Matrix size cannot be 0");
	//size 1,1 matrices are scalars
	else if (size1 == 1 && size2 == 1) {
		Variable sv(name, VariableType::Scalar, Dimensions(1, 1));
		sym_table->insert(sv);
		result = TokenCategory::ScalarDeclaration;
	} else {
		Variable mv(name, VariableType::Matrix, Dimensions(size1, size2));
		sym_table->insert(mv);
	}
}

/**
  * Parsing algorithm sends the whole token_vector to derivation function.
  * According to the returned statement type, symbol table must be updated.
  *
  * A succesfull derivation must "consume" all tokens in the token_vector in the
  * sense that all tokens must be used in a statement. If the returned index
  * by the derivation algorithm is not at the end of the token_vec, then not
  * all tokens are used. We should produce an error.
  */
TokenCategory Parser::parse(std::vector<Token>& token_vec)
{
	size_t return_index = 0; //start from the beginning
	TokenCategory result; //resulting statement type
	const auto index_result = this->derivation(token_vec, 0);
	return_index = index_result.first;
	result		 = index_result.second;
	if (return_index != token_vec.size()) {
		std::ostringstream oss;
		oss << result << ": Too few tokens";
		throw std::runtime_error(oss.str());
	}
	//if they are declarations, we need to update the symbol table
	switch (result) {
		case TokenCategory::ScalarDeclaration:
			sym_table_scalar(token_vec);
			break;
		case TokenCategory::VectorDeclaration:
			sym_table_vector(token_vec, result);
			break;
		case TokenCategory::MatrixDeclaration:
			sym_table_matrix(token_vec, result);
			break;
		default:
			break;
	}
	return result;
}

/**
  * Given the token_vector, tries to understand the statement type of the
  * token_vec. Returns the index-after-derivation and statement type
  */
std::pair<size_t, TokenCategory>
Parser::derivation(std::vector<Token>& token_vec, size_t index)
{
	size_t start_index = index;
	std::string error_msg; //error_msg thrown at the end if no match is found
	//var to keep the max match count. Used for determining which error_msg
	//to throw at the end
	size_t max_match_count = 0;
	//resulting token category
	TokenCategory result;
	bool match = false; //if the token vector is matching the current rule
	for (const auto& name_rule_pair : prod_rules) {
		//how many of the tokens in token_vec and the corresponding rule in
		//prod_rules match
		size_t match_count = 0;
		index = start_index; //reset the index at the start of each derivation
		const TokenCategory& rule_name = name_rule_pair.first;
		const auto&			 rule_vec  = name_rule_pair.second;
		//copy the token vector. We will manipulate it throughout the checking
		//don't want the upcoming checks to be affected by previous manip.s
		auto token_vec_copy = token_vec;
		try { //try the current rule
			for (const auto& token_category : rule_vec) {
				if (this->is_terminal(token_category)) {
					if (token_vec_copy.at(index).category() != token_category) {
						std::ostringstream oss;
						oss << "Expected: " << token_category << " Found: "
							<< token_vec_copy.at(index).category();
						throw std::runtime_error(oss.str());
					} else {
						//matches. Then we advance.
						++match_count;
						++index;
					}
					//if it is a nonterminal, derive it.
				} else if (this->is_nonterminal(token_category)) {
					if (token_category == TokenCategory::Expression) {
						//insert ExpressionBegin anchor
						token_vec_copy.emplace(token_vec_copy.begin() + index,
								"EXPR_BEGIN", TokenCategory::ExpressionBegin);
						//index points to EXPR_BEGIN. Advance it.
						++index;
						size_t expr_start_index = index; //including
						//parse_expression populates the expr_stack with the postfix
						//exression
						index = parse_expression(token_vec_copy, index);
						//insert ExpressionEnd anchor
						token_vec_copy.emplace(token_vec_copy.begin() + index,
								"EXPR_END", TokenCategory::ExpressionEnd);
						//index points to EXPR_END. Okay, we want so.
						size_t expr_end_index = index; //not including
						index = this->replace_infix_with_postfix(token_vec_copy,
																expr_start_index,
																expr_end_index,
																index);
						//index points to EXPR_END. Advance it so that it points
						//to the token that comes after the expression in the
						//original tokenizer output
						++index;
						++match_count;
					} else if (token_category == TokenCategory::InitializerList) {
						index = parse_init_list(token_vec_copy, index);
						++match_count;
					} else {
						std::ostringstream oss;
						oss << "Unexpected nonterminal: " << token_category;
						throw std::runtime_error(oss.str());
					}
				} else { //it must be a terminal or nonterminal. If not, ERROR.
					std::ostringstream oss;
					oss << "Unexpected token category: " << token_category
						<< " is neither a terminal nor a nonterminal";
					throw std::runtime_error(oss.str());
				}
			}
		} catch (std::runtime_error e) {
			//this rule produces an error
			//if the current rule matches more tokens than the max,
			//then it is more likely that the given token stream belongs to
			//this rule. Assign the error message to the error produced by
			//this rule
			if (match_count > max_match_count) {
				max_match_count = match_count;
				std::ostringstream oss;
				oss << rule_name << ": " << e.what();
				error_msg = oss.str();
			}
			continue; //continue checking other rules
		} catch (std::out_of_range e) {
			//this rule produces an error
			if (match_count > max_match_count) {
				max_match_count = match_count;
				std::ostringstream oss;
				oss << rule_name << ": " << e.what();
				error_msg = oss.str();
			}
			continue; //continue checking other rules
		}
		result = rule_name;
		match = true;
		token_vec = token_vec_copy;
		break; //if no exception was thrown, current rule holds. Break.
	}
	if (!match) {
		throw std::runtime_error(error_msg);
	}
	return std::pair<size_t, TokenCategory>(index, result);
}

/** Parses a single expression and returns the index just after the last elem
  * of the expression.
  *	 indices:  index index+1 index+2 index+3
  *	 expr:       1     +        2      
  * RETURN: index+3
  *
  */
size_t Parser::parse_expression(std::vector<Token>& token_vec, size_t index)
{
	//advance the index by a term. parse_term returns the index just after the
	//end of a term
	index = parse_term(token_vec, index);
	//if there is a + or -, the rest is expression. If not, we simply return
	//a term.

	//check for end. For a number at the end of an expression, the expr is still
	//valid.
	if (index < token_vec.size()) {
		if (token_vec.at(index).category() == TokenCategory::AdditionOperator) {
			index = parse_expression(token_vec, index + 1);
			//put the + after parsing the expression so that we get postfix
			expr_deque.emplace_back("+", TokenCategory::AdditionOperator);
		} else if (token_vec.at(index).category() == TokenCategory::SubtractionOperator) {
			//If a substraction operator is encountered, we turn it to a
			//addition operator by subtracting the following term from 0
			//Since multiple subtraction operations may be chained, do it in a
			//loop
			do {
				expr_deque.emplace_back("0", TokenCategory::Integer);
				index = parse_term(token_vec, index + 1);
				expr_deque.emplace_back("-", TokenCategory::SubtractionOperator);
				expr_deque.emplace_back("+", TokenCategory::AdditionOperator);
			} while (index < token_vec.size() &&
					 token_vec.at(index).category() == TokenCategory::SubtractionOperator);
			//After we finish processing the subsequent subtraction operators,
			//check if there are still more operations.
			//If so, we need to handle them as well.
			if (index < token_vec.size() && token_vec.at(index).category() == TokenCategory::AdditionOperator) {
				index = parse_expression(token_vec, index + 1);
				expr_deque.emplace_back("+", TokenCategory::AdditionOperator);
			}
		}
	}
	return index;
}

/** Parses a single term and returns the index just after the last elem
  * of the term.
  *	 indices:  index index+1 index+2 index+3
  *	 expr:       1     *        2      
  * RETURN: index+3
  *
  */
size_t Parser::parse_term(std::vector<Token>& token_vec, size_t index)
{
	//advance the index by a term. parse_term returns the index just after the
	//end of a term
	index = parse_factor(token_vec, index);
	if (index < token_vec.size()) {
		if (token_vec.at(index).category() == TokenCategory::MultiplicationOperator) {
			index = parse_term(token_vec, index + 1);
			//put the * after parsing the term so that we get postfix
			expr_deque.emplace_back("*", TokenCategory::MultiplicationOperator);
		}
	}
	return index;
}

/** Parses a single factor and returns the index just after the last elem
  * of the factor.
  *	 indices:  index index+1 index+2 index+3 index+4 index+5
  *	 expr:       (     2        +      3      )      
  * RETURN: index+5
  *
  * A factor is defined as the smallest unit in an expression that doesn't
  * need to be evaluated any further by +-* operators.
  */
size_t Parser::parse_factor(std::vector<Token>& token_vec, size_t index)
{
	switch (token_vec.at(index).category()) {
		case TokenCategory::Integer:
		case TokenCategory::Real:
			//nothing to do. Simply push them to the stack.
			expr_deque.push_back(token_vec.at(index));
			//advance the index
			return index + 1;
		case TokenCategory::Identifier:
			//identifier should also push itself but need to care subscripting!
		{
			//push the identifier
			expr_deque.push_back(token_vec.at(index));
			//advance the index
			++index;
			//It may be simply an identifier or be a subscript operator
			if (index < token_vec.size() &&
				token_vec.at(index).category() == TokenCategory::OpenSquareBrackets)
			{
				//SUBSCRIPT
				index = parse_subscript(token_vec, index);
			}
			return index;
		}
		// ( 1 + 2 ) is a factor
		case TokenCategory::OpenParenthesis:
			index = parse_expression(token_vec, index + 1);
			if (token_vec.at(index).category() != TokenCategory::CloseParenthesis) {
				throw std::runtime_error("Factor: Closing parenthesis expected");
			}
			return index + 1;
		/* TR FUNCTION START */
		case TokenCategory::TrFunction:
			return parse_tr_func(token_vec, index);
		/* SQRT FUNCTION START */
		case TokenCategory::SqrtFunction:
			return parse_sqrt_func(token_vec, index);
		/* CHOOSE FUNCTION START */
		case TokenCategory::ChooseFunction:
			return parse_choose_func(token_vec, index);
		default:
		{
			std::ostringstream oss;
			oss << "Unexpected factor: " << token_vec.at(index).value();
			throw std::runtime_error(oss.str());
		}
	}
}

/** Parses a subscript operation without an identifier. That is, parses
  *
  *	[<exprsession>] OR [<expression>, <expression>]
  *
  * Returns the index just after the end of the subscript. That is,
  *
  *				  index
  *	[<expression>  ]
  *
  * returns index + 1
  */
size_t Parser::parse_subscript(std::vector<Token>& token_vec, size_t index)
{
	//add an (int) cast to the subscript operations.
	//C subscripting cannot contain doubles and we store scalars as
	//doubles.
	token_vec.at(index).set_value(token_vec.at(index).value() + "(int)");
	//push the open square brackets
	expr_deque.push_back(token_vec.at(index));
	//indexing may contain expressions
	index = parse_expression(token_vec, index + 1);
	if (token_vec.at(index).category() == TokenCategory::CloseSquareBrackets) {
		//single subscript
		//adjust the array indexing. We must put a -1 at the each index.
		//push it in postfix notation
		expr_deque.emplace_back("1", TokenCategory::Integer);
		expr_deque.emplace_back("-", TokenCategory::SubtractionOperator);
		//push the closing squera brackets
		expr_deque.push_back(token_vec.at(index));
		//push the second subscript operation
		//single subscript means the second index is 0
		expr_deque.emplace_back("[(int)", TokenCategory::OpenSquareBrackets);
		expr_deque.emplace_back("0", TokenCategory::Integer);
		expr_deque.emplace_back("]", TokenCategory::CloseSquareBrackets);
		return index + 1;
	} else if (token_vec.at(index).category() == TokenCategory::Comma) {
		//double subscript
		//for the comma, we push open close brackets for 2d subscript
		expr_deque.emplace_back("1", TokenCategory::Integer);
		expr_deque.emplace_back("-", TokenCategory::SubtractionOperator);
		expr_deque.emplace_back("]", TokenCategory::CloseSquareBrackets);
		expr_deque.emplace_back("[(int)", TokenCategory::OpenSquareBrackets);
		index = parse_expression(token_vec, index + 1);
		//push the closing square brakects
		expr_deque.emplace_back("1", TokenCategory::Integer);
		expr_deque.emplace_back("-", TokenCategory::SubtractionOperator);
		//push teh closing square brackets
		expr_deque.push_back(token_vec.at(index));
		if (token_vec.at(index).category() == TokenCategory::CloseSquareBrackets) {
			return index + 1;
		} else {
			std::ostringstream oss;
			oss << "Subscript operator: Unexpected token "
				<< token_vec.at(index);
			throw std::runtime_error(oss.str());
		}
	} else {
		std::ostringstream oss;
		oss << "Subscript operator: Unexpected token "
			<< token_vec.at(index);
		throw std::runtime_error(oss.str());
	}
}

/** Parses tr function.
  *
  *		tr(<expression>)
  */
size_t Parser::parse_tr_func(std::vector<Token>& token_vec, size_t index)
{
	if (token_vec.at(index + 1).category() != TokenCategory::OpenParenthesis) {
		throw std::runtime_error("tr function: Expected opening parenthesis");
	}
	//push tr keyword
	expr_deque.push_back(token_vec.at(index));
	//push open paren
	expr_deque.push_back(token_vec.at(index + 1));
	index = parse_expression(token_vec, index + 2);
	if (token_vec.at(index).category() != TokenCategory::CloseParenthesis) {
		throw std::runtime_error("tr function: Closing parenthesis expected");
	}
	//push closing paren
	expr_deque.push_back(token_vec.at(index));
	return index + 1;
}

/** Parses sqrt function.
  *
  *		sqrt(<expression>)
  */
size_t Parser::parse_sqrt_func(std::vector<Token>& token_vec, size_t index)
{
	if (token_vec.at(index + 1).category() != TokenCategory::OpenParenthesis) {
		throw std::runtime_error("sqrt function: Expected opening parenthesis");
	}
	//push tr keyword
	expr_deque.push_back(token_vec.at(index));
	//push open paren
	expr_deque.push_back(token_vec.at(index + 1));
	index = parse_expression(token_vec, index + 2);
	if (token_vec.at(index).category() != TokenCategory::CloseParenthesis) {
		throw std::runtime_error("sqrt function: Closing parenthesis expected");
	}
	//push closing paren
	expr_deque.push_back(token_vec.at(index));
	return index + 1;
}

/** Parses choose function.
  *
  *		choose(<expr>, <expr>, <expr>, <expr>)
  */
size_t Parser::parse_choose_func(std::vector<Token>& token_vec, size_t index)
{
	if (token_vec.at(index + 1).category() != TokenCategory::OpenParenthesis) {
		throw std::runtime_error("choose function: Expected opening parenthesis");
	}
	//push choose keyword
	expr_deque.push_back(token_vec.at(index));
	//push open paren
	expr_deque.push_back(token_vec.at(index + 1));
	//go to the next token. Next token is open parenthesis.
	//Inside the for loop, parsing the expression returns the index
	//of a comma. Then, we call the next parse_expression with simply
	//index + 1.
	//
	//To be able to call index + 1 for the first step, increment index.
	++index;
	for (int i = 0; i < 3; ++i) {
		index = parse_expression(token_vec, index + 1);
		if (token_vec.at(index).category() != TokenCategory::Comma) {
			throw std::runtime_error("choose function: Expected 4 comma separated expressions");
		}
		//push comma
		expr_deque.push_back(token_vec.at(index));
	}
	index = parse_expression(token_vec, index + 1);
	if (token_vec.at(index).category() != TokenCategory::CloseParenthesis) {
		throw std::runtime_error("choose function: Closing parenthesis expected");
	}
	//push closing paren
	expr_deque.push_back(token_vec.at(index));
	return index + 1;
}

/** Parses an initializer list and returns the end index just after the
  * closing curly braces
  */
size_t Parser::parse_init_list(std::vector<Token>& token_vec,
							   size_t index)
{
	if (token_vec.at(index).category() != TokenCategory::OpenCurlyBraces) {
		throw std::runtime_error("Initializer list: Expected opening curly braces");
	}
	++index; //advance the iter to read the curly braces
	//now it has passed the open curly braces
	//if no CloseCurlyBraces, then out of index and calle catches that.
	while (token_vec.at(index).category() != TokenCategory::CloseCurlyBraces) {
		token_vec.emplace(token_vec.begin() + index, "EXPR_BEGIN",
						  TokenCategory::ExpressionBegin);
		++index;
		const size_t expr_begin_index = index;
		index = parse_expression(token_vec, index);
		token_vec.emplace(token_vec.begin() + index, "EXPR_END",
						  TokenCategory::ExpressionEnd);
		const size_t expr_end_index = index;
		index = this->replace_infix_with_postfix(token_vec, expr_begin_index,
												 expr_end_index, index);
		++index;
	}
	//advance the close braces
	return index + 1;
} 

/** Initializes the production rule for each type of derivation rule. Rules
  * are defined sequentially.
  */
void Parser::initialize_rules()
{
	prod_rules[TokenCategory::ScalarDeclaration] = 
	{
		TokenCategory::ScalarType,
		TokenCategory::Identifier
	};

	prod_rules[TokenCategory::VectorDeclaration] = 
	{
		TokenCategory::VectorType,
		TokenCategory::Identifier,
		TokenCategory::OpenSquareBrackets,
		TokenCategory::Integer,
		TokenCategory::CloseSquareBrackets
	};

	prod_rules[TokenCategory::MatrixDeclaration] = 
	{
		TokenCategory::MatrixType,
		TokenCategory::Identifier,
		TokenCategory::OpenSquareBrackets,
		TokenCategory::Integer,
		TokenCategory::Comma,
		TokenCategory::Integer,
		TokenCategory::CloseSquareBrackets
	};

	prod_rules[TokenCategory::SingleForStatement] = 
	{
		TokenCategory::ForKeyword,
		TokenCategory::OpenParenthesis,
		TokenCategory::Identifier,
		TokenCategory::InKeyword,
		TokenCategory::Expression,
		TokenCategory::DoubleColon,
		TokenCategory::Expression,
		TokenCategory::DoubleColon,
		TokenCategory::Expression,
		TokenCategory::CloseParenthesis,
		TokenCategory::OpenCurlyBraces,
	};

	prod_rules[TokenCategory::DoubleForStatement] = 
	{
		TokenCategory::ForKeyword,
		TokenCategory::OpenParenthesis,
		TokenCategory::Identifier,
		TokenCategory::Comma,
		TokenCategory::Identifier,
		TokenCategory::InKeyword,
		TokenCategory::Expression,
		TokenCategory::DoubleColon,
		TokenCategory::Expression,
		TokenCategory::DoubleColon,
		TokenCategory::Expression,
		TokenCategory::Comma,
		TokenCategory::Expression,
		TokenCategory::DoubleColon,
		TokenCategory::Expression,
		TokenCategory::DoubleColon,
		TokenCategory::Expression,
		TokenCategory::CloseParenthesis,
		TokenCategory::OpenCurlyBraces,
	};

	//close curly braces signifies the end of a for loop
	prod_rules[TokenCategory::CloseCurlyBraces] =
	{
		TokenCategory::CloseCurlyBraces
	};

	prod_rules[TokenCategory::PrintStatement] = 
	{
		TokenCategory::PrintFunction,
		TokenCategory::OpenParenthesis,
		TokenCategory::Expression,
		TokenCategory::CloseParenthesis
	};

	prod_rules[TokenCategory::PrintSepStatement] = 
	{
		TokenCategory::PrintSepFunction,
		TokenCategory::OpenParenthesis,
		TokenCategory::CloseParenthesis
	};

	prod_rules[TokenCategory::ExprAssignment] = 
	{
		TokenCategory::Identifier,
		TokenCategory::AssignmentOperator,
		TokenCategory::Expression,
	};

	prod_rules[TokenCategory::SingleSubscriptExprAssignment] = 
	{
		TokenCategory::Identifier,
		TokenCategory::OpenSquareBrackets,
		TokenCategory::Expression,
		TokenCategory::CloseSquareBrackets,
		TokenCategory::AssignmentOperator,
		TokenCategory::Expression,
	};

	prod_rules[TokenCategory::DoubleSubscriptExprAssignment] = 
	{
		TokenCategory::Identifier,
		TokenCategory::OpenSquareBrackets,
		TokenCategory::Expression,
		TokenCategory::Comma,
		TokenCategory::Expression,
		TokenCategory::CloseSquareBrackets,
		TokenCategory::AssignmentOperator,
		TokenCategory::Expression,
	};

	prod_rules[TokenCategory::ListAssignment] = 
	{
		TokenCategory::Identifier,
		TokenCategory::AssignmentOperator,
		TokenCategory::InitializerList,
	};
}

/** Replaces the expression between the given indices [begin, end) in
  * the token_vec with the expression in this->expr_stack.
  * If the sizes of the original expression in the token_vec and the expr
  * in the expr_stack differ, then the taken index is adjusted and returned.
  */
size_t Parser::replace_infix_with_postfix(std::vector<Token>& token_vec,
										  size_t orig_expr_begin_index,
										  size_t orig_expr_end_index,
										  size_t current_index)
{
	//size of the original expression in token vector
	size_t orig_expr_size = orig_expr_end_index - orig_expr_begin_index;
	//iterator at the end of the expression
	auto orig_expr_end_it = token_vec.begin() + orig_expr_end_index;
	//insert the postfix expr stored in the deque after the
	//position where original expression ends in token_vec
	token_vec.insert(orig_expr_end_it, this->expr_deque.begin(),
									   this->expr_deque.end());
	//iterator at the beginning of the original expression
	//Need to get it after the insertion as insertion INVALIDATES any previous
	//iterators. Thus, orig_expr_end_it is INVALIDATED as of now.

	//DO NOT USE orig_expr_end_it FROM NOW ON. IT IS INVALIDATED.

	auto orig_expr_begin_it = token_vec.begin() + orig_expr_begin_index;
	//remove the original expression from the vector
	token_vec.erase(orig_expr_begin_it, orig_expr_begin_it + orig_expr_size);
	//Postfix and infix notation may differ in sizes. We must
	//adjust the index so that it still points to the element it was pointing
	//to before this method was called.
	if (expr_deque.size() < orig_expr_size)
		current_index -= (orig_expr_size - expr_deque.size());
	else if (expr_deque.size() > orig_expr_size)
		current_index += (expr_deque.size() - orig_expr_size);
	//clear the deque for possible upcoming uses
	this->expr_deque.clear();
	return current_index;
}
