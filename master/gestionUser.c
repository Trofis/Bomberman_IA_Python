#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "master.h"
#include "clientserver.h"
#include "messages.h"

void tri(struct player *L, int nb){
  int i,j,k;
  struct player p;
  for (i=0; i<nb-1; i++) {
    k=i;
    for (j=i+1; j<nb;j++) if (L[j].pts>L[k].pts) k=j;
    //permuter i-k
    p=L[i];
    L[i]=L[k];
    L[k]=p;
  }
}

void afficher_scores(){ 
  int i;
  tri(listePlayer, nbPlayer);
  printf("%d player(s) connectés :\n", nbPlayer);
  for (i=0; i<nbPlayer; i++) 
    printf("   - %s : %d points (socket %d)\n", listePlayer[i].login, listePlayer[i].pts, listePlayer[i].sock);
  
  int nbe=get_nb_elimines();
  struct player *elim=get_elimines();
  tri(elim, nbe);
  printf("%d player(s) éliminés :\n", nbe);
  for (i=0; i<nbe; i++) 
    printf("   - %s : %d points (socket %d)\n", elim[i].login, elim[i].pts, elim[i].sock);
}

int save_scores(char *nomPartie){ 
  int i;
  FILE *fic=fopen(nomPartie,"wt");
  if (fic==NULL){
    printf("probleme avec le fichier %s\n",nomPartie);
    return -1;
  }

  for (i=0; i<nbPlayer; i++) 
    fprintf(fic,"%s,%d,%d\n",listePlayer[i].login, listePlayer[i].pts, listePlayer[i].bomb);
  
  int nbe=get_nb_elimines();
  struct player *elim=get_elimines();
  for (i=0; i<nbe; i++) 
    fprintf(fic,"%s,%d,%d\n", elim[i].login, elim[i].pts, elim[i].bomb);
  fclose(fic);
  return 0;
}

//gestion des commandes à la console:

