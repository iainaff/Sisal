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
OBJ = if2up.o up.o      \
      if2call.o if2noop.o if2refcnt.o if2class.o if2rwset.o if2ade.o   \
      if2replace.o if2count.o if2const.o if2owner.o if2refcntopt.o     \
      if2marks.o if2migrate.o

OBJPP = pprint.o if2pprint.o

COMMONSRC= ifxstuff.c
SRC = if2up.c up.c      \
      if2call.c if2noop.c if2refcnt.c if2class.c if2rwset.c if2ade.c   \
      if2replace.c if2count.c if2const.c if2owner.c if2refcntopt.c     \
      if2marks.c if2migrate.c

SRCPP = pprint.c if2pprint.c

INCL = world.h ../Library/IFX.h ../../config.h

MINIMAL=0
.c.o:
	${CC} ${CFLAGS} -c $*.c
	-@if [ $(MINIMAL) = 1 ]; then /bin/rm -f $*.c; else cat /dev/null; fi

local: if2up
optional: if2pprint
all: local optional

if2up:${PAR} ${OBJ} ${COMMON} ../Library/libifx.a
	${CC} ${CFLAGS} -o if2up ${OBJ} ${COMMON} ../Library/libifx.a -lm

if2pprint:${PAR} ${OBJPP} ${COMMON} ../Library/libifx.a
	${CC} ${CFLAGS} -o if2pprint ${OBJPP} ${COMMON} ../Library/libifx.a -lm

clean:
	rm -f ${OBJ}
	rm -f ${OBJPP}
	rm -f ${COMMON}
	rm -f if2up
	rm -f if2pprint
	rm -f lint.log

#------------------------------------------------------------
lint: lint.log

# 127 == main() returns random value to invocation environment
lint.log: $(SRC) $(SRCPP) $(COMMONSRC) $(INCL) ../Library/llib-lifx.ln
	@lint -u -v -woff 91,302,307,127 \
		`echo -DLINT $(CFLAGS) | tr ' ' '\012' | egrep '^-D'` \
		$(SRC) $(COMMONSRC) ../Library/llib-lifx.ln -lm \
		| tee lint.log \
		| egrep -v '( )(free|malloc|strlen|lseek|read|isspace|isdigit)(.*)llib-lc'
	@lint -u -v -woff 91,302,307,127 \
		`echo -DLINT $(CFLAGS) | tr ' ' '\012' | egrep '^-D'` \
		$(SRCPP) $(COMMONSRC) ../Library/llib-lifx.ln -lm \
		| tee -a lint.log \
		| egrep -v '( )(free|malloc|strlen|lseek|read|isspace|isdigit)(.*)llib-lc'

#------------------------------------------------------------
install:
	/bin/rm -f ${BIN_PATH}/if2up
	${BINSTALL} if2up ${BIN_PATH}/if2up
	chmod $(XMASK) ${BIN_PATH}/if2up

${OBJ}:   ${INCL}
${OBJPP}: ${INCL}
${COMMON}: ${INCL}

#
