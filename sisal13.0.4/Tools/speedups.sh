#!/bin/sh
# Execute a SISAL program on a range of processors to record resource usages.
#

if ( test $# -lt 3 ) then
    { echo 'Usage:  speedups startp endp abs [ options ] ...'; exit 1; }
fi

# Execute a SISAL program on a range of processors to record resource usages.
# Usage:  speedups startp endp abs [ options ] ...
#   where startp  = beginning number of processors to use
#         endp    = ending number of processors to use
#         abs     = executable SISAL program file
#         options = desired execution options
# Note:  Do not specify the -w or -r options; that's handled here.

if ( test $1 -gt $2 ) then
   { echo Error in processor range specification; exit 1; }
fi

f=$3.info.$1-$2
cp /dev/null $f
low=$1
hi=$2
abs=$3
shift;shift;shift

/bin/rm -f s.info
while ( test $low -le $hi ) do
   echo $abs -r -w$low $@
   $abs -r -w$low $@
   low=`expr $low + 1`
done
/bin/mv s.info $f
