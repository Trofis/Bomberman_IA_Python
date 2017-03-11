// -*- coding: utf-8 -*-
#include "libnetwork.hpp"

#include <string>

extern "C" {
#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
}


////////////////////////////////////////////////////////////////////////////////
// Données et méthodes privées


// Network()
Network::Network()
: _fd(-1)
{}


// _socket()
int Network::_socket()
{
	_fd = ::socket(PF_INET, SOCK_STREAM, 0);

	if (_fd < 0)
		{ return 0; }
	else
		{ return _fd; }
}


// _connect(const char*, int)
int Network::_connect(const char* host, int port)
{
	struct addrinfo* info;
	char buf[100];
	int r;

	r = ::sprintf(buf, "%d", port);

	if (!r)
		{ return 0; }

	r = ::getaddrinfo(host, buf, 0, &info);

	if (r)
		{ return 0; }

	while (1)
	{
		r = ::connect(_fd, info->ai_addr, info->ai_addrlen);

		if (r) {
			if ((errno == EAGAIN) || (errno == EINTR))
				{ continue; }
			else
				{ return 0; }
		}

		break;
	}

	::freeaddrinfo(info);

	return 1;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Fonctions publiques


// connectTo(const char*, int)
int Network::connectTo(const char* host, int port)
{
	return _socket() && _connect(host, port);
}


// connectTo(std::string&, int)
int Network::connectTo(const std::string& host, int port)
{
	return connectTo(host.data(), port);
}


// disconnect()
int Network::disconnect()
{
	int r;

	while (_fd >= 0)
	{
		r = ::close(_fd);

		if (r < 0) {
			if (errno == EINTR)
				{ continue; }
			else
				{ return 1; }
		}
		break;
	}
	_fd = -1;

	return 0;
}


// getInt()
int Network::getInt()
{
	return (int) getChar();
}


// getChar()
char Network::getChar()
{
	int r;
	char c;

	while ((r = ::read(_fd, &c, sizeof(char))) <= 0)
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
std::string Network::getString()
{
	int r, l = 0;
	char c;
	char buf[BUFSIZ];

	while (1)
	{
		r = ::read(_fd, &c, sizeof(char));

		if (r < 0) {
			if ((errno == EAGAIN) || (errno == EINTR))
				{ continue; }
			else
				{ return 0; }
		}

		buf[l] = c;

		if ((r == 0) || (c == '\0') || (c == '\n'))
		{
			char *s = (char*) ::malloc(l * sizeof(char));
			strncpy(s, buf, l);
			s[l] = 0;

			return std::string(s);
		}
		++l;
	}
}


// sendChar(char)
int Network::sendChar(char c)
{
	int r;

	while ((r = ::write(_fd, &c, sizeof(char))) <= 0)
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
int Network::sendString(const char* s)
{
	int n = strlen(s), r;

	char* buf = (char*) malloc((n+1) * sizeof(char));
	strncpy(buf, s, n);
	buf[n] = '\0';
	++n;

	while (n > 0)
	{
		r = ::write(_fd, buf, n);
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


// sendString(std::string& s)
int Network::sendString(const std::string& s)
{
	return sendString(s.data());
}


