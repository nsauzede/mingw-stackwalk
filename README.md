
Programmatic backtrace experiments with mingw
Based on various stackovf snippets

See LICENSE.txt file.
Nicolas Sauzede 2019.

It's based on addr2line from msys2 so won't be very useful without msys2 shell
(but if you want to debug duh)

for the mingw backtrace to print same infos as eg: on linux,
all functions should be declared with an extra declspec : __declspec(dllexport)
as I understand it, gcc is instruct to output an additional PDB-compatible symbol name,
in case resulting binary is to be linked with eg: MS linker etc..

here is the result :
$ make DECL=1
gcc -g -o bt.exe bt.c -DDECL -ldbghelp && ./bt
1|backtrace+0x2c 0x40168d: C:\msys64\home\sauzede\git\mingw-stackwalk/bt.c:27
1|printStack+0x3c 0x4016d2: C:\msys64\home\sauzede\git\mingw-stackwalk/bt.c:38
1|bar+0x10 0x40188d: C:\msys64\home\sauzede\git\mingw-stackwalk/bt.c:66
1|foo+0x13 0x4018ac: C:\msys64\home\sauzede\git\mingw-stackwalk/bt.c:71
1|main+0x37 0x4018e9: C:\msys64\home\sauzede\git\mingw-stackwalk/bt.c:76
0|0x4013b4: E:/mingwbuild/mingw-w64-crt-git/src/mingw-w64/mingw-w64-crt/crt/crtexe.c:341
0|0x40150b: E:/mingwbuild/mingw-w64-crt-git/src/mingw-w64/mingw-w64-crt/crt/crtexe.c:225
1|BaseThreadInitThunk+0x14 0x7ff8dd317bd4: ??:0
1|RtlUserThreadStart+0x21 0x7ff8ddceced1: ??:0

without this declspec, with mingw we would have :
$ make DECL=
gcc -g -o bt.exe bt.c  -ldbghelp && ./bt
0|0x40168d: C:\msys64\home\sauzede\git\mingw-stackwalk/bt.c:27
0|0x4016d2: C:\msys64\home\sauzede\git\mingw-stackwalk/bt.c:38
0|0x40188d: C:\msys64\home\sauzede\git\mingw-stackwalk/bt.c:66
0|0x4018ac: C:\msys64\home\sauzede\git\mingw-stackwalk/bt.c:71
0|0x4018e9: C:\msys64\home\sauzede\git\mingw-stackwalk/bt.c:76
0|0x4013b4: E:/mingwbuild/mingw-w64-crt-git/src/mingw-w64/mingw-w64-crt/crt/crtexe.c:341
0|0x40150b: E:/mingwbuild/mingw-w64-crt-git/src/mingw-w64/mingw-w64-crt/crt/crtexe.c:225
1|BaseThreadInitThunk+0x14 0x7ff8dd317bd4: ??:0
1|RtlUserThreadStart+0x21 0x7ff8ddceced1: ??:0

we loose func name +displacement
