SHELL=/bin/sh
XMASK = 555
MMASK = 444

.SUFFIXES: .c .o

CC           = cc
CFLAGS       = -g
BIN_PATH     = ../../../bin
LIB_PATH     = ../../../lib
INC_PATH     = ../../../include
MAN_PATH     = ../../../man
BINSTALL     = cp
SINSTALL     = cp
PAR          =

OBJLD  = p2clib.o file1.o file2.o file3.o
INCL   = p2c.h frontend.h

MINIMAL=0

.c.o:
	${CC} ${CFLAGS} -c $*.c
	-@if [ $(MINIMAL) = 1 ]; then /bin/rm -f $*.c; else cat /dev/null; fi

local: sisal
optional:
all: local optional

sisal:${PAR} ${OBJLD}
	${CC} ${CFLAGS} -o sisal ${OBJLD}

install:
	/bin/rm -f ${BIN_PATH}/sisal
	${BINSTALL} sisal ${BIN_PATH}/sisal
	chmod $(XMASK) ${BIN_PATH}/sisal
	/bin/rm -f ${LIB_PATH}/etabl.bin
	${SINSTALL} etabl.bin ${LIB_PATH}/etabl.bin
	chmod $(MMASK) ${LIB_PATH}/etabl.bin
	/bin/rm -f ${LIB_PATH}/ptabl.bin
	${SINSTALL} ptabl.bin ${LIB_PATH}/ptabl.bin
	chmod $(MMASK) ${LIB_PATH}/ptabl.bin
	/bin/rm -f ${MAN_PATH}/man5/sisal.5
	${SINSTALL} sisal.5 ${MAN_PATH}/man5/sisal.5
	chmod $(MMASK) ${MAN_PATH}/man5/sisal.5

clean:
	rm -f ${OBJLD}
	rm -f sisal

${OBJLD}:   ${INCL}
