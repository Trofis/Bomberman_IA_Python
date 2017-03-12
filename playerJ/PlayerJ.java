import java.util.Scanner;
import java.util.*;
import java.util.concurrent.*;
import java.lang.*;


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
    e.printStackTrace();
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
    e.printStackTrace();
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
	for (int i=0; i<nbLig; i++){
    for (int j=0; j<nbCol; j++){
      try{
        if  (bombes[i][j]==1){
          System.out.print("("+i+","+j+") ");
        }
      }
      catch(IndexOutOfBoundsException ie){
        ie.printStackTrace();
        System.out.println("pb mon gars");
      }
    }
  }
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
    this.newlaby = new char[nbLig][nbCol];
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
    e.printStackTrace();
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
        transformLaby();
        ArrayList<ArrayList<Character>> cheminsListe = parcoursProfondeur(posJoueur, new ArrayList<ArrayList<Character>>(),new ArrayList<Character>());
        ArrayList<Chemin> cheminsAmeliore = checkParticulariteChemins(cheminsListe);
        char destination = direction(cheminsAmeliore);
		    //char ordre=ordres.charAt(generateur.nextInt(5));
		    net.sendChar(destination);
		}
		else if (s.startsWith("DEAD") || s.startsWith("QUIT")){
		    System.out.println("Fin de la partie! serveur a dit "+s);
		    encore=false;
		}
		else
		    System.out.println("Ordre inattendu : "+s);
	    }
	    catch (Exception e) {
        e.printStackTrace();
		System.err.println(e);
		System.exit(0);
	    }
	}
	System.out.println("Déconnexion Master");
	net.disconnect();
    }



    // public void parcoursProfondeur(char[][] Position[] posNSEO, char[] chemin)
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
      for(int x=0;x<nbLig;x++)
      {
        for(int y=0;y<nbLig;y++)
        {
          this.newlaby[x][y] = '0';

        }
      }

    }

    public ArrayList<ArrayList<Character>> parcoursProfondeur(Position p, ArrayList<ArrayList<Character>> chemins, ArrayList<Character> chemin)
    {
      for(char[] lElemlaby : this.newlaby)
      {
        for(char elemlaby : lElemlaby)
        {
          System.err.println(elemlaby);

        }
      }
      this.newlaby[p.x][p.y] = '1';
      int jAdverse = -1;
      char jAdverseChar = ' ';
      try
      {
        if((int)p.y-1 == -1){
          p.y = nbCol-1;
        }
        int posY= p.y-1;
        jAdverse = (int)laby[p.x][posY];
        jAdverseChar = (char)jAdverse;
      }
      catch(Exception e)
      {
        System.out.print("f-1");
        e.printStackTrace();
      }

      try{
        if((int)p.y-1 == -1){
          p.y = nbCol-1;
        }
        int posY= p.y-1;
        if (
        laby[p.x][posY] != 'X' &&
        newlaby[p.x][posY] != '1' &&
        ((posJoueur.y - posY >= 4 &&
        posJoueur.y >=posY)||(posY - posJoueur.y<= 4 &&
         posY <= posJoueur.y)))
        {
          chemin.add('O');

          if
          (laby[p.x][posY] != 'X' &&
           laby[p.x][posY] != ' ' &&
           laby[p.x][posY] != jAdverseChar)
          {
            chemins.add(new ArrayList<Character>(chemin));
          }
          chemins = parcoursProfondeur(new Position(p.x, posY), chemins, chemin);
        }
      }
      catch(Exception e)
      {
        System.out.println("first");
        e.printStackTrace();

      }

      try{
        if((int)p.y+1 == nbCol){
          p.y = 0;
        }
        int posY =p.y+1;
        if (
        laby[p.x][posY] != 'X' &&
        newlaby[p.x][posY] != '1' &&
        ((posJoueur.y - posY <= 4 &&
        posJoueur.y >=posY)||(posY - posJoueur.y<= 4 &&
        posY <= posJoueur.y)))
        {
          chemin.add('E');
          if (
          laby[p.x][posY] != 'X' &&
          laby[p.x][posY] != ' ' &&
          laby[p.x][posY] != jAdverseChar)
          {
            chemins.add(new ArrayList<Character>(chemin));
          }
          chemins = parcoursProfondeur(new Position(p.x, posY), chemins, chemin);
        }
      }
      catch(Exception e)
      {
        System.out.println("s");
        e.printStackTrace();
      }
      try{
        if((int)p.x-1 == -1){
          p.x = nbLig-1;
        }
        int posX = p.x-1;
        if (
        laby[posX][p.y] != 'X' &&
        newlaby[posX][p.y] != '1' &&
        ((posJoueur.x - posX <= 4 &&
        posJoueur.x >= posX)|| (posX - posJoueur.x<= 4 &&
        posX <= posJoueur.x)))
        {
          System.out.println(laby[(p.x-1+nbLig)%nbLig][p.y]);
          chemin.add('N');
          if (
          laby[posX][p.y] != 'X' &&
           laby[posX][p.y] != ' ' &&
           laby[posX][p.y] != jAdverseChar)
          {
            chemins.add(new ArrayList<Character>(chemin));
          }
          chemins = parcoursProfondeur(new Position(posX, p.y), chemins, chemin);
        }
      }
      catch(Exception e)
      {
        System.out.println("third");
        e.printStackTrace();
      }
      try{
        if((int)p.x+1 == nbLig){
          p.x = 0;
        }
        int posX = p.x+1;
        if (
        laby[posX][p.y] != 'X' &&
        laby[posX][p.y] != '1' &&
        ((posJoueur.x - posX <= 4 &&
        posJoueur.x >=posX)||(posX - posJoueur.x<= 4 &&
        posX <= posJoueur.x)))
        {
          chemin.add('S');
          if (
          laby[posX][p.y] != 'X' &&
          laby[posX][p.y] != ' ' &&
          laby[posX][p.y] != jAdverseChar
          )
          {
            chemins.add(new ArrayList<Character>(chemin));
          }
          chemins = parcoursProfondeur(new Position(posX, p.y), chemins, chemin);
        }
      }
      catch(Exception e)
      {
        System.out.println("fourth");
        e.printStackTrace();
      }
      return chemins;
    }

    public ArrayList<Chemin> checkParticulariteChemins(ArrayList<ArrayList< Character>> chemins){
      ArrayList<Chemin> lesChemins = new ArrayList<Chemin>();
      int x = this.posJoueur.getX();
      int y = this.posJoueur.getY();
      System.out.println("chemin length"+chemins.size());
      for (ArrayList<Character> chemin :chemins)
        {
          boolean bombe = false;
          boolean mur = false;
          boolean bonusBombe = false;
          boolean bonusExplo = false;
          boolean bonusPoint = false;
          boolean bonusTP = false;
          int cpt = 0;
          for(char caseContaint :  chemin)
          {
            //Traitement verification des bonus
            // System.out.println(caseContaint);
            try{
              char caselaby = laby[(x+nbLig)%nbLig][(y+nbCol)%nbCol];
              if(caselaby =='%'){
                bonusBombe=true;
              }
              else if(caselaby =='$'){
                bonusPoint=true;
              }
              else if(caselaby =='#'){
                bonusExplo=true;
              }
              else if(caselaby =='@'){
                bonusTP=true;
              }
            }
            catch(ArrayIndexOutOfBoundsException e){
              System.out.println("x"+(x+nbLig)%nbLig);
              System.out.println("y"+(y+nbCol)%nbCol);
              System.out.println("x2"+x);
              System.out.println("y2"+y);


              System.out.println("nbcol"+nbCol);
              System.out.println("nblig"+nbLig);
              e.printStackTrace();
            }
            // Traitement verification des chemins
            try{
              if(bombes[(x+nbLig)%nbLig][(y+nbCol)%nbCol]==1){
                bombe =true;
              }
              // Ajout dans le chemin
              else if(caseContaint=='N'){
                y++;
              }
              else if(caseContaint=='S'){
                y--;
                if(y<0){
                  y = nbCol-1;
                }
              }
              else if(caseContaint=='E'){
                x++;
              }
              else if(caseContaint=='O'){
                if(x<0){
                  y = nbLig-1;
                }
              }
            }
            catch(Exception e){
              System.out.println("bug au try de flo2");
              e.printStackTrace();
            }
            cpt++;
          }
          if(!bombe && !mur){
            System.out.println("ajout d'un element");
            System.out.println(bonusBombe);
            System.out.println(bonusExplo);
            System.out.println(bonusTP);
            System.out.println(bonusPoint);
            // try{
            //   TimeUnit.SECONDS.sleep(2);
            // }
            // catch(InterruptedException e1){
            //   e1.printStackTrace();
            // }
            lesChemins.add(new Chemin(chemin,bonusBombe,bonusExplo,bonusPoint,bonusTP,(x+nbLig)%nbLig,(x+nbCol)%nbCol,cpt));
          }
        }
      return lesChemins;
    }


      public boolean checkBombes(int x, int y){
        //False if not bomb
        boolean boom=false;
        // while(!boom){
          for(int i=0; i<nbLig; i++){
            if (bombes[i][y]==1){
              boom = true;
            }
          }
          for(int i=0; i<nbCol; i++){
            if (bombes[x][i]==1){
              boom = true;
            }
          }

        // }
        return boom;
      }



      public char direction(ArrayList<Chemin> chemins){
        for(Chemin chemin : chemins ){
          if(chemin.bonusPoints ){
            System.out.println("pref");
              return chemin.liste.get(0);
          }
          else if(chemin.bonusBombe){
              System.out.println("pre2f");
              return chemin.liste.get(0);
          }
          else if(chemin.bonusTP ){
              System.out.println("pre3f");
              return chemin.liste.get(0);
          }
          else if(chemin.bonusExplosion){
            System.out.println("pre4f");
              return chemin.liste.get(0);
          }
        }
        // for(Chemin chemin : chemins ){
        //   if(chemin.bonusPoints && !checkBombes(chemin.x,chemin.y)){
        //       return chemin.liste.get(0);
        //   }
        //   else if(chemin.bonusBombe && !checkBombes(chemin.x,chemin.y)){
        //       return chemin.liste.get(0);
        //   }
        //   else if(chemin.bonusTP && !checkBombes(chemin.x,chemin.y)){
        //       return chemin.liste.get(0);
        //   }
        //   else if(chemin.bonusExplosion && !checkBombes(chemin.x,chemin.y)){
        //       return chemin.liste.get(0);
        //   }
        // }
        // System.out.println(chemins.get(0).liste.charAt(0));
        int posX =0;
        int posY =0;
        int posXP =0;
        int posYP =0;
        if(chemins.size() == 0){
          System.out.println("Le chemin est archi vide");
          if (this.posJoueur.x-1 <0){
            posX = this.nbCol -1 ;
          }
          else{
            posX = this.posJoueur.x-1;
          }
          if (this.posJoueur.y-1 <0){
            posY = this.nbLig -1;
          }
          else{
            posY = this.posJoueur.y-1;
          }
          if(this.posJoueur.y+1 == nbCol){
            posYP = 0;
          }
          else{
            posYP=this.posJoueur.y+1;
          }
          if(this.posJoueur.x+1 == nbLig){
            posXP = 0;
          }
          else{
            posXP = this.posJoueur.x+1;
          }
          System.out.println("poxY 0"+posY);
          System.out.println("posX"+posX);
          ArrayList<Character> decideur = new ArrayList<Character>();

          int partUnFinX = nbLig /2;
          // int partDeuxDebutX = (nbLig /2 +1);
          int partUnFinY = nbCol /2;
          // int partDeuxDebutY = (nbCol /2 +1);

          if(this.posJoueur.x<partUnFinX){
            //on oublie O reste NSE
            if(this.posJoueur.y< partUnFinY){
              //on oublie nord
              // On choisi sud ou est
              if(laby[posXP][this.posJoueur.y] != 'X'){
                decideur.add('S');
              }
              if(laby[this.posJoueur.x][posYP] != 'X'){
               decideur.add('E');
              }
            }
            else{
              if(laby[posXP][this.posJoueur.y] != 'X'){
                decideur.add('S');
              }
              if(laby[this.posJoueur.x][posY] != 'X'){
                decideur.add('O');
              }

            }
          }

          else{
            if(this.posJoueur.y< partUnFinY){
              //on oublie nord
              // On choisi nord est
              if(laby[this.posJoueur.x][posYP] != 'X'){
               decideur.add('E');
              }
              if(laby[posX][this.posJoueur.y] != 'X'){
                decideur.add('N');
              }

            }
            else{
              // on choisi nord ouest
              if(laby[posX][this.posJoueur.y] != 'X'){
                decideur.add('N');
              }
              if(laby[this.posJoueur.x][posY] != 'X'){
                decideur.add('O');
              }
            }
          }
          System.err.println("ee"+decideur.size());
          if(decideur.size() ==0){

            return ordres.charAt(generateur.nextInt(5));
          }
          int choisirCentreRandom = generateur.nextInt(5);
          char res =' ';
          if(choisirCentreRandom == 1){
            res =  decideur.get(generateur.nextInt(decideur.size()));
          }
          else{
            if(laby[posXP][this.posJoueur.y] != 'X'){
              decideur.add('S');
            }
            if(laby[this.posJoueur.x][posY] != 'X'){
              decideur.add('O');
            }
            if(laby[this.posJoueur.x][posYP] != 'X'){
             decideur.add('E');
            }
            if(laby[posX][this.posJoueur.y] != 'X'){
              decideur.add('N');
            }
            res =  decideur.get(generateur.nextInt(decideur.size()));

          }
          return res;

        }
        return chemins.get(0).liste.get(0);

      }
        // public void innondation()
        // {
        //   char[][] zoneLabyJ;
        //   for(int x = -4; x < 5; x++)
        //   {
        //     for(int y = -4; y < 5; y++)
        //     {
        //       ++cpt;
        //       zoneLabyJ[x][y] = laby[x][y];
        //       System.out.println("x"+x+"y"+y);
        //
        //     }
        //   }
        //   System.out.println(cpt);
        // }



        public static void main(String [] args) {
          try{
            // ArrayList<Character> al = new ArrayList<Character>();
            // al.add('a');
            // al.add('b');
            // al.add('c');
            // al.add('d');
            // Chemin c1 = new Chemin(al,false,false,false,false,0,0,1);
            // Chemin c2 = new Chemin(al,false,false,false,false,0,0,1);
            // ArrayList<Chemin> alc = new ArrayList<Chemin>();
            // alc.add(c1);
            // alc.add(c2);
            // System.out.println(alc.get(0).liste.get(0));
            PlayerJ p = new PlayerJ(args[0], Integer.parseInt(args[1]),args[2]);
            p.connexionServeur();
            p.demarrer(args[2]);
          }
          catch(Exception e){
            System.out.print("main");
            e.printStackTrace();
          }
        }
}
