SHELL=/bin/sh
XMASK = 555
MMASK = 444

.SUFFIXES: .c .f .o

CC           = cc
FC           = f77
BIN_PATH     = ../../../bin
LIB_PATH     = ../../../lib
INC_PATH     = ../../../include
MAN_PATH     = ../../../man
CONFIG_DIR   = ../..
INSTALL      = cp
RANLIB       = touch
AR           = ar r
PAR          =
LIBM         = mlib.o
CFLAGS       = -g
CPPFLAGS     = -I. -I$(CONFIG_DIR)
FFLAGS       =

MINIMAL=0

SRCC  =	p-worker.c p-timer.c p-spawn.c              \
	p-wait.c p-ready.c p-ppp.c p-dsa.c          \
	p-error.c p-signal.c p-globals.c            \
	p-fibre.c p-util.c p-interface.c            \
	p-init.c p-pointer.c p-sdbx.c		    \
	p-tracer.c 				    \
	psignal.c vectorIO.c
OBJC  =	p-worker.o p-timer.o p-spawn.o              \
	p-wait.o p-ready.o p-ppp.o p-dsa.o          \
	p-error.o p-signal.o p-globals.o            \
	p-fibre.o p-util.o p-interface.o            \
	p-init.o p-pointer.o p-sdbx.o		    \
	p-tracer.o				    \
	psignal.o vectorIO.o

SRC0  =	p-srt0.c
OBJ0  =	p-srt0.o

CINC  = $(CONFIG_DIR)/config.h
SINC  =	arrays.h conf.h fibre.h interface.h locks.h process.h
OINC  =	options.h usage.h world.h
INCC  =	$(CINC) $(SINC) $(OINC)
INCF  =	sisalfli.inc

#CFLAGS=-DDEBUG_DSA -DVERBOSE

.c.o:
	${CC} $(CPPFLAGS) $(CFLAGS) -c $*.c
	-@if [ $(MINIMAL) = 1 ]; then /bin/rm -f $*.c; else cat /dev/null; fi

local: libsisal.a srt0
optional:
all: local optional

libsisal.a: $(PAR) $(OBJC) $(LIBM)
	/bin/rm -f libsisal.a
	$(AR) libsisal.a $(OBJC) $(LIBM)

srt0: $(OBJ0)

${OBJC}: ${INCC}
${OBJ0}: ${INCC}

mlib.o: mlib.f sisalfli.inc
	${FC} $(CPPFLAGS) ${FFLAGS} -c mlib.f

clean:
	rm -f ${OBJC}
	rm -f ${OBJ0}
	rm -f libsisal.a
	rm -f lint.log

install:
	/bin/rm -f ${LIB_PATH}/p-srt0.o
	${INSTALL} p-srt0.o ${LIB_PATH}/p-srt0.o
	chmod $(MMASK) ${LIB_PATH}/p-srt0.o
	/bin/rm -f ${LIB_PATH}/libsisal.a
	${INSTALL} libsisal.a ${LIB_PATH}/libsisal.a
	${RANLIB}   ${LIB_PATH}/libsisal.a
	chmod $(MMASK) ${LIB_PATH}/libsisal.a

	/bin/rm -f ${INC_PATH}/sisalrt.h
	${INSTALL} world.h ${INC_PATH}/sisalrt.h
	chmod $(MMASK) ${INC_PATH}/sisalrt.h
	/bin/rm -f ${INC_PATH}/config.h
	${INSTALL} $(CONFIG_DIR)/config.h ${INC_PATH}/config.h
	chmod $(MMASK) ${INC_PATH}/config.h

	/bin/rm -f ${INC_PATH}/arrays.h
	${INSTALL} arrays.h ${INC_PATH}/arrays.h
	chmod $(MMASK) ${INC_PATH}/arrays.h
	/bin/rm -f ${INC_PATH}/conf.h
	${INSTALL} conf.h ${INC_PATH}/conf.h
	chmod $(MMASK) ${INC_PATH}/conf.h
	/bin/rm -f ${INC_PATH}/fibre.h
	${INSTALL} fibre.h ${INC_PATH}/fibre.h
	chmod $(MMASK) ${INC_PATH}/fibre.h
	/bin/rm -f ${INC_PATH}/interface.h
	${INSTALL} interface.h ${INC_PATH}/interface.h
	chmod $(MMASK) ${INC_PATH}/interface.h
	/bin/rm -f ${INC_PATH}/locks.h
	${INSTALL} locks.h ${INC_PATH}/locks.h
	chmod $(MMASK) ${INC_PATH}/locks.h
	/bin/rm -f ${INC_PATH}/process.h
	${INSTALL} process.h ${INC_PATH}/process.h
	chmod $(MMASK) ${INC_PATH}/process.h

	/bin/rm -f ${INC_PATH}/sisalfli.inc
	${INSTALL} sisalfli.inc ${INC_PATH}/sisalfli.inc
	chmod $(MMASK) ${INC_PATH}/sisalfli.inc

	/bin/rm -f ${MAN_PATH}/man1/s.out.1
	${INSTALL} s.out.1 ${MAN_PATH}/man1/s.out.1
	chmod $(MMASK) ${MAN_PATH}/man1/s.out.1

newoptions:
	$(BIN_PATH)/parseopts < options -usage "man.update" usage.h options.h
	awk '{print} /.\\" Start options/{exit}' < s.out.1 > s.out.1.new
	cat man.update >> s.out.1.new
	awk '/.\\" End options/{flag=1} {if (flag) print}' < s.out.1 >> s.out.1.new
	mv s.out.1 s.out.1.bu
	mv s.out.1.new s.out.1

lint: $(SRCC) $(SRC0)
	lint -u -v $(CFLAGS) $(SRCC) $(SRC0) > lint.log
#
