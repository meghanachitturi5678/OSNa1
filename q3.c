//Including all the libraries that would be needed
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>

//declaring a global variable buffersize(size of chunk/buffer)
#define buffersize 100

//function to find array of digits of octsl number for a decimal number
void dec_oct(int c, int octa[6])
{
    int q, r, i = 0;
    q = c;

    //Storing remainders until number is equal to zero
    while (q != 0)
    {
        octa[i++] = q % 8;
        q = q / 8;
    }
}

int main(int argc, char **argv)
{
    struct stat st1;
    struct stat st2;
    struct stat st3;
    int x, o, e, f, d = 1, rev = 1, y[3], octa[3][6], a[3], b[3], c[3], z[3];
    char buf[buffersize], buf1[buffersize], buf2[buffersize], buf_ter[1000];

    //stat(argv[1],&st1)->stat system call tries to write info about file represented path argv[1] to st1
    //if any problem occurs,the same is displayed out for both old file and new file
    if (stat(argv[1], &st1) < 0)
    {
        perror("Problem in opening new file!");
        exit(1);
    }
    if (stat(argv[2], &st2) < 0)
    {
        perror("Problem in opening old file!");
        exit(1);
    }

    //Trying to open directory and representing corresonding info(error if it is not directory)
    int dir = open(argv[3], O_DIRECTORY);
    sprintf(buf_ter, "Directory is created: ");
    write(1, buf_ter, strlen(buf_ter));
    if (dir < 0)
    {
        d = 0;
    }
    if (d == 0)
    {
        sprintf(buf_ter, "NO\n");
        write(1, buf_ter, strlen(buf_ter));
    }
    else
    {
        sprintf(buf_ter, "YES\n");
        write(1, buf_ter, strlen(buf_ter));
    }
    if (stat(argv[3], &st3) < 0)
    {
        perror("Problem in opening directory!");
        exit(1);
    }

    //opening the files for finding corresponding file descriptors and exit in case of error
    int new_fd = open(argv[1], __O_LARGEFILE);
    int old_fd = open(argv[2], __O_LARGEFILE);

    if (new_fd < 0 || old_fd < 0)
    {
        perror("Problem in opening files!");
        exit(1);
    }
    // read(new_fd, buf1, buffersize);
    //read(old_fd, buf2, buffersize);

    //finding size of the given files (exit with error message if error occurs)
    off_t size1 = lseek(new_fd, 0, SEEK_END);
    off_t size2 = lseek(old_fd, 0, SEEK_END);
    if (size1 < 0 || size2 < 0)
    {
        perror("Problem in positioning cursor at end of files!");
        exit(1);
    }

    //Finding whether the file contents are reversed in new file

    //storing string to be printed on terminal in a buffer and the writing it to terminal
    sprintf(buf_ter, "Whether file contents are reversed in newfile: ");
    write(1, buf_ter, strlen(buf_ter));

    //check if the contents are reversed in both the files
    if (rev != 0)
    {
        //finding number of chunks and size of rest
        e = size1 / buffersize;
        f = size1 % buffersize;

        //positioning the cursor of new file to beginning of new file
        lseek(new_fd, 0, SEEK_SET);

        //if size of rest is greater than zero,
        //first checking the last f size data of old file(in required way)
        //positioning cursor of input file to a position required to read last f size part of it
        if (f > 0)
            lseek(old_fd, e * buffersize, SEEK_SET);

        //reading f sized data from both the file
        read(old_fd, buf1, f);
        read(new_fd, buf2, f);

        //verifying if any character violate the position of being in reverse order from other file
        for (int i = 0; i < f; i++)
        {
            if (buf2[i] != buf1[f - 1 - i])
            {
                sprintf(buf_ter, "NO\n");
                write(1, buf_ter, strlen(buf_ter));
                rev = 0;
                i = f;
            }
        }

        //if not,then check for remaining data in old file
        if (rev != 0)
        {
            for (int i = 0; i < e; i++)
            {
                //placing cursors at required positions in both the files,
                //reading data and checking for violation of both the files being reversed
                lseek(new_fd, f + (i * buffersize), SEEK_SET);
                lseek(old_fd, (e - i - 1) * buffersize, SEEK_SET);
                read(old_fd, buf1, buffersize);
                read(new_fd, buf2, buffersize);
                for (int j = 0; j < buffersize; j++)
                {
                    if (buf2[j] != buf1[buffersize - 1 - j])
                    {
                        sprintf(buf_ter, "NO\n");
                        write(1, buf_ter, strlen(buf_ter));
                        j = buffersize;
                        i = e;
                        rev = 0;
                    }
                }
            }
        }
    }
    if (rev != 0)
    {
        sprintf(buf_ter, "YES\n");
        write(1, buf_ter, strlen(buf_ter));
    }

    //The file's type and it's permissions are encoded together
    // in st_mode field in stat
    //of which last 3 bits(in octal) correspond to their permissions
    //Hence taking the value as decimal value and
    //converting it into octal so that the last 3 digits we obtain denote permissions
    y[0] = st1.st_mode;
    y[1] = st2.st_mode;
    y[2] = st3.st_mode;
    for (int i = 0; i < 3; i++)
    {
        dec_oct(y[i], octa[i]);
        a[i] = octa[i][0]; //Permissions for others
        b[i] = octa[i][1]; //Permissions for Group
        c[i] = octa[i][2]; //Permissions for user
    }
    char file[3][15];

    strcpy(file[0], "newfile");
    strcpy(file[1], "oldfile");
    strcpy(file[2], "directory");
    for (int i = 0; i < 3; i++)
    {
        sprintf(buf_ter, "User has read permissions on %s: ", file[i]);
        write(1, buf_ter, strlen(buf_ter));

        if (c[i] > 3)
        {
            sprintf(buf_ter, "YES\n");
        }
        else
        {
            sprintf(buf_ter, "NO\n");
        }
        write(1, buf_ter, strlen(buf_ter));
        sprintf(buf_ter, "User has write permissions on %s: ", file[i]);
        write(1, buf_ter, strlen(buf_ter));
        if (c[i] == 2 || c[i] == 3 || c[i] == 6 || c[i] == 7)
        {
            sprintf(buf_ter, "YES\n");
        }
        else
        {
            sprintf(buf_ter, "NO\n");
        }
        write(1, buf_ter, strlen(buf_ter));
        sprintf(buf_ter, "User has execute permissions on %s: ", file[i]);
        write(1, buf_ter, strlen(buf_ter));
        if (c[i] % 2 == 1)
            sprintf(buf_ter, "YES\n");
        else
            sprintf(buf_ter, "NO\n");
        write(1, buf_ter, strlen(buf_ter));
        sprintf(buf_ter, "User has read permissions on %s: ", file[i]);
        write(1, buf_ter, strlen(buf_ter));

        if (b[i] > 3)
            sprintf(buf_ter, "YES\n");
        else
            sprintf(buf_ter, "NO\n");
        write(1, buf_ter, strlen(buf_ter));

        sprintf(buf_ter, "User has write permissions on %s: ", file[i]);
        write(1, buf_ter, strlen(buf_ter));

        if (b[i] == 2 || b[i] == 3 || b[i] == 6 || b[i] == 7)
            sprintf(buf_ter, "YES\n");
        else
            sprintf(buf_ter, "NO\n");
        write(1, buf_ter, strlen(buf_ter));
        sprintf(buf_ter, "User has execute permissions on %s: ", file[i]);
        write(1, buf_ter, strlen(buf_ter));

        if (b[i] % 2 == 1)
            sprintf(buf_ter, "YES\n");
        else
            sprintf(buf_ter, "NO\n");
        write(1, buf_ter, strlen(buf_ter));

        sprintf(buf_ter, "User has read permissions on %s: ", file[i]);
        write(1, buf_ter, strlen(buf_ter));

        if (a[i] > 3)
            sprintf(buf_ter, "YES\n");
        else
            sprintf(buf_ter, "NO\n");
        write(1, buf_ter, strlen(buf_ter));

        sprintf(buf_ter, "User has write permissions on %s: ", file[i]);
        write(1, buf_ter, strlen(buf_ter));

        if (a[i] == 2 || a[i] == 3 || a[i] == 6 || a[i] == 7)
            sprintf(buf_ter, "YES\n");
        else
            sprintf(buf_ter, "NO\n");
        write(1, buf_ter, strlen(buf_ter));

        sprintf(buf_ter, "User has execute permissions on %s: ", file[i]);
        write(1, buf_ter, strlen(buf_ter));

        if (a[i] % 2 == 1)
            sprintf(buf_ter, "YES\n");
        else
            sprintf(buf_ter, "NO\n");
        write(1, buf_ter, strlen(buf_ter));
        sprintf(buf_ter, "\n");
        write(1, buf_ter, strlen(buf_ter));
    }

    return 0;
}