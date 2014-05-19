#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	pid_t child_pid;

	printf("The name of this program is '%s'.\n", argv[0]);
	printf("This program was invoked with %d arguments.\n\n", (argc - 1));

	printf("The main program process ID is %d.\n", (int)getpid());

	child_pid = fork();

        printf("Hello Fork World!!!\n");

	if (child_pid != 0) {
		printf("This is the parent process with ID %d.\n", (int)getpid());
		printf("The child process ID is %d.\n", (int)child_pid);
	} else {
		printf("This is the child process with ID %d.\n", (int)getpid());
	}

	return 0;
}

