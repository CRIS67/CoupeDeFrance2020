import numpy as np
import matplotlib.pyplot as plt

##Bouées

###############################################################################

# 1 point par bouée valide dans le port 
# 1 point supplémentaire par bouée valide sur la ligne de chenal de la même couleur 
# 2 points par paires de bouées valides sur les lignes de chenaux d’un même port. Une paire valide est composée d’une bouée verte et d’une bouée rouge 

##Manches à air

###############################################################################

#5 points si une seule manche à air est relevée à la fin du match 
# 15 points si les deux manches à air sont relevées en fin de match 




###############################################################################
##piste
###############################################################################


x_size = (0,3000,3000,0,0)
y_size = (0,0,2000,2000,0)


##Port
port_vert_g = ((0,1085), 
             (400,1085))
x_vert_g = []
y_vert_g = []
for i in range (len(port_vert_g)):
        x_vert_g.append(port_vert_g[i][0])
        y_vert_g.append(port_vert_g[i][1])
        
port_rouge_g = ((0,515), 
             (400,515))
x_rouge_g = []
y_rouge_g = []
for i in range (len(port_vert_g)):
        x_rouge_g.append(port_rouge_g[i][0])
        y_rouge_g.append(port_rouge_g[i][1])
        
##phare
x_coord_phare_g = [225]
y_coord_phare_g = [-111]  
   
##bouées
coord_bouées_g = (
                #int
                (300,400,'r'),
                (300,1200,'g'),
                (450,510,'g'),
                (450,1080,'r'),
                (700,100,'r'),
                (950,400,'g'), 
                (1010,800,'r'),
                (1270,1200,'g'),
                (1005,1955,'r'),
                (1065,1650,'g'), 
                (1355,1650,'r'),
                (1395,1955,'g'),
        
                #ext
                (-67,1450,'r'),
                (-67,1525,'g'),
                (-67,1600,'r'), 
                (-67,1675,'g'),
                (-67,1750,'r'))

coord_bouées_d = (
                #int
                (1730,1200,'r'),
                (2000,800,'g'),
                (2050,400,'r'),
                (2330,100,'g'),
                (2550,510,'r'),
                (2550,1080,'g'), 
                (2700,400,'g'),
                (2700,1200,'r'),
                (1605,1955,'r'),
                (1665,1650,'g'), 
                (1935,1650,'r'),
                (1995,1955,'g'),
                #ext
                (3067,1450,'g'),
                (3067,1525,'r'),
                (3067,1600,'g'), 
                (3067,1675,'r'),
                (3067,1750,'g'))

## place les bouées selon leur couleur
x_red_g = []
y_red_g = []
x_green_g = []
y_green_g = []
x_red_d = []
y_red_d = []
x_green_d = []
y_green_d = []
for i in range (len(coord_bouées_g)):
    if coord_bouées_g[i][2] == 'r':
        x_red_g.append(coord_bouées_g[i][0])
        y_red_g.append(coord_bouées_g[i][1])
    else:
        x_green_g.append(coord_bouées_g[i][0])
        y_green_g.append(coord_bouées_g[i][1]) 
        
for i in range (len(coord_bouées_d)):       
    if coord_bouées_d[i][2] == 'r':
        x_red_d.append(coord_bouées_d[i][0])
        y_red_d.append(coord_bouées_d[i][1])
    else:
        x_green_d.append(coord_bouées_d[i][0])
        y_green_d.append(coord_bouées_d[i][1]) 
        
##bouées
coord_eceuil_g = (
    #ext
    (700,-67),
    (775,-67),
    (850,-67), 
    (925,-67),
    (1000,-67))

coord_eceuil_d = (
    #ext
    (2000,-67),
    (2075,-67),
    (2150,-67), 
    (2225,-67),
    (2300,-67))

x_ec_g = []
y_ec_g = []
x_ec_d = []
y_ec_d = []
for i in range (len(coord_eceuil_g)):
        x_ec_g.append(coord_eceuil_g[i][0])
        y_ec_g.append(coord_eceuil_g[i][1])
        x_ec_d.append(coord_eceuil_d[i][0])
        y_ec_d.append(coord_eceuil_d[i][1])
        
