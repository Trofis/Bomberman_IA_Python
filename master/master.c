#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>

#include "master.h"
#include "clientserver.h"

//gestion d'une nouvelle connexion
void *gereLogin(void *arg) {
  int gs=*((int *)arg);
  char com[100]; size_t k=0;
  k=read(gs, com, 100);// on lit la commande pour savoir qui se connecte
  printf("connexion ..."); fflush(stdout);
  if (k<5) {
    printf("message connexion trop court! -> déconnecté...\n"); 
    CS_close(gs); 
    return NULL;
  }
  if (strncmp("lview", com, 5)==0) {
    loginViewer(gs);
  } else if (strncmp("LOGIN", com, 5)==0){
    if (etat==ETAT_STOP) {
      //connexion player
      //if (k>100) k=99;
      com[k-1]=0;
      loginPlayer(gs, com);
    } else { 
      //pas de connexion de player si partie en cours
    printf("tentative de connexion en cours de partie! -> déconnecté...\n"); 
    write(gs, "Partie en cours!\n",17);
    CS_close(gs);      
    }
  } else {
    printf("message connexion non valide! -> déconnecté...\n"); 
    CS_close(gs);
  }
  return NULL;
}


int main(int args, char **argv){
  int ret;  socklen_t taille;
  struct sockaddr_in cli_addr;
  char *hostnameC;
  unsigned short portC;
  int sockgen, port, sock;
  pthread_t  login, io; 
  char c[100], *ipC;
  

  srand(time(NULL));
  debug=1; etat=ETAT_STOP;
  //Determination du mode (interactif ou non) et test d'usage
  if ((args != 1)&&(strcmp(argv[1],"-i")==0)) {//mode interactif
      printf("Port: "); scanf("%d", &port);
      printf("Affichage msg players ? (O/N): "); scanf("%s",c); 
      if ((strcmp(c,"O")==0)||(strcmp(c,"o")==0)) debug=1;
    } else if (args != 4) {
      char* name = strrchr(argv[0], '/');
      fprintf(stderr, "usage: %s FICHIER_PLOT PORT TYPE\n", (name)?(name+1):argv[0]);
      fprintf(stderr, "Le FICHIER_PLOT contient 10 lignes de 0 ou 1 (sans espace)\n");
      return -1;
    } else {
      port=atoi(argv[2]);
    }

  //lecture du fichier avec le plan du labyrinthe + affichage si OK
  ret = lireLab(argv[1], &lab);
  if (ret==-1) exit(0);
  printLab(&lab);
  nbViewer=nbPlayer=nbPart=0;

  MORT_SUBITE=strcmp(argv[3],"ms")==0;
  printf("mort subite %d\n",MORT_SUBITE);

  partie_init_delai();

  //initialisations verrous
  pthread_mutex_init(&verrouViewer, NULL);

  //thread interface utilisateur du master
  pthread_create(&io, NULL, gereUser, NULL);

  //initialisation socket server
  sockgen=CS_server(port, 5);
  if (sockgen<=0) {printf("Initialisation : echec\n"); exit(0);}
  printf("Serveur...ok\n");
  
  //boucle interaction server
  taille=sizeof(struct sockaddr_in);
  while (1) {
    sock= accept(sockgen, (struct sockaddr *) &cli_addr,&taille);
    decode_iaddr(&hostnameC,&ipC,&portC,cli_addr);
    fprintf(stderr,"%d : connexion avec %s(%s):%d\n",getpid(),hostnameC,ipC,portC);
    getIP(&ipC,cli_addr);
    fprintf(stderr,"connexion avec %s\n",ipC);
    int *param=(int *)malloc(sizeof(int));
    *param=sock; //recopie pour éviter les pb de connexions trop proches
    pthread_create(&login, NULL, gereLogin, (void *)param);

  }

  exit(0);
}
