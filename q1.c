//Including all the libraries that would be needed
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

//declaring a global variable buffersize(size of chunk/buffer)
#define buffersize 100

int main(int argc, char **argv)
{
    float progress;
    int d, f, slash, s_count = 0;
    char buf_ter[1000];
    //making path of required output file as string
    int k = strlen(argv[1]);
    slash = k - 1;
    while (argv[1][slash] != '/')
    {
        --slash;
        ++s_count;
    }
    char s[k + 15];
    s[0] = '.';
    s[1] = '/';
    char str[10] = "Assignment";
    for (int i = 0; i < 10; i++)
        s[i + 2] = str[i];
    s[12] = '/';
    s[13] = '1';
    s[14] = '_';
    for (int i = slash + 1, j = 0; j < s_count; i++, j++)
    {
        s[j + 15] = argv[1][i];
    }

    //opening the given file with required flags and exit in case of error,
    //finding the corresponding file descriptor
    int input_fd = open(argv[1], O_RDONLY);
    if (input_fd < 0)
    {
        perror("Problem in opening/accesing given file and performing operations on it!!");
        exit(1);
    }

    //creating and opening the file with required flags in a directory Assignment and exit in case of error,
    //finding the corresponding file descriptor
    mkdir("Assignment", 0777);
    int output_fd = open(s, O_CREAT | O_RDWR | O_TRUNC, 0777);
    if (output_fd < 0)
    {
        perror("Problem in creating/opening new file and performing operations on it!!");
        exit(1);
    }

    //finding size of the given file (exit with error message if error occurs)
    off_t size = lseek(input_fd, 0, SEEK_END);
    if (size < 0)
    {
        perror("Problem in positioning cursor at end!");
        exit(1);
    }

    //finding number of chunks and size of rest
    d = size / buffersize;
    f = size % buffersize;

    //positioning the cursor of output file to beginning of output file
    lseek(output_fd, 0, SEEK_SET);
    char buffer[buffersize], writer[buffersize];
    long long x = 0;
    float y;

    //if size of rest is greater than zero,
    //first writing the last f size data to output file(in required way)
    if (f > 0)
    {
        //positioning cursor of input file to a position required to read last f size part of it
        lseek(input_fd, buffersize * d, SEEK_SET);
        read(input_fd, buffer, f);

        //loop for reversing the read string
        for (int i = 0; i < f; i++)
            writer[i] = buffer[f - 1 - i];

        fflush(stdout); //to flush out the contents of buffer

        //writing in output file
        write(output_fd, writer, f);

        //calculating and printing the progress of process
        x += f;
        progress = x;
        progress /= size;
        progress *= 100;
        sprintf(buf_ter, "Progress of reversing the file: %f\r", progress);
        write(1, buf_ter, strlen(buf_ter));
    }

    //loop for performing operation chunk by chunk
    for (int i = 0; i < d; i++)
    {
        //positioning cursor of input file to a position required to read (d-i)th chunk
        lseek(input_fd, (d - 1 - i) * buffersize, SEEK_SET);
        read(input_fd, buffer, buffersize);

        //loop for writing chunk in reverse order
        for (int j = 0; j < buffersize; j++)
            writer[j] = buffer[buffersize - 1 - j];

        fflush(stdout); //to flush out the contents of buffer

        //writing in output file
        write(output_fd, writer, buffersize);

        //calculating and printing the progress of process
        x += buffersize;
        progress = x;
        progress /= size;
        progress *= 100;
        sprintf(buf_ter, "Progress of reversing the file: %f\r", progress);
        write(1, buf_ter, strlen(buf_ter));
    }
    sprintf(buf_ter, "\n");
    write(1, buf_ter, strlen(buf_ter));

    //closing input and output files
    close(input_fd);
    close(output_fd);

    return 0;
}
