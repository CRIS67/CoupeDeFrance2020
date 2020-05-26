#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <vector>
#include <queue>
#include "web.hpp"
#include "dspic.hpp"
#include "SPI.hpp"
#include "robot.hpp"
#include "actuator.hpp"
#include "lidar.hpp"
#include "hmi.hpp"

#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <ncurses.h>
#include <opencv/highgui.h>
#include <array>

// DStarIncludes 
#include <utility>
#include <algorithm>
#include <cmath>
#include <bits/stdc++.h>
#include <limits>
#include <map>
#include "mapGeneration.hpp"
#include "dStarLite.hpp"
#include "trajectoryHandle.hpp"
#include "GameElements.hpp"


#include <fstream>
#include <sstream>

#define DEBUG_ENABLE_PRINT          0

#if(DEBUG_ENABLE_PRINT)
#define DEBUG_PRINT(x)              std::cout << "DEBUG_PRINT>> " << x << std::endl;
#else
#define DEBUG_PRINT(x)              
#endif

#define MAP_MM_PER_ARRAY_ELEMENT    20


#define MM_ROWS                     2000
#define MM_COLUMNS                  3000

#define MM_START_X                  1000
#define MM_START_Y                  2500

#define MM_SIZE_ENNEMY              200
#define MM_SIZE_ENNEMY_MARGIN       220

#define SIZE_ENNEMY                 MM_SIZE_ENNEMY / MAP_MM_PER_ARRAY_ELEMENT   //size of ennemy /2 4 element in array map -> (here 40cm)WARNING depends of map resolution
#define SIZE_ENNEMY_MARGIN          MM_SIZE_ENNEMY_MARGIN / MAP_MM_PER_ARRAY_ELEMENT

#define ARRAY_START_X               MM_START_X / MAP_MM_PER_ARRAY_ELEMENT
#define ARRAY_START_Y               MM_START_Y / MAP_MM_PER_ARRAY_ELEMENT


//DStarGlobal 
int mapRows {MM_ROWS / MAP_MM_PER_ARRAY_ELEMENT};  
int mapColumns {MM_COLUMNS / MAP_MM_PER_ARRAY_ELEMENT};  
int km {0}; // variable for the D*

std::vector<std::vector<int>> mapVector; // the robot's map 

bool obstacleDetection {false}; 
bool pointReached {false}; 
 
Node startNode = {infinity,infinity,0,std::pair<int,int>(ARRAY_START_X,ARRAY_START_Y)};
Node goalNode  = {infinity,0,0,std::pair<int,int>(9,9), false};

priorityList uList; // priority List
mappedNodes knownNodes; // node the robot can see


const int enemyWidth = 500; // The distance the enemy takes place on the map, it is represented as a square 

std::vector<Node> simplifiedPath; 
std::vector<Node> completePath; 



// Strategy include 
#include "LK.h" 

#define PIN_JUMPER      27

#define HALF_LENGTH     124
#define HALF_WIDTH      155

#define PI              3.14159

#define FORWARD         0
#define BACKWARD        1

