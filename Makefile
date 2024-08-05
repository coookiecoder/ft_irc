NAME=ircserv

FILES=main message_handle

SRC=$(foreach f, $(FILES), src/$(f).c)
OBJ=$(foreach f, $(FILES), obj/$(f).o)

CXX=c++

CXXFLAGS=-std=c++98 -Wall -Wextra -Werror -Ihdr -g3

all: $(NAME) client

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

obj/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	$(RM) -rf obj client

fclean: clean
	$(RM) -rf $(NAME)

re: fclean $(NAME)

client:
	./make_client.sh

.PHONY: all clean fclean re
