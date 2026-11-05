

debug:
	clang-cl.exe -Ddebug_assertions /Od /c /Fobuild/main-debug.o main.c
	clang-cl.exe /Od /c /Fobuild/motor-debug.o motor.c
	clang-cl.exe /Od /c /Fobuild/legivel-debug.o legivel.c
	clang-cl.exe /Fe:./bin/debug.exe \
		build/legivel-debug.o			\
		build/motor-debug.o				\
		build/main-debug.o 

release:
	clang-cl.exe /O2 /Ot /c /Fobuild/main-release.o main.c
	clang-cl.exe /O2 /Ot /c /Fobuild/legivel-release.o legivel.c
	clang-cl.exe /O2 /Ot /c /Fobuild/motor-release.o motor.c
	clang-cl.exe /Fe:./bin/release.exe \
		build/legivel-release.o			  \
		build/motor-release.o			  \
		build/main-release.o 

test:
	clang-cl.exe /D__unit_tests__ /Od /c /Fobuild/motor-test.o motor.c
	clang-cl.exe /Fe:./bin/ut-motor.exe build/motor-test.o
