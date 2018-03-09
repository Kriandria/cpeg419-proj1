/* tcp_ client.c */ 
/* Programmed by Adarsh Sethi */
/* February 21, 2018 */     

#include <stdio.h>          /* for standard I/O functions */
#include <stdlib.h>         /* for exit */
#include <string.h>         /* for memset, memcpy, and strlen */
#include <netdb.h>          /* for struct hostent and gethostbyname */
#include <sys/socket.h>     /* for socket, connect, send, and recv */
#include <netinet/in.h>     /* for sockaddr_in */
#include <unistd.h>         /* for close */

#define STRING_SIZE 80


int main(void) {

   int sock_client;  /* Socket used by client */

   struct sockaddr_in server_addr;  /* Internet address structure that
                                        stores server address */
   struct hostent * server_hp;      /* Structure to store server's IP
                                        address */
   char server_hostname[STRING_SIZE] = "cisc450.cis.udel.edu"; /* Server's hostname */
   unsigned short server_port = 46668;  /* Port number used by server (remote port) */

   char filename[STRING_SIZE];  /* send message */
   char line[STRING_SIZE], line2[STRING_SIZE]; /* receive message */
   unsigned int msg_len;  /* length of message */                      
   int bytes_sent, bytes_sent2, bytes_recd, bytes_recd2; /* number of bytes sent or received */
   unsigned short header = 0; /* Header */
   FILE *fp;
  
   /* open a socket */

   if ((sock_client = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      perror("Client: can't open stream socket");
      exit(1);
   }

   /* Note: there is no need to initialize local client address information 
            unless you want to specify a specific local port
            (in which case, do it the same way as in udpclient.c).
            The local address initialization and binding is done automatically
            when the connect function is called later, if the socket has not
            already been bound. */

   /* initialize server address information */

   if ((server_hp = gethostbyname(server_hostname)) == NULL) {
      perror("Client: invalid server hostname");
      close(sock_client);
      exit(1);
   }

   /* Clear server address structure and initialize with server address */
   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family = AF_INET;
   memcpy((char *)&server_addr.sin_addr, server_hp->h_addr,
                                    server_hp->h_length);
   server_addr.sin_port = htons(server_port);

    /* connect to the server */
 		
   if (connect(sock_client, (struct sockaddr *) &server_addr, 
                                    sizeof (server_addr)) < 0) {
      perror("Client: can't connect to server");
      close(sock_client);
      exit(1);
   }
  
   /* user interface */

   printf("Please input the filename to be reiceved:\n");
   scanf("%s", filename);
   msg_len = strlen(filename) + 1;

   /* send message */
   header = htons(header);
   //bytes_sent = send(sock_client, (char*)header, msg_len, 0);
   bytes_sent2 = send(sock_client, filename, msg_len, 0);
   printf("\nFilename Sent");

   /* get response from server */

   printf("\nThe response from server is:\n");
   //bytes_recd = recv(sock_client, (char*)header, STRING_SIZE, 0);
   bytes_recd2 = recv(sock_client, line, STRING_SIZE, 0);
   header = ntohs(header);
   printf("message header reads %d", header);
   printf("Message reads %s", line);
   int count = 0;
   fp = fopen("out.txt", "a+");
do {
   count++;
   msg_len = strlen(line);
   bytes_recd2 = recv(sock_client, line2, STRING_SIZE, 0);
   int fputs( line, fp);
} while ((bytes_recd2 != 0) && (count < 50));
   /* close the socket */
   fclose(fp);
   close (sock_client);
}
