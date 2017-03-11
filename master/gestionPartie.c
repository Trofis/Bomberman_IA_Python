#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/select.h>

#include "master.h"
#include "clientserver.h"
#include "messages.h"

#define DUREE 180
#define POINTS_BONUS_BASE 10
#define POINTS_BONUS 50
#define BOMB_BONUS 5
#define POINTS_BOMB 5
#define POINTS_MOVE 1
#define PTS_FAIRE_EXPLOSER_AVATAR 200
#define PTS_FAIRE_EXPLOSER_MUR 30

//pts perdus si on se fait exploser
#define EXPLODING 200

#define TIME_BEFORE_EXPLODE 5
//nb de cases atteintes par une bombe
#define BOMB_POWER 2

//etats initiaux des joueurs
#define NB_VIES_INIT 1000
#define NB_POINTS_INIT 500
#define NB_BOMB_INIT 10

//delai pour la synchronisation
#define DELAI_TIMER_SECOND_INIT 0
#define DELAI_TIMER_NANO 500000000
//delai avant retrecissement
#define DELAI_RETRECISSEMENT_INIT 10

//probabilite d'ajout d'un bonus (en chances sur 1000)
#define PROBA_BONUS_STD 30
#define PROBA_BONUS_EXPLODE 20
#define PROBA_BONUS_BOMB 30
#define PROBA_BONUS_TELE 10

//nb de joueur pour que la partie soit finie
#define NB_JOUEUR_MINI 0


//stockage des infos des joueurs elimines
struct player liste_elimines[20];
int nb_elimines;
int retLinf, retLsup, retCinf, retCsup, retC, retL, retDir;
char **boum;

//temps écoulé depuis debut de partie
int tps;

int hasard(int min, int max){
return (int) (min + ((float) rand() / RAND_MAX * (max - min + 1)));
}

void send_message(char *msg){
  int i;
  printf(" ** %s\n", msg);
  pthread_mutex_lock(&verrouViewer);
  for (i=0; i<nbViewer; i++) {
    write(sockViewer[i], "MESG", 4);
    write(sockViewer[i], msg, strlen(msg)+1);
  }
  pthread_mutex_unlock(&verrouViewer);
}

//le joueur d'indice n est eliminé
void eliminer_player(int n){
  MSG_MASTER_PLAYER_DEAD(listePlayer[n].sock, debug);
  printf("joueur %d éliminé %d",n,tps-DUREE);
  listePlayer[n].pts=tps-DUREE;
  liste_elimines[nb_elimines++]=listePlayer[n];
  //close(listePlayer[n].sock);
  supprime_player_unlock(listePlayer[n].sock, listePlayer[n].login);
}

void player_explose(int i, int j, int m){
  int n;
  char *tmp;
  n=get_index_player_by_id(lab.map[i][j]);
  if (n>=0) {
    listePlayer[n].pts-=EXPLODING;
    listePlayer[n].vies--;
    if (m>-1)
      asprintf(&tmp,"%s explose! (killer: %s)", listePlayer[n].login, listePlayer[m].login);
    else
      asprintf(&tmp,"%s explose! (killer: DEAD)", listePlayer[n].login);
    send_message(tmp); free(tmp);
    if ((listePlayer[n].vies==0)||(listePlayer[n].pts<=0)) {
      asprintf(&tmp,"%s est eliminé !", listePlayer[n].login);
      send_message(tmp); free(tmp);
      eliminer_player(n);
      lab.map[i][j]=' ';
    }
  }
}

//calcul des prochaines lignes et colonnes en fonction des directions
int nextL(int l, int c, char dir){
  int v=l;
  if (dir=='N') {
    v--;
    if (v<0) v=lab.l-1;
  } else if (dir=='S') {
    v++;
    if (v==lab.l) v=0;
  }
  return v;
}

int nextC(int l, int c, char dir){
  int v=c;
  if (dir=='O') {
    v--;
    if (v<0) v=lab.c-1;
  } else if (dir=='E') {
    v++;
    if (v==lab.c) v=0;
  }
  return v;
}

