#include "code_generator.hpp"
#include <initializer_list>
#include <stack>
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include <sstream>

//given the message list, concatenates them and throws a runtime error
template<typename T, typename... Args>
[[ noreturn ]] void throw_error(const T&, Args... args);
//given the number, returns the strig "Error (Line num): "
std::string err_linenum(int num);
//compares the variable's type with the given type. If different,
//throws a runtime error
void confirm_type(const Variable&, const VariableType&);

void CodeGenerator::write_program_structure(std::ofstream& ofs) const
{
	write_preprocessor_commands(ofs);
	write_negative_matrix(ofs);
	write_matrix_matrix_multiply(ofs);
	write_matrix_matrix_add(ofs);
	write_matrix_matrix_subtract(ofs);
	write_scalar_matrix_multiply(ofs);
	write_matrix_assign(ofs);
	write_tr_function(ofs);
	write_choose_function(ofs);
	write_print_function(ofs);
	write_printsep_function(ofs);
}

void CodeGenerator::write_preprocessor_commands(std::ofstream& ofs) const
{
	ofs << "#include <stdio.h>" << std::endl;
	ofs << "#include <math.h>" << std::endl;
}

void CodeGenerator::write_tr_function(std::ofstream& ofs) const
{
	//tr function takes the size of the matrix to operate on and also
	//the result matrix that it is going to write to.
	ofs << "void tr(int size1, int size2, double matrix[size1][size2], double result[size2][size1])" << std::endl;
	ofs << "{" << std::endl;
	ofs << "\tint i;" << std::endl;
	ofs << "\tint j;" << std::endl;
	ofs << "\tfor (i = 0; i < size1; ++i) {" << std::endl;
	ofs << "\t\tfor (j = 0; j < size2; ++j) {" << std::endl;
	ofs << "\t\t\tresult[j][i] = matrix[i][j];" << std::endl;
	ofs << "\t\t}" << std::endl;
	ofs << "\t}" << std::endl;
	ofs << "}" << std::endl;
	ofs << std::endl;
}

void CodeGenerator::write_choose_function(std::ofstream& ofs) const
{
	ofs << "double choose(int condition, double first, double second, double third)"
		<< std::endl;
	ofs << "{" << std::endl;
	ofs << "\tif (condition == 0) {" << std::endl;
	ofs << "\t\treturn first;" << std::endl;
	ofs << "\t} else if (condition > 0) {" << std::endl;
	ofs << "\t\treturn second;" << std::endl;
	ofs << "\t} else {" << std::endl;
	ofs << "\t\treturn third;" << std::endl;
	ofs << "\t}" << std::endl;
	ofs << "}" << std::endl;
	ofs << std::endl;
}

void CodeGenerator::write_print_function(std::ofstream& ofs) const
{
	//For writing doubles as integers when they are "equal" to integers,
	//we do a check. If not "integer", then we print it with 7 precision.
	ofs << "void print(double value)" << std::endl;
	ofs << "{" << std::endl;
	ofs << "\tprintf(\"%g\\n\", value);" << std::endl;
	ofs << "}" << std::endl;
	ofs << std::endl;
	//Matrices should have their own function for printing
	ofs << "void print_mat(int size1, int size2, double matrix[size1][size2])" << std::endl;
	ofs << "{" << std::endl;
	ofs << "\tint i;" << std::endl;
	ofs << "\tint j;" << std::endl;
	ofs << "\tfor (i = 0; i < size1; ++i) {" << std::endl;
	ofs << "\t\tfor (j = 0; j < size2; ++j) {" << std::endl;
	ofs << "\t\t\tdouble value = matrix[i][j];" << std::endl;
	ofs << "\t\t\tprintf(\"%g\", value);" << std::endl;
	ofs << "\t\t\tif (j != size2 - 1)" << std::endl;
	ofs << "\t\t\t\tprintf(\"\\t\");" << std::endl;
	ofs << "\t\t}" << std::endl;
	ofs << "\t\t\tprintf(\"\\n\");" << std::endl;
	ofs << "\t}" << std::endl;
	ofs << "}" << std::endl;
	ofs << std::endl;
}

void CodeGenerator::write_printsep_function(std::ofstream& ofs) const
{
	ofs << "void printsep()" << std::endl;
	ofs << "{" << std::endl;
	ofs << "\tprintf(\"----------\\n\");" << std::endl;
	ofs << "}" << std::endl;
	ofs << std::endl;
}

void CodeGenerator::write_matrix_matrix_subtract(std::ofstream& ofs) const
{
	ofs << "void mat_mat_sub(int size1, int size2, double mat1[size1][size2], double mat2[size1][size2], double result[size1][size2])" << std::endl;
	ofs << "{" << std::endl;
	ofs << "\tint i;" << std::endl;
	ofs << "\tint j;" << std::endl;
	ofs << "\tfor (i = 0; i < size1; ++i) {" << std::endl;
	ofs << "\t\tfor (j = 0; j < size2; ++j) {" << std::endl;
	ofs << "\t\t\tresult[i][j] = mat1[i][j] - mat2[i][j];" << std::endl;
	ofs << "\t\t}" << std::endl;
	ofs << "\t}" << std::endl;
	ofs << "}" << std::endl;
	ofs << std::endl;
}

void CodeGenerator::write_matrix_matrix_add(std::ofstream& ofs) const
{
	ofs << "void mat_mat_add(int size1, int size2, double mat1[size1][size2], double mat2[size1][size2], double result[size1][size2])" << std::endl;
	ofs << "{" << std::endl;
	ofs << "\tint i;" << std::endl;
	ofs << "\tint j;" << std::endl;
	ofs << "\tfor (i = 0; i < size1; ++i) {" << std::endl;
	ofs << "\t\tfor (j = 0; j < size2; ++j) {" << std::endl;
	ofs << "\t\t\tresult[i][j] = mat1[i][j] + mat2[i][j];" << std::endl;
	ofs << "\t\t}" << std::endl;
	ofs << "\t}" << std::endl;
	ofs << "}" << std::endl;
	ofs << std::endl;
}

