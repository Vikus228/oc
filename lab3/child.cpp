#include <stdlib.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <string.h>

#define RED_COLOR "\x1b[31m"
#define GREEN_COLOR "\x1b[32m"
#define RESET_COLOR "\x1b[0m"

using namespace std;

bool IsPrime(int x)
{
    if (x < 0)
    {
        return true;
    }
    else if (x == 0 or x == 1)
    {
        return false;
    }

    for (int i = 2; i * i <= x; ++i)
    {
        if (x % i == 0)
        {
            return false;
        }
    }

    return true;
}

int main(int argc, char **argv)
{

    sem_t *sme = sem_open(argv[1], 0);
    sem_t *she = sem_open(argv[2], 0);

    int fd = shm_open(argv[3], O_RDWR, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);

    if (fd < 0)
    {
        perror("Failed to open file");
        exit(-1);
    }

    int *mmap_pipe = static_cast<int *>(mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));

    if (mmap_pipe == MAP_FAILED)
    {
        perror("Mapping Failed\n");
        exit(-1);
    }

    int tmp;
    bool is_EOF = true;

    while (scanf("%d", &tmp) != EOF)
    {
        cout << RED_COLOR << tmp << endl;
        cout << RED_COLOR << "Waiting for parent" << endl;
        sem_wait(sme);

        if (!IsPrime(tmp))
        {
            mmap_pipe[0] = tmp;
            cout << RED_COLOR << "Doing child's stuff " << mmap_pipe[0] << endl;
            sem_post(she);
        }
        else
        {
            mmap_pipe[0] = -1;
            is_EOF = false;
            sem_post(she);
            break;
        }
    }

    if (is_EOF)
    {
        mmap_pipe[0] = -1;
        sem_post(she);
    }

    sem_close(sme);
    sem_close(she);
}
