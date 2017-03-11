#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>


#include "master.h"
#include "clientserver.h"


pthread_t thViewer=0;

//gestion des interactions avec le viewer

void loginViewer(int sock){
  int ok;
    //debut section critique VIEWER
    pthread_mutex_lock(&verrouViewer);
    sockViewer[nbViewer++]=sock;
    pthread_mutex_unlock(&verrouViewer);
    //fin section critique VIEWER
    printf("viewer %d : OK  (#viewer=%d)\n",sock,nbViewer); fflush(stdout);

    //arret du thread de gestion des deconnexions
    if (thViewer!=0) pthread_cancel(thViewer);
    //lancement du thread de gestion des deconnexions des Viewers
    pthread_create(&thViewer, NULL, ecouteViewer, NULL);

    ok=sendSpecLaby(lab, MORT_SUBITE, sock);
    ok=sendLaby(lab, sock, 0);
    //    if (etat!=ETAT_STOP){
      sendInfoPlayers();
      //}
}

//suppression d'un viewer de la liste
void supprimeViewer(){
  int i=0,j=0;
  //debut section critique VIEWER
  while (i<nbViewer) {
    if (sockViewer[i]==-1) {
      j=i+1;
      //decalage des autres
      while (j<nbViewer) {sockViewer[j-1]=sockViewer[j];j++;} 
      nbViewer--;
      //printf("viewer %d deconnecté: (#viewer=%d)\n",i,nbViewer); 
    } else {
      i++;
    }
  }
  //fin section critique VIEWER
}


//fonction d'ecoute des viewer pour tester la deconnexion
//les viewers n'envoient pas d'information (après connexion)
//cette fonction attend un changement d'état sur les sockets des viewers
//pour tester les cas de déconnexion.
void *ecouteViewer(void *arg){
  int encore=1,i, maxc, val;
  char buffer[100];
  fd_set fd;

  while (encore){
    FD_ZERO(&fd);
    maxc=0;printf("==================Liste des viewers : ");
    for (i=0; i<nbViewer; i++) {
      FD_SET(sockViewer[i], &fd); printf("%d - ",sockViewer[i]); 
      if (sockViewer[i]>maxc) maxc=sockViewer[i];
    };
    printf("\n");
    maxc++;
    //printf("attente select\n");
    i=select(maxc, &fd, NULL, NULL, NULL);
    pthread_mutex_lock(&verrouViewer);
    //for (i=0; i<nbViewer; i++) 
    //if (FD_ISSET(sockViewer[i],&fd)) printf("%d*",sockViewer[i]);
    if (i==-1) {
      //printf("retour select -1\n");
    } else {
      for (i=0; i<nbViewer; i++) {
	if (FD_ISSET(sockViewer[i],&fd)) {
	  val=read(sockViewer[i], buffer,1);
	  if (val<=0) {
	    printf("viewer %d deconnecté: (#viewer=%d)\n",sockViewer[i],nbViewer); 
	    fflush(stdout);
	    CS_close(sockViewer[i]);
	    sockViewer[i]=-1;
	  }
	}
	supprimeViewer();
      }
    }
    pthread_mutex_unlock(&verrouViewer);
  }
  return NULL;


}
