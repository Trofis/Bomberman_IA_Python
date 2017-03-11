#define _GNU_SOURCE
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>

#include "libnetwork.h"


//---------------------------------------------------------------
//-------------------------------Variables globales
//identifiant du joueur
char ID;
//infos labyrinthe
char ** laby;
//les bombes
int ** bombes;
// dimensions du labyrinthe
int nbLig,nbCol;
// indicateur permmettant de savoir si la partie est standard ou mort subite
int mortSubite;
// ordres possibles
char *ordres="SNEOB";


//---------------------------------------------------------------
//-------------------------------Affichage labyrinthe ds console
//affichage dimensions
void printSpecLaby(int nbLig, int nbCol, int ID){
  printf("taille : %d x %d\nID : %c\n",nbLig,nbCol,ID);

}
//affichage labyrinthe
void printLaby(int nbLig, int nbCol, char **laby){
  int i,j;
  printf("+");
  for (j=0; j<nbCol; j++) printf("-");
  printf("+\n");
  for (i=0; i<nbLig; i++) {
    printf("|");
    for (j=0; j<nbCol; j++) printf("%c",laby[i][j]);
    printf("|\n");
  }
  printf("+");
  for (j=0; j<nbCol; j++) printf("-");
  printf("+\n");
}
//affichage de la liste des bombes
void printBombes(int nbLig, int nbCol, int **bombes){
  int i,j;
  printf("liste des bombes : ");
  for (i=0;i<nbLig;i++)
    for (j=0;j<nbCol;j++)
      if (bombes[i][j]==1)
	printf("(%d,%d) ",i,j);
  printf("\n");
}

//lecture labyrinthe
void lireLaby(int nbLig, int nbCol, char **lab,int **bombes) {
  int i=0, j,nbBombes;
  char vv, bx,by;
  for (i=0; i<nbLig; i++) 
    for (j=0; j<nbCol; j++) {
      vv=getInt();
      laby[i][j]=vv;
      bombes[i][j]=0;
    }

  //lecture bombes
  nbBombes=getInt();
  for (i=0; i<nbBombes; i++) {
    bx=getInt();
    by=getInt();
    bombes[bx][by]=1;
  }
}

//lecture taille labyrinthe + ID + type partie
int lireSpecLaby(char ***laby, int ***bombes) {
  int i=0, ligne, col, **bt;
  char *typePartie, **lt;
  

  ID=getInt();
  nbLig=getInt();
  nbCol=getInt();

  typePartie=getString();


  if (strcmp(typePartie,"ms")==0)
    mortSubite=1;
  else
    mortSubite=0;
  lt=(char **)malloc(nbLig*sizeof(char *));
  bt=(int **)malloc(nbLig*sizeof(int *));
  for (i=0; i<nbLig; i++){
    lt[i]=(char *)malloc(nbCol*sizeof(char));
    bt[i]=(int *)malloc(nbCol*sizeof(int));
  }
  *laby=lt;
  *bombes=bt;
  return 1;
}

void connexionServeur(char * login){
  char *msg;
  int val;
  //message login pour le serveur
  asprintf(&msg, "LOGIN %s", login);
  printf("%s\n",msg);
  sendString(msg);
  // reception de la réponse du serveur
  msg=getString();
  if (strncmp(msg, "SPEC",4)==0) {
    printf("Connexion ok\n");
    printf("Reception taille laby!\n");
    lireSpecLaby(&laby,&bombes);
    printSpecLaby(nbLig,nbCol,ID);
    if (mortSubite)
      printf("partie mort subite\n");
    else
      printf("partie standard\n");	
    }
  else{
    printf("Pb de connexion!!!\n message reçu %s\n",msg);
    disconnect();
    exit(0);
  }
}

void demarrer(){
  char *msg;
  int val,r,encore=1;
  
  while (encore) {
    // lecture de l'ordre
    msg=getString();
    printf("recu : %s\n",msg);
    if (strncmp(msg, "MAP",3)==0) {
      printf("Reception map!\n");
      lireLaby(nbLig,nbCol, laby, bombes);
      printLaby(nbLig,nbCol,laby);
      printBombes(nbLig,nbCol,bombes);
      // ici remplacer par le code de votre IA
      r = rand()%5;
      printf("Ordre envoyé %c\n",ordres[r]);
      sendChar(ordres[r]);

    } else if ((strncmp(msg, "DEAD",4)==0) || (strncmp(msg, "QUIT",4)==0)){
      printf("Fin de la partie! serveur a dit %s\n",msg);
      encore=0;
    }
    else if (strlen(msg)>0)
      printf("Message inattendu %s \n",msg);
    else{
      printf("déconnexion du serveur\n");
      encore=0;
    }
  }
  disconnect();
}


//---------------------------------------------------------------
//-------- main:
int main(int args, char **argv){
  int port, encore=1, r, val; 
  char *msg, buffer[100];

  srand(time(NULL)); 
  if (args != 4){
    char* name = strrchr(argv[0], '/');
    fprintf(stderr, "usage: %s HOST PORT LOGIN\n", (name)?(name+1):argv[0]);
    return -1;
  }

  port=atoi(argv[2]);
  connectTo(argv[1], port);
  connexionServeur(argv[3]);
  demarrer();
}
    