//test si un caractère est un identifiant de joueur
int joueurID(char c){
  if ((c!='X')&&(c!='x')&&(c!=' ')&&(c!=TYPE_BONUS_STD)&&(c!=TYPE_BONUS_EXPLODE)&&(c!=TYPE_BONUS_BOMB)&&(c!=TYPE_BONUS_TELE)) return 1;
  return 0;
}
//test si une cellule est explosable
int explosable(char c){
  if ((c==' ')||(c==TYPE_BONUS_STD)||(c==TYPE_BONUS_EXPLODE)||(c==TYPE_BONUS_BOMB)||(c==TYPE_BONUS_TELE)) return 1;
  return 0;
}

//une explosion affecte le case (l,c), bombe placée par le joueur n°n
void explosion(int l, int c, int n) {
  if (joueurID(lab.map[l][c])) {
    player_explose(l,c, n);
    if (n>-1) if (lab.map[l][c]!=listePlayer[n].id) {
	listePlayer[n].pts+=PTS_FAIRE_EXPLOSER_AVATAR;
      }
  }
  if (lab.map[l][c]=='x') {
    lab.map[l][c]=' ';
    if (n>-1) {
      listePlayer[n].pts+=PTS_FAIRE_EXPLOSER_MUR;
      char *tmp;
      asprintf(&tmp,"%s casse un mur",listePlayer[n].login);
      send_message(tmp); free(tmp);
    }
  }
}

void verifier_bombes(){
  int i,j,l,c, n, cpt,d;
  char pos[1000], *tmp;
  char nbombs=0, dir[4]={'N','S','E','O'};
  for (i=0; i<lab.l; i++) for (j=0; j<lab.c; j++) boum[i][j]=0;  
  for (i=0; i<lab.l; i++)
    for (j=0; j<lab.c; j++)
      if (lab.bombe[i][j]>1) {
	lab.bombe[i][j]--;
      } else if (lab.bombe[i][j]==1) {
	//qui avait posé cette bombe?
	n=get_index_player_by_id(lab.qui[i][j]);
	//explosion d'une bombe 
	if (n>-1)
	  asprintf(&tmp,"Explosion BOMB!!! (%d, %d, %s)",i,j,listePlayer[n].login);
	else
	   asprintf(&tmp,"Explosion BOMB!!! (%d, %d, DEAD)",i,j);
	send_message(tmp); free(tmp);
	boum[i][j]=1;
	pos[2*nbombs]=i; pos[2*nbombs+1]=j; nbombs++;
	if (lab.map[i][j]!=0) {
	  player_explose(i,j,n);
	}
	//dégage dans les 4 dir
	for(d=0; d<4; d++){
	  l=nextL(i,j,dir[d]); c=nextC(i,j,dir[d]); cpt=0;
	  while ((cpt<BOMB_POWER)&&(explosable(lab.map[l][c]))) {
	    boum[l][c]=1;l=nextL(l,c,dir[d]); c=nextC(l,c,dir[d]); cpt++;
	  }
	  if (joueurID(boum[l][c])) boum[l][c]=1;
	  //printf("explosion %d %d %d\n", l,c,n);
	  explosion(l,c,n);
	}
	lab.bombe[i][j]--;
      }

}

//envoi des points au viewer
void compter_points(){
  int i,j,k;

  /*msg[0]=0;
  for (i=0; i<nbPlayer; i++){
    char *buf;
    asprintf(&buf,"%s : %d (%d),   ",listePlayer[i].login,listePlayer[i].pts,listePlayer[i].bomb);
    k=strlen(msg);
    for (j=0; j<=strlen(buf); j++) msg[k+j]=buf[j];
    free(buf);
    }*/
  pthread_mutex_lock(&verrouViewer);   
  for (i=0; i<nbViewer; i++) {
      write(sockViewer[i], "PTS.", 4);
      j=htonl(nbPart);
      write(sockViewer[i], &j, 4);
      for(j=0; j<nbPlayer; j++){
	write(sockViewer[i], &(listePlayer[j].id), 1);
	k=ntohl(listePlayer[j].pts); 
	write(sockViewer[i], &k, 4);
	k=ntohl(listePlayer[j].bomb); 
	write(sockViewer[i], &k, 4);
      }

      for(j=0;j<nb_elimines;j++){
	write(sockViewer[i], &(liste_elimines[j].id), 1);
	k=ntohl(liste_elimines[j].pts); 
	write(sockViewer[i], &k, 4);
	k=ntohl(liste_elimines[j].bomb); 
	write(sockViewer[i], &k, 4);
      }
  }
  pthread_mutex_unlock(&verrouViewer);
  //printf("%s\n",msg);
}

