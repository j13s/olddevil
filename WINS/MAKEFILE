W_OBJ=w_init.o w_event.o wi_buts.o wi_keys.o wi_winma.o wi_menu.o w_draw.o \
 w_tools.o w_system.o w_list.o
compile: $(W_OBJ)
	copy *.d depend /Y >NUL
	del ..\d.out
	del ..\d.exe
include depend
makeall: w_test.exe
CFLAGS=-MMD -Wall -O3 -m486 -g -DGNU_C -DGO32 -ansi -pedantic -ffast-math -Wwrite-strings -c
%.exe: %.out
	rem strip $<
	coff2exe $<
w_test.out: $(W_OBJ) w_test.o /djgpp/grx20/lib/libgrx20.a
	gcc -o w_test.out w_test.o $(W_OBJ) -lgrx20 -lpc
w_system.o: w_system.c wins.h wins_int.h w_system.h w_list.h
	gcc -Wall -O3 -m486 -g -DGNU_C -c w_system.c -o w_system.o
