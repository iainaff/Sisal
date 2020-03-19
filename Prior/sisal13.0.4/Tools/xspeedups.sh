#!/bin/sh
# Execute a SISAL program on a range of processors to record resource usages.
# Usage:  xspeedups {xspeedup options} startp endp abs [ options ] ...
#   where startp  = beginning number of processors to use
#         endp    = ending number of processors to use
#         abs     = executable SISAL program file
#         options = desired execution options
# Note:  Do not specify the -w or -r options; that's handled here.

#------------------------------------------------------------
# Set some style parameters so we know what to draw
BARS=0
DEF=1
TIMES=1
SPEEDUP=1
PERFECT=1
DATADONE=0
THRESHOLD=95
NEEDMMAX=1
MMAX=0
DATANAMED=0

MORE=1
while [ $MORE = 1 ]; do
  case "$1" in
  -bars)	BARS=1;
		if [ $# -gt 0 ]; then shift; fi;;

  -times)	if [ $DEF = 1 ]; then TIMES=0; SPEEDUP=0; PERFECT=0; fi;
		DEF=0;
		TIMES=1;
		if [ $# -gt 0 ]; then shift; fi;;

  -speedups) 	if [ $DEF = 1 ]; then TIMES=0; SPEEDUP=0; PERFECT=0; fi;
		DEF=0;
		SPEEDUP=1;
		if [ $# -gt 0 ]; then shift; fi;;

  -perfect)	if [ $DEF = 1 ]; then TIMES=0; SPEEDUP=0; PERFECT=0; fi;
		DEF=0;
		PERFECT=1;
		if [ $# -gt 0 ]; then shift; fi;;

  -noplot)	TIMES=0; SPEEDUP=0; PERFECT=0;
		if [ $# -gt 0 ]; then shift; fi;;

  -threshold)	if [ $# -gt 1 ]; then THRESHOLD=$2; shift; fi;
		if [ $# -gt 0 ]; then shift; fi;;

  -usedata)	if [ $# -gt 1 ]; then f=$2; fshort=$2; DATADONE=1; shift; fi;
		if [ $# -gt 0 ]; then shift; fi;;

  -namedata)	if [ $# -gt 1 ]; then f=$2; fshort=$2; DATANAMED=1; shift; fi;
		if [ $# -gt 0 ]; then shift; fi;;

  -max)		if [ $# -gt 1 ]; then NEEDMMAX=0; MMAX=$2; shift; fi;
		if [ $# -gt 0 ]; then shift; fi;;

  *)		MORE=0;;
  esac;
done;

#------------------------------------------------------------
# If generating the datafile...
if [ $DATADONE = 0 ]; then

#------------------------------------------------------------
# Make sure the required arguments exist
  if [ $# -lt 3 ]; then
    echo 'Usage:  xspeedups {xspeed options} startp endp abs [ options ] ...'
    echo '	xspeed options: -bars -times -speedups -perfect -noplot'
    echo '			-threshold <arg> -usedata <arg>'
    echo '			-namedata <arg> -max <arg>'
    echo '	options: Any valid s.out option'
    exit 1
  fi

#------------------------------------------------------------
# Make sure the absolute is executable
  if [ ! -x $3 ]; then
    echo $3 is not an executable file
    exit 1
  fi

# ------------------------------------------------------------
# Check processor range
  if ( test $1 -gt $2 ) then
     echo Error in processor range specification
     exit 1
  fi

#------------------------------------------------------------
# Start with an empty file to collect information
  if [ $DATANAMED = 0 ]; then f=$3.info.$1-$2; fshort=$3; fi
  cp /dev/null $f
  low=$1
  hi=$2
  shift;
  shift

#------------------------------------------------------------
# Run the program once for each processor.  Collect the
# number of processors used, the average time, and the
# maximum time for each run.
  absolute=$1
  shift
  while ( test $low -le $hi ) do
     /bin/rm -f s.info
     $absolute -r -w$low $@

     awk '{if (start) print $0} /  CpuTime/{start=1}' < s.info \
	  | head -$low | tr -d '%' \
	  | awk ' BEGIN{TOT=0.0; MAX=0.0; MIN=999999; N=0} \
		     { if ( $1 < MIN ) MIN=$1; } \
		     { if ( $3 > 100 ) { print "***WARNING: Runtime share > 100% (",$3,"%) in -w'$low'" > "/dev/tty" }} \
		     { if ( $3 <  '$THRESHOLD' ) { print "***WARNING: Runtime share < '$THRESHOLD'% (",$3,"%) in -w'$low'" > "/dev/tty" }} \
		     { if ( $1 > MAX ) MAX=$1; } \
		     { TOT += $1; N++ } \
		     END{ print '$low',TOT/N,MIN,MAX } \
		   ' >> $f

     low=`expr $low + 1`
  done

else
# ------------------------------------------------------------
# Get the low and hi from the timing file (f choosen above)
  low=`head -1 $f | awk '{print $1}'`
  hi=`tail -1 $f | awk '{print $1}'`
fi


#------------------------------------------------------------
# MMAX can be one of two things.  It is either the maximum time
# in the timing run OR the maximum number of processors.  This
# is because we scale the perfect and speedup lines if there
# is a timing run, but not if there isn't
if [ $NEEDMMAX = 1 ]; then MMAX=$hi; fi

#------------------------------------------------------------
# If we have selected a timing run, draw the datapoints for
# the times.  If we want bracketing bars, draw those too.
# Make sure we change the scaling factor if we draw this. 
if [ $TIMES = 1 ]; then
  if [ $NEEDMMAX = 1 ]; then
    MMAX=`awk 'BEGIN{MMAX=0} {if ( $4 > MMAX ) MMAX=$4} END{print MMAX}' $f`
  fi
  echo "\"$fshort Times"
  awk '{ W=$1; AVG=$2; MIN=$3; MAX=$4; \
	    printf "draw %d %g\n",W,AVG; \
	    if ( '$BARS' == 1 && MIN != MAX ) { \
	      printf "move %d %g\n",W,MIN; \
	      printf "draw %d %g\n",W,MAX; \
	      printf "move %g %g\n",W-.02,MIN; \
	      printf "draw %g %g\n",W+.02,MIN; \
	      printf "move %g %g\n",W-.02,MAX; \
	      printf "draw %g %g\n",W+.02,MAX; \
	      printf "move %d %g\n",W,AVG; \
	    } \
	  }' $f
  echo move 1 0
  echo ""
fi

#------------------------------------------------------------
# The perfect speedup line goes from the origin to the (scaled)
# number of processors.
if [ $PERFECT = 1 ]; then
  echo "\"Perfect"
  echo move 0 0
  echo draw $hi $MMAX
  echo ""
fi;

#------------------------------------------------------------
# The speedup line shows the speedup for the average time at
# each processor point.  We assume perfect speedup for the
# first set of numbers (normally W=1).
if [ $SPEEDUP = 1 ]; then
  echo "\"$fshort Speedup"
  echo move $hi $MMAX
  echo move 0 0
  awk 'BEGIN{BASE=0;MMAX='$MMAX'}{ W=$1; AVG=$2; MIN=$3; MAX=$4; \
	    if ( BASE==0 ) BASE=AVG/W; \
	    printf "draw %g %g\n",W,(BASE/AVG)*(MMAX/'$hi'); \
	  }' $f
  echo ""
fi

#
