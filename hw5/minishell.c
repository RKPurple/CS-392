/*
Rohan Kallur
I pledge my honor that I have abided by the Stevens Honor System.
*/

#define _GNU_SOURCE

#define BLUE "\x1b[34;1m"
#define DEFAULT "\x1b[0m"

#include <stdio.h>
#include <limits.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/types.h>
#include <wait.h>

volatile sig_atomic_t unprocessedSig = 0;

void signal_handler(int signum)
{
    unprocessedSig = signum;
}

int main()
{
    struct sigaction command;
    memset(&command, 0, sizeof(struct sigaction));
    command.sa_handler = signal_handler;
    if (sigaction(SIGINT, &command, NULL) == -1)
    {
        fprintf(stderr, "Error: Cannot register signal handler. %s.\n", strerror(errno));
        return -1;
    }
    char cwd[PATH_MAX];
    while (1)
    {
        // store the path into the cwd variable & error check
        if (getcwd(cwd, sizeof(cwd)) == NULL)
        {
            fprintf(stderr, "Error: Cannot get current working directory. %s.\n", strerror(errno));
            return -1;
        }
        // print the directory and await input
        printf("[%s%s%s]> ", BLUE, cwd, DEFAULT);
        fflush(stdout);
        char input[4096];
        // read from the terminal and store in the input array
        if (fgets(input, 4096, stdin) == NULL)
        {
            // Check if Ctrl + C was used
            if (unprocessedSig == SIGINT)
            {
                unprocessedSig = 0;
                printf("\n");
                continue;
            }
            fprintf(stderr, "Error: Failed to read from stdin. %s.\n", strerror(errno));
            return -1;
        }
        int len = strlen(input);
        char resized_input[len];
        strncpy(resized_input, input, len);
        resized_input[len - 1] = '\0';
        // exit condition
        if (strcmp(resized_input, "exit") == 0)
        {
            return EXIT_SUCCESS;
        }

        // seperate the input string into arguments
        char *argv[100];
        char *token;
        token = strtok(resized_input, " ");
        int i = 0;
        while (token != NULL)
        {
            argv[i] = token;
            token = strtok(NULL, " ");
            i++;
        }
        argv[i] = NULL;
        uid_t uid = getuid();
        struct passwd *pw = getpwuid(uid);
        const char *home_dir = pw->pw_dir;
        if (pw == NULL)
        {
            fprintf(stderr, "Error: Cannot get passwd entry. %s.\n", strerror(errno));
            continue;
        }

        // cd condition
        if (strcmp(argv[0], "cd") == 0)
        {
            if (i > 2)
            {
                fprintf(stderr, "Error: Too many arguments to cd.\n");
                continue;
            }
            else if (i == 1 || strcmp(argv[1], "~") == 0) // action if home directory is destination
            {
                if (chdir(home_dir) == -1)
                {
                    fprintf(stderr, "Error: Cannot change directory to '%s'. %s.\n", home_dir, strerror(errno));
                    continue;
                }
            }
            else
            {
                if (chdir(argv[1]) == -1) // for any other specified directory
                {
                    fprintf(stderr, "Error: Cannot change directory to '%s'. %s.\n", argv[1], strerror(errno));
                    continue;
                }
            }
        }
        // Any other command
        else
        {
            pid_t pid = fork();
            if (pid == -1) // check if fork failed
            {
                fprintf(stderr, "Error: fork() failed. %s.\n", strerror(errno));
                continue;
            }
            else if (pid == 0) // child process
            {
                if (execvp(argv[0], argv) == -1)
                {
                    fprintf(stderr, "Error: exec() failed. %s.\n", strerror(errno));
                    return -1;
                }
            }
            else // parent process
            {
                if (waitpid(pid, NULL, 0) == -1)
                {
                    if (unprocessedSig == SIGINT)
                    {
                        unprocessedSig = 0;
                        printf("\n");
                        continue;
                    }
                    fprintf(stderr, "Error: wait() failed. %s.\n", strerror(errno));
                    continue;
                }
            }
        }
    }
    return 0;
}