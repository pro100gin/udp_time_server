#include <stdio.h>      /* standard C i/o facilities */
#include <stdlib.h>     /* needed for atoi() */
#include <unistd.h>     /* defines STDIN_FILENO, system calls,etc */
#include <sys/types.h>  /* system data type definitions */
#include <sys/socket.h> /* socket specific definitions */
#include <netinet/in.h> /* INET constants and stuff */
#include <arpa/inet.h>  /* IP address conversion stuff */
#include <netdb.h>      /* gethostbyname */
#include <string.h>

#define MAXBUF 40

int main( int argc, char **argv ) {
  int sk;
  struct sockaddr_in server;
  struct hostent *hp;
  char buf[MAXBUF];
  int buf_len;
  int n_sent;
  int n_read;


  if ((sk = socket( AF_INET, SOCK_DGRAM, 0 )) < 0)
    {
      printf("Problem creating socket\n");
      exit(1);
    }

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  server.sin_port = htons(7777);

  while(1){
      if (fgets(buf, MAXBUF, stdin) == NULL) {
          perror("Client: fgets(msg)");

          close(sk);
          exit(EXIT_FAILURE);
      }

      printf("Got %d bytes from stdin - sending...\n", strlen(buf));

      n_sent = sendto(sk, buf, strlen(buf), 0,
                      (struct sockaddr*) &server, sizeof(server));

      if (n_sent<0) {
          perror("Problem sending data");
          exit(1);
      }

      if (n_sent!=buf_len) {
          printf("Sendto sent %d bytes\n",n_sent);
      }

      n_read = recvfrom(sk,buf,MAXBUF,0,NULL,NULL);
      if (n_read<0) {
          perror("Problem in recvfrom");
          exit(1);
      }

      printf("Got back %d bytes\n",n_read);
      printf("%s\n", buf);
  }
  return(0);
}
