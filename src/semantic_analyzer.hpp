#pragma once
#include <utility>
#include <vector>
#include "token.hpp"
#include "symbol_table.hpp"
#include "definitions.hpp"

/** Semantically checks the whole source file given as a parameter to the
  * analyze function and reports any errors it finds.
  */
class SemanticAnalyzer {
public:
	//Takes a ptr to sym_table to make semantic checkes
	SemanticAnalyzer(SymbolTable* const sym_table_ptr)
		: sym_table(sym_table_ptr)
	{ };
	//analyzes the given source file and reports any errors.
	void analyze(const std::vector<stmt_with_info>&);
private:
	SymbolTable* const sym_table;
};
