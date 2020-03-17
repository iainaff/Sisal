AC_INIT([sisal], [14.1.0], [])
AC_PREREQ([2.62])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_AUX_DIR(config)
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([1.11 -Wall -Werror foreign])

# -----------------------------------------------
# Better to put . into path for some of the tests
# -----------------------------------------------
export PATH=".:$PATH"

extraArguments () { shift; echo $*; }

dnl -----------------------------------------------
dnl Needed for things like conftest to work
dnl -----------------------------------------------
PATH=".:$PATH"

dnl -----------------------------------------------
dnl check to see if FORTRAN turned off
dnl -----------------------------------------------
fortran_enabled=1
AC_SUBST(fortran_enabled)
AC_ARG_ENABLE(fortran, --disable-fortran   turn off FORTRAN support if you have to compile,[
  if test "$enableval" = "no"; then fortran_enabled=0; fi
])

# -----------------------------------------------
# Check for C and maybe FORTRAN
# -----------------------------------------------
AC_PROG_CC
if test "$fortran_enabled" = 1; then
  AC_CHECK_PROGS(F77, fl32 f77 fort77 xlf cf77 g77 f90 xlf90 f2c,noF77)
  if test "$F77" != "noF77"; then
    AC_PROG_F77
  else
    AC_MSG_WARN(FORTRAN support disabled)
    fortran_enabled=0
  fi
fi
CCARGUMENTS=`extraArguments $CC`
AC_PATH_PROG(CC,$CC,$CC)
CC="$CC $CCARGUMENTS"

AC_USE_SYSTEM_EXTENSIONS

dnl -----------------------------------------------
dnl FORTRAN can be turned off
dnl -----------------------------------------------
FDASHI_OK=0
AC_SUBST(FDASHI_OK)
if test "$fortran_enabled" = 1; then
  FFARGUMENTS=`extraArguments $F77`
  AC_PATH_PROG(F77,$F77,$F77)
  F77="$F77 $FFARGUMENTS"
  AC_F77_LIBRARY_LDFLAGS

  dnl -----------------------------------------------
  dnl Some Fortran compilers do not allow -I options
  dnl -----------------------------------------------
  AC_MSG_CHECKING(-I allowed in FORTRAN)
  cat > conftest.f <<EOF
        PROGRAM X
        STOP
        END
EOF
  if $F77 -c conftest.f -I. >/dev/null 2>&1; then
    rm -fr conftest.*
    AC_MSG_RESULT(yes)
    FDASHI_OK=1
  else
    rm -fr conftest.*
    AC_MSG_RESULT(no)
    FDASHI_OK=0
  fi
fi

# -----------------------------------------------
# We need some helper tools
# -----------------------------------------------
AC_PROG_INSTALL
AC_SUBST(AR) AC_PATH_PROG(AR,ar,ar)
AC_SUBST(DIFF) AC_PATH_PROG(DIFF,diff,diff)
AC_PROG_RANLIB AC_PATH_PROG(RANLIB,$RANLIB,$RANLIB)
AC_PROG_LN_S

# -----------------------------------------------
# Header checks...
# -----------------------------------------------
AC_CONFIG_HEADER(Include/sisalConfig.h)
AC_HEADER_STDC
AC_CHECK_HEADERS(unistd.h)
AC_CHECK_HEADERS(time.h)
AC_CHECK_HEADERS(limits.h)
AC_CHECK_HEADERS(float.h)
AC_CHECK_HEADERS(fcntl.h)
AC_CHECK_HEADERS(string.h)
AC_CHECK_HEADERS(strings.h)
AC_CHECK_HEADERS(sys/types.h)
AC_CHECK_HEADERS(sys/times.h)
AC_CHECK_HEADERS(sys/resource.h)
AC_CHECK_HEADERS(sys/stat.h)
AC_CHECK_HEADERS(sys/wait.h)
AC_HEADER_TIME

# -----------------------------------------------
# Check timer possibilities
# -----------------------------------------------
AC_MSG_CHECKING(Cray high resolution timer)
AC_RUN_IFELSE([AC_LANG_SOURCE([[
  extern double TSECND();
  int main() {double x = TSECND(); return 0;}
]])],[
  AC_MSG_RESULT(yes)
  AC_DEFINE([HAVE_TSECND_INTRINSIC],[],[Define if the TSECND intrinsic (Cray) is present])
],[
  AC_MSG_RESULT(no)
],[
  AC_MSG_RESULT(no)
  AC_MSG_ERROR(Cannot cross compile)
])

