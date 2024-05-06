#include "kernel/types.h"
#include "user/user.h"
#include "stddef.h"

// 以下函数用于将自定义的文件描述符复制到标准输入输出文件描述符上，以节省文件描述符的使用
void mapping(int std_fd, int custom_fd[]){
    close(std_fd); // 首先释放标准输入/输出文件描述符
    dup(custom_fd[std_fd]); // 创建一个新的文件描述符，使它和custom_fd指向同一个资源
    close(custom_fd[0]); // 同时关闭写入端和读取端，主要是因为read函数在管道没有数据时，如果写入端已经关闭，会直接返回0，否则一直阻塞
    close(custom_fd[1]);
}

void run(){
    int first_num = 0;
    int ret = read(0, &first_num, sizeof(int)); // 从管道中读取第一个数
    if(ret){ // 如果从管道中读到了数
        printf("prime %d\n", first_num); // 第一个数一定是一个素数，直接打印
        int fd[2];
        pipe(fd); // 创建一个管道，用于将筛选后的素数放入管道，供下一个进程访问
        int pid = fork();
        if(pid == 0){ // 子进程继续往管道中写
            mapping(1, fd); // 将管道写入端和标准输出fd绑定
            int next = 0;
            while(read(0, &next, sizeof(int))){ // 从旧管道中读取，写入到新管道
                if(next % first_num != 0){ // 如果是素数，写入管道
                    write(1, &next, sizeof(int));
                }
            }
            exit(0);
        }else{
            wait(NULL); // 等待子进程写完
            mapping(0, fd);
            run();
            exit(0);
        }
    }

}

int main(int argc, char** argv){
    int fd[2];
    pipe(fd); // 创建一个管道，管道对所有进程共享

    int ret = fork(); // 创建一个子进程
    if(ret == 0){ // 子进程
        // 让标准输出文件描述符和管道写入端绑定
        mapping(1, fd);
        for(int i=2; i<=35; i++){
            write(1, &i, sizeof(int));
        }
        exit(0);
    }else{ // 父进程
        wait(NULL); // 等待子进程向管道中写入数据
        // 写完后将标准输入fd和管道读取端绑定
        mapping(0, fd);
        // 然后执行素数筛选过程
        run();
        exit(0);
    }
}