all: matrix vector

matrix:
	mkdir -p build
	g++ -fPIC -shared -o build/matrix.so lib/core.cpp lib/matrix.cpp \
		lib/common.cpp -I /usr/lib/erlang/erts-9.2/include -I lib

vector:
	mkdir -p build
	g++ -fPIC -shared -o build/vector.so lib/core.cpp lib/vector.cpp \
		lib/common.cpp -I /usr/lib/erlang/erts-9.2/include -I lib