AC_MSG_CHECKING(getrusage())
AC_RUN_IFELSE([AC_LANG_SOURCE([[
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#if TIME_WITH_SYS_TIME
#include <sys/time.h>
#include <time.h>
#else
#if HAVE_SYS_TIME_H
#include <sys/time.h>
#else
#include <time.h>
#endif
#endif
#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif
int main() {
  struct rusage timerInfo;
  (void) getrusage(RUSAGE_SELF, &timerInfo);
  return 0;
}
]])],[
  AC_MSG_RESULT(yes)
  AC_DEFINE([HAVE_RUSAGE],[],[Define if we can get usage times with the getrusage() call])
],[
  AC_MSG_RESULT(no)
],[
  AC_MSG_RESULT(no)
  AC_MSG_ERROR(Cannot cross compile)
])

AC_MSG_CHECKING(times())
AC_RUN_IFELSE([AC_LANG_SOURCE([[
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#if TIME_WITH_SYS_TIME
#include <sys/time.h>
#include <time.h>
#else
#if HAVE_SYS_TIME_H
#include <sys/time.h>
#else
#include <time.h>
#endif
#endif
#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif
int main() {
  struct tms timerInfo;
  (void) times(&timerInfo);
  return 0;
}
]])],[
  AC_MSG_RESULT(yes)
  AC_DEFINE([HAVE_TIMES],[],[Define if we can get usage times with the times() call])
],[
  AC_MSG_RESULT(no)
],[
  AC_MSG_RESULT(no)
  AC_MSG_ERROR(Cannot cross compile)
])

dnl -----------------------------------------------
dnl We have some keywords to check for
dnl -----------------------------------------------
AC_MSG_CHECKING(register keyword)
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]], [[register int x;]])],[
  AC_MSG_RESULT(yes)
  AC_DEFINE([HAVE_REGISTER_KEYWORD],[],[Define if the register keyword works])
],[
  AC_MSG_RESULT(no)
])
AC_MSG_CHECKING(volatile keyword)
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]], [[volatile int x;]])],[
  AC_MSG_RESULT(yes)
  AC_DEFINE([HAVE_VOLATILE_KEYWORD],[],[Define if the volatile keyword works])
],[
  AC_MSG_RESULT(no)
])
AC_MSG_CHECKING(shared keyword)
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[static shared int x]], [[int y=x;]])],[
  AC_MSG_RESULT(yes)
  AC_DEFINE([HAVE_SHARED_KEYWORD],[],[Define if the shared keyword works])
],[
  AC_MSG_RESULT(no)
])

dnl -----------------------------------------------
dnl Prototypes
dnl -----------------------------------------------
have_prototypes=no
AC_MSG_CHECKING(for prototypes)
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
  extern int foo(int);
]], [[
  return foo(10);
]])],[
  AC_DEFINE([HAVE_PROTOTYPES],[],[Define if you allow normal C prototypes])
  have_prototypes=yes
],[])
AC_MSG_RESULT($have_prototypes)

dnl -----------------------------------------------
dnl GCC-style 'noreturn' attribute
dnl -----------------------------------------------
have_attr_no_return=no
AC_MSG_CHECKING(for GCC-style noreturn attribute)
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
  extern void fatal() __attribute__ ((noreturn));
]], [[
  fatal();
]])],[
  AC_DEFINE([HAVE_GCC_ATTR_NORETURN],[],[Define if GCC-style noreturn attribute is supported])
  have_attr_no_return=yes
],[])
AC_MSG_RESULT($have_attr_no_return)

dnl -----------------------------------------------
dnl const
dnl -----------------------------------------------
have_const=no
AC_MSG_CHECKING(for const)
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
]], [[
  const int x;
  x = 0;
]])],[
  AC_DEFINE([HAVE_CONST],[],[Define if C does not support const keyword])
  have_const=yes
],[])
AC_MSG_RESULT($have_const)


# -----------------------------------------------
# See if we can get the clock tick
# -----------------------------------------------
sisal_clock_tick=NONE
AC_SUBST(sisal_clock_tick)

if test "$sisal_clock_tick" = "NONE"; then
AC_MSG_CHECKING(CLK_TCK from sysconf)
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
]], [[
   int x = sysconf(_SC_CLK_TCK)
]])],[
  AC_MSG_RESULT(yes)
  sisal_clock_tick="sysconf(_SC_CLK_TCK)"
],[
  AC_MSG_RESULT(no)
])
fi

if test "$sisal_clock_tick" = "NONE"; then
AC_MSG_CHECKING(CLOCKS_PER_SEC in time.h)
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#ifdef HAVE_TIME_H
#include <time.h>
#endif
]], [[
   int x = CLOCKS_PER_SEC;
]])],[
  AC_MSG_RESULT(yes)
  sisal_clock_tick=CLOCKS_PER_SEC
],[
  AC_MSG_RESULT(no)
])
fi

