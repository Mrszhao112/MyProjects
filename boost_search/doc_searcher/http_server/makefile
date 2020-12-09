FLAG=-std=c++11 -I /home/dev/test/cppjieba/cppjieba-master/include -lpthread -L. -lsearcher 

http_server:http_server.cpp
	g++ $^ -o $@ $(FLAG)

.PHONY:clean
	clean:
	rm http_server