//ajout d'un bonus dans une case vide
void add_bonus(char type_bonus){
  int l,c;
  char *tmp;
  //placement aleatoire dans le laby
  do {
    l=hasard(1,lab.l)-1;
    c=hasard(1,lab.c)-1; 
  } while (lab.map[l][c]!=' ');
  lab.map[l][c]=type_bonus;
  asprintf(&tmp,"bonus %c ajouté en (%d,%d)",type_bonus,l,c);
  send_message(tmp); free(tmp);
}

void retrecissement(){
  int n; char *tmp;
  while (lab.map[retL][retC]=='X') {
    if (retDir==0) {//ret vers l'est
      if (retC<retCsup) retC++;
      else {retDir=1; retLinf++;}
    } else if (retDir==1) {//ret vers le sud
      if (retL<retLsup) retL++;
      else {retDir=2; retCsup--;}
    } else if (retDir==2) {//ret vers l'ouest
      if (retC>retCinf) retC--;
      else {retDir=3; retLsup--;}
    } else if (retDir==3) {//ret vers le sud
      if (retL>retLinf) retL--;
      else {retDir=0; retCinf++;}
    } 
  }
  //gestion de la disparition d'un joueur
  if (joueurID(lab.map[retL][retC])) {
      n=get_index_player_by_id(lab.map[retL][retC]);
      asprintf(&tmp,"%s est emmuré!", listePlayer[n].login);
      send_message(tmp); free(tmp);
      eliminer_player(n);
    }
  lab.map[retL][retC]='X';
  lab.bombe[retL][retC]=0;
  lab.qui[retL][retC]=0;
}

void *synchronizer(void *arg){
  int i,j,k,nb; 
  struct timespec tm; 
  tm.tv_sec=DELAI_TIMER_SECOND;       /* secondes     */
  tm.tv_nsec=DELAI_TIMER_NANO;      /* nanosecondes */
  tps=0;
  while (tps<DUREE) {
    nanosleep(&tm, NULL);
    if (etat==ETAT_PLAYING) {
      printf(".");fflush(stdout);
      pthread_mutex_lock(&verrouPlayer);
      etat=ETAT_SENDING;
      verifier_bombes();
      
      //envoi des labyrinthes aux players
      for (i=0; i<nbPlayer; i++) {
	if (listePlayer[i].dejaJoue) {
	  //si le joueur a joue on lui envoie la map
	  listePlayer[i].dejaJoue=0;
	  MSG_MASTER_PLAYER_MAP(listePlayer[i].sock, debug);
	  sendLaby(lab, listePlayer[i].sock, 1);
	}
      }

      //ajout eventuel de bonus
      if (hasard(0,1000)<PROBA_BONUS_STD) add_bonus(TYPE_BONUS_STD);
      if (hasard(0,1000)<PROBA_BONUS_EXPLODE) add_bonus(TYPE_BONUS_EXPLODE);
      if (hasard(0,1000)<PROBA_BONUS_BOMB) add_bonus(TYPE_BONUS_BOMB);
      if (hasard(0,1000)<PROBA_BONUS_TELE) add_bonus(TYPE_BONUS_TELE);

      //envoi du labyrinthe aux viewers
      pthread_mutex_lock(&verrouViewer);
      for (i=0; i<nbViewer; i++) {
	write(sockViewer[i], "INFO", 4);
	sendLaby(lab,sockViewer[i],0 );
      }
      //envoyer boum au viewer ...
      nb=0;
      for (i=0; i<lab.l; i++)  for (j=0;j<lab.c; j++) nb+=boum[i][j];
      if (nb>0) for (k=0; k<nbViewer; k++) {
	  write(sockViewer[k], "BOOM", 4);
	  for (i=0; i<lab.l; i++)
	    for (j=0;j<lab.c; j++) write(sockViewer[k], &(boum[i][j]) ,1);
	}
      
      pthread_mutex_unlock(&verrouViewer);   
      //envoi des scores au viewer
      compter_points();
      etat=ETAT_PLAYING;
      pthread_mutex_unlock(&verrouPlayer);

      tps++;
      if (DELAI_RETRECISSEMENT>0) DELAI_RETRECISSEMENT--;
      if (DELAI_RETRECISSEMENT==0) retrecissement();
      if (nbPlayer==NB_JOUEUR_MINI) {tps=DUREE;}
    }
    tm.tv_sec=DELAI_TIMER_SECOND;       /* secondes     */
    tm.tv_nsec=DELAI_TIMER_NANO;      /* nanosecondes */
  }
  send_message("Fin de la partie");
   etat=ETAT_STOP;
  for (i=0; i<nbPlayer; i++) 
    MSG_MASTER_PLAYER_GAME_OVER(listePlayer[i].sock, debug);

  //envoi du labyrinthe aux viewers
  pthread_mutex_lock(&verrouViewer);
  for (i=0; i<nbViewer; i++) {
    write(sockViewer[i], "INFO", 4);
    sendLaby(lab,sockViewer[i],0 );    
  }
  pthread_mutex_unlock(&verrouViewer);   
  //envoi des scores au viewer
  compter_points();
  for (i=0; i<nbViewer; i++) {
    MSG_MASTER_VIEWER_QUIT(sockViewer[i], debug); 
    CS_close(sockViewer[i]);
  }
  for (i=0; i<nbPlayer; i++) {
    MSG_MASTER_PLAYER_QUIT(listePlayer[i].sock, debug); 
    CS_close(listePlayer[i].sock);
  }
  afficher_scores();
  return NULL;
}

