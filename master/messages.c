#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "clientserver.h"

int send_string(int sock, char *s){
  return put_bytes(sock, s, strlen(s)+1);
}

int send_1arg(int sock, char *echo, char *msg, int debug){
  if (debug) {printf("    -->%s (%d)\n", echo, sock); }
  return send_string(sock, msg);
}

// MASTER -> Viewer
int MSG_MASTER_VIEWER_QUIT(int sock, int debug){
  return send_1arg(sock, "MSG_MASTER_VIEWER_QUIT", "QUIT", debug); }



//MSATER -> Player
int MSG_MASTER_PLAYER_QUIT(int sock, int debug){
  return send_1arg(sock, "MSG_MASTER_PLAYER_QUIT", "QUIT", debug);}

int MSG_MASTER_PLAYER_ERR(int sock, int debug){
  return send_1arg(sock, "MSG_MASTER_PLAYER_ERR", "ERR", debug);}

int MSG_MASTER_PLAYER_ERR_LOGIN(int sock, int debug){
  return send_1arg(sock, "MSG_MASTER_PLAYER_ERR_LOGIN", "ERR LOGIN UTILISE", debug);}

int MSG_MASTER_PLAYER_CONNEXION_IMPOSSIBLE(int sock, int debug){
  return send_1arg(sock, "MSG_MASTER_PLAYER_CONNEXION_IMPOSSIBLE", "ERR_CONNEXION", debug);}

int MSG_MASTER_PLAYER_MAP(int sock, int debug){
  return send_1arg(sock, "MSG_MASTER_PLAYER_MAP", "MAP", debug);}

int MSG_MASTER_PLAYER_SPEC(int sock, int debug){
  return send_1arg(sock, "MSG_MASTER_PLAYER_SPEC", "SPEC", debug);
}
int MSG_MASTER_PLAYER_ID(int sock, char id, int debug){
  if (debug) {printf("    -->MSG_MASTER_PLAYER_ID %c %d \n", id, sock); }
  return write(sock, &id, 1);
}

int MSG_MASTER_PLAYER_IGNORE(int sock, int debug){
  return send_1arg(sock, "MSG_MASTER_PLAYER_IGNORE", "IGNORE", debug);}

int MSG_MASTER_PLAYER_DEJA_JOUE(int sock, int debug) {
  return send_1arg(sock, "MSG_MASTER_PLAYER_DEJA_JOUE", "DEJA_JOUE", debug);}

int MSG_MASTER_PLAYER_GAME_OVER(int sock, int debug) {
  return send_1arg(sock, "MSG_MASTER_PLAYER_DEJA_JOUE", "GAME_OVER", debug);}

int MSG_MASTER_PLAYER_DEAD(int sock, int debug) {
  return send_1arg(sock, "MSG_MASTER_PLAYER_DEJA_JOUE", "DEAD", debug);}

















void aff_gid(int gid) {
  unsigned char *p, r,v,b, letter; 
  p=(unsigned char*)&gid;
  letter=*((char*)p);  r=*(p+1);  v=*(p+2);  b=*(p+3);  
  printf("gid(%c, %d, %d, %d)", letter, r, v, b);
}

void MSG_CODE(int sock, unsigned char com) {
    write(sock,&com,1); 
}
void MSG_CODE_GID(int sock, unsigned char com, int gid) {
  write(sock,&com,1); 
  write(sock, &gid, 4);   
}
void MSG_AVATAR_MOV(unsigned char code, int sock, int gid, unsigned char i, unsigned char j) {
  write(sock, &code, 1);
  write(sock, &gid, 4);   
  write(sock, &i, 1); 
  write(sock, &j, 1);
}
void MSG_PLOT_LOGIN(int sock, unsigned char i, unsigned char j) {
  unsigned char com=30;  //commande de login
  write(sock, &com, 1);
  write(sock, &i, 1); 
  write(sock, &j, 1);
}


//----------------------------------------------------------------
//MASTER -> PLOT
//----------------------------------------------------------------
void MSG_MASTER_PLOT_AVATAR_SET(int sock, int gid, unsigned char i, unsigned char j, int debug) {
  unsigned char code=0;
  write(sock, &code, 1);
  write(sock, &gid, 4);   
  write(sock, &i, 1); 
  write(sock, &j, 1);
  if (debug) {printf("    -->MSG_MASTER_PLOT_AVATAR_SET = %d  (socket %d)", code ,sock); aff_gid(gid); printf(" %d %d\n",i,j);}
}
void MSG_MASTER_PLOT_OK(int sock, int debug) {
  MSG_CODE(sock, 1);
  if (debug) printf("    -->MSG_MASTER_PLOT_OK = 1  (socket %d)\n",sock); 
}  
void MSG_MASTER_PLOT_ERR(int sock, int debug) {
  MSG_CODE(sock, 2);
  if (debug) printf("    -->MSG_MASTER_PLOT_ERR = 2  (socket %d)\n",sock); 
}
void MSG_MASTER_PLOT_AVATAR_CLR(int sock, int gid, int debug) {
  MSG_CODE_GID(sock, 3, gid);
  if (debug) printf("    -->MSG_MASTER_PLOT_AVATAR_CLR = 3  (socket %d)\n",sock); 
}
void MSG_MASTER_PLOT_AVATAR_MOV(int sock, int gid, char dir, int debug) {
  unsigned char code=4;
  write(sock, &code, 1);
  write(sock, &gid, 4);   
  write(sock, &dir, 1); 
  if (debug) {printf("    -->MSG_MASTER_PLOT_AVATAR_MOV = %d  (socket %d)", code ,sock); aff_gid(gid); printf(" %c\n",dir);}
}
void MSG_MASTER_PLOT_AVATAR_BOMB(int sock, int gid, int debug){
  MSG_CODE_GID(sock, 5, gid);
  if (debug) printf("    -->MSG_MASTER_PLOT_AVATAR_BOMB = 5  (socket %d)\n",sock); 
}
void MSG_MASTER_PLOT_EVENT_SENDALL(int sock, int debug){
  MSG_CODE(sock, 6);
  if (debug) printf("    -->MSG_MASTER_PLOT_EVENT_SENDALL = 6  (socket %d)\n",sock); 
}
void MSG_MASTER_PLOT_AVATAR_JMP(int sock, int gid, unsigned char i, unsigned char j, int debug) {
  unsigned char code=7;
  write(sock, &code, 1);
  write(sock, &gid, 4);   
  write(sock, &i, 1); 
  write(sock, &j, 1);
  if (debug) {printf("    -->MSG_MASTER_PLOT_AVATAR_JMP = %d  (socket %d)", code ,sock); aff_gid(gid); printf(" %d %d\n",i,j);}}