if test "$sisal_clock_tick" = "NONE"; then
AC_MSG_CHECKING(CLK_TCK in time.h)
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#ifdef HAVE_TIME_H
#include <time.h>
#endif
]], [[
   int x = CLK_TCK;
]])],[
  AC_MSG_RESULT(yes)
  sisal_clock_tick=CLK_TCK
],[
  AC_MSG_RESULT(no)
])
fi

if test "$sisal_clock_tick" = "NONE"; then
AC_MSG_CHECKING(CLK_TCK as a constant 60)
sisal_clock_tick=60
AC_MSG_RESULT(Who knows if this will work, but...)
fi


# -----------------------------------------------
# Check for bad behavior in compilers
# -----------------------------------------------
bad_forward=no
AC_MSG_CHECKING(for bad static forward)
AC_RUN_IFELSE([AC_LANG_SOURCE([[
struct s { int a; int b; };
static struct s foo;
int foobar() {
 static int random;
 random = (int) &foo;
 return random;
}
static struct s foo = { 1, 2 };
main() {
 exit(!((int)&foo == foobar()));
}
]])],[
  bad_forward=no
],[
  AC_DEFINE([BAD_STATIC_FORWARD],[],[Define if your compiler botches static forward declarations (as it does on SCI ODT 3.0)]) 
  bad_forward=yes,
],[
  bad_forward=no
])
AC_MSG_RESULT($bad_forward)

dnl -----------------------------------------------
dnl Unaccounted for process symbols
dnl -----------------------------------------------
if test "black" = "white"; then
CACHE_LINE
MaxInt
MinInt
MaxFloat
MinFloat
MaxDouble
MinDouble
fi

dnl -----------------------------------------------
dnl Set the proper model
dnl -----------------------------------------------
model=PThreads
AC_SUBST(model)
AC_MSG_CHECKING(parallel model)
AC_ARG_WITH(model,--with-model=<parallelism model>,[
   model=$withval
])
AC_MSG_RESULT($model)

dnl -----------------------------------------------
dnl Make sure we have support the choosen model
dnl -----------------------------------------------
AC_MSG_CHECKING(if $model is supported)
if test -d "${srcdir}/Runtime/${model}Model"; then
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
  AC_MSG_ERROR(no such model $model in Runtime directory)
fi

dnl -----------------------------------------------
dnl Set compile, link flags, and max_procs
dnl -----------------------------------------------
parallel_compile=""
parallel_link=""
max_procs=1

AC_SUBST(parallel_compile)
AC_SUBST(parallel_link)
AC_SUBST(max_procs)

if test -x "${srcdir}/Runtime/${model}Model/detect"; then
  AC_MSG_CHECKING(compile flags)
  parallel_compile=`"${srcdir}/Runtime/${model}Model/detect" compile $CC $CFLAGS`
  AC_MSG_RESULT($parallel_compile)

  AC_MSG_CHECKING(link flags)
  parallel_link=`"${srcdir}/Runtime/${model}Model/detect" link $CC $CFLAGS $parallel_compile $LIBS`
  AC_MSG_RESULT($parallel_link)

  AC_MSG_CHECKING(processor count)
  max_procs=`"${srcdir}/Runtime/${model}Model/detect" processors $CC $CFLAGS $parallel_compile $LIBS $parallel_link`
  if test "$max_procs" = "ERROR"; then max_procs=1; fi
  AC_MSG_RESULT($max_procs)

fi

if test "$parallel_compile" = "ERROR" || test "$parallel_link" = "ERROR"; then
  AC_MSG_WARN(Parallel model not implemented here.  Reverting to Serial)
  model=Serial
  parallel_compile=""
  parallel_link=""
  max_procs=1
fi

dnl -----------------------------------------------
dnl Length of pathnames
dnl -----------------------------------------------
max_path_length=2048
AC_SUBST(max_path_length)
AC_MSG_CHECKING(length of a pathname)
cat > conftest.$ac_ext <<EOF
#include <unistd.h>
#include <stdio.h>
int main() {
  int pl = pathconf("/",_PC_PATH_MAX);
  printf("%d\n",pl);
  return 0;
}
EOF
if ( eval $ac_link > /dev/null 2>&1 ); then
   path_length=`./conftest${ac_exeext}`
   if test "$path_length" != "0"; then
      max_path_length="$path_length";
   fi
fi
AC_MSG_RESULT($max_path_length)
rm -fr conftest.$ac_ext conftest${ac_exeext}

