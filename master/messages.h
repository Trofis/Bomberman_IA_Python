
// MASTER -> Viewer
int MSG_MASTER_VIEWER_QUIT(int sock, int debug);



//MSATER -> Player
int MSG_MASTER_PLAYER_QUIT(int sock, int debug);
int MSG_MASTER_PLAYER_ERR(int sock, int debug);
int MSG_MASTER_PLAYER_ERR_LOGIN(int sock, int debug);
int MSG_MASTER_PLAYER_CONNEXION_IMPOSSIBLE(int sock, int debug);
int MSG_MASTER_PLAYER_MAP(int sock, int debug);
int MSG_MASTER_PLAYER_SPEC(int sock, int debug);
int MSG_MASTER_PLAYER_ID(int sock, char id, int debug);
int MSG_MASTER_PLAYER_IGNORE(int sock, int debug);
int MSG_MASTER_PLAYER_DEJA_JOUE(int sock, int debug);
int MSG_MASTER_PLAYER_GAME_OVER(int sock, int debug);
int MSG_MASTER_PLAYER_DEAD(int sock, int debug);
