#include "../include/server.h"

char* get_cur_dt(char* format) {
    time_t rawtime;
    struct tm* timeinfo = NULL;
    char* date_time = NULL;
    date_time = malloc(MAXBUF * sizeof(char));
    if (date_time == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(date_time, MAXBUF, format, timeinfo);
    return date_time;
}



void* msg_hndl(void* args){
    key_t key = 0, n = 0;
    msg_queue *q_msg = NULL;
    int msg_id = 0;
    char* cur_dt = NULL;
    key = ftok("server", 'A');
    msg_id = msgget(key, 0);
    if(msg_id < 0){
        perror("msgget error");
		pthread_exit(NULL);
    }
    while(1){
        q_msg = malloc(sizeof(msg_queue));
        n = msgrcv(msg_id, (void *)q_msg, sizeof(msg_queue), 1L, 0);
        if(n < 0){
            perror("Reading error");
			continue;
        }
        cur_dt = get_cur_dt("[%D] [%T]");

        memset(q_msg->command, 0, MAXBUF);
        strncpy(q_msg->command, cur_dt, MAXBUF);
        q_msg->msg_type = 2L;

        msgsnd(msg_id, (void*) q_msg, sizeof(msg_queue), 0);
    }
}

void m_recv( int sd ) {
    int len, n, flags, i = 0;
    pthread_t tids[5];
    char bufin[MAXBUF];
    struct sockaddr_in remote;
    msg_queue *q_msg, rcv_msg;
	key_t key;
    int msg_id;

    key = ftok("server", 'A');
    msg_id = msgget(key, 0666 | IPC_CREAT);
    if (msg_id == -1) {
        perror("msgget error");
        exit(EXIT_FAILURE);
    }

    len = sizeof(struct sockaddr);

    flags = fcntl(sd, F_GETFL, 0);
    fcntl(sd, F_SETFL, flags | O_NONBLOCK);
    for(i=0; i < 1; ++i){
        pthread_create(&tids[i], 0, msg_hndl, NULL);
    }
    usleep(10);
    while (1) {
        n=recvfrom(sd, bufin, MAXBUF, 0, (struct sockaddr *)&remote, &len);

        if (n<0) {
            /*perror("Error receiving data");*/
        }
        else {
            q_msg = malloc(sizeof(msg_queue));
            memcpy(&(q_msg->addr), &remote, sizeof(struct sockaddr_in));
            strncpy(q_msg->command, bufin, MAXBUF);
            q_msg->msg_type = 1L;
            q_msg->len = len;
            msgsnd(msg_id, (void*) q_msg, sizeof(msg_queue), 0);

            printf("Got a datagram %s from %s port %d\n", bufin,
                    inet_ntoa(remote.sin_addr), ntohs(remote.sin_port));

            printf("In queue added %d bytes\n",n);
        }

        n = msgrcv(msg_id, (void *)&rcv_msg, sizeof(msg_queue),
													2L, IPC_NOWAIT);

        if(n < 0){
            /*perror("Error reading queue");*/
        }
        else{
            n = sendto(sd, rcv_msg.command, strlen(rcv_msg.command), 0,
                              (struct sockaddr *)&q_msg->addr, q_msg->len);
            if(n < 0){
                perror("Error sending to client");
            }
        }
	usleep(1000);
    }
}

int main() {
    int ld;
    struct sockaddr_in skaddr;
    int length;

    if ((ld = socket( AF_INET, SOCK_DGRAM, 0 )) < 0) {
        printf("Problem creating socket\n");
        exit(1);
    }

    skaddr.sin_family = AF_INET;
    skaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    skaddr.sin_port = htons(7777);

    if (bind(ld, (struct sockaddr *) &skaddr, sizeof(skaddr))<0) {
        printf("Problem binding\n");
        exit(0);
    }

    length = sizeof( skaddr );
    if (getsockname(ld, (struct sockaddr *) &skaddr, &length)<0) {
        printf("Error getsockname\n");
        exit(1);
    }

    printf("The server UDP port number is %d\n",ntohs(skaddr.sin_port));

    m_recv(ld);
    return(0);
}
