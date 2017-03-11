
import javax.swing.JFrame;
import java.awt.Color;
import java.awt.Font;
import java.awt.Canvas;
import java.awt.Graphics;
import java.awt.BorderLayout;
import java.awt.FontMetrics;
import java.lang.Thread;



public class ViewerFrame extends JFrame {
	   
    private javax.swing.JPanel general;
    private Graphique grap;
    private String title;
    private int ligne, col, sizze, nbMessages, maxMesg, nbScores, maxScores;
    private char map[][]=null;
    private int bomb[][]=null, dess;
    private String messages[], scores[];

    //tableau représentant l'état du monde (valeurs 0, 1 ou 2)

    /*---------------------------------------------------------------------------------------
     * Constructeur: initialisation des paramètre et construction du JPanel general
     * -titre : titre de la fenêtre
     * -taille_ : nombre plots de chaque coté du monde
     * -carre_ : taille de chaque carrés, en pixels
     */
    public ViewerFrame(String titre, int ligne_, int col_)  throws ViewerException {
	super();
	ligne=ligne_;
	col=col_;
	sizze=20;
	title=titre;
	nbMessages=0;
	maxMesg=8; dess=0;
	messages = new String[maxMesg];
	maxScores=20;
	scores = new String[maxScores];
	nbScores=0;
	for (int i=0; i<maxMesg; i++) messages[i]=" ";
	this.setVisible(true); 

	grap=new Graphique();
	general= new javax.swing.JPanel();
	general.setLayout(new BorderLayout());
	general.add(grap, "Center");
	this.setContentPane(general);

	Graphics gg=this.getGraphics();
	this.paintAll(gg);
	this.setSize(col*sizze+40+250,ligne*sizze+50);
	this.setTitle(title);
	
	//System.out.println("aa");
	//dessine();
	//test();

    }

    /*
     * Affiche l'etat d'une cellle à partir de la valeur du tableau etat
     *   -(i,j) : position dans la grille
     * si (i,j) est en dehors du tableau, ne fait rien
     */
    private void drawState(int i, int j, char state, int bomb) {
	if (!((i<0)||(j<0)||(i>=ligne)||(j>=col))) {
	    Graphics g=grap.getGraphics();
	    int a,b;
	    a=11+j*sizze;
	    b=11+i*sizze;
	    if (state=='X') {
		g.setColor(new Color(0.4f, 0.4f, 0.4f));g.fillRect(a, b, sizze-1, sizze-1);
	    } else if (state=='x') {
		g.setColor(new Color(0.7f, 0.7f, 0.7f));g.fillRect(a, b, sizze-1, sizze-1);
	    } else if (state=='$') {
		g.setColor(new Color(0.0f, 0.0f, 0.8f));g.fillRect(a, b, sizze-1, sizze-1);
	    } else if (state=='%') {
		g.setColor(new Color(0.7f, 0.0f, 0.7f));g.fillRect(a, b, sizze-1, sizze-1);
	    } else if (state=='#') {
		g.setColor(new Color(0.8f, 0.8f, 0.0f));g.fillRect(a, b, sizze-1, sizze-1);
	    } else if (state=='@') {
		g.setColor(new Color(0.0f, 0.8f, 0.0f));g.fillRect(a, b, sizze-1, sizze-1);
	    } else if (state=='*') {//bombe
		g.setColor(new Color(0.9f, 0.3f, 0.3f));g.fillRect(a, b, sizze-1, sizze-1);
	    } else if (bomb>0) {
		g.setColor(new Color(0.8f, 0f, 0f));g.fillRect(a, b, sizze-1, sizze-1);
	    } else {
		g.setColor(new Color(0.9f, 0.9f, 0.9f));g.fillRect(a, b, sizze-1, sizze-1);
	    } ;
	    if ((state!='X')&&(state!='x')&&(state!=' ')&&(state!='*'))
		drawAvatar(i,j,state);
	}
    }

 
    


