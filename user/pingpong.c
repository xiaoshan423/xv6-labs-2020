#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char** argv){
    int fd1[2]; // 0： 读， 1： 写
    int fd2[2];
    pipe(fd1); // 父进程向子进程通信，父进程写，子进程读
    pipe(fd2); // 子进程向父进程通信，子进程写，父进程读
    char buffer[] = "hell";
    int ret = fork();
    if(ret == 0){ // 子进程
        int pid = getpid();
        close(fd1[1]); // 释放子进程对fd1写入端的引用
        close(fd2[0]); // 释放子进程对fd2读取端的引用

        // 如果管道中没有数据，读操作会被阻塞，直到有数据或者写入端被关闭
        read(fd1[0], buffer, strlen(buffer));
        printf("%d: received %s\n", pid, buffer);
        write(fd2[1], "pong", strlen("pong"));

        close(fd1[0]); // 关闭管道
        close(fd2[1]);
        exit(0);
    }else{ // 父进程
        int pid = getpid();
        close(fd1[0]); // 释放父进程对fd1读取端的引用
        close(fd2[1]); // 释放父进程对fd2写入端的引用

        sleep(10);

        write(fd1[1], "ping", strlen(buffer));
        wait(0); // 等待子进程写完退出
        read(fd2[0], buffer, 4);
        printf("%d: received %s\n", pid, buffer);

        close(fd1[1]);
        close(fd2[0]);
        exit(0);
    }
}