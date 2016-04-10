#include <iostream>
#include <cstdio>
#include <string>
#include <fstream>
#include <stdexcept>
#include "regex.hpp"
#include "preprocessor.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "token.hpp"
#include "symbol_table.hpp"
#include "code_generator.hpp"
#include "semantic_analyzer.hpp"
#include "definitions.hpp"

/** Prints how to call the program from the shell
  * 
  */
void print_usage(const std::string& program_name)
{
	std::cout << "MATLANG2C: Matlang to C Compiler" << std::endl;
	std::cout << "Usage:" << std::endl;
	std::cout << program_name << " SOURCE_FILE" << std::endl;
	std::cout << program_name << " SOURCE_FILE -o OUTPUT_FILE" << std::endl;
}

/** Strips the last extension from the file name.
  * mat.c becomes mat. mat.pp.c becomes mat.pp
  */
std::string strip_extensions(const std::string& str)
{
	//Find the occurrence of the last '.'. Returns its index. The index of
	//the last '.' is the length of the string up to that '.'.
	return str.substr(0, str.rfind('.'));
}

int main(int argc, char** argv)
{
	//argc == 2 --> only the source file is given
	//argc == 4 --> source file and target C file is given
	if (argc != 2 && argc != 4) { //only 2 and 4 is accepted
		print_usage(argv[0]);
		return -1;
		//if the output file is given, flag must be -o
	} else if (argc == 4 && std::string(argv[2]) != "-o") {
		std::cout << "Error: Output file must be given after -o flag"
			<< std::endl;
		return -2;
	}
	//argc == 2 or argc == 4
	const std::string Source_name = argv[1];
	const std::string Default_output_name = strip_extensions(Source_name) +".c";
	// if we have only one cmd argument, produce a result with a default name.
	const std::string Output_name = (argc == 2) ? Default_output_name : argv[3];
	//preprocessor strips comments from the file without deleting any line
	//this way, line numbers are preserved
	Preprocessor pp;
	const std::string Preprocessed_name = pp.remove_comments(Source_name);
	//we work on the preprocessed file
	std::ifstream source_file(Preprocessed_name.c_str());
	if (!source_file) {
		std::cout << Preprocessed_name << " couldn't be opened" << std::endl;
		return -3;
	}
	SymbolTable sym_table;
	Lexer lexer; //lexer tokenizes the source file
	Parser parser(&sym_table); //parser does the syntax check
	SemanticAnalyzer sem_analyze(&sym_table); //semantic checks
	//generates the code. Does semantic checks on expressions
	CodeGenerator code_gen(&sym_table);
	std::string line; //holds the current line
	int line_count = 0;
	//stores the whole file as lines. Each line is a vector of tokens and a
	//statement type and the line number. See definitions.hpp for more info
	std::vector<stmt_with_info> source_as_tokens;
	Regex empty_line_rgx(R"(^[[:space:]]*$)");
	while (std::getline(source_file, line)) {
		++line_count;
		//whenever we see an empty line, we go to the next line
		if (!empty_line_rgx.search(line)) {
			try {
				//tokenize the current line
				auto token_vec = lexer.tokenize(line);
				if (token_vec.empty()) {
					continue;
				}
				//parser does syntax check on the token vector and returns
				//what kind of statement this particular token vector is
				auto stmt_category = parser.parse(token_vec);
				source_as_tokens.emplace_back(token_vec, stmt_category, line_count);
			} catch (std::runtime_error e) {
				//embed line info to the error and quit the program
				//no file is generated up to this point. Thus, no extra cleanup
				std::cout << "Error (Line " << line_count << "): " <<
					e.what() << std::endl;
				return -4;
			}
		}
	}
	//After the whole file passes the syntax check, pass the whole file to
	//semantic analyzer and code generator respectively.
	try {
		//if there is any semantic errors, catch the error, print and quit
		sem_analyze.analyze(source_as_tokens);
	} catch (std::runtime_error e) {
		std::cout << e.what() << std::endl;
		return -5;
	}
	try {
		//code generator starts generating some code as soon as it starts
		//reading lines. However, expression validity are checked at
		//code generation level. Thus, there is a possibility of errors. In
		//that case, we need to cleanup the created file
		code_gen.generate_c_code(source_as_tokens, Output_name);
	} catch(std::runtime_error e) {
		std::cout << e.what() << std::endl;
		std::remove(Output_name.c_str());
		std::remove(Preprocessed_name.c_str());
		return -7;
	}
	//At this point, all the job is done. Delete the preprocessed file
	std::remove(Preprocessed_name.c_str());
	return 0;
}
