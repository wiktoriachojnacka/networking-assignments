#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <string.h>

struct mymsg {
               long   type;       /* Message type. */
               char   text[50];    /* Message text. */
           }msg;


int main(int argc, char * argv[]){
	int msqid = msgget((key_t)69, 0600|IPC_CREAT);
	if(msqid == -1){
		perror("msgget");
	}
	msg.type = 1;
	strcpy(msg.text, argv[1]);
	if(msgsnd(msqid, (void *)&msg, sizeof(msg.text), 0) == -1){
		perror("msgsnd");
	}
	return 0;
}