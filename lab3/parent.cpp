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

using namespace std;

#define RED_COLOR "\x1b[31m"
#define GREEN_COLOR "\x1b[32m"
#define RESET_COLOR "\x1b[0m"

#define MAX_PATH_SIZE 64

int create_process()
{
    pid_t pid = fork();
    if (-1 == pid)
    {
        perror("fork");
        exit(-1);
    }
    return pid;
}

int main()
{

    char str_file_path[MAX_PATH_SIZE];
    cout << "Enter file path:" << endl;
    cin >> str_file_path;

    int input_file = open(str_file_path, O_RDONLY);
    if (input_file == -1)
    {
        perror("Can't open file");
        exit(-1);
    }

    const string shared_file = "shared_file";
    const string sem1 = "sem1";
    const string sem2 = "sem2";

    sem_t *sme = sem_open(sem1.c_str(), O_CREAT, 0644, 0);
    sem_t *she = sem_open(sem2.c_str(), O_CREAT, 0644, 0);
    if (sme == SEM_FAILED)
    {
        perror("Failed to open semaphore");
        exit(-1);
    }
    if (she == SEM_FAILED)
    {
        perror("Failed to open semaphore");
        exit(-1);
    }

    int fd = shm_open(shared_file.c_str(), O_RDWR | O_CREAT, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);

    if (fd < 0)
    {
        perror("Failed to open file");
        exit(-1);
    }

    ftruncate(fd, sizeof(int));

    int *mmap_pipe = static_cast<int *>(mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
    if (mmap_pipe == MAP_FAILED)
    {
        perror("Mapping Failed\n");
        exit(-1);
    }

    pid_t pid = create_process();

    if (pid == 0)
    {
        if (dup2(input_file, STDIN_FILENO) == -1)
        {
            perror("dup2 can't redirect stdin to input_file");
            exit(-1);
        }

        if (execl("./child", "./child", sem2.c_str(), sem1.c_str(), shared_file.c_str(), NULL) == -1)
        {
            perror("can't exec child process");
            exit(-1);
        }
    }
    else
    {

        close(input_file);

        while (mmap_pipe[0] == -1)
        {
            sem_post(she);
            cout << GREEN_COLOR << "Waiting for child" << endl;
            mmap_pipe[0] = -1;
            sem_wait(sme);
            cout << GREEN_COLOR << mmap_pipe[0] << endl;
        }
    }

    sem_close(sme);
    sem_close(she);

    shm_unlink(shared_file.c_str());
    close(fd);

    munmap(mmap_pipe, sizeof(int));
    close(input_file);
}
