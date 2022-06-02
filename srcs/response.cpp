#include "../incs/response.hpp"

template<class T>
std::string toString(const T &value) {
	std::ostringstream os;
	os << value;
	return os.str();
}

Response::Response(Request &request, Server *server):_req(request), _serv(server){
	setupError();
	setupConf();
	_ret = _req.getRet();
	isValidRequest(); // check if allowed methods is used, max_body_size,etc.. depending on server & location

	_methodFt["GET"] = &Response::methodGet;
	_methodFt["POST"] = &Response::methodPost;
	_methodFt["DELETE"] = &Response::methodDelete;
}

std::string		Response::process(){ // creation of response
	if (_ret == 200){
		(this->*_methodFt[_req.getMethod()])(); // get request method, and call corresponding function
	}
	else { // error on request
		setError(_ret);
	}
	writeResp();
	return _response.str();
}

void		Response::setError(int ret){
	_ret = ret;
	_headerTemplate["Content-Type"] = "text/html";
	_headerTemplate["Connection"] = "Close";
	std::map<std::string, std::string>::iterator errPage = _currentConf.error_page.find(toString(_ret));
	if (errPage != _currentConf.error_page.end() && isFile(errPage->second)){ // err Page is defined in conf file
		_body << getHtmlFile(errPage->second);
	}
	else{ //default error page
		_body << getHtmlFile("defaults/error_pages/" + toString(_ret) + ".html");
	}
	_headerTemplate["Content-Length"] = setContentLength();

}

void		Response::writeResp(){
	_header << "HTTP/1.1 " << _ret << " " << _errorTemplate[_ret] << "\r\n";
	std::map<std::string, std::string>::iterator it = _headerTemplate.begin();
	for ( ; it != _headerTemplate.end(); it++){
		_header << it->first << ": " << it->second << "\r\n";
	}
	_response << _header.str()<< "\r\n" << _body.str() << "\r\n\r\n";
}

void		Response::setupHeader(){ // setup header map, with corresponding function
	_headerTemplate["Server"] = "webserv/1.0.0 (Unix)";
	_headerTemplate["Connection"] = setConnection();
	_headerTemplate["Date"] = setDate();
	_headerTemplate["Content-Length"] = setContentLength();
	_headerTemplate["Location"] = setRedirect();
}

void		Response::setupError(){ // setup error code with corresponding message
	_errorTemplate[200] = "Ok";
	_errorTemplate[204] = "No Content";
	_errorTemplate[400] = "Bad Request";
	_errorTemplate[403] = "Forbidden";
	_errorTemplate[404] = "Not Found";
	_errorTemplate[405] = "Method Not Allowed";
	_errorTemplate[413] = "Request Entity Too Large";
	_errorTemplate[500] = "Internal Server Error";
	_errorTemplate[301] = "Moved Permanently";
}

Response::~Response(){}

std::string		Response::setConnection(){
	if (_req.getRet() >= 400)
		return "Close";
	else
		return "Keep-Alive";
}

void		Response::setupConf(){
	std::string host = _req.getHeaders().find("Host")->second;
	std::vector<s_server_config> _conf = _serv->getConf();
	std::vector<s_server_config>::iterator it = _conf.begin();

	for ( ; it != _conf.end(); it++){
		if (it->server_name == host.substr(0, host.find(':'))){
			_currentConf = *it;
			break;
		}
	}
	if (it == _conf.end())
		_currentConf = *_conf.begin();

	setLocation();
}

std::string		Response::setRedirect(){
	if (!_currentLoc.redirect.empty()) {
		_ret = 301;
		_currentPath = _currentLoc.redirect;
	}
	return _currentPath;
}

std::string		Response::setDate(){
	char			buffer[100];
	struct timeval	tv;
	struct tm		*tm;

	gettimeofday(&tv, NULL);
	tm = gmtime(&tv.tv_sec);
	strftime(buffer, 100, "%a, %d %b %Y %H:%M:%S GMT", tm);
	return std::string(buffer);
}