void CodeGenerator::write_negative_matrix(std::ofstream& ofs) const
{
	ofs << "void neg_mat(int size1, int size2, double mat[size1][size2], double result[size1][size2])" << std::endl;
	ofs << "{" << std::endl;
	ofs << "\tint i;" << std::endl;
	ofs << "\tint j;" << std::endl;
	ofs << "\tfor (i = 0; i < size1; ++i) {" << std::endl;
	ofs << "\t\tfor (j = 0; j < size2; ++j) {" << std::endl;
	ofs << "\t\t\tresult[i][j] = -mat[i][j];" << std::endl;
	ofs << "\t\t}" << std::endl;
	ofs << "\t}" << std::endl;
	ofs << "}" << std::endl;
}

void CodeGenerator::write_matrix_matrix_multiply(std::ofstream& ofs) const
{
	//For (MxN) (NxK) matrix multiplication, takes the sizes M, N, K
	//takes the matrices to multiply
	//takes the result matrix to write to
	ofs << "void mat_mat_mul(int size1_1, int common_size, int size2_2, double mat1[size1_1][common_size], double mat2[common_size][size2_2], double result[size1_1][size2_2])" << std::endl;
	ofs << "{" << std::endl;
	ofs << "\tint i;" << std::endl;
	ofs << "\tint j;" << std::endl;
	ofs << "\tint k;" << std::endl;
	ofs << "\tfor (i = 0; i < size1_1; ++i) {" << std::endl;
	ofs << "\t\tfor (j = 0; j < size2_2; ++j) {" << std::endl;
	ofs << "\t\t\tdouble sum = 0;" << std::endl;
	ofs << "\t\t\tfor (k = 0; k < common_size; ++k) {" << std::endl;
	ofs << "\t\t\t\tsum += mat1[i][k] * mat2[k][j];" << std::endl;
	ofs << "\t\t\t}" << std::endl;
	ofs << "\t\t\tresult[i][j] = sum;" << std::endl;
	ofs << "\t\t}" << std::endl;
	ofs << "\t}" << std::endl;
	ofs << "}" << std::endl;
	ofs << std::endl;
	//Multiplication of (1xN) (Nx1) results in a scalar.
	ofs << "double mat_mat_mul_s(int common_size, double mat1[1][common_size], double mat2[common_size][1])" << std::endl;
	ofs << "{" << std::endl;
	ofs << "\tint k;" << std::endl;
	ofs << "\tdouble sum = 0;" << std::endl;
	ofs << "\tfor (k = 0; k < common_size; ++k) {" << std::endl;
	ofs << "\t\tsum += mat1[0][k] * mat2[k][0];" << std::endl;
	ofs << "\t}" << std::endl;
	ofs << "\treturn sum;" << std::endl;
	ofs << "}" << std::endl;
}

void CodeGenerator::write_scalar_matrix_multiply(std::ofstream& ofs) const
{
	ofs << "void mat_sca_mul(int size1, int size2, double scalar, double matrix[size1][size2], double result[size1][size2])" << std::endl;
	ofs << "{" << std::endl;
	ofs << "\tint i;" << std::endl;
	ofs << "\tint j;" << std::endl;
	ofs << "\tfor (i = 0; i < size1; ++i) {" << std::endl;
	ofs << "\t\tfor (j = 0; j < size2; ++j) {" << std::endl;
	ofs << "\t\t\tresult[i][j] = scalar * matrix[i][j];" << std::endl;
	ofs << "\t\t}" << std::endl;
	ofs << "\t}" << std::endl;
	ofs << "}" << std::endl;
	ofs << std::endl;
}

void CodeGenerator::write_matrix_assign(std::ofstream& ofs) const
{
	ofs << "void mat_assign(int size1, int size2, double mat[size1][size2], double result[size1][size2])" << std::endl;
	ofs << "{" << std::endl;
	ofs << "\tint i;" << std::endl;
	ofs << "\tint j;" << std::endl;
	ofs << "\tfor (i = 0; i < size1; ++i) {" << std::endl;
	ofs << "\t\tfor (j = 0; j < size2; ++j) {" << std::endl;
	ofs << "\t\t\tresult[i][j] = mat[i][j];" << std::endl;
	ofs << "\t\t}" << std::endl;
	ofs << "\t}" << std::endl;
	ofs << "}" << std::endl;
}

/** From the given source file generates C code and writes it to the
  * out_file_name
  */
void CodeGenerator::generate_c_code(const std::vector<stmt_with_info>& src_file,
								    const std::string& out_file_name)
{
	std::ofstream ofs(out_file_name);
	if (!ofs) {
		throw_error(out_file_name, " couldn't be opened to write the output");
	}
	write_program_structure(ofs);
	//start main function. Everything is written inside main.
	ofs << "int main()" << std::endl;
	ofs << "{" << std::endl;
	this->indentation_level = 1;
	for (const auto& stmt_tuple : src_file) {
		//first --> token_vector
		//second --> statement category
		//third --> line number
		this->line_count = std::get<2>(stmt_tuple);
		const auto& token_vec = std::get<0>(stmt_tuple);
		//switch statement category
		switch (std::get<1>(stmt_tuple)) {
			case TokenCategory::ScalarDeclaration:
				write_scalar_declr(ofs, token_vec);
				break;
			case TokenCategory::VectorDeclaration:
				write_vector_declr(ofs, token_vec);
				break;
			case TokenCategory::MatrixDeclaration:
				write_matrix_declr(ofs, token_vec);
				break;
			case TokenCategory::SingleForStatement:
				write_single_for(ofs, token_vec);
				//when entering the for, we need to increase the indentation
				this->indentation_level++;
				break;
			case TokenCategory::DoubleForStatement:
				write_double_for(ofs, token_vec);
				//double for writes two nested for stmts. Need to increment
				//the indentation twice
				this->double_for = true;
				this->indentation_level += 2;
				break;
			case TokenCategory::CloseCurlyBraces:
				if (double_for)
					this->indentation_level -= 2;
				else
					this->indentation_level--;
				write_end_for(ofs);
				this->double_for = false;
				break;
			case TokenCategory::PrintStatement:
				write_print_stmt(ofs, token_vec);
				break;
			case TokenCategory::PrintSepStatement:
				write_printsep_stmt(ofs);
				break;
			case TokenCategory::ExprAssignment:
				write_expr_assignment(ofs, token_vec);
				break;
			case TokenCategory::SingleSubscriptExprAssignment:
				write_single_subscript_assignment(ofs, token_vec);
				break;
			case TokenCategory::DoubleSubscriptExprAssignment:
				write_double_subscript_assignment(ofs, token_vec);
				break;
			case TokenCategory::ListAssignment:
				write_list_assignment(ofs, token_vec);
				break;
			default:
			{
				throw_error("Error (Line ", std::get<2>(stmt_tuple),
							 "): Unexpected statement type: ",
							 std::get<1>(stmt_tuple));
			}
		}
	}
	//done. Close main and exit.
	ofs << "\treturn 0;" << std::endl;
	ofs << "}" << std::endl;
}

