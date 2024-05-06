#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
/* 
    前置知识：
        目录是一种特殊的文件，可以使用open和read系统调用，其中open返回一个文件描述符
    具体流程：
    1. 检查find传入的参数是否包含了查找目录和查找文件
    2. 使用open打开指定的查找目录，检查目录路径是否合法
    3. 如果合法，每次使用read读一个该目录下的条目，如果是目录，递归查找，否则确认其是否和指定文件同名
*/
void find(char* dir, char* file){
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;
    fd = open(dir, 0);
    if(fd < 0){ // 如果路径无效
        printf("dirName is not valid");
        return;
    }
    // 获取文件描述符对应路径的文件信息，获取成功返回0，失败返回-1
    if(fstat(fd, &st) < 0){
        printf("get fd state failed");
        // 记得关闭文件描述符
        close(fd);
        return;
    }
    // 如果这个路径不是目录
    if(st.type != T_DIR){
        printf("This is not a valid dir");
        close(fd);
    }
    // 如果路径过长放不进去缓冲区
    if(strlen(dir) + 1 + DIRSIZ + 1 > sizeof(buf)){
        printf("dir is too long");
        close(fd);
        return;
    }
    // 将dir指向的路径复制到buf
    strcpy(buf, dir);
    p = buf + strlen(buf);
    *p++ = '/';

    // xv6中，目录也是一个特殊的文件，文件中保存着一个个 dirent 结构的条目，每个条目代表这个目录下的子目录或者文件
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
        // 如果inum==0，说明这个目录无效或者被删除
        if(de.inum == 0){
            continue;
        }
        // 跳过当前目录和上层目录
        if(strcmp(".", de.name) == 0 || strcmp("..", de.name) == 0){
            continue;
        }
        // 将目录或者文件名复制到p指向的buf中，组成一个完整的文件路径
        memmove(p, de.name, DIRSIZ);
        // 在末尾处添加结束符，注意这里是指从p指向的位置向后遍历DIRSIZ个元素后再添加0，
        // 为什么可以这么做，注意memmove函数并没有直接操作p指针，而是重新定义了一个指针，所以p没变过
        p[DIRSIZ] = 0;

        // 用系统调用stat检查组合好的文件路径
        if(stat(buf, &st) < 0){
            printf("cannot get state of this file");
            continue;
        }
        if(st.type == T_DIR){
            find(buf, file);
        }else if(st.type == T_FILE){
            if(strcmp(de.name, file) == 0){
                printf("%s\n", buf);
            }
        }

    }
}

int main(int argc, char** argv){
    if(argc != 3){
        printf("usage: dirName fileName");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}