#define MSGSZR sizeof(int)
#define MSGSZQ sizeof(int)

struct msg_q
{
    long int mtype;
    char mtext[MSGSZQ];
} typedef msg_q;

struct msg_r
{
    long int mtype;
    char mtext[MSGSZR];
} typedef msg_r;

key_t key = 1234;

int create_q_id(){
    int q_id = msgget(key, 0666 | IPC_CREAT | IPC_EXCL);
    if (q_id == -1){
        perror("msgget create\n");
        exit(1);
    }
}

int get_q_id(){

    int q_id = msgget(key, 0666);
    return q_id;
}