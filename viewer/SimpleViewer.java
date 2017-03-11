import java.net.*;
import java.io.*;

public class SimpleViewer{

    static char map[][],boom[][],playerId[];
    static int bomb[][];
    static int nbPlayers;
    static String playerPseudo[];
    


    public static void connexionViewer(DataOutputStream to_server, DataInputStream from_server) throws Exception{
	byte message[]; String login="lview"; int i;
	message=new byte[10];
    
	for (i=0; i<login.length(); i++) message[i]=(byte)login.charAt(i);
	to_server.write(message, 0, login.length()); //connexion
    }
    
    public static int getInt(DataInputStream in) throws Exception{
	int i; byte message[]=new byte[2];
	in.read(message, 0, 1);
	i=message[0];
	return i;
    }
    public static int getInt4(DataInputStream in) throws Exception{
	int i; 
	i=in.readInt();
	return i;
    }
    public static char getChar(DataInputStream in) throws Exception{
	char c; byte message[]=new byte[2];
	in.read(message, 0, 1);
	c=(char)message[0];
	return c;
    }

    public static String getString(DataInputStream in) throws Exception{
	String s; byte message[]=new byte[1000]; int i=0, encore=1;
	while (encore==1) {
	    in.read(message, i, 1);
	    if ((message[i]==0)||(i==999)) encore=0;
	    i++;
	}
	s=new String(message);
	return s.substring(0,i);
    }

     public static void getLaby(int ligne, int col, DataInputStream in){
	int i,j, nbombs, x,y;
	try{ //lecture map
	    for (i=0; i<ligne; i++){
		for(j=0; j<col; j++){
		    map[i][j]=getChar(in); 	
		    //System.out.print(map[i][j]);
		}
		//System.out.println("");
	    }
	    //lecture nb bombes
	    nbombs=getInt(in);
	    // System.out.print("nb bombes : "+nbombs);
	    for (i=0; i<ligne; i++)
		for(j=0; j<col; j++) bomb[i][j]=0;
	    for (i=0; i<nbombs; i++) {
		x=getInt(in);
		y=getInt(in);
		bomb[x][y]=1;
		//	System.out.print("("+x+','+y+") ");
	    }
	    //System.out.println(" ");
	}
	catch(Exception e){System.out.println(e);}
    }

     public static void getBoom(int ligne, int col, DataInputStream in){
	int i,j, nbombs, x,y;
	try{ //lecture map
	    for (i=0; i<ligne; i++)
		for(j=0; j<col; j++){
		    boom[i][j]=getChar(in);
		}
	}
	catch(Exception e){System.out.println(e);}
     }

public static void main(String[] args) throws Exception{
    //test();

    ViewerFrame vf;
    DataOutputStream to_server;
    String line, dest;
    int i=1, ligne, col, taille, carre,j,k,nbombs, posBomb[]=new int[250], encore=1;
    Boolean mortSubite;
    
    BufferedReader br = new BufferedReader(
	                        new InputStreamReader(System.in));
    try{
    	Socket socket = new Socket(args[0],Integer.parseInt(args[1]));
        to_server = new DataOutputStream (
                          socket.getOutputStream());

	DataInputStream in =
	    new DataInputStream(socket.getInputStream());

	byte message[];	
	System.out.println("Connecté...");
	message=new byte[10];

	connexionViewer(to_server, in);
	System.out.println("login ok...");

	//reception taille du laby
	ligne=getInt(in);
	col=getInt(in);

	//reception du type de partie
	String typePartie=getString(in);
	mortSubite="ms".equals(typePartie);
	    
	
	map=new char[ligne][col];
	boom=new char[ligne][col];
	bomb=new int[ligne][col];
	String typeP;
	if (mortSubite){
	    typeP=" : Mort Subite";
	    System.out.println("Partie Mort Subite");
	}
	else{
	    typeP=" : Partie Standard";
	    System.out.println("Partie Standard");
	}
	System.out.println("Taille : "+ligne+ "x" +col+"...");

	getLaby(ligne, col, in);

	System.out.println(" Map OK!");

	vf=new ViewerFrame("Viewer..."+ligne+ "x" +col+typeP, ligne, col);
	System.out.println("Viewer initialisé");
	vf.setData(map, bomb);
	vf.repaint();
	while (encore>0){
	    in.read(message, 0, 4);
	    message[4]=0;
	    String recu=new String(message);
	    //System.out.println("cmd : "+recu);
	    if (recu.substring(0,4).equals("INFO")) {
		    //lire laby
		getLaby(ligne, col, in);
		vf.setData(map, bomb);
		vf.dessine();vf.repaint();
	    } else if (recu.substring(0,4).equals("BOOM")) {
		    //afficher explosions
		/*nbombs=getInt(in);
		  for (i=0; i<2*nbombs; i++) posBomb[i]=getInt(in);*/
		getBoom(ligne, col, in);
		vf.dessineBomb(boom);
	    } else if (recu.substring(0,4).equals("MESG")) {
		
		String msg=getString(in);
		//		System.out.println("Mesg : "+msg);
		vf.dessineMessages(msg);
	    } else if (recu.substring(0,4).equals("LIST")) {
		//récupérer liste des joueurs (Id et pseudo)
		System.out.println("Liste des joueurs --------------- ");
		nbPlayers=getInt(in);
		playerId=new char[nbPlayers];
		playerPseudo=new String[nbPlayers];
		for (i=0; i<nbPlayers; i++){
		    playerId[i]=getChar(in);
		    playerPseudo[i]=getString(in);
		    System.out.println("  "+playerId[i]+" : "+ playerPseudo[i]);
		}
		System.out.println("-------------------------------- ");
	    } else if (recu.substring(0,4).equals("PTS.")) {
		    //afficher explosions
		i=getInt4(in);
		char c[]=new char[i]; int pts[]=new int[i], nb[]=new int[i];
		for (j=0; j<i; j++){
		    c[j]=getChar(in); pts[j]=getInt4(in); nb[j]=getInt4(in);
		    //System.out.println(c[j]+" : "+pts[j]+" ("+nb[j]+")");
		}
		vf.dessinePts(c, pts, nb, i, playerId, playerPseudo);
	    } 
	}
	to_server.close();
	socket.close();
	vf.dispose();
	System.out.println("Serveur déconnecté!");
    }
     catch(Exception e){System.out.println(e);}
     }
}


