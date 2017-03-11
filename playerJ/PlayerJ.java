import java.util.Scanner;
import java.util.*;

public class PlayerJ {

    private static String ordres="NSEOB";
    private int nbLig;
    private int nbCol;
    private Network net;
    private char ID;
    private String login;
    private Position posJoueur;

    private char[][] laby;
    private int [][] bombes;
    private boolean mortSubite;
    private Random generateur;

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
        char dirBomb = checkBomb();
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

    public char[][] parcoursProfondeur(char[][] laby, Position p, ArrayList<ArrayList<char>> chemins)
    {
      char[][] newlaby = laby.copy();
      for(char elemlaby : newlaby)
      {
        elemlaby = '0';
      }
      newlaby[p.getX()][p.getY()] = '1';

      try{
        if (laby[p.x][p.y-1] != 'X' && newlaby[p.x][p.y-1] != '1')
        {
          if (laby[p.x][p.y-1] == 'B' || )
          parcoursProfondeur(newlaby, new Position(p.x, p.y-1))
        }
      }
      catch(Execption e)
      {

      }
      try{
        if (laby[p.x][p.y+1] != 'X' && newlaby[p.x][p.y+1] != '1')
        {
          parcoursProfondeur(newlaby, new Position(p.x, p.y+1))
        }
      }
      catch(Execption e)
      {

      }
      try{
        if (laby[p.x][p.y-1] != 'X' && newlaby[p.x-1][p.y] != '1')
        {
          parcoursProfondeur(newlaby, new Position(p.x-1, p.y))
        }
      }
      catch(Execption e)
      {

      }
      try{
        if (laby[p.x][p.y-1] != 'X' && newlaby[p.x+1][p.y] != '1')
        {
          parcoursProfondeur(newlaby, new Position(p.x+1, p.y))
        }
      }
      catch(Execption e)
      {

      }


      return newlaby;
    }

    public void innondation()
    {
      char[][] zoneLabyJ;
      for(int x = -4; x < 5; x++)
      {
        for(int y = -4; y < 5; y++)
        {
          ++cpt;
          zoneLabyJ[x][y] = laby[x][y];
          System.out.println("x"+x+"y"+y);

        }
      }
      System.out.println(cpt);
    }

    public char isBomb(int x,int y)
    {

    }



    public static void main(String [] args) {
	PlayerJ p = new PlayerJ(args[0], Integer.parseInt(args[1]),args[2]);
	p.connexionServeur();
	p.demarrer(args[2]);
    }
}
