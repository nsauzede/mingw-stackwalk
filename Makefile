
ifdef DECL
FL:=-DDECL
endif

all:
	gcc -g -o bt.exe bt.c $(FL) -ldbghelp && ./bt
