#ifndef GAMEELEMENTS_H
#define GAMEELEMENTS_H

#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
#include "actuator.hpp"
#include "web.hpp"

#define X_MAX 91
#define Y_MAX 61
#define T_MAX 90
#define MARGE_TEMPS 10
#define DELTA_TEMPS 0.166
//#define PRINT_STRAT_DEBUG(X)        std::cout<<X;
#define PRINT_STRAT_DEBUG(X)
#define PRINT_DO_DEBUG(X)        std::cout<<X;
//#define PRINT_DO_DEBUG(X)


class GameElements {
    public:
        GameElements();
        GameElements(std::string nom, char id, int x, int y, int afaire, int score, double duree, int depot);
        virtual ~GameElements();
        std::string GetNom();
        double GetX();
        double GetY();
        int GetScore();
        double GetDuree();
        int GetDepot();
        char GetId();
        int GetTodo();
        void SetTodo(int todo);
        void SetDepot(int depot);
        void SetStock(int stock);
        int GetStock();
    protected:
        std::string m_nom;
        char m_id;
        double m_x;
        double m_y;
        int m_afaire;
        int m_score;
        double m_duree;
        int m_depot;
        int m_stock;
};

double GetDistance(double x, double y, double xO, double yO);
int ComputeBestNode(double x, double y, double t, GameElements Elements[], int leng, int *Score, Web *web);
int LengGameElement(void);
void CreateGameElement(GameElements Elements[], Actuator Act[], int lengAct, Web *web);
//void Do(int i, double *x, double *y, double *t, GameElements Elements[]);
bool Do(int i, bool file, Actuator Act[]);

#endif // GAMEELEMENTS_H
