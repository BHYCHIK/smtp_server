code: fsm-gen
	gcc -g -Werror -Wall -Iinclude src/worker.c src/listener.c src/session.c src/smtp.c fsm.o -lpcre

fsm-gen:
	autogen src/server.def
	cp server-fsm.c src
	rm -f include/server-fsm.h
	cp server-fsm.h include
	gcc -g -Iinclude -c server-fsm.c -o fsm.o
