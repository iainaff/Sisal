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
OBJ = part.o if2cost.o  if2part.o if2count.o if2modules.o report.o \
	vector.o slice.o stream.o

COMMONSRC = ifxstuff.c
SRC = part.c if2cost.c  if2part.c if2count.c if2modules.c report.c \
	vector.c slice.c stream.c

INCL = world.h ../Library/IFX.h ../../config.h

MINIMAL=0
.c.o:
	${CC} ${CFLAGS} -c $*.c
	-@if [ $(MINIMAL) = 1 ]; then /bin/rm -f $*.c; else cat /dev/null; fi

local: if2part
optional:
all: local optional

if2part:${PAR} ${OBJ} ${COMMON}  ../Library/libifx.a
	${CC} ${CFLAGS} -o if2part ${OBJ} ${COMMON} ../Library/libifx.a -lm

clean:
	rm -f ${OBJ} ${COMMON}
	rm -f if2part
	rm -f lint.log

#------------------------------------------------------------
lint: lint.log

# 127 == main() returns random value to invocation environment
#  86 == null effect
lint.log: $(SRC) $(COMMONSRC) $(INCL) ../Library/llib-lifx.ln
	@lint -u -v -woff 91,302,307,127,86 \
		`echo -DLINT $(CFLAGS) | tr ' ' '\012' | egrep '^-D'` \
		$(SRC) $(COMMONSRC) ../Library/llib-lifx.ln -lm \
		| tee lint.log \
		| egrep -v '( )(free|malloc|strlen|lseek|read|isspace|isdigit)(.*)llib-lc'

#------------------------------------------------------------
install:
	/bin/rm -f ${BIN_PATH}/if2part
	${BINSTALL} if2part ${BIN_PATH}/if2part
	chmod $(XMASK) ${BIN_PATH}/if2part
	/bin/rm -f ${LIB_PATH}/s.costs
	${SINSTALL} s.costs ${LIB_PATH}/s.costs
	chmod $(MMASK) ${LIB_PATH}/s.costs

${OBJ}:   ${INCL}
${COMMON}: ${INCL}

#

