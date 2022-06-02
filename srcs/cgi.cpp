#include "../incs/cgi.hpp"
#include "../incs/response.hpp"

std::string	to_string(size_t n)
{
	std::stringstream tmp;
	tmp << n;
	return tmp.str();
}

cgi::cgi() {
}

void	cgi::setupEnv(Response *resp) {
	(void)resp;
	std::string truncPath = resp->_currentPath.substr(resp->_currentRoot.size());
	_env.clear();
	_env.push_back("SERVER_NAME=webserv");
	_env.push_back("SERVER_SOFTWARE=nginx/1.21.5");
	_env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	_env.push_back("SERVER_PORT=" + to_string(resp->_serv->getPort()));

	_env.push_back("GATEWAY_INTERFACE=CGI/1.1");

	_env.push_back("REQUEST_METHOD=" + resp->_req.getMethod());
	_env.push_back("PATH_INFO=" + truncPath);
	_env.push_back("SCRIPT_FILENAME=" + resp->_currentRoot + truncPath);
	_env.push_back("REQUEST_URI=" + truncPath);
	_env.push_back("SCRIPT_NAME=" + resp->_currentLoc.fastcgi_pass);
	_env.push_back("REDIRECT_STATUS=200");
	_env.push_back("PATH_TRANSLATED=" + truncPath);

	std::string tmp_dir = getenv("PWD") + (std::string)"/www/uploads";
	_env.push_back("TMP_DIR=" + tmp_dir);
	_env.push_back("QUERY_STRING=" + truncPath);
	if (resp->_req.getBody().size() > 0) {
		_env.push_back("CONTENT_LENGTH=" +  to_string(resp->_req.getBody().size()));
		_env.push_back("BODY=" + resp->_req.getBody());
		_env.push_back("CONTENT_TYPE=" + resp->_req._headers["Content-Type"]);
	}
}

void	cgi::convertToC(Response *resp) {
	int i = 0;
	setupEnv(resp);
	for (std::vector<std::string>::iterator it = _env.begin(); it != _env.end(); it++) {
		_envTab[i] = (char*)_env[i].c_str();
		_envTab[i][(_env[i].size())] = '\0';
		i++;
	}
	_envTab[i] = NULL;
}

std::string	cgi::exec_child(std::string exec, std::string body) {
	pid_t	pid;
	int		saveIn;
	int		saveOut;
	std::string resp;

	saveIn = dup(STDIN_FILENO);
	saveOut = dup(STDOUT_FILENO);
	FILE	*fileIn = tmpfile();
	FILE	*fileOut = tmpfile();
	long	fdIn = fileno(fileIn);
	long	fdOut = fileno(fileOut);
	int		ret = 1;
	(void)body;
	int ret_w = write(fdIn, body.c_str(), body.size());
	if (ret_w == -1) {
		std::cerr << "error: write" << std::endl;
		return NULL;
	}
	lseek(fdIn, 0, SEEK_SET);
	if ((pid = fork()) == -1) {
		std::cerr << "error fork" << std::endl;
		return NULL;
	}
	if (!pid) {
		char * const * nll = NULL;
		if (dup2(fdOut, STDOUT_FILENO) == -1) {
			std::cerr << "error : dup2 read failure" << std::endl;
			return NULL;
		}
		if (dup2(fdIn, STDIN_FILENO) == -1) {
			std::cerr << "error : dup2 write failure" << std::endl;
			return NULL;
		}
		if ((ret = execve(exec.c_str(), nll, _envTab)) == -1) {
			std::cerr << "error : execve failure" << std::endl;
			kill(pid, SIGTERM);
		}
	}
	else {
		char	buffer[100] = {0};

		if (waitpid(-1, NULL, 0) == -1)
			return NULL;
		lseek(fdOut, 0, SEEK_SET);
		ret = 1;
		while (ret > 0) {
			memset(buffer, 0, 100);
			if ((ret = read(fdOut, buffer, 100 - 1)) == -1) {
				std::cerr << "error : read failure" << std::endl;
				return NULL;
			}
			resp += buffer;
		}
	}
	dup2(saveIn, STDIN_FILENO);
	dup2(saveOut, STDOUT_FILENO);
	fclose(fileIn);
	fclose(fileOut);
	close(fdIn);
	close(fdOut);
	close(saveIn);
	close(saveOut);
	return resp;
}
