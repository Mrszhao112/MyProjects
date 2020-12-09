FLAG=-std=c++11 -I /home/dev/test/cppjieba/cppjieba-master/include -ljsoncpp
.PHONY:all
all:index_test searcher_test libsearcher.so


index_test:searcher.cpp index_test.cpp
	g++ -g -std=c++11 $^ -o $@ -I /home/dev/test/cppjieba/cppjieba-master/include	-ljsoncpp
searcher_test:searcher_test.cpp searcher.cpp
	g++ -g -std=c++11 $^ -o $@ -I /home/dev/test/cppjieba/cppjieba-master/include -ljsoncpp	

libsearcher.so:searcher.cpp searcher.h
	g++ searcher.cpp -o $@ -shared -fPIC $(FLAG)
.PHONY:clean
clean:
	rm index_test searcher_test libsearcher.so
