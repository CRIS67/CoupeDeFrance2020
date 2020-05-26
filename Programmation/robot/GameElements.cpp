#include "GameElements.hpp"

double GetDistance(double x, double y, double xO, double yO) {
    return sqrt((x-xO)*(x-xO)+(y-yO)*(y-yO));
}

/*void Do(int i, double *x, double *y, double *t, GameElements Elements[]) {
    srand (time(NULL));
    *x = Elements[i].GetX();
    *y = Elements[i].GetY();
    *t += Elements[i].GetDuree()+(rand()%11-5)+GetDistance(*x,*y,Elements[i].GetX(),Elements[i].GetY())*DELTA_TEMPS;
    std::cout << "Temps : " << *t << ", action " << Elements[i].GetNom()<< std::endl;
}*/

void Do(int i, bool file, Actuator Act[]) {
    if(file) {
        //D* Star
        std::cout << "lancement en fichier" << std::endl;
        std::ifstream MyFile;
        std::string line, token, token2;
        int cpt = 0, pos, pos2;
        MyFile.open("Actions.txt");
        if(!MyFile.is_open()) {
            std::cout << "error opening Actions.txt" << std::endl;
        } else {
            while(MyFile >> line) {
                if(i-1 < cpt) {break;}
                cpt++;
            }
            std::cout << line << std::endl;
            MyFile.close();
            //Traitement
            int card, id, val, p;
            char type[1];
            while ((pos = line.find(";")) != std::string::npos) {
            	token = line.substr(0, pos);
            	p = 1;
            	while ((pos2 = token.find(":")) != std::string::npos) {
            		token2 = token.substr(0, pos2);
            		switch(p) {
            			case 1:
            				token2 = token2.substr(0,1);
                        	std::copy(token2.begin(), token2.end(), type);
                        	PRINT_DO_DEBUG("type = ");
                        	PRINT_DO_DEBUG(type[0]);
                        	PRINT_DO_DEBUG("\n");
                        	break;
                        case 2:
                        	card = stod(token2);
                        	PRINT_DO_DEBUG("carte = ");
                        	PRINT_DO_DEBUG(card);
                        	PRINT_DO_DEBUG("\n");
                        	break;
                        case 3:
                        	id = stod(token2);
                        	PRINT_DO_DEBUG("id = ");
                        	PRINT_DO_DEBUG(id);
                        	PRINT_DO_DEBUG("\n");
                        	break;
                        case 4:
                        	val = stod(token2);
                        	PRINT_DO_DEBUG("val = ");
                        	PRINT_DO_DEBUG(val);
                        	PRINT_DO_DEBUG("\n");
                        	break;
                        default:
                        	std::cout << "erreur dans le fichier, ligne " << i << "colonne " << p << std::endl;
                        	break;
            		}
            		p++;
            		token.erase(0, pos2+1);
            	}
            	switch(type[0]) {
            		case 'S':
            			Act[card].MoveServo(id,val);
            			break;
            		case 'D':
            			Act[card].SetMot(id,val);
            			break;
            		case 'M':
            			Act[card].SetMot4QPos(id,150,1,val);
            			break;
            		case 'F':
            			Act[2].allumerPhare();
            			break;
            		default:
            			std::cout << "erreur action enregistrée non prise en charge !!!" << std::endl;
            			break;
            	}
            	std::cout << std::endl;
            	line.erase(0, pos+1);
            }
        }
    } else {
        std::cout << "lancement en code" << std::endl;
    }
}

void EraseObstacle(Web *web) {
    web->m_clearElementsPoints = true;
}

void PrintObstacles(GameElements Elements[], int leng, Web *web) {
    for(int i=0;i<leng;i++) {
        web->addElementPoints(Elements[i].GetX(),Elements[i].GetY());
        if(Elements[i].GetId() == 'F') {
            web->addElementAttribut(1);
        } else {
            web->addElementAttribut(0);
        }
    }
}

