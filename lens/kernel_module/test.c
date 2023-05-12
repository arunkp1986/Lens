#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc , char* argv[]){
    int fd = open("/dev/mychardev-0",O_RDWR);
    if(fd < 0) { perror("open:");}
    char pid[4] = "1234";
    if(write(fd, pid, sizeof(int)) < 0) {perror("write: ");}
    close(fd);
    return 0;
}