void CodeGenerator::write_scalar_declr(std::ofstream& ofs,
									   const std::vector<Token>& token_vec) const
{
	this->put_tabs(ofs);
	ofs << "double " << token_vec.at(1).value() << ";" << std::endl;
}

void CodeGenerator::write_vector_declr(std::ofstream& ofs,
									   const std::vector<Token>& token_vec) const
{
	//vector declaration is a 2d matrix declaration with second dimensio == 1
	this->put_tabs(ofs);
	ofs << "double " << token_vec.at(1).value() << "["
		<< token_vec.at(3).value() << "][1];"
		<< std::endl;
}

void CodeGenerator::write_matrix_declr(std::ofstream& ofs,
									   const std::vector<Token>& token_vec) const
{
	this->put_tabs(ofs);
	ofs << "double " << token_vec.at(1).value() << "["
		<< token_vec.at(3).value() << "][" << token_vec.at(5).value() << "];"
		<< std::endl;
}

/** Throws a runtime_error if the type of Variable is not the same as the give
  * VariableType
  *
  */
void CodeGenerator::confirm_type(const Variable& var, const VariableType& var_type) const
{
	if (var.type() != var_type) {
		throw_error(err_linenum(this->line_count), "Expected type: ",
					 var_type, ", Found type: ", var.type());
	}
}

/** Writes a single for statement to the ofs with the expressions
  * stored in token_vec.
  */
void CodeGenerator::write_single_for(std::ofstream& ofs,
									 const std::vector<Token>& token_vec) const
{
	/**
	  0   1 2 3   4			 5
	  for ( i in expr_begin expr expr_end : ... ) {
	  */
	//iterator must be scalar
	const auto first_name = token_vec.at(2).value();
	confirm_type(sym_table->lookup(first_name), VariableType::Scalar);
	//vector to store all three expressions of the for statement
	std::vector<Variable> c_expressions;
	/*			FIRST EXPRESSION  */
	//begin + 5 is the first token of the first expression
	auto it = token_vec.begin() + 5;
	//THREE EXPRESSIONS
	for (int i = 0; i < 3; ++i, it += 2) {
		c_expressions.push_back(convert_to_c_expr(it, ofs));
		confirm_type(c_expressions.back(), VariableType::Scalar);
		//pass the expr_end and double colon
	}
	this->put_tabs(ofs);
	//Write the whole for statement to ofs
	ofs << "for (" << first_name << " = " << c_expressions.at(0).name()
		<< "; " << first_name << " < " << c_expressions.at(1).name() << " + 1"
		<< "; " << first_name << " += " << c_expressions.at(2).name()
		<< ") {" << std::endl;
}

void CodeGenerator::write_double_for(std::ofstream& ofs,
									 const std::vector<Token>& token_vec) const
{
	/**
	   0  1  2  3  4  5     6		7		...
	  for ( id1 , id2 in expr_begin expr expr_end : ...) {
	  */
	const auto first_name = token_vec.at(2).value(); //first identifier
	const auto second_name = token_vec.at(4).value(); //second identifier
	//confirm the vars are scalar
	confirm_type(sym_table->lookup(first_name), VariableType::Scalar);
	confirm_type(sym_table->lookup(second_name), VariableType::Scalar);
	//vector to hold all 6 expressions
	std::vector<Variable> c_expressions;
	//start of the first expression
	auto it = token_vec.begin() + 7;
	//+=2 passes the expr_end token and comma token. It sets the iterator
	//to the beginning of the next expression. This also works for the comma
	//separating two expression sequences in double for.
	for (int i = 0; i < 6; ++i, it += 2) {
		c_expressions.push_back(convert_to_c_expr(it, ofs));
		confirm_type(c_expressions.back(), VariableType::Scalar);
	}
	this->put_tabs(ofs);
	ofs << "for (" << first_name << " = " << c_expressions.at(0).name()
		<< "; " << first_name << " < " << c_expressions.at(1).name() << "+1"
		<< "; " << first_name << " += " << c_expressions.at(2).name()
		<< ") {" << std::endl;
	this->put_tabs(ofs);
	//Two consecutive put_tabs wouldn't work here because put_tabs does not
	//put one tab. It puts this->indentation_level many tabs
	ofs << "\tfor (" << second_name << " = " << c_expressions.at(3).name()
		<< "; " << second_name << " < " << c_expressions.at(4).name() << "+1"
		<< "; " << second_name << " += " << c_expressions.at(5).name()
		<< ") {" << std::endl;
}

void CodeGenerator::write_end_for(std::ofstream& ofs) const
{
	//Adjust the indentation level according to the double_for condition
	if (double_for) {//if double for, we need to put one extra closing braces
		this->put_tabs(ofs);
		ofs << "\t}" << std::endl;
	}
	this->put_tabs(ofs);
	ofs << "}" << std::endl;
}