int ComputeBestNode(double x, double y, double t, GameElements Elements[], int leng, int *Score, Web *web) {
    double mini = X_MAX*T_MAX, t1 = 0, t2 = 0;
    int res = -1, j, k;
    t = 90-t;
    for(int i=0;i<leng;i++) {
        if(Elements[i].GetTodo() > 0) { //si element à réaliser
            t1 = Elements[i].GetDuree()+GetDistance(x,y,Elements[i].GetX(),Elements[i].GetY())*DELTA_TEMPS;
            if(Elements[i].GetDepot() == -1) { //si element à déposer
                for(j=0;j<leng;j++) {
                    if(Elements[i].GetId() == Elements[j].GetId() && Elements[j].GetDepot() > 0) {
                        break;
                    }
                }
                t2 = Elements[j].GetDuree()+GetDistance(Elements[i].GetX(),Elements[i].GetY(),Elements[j].GetX(),Elements[j].GetY())*DELTA_TEMPS;
            } else {
                t2 = 0;
            }
            if(t > t1+MARGE_TEMPS+t2) { //si on a le temps
                PRINT_STRAT_DEBUG(Elements[i].GetNom());
                PRINT_STRAT_DEBUG("\n");
                double miniR = t1/sqrt(Elements[i].GetScore());
                PRINT_STRAT_DEBUG(" ratio ");
                PRINT_STRAT_DEBUG(miniR);
                PRINT_STRAT_DEBUG("\n");
                PRINT_STRAT_DEBUG(" score ");
                PRINT_STRAT_DEBUG(Elements[i].GetScore());
                PRINT_STRAT_DEBUG("\n");
                if(mini > miniR) { //cherche le meilleure
                    mini = miniR;
                    res = i;
                    k = j;
                }
            }
        }
    }
    if(res != -1) {
        PRINT_STRAT_DEBUG("----------");
        PRINT_STRAT_DEBUG("\n");
        PRINT_STRAT_DEBUG("--");
        PRINT_STRAT_DEBUG(Elements[res].GetNom());
        PRINT_STRAT_DEBUG("\n");
        Elements[res].SetTodo(0);
        if(Elements[res].GetDepot() == -1) {
            EraseObstacle(web);
            PrintObstacles(Elements,leng,web);
            int r =Elements[k].GetStock();
            r--;
            Elements[k].SetStock(r);
            PRINT_STRAT_DEBUG(" ");
            PRINT_STRAT_DEBUG(Elements[k].GetStock());
            PRINT_STRAT_DEBUG("\n");
            Elements[k].SetTodo(1);
            PRINT_STRAT_DEBUG("->");
            PRINT_STRAT_DEBUG(Elements[k].GetNom());
            PRINT_STRAT_DEBUG("\n");
            if(Elements[k].GetStock() == 0) { //stockage plein
                for(int i=0;i<leng;i++) {
                    if(Elements[i].GetId() == Elements[k].GetId() && Elements[i].GetDepot() < 0 && Elements[i].GetTodo() == 1) {
                        Elements[i].SetTodo(-1);
                        PRINT_STRAT_DEBUG("<-");
                        PRINT_STRAT_DEBUG(Elements[i].GetNom());
                        PRINT_STRAT_DEBUG("\n");
                    }
                }
            }
        } else {
            *Score += Elements[res].GetScore();
        }
        int u = Elements[res].GetDepot();
        if(u > 0) { //si on va au dépot
            Elements[res].SetStock(u); //On vide le stock
            for(int i=0;i<leng;i++) {
                if(Elements[res].GetId() == Elements[i].GetId() && Elements[i].GetTodo() == -1) {
                    Elements[i].SetTodo(1);
                    PRINT_STRAT_DEBUG("->");
                    PRINT_STRAT_DEBUG(Elements[i].GetNom());
                    PRINT_STRAT_DEBUG("\n");
                }
            }
        }
    }
    return res;
}

int LengGameElement(void) {
    std::ifstream MyFile;
    std::string line;
    int cpt = 0;
    MyFile.open("GameElement.txt");
    if(!MyFile.is_open()) {
        std::cout << "error opening GameElement.txt" << std::endl;
    } else {
        while(MyFile >> line) {
            cpt++;
        }
        MyFile.close();
    }
    return cpt;
}