##manches à air 
coord_manches = (
                #int
                (230,2022,'off','equipe_A'),
                (635,2022,'off','equipe_A'),
                (2365,2022,'off','equipe_B'),
                (2770,2022,'off','equipe_B')
                )

x_m = []
y_m = []
for i in range (len(coord_manches)):
    x_m.append(coord_manches[i][0])
    y_m.append(coord_manches[i][1])
    
#ensembles des points d'intérêts
taches = coord_bouées_g + coord_manches   
taches_coord = list(coord_bouées_g + coord_manches) 
    


###############################################################################
##robot
###############################################################################

r = 30 #[cm]
x_coord_robot = 200
y_coord_robot = 800
v_robot = 0.24 #[m/s]
G_robot = 90 #[deg]

bras_devant = np.array((0,0,0))
bras_derriere =  np.array((0,0,0))

###############################################################################
##fonction
###############################################################################



#Calcul la distance de tous les objectifs et sélectionne le plus proches
def proximite_robot(taches):
    mini = (10000,0)
    for i in range (len(taches_coord)):
        
        taches_coord_x = taches_coord[i][0]
        taches_coord_y = taches_coord[i][1]
        
        D = np.sqrt( (taches_coord_x - x_coord_robot)**2 + (taches_coord_y - y_coord_robot)**2 ) - 120
        
        if D <= mini[0]:
            mini = (D,i)
    j = mini[1]
    plt.scatter(taches_coord[j][0], taches_coord[j][1], color='violet', s= 150 )
    return mini, taches_coord[j][0], taches_coord[j][1]


#calcul le temps de trajet voir le pts de coordonnées Xfinal, Yfinal
def temps_trajet(T,Xfinal, Yfinal):
    D = np.sqrt( (Xfinal - x_coord_robot)**2 + (Yfinal - y_coord_robot)**2 ) - 120
    T += (D/(1000*v_robot))
    return T
    
    
#Simule le temps de rotation du robot 
def tourner(T,bras_devant, bras_derriere):
    temp = bras_devant
    bras_devant = bras_derriere
    bras_derriere = temp 
    return T+5, bras_devant, bras_derriere 



#Simule la prise d'une bouée
def prendre(T, x_coord_robot, y_coord_robot, bras_devant, bras_derriere ):
    if bras_devant[0]+bras_devant[1]+bras_devant[2] ==3:
        T, bras_devant, bras_derriere = tourner(T,bras_devant, bras_derriere)
        
    if bras_devant[1]==0:
        bras_devant[1] += 1
    else:
        if bras_devant[0]==0:
            bras_devant[0] += 1
        else:
            if bras_devant[2]==0:
                bras_devant[2] +=  1
                
    if bras_devant[0]+bras_devant[1]+bras_devant[2] + bras_derriere[0]+bras_derriere[1]+bras_derriere[2] ==6:
        T, x_coord_robot, y_coord_robot, bras_devant, bras_derriere = retour_pos_depart(T, x_coord_robot, y_coord_robot, bras_devant, bras_derriere )

    return T+5, x_coord_robot, y_coord_robot, bras_devant, bras_derriere



#Simule le temps de retour à la position de départ 
def retour_pos_depart(T, x_coord_robot, y_coord_robot, bras_devant, bras_derriere):
    D = np.sqrt( (200 - x_coord_robot)**2 + (800 - y_coord_robot)**2 )
    T_retour = (D/(1000*v_robot))
    if T + T_retour <= 100:
        print(T + T_retour,"dans les temps")
    else:
        print(T + T_retour,"pas arrivé dans les temps")
    return T + T_retour, 200, 800, np.array((0,0,0)), np.array((0,0,0))


