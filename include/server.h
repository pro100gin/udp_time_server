#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <stdio.h>      /* standard C i/o facilities */
#include <stdlib.h>     /* needed for atoi() */
#include <unistd.h>     /* defines STDIN_FILENO, system calls,etc */
#include <sys/types.h>  /* system data type definitions */
#include <sys/socket.h> /* socket specific definitions */
#include <netinet/in.h> /* INET constants and stuff */
#include <arpa/inet.h>  /* IP address conversion stuff */
#include <netdb.h>      /* gethostbyname */
#include <time.h>
#include <pthread.h>
#include <string.h>

#define MAXBUF 40


typedef struct MQ msg_queue;
struct MQ{
    long msg_type;
    struct sockaddr_in addr;
    int len;
    char command[MAXBUF];
};


char* get_cur_dt(char* );

void* msg_hndl(void* );

void m_recv(int );
