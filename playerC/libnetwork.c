// -*- coding: utf-8 -*-
#include "libnetwork.h"

#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


////////////////////////////////////////////////////////////////////////////////
// Données et fonctions internes


int bm_fd = -1;


// bm_socket()
int bm_socket()
{
	bm_fd = socket(PF_INET, SOCK_STREAM, 0);
	
	if (bm_fd < 0)
		{ return 0; }
	else
	  { return bm_fd; }
}


// bm_connect(const char*, int)
int bm_connect(const char* host, int port)
{
	struct addrinfo* info;
	char buf[100];
	int r;

	r = sprintf(buf, "%d", port);

	if (!r)
		{return 0; }

	r = getaddrinfo(host, buf, 0, &info);

	if (r)
	  {	return 0; }

	while (1)
	{
		r = connect(bm_fd, info->ai_addr, info->ai_addrlen);

		if (r) {
			if ((errno == EAGAIN) || (errno == EINTR))
				{ continue; }
			else
				{ return 0; }
		}

		break;
	}

	freeaddrinfo(info);

	return 1;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Fonctions externes


// connectTo(const char*, int)
int connectTo(const char* host, int port)
{
	return !((bm_socket()) && bm_connect(host, port));
}


// disconnect()
int disconnect()
{
	int r;

	while (bm_fd >= 0)
	{
		r = close(bm_fd);

		if (r < 0) {
			if (errno == EINTR)
				{ continue; }
			else
				{ return 1; }
		}
		break;
	}
	bm_fd = -1;

	return 0;
}


// getInt()
int getInt()
{
	return (int) getChar();
}


// getChar()
char getChar()
{
	int r;
	char c;

	while ((r = read(bm_fd, &c, sizeof(char))) <= 0)
	{
		if (r < 0) {
			if ((errno == EAGAIN) || (errno == EINTR))
				{ continue; }
			else
				{ return 0; }
		}

		if (r == 0)
			{ return 0; }
	}

	return c;
}


// getString()
char* getString()
{
	int r, l = 0;
	char c;
	char buf[BUFSIZ];

	while (1)
	{
		r = read(bm_fd, &c, sizeof(char));

		if (r < 0) {
			if ((errno == EAGAIN) || (errno == EINTR))
				{ continue; }
			else
				{ return 0; }
		}

		buf[l] = c;

		if ((r == 0) || (c == '\0') || (c == '\n'))
		{
			char *s = (char*) malloc(l * sizeof(char));
			strncpy(s, buf, l);
			s[l] = 0;
			return s;
		}
		++l;
	}
}


// sendChar(char)
int sendChar(char c)
{
	int r;

	while ((r = write(bm_fd, &c, sizeof(char))) <= 0)
	{
		if (r < 0) {
			if ((errno == EAGAIN) || (errno == EINTR))
				{ continue; }
			else
				{ return 0; }
		}
	}

	return 1;
}


// sendString(const char*)
int sendString(const char* s)
{
	int n = strlen(s), r;

	char* buf = (char*) malloc((n+1) * sizeof(char));
	strncpy(buf, s, n);
	buf[n] = '\0';
	++n;

	while (n > 0)
	{
		r = write(bm_fd, buf, n);
		if (r < 0) {
			if ((errno == EAGAIN) || (errno == EINTR))
				{ continue; }
			else
				{ return 0; }
		}

		buf += r;
		n -= r;
	}

	return 1;
}

