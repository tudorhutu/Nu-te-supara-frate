/* cliTCPIt.c - Exemplu de client TCP
   Trimite un numar la server; primeste de la server numarul incrementat.
         
   Autor: Lenuta Alboaie  <adria@infoiasi.ro> (c)2009
*/

//./cli.out 127.0.0.1 2908
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <chrono>
#include <thread>
/* codul de eroare returnat de anumite apeluri */
//extern int errno;

/* portul de conectare la server*/
int port;

int main(int argc, char *argv[])
{
  int sd;                    // descriptorul de socket
  struct sockaddr_in server; // structura folosita pentru conectare
                             // mesajul trimis
  int nr = 0;
  char buf[10];

  /* exista toate argumentele in linia de comanda? */
  if (argc != 3)
  {
    printf("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
    return -1;
  }

  /* stabilim portul */
  port = atoi(argv[2]);

  /* cream socketul */
  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("Eroare la socket().\n");
    return errno;
  }

  /* umplem structura folosita pentru realizarea conexiunii cu serverul */
  /* familia socket-ului */
  server.sin_family = AF_INET;
  /* adresa IP a serverului */
  server.sin_addr.s_addr = inet_addr(argv[1]);
  /* portul de conectare */
  server.sin_port = htons(port);

  /* ne conectam la server */
  if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
  {
    perror("[client]Eroare la connect().\n");
    return errno;
  }
  while (1)
  {
    //check if client is still connected to server
    if (read(sd, &nr, sizeof(int)) == 0)
    {
      printf("[client]Disconnected from server\n");
      close(sd);
      exit(EXIT_SUCCESS);
    }
    while (1)
    {
      printf("[client]What pawn would you like to move?: ");
      fflush(stdout);
      read(0, buf, sizeof(buf));
      nr = atoi(buf);
      if (nr > 0 && nr <= 6)
        break;
      else
        printf("invalid pawn number selected\n");
    }
    /* trimiterea mesajului la server */
    if (write(sd, &nr, sizeof(int)) <= 0)
    {
      perror("[client]Eroare la write() spre server.\n");
      return errno;
    }
    int sentcommand = nr;
    /* citirea raspunsului dat de server 
     (apel blocant pina cind serverul raspunde) */
    if (read(sd, &nr, sizeof(int)) < 0)
    {
      perror("[client]Eroare la read() de la server.\n");
      return errno;
    }
    /* afisam mesajul primit */
    if (sentcommand == 5)
      printf("Saving and quitting...\n");
    if (sentcommand == 6)
      printf("Quitting...\n");
    if (nr == -1)
      printf("It's not your turn yet\n");
    else if (sentcommand != 5 && sentcommand != 6)
      printf("[client]The pawn will move %d spaces\n", nr);
  }
  /* inchidem conexiunea, am terminat */
  close(sd);
}