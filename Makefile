NAME		=	ircserv

SRC			=	src/main.cpp \
				src/Channel.cpp \
				src/channelUtils.cpp \
				src/connectionMessages.cpp \
				src/Client.cpp \
				src/handleUserInput.cpp \
				src/initializeMsg.cpp \
				src/invite.cpp \
				src/join.cpp \
				src/joinChecks.cpp \
				src/kick.cpp \
				src/makeMessages.cpp \
				src/makeSelectAndRunCommand.cpp \
				src/mode.cpp \
				src/part.cpp \
				src/privmsg.cpp \
				src/Server.cpp \
				src/topic.cpp \

OBJ			=	$(SRC:.cpp=.o)

CC			=	c++

CFLAGS		=	-Wextra -Wall -Werror 

RM			=	rm -f




all: 		$(NAME)

$(NAME): $(OBJ)
			$(CC) $(CFLAGS) -g $(OBJ) -o $(NAME) 

%.o:%.cpp
			$(CC) $(CFLAGS) -c $< -o $@

clean:
			${RM} $(OBJ)

fclean: 	clean
			${RM} $(NAME)

re:			fclean all

.PHONY:		all clean fclean re