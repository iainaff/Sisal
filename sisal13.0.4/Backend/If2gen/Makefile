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

COMMON= ifxstuff.o
OBJ = gen.o if2yank.o if2temp.o  if2names.o    \
      if2preamble.o if2smash.o if2print.o if2record.o if2array.o         \
      if2select.o if2loop.o if2aimp.o if2opt.o if2vector.o if2prebuild.o \
      if2fibre.o if2free.o if2interface.o if2sdbx.o if2ureduce.o

COMMONSRC= ifxstuff.c
SRC = gen.c if2yank.c if2temp.c  if2names.c    \
      if2preamble.c if2smash.c if2print.c if2record.c if2array.c         \
      if2select.c if2loop.c if2aimp.c if2opt.c if2vector.c if2prebuild.c \
      if2fibre.c if2free.c if2interface.c if2sdbx.c if2ureduce.c

INCL = world.h ../Library/IFX.h ../../config.h

MINIMAL=0
.c.o:
	${CC} ${CFLAGS} -c $*.c
	-@if [ $(MINIMAL) = 1 ]; then /bin/rm -f $*.c; else cat /dev/null; fi

local: if2gen
optional:
all: local optional

if2gen:${PAR} ${OBJ} ${COMMON} ../Library/libifx.a
	${CC} ${CFLAGS} -o if2gen ${OBJ} ${COMMON} ../Library/libifx.a -lm

clean:
	rm -f ${OBJ} ${COMMON}
	rm -f if2gen
	rm -f lint.log

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
	/bin/rm -f  ${BIN_PATH}/if2gen
	${BINSTALL} if2gen ${BIN_PATH}/if2gen
	chmod $(XMASK) ${BIN_PATH}/if2gen
	/bin/rm -f  ${INC_PATH}/sisal.h
	${SINSTALL} sisal.h ${INC_PATH}/sisal.h
	chmod $(MMASK) ${INC_PATH}/sisal.h

${OBJ}:   ${INCL}
${COMMON}:   ${INCL}

#
