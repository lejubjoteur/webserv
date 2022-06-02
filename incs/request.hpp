#pragma once

#include "webserv.hpp"

class Request
{

		public:

			Request(const std::string& str);
			~Request();

			void		setHeader();
			void		parseHeading(const std::string& str);

			std::string		getMethod();
			std::string     getHttpVersion();
			int				getRet();
			std::string		getBody();
			std::string		getPath();
			std::map<std::string, std::string>	getHeaders();
			std::map<std::string, std::string>	_headers;

		private:

			std::string							_method;
			std::string    						_httpVersion;
			int									_ret;
			std::string							_info;
			std::string							_body;
			std::string							_path;

			void			parse(std::string& content);
			std::string		getLine(const std::string& content, size_t &line);
			void			setErrorCode(int code, std::string info);
			void			isValidRequest(void);
		friend std::ostream& operator<< (std::ostream& stream, const Request& req) {
			if (req._ret < 300){
				stream << GREEN << std::endl;
				stream << "Method: " << req._method << std::endl;
				stream << "Path: " << req._path << std::endl;
				stream << "Http version: " << req._httpVersion << std::endl;
				std::map<std::string, std::string>::const_iterator it = req._headers.begin();
				std::map<std::string, std::string>::const_iterator ite = req._headers.end();
				for (; it != ite; it++){
					if (it->second.length() > 0)
						stream << it->first << ": " << it->second << std::endl;
				}
				std::cout << req._body;
				stream << std::endl << "Valid Request\n" << WHITE;
			}
			else if (req._ret >= 400) {

				stream << RED << std::endl << "[" << req._ret << "] " << req._info << "Invalid Request.\n" << WHITE;
			}
			return stream;
		}
};