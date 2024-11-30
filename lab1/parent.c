#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main()
{
    char filename[256];

    printf("Enter filename: ");
    scanf("%255s", filename);

    int file_fd = open(filename, O_RDONLY);

    int pipefd[2];
    pipe(pipefd);

    pid_t pid = fork();
    if (pid == 0)
    {
        dup2(file_fd, STDIN_FILENO);

        dup2(pipefd[1], STDOUT_FILENO);

        close(file_fd);
        close(pipefd[0]);
        close(pipefd[1]);

        execlp("./child", "child", (char *)NULL);

        perror("execlp");
        exit(EXIT_FAILURE);
    }
    else
    {
        close(file_fd);
        close(pipefd[1]);

        char buffer[1024];
        ssize_t bytes_read;
        while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer))) > 0)
        {
            if (write(STDOUT_FILENO, buffer, bytes_read) != bytes_read)
            {
                perror("write");
                break;
            }
        }

        if (bytes_read == -1)
        {
            perror("read");
        }

        close(pipefd[0]);

        int status;
        waitpid(pid, &status, 0);
    }

    return 0;
}