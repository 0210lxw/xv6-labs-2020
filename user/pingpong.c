#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    char buff; // 用于传送的字节

    int parent_to_child[2]; // 父进程到子进程的管道
    int child_to_parent[2]; // 子进程到父进程的管道

    pipe(parent_to_child);
    pipe(child_to_parent);

    int pid = fork();
    if (pid < 0)
    {
        fprintf(2, "fork failed!");
        exit(1);
    }

    if (pid == 0)
    {
        // 子进程
        close(parent_to_child[1]); // 关闭写端
        close(child_to_parent[0]); // 关闭读端

        read(parent_to_child[0], &buff, 1); // 从父进程读取进程
        printf("%d: received ping\n", getpid());

        write(child_to_parent[1], &buff, 1); // 向父进程写入字节
        exit(0);
    }
    else
    {
        // 父进程
        close(parent_to_child[0]); // 关闭读端
        close(child_to_parent[1]); // 关闭写端

        buff = 'p';
        write(parent_to_child[1], &buff, 1); // 向父进程写入字节

        read(child_to_parent[0], &buff, 1); // 从子进程读取字节
        printf("%d: received pong\n", getpid());

        exit(0);
    }

    exit(0);
}
