#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "master.h"
#include "clientserver.h"

int lireLab(char *nom, struct laby *lab) {
  FILE *f=fopen(nom, "r");
  int i=0, j, ligne, col;
  char *line=NULL;
  ssize_t lu=0; size_t lire=0;

  lu=getline(&line, &lire, f);
  ligne=lab->l=atoi(line);
  if ((ligne<=0)||(ligne>1000)) { 
    printf("Format de fichier non valide : nombre de lignes = %s\n", line);
    return -1; 
  }
  line=NULL;
  lu=getline(&line, &lire, f);
  col=lab->c=atoi(line);
  if ((col<=0)||(col>1000)) { 
    printf("Format de fichier non valide : nombre de colonnes = %s\n", line);
    return -1; 
  }

  lab->map=(char **)malloc(ligne*sizeof(char *));
  for (i=0; i<ligne; i++) lab->map[i]=(char *)malloc(col*sizeof(char));

  //creation et initialisation du tableau des bombes
  lab->bombe=(int **)malloc(ligne*sizeof(int *));
  for (i=0; i<ligne; i++) {
    lab->bombe[i]=(int *)malloc(col*sizeof(int));
    for (j=0; j<col; j++) lab->bombe[i][j]=0;
  }
  //creation et initialisation du tableau des poseurs bombes
  lab->qui=(char **)malloc(ligne*sizeof(char *));
  for (i=0; i<ligne; i++) {
    lab->qui[i]=(char *)malloc(col*sizeof(char));
    for (j=0; j<col; j++) lab->qui[i][j]=0;
  }

  line=(char *)malloc(col+2);
  for (i=0; i<ligne; i++) {
    lu=getline(&line, &lire, f);
    if (lu-1!=col) { 
      printf("Format de fichier nom valide : map ligne %d = %s", i+1,line); 
      return -1; 
    }
    for (j=0; j<col; j++) lab->map[i][j]=line[j];
  }
  fclose(f);
 
  printf("Lecture fichier %s : OK\n", nom);
  return 1;
}

void printLab(struct laby *lab){
  int i,j;
  printf("+");
  for (j=0; j<lab->c; j++) printf("-");
  printf("+\n");
  for (i=0; i<lab->l; i++) {
    printf("|");
    for (j=0; j<lab->c; j++) printf("%c",lab->map[i][j]);
    printf("|\n");
  }
  printf("+");
  for (j=0; j<lab->c; j++) printf("-");
  printf("+\n");
  }

//envoie d'un labyrinthe, retourne -1 en cas d'erreur, 0 sinon
//si filtre vaut 1, les murs cassables sont masqués, i.e. envoyés comme murs
int sendLaby(struct laby lab, int sock, int filtre){
  int nbomb=0,i, j, ec; char c;
  char v;
  
    for (i=0; i<lab.l; i++)
      for (j=0;j<lab.c; j++) {
	if (lab.bombe[i][j]) nbomb++;
	c=lab.map[i][j];
	if ((filtre)&&(c=='x')) c='X'; //masquage des murs explosables
	//if (c=='o') cc=1;
	//if (c=='i') cc=3;
	ec=put_bytes(sock,&c, 1);
	if (ec<0) return -1;
      }
    //envoi du nb de bombes
    v=nbomb; //printf("%d bombes : ",v);
    ec=put_bytes(sock,&v, 1);
    if (ec<0) return -1;
    //envoi position des bombes
    if (nbomb) for (i=0; i<lab.l; i++)
      for (j=0;j<lab.c; j++) {
	if (lab.bombe[i][j]) {
	  v=1*i;
	  ec=put_bytes(sock,&v, 1); 
	  if (ec<0) return -1;
	  v=1*j;
	  ec=put_bytes(sock,&v, 1); 
	  if (ec<0) return -1;
	}
      }
    return 0;
}
//envoie la taille du labyrinthe, retourne -1 en cas d'erreur, 0 sinon
int sendSpecLaby(struct laby lab, int ms, int sock){
  int ec; 
  char vv;
    vv=1*lab.l; 
    ec=put_bytes(sock, &vv, 1); 
    if (ec<0) return -1;
    vv=1*lab.c; 
    ec=put_bytes(sock,  &vv, 1);
    if (ec<0) return -1;
    if (ms)
      ec=put_bytes(sock,"ms",3);
    else
      ec=put_bytes(sock,"st",3);
    if (ec<0) return -1;
    return 0;
}