dnl -----------------------------------------------
dnl Help the pthreads implementation for Draft 4 vs.
dnl standard
dnl -----------------------------------------------
AC_CHECK_HEADERS(sched.h)
if test "$model" = "PThreads"; then
  saveLIBS="$LIBS"
  LIBS="$LIBS $parallel_link"
  AC_CHECK_FUNCS(sched_yield)
  LIBS="$saveLIBS"
fi

dnl -----------------------------------------------
dnl Non coherent cache issues!
dnl -----------------------------------------------
AC_MSG_CHECKING(non-coherent cache)
if test "$model" = "IBMPower4"; then
  AC_DEFINE([NON_COHERENT_CACHE],[],[Define if model has non-coherent cache (Dead IBM Power 4)])
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi

dnl -----------------------------------------------
dnl Mutable descriptors in FLI
dnl -----------------------------------------------
AC_MSG_CHECKING(support for mutable descriptors)
if test "black" = "white"; then
  AC_DEFINE([MUTABLE_DESCRIPTORS],[],[Define if your FLI supports mutable descriptors])
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi

dnl -----------------------------------------------
dnl psignal
dnl -----------------------------------------------
AC_CHECK_FUNCS(psignal)

dnl -----------------------------------------------
dnl Distributed DSA (not used for a while)
dnl -----------------------------------------------
AC_ARG_ENABLE(dist-dsa, --enable-dist-dsa   Turn on the distributed memory allocation scheme (broken),[
  DIST_DSA="$enableval"
],[
  DIST_DSA="no"
])
  
AC_MSG_CHECKING(distributed dsa)
if test "$DIST_DSA" = "yes"; then
  AC_DEFINE([DIST_DSA],[],[Define if you choose a distributed DSA model (broken)])
fi
AC_MSG_RESULT($DIST_DSA)

dnl -----------------------------------------------
dnl Static sharing problem?
dnl -----------------------------------------------
AC_ARG_ENABLE(bad-static-sharing, --enable-bad-static-sharing  Problem with some obsolete C compilers,[
  bad_static_sharing="$enableval"
],[
  bad_static_sharing="no"
])

AC_MSG_CHECKING(static sharing problem?)
if test "$bad_static_sharing" = "yes"; then
  AC_DEFINE([NO_STATIC_SHARED],[],[Define if your C compiler cannot do static sharing])
fi
AC_MSG_RESULT($bad_static_sharing)

dnl -----------------------------------------------
dnl Build instrumented version?
dnl -----------------------------------------------
AC_ARG_ENABLE(instrumentation, --enable-instrumentation  Build instrumentation into runtime,[
  instrument_runtime="$enableval"
],[
  instrument_runtime="no"
])
AC_MSG_CHECKING(Build instrumented version?)
if test "$instrument_runtime" = "yes"; then
  AC_DEFINE([VERBOSE],[],[Define if you want verbose information from the runtime])
  AC_DEFINE([DEBUG_DSA],[],[Define if you want debug info for the DSA])
fi
AC_MSG_RESULT($instrument_runtime)

AM_CPPFLAGS="${AM_CPPFLAGS} -DSISAL_PATH_PREFIX='\"\$(prefix)\"'"
AM_CPPFLAGS="${AM_CPPFLAGS} -DSISAL_PATH_LIBEXECDIR='\"\$(pkglibexecdir)\"'"
AM_CPPFLAGS="${AM_CPPFLAGS} -DSISAL_PATH_DATADIR='\"\$(pkgdatadir)\"'"
AM_CPPFLAGS="${AM_CPPFLAGS} -DSISAL_PATH_INCLUDEDIR='\"\$(pkgincludedir)\"'"
AM_CPPFLAGS="${AM_CPPFLAGS} -DSISAL_PATH_LIBDIR='\"\$(pkglibdir)\"'"

AC_SUBST(AM_CPPFLAGS)

AC_CONFIG_FILES([Makefile
	Include/sisalPaths.h
	Include/sisalInfo.h
	Frontend/Front1/Makefile
	Backend/Library/Makefile
	Backend/If1ld/Makefile
	Backend/If1opt/Makefile
	Backend/If2gen/Makefile
	Backend/If2mem/Makefile
	Backend/If2part/Makefile
	Backend/If2up/Makefile
	Runtime/Makefile
	Runtime/AlliantModel/Makefile
	Runtime/CrayModel/Makefile
	Runtime/EncoreModel/Makefile
	Runtime/IBMPower4Model/Makefile
	Runtime/PThreadsModel/Makefile
	Runtime/SequentModel/Makefile
	Runtime/SerialModel/Makefile
	Runtime/SGIModel/Makefile
	Runtime/Sharedlib/sisalrt.h
	Runtime/Sharedlib/Makefile
	Runtime/SolarisModel/Makefile
	Tools/sisalc/Makefile
	Tests/Makefile
])
AC_OUTPUT

