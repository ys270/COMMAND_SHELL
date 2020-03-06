ffosh:main.cpp myshell.cpp myshell.h environment.cpp environment.h variable.cpp variable.h
	g++ -std=gnu++11 -ggdb3 -Wall -Werror -pedantic -o ffosh main.cpp myshell.cpp environment.cpp variable.cpp