//----------------------------------------------------------------
//MASTER -> PLAYER
//----------------------------------------------------------------
void MSG_MASTER_PLAYER_OK_LOGIN(int sock, int gid, int debug) {
  unsigned char r=0; //code OK/LOGIN
  write(sock,&r,1); 
  write(sock, &gid, 4); 
  if (debug) {printf("    -->MSG_MASTER_PLAYER_OK_LOGIN = %d  (socket %d)", 0,sock); aff_gid(gid); printf("\n");}

}
void MSG_MASTER_PLAYER_OK(int sock, int debug) {
  MSG_CODE(sock, 1);
  if (debug) printf("    -->MSG_MASTER_PLAYER_OK = 1  (socket %d)\n",sock); 
}


void MSG_MASTER_PLAYER_BOMB(int sock, int debug) {
  MSG_CODE(sock, 3);
  if (debug) printf("    -->MSG_MASTER_PLAYER_BOMB = 3  (socket %d)\n", sock); 
}
void MSG_MASTER_PLAYER_GOODBYE(int sock, int debug) {
  MSG_CODE(sock, 4);
  if (debug) printf("    -->MSG_MASTER_PLAYER_GOODBYE = 4  (socket %d)\n", sock); 
}


//----------------------------------------------------------------
//PLAYER -> MASTER
//----------------------------------------------------------------
 void MSG_PLAYER_MASTER_LOGIN(int sock, unsigned char i, unsigned char j){
  unsigned char com=10;  //commande de login
  write(sock, &com, 1);
  write(sock, &i, 1); 
  write(sock, &j, 1);
}
void MSG_PLAYER_MASTER_AVATAR_MOV(int sock, int gid, char dir)  {
  unsigned char code=11;
  write(sock, &code, 1);
  write(sock, &gid, 4);   
  write(sock, &dir, 1); 
}
void MSG_PLAYER_MASTER_AVATAR_BOMB(int sock, int gid) {MSG_CODE_GID(sock, 12, gid);}
void MSG_PLAYER_MASTER_LOGOUT(int sock, int gid) {MSG_CODE_GID(sock, 13, gid);}


//----------------------------------------------------------------
//PLOT -> MASTER
//----------------------------------------------------------------
void MSG_PLOT_MASTER_LOGIN(int sock, unsigned char i, unsigned char j){
  unsigned char com=20;  //
  write(sock, &com, 1);
  write(sock, &i, 1); 
  write(sock, &j, 1);
}
void MSG_PLOT_MASTER_OK(int sock, int gid) {MSG_CODE_GID(sock, 21, gid);}
void MSG_PLOT_MASTER_ERR(int sock, int gid) {MSG_CODE_GID(sock, 22, gid);}
void MSG_PLOT_MASTER_EVENT_CELL_STATE(int sock, unsigned char i, unsigned char j, unsigned char state){
  unsigned char com=23;  //
  write(sock, &com, 1);
  write(sock, &i, 1); 
  write(sock, &j, 1);
  write(sock, &state, 1);
 }
void MSG_PLOT_MASTER_EVENT_AVATAR_SET(int sock, int gid, unsigned char i, unsigned char j){
  unsigned char com=24;  //
  write(sock, &com, 1);
  write(sock, &gid, 4);
  write(sock, &i, 1); 
  write(sock, &j, 1);
 }
void MSG_PLOT_MASTER_AVATAR_JMP(int sock, int gid, unsigned char i, unsigned char j){
  unsigned char com=25;  //
  write(sock, &com, 1);
  write(sock, &gid, 4);
  write(sock, &i, 1); 
  write(sock, &j, 1);
 }
void MSG_PLOT_MASTER_BOMB(int sock, int gid) {MSG_CODE_GID(sock, 26, gid);}



//----------------------------------------------------------------
//MASTER->VIEWER
//----------------------------------------------------------------
void MSG_MASTER_VIEWER_EVENT_CELL_STATE(int sock, unsigned char i, unsigned char j, unsigned char state) {
  unsigned char com=3;  //
  write(sock, &com, 1);
  write(sock, &i, 1); 
  write(sock, &j, 1);
  write(sock, &state, 1);
 }
void MSG_MASTER_VIEWER_EVENT_AVATAR_SET(int sock, int gid, unsigned char i, unsigned char j) {
  unsigned char com=4;  //commande de login
  write(sock, &com, 1);
  write(sock, &gid, 4);
  write(sock, &i, 1); 
  write(sock, &j, 1);
 }
