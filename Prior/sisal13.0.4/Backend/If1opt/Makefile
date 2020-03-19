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

COMMON = ifxstuff.o util.o
OBJ    = opt.o if1normal.o if1inline.o if1cse.o if1gcse.o                \
	 if1fission.o if1count.o if1fusion.o if1invar.o if1reduce.o      \
	 if1split.o if1dope.o if1invert.o if1parallel.o if1unroll.o      \
	 if1fold.o if1move.o if1dead.o if1explode.o AssignIDs.o \
	 callreorder.o cascade.o
OBJK   = check.o if1check.o
OBJPP  = pprint.o if1pprint.o
OBJO   = count.o if1count.o

COMMONSRC = ifxstuff.c util.c
SRC    = opt.c if1normal.c if1inline.c if1cse.c if1gcse.c                \
	 if1fission.c if1count.c if1fusion.c if1invar.c if1reduce.c      \
	 if1split.c if1dope.c if1invert.c if1parallel.c if1unroll.c      \
	 if1fold.c if1move.c if1dead.c if1explode.c AssignIDs.c \
	 callreorder.c cascade.c
SRCK   = check.c if1check.c
SRCPP  = pprint.c if1pprint.c
SRCO   = count.c if1count.c

INCL   = world.h ../../config.h ../Library/IFX.h

MINIMAL=0
.c.o:
	${CC} ${CFLAGS} -c $*.c
	-@if [ $(MINIMAL) = 1 ]; then /bin/rm -f $*.c; else cat /dev/null; fi

local: if1opt
optional: if1check if1pprint if1count
all: local optional

if1opt:${PAR} ${OBJ} ${COMMON} ../Library/libifx.a
	${CC} ${CFLAGS} -o if1opt ${OBJ} ${COMMON} ../Library/libifx.a -lm

if1count:${PAR} ${OBJO} ${COMMON}
	${CC} ${CFLAGS} -o if1count ${OBJO} ${COMMON} ../Library/libifx.a -lm

if1pprint:${PAR} ${OBJPP} ${COMMON}
	${CC} ${CFLAGS} -o if1pprint ${OBJPP} ${COMMON} ../Library/libifx.a -lm

if1check:${PAR} ${OBJK} ${COMMON}
	${CC} ${CFLAGS} -o if1check ${OBJK} ${COMMON} ../Library/libifx.a -lm

#------------------------------------------------------------
lint: lint.log

# 127 == main() returns random value to invocation environment
lint.log: $(SRC) $(COMMONSRC) $(INCL) ../Library/llib-lifx.ln
	@lint -u -v -woff 91,302,307,127 \
		`echo -DLINT $(CFLAGS) | tr ' ' '\012' | egrep '^-D'` \
		$(SRC) $(COMMONSRC) ../Library/llib-lifx.ln -lm \
		| tee lint.log \
		| egrep -v '( )(free|malloc|strlen|lseek|read|isspace|isdigit)(.*)llib-lc'
	@lint -u -v -woff 91,302,307,127 \
		`echo -DLINT $(CFLAGS) | tr ' ' '\012' | egrep '^-D'` \
		$(SRCO) $(COMMONSRC) ../Library/llib-lifx.ln -lm \
		| tee -a lint.log \
		| egrep -v '( )(free|malloc|strlen|lseek|read|isspace|isdigit)(.*)llib-lc'
	@lint -u -v -woff 91,302,307,127 \
		`echo -DLINT $(CFLAGS) | tr ' ' '\012' | egrep '^-D'` \
		$(SRCPP) $(COMMONSRC) ../Library/llib-lifx.ln -lm \
		| tee -a lint.log \
		| egrep -v '( )(free|malloc|strlen|lseek|read|isspace|isdigit)(.*)llib-lc'
	@lint -u -v -woff 91,302,307,127 \
		`echo -DLINT $(CFLAGS) | tr ' ' '\012' | egrep '^-D'` \
		$(SRCK) $(COMMONSRC) ../Library/llib-lifx.ln -lm \
		| tee -a lint.log \
		| egrep -v '( )(free|malloc|strlen|lseek|read|isspace|isdigit)(.*)llib-lc'

#------------------------------------------------------------
install:
	/bin/rm -f ${BIN_PATH}/if1opt
	${BINSTALL} if1opt ${BIN_PATH}/if1opt
	chmod $(XMASK) ${BIN_PATH}/if1opt

clean:
	rm -f ${OBJK}
	rm -f ${OBJO}
	rm -f ${OBJPP}
	rm -f ${COMMON}
	rm -f ${OBJ}
	rm -f if1check
	rm -f if1pprint
	rm -f if1count
	rm -f if1opt
	rm -f lint.log

${COMMON}: ${INCL}
${OBJK}:   ${INCL}
${OBJPP}:  ${INCL}
${OBJO}:   ${INCL}
${OBJ}: ${INCL}

#
