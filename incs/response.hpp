#pragma once

#include "webserv.hpp"
#include "server.hpp"
#include "request.hpp"
#include "cgi.hpp"

class Response{

	public:

	Response(Request &request, Server *server);
	~Response(void);

	std::string		process();

	Request			_req; //store request
	Server*			_serv; // server who received request
	s_server_config _currentConf; // server's conf
	s_location		_currentLoc; // location's config
	std::string		_currentPath; // Parsed path
	std::string		_currentRoot;
	std::stringstream		_response; //response which will be send
	private:

	int				_ret; // error code

	std::stringstream		_header; //heade rof resp
	std::stringstream		_body; //body of resp
	std::map <std::string, std::string>	_headerTemplate; // stored header value
	std::map<int, std::string>		_errorTemplate; // errorCode & phrase

	typedef void (Response::*f)();
	std::map <std::string, f> _methodFt; //map of function's method


	void	setupHeader(void); //setup headerTemplate variable
	void	setupError(void); // setup error code and corresponding phrase
	void	setupConf(void); // get config of the virtual server

	void	setError(int ret); // set header & body for specific error code

	void	writeResp(void); // write error & body

	void	methodGet(void);
	void	methodDelete(void);
	void	methodPost(void);
	void	methodHead(void);

	std::string		setConnection(void);
	std::string		setDate(void);
	std::string		setContentLength(void);
	std::string		setContentType(std::string path);
	std::string		setRedirect(void);

	std::string		getHtmlFile(const std::string& path); // read file and return string
	void			writeInFile(std::string body);

	int		readFile(std::string path);
	int		isFile(std::string path);
	void	setLocation(void);
	int		autoIndex(std::string path);
	void	isValidRequest(void);

};
