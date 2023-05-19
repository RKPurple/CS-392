/*
Rohan Kallur
I pledge my honor that I have abided by the Stevens Honor System.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: ./spfind <directory> <permissions string>\n");
        return EXIT_FAILURE;
    }

    int findpipe[2], sortpipe[2];
    // Create 2 pipes
    if (pipe(findpipe) < 0 || pipe(sortpipe) < 0)
    {
        fprintf(stderr, "Error: failed to create pipe.\n");
        return EXIT_FAILURE;
    }

    pid_t pfind_pid;
    if ((pfind_pid = fork()) == -1)
    {
        fprintf(stderr, "Error: Failed to fork.\n");
        return EXIT_FAILURE;
    }
    if (pfind_pid == 0)
    {
        // Child process for pfind

        close(findpipe[0]);
        if ((dup2(findpipe[1], STDOUT_FILENO)) == -1)
        {
            fprintf(stderr, "Error: dup2 failed.\n");
            close(findpipe[1]);
            return EXIT_FAILURE;
        }

        close(sortpipe[0]);
        close(sortpipe[1]);

        if ((execv("pfind", argv)) < 0)
        {
            fprintf(stderr, "Error: pfind failed. %s.\n", strerror(errno));
            return EXIT_FAILURE;
        }
    }

    // Parent Process
    pid_t sort_pid;
    if ((sort_pid = fork()) == -1)
    {
        fprintf(stderr, "Error: failed to fork.\n");
        return EXIT_FAILURE;
    }
    if (sort_pid == 0)
    {
        // Child process for sort
        if ((dup2(findpipe[0], STDIN_FILENO)) == -1)
        {
            fprintf(stderr, "Error: dup2 failed.\n");
            close(findpipe[0]);
            close(sortpipe[1]);
            return EXIT_FAILURE;
        }

        close(findpipe[1]);
        close(sortpipe[0]);

        if ((dup2(sortpipe[1], STDOUT_FILENO)) == -1)
        {
            fprintf(stderr, "Error: dup2 failed.\n");
            close(findpipe[0]);
            close(sortpipe[1]);
            return EXIT_FAILURE;
        }

        if (execlp("sort", "sort", NULL) == -1)
        {
            fprintf(stderr, "Error: sort failed.\n");
            return EXIT_FAILURE;
        }
    }
    // Parent Process
    close(findpipe[0]);
    close(findpipe[1]);

    if (dup2(sortpipe[0], STDIN_FILENO) == -1)
    {
        fprintf(stderr, "Error: sort failed.\n");
        close(sortpipe[0]);
        return EXIT_FAILURE;
    }

    close(sortpipe[1]);

    char buf[4096];
    int num_lines = 0;
    int bytes_read;

    while ((bytes_read = read(STDIN_FILENO, buf, sizeof(buf))) > 0)
    {
        if (bytes_read == -1)
        {
            fprintf(stderr, "Error: failed to read.\n");
            return EXIT_FAILURE;
        }
        if ((write(STDOUT_FILENO, buf, bytes_read)) < 0)
        {
            fprintf(stderr, "Error: failed to write.\n");
            return EXIT_FAILURE;
        }
        for (int i = 0; i < bytes_read; i++)
        {
            if (buf[i] == '\n')
            {
                num_lines++;
            }
        }
    }
    close(sortpipe[0]);
    if ((write(STDOUT_FILENO, buf, bytes_read)) < 0)
    {
        fprintf(stderr, "Error: failed to write.\n");
        return EXIT_FAILURE;
    }

    int pfind_status, sort_status;
    if ((waitpid(pfind_pid, &pfind_status, 0)) < 0)
    {
        fprintf(stderr, "Error: wait failed.\n");
        return EXIT_FAILURE;
    }
    if ((waitpid(sort_pid, &sort_status, 0)) < 0)
    {
        fprintf(stderr, "Error: wait failed.\n");
        return EXIT_FAILURE;
    }
    if ((WEXITSTATUS(pfind_status) == EXIT_SUCCESS) && (WEXITSTATUS(sort_status) == EXIT_SUCCESS) && WIFEXITED(pfind_status) && WIFEXITED(sort_status))
    {
        printf("Total matches: %d\n", num_lines);
    }
    return EXIT_SUCCESS;
}