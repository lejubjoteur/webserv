NAME = webserv

HEADER = ./incs/

COMPILER = c++

FLAGS = -Wall -Wextra -Werror -std=c++98

SRC_PATH = srcs/

OBJ_PATH = obj/

SRC = main.cpp cluster.cpp conf_parser.cpp server.cpp request.cpp response.cpp responseMethod.cpp cgi.cpp

OBJ = $(addprefix $(OBJ_PATH), $(SRC:.cpp=.o))

all: $(OBJ_PATH) $(NAME)

$(NAME): $(OBJ)
					$(COMPILER) $(FLAGS) $(OBJ) -I $(HEADER) -o $(NAME) 
					@echo $(NAME) created

client: test/client.o
					$(COMPILER) $(FLAGS) test/client.o -o client 
					@echo client created

${OBJ_PATH}:
					@mkdir -p $@

${OBJ_PATH}%.o:		$(SRC_PATH)%.cpp
					$(COMPILER) $(FLAGS) -c $< -o $@

clean:
					/bin/rm -rf  $(OBJ_PATH)
					/bin/rm -rf  test/client.o
					@echo Objects and libraries cleaned

fclean:				clean
					@/bin/rm -f $(NAME)
					@/bin/rm -f client
					@echo $(NAME) deleted

re:					fclean all

.PHONY:				all clean fclean re