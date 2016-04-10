#include "semantic_analyzer.hpp"
#include <sstream>

/** Semantically analyzes the source file and reports any errors.
  *
  * As of now, analyze function only checks if a variable is declared before
  * it is being used
  */
void SemanticAnalyzer::analyze(const std::vector<stmt_with_info>& statements)
{
	//Simply checks if a variable is declared before used by calling
	//sym_table->lookup(var)
	for (const auto& stmt_tuple : statements) {
		//std::get<0>(stmt_with_info) is the list of tokens
		//std::get<1>(stmt_with_info) is the statement type
		//std::get<2>(stmt_with_info) is the line count
		for (const auto& t : std::get<0>(stmt_tuple)) {
			if (t.category() == TokenCategory::Identifier) {
				//if lookup fails, lookup throws a runtime error
				try {
					sym_table->lookup(t.value());
				} catch (std::runtime_error e) {
					std::ostringstream oss;
					oss << "Error (Line " << std::get<2>(stmt_tuple)
						<< "): " << e.what();
					throw std::runtime_error(oss.str());
				}
			}
		}
	}
}
