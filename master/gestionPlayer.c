#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/select.h>

#include "master.h"
#include "clientserver.h"
#include "messages.h"


#define MAX_DEJA_JOUE 20


int get_index_player_by_id(char id){
  int i=0;
  while ((i<nbPlayer)&&(listePlayer[i].id!=id)) i++;
  if (i==nbPlayer) return -1;
  return i;
}

int login_libre(char *login){
  int nb=strlen(login), i;
  i=0;
  while (i<nbPlayer) {
    if (strlen(listePlayer[i].login)==nb) {
      if (strncmp(listePlayer[i].login, login, nb)==0) return 0;
    }
    i++;
  }
  return 1;
}


//gestion des interactions avec le player

void loginPlayer(int sock, char *com){
  int ok;
  char *nom;
  
  if (strlen(com)==99) {
    MSG_MASTER_PLAYER_ERR(sock, debug);
    printf("Echec connexion player (message trop long)\n"); fflush(stdout);
    CS_close(sock);
  } else {
    nom=(char *)malloc(strlen(com));
    strcpy(nom, com+6);
    if (login_libre(nom)==0) {
      MSG_MASTER_PLAYER_ERR_LOGIN(sock, debug);
      printf("Echec connexion player (login \"%s\" utilise)\n", nom); fflush(stdout);
      CS_close(sock);
      return;
    }
    //debut section critique PLAYER
    pthread_mutex_lock(&verrouPlayer);
    if (nbPlayer<MAX_PLAYER) {
      listePlayer[nbPlayer].login=nom;
      listePlayer[nbPlayer].sock=sock;
      listePlayer[nbPlayer].pts=1000;
      listePlayer[nbPlayer].dejaJoue=0;
      listePlayer[nbPlayer].id='0'+nbPlayer+1;
      printf("player %s : OK  (#player=%d)\n",nom,nbPlayer); fflush(stdout);   
      MSG_MASTER_PLAYER_SPEC(sock, debug);
      MSG_MASTER_PLAYER_ID(sock, listePlayer[nbPlayer].id, debug);
      ok=sendSpecLaby(lab,MORT_SUBITE, sock);  
      if (ok==-1) supprimePlayer(sock, nom);
      nbPlayer++;
      nbPart++;
      sendInfoPlayers();
    } else {
      MSG_MASTER_PLAYER_CONNEXION_IMPOSSIBLE(sock, debug);
      printf("Echec connexion player (nb MAX player)\n"); fflush(stdout);
      CS_close(sock);
      return;
    }
    //fin section critique PLAYER
    pthread_mutex_unlock(&verrouPlayer);
  }
}

//suppression d'un viewer de la liste SANS POSITIONNER LE VERROU
void supprime_player_unlock(int sock, char *nom){
  int i=0,j;
  while ((i<nbPlayer)&&(listePlayer[i].sock!=sock)) i++; //position du player a enlever
  j=i+1;
  while (j<nbPlayer) {listePlayer[j-1]=listePlayer[j];j++;} //decalage des autres
  nbPlayer--;
  CS_close(sock);
  printf("player %s deconnecté\n",nom); fflush(stdout);
}
//suppression d'un viewer de la liste
void supprimePlayer(int sock, char *nom){
  //debut section critique PLAYER
  pthread_mutex_lock(&verrouPlayer);
  supprime_player_unlock(sock, nom);
  pthread_mutex_unlock(&verrouPlayer);
}

void retirerJoueurLab(int idJ){
  int i,j;
  char c=listePlayer[idJ].id;
  for (i=0; i<lab.l; i++)
    for (j=0; j<lab.c; j++)
      if (lab.map[i][j]==c){
	lab.map[i][j]=' ';
	return;
      }
}

void *gestionPlayer(void *arg){
  fd_set fd;
  char msg[100];
  int max, i, val;
 
  while (1) {
    //creation de la liste de sockets a ecouter
    FD_ZERO(&fd);
    max=-1;
    pthread_mutex_lock(&verrouPlayer);
    for(i=0; i<nbPlayer; i++) {
      FD_SET(listePlayer[i].sock, &fd); 
      if (listePlayer[i].sock>max) max=listePlayer[i].sock;
    }
    pthread_mutex_unlock(&verrouPlayer);
    if (max==-1) {
      sleep(1);
    } else {
      val=select(max+1, &fd, NULL, NULL, NULL);
      //parcours des connexion actives
      if (val==-1) {
	printf("Warning : erreur select dans player\n");
      } else {
	pthread_mutex_lock(&verrouPlayer);
	for (i=0; i<nbPlayer; i++) if (FD_ISSET(listePlayer[i].sock, &fd)) {
	    val=read(listePlayer[i].sock, msg, 1);
	    if (val<=0) {
	      printf(" !!! %s s'est déconnecté\n",listePlayer[i].login);
	      retirerJoueurLab(i);
	      eliminer_player(i);
	      i--;
	      //supprime_player_unlock(listePlayer[i].sock,listePlayer[i].login);
	    } else {
	      if (debug) printf("%s -> %s %d\n", listePlayer[i].login, msg,val);
	      if ((etat==ETAT_STOP)||(etat==ETAT_SENDING)) {
		val=MSG_MASTER_PLAYER_IGNORE(listePlayer[i].sock, debug);
	      }
	      if (etat==ETAT_PLAYING) {
		if (listePlayer[i].dejaJoue) {
		  MSG_MASTER_PLAYER_DEJA_JOUE(listePlayer[i].sock, debug);
		  listePlayer[i].dejaJoue++;
		  if (listePlayer[i].dejaJoue>MAX_DEJA_JOUE) {
		    //deconnexion pour tentative de debordement
		    printf("disqualification de %s (tentative de débordement)\n",listePlayer[i].login);
		    //		    pthread_mutex_lock(&verrouPlayer);
		    retirerJoueurLab(i);
		    eliminer_player(i);
		    i--;
		    //		    pthread_mutex_unlock(&verrouPlayer);
		    //supprimePlayer(listePlayer[i].sock,listePlayer[i].login);
		  }
		} else {
		  //prise en compte de l'action du joueur d'indice i
		  play(i, *msg);
		  listePlayer[i].dejaJoue=1;
		}
	      }
	    }
	  }
	pthread_mutex_unlock(&verrouPlayer);
      }
    }
  }
}
