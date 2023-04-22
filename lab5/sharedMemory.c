#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main() {

  char foo[] = "This is some text from proc-1\n";

  printf("pid/addr/len: %d  %lx  %lu\n", getpid(), (long unsigned int) foo, strlen(foo)+1);

  char command[50];
  printf("break\texit\nshow\tprint variable\n");
  while(1){
    fgets(command, sizeof(command), stdin);

    if (!strcmp(command, "break\n")){
        break;
    }

    if (!strcmp(command, "show\n")){
        printf("%s", foo);
    }
  }
}