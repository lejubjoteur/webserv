#ifndef PARSER_HPP
# define PARSER_HPP

#include "webserv.hpp"

class Parser {
	
	public:

	std::vector<std::string>		getLineFile(std::string filename);
	std::string						StripLine(std::string in);
	std::vector<std::string>		ParseSection(std::string &src);
	void							check_brackets(std::vector<std::string> &conf);
	std::vector<std::string>		ParseFile(std::string filename);
};

namespace constants {
	const char SPACE = ' ';
	const char TAB = '\t';
	const char HYPHEN = '-';
	const char UNDERSCORE = '_';
	const char EQUALS = '=';
	const char SEMICOLON = ';';
	const char SECTION_START = '{';
	const char SECTION_END = '}';
	const char QUOTE = '\"';
	const char DECIMAL = '.';
	const char COMMA = ',';
	const char COMMENT_DELIM = '#';
}



#endif