void CodeGenerator::write_print_stmt(std::ofstream& ofs,
									 const std::vector<Token>& token_vec) const
{
	//convert the expression to C expression
	auto it = token_vec.begin() + 3;
	const Variable result = convert_to_c_expr(it, ofs);
	this->put_tabs(ofs);
	switch (result.type()) {
		case VariableType::Matrix:
			ofs << "print_mat(" << result.dim().rows << ", "
				<< result.dim().cols << ", " << result.name() << ");"
				<< std::endl;
			break;
		case VariableType::Scalar:
			ofs << "print(" << result.name() << ");" << std::endl;
			break;
		default:
		{
			//shouldn't be entering here at all unless new types are added
			//to the program in which case this exception will be a warning
			throw_error("Print statement: Unexpecetd argument");
		}
	}
}

void CodeGenerator::write_printsep_stmt(std::ofstream& ofs) const
{
	this->put_tabs(ofs);
	ofs << "printsep();" << std::endl;
}

/** Writes a single subscript assignment to output C file.
  * A single subscript assignment has the following form:
  *
  *		A[<expression>] = <expression>
  *
  *	Both expression must evaluate to a scalar value. Otherwise, exception is
  * thrown
  *
  */
void CodeGenerator::write_single_subscript_assignment(std::ofstream& ofs,
									 const std::vector<Token>& token_vec) const
{
	const Variable id = sym_table->lookup(token_vec.at(0).value());
	this->confirm_type(id, VariableType::Matrix);
	if (id.dim().cols != 1) {
		throw_error(err_linenum(this->line_count),
					"Invalid subscript operation: ", id.name(),
					" has dimensions ", id.dim(),
					" but subscripted with only one expression");
	}
	//		0     1    2		3		...
	// identifier [ expr_begin expr expr_end ] = expr_begin expr expr_end
	auto iter = token_vec.begin() + 3;
	const Variable index = convert_to_c_expr(iter, ofs);
	this->confirm_type(index, VariableType::Scalar);
	//find the the start of the next expression
	iter = std::find_if(iter, token_vec.end(),
			[] (const Token& t)
			{
				return t.category() == TokenCategory::ExpressionBegin;
			});
	//advance the iter so that it points to the first token of the expression
	++iter;
	const Variable rhs = convert_to_c_expr(iter, ofs);
	this->confirm_type(rhs, VariableType::Scalar);
	this->put_tabs(ofs);
	ofs << token_vec.at(0).value() << "[(int)" << index.name() << " - 1][0] = "
		<< rhs.name() << ";" << std::endl;
}


/** Writes a double subscript assignment to output C file.
  * A single subscript assignment has the following form:
  *
  *		A[<expression>, <expression>] = <expression>
  *
  *	All expressions must evaluate to a scalar value. Otherwise, exception is
  * thrown
  *
  */
void CodeGenerator::write_double_subscript_assignment(std::ofstream& ofs,
									 const std::vector<Token>& token_vec) const
{
	const Variable id = sym_table->lookup(token_vec.at(0).value());
	this->confirm_type(id, VariableType::Matrix);
	//		0     1    2		3		...
	// identifier [ expr_begin expr expr_end , expr_begin expr expr_end ]
	// = expr_begin expr expr_end
	auto iter = token_vec.begin() + 3;
	const Variable index1 = convert_to_c_expr(iter, ofs);
	this->confirm_type(index1, VariableType::Scalar);
	//find the the start of the next expression
	//pass comma and expr_begin
	iter += 2;
	const Variable index2 = convert_to_c_expr(iter, ofs);
	this->confirm_type(index2, VariableType::Scalar);
	//find the beginning of the result expression
	iter = std::find_if(iter, token_vec.end(),
			[] (const Token& t)
			{
				return t.category() == TokenCategory::ExpressionBegin;
			});
	//advance the iter so that it points to the first token of the expression
	++iter;
	const Variable rhs = convert_to_c_expr(iter, ofs);
	this->confirm_type(rhs, VariableType::Scalar);
	this->put_tabs(ofs);
	ofs << token_vec.at(0).value() << "[(int)" << index1.name() << " - 1][(int)"
		<< index2.name() << " - 1] = " << rhs.name() << ";" << std::endl;
}

/** Writes an expression assignment to the output C file.
  * An expression assignment may have the following formats:
  * 
  *		A		= expression where A is an identifier previously defined
  *		A[i][0] = expression
  *		A[i][j] = expression
  * In the first case, resulting type of expression and A must match.
  * In the latter two cases, resulting type of expression MUST be a SCALAR
  * since we do not allow subscript operations that return types other than
  * scalars.
  */
void CodeGenerator::write_expr_assignment(std::ofstream& ofs,
									      const std::vector<Token>& token_vec) const
{
	/* auto expr_start_index = token_vec.begin(); */
	/* if (token_vec.at(1).category() == TokenCategory::OpenSquareBrackets) { */

	/* } */
	const Variable lhs = sym_table->lookup(token_vec.at(0).value());
	auto it = token_vec.begin() + 3;
	const Variable rhs = convert_to_c_expr(it, ofs);
	this->put_tabs(ofs);
	if (lhs.type() == VariableType::Scalar) {
		confirm_type(rhs, VariableType::Scalar);
		ofs << lhs.name() << " = " << rhs.name() << ";" << std::endl;
	} else if (lhs.type() == VariableType::Matrix) {
		confirm_type(rhs, VariableType::Matrix);
		if (lhs.dim() != rhs.dim()) {
			throw_error(err_linenum(this->line_count),
						"Invalid assignment: Matrices have different sizes");
		}
		//we assign to lhs. mat_assign accepts the result of the assignment
		//as its last argument.
		ofs << "mat_assign(" << lhs.dim().rows << ", " << lhs.dim().cols
			<< ", " << rhs.name() << ", "
			<< lhs.name() << ");" << std::endl;
	} else {
		throw_error(err_linenum(this->line_count),
					"Unexpected type for the LHS of the assignment: ",
					lhs.type());
	}
}

