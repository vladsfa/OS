#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "queue.h"

#define STDIN_LEN 30
#define ALARM_S 3

int is_stop = 0;

void alarm_handler(int signum){
    if (signum == SIGALRM){
        char answer_str[sizeof(int)];
        printf("The waiting time has expired, maybe the calculation is stuck, wait some more?\n1. Continue the calculation\n2. Stop\n3. Continue without further questioning\n(1/2/3)? ");
        while(1){
            fgets(answer_str, sizeof(int), stdin);

            int answer = atoi(answer_str);

            if (answer == 1){
                alarm(3);
                break;
            }
            else if(answer == 2){
                is_stop = 1;
                break;
            }
            else if(answer == 3){
                break;
            }
        }
    }
}

void msg_remove(int q_id){
    if (msgctl(q_id, IPC_RMID, NULL) == -1) {
        perror("msgctl (IPC_RMID)");
        exit(1);
    }
}

void msg_query(int q_id, long int type, int msg){
    msg_q query;
    query.mtype = type;
    sprintf(query.mtext, "%d", msg);

    if (msgsnd(q_id, &query, MSGSZQ, IPC_NOWAIT) < 0){
        perror("Query fail");
        exit(1);
    }
}

void msg_response(int q_id, msg_r* buffer){
    if (msgrcv(q_id, buffer, MSGSZR, 3, 0) < 0){
        if (errno != EINTR){
            perror("Response fail");
            exit(1);
        }
    }
}

int msg_eval(int q_id, int type_to, int msg){
    int res = -1;

    msg_query(q_id, type_to, msg);
    alarm(ALARM_S);
    while (!is_stop)
    {
        msg_r response;
        msg_response(q_id, &response);
        if (errno == 0){
            alarm(0);
            is_stop = 1;
        }
        errno = 0;

        res = atoi(response.mtext);
    }
    is_stop = 0;

    return res;
}

int msg_f(int q_id, int msg){
    return msg_eval(q_id, 1, msg);
}

int msg_g(int q_id, int msg){
    return msg_eval(q_id, 2, msg);
}

int read_from_stdin(const char* msg){
    char buf[STDIN_LEN];
    printf("%s ", msg);
    fgets(buf, sizeof(buf), stdin);
    return atoi(buf);
}

int main(){
    signal(SIGALRM, alarm_handler);

    int q_id = get_q_id();
    if (q_id < 0){
        if (errno == ENOENT){
            q_id = create_q_id();
        }
        else{
            perror("main get_q_id");
        }
    }

    if (msg_f(q_id, read_from_stdin("f input:")) || msg_g(q_id, read_from_stdin("g input:"))){
        printf("Result: true\n");
    }
    else{
        printf("Result: false\n");
    }

    msg_remove(q_id);
}