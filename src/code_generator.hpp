#pragma once
#include "symbol_table.hpp"
#include "definitions.hpp"
#include <fstream>

/** Constructed with a pointer to the symbol table that is updated in the
  * previous steps of the compilation.
  *
  * CodeGenerator generates C code from a given source file and writes it to
  * file specified with a name.
  *
  * In the process of generating the C code, code generator does semantic checks
  * on expressions such as assignment type matching, operation type and
  * dimension matching, etc.
  */
class CodeGenerator {
public:
	CodeGenerator(SymbolTable* const sym_table_ptr)
		: sym_table(sym_table_ptr)
		, double_for(false)
		, indentation_level(0)
		  //give a kind of unique prefix to the var.s in order to prevent clashes
		, helper_name_prefix("_E4_")
		, line_count(0)
	{ };
	~CodeGenerator() {};
	CodeGenerator(const CodeGenerator&) = default;
	CodeGenerator& operator=(const CodeGenerator&) = default;

	//Generates C code from the given source file and writes it to the file
	//specified with the given output_file_name
	void generate_c_code(const std::vector<stmt_with_info>& src_file,
						 const std::string& out_file_name);
private:
	//returns a unique name each time called. Used for helper variable naming.
	std::string get_unique_name() const;
	//puts indentation amount of tabs
	void put_tabs						  (std::ofstream&) const;
	typedef std::vector<Token>::const_iterator citer;
	//confirms the var's type. If not the same, throws an exception
	void confirm_type(const Variable& var, const VariableType& var_type) const;
	//converts the postfix expression starting at first. Converts the expression
	//until it encounters a TokenCategory::ExpressionEnd. When this function
	//returns, the passed iterator will be pointing to ExpressionEnd
	//During the expression conversion, helper variables are created. Thus,
	//ofs must be passed as a paremeter as well.
	Variable convert_to_c_expr(citer& first, std::ofstream& ofs) const;
private:
	/* HELPER FUNCTIONS */
	//program structure. Basically calls the other functions.
	void write_program_structure        (std::ofstream&) const;
	//preprocessor commands. As of now, only include statements.
	void write_preprocessor_commands	(std::ofstream&) const;
	//These are program structure functions. They are written before the main
	//function and called when necessary.
	void write_tr_function		          (std::ofstream&) const;
	void write_choose_function	          (std::ofstream&) const;
	void write_print_function	          (std::ofstream&) const;
	void write_printsep_function          (std::ofstream&) const;
	void write_matrix_matrix_multiply     (std::ofstream&) const;
	void write_negative_matrix			  (std::ofstream&) const;
	void write_matrix_matrix_subtract     (std::ofstream&) const;
	void write_matrix_matrix_add          (std::ofstream&) const;
	void write_scalar_matrix_multiply     (std::ofstream&) const;
	void write_matrix_assign			  (std::ofstream&) const;
	//Statements that appear inside the   main function.
	void write_end_for		    		  (std::ofstream&) const;
	void write_printsep_stmt			  (std::ofstream&) const;
	void write_scalar_declr				  (std::ofstream&, const std::vector<Token>&) const;
	void write_vector_declr				  (std::ofstream&, const std::vector<Token>&) const;
	void write_matrix_declr				  (std::ofstream&, const std::vector<Token>&) const;
	void write_single_for				  (std::ofstream&, const std::vector<Token>&) const;
	void write_double_for				  (std::ofstream&, const std::vector<Token>&) const;
	void write_print_stmt				  (std::ofstream&, const std::vector<Token>&) const;
	void write_expr_assignment			  (std::ofstream&, const std::vector<Token>&) const;
	void write_single_subscript_assignment(std::ofstream&, const std::vector<Token>&) const;
	void write_double_subscript_assignment(std::ofstream&, const std::vector<Token>&) const;
	void write_list_assignment			  (std::ofstream&, const std::vector<Token>&) const;
	//The following functions are helpers for convert_to_c_expr
	//Each function are called when certain preconditions are met. For example,
	//when two matrices are multiplied, conversion function for mat mat
	//multiplication is called.
	void convert_scalar_ops				  (std::vector<Variable>&, const TokenCategory&,
							    		   const Variable&, const Variable&) const;
	void convert_scalar_mat_mul 		  (std::vector<Variable>&, std::ofstream&,
							    		   const Variable&, const Variable&) const;
	void convert_mat_neg				  (std::vector<Variable>&, std::ofstream&,
										   const Variable&) const;
	void convert_mat_mat_add    		  (std::vector<Variable>&, std::ofstream&,
							    		   const Variable&, const Variable&) const;
	void convert_mat_mat_sub    		  (std::vector<Variable>&, std::ofstream&,
							    		   const Variable&, const Variable&) const;
	void convert_mat_mat_mul    		  (std::vector<Variable>&, std::ofstream&,
							    		   const Variable&, const Variable&) const;
	void convert_function_call  		  (std::vector<Variable>&, std::ofstream&) const;
	void convert_tr_function    		  (std::vector<Variable>&,
							    		   const std::vector<Variable>&,
							    		   std::ofstream&) const;
	void convert_sqrt_function  		  (std::vector<Variable>&,
							    		   const std::vector<Variable>&) const;
	void convert_choose_function		  (std::vector<Variable>&,
										   const std::vector<Variable>&) const;
	void convert_subscript      		  (citer, std::vector<Variable>&) const;
private:
	SymbolTable* const sym_table;
	bool double_for; //if there is a double loop. Used for indentation
	int indentation_level; //indentation_level in tabs
	//used in giving in unique names to helper variables in the resulting program
	const std::string helper_name_prefix;
	//the current line we are at. Updated at each iteration of code generation
	int line_count;
};