    /*
     * Affichage d'un avatar dans le tableau des avatars:
     *  - (i,j) : position de l'avatar dans la grille
     * si (i,j) est en dehors du tableau, ne fait rien
     */
    private void drawAvatar(int i, int j, char av)  {
	String avn=""+av;

	Graphics g=grap.getGraphics();
	g.setColor(new Color(0.0f, 0.0f, 0.0f));

	//fonte dont la taille depend de la taille des carres
	g.setFont(new Font("Arial", Font.BOLD, sizze-1));
	
	//pour centrer le caractere dans la cellule
	FontMetrics fm = g.getFontMetrics();
	int msg_width = fm.stringWidth(avn);	// largeur du texte
	int ascent = fm.getMaxAscent ();        // pixel haut
	int descent= fm.getMaxDescent ();       // pixel bas
	
	//calcule de la position et affichage
	int x,y;
	x=(int)(11+j*sizze+(sizze-msg_width)/2);
	y=(int)(11+i*sizze+sizze-1-(sizze-(ascent-descent))/2);
	g.drawString(avn, x, y);
    }
    private void drawScore(boolean on)  {
	Graphics g=grap.getGraphics();
	if (on) {
	    g.setColor(new Color(0.9f, 0.0f, 0.0f));
	} else {
	    g.setColor(new Color(0.9f, 0.9f, 0.9f));
	}
	g.setFont(new Font("Arial", Font.BOLD, 14));
	
	//calcule de la position et affichage
	int x,y;
     
	x=col*sizze+20;
	for(int i=0; i<nbScores; i++) {
	    //y=(int)(11+(ligne+1)*sizze+i*11);
	    y=30+14*i;
	    g.drawString(scores[i], x, y);
	}
    }
    private void drawMessages(boolean on)  {
	int x,y,i;
	Graphics g=grap.getGraphics();
	if (on) {
	    g.setColor(new Color(0.f, 0.0f, 0.9f));
	} else {
	    g.setColor(new Color(1f, 1f, 1f));
	}
	g.setFont(new Font("Arial", Font.BOLD, 12));
	
	//calcule de la position et affichage
	x=col*sizze+20;
	for( i=0; i<maxMesg; i++) {
	    //y=(int)(11+(ligne+1)*sizze+i*11);
	    y=ligne*sizze+50-11*(maxMesg+3)+11*i;
	    g.drawString(messages[i], x, y);
	}
    }

    //test si un caractère est un identifiant de joueur
    private int joueurID(char c){
	if ((c!='X')&&(c!='x')&&(c!=' ')&&(c!='$')&&(c!='%')) return 1;
	return 0;
    }
    //test si une cellule est explosable
    private boolean explosable(char c){
	if ((c==' ')||(c=='$')||(c=='%')) return true;
	return false;
    }


    /*---------------------------------------------------------------
     * dessin de la grille, en fonction de taille et carre
     */
    private void dessineGrille()  {
	Graphics g=grap.getGraphics();
	int i,j;

	//grille principale en noir
	g.setColor(new Color(0f, 0f, 0f));
	for(i=0; i<=ligne; i++)
	    g.drawLine(10, 10+i*sizze, 10+sizze*col, 10+i*sizze);
	for(i=0; i<=col; i++)
	    g.drawLine(10+i*sizze, 10, 10+i*sizze, sizze*ligne+10);
	
	if (dess==0) {
	    g.setColor(new Color(1f, 1f, 1f));
	    g.fillRect(col*sizze+15, ligne*sizze+30-11*(maxMesg+3) , 260, 11*maxMesg+20);
	    dess=1;
	}

    }

    //dessine tous les etats de cellules
    private void dessineEtat()  {
	for (int i=0; i<ligne; i++) for(int j=0; j<col; j++) drawState(i,j,map[i][j], bomb[i][j]);
    }

    //methode qui dessine le score
    public void dessinePts(char c[], int pts[], int nb[], int i, char playerId[], String playerPseudo[]){
    //public void dessinePts(String pts){
	int a,b;
	drawScore(false);
	nbScores=i;
	for(a=0; a<nbScores; a++) {
	    b=0; 
	    while ((playerId[b]!=c[a])&&(b<nbScores)) b++;
	    scores[a]=new String(c[a]+" = "+playerPseudo[b]+" : "+pts[a]+"pts ("+nb[a]+" bombs)");
	}	
	drawScore(true);
	repaint();
    }
    //methode qui dessine le score
    public void dessineMessages(String mes){
	drawMessages(false);
	if (nbMessages<maxMesg) 
	    messages[nbMessages++]=mes;
	else {
	    for(int i=1; i<maxMesg; i++) messages[i-1]=messages[i];
	    messages[maxMesg-1]=mes;
	}
	drawMessages(true);
	repaint();
    }

    //methode qui dessine tout; appelée par paint.
    public void dessine(){
	dessineGrille();//System.out.println("bb");
	if (map!=null) dessineEtat();//System.out.println("cc");
	drawScore(true);//System.out.println("dd");
	drawMessages(true);
    }

    private void sleepMs(int ms){
	try {
	    Thread.currentThread().sleep(ms);
	} catch (Exception e) {}
    }

    //methode qui dessine tout; appelée par paint.
    public void dessineBomb(char boom[][]){
	int i,j;
	for (i=0; i<ligne; i++) 
	    for(j=0; j<col; j++) 
		if (boom[i][j]==1) drawState(i,j,'*',0);//dessineUneBombe(i,j); 
	repaint();
	sleepMs(300);
	dessine(); 
	repaint();
    }

    public void setData(char _map[][], int _bomb[][]){
	map=_map;
	bomb=_bomb;
    }

    
        public void paint(Graphics g) {
	    //System.out.println("repaint...");

    }

class Graphique extends Canvas {
    int tempo=1;

    public Graphique(){
	super();
	this.setBackground(new Color(0.9f, 0.9f, 0.9f));
    }
       public void paint(Graphics g) {
	   //System.out.println("repaint2...");
	/*	    try{
		Thread.sleep(500);
		} catch (Exception e){};*/
	dessine();
	//System.out.println("done!2");
    }
}


}
