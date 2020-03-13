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

    std::cout << std::endl << "     Coupe de France 2019 by CRIS" << std::endl << std::endl;
    wiringPiSetup();

    std::cout << "Initialisation..." << std::endl;

    DsPIC dspic;
    dspic.reset();
    delay(250); //wait for reset
    dspic.async_read(); //flush rx buffer

    SPI spi(SPI_CHANNEL,SPI_SPEED); //initialise SPI

    Actuator actBack("Actuator Back",&spi,SPI_ID_ACT_BACK,6,0,0,0,2,0,6,0,0);
    //Actuator actBack("Actuator Back 2019",&spi,SPI_ID_ACT_BACK,3,0,3,3,0,0,0,0,0);
    Actuator actScara("Actuator Scara",&spi,SPI_ID_ACT_SCARA,0,2,2,2,0,0,5,3,1);

    Web web(&dspic, &actScara, &actBack);

    Hmi hmi("HMI",&spi,SPI_ID_HMI);
    Lidar lidar("Lidar",&spi,SPI_ID_LIDAR,&web);
    Actuator xbee("Xbee",&spi,SPI_ID_XBEE,4,2,0,0,0,1,3,0,0);

    //actScara.reset();
    actScara.reset();
    //lidar.stop();

    puts("Hello human ! I, your fervent robot, am initialised. Press <ENTER> to continue.");
    getchar();

    /*lidar.start();
    dspic.start();
    dspic.startThreadReception();
    web.startThread();
    lidar.startThreadDetection();
    hmi.startThreadDetection();
    actScara.startThreadDetection();*/
    //actBack.startThreadDetection();

    /*dspic.setVar8(CODE_VAR_VERBOSE,1);
    puts("verbose set to 1");
    dspic.getVar(CODE_VAR_BAT);
    dspic.loadVarDspicFromFile("config.txt");
	
	int xInit = 1200-HALF_WIDTH;
    int yInit = 2550+HALF_LENGTH;
    double tInit = -PI/2;
    //double tInit = 0;
    dspic.initPos(1045,1500,0);
    
    std::cout << "Press enter to dspic.start() " << std::endl; 
    hmi.setScore(666);
    getchar();

    dspic.go(1445, 1500, 0, 0);
    xbee.allumerPhare();
    delay(4000);*/
    //hmi.setScore(666);
    //hmi.SetPos(10,27);
    //hmi.startThreadDetection();
    actScara.startThreadDetection();

    while(!hmi.isStopMain()){
    	//actBack.GetColor(0);
    	//actBack.GetColor(1);
        //actScara.SetAx12(1, 820);
        //std::cout << "1 = " << actBack.Color(1) << std::endl;
        delay(1000);
        actScara.SetAx12(1, 600);
        delay(1000);
        actScara.SetAx12(1, 800);
	}

    dspic.stop();
    dspic.setVar8(CODE_VAR_VERBOSE,0);
    dspic.stopThreadReception();
    puts("verbose set to 0");
    puts("exiting ...");
    lidar.stop();
    lidar.stopThreadDetection();
    //hmi.stopThreadDetection();
    actScara.stopThreadDetection();
    actBack.stopThreadDetection();
    web.stopThread();

    puts("exiting...");

    delay(200);

    if(hmi.isStopPrgm()) {
        system("sudo shutdown now");
    }

    
    return 0;
}
