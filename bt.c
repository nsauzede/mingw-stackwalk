// Copyright(C) 2019 Nicolas Sauzede. All rights reserved.
// Use of this source code is governed by an MIT license
// that can be found in the LICENSE.txt file.

#include <inttypes.h>
#include <stdio.h>

#include <windows.h>
#include <process.h>
#include <dbghelp.h>

#ifdef DECL
#define WDECL __declspec(dllexport)
#else
#define WDECL
#endif

WDECL void a2l(void *p) {
	char prog[256];
	GetModuleFileName(NULL, prog, sizeof(prog));
	char addr[100];
	snprintf(addr, sizeof(addr), "0x%" PRIX64, (uint64_t)p);
	char *cmd = "\\msys64\\mingw64\\bin\\addr2line.exe";
	intptr_t res = _spawnl(_P_WAIT, cmd, cmd, "-e", prog, addr, NULL);
}

#if 0
#define backtrace(buffer, size) CaptureStackBackTrace(0, size, buffer, NULL)
#else
WDECL int backtrace(void **buffer, int size) {
	return CaptureStackBackTrace(0, size, buffer, NULL);
}
#endif

WDECL void printStack( void )
{
	unsigned int i;
	void *stack[100];
	unsigned short frames;
	HANDLE hpr = GetCurrentProcess();
	int slen = sizeof(stack) / sizeof(stack[0]);
	frames = backtrace(stack, slen);
	SymInitialize(hpr, NULL, TRUE);
#define MAX_NAME_LEN 100
	int ssi = sizeof(SYMBOL_INFO) + MAX_NAME_LEN * sizeof(TCHAR);
	char *_si = malloc(ssi);
	SYMBOL_INFO *psi = (SYMBOL_INFO *)_si;
	for( i = 0; i < frames; i++ )
	{
		DWORD64 disp;
		memset(_si, 0, ssi);
		psi->SizeOfStruct = sizeof(SYMBOL_INFO);
		psi->MaxNameLen = MAX_NAME_LEN;
		BOOL ret = SymFromAddr(hpr, (DWORD64)stack[i], &disp, psi);
		printf( "%d|", ret);
		if (psi->Name[0])
		printf( "%s", psi->Name);
		if (disp)
		printf( "+0x%" PRIx64, disp);
		if (psi->Name[0])
		printf( " ");
		printf( "0x%" PRIx64 ": ", stack[i]);
		a2l(stack[i]);
	}
	free(_si);
}

WDECL int bar(int a) {
	printStack();
	return a * 42;
}

WDECL int foo(int a) {
	return bar(a);
}

WDECL int main(int argc, char *argv[]) {
	setbuf(stdout, 0);
	foo(argc);
	return 0;
}