void *gereUser(void *arg){
  char msg[100], user[100]; int i, j, encore,k;
  pthread_t play, synchro;

  while (1) {
    get_ligne(msg);

    //=====================================QUIT======================
    if (strncmp("quit", msg, 4)==0) {
      //verrou???
      for (i=0; i<nbViewer; i++) {
	MSG_MASTER_VIEWER_QUIT(sockViewer[i], debug); 
	CS_close(sockViewer[i]);
      }
      for (i=0; i<nbPlayer; i++) {
	MSG_MASTER_PLAYER_QUIT(listePlayer[i].sock, debug); 
	CS_close(listePlayer[i].sock);
      }
      save_scores("scoretmp.csv");
      exit(0);
    //=====================================HELP======================
    } else if (strncmp("help", msg, 4)==0) {
      printf("------------------------------------------------------------\n");
      printf("Commandes disponibles :\n");
      printf("  quit : deconnexion des tous les clients et arret du serveur\n");
      printf("  liste : affiche la liste des players connectés\n");
      printf("  kill player : deconnecte le player (login)\n");
      printf("  start : commence une nouvelle partie\n");
      printf("  pause  : suspend partie en cours\n");
      printf("  continue : reprend une partie suspendue\n");
      printf("  +  : augmente d'une seconde le delai de synchro\n");
      printf("  -  : reduit d'une seconde le delai de synchro\n");
      printf("  ret  : bascule pour le retrecissement\n");
      printf("  save  : sauvegarde les scores des joueurs\n");
      printf("  stop  : termine la partie en cours\n");
      printf("------------------------------------------------------------\n");
    //=====================================LISTE======================
    } else if (strncmp("liste", msg, 5)==0) {
      printf("-----------------------------------------\n");
      pthread_mutex_lock(&verrouPlayer);
      afficher_scores();
      pthread_mutex_unlock(&verrouPlayer);
      printf("-----------------------------------------\n");
    //=====================================KILL======================
    } else if (strncmp("kill", msg, 4)==0) {
      i=0; encore=1; 
      while ((encore!=0)&&(i<nbPlayer)) {
	encore=strcmp(msg+5, listePlayer[i].login);
	//printf("%s = %s : %d\n",msg+5,listePlayer[i].login,encore);
	if (encore) i++;
      }
      if (i<nbPlayer) {printf("supp %d %s \n",i, listePlayer[i].login);fflush(stdout);
	for (k=0; k<lab.l; k++)
	  for (j=0;j<lab.c; j++) 
	    if (lab.map[k][j]==listePlayer[i].id) lab.map[k][j]=' ';
	strcpy(user, listePlayer[i].login);
	supprimePlayer(listePlayer[i].sock, user); 
      } else {
	printf("player %s n'existe pas\n",msg+5);
      }
    //=====================================START======================
    }  else if (strncmp("start", msg, 5)==0)  {
      if (etat!=ETAT_STOP){
	printf("Partie en cours...\n");
      } else if (nbPlayer==0){
	printf("Aucun joueur connecté...\n");
      } else {
	printf("  La partie commence...\n");
	//initialisation players -> defini dans gestionPartie
	init_partie();
	
	//envoi des labyrinthes
	for (i=0; i<nbPlayer; i++) {
	  MSG_MASTER_PLAYER_MAP(listePlayer[i].sock, debug);
	  sendLaby(lab, listePlayer[i].sock,1);
	}

	etat=ETAT_PLAYING;
	pthread_create(&play, NULL, gestionPlayer, NULL);
	pthread_create(&synchro, NULL, synchronizer, NULL);
      }
    //=====================================PAUSE======================
    } else if (strncmp("pause", msg, 5)==0) {
      if (etat!=ETAT_PLAYING){
	printf("Pas de partie en cours...\n");
      } else {
	printf("-----------------------------------------\nTapez \"continue\" pour reprendre...\n");
	etat=ETAT_PAUSE;
      }
    //=====================================CONTINUE======================
    } else if (strncmp("continue", msg, 8)==0) {
      if (etat!=ETAT_PAUSE){
	printf("Pas de partie en pause...\n");
      } else {
	etat=ETAT_PLAYING;
      }
    //=====================================  +  ======================
    } else if (strncmp("+", msg, 1)==0) {
      DELAI_TIMER_SECOND++;
      printf("delai de synchro : %d,5 s\n",DELAI_TIMER_SECOND);
      //=====================================  -  ======================
    } else if (strncmp("-", msg, 1)==0) {
      if (DELAI_TIMER_SECOND>0) DELAI_TIMER_SECOND--;
      printf("delai de synchro : %d,5 s\n",DELAI_TIMER_SECOND);
      //=====================================  ret  ======================
    } else if (strncmp("ret", msg, 3)==0) {
      if (DELAI_RETRECISSEMENT>0) {
	DELAI_RETRECISSEMENT=0;
	printf("retrecissement activé\n");
      } else {
     	DELAI_RETRECISSEMENT=100000;
	printf("retrecissement desactivé\n");
      }
      //=====================================STOP======================
    }  else if (strncmp("stop", msg, 4)==0)  {
      if ((etat!=ETAT_PLAYING)&&(etat!=ETAT_PAUSE)){
	printf("Pas de partie en cours...\n");
      } else {
	printf("Fin de la partie...\n");
	for (i=0; i<nbPlayer; i++) {
	  MSG_MASTER_PLAYER_GAME_OVER(listePlayer[i].sock, debug);
	}
	for (k=0; k<lab.l; k++)
	  for (j=0;j<lab.c; j++) 
	    if (joueurID(lab.map[k][j])) lab.map[k][j]=' ';
	etat=ETAT_STOP;
	pthread_cancel(play);
	pthread_cancel(synchro);
	afficher_scores();
      }
	//=====================================SAVE======================
    } else if (strncmp("save", msg, 4)==0)  {
      if (etat==ETAT_PLAYING)
	printf("Partie en cours, impossible de sauver les scores...\n");
      else if (nbPlayer==0)
	printf("Aucun joueur connecté... rien à sauvegarder\n");
      else{
	for (i=5;i<strlen(msg)&&msg[i]==' ';i++);
	printf("Sauvegarde des scores dans le fichier %s\n",msg+i);
	if (save_scores(msg+i)==0)
	  printf("Sauvegarde des scores réussie\n");
      }
    } else
      //=====================================AUTRES======================
      printf("commande non valide : %s**\n",msg);
  }
}
