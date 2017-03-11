import java.util.Scanner;
import java.util.*;
import java.util.ArrayList;

public class PlayerJ {

    public static String ordres="NSEOB";
    public int nbLig;
    public int nbCol;
    public Network net;
    public char ID;
    public String login;
    public Position posJoueur;

    public char[][] newlaby;
    public char[][] laby;
    public int [][] bombes;
    public boolean mortSubite;
    public Random generateur;

    public PlayerJ (String h, int p, String l) {
	net = new Network();
	net.connectTo(h,p);
	login=l;

	generateur=new Random();

    }

    public void lireSpecLaby(){
	try{
	    ID = net.getChar();
	    nbLig  = net.getInt();
	    nbCol  = net.getInt();
	    String typePartie=net.getString();
	    mortSubite="ms".equals(typePartie);
	    laby= new char [nbLig][nbCol];
	    bombes= new int [nbLig][nbCol];
	}
	catch (Exception e) {
	    System.err.println(e);
	    System.exit(0);
	}
    }
    public void lireLaby(){
	try{
	    for (int i=0;i<nbLig;i++)
		for (int j=0;j<nbCol;j++){
		    char c=net.getChar();
        if (c == ID)
        {
          posJoueur = new Position(i, j);
        }
		    laby[i][j]=c;
		    bombes[i][j]=0;
		}
	    // lecture bombes
	    int nbBombes=net.getInt();
	    for (int i=0;i<nbBombes;i++){
		int l=net.getInt();
		int c=net.getInt();
		bombes[l][c]=1;
	    }
	}
	catch (Exception e) {
	    System.err.println(e);
	}
    }

    public void printSpecLaby(){
	System.out.println("taille : "+ nbLig + " x " + nbCol);
	System.out.println("ID : "+ ID);

    }

    public void printLaby(){
	int i,j;
	System.out.print("+");
	for (j=0; j<nbCol; j++)
	    System.out.print("-");
	System.out.print("+\n");
	for (i=0; i<nbLig; i++) {
	    System.out.print("|");
	    for (j=0; j<nbCol; j++)
		System.out.print(laby[i][j]);
	    System.out.print("|\n");
	}
	System.out.print("+");
	for (j=0; j<nbCol; j++)
	    System.out.print("-");
	System.out.print("+\n");
    }

    public void printBombes(){
	System.out.print("liste des bombes : ");
	for (int i=0; i<nbLig; i++)
	    for (int j=0; j<nbCol; j++)
		if  (bombes[i][j]==1)
		    System.out.print("("+i+","+j+") ");
	System.out.println();
    }

    public  void connexionServeur(){
	String msg = "LOGIN "+ login;
	System.out.println("Message sent: "+ msg);
	net.sendString(msg);
	try {
	    String s = net.getString();
	    if (s.startsWith("SPEC")) {
		System.out.println("Connexion ok");
		System.out.println("Reception taille laby!");
		lireSpecLaby();
		printSpecLaby();
		if (mortSubite)
		    System.out.println("partie mort subite");
		else
		    System.out.println("partie standard");
	    }
	    else{
		System.out.println("Pb de connexion!!!\n message reçu "+s);
		net.disconnect();
		return;
	    }
	}
	catch (Exception e) {
	    System.err.println(e);
	    System.exit(0);
	}

    }
    public void demarrer(String login){
	boolean encore = true;
	while(encore) {
	    try {
		String s = net.getString();
		System.out.println("reçu : "+s);
		if (s.startsWith("MAP")) {
		    System.out.println("Reception map!");
		    lireLaby();
		    printLaby();
		    printBombes();
        ArrayList<ArrayList<Character>> cheminsListe = parcoursProfondeur(laby, posJoueur, new ArrayList<ArrayList<Character>>(),new ArrayList<Character>());
		    char ordre=ordres.charAt(generateur.nextInt(5));
		    net.sendChar(ordre);
		}
		else if (s.startsWith("DEAD") || s.startsWith("QUIT")){
		    System.out.println("Fin de la partie! serveur a dit "+s);
		    encore=false;
		}
		else
		    System.out.println("Ordre inattendu : "+s);
	    }
	    catch (Exception e) {
		System.err.println(e);
		System.exit(0);
	    }
	}
	System.out.println("Déconnexion Master");
	net.disconnect();
    }



