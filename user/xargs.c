#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"
#include "kernel/param.h"

#define MAXN 1024

int main(int argc, char** argv){
    if(argc < 2){
        printf("There is no cmd");
        exit(1);
    }
    // max exec arguments,存放子进程exec的参数
    char* argvs[MAXARG];
    
    // 将xargs的命令行参数保存下来
    int index = 0;
    for(int i=1; i<argc; i++){
        argvs[index++] = argv[i];
    }
    // 存放从管道中读出的数据
    char buf[MAXN] = "\0";

    // 标准输入文件描述符已经被重定向到了管道的读取端，所以是0
    // 所谓重定向，指的是close(0)或close(1),然后创建一个管道，管道返回的文件描述符为最小的未分配的非负整数，所以肯定是0或1
    int n = 0;
    while((n = read(0, buf, MAXN)) > 0){
        char temp[MAXN] = "\0";
        // 将读取的参数追加到xargs命令参数的后面
        argvs[index++] = temp;

        for(int i=0; i<strlen(buf); i++){
            if(buf[i] == '\n'){
                if(fork() == 0){
                    // argv[1]是希望使用exec执行的程序名或路径，argvs是该程序需要的参数
                    exec(argv[1], argvs);
                }else{
                    wait(0);
                }
            }else{
                temp[i] = buf[i];
            }
        }
    }
    exit(0);
}