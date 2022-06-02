
#include "../incs/server.hpp"
#include "../incs/request.hpp"
#include "../incs/response.hpp"
#include "../incs/cluster.hpp"

Server::Server(int port, std::vector<s_server_config> conf) : _port(port), _conf(conf){
}

int Server::setup()
{
	_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	// config socket
	if (_sockfd == -1)
	{
		std::cerr << "Failed to create socket." << std::endl;
		return -1;
	}
	int on = 1;
	/* Enable address reuse */
	setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sockaddr.sin_port = htons(_port);
	if (::bind(_sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0)
	{
		std::cerr << "Bind failed " << _port << std::endl;
		close(_sockfd);
		return -1;
	}
	// Start listening. Hold at most 1024 connections in the queue
	if (::listen(_sockfd, 1024) < 0)
	{
		std::cerr << "Listen failed" << std::endl;
		close(_sockfd);
		return -1;
	};
	std::cout << GREEN << "Server listenning on port " << _port << WHITE<< std::endl;
	return 0;
}

int Server::accept()
{
	int connection;
	size_t addrlen = sizeof(sockaddr);

	connection = ::accept(_sockfd, (struct sockaddr *)&sockaddr, (socklen_t *)&addrlen); // accept incoming connection, and create specific socket for client (connection)
	if (connection < 0)
	{
		std::cerr << "Failed to grab connection." << std::endl;
		close(connection);
	}
	return connection;
}

int		checkEnd(const std::string& str, const std::string& end)
{
	size_t	i = str.size();
	size_t	j = end.size();

	while (j > 0)
	{
		i--;
		j--;
		if ((int)i < 0 || str[i] != end[j])
			return (1);
	}
	return (0);
}

bool Server::listenClient(int client_fd, std::map<int, std::pair<std::string, int> > &_response)
{
	int ret;
	size_t len = 0;
	char buffer[8192] = {0};
	size_t	contentLen = 0;
	ret = ::recv(client_fd, buffer, 8192 - 1, 0); // listen to client
	if (ret <= 0)
		return ret;
	_requests[client_fd] +=buffer; // add content to client's request
	if ((len = _requests[client_fd].find("\r\n\r\n")) != std::string::npos)// if end of request
	{
		if (_requests[client_fd].find("Content-Length: ") != std::string::npos)
			contentLen = std::atoi(_requests[client_fd].substr(_requests[client_fd].find("Content-Length: ") + 16, 10).c_str());
		if (_requests[client_fd].find("Transfer-Encoding: chunked") != std::string::npos && checkEnd(_requests[client_fd], "0\r\n\r\n") == 1)
			return 1;
		if (_requests[client_fd].size() >= len + contentLen){
			// process it
			Request req(_requests[client_fd]);
			//send response
			Response resp(req, this);
			std::string res = resp.process();
			_response.insert(std::make_pair(client_fd, std::make_pair(res, req.getRet() >= 400 ? 0 : 1)));
			_requests[client_fd].clear(); //clear request
		}
		return 1;
	}
	return 1;
}

Server::~Server()
{
	if (_sockfd)
	{
		close(_sockfd);
		std::cout << "\nServer shutdown." << std::endl;
	}
}

int Server::getPort()
{
	return _port;
}
int Server::getSocket()
{
	return _sockfd;
}
sockaddr_in Server::getSockaddr()
{
	return sockaddr;
}

std::vector<s_server_config> Server::getConf(){
	return _conf;
}

