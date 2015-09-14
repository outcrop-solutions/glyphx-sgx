#include "sgxstacktrace.h"
#include <stdio.h>
#include <stdlib.h> 
#include <signal.h>

	void abortHandler(int signum)
	{
		// associate each signal with a signal name string.
		const char* name = NULL;
		switch (signum)
		{
		case SIGABRT: name = "SIGABRT";  break;
		case SIGSEGV: name = "SIGSEGV";  break;
			//case SIGBUS:  name = "SIGBUS";   break;
		case SIGILL:  name = "SIGILL";   break;
		case SIGFPE:  name = "SIGFPE";   break;
		}

		// Notify the user which signal was caught. We use printf, because this is the 
		// most basic output function. Once you get a crash, it is possible that more 
		// complex output systems like streams and the like may be corrupted. So we 
		// make the most basic call possible to the lowest level, most 
		// standard print function.
		if (name)
			fprintf(stderr, "Caught signal %d (%s)\n", signum, name);
		else
			fprintf(stderr, "Caught signal %d\n", signum);

		// Dump a stack trace.
		// This is the function we will be implementing next.
		//printStackTrace();

		// If you caught one of the above signals, it is likely you just 
		// want to quit your program right now.
		exit(signum);
	}

	SGXStackTrace::SGXStackTrace()
	{
		signal(SIGABRT, abortHandler);
		signal(SIGSEGV, abortHandler);
		signal(SIGILL, abortHandler);
		signal(SIGFPE, abortHandler);
	}
	
	SGXStackTrace::~SGXStackTrace()
	{
	}