int main() {
    int i_main;
    std::cout << std::endl << "     Coupe de France 2020 by CRIS" << std::endl << std::endl;
    wiringPiSetup();

    std::cout << "Initialisation..." << std::endl;

    DsPIC dspic;
    dspic.reset();
    delay(250); //wait for reset
    dspic.async_read(); //flush rx buffer

    SPI spi(SPI_CHANNEL,SPI_SPEED); //initialise SPI


    std::string xbeeName[1] = {"XBee"};
    int xbeeAddr[1] = {1};
    Actuator Act[] = {Actuator("Back Actuator",&spi,SPI_ID_ACT_BACK,6,0,0,0,2,6,0,0),
                      Actuator("Scara Actuator",&spi,SPI_ID_ACT_SCARA,0,2,2,2,0,5,3,1),
                      Actuator("Xbee Actuator",&spi,SPI_ID_XBEE,4,2,0,0,0,3,0,0,1,xbeeName,xbeeAddr)};
    const int LengAct = sizeof(Act)/sizeof(Act[0]);
    //Actuator actBack("Actuator Back 2019",&spi,SPI_ID_ACT_BACK,3,0,3,3,0,0,0,0);

    Web web(&dspic, Act, LengAct);

    Hmi hmi("HMI",&spi,SPI_ID_HMI);
    Lidar lidar("Lidar",&spi,SPI_ID_LIDAR,&web);

    GameElements Elements[LengGameElement()];
    CreateGameElement(Elements, Act, LengAct, &web);
    int Leng = sizeof(Elements)/sizeof(Elements[0]);

    lidar.stop();
    delay(1000);

    puts("Hello human ! I, your fervent robot, am initialised. Press <ENTER> to continue.");
    getchar();

    lidar.start();
    dspic.start();
    dspic.startThreadReception();
    web.startThread();
    lidar.startThreadDetection();
    hmi.startThreadDetection();
    for(i_main=0;i_main<LengAct;i_main++) {Act[i_main].startThreadDetection();}

    dspic.setVar8(CODE_VAR_VERBOSE,1);
    puts("verbose set to 1");
    dspic.getVar(CODE_VAR_BAT);
    dspic.loadVarDspicFromFile("config.txt");
	
	int xInit = 1200-HALF_WIDTH;
    int yInit = 2550+HALF_LENGTH;
    double tInit = -PI/2;
    //double tInit = 0;
    dspic.initPos(1045,1500,0);
    
    //std::cout << "Press enter to dspic.start() " << std::endl; 
    Act[0].MoveServo(2, 980);
    Act[0].MoveServo(0, 1600);
    Act[0].MoveServo(1, 1800);
    std::cout << "Reamorcer le drapeau ! ensuite appuyer sur entrÃ©e" << std::endl;
    hmi.setScore(666);
    getchar();
    Act[0].MoveServo(2, 1100);
    
    Act[2].eteindrePhare();
    std::cout << "Appuyer sur entrée quand le phare est en bas" << std::endl;
    getchar();

    Do(0,true,Act);


    //dspic.go(1445, 1500, 0, 0);

    //actBack.MoveServo(2, 1100); //drapeau haut/bas
    //actBack.MoveServo(2, 980);

    //actBack.MoveServo(0, 1600); //prehenseur haut/bas
    //actBack.MoveServo(0, 850);

    //actBack.MoveServo(1, 1800); //orienter prehenseur
    //actBack.MoveServo(1, 1400);

    //actScara.SetMot(0, 0); //Scara
    //actScara.SetAx12(1, 400); //pump droite
    //actScara.SetAx12(2, 600);
    //actScara.SetAx12(3, 800);  //pump droite
    //delay(2000);
    //delay(2000);
    //actScara.SetMot4QPos(0, 200, 1, 400); //ascensseur
    /*actScara.SetMot4QVit(0, 115, 0);
    delay(200);
    actScara.SetMot4QVit(0, 0, 0);*/

    /*actBack.MoveServo(1, 1800);
    actScara.SetMot4QPos(1, 150, 0, 4000);
    delay(5000);
    actBack.MoveServo(0, 800);
    delay(1000);
    actScara.SetMot4QPos(1, 150, 1, 4000);
    delay(5000);
    actBack.MoveServo(1, 1400);*/


    //Act[2].allumerPhare();
    //delay(12000);
    //xbee.resetXbee(1);


    //actScara.SetMot4QVit(1,255,0);
    /*std::cout << "arret ?" << std::endl;
    while(actScara.Rupt(0) != 1) {}
    std::cout << "ok" << std::endl;*/
    //getchar();
    //actScara.SetMot4QVit(1,0,1);
    std::cout << "main" << std::endl;
    /*while(!hmi.isStopMain()) {
    	//xbee.PingXbee(1);
        //actBack.MoveServo(0, 1600);
        //actScara.SetMot4QPos(0, 115, 0, 200);
        delay(1000);
        //std::cout << xbee.GetPingXbee(1) << std::endl;
        //actBack.MoveServo(0, 800);
        //actScara.SetMot4QPos(0, 200, 1, 200);
    	//std::cout << actBack.ColorOne(0) << std::endl;
    	//std::cout << actBack.ColorOne(1) << std::endl;
        delay(1000);
	}*/
    dspic.stop();
    dspic.setVar8(CODE_VAR_VERBOSE,0);
    dspic.stopThreadReception();
    puts("verbose set to 0");
    puts("exiting ...");
    lidar.stop();
    lidar.stopThreadDetection();
    hmi.stopThreadDetection();
    for(i_main=0;i_main<LengAct;i_main++) {Act[i_main].stopThreadDetection();}
    hmi.stopThreadDetection();
    web.stopThread();

    puts("exiting...");

    delay(200);

    if(hmi.isStopPrgm()) {
        system("sudo shutdown now");
    }

    
    return 0;
}
