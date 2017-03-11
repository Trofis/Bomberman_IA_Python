#include "libnetwork.hpp"
#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>



class Player{
private:
  Network net;
  
public:
  int nbLig,nbCol;
  char ** laby;
  int ** bombes;
  char ID;
  bool mortSubite;
  std::string ordres;
  std::string login;
  

  Player(const std::string& host, int port, const std::string& _login){
    ordres="SNEOB";
    net=Network();
    net.connectTo(host,port);
    login=_login;
    srand(time(NULL)); 
  }
  ~Player(){
    std::cout<<"Deconnexion du serveur"<<std::endl;
    net.disconnect();
  }

  void printSpecLaby(){
    std::cout<<"taille : "<<nbLig<<" x "<<nbCol<<std::endl;
    std::cout<<"ID : "<<ID<<std::endl;
  }

  void printLaby(){
      int i,j;
      std::cout<<"+";
      for (j=0; j<nbCol; j++) std::cout<<"-";
      std::cout<<"+"<<std::endl;

      for (i=0; i<nbLig; i++){
	std::cout<<"|";
	for (j=0; j<nbCol; j++)
	  std::cout<<laby[i][j];
	std::cout<<"|"<<std::endl;	
      }
      std::cout<<"+";
      for (j=0; j<nbCol; j++) std::cout<<"-";
      std::cout<<"+"<<std::endl;
  }
  
  void printBombes(){
    std::cout<<"liste des bombes : ";
    for (int i=0; i<nbLig; i++) 
      for (int j=0; j<nbCol; j++)
	if (bombes[i][j]==1)
	  std::cout<<"("<<i<<","<<j<<") ";
    std::cout<<std::endl;
  }

  void lireLaby(){
    for (int i=0; i<nbLig; i++) 
      for (int j=0; j<nbCol; j++) {
	laby[i][j]=net.getInt();
	bombes[i][j]=0;
      }
    int nbBombes=net.getInt();
    for (int i=0; i<nbBombes; i++){
      int l=net.getInt();
      int c=net.getInt();
      bombes[l][c]=1;
    }
  }

  void lireSpecLaby(){
    ID=net.getChar();
    nbLig=net.getInt();
    nbCol=net.getInt();
    std::string typePartie=net.getString();
    mortSubite= typePartie=="ms";
    laby=new char*[nbLig];
    bombes=new int*[nbLig];
    for (int i=0;i<nbLig;i++){
      laby[i]=new char[nbCol];
      bombes[i]=new int[nbCol];
    }
  }

  void connexionServeur(){
    net.sendString("LOGIN "+login);
    std::string reponse=net.getString();
    if (reponse=="SPEC"){
      std::cout<<"Connexion ok"<<std::endl;
      std::cout<<"Reception taille laby!"<<std::endl;
      lireSpecLaby();
      printSpecLaby();
      if (mortSubite)
	std::cout<<"partie mort subite"<<std::endl;
      else
	std::cout<<"partie standard"<<std::endl;
    }
    else{
       std::cout<<"Problème de connexion!!!"<<std::endl;
       std::cout<<"message reçu "<<reponse<<std::endl;
       exit(0);
    }
  }
  
  void demarrer(){
    bool encore=true;
    while(encore){
      std::string reponse=net.getString();
      std::cout<<"reçu : "<<reponse<<std::endl;
      if (reponse=="MAP"){
	std::cout<<"Reception map!"<<std::endl;
	lireLaby();
	printLaby();
	printBombes();
	// ici remplacer par le code de votre IA
	int r = rand()%5;
	std::cout<<"Ordre envoyé "<<ordres[r]<<std::endl;
	net.sendChar(ordres[r]);
      }
      else if (reponse=="DEAD" or reponse=="QUIT"){
	std::cout<<"Fin de la partie! serveur a dit "<<reponse<<std::endl;
	encore=false;
      }
      else
	std::cout<<"Message inattendu "<<reponse<<std::endl;
    }
    net.disconnect();
  }
};

int main(int argc, char *argv[]){
  if (argc!= 4){
    std::cerr<<"usage: %s HOST PORT LOGIN"<<std::endl;
    return -1;
  }
  int port=atoi(argv[2]);
  Player p=Player(std::string(argv[1]), port, std::string(argv[3]));
  p.connexionServeur();
  p.demarrer();
}
