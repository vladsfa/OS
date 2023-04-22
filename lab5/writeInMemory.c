#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {

  if (argc != 4) {
    printf("argv: pid addr len\n");
    exit(1);
  }

  int           pid  = strtol (argv[1], NULL, 10);
  unsigned long addr = strtoul(argv[2], NULL, 16);
  int           len  = strtol (argv[3], NULL, 10);
  printf("input pid/addr/len: %d  %lx  %d\n", pid, addr, len);

  char proc_mem[50];
  sprintf(proc_mem, "/proc/%d/mem", pid);

  int fd = open(proc_mem, O_WRONLY);
  if (fd == -1) {
    printf("Could not open %s\n", proc_mem);
    exit(1);
  }

  char buf[len];
  printf("break\texit\n`any string`\twrite to variable\n");
  while(1){
    fgets(buf, len, stdin);
    if (!strcmp(buf, "break\n")){
        break;
    }
    else{
        lseek(fd, addr, SEEK_SET);
        if (write(fd, buf, len) == -1) {
            printf("Error while writing\n");
            exit(1);
        }
        else{
          printf("success\n");
        }
    }
  }

  if (close(fd) == -1){
    printf("Cannot close fd");
  }
}