// -*- coding: utf-8 -*-
#ifndef _LIBNETWORK_HPP_
#define _LIBNETWORK_HPP_

#include <string>

class Network
{
private:
	int _fd;
	int _socket();
	int _connect(const char*, int);

public:
	/// Construit un objet de type Network
	Network();

	/// Connecte le client à un serveur.
	/// \param host nom ou adresse IP du serveur hôte.
	/// \param port numéro du port sur lequel se connecter.
	/// \return 0 si la connexion a réussi, 1 sinon.
	int connectTo(const char* host, int port);
	int connectTo(const std::string& host, int port);

	/// Déconnecte le client du serveur actuel.
	/// \return 0 si la déconnexion a réussi, 1 sinon.
	int disconnect();

	/// Lit un entier envoyé par le serveur.
	/// \return l'entier reçu du serveur, 0 sinon.
	int getInt();

	/// Lit un caractère envoyé par le serveur.
	/// \return le caractère reçu du serveur, le caractère nul (0) sinon.
	char getChar();

	/// Lit une chaîne de caractères envoyée par le serveur.
	/// \return la chaîne de caractères reçue du serveur, 0 sinon.
	std::string getString();

	/// Envoie un caractère au serveur.
	/// \param c caractère à envoyer.
	/// \return 1 si l'envoi a réussi, 0 sinon.
	int sendChar(char c);

	/// Envoie une chaîne de caractères au serveur.
	/// \param s chaîne de caractères à envoyer.
	/// \return 1 si l'envoi a réussi, 0 sinon.
	int sendString(const char* s);
	int sendString(const std::string& s);
};

#endif

