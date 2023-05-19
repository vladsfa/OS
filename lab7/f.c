#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "queue.h"

/*f(x)*/
int main(){
    int q_id = get_q_id();

    msg_q query;
    if (msgrcv(q_id, &query, MSGSZQ, 1, 0) < 0){
        perror("f(x) receive\n");
        
        exit(1);
    }
    printf("f(x): %s\n", query.mtext);

    msg_r response;
    response.mtype = 3;
    strcpy(response.mtext, query.mtext);
    if (msgsnd(q_id, &response, MSGSZR, IPC_NOWAIT) < 0){
        perror("f(x) send\n");
        exit(1);
    };

    return 0;
}