//le joueur est identifié par son n° dans la liste
void play(int i, char order){
  int l,c,trouve=0, new_l, new_c, ii,jj, ll, cc, teleport=0;
  char *tmp;
  //recherche de la position dans le laby
  
  l=0;
  do {
    c=0;
    while ((c<lab.c)&&(lab.map[l][c]!=listePlayer[i].id)) c++;
    if (c==lab.c) {l++;} else {trouve=1;}
  } while ((l<lab.l)&&(trouve==0));
  if (trouve==0) { //ca ne doit pas pouvoir etre possible?
    printf("Impossible de trouver %s (id=%c)\n", listePlayer[i].login, listePlayer[i].id);
    return;
  }

  if ((order=='N')||(order=='S')||(order=='E')||(order=='O')){
    new_l=nextL(l,c,order); new_c=nextC(l,c,order);
    if ((lab.bombe[new_l][new_c]==0)&&(explosable(lab.map[new_l][new_c]))) {
      //points pour avoir trouve un bonus
      if (lab.map[new_l][new_c]==TYPE_BONUS_STD) {
	asprintf(&tmp, "Bonus POINT pour %s",listePlayer[i].login);
	send_message(tmp); free(tmp);
	listePlayer[i].pts+=POINTS_BONUS;
      }
      if (lab.map[new_l][new_c]==TYPE_BONUS_BOMB) {
	asprintf(&tmp, "Bonus BOMBES pour %s",listePlayer[i].login);
	send_message(tmp); free(tmp);
	listePlayer[i].pts+=POINTS_BONUS_BASE;
	listePlayer[i].bomb+=BOMB_BONUS;
      }
      if (lab.map[new_l][new_c]==TYPE_BONUS_TELE) {
	asprintf(&tmp, "Bonus TELEPORT par %s",listePlayer[i].login);
	send_message(tmp); free(tmp);
	listePlayer[i].pts+=POINTS_BONUS_BASE;
	teleport=1;	
      }
      if (lab.map[new_l][new_c]==TYPE_BONUS_EXPLODE) {
	asprintf(&tmp, "Bonus EXPLOSE par %s",listePlayer[i].login);
	send_message(tmp); free(tmp);
	for (ii=0; ii<lab.l; ii++)
	  for (jj=0; jj<lab.c; jj++)
	    if (lab.bombe[ii][jj]>1) lab.bombe[ii][jj]=1;
	//listePlayer[i].pts+=POINTS_BONUS;
	listePlayer[i].pts+=POINTS_BONUS_BASE;
      }
      listePlayer[i].pts+=POINTS_MOVE; //points pour un mouvement reussi
      //deplacement
      lab.map[l][c]=' ';
      lab.map[new_l][new_c]=listePlayer[i].id;
      if (teleport) {
	do {
	  ll=hasard(1,lab.l)-1;
	  cc=hasard(1,lab.c)-1; //printf("%d %d %c\n",l,c,listePlayer[i].id);
	} while (lab.map[ll][cc]!=' ');
	int tele=hasard(1, nbPlayer-1)-1;
	for (ii=0; ii<lab.l; ii++)
	  for (jj=0; jj<lab.c; jj++)
	    if (lab.map[ii][jj]==listePlayer[tele].id) lab.map[ii][jj]=' ';
	lab.map[ll][cc]=listePlayer[tele].id;
	asprintf(&tmp, "%s teleporté en (%d, %d)",listePlayer[tele].login,ll,cc);
	send_message(tmp); free(tmp);
      }
    }
  } else if (order=='B'){
    if ((listePlayer[i].bomb>0)&&(lab.bombe[l][c]==0)) {
      lab.bombe[l][c]=TIME_BEFORE_EXPLODE;
      listePlayer[i].pts+=POINTS_BOMB;
      asprintf(&tmp,"Bombe placee par %s",listePlayer[i].login);
      send_message(tmp); free(tmp);
      listePlayer[i].bomb--;
      lab.qui[l][c]=listePlayer[i].id;
    }
  }
}
  
