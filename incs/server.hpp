#pragma once

#include "webserv.hpp"

struct s_location {
	std::string							path;
	std::string							root;
	std::list<std::string>				index;
	std::string							autoindex;
	std::vector<std::string>			methods;
	std::string							fastcgi_pass;
	std::string 						redirect;
	// std::map<std::string, std::string>	fastcgi_param;
	std::vector<s_location>				location;
};

struct s_server_config {
	std::string							listen;
	int									port;
	std::string							server_name;
	std::map<std::string, std::string>	error_page;
	std::string							client_max_body_size;
	std::string							root;
	std::string							uploads;
	std::list<std::string>				index;
	std::string							autoindex;
	std::vector<s_location>				location;
};

class Server{

	public:

	Server(int port, std::vector<s_server_config>);
	~Server();
	int		setup();
	int		accept();
	bool	listenClient(int client_fd, std::map<int, std::pair<std::string, int> > &_response);
	std::vector<s_server_config> getConf();
	int 			getPort();
	int				getSocket();
	sockaddr_in 	getSockaddr();

	std::string 	createResponse(int code, std::string param);
	std::string		getHtmlFile(const std::string& path);

	int								_sockfd;
	private:

	int								_port;
	sockaddr_in						sockaddr;
	std::vector<s_server_config>	_conf; //conf of server/host
	std::map<int, std::string>		_requests; // stores requests.
};
