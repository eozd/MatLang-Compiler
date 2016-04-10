#include "lexer.hpp"
#include <sstream>
#include "regex.hpp"

Lexer::Lexer()
	: tok_rep()
{
}

/** Reads the given input string sequentially and tokenizes the input.
  * Regex is used for validating input words. Regex representation of each
  * Token category that makes sense to Lexer is defined in Token class.
  */
std::vector<Token> Lexer::tokenize(const std::string& line)
{
	std::vector<Token> token_vec; //resulting token vector
	/*compound_str is used to accumulate characters for recursive definitions.
	  E.g.: integer definition is recursive. In order to get the all of the
	  integer, not just one digit of it, we need to accumulate digits. This
	  string is used as a queue to accumulate digits.
	*/
	std::string compound_str;
	//Flag to indicate that iterator is one past the last elem of line
	const char End_char = '\0';
	//regex for finding any whitespace: space, tab, \r, etc. Check [:space:].
	Regex white_space_rgx(R"(^[[:space:]]$)");
	//regex for finding empty lines and skipping them
	Regex empty_line_rgx(R"(^[[:space:]]+$)");
	//if empty line, nothing to do. Return an empty token vector
	if (line.size() == 0 || empty_line_rgx.search(line)) {
		return token_vec;
	}
	/* ALGORITHM BEGIN */
	//initializes all of the regex objects
	for (size_t iter = 0; iter <= line.size(); ++iter) { //iterate over line
		//if iter has passed the last elem, assign the flag value.
		char sub = (iter != line.size()) ? line[iter] : End_char;
		//if the current character is any of the ones below, we MAY have a
		//recursively defined value. Must accumulate digits.
		if (tok_rep.representation(TokenCategory::Identifier).search(sub)
		 || tok_rep.representation(TokenCategory::Integer).search(sub)
		 //for reals
		 || tok_rep.representation(TokenCategory::Dot).search(sub))
		{
			compound_str.push_back(sub); //accumulate
		} else {
			//If current char is not any of the above, we must give meaning
			//to the accumulated string and the current char. If any of the two
			//cannot be given any meaning, throw an exception.
			bool compound_match = false; //is compound_str meaningful
			bool char_match = false; //is current_char meaningful
			/*  BEGIN GIVE MEANING TO compound_str */

			for (const auto& category : this->token_categories) {
				//get the corresponding Regex object mapped to category
				const auto& rgx = tok_rep.representation(category);
				if (rgx.search(compound_str)) { //if it can be given a meaning
					//We got a MATCH
					//tokenize and push to the vector
					token_vec.emplace_back(compound_str, category);
					compound_match = true;
					//Need to reset the compound_str. We will start accumulating
					//new chars on it.
					compound_str.clear();
					//Regex objects are stored with the priority order. If
					//the current Regex accepts the compound_str, then we are
					//DONE with compound_str. Break out.
					break;
				}
			}
			/*  END GIVE MEANING TO compound_str */
			/*  BEGIN GIVE MEANING TO sub */
			for (const auto& category : this->token_categories) {
				const auto& rgx = tok_rep.representation(category);
				bool res = rgx.search(sub);
				//if it can given a meaning, char_match will become True.
				//else it will not change
				char_match = char_match || res;
				if (res) {
					//tokenize
					token_vec.emplace_back(sub, category);
					break;
				}
			}
			/* END GIVE MEANING TO sub */
			//if sub is a whitespace, assume it is a match. No meaning needed.
			if (white_space_rgx.search(sub) || sub == '\0') {
				char_match = true;
			}
			//if compound_str is not filled yet, assume it is a match.
			if (compound_str == "") {
				compound_match = true;
			}
			//If the current character cannot be given any meaning, ERROR
			if (!char_match) {
				std::ostringstream oss;
				oss << "No meaning can be given to " << sub
					<< std::endl;
				throw std::runtime_error(oss.str());
			}
			//If the compound string cannot be given any meaning, ERROR
			if (!compound_match) {
				std::ostringstream oss;
				oss << "No meaning can be given to " << compound_str
					<< std::endl;
				throw std::runtime_error(oss.str());
			}
		}
	}
	/* ALGORITHM END */
	return token_vec;
}
