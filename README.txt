README

install manuelle à faire:

1-Dans le master tapez la commande make pour compiler le serveur de jeu
2-Dans le dossier viewer, tapez la commande javac SimpleViewer.java pour compiler le viewer
3-Suivant le langage que vous avez choisi pour votre IA suivez les instructions suivantes:
  a) langage C: dans le répertoire playerC tapez la commande make
  b) langage C++: dans le répertoire playerC tapez la commande make
  c) JAVA: dans le répertoire playerJ tapez la commande javac PlayerJ.java
  d) Python: il n'y a rien à faire.
  
4- Dans un terminal, lancez le serveur en spécifiant le fichier contenant le plan du labyrinthe, le port et le type de partie (ms=mort subite, st=standard). Par exemple:
   ./master map1 9090 ms
5- Dans un autre terminal, lancez le viewer en tapant la commande
   java SimpleViewer localhost 9090
6-Lancer le player correspondant au langage choisi en spécifiant le nom d'hote serveur (ou 
l'ip), le port serveur et un pseudo:
   a) langage C:  ./player localhost 9090 tarzan
   b) langage C++: ./player localhost 9090 tarzan
   c) JAVA: java PlayerJ localhost 9090 tarzan
   d) Python: ./player.py localhost 9090 tarzan
7-Il y a quelques commandes en ligne pour le serveur (help), par exemple 
"start" démarre une partie. A partir de ce moment, le temps s'écoule 
(durée modifiable dans le code master/gestionPartie.c, #define DUREE en 
secondes, pour l'instant 180).
8- Pour implémenter votre IA vous n'avez qu'à modifier le fichier player de votre langage préféré.
9- Le script jouer servira à lancer votre player lors des matchs. Vous devez décommenter les lignes qui concernent votre player (suivant le langage utilisé). Faites bien attention à ce que votre programme se lance bien à partir de ce script.