/** Writes a list assignment statement to output C file.
  * A list assignment statement has the following format:
  *
  *		A = {expression_list} where A is a previously defined matrix or vector
  *							  and expression_list is a list of expressions
  *							  separated with TokenCategory::ListSeparator.
  *
  * This method translates this list assignment to C code by assigning each
  * value of the matrix separately. Thus, if expression_list contains 40
  * expressions, this method would write 40 lines of assignments to the C code.
  */
void CodeGenerator::write_list_assignment(std::ofstream& ofs,
									      const std::vector<Token>& token_vec) const
{
	//		0	   1  2		3		4		...
	// identifier  =  {  expr_begin expr  expr_end expr_begin ...... }
	const auto name = token_vec.at(0).value();
	this->confirm_type(sym_table->lookup(name), VariableType::Matrix);
	int size1 = sym_table->lookup(name).dim().rows;
	int size2 = sym_table->lookup(name).dim().cols;
	//expression begin iterator
	auto it = token_vec.begin() + 4;
	for (int i = 0; i < size1; ++i) {
		for (int j = 0; j < size2; ++j) {
			//if there is still expressions to read and the iterator
			//is at close curly braces or at the end of the vector
			if (it->category() == TokenCategory::CloseCurlyBraces ||
				it == token_vec.end())
			{
				throw_error(err_linenum(this->line_count),
							"List Initialization: Expected expressions ",
							size1 + size2, " Found expressions ",
							i*size2 + j + 1);
			}
			const auto var = this->convert_to_c_expr(it, ofs);
			confirm_type(var, VariableType::Scalar);
			this->put_tabs(ofs);
			ofs << name << "[" << i << "][" << j << "] = " << var.name()
				<< ";" << std::endl;
			//pass expr_end and expr_begin so that it on the start of the next
			//expr
			it += 2;
		}
	}
	//after reading all, if current index is not at the closing curly braces,
	//there are extra expressions!
	if (it == token_vec.end())
		--it;
	if (it->category() != TokenCategory::CloseCurlyBraces) {
		throw_error(err_linenum(this->line_count),
					"List initialization: Expected closing curly braces, Found: ",
					it->value());
	}
}

/**
  * Finds the first unmatched opening parenthesis in the expr_stack.
  * The first unmatched opening parenthesis means it is the beginning element
  * of a function call. This can be shown as the following:
  *
  * expr_stack contains a postfix expression. Postfix expressions do not contai
  * parenthesis by default. The existing parenthesis in the expr_stack is put
  * by the parser while parsing function calls. Thus, each parenthesis in the
  * expression stack belongs to a function call. When we find an unmatched
  * opening parenthesis, that is the opening parenthesis of a function and also
  * the next element in the stack is the function name.
  *
  * Thus, this method after finding the unmatched parenthesis, reads one more
  * element to get the function name and returns all elements as a vector.
  *
  * ASSUMES: A single close parenthesis has been read previously. Thus, this
  * method doesn't read a closing parenthesis. It just finds a matching
  * opening parenthesis, also reads the function identifier and returns them
  * all as a vector.

  * ASSUMES: Syntax is correct. It SHOULD be if we are at code generation
  * after all.
  */
std::vector<Variable> get_function(std::vector<Variable>& expr_stack)
{
	int close_count = 0;
	int open_count = 0;
	std::vector<Variable> res;
	while (open_count - close_count != 1) {
		const Variable v = expr_stack.back();
		expr_stack.pop_back();
		if (v.name() == "(")
			++open_count;
		else if (v.name() == ")")
			++close_count;
		res.insert(res.begin(), v);
	}
	//get the identifier
	if (expr_stack.back().name() == "tr" ||
		expr_stack.back().name() == "choose" ||
		expr_stack.back().name() == "sqrt")
	{
		const Variable v = expr_stack.back();
		expr_stack.pop_back();
		res.insert(res.begin(), v);
	}
	res.emplace_back(")", VariableType::Scalar, Dimensions(1, 1));
	return res;
}

/**
  * This method finds the first unmatched opening square brackets that doesn't
  * have another closing square brackets as its next element. For example, 
  *
  *   ][oeueou
  *    ^ closing square bracket DOESN'T count as it has another closing square
  * brakects as its next element.
  *
  *  A[oeuoe][oeueu]
  *   ^  counts.
  *
  * After finding the first unmatched open square brakects, reads one more
  * element to get the identifier as well and returns all elements as a vector.
  * ASSUMES: A closing square brackets has already been read from the expr_stack
  *
  */
std::vector<Variable> get_whole_subscript(std::vector<Variable>& expr_stack)
{
	int close_count = 0;
	int open_count = 0;
	std::vector<Variable> res;
	//stop when we find a free open square brackets. However, 
	//when we find an open square bracket, we need to check the prev elem.
	//If it is a close square bracket, then we have a double subscript and
	//shouldn't stop in that case
	while (!(open_count - close_count == 1 &&
				expr_stack.back().name() != "]")) {
		const Variable v = expr_stack.back();
		expr_stack.pop_back();
		//subscript open square brakects are put (int) cast to suppress
		//gcc warnings. If the inside of the subscript is integer is checked
		//in syntax and semantic levels.
		if (v.name() == "[(int)")
			++open_count;
		else if (v.name() == "]")
			++close_count;
		res.insert(res.begin(), v);
	}
	//get the identfier and insert it also
	const Variable v = expr_stack.back();
	expr_stack.pop_back();
	res.insert(res.begin(), v);
	res.emplace_back("]", VariableType::Scalar, Dimensions(1, 1));
	return res;
}

