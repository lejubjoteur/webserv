#pragma once

#include "cosmetics.hpp"

#include <unistd.h>
#include <cstdlib>
#include <algorithm>
#include <dirent.h>

#include <csignal>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ostream>

#include <netinet/in.h>
#include <errno.h>

#include <string.h>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <stack>

#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>

# define R_BUFFER_SIZE 10000
# define CGI_BIN "./cgi/ubuntu_cgi_tester"