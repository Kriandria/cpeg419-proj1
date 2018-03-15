/* tcpserver.c */
/* Programmed by Adarsh Sethi */
/* February 21, 2018 */    

#include <ctype.h>          /* for toupper */
#include <stdio.h>          /* for standard I/O functions */
#include <stdlib.h>         /* for exit */
#include <string.h>         /* for memset */
#include <sys/socket.h>     /* for socket, bind, listen, accept */
#include <netinet/in.h>     /* for sockaddr_in */
#include <unistd.h>         /* for close */

#define STRING_SIZE 80 

/* SERV_TCP_PORT is the port number on which the server listens for
   incoming requests from clients. You should change this to a different
   number to prevent conflicts with others in the class. */

#define SERV_TCP_PORT 46668

struct header{
    unsigned short ihead; // integer value of sequence number
    char chead[5];        // string value of sequence number
    unsigned short ilen;  // integer value of char count
    char clen[5];         // string value of char count
    char comb[8];         // combined message of chead and clen
};

int main(int argc, char *argv[]) {

   int sock_server;  /* Socket on which server listens to clients */
   int sock_connection;  /* Socket on which server exchanges data with client */

   struct sockaddr_in server_addr;  /* Internet address structure that
                                        stores server address */
   unsigned int server_addr_len;  /* Length of server address structure */
   unsigned short server_port;  /* Port number used by server (local port) */

   struct sockaddr_in client_addr;  /* Internet address structure that
                                        stores client address */
   unsigned int client_addr_len;  /* Length of client address structure */

   char filename[STRING_SIZE];  /* receive message */
   char line[STRING_SIZE]; /* send message */
   unsigned short msg_len;  /* length of message */
   int bytes_sent, bytes_sent2, bytes_recd, bytes_recd2; /* number of bytes sent or received */
   unsigned int i;  /* temporary loop variable */
   FILE *file;  /* file requested by client */
   int sumCount = 0;    // Sum of all data bytes transmitted
   struct header head; // Initializing struct
   head.ihead = 0;
   head.ilen = 0;


   /* open a socket */

   if ((sock_server = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      perror("Server: can't open stream socket");
      exit(1);                                                
   }

   /* initialize server address information */
    
   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family = AF_INET;
   server_addr.sin_addr.s_addr = htonl (INADDR_ANY);  /* This allows choice of
                                        any host interface, if more than one
                                        are present */ 
   server_port = SERV_TCP_PORT; /* Server will listen on this port */
   server_addr.sin_port = htons(server_port);

   /* bind the socket to the local server port */

   if (bind(sock_server, (struct sockaddr *) &server_addr,
                                    sizeof (server_addr)) < 0) {
      perror("Server: can't bind to local address");
      close(sock_server);
      exit(1);
   }                     

   /* listen for incoming requests from clients */

   if (listen(sock_server, 50) < 0) {    /* 50 is the max number of pending */
      perror("Server: error on listen"); /* requests that will be queued */
      close(sock_server);
      exit(1);
   }
   printf("I am here to listen ... on port %hu\n\n", server_port);
  
   client_addr_len = sizeof (client_addr);

   /* wait for incoming connection requests in an indefinite loop */

   for (;;) {

      sock_connection = accept(sock_server, (struct sockaddr *) &client_addr, 
                                         &client_addr_len);
                     /* The accept function blocks the server until a
                        connection request comes from a client */
      if (sock_connection < 0) {
         perror("Server: accept() error\n"); 
         close(sock_server);
         exit(1);
      }
 
      /* receive the message */
        bytes_recd = recv(sock_connection, head.comb, 8, 0);   // header

        strncpy(head.chead, head.comb, 4); // 
        strcpy(head.clen, head.comb+4);    //  Deconstructs header
        head.ihead = atoi(head.chead);     //
        head.ilen = atoi(head.clen);       //

        bytes_recd2 = recv(sock_connection, filename, head.ilen, 0);  // Payload
        printf("\nPacket %d received with %d data bytes\n",\
              head.ihead, head.ilen);

      if (bytes_recd > 0){
         printf("Received filename is:\n%s", filename);
         printf("\nwith length %d\n\n", head.ilen);
/*----------------------------------------------------------------------------------*/
        /* process file */
        file = fopen(filename, "r");  // open requested file

        while (fgets(line, STRING_SIZE, (FILE*)file)!= NULL) { // while not EOF

            /* prepare message and header */
            msg_len = strlen(line);                   // count
            sumCount += msg_len;                      // update total char bytes sent
            head.ihead++;                             // increment sequence number
            sprintf(head.chead, "%04d", head.ihead);  // store Seq.# as a 4 char string
            strcpy(head.comb, head.chead);            // append to front of head.comb
            sprintf(head.clen, "%04d", msg_len);        // store count as a string
            strcat(head.comb, head.clen);             // append to end of head.comb

            /* send message and header */
            bytes_sent = send(sock_connection, head.comb, 8, 0);    // header
            bytes_sent2 = send(sock_connection, line, msg_len, 0);  // payload
            printf("Packet %d transmitted with %d data bytes\n",\
                 head.ihead, msg_len);
        }
        bytes_sent2 = send(sock_connection, NULL, 0, 0);  // EoT packet
        printf("\nEnd of Transmission Packet with sequence number %d transmitted with %d data bytes\n\n",\
                 head.ihead+1, 0);
      }

      printf("Number of packets transmitted: %d\n", head.ihead);
      printf("Total number of data bytes transmitted: %d\n\n", sumCount);
      /* close the socket */
      fclose(file);
      close(sock_connection);
      break;
   }
   return 0;
}