void CreateGameElement(GameElements Elements[], Actuator Act[], int lengAct, Web *web) {
    std::ifstream MyFile;
    std::string line;
    std::string delimiter = ":";
    int cpt = -1;
    MyFile.open("GameElement.txt");
    if(!MyFile.is_open()) {
        std::cout << "error opening GameElement.txt" << std::endl;
    } else {
        while(MyFile >> line) {
            cpt++;
            size_t pos = 0, pos2 = 0;
            int p = 0, afaire = 1, score, depot;
            double x, y, temps;
            char id[1];
            std::string nom;
            std::string token;
            while ((pos = line.find(delimiter)) != std::string::npos) {
                token = line.substr(0, pos);
                switch(p) {
                    case 0:
                        nom = token;
                        PRINT_STRAT_DEBUG("nom : ");
                        PRINT_STRAT_DEBUG(nom);
                        PRINT_STRAT_DEBUG("\n");
                        break;
                    case 1:
                        token = token.substr(0,1);
                        std::copy(token.begin(), token.end(), id);
                        PRINT_STRAT_DEBUG("id : ");
                        PRINT_STRAT_DEBUG(id[0]);
                        PRINT_STRAT_DEBUG("\n");
                        break;
                     case 2:
                        x = stod(token);
                        PRINT_STRAT_DEBUG("x : ");
                        PRINT_STRAT_DEBUG(x);
                        PRINT_STRAT_DEBUG("\n");
                        break;
                    case 3:
                        y = stod(token);
                        PRINT_STRAT_DEBUG("y : ");
                        PRINT_STRAT_DEBUG(y);
                        PRINT_STRAT_DEBUG("\n");
                        break;
                    case 4:
                        score = stoi(token);
                        PRINT_STRAT_DEBUG("score : ");
                        PRINT_STRAT_DEBUG(score);
                        PRINT_STRAT_DEBUG("\n");
                        break;
                    case 5:
                        depot = stoi(token);
                        if(depot > 0) {
                            PRINT_STRAT_DEBUG("Depot : ");
                            PRINT_STRAT_DEBUG(depot);
                            PRINT_STRAT_DEBUG("\n");
                        }
                        if(depot == -1) {
                            web->addElementPoints(x,y);
                            if(id[0] == 'F') {
                            	web->addElementAttribut(1);
                            } else {
                            	web->addElementAttribut(0);
                            }
                            PRINT_STRAT_DEBUG("Element deposable");
                            PRINT_STRAT_DEBUG("\n");
                        }
                        break;
                    case 6:
                        if(depot < 1) {
                            PRINT_STRAT_DEBUG("a faire : ");
                            PRINT_STRAT_DEBUG(token);
                            PRINT_STRAT_DEBUG(" => ");
                            pos2 = 0;
                            for(int m=0;m<token.length();m++) {
                                if(token.substr(m,1) == "0" || token.substr(m,1) == "1" || token.substr(m,1) == "2") {
                                    if(lengAct > stoi(token.substr(m,1)) && -1 < stoi(token.substr(m,1))) {
                                        if(!Act[stoi(token.substr(m,1))].isConnected()) {
                                            afaire = 0;
                                        }
                                    } else {
                                        PRINT_STRAT_DEBUG("error nb card");
                                    }
                                    PRINT_STRAT_DEBUG(stoi(token.substr(m,m+1)));
                                    PRINT_STRAT_DEBUG(";");
                                }
                            }
                            if(afaire) {
                                PRINT_STRAT_DEBUG(" = 1\n");
                            } else {
                                PRINT_STRAT_DEBUG(" = 0\n");
                            }
                        }else{
                            afaire = 0;
                            PRINT_STRAT_DEBUG("a faire : 0 depot \n");
                        }
                        break;
                    case 7:
                        temps = stod(token);
                        PRINT_STRAT_DEBUG("temps : ");
                        PRINT_STRAT_DEBUG(temps);
                        PRINT_STRAT_DEBUG("\n");
                        break;
                    default:
                        std::cout << "erreur dans le fichier, ligne " << cpt << "colonne " << p << std::endl;
                }
                p++;
                line.erase(0, pos + delimiter.length());
            }
            Elements[cpt] = GameElements(nom,id[0],x,y,afaire,score,temps,depot);
            afaire = 1;
            PRINT_STRAT_DEBUG("\n");
        }
        MyFile.close();
        web->m_ElementCreateEnd = true;
    }
}

GameElements::GameElements() {}

GameElements::GameElements(std::string nom, char id, int x, int y, int afaire, int score, double duree, int depot) {
    m_nom = nom;
    m_id = id;
    m_x = x;
    m_y = y;
    m_afaire = afaire;
    m_score = score;
    m_duree = duree;
    m_depot = depot;
    m_stock = depot;
}

GameElements::~GameElements() {}

std::string GameElements::GetNom() {
    return m_nom;
}

double GameElements::GetX() {
    return m_x;
}

double GameElements::GetY() {
    return m_y;
}

int GameElements::GetScore() {
    int score;
    if(GetDepot() > 0) {
        score = (m_depot-m_stock)*m_score;
    } else {
        score = m_score;
    }
    return int(score);
}

double GameElements::GetDuree() {
    return m_duree;
}

int GameElements::GetDepot() {
    return m_depot;
}

char GameElements::GetId() {
    return m_id;
}

int GameElements::GetTodo() {
    return m_afaire;
}

void GameElements::SetTodo(int todo) {
    m_afaire = todo;
}

void GameElements::SetDepot(int depot) {
    m_depot = depot;
}

int GameElements::GetStock() {
    return m_stock;
}

void GameElements::SetStock(int stock) {
    m_stock = stock;
}
