###############################################################################
# Makefile for OSC (SISAL COMPILER)
# Generated Sat Nov 25 12:24:09 MST 2000 using ./sinstall.
# CHECK FOR ACCURACY
#     Documentation symbols: [] = optional, {} = pick one
#     Command line macro definitions will override those shown here
###############################################################################

# **** PATHS TO COMMANDS USED BY THE MAKEFILE
SHELL       = /bin/sh
CC          = gcc
TARGETCC    = gcc
INSTALL     = /bin/cp
RANLIB      = ranlib
AR          = ar r
DIFF        = diff

# **** HOST AND TARGET SYSTEM
# **** HOST = -D{ENCORE,ALLIANT,etc.}, TARGET = {CRAYT3D,etc.}
HOST = -DSUNIX 
TARGET = 

# *** NUMBER OF AVAILABLE PROCESSORS IN THE HOST SYSTEM
PROCS = 1

# **** FLOATING POINT CHIP (FOR EXAMPLE, SUN)
# **** FPO = -f68881
FPO = 
DPO = 

# **** IS GANGD TO BE USED: ONLY SUPPORTED ON BALANCE
# **** GANGD = [-DGANGD]
GANGD = 

# **** SHOULD THE MAKEFILE GO IN PARALLEL: ONLY SUPPORTED ON BALANCE
# **** PAR = [&]
PAR = 

# **** Miscelleneous def's
MISCDEF = -DLINUX

# **** OPTIMIZE THE GENERATED ASSEMBLY CODE
# **** PHASEOPT = [-O]
PHASEOPT = -g
TARGETOPT = -O 
TARGETLD = -lm

# **** FILE CREATION MASKS
DMASK = 755
XMASK = 755
MMASK = 644
ALLMASKS = DMASK=${DMASK} XMASK=${XMASK} MMASK=${MMASK}

BINSTALL=${INSTALL}
SINSTALL=${INSTALL}

# **** ABSOLUTE PATHS TO EXECUTABLE (BIN_PATH) AND MAN PAGE INSTALLATION
# **** SITES. (Ex. BIN_PATH = /usr/local/bin, MAN_PATH = /usr/local/man)
BIN_PATH = /usr/local/bin
LIB_PATH = /usr/local/lib
INC_PATH = /usr/local/include
MAN_PATH = /usr/local/man

###############################################################################
# DO NOT MODIFY ANYTHING ELSE
###############################################################################

MINIMAL=0

LOCALTARGET=local



PR          = -DMAX_PROCS=1
FC          = f2c
FFLAGS      = 

T_DEF1      = BIN_PATH=${BIN_PATH}
T_DEF2      = MAN_PATH=${MAN_PATH}

LIBM        = 
ANS         = negzero

TIMEM       = 

CC_OPTS     = ${HOST} ${PR} ${FPO} ${DPO} ${GANGD} ${TIMEM} ${MISCDEF}


VERSIONPREFIX=
TMPDIR=
CC_CMD      = "CC=${CC}" "CFLAGS=${CC_OPTS} ${PHASEOPT}" "PAR=${PAR}" "AR=${AR}"
F_CC_CMD    = "CC=${CC}" "CFLAGS=${CC_OPTS} ${PHASEOPT}"
T_CC_CMD    = "CC=${CC}" "CFLAGS=${CC_OPTS} ${PHASEOPT} -DUSESPP=0 -DSCC=0 -DGNU=1 -DGETTMPDIR '"'-DVPREFIX="${VERSIONPREFIX}"'"' '"'-DUSE_TMPDIR="${TMPDIR}"'"' '"'-DUSECC="${TARGETCC}"'"' '"'-DUSEOPT="${TARGETOPT}"'"' '"'-DUSELD="${TARGETLD}"'"' '"'-DUSEFF="${FC}"'"'" "${T_DEF1}" "${T_DEF2}" "PAR=${PAR}" 
R_CC_CMD="CC=${TARGETCC}" "CFLAGS=${CC_OPTS} ${TARGETOPT}" "FC=${FC}" "FFLAGS=${FFLAGS}" "PAR=${PAR}" "AR=${AR}"

PATHS = "BIN_PATH=${BIN_PATH}" "LIB_PATH=${LIB_PATH}" "INC_PATH=${INC_PATH}" "MAN_PATH=${MAN_PATH}"


# **** LOCAL
local: library tools frontend backend runtime

# **** ALL
all: local install

# **** TOOLS
tools:
	cd Tools; \
	$(MAKE) MINIMAL=$(MINIMAL) $(LOCALTARGET) ${T_CC_CMD} ${PATHS}

