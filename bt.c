#include <inttypes.h>
#include <stdio.h>

#include <process.h>
#include <windows.h>

#include <dbghelp.h>

void a2l(void *p) {
#if 1
	char addr[100];
	snprintf(addr, sizeof(addr), "0x%" PRIX64, (uint64_t)p);
//	_spawnl(_P_WAIT, "addr2line", "addr2line", "-e", "bt.exe", addr);
	char *cmd = "\\msys64\\mingw64\\bin\\addr2line.exe";
	intptr_t res = _spawnl(_P_WAIT, cmd, cmd, "-e", "bt.exe", addr, NULL);
//	printf("res=%p errno=%d\n", res, errno);
#else
	char cmd[100];
	snprintf(cmd, sizeof(cmd), "addr2line -e bt.exe 0x%" PRIX64, (uint64_t)p);
	_popen(cmd);
#endif
}

int main();

BOOL __stdcall myReadProcMem(HANDLE  hProcess,
                                          DWORD64 qwBaseAddress,
                                          PVOID   lpBuffer,
                                          DWORD   nSize,
                                          LPDWORD lpNumberOfBytesRead)
{
    SIZE_T st;
    BOOL   bRet = ReadProcessMemory(hProcess, (LPVOID)qwBaseAddress, lpBuffer, nSize, &st);
    *lpNumberOfBytesRead = (DWORD)st;
    printf("ReadMemory: hProcess: %p, baseAddr: %p, buffer: %p, size: %d, read: %d, result: %d\n", hProcess, (LPVOID) qwBaseAddress, lpBuffer, nSize, (DWORD) st, (DWORD) bRet);
    return bRet;
}

void init(HANDLE ht) {
	DWORD htr = GetCurrentThreadId();
	printf("hello init thread %lx\n", htr);
	BOOL res = SymInitialize(0, "", FALSE);
	printf("res=%d\n", res);
	CONTEXT c;
	memset(&c, 0, sizeof(c));
	c.ContextFlags = CONTEXT_FULL;
#if 0
	if (GetThreadContext(ht, &c)) {
		void *rip = (void *)c.Rip;
		printf("eip= %p\n", rip);
		a2l(rip);
		printf("main= %p\n", main);
		a2l(main);
	}
#else
	RtlCaptureContext(&c);
#endif
	STACKFRAME64 s;
	s.AddrPC.Offset = c.Rip;
	s.AddrPC.Mode = AddrModeFlat;
	s.AddrFrame.Offset = c.Rsp;
	s.AddrFrame.Mode = AddrModeFlat;
	s.AddrStack.Offset = c.Rsp;
	s.AddrStack.Mode = AddrModeFlat;
	printf("offs= 0x%" PRIx64 "\n", s.AddrPC.Offset);
	printf("err=%lx\n", GetLastError());
	void *frames[3];
	int nframes = sizeof(frames) / sizeof(frames[0]);
	memset(frames, 0, sizeof(frames));
	for (int i = 0; i < nframes; i++) {
		res = StackWalk64(IMAGE_FILE_MACHINE_AMD64, GetCurrentProcess(), ht, &s, &c, 
//			0,
			myReadProcMem,
			SymFunctionTableAccess64,  SymGetModuleBase64, NULL);
		printf("res=%d err=%lx\n", res, GetLastError());
		printf("offs= 0x%" PRIx64 "\n", s.AddrPC.Offset);
		frames[i] = (void *)s.AddrPC.Offset;
	}
	for (int i = 0; i < nframes; i++) {
		a2l(frames[i]);
	}
}

void init0(HANDLE ht) {
	init(ht);
}

void init1(HANDLE ht) {
	init0(ht);
}

void init2(HANDLE ht) {
	init1(ht);
}

DWORD WINAPI MyThread(LPVOID lp) {
	HANDLE htr = GetCurrentThread();
	printf("hello MyThread thread %p\n", htr);
	init(lp);
	return 0;
}

int main() {
	setbuf(stdout, 0);
	DWORD htr = GetCurrentThreadId();
	printf("hello main thread %lx\n", htr);
//	HANDLE ht = CreateThread(NULL, 0, MyThread, htr, 0, NULL);
//	printf("createthread returned %p\n", ht);
	init2(GetCurrentThread());
	return 0;
}
