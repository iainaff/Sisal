SHELL = /bin/sh
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

COMMON = ifxstuff.o
COMMONSRC = ifxstuff.c
OBJ = if2mem.o mem.o if2level.o if2size.o if2alloc.o if2clean.o if2pic.o
SRC = if2mem.c mem.c if2level.c if2size.c if2alloc.c if2clean.c if2pic.c

INCL   = world.h ../Library/IFX.h ../../config.h

MINIMAL=0
.c.o:
	${CC} ${CFLAGS} -c $*.c
	-@if [ $(MINIMAL) = 1 ]; then /bin/rm -f $*.c; else cat /dev/null; fi

local: if2mem
optional:
all: local optional

if2mem:${PAR} ${OBJ} ${COMMON}  ../Library/libifx.a
	${CC} ${CFLAGS} -o if2mem ${OBJ} ${COMMON} ../Library/libifx.a -lm

clean:
	rm -f ${OBJ}
	rm -f ${COMMON}
	rm -f if2mem
	rm -f  lint.log

#------------------------------------------------------------
lint: lint.log

# 127 == main() returns random value to invocation environment
lint.log: $(SRC) $(COMMONSRC) $(INCL) ../Library/llib-lifx.ln
	@lint -u -v -woff 91,302,307,127 \
		`echo -DLINT $(CFLAGS) | tr ' ' '\012' | egrep '^-D'` \
		$(SRC) $(COMMONSRC) ../Library/llib-lifx.ln -lm \
		| tee lint.log \
		| egrep -v '( )(free|malloc|strlen|lseek|read|isspace|isdigit)(.*)llib-lc'

#------------------------------------------------------------
install:
	/bin/rm -f ${BIN_PATH}/if2mem
	${BINSTALL} if2mem ${BIN_PATH}/if2mem
	chmod $(XMASK) ${BIN_PATH}/if2mem

${OBJ}:   ${INCL}
${COMMON}:	${INCL}