std::string		Response::setContentLength(){
	return toString(_body.str().size());
}

std::string		Response::setContentType(std::string path)
{
	std::string type = path.substr(path.rfind(".") + 1, path.size() - path.rfind("."));
	if (type == "html")
		return "text/html";
	else if (type == "css")
		return "text/css";
	else if (type == "js")
		return "text/javascript";
	else if (type == "jpeg" || type == "jpg")
		return "image/jpeg";
	else if (type == "png")
		return "image/png";
	else if (type == "bmp")
		return "image/bmp";
	else
		return "text/plain";
}

std::string		Response::getHtmlFile(const std::string& path)
{
	std::ofstream		file;
	std::stringstream	buffer;

	file.open(path.c_str(), std::ifstream::in);
	buffer << file.rdbuf();
	file.close();
	return (buffer.str());
}


float		locationSim(std::string confLoc, std::string path){
	int score = 0;

	if (confLoc.size() > path.size()) // if conf location is longer than path
		return -1;
	if (confLoc == path) // exact same path
		return 0;
	if (confLoc[0] == '.'){
		if (path.compare(path.size() - confLoc.size(), confLoc.size(), confLoc) == 0)//does path end with '.something'
			return 1000;
		return -1;
	}
	for (std::string::size_type i = 0; i < path.size(); i++) {
		if (path[i] == confLoc[i]){
			if (path[i] == '/')
				score++;
		}
		else {
			if (path[i] == '/')
				score++;
			else if (confLoc[i] != '\0')
				score = -1;
			return score;
		}
	}
	return 0;
}

void			Response::setLocation(){ // find best match for location
	std::pair<int, s_location >	ret;
	int							sim;

	ret.first = -1;
	std::vector<s_location>::iterator ite = _currentConf.location.begin();
	for ( ; ite != _currentConf.location.end(); ite++){ // search for location in server conf
		sim = locationSim(ite->path, _req.getPath()); // return a "score", -1 bad location, 0 exact location, +0 best match
		if (sim > ret.first || sim == 0) { // store the best match, if 0 break
			ret.first = sim;
			ret.second = *ite;
			if (sim == 0)
				break;
		}
	}

	_currentLoc = ret.second; // store loc
	for (ite = _currentLoc.location.begin(); ite != _currentLoc.location.end(); ite++){ // search location in location conf
		sim = locationSim(ite->path, _req.getPath());
		if (sim > ret.first || sim == 0) {
			ret.first = sim;
			ret.second = *ite;
			if (sim == 0)
				break;
		}
	}
	_currentRoot = _currentLoc.root.size() ? _currentLoc.root : _currentConf.root;
	_currentPath = _currentRoot + _req.getPath();
	//reinterpret the path if an index.html is specified and there is no autoindex
	if (_currentLoc.path == _req.getPath()) {
		if ((_currentConf.autoindex != "on" && _currentLoc.autoindex != "on" ) || _currentLoc.autoindex == "off"){
			if (_currentLoc.index.size() > 0 || _currentConf.index.size() > 0)
				_currentPath = _currentRoot + "/" +
							(_currentLoc.index.size() > 0 ? _currentLoc.index.front() : _currentConf.index.front());
		}
	}
	//std::cout << GREEN << "\nCoresponding location: " <<ret.second.path  << WHITE<< std::endl;
}

void		Response::isValidRequest(){
	//is method allowed in location
	if (_currentLoc.methods.size() > 0 && std::find(_currentLoc.methods.begin(), _currentLoc.methods.end(), _req.getMethod()) == _currentLoc.methods.end()){
		_ret = 405;
		return ;
	}
	//is body too large
	if ((unsigned)atoi(_currentConf.client_max_body_size.c_str()) > 0 && _req.getBody().size() > (unsigned)atoi(_currentConf.client_max_body_size.c_str())){
		_ret = 413;
		return ;
	}
}
