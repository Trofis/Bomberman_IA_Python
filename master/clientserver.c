#include "clientserver.h"

#include <sys/select.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>

int CS_server(int port, int backlog)
{
  int fd = socket(PF_INET, SOCK_STREAM, 0);

  if (fd < 0)
    {
      perror("socket");
      exit(0);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
      {
	perror("bind");
	exit(0);
      }
 
  if (listen(fd, backlog) < 0)
    {
     	perror("listen");
	exit(0);
    }

  return fd;

}

int CS_client(char* host, int port)
{

  int fd = socket(PF_INET, SOCK_STREAM, 0);

  if (fd < 0)
    {
     	perror("socket");
	exit(0);
     }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    struct hostent* hp = gethostbyname(host);

    if (!hp)
      {
    	perror("gethostbyname");
	exit(0);
     }

    addr.sin_addr.s_addr = (* (struct in_addr *) hp->h_addr_list[0]).s_addr;

    while (connect(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
      {
	/* recommencer le connect(2) tant qu'il est interrompu par un signal */
	if (errno == EINTR) continue;
	exit(0);
      }
   return fd;

}

void CS_close(int fd)	/* fermer un descripteur de manière robuste */
{
  if (fd >= 0)			/* descripteur valide? */
    /* recommencer tant que close(2) est interrompu par un signal */
    while (close(fd) < 0)
      if (errno != EINTR) break;
}

void decode_iaddr(char ** phost, char ** pip, unsigned short * pp,
		  struct sockaddr_in nom) {
  union {
    uint32_t unb;
    unsigned char qnb[4];
  } aip; 
  struct hostent * info=gethostbyaddr((char *)&nom.sin_addr.s_addr,4,AF_INET);
  if (info) asprintf(phost,"%s",info->h_name);
  else asprintf(phost,"%s","()");
  *pp=ntohs(nom.sin_port);
  aip.unb=nom.sin_addr.s_addr;/* le byte order est network i.e. BIG ENDIAN */
  asprintf(pip,"%d.%d.%d.%d", aip.qnb[0], aip.qnb[1], aip.qnb[2], aip.qnb[3]);
}

void getIP(char ** pip, struct sockaddr_in nom) {
  union {
    uint32_t unb;
    unsigned char qnb[4];
  } aip; 
  aip.unb=nom.sin_addr.s_addr;/* le byte order est network i.e. BIG ENDIAN */
  asprintf(pip,"%d.%d.%d.%d", aip.qnb[0], aip.qnb[1], aip.qnb[2], aip.qnb[3]);
}

int put_bytes(int sock, char* buf, int siz)
{
  while (siz)
    {
      ssize_t k = write(sock, buf, siz);
      if (k < 0)
	{
	  if (errno == EINTR) continue;
	  perror("write");
	  return -1;
	}
      buf += k;
      siz -= k;
    }
  return 0;
}

int get_bytes(int sock, char* buf, int siz)
{
  while (siz)
    {
      int k = read(sock, buf, siz);
      if (k==0)
	{
	  perror("read");
	  return -1;
	}
      if (k < 0)
	{
	  perror("read");
	  return -1;
	}
      buf += k;
      siz -= k;
    }
  return 0;
}
int get_string(int sock, char* buf) {
  int i=0, encore=1;
  while (encore)
    {
      int k = read(sock, buf, 1);
      if (k==0)
	{
	  perror("read");
	  return -1;
	}
      if (k < 0)
	{
	  perror("read");
	  return -1;
	}
      if (*buf==0) encore=0;
      buf ++; i++;
    }
  return i;
}
int get_ligne(char* buf) {
  int i=0, encore=1;
  while (encore)
    {
      int k = scanf("%c", buf);//read(sock, buf, 1);
      if (k==0)
	{
	  perror("read");
	  return -1;
	}
      if (k < 0)
	{
	  perror("read");
	  return -1;
	}
      if (*buf=='\n') { encore=0; *buf=0;}
      buf ++; i++;
    }
  
  return i;
}

int protected_get_bytes(int sock, char* buf, int siz)
{ 
  fd_set fd;
  int val; 
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 200000;

  //on lit une première fois avec attente éventuelle 200 ms
  FD_ZERO(&fd);
  FD_SET(sock, &fd);
  val=select(sock+1, &fd, NULL, NULL, &tv);
  if (FD_ISSET(sock, &fd)) {
    val=read(sock, buf, siz);
    buf+=val;
    siz-=val;
  }
  if (siz==0) return 0;
  //si incomplet, on lit une 2eme fois  avec attente éventuelle 200 ms
  tv.tv_sec = 0;
  tv.tv_usec = 200000;
  FD_ZERO(&fd);
  FD_SET(sock, &fd);
  val=select(sock+1, &fd, NULL, NULL, &tv);
  if (FD_ISSET(sock, &fd)) {
    val=read(sock, buf, siz);
    siz-=val;
  }
  if (siz==0) return 0;
  return -1;
}
  
    