# affiche la piste
def print_piste(x_coord_robot, y_coord_robot):
    ##piste
    plt.figure()
    plt.grid()
    plt.xlabel('x')
    plt.ylabel('y') 
    plt.plot(x_size, y_size, color='black')
    plt.plot(x_vert_g, y_vert_g, color='green')
    plt.plot(x_rouge_g, y_rouge_g, color='red')
    
    ##bouées
    plt.scatter(x_red_g, y_red_g, color='red')
    plt.scatter(x_green_g, y_green_g, color='green')
    
    ##ec
    plt.scatter(x_ec_g, y_ec_g, color='grey')
    plt.scatter(x_ec_d, y_ec_d, color='grey')
    ##manches à air
    plt.scatter(x_m, y_m, color='blue', s= 115 )
    ##phare
    plt.scatter(x_coord_phare_g, y_coord_phare_g, color='blue', s= 450)
    ##robot
    plt.scatter(x_coord_robot, y_coord_robot, color='orange', s= 300 )
    return



###############################################################################
##partie
###############################################################################

#etat initial
print_piste(x_coord_robot, y_coord_robot)
T = 0 #[s]



#manches à air gauches (15pts)
#orienté
T, bras_devant, bras_derriere = tourner(T,bras_devant, bras_derriere)
print_piste(x_coord_robot, y_coord_robot)
#avancé
T = temps_trajet(T,x_coord_robot, 1880)
x_coord_robot += 0
y_coord_robot = 1880
print_piste(x_coord_robot, y_coord_robot)
#levé bras hauteur max
T += 2
print_piste(x_coord_robot, y_coord_robot)
#orienté
T, bras_devant, bras_derriere = tourner(T,bras_devant, bras_derriere)
print_piste(x_coord_robot, y_coord_robot)
#bras hauteur min
T += 2
print_piste(x_coord_robot, y_coord_robot)
#recul
T = temps_trajet(T,670, y_coord_robot)
x_coord_robot = 670
y_coord_robot += 0
print_piste(x_coord_robot, y_coord_robot)
print(T, "temps manche à air")




#drapeau (auto)

#bouées échouées 
#orienté
T, bras_devant, bras_derriere = tourner(T,bras_devant, bras_derriere)
print_piste(x_coord_robot, y_coord_robot)
#avancé
T = temps_trajet(T, 120, 1525)
x_coord_robot = 120
y_coord_robot = 1525
print_piste(x_coord_robot, y_coord_robot)
#avancé
T = temps_trajet(T,x_coord_robot, 1675)
x_coord_robot += 0
y_coord_robot = 1675
print_piste(x_coord_robot, y_coord_robot)
#orienté
T, bras_devant, bras_derriere = tourner(T,bras_devant, bras_derriere)
print_piste(x_coord_robot, y_coord_robot)
#avancé
T = temps_trajet(T, 775, 120)
x_coord_robot = 775
y_coord_robot = 120
print_piste(x_coord_robot, y_coord_robot)
#orienté
T, bras_devant, bras_derriere = tourner(T,bras_devant, bras_derriere)
print_piste(x_coord_robot, y_coord_robot)
#avancé
T = temps_trajet(T, 925, 120)
x_coord_robot = 925
y_coord_robot = 120
print_piste(x_coord_robot, y_coord_robot)
print(T, "bouées échouées ")


#Phares (10pts)
#orienté
T, bras_devant, bras_derriere = tourner(T,bras_devant, bras_derriere)
print_piste(x_coord_robot, y_coord_robot)
#avancé
T = temps_trajet(T,225, 120)
x_coord_robot = 225
y_coord_robot = 120
print_piste(x_coord_robot, y_coord_robot)
print(T, "temps phare")


#objectif le plus proche
while T < 93:
    mini, taches_coord_x, taches_coord_y = proximite_robot(taches)
    D, j = mini
    taches_coord.pop(j)
    x_coord_robot = taches_coord_x
    y_coord_robot = taches_coord_y
    T += (D/(1000*v_robot))
    T, x_coord_robot, y_coord_robot, bras_devant, bras_derriere = prendre(T, x_coord_robot, y_coord_robot, bras_devant, bras_derriere)
    print("bras_devant", bras_devant)
    print("bras_derriere", bras_derriere)
    print_piste(x_coord_robot, y_coord_robot)
    
    
T, x_coord_robot, y_coord_robot, bras_devant, bras_derriere = retour_pos_depart(T, x_coord_robot, y_coord_robot, bras_devant, bras_derriere )
print_piste(x_coord_robot, y_coord_robot)   
print(T)
plt.savefig('pist')