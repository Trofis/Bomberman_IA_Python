import java.net.*;
import java.io.*;

public class Network {

    public Socket          sock;
    public DataInputStream   in;
    public DataOutputStream out;

    /**
     * Construit un objet de type Network
     *
     */
    public Network() {
	sock=null;
	in  =null;
	out =null;
	// to be explicit
    }

    /**
     * Ouvre une connexion à un serveur
     * @param host nom du serveur hote
     * @param port (entier) sur lequel se connecter
     * @return 0 si la connexion a réussi, 1 sinon
     */
    public int connectTo(String host, int port) {
	try {
	    sock = new Socket(host, port);
	    in = new  DataInputStream(sock.getInputStream());
	    out= new DataOutputStream(sock.getOutputStream());
	    return 0;
	}
	catch (Exception e) {
	    System.err.println(e);
	    return 1;
	}
    }

    /**
     * Ferme la connexion avec le serveur
     * @return 0 si la déconnexion a réussi, 1 sinon
     */
    public int disconnect() {
	try {
	    sock.close();
	    return 0;
	}
	catch (Exception e) {
	    System.err.println(e);
	    return 1;
	}
    }

    /**
     * Lit un entier envoyé par le serveur
     * @return l'entier recu du serveur
     * @throws Exception
     */
    public int getInt() throws Exception {
        int i;
	byte message[]=new byte[2];
        in.read(message, 0, 1);
        i=message[0];
        return i;
    }

    /**
     * Lit un caractère envoyé par le serveur
     * @return le caractère recu du serveur
     * @throws Exception
     */
    public char getChar() throws Exception {
        char c;
	byte message[]=new byte[2];
        in.read(message, 0, 1);
        c=(char)message[0];
        return c;
    }

    /**
     * Lit une chaine de caractères envoyée par le serveur
     * @return la chaine recue du serveur
     * @throws Exception
     */
    public String getString() throws Exception {
        String s; byte message[]=new byte[1000]; int i=0, encore=1;
        while (encore==1) {
            in.read(message, i, 1);
            if ((message[i]==0)||(i==999)) encore=0;
            i++;
        }
        s=new String(message);
        return s.substring(0,i);
    }

    /**
     * Envoie un caractère au serveur
     * @param c le caractère à envoyer
     * @return 0 si l'envoi a réussi, 1 sinon
     */
    public int sendChar(char c) {
	try {
	    byte message[]=new byte[2];
	    message[0] = (byte)c;
	    out.write(message, 0, 1);
	    return 0;
	}
	catch (Exception e) {
	    System.err.println(e);
	    return 1;
	}
    }

    /**
     * Envoie une chaîne de caractères au serveur
     * @param s la chaîne à envoyer
     * @return 0 si l'envoi a réussi, 1 sinon
     */
    public int sendString(String s) {
	try {
	    int i = 0;
	    byte message[]=new byte[s.length()+1];
	    for (i=0; i<s.length(); i++)
		message[i]=(byte)s.charAt(i);
	    message[i] = (byte)'\u0000';
	    out.write(message, 0, s.length()+1);
	    return 0;
	}
	catch (Exception e) {
	    System.err.println(e);
	    return 1;
	}
    }
}
