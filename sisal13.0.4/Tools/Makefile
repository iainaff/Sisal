SHELL=/bin/sh
XMASK = 555
MMASK = 444

CC           = cc
CFLAGS       = -g -DMAX_PROCS=1
BIN_PATH     = ../../bin
LIB_PATH     = ../../lib
INC_PATH     = ../../include
MAN_PATH     = ../../man
BINSTALL     = cp
SINSTALL     = cp

BP           = \"${BIN_PATH}\"
LP           = \"${LIB_PATH}\"
IP           = \"${INC_PATH}\"

SRCC = osc.c spp2.c
SRCS = $(SRCC) speedups.sh spprun.sh xspeedups.sh

OBJC = osc.o spp2.o
OUTS = osc spp2 speedups spprun xspeedups

MINIMAL=0

local: $(OUTS)
optional:
all: local optional

clean:
	rm -f $(OBJC)
	rm -f $(OUTS)
	rm -f man.update osc.1.bu osc.1.new
	rm -f lint.log

newoptions:
	$(BIN_PATH)/parseopts < options -usage "man.update" usage.h options.h
	awk -e '{print} /.\\" Start options/{exit}' < osc.1 > osc.1.new
	cat man.update >> osc.1.new
	awk -e '/.\\" End options/{flag=1} {if (flag) print}' < osc.1 >> osc.1.new
	mv osc.1 osc.1.bu
	mv osc.1.new osc.1

osc: osc.c usage.h options.h ../Makefile ../config.h
	$(CC) $(CFLAGS) -o osc osc.c -DBIN_PATH=${BP} -DLIB_PATH=${LP} -DINC_PATH=${IP}

spp2: spp2.c ../Makefile ../config.h
	$(CC) $(CFLAGS) -o spp2 spp2.c

speedups: speedups.sh
	cp speedups.sh speedups
	chmod +x speedups
spprun: spprun.sh
	cp spprun.sh spprun
	chmod +x spprun
xspeedups: xspeedups.sh
	cp xspeedups.sh xspeedups
	chmod +x xspeedups

install:
	/bin/rm -f ${BIN_PATH}/osc
	${BINSTALL} osc ${BIN_PATH}/osc
	chmod $(XMASK) ${BIN_PATH}/osc
	/bin/rm -f ${BIN_PATH}/spp2
	${BINSTALL} spp2 ${BIN_PATH}/spp2
	chmod $(XMASK) ${BIN_PATH}/spp2
	/bin/rm -f ${BIN_PATH}/speedups
	${BINSTALL} speedups ${BIN_PATH}/speedups
	chmod $(XMASK) ${BIN_PATH}/speedups
	/bin/rm -f ${BIN_PATH}/xspeedups
	${BINSTALL} xspeedups ${BIN_PATH}/xspeedups
	chmod $(XMASK) ${BIN_PATH}/xspeedups
	/bin/rm -f ${BIN_PATH}/spprun
	${BINSTALL} spprun ${BIN_PATH}/spprun
	chmod $(XMASK) ${BIN_PATH}/spprun
	/bin/rm -f ${MAN_PATH}/man1/osc.1
	${SINSTALL} osc.1 ${MAN_PATH}/man1/osc.1
	chmod $(MMASK) ${MAN_PATH}/man1/osc.1
	/bin/rm -f ${MAN_PATH}/man1/speedups.1
	${SINSTALL} speedups.1 ${MAN_PATH}/man1/speedups.1
	chmod $(MMASK) ${MAN_PATH}/man1/speedups.1
	/bin/rm -f ${MAN_PATH}/man1/xspeedups.1
	${SINSTALL} xspeedups.1 ${MAN_PATH}/man1/xspeedups.1
	chmod $(MMASK) ${MAN_PATH}/man1/xspeedups.1

# 127 == main() returns random value to invocation environment
# 183 == Previous declaration had prototype
lint: $(SRCC)
	@lint -u -v -woff 91,302,307,127,183 \
		`echo -DLINT $(CFLAGS) -DBIN_PATH=${BP} | tr ' ' '\012' | egrep ^-D | egrep -v "^-DUSE(CC|FF)="`\
		$(SRC) -lm \
		| tee lint.log \
		| egrep -v '( )(bcopy|free|malloc|strlen|lseek|read|isalpha|strcmp|strncmp|strcpy|strncpy|strcat|isspace|isdigit)(.*)llib-lc'

#
