#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <vector>
#include <queue>
#include "web.hpp"
#include "dspic.hpp"
#include "actuator.hpp"
#include "SPI.hpp"
#include "lidar.hpp"
#include "hmi.hpp"
#include "robot.cpp"

#include <wiringPi.h>
#include <wiringPiSPI.h>

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

#include "LK.h"

#define DEBUG_ENABLE_PRINT 1

#if (DEBUG_ENABLE_PRINT)
#define DEBUG_PRINT(x) std::cout << "DEBUG_PRINT>> " << x << std::endl;
#else
#define DEBUG_PRINT(x)
#endif


#define MAP_MM_PER_ARRAY_ELEMENT 10

#define MM_ROWS 2000
#define MM_COLUMNS 3000

#define MM_START_X 29
#define MM_START_Y 19

#define MM_SIZE_ENNEMY 200
#define MM_SIZE_ENNEMY_MARGIN 220

#define SIZE_ENNEMY MM_SIZE_ENNEMY / MAP_MM_PER_ARRAY_ELEMENT //size of ennemy /2 4 element in array map -> (here 40cm)WARNING depends of map resolution
#define SIZE_ENNEMY_MARGIN MM_SIZE_ENNEMY_MARGIN / MAP_MM_PER_ARRAY_ELEMENT

#define ARRAY_START_X MM_START_X / MAP_MM_PER_ARRAY_ELEMENT
#define ARRAY_START_Y MM_START_Y / MAP_MM_PER_ARRAY_ELEMENT

//DStarGlobal
int mapRows{MM_ROWS / MAP_MM_PER_ARRAY_ELEMENT};
int mapColumns{MM_COLUMNS / MAP_MM_PER_ARRAY_ELEMENT};
int km{0}; // variable for the D*

std::vector<std::vector<int>> mapVector;    // the robot's map
std::vector<std::vector<int>> newMap;       //temp map with moving object
std::vector<std::vector<int>> augmentedMap; //temp map with bigger moving object to avoid colision

std::queue<pointFloat2d> points;
std::queue<pointFloat2d> savePoints;

bool obstacleDetection{false};
bool pointReached{false};

/*Node startNode = {INT_MAX,INT_MAX,0,std::pair<int,int>(19,29)};
Node goalNode = {INT_MAX,0,0,std::pair<int,int>(8,8), false};*/
Node startNode = {std::pair<int, int>(ARRAY_START_X, ARRAY_START_Y)};
Node goalNode;
Node lastNode;
Node robotNode;

priorityList uList;     // priority List
mappedNodes knownNodes; // node the robot can see

const int enemyWidth = 500; // The distance the enemy takes place on the map, it is represented as a square
//std::string s;

std::vector<Node> simplifiedPath;
std::vector<Node> completePath;

void *print(void *ptr);
void debugAct();
void debugTestAllDelay();
void debugTestAllInstant();
void debugBN();
void debugGoldenium();

// void detectNewObject(uint startX, uint startY, int width, int height, std::vector<pointFloat2d> &points)
// {
//     for (int y = startY; y < startY + height; y++)
//     {
//         for (int x = startX; x < startX + width; x++)
//         {
//             //std::cout<<i<<"  "<<j<<std::endl;
//             pointFloat2d pointPosition = {x, y};
//             points.push_back(pointPosition);
//         }
//     }
// }

