makeall: d.exe 
include depend
# include wins/makefile
# plotsys.o is not linked. Instead psysfast.o is used (see file "link" and
# plotsys.c).
OBJ=devil.o userio.o tools.o tag.o insert.o calctxt.o initio.o config.o \
 askcfg.o plot.o plottxt.o plotsys.o click.o savetool.o readlvl.o \
 readtxt.o do_event.o do_stat.o do_ins.o do_mod.o do_light.o do_move.o \
 do_tag.o do_side.o grfx.o do_opts.o opt_txt.o options.o macros.o title.o\
 lac_cfg.o

ifdef GER
CFLAGS=-Wall -Werror -O3 -m486 -g -DGNU_C -DGO32 -DGER -ansi -pedantic -ffast-math -Wwrite-strings -I wins -I language -c
else
CFLAGS=-Wall -Werror -O3 -m486 -g -DGNU_C -DGO32 -ansi -pedantic -ffast-math -Wwrite-strings -I wins -I language -c
#CFLAGS=-Wall -Werror -O3 -m486 -g -DLIST_DEBUG -DGNU_C -DGO32 -ansi -pedantic -ffast-math -Wwrite-strings -I wins -I language -c
#CFLAGS=-Wall -Werror -O3 -m486 -DGNU_C -DGO32 -ansi -pedantic -ffast-math -Wwrite-strings -I wins -I language -c
endif

# %.exe: %.out
#	rem strip $<
#	coff2exe $<
d.exe: $(OBJ) 
	gcc @linkx
	strip -o dx dx.out
	coff2exe dx
	del dx
	copy *.d depend /Y >NUL

compile: $(OBJ)
%.o: %.c
	gcc -MMD $(CFLAGS) -o $@ $<
plotsys.o: plotsys.c plotsys.h plotdata.h structs.h
	gcc -I wins -I language -Wall -m486 -g3 -DGNU_C -c plotsys.c -o plotsys.o
plottxt.o: plottxt.c plottxt.h plotsys.h plotdata.h structs.h
	gcc -I wins -I language -Wall -Werror -O3 -m486 -g -DGNU_C -c plottxt.c -o plottxt.o
title.o: title.c title.h structs.h
	gcc -I wins -I language -Wall -O3 -m486 -g -DGNU_C -c title.c -o title.o
lac_cfg.o: lac_cfg.c lac_cfg.h
	gcc -I wins -I language -Wall -O3 -m486 -g -DGNU_C -c lac_cfg.c -o lac_cfg.o

