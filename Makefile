code: fsm-gen
	gcc -g -Werror -Wall -Iinclude src/worker.c src/listener.c src/session.c src/smtp.c


fsm-gen:
	autogen src/server.def
	mv server-fsm.c src
	rm -f include/server-fsm.h
	mv server-fsm.h include

