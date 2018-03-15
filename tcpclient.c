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

struct header{
    unsigned short ihead; // integer value of sequence number
    char chead[5];        // string value of sequence number
    unsigned short ilen;  // integer value of char count
    char clen[5];         // string value of char count
    char comb[9];         // combined message of chead and clen
};

int main(int argc, char *argv[]) {

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
   FILE *fp;

   int sumCount = 0;   // Sum of all data bytes received
   struct header head; // Initializing struct
   head.ihead = 0;
   head.ilen = 0;
  
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
    sprintf(head.chead, "%04d", head.ihead);    //
    strcpy(head.comb, head.chead);              //  Constructs header
    sprintf(head.clen, "%04d", msg_len);        //
    strcat(head.comb, head.clen);               //

   bytes_sent = send(sock_client, head.comb, 8, 0);         //header
   bytes_sent2 = send(sock_client, filename, msg_len, 0);   //payload
   printf("\nPacket %d transmitted with %d data bytes\n",\
                 head.ihead, msg_len);
/*-----------------------------------------------------------------------*/
   /* get response from server */

   fp = fopen("out.txt", "w");     // open out.txt
    do{
        bytes_recd = recv(sock_client, head.comb, 8, 0);    // header
        if (bytes_recd <= 0) {  // If EoT packet, break loop
        printf("\nEnd of Transmission Packet with sequence number %d received with %d data bytes\n",\
            head.ihead+1, 0);
            break;
        }    

        sprintf(head.chead, "%.4s", head.comb);  //
        sprintf(head.clen, "%s",head.comb+4);    //  Deconstructs header
        head.ihead = atoi(head.chead);           //
        head.ilen = atoi(head.clen);             //
        sumCount += head.ilen;                   // Update total char bytes received

        bytes_recd2 = recv(sock_client, line, head.ilen, 0); // Payload
        printf("Packet with sequence number %d received with %d data bytes\n",\
            head.ihead, head.ilen);
        line[bytes_recd2] = '\0';
        fputs(line, fp);    // Drops payload in file

    } while(1);

    printf("Number of packets received: %d\n", head.ihead);
    printf("Total number of data bytes received: %d\n\n", sumCount);

   fclose(fp);
   close (sock_client);
   return 0;
}