/** Converts the given expression starting from the iterator parameter. Stops
  * when a TokenCategory::ExpressionEnd is found. If not ExpressionEnd is found
  * this function will go into an infinite loop and the results are undefined.

  * While converting it, compounds subexpressions, function calls
  * and subscript operations. That is, whenever a token stream of the form
  * A [ 1 ] [ 4 ] comes, all 6 tokens are compounded into a single variable.
  * This way, whenever an operation symbol is encountered, the operands are
  * always the first two elements of the stack.

  * Uses the information stored in the symbol table about identfiers to get
  * type and dimension information about the identifiers encountered in the
  * expression. This information is used when checking if a given operation
  * is allowed. For example, for the program
  *
  * scalar x
  * vector y[2]
  * vector k[2]
  * k = x*y
  * 
  * when * operation is encountered, y and x are fetched from the stack and
  * their types and dimensions are checked. Since one is scalar and one is
  * a vector, the operation is allowed and substituted with the corresponding
  * function call in the C program.

  * For the above program, the resulting C program contains a mat_sca_mul
  * function call with size parameters fetched from the symbol table.
  * mat_sca_mul requires the resulting matrix to be given as an argument as well.
  * For these purposes, this method defines helper variables whenvever needed
  * with the correct sizes and passes those helper variables to function calls.
  * Then the resulting C program looks like the following:
  *
  * double x;
  * double y[2][1];
  * double k[2][1];
  *
  * double _E4_1[2][1]; // The helper variable.
  * mat_sca_mul(2, 1, x, y, _E4_1);
  * assign_mat(2, 1, _E4_1, k);
  */
Variable CodeGenerator::convert_to_c_expr(citer& first, std::ofstream& ofs) const
{
	std::vector<Variable> expr_stack; //use as a stack
	for (; first->category() != TokenCategory::ExpressionEnd; ++first) {
		const TokenCategory category = first->category();
		//if there is an operation
		if (category == TokenCategory::AdditionOperator    ||
		    category == TokenCategory::SubtractionOperator ||
		    category == TokenCategory::MultiplicationOperator)
		{
			//RHS of +-*
			const Variable right_op = expr_stack.back();
			expr_stack.pop_back();
			//LHS of +-*
			const Variable left_op = expr_stack.back();
			expr_stack.pop_back();
			//depending on the type of operands, call corresponding functions
			//to convert each type of sub-expression
			if (left_op.type() == VariableType::Scalar &&
				right_op.type() == VariableType::Scalar)
			{
				convert_scalar_ops(expr_stack, category, left_op, right_op);
			} else if (left_op.type() == VariableType::Scalar &&
					   right_op.type() == VariableType::Matrix)
			{
				//Only allowed operations between scalar and a matrix:
				//
				// 1. Multiplication
				// 2. 0-<matrix>. This operation results in the negative of the
				// matrix.
				if (category == TokenCategory::SubtractionOperator &&
					left_op.name() == "0")
				{
					convert_mat_neg(expr_stack, ofs, right_op);
				} else if (category != TokenCategory::MultiplicationOperator) {
					throw_error(err_linenum(this->line_count),
								"Cannot add or subtract matrix and scalar");
				} else {
					convert_scalar_mat_mul(expr_stack, ofs, left_op, right_op);
				}
			} else if (left_op.type() == VariableType::Matrix &&
					   right_op.type() == VariableType::Scalar)
			{
				if (category != TokenCategory::MultiplicationOperator) {
					throw_error(err_linenum(this->line_count),
								"Cannot add or subtract matrix and scalar");
				}
													 // scalar,	 matrix
				convert_scalar_mat_mul(expr_stack, ofs, left_op, right_op);
			} else { //both MATRIX
				//if both matrix, call the appropriate function to do the work
				if (category == TokenCategory::AdditionOperator) {
					convert_mat_mat_add(expr_stack, ofs, left_op, right_op);
				} else if (category == TokenCategory::SubtractionOperator) {
					convert_mat_mat_sub(expr_stack, ofs, left_op, right_op);
				} else {
					//cannot be an operation other than multiplication if we
					//are in this block. If we are here, operand may only
					//be +, - or *
					convert_mat_mat_mul(expr_stack, ofs, left_op, right_op);
				}
			}
		} else if (category == TokenCategory::CloseParenthesis) {
			//function call
			this->convert_function_call(expr_stack, ofs);
		} else if (category == TokenCategory::CloseSquareBrackets) {
			//subscript operation
			this->convert_subscript(first, expr_stack);
		} else {
			//if it is an identifier, it should have an entry in symbol table.
			//query the sym table and get type and dimension info
			if (first->category() == TokenCategory::Identifier) {
				//get the type and dim info from the symbol table.
				//this way, we are embedding type and dim info to every
				//identifier we get from the given expression. Later, we use
				//this information to check if operations and function calls
				//are valid.
				Variable var_in_system = sym_table->lookup(first->value());
				//push the variable to the stack
				expr_stack.emplace_back(first->value(), var_in_system.type(),
										var_in_system.dim());
			} else {
				//we push the rest as scalars. This includes square brackets
				//and function names as well but type and dim info is not used
				//for those tokens.
				expr_stack.emplace_back(first->value(), VariableType::Scalar,
										Dimensions(1, 1));
			}
		}
	}
	return expr_stack.back();
}

void CodeGenerator::convert_mat_neg(std::vector<Variable>& expr_stack,
									std::ofstream& ofs,
									const Variable& right_op) const
{
	const auto var_name = this->get_unique_name();
	this->put_tabs(ofs);
	const auto dims = right_op.dim();
	ofs << "double " << var_name << "[" << dims.rows
		<< "][" << dims.cols << "];" << std::endl;
	this->put_tabs(ofs);
	ofs << "neg_mat(" << dims.rows << ", "
		<< dims.cols << ", " << right_op.name()
		<< ", " << var_name << ");" << std::endl;
	const Variable res(var_name, VariableType::Matrix, dims);
	sym_table->insert(res);
	expr_stack.push_back(res);
}

