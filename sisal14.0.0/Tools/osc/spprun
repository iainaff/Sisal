#!/bin/sh

# RUN THE C PREPROCESSOR ON THE PREPARED SISAL SOURCE FILE AND LEAVE THE RESULT
# IN $3.i
# ARGS: $1=spp2 $2=cc_cmd $3=foo.sis $4=temp.c $5=temp.spp $6=foo.i $7=rest

spp2cmd=$1
cccmd=$2
sisfile=$3
cfile=$4
sppfile=$5
result=$6
shift;shift;shift;shift;shift;shift

if [ -f $cfile ]; then
  if [ ! -w $cfile ]; then
    echo cannot overwrite $cfile
    exit 1
  fi
fi
/bin/rm -f $cfile

if [ -f $sppfile ]; then
  if [ ! -w $sppfile ]; then
    echo cannot overwrite $sppfile
    exit 1
  fi
fi
/bin/rm -f $sppfile
  
if [ ! -f $sisfile ]; then
  echo $sisfile does not exist
  exit 1
fi

if /bin/cp $sisfile $cfile; then status=0; else status=1; fi
if [ $status != 0 ]; then
  /bin/rm -f $cfile
  exit 1
fi

if $cccmd -E -I. $* $cfile > $sppfile; then status=0; else status=1; fi
if [ $status != 0 ]; then
  /bin/rm -f $cfile
  /bin/rm -f $sppfile
  exit 1
fi

if $spp2cmd -B$cfile -R$sisfile < $sppfile > $result; then status=0; else status=1; fi
if [ $status != 0 ]; then
  /bin/rm -f $sppfile
  /bin/rm -f $cfile
  exit 1
fi

/bin/rm -f $sppfile
/bin/rm -f $cfile

exit 0
