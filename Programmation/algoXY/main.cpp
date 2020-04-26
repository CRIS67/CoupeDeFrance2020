#include <iostream>
#include <cmath>

#define D1  80
#define D2  60
#define PI  3.14159265358979
#define PTxFORB  100
#define PTyFORB  100

#define ZONE_1 1
#define ZONE_2 0

using namespace std;
void CalculerPosScara(double d, double thet, int *thetI1, int *thetI2);
uint8_t TestScara(int d, int thet, int* thetI1, int* thetI2);
void GoScara(int thet, int thet1, int thet2);
void SetAx12(int nb_bras, int pos);
void delay(int temps);

uint8_t Zone = ZONE_1;



int main() {
    int thetI1, thetI2;
    int d = 100, a = 50;
    if(TestScara(d, a, &thetI1, &thetI2)) {
        GoScara(a, thetI1, thetI2);
    }
    return 0;
}

void CalculerPosScara(double d, double thet, int* thetI1, int* thetI2) {
    double thet1, thet2, thet3;
    thet = thet*PI/180;
    thet2 = acos((D1*D1+D2*D2-d*d)/(2*D1*D2));
    thet3 = acos((D1*D1+d*d-D2*D2)/(2*D1*d));
    thet1 = thet-thet3;
    thet1 = thet1*180/PI;
    thet2 = thet2*180/PI;
    *thetI1 = thet1;
    *thetI2 = thet2;
}

uint8_t TestScara(int d, int thet, int* thetI1, int* thetI2) {
    if(d > D1+D2) {
        cout << "erreur d trop grand : " << d << endl;
        return 0;
    } else {
        CalculerPosScara(d, thet, *&thetI1, *&thetI2);
        double tt = thet*PI/180, tt1 = *thetI1*PI/180;
        if(*thetI1 < 0 || *thetI1 > 180) {
            cout << "erreur : thet1 = " << *thetI1 << endl;
            return 0;
        } else if(d*cos(tt) > PTxFORB && d*sin(tt) > PTyFORB) {
            cout << "erreur : zone morte, x = " << d*cos(tt) << " y = " << d*sin(tt) << endl;
            return 0;
        } else if(D1*cos(tt1) > PTxFORB && D1*sin(tt1) > PTyFORB) {
            cout << "erreur : zone morte, x1 = " << D1*cos(tt1) << " y1 = " << D1*sin(tt1) << endl;
            return 0;
        } else {
            return 1;
        }
    }
}

void GoScara(int thet, int thet1, int thet2) {
    if((thet > 45 && Zone == ZONE_1) || (thet < 45 && Zone == ZONE_2)) {
        Zone = ~Zone;
        SetAx12(1, 0);
        delay(50);
        SetAx12(0, thet1);
        delay(20);
    } else {
        SetAx12(0, thet1);
    }
    SetAx12(1, thet2);
}

void SetAx12(int nb_bras, int pos) {
    cout << "Move bras : " << nb_bras << " to " << pos << endl;
}

void delay(int temps) {
    cout << "attente de " << temps << "ms" << endl;
}
