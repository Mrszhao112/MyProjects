all:client main simple_test
client : client.c
	gcc $^ -o $@
main : main.c coroutine.h coroutine.c
	gcc $^ -o $@	
simple_test : simple_test.c coroutine.h coroutine.c
	gcc $^ -o $@
