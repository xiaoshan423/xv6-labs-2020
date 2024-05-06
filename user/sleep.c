#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// argc 表示传入参数的数量，argv是一个参数数组，第一个参数永远是当前程序可执行文件的全名
int main(int argc, char** argv){
    if(argc != 2){
        // 关联标准错误文件描述符2
        fprintf(2, "usage: sleep [ticks num]\n");
        // fprintf(1, "path = %s\n", argv[0]);
        exit(1);
    }
    int ticks = atoi(argv[1]);
    int ret = sleep(ticks);
    exit(ret);
}