void sendInfoPlayers(){
  char nb=nbPlayer, c=0;
  int i,j;
  pthread_mutex_lock(&verrouViewer);
  for (i=0; i<nbViewer; i++) {
    write(sockViewer[i], "LIST", 4);
    write(sockViewer[i], &nb, 1);
    for (j=0; j<nbPart; j++) {
      write(sockViewer[i], &(listePlayer[j].id), 1);
      write(sockViewer[i], listePlayer[j].login, strlen(listePlayer[j].login));
      write(sockViewer[i], &c, 1);
    }
  }
  pthread_mutex_unlock(&verrouViewer);
}

void init_partie(){
  int i,l,c,j;
  boum=(char **)malloc(lab.l*sizeof(char *));
  for (i=0; i<lab.l; i++)
    boum[i]=(char *)malloc(lab.c*sizeof(char));

  nb_elimines=0;
  for (i=0; i<nbPlayer; i++) {
    listePlayer[i].pts=NB_POINTS_INIT;
    listePlayer[i].bomb=NB_BOMB_INIT;
    listePlayer[i].dejaJoue=0;
    listePlayer[i].vies=NB_VIES_INIT;
    //placement aleatoire dans le laby
    do {
      l=hasard(1,lab.l)-1;
      c=hasard(1,lab.c)-1; //printf("%d %d %c\n",l,c,listePlayer[i].id);
    } while (lab.map[l][c]!=' ');
    lab.map[l][c]=listePlayer[i].id;
  }
  
  sendInfoPlayers();
  for (i=0; i<nbViewer; i++) {
    write(sockViewer[i], "INFO", 4);
    sendLaby(lab, sockViewer[i], 0);
  }
  //initialisation des bombes
  for (i=0; i<lab.l; i++) for (j=0; j<lab.c; j++) lab.bombe[i][j]=0;
  
  //initialisations pour le retrecissement
  retLinf=0; retLsup=lab.l-1; retCinf=0; retCsup=lab.c-1; 
  retC=0; retL=0; retDir=0;
}

struct player *get_elimines(){
  return liste_elimines;
}

int get_nb_elimines(){
  return nb_elimines;
}

void partie_init_delai(){
  //delai pour la synchronisation
  DELAI_TIMER_SECOND=DELAI_TIMER_SECOND_INIT;
  //delai avant retrecissement
  if (MORT_SUBITE)
    DELAI_RETRECISSEMENT=DELAI_RETRECISSEMENT_INIT;
  else
    DELAI_RETRECISSEMENT=DUREE+1;
  printf("delai %d\n",DELAI_RETRECISSEMENT);
}
