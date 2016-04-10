#pragma once

#include <string>

/** Preprocesses the given file before the tokenizing begins
  */
class Preprocessor {
public:
	//Preprocessed files are added an extension
	const std::string File_extension{".pp"};
	//removes comments from the file without deleting any lines
	//in order to preserve the line number information
	const std::string remove_comments(const std::string& source_name);
};
