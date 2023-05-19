/*
Rohan Kallur
I pledge my honor that I have abided by the Stevens Honor System.
*/

#include "stdiox.h"

int int_to_str(int x, char *str)
{
    int digit = 0;
    int neg = 0;
    // negative case
    if (x < 0)
    {
        neg = 1;
        x = -x;
    }
    // Convert the digit of x to a character
    while (x > 0)
    {
        str[digit++] = x % 10 + '0';
        x /= 10;
    }
    // If the number is negative add a -
    if (neg)
    {
        str[digit++] = '-';
    }
    str[digit] = '\0';
    int ret_val = digit + 1;
    int len = digit;
    for (int i = 0; i < len / 2; i++)
    {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
    // return the of the string for printing
    return ret_val;
}

int fprintfx(char *filename, char format, void *data)
{
    // Check if the data is NULL
    if (data == NULL)
    {
        errno = EIO;
        return -1;
    }

    int x;
    int len;
    char str[2048];
    char newline = '\n';
    int fd;
    /* Check if the format is int and then convert the int to a char[]
    then  store it in str using helper function */
    if (format == 'd')
    {
        x = *(int *)data;
        len = int_to_str(x, str);
    }
    /* if the format is string then simply store the data as a char[] and find the len */
    else if (format == 's')
    {
        len = strlen(data);
        // char str[len + 1];
        strcpy(str, data);
        str[len + 1] = '\0';
    }
    else
    {
        errno = EIO;
        return -1;
    }
    // Check if the filename is empty
    if (strcmp(filename, "") == 0)
    {
        fd = 1;
    }
    // Otherwise open and use tags to append or create the file
    else
    {
        fd = open(filename, O_CREAT | O_APPEND | O_WRONLY, 0640);
    }
    // write to the destination
    write(fd, &str, len);
    if (str[len - 1] != '\n')
    {
        write(fd, &newline, sizeof(newline));
    }

    return 0;
}

int fscanfx(char *filename, char format, void *dst)
{
    // make sure dst is not NULL
    if (dst == NULL)
    {
        return 0;
    }
    int fd;
    // if filename is empty sets fd to 0
    if (strcmp(filename, "") == 0)
    {
        fd = 0;
    }
    // check if the file is currently opened and set the fd by opening it if its not open already
    else
    {
        DIR *dp;
        dp = opendir("/proc/self/fd");
        int found = 0;
        struct dirent *current;
        struct stat sb;
        char file[PATH_MAX + 1];
        stat(realpath(filename, file), &sb);
        while ((current = readdir(dp)) != NULL)
        {
            struct stat fb;
            int x = atoi(current->d_name);
            fstat(x, &fb);
            if (sb.st_ino == fb.st_ino)
            {
                fd = x;
                found++;
                break;
            }
        }
        closedir(dp);
        if (found == 0)
        {
            fd = open(filename, O_RDONLY);
            if (fd == -1)
            {
                errno = ENOENT;
                return -1;
            }
        }
    }
    // malloc an array of 128 bytes
    char *array = (char *)malloc(128 * sizeof(char)); // allocate memory for read()
    int total_bytes = 0;
    // read from the file
    int bytes_read = read(fd, array + total_bytes, 1);
    total_bytes += bytes_read;
    // continue to read from the file until you reach the end of the line or the file
    while (array[total_bytes - 1] != '\n')
    {
        if (bytes_read == -1)
        {
            free(array);
            errno = EIO;
            return -1;
        }
        if (bytes_read == 0)
        {
            free(array);
            return -1;
        }
        if (total_bytes % 128 == 0)
        {
            array = (char *)realloc(array, 128);
        }
        bytes_read = read(fd, array + total_bytes, 1);
        total_bytes += bytes_read;
    }
    array[total_bytes - 1] = '\0';
    if (format == 'd') // store into dst as an integer
    {
        *(int *)dst = (atoi(array));
    }
    else if (format == 's') // store into dst as a string
    {
        memcpy(dst, array, total_bytes);
    }
    // check if the format parameter is valid
    else
    {
        free(array);
        errno = EIO;
        return -1;
    }
    free(array);
    return 0;
}

int clean()
{
    // parse through the files of /proc/self/fd and close the correct ones.
    DIR *dp;
    dp = opendir("/proc/self/fd");
    struct dirent *current;
    int fd;
    while ((current = readdir(dp)) != NULL)
    {
        fd = atoi(current->d_name);
        if (fd > 2 && (strcmp(current->d_name, ".") != 0) && (strcmp(current->d_name, "..")) != 0)
        {
            if (close(fd) == -1)
            {
                closedir(dp);
                errno = EIO;
                return -1;
            }
        }
    }
    closedir(dp);
    return 0;
}
