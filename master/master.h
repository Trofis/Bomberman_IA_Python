#define MAX_PLAYER 10
#define ETAT_STOP 0
#define ETAT_SENDING 1
#define ETAT_PLAYING 2
#define ETAT_PAUSE 3

#define TYPE_BONUS_STD '$'
#define TYPE_BONUS_EXPLODE '#'
#define TYPE_BONUS_BOMB '%'
#define TYPE_BONUS_TELE '@'

//Definition du type labyrinthe
struct laby{
  char ** map;
  int l,c;
  //position des bombes et temps restant + indice de qui a place une bombe
  int **bombe; char **qui;
};

struct player{
  char *login;
  int sock;
  int pts;
  int dejaJoue;
  char id;
  int bomb;
  int vies;
};

//Variables globales
//===============================================
// flag pour specifier le mode avec affichage détaillé
int debug, etat; 
//verrous sur les liste de players et de viewers
pthread_mutex_t verrouPlayer, verrouViewer;
//labyrinthe
struct laby lab;
//listes de viewers et de players
int sockViewer[100], nbViewer, nbPlayer, nbPart;
struct player listePlayer[MAX_PLAYER];
//delai pour la synchronisation
int DELAI_TIMER_SECOND;
//delai avant retrecissement
int DELAI_RETRECISSEMENT;
//type de partie
int MORT_SUBITE;


//================================================
//gestion interface utilisateur
void *gereUser(void *arg);

//gestionViewer
void loginViewer(int sockViewer);
void *ecouteViewer(void *arg);

//gestionPlayer
void loginPlayer(int sockViewer, char *com);
void supprime_player_unlock(int sock, char *nom);
void supprimePlayer(int sock, char *nom);
void *gestionPlayer(void *arg);
int get_index_player_by_id(char id);
void sendInfoPlayers();

//utilitaires labyrinthe
int sendLaby(struct laby lab, int sock, int filtre);
int sendSpecLaby(struct laby lab, int ms, int sock);
int lireLab(char *nom, struct laby *lab);
void printLab(struct laby *lab);

//gestion du jeu et de la partie
void play(int i, char order);
void *synchronizer(void *arg);
void init_partie();
struct player *get_elimines();
int get_nb_elimines();
int joueurID(char c);
void partie_init_delai();
