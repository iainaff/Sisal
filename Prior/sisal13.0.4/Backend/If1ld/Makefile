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
PAR          =

COMMON   = ifxstuff.o
COMMONSRC = ifxstuff.c
OBJ  = if1smash.o if1names.o ld.o
SRC  = if1smash.c if1names.c ld.c

INCL   = world.h ../Library/IFX.h ../../config.h

MINIMAL=0
.c.o:
	${CC} ${CFLAGS} -c $*.c
	-@if [ $(MINIMAL) = 1 ]; then /bin/rm -f $*.c; else cat /dev/null; fi

local: if1ld

all: if1ld

if1ld:${PAR} ${OBJ} ${COMMON} ../Library/libifx.a 
	${CC} ${CFLAGS} -o if1ld ${OBJ} ${COMMON} ../Library/libifx.a -lm

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
	/bin/rm -f ${BIN_PATH}/if1ld
	${BINSTALL} if1ld  ${BIN_PATH}/if1ld
	chmod $(XMASK)  ${BIN_PATH}/if1ld 

clean:
	rm -f ${OBJ} ${COMMON}
	rm -f if1ld
	rm -f lint.log

${OBJ}:   ${INCL}
${COMMON}:    ${INCL}

#
