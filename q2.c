//Including all the libraries that would be needed
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

//declaring a global variable buffersize(size of chunk/buffer)
#define buffersize 100

int main(int argc, char **argv)
{
    float progress;
    int m, n, o, p, q, d, f, slash, s_count = 0;
    char prin[100], buf_ter[1000];

    //finding the decimal values of indices which were taken as string inputs
    o = strlen(argv[2]);
    m = 0;
    q = 1;
    for (int i = o - 1; i > -1; i--)
    {
        p = argv[2][i] - '0';
        m += p * q;
        q *= 10;
    }
    o = strlen(argv[3]);
    n = 0;
    q = 1;
    for (int i = o - 1; i > -1; i--)
    {
        p = argv[3][i] - '0';
        n += p * q;
        q *= 10;
    }

    //opening the given file with required flags,
    //finding the corresponding file descriptor
    int input_fd = open(argv[1], O_RDONLY);
    if (input_fd < 0)
    {
        perror("Problem in opening/accesing given file and performing operations on it!!");
        exit(1);
    }

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
    s[13] = '2';
    s[14] = '_';
    for (int i = slash + 1, j = 0; j < s_count; i++, j++)
    {
        s[j + 15] = argv[1][i];
    }

    //creating and opening the file with required flags in a directory Assignment,
    //finding the corresponding file descriptor
    mkdir("Assignment", 0777);
    int output_fd = open(s, O_CREAT | O_RDWR | O_TRUNC, 0777);
    if (output_fd < 0)
    {
        perror("Problem in creating/opening new file and performing operations on it!!");
        exit(1);
    }

    //finding size of the given file,size of file before first index,
    //size of file between indices and size of size of file after second index
    //And also finding if problem in the process of positioning cursor at required position
    off_t size = lseek(input_fd, 0, SEEK_END);
    off_t size2 = lseek(input_fd, -(n + 1), SEEK_END);
    off_t size3 = lseek(input_fd, -m, SEEK_END);
    if (size < 0 || size2 < 0 || size3 < 0)
    {
        perror("Problem in positioning cursor at end/making required offset!");
        exit(1);
    }
    off_t size1 = size - size3;
    size3 -= size2;

    //finding number of chunks and size of rest in part of the file before first index(first part)
    d = size1 / buffersize;
    f = size1 % buffersize;

    //positioning the cursor of output file to beginning of output file
    lseek(output_fd, 0, SEEK_SET);
    char buffer[buffersize], writer[buffersize];
    long long x = 0;

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
        sprintf(buf_ter, "Progress of file written: %f\r", progress);
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
        sprintf(buf_ter, "Progress of file written: %f\r", progress);
        write(1, buf_ter, strlen(buf_ter));
    }

    //positioning cursor at first index in output file
    lseek(output_fd, m, SEEK_SET);

    //finding number of chunks and size of rest in part of the file before first index(second  part)
    d = size3 / buffersize;
    f = size3 % buffersize;
    char buf[buffersize];

    //if size of rest is greater than zero,
    //first writing the last f size data to output file(in required way)
    //positioning cursor of input file to a position required to read last f size part of it
    if (f > 0)
        lseek(input_fd, m + buffersize * d, SEEK_SET);

    //Reading and writing the left part to output file
    read(input_fd, buf, f);
    write(output_fd, buf, f);
    fflush(stdout); //to flush out the contents of buffer

    //calculating and printing the progress of process
    x += f;
    progress = x / size;
    progress *= f;
    sprintf(buf_ter, "Progress of file written: %f\r", progress);
    write(1, buf_ter, strlen(buf_ter));
    //loop for performing operation chunk by chunk
    for (int i = 0; i < d; i++)
    {
        //positioning cursor of input file to a position required to read (d-i)th chunk and write it to output file
        lseek(input_fd, m + ((d - 1 - i) * buffersize), SEEK_SET);
        read(input_fd, buf, buffersize);
        write(output_fd, buf, buffersize);
        fflush(stdout); //to flush out the contents of buffer

        //calculating and printing the progress of process
        x += buffersize;
        progress = x / size;
        progress *= buffersize;
        sprintf(buf_ter, "Progress of file written: %f\r", progress);
        write(1, buf_ter, strlen(buf_ter));
    }

    //finding number of chunks and size of rest in part of the file before first index(third part)
    d = size2 / buffersize;
    f = size2 % buffersize;
    //positioning the cursor of output file to second index of output file
    lseek(output_fd, n + 1, SEEK_SET);

    //if size of rest is greater than zero,
    //first writing the last f size data to output file(in required way)
    if (f > 0)
    {
        //positioning cursor of input file to a position required to read last f size part of it
        lseek(input_fd, n + 1 + (buffersize * d), SEEK_SET);
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
        sprintf(buf_ter, "Progress of file written: %f\r", progress);
        write(1, buf_ter, strlen(buf_ter));
    }

    //loop for performing operation chunk by chunk
    for (int i = 0; i < d; i++)
    {
        //positioning cursor of input file to a position required to read (d-i)th chunk
        lseek(input_fd, n + 1 + ((d - 1 - i) * buffersize), SEEK_SET);
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
        sprintf(buf_ter, "Progress of file written: %f\r", progress);
        write(1, buf_ter, strlen(buf_ter));
    }
    sprintf(buf_ter, "\n");
    write(1, buf_ter, strlen(buf_ter));
    //closing input and output files
    close(input_fd);
    close(output_fd);
    return 0;
}
