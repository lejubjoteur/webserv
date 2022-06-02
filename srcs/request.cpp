#include "../incs/request.hpp"

Request::Request(const std::string& content):_method(""), _httpVersion(""), _ret(200), _body(""), _path("")
{
	setHeader();
	parse((std::string &)content);
	isValidRequest();
}

Request::~Request(){
}

std::string Request::getLine(const std::string& content, size_t &line){
	std::string		res;
	size_t			j;

	if (line == std::string::npos)
		return "";
	j = content.find_first_of('\n', line);
	res = content.substr(line, j - line);
	if (res[res.size() - 1] == '\r')
		res.resize(res.size() - 1);
	line = (j == std::string::npos ? j : j + 1);
	return res;
}

void		Request::parseHeading(const std::string& str){
	char const *x[] = { "GET", "POST", "DELETE"};
	std::vector<std::string> avMethods(x, x + sizeof(x) / sizeof(*x));
	size_t	i;
	size_t	j;

	i = 0; //find first space
	j = str.find_first_of(' ', i);//get end of path
	if (j == std::string::npos) //no space
		return setErrorCode(400, "No space after method.\n");
	_method.assign(str, 0, j);
	if (std::find(avMethods.begin(), avMethods.end(), _method) == avMethods.end()) //get method
		return setErrorCode(405, "Method not allowed.\n");
	i = str.find_first_not_of(' ', j); //get begin of path
	j = str.find_first_of(' ', i);//get end of path
	if (i == std::string::npos || j == std::string::npos || str[i] != '/')//check if valid pos
		return setErrorCode(400, "Invalid path format.\n");
	_path.assign(str, i, j - i);

	i = str.find_first_not_of(' ', j); //get begin of httpversion
	_httpVersion.assign(str, i, i + 7);//get httpversion
	if (_httpVersion != "HTTP/1.1" && _httpVersion != "HTTP/1.0") // bad version
		return setErrorCode(400, "Invalid http version.\n");

}

std::string	chunk(std::string &content){
	std::string	head = content.substr(0, content.find("\r\n\r\n"));
	std::string	chunks = content.substr(content.find("\r\n\r\n") + 4, content.find("0\r\n\r\n") - (content.find("\r\n\r\n") + 4));
	std::string	body = "";
	size_t		i = 0;
	while (i < chunks.size())
	{
		i = chunks.find("\r\n", i);
		body += chunks.substr(i + 2, chunks.find("\r\n", i + 2));
		i = chunks.find("\r\n", i + 2);
	}

	content = head + "\r\n\r\n" + body;
	return content;
}

void	Request::parse(std::string& content){
	size_t		lineIdx(0);
	std::string line;
	if (content.find("Transfer-Encoding: chunked") != std::string::npos && content.find("Transfer-Encoding: chunked") < content.find("\r\n\r\n"))
		content = chunk(content);
	parseHeading(getLine(content, lineIdx)); //parse first line (method, path, httpversion)
	while ((line = getLine(content, lineIdx)) != "\r" && _ret != 400 && line != ""){ //iterate on request
		size_t del = line.find(":"); //find delimiter ":"
		std::string key = line.substr(0, del); //get key
		std::string value = line.substr(line.find_first_not_of(' ', del + 1), line.length()); //get value
		if (del == std::string::npos || line[del -1] == ' ' || line[line.length() - 1] == ' ' /*|| !isupper(key[key.find('-') + 1])*/){
			setErrorCode(400, "Invalid header format.\n");
			break ;
		}
		remove(key.begin(), key.end(), ' '); //remove space
		remove(value.begin(), value.end(), ' ');
		if (_headers.count(key)) //find key and set value if exist, if not error 400 (bad header)
			_headers[key] = value;
	}
	if (lineIdx != std::string::npos) //get body part
		_body.assign(content.substr(lineIdx, std::string::npos));
}

void	Request::setHeader()
{
	//All known key's headers
	_headers.clear();

	_headers["Accept-Charsets"] = "";
	_headers["Accept-Language"] = "";
	_headers["Allow"] = "";
	_headers["Auth-Scheme"] = "";
	_headers["Authorization"] = "";
	_headers["Content-Language"] = "";
	_headers["Content-Length"] = "";
	_headers["Content-Location"] = "";
	_headers["Content-Type"] = "";
	_headers["Date"] = "";
	_headers["Host"] = "";
	_headers["Last-Modified"] = "";
	_headers["Location"] = "";
	_headers["Referer"] = "";
	_headers["Retry-After"] = "";
	_headers["Server"] = "";
	_headers["Transfer-Encoding"] = "";
	_headers["User-Agent"] = "";
	_headers["Www-Authenticate"] = "";
	_headers["Connection"] = "Keep-Alive";
}

void	Request::isValidRequest(void){
	if (_ret <= 300){

		if (!_headers["Host"].size())
			setErrorCode(400, "No Host specified.\n");
	}
}

void	Request::setErrorCode(int code, std::string info){
	_ret = code;
	_info = info;
}

std::string		Request::getMethod(){
	return (_method);
}
std::string		Request::getHttpVersion(){
	return (_httpVersion);
}
int				Request::getRet(){
	return (_ret);
}
std::string		Request::getBody(){
	return (_body);
}
std::string		Request::getPath(){
	return (_path);
}
std::map<std::string, std::string>			Request::getHeaders(){
	return (_headers);
}