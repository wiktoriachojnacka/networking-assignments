#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/msg.h>

int msqid;
           struct message {
               long type;
               char text[50];
           } msg;


void signalHandler(int sig){
	printf("server stop\n");
	msgctl(msqid, IPC_RMID, NULL);
	exit(0);
}

int main(){
	signal(SIGINT, signalHandler);
	msqid = msgget((key_t)69, 0600|IPC_CREAT);
	if(msqid == -1){
		perror("msgget");
	}
	while(1){
		if(msgrcv(msqid, (void *) &msg, sizeof(msg.text), 0, 0) == -1){
			perror("msgrcv");
		}
		printf("from client: %s\n",msg.text);
	}
}