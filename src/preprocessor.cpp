#include <string>
#include <stdexcept>
#include <fstream>
#include "preprocessor.hpp"

/** Removes the comments from the file with the specified file name
  * If the specified file doesn't exist, throws a runtime_error
  */
const std::string Preprocessor::remove_comments(const std::string& source_name)
{
	const std::string Output_name = source_name + this->File_extension;
	std::ifstream source_file(source_name.c_str());
	if (!source_file) {
		throw std::runtime_error(source_name + " couldn't be opened!");
	}
	std::ofstream output_file(Output_name.c_str());
	if (!output_file) {
		throw std::runtime_error(Output_name + " couldn't be opened!");
	}
	std::string line;
	while (std::getline(source_file, line)) { //read each line
		// get the substring until the first # in a line.
		// if there is no #, line is not changed
		line = line.substr(0, line.find('#'));
		output_file << line << std::endl;
	}
	return Output_name;
}
