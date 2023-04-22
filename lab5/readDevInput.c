#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <stdlib.h>
#include <libevdev/libevdev.h>

int main(){
    char path[] = "/dev/input/event2";

    int fd = open(path, O_RDONLY);
    if (fd == -1){
        perror("Cannot open file");
        exit(1);     
    }

    struct input_event ev;
    while(1){
        if (read(fd, &ev, sizeof(ev)) != 0 && ev.type == 1 && ev.value == 1){
            printf("%s\n", libevdev_event_code_get_name(ev.type, ev.code));
        }
    }

    if(close(fd) == -1){
        perror("Cannot close file");
    }
    
}