# **** LIBRARY
library:
	cd Backend/Library; \
	$(MAKE) MINIMAL=$(MINIMAL) $(LOCALTARGET) ${CC_CMD} "RANLIB=${RANLIB}"

# **** FRONTEND
frontend: front1
front1:
	cd Frontend/Front1; \
	$(MAKE) MINIMAL=$(MINIMAL) $(LOCALTARGET) ${F_CC_CMD}
front90:
	if [ -d Frontend/Front90 ]; then cd Frontend/Front90; \
	   $(MAKE) MINIMAL=$(MINIMAL) $(LOCALTARGET) ${F_CC_CMD}; \
	fi
expand90:
	if [ -d Frontend/Expand90 ]; then cd Frontend/Expand90; \
	   $(MAKE) MINIMAL=$(MINIMAL) $(LOCALTARGET) ${F_CC_CMD}; \
	fi

# **** BACKEND
backend: library if1ld if1opt if2mem if2up if2part if2gen
if1ld:
	cd Backend/If1ld; \
	$(MAKE) MINIMAL=$(MINIMAL) $(LOCALTARGET) ${CC_CMD}
if1opt:
	cd Backend/If1opt; \
	$(MAKE) MINIMAL=$(MINIMAL) $(LOCALTARGET) ${CC_CMD}
if2mem:
	cd Backend/If2mem; \
	$(MAKE) MINIMAL=$(MINIMAL) $(LOCALTARGET) ${CC_CMD}
if2up:
	cd Backend/If2up; \
	$(MAKE) MINIMAL=$(MINIMAL) $(LOCALTARGET) ${CC_CMD}
if2part:
	cd Backend/If2part; \
	$(MAKE) MINIMAL=$(MINIMAL) $(LOCALTARGET) ${CC_CMD}
if2gen:
	cd Backend/If2gen; \
	$(MAKE) MINIMAL=$(MINIMAL) $(LOCALTARGET) ${CC_CMD}

# **** RUNTIME
runtime: sharedlib
sharedlib:
	cd Runtime/Sharedlib; \
	$(MAKE) MINIMAL=$(MINIMAL) $(LOCALTARGET) LIBM=$(LIBM) ${R_CC_CMD}
streamlib:
	cd Runtime/Streamlib; \
	$(MAKE) MINIMAL=$(MINIMAL) $(LOCALTARGET) LIBM=$(LIBM) ${R_CC_CMD}
toollib:
	cd Runtime/Toollib; \
	$(MAKE) MINIMAL=$(MINIMAL) $(LOCALTARGET) LIBM=$(LIBM) ${R_CC_CMD}

# **** CHECK
check: clean_check
	cd Check; \
	$(MAKE) ANS=$(ANS) OSC=$(BIN_PATH)/osc DIFF=$(DIFF)
check_streams: clean_check
	cd Check; \
	$(MAKE) ANS=$(ANS) OSC=$(BIN_PATH)/osc DIFF=$(DIFF) SFLAGS_ALL="-v -Keep -STREAMS" check_all
check_all: clean_check
	cd Check; \
	$(MAKE) ANS=$(ANS) OSC=$(BIN_PATH)/osc DIFF=$(DIFF) check_all
check_par:
	cd Check; \
	$(MAKE) ANS=$(ANS) OSC=$(BIN_PATH)/osc DIFF=$(DIFF) PROCS=$(PROCS) check_par

# **** INSTALL
install: install_tools install_library install_frontend install_backend install_runtime
install_all: install install_front90 install_expand90

install_tools:
	cd Tools; \
	$(MAKE) install $(ALLMASKS) -i ${T_CC_CMD} ${PATHS}
install_library:
	cd Backend/Library; \
	$(MAKE) install $(ALLMASKS) -i ${PATHS}
install_frontend: install_front1
install_front1:
	cd Frontend/Front1; \
	$(MAKE) install $(ALLMASKS) -i ${PATHS}
install_front90:
	if [ -d Frontend/Front90 ]; then cd Frontend/Front90; \
	   $(MAKE) install $(ALLMASKS) -i ${PATHS}; \
	fi
install_expand90:
	if [ -d Frontend/Expand90 ]; then cd Frontend/Expand90; \
	   $(MAKE) install $(ALLMASKS) -i ${PATHS}; \
	fi
install_backend: install_if1ld install_if1opt install_if2mem install_if2up install_if2part install_if2gen 
install_if1ld:
	cd Backend/If1ld; \
	$(MAKE) install $(ALLMASKS) -i ${PATHS}
