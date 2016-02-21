fsm-gen:
	autogen src/server.def
	mv server-fsm.c src
	mv server-fsm.h include
