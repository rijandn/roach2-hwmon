#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include "fork-parent.h"
#include "sensorlib.h"

#define SCAN_INTERVAL 	(10)

static volatile sig_atomic_t doScan = 1;

void signalHandler(int signalNumber)
{
	switch(signalNumber) {
		case SIGTERM:
		case SIGINT:
			doScan = 0;
			break;

		default:
			return; 
	}
}

/* c-function prototypes */
static int daemonize(void);

int main(int argc, char *argv[])
{
	struct sigaction sa;

    printf("The name of this program is '%s'.\n", argv[0]);
	printf("The parent process ID is %d.\n", (int)getpid());

	/* initialize the signal handler */
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = &signalHandler;
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGINT, &sa, NULL);

	/* run process by default in the foreground */
    /* \todo add -d daemon option */
	if (argc != 1) {
		if (daemonize() < 0) {
			printf("Could not daemonize...\n");
			return -1;
		}
	}

	if (sensorlib_load() < 0) {
		fprintf(stderr, "Could not initialize sensor library.\n");
		return -1;
	}

    /* main process loop ... */
    while (doScan) {
		/* print status message to katcp log here */
		printf("start scan...");
		printf("done.\n");
		sleep(SCAN_INTERVAL);
	}

	/* clean-up */
	printf("Performing clean-up...\n");	
	sensorlib_unload();

	/* should not end up here.... */
	return 0;
}

/* deamonize the process by detaching itself from the parent */
int daemonize(void)
{
	if (fork_parent() < 0) {
        fprintf(stderr, "Unable to launch child process.\n");
		return -1;
	}

	/* print init strings via parent stderr pipe */
    fprintf(stderr, "Initializing daemon...\n");
	fprintf(stderr, "The daemon process id is %d.\n", (int)getpid());

	/* change daemon working directory to root */
	if (chdir("/") < 0) {
		fprintf(stderr, "Could not change working directory to root.\n");
		return -1;
	}
	
	/* close the pipe which wil close the parent... */
	fflush(stderr);
	close(STDERR_FILENO);

	return 0;
}