    // public void parcoursProfondeur(char[][] laby, Position[] posNSEO, char[] chemin)
    // {
    //   for(Position p: Position[] posNSEO)
    //   {
    //     if ()
    //   }
    //   pour tout sommet t voisin du sommet s
    //         si t n'est pas marqué alors
    //                explorer(G, t);
    // }

    public void transformLaby()
    {
      newlaby = new char[nbLig][nbCol];
      for(char[] lElemlaby : newlaby)
      {
        for(char elemlaby : lElemlaby)
        {
          elemlaby = '0';

        }
      }

    }

    public ArrayList<ArrayList<Character>> parcoursProfondeur(char[][] laby, Position p, ArrayList<ArrayList<Character>> chemins, ArrayList<Character> chemin)
    {

      newlaby[p.getX()][p.getY()] = '1';
      int jAdverse = -1;
      char jAdverseChar = ' ';
      try
      {
        jAdverse = (int)laby[p.x][p.y-1];
        jAdverseChar = (char)jAdverse;
      }
      catch(Exception e)
      {

      }

      try{
        if (laby[p.x][p.y-1] != 'X' && newlaby[p.x][p.y-1] != '1' && ((posJoueur.y - (p.y-1) <= 4 && posJoueur.y >=p.y-1)||((p.y-1) - posJoueur.y<= 4 && p.y-1 <= posJoueur.y)))
        {
          chemin.add('O');

          if (laby[p.x][p.y-1] != 'X' && laby[p.x][p.y-1] != ' ' && laby[p.x][p.y-1] != jAdverseChar)
          {
            chemins.add(new ArrayList<Character>(chemin));
          }
          chemins = parcoursProfondeur(newlaby, new Position(p.x, p.y-1), chemins, chemin);
        }
      }
      catch(Exception e)
      {

      }
      try{
        if (laby[p.x][p.y+1] != 'X' && newlaby[p.x][p.y+1] != '1' && ((posJoueur.y - (p.y+1) <= 4 && posJoueur.y >=p.y+1)||((p.y+1) - posJoueur.y<= 4 && p.y+1 <= posJoueur.y)))
        {
          chemin.add('E');
          if (laby[p.x][p.y+1] != 'X' && laby[p.x][p.y+1] != ' ' && laby[p.x][p.y+1] != jAdverseChar)
          {
            chemins.add(new ArrayList<Character>(chemin));
          }
          chemins = parcoursProfondeur(newlaby, new Position(p.x, p.y+1), chemins, chemin);
        }
      }
      catch(Exception e)
      {

      }
      try{
        if (laby[p.x-1][p.y] != 'X' && newlaby[p.x-1][p.y] != '1' && ((posJoueur.x - (p.x-1) <= 4 && posJoueur.x >=p.x-1)||((p.x-1) - posJoueur.x<= 4 && p.x-1 <= posJoueur.x)))
        {
          chemin.add('S');
          if (laby[p.x-1][p.y] != 'X' && laby[p.x-1][p.y] != ' ' && laby[p.x-1][p.y] != jAdverseChar)
          {
            chemins.add(new ArrayList<Character>(chemin));
          }
          chemins = parcoursProfondeur(newlaby, new Position(p.x-1, p.y), chemins, chemin);
        }
      }
      catch(Exception e)
      {

      }
      try{
        if (laby[p.x+1][p.y] != 'X' && newlaby[p.x+1][p.y] != '1' && ((posJoueur.x - (p.x+1) <= 4 && posJoueur.x >=p.x+1)||((p.x+1) - posJoueur.x<= 4 && p.x+1 <= posJoueur.x)))
        {
          chemin.add('N');
          if (laby[p.x+1][p.y] != 'X' && laby[p.x+1][p.y] != ' ' && laby[p.x+1][p.y] != jAdverseChar)
          {
            chemins.add(new ArrayList<Character>(chemin));
          }
          chemins = parcoursProfondeur(newlaby, new Position(p.x+1, p.y), chemins, chemin);
        }
      }
      catch(Exception e)
      {

      }


      return chemins;
    }



    public static void main(String [] args) {
	PlayerJ p = new PlayerJ(args[0], Integer.parseInt(args[1]),args[2]);
	p.connexionServeur();
	p.demarrer(args[2]);
    }
}
