#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[])
{
    char *p[MAXARG];
    int i;

    if (argc < 2)
    {
        fprintf(2, "Usage: %s <command> [args...]\n", argv[0]);
        exit(1);
    }

    for (i = 1; i < argc; i++)
    {
        p[i - 1] = argv[i];
    }

    char buffer[512]; // 缓冲区用于存储从标准输入读取的行

    while (gets(buffer, sizeof(buffer)))
    {
        if (buffer[0] == 0) // 空行跳过
            continue;

        // 去掉行末的换行符
        if (buffer[strlen(buffer) - 1] == '\n')
            buffer[strlen(buffer) - 1] = 0;

        // 将输入行添加到参数数组
        p[argc - 1] = buffer;
        p[argc] = 0; // 确保参数数组以 NULL 结尾

        int pid = fork();
        if (pid < 0)
        {
            fprintf(2, "fork failed\n");
            exit(1);
        }
        else if (pid == 0)
        {
            exec(argv[1], p);
            // exec 如果成功，下面的代码不会执行
            fprintf(2, "exec %s failed\n", argv[1]);
            exit(1);
        }
        else
        {
            wait(0); // 父进程等待子进程结束
        }
    }

    exit(0);
}
