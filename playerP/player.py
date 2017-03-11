#! /usr/bin/python3
import time
import random
import sys
import os
from mySocket import *

ordres=['N','S','E','O','B']
laby=None
bombes=None
ID=-1
mortSubite=False
nbLig=-1
nbCol=-1

def printSpecLaby(nbLig,nbCol,ID):
    print('taille :',nbLig,'x',nbCol)
    print('ID :',ID)
            
def printLaby(laby):
    nbCol=len(laby[0])
    ligne='+'+'-'*nbCol+'+'
    print(ligne)
    for i in range(len(laby)):
        print('|',end='')
        for j in range(nbCol):
            print(laby[i][j],end='')
        print('|')
    print(ligne)

def printBombes(bombes):
    print("liste des bombes : ",end='')
    for i in range(len(bombes)):
        for j in range(len(bombes[0])):
            if bombes[i][j]==1:
                print("(",i,",",j,") ",sep='',end='')
    print()
                
def lireSpecLaby(net):
    ID=socket.getChar()  
    nbLig=socket.getInt()
    nbCol=socket.getInt()
    typePartie=socket.getString()
    mortSubite=typePartie=="ms"
    laby=[]
    bombes=[]
    for i in range(nbLig):
        laby.append([' ']*nbCol)
        bombes.append([0]*nbCol)
    return ID,nbLig,nbCol,mortSubite,laby,bombes


def lireLaby(net,nbLig,nbCol,laby,bombes):
    for i in range(nbLig):
        for j in range(nbCol):
            laby[i][j]=net.getChar()
            bombes[i][j]=0
    nbBombes=net.getInt()
    for i in range(nbBombes):
        lig=net.getInt()
        col=net.getInt()
        bombes[lig][col]=1
        
def connexionServeur(net,login):
    socket.send("LOGIN "+login)
    typeMsg=net.getString()
    print("Reçu :",typeMsg)
    if typeMsg=="SPEC": 
        print('connexion réussie')
        print('Reception taille laby!')
        ID,nbLig,nbCol,mortSubite,laby,bombes=lireSpecLaby(net)
        printSpecLaby(nbLig,nbCol,ID)
        if mortSubite:
            print("partie mort subite")
        else:
            print("partie standard")
    else:
        print("Problème de connexion!!!")
        print("message reçu ",typeMsg)
        net.close()
        sys.exit(1)
    return ID,nbLig,nbCol,mortSubite,laby,bombes
    


def demarrer(net,ID,nbLig,nbCol,mortSubite,laby,bombes):
    fini=False
    while(not fini):
        typeMsg=net.getString()
        print("Reçu :",typeMsg)
        if typeMsg=='MAP':
            lireLaby(net,nbLig,nbCol,laby,bombes)
            printLaby(laby)
            printBombes(bombes)
            # ici remplacer par le code de votre IA
            ordre=random.choice(ordres)
            print('Ordre envoyé',ordre)
            net.send(ordre)
        elif typeMsg in ['QUIT','DEAD']:
            print("fin de la partie",typeMsg)
            fini=True
        else:
            print('message du serveur inattendu',typeMsg)
    net.close()
        
        
opts=sys.argv[1:]
socket=MySocket()
socket.connect(opts[0],int(opts[1]))
ID,nbLig,nbCol,mortSubite,laby,bombes=connexionServeur(socket,opts[2])
demarrer(socket,ID,nbLig,nbCol,mortSubite,laby,bombes)

