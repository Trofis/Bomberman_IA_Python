
#define _GNU_SOURCE 

#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>



int CS_server(int port, int backlog);

int CS_client(char* host, int port);

void CS_close(int); 

void decode_iaddr(char ** phost, char ** pip, unsigned short * pp,
		  struct sockaddr_in nom);
void getIP(char ** pip, struct sockaddr_in nom);

int get_bytes(int sock, char* buf, int siz);
int get_string(int sock, char* buf);
int get_ligne(char* buf);

int put_bytes(int sock, char* buf, int siz);

int protected_get_bytes(int sock, char* buf, int siz);