int main()
{
    std::string command = "rm ./logMaps/*.txt";
    const char *commandPointer = command.c_str();
    std::system(commandPointer);

    std::cout << std::endl<< "     Coupe de France 2020 by CRIS" << std::endl<< std::endl;
    wiringPiSetup();

    std::cout << "Initialisation..." << std::endl;

    DsPIC dspic;
    dspic.reset();
    delay(250); //wait for reset
    dspic.async_read(); //flush rx buffer

    SPI spi(SPI_CHANNEL,SPI_SPEED); //initialise SPI

    Actuator actBack("Actuator Back 2019",&spi,SPI_ID_ACT_BACK,3,0,3,3,0,0,0,0,0);
    Actuator actScara("Actuator Scara",&spi,SPI_ID_ACT_SCARA,0,2,2,2,0,0,5,3,1);

    Web web(&dspic, &actScara, &actBack);

    Hmi hmi("HMI",&spi,SPI_ID_HMI);
    Lidar lidar("Lidar",&spi,SPI_ID_LIDAR,&web);
    Actuator xbee("Xbee",&spi,SPI_ID_XBEE,4,2,0,0,0,1,3,0,0);

    //actScara.reset();
    actBack.reset();
    //lidar.stop();

    puts("Hello human ! I, your fervent robot, am initialised. Press <ENTER> to continue.");
    getchar();

    lidar.start();
    dspic.startThreadReception();
    web.startThread();
    lidar.startThreadDetection();
    hmi.startThreadDetection();
    //actFront.startThreadDetection();
    actBack.startThreadDetection();

    dspic.setVar8(CODE_VAR_VERBOSE,1);
    puts("verbose set to 1");
    dspic.getVar(CODE_VAR_BAT);
    dspic.loadVarDspicFromFile("config.txt");
    dspic.initPos(startNode.coord.first*MAP_MM_PER_ARRAY_ELEMENT,startNode.coord.second*MAP_MM_PER_ARRAY_ELEMENT, -3.14159 -3.14159/2);

    std::cout << "Press enter to dspic.start() " << std::endl;
    getchar();
    dspic.start();

    std::cout << "Press enter to start the D*" << std::endl;
    getchar();



    std::vector<Node> strategyTour;   // Contains the nodes we are going to visit
    LK strategy(100);                 //faire la liste des taches jusqu'a 80 secondes
    strategy.readNodes(strategyTour); // reads the nodes from the entree.txt file

    strategy.optimize(strategyTour); // optimize the tour
    //strategy.printSolution(); // debug

    std::cout << "Initial distance " << strategy.calculateDistance(strategyTour) << std::endl;
    std::cout << "Final distance " << strategy.getDistance() << std::endl;

    strategyTour = strategy.getSolution(); // We update the optimized tour
    for (uint i = 0; i < strategyTour.size(); i++)
    {
        strategyTour.at(i).coord.first /= MAP_MM_PER_ARRAY_ELEMENT;
        strategyTour.at(i).coord.second /= MAP_MM_PER_ARRAY_ELEMENT;
    }

    // Map Generation
    generateMap(mapVector, mapRows, mapColumns);     // generates empty map
    createRectangle(0, 0, mapColumns, 5, mapVector); // creates a 400x2000 obstacle rectangle  at (250,0)

    createRectangle(20, 10, 5, 5, mapVector);
    //detectNewObject(16, 15, 5, 5, points);
    printMap(mapRows, mapColumns, mapVector);

    int counter;
    unsigned int nbPath = 0;
    unsigned int nbPoints;

    for (uint i = 0; i < strategyTour.size(); i++)
    {
        /*=============DStarImplementation START===================*/

        //reset lists when going to next point
        uList.clear();
        knownNodes.clear();

        //change start and goal node
        lastNode = startNode;
        goalNode.coord = strategyTour.at(i).coord;

        //initialize lists
        initialize(mapVector, knownNodes, uList, startNode, goalNode);
        //printKnownNode(knownNodes);
        goalNode = knownNodes.at(goalNode.coord);

        //computing best path to goal
        std::cout << "Start computing..." << std::endl;
        computeShortestPath(uList, knownNodes, startNode.coord, goalNode);
        std::cout << "End  computing..." << std::endl;
        //printKnownNode(knownNodes);
        startNode = knownNodes.at(startNode.coord); // we update the start node
        goalNode = knownNodes.at(goalNode.coord);   // we update the goal node

        //debug
        //findPath(mapVector, knownNodes, startNode, goalNode);               // prints the path in the terminal
        completePath = getPath(mapVector, knownNodes, startNode, goalNode); // get the whole path
        simplifiedPath = pathTreatment(completePath);
        simplifiedPath.push_back(goalNode); // we need to add the last node manually :(
        /*std::cout << "Simplified path" << std::endl;
        printPath(simplifiedPath,mapVector);*/

        //printKnownNode(knownNodes);
        counter = 0;

        while (startNode.coord != goalNode.coord)
        {
            if (startNode.costG == INT_MAX)
            {
                std::cerr << "NOT KNOWN PATH" << std::endl;
                break;
            }
            //we move to the next path corner
            startNode = completePath.at(counter);
            std::cout<<"Actual position : x="<<startNode.coord.first<<"  y="<<startNode.coord.second<<std::endl;

            if(newMap.size()==0) findPath(mapVector, knownNodes, startNode, goalNode); // prints the path in the terminal
            else findPath(newMap, knownNodes, startNode, goalNode);

            int xSetpoint = startNode.coord.first * MAP_MM_PER_ARRAY_ELEMENT;
            int ySetpoint = startNode.coord.second * MAP_MM_PER_ARRAY_ELEMENT;
            std::cout << "send go " << xSetpoint << " ; " << ySetpoint << std::endl;
            dspic.go(xSetpoint, ySetpoint,0,0); // we move the robot to the next point
            delay(50);  //wait before asking so the dspic can start the movement /  and don't SPAM the UART channel


            lidar.setFillBuffer(true);  //keep track of the points detected by lidar in a member buffer
            while((dspic.getX() - xSetpoint)*(dspic.getX() - xSetpoint) + (dspic.getY() - ySetpoint)*(dspic.getY() - ySetpoint) > 400){
                points = lidar.getAndClearDetectedPoints(); //get detected points and clear internal buffer
                savePoints = points;
                //printMap(mapRows, mapColumns, mapVector);
                newMap = mapVector;
                nbPoints = points.size();
                for (unsigned int i = 0; i < nbPoints; i++)
                {
                    pointFloat2d p = points.front();
                    points.pop();

                    int xPoint = (int)round(p.x / MAP_MM_PER_ARRAY_ELEMENT);
                    int yPoint = (int)round(p.y / MAP_MM_PER_ARRAY_ELEMENT);
                    for (int y = yPoint - SIZE_ENNEMY; y <= yPoint + SIZE_ENNEMY; y++)
                    {
                        for (int x = xPoint - SIZE_ENNEMY; x <= xPoint + SIZE_ENNEMY; x++)
                        {
                            if (y >= 0 && y < mapRows && x >= 0 && x < mapColumns)
                            {
                                newMap.at(y).at(x) = 1;
                                //std::cout << "(" << j << " ; " << k << ") ";
                            }
                        }
                    }
                }
                robotNode.coord.first = (int)(dspic.getX()/MAP_MM_PER_ARRAY_ELEMENT);
                robotNode.coord.second = (int)(dspic.getY()/MAP_MM_PER_ARRAY_ELEMENT);
                obstacleDetection = detectCollision(newMap, simplifiedPath, robotNode, simplifiedPath.at(counter + 1)); //ATTENTION à changer plus tard pour ne pas prendre en compte la partie du trajet déjà fait

                std::ofstream file;
                std::ostringstream ss;
                ss << "logMaps/out_map" << nbPath++ << ".txt";
                file.open(ss.str());
                for (unsigned int i = 0; i < simplifiedPath.size(); i++)
                {
                    newMap.at(simplifiedPath.at(i).coord.second).at(simplifiedPath.at(i).coord.first) = 4;
                }
                newMap.at(robotNode.coord.second).at(robotNode.coord.first) = 3;
                for (int i = 0; i < mapRows; i++)
                {
                    for (int j = 0; j < mapColumns; j++)
                    {
                        file << newMap.at(i).at(j) << " ";
                    }
                    file << std::endl;
                }
                file.close();



                if(obstacleDetection){
                    std::cout << "OBSTACLE DETECTED" << std::endl;
                    lidar.setFillBuffer(false);

                    augmentedMap = mapVector;
                    unsigned int nbPoints = points.size();
                    for (unsigned int i = 0; i < nbPoints; i++)
                    {
                        pointFloat2d p = savePoints.front();
                        savePoints.pop();
                        int xPoint = (int)round(p.x / MAP_MM_PER_ARRAY_ELEMENT);
                        int yPoint = (int)round(p.y / MAP_MM_PER_ARRAY_ELEMENT);
                        for (int y = yPoint - SIZE_ENNEMY_MARGIN; y <= yPoint + SIZE_ENNEMY_MARGIN; y++)
                        {
                            for (int x = xPoint - SIZE_ENNEMY_MARGIN; x <= xPoint + SIZE_ENNEMY_MARGIN; x++)
                            {
                                if (y >= 0 && y < mapRows && x >= 0 && x < mapColumns)
                                {
                                    augmentedMap.at(y).at(x) = 1;
                                    //std::cout << "(" << j << " ; " << k << ") ";
                                }
                            }
                        }
                    }

                    km += distance2(lastNode, startNode);
                    lastNode = startNode;
                    updateMap(knownNodes, augmentedMap, uList, startNode.coord, goalNode);

                    computeShortestPath(uList, knownNodes, startNode.coord, goalNode);
                    startNode = knownNodes.at(startNode.coord); // we update the start node
                    robotNode.coord.first = startNode.coord.first;
                    robotNode.coord.second = startNode.coord.second;
                    goalNode = knownNodes.at(goalNode.coord);   // we update the goal node
                    //printKnownNode(knownNodes);

                    //findPath(mapVector,knownNodes,startNode,goalNode); // prints the path in the terminal
                    completePath = getPath(augmentedMap, knownNodes, startNode, goalNode); // get the hole path
                    simplifiedPath = pathTreatment(completePath);
                    simplifiedPath.push_back(goalNode); // we need to add the last node manually :(
                    counter = 0;

                    if (augmentedMap.at(robotNode.coord.second).at(robotNode.coord.first) == 1)
                    { // If the start is an obstacle
                        std::cout << "Start Node is an obstacle !" << std::endl;
                        dspic.stop();
                        dspic.setVar8(CODE_VAR_VERBOSE,0);
                        dspic.stopThreadReception();
                        puts("verbose set to 0");
                        puts("exiting ...");
                        lidar.stop();
                        lidar.stopThreadDetection();
                        web.stopThread();

                        puts("exiting...");

                        delay(200);
                        return -1;
                    }

                    std::ofstream file;
                    std::ostringstream ss;
                    ss << "logMaps/augmented_map" << nbPath << ".txt";
                    file.open(ss.str());
                    for (unsigned int i = 0; i < simplifiedPath.size(); i++)
                    {
                        augmentedMap.at(simplifiedPath.at(i).coord.second).at(simplifiedPath.at(i).coord.first) = 4;
                    }
                    augmentedMap.at(robotNode.coord.second).at(robotNode.coord.first) = 3;
                    for (int i = 0; i < mapRows; i++)
                    {
                        for (int j = 0; j < mapColumns; j++)
                        {
                            file << augmentedMap.at(i).at(j) << " ";
                        }
                        file << std::endl;
                    }
                    file.close();
                }
            }
            lidar.setFillBuffer(false); //stop keeping lidar points to avoid running out of memory
            counter++;
        }
        /*=============DStarImplementation END===================*/

        std::cout << "Press nothing to continue to the next destination" << std::endl;
        //std::getline(std::cin,s);
        if (startNode.coord != goalNode.coord)
        {
            std::cout << "Impossible to reach goal \nContinue to next destination" << std::endl;
        }
        //startNode = goalNode;
    }

    return 0;
}
