.TH XSPEEDUPS local
.SH NAME
xspeedups \- SISAL parallel speedups data gatherer and plot generator
.SH SYNOPSIS
.B speedups
[ -bars ]
[ -times ]
[ -speedups ]
[ -perfect ]
[ -noplot ]
[ -threshold <num> ]
[ -usedata <file> ]
[ -namedata <file> ]
[ -max <float> ]
{strtnr endnr abs} [ options ] [ infile ] [ outfile ]

.SH DESCRIPTION
In data generation mode,
.I Xspeedups
repeatedly executes the compiled SISAL program ``abs'' using
different numbers of processors, beginning with ``startnr'' and
\fIincrementing\fR through ``endnr.''  For each execution it
automatically appends the ``-\fBw\fRNUM'' option (overriding any of your own).
It also enables ``-\fBr\fR'' so that each execution produces file s.info.
When speedups completes, a file named
``$abs:t.info.$strtnr-$endnr'' will contain worker counts, average,
min, and max times for each execution.
The name can be overridden.

In plot mode,
.I xspeedups
will generate a plot file suitable for
.I xgraph
(l).
The plot will graph the average time, the perfect speedup line, and the
actual speedup line.
You can request a subset of the plots, modify the plot parameters, and
ask for min/max bounding bars on the runtimes.

By default,
.I xspeedups
will both generate data and plot files.  See the options to modify this
behavior.

.SH OPTIONS
.TP 12
.B \-bars
Generate bounding bars to show the minimum and maximum times of the workers.

.TP 12
.B \-times
Plot runtimes.  The speedup curve and the perfect speedup line are disabled
unless specifically requested using the \-speedups and \-perfect options.

.TP 12
.B \-speedups
Plot the speedup curve.  The times and the perfect speedup line are disabled
unless specifically requested using the \-times and \-perfect options.

.TP 12
.B \-perfect
Plot the perfect speedup line.  The times and the speedup curve are disabled
unless specifically requested using the \-times and \-speedups options.

.TP 12
.B \-threshold <num>
.I
Xspeedups
will generate warnings if the percentage of cpu time drops below 95%.
Use this option to raise or lower the percentage.  Note that warnings
will not inhibit data or plot generation.

.TP 12
.B \-noplot
Just generate the data file.

.TP 12
.B \-usedata <file>
Use <file> (created from an earlier run of
.I xspeedups
) to generate the plots rather than executing a SISAL program.

.TP 12
.B \-namedata <file>
Name the datafile <file> instead of ``$abs:t.info.$strtnr-$endnr''.

.TP 12
.B \-max <float>
Plots are normally generated on a field from (1,0) to ($endnr,max)
(from (0,0) to ($endnr,$endnr) for speedups).
This option allows you to modify what maximum value to use either
to make your plots end on a round number or to combine several plots.

.SH USAGE

Create the datafile s.out.1-4 and plot using
.I xgraph
(l).
.br
% xspeedups 1 4 s.out -z infile | xgraph

Plot just the times, but use bounding bars.
.br
% xspeedups -bars -times 1 4 s.out -z infile | xgraph

Just create a datafile (no plot file)
.br
% xspeedups -noplot 1 4 s.out -z infile

And now plot times and speedups separately
.br
% xspeedups -bars -times -usedata s.out.1-4 | xgraph &
.br
% xspeedups -perfect -speedups -usedata s.out.1-4 | xgraph &

Make several plots to different files and combine them (note the
use of the \-max option so that the plots use the same scale)
.br
% xspeedups -noplot -namedata blocked 1 4 s.out -z infile
.br
% xspeedups -noplot -namedata strided 1 4 s.out -z -strided infile
.br
% xspeedups -noplot -namedata gss 1 4 s.out -z -gss infile
.br
% xspeedups -usedata blocked -max 12.0 > plot
.br
% xspeedups -usedata strided -max 12.0 >> plot
.br
% xspeedups -usedata gss     -max 12.0 >> plot
.br
% xgraph plot

