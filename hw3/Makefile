all: matrix

matrix:
	mkdir -p build
	g++ -O2 -Wall -Wextra -Wpedantic -fPIC -shared -o build/matrix.so lib/core.cpp lib/matrix.cpp \
		-I /usr/lib/erlang/erts-9.3/include -I lib