void CodeGenerator::convert_subscript(citer index,
									  std::vector<Variable>& expr_stack) const
{
	if ((index + 1)->category() != TokenCategory::ExpressionEnd &&
		(index + 1)->category() == TokenCategory::OpenSquareBrackets) {
		//means this close square brackets is the not the end of the
		//subscripting. We still have more for double subscript
		expr_stack.emplace_back("]", VariableType::Scalar, Dimensions(1, 1));
	} else {
		//every subscript in our system is a double susbcrip of the form
		// identifir [ scalar ] [ scalar ]
		//   0		 1    2   3 4   5    6
		const std::vector<Variable> var_seq = get_whole_subscript(expr_stack);
		//only matrices can be subscripted. Remember, vectors are stored as matrix
		confirm_type(sym_table->lookup(var_seq.front().name()), VariableType::Matrix);
		confirm_type(var_seq.at(2), VariableType::Scalar);
		confirm_type(var_seq.at(5), VariableType::Scalar);
		std::ostringstream oss;
		for (const auto& v : var_seq)
			oss << v.name();
		expr_stack.emplace_back(oss.str(), VariableType::Scalar, Dimensions(1, 1));
	}
}

void CodeGenerator::convert_function_call(std::vector<Variable>& expr_stack,
										  std::ofstream& ofs) const
{
	//It is a function call. Get the function and then deal with it !!
	const std::vector<Variable> whole_func = get_function(expr_stack);
	//first item of the function vector is the identfier
	if (whole_func.front().name() == "tr") {
		this->convert_tr_function(expr_stack, whole_func, ofs);
	} else if (whole_func.front().name() == "sqrt") {
		this->convert_sqrt_function(expr_stack, whole_func);
	} else if (whole_func.front().name() == "choose") {
		this->convert_choose_function(expr_stack, whole_func);
	} else {
		throw_error(err_linenum(this->line_count),
					"Unexpected function ", whole_func.front().name());
	}
}

void CodeGenerator::convert_choose_function(std::vector<Variable>& expr_stack,
											const std::vector<Variable>& var_seq) const
{
	// choose ( v1 , v2 , v3 , v4 )
	//   0    1 2  3 4  5  6 7  8 9
	//Again we can be sure about these indices because the stmt
	//already passed the syntax check. Also, if we are here, then
	//it is because the alg. found a ) in the expr stack. That
	//means all the expressions that can be written inside the
	//choose function are already evaluated, compounded and their
	//types are determined.
	confirm_type(var_seq.at(2), VariableType::Scalar);
	confirm_type(var_seq.at(4), VariableType::Scalar);
	confirm_type(var_seq.at(6), VariableType::Scalar);
	confirm_type(var_seq.at(8), VariableType::Scalar);
	std::ostringstream oss;
	oss << "choose(" << var_seq.at(2).name() << ", "
					 << var_seq.at(4).name() << ", "
					 << var_seq.at(6).name() << ", "
					 << var_seq.at(8).name() << ")";
	expr_stack.emplace_back(oss.str(), VariableType::Scalar,
			Dimensions(1, 1));
}

void CodeGenerator::convert_sqrt_function(std::vector<Variable>& expr_stack,
										  const std::vector<Variable>& var_seq) const
{
	const Variable argument = var_seq.at(2);
	confirm_type(argument, VariableType::Scalar);
	//no external function call this time. We leave
	//sqrt(x) as it is in the expression. Thus, its return value
	//is an rvalue and doesn't need to be added to the sym_table
	std::ostringstream oss;
	oss << "sqrt(" << argument.name() << ")";
	//push to the expr_stack
	expr_stack.emplace_back(oss.str(), VariableType::Scalar,
			Dimensions(1, 1));
}

void CodeGenerator::convert_tr_function(std::vector<Variable>& expr_stack,
										const std::vector<Variable>& var_seq,
										std::ofstream& ofs) const
{
	//since we are compounding subexpressions into variables,
	//we only need one indexing to get all the info about the
	//whole expression put into the tr function
	//var_seq format: tr ( argument )
	//				  0  1    2     3
	const Variable argument = var_seq.at(2);
	if (argument.type() == VariableType::Matrix) {
		const Dimensions dims = argument.dim();
		//get a new, unique name for our new helper variable
		const std::string helper_name = get_unique_name();
		//rows and cols are exchanged because of transpose
		this->put_tabs(ofs);
		//declare the helper vari
		ofs << "double " << helper_name << "[" << dims.cols
			<< "][" << dims.rows << "];" << std::endl;
		this->put_tabs(ofs);
		//call the tr function
		ofs << "tr(" << dims.rows << ", " << dims.cols << ", "
			<< argument.name() << ", " << helper_name << ");"
			<< std::endl;
		Variable new_var(helper_name, VariableType::Matrix,
						 Dimensions(dims.cols, dims.rows));
		//add the new variable to the symbol table
		sym_table->insert(new_var);
		//push to the expr_stack
		expr_stack.push_back(new_var);
	} else if (argument.type() == VariableType::Scalar) {
		//nothing to do. simply push it since tr(scalar_x) == scalar_x
		expr_stack.push_back(argument);
	}
}

void CodeGenerator::convert_scalar_ops(std::vector<Variable>& expr_stack,
									   const TokenCategory& op_type,
									   const Variable& left_op,
									   const Variable& right_op) const
{
	std::ostringstream oss;
	if (op_type == TokenCategory::AdditionOperator) {
		oss << "(" << left_op.name() << "+" << right_op.name() << ")";
		expr_stack.emplace_back(oss.str(), VariableType::Scalar,
								Dimensions(1, 1));
	} else if (op_type == TokenCategory::SubtractionOperator) {
		oss << "(" << left_op.name() << "-" << right_op.name() << ")";
		expr_stack.emplace_back(oss.str(), VariableType::Scalar,
								Dimensions(1, 1));
	} else if (op_type == TokenCategory::MultiplicationOperator) {
		oss << "(" << left_op.name() << "*" << right_op.name() << ")";
		expr_stack.emplace_back(oss.str(), VariableType::Scalar,
								Dimensions(1, 1));
	}
}

