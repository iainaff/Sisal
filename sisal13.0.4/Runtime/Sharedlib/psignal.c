#include "world.h"

#if defined(HPUX) || defined(HPUXPA) || defined(CRAYT3D)

#define SOMECODE

char	*sys_siglist[NSIG] = {
	"Signal 0",
	"Hangup",			/* SIGHUP    */
	"Interrupt",			/* SIGINT    */
	"Quit",				/* SIGQUIT   */
	"Illegal instruction",		/* SIGILL    */
	"Trace/BPT trap",		/* SIGTRAP   */
	"IOT trap",			/* SIGIOT    */
	"EMT trap",			/* SIGEMT    */
	"Floating point exception",	/* SIGFPE    */
	"Killed",			/* SIGKILL   */
	"Bus error",			/* SIGBUS    */
	"Segmentation fault",		/* SIGSEGV   */
	"Bad system call",		/* SIGSYS    */
	"Broken pipe",			/* SIGPIPE   */
	"Alarm clock",			/* SIGALRM   */
	"Terminated",			/* SIGTERM   */
	"User defined signal 1",	/* SIGUSR1   */
	"User defined signal 2",	/* SIGUSR2   */
	"Child exited",			/* SIGCLD    */
	"Power-fail restart",		/* SIGPWR    */
	"Virtual timer expired",	/* SIGVTALRM */
	"Profiling timer expired",	/* SIGPROF   */
	"I/O possible",			/* SIGIO     */
	"Window size changes",		/* SIGWINDOW */
	"Stopped (signal)",		/* SIGSTOP   */
	"Stopped",			/* SIGTSTP   */
	"Continued",			/* SIGCONT   */
	"Stopped (tty input)",		/* SIGTTIN   */
	"Stopped (tty output)",		/* SIGTTOU   */
	"Urgent I/O condition",		/* SIGURG    */
	"Remote lock lost (NFS)" 	/* SIGLOST   */
#if !defined(HPUX) && !defined(CRAYT3D)
	"Signal 31",			/* reserved  */
	"DIL signal"			/* SIGDIL    */
#endif
};

extern	char *sys_siglist[];

psignal(sig, s)
	unsigned sig;
	char *s;
{
	register char *c;
	register n;

	c = "Unknown signal";
	if (sig < NSIG && sig >= 0)
		c = sys_siglist[sig];
	n = strlen(s);
	if (n) {
		write(2, s, n);
		write(2, ": ", 2);
	}
	write(2, c, strlen(c));
	write(2, "\n", 1);
}

#endif

#ifndef SOMECODE
int UNUSED_psignal;		/* Make sure there's something... */
#endif