install_if1opt:
	cd Backend/If1opt; \
	$(MAKE) install $(ALLMASKS) -i ${PATHS}
install_if2mem:
	cd Backend/If2mem; \
	$(MAKE) install $(ALLMASKS) -i ${PATHS}
install_if2up:
	cd Backend/If2up; \
	$(MAKE) install $(ALLMASKS) -i ${PATHS}
install_if2part:
	cd Backend/If2part; \
	$(MAKE) install $(ALLMASKS) -i ${PATHS}
install_if2gen:
	cd Backend/If2gen; \
	$(MAKE) install $(ALLMASKS) -i ${PATHS}
install_runtime: install_sharedlib
install_sharedlib:
	cd Runtime/Sharedlib; \
	$(MAKE) install $(ALLMASKS) -i ${PATHS} LIBM=$(LIBM) "AR=${AR}" "RANLIB=${RANLIB}"
install_toollib:
	cd Runtime/Toollib; \
	$(MAKE) install $(ALLMASKS) -i ${PATHS} LIBM=$(LIBM) "AR=${AR}" "RANLIB=${RANLIB}"
install_streamlib:
	cd Runtime/Streamlib; \
	$(MAKE) install $(ALLMASKS) -i ${PATHS} LIBM=$(LIBM) "AR=${AR}" "RANLIB=${RANLIB}"

# **** LINT
lint_library:
	cd Backend/Library;	$(MAKE) lint ${CC_CMD}
lint_if1ld:
	cd Backend/If1ld;	$(MAKE) lint ${CC_CMD}
lint_if1opt:
	cd Backend/If1opt;	$(MAKE) lint ${CC_CMD}
lint_if2mem:
	cd Backend/If2mem;	$(MAKE) lint ${CC_CMD}
lint_if2up:
	cd Backend/If2up;	$(MAKE) lint ${CC_CMD}
lint_if2part:
	cd Backend/If2part;	$(MAKE) lint ${CC_CMD}
lint_if2gen:
	cd Backend/If2gen;	$(MAKE) lint ${CC_CMD}
lint_sharedlib:
	cd Runtime/Sharedlib;	$(MAKE) lint ${R_CC_CMD}
lint_toollib:
	cd Runtime/Toollib;	$(MAKE) lint ${R_CC_CMD}
lint_streamlib:
	cd Runtime/Streamlib;	$(MAKE) lint ${R_CC_CMD}
lint_tools:
	cd Tools;		$(MAKE) lint ${T_CC_CMD}

# **** TAR
 
tar:
	cd ..; tar cvf OSC.tar OSC
compress:
	cd ..; compress OSC.tar
tar.Z: tar compress

# **** CLEAN
clean: clean_runtime clean_check clean_tools clean_backend clean_frontend
clean_all: clean clean_toollib clean_streamlib clean_front90 clean_expand90 
	rm -f sinstall.res Makefile
clean_runtime: clean_sharedlib
clean_sharedlib:
	cd Runtime/Sharedlib;	$(MAKE) clean LIBM=$(LIBM)
clean_toollib:
	if [ -d Runtime/Toollib ]; then cd Runtime/Toollib; \
	   $(MAKE) clean LIBM=$(LIBM); \
	fi
clean_streamlib:
	if [ -d Runtime/Streamlib ]; then cd Runtime/Streamlib; \
	   $(MAKE) clean LIBM=$(LIBM); \
	fi
clean_check:
	cd Check;		$(MAKE) clean
clean_tools:
	cd Tools;		$(MAKE) clean

clean_backend: clean_library \
  clean_if1ld clean_if1opt clean_if2mem clean_if2up clean_if2part clean_if2gen
clean_library:
	cd Backend/Library;	$(MAKE) clean
clean_if1ld:
	cd Backend/If1ld;	$(MAKE) clean
clean_if1opt:
	cd Backend/If1opt;	$(MAKE) clean
clean_if2mem:
	cd Backend/If2mem;	$(MAKE) clean
clean_if2up:
	cd Backend/If2up;	$(MAKE) clean
clean_if2part:
	cd Backend/If2part;	$(MAKE) clean
clean_if2gen:
	cd Backend/If2gen;	$(MAKE) clean

clean_frontend: clean_front1 
clean_front1:
	cd Frontend/Front1;	$(MAKE) clean
clean_front90:
	if [ -d Frontend/Front90 ]; then cd Frontend/Front90; \
	   $(MAKE) clean; \
	fi
clean_expand90:
	if [ -d Frontend/Expand90 ]; then cd Frontend/Expand90; \
	    $(MAKE) clean; \
	fi
#