void CodeGenerator::convert_scalar_mat_mul(std::vector<Variable>& expr_stack,
										   std::ofstream& ofs,
										   const Variable& matrix,
										   const Variable& scalar) const
{
	const auto var_name = this->get_unique_name();
	const auto dims = matrix.dim();
	//need the dimensions of the matrix
	this->put_tabs(ofs);
	ofs << "double " << var_name << "[" << dims.rows << "]["
		<< dims.cols << "];" << std::endl;
	this->put_tabs(ofs);
	ofs << "mat_sca_mul(" << dims.rows << ", " << dims.cols
		<< ", " << scalar.name() << ", " << matrix.name()
		<< ", " << var_name << ");" << std::endl;
	//update the symbol table
	sym_table->insert(Variable(var_name, VariableType::Matrix,
				dims));
	//Now as the result, push var_name !
	expr_stack.emplace_back(var_name, VariableType::Matrix,
			dims);
}

void CodeGenerator::convert_mat_mat_mul(std::vector<Variable>& expr_stack,
										std::ofstream& ofs,
										const Variable& left_op,
										const Variable& right_op) const
{
	const auto left_dims = left_op.dim();
	const auto right_dims = right_op.dim();
	if (left_dims.cols != right_dims.rows) {
		throw_error(err_linenum(this->line_count),
					"Matrix multiplication dimension mismatch: ",
					left_dims, " and ", right_dims);
	}
	//need the dimensions of the matrix
	if (left_dims.rows == 1 && right_dims.cols == 1) {
		//then we can return a single double
		std::ostringstream oss;
		oss << "mat_mat_mul_s(" << left_dims.cols << ", "
			<< left_op.name() << ", " << right_op.name()
			<< ")";
		expr_stack.emplace_back(oss.str(), VariableType::Scalar,
								Dimensions(1, 1));
	} else {
		const auto var_name = this->get_unique_name();
		this->put_tabs(ofs);
		ofs << "double " << var_name << "[" << left_dims.rows << "]["
			<< right_dims.cols << "];" << std::endl;
		this->put_tabs(ofs);
		ofs << "mat_mat_mul(" << left_dims.rows << ", "
			<< left_dims.cols << ", " << right_dims.cols << ", "
			<< left_op.name() << ", " << right_op.name() <<
			", " << var_name << ");" << std::endl;
		//Update the symbol table
		Variable res(var_name ,VariableType::Matrix,
					 Dimensions(left_dims.rows, right_dims.cols));
		sym_table->insert(res);
		//Now as the result, push var_name !
		expr_stack.push_back(res);
	}
}

void CodeGenerator::convert_mat_mat_sub(std::vector<Variable>& expr_stack,
										std::ofstream& ofs,
										const Variable& left_op,
										const Variable& right_op) const
{
	const auto var_name = this->get_unique_name();
	const auto left_dims = left_op.dim();
	const auto right_dims = right_op.dim();
	if (left_dims != right_dims) {
		throw_error(err_linenum(this->line_count),
					"Matrix subtraction dimension mismatch: ",
					left_dims, " and ", right_dims);
	}
	//need the dimensions of the matrix
	this->put_tabs(ofs);
	ofs << "double " << var_name << "[" << left_dims.rows << "]["
		<< left_dims.cols << "];" << std::endl;
	this->put_tabs(ofs);
	ofs << "mat_mat_sub(" << left_dims.rows << ", "
		<< left_dims.cols << ", " << left_op.name() << ", "
		<< right_op.name() << ", " << var_name << ");"
		<< std::endl;
	//Update the symbol table
	sym_table->insert(Variable(var_name, VariableType::Matrix,
							   left_dims));
	//Now as the result, push var_name !
	expr_stack.emplace_back(var_name, VariableType::Matrix, left_dims);
}

void CodeGenerator::convert_mat_mat_add(std::vector<Variable>& expr_stack,
										std::ofstream& ofs,
										const Variable& left_op,
										const Variable& right_op) const
{
 //addition
	const auto var_name = this->get_unique_name();
	const auto left_dims = left_op.dim();
	const auto right_dims = right_op.dim();
	if (left_dims != right_dims) {
		throw_error(err_linenum(this->line_count),
					"Matrix addition dimension mismatch: ",
					left_dims, " and ", right_dims);
	}
	//need the dimensions of the matrix
	this->put_tabs(ofs);
	ofs << "double " << var_name << "[" << left_dims.rows << "]["
		<< left_dims.cols << "];" << std::endl;
	this->put_tabs(ofs);
	ofs << "mat_mat_add(" << left_dims.rows << ", "
		<< left_dims.cols << ", " << left_op.name() << ", "
		<< right_op.name() << ", " << var_name << ");"
		<< std::endl;
	//Update the symbol table
	sym_table->insert(Variable(var_name, VariableType::Matrix,
							   left_dims));
	//Now as the result, push var_name !
	expr_stack.emplace_back(var_name, VariableType::Matrix, left_dims);
}

/** puts tabs by the quantity specified by this->indentation_level to ofs
  */
void CodeGenerator::put_tabs(std::ofstream& ofs) const
{
	for (int i = 0; i < this->indentation_level; ++i) {
		ofs << '\t';
	}
}

/** Returns a unique identifier name each time called.
  *
  * POSSIBLE IMPROVEMENT: If called std::numeric_limit<int>::max() times or more
  * consecutively, this method will return wrong results.
  *
  */
std::string CodeGenerator::get_unique_name() const
{
	static int count = 0;
	return helper_name_prefix + std::to_string(count++);
}

/** Base concatenate method. To stop the recursion
  */
template<typename T>
void concatenate(std::ostringstream& oss, const T& t)
{
	oss << t;
}

/** Helper function for throw_error. Concatenates all given arguments to each
  * other with the help of passed ostringstream reference
  */
template<typename T, typename... Args>
void concatenate(std::ostringstream& oss, const T& t, Args... args)
{
	oss << t;
	concatenate(oss, args...);
}

/** Constructs an error message from the given arguments and throws it
  *	All passed arguments must have overloaded << operator.
  */
template<typename T, typename... Args>
[[ noreturn ]] void throw_error(const T& t, Args... args)
{
	std::ostringstream oss;
	concatenate(oss, t, args...);
	throw std::runtime_error(oss.str());
}

std::string err_linenum(int num)
{
	return "Error (Line " + std::to_string(num) + "): ";
}
